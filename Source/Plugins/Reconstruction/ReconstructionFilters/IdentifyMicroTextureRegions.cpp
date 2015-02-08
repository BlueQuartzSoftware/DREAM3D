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

#include "IdentifyMicroTextureRegions.h"


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/atomic.h>
#include <tbb/tick_count.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>
#endif


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/GeometryMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "OrientationLib/Math/OrientationMath.h"
#include "OrientationLib/OrientationOps/OrientationOps.h"


//included so we can call under the hood to segment the patches found in this filter
#include "Reconstruction/ReconstructionFilters/VectorSegmentFeatures.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();



class FindPatchMisalignmentsImpl
{
  public:
    FindPatchMisalignmentsImpl(IntVec3_t newDims, IntVec3_t origDims, float* caxisLocs, int32_t* phases, unsigned int* crystructs, float* volFrac, float* avgCAxis, bool* inMTR, IntVec3_t critDim, float minVolFrac, float caxisTol) :
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
      int xDim = (2 * m_CritDim.x) + 1;
      int yDim = (2 * m_CritDim.y) + 1;
      int zDim = (2 * m_CritDim.z) + 1;
      QVector<size_t> tDims(1, xDim * yDim * zDim);
      QVector<size_t> cDims(1, 3);
      FloatArrayType::Pointer cAxisLocsPtr = FloatArrayType::CreateArray(tDims, cDims, "cAxisLocs");
      cAxisLocsPtr->initializeWithValue(0);
      float* cAxisLocs = cAxisLocsPtr->getPointer(0);
      std::vector<int32_t> goodCounts;

      int xc, yc, zc;
      for (size_t iter = start; iter < end; iter++)
      {
        int zStride, yStride;
        int count = 0;
        xc = ((iter % m_DicDims.x) * m_CritDim.x) + (m_CritDim.x / 2);
        yc = (((iter / m_DicDims.x) % m_DicDims.y) * m_CritDim.y) + (m_CritDim.y / 2);
        zc = ((iter / (m_DicDims.x * m_DicDims.y)) * m_CritDim.z) + (m_CritDim.z / 2);
        for(int k = -m_CritDim.z; k <= m_CritDim.z; k++)
        {
          if((zc + k) >= 0 && (zc + k) < m_VolDims.z)
          {
            zStride = ((zc + k) * m_VolDims.x * m_VolDims.y);
            for(int j = -m_CritDim.y; j <= m_CritDim.y; j++)
            {
              if((yc + j) >= 0 && (yc + j) < m_VolDims.y)
              {
                yStride = ((yc + j) * m_VolDims.x);
                for(int i = -m_CritDim.x; i <= m_CritDim.x; i++)
                {
                  if((xc + i) >= 0 && (xc + i) < m_VolDims.x)
                  {
                    if(m_CrystalStructures[m_CellPhases[(zStride + yStride + xc + i)]] == Ebsd::CrystalStructure::Hexagonal_High)
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
        float angle;
        goodCounts.resize(count);
        goodCounts.assign(count, 0);
        for(int i = 0; i < count; i++)
        {
          for(int j = i; j < count; j++)
          {
            angle = GeometryMath::AngleBetweenVectors(cAxisLocsPtr->getPointer(3 * i), cAxisLocsPtr->getPointer(3 * j));
            if(angle <= m_CAxisTolerance || (DREAM3D::Constants::k_Pi - angle) <= m_CAxisTolerance)
            {
              goodCounts[i]++;
              goodCounts[j]++;
            }
          }
        }
        int goodPointCount = 0;
        for(int i = 0; i < count; i++)
        {
          if(float(goodCounts[i]) / float(count) > m_MinVolFrac) { goodPointCount++; }
        }
        float avgCAxis[3] = {0.0, 0.0, 0.0};
        float frac = float(goodPointCount) / float(count);
        m_VolFrac[iter] = frac;
        if(frac > m_MinVolFrac)
        {
          m_InMTR[iter] = true;
          for(int i = 0; i < count; i++)
          {
            if(float(goodCounts[i]) / float(count) >= m_MinVolFrac)
            {
              if(MatrixMath::DotProduct3x1(avgCAxis, cAxisLocsPtr->getPointer(3 * i)) < 0)
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
          if(avgCAxis[2] < 0) { MatrixMath::Multiply3x1withConstant(avgCAxis, -1); }
          m_AvgCAxis[3 * iter] = avgCAxis[0];
          m_AvgCAxis[3 * iter + 1] = avgCAxis[1];
          m_AvgCAxis[3 * iter + 2] = avgCAxis[2];
        }
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      convert(r.begin(), r.end());
    }
#endif
  private:
    IntVec3_t m_DicDims;
    IntVec3_t m_VolDims;
    float* m_CAxisLocations;
    int32_t* m_CellPhases;
    unsigned int* m_CrystalStructures;
    bool* m_InMTR;
    float* m_VolFrac;
    float* m_AvgCAxis;
    IntVec3_t m_CritDim;
    float m_MinVolFrac;
    float m_CAxisTolerance;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IdentifyMicroTextureRegions::IdentifyMicroTextureRegions() :
  AbstractFilter(),
  m_NewCellFeatureAttributeMatrixName(DREAM3D::Defaults::NewCellFeatureAttributeMatrixName),
  m_CAxisTolerance(1.0f),
  m_MinMTRSize(1.0f),
  m_MinVolFrac(1.0f),
  m_RandomizeMTRIds(false),
  m_CAxisLocationsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::CAxisLocation),
  m_CellPhasesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_MTRIdsArrayName(DREAM3D::CellData::ParentIds),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_MTRIds(NULL),
  m_CAxisLocationsArrayName(DREAM3D::CellData::CAxisLocation),
  m_CAxisLocations(NULL),
  m_CellPhasesArrayName(DREAM3D::CellData::CAxisLocation),
  m_CellPhases(NULL),
  m_Active(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL),
  m_InMTR(NULL),
  m_VolFrac(NULL),
  m_AvgCAxis(NULL),
  m_PatchIds(NULL),
  m_PatchActive(NULL)
{
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
  parameters.push_back(FilterParameter::New("C-Axis Alignment Tolerance", "CAxisTolerance", FilterParameterWidgetType::DoubleWidget, getCAxisTolerance(), false, "Degrees"));
  parameters.push_back(FilterParameter::New("Minimum MicroTextured Region Size (Diameter)", "MinMTRSize", FilterParameterWidgetType::DoubleWidget, getMinMTRSize(), false));
  parameters.push_back(FilterParameter::New("Minimum Volume Fraction in MTR", "MinVolFrac", FilterParameterWidgetType::DoubleWidget, getMinVolFrac(), false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("CAxisLocations", "CAxisLocationsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCAxisLocationsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("CellPhases", "CellPhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellPhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("MTR Ids", "MTRIdsArrayName", FilterParameterWidgetType::StringWidget, getMTRIdsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("New Cell Feature Attribute Matrix Name", "NewCellFeatureAttributeMatrixName", FilterParameterWidgetType::StringWidget, getNewCellFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Active", "ActiveArrayName", FilterParameterWidgetType::StringWidget, getActiveArrayName(), true, ""));
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
int IdentifyMicroTextureRegions::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(ActiveArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(MTRIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CAxisLocationsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CAxisTolerance)
  DREAM3D_FILTER_WRITE_PARAMETER(MinMTRSize)
  DREAM3D_FILTER_WRITE_PARAMETER(MinVolFrac)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
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
void IdentifyMicroTextureRegions::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_CAxisLocationsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer newCellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getNewCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0 || NULL == newCellFeatureAttrMat.get()) { return; }

  ImageGeom::Pointer image = m->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0 || NULL == image.get()) { return; }

  QVector<size_t> dims(1, 3);
  // Cell Data
  m_CAxisLocationsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, m_CAxisLocationsArrayPath, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CAxisLocationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CAxisLocations = m_CAxisLocationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, m_CellPhasesArrayPath, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(m_CAxisLocationsArrayPath.getDataContainerName(), m_CAxisLocationsArrayPath.getAttributeMatrixName(), getMTRIdsArrayName() );
  m_MTRIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_MTRIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_MTRIds = m_MTRIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // New Feature Data
  tempPath.update(m_CAxisLocationsArrayPath.getDataContainerName(), getNewCellFeatureAttributeMatrixName(), getActiveArrayName() );
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
void IdentifyMicroTextureRegions::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCAxisLocationsArrayPath().getDataContainerName());
  int64_t totalPoints = m_MTRIdsPtr.lock()->getNumberOfTuples();

  //calculate dimensions of DIC-like grid
  size_t dcDims[3] = { 0, 0, 0};
  float xRes, yRes, zRes;
  float m_Origin[3];
  m->getGeometryAs<ImageGeom>()->getDimensions(dcDims[0], dcDims[1], dcDims[2]);
  m->getGeometryAs<ImageGeom>()->getResolution(xRes, yRes, zRes);
  m->getGeometryAs<ImageGeom>()->getOrigin(m_Origin);

  //Find number of original cells in radius of patch
  IntVec3_t critDim;
  critDim.x = int(m_MinMTRSize / (4.0 * xRes));
  critDim.y = int(m_MinMTRSize / (4.0 * yRes));
  critDim.z = int(m_MinMTRSize / (4.0 * zRes));

  //Find physical distance of patch steps
  FloatVec3_t critRes;
  critRes.x = float(critDim.x) * xRes;
  critRes.y = float(critDim.y) * yRes;
  critRes.z = float(critDim.z) * zRes;

  //Find number of patch steps in each dimension
  int newDimX = int(dcDims[0] / critDim.x);
  int newDimY = int(dcDims[1] / critDim.y);
  int newDimZ = int(dcDims[2] / critDim.z);
  if(dcDims[0] == 1) { newDimX = 1, critDim.x = 0; }
  if(dcDims[1] == 1) { newDimY = 1, critDim.y = 0; }
  if(dcDims[2] == 1) { newDimZ = 1, critDim.z = 0; }

  //Store the original and patch dimensions for passing into the parallel algo below
  IntVec3_t origDims;
  origDims.x = dcDims[0];
  origDims.y = dcDims[1];
  origDims.z = dcDims[2];
  IntVec3_t newDims;
  newDims.x = newDimX;
  newDims.y = newDimY;
  newDims.z = newDimZ;
  size_t totalPatches = (newDimX * newDimY * newDimZ);

  //Create temporary DataContainer and AttributeMatrix for holding the patch data
  DataContainer::Pointer tmpDC = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, "PatchDataContainer(Temp)");
  if(getErrorCondition() < 0) { return; }
  tmpDC->getGeometryAs<ImageGeom>()->setDimensions(static_cast<size_t>(newDimX), static_cast<size_t>(newDimY), static_cast<size_t>(newDimZ));
  tmpDC->getGeometryAs<ImageGeom>()->setResolution(critRes.x, critRes.y, critRes.z);
  tmpDC->getGeometryAs<ImageGeom>()->setOrigin(m_Origin[0], m_Origin[1], m_Origin[2]);

  QVector<size_t> tDims(3, 0);
  tDims[0] = newDimX;
  tDims[1] = newDimY;
  tDims[2] = newDimZ;
//  AttributeMatrix::Pointer tempPatchAttrMat = tmpDC->createNonPrereqAttributeMatrix<AbstractFilter>(this, "PatchAM(Temp)", tDims, DREAM3D::AttributeMatrixType::Cell);
//  if(getErrorCondition() < 0) { return; }

  DataArrayPath tempPath;
  tDims[0] = totalPatches;
  QVector<size_t> cDims(1, 1);
  tempPath.update("PatchDataContainer(Temp)", "PatchAM(Temp)", "InMTR");
  m_InMTRPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_InMTRPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_InMTR = m_InMTRPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update("PatchDataContainer(Temp)", "PatchAM(Temp)", "VolFrac");
  m_VolFracPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_VolFracPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_VolFrac = m_VolFracPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  cDims[0] = 3;
  tempPath.update("PatchDataContainer(Temp)", "PatchAM(Temp)", "AvgCAxis");
  m_AvgCAxisPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgCAxisPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgCAxis = m_AvgCAxisPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Convert user defined tolerance to radians.
  caxisTolerance = m_CAxisTolerance * DREAM3D::Constants::k_Pi / 180.0f;

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  // Tell the user we are starting the filter
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Starting");

//first determine the misorientation vectors on all the voxel faces
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPatches),
                      FindPatchMisalignmentsImpl(newDims, origDims, m_CAxisLocations, m_CellPhases, m_CrystalStructures, m_VolFrac, m_AvgCAxis, m_InMTR, critDim, m_MinVolFrac, caxisTolerance), tbb::auto_partitioner());

  }
  else
#endif
  {
    FindPatchMisalignmentsImpl serial(newDims, origDims, m_CAxisLocations, m_CellPhases, m_CrystalStructures, m_VolFrac, m_AvgCAxis, m_InMTR, critDim, m_MinVolFrac, caxisTolerance);
    serial.convert(0, totalPatches);
  }

  //Call the SegmentFeatures(Vector) filter under the hood to segment the patches based on average c-axis of the patch
  VectorSegmentFeatures::Pointer filter = VectorSegmentFeatures::New();
  filter->setDataContainerArray(getDataContainerArray());
  tempPath.update("PatchDataContainer(Temp)", "PatchAM(Temp)", "AvgCAxis");
  filter->setSelectedVectorArrayPath(tempPath);
  filter->setAngleTolerance(m_CAxisTolerance);
  filter->setUseGoodVoxels(true);
  tempPath.update("PatchDataContainer(Temp)", "PatchAM(Temp)", "InMTR");
  filter->setGoodVoxelsArrayPath(tempPath);
  filter->setFeatureIdsArrayName("PatchFeatureIds");
  filter->setCellFeatureAttributeMatrixName("PatchFeatureData");
  filter->setActiveArrayName("Active");
  filter->execute();

  //get the data created by the SegmentFeatures(Vector) filter
  cDims[0] = 1;
  tempPath.update("PatchDataContainer(Temp)", "PatchAM(Temp)", "PatchFeatureIds");
  m_PatchIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, tempPath, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_PatchIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PatchIds = m_PatchIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update("PatchDataContainer(Temp)", "PatchFeatureData", "Active");
  m_PatchActivePtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, tempPath, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_PatchActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PatchActive = m_PatchActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  //Remove the small patches-----planning to remove/redsesign this
  size_t numPatchFeatures = m_PatchActivePtr.lock()->getNumberOfTuples();
  QVector<bool> activeObjects(numPatchFeatures, true);
  std::vector<size_t> counters(numPatchFeatures, 0);
  for(size_t iter = 0; iter < totalPatches; ++iter)
  {
    counters[m_PatchIds[iter]]++;
  }
  for(size_t iter = 0; iter < numPatchFeatures; ++iter)
  {
    if(counters[iter] < 4) { activeObjects[iter] = false; }
  }
  tempPath.update("PatchDataContainer(Temp)", "PatchFeatureData", "Active");
  AttributeMatrix::Pointer patchFeatureAttrMat = getDataContainerArray()->getAttributeMatrix(tempPath);
  patchFeatureAttrMat->removeInactiveObjects(activeObjects, m_PatchIdsPtr.lock());


  //Resize the feature attribute matrix for the MTRs to the number identified from SegmentFeatures(Vector) after filtering for size
  tDims.resize(1);
  tDims[0] = patchFeatureAttrMat->getNumTuples();
  m->getAttributeMatrix(getNewCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateFeatureInstancePointers();

  size_t point, patch;
  int zStride, yStride;
  int zStrideP, yStrideP;
  int pCol, pRow, pPlane;

  for(int k = 0; k < origDims.z; k++)
  {
    if(critDim.z > 0) { pPlane = (k / critDim.z); }
    else { pPlane = 0; }
    if(pPlane >= newDims.z) { pPlane = newDims.z - 1; }
    zStride = (k * origDims.x * origDims.y);
    zStrideP = (pPlane * newDims.x * newDims.y);
    for(int j = 0; j < origDims.y; j++)
    {
      if(critDim.y > 0) { pRow = (j / critDim.y); }
      else { pRow = 0; }
      if(pRow >= newDims.y) { pRow = newDims.y - 1; }
      yStride = (j * origDims.x);
      yStrideP = (pRow * newDims.x);
      for(int i = 0; i < origDims.x; i++)
      {
        if(critDim.x > 0) { pCol = (i / critDim.x); }
        else { pCol = 0; }
        if(pCol >= newDims.x) { pCol = newDims.x - 1; }
        point = zStride + yStride + i;
        patch = zStrideP + yStrideP + pCol;
        m_MTRIds[point] = m_PatchIds[patch];
        if(m_PatchIds[patch] > 0)
        {
          m_CAxisLocations[3 * point + 0] = m_AvgCAxis[3 * patch + 0];
          m_CAxisLocations[3 * point + 1] = m_AvgCAxis[3 * patch + 1];
          m_CAxisLocations[3 * point + 2] = m_AvgCAxis[3 * patch + 2];
        }
      }
    }
  }

  //remove the data container temporarily created to hold the patch data
  //getDataContainerArray()->removeDataContainer("PatchDataContainer(Temp)");

  findMTRregions();

  size_t totalFeatures = m_ActivePtr.lock()->getNumberOfTuples();
  if (totalFeatures < 2)
  {
//    setErrorCondition(-87000);
//    notifyErrorMessage(getHumanLabel(), "The number of Grouped Features was 0 or 1 which means no grouped features were detected. Is a grouping value set to high?", getErrorCondition());
//    return;
  }

  // By default we randomize grains
  if (true == getRandomizeMTRIds() && getCancel() == false)
  {
    totalPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getNumberOfTuples());
    randomizeFeatureIds(totalPoints, totalFeatures);
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "IdentifyMicroTextureRegions Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::randomizeFeatureIds(int64_t totalPoints, size_t totalFeatures)
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
//    m_MTRIds[i] = gid[ m_MTRIds[i] ];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::findMTRregions()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::initializeVoxelSeedGenerator(const size_t rangeMin, const size_t rangeMax)
{
  // The way we are using the boost random number generators is that we are asking for a NumberDistribution (see the typedef)
  // to guarantee the numbers are betwee a specific range and will only be generated once. We also keep a tally of the
  // total number of numbers generated as a way to make sure the while loops eventually terminate. This setup should
  // make sure that every voxel can be a seed point.
  //  const size_t rangeMin = 0;
  //  const size_t rangeMax = totalPoints - 1;
  m_Distribution = boost::shared_ptr<NumberDistribution>(new NumberDistribution(rangeMin, rangeMax));
  m_RandomNumberGenerator = boost::shared_ptr<RandomNumberGenerator>(new RandomNumberGenerator);
  m_NumberGenerator = boost::shared_ptr<Generator>(new Generator(*m_RandomNumberGenerator, *m_Distribution));
  m_RandomNumberGenerator->seed(static_cast<size_t>( QDateTime::currentMSecsSinceEpoch() )); // seed with the current time
  m_TotalRandomNumbersGenerated = 0;
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
{ return Reconstruction::ReconstructionBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyMicroTextureRegions::getGroupName()
{ return DREAM3D::FilterGroups::ReconstructionFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyMicroTextureRegions::getSubGroupName()
{return DREAM3D::FilterSubGroups::GroupingFilters;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyMicroTextureRegions::getHumanLabel()
{ return "Identify MicroTexture Patches (C-Axis Misalignment)"; }

