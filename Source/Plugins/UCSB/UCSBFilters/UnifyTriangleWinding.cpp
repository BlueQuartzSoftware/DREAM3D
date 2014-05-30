/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "UnifyTriangleWinding.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UnifyTriangleWinding::UnifyTriangleWinding() :
  AbstractFilter(),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UnifyTriangleWinding::~UnifyTriangleWinding()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UnifyTriangleWinding::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void UnifyTriangleWinding::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int UnifyTriangleWinding::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UnifyTriangleWinding::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-383);
  }
  else
  {
    // We MUST have Nodes
    if(sm->getVertices().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
      setErrorCondition(-384);
    }

    //Must have triangles
    if(sm->getFaces().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -384);
      setErrorCondition(-385);
    }

    GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshFaceLabels, ss, -386, int32_t, Int32ArrayType, fields, 2)
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UnifyTriangleWinding::preflight()
{
  dataCheckSurfaceMesh(true, 1, 1, 1);

  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  setErrorCondition(0);
  std::stringstream ss;
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UnifyTriangleWinding::execute()
{
  int err = 0;
  setErrorCondition(err);

  //get mesh
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();

  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = sm->getVertices();
  DREAM3D::SurfaceMesh::FaceListPointer_t trianglesPtr = sm->getFaces();
  DREAM3D::SurfaceMesh::Vert_t* nodes = nodesPtr->GetPointer(0);
  DREAM3D::SurfaceMesh::Face_t* triangles = trianglesPtr->GetPointer(0);
  size_t numberNodes = nodesPtr->GetNumberOfTuples();

  dataCheckSurfaceMesh(false, numberNodes, sm->getNumFaceTuples(), 0);

  //get mesh connectivity
  MeshVertLinks::Pointer node2TrianglePtr =sm->getMeshVertLinks();
  if (node2TrianglePtr.get() == NULL) {
    sm->buildMeshVertLinks();
    node2TrianglePtr =sm->getMeshVertLinks();
  }

  //loop over faces
  int numberFaces = sm->getNumFaceTuples();
  for(int i=0; i<numberFaces; i++)
  {
    //arbitrary convention: gid label1 < gid label2
    if(m_SurfaceMeshFaceLabels[2*i+1]>m_SurfaceMeshFaceLabels[2*i+0])
    {
      //swap face labels
      int32_t fId = m_SurfaceMeshFaceLabels[2*i+0];
      m_SurfaceMeshFaceLabels[2*i+0]=m_SurfaceMeshFaceLabels[2*i+1];
      m_SurfaceMeshFaceLabels[2*i+1]=fId;

      //reverse winding
      int nId = triangles[i].verts[2];
      triangles[i].verts[2]=triangles[i].verts[1];
      triangles[i].verts[1]=nId;
    }
  }

  notifyStatusMessage("Completed");
}

