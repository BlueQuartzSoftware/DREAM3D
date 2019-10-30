/*
 * Your License or Copyright can go here
 */

#include <memory>

#include "GeneratePrecipitateStatsData.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/FilterParameters/AttributeMatrixCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec2FilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Math/RadialDistributionFunction.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "EbsdLib/EbsdConstants.h"

#include "OrientationLib/Texture/StatsGen.hpp"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"
#include "SyntheticBuilding/SyntheticBuildingVersion.h"
#include "SyntheticBuilding/SyntheticBuildingFilters/Presets/AbstractMicrostructurePreset.h"
#include "SyntheticBuilding/SyntheticBuildingFilters/Presets/PrimaryEquiaxedPreset.h"
#include "SyntheticBuilding/SyntheticBuildingFilters/Presets/PrimaryRecrystallizedPreset.h"
#include "SyntheticBuilding/SyntheticBuildingFilters/Presets/PrimaryRolledPreset.h"
#include "SyntheticBuilding/SyntheticBuildingFilters/StatsGeneratorUtilities.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID20 = 20,
  AttributeMatrixID21 = 21,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GeneratePrecipitateStatsData::GeneratePrecipitateStatsData()
: m_PhaseName("Primary Phase")
, m_PhaseIndex(-1)
, m_CrystalSymmetry(1)
, m_MicroPresetModel(0)
, m_PhaseFraction(0.05f)
, m_Mu(1)
, m_Sigma(0.1)
, m_MinCutOff(5)
, m_MaxCutOff(5)
, m_BinStepSize(0.5)
, m_CreateEnsembleAttributeMatrix(true)
, m_DataContainerName(SIMPL::Defaults::StatsGenerator)
, m_CellEnsembleAttributeMatrixName(SIMPL::Defaults::CellEnsembleAttributeMatrixName)
, m_AppendToExistingAttributeMatrix(false)
, m_PrecipitateStatsData(nullptr)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GeneratePrecipitateStatsData::~GeneratePrecipitateStatsData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
  m_StatsDataArray = nullptr;
  m_PrecipitateStatsData = nullptr;
  m_CrystalStructures = nullptr;
  m_PhaseTypes = nullptr;
  m_PhaseNames = nullptr;
  m_RdfMinMaxDistance[0] = 10;
  m_RdfMinMaxDistance[1] = 80;
  m_RdfNumBins = 50;
  m_RdfBoxSize[0] = 100;
  m_RdfBoxSize[1] = 100;
  m_RdfBoxSize[2] = 100;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_STRING_FP("Phase Name", PhaseName, FilterParameter::Parameter, GeneratePrecipitateStatsData));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Crystal Symmetry");
    parameter->setPropertyName("CrystalSymmetry");

    QVector<QString> choices; // Please add choices to the choices QVector to finish this widget
    choices << "Hexagonal (High)"
            << "Cubic (High)"
            << "Hexagonal (Low)"
            << "Cubic (Low)"
            << "Triclinic"
            << "Monoclinic"
            << "OrthoRhombic"
            << "Tetragonal (Low)"
            << "Tetragonal (High)"
            << "Trigonal (Low)"
            << "Trigonal (High)";

    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameter->setSetterCallback(SIMPL_BIND_SETTER(GeneratePrecipitateStatsData, this, CrystalSymmetry));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(GeneratePrecipitateStatsData, this, CrystalSymmetry));
    parameters.push_back(parameter);
  }

  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Microstructure Preset Model");
    parameter->setPropertyName("MicroPresetModel");

    QVector<QString> choices; // Please add choices to the choices QVector to finish this widget
    choices << "Precipitate Equiaxed"
            << "Precipitate Rolled";

    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameter->setSetterCallback(SIMPL_BIND_SETTER(GeneratePrecipitateStatsData, this, MicroPresetModel));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(GeneratePrecipitateStatsData, this, MicroPresetModel));
    parameters.push_back(parameter);
  }

  parameters.push_back(SIMPL_NEW_DOUBLE_FP("Phase Fraction", PhaseFraction, FilterParameter::Parameter, GeneratePrecipitateStatsData));
  parameters.push_back(SIMPL_NEW_DOUBLE_FP("Mu", Mu, FilterParameter::Parameter, GeneratePrecipitateStatsData));
  parameters.push_back(SIMPL_NEW_DOUBLE_FP("Sigma", Sigma, FilterParameter::Parameter, GeneratePrecipitateStatsData));
  parameters.push_back(SIMPL_NEW_DOUBLE_FP("Min.Cut Off", MinCutOff, FilterParameter::Parameter, GeneratePrecipitateStatsData));
  parameters.push_back(SIMPL_NEW_DOUBLE_FP("Max Cut Off", MaxCutOff, FilterParameter::Parameter, GeneratePrecipitateStatsData));
  parameters.push_back(SIMPL_NEW_DOUBLE_FP("Bin Step Size", BinStepSize, FilterParameter::Parameter, GeneratePrecipitateStatsData));

  //---------------------------
  PreflightUpdatedValueFilterParameter::Pointer param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Bins Created:", NumberOfBins, FilterParameter::Parameter, GeneratePrecipitateStatsData);
  param->setReadOnly(true);
  parameters.push_back(param);

  PreflightUpdatedValueFilterParameter::Pointer param2 = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Feature ESD:", FeatureESD, FilterParameter::Parameter, GeneratePrecipitateStatsData);
  param2->setReadOnly(true);
  parameters.push_back(param2);
  //---------------------------

  // Table 3 - Dynamic rows and fixed columns
  {
    QStringList cHeaders;
    cHeaders << "Euler 1"
             << "Euler 2"
             << "Euler 3"
             << "Weight"
             << "Sigma";
    std::vector<std::vector<double>> defaultTable(1, std::vector<double>(5, 0.0));
    m_OdfData.setColHeaders(cHeaders);
    m_OdfData.setTableData(defaultTable);
    m_OdfData.setDynamicRows(true);
    parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("ODF", OdfData, FilterParameter::Parameter, GeneratePrecipitateStatsData, false));
  }
  {
    QStringList cHeaders;
    cHeaders << "Angle(w)"
             << "Axis (h)"
             << "Axis (k)"
             << "Axis (l)"
             << "Weight (MRD)";
    std::vector<std::vector<double>> defaultTable(1, std::vector<double>(5, 0.0));
    m_MdfData.setColHeaders(cHeaders);
    m_MdfData.setTableData(defaultTable);
    m_MdfData.setDynamicRows(true);
    parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("MDF", MdfData, FilterParameter::Parameter, GeneratePrecipitateStatsData, false));
  }
  {
    QStringList cHeaders;
    cHeaders << "Euler 1"
             << "Euler 2"
             << "Euler 3"
             << "Weight"
             << "Sigma";
    std::vector<std::vector<double>> defaultTable(1, std::vector<double>(5, 0.0));
    m_AxisOdfData.setColHeaders(cHeaders);
    m_AxisOdfData.setTableData(defaultTable);
    m_AxisOdfData.setDynamicRows(true);
    parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Axis ODF", AxisOdfData, FilterParameter::Parameter, GeneratePrecipitateStatsData, false));
  }

  // Insert Parameters for RDF to be calculated.
  parameters.push_back(SIMPL_NEW_FLOAT_VEC2_FP("[RDF] Min/Max Distance", RdfMinMaxDistance, FilterParameter::Parameter, GeneratePrecipitateStatsData));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("[RDF] Number of Bins", RdfNumBins, FilterParameter::Parameter, GeneratePrecipitateStatsData));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("[RDF] Box Size (X, Y, Z)", RdfBoxSize, FilterParameter::Parameter, GeneratePrecipitateStatsData));

  QStringList linkedProps("DataContainerName");
  linkedProps << "CellEnsembleAttributeMatrixName";
  parameters.push_back(
      SIMPL_NEW_LINKED_BOOL_FP("Create Data Container & Ensemble AttributeMatrix", CreateEnsembleAttributeMatrix, FilterParameter::Parameter, GeneratePrecipitateStatsData, linkedProps));

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", DataContainerName, FilterParameter::CreatedArray, GeneratePrecipitateStatsData));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Ensemble Attribute Matrix", CellEnsembleAttributeMatrixName, DataContainerName, FilterParameter::CreatedArray, GeneratePrecipitateStatsData));

  linkedProps.clear();
  linkedProps << "SelectedEnsembleAttributeMatrix";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Append To Existing AttributeMatrix", AppendToExistingAttributeMatrix, FilterParameter::Parameter, GeneratePrecipitateStatsData, linkedProps));
  AttributeMatrixSelectionFilterParameter::RequirementType amReq2;
  parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Selected Ensemble AttributeMatrix", SelectedEnsembleAttributeMatrix, FilterParameter::Parameter, GeneratePrecipitateStatsData, amReq2));
  setFilterParameters(parameters);
}

#define FLOAT_RANGE_CHECK(var, min, max, error)                                                                                                                                                        \
  if(m_##var < min || m_##var > max)                                                                                                                                                                   \
  {                                                                                                                                                                                                    \
    setErrorCondition(error, "Valid range for " #var " is " #min "~" #max);                                                                                                                            \
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::dataCheck()
{
  initialize();
  clearErrorCode();
  clearWarningCode();

  FLOAT_RANGE_CHECK(Mu, 0.0001, 10.0, -95000);
  FLOAT_RANGE_CHECK(Sigma, 0.0, 5.0, -95001);
  FLOAT_RANGE_CHECK(MinCutOff, 0, 1000000, -95002)
  FLOAT_RANGE_CHECK(MaxCutOff, 0, 1000000, -95003)

  if((m_CreateEnsembleAttributeMatrix && m_AppendToExistingAttributeMatrix) || (!m_CreateEnsembleAttributeMatrix && !m_AppendToExistingAttributeMatrix))
  {
    setErrorCondition(-95010, "CreateEnsembleAttributeMatrix & AppendToExistingAttributeMatrix can NOT both be true or false. One must be true and one must be false.");
    return;
  }

  // Use has asked to create a whole new EnsembleAttributeMatrix
  if(m_CreateEnsembleAttributeMatrix && !m_AppendToExistingAttributeMatrix)
  {
    DataContainerArray::Pointer dca = getDataContainerArray();
    DataContainer::Pointer dc = dca->createNonPrereqDataContainer(this, getDataContainerName());
    if(getErrorCode() < 0)
    {
      return;
    }

    std::vector<size_t> tDims(1, 2); // we need 2 slots in the array. ZERO=Junk, 1 = our new primary stats data
    AttributeMatrix::Pointer cellEnsembleAttrMat = dc->createNonPrereqAttributeMatrix(this, getCellEnsembleAttributeMatrixName(), tDims, AttributeMatrix::Type::CellEnsemble, AttributeMatrixID21);
    if(getErrorCode() < 0)
    {
      return;
    }
    StatsDataArray::Pointer statsDataArray = StatsDataArray::New();
    statsDataArray->resizeTuples(tDims[0]);
    cellEnsembleAttrMat->insertOrAssign(statsDataArray);
    m_StatsDataArray = statsDataArray.get();

    PrecipitateStatsData::Pointer PrecipitateStatsData = PrecipitateStatsData::New();
    statsDataArray->setStatsData(1, PrecipitateStatsData);
    m_PrecipitateStatsData = PrecipitateStatsData.get();

    std::vector<size_t> cDims(1, 1);
    UInt32ArrayType::Pointer crystalStructures = UInt32ArrayType::CreateArray(tDims, cDims, SIMPL::EnsembleData::CrystalStructures, true);
    crystalStructures->setValue(0, Ebsd::CrystalStructure::UnknownCrystalStructure);
    cellEnsembleAttrMat->insertOrAssign(crystalStructures);
    m_CrystalStructures = crystalStructures.get();

    UInt32ArrayType::Pointer phaseTypes = UInt32ArrayType::CreateArray(tDims, cDims, SIMPL::EnsembleData::PhaseTypes, true);
    phaseTypes->setValue(0, static_cast<PhaseType::EnumType>(PhaseType::Type::Unknown));
    cellEnsembleAttrMat->insertOrAssign(phaseTypes);
    m_PhaseTypes = phaseTypes.get();

    StringDataArray::Pointer phaseNames = StringDataArray::CreateArray(tDims[0], SIMPL::EnsembleData::PhaseName, true);
    phaseNames->setValue(0, PhaseType::getPhaseTypeString(PhaseType::Type::Unknown));
    cellEnsembleAttrMat->insertOrAssign(phaseNames);
    m_PhaseNames = phaseNames.get();

    setPhaseIndex(1); // If we are creating the StatsDataArray then we are the first phase
  }

  // User wants to Append to existing AttributeMatrix
  if(!m_CreateEnsembleAttributeMatrix && m_AppendToExistingAttributeMatrix)
  {
    DataContainerArray::Pointer dca = getDataContainerArray();
    AttributeMatrix::Pointer cellEnsembleAttrMat = dca->getAttributeMatrix(m_SelectedEnsembleAttributeMatrix);
    if(nullptr == cellEnsembleAttrMat.get())
    {
      setErrorCondition(-95020, QString("AttributeMatrix does not exist at path %1").arg(m_SelectedEnsembleAttributeMatrix.serialize("/")));
      return;
    }

    // Resize the AttributeMatrix, which should resize all the AttributeArrays
    std::vector<size_t> tDims(1, cellEnsembleAttrMat->getNumberOfTuples() + 1);
    cellEnsembleAttrMat->resizeAttributeArrays(tDims);

    StatsDataArray::Pointer statsDataArray = cellEnsembleAttrMat->getAttributeArrayAs<StatsDataArray>(SIMPL::EnsembleData::Statistics);
    if(nullptr == statsDataArray.get())
    {
      statsDataArray = StatsDataArray::New();
      statsDataArray->resizeTuples(tDims[0]);
      cellEnsembleAttrMat->insertOrAssign(statsDataArray);
    }
    m_StatsDataArray = statsDataArray.get();

    PrecipitateStatsData::Pointer PrecipitateStatsData = PrecipitateStatsData::New();
    statsDataArray->setStatsData(tDims[0] - 1, PrecipitateStatsData);
    m_PrecipitateStatsData = PrecipitateStatsData.get();

    std::vector<size_t> cDims(1, 1);

    UInt32ArrayType::Pointer crystalStructures = cellEnsembleAttrMat->getAttributeArrayAs<UInt32ArrayType>(SIMPL::EnsembleData::CrystalStructures);
    if(nullptr == crystalStructures.get())
    {
      crystalStructures = UInt32ArrayType::CreateArray(tDims, cDims, SIMPL::EnsembleData::CrystalStructures, true);
      crystalStructures->setValue(0, Ebsd::CrystalStructure::UnknownCrystalStructure);
      cellEnsembleAttrMat->insertOrAssign(crystalStructures);
    }
    m_CrystalStructures = crystalStructures.get();

    UInt32ArrayType::Pointer phaseTypes = cellEnsembleAttrMat->getAttributeArrayAs<UInt32ArrayType>(SIMPL::EnsembleData::PhaseTypes);
    if(nullptr == phaseTypes.get())
    {
      phaseTypes = UInt32ArrayType::CreateArray(tDims, cDims, SIMPL::EnsembleData::PhaseTypes, true);
      phaseTypes->setValue(0, static_cast<PhaseType::EnumType>(PhaseType::Type::Unknown));
      cellEnsembleAttrMat->insertOrAssign(phaseTypes);
    }
    m_PhaseTypes = phaseTypes.get();

    StringDataArray::Pointer phaseNames = cellEnsembleAttrMat->getAttributeArrayAs<StringDataArray>(SIMPL::EnsembleData::PhaseName);
    if(nullptr == phaseNames.get())
    {
      phaseNames = StringDataArray::CreateArray(tDims[0], SIMPL::EnsembleData::PhaseName, true);
      phaseNames->setValue(0, PhaseType::getPhaseTypeString(PhaseType::Type::Unknown));
      cellEnsembleAttrMat->insertOrAssign(phaseNames);
    }
    m_PhaseNames = phaseNames.get();

    setPhaseIndex(tDims[0] - 1); // If we are creating the StatsDataArray then we are the first phase
  }

  // If all goes well, then calculate the number of bins to display back on the user interface.
  float max, min; // Only needed for the method. Not used otherwise.
  int numBins = StatsGen::ComputeNumberOfBins(m_Mu, m_Sigma, m_MinCutOff, m_MaxCutOff, m_BinStepSize, max, min);
  m_NumberOfBins = QString::number(numBins, 10);
  // Calculate a more understandable "Grain Size" for the user
  float esd = std::exp(m_Mu);
  QLocale loc = QLocale::system();
  m_FeatureESD = loc.toString(esd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getCancel())
  {
    return;
  }

  if(getErrorCode() < 0)
  {
    QString ss = QObject::tr("Some error message");
    setErrorCondition(-95012, ss);
    return;
  }

  typedef QVector<float> FloatVectorType;
  int err = 0;
  int size = 250;
  FloatVectorType x;
  FloatVectorType y;
  err = StatsGen::GenLogNormalPlotData<FloatVectorType>(m_Mu, m_Sigma, x, y, size, m_MinCutOff, m_MaxCutOff);
  if(err == 1)
  {
    setErrorCondition(-95011, "Error generating the LogNormal Data");
    return;
  }
  float yMax = 0.0f;
  // float xMax = x[size-1];
  for(int i = 0; i < size; ++i)
  {
    if(y[i] > yMax)
    {
      yMax = y[i];
    }
  }

  FloatVectorType xCo;
  FloatVectorType yCo;
  int numsizebins = 1;
  FloatVectorType binSizes;
  err = StatsGen::GenCutOff<float, FloatVectorType>(m_Mu, m_Sigma, m_MinCutOff, m_MaxCutOff, m_BinStepSize, xCo, yCo, yMax, numsizebins, binSizes);
  if(err == 1)
  {
    setErrorCondition(-95012, "Error generating the Min or Max Cut Off values");
    return;
  }

  QMap<QString, QVector<float>> dataMap;
  dataMap[AbstractMicrostructurePreset::kBinNumbers] = binSizes;

  AbstractMicrostructurePreset::Pointer absPresetPtr;
  if(m_MicroPresetModel == 0)
  {
    absPresetPtr = PrimaryEquiaxedPreset::New();
  }
  else if(m_MicroPresetModel == 1)
  {
    absPresetPtr = PrimaryRolledPreset::New();
  }
  else if(m_MicroPresetModel == 2)
  {
    absPresetPtr = PrimaryRecrystallizedPreset::New();
  }

  m_CrystalStructures->setComponent(m_PhaseIndex, 0, m_CrystalSymmetry);
  m_PhaseNames->setValue(m_PhaseIndex, m_PhaseName);
  m_PhaseTypes->setValue(m_PhaseIndex, static_cast<PhaseType::EnumType>(PhaseType::Type::Precipitate));
  m_PrecipitateStatsData->setName(m_PhaseName);
  m_PrecipitateStatsData->setPhaseFraction(m_PhaseFraction);

  normalizePhaseFractions(m_StatsDataArray);

  // Feature Diameter Info
  m_PrecipitateStatsData->setBinStepSize(m_BinStepSize);
  m_PrecipitateStatsData->setMaxFeatureDiameter(xCo[1]);
  m_PrecipitateStatsData->setMinFeatureDiameter(xCo[0]);
  // Feature Size Distribution
  {
    VectorOfFloatArray data;
    FloatArrayType::Pointer d1 = FloatArrayType::CreateArray(1, SIMPL::StringConstants::Average, true);
    FloatArrayType::Pointer d2 = FloatArrayType::CreateArray(1, SIMPL::StringConstants::StandardDeviation, true);
    data.push_back(d1);
    data.push_back(d2);
    d1->setValue(0, m_Mu);
    d2->setValue(0, m_Sigma);
    m_PrecipitateStatsData->setFeatureSizeDistribution(data);
    m_PrecipitateStatsData->setFeatureSize_DistType(SIMPL::DistributionType::LogNormal);
    m_PrecipitateStatsData->generateBinNumbers();
  }

  {
    absPresetPtr->initializeOmega3TableModel(dataMap); // Beta
    VectorOfFloatArray data;
    FloatArrayType::Pointer d1 = FloatArrayType::FromQVector(dataMap[AbstractMicrostructurePreset::kAlpha], SIMPL::StringConstants::Alpha);
    FloatArrayType::Pointer d2 = FloatArrayType::FromQVector(dataMap[AbstractMicrostructurePreset::kBeta], SIMPL::StringConstants::Beta);
    data.push_back(d1);
    data.push_back(d2);
    m_PrecipitateStatsData->setFeatureSize_Omegas(data);
    m_PrecipitateStatsData->setOmegas_DistType(absPresetPtr->getDistributionType(AbstractMicrostructurePreset::kOmega3Distribution));
  }

  {
    absPresetPtr->initializeBOverATableModel(dataMap); // Beta
    VectorOfFloatArray data;
    FloatArrayType::Pointer d1 = FloatArrayType::FromQVector(dataMap[AbstractMicrostructurePreset::kAlpha], SIMPL::StringConstants::Alpha);
    FloatArrayType::Pointer d2 = FloatArrayType::FromQVector(dataMap[AbstractMicrostructurePreset::kBeta], SIMPL::StringConstants::Beta);
    data.push_back(d1);
    data.push_back(d2);
    m_PrecipitateStatsData->setFeatureSize_BOverA(data);
    m_PrecipitateStatsData->setBOverA_DistType(absPresetPtr->getDistributionType(AbstractMicrostructurePreset::kBOverADistribution));
  }

  {
    absPresetPtr->initializeCOverATableModel(dataMap); // Beta
    VectorOfFloatArray data;
    FloatArrayType::Pointer d1 = FloatArrayType::FromQVector(dataMap[AbstractMicrostructurePreset::kAlpha], SIMPL::StringConstants::Alpha);
    FloatArrayType::Pointer d2 = FloatArrayType::FromQVector(dataMap[AbstractMicrostructurePreset::kBeta], SIMPL::StringConstants::Beta);
    data.push_back(d1);
    data.push_back(d2);
    m_PrecipitateStatsData->setFeatureSize_COverA(data);
    m_PrecipitateStatsData->setBOverA_DistType(absPresetPtr->getDistributionType(AbstractMicrostructurePreset::kCOverADistribution));
  }

  //  {
  //    absPresetPtr->initializeNeighborTableModel(dataMap, colors); // LogNormal

  //    VectorOfFloatArray data;
  //    FloatArrayType::Pointer d1 = FloatArrayType::FromQVector(dataMap[AbstractMicrostructurePreset::kMu], SIMPL::StringConstants::Average);
  //    FloatArrayType::Pointer d2 = FloatArrayType::FromQVector(dataMap[AbstractMicrostructurePreset::kSigma], SIMPL::StringConstants::StandardDeviation);
  //    data.push_back(d1);
  //    data.push_back(d2);
  //    m_PrecipitateStatsData->setFeatureSize_Neighbors(data);
  //    m_PrecipitateStatsData->setBOverA_DistType(absPresetPtr->getDistributionType(AbstractMicrostructurePreset::kNeighborDistribution));
  //  }

  QString msg;
  QTextStream ss(&msg);

  ss << getPhaseName() << ":: Initialize ODF Values....";
  notifyStatusMessage(msg);

  {
    absPresetPtr->initializeODFTableModel(dataMap);

    QVector<float> e1s;
    QVector<float> e2s;
    QVector<float> e3s;
    QVector<float> weights;
    QVector<float> sigmas;

    std::vector<std::vector<double>> odfData = m_OdfData.getTableData();
    for(size_t i = 0; i < odfData.size(); i++)
    {
      e1s.push_back(static_cast<float>(odfData[i][0] * SIMPLib::Constants::k_PiOver180));
      e2s.push_back(static_cast<float>(odfData[i][1] * SIMPLib::Constants::k_PiOver180));
      e3s.push_back(static_cast<float>(odfData[i][2] * SIMPLib::Constants::k_PiOver180));
      weights.push_back(static_cast<float>(odfData[i][3]));
      sigmas.push_back(static_cast<float>(odfData[i][4]));
    }
    // Convert angles to Radians when this is implemented
    StatsGeneratorUtilities::GenerateODFBinData(m_PrecipitateStatsData, PhaseType::Type::Precipitate, static_cast<uint32_t>(m_CrystalSymmetry), e1s, e2s, e3s, weights, sigmas, true);
  }

  msg.clear();
  ss << getPhaseName() << ":: Initialize MDF Values....";
  notifyStatusMessage(msg);
  {
    absPresetPtr->initializeMDFTableModel(dataMap);
    QVector<float> e1s;
    QVector<float> e2s;
    QVector<float> e3s;
    QVector<float> odf_weights;
    QVector<float> sigmas;
    std::vector<std::vector<double>> odfData = m_OdfData.getTableData();
    for(size_t i = 0; i < odfData.size(); i++)
    {
      e1s.push_back(static_cast<float>(odfData[i][0] * SIMPLib::Constants::k_PiOver180));
      e2s.push_back(static_cast<float>(odfData[i][1] * SIMPLib::Constants::k_PiOver180));
      e3s.push_back(static_cast<float>(odfData[i][2] * SIMPLib::Constants::k_PiOver180));
      odf_weights.push_back(static_cast<float>(odfData[i][3]));
      sigmas.push_back(static_cast<float>(odfData[i][4]));
    }

    QVector<float> odf = StatsGeneratorUtilities::GenerateODFData(m_CrystalSymmetry, e1s, e2s, e3s, odf_weights, sigmas, true);

    QVector<float> angles;
    QVector<float> axes;
    QVector<float> weights;
    std::vector<std::vector<double>> mdfData = m_MdfData.getTableData();
    for(size_t i = 0; i < mdfData.size(); i++)
    {
      angles.push_back(static_cast<float>(mdfData[i][0]));
      axes.push_back(static_cast<float>(mdfData[i][1]));
      axes.push_back(static_cast<float>(mdfData[i][2]));
      axes.push_back(static_cast<float>(mdfData[i][3]));
      weights.push_back(static_cast<float>(mdfData[i][4]));
    }

    StatsGeneratorUtilities::GenerateMisorientationBinData(m_PrecipitateStatsData, PhaseType::Type::Precipitate, m_CrystalSymmetry, odf, angles, axes, weights, true);
  }

  msg.clear();
  ss << getPhaseName() << ":: Initialize Axis ODF Values....";
  notifyStatusMessage(msg);
  {
    absPresetPtr->initializeAxisODFTableModel(dataMap);
    QVector<float> e1s;
    QVector<float> e2s;
    QVector<float> e3s;
    QVector<float> weights;
    QVector<float> sigmas;
    std::vector<std::vector<double>> axisOdfData = m_AxisOdfData.getTableData();
    for(size_t i = 0; i < axisOdfData.size(); i++)
    {
      e1s.push_back(static_cast<float>(axisOdfData[i][0] * SIMPLib::Constants::k_PiOver180));
      e2s.push_back(static_cast<float>(axisOdfData[i][1] * SIMPLib::Constants::k_PiOver180));
      e3s.push_back(static_cast<float>(axisOdfData[i][2] * SIMPLib::Constants::k_PiOver180));
      weights.push_back(static_cast<float>(axisOdfData[i][3]));
      sigmas.push_back(static_cast<float>(axisOdfData[i][4]));
    }
    StatsGeneratorUtilities::GenerateAxisODFBinData(m_PrecipitateStatsData, PhaseType::Type::Precipitate, e1s, e2s, e3s, weights, sigmas, true);
  }

  msg.clear();
  ss << getPhaseName() << ":: Initialize RDF Values....";
  notifyStatusMessage(msg);

  {
    std::array<float, 3> boxDims = {m_RdfBoxSize[0], m_RdfBoxSize[1], m_RdfBoxSize[2]};
    std::array<float, 3> boxRes = {0.1f, 0.1f, 0.1f};

    // Generate the RDF Frequencies
    std::vector<float> rdfFrequencies = RadialDistributionFunction::GenerateRandomDistribution(m_RdfMinMaxDistance[0], m_RdfMinMaxDistance[1], m_RdfNumBins, boxDims, boxRes);
    QVector<float> qFreq = QVector<float>::fromStdVector(rdfFrequencies);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GeneratePrecipitateStatsData::getNumberOfBins()
{
  return m_NumberOfBins;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GeneratePrecipitateStatsData::getFeatureESD()
{
  return m_FeatureESD;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::normalizePhaseFractions(StatsDataArray* statsDataArray)
{
  size_t count = statsDataArray->getNumberOfTuples();
  // Start at index 1 since the first one is junk
  float totalPhaseFraction = 0.0f;
  for(size_t i = 1; i < count; i++)
  {
    StatsData::Pointer statsData = statsDataArray->getStatsData(i);
    totalPhaseFraction = totalPhaseFraction + statsData->getPhaseFraction();
  }
  // Now loop again and set the correct phase fractions
  for(size_t i = 1; i < count; i++)
  {
    StatsData::Pointer statsData = statsDataArray->getStatsData(i);
    statsData->setPhaseFraction(statsData->getPhaseFraction() / totalPhaseFraction);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GeneratePrecipitateStatsData::newFilterInstance(bool copyFilterParameters) const
{
  GeneratePrecipitateStatsData::Pointer filter = GeneratePrecipitateStatsData::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GeneratePrecipitateStatsData::getCompiledLibraryName() const
{
  return SyntheticBuildingConstants::SyntheticBuildingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GeneratePrecipitateStatsData::getBrandingString() const
{
  return "StatsGenerator";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GeneratePrecipitateStatsData::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SyntheticBuilding::Version::Major() << "." << SyntheticBuilding::Version::Minor() << "." << SyntheticBuilding::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GeneratePrecipitateStatsData::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid GeneratePrecipitateStatsData::getUuid() const
{
  return QUuid("{16659766-5c53-5ada-a7b7-8a95c29ea674}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GeneratePrecipitateStatsData::getSubGroupName() const
{
  return "StatsGenerator";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GeneratePrecipitateStatsData::getHumanLabel() const
{
  return "Generate Precipitate StatsData";
}

// -----------------------------------------------------------------------------
GeneratePrecipitateStatsData::Pointer GeneratePrecipitateStatsData::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<GeneratePrecipitateStatsData> GeneratePrecipitateStatsData::New()
{
  struct make_shared_enabler : public GeneratePrecipitateStatsData
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString GeneratePrecipitateStatsData::getNameOfClass() const
{
  return QString("GeneratePrecipitateStatsData");
}

// -----------------------------------------------------------------------------
QString GeneratePrecipitateStatsData::ClassName()
{
  return QString("GeneratePrecipitateStatsData");
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setPhaseName(const QString& value)
{
  m_PhaseName = value;
}

// -----------------------------------------------------------------------------
QString GeneratePrecipitateStatsData::getPhaseName() const
{
  return m_PhaseName;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setPhaseIndex(int value)
{
  m_PhaseIndex = value;
}

// -----------------------------------------------------------------------------
int GeneratePrecipitateStatsData::getPhaseIndex() const
{
  return m_PhaseIndex;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setCrystalSymmetry(int value)
{
  m_CrystalSymmetry = value;
}

// -----------------------------------------------------------------------------
int GeneratePrecipitateStatsData::getCrystalSymmetry() const
{
  return m_CrystalSymmetry;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setMicroPresetModel(int value)
{
  m_MicroPresetModel = value;
}

// -----------------------------------------------------------------------------
int GeneratePrecipitateStatsData::getMicroPresetModel() const
{
  return m_MicroPresetModel;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setPhaseFraction(double value)
{
  m_PhaseFraction = value;
}

// -----------------------------------------------------------------------------
double GeneratePrecipitateStatsData::getPhaseFraction() const
{
  return m_PhaseFraction;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setMu(double value)
{
  m_Mu = value;
}

// -----------------------------------------------------------------------------
double GeneratePrecipitateStatsData::getMu() const
{
  return m_Mu;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setSigma(double value)
{
  m_Sigma = value;
}

// -----------------------------------------------------------------------------
double GeneratePrecipitateStatsData::getSigma() const
{
  return m_Sigma;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setMinCutOff(double value)
{
  m_MinCutOff = value;
}

// -----------------------------------------------------------------------------
double GeneratePrecipitateStatsData::getMinCutOff() const
{
  return m_MinCutOff;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setMaxCutOff(double value)
{
  m_MaxCutOff = value;
}

// -----------------------------------------------------------------------------
double GeneratePrecipitateStatsData::getMaxCutOff() const
{
  return m_MaxCutOff;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setBinStepSize(double value)
{
  m_BinStepSize = value;
}

// -----------------------------------------------------------------------------
double GeneratePrecipitateStatsData::getBinStepSize() const
{
  return m_BinStepSize;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setCreateEnsembleAttributeMatrix(bool value)
{
  m_CreateEnsembleAttributeMatrix = value;
}

// -----------------------------------------------------------------------------
bool GeneratePrecipitateStatsData::getCreateEnsembleAttributeMatrix() const
{
  return m_CreateEnsembleAttributeMatrix;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setDataContainerName(const DataArrayPath& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GeneratePrecipitateStatsData::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setCellEnsembleAttributeMatrixName(const QString& value)
{
  m_CellEnsembleAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString GeneratePrecipitateStatsData::getCellEnsembleAttributeMatrixName() const
{
  return m_CellEnsembleAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setAppendToExistingAttributeMatrix(bool value)
{
  m_AppendToExistingAttributeMatrix = value;
}

// -----------------------------------------------------------------------------
bool GeneratePrecipitateStatsData::getAppendToExistingAttributeMatrix() const
{
  return m_AppendToExistingAttributeMatrix;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setSelectedEnsembleAttributeMatrix(const DataArrayPath& value)
{
  m_SelectedEnsembleAttributeMatrix = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GeneratePrecipitateStatsData::getSelectedEnsembleAttributeMatrix() const
{
  return m_SelectedEnsembleAttributeMatrix;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setOdfData(const DynamicTableData& value)
{
  m_OdfData = value;
}

// -----------------------------------------------------------------------------
DynamicTableData GeneratePrecipitateStatsData::getOdfData() const
{
  return m_OdfData;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setMdfData(const DynamicTableData& value)
{
  m_MdfData = value;
}

// -----------------------------------------------------------------------------
DynamicTableData GeneratePrecipitateStatsData::getMdfData() const
{
  return m_MdfData;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setAxisOdfData(const DynamicTableData& value)
{
  m_AxisOdfData = value;
}

// -----------------------------------------------------------------------------
DynamicTableData GeneratePrecipitateStatsData::getAxisOdfData() const
{
  return m_AxisOdfData;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setRdfMinMaxDistance(const FloatVec2Type& value)
{
  m_RdfMinMaxDistance = value;
}

// -----------------------------------------------------------------------------
FloatVec2Type GeneratePrecipitateStatsData::getRdfMinMaxDistance() const
{
  return m_RdfMinMaxDistance;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setRdfNumBins(int value)
{
  m_RdfNumBins = value;
}

// -----------------------------------------------------------------------------
int GeneratePrecipitateStatsData::getRdfNumBins() const
{
  return m_RdfNumBins;
}

// -----------------------------------------------------------------------------
void GeneratePrecipitateStatsData::setRdfBoxSize(const FloatVec3Type& value)
{
  m_RdfBoxSize = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type GeneratePrecipitateStatsData::getRdfBoxSize() const
{
  return m_RdfBoxSize;
}
