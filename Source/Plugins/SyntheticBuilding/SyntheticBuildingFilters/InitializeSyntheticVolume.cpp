/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "InitializeSyntheticVolume.h"

#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/DataArrays/StringDataArray.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/StatsData/BoundaryStatsData.h"
#include "SIMPLib/StatsData/MatrixStatsData.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/StatsData/TransformationStatsData.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"
#include "SyntheticBuilding/SyntheticBuildingVersion.h"

#define INIT_SYNTH_VOLUME_CHECK(var, errCond)                                                                                                                                                          \
  if(m_##var <= 0)                                                                                                                                                                                     \
  {                                                                                                                                                                                                    \
    QString ss = QObject::tr("%1 must be positive").arg(#var);                                                                                                                                         \
    notifyErrorMessage(getHumanLabel(), ss, errCond);                                                                                                                                                  \
  }

// Include the MOC generated file for this class
#include "moc_InitializeSyntheticVolume.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InitializeSyntheticVolume::InitializeSyntheticVolume()
: AbstractFilter()
, m_DataContainerName(SIMPL::Defaults::SyntheticVolumeDataContainerName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_EnsembleAttributeMatrixName(SIMPL::Defaults::CellEnsembleAttributeMatrixName)
, m_InputStatsArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::Statistics)
, m_InputPhaseTypesArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::PhaseTypes)
, m_InputPhaseNamesArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::PhaseName)
, m_EstimateNumberOfFeatures(false)
, m_InputStatsFile("")
, m_EstimatedPrimaryFeatures("")
{
  m_Dimensions.x = 128;
  m_Dimensions.y = 128;
  m_Dimensions.z = 128;

  m_Resolution.x = 0.25;
  m_Resolution.y = 0.25;
  m_Resolution.z = 0.25;

  m_Origin.x = 0.0f;
  m_Origin.y = 0.0f;
  m_Origin.z = 0.0f;

  m_EstimatedPrimaryFeatures = "";

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InitializeSyntheticVolume::~InitializeSyntheticVolume()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::setupFilterParameters()
{
  FilterParameterVector parameters;
  QStringList linkedProps("EstimatedPrimaryFeatures");
  linkedProps << "InputStatsFile"
              << "InputPhaseTypesArrayPath";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Estimate Number of Features", EstimateNumberOfFeatures, FilterParameter::Parameter, InitializeSyntheticVolume, linkedProps));
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input Statistics File", InputStatsFile, FilterParameter::Parameter, InitializeSyntheticVolume, "*.dream3d"));

  PreflightUpdatedValueFilterParameter::Pointer param =
      SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Estimated Primary Features", EstimatedPrimaryFeatures, FilterParameter::Parameter, InitializeSyntheticVolume);
  param->setReadOnly(true);
  parameters.push_back(param);

  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));

  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::StatsDataArray, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    IGeometry::Types geomTypes;
    geomTypes.push_back(IGeometry::Type::Image);
    geomTypes.push_back(IGeometry::Type::Unknown);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Statistics", InputStatsArrayPath, FilterParameter::RequiredArray, InitializeSyntheticVolume, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    IGeometry::Types geomTypes;
    geomTypes.push_back(IGeometry::Type::Image);
    geomTypes.push_back(IGeometry::Type::Unknown);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phase Types", InputPhaseTypesArrayPath, FilterParameter::RequiredArray, InitializeSyntheticVolume, req));
  }
  //  {
  //    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble,
  //    IGeometry::Type::Any);
  //    IGeometry::Types geomTypes;
  //    geomTypes.push_back(IGeometry::Type::Image);
  //    geomTypes.push_back(IGeometry::Type::Unknown);
  //    req.dcGeometryTypes = geomTypes;
  //    req.daTypes = QVector<QString>(1, SIMPL::TypeNames::StringArray);
  //    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phase Names", InputPhaseNamesArrayPath, FilterParameter::RequiredArray, InitializeSyntheticVolume, req));
  //  }
  parameters.push_back(SIMPL_NEW_STRING_FP("Synthetic Volume Data Container", DataContainerName, FilterParameter::CreatedArray, InitializeSyntheticVolume));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix", CellAttributeMatrixName, FilterParameter::CreatedArray, InitializeSyntheticVolume));
  parameters.push_back(SIMPL_NEW_STRING_FP("Ensemble Attribute Matrix", EnsembleAttributeMatrixName, FilterParameter::CreatedArray, InitializeSyntheticVolume));

  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Dimensions", Dimensions, FilterParameter::Parameter, InitializeSyntheticVolume));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Resolution", Resolution, FilterParameter::Parameter, InitializeSyntheticVolume));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, InitializeSyntheticVolume));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  setDimensions(reader->readIntVec3("Dimensions", getDimensions()));
  setResolution(reader->readFloatVec3("Resolution", getResolution()));
  setOrigin(reader->readFloatVec3("Origin", getOrigin()));
  setInputStatsArrayPath(reader->readDataArrayPath("InputStatsArrayPath", getInputStatsArrayPath()));
  setInputPhaseTypesArrayPath(reader->readDataArrayPath("InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath()));
  setInputStatsFile(reader->readString("InputStatsFile", getInputStatsFile()));
  setEstimateNumberOfFeatures(reader->readValue("EstimateNumberOfFeatures", getEstimateNumberOfFeatures()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  // Create the output Data Container
  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);

  // Sanity Check the Dimensions and Resolution
  INIT_SYNTH_VOLUME_CHECK(Dimensions.x, -5000);
  INIT_SYNTH_VOLUME_CHECK(Dimensions.y, -5001);
  INIT_SYNTH_VOLUME_CHECK(Dimensions.z, -5002);
  INIT_SYNTH_VOLUME_CHECK(Resolution.x, -5003);
  INIT_SYNTH_VOLUME_CHECK(Resolution.y, -5004);
  INIT_SYNTH_VOLUME_CHECK(Resolution.z, -5005);

  // Set the Dimensions, Resolution and Origin of the output data container
  m->getGeometryAs<ImageGeom>()->setDimensions(m_Dimensions.x, m_Dimensions.y, m_Dimensions.z);
  m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->getGeometryAs<ImageGeom>()->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

  // Create our output Cell and Ensemble Attribute Matrix objects
  QVector<size_t> tDims(3, 0);
  tDims[0] = m_Dimensions.x;
  tDims[1] = m_Dimensions.y;
  tDims[2] = m_Dimensions.z;
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
  if(getErrorCondition() < 0 && cellAttrMat.get() == nullptr)
  {
    return;
  }

#if 0
  // Get the number of tuples from the Ensemble Attribute Matrix from the Synthetic Stats Side of things
  AttributeMatrix::Pointer ensembleAM = getDataContainerArray()->getAttributeMatrix(getInputPhaseTypesArrayPath());
  tDims.resize(1);
  tDims[0] = ensembleAM->getNumberOfTuples();

  // Create our own Ensemble Attribute Matrix
  AttributeMatrix::Pointer ensembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getEnsembleAttributeMatrixName(), tDims, AttributeMatrix::Type::CellEnsemble);
  if(getErrorCondition() < 0 && cellAttrMat.get() == nullptr) { return; }

  QVector<size_t> cDims(1, 1); // This states that we are looking for an array with a single component
  UInt32ArrayType::Pointer phaseType = getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType, AbstractFilter>(this, getInputPhaseTypesArrayPath(), cDims);
  if(getErrorCondition() < 0 && phaseType.get() == nullptr) { return; }
  IDataArray::Pointer phaseTypesCopy = phaseType->deepCopy(getInPreflight());
  ensembleAttrMat->addAttributeArray(phaseTypesCopy->getName(), phaseTypesCopy);


  ensembleAM = getDataContainerArray()->getAttributeMatrix(getInputPhaseNamesArrayPath());
  StringDataArray::Pointer phaseNames = getDataContainerArray()->getPrereqArrayFromPath<StringDataArray, AbstractFilter>(this, getInputPhaseNamesArrayPath(), cDims);
  if(getErrorCondition() < 0 && phaseNames.get() == nullptr) { return; }
  IDataArray::Pointer phaseNamesCopy = phaseNames->deepCopy(getInPreflight());
  ensembleAttrMat->addAttributeArray(phaseNamesCopy->getName(), phaseNamesCopy);


  QVector<size_t> statsDims(1, 1);
  StatsDataArray::Pointer statsPtr = getDataContainerArray()->getPrereqArrayFromPath<StatsDataArray, AbstractFilter>(this, getInputStatsArrayPath(), statsDims);
  if(getErrorCondition() < 0 && statsPtr.get() == nullptr) { return; }

#endif
  if(m_EstimateNumberOfFeatures)
  {
    m_EstimatedPrimaryFeatures = estimateNumFeatures(m_Dimensions, m_Resolution);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());

  // Resize the Cell AttributeMatrix to have the correct Tuple Dimensions.
  QVector<size_t> tDims(3, 0);
  tDims[0] = m->getGeometryAs<ImageGeom>()->getXPoints();
  tDims[1] = m->getGeometryAs<ImageGeom>()->getYPoints();
  tDims[2] = m->getGeometryAs<ImageGeom>()->getZPoints();
  cellAttrMat->resizeAttributeArrays(tDims);

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InitializeSyntheticVolume::estimateNumFeatures(IntVec3_t dims, FloatVec3_t res)
{
  float totalvol = 0.0f;
  int32_t phase = 0;

  totalvol = (dims.x * res.x) * (dims.y * res.y) * (dims.z * res.z);
  if(totalvol == 0.0)
  {
    return "-1";
  }

  DataContainerArray::Pointer dca = getDataContainerArray();

  // Get the PhaseTypes - Remember there is a Dummy PhaseType in the first slot of the array
  QVector<size_t> cDims(1, 1); // This states that we are looking for an array with a single component
  UInt32ArrayType::Pointer phaseType = dca->getPrereqArrayFromPath<UInt32ArrayType, AbstractFilter>(nullptr, getInputPhaseTypesArrayPath(), cDims);
  if(phaseType.get() == nullptr)
  {
    QString ss =
        QObject::tr("Phase types array could not be downcast using std::dynamic_pointer_cast<T> when estimating the number of grains. The path is %1").arg(getInputPhaseTypesArrayPath().serialize());
    setErrorCondition(-11002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return "0";
  }

  QVector<size_t> statsDims(1, 1);
  StatsDataArray::Pointer statsPtr = dca->getPrereqArrayFromPath<StatsDataArray, AbstractFilter>(this, getInputStatsArrayPath(), statsDims);
  if(statsPtr.get() == nullptr)
  {
    QString ss =
        QObject::tr("Statistics array could not be downcast using std::dynamic_pointer_cast<T> when estimating the number of grains. The path is %1").arg(getInputStatsArrayPath().serialize());
    notifyErrorMessage(getHumanLabel(), ss, -11001);
    return "0";
  }

  if(!phaseType->isAllocated())
  {
    if(getInputStatsFile().isEmpty())
    {
      QString ss = QObject::tr("Phase types array has not been allocated and the input statistics file is empty");
      setWarningCondition(-1000);
      notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
      return "-1";
    }
    QFileInfo fi(getInputStatsFile());
    if(fi.exists() == false)
    {
      QString ss = QObject::tr("Phase types array has not been allocated and the input statistics file does not exist at '%1'").arg(fi.absoluteFilePath());
      setWarningCondition(-1001);
      notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
      return "-1";
    }

    hid_t fileId = -1;
    herr_t err = 0;
    // open the file
    fileId = H5Utilities::openFile(getInputStatsFile().toLatin1().constData(), true);
    // This will make sure if we return early from this method that the HDF5 File is properly closed.
    HDF5ScopedFileSentinel scopedFileSentinel(&fileId, true);

    DataArrayPath dap = getInputPhaseTypesArrayPath();
    // Generate the path to the AttributeMatrix
    QString hPath = SIMPL::StringConstants::DataContainerGroupName + "/" + dap.getDataContainerName() + "/" + dap.getAttributeMatrixName();
    // Open the AttributeMatrix Group
    hid_t amGid = H5Gopen(fileId, hPath.toLatin1().data(), H5P_DEFAULT);
    scopedFileSentinel.addGroupId(&amGid);
    err = phaseType->readH5Data(amGid);
    if(err < 0)
    {
      QString ss = QObject::tr("Error reading phase type data");
      setWarningCondition(-1003);
      notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
      return "-1";
    }
    if(!phaseType->isAllocated())
    {
      QString ss = QObject::tr("Phase types Array was not allocated due to an error reading the data from the statistics file %1").arg(fi.absoluteFilePath());
      setWarningCondition(-1002);
      notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
      return "-1";
    }
  }

  // Create a Reference Variable so we can use the [] syntax
  StatsDataArray& statsDataArray = *(statsPtr.get());

  std::vector<int32_t> primaryphases;
  std::vector<double> primaryphasefractions;
  double totalprimaryfractions = 0.0;
  // find which phases are primary phases

  for(size_t i = 1; i < phaseType->getNumberOfTuples(); ++i)
  {
    if(phaseType->getValue(i) == static_cast<PhaseType::EnumType>(PhaseType::Type::Primary))
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      primaryphases.push_back(i);
      primaryphasefractions.push_back(pp->getPhaseFraction());
      totalprimaryfractions = totalprimaryfractions + pp->getPhaseFraction();
    }
  }

  // scale the primary phase fractions to total to 1
  for(int32_t i = 0; i < primaryphasefractions.size(); i++)
  {
    primaryphasefractions[i] = primaryphasefractions[i] / totalprimaryfractions;
  }

  SIMPL_RANDOMNG_NEW()
  // generate the Features
  int32_t gid = 1;

  float currentvol = 0.0f;
  float vol = 0.0f;
  float diam = 0.0f;
  bool volgood = false;
  for(size_t j = 0; j < primaryphases.size(); ++j)
  {
    float curphasetotalvol = totalvol * primaryphasefractions[j];
    while(currentvol < curphasetotalvol)
    {
      volgood = false;
      phase = primaryphases[j];
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
      if(nullptr == pp)
      {
        QString ss = QObject::tr("Tried to cast a statsDataArray[%1].get() to a PrimaryStatsData* "
                                 "pointer but this resulted in a nullptr pointer.\n")
                         .arg(phase)
                         .arg(phase);
        setErrorCondition(-666);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return "-1";
      }
      while(volgood == false)
      {
        volgood = true;
        if(pp->getFeatureSize_DistType() == SIMPL::DistributionType::LogNormal)
        {
          VectorOfFloatArray fsdist = pp->getFeatureSizeDistribution();
          float avgdiam = 1.0f;
          float sddiam = 0.1f;
          if(fsdist.size() >= 2)
          {
            avgdiam = pp->getFeatureSizeDistribution().at(0)->getValue(0);
            sddiam = pp->getFeatureSizeDistribution().at(1)->getValue(0);
          }
          else
          {
            return QString("-1");
          }
          diam = rg.genrand_norm(avgdiam, sddiam);
          diam = expf(diam);
          if(diam >= pp->getMaxFeatureDiameter())
          {
            volgood = false;
          }
          if(diam < pp->getMinFeatureDiameter())
          {
            volgood = false;
          }
          vol = (4.0f / 3.0f) * (M_PI) * ((diam * 0.5f) * (diam * 0.5f) * (diam * 0.5f));
        }
      }
      currentvol = currentvol + vol;
      gid++;
    }
  }
  return QString::number(gid);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InitializeSyntheticVolume::getEstimatedPrimaryFeatures()
{
  return m_EstimatedPrimaryFeatures;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer InitializeSyntheticVolume::newFilterInstance(bool copyFilterParameters)
{
  InitializeSyntheticVolume::Pointer filter = InitializeSyntheticVolume::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeSyntheticVolume::getCompiledLibraryName()
{
  return SyntheticBuildingConstants::SyntheticBuildingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeSyntheticVolume::getBrandingString()
{
  return "SyntheticBuilding";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeSyntheticVolume::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SyntheticBuilding::Version::Major() << "." << SyntheticBuilding::Version::Minor() << "." << SyntheticBuilding::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeSyntheticVolume::getGroupName()
{
  return SIMPL::FilterGroups::SyntheticBuildingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeSyntheticVolume::getSubGroupName()
{
  return SIMPL::FilterSubGroups::PackingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeSyntheticVolume::getHumanLabel()
{
  return "Initialize Synthetic Volume";
}
