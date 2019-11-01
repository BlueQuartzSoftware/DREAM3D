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

#include <memory>

#include "SineParamsSegmentFeatures.h"

#include <chrono>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Math/SIMPLibRandom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataArrayID30 = 30,
  DataArrayID31 = 31,
};

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SineParamsSegmentFeatures::SineParamsSegmentFeatures()
: m_CellFeatureAttributeMatrixName(SIMPL::Defaults::CellFeatureAttributeMatrixName)
, m_SineParamsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::SineParams)
, m_RandomizeFeatureIds(true)
, m_UseGoodVoxels(true)
, m_GoodVoxelsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::GoodVoxels)
, m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds)
, m_ActiveArrayName(SIMPL::FeatureData::Active)
, m_MissingGoodVoxels(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SineParamsSegmentFeatures::~SineParamsSegmentFeatures() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::setupFilterParameters()
{
  SegmentFeatures::setupFilterParameters();
  FilterParameterVectorType parameters;

  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Good Voxels Array", UseGoodVoxels, FilterParameter::Parameter, SineParamsSegmentFeatures, linkedProps));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Sine Parameters", SineParamsArrayPath, FilterParameter::RequiredArray, SineParamsSegmentFeatures, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("GoodVoxels", GoodVoxelsArrayPath, FilterParameter::RequiredArray, SineParamsSegmentFeatures, req));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("FeatureIds", FeatureIdsArrayName, FilterParameter::CreatedArray, SineParamsSegmentFeatures));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Feature Attribute Matrix Name", CellFeatureAttributeMatrixName, FilterParameter::CreatedArray, SineParamsSegmentFeatures));
  parameters.push_back(SIMPL_NEW_STRING_FP("Active", ActiveArrayName, FilterParameter::CreatedArray, SineParamsSegmentFeatures));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setActiveArrayName(reader->readString("ActiveArrayName", getActiveArrayName()));
  setCellFeatureAttributeMatrixName(reader->readString("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName()));
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath()));
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels()));
  setSineParamsArrayPath(reader->readDataArrayPath("SineParamsArrayPath", getSineParamsArrayPath()));
  // setAngleTolerance( reader->readValue("AngleTolerance", getAngleTolerance()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::updateFeatureInstancePointers()
{
  clearErrorCode();
  clearWarningCode();

  if(nullptr != m_ActivePtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Active = m_ActivePtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::initialize()
{
  m_MissingGoodVoxels = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::dataCheck()
{
  DataArrayPath tempPath;
  initialize();
  clearErrorCode();
  clearWarningCode();

  // Set the DataContainerName for the Parent Class (SegmentFeatures) to Use
  setDataContainerName(m_SineParamsArrayPath.getDataContainerName());

  SegmentFeatures::dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerName(), false);
  if(getErrorCode() < 0 || nullptr == m)
  {
    return;
  }
  std::vector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer cellFeatureAttrMat = m->createNonPrereqAttributeMatrix(this, getCellFeatureAttributeMatrixName(), tDims, AttributeMatrix::Type::CellFeature, AttributeMatrixID21);
  if(getErrorCode() < 0 || nullptr == cellFeatureAttrMat.get())
  {
    return;
  }

  ImageGeom::Pointer image = m->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCode() < 0 || nullptr == image.get())
  {
    return;
  }

  std::vector<size_t> dims(1, 3);
  m_SineParamsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getSineParamsArrayPath(), dims);
  if(nullptr != m_SineParamsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SineParams = m_SineParamsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 1;
  tempPath.update(getDataContainerName(), m_SineParamsArrayPath.getAttributeMatrixName(), getFeatureIdsArrayName());
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, dims);               /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(m_UseGoodVoxels)
  {
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(),
                                                                                                       dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_GoodVoxelsPtr.lock())                                                                     /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  tempPath.update(getDataContainerName(), getCellFeatureAttributeMatrixName(), getActiveArrayName());
  m_ActivePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, true, dims, "", DataArrayID31);
  if(nullptr != m_ActivePtr.lock())                                                                                 /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Active = m_ActivePtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
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
  clearErrorCode();
  clearWarningCode();

  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  std::vector<size_t> tDims(1, 1);
  m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);

  // This runs a subfilter
  int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  // Tell the user we are starting the filter
  notifyStatusMessage("Starting");

  // Convert user defined tolerance to radians.
  // angleTolerance = m_AngleTolerance * SIMPLib::Constants::k_Pi / 180.0f;
  for(int64_t i = 0; i < totalPoints; i++)
  {
    m_FeatureIds[i] = 0;
  }

  // Generate the random voxel indices that will be used for the seed points to start a new grain growth/agglomeration
  const size_t rangeMin = 0;
  const size_t rangeMax = totalPoints - 1;
  initializeVoxelSeedGenerator(rangeMin, rangeMax);

  SegmentFeatures::execute();

  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumberOfTuples();
  if(totalFeatures < 2)
  {
    setErrorCondition(-87000, "The number of Features was 0 or 1 which means no features were detected. Is a threshold value set to high?");
    return;
  }

  // By default we randomize grains
  if(m_RandomizeFeatureIds)
  {
    randomizeFeatureIds(totalPoints, totalFeatures);
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::randomizeFeatureIds(int64_t totalPoints, size_t totalFeatures)
{
  notifyStatusMessage("Randomizing Feature Ids");
  // Generate an even distribution of numbers between the min and max range
  const size_t rangeMin = 1;
  const size_t rangeMax = totalFeatures - 1;
  initializeVoxelSeedGenerator(rangeMin, rangeMax);

  DataArray<int32_t>::Pointer rndNumbers = DataArray<int32_t>::CreateArray(totalFeatures, "New GrainIds", true);

  int32_t* gid = rndNumbers->getPointer(0);
  gid[0] = 0;
  for(size_t i = 1; i < totalFeatures; ++i)
  {
    gid[i] = i;
  }

  size_t r;
  size_t temp;
  //--- Shuffle elements by randomly exchanging each with one other.
  for(size_t i = 1; i < totalFeatures; i++)
  {
    r = m_Distribution(m_Generator); // Random remaining position.
    if(r >= totalFeatures)
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
    m_FeatureIds[i] = gid[m_FeatureIds[i]];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t SineParamsSegmentFeatures::getSeed(int32_t gnum, int64_t nextSeed)
{
  clearErrorCode();
  clearWarningCode();
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  int64_t seed = -1;
  // start with the next voxel after the last seed
  size_t randpoint = static_cast<size_t>(nextSeed);
  while(seed == -1 && randpoint < totalPoints)
  {
    if(m_FeatureIds[randpoint] == 0) // If the GrainId of the voxel is ZERO then we can use this as a seed point
    {
      if(!m_UseGoodVoxels || m_GoodVoxels[randpoint])
      {
        seed = randpoint;
      }
      else
      {
        randpoint += 1;
      }
    }
    else
    {
      randpoint += 1;
    }
  }
  if(seed >= 0)
  {
    m_FeatureIds[seed] = gnum;
    std::vector<size_t> tDims(1, gnum + 1);
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
  float step = 45.0f * SIMPLib::Constants::k_PiOver180;
  float avgDiff = 0;
  if(m_FeatureIds[neighborpoint] == 0 && (!m_UseGoodVoxels || m_GoodVoxels[neighborpoint]))
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

  std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
  m_Generator.seed(seed);
  m_Distribution = std::uniform_int_distribution<int64_t>(rangeMin, rangeMax);
  m_Distribution = std::uniform_int_distribution<int64_t>(rangeMin, rangeMax);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SineParamsSegmentFeatures::newFilterInstance(bool copyFilterParameters) const
{
  SineParamsSegmentFeatures::Pointer filter = SineParamsSegmentFeatures::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SineParamsSegmentFeatures::getCompiledLibraryName() const
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SineParamsSegmentFeatures::getBrandingString() const
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SineParamsSegmentFeatures::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Reconstruction::Version::Major() << "." << Reconstruction::Version::Minor() << "." << Reconstruction::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SineParamsSegmentFeatures::getGroupName() const
{
  return SIMPL::FilterGroups::ReconstructionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid SineParamsSegmentFeatures::getUuid() const
{
  return QUuid("{2d05beff-cfda-5f38-afae-727bfc529585}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SineParamsSegmentFeatures::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::SegmentationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SineParamsSegmentFeatures::getHumanLabel() const
{
  return "Segment Features (Sine Parameters)";
}

// -----------------------------------------------------------------------------
SineParamsSegmentFeatures::Pointer SineParamsSegmentFeatures::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<SineParamsSegmentFeatures> SineParamsSegmentFeatures::New()
{
  struct make_shared_enabler : public SineParamsSegmentFeatures
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString SineParamsSegmentFeatures::getNameOfClass() const
{
  return QString("SineParamsSegmentFeatures");
}

// -----------------------------------------------------------------------------
QString SineParamsSegmentFeatures::ClassName()
{
  return QString("SineParamsSegmentFeatures");
}

// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::setCellFeatureAttributeMatrixName(const QString& value)
{
  m_CellFeatureAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString SineParamsSegmentFeatures::getCellFeatureAttributeMatrixName() const
{
  return m_CellFeatureAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::setSineParamsArrayPath(const DataArrayPath& value)
{
  m_SineParamsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath SineParamsSegmentFeatures::getSineParamsArrayPath() const
{
  return m_SineParamsArrayPath;
}

// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::setRandomizeFeatureIds(bool value)
{
  m_RandomizeFeatureIds = value;
}

// -----------------------------------------------------------------------------
bool SineParamsSegmentFeatures::getRandomizeFeatureIds() const
{
  return m_RandomizeFeatureIds;
}

// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::setUseGoodVoxels(bool value)
{
  m_UseGoodVoxels = value;
}

// -----------------------------------------------------------------------------
bool SineParamsSegmentFeatures::getUseGoodVoxels() const
{
  return m_UseGoodVoxels;
}

// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::setGoodVoxelsArrayPath(const DataArrayPath& value)
{
  m_GoodVoxelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath SineParamsSegmentFeatures::getGoodVoxelsArrayPath() const
{
  return m_GoodVoxelsArrayPath;
}

// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::setFeatureIdsArrayName(const QString& value)
{
  m_FeatureIdsArrayName = value;
}

// -----------------------------------------------------------------------------
QString SineParamsSegmentFeatures::getFeatureIdsArrayName() const
{
  return m_FeatureIdsArrayName;
}

// -----------------------------------------------------------------------------
void SineParamsSegmentFeatures::setActiveArrayName(const QString& value)
{
  m_ActiveArrayName = value;
}

// -----------------------------------------------------------------------------
QString SineParamsSegmentFeatures::getActiveArrayName() const
{
  return m_ActiveArrayName;
}
