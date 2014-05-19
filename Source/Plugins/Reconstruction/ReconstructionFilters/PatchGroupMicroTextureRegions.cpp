/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "PatchGroupMicroTextureRegions.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/GeometryMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1




#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PatchGroupMicroTextureRegions::PatchGroupMicroTextureRegions() :
  GroupFeatures(),
  m_NewCellFeatureAttributeMatrixName(DREAM3D::Defaults::NewCellFeatureAttributeMatrixName),
  m_CAxisTolerance(1.0f),
  m_UseRunningAverage(false),
  m_MinMTRSize(1.0f),
  m_PatchVolumeFractionForMTRGrowth(1.0f),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_VolumesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Volumes),
  m_CentroidsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Centroids),
  m_AvgQuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_CellParentIdsArrayName(DREAM3D::CellData::ParentIds),
  m_FeatureParentIdsArrayName(DREAM3D::FeatureData::ParentIds),
  m_NumCellsArrayName(DREAM3D::FeatureData::NumCells),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_NeighborhoodsArrayName(DREAM3D::FeatureData::Neighborhoods),
  m_NeighborhoodListArrayName(DREAM3D::FeatureData::NeighborhoodList),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellParentIds(NULL),
  m_FeatureParentIds(NULL),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_AvgQuats(NULL),
  m_Active(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_VolumesArrayName(DREAM3D::FeatureData::Volumes),
  m_Volumes(NULL),
  m_NumCells(NULL),
  m_CentroidsArrayName(DREAM3D::FeatureData::Centroids),
  m_Centroids(NULL),
  m_Neighborhoods(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PatchGroupMicroTextureRegions::~PatchGroupMicroTextureRegions()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PatchGroupMicroTextureRegions::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("C-Axis Alignment Tolerance", "CAxisTolerance", FilterParameterWidgetType::DoubleWidget, "float", false, "Degrees"));
  parameters.push_back(FilterParameter::New("Group C-Axes With Running Average", "UseRunningAverage", FilterParameterWidgetType::BooleanWidget, "bool", false));
  parameters.push_back(FilterParameter::New("Minimum MicroTextured Region Size (Diameter)", "MinMTRSize", FilterParameterWidgetType::DoubleWidget, "float", false));
  parameters.push_back(FilterParameter::New("Minimum C-Axis Aligned Patch Volume Fraction For MTR Growth", "PatchVolumeFractionForMTRGrowth", FilterParameterWidgetType::DoubleWidget, "float", false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("FeaturePhases", "FeaturePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Volumes", "VolumesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Centroids", "CentroidsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("AvgQuats", "AvgQuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("CrystalStructures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("New Cell Feature Attribute Matrix Name", "NewCellFeatureAttributeMatrixName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  parameters.push_back(FilterParameter::New("CellParentIds", "CellParentIdsArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  parameters.push_back(FilterParameter::New("FeatureParentIds", "FeatureParentIdsArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  parameters.push_back(FilterParameter::New("NumCells", "NumCellsArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  parameters.push_back(FilterParameter::New("Active", "ActiveArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  parameters.push_back(FilterParameter::New("Neighborhoods", "NeighborhoodsArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  parameters.push_back(FilterParameter::New("NeighborhoodLists", "NeighborhoodListArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PatchGroupMicroTextureRegions::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  GroupFeatures::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setNewCellFeatureAttributeMatrixName(reader->readString("NewCellFeatureAttributeMatrixName", getNewCellFeatureAttributeMatrixName() ) );
  setNeighborhoodsArrayName(reader->readString("NeighborhoodsArrayName", getNeighborhoodsArrayName() ) );
  setNeighborhoodListArrayName(reader->readString("NeighborhoodListArrayName", getNeighborhoodListArrayName() ) );
  setActiveArrayName(reader->readString("ActiveArrayName", getActiveArrayName() ) );
  setNumCellsArrayName(reader->readString("NumCellsArrayName", getNumCellsArrayName() ) );
  setFeatureParentIdsArrayName(reader->readString("FeatureParentIdsArrayName", getFeatureParentIdsArrayName() ) );
  setCellParentIdsArrayName(reader->readString("CellParentIdsArrayName", getCellParentIdsArrayName() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath() ) );
  setCentroidsArrayPath(reader->readDataArrayPath("CentroidsArrayPath", getCentroidsArrayPath() ) );
  setVolumesArrayPath(reader->readDataArrayPath("VolumesArrayPath", getVolumesArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setCAxisTolerance( reader->readValue("CAxisTolerance", getCAxisTolerance()) );
  setUseRunningAverage( reader->readValue("UseRunningAverage", getUseRunningAverage()) );
  setMinMTRSize( reader->readValue("MinMTRSize", getMinMTRSize()) );
  setPatchVolumeFractionForMTRGrowth( reader->readValue("PatchVolumeFractionForMTRGrowth", getPatchVolumeFractionForMTRGrowth()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PatchGroupMicroTextureRegions::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  GroupFeatures::writeFilterParameters(writer, index);
  writer->openFilterGroup(this, index);
  writer->writeValue("ActiveArrayName", getActiveArrayName() );
  writer->writeValue("NewCellFeatureAttributeMatrixName", getNewCellFeatureAttributeMatrixName() );
  writer->writeValue("NeighborhoodsArrayName", getNeighborhoodsArrayName() );
  writer->writeValue("NeighborhoodListArrayName", getNeighborhoodListArrayName() );
  writer->writeValue("NumCellsArrayName", getNumCellsArrayName() );
  writer->writeValue("FeatureParentIdsArrayName", getFeatureParentIdsArrayName() );
  writer->writeValue("CellParentIdsArrayName", getCellParentIdsArrayName() );
  writer->writeValue("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() );
  writer->writeValue("AvgQuatsArrayPath", getAvgQuatsArrayPath() );
  writer->writeValue("CentroidsArrayPath", getCentroidsArrayPath() );
  writer->writeValue("VolumesArrayPath", getVolumesArrayPath() );
  writer->writeValue("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() );
  writer->writeValue("FeatureIdsArrayPath", getFeatureIdsArrayPath() );
  writer->writeValue("CAxisTolerance", getCAxisTolerance() );
  writer->writeValue("UseRunningAverage", getUseRunningAverage() );
  writer->writeValue("MinMTRSize", getMinMTRSize() );
  writer->writeValue("PatchVolumeFractionForMTRGrowth", getPatchVolumeFractionForMTRGrowth() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PatchGroupMicroTextureRegions::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PatchGroupMicroTextureRegions::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, m_FeatureIdsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer newCellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getNewCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), m_FeatureIdsArrayPath.getAttributeMatrixName(), getCellParentIdsArrayName() );
  m_CellParentIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, -1, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellParentIds = m_CellParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Data
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(m_FeaturePhasesArrayPath.getDataContainerName(), m_FeaturePhasesArrayPath.getAttributeMatrixName(), getFeatureParentIdsArrayName() );
  m_FeatureParentIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, -1, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureParentIds = m_FeatureParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if (m_UseRunningAverage == true)
  {
    m_VolumesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getVolumesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_VolumesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_Volumes = m_VolumesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    tempPath.update(m_VolumesArrayPath.getDataContainerName(), m_VolumesArrayPath.getAttributeMatrixName(), getNumCellsArrayName() );
    m_NumCellsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_NumCellsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_NumCells = m_NumCellsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dims[0] = 3;
    m_CentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCentroidsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  dims[0] = 4;
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // New Feature Data
  dims[0] = 1;
  tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), getNewCellFeatureAttributeMatrixName(), getActiveArrayName() );
  m_ActivePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, true, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Do this whole block FIRST otherwise the side effect is that a call to m->getNumCellFeatureTuples will = 0
  // because we are just creating an empty NeighborList object.
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  if(getErrorCondition() < 0) { return; }

  tempPath.update(m_VolumesArrayPath.getDataContainerName(), m_VolumesArrayPath.getAttributeMatrixName(), getNeighborhoodListArrayName() );
  m_NeighborhoodList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<int>, AbstractFilter, int>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */

  tempPath.update(m_VolumesArrayPath.getDataContainerName(), m_VolumesArrayPath.getAttributeMatrixName(), getNeighborhoodsArrayName() );
  m_NeighborhoodsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NeighborhoodsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Neighborhoods = m_NeighborhoodsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Ensemble Data
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims)
                           ; /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PatchGroupMicroTextureRegions::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PatchGroupMicroTextureRegions::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  // Convert user defined tolerance to radians.
  caxisTolerance = m_CAxisTolerance * DREAM3D::Constants::k_Pi / 180.0f;

  avgCaxes[0] = 0.0f;
  avgCaxes[1] = 0.0f;
  avgCaxes[2] = 0.0f;

  // Tell the user we are starting the filter
  notifyStatusMessage(getHumanLabel(), "Starting");
  size_t numPatches = determinePatchFeatureCentroids();
  determinePatchFeatureVolumes(numPatches);
  // Making some calls to GroupFeatures
  setUseNonContiguousNeighbors(true);
  setPatchGrouping(true);
  GroupFeatures::execute();

  size_t totalFeatures = m_ActivePtr.lock()->getNumberOfTuples();
  if (totalFeatures < 2)
  {
    setErrorCondition(-87000);
    notifyErrorMessage(getHumanLabel(), "The number of Grouped Features was 0 or 1 which means no grouped features were detected. Is a grouping value set to high?", getErrorCondition());
    return;
  }

  size_t numParents = 0;
  size_t totalPoints = static_cast<size_t>(m_FeatureIdsPtr.lock()->getNumberOfTuples());
  for (size_t k = 0; k < totalPoints; k++)
  {
    int featurename = m_FeatureIds[k];
    m_CellParentIds[k] = m_FeatureParentIds[featurename];
    if(m_FeatureParentIds[featurename] > numParents) { numParents = m_FeatureParentIds[featurename]; }
  }
  numParents += 1;

  m_RandomizeParentIds = false;
  if (true == m_RandomizeParentIds)
  {
    // Generate all the numbers up front
    const int rangeMin = 1;
    const int rangeMax = numParents - 1;
    typedef boost::uniform_int<int> NumberDistribution;
    typedef boost::mt19937 RandomNumberGenerator;
    typedef boost::variate_generator < RandomNumberGenerator&,
            NumberDistribution > Generator;

    NumberDistribution distribution(rangeMin, rangeMax);
    RandomNumberGenerator generator;
    Generator numberGenerator(generator, distribution);
    generator.seed(static_cast<boost::uint32_t>( QDateTime::currentMSecsSinceEpoch() )); // seed with the current time

    DataArray<int32_t>::Pointer rndNumbers = DataArray<int32_t>::CreateArray(numParents, "New ParentIds");
    int32_t* pid = rndNumbers->getPointer(0);
    pid[0] = 0;
    QSet<int32_t> parentIdSet;
    parentIdSet.insert(0);
    for(int i = 1; i < numParents; ++i)
    {
      pid[i] = i; //numberGenerator();
      parentIdSet.insert(pid[i]);
    }

    int r;
    size_t temp;
    //--- Shuffle elements by randomly exchanging each with one other.
    for (int i = 1; i < numParents; i++)
    {
      r = numberGenerator(); // Random remaining position.
      if (r >= numParents)
      {
        continue;
      }
      temp = pid[i];
      pid[i] = pid[r];
      pid[r] = temp;
    }

    // Now adjust all the Feature Id values for each Voxel

    for(int64_t i = 0; i < totalPoints; ++i)
    {
      m_CellParentIds[i] = pid[ m_CellParentIds[i] ];
      m_FeatureParentIds[m_FeatureIds[i]] = m_CellParentIds[i];
    }
  }

  notifyStatusMessage(getHumanLabel(), "Characterizing MicroTexture Regions");
  // characterize_micro_texture_regions();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "PatchGroupMicroTextureRegions Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PatchGroupMicroTextureRegions::getSeed(int newFid)
{
  setErrorCondition(0);
  size_t numfeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  int stop = 0;
  avgCaxes[0] = 0.0f;
  avgCaxes[1] = 0.0f;
  avgCaxes[2] = 0.0f;
  if (newFid < numfeatures) { return newFid; }
  else { return -1; }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PatchGroupMicroTextureRegions::determineGrouping(int referenceFeature, int neighborFeature, int newFid)
{
  unsigned int phase1 = 0, phase2 = 0;
  float w = 0.0f;
  float g1[3][3];
  float g2[3][3];
  float g1t[3][3];
  float g2t[3][3];
  float c1[3];
  float c2[3];
  float caxis[3] = {0, 0, 1};
  QuatF q1 = QuaternionMathF::New(0.0f, 0.0f, 0.0f, 0.0f);
  QuatF q2 = QuaternionMathF::New(0.0f, 0.0f, 0.0f, 0.0f);

  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  if (m_FeatureParentIds[neighborFeature] == -1 && m_FeaturePhases[referenceFeature] > 0 && m_FeaturePhases[neighborFeature] > 0)
  {
    if (m_UseRunningAverage == false)
    {
      QuaternionMathF::Copy(avgQuats[referenceFeature], q1);
      phase1 = m_CrystalStructures[m_FeaturePhases[referenceFeature]];
      OrientationMath::QuattoMat(q1, g1);
      //transpose the g matrix so when caxis is multiplied by it
      //it will give the sample direction that the caxis is along
      MatrixMath::Transpose3x3(g1, g1t);
      MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
      //normalize so that the dot product can be taken below without
      //dividing by the magnitudes (they would be 1)
      MatrixMath::Normalize3x1(c1);
    }
    if (m_UseRunningAverage == true && avgCaxes[0] == 0.0f && avgCaxes[1] == 0.0f && avgCaxes[2] == 0.0f)
    {
      QuaternionMathF::Copy(avgQuats[referenceFeature], q1);
      phase1 = m_CrystalStructures[m_FeaturePhases[referenceFeature]];
      OrientationMath::QuattoMat(q1, g1);
      //transpose the g matrix so when caxis is multiplied by it
      //it will give the sample direction that the caxis is along
      MatrixMath::Transpose3x3(g1, g1t);
      MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
      //normalize so that the dot product can be taken below without
      //dividing by the magnitudes (they would be 1)
      MatrixMath::Normalize3x1(c1);

      MatrixMath::Copy3x1(c1, avgCaxes);
      MatrixMath::Multiply3x1withConstant(avgCaxes, m_Volumes[referenceFeature]);
    }
    phase2 = m_CrystalStructures[m_FeaturePhases[neighborFeature]];
    if (phase1 == phase2 && (phase1 == Ebsd::CrystalStructure::Hexagonal_High) )
    {
      QuaternionMathF::Copy(avgQuats[neighborFeature], q2);
      OrientationMath::QuattoMat(q2, g2);
      //transpose the g matrix so when caxis is multiplied by it
      //it will give the sample direction that the caxis is along
      MatrixMath::Transpose3x3(g2, g2t);
      MatrixMath::Multiply3x3with3x1(g2t, caxis, c2);
      //normalize so that the dot product can be taken below without
      //dividing by the magnitudes (they would be 1)
      MatrixMath::Normalize3x1(c2);

      if (m_UseRunningAverage == true) { w = GeometryMath::CosThetaBetweenVectors(avgCaxes, c2); }
      else { w = GeometryMath::CosThetaBetweenVectors(c1, c2); }
      DREAM3DMath::boundF(w, -1, 1);
      w = acosf(w);
      if (w <= caxisTolerance || (DREAM3D::Constants::k_Pi - w) <= caxisTolerance)
      {
        if (m_UseRunningAverage == true)
        {
          MatrixMath::Multiply3x1withConstant(c2, m_Volumes[neighborFeature]);
          MatrixMath::Add3x1s(avgCaxes, c2, avgCaxes);
          patchFeatureVolumeFractions[newFid] += m_Volumes[neighborFeature];
        }
        return true;
      }
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t PatchGroupMicroTextureRegions::determinePatchFeatureCentroids()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());
  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();
  int patchEdgeLengthX = int(xRes * m_MinMTRSize);
  int patchEdgeLengthY = int(yRes * m_MinMTRSize);
  int patchEdgeLengthZ = int(zRes * m_MinMTRSize);
  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  int64_t totalPoints = m->getTotalPoints();
  size_t zPatch, yPatch, xPatch;

  // round down to ensure you don't go out of bounds when quilting the landscape
  // consider alternatives in the future...

  // if z dimension = 0
  if (udims[2] == 1) { zPatch = 1; }
  else { zPatch = int(udims[2] / patchEdgeLengthZ); }
  // if y dimension = 0
  if (udims[1] == 1) { yPatch = 1; }
  else { yPatch = int(udims[1] / patchEdgeLengthY); }
  // if x dimension = 0
  if (udims[0] == 1) { xPatch = 1; }
  else { xPatch = int(udims[0] / patchEdgeLengthX); }
  size_t totalPatches = xPatch * yPatch * zPatch;
  patchFeatureVolumeFractions.resize(totalPatches + 1);
  for (int i = 1; i < totalPatches; i++)
  {
    m_FeatureParentIds[i] = i;
    patchFeatureVolumeFractions[i] = 0.0f;
  }

  // As a first go, will implement this like find neighborhoods.
  // All feature centroids within the patch centroid will be
  // patch-grouped.  The volume fraction will, somewhat
  // incorrectly, be the feature cell summation over the total
  // patch cells.
  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  // determine patch centroids
  QVector<float> patchCenters(totalPatches * 5, 0.0f);
  patchCentroids.resize(totalPatches * 3);

  int xPoints = static_cast<int>(m->getXPoints());
  int yPoints = static_cast<int>(m->getYPoints());
  int zPoints = static_cast<int>(m->getZPoints());

  // initialize every element to 0.0f
  for (size_t i = 0; i < totalPatches * 5; i++) { patchCenters[i] = 0.0f; }

  int count = 0;
  float x, y, z;
  size_t patchnum, patchIntervalX, patchIntervalY, patchIntervalZ;
  for(size_t i = 0; i < zPoints; i++)
  {
    for (size_t j = 0; j < yPoints; j++)
    {
      for(size_t k = 0; k < xPoints; k++)
      {
        patchIntervalZ = int(i / patchEdgeLengthZ) * xPatch * yPatch;
        patchIntervalY = int(j / patchEdgeLengthY) * xPatch;
        patchIntervalX = int(k / patchEdgeLengthX);
        patchnum = patchIntervalZ + patchIntervalY + patchIntervalX;
        m_CellParentIds[count] = patchnum + 1;
        x = float(k) * xRes;
        y = float(j) * yRes;
        z = float(i) * zRes;
        patchCenters[patchnum * 5 + 0]++;
        patchCenters[patchnum * 5 + 1] = patchCenters[patchnum * 5 + 1] + x;
        patchCenters[patchnum * 5 + 2] = patchCenters[patchnum * 5 + 2] + y;
        patchCenters[patchnum * 5 + 3] = patchCenters[patchnum * 5 + 3] + z;
        ++count;
      }
    }
  }

  // FIX - check for some roundoff error
  for (size_t i = 1; i < totalPatches; i++)
  {
    patchCenters[i * 5 + 1] = patchCenters[i * 5 + 1] / patchCenters[i * 5 + 0];
    patchCenters[i * 5 + 2] = patchCenters[i * 5 + 2] / patchCenters[i * 5 + 0];
    patchCenters[i * 5 + 3] = patchCenters[i * 5 + 3] / patchCenters[i * 5 + 0];
    patchCentroids[3 * i] = patchCenters[i * 5 + 1];
    patchCentroids[3 * i + 1] = patchCenters[i * 5 + 2];
    patchCentroids[3 * i + 2] = patchCenters[i * 5 + 3];
    // debugging lines
//    float checkPatchCentersx = patchCentroids[3 * i];
//    float checkPatchCentersy = patchCentroids[3 * i + 1];
//    float checkPatchCentersz = patchCentroids[3 * i + 2];
//    int stop = 0;
  }

  return totalPatches;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PatchGroupMicroTextureRegions::determinePatchFeatureVolumes(size_t totalPatches)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());
  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  QVector<QVector<int> > patchFeatureList;
  patchFeatureList.resize(totalPatches);
  float m_OriginX, m_OriginY, m_OriginZ;
  m->getOrigin(m_OriginX, m_OriginY, m_OriginZ);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif

  // taking sphere (circle) eq dia as the critical distance
  float patchCriticalDistance, patchVolume, radsquared, radcubed = 0.0f;
  // if 2D...
  if (m->getXPoints() == 1 || m->getYPoints() == 1 || m->getZPoints() == 1)
  {
    patchVolume = m_MinMTRSize * m_MinMTRSize;
    radsquared = patchVolume / DREAM3D::Constants::k_Pi;
    patchCriticalDistance = 2.0f * sqrtf(radsquared);
  }
  // if 3D...
  else
  {
    float vol_term = static_cast<double>( (4.0f / 3.0f) * DREAM3D::Constants::k_Pi );
    patchVolume = m_MinMTRSize * m_MinMTRSize * m_MinMTRSize;
    radcubed = patchVolume / vol_term;
    patchCriticalDistance = 2.0f * powf(radcubed, 0.3333333333f);
  }

  // bin features according to critical distance
  int xbin, ybin, zbin, x, y, z;
  QVector<size_t> featureBins(3 * totalFeatures, 0);
  for (size_t i = 1; i < totalFeatures; i++)
  {
    x = m_Centroids[3 * i];
    y = m_Centroids[3 * i + 1];
    z = m_Centroids[3 * i + 2];
    xbin = int((x - m_OriginX) / patchCriticalDistance);
    ybin = int((y - m_OriginY) / patchCriticalDistance);
    zbin = int((z - m_OriginZ) / patchCriticalDistance);
    featureBins[3 * i] = xbin;
    featureBins[3 * i + 1] = ybin;
    featureBins[3 * i + 2] = zbin;
  }

  // bin patches according to critical distance
  QVector<size_t> patchBins(3 * totalPatches, 0);
  for (size_t i = 1; i < totalPatches; i++)
  {
    x = patchCentroids[3 * i];
    y = patchCentroids[3 * i + 1];
    z = patchCentroids[3 * i + 2];
    xbin = int((x - m_OriginX) / patchCriticalDistance);
    ybin = int((y - m_OriginY) / patchCriticalDistance);
    zbin = int((z - m_OriginZ) / patchCriticalDistance);
    patchBins[3 * i] = xbin;
    patchBins[3 * i + 1] = ybin;
    patchBins[3 * i + 2] = zbin;
  }


  // FIX - Find a way that the data check doesn't have to be called here
  // now that we know how many patches, we create another attribute matrix sized for patches
  QVector<size_t> tDims(1, totalPatches + 1);
  m->getAttributeMatrix(getNewCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateFeatureInstancePointers();

  // create neighborlists for each patch to store their critical distance accepted centroid features
  // Do this whole block FIRST otherwise the side effect is that a call to m->getNumCellFeatureTuples will = 0
  // because we are just creating an empty NeighborList object.
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  DataArrayPath tempPath;
  QVector<size_t> dims(1, 1);
  dims[0] = 1;
  tempPath.update(m_VolumesArrayPath.getDataContainerName(), m_VolumesArrayPath.getAttributeMatrixName(), getNeighborhoodListArrayName() );
  m_NeighborhoodList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<int>, AbstractFilter, int>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */

  tempPath.update(m_VolumesArrayPath.getDataContainerName(), m_VolumesArrayPath.getAttributeMatrixName(), getNeighborhoodsArrayName() );
  m_NeighborhoodsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NeighborhoodsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Neighborhoods = m_NeighborhoodsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // find patch neighboorhoods comparing the critical distance binned features and patches


  int xPoints = static_cast<int>(m->getXPoints());
  int yPoints = static_cast<int>(m->getYPoints());
  int zPoints = static_cast<int>(m->getZPoints());

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

  size_t zStride, yStride;
  for (size_t l = 0; l < totalFeatures * 3; l += 3)
  {
    int i = m_Centroids[l + 0] * xRes;
    int j = m_Centroids[l + 1] * yRes;
    int k = m_Centroids[l + 2] * zRes;
    zStride = i * xPoints * yPoints;
    yStride = j * xPoints;
    int pnum = m_CellParentIds[zStride + yStride + k];
    int fnum = m_FeatureIds[int(l * 0.3333333f)];
    patchFeatureList[pnum].push_back(fnum);
    // debugging line
    int stop = 0;
  }

  /*


    int bin1x, bin2x, bin1y, bin2y, bin1z, bin2z, dBinX, dBinY, dBinZ;;
    QVector<float> patchFeatureVolume(totalPatches, 0.0f);
    for (size_t i = 1; i < totalPatches; i++)
    {
      if (i % 1000 == 0)
      {
        QString ss = QObject::tr("Working On Patch %1 of %2").arg(i).arg(totalPatches);
        notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      }
      bin1x = patchBins[3*i];
      bin1y = patchBins[3*i+1];
      bin1z = patchBins[3*i+2];

      for (size_t j = i + 1; j < totalFeatures; j++)
      {
        bin2x = featureBins[3*j];
        bin2y = featureBins[3*j+1];
        bin2z = featureBins[3*j+2];

        dBinX = abs(bin2x - bin1x);
        dBinY = abs(bin2y - bin1y);
        dBinZ = abs(bin2z - bin1z);

        // if the bin separation is less than the critical distance in x,y,z then add it to the patch's feature list
        if (dBinX < patchCriticalDistance && dBinY < patchCriticalDistance && dBinZ < patchCriticalDistance)
        {
          m_Neighborhoods[i]++;
          patchFeatureList[i].push_back(j);
          patchFeatureVolume[i] += m_Volumes[j];
        }
      }
      // debugging lines
      int test = m_Neighborhoods[i];
      int stop = 0;
    }

    */

  for (size_t i = 1; i < totalPatches; i++)
  {
    // Set the vector for each list into the NeighborhoodList Object
    NeighborList<int>::SharedVectorType sharedNeiLst(new std::vector<int>);
    sharedNeiLst->assign(patchFeatureList[i].begin(), patchFeatureList[i].end());
    m_NeighborhoodList.lock()->setList(static_cast<int>(i), sharedNeiLst);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PatchGroupMicroTextureRegions::growPatch(int currentPatch)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());
  float patchVolume = 0.0f;
  if (m->getXPoints() == 1 || m->getYPoints() == 1 || m->getZPoints() == 1) { patchVolume = m_MinMTRSize * m_MinMTRSize; }
  else { patchVolume = m_MinMTRSize * m_MinMTRSize * m_MinMTRSize; }
//  // debugging lines
//  float check1 = patchFeatureVolumeFractions[currentPatch];
//  float check = patchFeatureVolumeFractions[currentPatch] / patchVolume;
//  int stop = 0;
  // only flag as an MTR if above the threshold volume fraction of c-axis aligns alpha globs within the patch
  if ((patchFeatureVolumeFractions[currentPatch] / patchVolume) > m_PatchVolumeFractionForMTRGrowth) { return true; }
  else { return false; }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// FIX - find a way that determineGrouping function can do this
bool PatchGroupMicroTextureRegions::growGrouping(int referenceFeature, int neighborFeature, int newFid)
{
  unsigned int phase1 = 0, phase2 = 0;
  float w = 0.0f;
  float g1[3][3];
  float g2[3][3];
  float g1t[3][3];
  float g2t[3][3];
  float c1[3];
  float c2[3];
  float caxis[3] = {0, 0, 1};
  QuatF q1 = QuaternionMathF::New(0.0f, 0.0f, 0.0f, 0.0f);
  QuatF q2 = QuaternionMathF::New(0.0f, 0.0f, 0.0f, 0.0f);

  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  if (m_FeatureParentIds[neighborFeature] == -1 && m_FeaturePhases[referenceFeature] > 0 && m_FeaturePhases[neighborFeature] > 0)
  {
    if (m_UseRunningAverage == false)
    {
      QuaternionMathF::Copy(avgQuats[referenceFeature], q1);
      phase1 = m_CrystalStructures[m_FeaturePhases[referenceFeature]];
      OrientationMath::QuattoMat(q1, g1);
      //transpose the g matrix so when caxis is multiplied by it
      //it will give the sample direction that the caxis is along
      MatrixMath::Transpose3x3(g1, g1t);
      MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
      //normalize so that the dot product can be taken below without
      //dividing by the magnitudes (they would be 1)
      MatrixMath::Normalize3x1(c1);
    }
    if (m_UseRunningAverage == true && avgCaxes[0] == 0.0f && avgCaxes[1] == 0.0f && avgCaxes[2] == 0.0f)
    {
      QuaternionMathF::Copy(avgQuats[referenceFeature], q1);
      phase1 = m_CrystalStructures[m_FeaturePhases[referenceFeature]];
      OrientationMath::QuattoMat(q1, g1);
      //transpose the g matrix so when caxis is multiplied by it
      //it will give the sample direction that the caxis is along
      MatrixMath::Transpose3x3(g1, g1t);
      MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
      //normalize so that the dot product can be taken below without
      //dividing by the magnitudes (they would be 1)
      MatrixMath::Normalize3x1(c1);

      MatrixMath::Copy3x1(c1, avgCaxes);
      MatrixMath::Multiply3x1withConstant(avgCaxes, m_Volumes[referenceFeature]);
    }
    phase2 = m_CrystalStructures[m_FeaturePhases[neighborFeature]];

    // only line that is different from determine grouping
    if (growPatch(newFid) == true) { m_FeatureParentIds[neighborFeature] = newFid; }
    if (phase1 == phase2 && (phase1 == Ebsd::CrystalStructure::Hexagonal_High) )
    {
      QuaternionMathF::Copy(avgQuats[neighborFeature], q2);
      OrientationMath::QuattoMat(q2, g2);
      //transpose the g matrix so when caxis is multiplied by it
      //it will give the sample direction that the caxis is along
      MatrixMath::Transpose3x3(g2, g2t);
      MatrixMath::Multiply3x3with3x1(g2t, caxis, c2);
      //normalize so that the dot product can be taken below without
      //dividing by the magnitudes (they would be 1)
      MatrixMath::Normalize3x1(c2);

      if (m_UseRunningAverage == true) { w = GeometryMath::CosThetaBetweenVectors(avgCaxes, c2); }
      else { w = GeometryMath::CosThetaBetweenVectors(c1, c2); }
      DREAM3DMath::boundF(w, -1, 1);
      w = acosf(w);
      if (w <= caxisTolerance || (DREAM3D::Constants::k_Pi - w) <= caxisTolerance)
      {
        if (m_UseRunningAverage == true)
        {
          MatrixMath::Multiply3x1withConstant(c2, m_Volumes[neighborFeature]);
          MatrixMath::Add3x1s(avgCaxes, c2, avgCaxes);
          patchFeatureVolumeFractions[newFid] += m_Volumes[neighborFeature];
        }
        return true;
      }
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer PatchGroupMicroTextureRegions::newFilterInstance(bool copyFilterParameters)
{
  PatchGroupMicroTextureRegions::Pointer filter = PatchGroupMicroTextureRegions::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters() );

    //Loop over each Filter Parameter that is registered to the filter either through this class or a parent class
    // and copy the value from the current instance of the object into the "new" instance that was just created
    QVector<FilterParameter::Pointer> options = getFilterParameters(); // Get the current set of filter parameters
    for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
    {
      FilterParameter* parameter = (*iter).get();
      if (parameter->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0 )
      {
        continue; // Skip this type of filter parameter as it has nothing to do with anything in the filter.
      }
      // Get the property from the current instance of the filter
      QVariant var = property(parameter->getPropertyName().toLatin1().constData());
      bool ok = filter->setProperty(parameter->getPropertyName().toLatin1().constData(), var);
      if(false == ok)
      {
        QString ss = QString("Error occurred transferring the Filter Parameter '%1' in Filter '%2' to the filter instance. The pipeline may run but the underlying filter will NOT be using the values from the GUI."
                             " Please report this issue to the developers of this filter.").arg(parameter->getPropertyName()).arg(filter->getHumanLabel());
        Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
      }

      if(parameter->isConditional() == true)
      {
        QVariant cond = property(parameter->getConditionalProperty().toLatin1().constData() );
        ok = filter->setProperty(parameter->getConditionalProperty().toLatin1().constData(), cond);
        if(false == ok)
        {
          QString ss = QString("%1::newFilterInstance()\nError occurred transferring the Filter Parameter '%2' in Filter '%3' to the filter instance. "
                               " The filter parameter has a conditional property '%4'. The transfer of this property from the old filter to the new filter failed."
                               " Please report this issue to the developers of this filter.").arg(filter->getNameOfClass())
                       .arg(parameter->getPropertyName())
                       .arg(filter->getHumanLabel())
                       .arg(parameter->getConditionalProperty());
          Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
        }
      }
    }
  }
  return filter;
}
