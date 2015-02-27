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

#include "SampleSurfaceMesh.h"

#include <QtCore/QMap>

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/GeometryMath.h"
#include "DREAM3DLib/DataArrays/DynamicListArray.hpp"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

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
      float radius;
      int64_t numPoints = m_Points->getNumberOfVertices();
      float* ll;
      float* ur;
      float* point;
      char code;

      for(int iter = start; iter < end; iter++)
      {
        //find bounding box for current feature
        GeometryMath::FindBoundingBoxOfFaces(m_Faces, m_FaceIds->getElementList(iter), ll, ur);
        GeometryMath::FindDistanceBetweenPoints(ll, ur, radius);

        //check points in vertex array to see if they are in the bounding box of the feature
        for(int64_t i = 0; i < numPoints; i++)
        {
          point = m_Points->getVertexPointer(i);
          if(m_PolyIds[i] == 0 && GeometryMath::PointInBox(point, ll, ur) == true)
          {
            code = GeometryMath::PointInPolyhedron(m_Faces, m_FaceIds->getElementList(iter), m_FaceBBs, point, ll, ur, radius);
            if(code == 'i' || code == 'V' || code == 'E' || code == 'F') { m_PolyIds[i] = iter; }
          }
        }
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      checkPoints(r.begin(), r.end());
    }
#endif
  private:

};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SampleSurfaceMesh::SampleSurfaceMesh() :
  AbstractFilter(),
  m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
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
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("SurfaceMeshFaceLabels", "SurfaceMeshFaceLabelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  setFilterParameters(parameters);
}


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
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::dataCheck()
{
  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

  TriangleGeom::Pointer triangles = sm->getPrereqGeometry<TriangleGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0) { return; }

  // We MUST have Nodes
  if(triangles->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
  }

  // We MUST have Triangles defined also.
  if(triangles->getTriangles().get() == NULL)
  {
    setErrorCondition(-385);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
  }
  else
  {
    QVector<size_t> dims(1, 2);
    m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayPath, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
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
void SampleSurfaceMesh::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName());
  DREAM3D_RANDOMNG_NEW()

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  setErrorCondition(0);

  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();

  //pull down faces
  int64_t numFaces = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();

  //create array to hold bounding vertices for each face
  float* ll;
  float* ur;
  //VertexArray::Vert_t point;
  VertexGeom::Pointer faceBBs = VertexGeom::CreateGeometry(2 * numFaces, "faceBBs");

  //walk through faces to see how many features there are
  int g1, g2;
  int maxFeatureId = 0;
  for(int64_t i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if(g1 > maxFeatureId) { maxFeatureId = g1; }
    if(g2 > maxFeatureId) { maxFeatureId = g2; }
  }
  //add one to account for feature 0
  int numFeatures = maxFeatureId + 1;

  //create a dynamic list array to hold face lists
  Int32Int32DynamicListArray::Pointer faceLists = Int32Int32DynamicListArray::New();
  std::vector<size_t> linkCount(numFeatures, 0);
  int32_t* linkLoc;

  // fill out lists with number of references to cells
  typedef boost::shared_array<int32_t> SharedInt32Array_t;
  SharedInt32Array_t linkLocPtr(new int32_t[numFaces]);
  linkLoc = linkLocPtr.get();

  ::memset(linkLoc, 0, numFaces * sizeof(int32_t));

  // traverse data to determine number of faces belonging to each feature
  for (int64_t i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if(g1 > 0) { linkCount[g1]++; }
    if(g2 > 0) { linkCount[g2]++; }
  }

  // now allocate storage for the faces
  faceLists->allocateLists(linkCount);

  // traverse data again to get the faces belonging to each feature
  for (int64_t i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if(g1 > 0) { faceLists->insertCellReference(g1, (linkLoc[g1])++, i); }
    if(g2 > 0) { faceLists->insertCellReference(g2, (linkLoc[g2])++, i); }
    //find bounding box for each face
    GeometryMath::FindBoundingBoxOfFace(triangleGeom, i, ll, ur);
    faceBBs->setCoords(2 * i, ll);
    faceBBs->setCoords(2 * i + 1, ur);
  }

  //generate the list of sampling points fom subclass
  VertexGeom::Pointer points = generate_points();
  int64_t numPoints = points->getNumberOfVertices();

  //create array to hold which polyhedron (feature) each point falls in
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();
  iArray = Int32ArrayType::CreateArray(numPoints, "polyhedronIds");
  iArray->initializeWithZeros();
  int32_t* polyIds = iArray->getPointer(0);

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
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
VertexGeom::Pointer SampleSurfaceMesh::generate_points()
{
  return VertexGeom::CreateGeometry(0, "error");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::assign_points(Int32ArrayType::Pointer iArray)
{

}

