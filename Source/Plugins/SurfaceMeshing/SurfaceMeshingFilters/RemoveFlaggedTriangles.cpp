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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "RemoveFlaggedTriangles.h"

#include <QtCore/QTextStream>
#include <algorithm>
#include <vector>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Utilities/ParallelDataAlgorithm.h"

#include "SurfaceMeshing/SurfaceMeshingFilters/util/TriangleOps.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataContainerID = 1
};

/**
 * @brief The PopulateReducedGeometryTrianglesImpl pulls the vertices associated with a triangle then loactes the indices in
 * the new VertexList then assigns that "new" triangle to Reduced Geometry
 */
class PopulateReducedGeometryTrianglesImpl
{
public:
  PopulateReducedGeometryTrianglesImpl(const TriangleGeom& originalTriangle, TriangleGeom& reducedTriangle, const std::vector<size_t>& newTrianglesIndex, const std::vector<size_t>& newVerticesIndex)
  : m_OriginalTriangle(originalTriangle)
  , m_ReducedTriangle(reducedTriangle)
  , m_NewTrianglesIndex(newTrianglesIndex)
  , m_NewVerticesIndex(newVerticesIndex)
  {
  }
  virtual ~PopulateReducedGeometryTrianglesImpl() = default;

  void generate(size_t start, size_t end) const
  {
    for(size_t index = start; index < end; index++)
    {
      // pull old vertices
      size_t oldVertexIndices[3] = {0, 0, 0};
      m_OriginalTriangle.getVertsAtTri(m_NewTrianglesIndex[index], oldVertexIndices);

      // locate new vertex index for each vertex index
      size_t newVertexIndices[3] = {0, 0, 0};
      for(size_t vertIndex = 0; vertIndex < 3; vertIndex++)
      {
        const auto& itr = lower_bound(m_NewVerticesIndex.begin(), m_NewVerticesIndex.end(), oldVertexIndices[vertIndex]); // find first instance of value as iterator
        size_t indexOfTarget = distance(m_NewVerticesIndex.begin(), itr);
        newVertexIndices[vertIndex] = indexOfTarget;
      }

      // set the triangle in reduced
      m_ReducedTriangle.setVertsAtTri(index, newVertexIndices);
    }
  }

  void operator()(const SIMPLRange& range) const
  {
    generate(range.min(), range.max());
  }

private:
  const TriangleGeom& m_OriginalTriangle;
  TriangleGeom& m_ReducedTriangle;
  const std::vector<size_t>& m_NewTrianglesIndex;
  const std::vector<size_t>& m_NewVerticesIndex;
};

/**
 * @brief The LocateTriangleIndexImpl class implements a threaded algorithm that updates a mask with
 * bool for if the value should be removed
 */
class LocateTriangleIndexImpl
{
public:
  LocateTriangleIndexImpl(const DataArray<bool>& IDsMask, const DataArray<int32_t>& regionIDs, std::vector<bool>& preallocatedIndexMask)
  : m_IDsMask(IDsMask)
  , m_RegionIDs(regionIDs)
  , m_PreallocatedIndexMask(preallocatedIndexMask)
  {
  }
  virtual ~LocateTriangleIndexImpl() = default;

  void generate(size_t start, size_t end) const
  {
    for(size_t index = start; index < end; index++)
    {
      m_PreallocatedIndexMask[index] = m_IDsMask[m_RegionIDs[index]];
    }
  }

  void operator()(const SIMPLRange& range) const
  {
    generate(range.min(), range.max());
  }

private:
  const DataArray<bool>& m_IDsMask;
  const DataArray<int32_t>& m_RegionIDs;
  std::vector<bool>& m_PreallocatedIndexMask;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveFlaggedTriangles::RemoveFlaggedTriangles() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveFlaggedTriangles::~RemoveFlaggedTriangles() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFlaggedTriangles::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  DataContainerSelectionFilterParameter::RequirementType dcsReq;
  IGeometry::Types geomTypes = {IGeometry::Type::Triangle};
  dcsReq.dcGeometryTypes = geomTypes;
  parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Triangle Geometry", TriangleGeometry, FilterParameter::Category::RequiredArray, RemoveFlaggedTriangles, dcsReq));
  DataArraySelectionFilterParameter::RequirementType dasReq = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Category::Feature);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", MaskArrayPath, FilterParameter::Category::RequiredArray, RemoveFlaggedTriangles, dasReq));
  dasReq = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Region IDs", RegionIDsArrayPath, FilterParameter::Category::RequiredArray, RemoveFlaggedTriangles, dasReq));
  parameters.push_back(SIMPL_NEW_STRING_FP("Reduced Triangle Data Container", ReducedTriangleGeometry, FilterParameter::Category::CreatedArray, RemoveFlaggedTriangles));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFlaggedTriangles::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  TriangleGeom::Pointer triangle = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom>(this, getTriangleGeometry());

  if(getErrorCode() < 0)
  {
    return;
  }

  std::vector<size_t> cDims(1, 1);

  m_MaskPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>>(this, getMaskArrayPath(), cDims);
  if(m_MaskPtr.lock())
  {
    m_Mask = m_MaskPtr.lock()->getPointer(0);
  }
  if(getErrorCode() < 0)
  {
    return;
  }

  m_RegionIDsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getRegionIDsArrayPath(), cDims);
  if(m_RegionIDsPtr.lock())
  {
    m_RegionIDs = m_RegionIDsPtr.lock()->getPointer(0);
  }
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer reducedDataContainerPtr = getDataContainerArray()->createNonPrereqDataContainer(this, getReducedTriangleGeometry(), DataContainerID);

  if(getErrorCode() < 0)
  {
    return;
  }

  SharedVertexList::Pointer vertices = TriangleGeom::CreateSharedVertexList(0);
  TriangleGeom::Pointer reduced = TriangleGeom::CreateGeometry(0, vertices, SIMPL::Geometry::TriangleGeometry, !getInPreflight());
  reducedDataContainerPtr->setGeometry(reduced);

  if(getErrorCode() < 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveFlaggedTriangles::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }
  if(getCancel())
  {
    return;
  }

  // Remove Triangles from reduced according to removeTrianglesIndex
  DataContainer& original = *getDataContainerArray()->getDataContainer(getTriangleGeometry());
  TriangleGeom& originalTriangle = *original.getGeometryAs<TriangleGeom>();
  DataContainer& reduced = *getDataContainerArray()->getDataContainer(getReducedTriangleGeometry());
  TriangleGeom& reducedTriangle = *reduced.getGeometryAs<TriangleGeom>();

  // Set up preallocated masks
  auto size = originalTriangle.getNumberOfTris();
  std::vector<bool> maskTriangles;
  std::vector<size_t> newTrianglesIndexList;
  maskTriangles.resize(size, false);
  newTrianglesIndexList.reserve(size);

  // Parse all the triangles and create a remove list !!!!! true will be removed !!!!!
  ParallelDataAlgorithm dataAlg;
  dataAlg.setRange(0, size);
  dataAlg.execute(LocateTriangleIndexImpl(*m_MaskPtr.lock(), *m_RegionIDsPtr.lock(), maskTriangles));

  // parse mask Triangles list and load a list of indices for triangles to keep
  for(size_t index = 0; index < size; index++)
  {
    if(!maskTriangles[index])
    {
      newTrianglesIndexList.push_back(index);
    }
  }
  newTrianglesIndexList.shrink_to_fit();

  if(getCancel())
  {
    return;
  }
  if(newTrianglesIndexList.size() == 0)
  {
    QString ss = "Re-evalute mask conditions - with current configuration all triangles will be stripped!";
    setErrorCondition(-67880, ss);
  }
  if(getErrorCode() < 0)
  {
    return;
  }

  // flatten a list of the indices of vertices used by the triangles
  std::vector<size_t> psuedoVertexList; // also used as a pseudo look up table in PopulateReducedGeometryTrianglesImpl
  for(size_t index = 0; index < newTrianglesIndexList.size(); index++)
  {
    size_t vertIDs[3] = {0, 0, 0};
    originalTriangle.getVertsAtTri(newTrianglesIndexList[index], vertIDs);
    psuedoVertexList.push_back(vertIDs[0]);
    psuedoVertexList.push_back(vertIDs[1]);
    psuedoVertexList.push_back(vertIDs[2]);
  }
  if(getCancel())
  {
    return;
  }

  if(psuedoVertexList.size() == 0)
  {
    QString ss = "Re-evalute mask conditions - with current configuration all vetrices will be dumped!";
    setErrorCondition(-67881, ss);
  }
  if(getErrorCode() < 0)
  {
    return;
  }

  // clear duplicate values out of vector
  std::sort(psuedoVertexList.begin(), psuedoVertexList.end()); // orders ascendingly !!!!! Basis for later search !!!!!
  auto dupes = std::unique(psuedoVertexList.begin(), psuedoVertexList.end());
  psuedoVertexList.erase(dupes, psuedoVertexList.end());

  // define new sizing
  size = psuedoVertexList.size();
  reducedTriangle.resizeVertexList(size); // resize accordingly

  // load reduced Geometry Vertex list according to used vertices
  float coords[3] = {0.0f, 0.0f, 0.0f};
  for(size_t i = 0; i < size; i++)
  {
    originalTriangle.getCoords(psuedoVertexList[i], coords);
    reducedTriangle.setCoords(i, coords);
  }

  if(getCancel())
  {
    return;
  }

  // Set up preprocessing conditions (preallocation for parallelization)
  size = newTrianglesIndexList.size();
  reducedTriangle.resizeTriList(size); // resize accordingly

  // parse triangles and reassign indexes to match new vertex list
  dataAlg.setRange(0, size);
  dataAlg.execute(PopulateReducedGeometryTrianglesImpl(originalTriangle, reducedTriangle, newTrianglesIndexList, psuedoVertexList));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RemoveFlaggedTriangles::newFilterInstance(bool copyFilterParameters) const
{
  RemoveFlaggedTriangles::Pointer filter = RemoveFlaggedTriangles::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RemoveFlaggedTriangles::getCompiledLibraryName() const
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RemoveFlaggedTriangles::getBrandingString() const
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RemoveFlaggedTriangles::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RemoveFlaggedTriangles::getGroupName() const
{
  return SIMPL::FilterGroups::SurfaceMeshingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid RemoveFlaggedTriangles::getUuid() const
{
  return QUuid("{379ccc67-16dd-530a-984f-177db9351bac}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RemoveFlaggedTriangles::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CleanupFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RemoveFlaggedTriangles::getHumanLabel() const
{
  return "Remove Flagged Triangles";
}

// -----------------------------------------------------------------------------
RemoveFlaggedTriangles::Pointer RemoveFlaggedTriangles::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<RemoveFlaggedTriangles> RemoveFlaggedTriangles::New()
{
  struct make_shared_enabler : public RemoveFlaggedTriangles
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString RemoveFlaggedTriangles::getNameOfClass() const
{
  return QString("RemoveFlaggedTriangles");
}

// -----------------------------------------------------------------------------
QString RemoveFlaggedTriangles::ClassName()
{
  return QString("RemoveFlaggedTriangles");
}

// -----------------------------------------------------------------------------
void RemoveFlaggedTriangles::setTriangleGeometry(const DataArrayPath& value)
{
  m_TriangleGeometry = value;
}

// -----------------------------------------------------------------------------
DataArrayPath RemoveFlaggedTriangles::getTriangleGeometry() const
{
  return m_TriangleGeometry;
}

// -----------------------------------------------------------------------------
void RemoveFlaggedTriangles::setMaskArrayPath(const DataArrayPath& value)
{
  m_MaskArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath RemoveFlaggedTriangles::getMaskArrayPath() const
{
  return m_MaskArrayPath;
}

// -----------------------------------------------------------------------------
void RemoveFlaggedTriangles::setRegionIDsArrayPath(const DataArrayPath& value)
{
  m_RegionIDsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath RemoveFlaggedTriangles::getRegionIDsArrayPath() const
{
  return m_RegionIDsArrayPath;
}

// -----------------------------------------------------------------------------
void RemoveFlaggedTriangles::setReducedTriangleGeometry(const QString& value)
{
  m_ReducedTriangleGeometry = value;
}

// -----------------------------------------------------------------------------
QString RemoveFlaggedTriangles::getReducedTriangleGeometry() const
{
  return m_ReducedTriangleGeometry;
}
