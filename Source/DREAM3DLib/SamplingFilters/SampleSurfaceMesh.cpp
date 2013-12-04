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
#include "DREAM3DLib/DataContainers/DynamicListArray.hpp"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

class SampleSurfaceMeshImpl
{
    FaceArray::Pointer m_Faces;
    Int32DynamicListArray::Pointer m_FaceIds;
    VertexArray::Pointer m_FaceBBs;
    VertexArray::Pointer m_Points;
    int32_t* m_PolyIds;

  public:
    SampleSurfaceMeshImpl(FaceArray::Pointer faces, Int32DynamicListArray::Pointer faceIds, VertexArray::Pointer faceBBs, VertexArray::Pointer points, int32_t* polyIds) :
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
      int numPoints = m_Points->count();
      VertexArray::Vert_t ll, ur;
      VertexArray::Vert_t point;
      char code;

      for(int iter = start; iter < end; iter++)
      {
        //find bounding box for current feature
        GeometryMath::FindBoundingBoxOfFaces(m_Faces, m_FaceIds->getElementList(iter), ll, ur);
        GeometryMath::FindDistanceBetweenPoints(ll, ur, radius);

        //check points in vertex array to see if they are in the bounding box of the feature
        for(int i = 0; i < numPoints; i++)
        {
          point = m_Points->getVert(i);
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
  m_SurfaceDataContainerName(DREAM3D::Defaults::SurfaceDataContainerName),
  m_FaceAttributeMatrixName(DREAM3D::Defaults::FaceAttributeMatrixName),
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

  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
void SampleSurfaceMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SampleSurfaceMesh::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::dataCheck()
{
  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getSurfaceDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

  // We MUST have Nodes
  if(sm->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
  }

  // We MUST have Triangles defined also.
  if(sm->getFaces().get() == NULL)
  {
    setErrorCondition(-385);
    addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
  }
  else
  {
    QVector<int> dims(1, 2);
    m_SurfaceMeshFaceLabelsPtr = sattrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayName, -386, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::preflight()
{
  setErrorCondition(0);

  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getSurfaceDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

  dataCheck(false, 1, 1, 1);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::execute()
{
  int err = 0;
  setErrorCondition(err);
  DREAM3D_RANDOMNG_NEW()

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  if(NULL == sm)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "SurfaceDataContainer is missing", getErrorCondition());
  }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  setErrorCondition(0);

  //pull down faces
  FaceArray::Pointer faces = sm->getFaces();
  int numFaces = sm->getAttributeMatrix(getFaceAttributeMatrixName())->getNumTuples();

  dataCheck(true, numFaces, 0, 0);

  //create array to hold bounding vertices for each face
  VertexArray::Vert_t ll, ur;
  VertexArray::Vert_t point;
  VertexArray::Pointer faceBBs = VertexArray::CreateArray(2 * numFaces, "faceBBs");

  //walk through faces to see how many features there are
  int g1, g2;
  int maxFeatureId = 0;
  for(int i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if(g1 > maxFeatureId) { maxFeatureId = g1; }
    if(g2 > maxFeatureId) { maxFeatureId = g2; }
  }
  //add one to account for feature 0
  int numFeatures = maxFeatureId + 1;

  //create a dynamic list array to hold face lists
  Int32DynamicListArray::Pointer faceLists = Int32DynamicListArray::New();
  QVector<uint16_t> linkCount(numFeatures, 0);
  unsigned short* linkLoc;

  // fill out lists with number of references to cells
  typedef boost::shared_array<unsigned short> SharedShortArray_t;
  SharedShortArray_t linkLocPtr(new unsigned short[numFaces]);
  linkLoc = linkLocPtr.get();

  ::memset(linkLoc, 0, numFaces * sizeof(unsigned short));

  // traverse data to determine number of faces belonging to each feature
  for (int i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if(g1 > 0) { linkCount[g1]++; }
    if(g2 > 0) { linkCount[g2]++; }
  }

  // now allocate storage for the faces
  faceLists->allocateLists(linkCount);

  // traverse data again to get the faces belonging to each feature
  for (int i = 0; i < numFaces; i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2 * i];
    g2 = m_SurfaceMeshFaceLabels[2 * i + 1];
    if(g1 > 0) { faceLists->insertCellReference(g1, (linkLoc[g1])++, i); }
    if(g2 > 0) { faceLists->insertCellReference(g2, (linkLoc[g2])++, i); }
    //find bounding box for each face
    GeometryMath::FindBoundingBoxOfFace(faces, i, ll, ur);
    faceBBs->setCoords(2 * i, ll.pos);
    faceBBs->setCoords(2 * i + 1, ur.pos);
  }

  //generate the list of sampling points fom subclass
  VertexArray::Pointer points = generate_points();
  int numPoints = points->count();

  //create array to hold which polyhedron (feature) each point falls in
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();
  iArray = Int32ArrayType::CreateArray(numPoints, "polyhedronIds");
  iArray->initializeWithZeros();
  int32_t* polyIds = iArray->getPointer(0);

  char code;
  float radius;

  int zStride, yStride;

  int count = 0;
  float coords[3];

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numFeatures),
                      SampleSurfaceMeshImpl(faces, faceLists, faceBBs, points, polyIds), tbb::auto_partitioner());

  }
  else
#endif
  {
    SampleSurfaceMeshImpl serial(faces, faceLists, faceBBs, points, polyIds);
    serial.checkPoints(0, numFeatures);
  }

  assign_points(iArray);

  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexArray::Pointer SampleSurfaceMesh::generate_points()
{
  return VertexArray::CreateArray(0, "error");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::assign_points(Int32ArrayType::Pointer iArray)
{

}

