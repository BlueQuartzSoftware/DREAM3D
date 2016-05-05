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

#include "ScalarSegmentFeatures.h"

#include <QtCore/QDateTime>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Reconstruction/ReconstructionConstants.h"

/* from http://www.newty.de/fpt/functor.html */
/**
 * @brief The CompareFunctor class serves as a functor superclass for specific implementations
 * of performing scalar comparisons
 */
class CompareFunctor
{
  public:
    virtual ~CompareFunctor() {}

    virtual bool operator()(int64_t index, int64_t neighIndex, int32_t gnum)  // call using () operator
    {
      return false;
    }
};

/**
 * @brief The TSpecificCompareFunctorBool class extends @see CompareFunctor to compare boolean data
 */
class TSpecificCompareFunctorBool : public CompareFunctor
{
  public:
    TSpecificCompareFunctorBool(void* data, int64_t length, bool tolerance, int32_t* featureIds) :
      m_Length(length),
      m_FeatureIds(featureIds)
    {
      m_Data = reinterpret_cast<bool*>(data);
    }
    virtual ~TSpecificCompareFunctorBool() {}

    virtual bool operator()(int64_t referencepoint, int64_t neighborpoint, int32_t gnum)
    {
      // Sanity check the indices that are being passed in.
      if (referencepoint >= m_Length || neighborpoint >= m_Length) { return false; }

      if ( m_Data[neighborpoint] == m_Data[referencepoint])
      {
        m_FeatureIds[neighborpoint] = gnum;
        return true;
      }
      return false;
    }

  protected:
    TSpecificCompareFunctorBool() {}

  private:
    bool* m_Data; // The data that is being compared
    int64_t m_Length; // Length of the Data Array
    int32_t* m_FeatureIds; // The Feature Ids
};

/**
 * @brief The TSpecificCompareFunctor class extens @see CompareFunctor to compare templated data
 */
template<class T>
class TSpecificCompareFunctor : public CompareFunctor
{
  public:
    TSpecificCompareFunctor(void* data, int64_t length, T tolerance, int32_t* featureIds) :
      m_Length(length),
      m_Tolerance(tolerance),
      m_FeatureIds(featureIds)
    {
      m_Data = reinterpret_cast<T*>(data);
    }
    virtual ~TSpecificCompareFunctor() {}

    virtual bool operator()(int64_t referencepoint, int64_t neighborpoint, int32_t gnum)
    {
      // Sanity check the indices that are being passed in.
      if (referencepoint >= m_Length || neighborpoint >= m_Length) { return false; }

      if(m_Data[referencepoint] >= m_Data[neighborpoint])
      {
        if ((m_Data[referencepoint] - m_Data[neighborpoint]) <= m_Tolerance)
        {
          m_FeatureIds[neighborpoint] = gnum;
          return true;
        }
      }
      else
      {
        if ((m_Data[neighborpoint] - m_Data[referencepoint]) <= m_Tolerance)
        {
          m_FeatureIds[neighborpoint] = gnum;
          return true;
        }
      }
      return false;
    }

  protected:
    TSpecificCompareFunctor() {}

  private:
    T* m_Data; // The data that is being compared
    int64_t m_Length; // Length of the Data Array
    T      m_Tolerance; // The tolerance of the comparison
    int32_t* m_FeatureIds; // The Feature Ids
};

// Include the MOC generated file for this class
#include "moc_ScalarSegmentFeatures.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ScalarSegmentFeatures::ScalarSegmentFeatures() :
  SegmentFeatures(),
  m_CellFeatureAttributeMatrixName(SIMPL::Defaults::CellFeatureAttributeMatrixName),
  m_ScalarArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, ""),
  m_ScalarTolerance(5.0f),
  m_RandomizeFeatureIds(true),
  m_UseGoodVoxels(true),
  m_GoodVoxelsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Mask),
  m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds),
  m_ActiveArrayName(SIMPL::FeatureData::Active),
  m_GoodVoxels(NULL),
  m_InputData(NULL),
  m_FeatureIds(NULL),
  m_Active(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ScalarSegmentFeatures::~ScalarSegmentFeatures()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;
  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(DoubleFilterParameter::New("Scalar Tolerance", "ScalarTolerance", getScalarTolerance(), FilterParameter::Parameter));
  parameters.push_back(LinkedBooleanFilterParameter::New("Use Mask Array", "UseGoodVoxels", getUseGoodVoxels(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, 1, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Scalar Array to Segment", "ScalarArrayPath", getScalarArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Bool, 1, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Mask", "GoodVoxelsArrayPath", getGoodVoxelsArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Feature Ids", "FeatureIdsArrayName", getFeatureIdsArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Feature Attribute Matrix", "CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Active", "ActiveArrayName", getActiveArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setActiveArrayName(reader->readString("ActiveArrayName", getActiveArrayName() ) );
  setCellFeatureAttributeMatrixName(reader->readString("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName() ) );
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName() ) );
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels() ) );
  setScalarArrayPath( reader->readDataArrayPath( "ScalarArrayPath", getScalarArrayPath() ) );
  setScalarTolerance( reader->readValue("ScalarTolerance", getScalarTolerance()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ScalarSegmentFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(ActiveArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(FeatureIdsArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(GoodVoxelsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(UseGoodVoxels)
  SIMPL_FILTER_WRITE_PARAMETER(ScalarArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(ScalarTolerance)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  // Set the DataContainerName for the Parent Class (SegmentFeatures) to Use
  setDataContainerName(m_ScalarArrayPath.getDataContainerName());

  SegmentFeatures::dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m.get()) { return; }

  QVector<size_t> tDims(1, 0);
  m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::CellFeature);

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 1);
  tempPath.update(getDataContainerName(), m_ScalarArrayPath.getAttributeMatrixName(), getFeatureIdsArrayName() );
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_InputDataPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getScalarArrayPath()); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_InputDataPtr.lock().get() )
  {
    m_InputData = m_InputDataPtr.lock()->getVoidPointer(0);
    if (m_InputDataPtr.lock()->getNumberOfComponents() != 1)
    {
      QString ss = QObject::tr("The selected array is not a scalar array. The number of components is %1").arg(m_InputDataPtr.lock()->getNumberOfComponents());
      setErrorCondition(-3011);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getScalarArrayPath()); }

  if(m_UseGoodVoxels == true)
  {
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getGoodVoxelsArrayPath()); }
  }

  tempPath.update(getDataContainerName(), getCellFeatureAttributeMatrixName(), getActiveArrayName() );
  m_ActivePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, true, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::preflight()
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
void ScalarSegmentFeatures::randomizeFeatureIds(int64_t totalPoints, int64_t totalFeatures)
{
  notifyStatusMessage(getHumanLabel(), "Randomizing Feature Ids");
  // Generate an even distribution of numbers between the min and max range
  const int64_t rangeMin = 1;
  const int64_t rangeMax = totalFeatures - 1;
  initializeVoxelSeedGenerator(rangeMin, rangeMax);

  // Get a reference variable to the Generator object
  Generator& numberGenerator = *m_NumberGenerator;

  DataArray<int64_t>::Pointer rndNumbers = DataArray<int64_t>::CreateArray(totalFeatures, "_INTERNAL_USE_ONLY_NewFeatureIds");

  int64_t* gid = rndNumbers->getPointer(0);
  gid[0] = 0;

  for (int64_t i = 1; i < totalFeatures; ++i)
  {
    gid[i] = i;
  }

  int64_t r = 0;
  int64_t temp = 0;

  //--- Shuffle elements by randomly exchanging each with one other.
  for (int64_t i = 1; i < totalFeatures; i++)
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
  for (int64_t i = 0; i < totalPoints; ++i)
  {
    m_FeatureIds[i] = gid[m_FeatureIds[i]];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t ScalarSegmentFeatures::getSeed(int32_t gnum, int64_t nextSeed)
{
  setErrorCondition(0);
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  int64_t seed = -1;
  // start with the next voxel after the last seed
  size_t randpoint = static_cast<size_t>(nextSeed);
  while (seed == -1 && randpoint < totalPoints)
  {
    if (m_FeatureIds[randpoint] == 0) // If the GrainId of the voxel is ZERO then we can use this as a seed point
    {
      if (m_UseGoodVoxels == false || m_GoodVoxels[randpoint] == true)
      {
        seed = randpoint;
      }
      else { randpoint += 1; }
    }
    else { randpoint += 1; }
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
bool ScalarSegmentFeatures::determineGrouping(int64_t referencepoint, int64_t neighborpoint, int32_t gnum)
{
  if (m_FeatureIds[neighborpoint] == 0 && (m_UseGoodVoxels == false || m_GoodVoxels[neighborpoint] == true))
  {
    CompareFunctor* func = m_Compare.get();
    return (*func)( (size_t)(referencepoint), (size_t)(neighborpoint), gnum );
    //     | Functor  ||calling the operator() method of the CompareFunctor Class |
  }
  else
  {
    return false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::initializeVoxelSeedGenerator(const int64_t rangeMin, const int64_t rangeMax)
{
  m_Distribution = std::shared_ptr<NumberDistribution>(new NumberDistribution(rangeMin, rangeMax));
  m_RandomNumberGenerator = std::shared_ptr<RandomNumberGenerator>(new RandomNumberGenerator);
  m_NumberGenerator = std::shared_ptr<Generator>(new Generator(*m_RandomNumberGenerator, *m_Distribution));
  m_RandomNumberGenerator->seed(static_cast<size_t>( QDateTime::currentMSecsSinceEpoch() )); // seed with the current time
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  QVector<size_t> tDims(1, 1);
  m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateFeatureInstancePointers();

  int64_t totalPoints = static_cast<int64_t>(m_FeatureIdsPtr.lock()->getNumberOfTuples());
  int64_t inDataPoints = static_cast<int64_t>(m_InputDataPtr.lock()->getNumberOfTuples());

  QString dType = m_InputDataPtr.lock()->getTypeAsString();
  if (m_InputDataPtr.lock()->getNumberOfComponents() != 1)
  {
    m_Compare = std::shared_ptr<CompareFunctor>(new CompareFunctor()); // The default CompareFunctor which ALWAYS returns false for the comparison
  }
  else if (dType.compare("int8_t") == 0)
  {
    m_Compare = std::shared_ptr<TSpecificCompareFunctor<int8_t> >(new TSpecificCompareFunctor<int8_t>(m_InputData, inDataPoints, m_ScalarTolerance, m_FeatureIds));
  }
  else if (dType.compare("uint8_t") == 0)
  {
    m_Compare = std::shared_ptr<TSpecificCompareFunctor<uint8_t> >(new TSpecificCompareFunctor<uint8_t>(m_InputData, inDataPoints, m_ScalarTolerance, m_FeatureIds));
  }
  else if (dType.compare("bool") == 0)
  {
    m_Compare = std::shared_ptr<TSpecificCompareFunctorBool>(new TSpecificCompareFunctorBool(m_InputData, inDataPoints, m_ScalarTolerance, m_FeatureIds));
  }
  else if (dType.compare("int16_t") == 0)
  {
    m_Compare = std::shared_ptr<TSpecificCompareFunctor<int16_t> >(new TSpecificCompareFunctor<int16_t>(m_InputData, inDataPoints, m_ScalarTolerance, m_FeatureIds));
  }
  else if (dType.compare("uint16_t") == 0)
  {
    m_Compare = std::shared_ptr<TSpecificCompareFunctor<uint16_t> >(new TSpecificCompareFunctor<uint16_t>(m_InputData, inDataPoints, m_ScalarTolerance, m_FeatureIds));
  }
  else if (dType.compare("int32_t") == 0)
  {
    m_Compare = std::shared_ptr<TSpecificCompareFunctor<int32_t> >(new TSpecificCompareFunctor<int32_t>(m_InputData, inDataPoints, m_ScalarTolerance, m_FeatureIds));
  }
  else if (dType.compare("uint32_t") == 0)
  {
    m_Compare = std::shared_ptr<TSpecificCompareFunctor<uint32_t> >(new TSpecificCompareFunctor<uint32_t>(m_InputData, inDataPoints, m_ScalarTolerance, m_FeatureIds));
  }
  else if (dType.compare("int64_t") == 0)
  {
    m_Compare = std::shared_ptr<TSpecificCompareFunctor<int64_t> >(new TSpecificCompareFunctor<int64_t>(m_InputData, inDataPoints, m_ScalarTolerance, m_FeatureIds));
  }
  else if (dType.compare("uint64_t") == 0)
  {
    m_Compare = std::shared_ptr<TSpecificCompareFunctor<uint64_t> >(new TSpecificCompareFunctor<uint64_t>(m_InputData, inDataPoints, m_ScalarTolerance, m_FeatureIds));
  }
  else if (dType.compare("float") == 0)
  {
    m_Compare = std::shared_ptr<TSpecificCompareFunctor<float> >(new TSpecificCompareFunctor<float>(m_InputData, inDataPoints, m_ScalarTolerance, m_FeatureIds));
  }
  else if (dType.compare("double") == 0)
  {
    m_Compare = std::shared_ptr<TSpecificCompareFunctor<double> >(new TSpecificCompareFunctor<double>(m_InputData, inDataPoints, m_ScalarTolerance, m_FeatureIds));
  }

  // Generate the random voxel indices that will be used for the seed points to start a new grain growth/agglomeration
  const int64_t rangeMin = 0;
  const int64_t rangeMax = totalPoints - 1;
  initializeVoxelSeedGenerator(rangeMin, rangeMax);

  SegmentFeatures::execute();

  int64_t totalFeatures = static_cast<int64_t>(m_ActivePtr.lock()->getNumberOfTuples());
  if (totalFeatures < 2)
  {
    setErrorCondition(-87000);
    notifyErrorMessage(getHumanLabel(), "The number of Features was 0 or 1 which means no Features were detected. A threshold value may be set too high", getErrorCondition());
    return;
  }

  // By default we randomize grains
  if (true == m_RandomizeFeatureIds)
  {
    totalPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getNumberOfElements());
    randomizeFeatureIds(totalPoints, totalFeatures);
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ScalarSegmentFeatures::newFilterInstance(bool copyFilterParameters)
{
  ScalarSegmentFeatures::Pointer filter = ScalarSegmentFeatures::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ScalarSegmentFeatures::getCompiledLibraryName()
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ScalarSegmentFeatures::getBrandingString()
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ScalarSegmentFeatures::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ScalarSegmentFeatures::getGroupName()
{ return SIMPL::FilterGroups::ReconstructionFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ScalarSegmentFeatures::getSubGroupName()
{ return SIMPL::FilterSubGroups::SegmentationFilters;}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ScalarSegmentFeatures::getHumanLabel()
{ return "Segment Features (Scalar)"; }
