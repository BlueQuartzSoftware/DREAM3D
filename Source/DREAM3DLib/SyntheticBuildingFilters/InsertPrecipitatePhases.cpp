/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "InsertPrecipitatePhases.h"

#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "DREAM3DLib/ShapeOps/CubeOctohedronOps.h"
#include "DREAM3DLib/ShapeOps/CylinderOps.h"
#include "DREAM3DLib/ShapeOps/EllipsoidOps.h"
#include "DREAM3DLib/ShapeOps/SuperEllipsoidOps.h"

#include "DREAM3DLib/GenericFilters/FindSurfaceCells.h"
#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/RenumberFeatures.h"

#include "DREAM3DLib/StatsData/PrecipitateStatsData.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertPrecipitatePhases::InsertPrecipitatePhases() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::HDF5::CellAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::HDF5::CellFeatureAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::HDF5::CellEnsembleAttributeMatrixName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_SurfaceVoxelsArrayName(DREAM3D::CellData::SurfaceVoxels),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_AxisEulerAnglesArrayName(DREAM3D::FeatureData::AxisEulerAngles),
  m_AxisLengthsArrayName(DREAM3D::FeatureData::AxisLengths),
  m_CentroidsArrayName(DREAM3D::FeatureData::Centroids),
  m_EquivalentDiametersArrayName(DREAM3D::FeatureData::EquivalentDiameters),
  m_ClusteringListArrayName(DREAM3D::FeatureData::ClusteringList),
  m_NumCellsArrayName(DREAM3D::FeatureData::NumCells),
  m_Omega3sArrayName(DREAM3D::FeatureData::Omega3s),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_VolumesArrayName(DREAM3D::FeatureData::Volumes),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_ShapeTypesArrayName(DREAM3D::EnsembleData::ShapeTypes),
  m_NumFeaturesArrayName(DREAM3D::EnsembleData::NumFeatures),
  m_ErrorOutputFile("/Users/joetuck/Desktop/Microtexture/StatsGen/error.txt"),
  m_CsvOutputFile(""),
  m_PeriodicBoundaries(false),
  m_WriteGoalAttributes(false),
  m_FeatureIds(NULL),
  m_CellPhases(NULL),
  m_SurfaceVoxels(NULL),
  m_AxisEulerAngles(NULL),
  m_Centroids(NULL),
  m_AxisLengths(NULL),
  m_Volumes(NULL),
  m_Omega3s(NULL),
  m_EquivalentDiameters(NULL),
  m_Active(NULL),
  m_FeaturePhases(NULL),
  m_NumCells(NULL),
  m_ClusteringList(NULL),
  m_PhaseTypes(NULL),
  m_ShapeTypes(NULL),
  m_NumFeatures(NULL)
{
  m_EllipsoidOps = EllipsoidOps::New();
  m_ShapeOps[DREAM3D::ShapeType::EllipsoidShape] = m_EllipsoidOps.get();
  m_SuperEllipsoidOps = SuperEllipsoidOps::New();
  m_ShapeOps[DREAM3D::ShapeType::SuperEllipsoidShape] = m_SuperEllipsoidOps.get();
  m_CubicOctohedronOps = CubeOctohedronOps::New();
  m_ShapeOps[DREAM3D::ShapeType::CubeOctahedronShape] = m_CubicOctohedronOps.get();
  m_CylinderOps = CylinderOps::New();
  m_ShapeOps[DREAM3D::ShapeType::CylinderShape] = m_CylinderOps.get();
  m_UnknownShapeOps = ShapeOps::New();
  m_ShapeOps[DREAM3D::ShapeType::UnknownShapeType] = m_UnknownShapeOps.get();

  m_OrthoOps = OrthoRhombicOps::New();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertPrecipitatePhases::~InsertPrecipitatePhases()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void InsertPrecipitatePhases::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Periodic Boundary");
    option->setPropertyName("PeriodicBoundaries");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Goal Attributes");
    option->setPropertyName("WriteGoalAttributes");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Goal Attribute CSV File");
    option->setPropertyName("CsvOutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.csv");
    option->setFileType("Comma Separated Data");
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setPeriodicBoundaries( reader->readValue("PeriodicBoundaries", false) );
  setWriteGoalAttributes( reader->readValue("WriteGoalAttributes", false) );
  setCsvOutputFile( reader->readString( "CsvOutputFile", getCsvOutputFile() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int InsertPrecipitatePhases::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("PeriodicBoundaries", getPeriodicBoundaries() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  // Cell Data
  QVector<int> dims(1,1);
  m_FeatureIdsPtr = m->getPrereqArray<int32_t, Int32ArrayType, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -301, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_FeatureIds */
  m_CellPhasesPtr = m->getPrereqArray<int32_t, Int32ArrayType, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellPhasesArrayName, -301, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_CellPhases */
  m_SurfaceVoxelsPtr = m->getPrereqArray<int8_t, Int8ArrayType, AbstractFilter>(this, m_CellAttributeMatrixName,  m_SurfaceVoxelsArrayName, -301, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_SurfaceVoxels = m_SurfaceVoxelsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_SurfaceVoxels */

  // Feature Data
  m_FeaturePhasesPtr = m->createNonPrereqArray<int32_t, Int32ArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_FeaturePhasesArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_FeaturePhases */
  m_ActivePtr = m->createNonPrereqArray<bool, BoolArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_ActiveArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_Active = m_ActivePtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_Active */
  m_NumCellsPtr = m->createNonPrereqArray<int32_t, Int32ArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_NumCellsArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_NumCells = m_NumCellsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_NumCells */
  m_EquivalentDiametersPtr = m->createNonPrereqArray<float, FloatArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_EquivalentDiametersArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_EquivalentDiameters */
  m_VolumesPtr = m->createNonPrereqArray<float, FloatArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_VolumesArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_Volumes = m_VolumesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_Volumes */
  m_Omega3sPtr = m->createNonPrereqArray<float, FloatArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_Omega3sArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_Omega3s = m_Omega3sPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_Omega3s */
  dims[0] = 3;
  m_CentroidsPtr = m->createNonPrereqArray<float, FloatArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_CentroidsArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_Centroids = m_CentroidsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_Centroids */
  m_AxisEulerAnglesPtr = m->createNonPrereqArray<float, FloatArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_AxisEulerAnglesArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_AxisEulerAngles = m_AxisEulerAnglesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_AxisEulerAngles */
  m_AxisLengthsPtr = m->createNonPrereqArray<float, FloatArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_AxisLengthsArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_AxisLengths = m_AxisLengthsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_AxisLengths */

  //Ensemble Data
  dims[0] = 1;
  typedef DataArray<unsigned int> PhaseTypeArrayType;
  typedef DataArray<unsigned int> ShapeTypeArrayType;
  m_PhaseTypesPtr = m->getPrereqArray<unsigned int, PhaseTypeArrayType, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_PhaseTypesArrayName, -301, ensembles, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_PhaseTypes */
  m_ShapeTypesPtr = m->getPrereqArray<unsigned int, PhaseTypeArrayType, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_ShapeTypesArrayName, -301, ensembles, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_ShapeTypes = m_ShapeTypesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_ShapeTypes */
  m_NumFeaturesPtr = m->createNonPrereqArray<int32_t, Int32ArrayType, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_NumFeaturesArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_NumFeatures = m_NumFeaturesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_NumFeatures */

  m_StatsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getAttributeArray(DREAM3D::EnsembleData::Statistics).get());
  if(m_StatsDataArray == NULL)
  {
    QString ss = QObject::tr("Stats Array Not Initialized At Beginning Correctly");
    setErrorCondition(-308);
    addErrorMessage(getHumanLabel(), ss, -308);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    addErrorMessage(getHumanLabel(), "The VolumeDataContainer Object with the specific name " + getDataContainerName() + " was not available.", getErrorCondition());
    return;
  }

  dataCheck(true, 1, 1, 1);

  if (m_WriteGoalAttributes == true && getCsvOutputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Csv Output File Set and it was not.").arg(ClassName());
    addErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-387);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::execute()
{
  int err = 0;
  setErrorCondition(err);
  DREAM3D_RANDOMNG_NEW()
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  int64_t totalPoints = m->getAttributeMatrix(m_CellAttributeMatrixName)->getNumTuples();
  int64_t totalFeatures = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();
  int64_t totalEnsembles = m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getNumTuples();

  if(totalFeatures == 0) { totalFeatures = 1; }
  dataCheck(false, totalPoints, totalFeatures, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  sizex = m->getXPoints() * m->getXRes();
  sizey = m->getYPoints() * m->getYRes();
  sizez = m->getZPoints() * m->getZRes();

  totalvol = sizex * sizey * sizez;

  notifyStatusMessage("Packing Precipitates - Generating and Placing Precipitates");
// this initializes the arrays to hold the details of the locations of all of the features during packing
  Int32ArrayType::Pointer featureOwnersPtr = initialize_packinggrid();
  // Get a pointer to the Feature Owners that was just initialized in the initialize_packinggrid() method
//  int32_t* featureOwners = featureOwnersPtr->getPointer(0);
//  size_t featureOwnersIdx = 0;


  place_precipitates(featureOwnersPtr);

  notifyStatusMessage("Packing Precipitates - Assigning Voxels");
  assign_voxels();

  notifyStatusMessage("Packing Precipitates - Renumbering Features");
  RenumberFeatures::Pointer renumber_features1 = RenumberFeatures::New();
  renumber_features1->setObservers(this->getObservers());
  renumber_features1->setDataContainerArray(getDataContainerArray());
  renumber_features1->execute();
  err = renumber_features1->getErrorCondition();
  if (err < 0)
  {
    setErrorCondition(renumber_features1->getErrorCondition());
    addErrorMessages(renumber_features1->getPipelineMessages());
    return;
  }

  totalPoints = m->getAttributeMatrix(m_CellAttributeMatrixName)->getNumTuples();
  totalFeatures = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();
  totalEnsembles = m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getNumTuples();

  dataCheck(false, totalPoints, totalFeatures, totalEnsembles);

  notifyStatusMessage("Packing Precipitates - Filling Gaps");
  assign_gaps();

  notifyStatusMessage("Packing Precipitates - Cleaning Up Volume");
  cleanup_features();

  notifyStatusMessage("Packing Precipitates - Renumbering Features");
  RenumberFeatures::Pointer renumber_features2 = RenumberFeatures::New();
  renumber_features2->setObservers(this->getObservers());
  renumber_features2->setDataContainerArray(getDataContainerArray());
  renumber_features2->execute();
  err = renumber_features2->getErrorCondition();
  if (err < 0)
  {
    setErrorCondition(renumber_features2->getErrorCondition());
    addErrorMessages(renumber_features2->getPipelineMessages());
    return;
  }

  if(m_WriteGoalAttributes == true)
  {
    write_goal_attributes();
  }

  m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->removeAttributeArray(m_EquivalentDiametersArrayName);
  m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->removeAttributeArray(m_Omega3sArrayName);
  m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->removeAttributeArray(m_AxisEulerAnglesArrayName);
  m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->removeAttributeArray(m_AxisLengthsArrayName);
  m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->removeAttributeArray(m_VolumesArrayName);
  m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->removeAttributeArray(m_CentroidsArrayName);
  m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->removeAttributeArray(m_NumCellsArrayName);

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("InsertPrecipitatePhases Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void  InsertPrecipitatePhases::place_precipitates(Int32ArrayType::Pointer featureOwnersPtr)
{
  bool writeErrorFile = true;
  std::ofstream outFile;
  if(m_ErrorOutputFile.isEmpty() == false)
  {
    outFile.open(m_ErrorOutputFile.toLatin1().data(), std::ios_base::binary);
    writeErrorFile = true;
  }

  notifyStatusMessage("Placing Precipitates");
  DREAM3D_RANDOMNG_NEW()

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]), static_cast<DimType>(udims[1]), static_cast<DimType>(udims[2]), };

  sizex = dims[0] * m->getXRes();
  sizey = dims[1] * m->getYRes();
  sizez = dims[2] * m->getZRes();
  totalvol = sizex * sizey * sizez;

  int64_t totalPoints = m->getAttributeMatrix(m_CellAttributeMatrixName)->getNumTuples();
  int64_t currentnumfeatures = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();
  int64_t numensembles = m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getNumTuples();
  if(currentnumfeatures == 0)
  {
    m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->resizeAttributeArrays(1);
    dataCheck(false, totalPoints, 1, numensembles);
    currentnumfeatures = 1;
  }
  firstPrecipitateFeature = currentnumfeatures;
  int phase;
  float precipboundaryfraction = 0.0;
  float random = 0.0f;
  int random2 = 0;
  float xc, yc, zc;
  float oldxc, oldyc, oldzc;
  int randomfeature;
  int acceptedmoves = 0;
  double totalprecipitatefractions = 0.0;

  for (int64_t i = 1; i < numensembles; ++i)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_NumFeatures[i] = 0;
      precipitatephases.push_back(i);
      precipitatephasefractions.push_back(pp->getPhaseFraction());
      totalprecipitatefractions = totalprecipitatefractions + pp->getPhaseFraction();
    }
  }
  for (size_t i = 0; i < precipitatephases.size(); i++)
  {
    precipitatephasefractions[i] = static_cast<float>(precipitatephasefractions[i] / totalprecipitatefractions);
  }

  // initialize the sim and goal size distributions for the precipitate phases
  featuresizedist.resize(precipitatephases.size());
  simfeaturesizedist.resize(precipitatephases.size());
  featuresizediststep.resize(precipitatephases.size());
  for (size_t i = 0; i < precipitatephases.size(); i++)
  {
    phase = precipitatephases[i];
    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
    featuresizedist[i].resize(40);
    simfeaturesizedist[i].resize(40);
    featuresizediststep[i] = static_cast<float>(((2 * pp->getMaxFeatureDiameter()) - (pp->getMinFeatureDiameter() / 2.0)) / featuresizedist[i].size());
    float input = 0;
    float previoustotal = 0;
    VectorOfFloatArray GSdist = pp->getFeatureSizeDistribution();
    float avg = GSdist[0]->GetValue(0);
    float stdev = GSdist[1]->GetValue(0);
    float denominatorConst = sqrtf(2.0f * stdev * stdev); // Calculate it here rather than calculating the same thing multiple times below
    for (size_t j = 0; j < featuresizedist[i].size(); j++)
    {
      input = (float(j + 1) * featuresizediststep[i]) + (pp->getMinFeatureDiameter() / 2.0f);
      float logInput = logf(input);
      if(logInput <= avg)
      {
        featuresizedist[i][j] = 0.5f - 0.5f * (DREAM3DMath::erf((avg - logInput) / denominatorConst)) - previoustotal;
      }
      if(logInput > avg)
      {
        featuresizedist[i][j] = 0.5f + 0.5f * (DREAM3DMath::erf((logInput - avg) / denominatorConst)) - previoustotal;
      }
      previoustotal = previoustotal + featuresizedist[i][j];
    }
  }

  Precip precip;
  QVector<float> curphasevol;
  curphasevol.resize(precipitatephases.size());
  float change = 0.0f;
  float factor = 1.0;
  size_t iter = 0;
  for (size_t j = 0; j < precipitatephases.size(); ++j)
  {
    curphasevol[j] = 0;
    float curphasetotalvol = static_cast<float>(totalvol * totalprecipitatefractions * precipitatephasefractions[j]);
    while (curphasevol[j] < (factor * curphasetotalvol))
    {
      iter++;
      Seed++;
      phase = precipitatephases[j];
      generate_precipitate(phase, static_cast<int>(Seed), &precip, m_ShapeTypes[phase], m_OrthoOps);
      currentsizedisterror = check_sizedisterror(&precip);
      change = (currentsizedisterror) - (oldsizedisterror);
      if(change > 0 || currentsizedisterror > (1.0 - (float(iter) * 0.001)) || curphasevol[j] < (0.75 * factor * curphasetotalvol))
      {
        QString ss = QObject::tr("Packing Precipitates - Generating Feature #%1").arg(currentnumfeatures);
        notifyStatusMessage(ss);

        m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->resizeAttributeArrays(currentnumfeatures + 1);
        dataCheck(false, totalPoints, currentnumfeatures + 1, numensembles);
        m_Active[currentnumfeatures] = true;
        transfer_attributes(currentnumfeatures, &precip);
        oldsizedisterror = currentsizedisterror;
        curphasevol[j] = curphasevol[j] + m_Volumes[currentnumfeatures];
        //FIXME: Initialize the Feature with some sort of default data
        iter = 0;
        currentnumfeatures++;
      }
    }
  }

  // initialize the sim and goal clustering distribution for the primary phases
  clusteringdist.resize(precipitatephases.size());
  simclusteringdist.resize(precipitatephases.size());
  clusteringdiststep.resize(precipitatephases.size());
  for (size_t i = 0; i < precipitatephases.size(); i++)
  {
    phase = precipitatephases[i];
    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
    clusteringdist[i].resize(pp->getBinNumbers()->GetSize());
//    int binNumbers = pp->getBinNumbers();
    simclusteringdist[i].resize(pp->getBinNumbers()->GetSize());
    VectorOfFloatArray Neighdist = pp->getFeatureSize_Clustering();
    float normalizer = 0;
    for (size_t j = 0; j < clusteringdist[i].size(); j++)
    {
      clusteringdist[i][j].resize(40);
      simclusteringdist[i][j].resize(40);
      float input = 0;
      float previoustotal = 0;
      float avg = Neighdist[0]->GetValue(j);
      float stdev = Neighdist[1]->GetValue(j);
      clusteringdiststep[i] = 2;
      float denominatorConst = sqrtf(2.0f * stdev * stdev); // Calculate it here rather than calculating the same thing multiple times below
      for (size_t k = 0; k < clusteringdist[i][j].size(); k++)
      {
        input = (float(k + 1) * clusteringdiststep[i]);
        float logInput = logf(input);
        if(logInput <= avg)
        {
          clusteringdist[i][j][k] = 0.5f - 0.5f * (DREAM3DMath::erf((avg - logInput) / denominatorConst)) - previoustotal;
        }
        if(logInput > avg)
        {
          clusteringdist[i][j][k] = 0.5f + 0.5f * (DREAM3DMath::erf((logInput - avg) / denominatorConst)) - previoustotal;
        }
        previoustotal = previoustotal + clusteringdist[i][j][k];
      }
      normalizer = normalizer + previoustotal;
    }
    for (size_t j = 0; j < clusteringdist[i].size(); j++)
    {
      for (size_t k = 0; k < clusteringdist[i][j].size(); k++)
      {
        clusteringdist[i][j][k] = clusteringdist[i][j][k] / normalizer;
        simclusteringdist[i][j][k] = 0;
      }
    }
  }

  //  for each feature : select centroid, determine voxels in feature, monitor filling error and decide of the 10 placements which
  // is the most beneficial, then the feature is added and its clustering are determined

  size_t numfeatures = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();

  columnlist.resize(numfeatures);
  rowlist.resize(numfeatures);
  planelist.resize(numfeatures);
  packqualities.resize(numfeatures);
  fillingerror = 1;
  for (size_t i = firstPrecipitateFeature; i < numfeatures; i++)
  {

    QString ss = QObject::tr("Packing Precipitates - Placing Precipitate #%1").arg(i);
    notifyStatusMessage(ss);

    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[m_FeaturePhases[i]].get());
    precipboundaryfraction = pp->getPrecipBoundaryFraction();
    random = static_cast<float>(rg.genrand_res53());
    if(random <= precipboundaryfraction)
    {
      random2 = int(rg.genrand_res53() * double(totalPoints - 1));
      while (m_SurfaceVoxels[random2] == 0 || m_FeatureIds[random2] >= firstPrecipitateFeature)
      {
        random2++;
        if(random2 >= totalPoints) { random2 = static_cast<int>(random2 - totalPoints); }
      }
    }
    else if(random > precipboundaryfraction)
    {
      random2 = static_cast<int>(rg.genrand_res53() * (totalPoints - 1));
      while (m_SurfaceVoxels[random2] != 0 || m_FeatureIds[random2] >= firstPrecipitateFeature)
      {
        random2++;
        if(random2 >= totalPoints) { random2 = static_cast<int>(random2 - totalPoints); }
      }
    }
    xc = find_xcoord(random2);
    yc = find_ycoord(random2);
    zc = find_zcoord(random2);
    m_Centroids[3 * i] = xc;
    m_Centroids[3 * i + 1] = yc;
    m_Centroids[3 * i + 2] = zc;
    insert_precipitate(i);
//    fillingerror = check_fillingerror(i, -1000, featureOwnersPtr);
//    for (int iter_fill = 0; iter_fill < 10; iter_fill++)
//    {
//      random = static_cast<float>(rg.genrand_res53());
//      if(random <= precipboundaryfraction)
//      {
//        random2 = int(rg.genrand_res53() * double(totalPoints - 1));
//        while (m_SurfaceVoxels[random2] == 0 || m_FeatureIds[random2] >= firstPrecipitateFeature)
//        {
//          random2++;
//          if(random2 >= totalPoints) { random2 = static_cast<int>(random2 - totalPoints); }
//        }
//      }
//      else if(random > precipboundaryfraction)
//      {
//        random2 = static_cast<int>(rg.genrand_res53() * (totalPoints - 1));
//        while (m_SurfaceVoxels[random2] != 0 || m_FeatureIds[random2] >= firstPrecipitateFeature)
//        {
//          random2++;
//          if(random2 >= totalPoints) { random2 = static_cast<int>(random2 - totalPoints); }
//        }
//      }
//      xc = find_xcoord(random2);
//      yc = find_ycoord(random2);
//      zc = find_zcoord(random2);
//      oldxc = m_Centroids[3 * i];
//      oldyc = m_Centroids[3 * i + 1];
//      oldzc = m_Centroids[3 * i + 2];
//      oldfillingerror = fillingerror;
//      fillingerror = check_fillingerror(-1000, i, featureOwnersPtr);
//      move_precipitate(i, xc, yc, zc);
//      fillingerror = check_fillingerror(i, -1000, featureOwnersPtr);
//      if(fillingerror > oldfillingerror)
//      {
//        fillingerror = check_fillingerror(-1000, i, featureOwnersPtr);
//        move_precipitate(i, oldxc, oldyc, oldzc);
//        fillingerror = check_fillingerror(i, -1000, featureOwnersPtr);
//      }
//    }
  }
  for (size_t i = firstPrecipitateFeature; i < numfeatures; i++)
  {
    oldclusteringerror = check_clusteringerror(i, -1000);
    for (size_t k = 0; k < clusteringdist[0][0].size(); k++)
    {
      outFile << clusteringdist[0][0][k] << " " << simclusteringdist[0][0][k] << "\n";
    }
    outFile << "\n\n";
  }

  notifyStatusMessage("Packing Features - Initial Feature Placement Complete");


  // begin swaping/moving/adding/removing features to try to improve packing
  int totalAdjustments = static_cast<int>(100 * ((numfeatures - firstPrecipitateFeature) - 1));
  for (int iteration = 0; iteration < totalAdjustments; ++iteration)
  {

    QString ss;
    ss = QObject::tr("Packing Features - Swapping/Moving/Adding/Removing Features Iteration %1/%2").arg(iteration).arg(totalAdjustments);
    if(iteration % 100 == 0) { notifyStatusMessage(ss); }

    //    change1 = 0;
    //    change2 = 0;
    int option = iteration % 2;

    if(writeErrorFile == true && iteration % 25 == 0)
    {
      outFile << iteration << " " << oldclusteringerror << " " << acceptedmoves
              << "\n";

      for (size_t k = 0; k < clusteringdist[0][0].size(); k++)
      {
          outFile << clusteringdist[0][0][k] << " " << simclusteringdist[0][0][k] << "\n";
      }
    }

    // JUMP - this option moves one feature to a random spot in the volume
    if(option == 0)
    {
      randomfeature = firstPrecipitateFeature + int(rg.genrand_res53() * (numfeatures - firstPrecipitateFeature));
      if(randomfeature < firstPrecipitateFeature) { randomfeature = firstPrecipitateFeature; }
      if(randomfeature >= static_cast<int>(numfeatures))
      {
        randomfeature = static_cast<int>(numfeatures) - 1;
      }
      Seed++;

      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[m_FeaturePhases[randomfeature]].get());
      if (NULL == pp)
      {
        continue;
      }
      precipboundaryfraction = pp->getPrecipBoundaryFraction();
      random = static_cast<float>(rg.genrand_res53());
      if(random <= precipboundaryfraction)
      {
        random2 = int(rg.genrand_res53() * double(totalPoints - 1));
        while (m_SurfaceVoxels[random2] == 0 || m_FeatureIds[random2] >= firstPrecipitateFeature)
        {
          random2++;
          if(random2 >= totalPoints) { random2 = static_cast<int>(random2 - totalPoints); }
        }
      }
      else if(random > precipboundaryfraction)
      {
        random2 = static_cast<int>(rg.genrand_res53() * (totalPoints - 1));
        while (m_SurfaceVoxels[random2] != 0 || m_FeatureIds[random2] >= firstPrecipitateFeature)
        {
          random2++;
          if(random2 >= totalPoints) { random2 = static_cast<int>(random2 - totalPoints); }
        }
      }
      xc = find_xcoord(random2);
      yc = find_ycoord(random2);
      zc = find_zcoord(random2);
      oldxc = m_Centroids[3 * randomfeature];
      oldyc = m_Centroids[3 * randomfeature + 1];
      oldzc = m_Centroids[3 * randomfeature + 2];
//      oldfillingerror = fillingerror;
//      fillingerror = check_fillingerror(-1000, static_cast<int>(randomfeature), featureOwnersPtr);
//      move_precipitate(randomfeature, xc, yc, zc);
//      fillingerror = check_fillingerror(static_cast<int>(randomfeature), -1000, featureOwnersPtr);
      currentclusteringerror = check_clusteringerror(-1000, randomfeature);
      move_precipitate(randomfeature, xc, yc, zc);
      currentclusteringerror = check_clusteringerror(randomfeature, -1000);
      if(currentclusteringerror >= oldclusteringerror)
      {
        oldclusteringerror = currentclusteringerror;
        acceptedmoves++;
      }
      else if(currentclusteringerror < oldclusteringerror)
      {
//      fillingerror = check_fillingerror(-1000, static_cast<int>(randomfeature), featureOwnersPtr);
//      move_precipitate(randomfeature, oldxc, oldyc, oldzc);
//      fillingerror = check_fillingerror(static_cast<int>(randomfeature), -1000, featureOwnersPtr);
        currentclusteringerror = check_clusteringerror(-1000, randomfeature);
        move_precipitate(randomfeature, oldxc, oldyc, oldzc);
        currentclusteringerror = check_clusteringerror(randomfeature, -1000);
        oldclusteringerror = currentclusteringerror;
      }
    }
    // NUDGE - this option moves one feature to a spot close to its current centroid
    if(option == 1)
    {
      randomfeature = firstPrecipitateFeature + int(rg.genrand_res53() * (numfeatures - firstPrecipitateFeature));
      if(randomfeature < firstPrecipitateFeature) { randomfeature = firstPrecipitateFeature; }
      if(randomfeature >= static_cast<int>(numfeatures))
      {
        randomfeature = static_cast<int>(numfeatures) - 1;
      }
      Seed++;
      oldxc = m_Centroids[3 * randomfeature];
      oldyc = m_Centroids[3 * randomfeature + 1];
      oldzc = m_Centroids[3 * randomfeature + 2];
      xc = static_cast<float>(oldxc + ((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * m_PackingRes[0])));
      yc = static_cast<float>(oldyc + ((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * m_PackingRes[1])));
      zc = static_cast<float>(oldzc + ((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * m_PackingRes[2])));
//      oldfillingerror = fillingerror;
//      fillingerror = check_fillingerror(-1000, static_cast<int>(randomfeature), featureOwnersPtr);
//      move_precipitate(randomfeature, xc, yc, zc);
//      fillingerror = check_fillingerror(static_cast<int>(randomfeature), -1000, featureOwnersPtr);
      currentclusteringerror = check_clusteringerror(-1000, randomfeature);
      move_precipitate(randomfeature, xc, yc, zc);
      currentclusteringerror = check_clusteringerror(randomfeature, -1000);
      if(currentclusteringerror >= oldclusteringerror)
      {
        oldclusteringerror = currentclusteringerror;
        acceptedmoves++;
      }
      else if(currentclusteringerror < oldclusteringerror)
      {
//      fillingerror = check_fillingerror(-1000, static_cast<int>(randomfeature), featureOwnersPtr);
//      move_precipitate(randomfeature, oldxc, oldyc, oldzc);
//      fillingerror = check_fillingerror(static_cast<int>(randomfeature), -1000, featureOwnersPtr);
        currentclusteringerror = check_clusteringerror(-1000, randomfeature);
        move_precipitate(randomfeature, oldxc, oldyc, oldzc);
        currentclusteringerror = check_clusteringerror(randomfeature, -1000);
        oldclusteringerror = currentclusteringerror;
      }
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::generate_precipitate(int phase, int seed, Precip* precip, unsigned int shapeclass, OrientationOps::Pointer OrthoOps)
{
  DREAM3D_RANDOMNG_NEW_SEEDED(seed)

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float r1 = 1;
  float a2 = 0, a3 = 0;
  float b2 = 0, b3 = 0;
  float diam = 0;
  float vol = 0;
  int volgood = 0;
  float phi1, PHI, phi2;
  float fourThirdsPi =  static_cast<float>((4.0f / 3.0f) * (DREAM3D::Constants::k_Pi));
  PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
  VectorOfFloatArray GSdist = pp->getFeatureSizeDistribution();
  float avg = GSdist[0]->GetValue(0);
  float stdev = GSdist[1]->GetValue(0);
  while (volgood == 0)
  {
    volgood = 1;
    diam = static_cast<float>(rg.genrand_norm(avg, stdev));
    diam = exp(diam);
    if(diam >= pp->getMaxFeatureDiameter()) { volgood = 0; }
    if(diam < pp->getMinFeatureDiameter()) { volgood = 0; }
    vol = fourThirdsPi * ((diam / 2.0f) * (diam / 2.0f) * (diam / 2.0f));
  }
  int diameter = int((diam - pp->getMinFeatureDiameter()) / pp->getBinStepSize());
  float r2 = 0.0f, r3 = 1.0f;
  VectorOfFloatArray bovera = pp->getFeatureSize_BOverA();
  VectorOfFloatArray covera = pp->getFeatureSize_COverA();
  while (r2 < r3)
  {
    r2 = 0.0f, r3 = 0.0f;
    a2 = bovera[0]->GetValue(diameter);
    b2 = bovera[1]->GetValue(diameter);
    if(a2 == 0)
    {
      a2 = bovera[0]->GetValue(diameter - 1);
      b2 = bovera[1]->GetValue(diameter - 1);
    }
    r2 = static_cast<float>(rg.genrand_beta(a2, b2));
    a3 = covera[0]->GetValue(diameter);
    b3 = covera[1]->GetValue(diameter);
    if(a3 == 0)
    {
      a3 = covera[0]->GetValue(diameter - 1);
      b3 = covera[1]->GetValue(diameter - 1);
    }
    r3 = static_cast<float>( rg.genrand_beta(a3, b3) );
  }
  float random = static_cast<float>( rg.genrand_res53() );
  float totaldensity = 0;
  int bin = 0;
  FloatArrayType::Pointer axisodf = pp->getAxisOrientation();
  while (random > totaldensity && bin < static_cast<int>(axisodf->GetSize()))
  {
    totaldensity = totaldensity + axisodf->GetValue(bin);
    bin++;
  }
  OrthoOps->determineEulerAngles(bin, phi1, PHI, phi2);
  VectorOfFloatArray omega3 = pp->getFeatureSize_Omegas();
  float mf = omega3[0]->GetValue(diameter);
  float s = omega3[1]->GetValue(diameter);
  float omega3f = static_cast<float>(rg.genrand_beta(mf, s));
  if(shapeclass == DREAM3D::ShapeType::EllipsoidShape) { omega3f = 1; }

  precip->m_Volumes = vol;
  precip->m_EquivalentDiameters = diam;
  precip->m_AxisLengths[0] = r1;
  precip->m_AxisLengths[1] = r2;
  precip->m_AxisLengths[2] = r3;
  precip->m_AxisEulerAngles[0] = phi1;
  precip->m_AxisEulerAngles[1] = PHI;
  precip->m_AxisEulerAngles[2] = phi2;
  precip->m_Omega3s = omega3f;
  precip->m_FeaturePhases = phase;
}

void InsertPrecipitatePhases::transfer_attributes(int gnum, Precip* precip)
{
  m_Volumes[gnum] = precip->m_Volumes;
  m_EquivalentDiameters[gnum] = precip->m_EquivalentDiameters;
  m_AxisLengths[3 * gnum + 0] = precip->m_AxisLengths[0];
  m_AxisLengths[3 * gnum + 1] = precip->m_AxisLengths[1];
  m_AxisLengths[3 * gnum + 2] = precip->m_AxisLengths[2];
  m_AxisEulerAngles[3 * gnum + 0] = precip->m_AxisEulerAngles[0];
  m_AxisEulerAngles[3 * gnum + 1] = precip->m_AxisEulerAngles[1];
  m_AxisEulerAngles[3 * gnum + 2] = precip->m_AxisEulerAngles[2];
  m_Omega3s[gnum] = precip->m_Omega3s;
  m_FeaturePhases[gnum] = precip->m_FeaturePhases;
}

void InsertPrecipitatePhases::move_precipitate(size_t gnum, float xc, float yc, float zc)
{
  int occolumn, ocrow, ocplane;
  int nccolumn, ncrow, ncplane;
  int shiftcolumn, shiftrow, shiftplane;
  float oxc = m_Centroids[3 * gnum];
  float oyc = m_Centroids[3 * gnum + 1];
  float ozc = m_Centroids[3 * gnum + 2];
  occolumn = static_cast<int>( (oxc - (m_PackingRes[0] / 2.0f)) / m_PackingRes[0] );
  ocrow = static_cast<int>( (oyc - (m_PackingRes[1] / 2.0f)) / m_PackingRes[1] );
  ocplane = static_cast<int>( (ozc - (m_PackingRes[2] / 2.0f)) / m_PackingRes[2] );
  nccolumn = static_cast<int>( (xc - (m_PackingRes[0] / 2.0f)) / m_PackingRes[0] );
  ncrow = static_cast<int>( (yc - (m_PackingRes[1] / 2.0f)) / m_PackingRes[1] );
  ncplane = static_cast<int>( (zc - (m_PackingRes[2] / 2.0f)) / m_PackingRes[2] );
  shiftcolumn = nccolumn - occolumn;
  shiftrow = ncrow - ocrow;
  shiftplane = ncplane - ocplane;
  m_Centroids[3 * gnum] = xc;
  m_Centroids[3 * gnum + 1] = yc;
  m_Centroids[3 * gnum + 2] = zc;
  size_t size = columnlist[gnum].size();

  for (size_t i = 0; i < size; i++)
  {
    int& cl = columnlist[gnum][i];
    cl += shiftcolumn;
    int& rl = rowlist[gnum][i];
    rl += shiftrow;
    int& pl = planelist[gnum][i];
    pl += shiftplane;
  }
}


void InsertPrecipitatePhases::determine_clustering(size_t gnum, int add)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  float x, y, z;
  float xn, yn, zn;
  float r;
  float dia, dia2;
  int iter = 0;
  int diabin, dia2bin, clusterbin;

  int phase = m_FeaturePhases[gnum];
  while (phase != precipitatephases[iter]) iter++;

  StatsDataArray& statsDataArray = *m_StatsDataArray;
  typedef QVector<QVector<float> > VectOfVectFloat_t;

  PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
//  VectOfVectFloat_t& curSimClusteringDist = simclusteringdist[iter];
//  size_t curSImClusteringDist_Size = curSimClusteringDist.size();
  float oneOverClusteringDistStep = 1.0f / clusteringdiststep[iter];

  float maxFeatureDia = pp->getMaxFeatureDiameter();
  float minFeatureDia = pp->getMinFeatureDiameter();
  float oneOverBinStepSize = 1.0f / pp->getBinStepSize();

  x = m_Centroids[3 * gnum];
  y = m_Centroids[3 * gnum + 1];
  z = m_Centroids[3 * gnum + 2];
  size_t numFeatures = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();
  for (size_t n = firstPrecipitateFeature; n < numFeatures; n++)
  {
    if (m_FeaturePhases[n] == phase && n != gnum)
    {
      xn = m_Centroids[3 * n];
      yn = m_Centroids[3 * n + 1];
      zn = m_Centroids[3 * n + 2];
      r = sqrtf((x - xn) * (x - xn) + (y - yn) * (y - yn) + (z - zn) * (z - zn));

      dia = m_EquivalentDiameters[gnum];
//      dia2 = m_EquivalentDiameters[n];
      if(dia > maxFeatureDia) { dia = maxFeatureDia; }
      if(dia < minFeatureDia) { dia = minFeatureDia; }
//      if(dia2 > maxFeatureDia) { dia2 = maxFeatureDia; }
//      if(dia2 < minFeatureDia) { dia2 = minFeatureDia; }
      diabin = static_cast<size_t>(((dia - minFeatureDia) * oneOverBinStepSize) );
//      dia2bin = static_cast<size_t>(((dia2 - minFeatureDia) * oneOverBinStepSize) );
      clusterbin = static_cast<size_t>( r * oneOverClusteringDistStep );
      if(clusterbin >= 40) { clusterbin = 39; }
      simclusteringdist[iter][diabin][clusterbin] += add;
//      simclusteringdist[iter][dia2bin][clusterbin] += add;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::check_clusteringerror(int gadd, int gremove)
{
//  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  float clusteringerror;
  float bhattdist;

  for (size_t iter = 0; iter < simclusteringdist.size(); ++iter)
  {
    if(gadd > 0)
    {
      determine_clustering(gadd, 1);
    }
    if(gremove > 0)
    {
      determine_clustering(gremove, -1);
    }
  }
  compare_3Ddistributions(simclusteringdist, clusteringdist, bhattdist);
  clusteringerror = bhattdist;
  return clusteringerror;
}
void InsertPrecipitatePhases::compare_1Ddistributions(QVector<float> array1, QVector<float> array2, float& bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
    bhattdist = bhattdist + sqrt((array1[i] * array2[i]));
  }
}
void InsertPrecipitatePhases::compare_2Ddistributions(QVector<QVector<float> > array1, QVector<QVector<float> > array2, float& bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
    for (size_t j = 0; j < array1[i].size(); j++)
    {
      bhattdist = bhattdist + sqrt((array1[i][j] * array2[i][j]));
    }
  }
}

void InsertPrecipitatePhases::compare_3Ddistributions(QVector<QVector<QVector<float> > > array1, QVector<QVector<QVector<float> > > array2, float& bhattdist)
{
  bhattdist = 0;
  QVector<QVector<float> > counts1(array1.size());
  QVector<QVector<float> > counts2(array2.size());
  for (size_t i = 0; i < array1.size(); i++)
  {
    counts1[i].resize(array1[i].size());
    counts2[i].resize(array2[i].size());
    for (size_t j = 0; j < array1[i].size(); j++)
    {
      for (size_t k = 0; k < array1[i][j].size(); k++)
      {
        counts1[i][j] += array1[i][j][k];
        counts2[i][j] += array2[i][j][k];
      }
    }
  }
  for (size_t i = 0; i < array1.size(); i++)
  {
    for (size_t j = 0; j < array1[i].size(); j++)
    {
      for (size_t k = 0; k < array1[i][j].size(); k++)
      {
        bhattdist = bhattdist + sqrt(((array1[i][j][k] / counts1[i][j]) * (array2[i][j][k] / counts2[i][j])));
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::check_sizedisterror(Precip* precip)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float dia;
  float sizedisterror = 0;
  float bhattdist;
  int index;
  int count = 0;
  int phase;
  size_t featureSizeDist_Size = featuresizedist.size();
  for (size_t iter = 0; iter < featureSizeDist_Size; ++iter)
  {
    phase = precipitatephases[iter];
    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
    count = 0;
    QVector<float>& curFeatureSizeDist = featuresizedist[iter];
    QVector<float>::size_type curFeatureSizeDistSize = curFeatureSizeDist.size();
    QVector<float>& curSimFeatureSizeDist = simfeaturesizedist[iter];
    // Initialize all Values to Zero
    for (size_t i = 0; i < curFeatureSizeDistSize; i++)
    {
      curSimFeatureSizeDist[i] = 0.0f;
    }

    size_t nFeatureTuples = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();
    float oneOverCurFeatureSizeDistStep = 1.0f / featuresizediststep[iter];
    float halfMinFeatureDiameter = pp->getMinFeatureDiameter() * 0.5f;
    for (size_t b = firstPrecipitateFeature; b < nFeatureTuples; b++)
    {
      index = b;
      if(m_FeaturePhases[index] == phase)
      {
        dia = m_EquivalentDiameters[index];
        dia = (dia - halfMinFeatureDiameter) * oneOverCurFeatureSizeDistStep;
        if(dia < 0) { dia = 0; }
        if(dia > curFeatureSizeDistSize - 1.0f) { dia = curFeatureSizeDistSize - 1.0f; }
        curSimFeatureSizeDist[int(dia)]++;
        count++;
      }
    }

    if(precip->m_FeaturePhases == phase)
    {
      dia = precip->m_EquivalentDiameters;
      dia = (dia - halfMinFeatureDiameter) * oneOverCurFeatureSizeDistStep;
      if(dia < 0) { dia = 0; }
      if(dia > curFeatureSizeDistSize - 1.0f) { dia = curFeatureSizeDistSize - 1.0f; }
      curSimFeatureSizeDist[int(dia)]++;
      count++;
    }
    float oneOverCount = 1.0f / count;

    if (count == 0)
    {
      for (size_t i = 0; i < curFeatureSizeDistSize; i++) { curSimFeatureSizeDist[i] = 0.0; }
    }
    else
    {
      for (size_t i = 0; i < curFeatureSizeDistSize; i++)
      {
        curSimFeatureSizeDist[i] = curSimFeatureSizeDist[i] * oneOverCount;
      }
    }
  }
  compare_2Ddistributions(simfeaturesizedist, featuresizedist, bhattdist);
  sizedisterror = bhattdist;
  return sizedisterror;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::check_fillingerror(int gadd, int gremove, Int32ArrayType::Pointer featureOwnersPtr)
{
  size_t featureOwnersIdx = 0;
  int32_t* featureOwners = featureOwnersPtr->getPointer(0);
  fillingerror = fillingerror * float(m_TotalPackingPoints);
  int col, row, plane;
  if(gadd > 0)
  {
    size_t size = columnlist[gadd].size();
    QVector<int>& cl_gadd = columnlist[gadd];
    QVector<int>& rl_gadd = rowlist[gadd];
    QVector<int>& pl_gadd = planelist[gadd];
    float packquality = 0;
    for (size_t i = 0; i < size; i++)
    {
      col = cl_gadd[i];
      row = rl_gadd[i];
      plane = pl_gadd[i];

      if(m_PeriodicBoundaries == true)
      {
        if(col < 0) { col = col + m_PackingPoints[0]; }
        if(col > m_PackingPoints[0] - 1) { col = col - m_PackingPoints[0]; }
        if(row < 0) { row = row + m_PackingPoints[1]; }
        if(row > m_PackingPoints[1] - 1) { row = row - m_PackingPoints[1]; }
        if(plane < 0) { plane = plane + m_PackingPoints[2]; }
        if(plane > m_PackingPoints[2] - 1) { plane = plane - m_PackingPoints[2]; }
        featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + col;
        int currentFeatureOwner = featureOwners[featureOwnersIdx];
        fillingerror = fillingerror + (2 * currentFeatureOwner - 1);
        packquality = packquality + ((currentFeatureOwner) * (currentFeatureOwner));
        ++currentFeatureOwner;
      }
      else
      {
        if(col >= 0 && col <= m_PackingPoints[0] - 1 && row >= 0 && row <= m_PackingPoints[1] - 1 && plane >= 0 && plane <= m_PackingPoints[2] - 1)
        {
          featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + col;
          int currentFeatureOwner = featureOwners[featureOwnersIdx];
          fillingerror = fillingerror + (2 * currentFeatureOwner - 1);
          packquality = packquality + ((currentFeatureOwner) * (currentFeatureOwner));
          ++currentFeatureOwner;
        }
      }
    }
    packqualities[gadd] = static_cast<int>( packquality / float(size) );
  }
  if(gremove > 0)
  {
    size_t size = columnlist[gremove].size();
    QVector<int>& cl_gremove = columnlist[gremove];
    QVector<int>& rl_gremove = rowlist[gremove];
    QVector<int>& pl_gremove = planelist[gremove];
    for (size_t i = 0; i < size; i++)
    {
      col = cl_gremove[i];
      row = rl_gremove[i];
      plane = pl_gremove[i];
      if(m_PeriodicBoundaries == true)
      {
        if(col < 0) { col = col + m_PackingPoints[0]; }
        if(col > m_PackingPoints[0] - 1) { col = col - m_PackingPoints[0]; }
        if(row < 0) { row = row + m_PackingPoints[1]; }
        if(row > m_PackingPoints[1] - 1) { row = row - m_PackingPoints[1]; }
        if(plane < 0) { plane = plane + m_PackingPoints[2]; }
        if(plane > m_PackingPoints[2] - 1) { plane = plane - m_PackingPoints[2]; }
        featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + col;
        int currentFeatureOwner = featureOwners[featureOwnersIdx];
        fillingerror = fillingerror + (-2 * currentFeatureOwner + 3);
        currentFeatureOwner = currentFeatureOwner - 1;
      }
      else
      {
        if(col >= 0 && col <= m_PackingPoints[0] - 1 && row >= 0 && row <= m_PackingPoints[1] - 1 && plane >= 0 && plane <= m_PackingPoints[2] - 1)
        {
          featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + col;
          int currentFeatureOwner = featureOwners[featureOwnersIdx];
          fillingerror = fillingerror + (-2 * currentFeatureOwner + 3);
          currentFeatureOwner = currentFeatureOwner - 1;
        }
      }
    }
  }
  fillingerror = fillingerror / float(m_TotalPackingPoints);
  return fillingerror;
}

void InsertPrecipitatePhases::insert_precipitate(size_t gnum)
{
  DREAM3D_RANDOMNG_NEW()
  //   DataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  //  float dist;
  float inside = -1;
  int column, row, plane;
  int centercolumn, centerrow, centerplane;
  int xmin, xmax, ymin, ymax, zmin, zmax;
  float xc, yc, zc;
  float coordsRotated[3];
  float coords[3];
  float volcur = m_Volumes[gnum];
  float bovera = m_AxisLengths[3 * gnum + 1];
  float covera = m_AxisLengths[3 * gnum + 2];
  float omega3 = m_Omega3s[gnum];
  float radcur1 = 1;
  unsigned int shapeclass = m_ShapeTypes[m_FeaturePhases[gnum]];

  // init any values for each of the Shape Ops
  for (QMap<unsigned int, ShapeOps*>::iterator ops = m_ShapeOps.begin(); ops != m_ShapeOps.end(); ++ops)
  {
    ops.value()->init();
  }
  // Create our Argument Map
  QMap<ShapeOps::ArgName, float> shapeArgMap;
  shapeArgMap[ShapeOps::Omega3] = omega3;
  shapeArgMap[ShapeOps::VolCur] = volcur;
  shapeArgMap[ShapeOps::B_OverA] = bovera;
  shapeArgMap[ShapeOps::C_OverA] = covera;

  radcur1 = m_ShapeOps[shapeclass]->radcur1(shapeArgMap);

  float radcur2 = (radcur1 * bovera);
  float radcur3 = (radcur1 * covera);
  float phi1 = m_AxisEulerAngles[3 * gnum];
  float PHI = m_AxisEulerAngles[3 * gnum + 1];
  float phi2 = m_AxisEulerAngles[3 * gnum + 2];
  float ga[3][3];
  OrientationMath::EulertoMat(phi1, PHI, phi2, ga);
  xc = m_Centroids[3 * gnum];
  yc = m_Centroids[3 * gnum + 1];
  zc = m_Centroids[3 * gnum + 2];
  centercolumn = static_cast<int>( (xc - (m_PackingRes[0] / 2)) / m_PackingRes[0] );
  centerrow = static_cast<int>( (yc - (m_PackingRes[1] / 2)) / m_PackingRes[1] );
  centerplane = static_cast<int>( (zc - (m_PackingRes[2] / 2)) / m_PackingRes[2] );
  xmin = int(centercolumn - ((radcur1 / m_PackingRes[0]) + 1));
  xmax = int(centercolumn + ((radcur1 / m_PackingRes[0]) + 1));
  ymin = int(centerrow - ((radcur1 / m_PackingRes[1]) + 1));
  ymax = int(centerrow + ((radcur1 / m_PackingRes[1]) + 1));
  zmin = int(centerplane - ((radcur1 / m_PackingRes[2]) + 1));
  zmax = int(centerplane + ((radcur1 / m_PackingRes[2]) + 1));
  if(xmin < -m_PackingPoints[0]) { xmin = -m_PackingPoints[0]; }
  if(xmax > 2 * m_PackingPoints[0] - 1) { xmax = (2 * m_PackingPoints[0] - 1); }
  if(ymin < -m_PackingPoints[1]) { ymin = -m_PackingPoints[1]; }
  if(ymax > 2 * m_PackingPoints[1] - 1) { ymax = (2 * m_PackingPoints[1] - 1); }
  if(zmin < -m_PackingPoints[2]) { zmin = -m_PackingPoints[2]; }
  if(zmax > 2 * m_PackingPoints[2] - 1) { zmax = (2 * m_PackingPoints[2] - 1); }
  for (int iter1 = xmin; iter1 < xmax + 1; iter1++)
  {
    for (int iter2 = ymin; iter2 < ymax + 1; iter2++)
    {
      for (int iter3 = zmin; iter3 < zmax + 1; iter3++)
      {
        column = iter1;
        row = iter2;
        plane = iter3;
        coords[0] = float(column) * m_PackingRes[0];
        coords[1] = float(row) * m_PackingRes[1];
        coords[2] = float(plane) * m_PackingRes[2];
        inside = -1;
        coords[0] = coords[0] - xc;
        coords[1] = coords[1] - yc;
        coords[2] = coords[2] - zc;
        MatrixMath::Multiply3x3with3x1(ga, coords, coordsRotated);
        float axis1comp = coordsRotated[0] / radcur1;
        float axis2comp = coordsRotated[1] / radcur2;
        float axis3comp = coordsRotated[2] / radcur3;
        inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
        if(inside >= 0)
        {
          columnlist[gnum].push_back(column);
          rowlist[gnum].push_back(row);
          planelist[gnum].push_back(plane);
        }
      }
    }
  }
}

void InsertPrecipitatePhases::assign_voxels()
{
  notifyStatusMessage("Assigning Voxels");

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int index;
  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  DimType neighpoints[6];
  neighpoints[0] = -dims[0] * dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0] * dims[1];

  float totalPoints = dims[0] * dims[1] * dims[2];
  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

  int oldname;
  size_t column, row, plane;
  float inside;
  float xc, yc, zc;
  float coordsRotated[3];
  //float dist;
  float coords[3];
  DimType xmin, xmax, ymin, ymax, zmin, zmax;
  int64_t numFeatures = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();
  gsizes.resize(numFeatures);

  for (int64_t i = firstPrecipitateFeature; i < numFeatures; i++)
  {
    gsizes[i] = 0;
  }
  for (int64_t i = firstPrecipitateFeature; i < numFeatures; i++)
  {
    float volcur = m_Volumes[i];
    float bovera = m_AxisLengths[3 * i + 1];
    float covera = m_AxisLengths[3 * i + 2];
    float omega3 = m_Omega3s[i];
    xc = m_Centroids[3 * i];
    yc = m_Centroids[3 * i + 1];
    zc = m_Centroids[3 * i + 2];
    float radcur1 = 0.0f;
    //Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
    unsigned int shapeclass = m_ShapeTypes[m_FeaturePhases[i]];

    // init any values for each of the Shape Ops
    for (QMap<unsigned int, ShapeOps*>::iterator ops = m_ShapeOps.begin(); ops != m_ShapeOps.end(); ++ops )
    {
      ops.value()->init();
    }
    // Create our Argument Map
    QMap<ShapeOps::ArgName, float> shapeArgMap;
    shapeArgMap[ShapeOps::Omega3] = omega3;
    shapeArgMap[ShapeOps::VolCur] = volcur;
    shapeArgMap[ShapeOps::B_OverA] = bovera;
    shapeArgMap[ShapeOps::C_OverA] = covera;

    radcur1 = m_ShapeOps[shapeclass]->radcur1(shapeArgMap);

    float radcur2 = (radcur1 * bovera);
    float radcur3 = (radcur1 * covera);
    float phi1 = m_AxisEulerAngles[3 * i];
    float PHI = m_AxisEulerAngles[3 * i + 1];
    float phi2 = m_AxisEulerAngles[3 * i + 2];
    float ga[3][3];
    OrientationMath::EulertoMat(phi1, PHI, phi2, ga);
    column = static_cast<size_t>( (xc - (xRes / 2.0f)) / xRes );
    row = static_cast<size_t>( (yc - (yRes / 2.0f)) / yRes );
    plane = static_cast<size_t>( (zc - (zRes / 2.0f)) / zRes );
    xmin = int(column - ((radcur1 / xRes) + 1));
    xmax = int(column + ((radcur1 / xRes) + 1));
    ymin = int(row - ((radcur1 / yRes) + 1));
    ymax = int(row + ((radcur1 / yRes) + 1));
    zmin = int(plane - ((radcur1 / zRes) + 1));
    zmax = int(plane + ((radcur1 / zRes) + 1));
    if (m_PeriodicBoundaries == true)
    {
      if (xmin < -dims[0]) { xmin = -dims[0]; }
      if (xmax > 2 * dims[0] - 1) { xmax = (2 * dims[0] - 1); }
      if (ymin < -dims[1]) { ymin = -dims[1]; }
      if (ymax > 2 * dims[1] - 1) { ymax = (2 * dims[1] - 1); }
      if (zmin < -dims[2]) { zmin = -dims[2]; }
      if (zmax > 2 * dims[2] - 1) { zmax = (2 * dims[2] - 1); }
    }
    if (m_PeriodicBoundaries == false)
    {
      if (xmin < 0) { xmin = 0; }
      if (xmax > dims[0] - 1) { xmax = dims[0] - 1; }
      if (ymin < 0) { ymin = 0; }
      if (ymax > dims[1] - 1) { ymax = dims[1] - 1; }
      if (zmin < 0) { zmin = 0; }
      if (zmax > dims[2] - 1) { zmax = dims[2] - 1; }
    }
    for (DimType iter1 = xmin; iter1 < xmax + 1; iter1++)
    {
      for (DimType iter2 = ymin; iter2 < ymax + 1; iter2++)
      {
        for (DimType iter3 = zmin; iter3 < zmax + 1; iter3++)
        {
          column = iter1;
          row = iter2;
          plane = iter3;
          if (iter1 < 0) { column = iter1 + dims[0]; }
          if (iter1 > dims[0] - 1) { column = iter1 - dims[0]; }
          if (iter2 < 0) { row = iter2 + dims[1]; }
          if (iter2 > dims[1] - 1) { row = iter2 - dims[1]; }
          if (iter3 < 0) { plane = iter3 + dims[2]; }
          if (iter3 > dims[2] - 1) { plane = iter3 - dims[2]; }
          index = (plane * dims[0] * dims[1]) + (row * dims[0]) + column;
          inside = -1;
          coords[0] = float(column) * xRes;
          coords[1] = float(row) * yRes;
          coords[2] = float(plane) * zRes;
          if (iter1 < 0) { coords[0] = coords[0] - sizex; }
          if (iter1 > dims[0] - 1) { coords[0] = coords[0] + sizex; }
          if (iter2 < 0) { coords[1] = coords[1] - sizey; }
          if (iter2 > dims[1] - 1) { coords[1] = coords[1] + sizey; }
          if (iter3 < 0) { coords[2] = coords[2] - sizez; }
          if (iter3 > dims[2] - 1) { coords[2] = coords[2] + sizez; }
//          dist = ((coords[0] - xc) * (coords[0] - xc)) + ((coords[1] - yc) * (coords[1] - yc)) + ((coords[2] - zc) * (coords[2] - zc));
//          dist = sqrtf(dist);
//          if (dist < radcur1)
//          {
          coords[0] = coords[0] - xc;
          coords[1] = coords[1] - yc;
          coords[2] = coords[2] - zc;
          MatrixMath::Multiply3x3with3x1(ga, coords, coordsRotated);
          float axis1comp = coordsRotated[0] / radcur1;
          float axis2comp = coordsRotated[1] / radcur2;
          float axis3comp = coordsRotated[2] / radcur3;
          inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
          if (inside >= 0)
          {
            int currentpoint = index;
            if (m_FeatureIds[currentpoint] > firstPrecipitateFeature)
            {
              oldname = m_FeatureIds[currentpoint];
              m_FeatureIds[currentpoint] = -2;
            }
            if (m_FeatureIds[currentpoint] < firstPrecipitateFeature && m_FeatureIds[currentpoint] != -2)
            {
              m_FeatureIds[currentpoint] = static_cast<int32_t>(i);
            }
          }
//          }
        }
      }
    }
  }
  for (int64_t i = firstPrecipitateFeature; i < numFeatures; i++)
  {
    m_Active[i] = false;
  }
  int gnum;
  for(size_t i = 0; i < totalPoints; i++)
  {
    gnum = m_FeatureIds[i];
    if(gnum >= 0) { m_Active[gnum] = true; }
  }
}

void InsertPrecipitatePhases::assign_gaps()
{
  notifyStatusMessage("Assigning Gaps");

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totpoints = m->getTotalPoints();

  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };


  int index;
  int timestep = 100;
  int unassignedcount = 1;
  DimType column, row, plane;
  float inside;
  float xc, yc, zc;
  float coordsRotated[3];
  float dist;
  float coords[3];

  DimType xmin, xmax, ymin, ymax, zmin, zmax;

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

  Int32ArrayType::Pointer newownersPtr = Int32ArrayType::CreateArray(totpoints, "newowners");
  int32_t* newowners = newownersPtr->getPointer(0);
  newownersPtr->initializeWithZeros();

  FloatArrayType::Pointer ellipfuncsPtr = FloatArrayType::CreateArray(totpoints, "ellipfuncs");
  float* ellipfuncs = ellipfuncsPtr->getPointer(0);
  ellipfuncsPtr->initializeWithValues(-1);

  int64_t numEnsembles = m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getNumTuples();
  while (unassignedcount != 0)
  {
    unassignedcount = 0;
    timestep = timestep + 50;
    for (int64_t i = firstPrecipitateFeature; i < numEnsembles; i++)
    {
      float volcur = m_Volumes[i];
      float bovera = m_AxisLengths[3 * i + 1];
      float covera = m_AxisLengths[3 * i + 2];
      float omega3 = m_Omega3s[i];
      xc = m_Centroids[3 * i];
      yc = m_Centroids[3 * i + 1];
      zc = m_Centroids[3 * i + 2];
      float radcur1 = 0.0f;
      //Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
      unsigned int shapeclass = m_ShapeTypes[m_FeaturePhases[i]];

      // init any values for each of the Shape Ops
      for (QMap<unsigned int, ShapeOps*>::iterator ops = m_ShapeOps.begin(); ops != m_ShapeOps.end(); ++ops )
      {
        ops.value()->init();
      }
      // Create our Argument Map
      QMap<ShapeOps::ArgName, float> shapeArgMap;
      shapeArgMap[ShapeOps::Omega3] = omega3;
      shapeArgMap[ShapeOps::VolCur] = volcur;
      shapeArgMap[ShapeOps::B_OverA] = bovera;
      shapeArgMap[ShapeOps::C_OverA] = covera;

      radcur1 = m_ShapeOps[shapeclass]->radcur1(shapeArgMap);

      float radcur2 = (radcur1 * bovera);
      float radcur3 = (radcur1 * covera);
      radcur1 = (float(timestep) / 100.0f) * radcur1;
      radcur2 = (float(timestep) / 100.0f) * radcur2;
      radcur3 = (float(timestep) / 100.0f) * radcur3;
      float phi1 = m_AxisEulerAngles[3 * i];
      float PHI = m_AxisEulerAngles[3 * i + 1];
      float phi2 = m_AxisEulerAngles[3 * i + 2];
      float ga[3][3];
      OrientationMath::EulertoMat(phi1, PHI, phi2, ga);
      column = static_cast<DimType>( (xc - (xRes / 2.0f)) / xRes );
      row = static_cast<DimType>( (yc - (yRes / 2.0f)) / yRes );
      plane = static_cast<DimType>( (zc - (zRes / 2.0f)) / zRes );
      xmin = int(column - ((radcur1 / xRes) + 1));
      xmax = int(column + ((radcur1 / xRes) + 1));
      ymin = int(row - ((radcur1 / yRes) + 1));
      ymax = int(row + ((radcur1 / yRes) + 1));
      zmin = int(plane - ((radcur1 / zRes) + 1));
      zmax = int(plane + ((radcur1 / zRes) + 1));
      if (m_PeriodicBoundaries == true)
      {
        if (xmin < -dims[0]) { xmin = -dims[0]; }
        if (xmax > 2 * dims[0] - 1) { xmax = (2 * dims[0] - 1); }
        if (ymin < -dims[1]) { ymin = -dims[1]; }
        if (ymax > 2 * dims[1] - 1) { ymax = (2 * dims[1] - 1); }
        if (zmin < -dims[2]) { zmin = -dims[2]; }
        if (zmax > 2 * dims[2] - 1) { zmax = (2 * dims[2] - 1); }
      }
      if (m_PeriodicBoundaries == false)
      {
        if (xmin < 0) { xmin = 0; }
        if (xmax > dims[0] - 1) { xmax = dims[0] - 1; }
        if (ymin < 0) { ymin = 0; }
        if (ymax > dims[1] - 1) { ymax = dims[1] - 1; }
        if (zmin < 0) { zmin = 0; }
        if (zmax > dims[2] - 1) { zmax = dims[2] - 1; }
      }
      for (DimType iter1 = xmin; iter1 < xmax + 1; iter1++)
      {
        for (DimType iter2 = ymin; iter2 < ymax + 1; iter2++)
        {
          for (DimType iter3 = zmin; iter3 < zmax + 1; iter3++)
          {
            column = iter1;
            row = iter2;
            plane = iter3;
            if (iter1 < 0) { column = iter1 + dims[0]; }
            if (iter1 > dims[0] - 1) { column = iter1 - dims[0]; }
            if (iter2 < 0) { row = iter2 + dims[1]; }
            if (iter2 > dims[1] - 1) { row = iter2 - dims[1]; }
            if (iter3 < 0) { plane = iter3 + dims[2]; }
            if (iter3 > dims[2] - 1) { plane = iter3 - dims[2]; }
            index = static_cast<int>( (plane * dims[0] * dims[1]) + (row * dims[0]) + column );
            if(m_FeatureIds[index] <= 0)
            {
              inside = -1;
              coords[0] = float(column) * xRes;
              coords[1] = float(row) * yRes;
              coords[2] = float(plane) * zRes;
              if (iter1 < 0) { coords[0] = coords[0] - sizex; }
              if (iter1 > dims[0] - 1) { coords[0] = coords[0] + sizex; }
              if (iter2 < 0) { coords[1] = coords[1] - sizey; }
              if (iter2 > dims[1] - 1) { coords[1] = coords[1] + sizey; }
              if (iter3 < 0) { coords[2] = coords[2] - sizez; }
              if (iter3 > dims[2] - 1) { coords[2] = coords[2] + sizez; }
              dist = ((coords[0] - xc) * (coords[0] - xc)) + ((coords[1] - yc) * (coords[1] - yc)) + ((coords[2] - zc) * (coords[2] - zc));
              dist = sqrtf(dist);
              if (dist < radcur1)
              {
                coords[0] = coords[0] - xc;
                coords[1] = coords[1] - yc;
                coords[2] = coords[2] - zc;
                MatrixMath::Multiply3x3with3x1(ga, coords, coordsRotated);
                float axis1comp = coordsRotated[0] / radcur1;
                float axis2comp = coordsRotated[1] / radcur2;
                float axis3comp = coordsRotated[2] / radcur3;
                inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
                if (inside >= 0 && inside > ellipfuncs[index])
                {
                  newowners[index] = i;
                  ellipfuncs[index] = inside;
                }
              }
            }
          }
        }
      }
    }
    for (int i = 0; i < totpoints; i++)
    {
      if (ellipfuncs[i] >= 0) { m_FeatureIds[i] = newowners[i]; }
      if (m_FeatureIds[i] <= 0) { unassignedcount++; }
      newowners[i] = -1;
      ellipfuncs[i] = -1.0;
    }
  }
  for (int i = 0; i < totpoints; i++)
  {
    if(m_FeatureIds[i] > 0) { m_CellPhases[i] = m_FeaturePhases[m_FeatureIds[i]]; }
  }
}
void InsertPrecipitatePhases::cleanup_features()
{
  notifyStatusMessage("Cleaning Up Features");

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  int64_t totpoints = m->getTotalPoints();
  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  DimType neighpoints[6];
  DimType xp = dims[0];
  DimType yp = dims[1];
  DimType zp = dims[2];

  int64_t numFeatures = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();

  neighpoints[0] = -(xp * yp);
  neighpoints[1] = -xp;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xp;
  neighpoints[5] = (xp * yp);
  QVector<QVector<int> > vlists;
  vlists.resize(numFeatures);
  QVector<int> currentvlist;
  QVector<bool> checked(totpoints, false);
  size_t count;
  int touchessurface = 0;
  int good;
  int neighbor;
  DimType column, row, plane;
  int index;
  float minsize = 0;
  gsizes.resize(numFeatures);
  for (int64_t i = 1; i < numFeatures; i++)
  {
    gsizes[i] = 0;
    m_Active[i] = true;
  }

  float resConst = m->getXRes() * m->getYRes() * m->getZRes();
  for (int i = 0; i < totpoints; i++)
  {
    touchessurface = 0;
    if(checked[i] == false && m_FeatureIds[i] > firstPrecipitateFeature)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[m_CellPhases[i]].get());
      minsize = static_cast<float>( pp->getMinFeatureDiameter() * pp->getMinFeatureDiameter() * pp->getMinFeatureDiameter() * M_PI / 6.0f );
      minsize = static_cast<float>( int(minsize / (resConst)) );
      currentvlist.push_back(i);
      count = 0;
      while (count < currentvlist.size())
      {
        index = currentvlist[count];
        column = index % xp;
        row = (index / xp) % yp;
        plane = index / (xp * yp);
        if(column == 0 || column == xp || row == 0 || row == yp || plane == 0 || plane == zp) { touchessurface = 1; }
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighbor = static_cast<int>( index + neighpoints[j] );
          if(m_PeriodicBoundaries == false)
          {
            if(j == 0 && plane == 0) { good = 0; }
            if(j == 5 && plane == (zp - 1)) { good = 0; }
            if(j == 1 && row == 0) { good = 0; }
            if(j == 4 && row == (yp - 1)) { good = 0; }
            if(j == 2 && column == 0) { good = 0; }
            if(j == 3 && column == (xp - 1)) { good = 0; }
            if(good == 1 && m_FeatureIds[neighbor] == m_FeatureIds[index] && checked[neighbor] == false)
            {
              currentvlist.push_back(neighbor);
              checked[neighbor] = true;
            }
          }
          else if(m_PeriodicBoundaries == true)
          {
            if(j == 0 && plane == 0) { neighbor = static_cast<int>( neighbor + (xp * yp * zp) ); }
            if(j == 5 && plane == (zp - 1)) { neighbor = static_cast<int>( neighbor - (xp * yp * zp) ); }
            if(j == 1 && row == 0) { neighbor = static_cast<int>( neighbor + (xp * yp) ); }
            if(j == 4 && row == (yp - 1)) { neighbor = static_cast<int>( neighbor - (xp * yp) ); }
            if(j == 2 && column == 0) { neighbor = static_cast<int>( neighbor + (xp) ); }
            if(j == 3 && column == (xp - 1)) { neighbor = static_cast<int>( neighbor - (xp) ); }
            if(m_FeatureIds[neighbor] == m_FeatureIds[index] && checked[neighbor] == false)
            {
              currentvlist.push_back(neighbor);
              checked[neighbor] = true;
            }
          }
        }
        count++;
      }
      size_t size = vlists[m_FeatureIds[i]].size();
      if(size > 0)
      {
        if(size < currentvlist.size())
        {
          for (size_t k = 0; k < vlists[m_FeatureIds[i]].size(); k++)
          {
            m_FeatureIds[vlists[m_FeatureIds[i]][k]] = -1;
          }
          vlists[m_FeatureIds[i]].resize(currentvlist.size());
          vlists[m_FeatureIds[i]].swap(currentvlist);
        }
        else if(size >= currentvlist.size())
        {
          for (size_t k = 0; k < currentvlist.size(); k++)
          {
            m_FeatureIds[currentvlist[k]] = -1;
          }
        }
      }
      else if(size == 0)
      {
        if(currentvlist.size() >= minsize || touchessurface == 1)
        {
          vlists[m_FeatureIds[i]].resize(currentvlist.size());
          vlists[m_FeatureIds[i]].swap(currentvlist);
        }
        if(currentvlist.size() < minsize && touchessurface == 0)
        {
          for (size_t k = 0; k < currentvlist.size(); k++)
          {
            m_FeatureIds[currentvlist[k]] = -1;
          }
        }
      }
      currentvlist.clear();
    }
  }
  assign_gaps();
  for (int i = 0; i < totpoints; i++)
  {
    if(m_FeatureIds[i] > 0) { gsizes[m_FeatureIds[i]]++; }
  }
  for (int64_t i = firstPrecipitateFeature; i < numFeatures; i++)
  {
    if(gsizes[i] == 0) { m_Active[i] = false; }
  }
  for (int i = 0; i < totpoints; i++)
  {
    if(m_FeatureIds[i] > 0) { m_CellPhases[i] = m_FeaturePhases[m_FeatureIds[i]]; }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Int32ArrayType::Pointer  InsertPrecipitatePhases::initialize_packinggrid()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  m_PackingRes[0] = m->getXRes() * 2.0f;
  m_PackingRes[1] = m->getYRes() * 2.0f;
  m_PackingRes[2] = m->getZRes() * 2.0f;

  m_HalfPackingRes[0] = m_PackingRes[0] * 0.5;
  m_HalfPackingRes[1] = m_PackingRes[1] * 0.5;
  m_HalfPackingRes[2] = m_PackingRes[2] * 0.5;

  m_OneOverHalfPackingRes[0] = 1.0f / m_HalfPackingRes[0];
  m_OneOverHalfPackingRes[1] = 1.0f / m_HalfPackingRes[1];
  m_OneOverHalfPackingRes[2] = 1.0f / m_HalfPackingRes[2];

  m_PackingPoints[0] = m->getXPoints() / 2;
  m_PackingPoints[1] = m->getYPoints() / 2;
  m_PackingPoints[2] = m->getZPoints() / 2;

  m_TotalPackingPoints = m_PackingPoints[0] * m_PackingPoints[1] * m_PackingPoints[2];

  Int32ArrayType::Pointer featureOwnersPtr = Int32ArrayType::CreateArray(m_TotalPackingPoints, "PackPrimaryFeatures::feature_owners");
  featureOwnersPtr->initializeWithZeros();

  return featureOwnersPtr;
}


float InsertPrecipitatePhases::find_xcoord(long long int index)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  float x = m->getXRes() * float(index % m->getXPoints());
  return x;
}
float InsertPrecipitatePhases::find_ycoord(long long int index)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  float y = m->getYRes() * float((index / m->getXPoints()) % m->getYPoints());
  return y;
}
float InsertPrecipitatePhases::find_zcoord(long long int index)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  float z = m->getZRes() * float(index / (m->getXPoints() * m->getYPoints()));
  return z;
}
void InsertPrecipitatePhases::write_goal_attributes()
{
  int err = 0;
  setErrorCondition(err);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path

  QFileInfo fi(m_CsvOutputFile);
  QString parentPath = fi.path();
  QDir dir;
  if(!dir.mkpath(parentPath))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    notifyErrorMessage(ss, -1);
    setErrorCondition(-1);
    return;
  }


  QFile outFile(getCsvOutputFile());
  if (!outFile.open(QIODevice::WriteOnly))
  {
    QString msg = QObject::tr("CSV Output file could not be opened: %1").arg(getCsvOutputFile());
    setErrorCondition(-200);
    notifyErrorMessage(msg, getErrorCondition());
    return;
  }

  int64_t numFeatures = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getNumTuples();

  QTextStream dStream(&outFile);

  char space = ',';
  // Write the total number of features
  dStream << static_cast<qint32>(numFeatures - firstPrecipitateFeature);
  // Get all the names of the arrays from the Data Container
  QList<QString> headers = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getAttributeArrayNameList();

  QVector<IDataArray::Pointer> data;

  //For checking if an array is a neighborlist
  NeighborList<float>::Pointer neighborlistPtr = NeighborList<float>::New();

  // Print the FeatureIds Header before the rest of the headers
  dStream << DREAM3D::FeatureData::FeatureID;
  // Loop throught the list and print the rest of the headers, ignoring those we don't want
  for(QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
  {
    // Only get the array if the name does NOT match those listed
    IDataArray::Pointer p = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName)->getAttributeArray(*iter);
    if(p->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) != 0)
    {
      if (p->GetNumberOfComponents() == 1)
      {
        dStream << space << (*iter);
      }
      else // There are more than a single component so we need to add multiple header values
      {
        for(int k = 0; k < p->GetNumberOfComponents(); ++k)
        {
          dStream << space << (*iter) << "_" << k;
        }
      }
      // Get the IDataArray from the DataContainer
      data.push_back(p);
    }
  }
  dStream << "\n";


  // Get the number of tuples in the arrays
  size_t numTuples = data[0]->getNumberOfTuples();

  float threshold = 0.0f;

  // Skip the first feature
  for(qint32 i = firstPrecipitateFeature; i < numTuples; ++i)
  {
    if (((float)i / numTuples) * 100.0f > threshold)
    {
      QString ss = QObject::tr("Writing Feature Data - %1% Complete").arg(((float)i / numTuples) * 100);
      notifyStatusMessage(ss);
      threshold = threshold + 5.0f;
      if (threshold < ((float)i / numTuples) * 100.0f)
      {
        threshold = ((float)i / numTuples) * 100.0f;
      }
    }

    // Print the feature id
    dStream << i;
    // Print a row of data
    for(qint32 p = 0; p < data.size(); ++p)
    {
      dStream << space;
      data[p]->printTuple(dStream, i, space);
    }
    dStream << "\n";
  }
}
