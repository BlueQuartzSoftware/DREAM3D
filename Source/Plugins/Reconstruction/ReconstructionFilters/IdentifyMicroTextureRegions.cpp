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

#include "IdentifyMicroTextureRegions.h"

#include <QtCore/QDateTime>

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/atomic.h>
#include <tbb/tick_count.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"

#include "EbsdLib/EbsdConstants.h"


// included so we can call under the hood to segment the patches found in this filter
#include "Reconstruction/ReconstructionFilters/VectorSegmentFeatures.h"

#include "Reconstruction/ReconstructionConstants.h"
#include "Reconstruction/ReconstructionVersion.h"

/**
 * @brief The FindPatchMisalignmentsImpl class implements a threaded algorithm that determines the misorientations
 * between for all cell faces in the structure
 */
class FindPatchMisalignmentsImpl
{
  public:
    FindPatchMisalignmentsImpl(int64_t* newDims, int64_t* origDims, float* caxisLocs, int32_t* phases, uint32_t* crystructs, float* volFrac, float* avgCAxis, bool* inMTR, int64_t* critDim, float minVolFrac, float caxisTol) :
      m_DicDims(newDims),
      m_VolDims(origDims),
      m_CAxisLocations(caxisLocs),
      m_CellPhases(phases),
      m_CrystalStructures(crystructs),
      m_InMTR(inMTR),
      m_VolFrac(volFrac),
      m_AvgCAxis(avgCAxis),
      m_CritDim(critDim),
      m_MinVolFrac(minVolFrac),
      m_CAxisTolerance(caxisTol)
    {
    }
    virtual ~FindPatchMisalignmentsImpl() {}

    void convert(size_t start, size_t end) const
    {
      int64_t xDim = (2 * m_CritDim[0]) + 1;
      int64_t yDim = (2 * m_CritDim[1]) + 1;
      int64_t zDim = (2 * m_CritDim[2]) + 1;
      QVector<size_t> tDims(1, xDim * yDim * zDim);
      QVector<size_t> cDims(1, 3);
      FloatArrayType::Pointer cAxisLocsPtr = FloatArrayType::CreateArray(tDims, cDims, "_INTERNAL_USE_ONLY_cAxisLocs");
      cAxisLocsPtr->initializeWithValue(0);
      float* cAxisLocs = cAxisLocsPtr->getPointer(0);
      std::vector<int64_t> goodCounts;

      int64_t xc = 0, yc = 0, zc = 0;
      for (size_t iter = start; iter < end; iter++)
      {
        int64_t zStride = 0, yStride = 0;
        int64_t count = 0;
        xc = ((iter % m_DicDims[0]) * m_CritDim[0]) + (m_CritDim[0] / 2);
        yc = (((iter / m_DicDims[0]) % m_DicDims[1]) * m_CritDim[1]) + (m_CritDim[1] / 2);
        zc = ((iter / (m_DicDims[0] * m_DicDims[1])) * m_CritDim[2]) + (m_CritDim[2] / 2);
        for (int64_t k = -m_CritDim[2]; k <= m_CritDim[2]; k++)
        {
          if ((zc + k) >= 0 && (zc + k) < m_VolDims[2])
          {
            zStride = ((zc + k) * m_VolDims[0] * m_VolDims[1]);
            for (int64_t j = -m_CritDim[1]; j <= m_CritDim[1]; j++)
            {
              if ((yc + j) >= 0 && (yc + j) < m_VolDims[1])
              {
                yStride = ((yc + j) * m_VolDims[0]);
                for (int64_t i = -m_CritDim[0]; i <= m_CritDim[0]; i++)
                {
                  if ((xc + i) >= 0 && (xc + i) < m_VolDims[0])
                  {
                    if (m_CrystalStructures[m_CellPhases[(zStride + yStride + xc + i)]] == Ebsd::CrystalStructure::Hexagonal_High)
                    {
                      cAxisLocs[3 * count + 0] = m_CAxisLocations[3 * (zStride + yStride + xc + i) + 0];
                      cAxisLocs[3 * count + 1] = m_CAxisLocations[3 * (zStride + yStride + xc + i) + 1];
                      cAxisLocs[3 * count + 2] = m_CAxisLocations[3 * (zStride + yStride + xc + i) + 2];
                      count++;
                    }
                  }
                }
              }
            }
          }
        }
        float angle = 0.0f;
        goodCounts.resize(count);
        goodCounts.assign(count, 0);
        for (int64_t i = 0; i < count; i++)
        {
          for (int64_t j = i; j < count; j++)
          {
            angle = GeometryMath::AngleBetweenVectors(cAxisLocsPtr->getPointer(3 * i), cAxisLocsPtr->getPointer(3 * j));
            if (angle <= m_CAxisTolerance || (SIMPLib::Constants::k_Pi - angle) <= m_CAxisTolerance)
            {
              goodCounts[i]++;
              goodCounts[j]++;
            }
          }
        }
        int64_t goodPointCount = 0;
        for (int64_t i = 0; i < count; i++)
        {
          if (float(goodCounts[i]) / float(count) > m_MinVolFrac) { goodPointCount++; }
        }
        float avgCAxis[3] = { 0.0f, 0.0f, 0.0f };
        float frac = float(goodPointCount) / float(count);
        m_VolFrac[iter] = frac;
        if (frac > m_MinVolFrac)
        {
          m_InMTR[iter] = true;
          for (int64_t i = 0; i < count; i++)
          {
            if (float(goodCounts[i]) / float(count) >= m_MinVolFrac)
            {
              if (MatrixMath::DotProduct3x1(avgCAxis, cAxisLocsPtr->getPointer(3 * i)) < 0)
              {
                avgCAxis[0] -= cAxisLocs[3 * i];
                avgCAxis[1] -= cAxisLocs[3 * i + 1];
                avgCAxis[2] -= cAxisLocs[3 * i + 2];
              }
              else
              {
                avgCAxis[0] += cAxisLocs[3 * i];
                avgCAxis[1] += cAxisLocs[3 * i + 1];
                avgCAxis[2] += cAxisLocs[3 * i + 2];
              }
            }
          }
          MatrixMath::Normalize3x1(avgCAxis);
          if (avgCAxis[2] < 0) { MatrixMath::Multiply3x1withConstant(avgCAxis, -1); }
          m_AvgCAxis[3 * iter] = avgCAxis[0];
          m_AvgCAxis[3 * iter + 1] = avgCAxis[1];
          m_AvgCAxis[3 * iter + 2] = avgCAxis[2];
        }
      }
    }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      convert(r.begin(), r.end());
    }
#endif
  private:
    int64_t* m_DicDims;
    int64_t* m_VolDims;
    float* m_CAxisLocations;
    int32_t* m_CellPhases;
    uint32_t* m_CrystalStructures;
    bool* m_InMTR;
    float* m_VolFrac;
    float* m_AvgCAxis;
    int64_t* m_CritDim;
    float m_MinVolFrac;
    float m_CAxisTolerance;
};

// Include the MOC generated file for this class
#include "moc_IdentifyMicroTextureRegions.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IdentifyMicroTextureRegions::IdentifyMicroTextureRegions() :
  AbstractFilter(),
  m_NewCellFeatureAttributeMatrixName(SIMPL::Defaults::NewCellFeatureAttributeMatrixName),
  m_CAxisTolerance(1.0f),
  m_MinMTRSize(1.0f),
  m_MinVolFrac(1.0f),
  m_RandomizeMTRIds(false),
  m_CAxisLocationsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::CAxisLocation),
  m_CellPhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases),
  m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures),
  m_MTRIdsArrayName(SIMPL::CellData::ParentIds),
  m_ActiveArrayName(SIMPL::FeatureData::Active),
  m_CAxisLocations(NULL),
  m_CellPhases(NULL),
  m_CrystalStructures(NULL),
  m_MTRIds(NULL),
  m_Active(NULL),
  m_InMTR(NULL),
  m_VolFrac(NULL),
  m_AvgCAxis(NULL),
  m_PatchIds(NULL),
  m_PatchActive(NULL)
{
  m_CAxisToleranceRad = 0.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IdentifyMicroTextureRegions::~IdentifyMicroTextureRegions()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(DoubleFilterParameter::New("C-Axis Alignment Tolerance (Degrees)", "CAxisTolerance", getCAxisTolerance(), FilterParameter::Parameter, SIMPL_BIND_SETTER(IdentifyMicroTextureRegions, this, CAxisTolerance), SIMPL_BIND_GETTER(IdentifyMicroTextureRegions, this, CAxisTolerance)));
  parameters.push_back(DoubleFilterParameter::New("Minimum MicroTextured Region Size (Diameter)", "MinMTRSize", getMinMTRSize(), FilterParameter::Parameter, SIMPL_BIND_SETTER(IdentifyMicroTextureRegions, this, MinMTRSize), SIMPL_BIND_GETTER(IdentifyMicroTextureRegions, this, MinMTRSize)));
  parameters.push_back(DoubleFilterParameter::New("Minimum Volume Fraction in MTR", "MinVolFrac", getMinVolFrac(), FilterParameter::Parameter, SIMPL_BIND_SETTER(IdentifyMicroTextureRegions, this, MinVolFrac), SIMPL_BIND_GETTER(IdentifyMicroTextureRegions, this, MinVolFrac)));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(DataArraySelectionFilterParameter::New("C-Axis Locations", "CAxisLocationsArrayPath", getCAxisLocationsArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(IdentifyMicroTextureRegions, this, CAxisLocationsArrayPath), SIMPL_BIND_GETTER(IdentifyMicroTextureRegions, this, CAxisLocationsArrayPath)));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(DataArraySelectionFilterParameter::New("Cell Phases", "CellPhasesArrayPath", getCellPhasesArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(IdentifyMicroTextureRegions, this, CellPhasesArrayPath), SIMPL_BIND_GETTER(IdentifyMicroTextureRegions, this, CellPhasesArrayPath)));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(IdentifyMicroTextureRegions, this, CrystalStructuresArrayPath), SIMPL_BIND_GETTER(IdentifyMicroTextureRegions, this, CrystalStructuresArrayPath)));
  }

  parameters.push_back(StringFilterParameter::New("MTR Ids", "MTRIdsArrayName", getMTRIdsArrayName(), FilterParameter::CreatedArray, SIMPL_BIND_SETTER(IdentifyMicroTextureRegions, this, MTRIdsArrayName), SIMPL_BIND_GETTER(IdentifyMicroTextureRegions, this, MTRIdsArrayName)));
  parameters.push_back(StringFilterParameter::New("New Cell Feature Attribute Matrix Name", "NewCellFeatureAttributeMatrixName", getNewCellFeatureAttributeMatrixName(), FilterParameter::CreatedArray, SIMPL_BIND_SETTER(IdentifyMicroTextureRegions, this, NewCellFeatureAttributeMatrixName), SIMPL_BIND_GETTER(IdentifyMicroTextureRegions, this, NewCellFeatureAttributeMatrixName)));
  parameters.push_back(StringFilterParameter::New("Active", "ActiveArrayName", getActiveArrayName(), FilterParameter::CreatedArray, SIMPL_BIND_SETTER(IdentifyMicroTextureRegions, this, ActiveArrayName), SIMPL_BIND_GETTER(IdentifyMicroTextureRegions, this, ActiveArrayName)));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setActiveArrayName(reader->readString("ActiveArrayName", getActiveArrayName() ) );
  setNewCellFeatureAttributeMatrixName(reader->readString("NewCellFeatureAttributeMatrixName", getNewCellFeatureAttributeMatrixName() ) );
  setMTRIdsArrayName(reader->readString("MTRIdsArrayName", getMTRIdsArrayName() ) );
  setCAxisLocationsArrayPath(reader->readDataArrayPath("CAxisLocationsArrayPath", getCAxisLocationsArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setCAxisTolerance( reader->readValue("CAxisTolerance", getCAxisTolerance()) );
  setMinMTRSize( reader->readValue("MinMTRSize", getMinMTRSize()) );
  setMinVolFrac( reader->readValue("MinVolFrac", getMinVolFrac()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::initialize()
{
  m_CAxisToleranceRad = 0.0f;
  m_TotalRandomNumbersGenerated = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::dataCheck()
{
  setErrorCondition(0);
  initialize();

  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getCAxisLocationsArrayPath().getDataContainerName());

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getCAxisLocationsArrayPath().getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m.get()) { return; }

  QVector<size_t> tDims(1, 0);
  m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getNewCellFeatureAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::CellFeature);

  QVector<size_t> cDims(1, 3);

  QVector<DataArrayPath> dataArrayPaths;

  // Cell Data
  m_CAxisLocationsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCAxisLocationsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CAxisLocationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CAxisLocations = m_CAxisLocationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getCAxisLocationsArrayPath()); }

  cDims[0] = 1;
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getCellPhasesArrayPath()); }

  tempPath.update(m_CAxisLocationsArrayPath.getDataContainerName(), getCAxisLocationsArrayPath().getAttributeMatrixName(), getMTRIdsArrayName() );
  m_MTRIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_MTRIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_MTRIds = m_MTRIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // New Feature Data
  tempPath.update(m_CAxisLocationsArrayPath.getDataContainerName(), getNewCellFeatureAttributeMatrixName(), getActiveArrayName() );
  m_ActivePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, true, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Ensemble Data
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::preflight()
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
void IdentifyMicroTextureRegions::randomizeFeatureIds(int64_t totalPoints, int64_t totalFeatures)
{
  notifyStatusMessage(getHumanLabel(), "Randomizing Feature Ids");
  // Generate an even distribution of numbers between the min and max range
  const int32_t rangeMin = 1;
  const int32_t rangeMax = totalFeatures - 1;
  initializeVoxelSeedGenerator(rangeMin, rangeMax);

  // Get a reference variable to the Generator object
  Generator& numberGenerator = *m_NumberGenerator;

  DataArray<int32_t>::Pointer rndNumbers = DataArray<int32_t>::CreateArray(totalFeatures, "_INTERNAL_USE_ONLY_NewFeatureIds");

  int32_t* gid = rndNumbers->getPointer(0);
  gid[0] = 0;
  for (int64_t i = 1; i < totalFeatures; ++i)
  {
    gid[i] = i;
  }

  int32_t r = 0;
  int32_t temp = 0;

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
    //m_MTRIds[i] = gid[ m_MTRIds[i] ];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::findMTRregions()
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::initializeVoxelSeedGenerator(const int32_t rangeMin, const int32_t rangeMax)
{
  // The way we are using the boost random number generators is that we are asking for a NumberDistribution (see the typedef)
  // to guarantee the numbers are betwee a specific range and will only be generated once. We also keep a tally of the
  // total number of numbers generated as a way to make sure the while loops eventually terminate. This setup should
  // make sure that every voxel can be a seed point.
  m_Distribution = std::shared_ptr<NumberDistribution>(new NumberDistribution(rangeMin, rangeMax));
  m_RandomNumberGenerator = std::shared_ptr<RandomNumberGenerator>(new RandomNumberGenerator);
  m_NumberGenerator = std::shared_ptr<Generator>(new Generator(*m_RandomNumberGenerator, *m_Distribution));
  m_RandomNumberGenerator->seed(static_cast<size_t>( QDateTime::currentMSecsSinceEpoch() )); // seed with the current time
  m_TotalRandomNumbersGenerated = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCAxisLocationsArrayPath().getDataContainerName());
  int64_t totalPoints = static_cast<int64_t>(m_MTRIdsPtr.lock()->getNumberOfTuples());

  // calculate dimensions of DIC-like grid
  size_t dcDims[3] = { 0, 0, 0 };
  float xRes = 0.0f, yRes = 0.0f, zRes = 0.0f;
  float origin[3] = { 0.0f, 0.0f, 0.0f };
  m->getGeometryAs<ImageGeom>()->getDimensions(dcDims[0], dcDims[1], dcDims[2]);
  m->getGeometryAs<ImageGeom>()->getResolution(xRes, yRes, zRes);
  m->getGeometryAs<ImageGeom>()->getOrigin(origin);

  // Find number of original cells in radius of patch
  int64_t critDim[3] = { 0, 0, 0 };
  critDim[0] = static_cast<int64_t>(m_MinMTRSize / (4.0f * xRes));
  critDim[1] = static_cast<int64_t>(m_MinMTRSize / (4.0f * yRes));
  critDim[2] = static_cast<int64_t>(m_MinMTRSize / (4.0f * zRes));

  // Find physical distance of patch steps
  FloatVec3_t critRes;
  critRes.x = static_cast<float>(critDim[0]) * xRes;
  critRes.y = static_cast<float>(critDim[1]) * yRes;
  critRes.z = static_cast<float>(critDim[2]) * zRes;

  // Find number of patch steps in each dimension
  int64_t newDimX = static_cast<int64_t>(dcDims[0] / critDim[0]);
  int64_t newDimY = static_cast<int64_t>(dcDims[1] / critDim[1]);
  int64_t newDimZ = static_cast<int64_t>(dcDims[2] / critDim[2]);
  if (dcDims[0] == 1) { newDimX = 1, critDim[0] = 0; }
  if (dcDims[1] == 1) { newDimY = 1, critDim[1] = 0; }
  if (dcDims[2] == 1) { newDimZ = 1, critDim[2] = 0; }

  // Store the original and patch dimensions for passing into the parallel algo below
  int64_t origDims[3] = { 0, 0, 0 };
  origDims[0] = dcDims[0];
  origDims[1] = dcDims[1];
  origDims[2] = dcDims[2];
  int64_t newDims[3] = { 0, 0, 0 };
  newDims[0] = newDimX;
  newDims[1] = newDimY;
  newDims[2] = newDimZ;
  size_t totalPatches = static_cast<size_t>(newDimX * newDimY * newDimZ);

  // Create temporary DataContainer and AttributeMatrix for holding the patch data
  DataContainer::Pointer tmpDC = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, "_INTERNAL_USE_ONLY_PatchDataContainer(Temp)");
  if(getErrorCondition() < 0) { return; }
  tmpDC->getGeometryAs<ImageGeom>()->setDimensions(static_cast<size_t>(newDimX), static_cast<size_t>(newDimY), static_cast<size_t>(newDimZ));
  tmpDC->getGeometryAs<ImageGeom>()->setResolution(critRes.x, critRes.y, critRes.z);
  tmpDC->getGeometryAs<ImageGeom>()->setOrigin(origin[0], origin[1], origin[2]);

  QVector<size_t> tDims(3, 0);
  tDims[0] = newDimX;
  tDims[1] = newDimY;
  tDims[2] = newDimZ;
  tmpDC->createNonPrereqAttributeMatrix<AbstractFilter>(this, "_INTERNAL_USE_ONLY_PatchAM(Temp)", tDims, SIMPL::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  DataArrayPath tempPath;
  tDims[0] = totalPatches;
  QVector<size_t> cDims(1, 1);
  tempPath.update("_INTERNAL_USE_ONLY_PatchDataContainer(Temp)", "_INTERNAL_USE_ONLY_PatchAM(Temp)", "_INTERNAL_USE_ONLY_InMTR");
  m_InMTRPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_InMTRPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_InMTR = m_InMTRPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update("_INTERNAL_USE_ONLY_PatchDataContainer(Temp)", "_INTERNAL_USE_ONLY_PatchAM(Temp)", "_INTERNAL_USE_ONLY_VolFrac");
  m_VolFracPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_VolFracPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_VolFrac = m_VolFracPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  tempPath.update("_INTERNAL_USE_ONLY_PatchDataContainer(Temp)", "_INTERNAL_USE_ONLY_PatchAM(Temp)", "_INTERNAL_USE_ONLY_AvgCAxis");
  m_AvgCAxisPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgCAxisPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgCAxis = m_AvgCAxisPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Convert user defined tolerance to radians.
  m_CAxisToleranceRad = m_CAxisTolerance * SIMPLib::Constants::k_Pi / 180.0f;

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

// first determine the misorientation vectors on all the voxel faces
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPatches),
                      FindPatchMisalignmentsImpl(newDims, origDims, m_CAxisLocations, m_CellPhases, m_CrystalStructures, m_VolFrac, m_AvgCAxis, m_InMTR, critDim, m_MinVolFrac, m_CAxisToleranceRad), tbb::auto_partitioner());

  }
  else
#endif
  {
    FindPatchMisalignmentsImpl serial(newDims, origDims, m_CAxisLocations, m_CellPhases, m_CrystalStructures, m_VolFrac, m_AvgCAxis, m_InMTR, critDim, m_MinVolFrac, m_CAxisToleranceRad);
    serial.convert(0, totalPatches);
  }

  // Call the SegmentFeatures(Vector) filter under the hood to segment the patches based on average c-axis of the patch
  VectorSegmentFeatures::Pointer filter = VectorSegmentFeatures::New();
  filter->setDataContainerArray(getDataContainerArray());
  tempPath.update("_INTERNAL_USE_ONLY_PatchDataContainer(Temp)", "_INTERNAL_USE_ONLY_PatchAM(Temp)", "_INTERNAL_USE_ONLY_AvgCAxis");
  filter->setSelectedVectorArrayPath(tempPath);
  filter->setAngleTolerance(m_CAxisTolerance);
  filter->setUseGoodVoxels(true);
  tempPath.update("_INTERNAL_USE_ONLY_PatchDataContainer(Temp)", "_INTERNAL_USE_ONLY_PatchAM(Temp)", "_INTERNAL_USE_ONLY_InMTR");
  filter->setGoodVoxelsArrayPath(tempPath);
  filter->setFeatureIdsArrayName("PatchFeatureIds");
  filter->setCellFeatureAttributeMatrixName("PatchFeatureData");
  filter->setActiveArrayName("Active");
  filter->execute();

  // get the data created by the SegmentFeatures(Vector) filter
  cDims[0] = 1;
  tempPath.update("_INTERNAL_USE_ONLY_PatchDataContainer(Temp)", "_INTERNAL_USE_ONLY_PatchAM(Temp)", "_INTERNAL_USE_ONLY_PatchFeatureIds");
  m_PatchIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, tempPath, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_PatchIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PatchIds = m_PatchIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update("_INTERNAL_USE_ONLY_PatchDataContainer(Temp)", "_INTERNAL_USE_ONLY_PatchFeatureData", "_INTERNAL_USE_ONLY_Active");
  m_PatchActivePtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, tempPath, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_PatchActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PatchActive = m_PatchActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Remove the small patches-----planning to remove/redsesign this
  size_t numPatchFeatures = m_PatchActivePtr.lock()->getNumberOfTuples();
  QVector<bool> activeObjects(numPatchFeatures, true);
  std::vector<size_t> counters(numPatchFeatures, 0);
  for (size_t iter = 0; iter < totalPatches; ++iter)
  {
    counters[m_PatchIds[iter]]++;
  }
  for (size_t iter = 0; iter < numPatchFeatures; ++iter)
  {
    if (counters[iter] < 4) { activeObjects[iter] = false; }
  }
  tempPath.update("_INTERNAL_USE_ONLY_PatchDataContainer(Temp)", "_INTERNAL_USE_ONLY_PatchFeatureData", "_INTERNAL_USE_ONLY_Active");
  AttributeMatrix::Pointer patchFeatureAttrMat = getDataContainerArray()->getAttributeMatrix(tempPath);
  patchFeatureAttrMat->removeInactiveObjects(activeObjects, m_PatchIdsPtr.lock());

  // Resize the feature attribute matrix for the MTRs to the number identified from SegmentFeatures(Vector) after filtering for size
  tDims.resize(1);
  tDims[0] = patchFeatureAttrMat->getNumTuples();
  m->getAttributeMatrix(getNewCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateFeatureInstancePointers();

  int64_t point = 0, patch = 0;
  int64_t zStride = 0, yStride = 0;
  int64_t zStrideP = 0, yStrideP = 0;
  int64_t pCol = 0, pRow = 0, pPlane = 0;

  for (int64_t k = 0; k < origDims[2]; k++)
  {
    if (critDim[2] > 0) { pPlane = (k / critDim[2]); }
    else { pPlane = 0; }
    if (pPlane >= newDims[2]) { pPlane = newDims[2] - 1; }
    zStride = (k * origDims[0] * origDims[1]);
    zStrideP = (pPlane * newDims[0] * newDims[1]);
    for (int64_t j = 0; j < origDims[1]; j++)
    {
      if (critDim[1] > 0) { pRow = (j / critDim[1]); }
      else { pRow = 0; }
      if (pRow >= newDims[1]) { pRow = newDims[1] - 1; }
      yStride = (j * origDims[0]);
      yStrideP = (pRow * newDims[0]);
      for (int64_t i = 0; i < origDims[0]; i++)
      {
        if (critDim[0] > 0) { pCol = (i / critDim[0]); }
        else { pCol = 0; }
        if (pCol >= newDims[0]) { pCol = newDims[0] - 1; }
        point = zStride + yStride + i;
        patch = zStrideP + yStrideP + pCol;
        m_MTRIds[point] = m_PatchIds[patch];
        if (m_PatchIds[patch] > 0)
        {
          m_CAxisLocations[3 * point + 0] = m_AvgCAxis[3 * patch + 0];
          m_CAxisLocations[3 * point + 1] = m_AvgCAxis[3 * patch + 1];
          m_CAxisLocations[3 * point + 2] = m_AvgCAxis[3 * patch + 2];
        }
      }
    }
  }

  // remove the data container temporarily created to hold the patch data
  getDataContainerArray()->removeDataContainer("PatchDataContainer(Temp)");

  findMTRregions();

  int64_t totalFeatures = static_cast<int64_t>(m_AvgCAxisPtr.lock()->getNumberOfTuples());

  // By default we randomize grains
  if (true == getRandomizeMTRIds() && getCancel() == false)
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
AbstractFilter::Pointer IdentifyMicroTextureRegions::newFilterInstance(bool copyFilterParameters)
{
  IdentifyMicroTextureRegions::Pointer filter = IdentifyMicroTextureRegions::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters() );
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyMicroTextureRegions::getCompiledLibraryName()
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyMicroTextureRegions::getBrandingString()
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyMicroTextureRegions::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  Reconstruction::Version::Major() << "." << Reconstruction::Version::Minor() << "." << Reconstruction::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyMicroTextureRegions::getGroupName()
{ return SIMPL::FilterGroups::ReconstructionFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyMicroTextureRegions::getSubGroupName()
{return SIMPL::FilterSubGroups::GroupingFilters;}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyMicroTextureRegions::getHumanLabel()
{ return "Identify MicroTexture Patches (C-Axis Misalignment)"; }
