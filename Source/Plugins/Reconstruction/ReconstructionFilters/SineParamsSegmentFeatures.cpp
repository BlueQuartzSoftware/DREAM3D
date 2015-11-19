/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#include "SineParamsSegmentFeatures.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Math/GeometryMath.h"

#include "SIMPLib/Utilities/SIMPLibRandom.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

// Include the MOC generated file for this class
#include "moc_SineParamsSegmentFeatures.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SineParamsSegmentFeatures::SineParamsSegmentFeatures() :
  SegmentFeatures(),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_SineParamsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::SineParams),
  m_RandomizeFeatureIds(true),
  m_UseGoodVoxels(true),
  m_GoodVoxelsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::GoodVoxels),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_SineParams(NULL),
  m_FeatureIds(NULL),
  m_GoodVoxels(NULL),
  m_Active(NULL)
{
  m_BeenPicked = NULL;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SineParamsSegmentFeatures::~SineParamsSegmentFeatures()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;

  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(LinkedBooleanFilterParameter::New("Use Good Voxels Array", "UseGoodVoxels", getUseGoodVoxels(), linkedProps, FilterParameter::Parameter));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(DataArraySelectionFilterParameter::New("Sine Parameters", "SineParamsArrayPath", getSineParamsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(DataArraySelectionFilterParameter::New("GoodVoxels", "GoodVoxelsArrayPath", getGoodVoxelsArrayPath(), FilterParameter::RequiredArray, req));
  }

  parameters.push_back(StringFilterParameter::New("FeatureIds", "FeatureIdsArrayName", getFeatureIdsArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Feature Attribute Matrix Name", "CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Active", "ActiveArrayName", getActiveArrayName(), FilterParameter::CreatedArray));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setActiveArrayName(reader->readString("ActiveArrayName", getActiveArrayName() ) );
  setCellFeatureAttributeMatrixName(reader->readString("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName() ) );
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName() ) );
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels() ) );
  setSineParamsArrayPath( reader->readDataArrayPath( "SineParamsArrayPath", getSineParamsArrayPath() ) );
  //setAngleTolerance( reader->readValue("AngleTolerance", getAngleTolerance()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SineParamsSegmentFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(ActiveArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(FeatureIdsArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(GoodVoxelsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(UseGoodVoxels)
  SIMPL_FILTER_WRITE_PARAMETER(SineParamsArrayPath)
  //SIMPL_FILTER_WRITE_PARAMETER(AngleTolerance)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  //Set the DataContainerName for the Parent Class (SegmentFeatures) to Use
  setDataContainerName(m_SineParamsArrayPath.getDataContainerName());

  SegmentFeatures::dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer cellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0 || NULL == cellFeatureAttrMat.get()) { return; }

  ImageGeom::Pointer image = m->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0 || NULL == image.get()) { return; }

  QVector<size_t> dims(1, 3);
  m_SineParamsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getSineParamsArrayPath(), dims);
  if( NULL != m_SineParamsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SineParams = m_SineParamsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 1;
  tempPath.update(getDataContainerName(), m_SineParamsArrayPath.getAttributeMatrixName(), getFeatureIdsArrayName() );
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(m_UseGoodVoxels == true)
  {
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  tempPath.update(getDataContainerName(), getCellFeatureAttributeMatrixName(), getActiveArrayName() );
  m_ActivePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, true, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::preflight()
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
void SineParamsSegmentFeatures::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  QVector<size_t> tDims(1, 1);
  m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);

  // This runs a subfilter
  int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  m_BeenPickedPtr = BoolArrayType::CreateArray(totalPoints, "BeenPicked INTERNAL ARRAY ONLY");
  m_BeenPickedPtr->initializeWithValue(0);
  m_BeenPicked = m_BeenPickedPtr->getPointer(0);

  // Tell the user we are starting the filter
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Starting");

  //Convert user defined tolerance to radians.
  //angleTolerance = m_AngleTolerance * SIMPLib::Constants::k_Pi / 180.0f;
  for(int64_t i = 0; i < totalPoints; i++)
  {
    m_FeatureIds[i] = 0;
  }

  // Generate the random voxel indices that will be used for the seed points to start a new grain growth/agglomeration
  const size_t rangeMin = 0;
  const size_t rangeMax = totalPoints - 1;
  initializeVoxelSeedGenerator(rangeMin, rangeMax);

  SegmentFeatures::execute();

  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  if (totalFeatures < 2)
  {
    setErrorCondition(-87000);
    notifyErrorMessage(getHumanLabel(), "The number of Features was 0 or 1 which means no features were detected. Is a threshold value set to high?", getErrorCondition());
    return;
  }

  // By default we randomize grains
  if (true == m_RandomizeFeatureIds)
  {
    randomizeFeatureIds(totalPoints, totalFeatures);
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::randomizeFeatureIds(int64_t totalPoints, size_t totalFeatures)
{
  notifyStatusMessage(getHumanLabel(), "Randomizing Feature Ids");
  // Generate an even distribution of numbers between the min and max range
  const size_t rangeMin = 1;
  const size_t rangeMax = totalFeatures - 1;
  initializeVoxelSeedGenerator(rangeMin, rangeMax);

// Get a reference variable to the Generator object
  Generator& numberGenerator = *m_NumberGenerator;

  DataArray<int32_t>::Pointer rndNumbers = DataArray<int32_t>::CreateArray(totalFeatures, "New GrainIds");

  int32_t* gid = rndNumbers->getPointer(0);
  gid[0] = 0;
  for(size_t i = 1; i < totalFeatures; ++i)
  {
    gid[i] = i;
  }

  size_t r;
  size_t temp;
  //--- Shuffle elements by randomly exchanging each with one other.
  for (size_t i = 1; i < totalFeatures; i++)
  {
    r = numberGenerator(); // Random remaining position.
    if (r >= totalFeatures)
    {
      continue;
    }
    temp = gid[i];
    gid[i] = gid[r];
    gid[r] = temp;
  }

  // Now adjust all the Grain Id values for each Voxel
  for(int64_t i = 0; i < totalPoints; ++i)
  {
    m_FeatureIds[i] = gid[ m_FeatureIds[i] ];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t SineParamsSegmentFeatures::getSeed(int32_t gnum)
{
  setErrorCondition(0);
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  int seed = -1;
  Generator& numberGenerator = *m_NumberGenerator;
  while(seed == -1 && m_TotalRandomNumbersGenerated < totalPoints)
  {
    // Get the next voxel index in the precomputed list of voxel seeds
    size_t randpoint = numberGenerator();
    if (m_BeenPicked[randpoint] == false) { m_TotalRandomNumbersGenerated++; } // Increment this counter
    m_BeenPicked[randpoint] = true;
    if(m_FeatureIds[randpoint] == 0) // If the GrainId of the voxel is ZERO then we can use this as a seed point
    {
      if (m_UseGoodVoxels == false || m_GoodVoxels[randpoint] == true)
      {
        seed = randpoint;
      }
    }
  }
  if (seed >= 0)
  {
    m_FeatureIds[seed] = gnum;
    QVector<size_t> tDims(1, gnum + 1);
    m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
    updateFeatureInstancePointers();
  }
  return seed;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SineParamsSegmentFeatures::determineGrouping(int64_t referencepoint, int64_t neighborpoint, int32_t gnum)
{
  bool group = false;
  float v1;
  float v2;
  float shift;
  float step = 45.0 * SIMPLib::Constants::k_PiOver180;
  float avgDiff = 0;
  if(m_FeatureIds[neighborpoint] == 0 && (m_UseGoodVoxels == false || m_GoodVoxels[neighborpoint] == true))
  {
    for(int i = 0; i < 8; i++)
    {
      shift = float(i) * step;
      v1 = m_SineParams[3 * referencepoint] * sin(2.0 * (shift + m_SineParams[3 * referencepoint + 2])) + m_SineParams[3 * referencepoint + 1];
      v2 = m_SineParams[3 * neighborpoint] * sin(2.0 * (shift + m_SineParams[3 * neighborpoint + 2])) + m_SineParams[3 * neighborpoint + 1];
      avgDiff += fabs(v1 - v2);
    }
    avgDiff /= 8.0;
    if(avgDiff < 7)
    {
      group = true;
      m_FeatureIds[neighborpoint] = gnum;
    }
  }

  return group;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::initializeVoxelSeedGenerator(const size_t rangeMin, const size_t rangeMax)
{
// The way we are using the boost random number generators is that we are asking for a NumberDistribution (see the typedef)
// to guarantee the numbers are betwee a specific range and will only be generated once. We also keep a tally of the
// total number of numbers generated as a way to make sure the while loops eventually terminate. This setup should
// make sure that every voxel can be a seed point.
//  const size_t rangeMin = 0;
//  const size_t rangeMax = totalPoints - 1;
  m_Distribution = std::shared_ptr<NumberDistribution>(new NumberDistribution(rangeMin, rangeMax));
  m_RandomNumberGenerator = std::shared_ptr<RandomNumberGenerator>(new RandomNumberGenerator);
  m_NumberGenerator = std::shared_ptr<Generator>(new Generator(*m_RandomNumberGenerator, *m_Distribution));
  m_RandomNumberGenerator->seed(static_cast<size_t>( QDateTime::currentMSecsSinceEpoch() )); // seed with the current time
  m_TotalRandomNumbersGenerated = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SineParamsSegmentFeatures::newFilterInstance(bool copyFilterParameters)
{
  SineParamsSegmentFeatures::Pointer filter = SineParamsSegmentFeatures::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SineParamsSegmentFeatures::getCompiledLibraryName()
{ return ReconstructionConstants::ReconstructionBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SineParamsSegmentFeatures::getGroupName()
{ return DREAM3D::FilterGroups::ReconstructionFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SineParamsSegmentFeatures::getSubGroupName()
{return DREAM3D::FilterSubGroups::SegmentationFilters;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SineParamsSegmentFeatures::getHumanLabel()
{ return "Segment Features (Sine Parameters)"; }

