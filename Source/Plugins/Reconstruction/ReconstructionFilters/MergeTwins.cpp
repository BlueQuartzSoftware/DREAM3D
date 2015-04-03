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

#include "MergeTwins.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "OrientationLib/OrientationOps/OrientationOps.h"


#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeTwins::MergeTwins() :
  GroupFeatures(),
  m_NewCellFeatureAttributeMatrixName(DREAM3D::Defaults::NewCellFeatureAttributeMatrixName),
  m_AxisTolerance(1.0f),
  m_AngleTolerance(1.0f),
  m_RandomizeParentIds(true),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_AvgQuatsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_CellParentIdsArrayName(DREAM3D::CellData::ParentIds),
  m_FeatureParentIdsArrayName(DREAM3D::FeatureData::ParentIds),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellParentIds(NULL),
  m_FeatureParentIds(NULL),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_AvgQuats(NULL),
  m_Active(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsQVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeTwins::~MergeTwins()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::setupFilterParameters()
{
  FilterParameterVector parameters;

  //These are parameters that the super class needs
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("Contiguous NeighborList Array", "ContiguousNeighborListArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getContiguousNeighborListArrayPath(), true, ""));

  parameters.push_front(FilterParameter::New("Axis Tolerance", "AxisTolerance", FilterParameterWidgetType::DoubleWidget, getAxisTolerance(), false, "Degrees"));
  parameters.push_front(FilterParameter::New("Angle Tolerance", "AngleTolerance", FilterParameterWidgetType::DoubleWidget, getAngleTolerance(), false, "Degrees"));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("FeaturePhases", "FeaturePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeaturePhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("AvgQuats", "AvgQuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getAvgQuatsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("CellParentIds", "CellParentIdsArrayName", FilterParameterWidgetType::StringWidget, getCellParentIdsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("FeatureParentIds", "FeatureParentIdsArrayName", FilterParameterWidgetType::StringWidget, getFeatureParentIdsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("New Cell Feature Attribute Matrix Name", "NewCellFeatureAttributeMatrixName", FilterParameterWidgetType::StringWidget, getNewCellFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Active", "ActiveArrayName", FilterParameterWidgetType::StringWidget, getActiveArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  GroupFeatures::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setNewCellFeatureAttributeMatrixName(reader->readString("NewCellFeatureAttributeMatrixName", getNewCellFeatureAttributeMatrixName() ) );
  setActiveArrayName(reader->readString("ActiveArrayName", getActiveArrayName() ) );
  setFeatureParentIdsArrayName(reader->readString("FeatureParentIdsArrayName", getFeatureParentIdsArrayName() ) );
  setCellParentIdsArrayName(reader->readString("CellParentIdsArrayName", getCellParentIdsArrayName() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setAxisTolerance( reader->readValue("AxisTolerance", getAxisTolerance()) );
  setAngleTolerance( reader->readValue("AngleTolerance", getAngleTolerance()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MergeTwins::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  GroupFeatures::writeFilterParameters(writer, index);
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(ActiveArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(NewCellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureParentIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellParentIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(AvgQuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(AxisTolerance)
  DREAM3D_FILTER_WRITE_PARAMETER(AngleTolerance)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_FeatureIdsArrayPath.getDataContainerName(), false);
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
  dims[0] = 4;
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // NewFeature Data
  dims[0] = 1;
  tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), getNewCellFeatureAttributeMatrixName(), getActiveArrayName() );
  m_ActivePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, true, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Ensemble Data
//typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims)
                           ; /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::preflight()
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
void MergeTwins::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  axisTolerance = m_AxisTolerance * DREAM3D::Constants::k_Pi / 180.0f;

  // Tell the user we are starting the filter
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Starting");
  m_FeatureParentIds[0] = 0; // set feature 0 to be parent 0
  GroupFeatures::execute();

  size_t totalFeatures = m_ActivePtr.lock()->getNumberOfTuples();
  if (totalFeatures < 2)
  {
    setErrorCondition(-87000);
    notifyErrorMessage(getHumanLabel(), "The number of Grouped Features was 0 or 1 which means no grouped features were detected. Is a grouping value set to high?", getErrorCondition());
    return;
  }

  size_t numParents = 0;
  int64_t totalPoints = static_cast<size_t>(m_FeatureIdsPtr.lock()->getNumberOfTuples());
  for (int64_t k = 0; k < totalPoints; k++)
  {
    int featurename = m_FeatureIds[k];
    m_CellParentIds[k] = m_FeatureParentIds[featurename];
    if(m_FeatureParentIds[featurename] > numParents) { numParents = m_FeatureParentIds[featurename]; }
  }
  numParents += 1;

  notifyStatusMessage(getHumanLabel(), "Characterizing Twins");
  characterize_twins();

// m_RandomizeParentIds = false;
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
      m_CellParentIds[i] = pid[m_CellParentIds[i]];
      m_FeatureParentIds[m_FeatureIds[i]] = m_CellParentIds[i];
    }
  }

  notifyStatusMessage(getHumanLabel(), "Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MergeTwins::getSeed(int newFid)
{
  setErrorCondition(0);

  size_t numfeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

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
    QVector<size_t> tDims(1, newFid + 1);
    getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName())->getAttributeMatrix(getNewCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
    updateFeatureInstancePointers();
  }
  return seed;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MergeTwins::determineGrouping(int referenceFeature, int neighborFeature, int newFid)
{
  float w;
  float n1, n2, n3;
  bool twin = false;
  QuatF q1;
  QuatF q2;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  if (m_FeatureParentIds[neighborFeature] == -1 && m_FeaturePhases[referenceFeature] > 0 && m_FeaturePhases[neighborFeature] > 0)
  {
    //w = 10000.0f;
    QuaternionMathF::Copy(avgQuats[referenceFeature], q1);
    unsigned int phase1 = m_CrystalStructures[m_FeaturePhases[referenceFeature]];
    QuaternionMathF::Copy(avgQuats[neighborFeature], q2);
    unsigned int phase2 = m_CrystalStructures[m_FeaturePhases[neighborFeature]];
    if (phase1 == phase2 && (phase1 == Ebsd::CrystalStructure::Cubic_High))
    {
      w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
      w = w * (180.0f / DREAM3D::Constants::k_Pi);
      float axisdiff111 = acosf(fabs(n1) * 0.57735f + fabs(n2) * 0.57735f + fabs(n3) * 0.57735f);
      float angdiff60 = fabs(w - 60.0f);
      if (axisdiff111 < axisTolerance && angdiff60 < m_AngleTolerance) { twin = true; }
      if (twin == true)
      {
        m_FeatureParentIds[neighborFeature] = newFid;
        return true;
      }
    }
  }
  return false;
}

void MergeTwins::characterize_twins()
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MergeTwins::newFilterInstance(bool copyFilterParameters)
{
  MergeTwins::Pointer filter = MergeTwins::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeTwins::getCompiledLibraryName()
{ return Reconstruction::ReconstructionBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeTwins::getGroupName()
{ return DREAM3D::FilterGroups::ReconstructionFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeTwins::getSubGroupName()
{return DREAM3D::FilterSubGroups::GroupingFilters;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeTwins::getHumanLabel()
{ return "Merge Twins"; }

