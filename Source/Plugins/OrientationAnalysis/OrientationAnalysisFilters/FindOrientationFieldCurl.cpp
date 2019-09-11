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

#include "FindOrientationFieldCurl.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/atomic.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "OrientationLib/Core/Quaternion.hpp"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

class FindMisorientationVectorsImpl
{
public:
  FindMisorientationVectorsImpl(float* quats, float* misoVecs, int64_t* neighbors, int64_t* faceIds)
  : m_Quats(quats)
  , m_MisoVecs(misoVecs)
  , m_Neighbors(neighbors)
  , m_FaceIds(faceIds)
  {
    m_OrientationOps = LaueOps::getOrientationOpsQVector();
  }
  virtual ~FindMisorientationVectorsImpl() = default;

  void convert(size_t start, size_t end) const
  {
    using QuaternionType = Quaternion<double>;

    for(size_t i = start; i < end; i++)
    {
      QuaternionType q1(m_Quats[i * 4 + 0], m_Quats[i * 4 + 1], m_Quats[i * 4 + 2], m_Quats[i * 4 + 3]);
      for(int j = 0; j < 3; j++)
      {
        if(m_Neighbors[3 * i + j] > 0)
        {
          size_t idx = m_Neighbors[3 * i + j];
          QuaternionType q2(m_Quats[idx * 4 + 0], m_Quats[idx * 4 + 1], m_Quats[idx * 4 + 2], m_Quats[idx * 4 + 3]);
          QuaternionType delq = q1 * (q2.conjugate());

          delq = m_OrientationOps[1]->getFZQuat(delq);
          std::array<double, 3> misoVec = delq.getMisorientationVector();
          m_MisoVecs[3 * m_FaceIds[3 * i + j] + 0] = misoVec[0];
          m_MisoVecs[3 * m_FaceIds[3 * i + j] + 1] = misoVec[1];
          m_MisoVecs[3 * m_FaceIds[3 * i + j] + 2] = misoVec[2];
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
  float* m_Quats = nullptr;
  float* m_MisoVecs = nullptr;
  int64_t* m_Neighbors = nullptr;
  int64_t* m_FaceIds = nullptr;
  QVector<LaueOps::Pointer> m_OrientationOps;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindOrientationFieldCurl::FindOrientationFieldCurl()
: m_CellPhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases)
, m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures)
, m_QuatsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Quats)
, m_DislocationTensorsArrayName(SIMPL::CellData::DislocationTensors)
{
  m_OrientationOps = LaueOps::getOrientationOpsQVector();

  m_CurlSize[0] = 1;
  m_CurlSize[1] = 1;
  m_CurlSize[2] = 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindOrientationFieldCurl::~FindOrientationFieldCurl() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindOrientationFieldCurl::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Curl Radius (Pixels)", CurlSize, FilterParameter::Parameter, FindOrientationFieldCurl));

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Cell Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, FindOrientationFieldCurl, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, FindOrientationFieldCurl, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::Defaults::AnyPrimitive, 4, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Quats", QuatsArrayPath, FilterParameter::RequiredArray, FindOrientationFieldCurl, req));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("DislocationTensors", DislocationTensorsArrayName, FilterParameter::CreatedArray, FindOrientationFieldCurl));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindOrientationFieldCurl::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDislocationTensorsArrayName(reader->readString("DislocationTensorsArrayName", getDislocationTensorsArrayName()));
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  setCurlSize(reader->readIntVec3("CurlSize", getCurlSize()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindOrientationFieldCurl::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindOrientationFieldCurl::dataCheck()
{
  DataArrayPath tempPath;
  clearErrorCode();
  clearWarningCode();

  std::vector<size_t> dims(1, 1);
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(),
                                                                                                        dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellPhasesPtr.lock())                                                                        /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getCellPhasesArrayPath().getDataContainerName())->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCode() < 0 || nullptr == image.get())
  {
    return;
  }

  dims[0] = 9;
  tempPath.update(m_CellPhasesArrayPath.getDataContainerName(), m_CellPhasesArrayPath.getAttributeMatrixName(), getDislocationTensorsArrayName());
  m_DislocationTensorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, dims);                       /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_DislocationTensorsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_DislocationTensors = m_DislocationTensorsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 1;
  // typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                    dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 4;
  m_QuatsPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_QuatsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Quats = m_QuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindOrientationFieldCurl::preflight()
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
void FindOrientationFieldCurl::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_CellPhasesArrayPath.getDataContainerName());
  size_t xP = m->getGeometryAs<ImageGeom>()->getXPoints();
  size_t yP = m->getGeometryAs<ImageGeom>()->getYPoints();
  size_t zP = m->getGeometryAs<ImageGeom>()->getZPoints();

  // QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  size_t totalFaces = ((xP + 1) * yP * zP) + ((yP + 1) * xP * zP) + ((zP + 1) * xP * yP);
  std::vector<size_t> tDims(1, totalFaces);
  std::vector<size_t> cDims(1, 3);
  FloatArrayType::Pointer misoVecsPtr = FloatArrayType::CreateArray(tDims, cDims, "misoVecs", true);
  misoVecsPtr->initializeWithValue(0);
  float* misoVecs = misoVecsPtr->getPointer(0);
  tDims[0] = (xP * yP * zP);
  Int64ArrayType::Pointer neighborsPtr = Int64ArrayType::CreateArray(tDims, cDims, "neighbors", true);
  neighborsPtr->initializeWithValue(-1);
  int64_t* neighbors = neighborsPtr->getPointer(0);
  Int64ArrayType::Pointer faceIdsPtr = Int64ArrayType::CreateArray(tDims, cDims, "faceIds", true);
  faceIdsPtr->initializeWithValue(-1);
  int64_t* faceIds = faceIdsPtr->getPointer(0);

  size_t count = 0;
  size_t yshift = (xP + 1) * yP * zP;
  size_t zshift = ((xP + 1) * yP * zP) + ((yP + 1) * xP * zP);

  for(size_t i = 0; i < zP; i++)
  {
    for(size_t j = 0; j < yP; j++)
    {
      for(size_t k = 0; k < xP; k++)
      {
        if(k != xP - 1)
        {
          neighbors[3 * count + 0] = count + 1, faceIds[3 * count + 0] = k + (j * (xP + 1)) + (i * (xP + 1) * yP) + 1;
        }
        if(j != yP - 1)
        {
          neighbors[3 * count + 1] = count + xP, faceIds[3 * count + 1] = j + (k * (yP + 1)) + (i * (yP + 1) * xP) + yshift + 1;
        }
        if(i != zP - 1)
        {
          neighbors[3 * count + 2] = count + (xP * yP), faceIds[3 * count + 2] = i + (k * (zP + 1)) + (j * (zP + 1) * xP) + zshift + 1;
        }
        count++;
      }
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

// first determine the misorientation vectors on all the voxel faces
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, (xP * yP * zP)), FindMisorientationVectorsImpl(m_Quats, misoVecs, neighbors, faceIds), tbb::auto_partitioner());
  }
  else
#endif
  {
    FindMisorientationVectorsImpl serial(m_Quats, misoVecs, neighbors, faceIds);
    serial.convert(0, (xP * yP * zP));
  }

  int good = 0;

  //  float w, totalmisorientation;
  //  float n1, n2, n3;
  //  unsigned int phase1 = Ebsd::CrystalStructure::UnknownCrystalStructure;
  //  unsigned int phase2 = Ebsd::CrystalStructure::UnknownCrystalStructure;
  SizeVec3Type udims = m->getGeometryAs<ImageGeom>()->getDimensions();

  int64_t xPoints = static_cast<int64_t>(udims[0]);
  int64_t yPoints = static_cast<int64_t>(udims[1]);
  int64_t zPoints = static_cast<int64_t>(udims[2]);

  int64_t point;
  size_t neighbor = 0;
  int64_t rowShift = xPoints;
  int64_t planeShift = xPoints * yPoints;
  int64_t rowStride;
  int64_t planeStride;

  float kappa11 = 0;
  float kappa21 = 0;
  float kappa31 = 0;
  float kappa12 = 0;
  float kappa22 = 0;
  float kappa32 = 0;
  float kappa13 = 0;
  float kappa23 = 0;
  float kappa33 = 0;
  for(int64_t plane = 0; plane < zPoints; plane++)
  {
    planeStride = plane * planeShift;
    for(int64_t row = 0; row < yPoints; row++)
    {
      rowStride = row * rowShift;
      for(int64_t col = 0; col < xPoints; col++)
      {
        point = planeStride + rowStride + col;
        if(m_CellPhases[point] > 0)
        {
          count = 0;
          kappa11 = 0;
          kappa21 = 0;
          kappa31 = 0;
          kappa12 = 0;
          kappa22 = 0;
          kappa32 = 0;
          kappa13 = 0;
          kappa23 = 0;
          kappa33 = 0;
          for(int j = -m_CurlSize[2]; j < m_CurlSize[2]; j++)
          {
            good = 1;
            if(plane + j < 0)
            {
              good = 0;
            }
            else if(plane + j >= zPoints - 1)
            {
              good = 0;
            }
            if(good == 1)
            {
              neighbor = point + (j * planeShift);
              kappa13 += misoVecs[3 * faceIds[3 * neighbor + 2]];
              kappa23 += misoVecs[3 * faceIds[3 * neighbor + 2] + 1];
              kappa33 += misoVecs[3 * faceIds[3 * neighbor + 2] + 2];
              count++;
            }
          }
          if(count > 0)
          {
            kappa13 /= count;
            kappa23 /= count;
            kappa33 /= count;
            count = 0;
          }

          count = 0;
          for(int k = -m_CurlSize[1]; k < m_CurlSize[1]; k++)
          {
            good = 1;
            if(row + k < 0)
            {
              good = 0;
            }
            else if(row + k >= yPoints - 1)
            {
              good = 0;
            }
            if(good == 1)
            {
              neighbor = point + (k * rowShift);
              kappa12 += misoVecs[3 * faceIds[3 * neighbor + 1]];
              kappa22 += misoVecs[3 * faceIds[3 * neighbor + 1] + 1];
              kappa32 += misoVecs[3 * faceIds[3 * neighbor + 1] + 2];
              count++;
            }
          }
          if(count > 0)
          {
            kappa12 /= count;
            kappa22 /= count;
            kappa32 /= count;
            count = 0;
          }

          for(int l = -m_CurlSize[0]; l < m_CurlSize[2]; l++)
          {
            good = 1;
            if(col + l < 0)
            {
              good = 0;
            }
            else if(col + l >= xPoints - 1)
            {
              good = 0;
            }
            if(good == 1)
            {
              neighbor = point + l;
              kappa11 += misoVecs[3 * faceIds[3 * neighbor]];
              kappa21 += misoVecs[3 * faceIds[3 * neighbor] + 1];
              kappa31 += misoVecs[3 * faceIds[3 * neighbor] + 2];
              count++;
            }
          }
          if(count > 0)
          {
            kappa11 /= count;
            kappa21 /= count;
            kappa31 /= count;
            count = 0;
          }

          m_DislocationTensors[9 * point] = -kappa22 - kappa33;
          m_DislocationTensors[9 * point + 1] = kappa21;
          m_DislocationTensors[9 * point + 2] = kappa31;
          m_DislocationTensors[9 * point + 3] = kappa12;
          m_DislocationTensors[9 * point + 4] = -kappa11 - kappa33;
          m_DislocationTensors[9 * point + 5] = kappa32;
          m_DislocationTensors[9 * point + 6] = kappa13;
          m_DislocationTensors[9 * point + 7] = kappa23;
          m_DislocationTensors[9 * point + 8] = -kappa11 - kappa22;
        }
      }
    }
  }

  notifyStatusMessage("FindOrientationFieldCurl Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindOrientationFieldCurl::newFilterInstance(bool copyFilterParameters) const
{
  FindOrientationFieldCurl::Pointer filter = FindOrientationFieldCurl::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindOrientationFieldCurl::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindOrientationFieldCurl::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindOrientationFieldCurl::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindOrientationFieldCurl::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid FindOrientationFieldCurl::getUuid()
{
  return QUuid("{d2011845-cae8-549a-88eb-7ecadf51a916}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindOrientationFieldCurl::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindOrientationFieldCurl::getHumanLabel() const
{
  return "Find Curl of Orientation Field";
}
