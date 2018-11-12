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

#include "FindNeighborhoods.h"

#include <mutex>

#include <QtCore/QDateTime>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

class FindNeighborhoodsImpl
{
public:
  FindNeighborhoodsImpl(FindNeighborhoods* filter, size_t totalFeatures, float* centroids, const std::vector<int64_t>& bins, const std::vector<float>& criticalDistance)
  : m_Filter(filter)
  , m_TotalFeatures(totalFeatures)
  , m_Centroids(centroids)
  , m_Bins(bins)
  , m_CriticalDistance(criticalDistance)
  {
  }

  void convert(size_t start, size_t end) const
  {
    float x = 0.0f, y = 0.0f, z = 0.0f;
    int64_t bin1x = 0, bin2x = 0, bin1y = 0, bin2y = 0, bin1z = 0, bin2z = 0;
    float dBinX = 0, dBinY = 0, dBinZ = 0;
    float criticalDistance1 = 0, criticalDistance2 = 0;

    size_t increment = (end - start) / 100;
    size_t incCount = 0;
    // NEVER start at 0.
    if(start == 0)
    {
      start = 1;
    }
    for(size_t i = start; i < end; i++)
    {
      incCount++;
      if(incCount == increment || i == end - 1)
      {
        incCount = 0;
        m_Filter->updateProgress(increment, m_TotalFeatures);
      }
      if(m_Filter->getCancel())
      {
        break;
      }
      x = m_Centroids[3 * i];
      y = m_Centroids[3 * i + 1];
      z = m_Centroids[3 * i + 2];
      bin1x = m_Bins[3 * i];
      bin1y = m_Bins[3 * i + 1];
      bin1z = m_Bins[3 * i + 2];
      criticalDistance1 = m_CriticalDistance[i];

      for(size_t j = i + 1; j < m_TotalFeatures; j++)
      {
        bin2x = m_Bins[3 * j];
        bin2y = m_Bins[3 * j + 1];
        bin2z = m_Bins[3 * j + 2];
        criticalDistance2 = m_CriticalDistance[j];
        // Use the llabs version of the "C" abs function because we are using int64_t
        // do NOT try to use the std::abs() function as this is C++11 ONLY
        dBinX = llabs(bin2x - bin1x);
        dBinY = llabs(bin2y - bin1y);
        dBinZ = llabs(bin2z - bin1z);

        if(dBinX < criticalDistance1 && dBinY < criticalDistance1 && dBinZ < criticalDistance1)
        {
          m_Filter->updateNeighborHood(i, j);
        }

        if(dBinX < criticalDistance2 && dBinY < criticalDistance2 && dBinZ < criticalDistance2)
        {
          m_Filter->updateNeighborHood(j, i);
        }
      }
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    convert(r.begin(), r.end());
  }
#endif

private:
  FindNeighborhoods* m_Filter = nullptr;
  size_t m_TotalFeatures = 0;
  float* m_Centroids = nullptr;
  const std::vector<int64_t>& m_Bins;
  const std::vector<float>& m_CriticalDistance;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighborhoods::FindNeighborhoods()
: m_NeighborhoodListArrayName(SIMPL::FeatureData::NeighborhoodList)
, m_MultiplesOfAverage(1.0f)
, m_EquivalentDiametersArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::EquivalentDiameters)
, m_FeaturePhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases)
, m_CentroidsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Centroids)
, m_NeighborhoodsArrayName(SIMPL::FeatureData::Neighborhoods)
, m_FeaturePhases(nullptr)
, m_Centroids(nullptr)
, m_EquivalentDiameters(nullptr)
, m_Neighborhoods(nullptr)
{
  m_NeighborhoodList = NeighborList<int32_t>::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighborhoods::~FindNeighborhoods() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Multiples of Average Diameter", MultiplesOfAverage, FilterParameter::Parameter, FindNeighborhoods));
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 1, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Equivalent Diameters", EquivalentDiametersArrayPath, FilterParameter::RequiredArray, FindNeighborhoods, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, FindNeighborhoods, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Centroids", CentroidsArrayPath, FilterParameter::RequiredArray, FindNeighborhoods, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Neighborhoods", NeighborhoodsArrayName, FilterParameter::CreatedArray, FindNeighborhoods));
  parameters.push_back(SIMPL_NEW_STRING_FP("Neighborhood List", NeighborhoodListArrayName, FilterParameter::CreatedArray, FindNeighborhoods));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindNeighborhoods::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNeighborhoodsArrayName(reader->readString("NeighborhoodsArrayName", getNeighborhoodsArrayName()));
  setNeighborhoodListArrayName(reader->readString("NeighborhoodListArrayName", getNeighborhoodListArrayName()));
  setCentroidsArrayPath(reader->readDataArrayPath("CentroidsArrayPath", getCentroidsArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setEquivalentDiametersArrayPath(reader->readDataArrayPath("EquivalentDiametersArrayPath", getEquivalentDiametersArrayPath()));
  setMultiplesOfAverage(reader->readValue("MultiplesOfAverage", getMultiplesOfAverage()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::initialize()
{
  m_NeighborhoodList = NeighborList<int32_t>::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  initialize();
  DataArrayPath tempPath;

  QVector<DataArrayPath> dataArrayPaths;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getEquivalentDiametersArrayPath().getDataContainerName());

  // Feature Data
  // Do this whole block FIRST otherwise the side effect is that a call to m->getNumCellFeatureTuples will = 0
  // because we are just creating an empty NeighborList object.
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  QVector<size_t> cDims(1, 1);
  tempPath.update(m_EquivalentDiametersArrayPath.getDataContainerName(), m_EquivalentDiametersArrayPath.getAttributeMatrixName(), getNeighborhoodListArrayName());
  m_NeighborhoodList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */

  m_EquivalentDiametersPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getEquivalentDiametersArrayPath(),
                                                                                                               cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_EquivalentDiametersPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getEquivalentDiametersArrayPath());
  }

  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getFeaturePhasesArrayPath());
  }

  tempPath.update(m_EquivalentDiametersArrayPath.getDataContainerName(), m_EquivalentDiametersArrayPath.getAttributeMatrixName(), getNeighborhoodsArrayName());
  m_NeighborhoodsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims);           /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_NeighborhoodsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Neighborhoods = m_NeighborhoodsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  m_CentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCentroidsArrayPath(),
                                                                                                     cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CentroidsPtr.lock())                                                                       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Centroids = m_CentroidsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getCentroidsArrayPath());
  }

  getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::preflight()
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
void FindNeighborhoods::find_neighborhoods()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }
  m_IncCount = 0;

  float x = 0.0f, y = 0.0f, z = 0.0f;
  m_NumCompleted = 0;
  std::vector<float> criticalDistance;

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_EquivalentDiametersArrayPath.getDataContainerName());
  size_t totalFeatures = m_EquivalentDiametersPtr.lock()->getNumberOfTuples();

  m_ProgIncrement = totalFeatures / 100;

  m_LocalNeighborhoodList.resize(totalFeatures);
  criticalDistance.resize(totalFeatures);

  float aveDiam = 0.0f;
  for(size_t i = 1; i < totalFeatures; i++)
  {
    m_Neighborhoods[i] = 0;
    aveDiam += m_EquivalentDiameters[i];
    criticalDistance[i] = m_EquivalentDiameters[i] * m_MultiplesOfAverage;
  }
  aveDiam /= totalFeatures;
  for(size_t i = 1; i < totalFeatures; i++)
  {
    criticalDistance[i] /= aveDiam;
  }

  float m_OriginX = 0.0f, m_OriginY = 0.0f, m_OriginZ = 0.0f;
  m->getGeometryAs<ImageGeom>()->getOrigin(m_OriginX, m_OriginY, m_OriginZ);
  size_t udims[3] = {0, 0, 0};
  std::tie(udims[0], udims[1], udims[2]) = m->getGeometryAs<ImageGeom>()->getDimensions();

  size_t xbin = 0, ybin = 0, zbin = 0;
  std::vector<int64_t> bins(3 * totalFeatures, 0);
  for(size_t i = 1; i < totalFeatures; i++)
  {
    x = m_Centroids[3 * i];
    y = m_Centroids[3 * i + 1];
    z = m_Centroids[3 * i + 2];
    xbin = static_cast<size_t>((x - m_OriginX) / aveDiam);
    ybin = static_cast<size_t>((y - m_OriginY) / aveDiam);
    zbin = static_cast<size_t>((z - m_OriginZ) / aveDiam);
    bins[3 * i] = static_cast<int64_t>(xbin);
    bins[3 * i + 1] = static_cast<int64_t>(ybin);
    bins[3 * i + 2] = static_cast<int64_t>(zbin);
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalFeatures), FindNeighborhoodsImpl(this, totalFeatures, m_Centroids, bins, criticalDistance), tbb::auto_partitioner());
  }
  else
#endif
  {
    FindNeighborhoodsImpl serial(this, totalFeatures, m_Centroids, bins, criticalDistance);
    serial.convert(0, totalFeatures);
  }

  for(size_t i = 1; i < totalFeatures; i++)
  {
    // Set the vector for each list into the NeighborhoodList Object
    NeighborList<int32_t>::SharedVectorType sharedNeiLst(new std::vector<int32_t>);
    sharedNeiLst->assign(m_LocalNeighborhoodList[i].begin(), m_LocalNeighborhoodList[i].end());
    m_NeighborhoodList.lock()->setList(static_cast<int32_t>(i), sharedNeiLst);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::updateNeighborHood(size_t sourceIndex, size_t destIndex)
{
  static std::mutex mutex;
  std::lock_guard<std::mutex> lock(mutex);
  m_Neighborhoods[sourceIndex]++;
  m_LocalNeighborhoodList[sourceIndex].push_back(static_cast<int32_t>(destIndex));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::updateProgress(size_t numCompleted, size_t totalFeatures)
{
  static std::mutex mutex;
  std::lock_guard<std::mutex> lock(mutex);
  m_IncCount += numCompleted;
  m_NumCompleted = m_NumCompleted + numCompleted;
  if(m_IncCount > m_ProgIncrement)
  {
    m_IncCount = 0;
    QString ss = QObject::tr("Working on Feature %1 of %2").arg(m_NumCompleted).arg(totalFeatures);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindNeighborhoods::newFilterInstance(bool copyFilterParameters) const
{
  FindNeighborhoods::Pointer filter = FindNeighborhoods::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighborhoods::getCompiledLibraryName() const
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighborhoods::getBrandingString() const
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighborhoods::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighborhoods::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid FindNeighborhoods::getUuid()
{
  return QUuid("{697ed3de-db33-5dd1-a64b-04fb71e7d63e}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighborhoods::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighborhoods::getHumanLabel() const
{
  return "Find Feature Neighborhoods";
}
