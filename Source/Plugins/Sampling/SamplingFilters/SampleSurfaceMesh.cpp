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

#include "SampleSurfaceMesh.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/DataArrays/DynamicListArray.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"

#include "Sampling/SamplingConstants.h"

/**
 * @brief The SampleSurfaceMeshImpl class implements a threaded algorithm that samples a surface mesh based on points passed from subclassed Filters.
 */
class SampleSurfaceMeshImpl
{
    TriangleGeom::Pointer m_Faces;
    Int32Int32DynamicListArray::Pointer m_FaceIds;
    VertexGeom::Pointer m_FaceBBs;
    VertexGeom::Pointer m_Points;
    int32_t* m_PolyIds;

  public:
    SampleSurfaceMeshImpl(TriangleGeom::Pointer faces, Int32Int32DynamicListArray::Pointer faceIds, VertexGeom::Pointer faceBBs, VertexGeom::Pointer points, int32_t* polyIds) :
      m_Faces(faces),
      m_FaceIds(faceIds),
      m_FaceBBs(faceBBs),
      m_Points(points),
      m_PolyIds(polyIds)
    {}
    virtual ~SampleSurfaceMeshImpl() {}

    void checkPoints(size_t start, size_t end) const
    {
      float radius = 0.0f;
      float distToBoundary = 0.0f;
      int64_t numPoints = m_Points->getNumberOfVertices();
      FloatArrayType::Pointer llPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Lower");
      FloatArrayType::Pointer urPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Upper_Right");
      float* ll = llPtr->getPointer(0);
      float* ur = urPtr->getPointer(0);
      float* point = NULL;
      char code = ' ';

      for (size_t iter = start; iter < end; iter++)
      {
        // find bounding box for current feature
        GeometryMath::FindBoundingBoxOfFaces(m_Faces, m_FaceIds->getElementList(iter), ll, ur);
        GeometryMath::FindDistanceBetweenPoints(ll, ur, radius);

        // check points in vertex array to see if they are in the bounding box of the feature
        for (int64_t i = 0; i < numPoints; i++)
        {
          point = m_Points->getVertexPointer(i);
          if (m_PolyIds[i] == 0 && GeometryMath::PointInBox(point, ll, ur) == true)
          {
            code = GeometryMath::PointInPolyhedron(m_Faces, m_FaceIds->getElementList(iter), m_FaceBBs, point, ll, ur, radius, distToBoundary);
            if (code == 'i' || code == 'V' || code == 'E' || code == 'F') { m_PolyIds[i] = iter; }
          }
        }
      }
    }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      checkPoints(r.begin(), r.end());
    }
#endif
  private:

};

// Include the MOC generated file for this class
#include "moc_SampleSurfaceMesh.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SampleSurfaceMesh::SampleSurfaceMesh() :
  AbstractFilter(),
  m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SampleSurfaceMesh::~SampleSurfaceMesh()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, SIMPL::AttributeMatrixType::Face, SIMPL::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Face Labels", "SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath(), FilterParameter::RequiredArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SampleSurfaceMesh::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::dataCheck()
{
  TriangleGeom::Pointer triangles = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  QVector<IDataArray::Pointer> dataArrays;

  if(getErrorCondition() >= 0) { dataArrays.push_back(triangles->getTriangles()); }

  QVector<size_t> cDims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrays.push_back(m_SurfaceMeshFaceLabelsPtr.lock()); }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrays);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::preflight()
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
VertexGeom::Pointer SampleSurfaceMesh::generate_points()
{
  return VertexGeom::CreateGeometry(0, "ERROR_SAMPLE_SURFACE_MESH");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::assign_points(Int32ArrayType::Pointer iArray)
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName());
  SIMPL_RANDOMNG_NEW()

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();

  // pull down faces
  int64_t numFaces = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();

  // create array to hold bounding vertices for each face
  FloatArrayType::Pointer llPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Lower_Left");
  FloatArrayType::Pointer urPtr = FloatArrayType::CreateArray(3, "_INTERNAL_USE_ONLY_Upper_Right");
  float* ll = llPtr->getPointer(0);
  float* ur = urPtr->getPointer(0);
  VertexGeom::Pointer faceBBs = VertexGeom::CreateGeometry(2 * numFaces, "_INTERNAL_USE_ONLY_faceBBs");

  // walk through faces to see how many features there are
  int32_t g1 = 0, g2 = 0;
  int32_t maxFeatureId = 0;
  for (int64_t i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if (g1 > maxFeatureId) { maxFeatureId = g1; }
    if (g2 > maxFeatureId) { maxFeatureId = g2; }
  }
  // add one to account for feature 0
  int32_t numFeatures = maxFeatureId + 1;

  // create a dynamic list array to hold face lists
  Int32Int32DynamicListArray::Pointer faceLists = Int32Int32DynamicListArray::New();
  std::vector<int32_t> linkCount(numFeatures, 0);

  // fill out lists with number of references to cells
  Int32ArrayType::Pointer linkLocPtr = Int32ArrayType::CreateArray(numFaces, "_INTERNAL_USE_ONLY_cell refs");
  linkLocPtr->initializeWithZeros();
  int32_t* linkLoc = linkLocPtr->getPointer(0);

  // traverse data to determine number of faces belonging to each feature
  for (int64_t i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if (g1 > 0) { linkCount[g1]++; }
    if (g2 > 0) { linkCount[g2]++; }
  }

  // now allocate storage for the faces
  faceLists->allocateLists(linkCount);

  // traverse data again to get the faces belonging to each feature
  for (int64_t i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if (g1 > 0) { faceLists->insertCellReference(g1, (linkLoc[g1])++, i); }
    if (g2 > 0) { faceLists->insertCellReference(g2, (linkLoc[g2])++, i); }
    // find bounding box for each face
    GeometryMath::FindBoundingBoxOfFace(triangleGeom, i, ll, ur);
    faceBBs->setCoords(2 * i, ll);
    faceBBs->setCoords(2 * i + 1, ur);
  }

  // generate the list of sampling points from subclass
  VertexGeom::Pointer points = generate_points();
  if(getErrorCondition() < 0 || NULL == points.get()) { return; }
  int64_t numPoints = points->getNumberOfVertices();

  // create array to hold which polyhedron (feature) each point falls in
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();
  iArray = Int32ArrayType::CreateArray(numPoints, "_INTERNAL_USE_ONLY_polyhedronIds");
  iArray->initializeWithZeros();
  int32_t* polyIds = iArray->getPointer(0);

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numFeatures),
                      SampleSurfaceMeshImpl(triangleGeom, faceLists, faceBBs, points, polyIds), tbb::auto_partitioner());
  }
  else
#endif
  {
    SampleSurfaceMeshImpl serial(triangleGeom, faceLists, faceBBs, points, polyIds);
    serial.checkPoints(0, numFeatures);
  }

  assign_points(iArray);

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SampleSurfaceMesh::newFilterInstance(bool copyFilterParameters)
{
  SampleSurfaceMesh::Pointer filter = SampleSurfaceMesh::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMesh::getCompiledLibraryName()
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMesh::getBrandingString()
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMesh::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMesh::getGroupName()
{ return SIMPL::FilterGroups::SamplingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMesh::getSubGroupName()
{ return SIMPL::FilterSubGroups::ResolutionFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMesh::getHumanLabel()
{ return "Sample Triangle Geometry"; }
