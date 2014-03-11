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
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_NewCellFeatureAttributeMatrixName(DREAM3D::Defaults::NewCellFeatureAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_CAxisTolerance(1.0f),
  m_UseRunningAverage(false),
  m_PatchEdgeLength(1),
  m_PatchVolumeFractionForMTRGrowth(1.0f),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellParentIdsArrayName(DREAM3D::CellData::ParentIds),
  m_FeatureParentIdsArrayName(DREAM3D::FeatureData::ParentIds),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_VolumesArrayName(DREAM3D::FeatureData::Volumes),
  m_NumCellsArrayName(DREAM3D::FeatureData::NumCells),
  m_CentroidsArrayName(DREAM3D::FeatureData::Centroids),
  m_NeighborhoodListArrayName(DREAM3D::FeatureData::NeighborhoodList),
  m_NeighborhoodsArrayName(DREAM3D::FeatureData::Neighborhoods),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures)
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
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("C-Axis Alignment Tolerance");
    parameter->setPropertyName("CAxisTolerance");
    parameter->setWidgetType(FilterParameterWidgetType::DoubleWidget);
    parameter->setValueType("float");
    parameter->setCastableValueType("double");
    parameter->setUnits("Degrees");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Group C-Axes With Running Average");
    parameter->setPropertyName("UseRunningAverage");
    parameter->setWidgetType(FilterParameterWidgetType::BooleanWidget);
    parameter->setValueType("bool");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  {
  // FIX - change to minimum MTR size (diameter)
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Patch Edge Length");
    parameter->setPropertyName("PatchEdgeLength");
    parameter->setWidgetType(FilterParameterWidgetType::IntWidget);
    parameter->setValueType("int");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Minimum C-Axis Aligned Patch Volume Fraction For MTR Growth");
    parameter->setPropertyName("PatchVolumeFractionForMTRGrowth");
    parameter->setWidgetType(FilterParameterWidgetType::DoubleWidget);
    parameter->setValueType("float");
    parameter->setCastableValueType("double");
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PatchGroupMicroTextureRegions::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setCAxisTolerance( reader->readValue("CAxisTolerance", getCAxisTolerance()) );
  setUseRunningAverage( reader->readValue("UseRunningAverage", getUseRunningAverage()) );
  setPatchEdgeLength( reader->readValue("PatchEdgeLength", getPatchEdgeLength()) );
  setPatchVolumeFractionForMTRGrowth( reader->readValue("PatchVolumeFractionForMTRGrowth", getPatchVolumeFractionForMTRGrowth()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PatchGroupMicroTextureRegions::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("CAxisTolerance", getCAxisTolerance() );
  writer->writeValue("UseRunningAverage", getUseRunningAverage() );
  writer->writeValue("PatchEdgeLength", getPatchEdgeLength() );
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
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer newCellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getNewCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }
  if(NULL == m)
  {
    QString ss = QObject::tr("The Volume Data Container with name '%1'' was not found in the Data Container Array.").arg(getDataContainerName());
    setErrorCondition(-1001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<size_t> dims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureIdsArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellParentIdsPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_CellParentIdsArrayName, -1, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellParentIds = m_CellParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Data
  m_FeaturePhasesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeaturePhasesArrayName, -303, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_FeatureParentIdsPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_FeatureParentIdsArrayName, -1, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureParentIds = m_FeatureParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if (m_UseRunningAverage == true)
  {
  m_VolumesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_VolumesArrayName, -304, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_VolumesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Volumes = m_VolumesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_NumCellsPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_NumCellsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumCellsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumCells = m_NumCellsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  m_CentroidsPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_CentroidsArrayName, -305, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  dims[0] = 4;
  m_AvgQuatsPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_AvgQuatsArrayName, -302, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // New Feature Data
  dims[0] = 1;
  m_ActivePtr = newCellFeatureAttrMat->createNonPrereqArray<DataArray<bool>, AbstractFilter, bool>(this, m_ActiveArrayName, true, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Do this whole block FIRST otherwise the side effect is that a call to m->getNumCellFeatureTuples will = 0
  // because we are just creating an empty NeighborList object.
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  if(getErrorCondition() < 0) { return; }
  m_NeighborhoodList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >
      (m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(m_NeighborhoodListArrayName).get());
  if(m_NeighborhoodList == NULL)
  {
    NeighborList<int>::Pointer neighborhoodlistPtr = NeighborList<int>::New();
    neighborhoodlistPtr->setName(m_NeighborhoodListArrayName);
	neighborhoodlistPtr->resize(cellFeatureAttrMat->getNumTuples());
    neighborhoodlistPtr->setNumNeighborsArrayName(m_NeighborhoodsArrayName);
    m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(m_NeighborhoodListArrayName, neighborhoodlistPtr);

    if (neighborhoodlistPtr.get() == NULL)
    {
      QString ss = QObject::tr("NeighborhoodLists Array Not Initialized at Beginning of FindNeighbors Filter");
      setErrorCondition(-308);
    }
    m_NeighborhoodList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >
        (m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(m_NeighborhoodListArrayName).get());

    CreatedArrayHelpIndexEntry::Pointer e = CreatedArrayHelpIndexEntry::New();
    e->setFilterName(this->getNameOfClass());
    e->setFilterHumanLabel(this->getHumanLabel());
    e->setFilterGroup(this->getGroupName());
    e->setFilterSubGroup(this->getSubGroupName());
    e->setArrayDefaultName(m_NeighborhoodListArrayName);
    e->setArrayGroup("Feature");
    e->setArrayNumComponents(0);
    e->setArrayType("NeighborList");
    addCreatedArrayHelpIndexEntry(e);
  }

  m_NeighborhoodsPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_NeighborhoodsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NeighborhoodsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Neighborhoods = m_NeighborhoodsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  
  // Ensemble Data
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = cellEnsembleAttrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(this, m_CrystalStructuresArrayName, -305, dims)
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

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  // Convert user defined tolerance to radians.
  m_CAxisTolerance = m_CAxisTolerance * DREAM3D::Constants::k_Pi / 180.0f;

  avgCaxes[0] = 0.0f;
  avgCaxes[1] = 0.0f;
  avgCaxes[2] = 0.0f;

  // Tell the user we are starting the filter
  notifyStatusMessage(getHumanLabel(), "Starting");
  size_t numPatches = determinePatchFeatureCentroids();
  determinePatchFeatureVolumes(numPatches);
  // Making some calls to 
  setUseNonContiguousNeighbors(true);
  setPatchGrouping(true);
  GroupFeatures::execute();

  size_t totalFeatures = m->getAttributeMatrix(getNewCellFeatureAttributeMatrixName())->getNumTuples();
  if (totalFeatures < 2)
  {
    setErrorCondition(-87000);
    notifyErrorMessage(getHumanLabel(), "The number of Grouped Features was 0 or 1 which means no grouped features were detected. Is a grouping value set to high?", getErrorCondition());
    return;
  }

  size_t numParents = 0;
  size_t totalPoints = static_cast<size_t>(m->getTotalPoints());
  for (size_t k = 0; k < totalPoints; k++)
  {
    int featurename = m_FeatureIds[k];
    m_CellParentIds[k] = m_FeatureParentIds[featurename];
    if(m_FeatureParentIds[featurename] > numParents) numParents = m_FeatureParentIds[featurename];
  }
  numParents += 1;

  //m_RandomizeParentIds = false;
  if (true == m_RandomizeParentIds)
  {
    int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();

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
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  if (newFid <= numfeatures) return newFid;
  else return -1;
  /*

  float c1[3];
  unsigned int phase1;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);
  float caxis[3] = {0, 0, 1};
  QuatF q1;
  float g1[3][3];
  float g1t[3][3];

  DREAM3D_RANDOMNG_NEW()
  int seed = -1;
  int randfeature = 0;

  // Precalculate some constants
  size_t totalFMinus1 = numfeatures - 1;

  size_t counter = 0;
  randfeature = int(float(rg.genrand_res53()) * float(totalFMinus1));
  while (seed == -1 && counter < numfeatures)
  {
    if (randfeature > totalFMinus1) { randfeature = static_cast<int>( randfeature - numfeatures ); }
    if (m_FeatureParentIds[randfeature] == -1) { seed = randfeature; }
    randfeature++;
    counter++;
  }
  if (seed >= 0)
  {
    m_FeatureParentIds[seed] = newFid;
    QVector<size_t> tDims(1, newFid+1);
    m->getAttributeMatrix(getNewCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
    updateFeatureInstancePointers();

    if (m_UseRunningAverage == true)
    {
      QuaternionMathF::Copy(avgQuats[seed], q1);
      phase1 = m_CrystalStructures[m_FeaturePhases[seed]];
      OrientationMath::QuattoMat(q1, g1);
      //transpose the g matrix so when caxis is multiplied by it
      //it will give the sample direction that the caxis is along
      MatrixMath::Transpose3x3(g1, g1t);
      MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
      //normalize so that the dot product can be taken below without
      //dividing by the magnitudes (they would be 1)
      MatrixMath::Normalize3x1(c1);

      MatrixMath::Copy3x1(c1,avgCaxes);
      MatrixMath::Multiply3x1withConstant(avgCaxes,m_Volumes[seed]);
    }
  }
  */
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

  if (m_FeaturePhases[referenceFeature] > 0 && m_FeaturePhases[neighborFeature] > 0)
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

      MatrixMath::Copy3x1(c1,avgCaxes);
      MatrixMath::Multiply3x1withConstant(avgCaxes,m_Volumes[referenceFeature]);
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

      if (m_UseRunningAverage == true) w = GeometryMath::CosThetaBetweenVectors(avgCaxes, c2);
      else w = GeometryMath::CosThetaBetweenVectors(c1, c2);
      DREAM3DMath::boundF(w, -1, 1);
      w = acosf(w);
      if (w <= m_CAxisTolerance || (DREAM3D::Constants::k_Pi - w) <= m_CAxisTolerance)
      {
//        m_FeatureParentIds[neighborFeature] = newFid;
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
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  int64_t totalPoints = m->getTotalPoints();
//  QVector<int> patchIds(totalPoints, 0);
  size_t zPatch;
  if (udims[2] == 1) zPatch = 1;
  else zPatch = floor(udims[2] / m_PatchEdgeLength);

  size_t totalPatches = floor(udims[0] / m_PatchEdgeLength) * floor(udims[1] / m_PatchEdgeLength) * zPatch;
  patchFeatureVolumeFractions.resize(totalPatches+1);
  for (int i = 0; i < totalPatches; i++)
  {
	m_FeatureParentIds[i+1] = i+1;
	patchFeatureVolumeFractions[i+1] = 0.0f;
  }

// FIX - figure out what to do when patches don't fit perfectly
//  if (udims[0] % m_PatchEdgeLength != 0)
//  {
//
//  }

  // figure out what centroids are in each patch

  // As a first go, will implement this like find neighborhoods.
  // All feature centroids within the patch centroid will be
  // patch-grouped.  The volume fraction will, somewhat
  // incorrectly, be the feature cell summation over the total
  // patch cells.

  float x, y, z;

  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  // determine patch centroids

  QVector<float> patchCenters(totalPatches*5, 0.0f);
  patchCentroids.resize(totalPatches * 3);

  int xPoints = static_cast<int>(m->getXPoints());
  int yPoints = static_cast<int>(m->getYPoints());
  int zPoints = static_cast<int>(m->getZPoints());

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

  // Initialize every element to 0.0
  for (size_t i = 0; i < totalPatches * 5; i++)
  {
    patchCenters[i] = 0.0f;
  }

  // right now, only works if dimensions are divisible by patch length
  int count = 0;
  size_t zStride, yStride, patchnum, patchIntervalX, patchIntervalY, patchIntervalZ;
  for(size_t i = 0; i < zPoints; i++)
  {
    for (size_t j = 0; j < yPoints; j++)
	{
      for(size_t k = 0; k < xPoints; k++)
	  {
		patchIntervalZ = int(floor(i/m_PatchEdgeLength)) * int(zPoints/m_PatchEdgeLength);
		patchIntervalY = int(floor(j/m_PatchEdgeLength)) * int(yPoints/m_PatchEdgeLength);
	    patchIntervalX = int(floor(k/m_PatchEdgeLength));
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
	float checkPatchCentersx = patchCentroids[3 * i];
	float checkPatchCentersy = patchCentroids[3 * i + 1];
	float checkPatchCentersz = patchCentroids[3 * i + 2];
	int stop = 0;
  }

  return totalPatches;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PatchGroupMicroTextureRegions::determinePatchFeatureVolumes(size_t totalPatches)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();
  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
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
  float patchCriticalDistance, patchVolume, radsquared, radcubed, res_scalar = 0.0f;
  if (m->getXPoints() == 1 || m->getYPoints() == 1 || m->getZPoints() == 1)
  {
	if(m->getXPoints() == 1) { res_scalar = m->getYRes() * m->getZRes(); }
	else if(m->getYPoints() == 1) { res_scalar = m->getXRes() * m->getZRes(); }
	else if(m->getZPoints() == 1) { res_scalar = m->getXRes() * m->getYRes(); }
	patchVolume = m_PatchEdgeLength * m_PatchEdgeLength / res_scalar;
	radsquared = patchVolume / DREAM3D::Constants::k_Pi;
	patchCriticalDistance = 2.0f * sqrtf(radsquared);
  }
  else
  {
	res_scalar = m->getXRes() * m->getYRes() * m->getZRes();
	float vol_term = static_cast<double>( (4.0f / 3.0f) * DREAM3D::Constants::k_Pi );
	patchVolume = m_PatchEdgeLength * m_PatchEdgeLength * m_PatchEdgeLength / res_scalar;
	radcubed = patchVolume / vol_term;
	patchCriticalDistance = 2.0f * powf(radcubed, 0.3333333333f);
  }

  int xbin, ybin, zbin, x, y, z;
  QVector<size_t> featureBins(3*totalFeatures, 0);
  for (size_t i = 1; i < totalFeatures; i++)
  {
    x = m_Centroids[3 * i];
    y = m_Centroids[3 * i + 1];
    z = m_Centroids[3 * i + 2];
    xbin = int((x - m_OriginX) / patchCriticalDistance);
    ybin = int((y - m_OriginY) / patchCriticalDistance);
    zbin = int((z - m_OriginZ) / patchCriticalDistance);
    featureBins[3*i] = xbin;
    featureBins[3*i+1] = ybin;
    featureBins[3*i+2] = zbin;
  }

  QVector<size_t> patchBins(3*totalPatches, 0);
  for (size_t i = 1; i < totalPatches; i++)
  {
    x = patchCentroids[3 * i];
    y = patchCentroids[3 * i + 1];
    z = patchCentroids[3 * i + 2];
    xbin = int((x - m_OriginX) / patchCriticalDistance);
    ybin = int((y - m_OriginY) / patchCriticalDistance);
    zbin = int((z - m_OriginZ) / patchCriticalDistance);
    patchBins[3*i] = xbin;
    patchBins[3*i+1] = ybin;
    patchBins[3*i+2] = zbin;
  }

  QVector<size_t> tDims(1, totalPatches+1);
  m->getAttributeMatrix(getNewCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateFeatureInstancePointers();

  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  // Do this whole block FIRST otherwise the side effect is that a call to m->getNumCellFeatureTuples will = 0
  // because we are just creating an empty NeighborList object.
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  if(getErrorCondition() < 0) { return; }
  m_NeighborhoodList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >
      (m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(m_NeighborhoodListArrayName).get());
  if(m_NeighborhoodList == NULL)
  {
    NeighborList<int>::Pointer neighborhoodlistPtr = NeighborList<int>::New();
    neighborhoodlistPtr->setName(m_NeighborhoodListArrayName);
	neighborhoodlistPtr->resize(cellFeatureAttrMat->getNumTuples());
    neighborhoodlistPtr->setNumNeighborsArrayName(m_NeighborhoodsArrayName);
    m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(m_NeighborhoodListArrayName, neighborhoodlistPtr);

    if (neighborhoodlistPtr.get() == NULL)
    {
      QString ss = QObject::tr("NeighborhoodLists Array Not Initialized at Beginning of FindNeighbors Filter");
      setErrorCondition(-308);
    }
    m_NeighborhoodList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >
        (m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(m_NeighborhoodListArrayName).get());

    CreatedArrayHelpIndexEntry::Pointer e = CreatedArrayHelpIndexEntry::New();
    e->setFilterName(this->getNameOfClass());
    e->setFilterHumanLabel(this->getHumanLabel());
    e->setFilterGroup(this->getGroupName());
    e->setFilterSubGroup(this->getSubGroupName());
    e->setArrayDefaultName(m_NeighborhoodListArrayName);
    e->setArrayGroup("Feature");
    e->setArrayNumComponents(0);
    e->setArrayType("NeighborList");
    addCreatedArrayHelpIndexEntry(e);
  }
  QVector<size_t> dims(1, 1);
  dims[0] = 1;
  m_NeighborhoodsPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_NeighborhoodsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NeighborhoodsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Neighborhoods = m_NeighborhoodsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  int bin1x, bin2x, bin1y, bin2y, bin1z, bin2z;
  int dBinX, dBinY, dBinZ;
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
  for (size_t i = 1; i < totalPatches; i++)
  {
    // Set the vector for each list into the NeighborhoodList Object
    NeighborList<int>::SharedVectorType sharedNeiLst(new std::vector<int>);
    sharedNeiLst->assign(patchFeatureList[i].begin(), patchFeatureList[i].end());
    m_NeighborhoodList->setList(static_cast<int>(i), sharedNeiLst);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PatchGroupMicroTextureRegions::growPatch(int currentPatch)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  float res_scalar;
  float patchVolume = 0.0f;
  if (m->getXPoints() == 1 || m->getYPoints() == 1 || m->getZPoints() == 1)
  {
	if(m->getXPoints() == 1) { res_scalar = m->getYRes() * m->getZRes(); }
	else if(m->getYPoints() == 1) { res_scalar = m->getXRes() * m->getZRes(); }
	else if(m->getZPoints() == 1) { res_scalar = m->getXRes() * m->getYRes(); }
	patchVolume = m_PatchEdgeLength * m_PatchEdgeLength * res_scalar;
  }
  else
  {
	res_scalar = m->getXRes() * m->getYRes() * m->getZRes();
	patchVolume = (m_PatchEdgeLength * m_PatchEdgeLength * m_PatchEdgeLength);
  }

  // only flag as an MTR if above the threshold volume fraction of c-ax0s aligns alpha globs within the patch
  if ((patchFeatureVolumeFractions[currentPatch] / patchVolume) > m_PatchVolumeFractionForMTRGrowth);
  {

  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer PatchGroupMicroTextureRegions::newFilterInstance(bool copyFilterParameters)
{
  /*
  * CAxisTolerance
  * UseRunningAverage
  * PatchEdgeLength
  */
  PatchGroupMicroTextureRegions::Pointer filter = PatchGroupMicroTextureRegions::New();
  if(true == copyFilterParameters)
  {
    filter->setCAxisTolerance( getCAxisTolerance() );
    filter->setUseRunningAverage( getUseRunningAverage() );
    filter->setPatchEdgeLength( getPatchEdgeLength() );
    filter->setRandomizeParentIds( getRandomizeParentIds() );
    filter->setPatchGrouping( getPatchGrouping() );
  }
  return filter;
}
