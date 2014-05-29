/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FindSurfaceMeshNodeType.h"

#include "DREAM3DLib/SurfaceMeshingFilters/MeshVertLinks.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceMeshNodeType::FindSurfaceMeshNodeType() :
  AbstractFilter(),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshNodeTypeArrayName(DREAM3D::VertexData::SurfaceMeshNodeType),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshNodeType(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceMeshNodeType::~FindSurfaceMeshNodeType()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceMeshNodeType::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindSurfaceMeshNodeType::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSurfaceMeshNodeType::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceMeshNodeType::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
    CREATE_NON_PREREQ_DATA(sm, DREAM3D, VertexData, SurfaceMeshNodeType, ss, int8_t, Int8ArrayType, 0, voxels, 1)
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceMeshNodeType::preflight()
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
void FindSurfaceMeshNodeType::execute()
{
  int err = 0;
  setErrorCondition(err);

  //get mesh
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();

  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = sm->getVertices();
  DREAM3D::SurfaceMesh::Vert_t* nodes = nodesPtr->GetPointer(0);
  size_t numberNodes = nodesPtr->GetNumberOfTuples();

  dataCheckSurfaceMesh(false, numberNodes, sm->getNumFaceTuples(), 0);

  //get mesh connectivity
  MeshVertLinks::Pointer node2TrianglePtr =sm->getMeshVertLinks();
  if (node2TrianglePtr.get() == NULL) {
    sm->buildMeshVertLinks();
    node2TrianglePtr =sm->getMeshVertLinks();
  }

  //loop over nodes getting triangles
  for(int i=0; i<numberNodes; i++)
  {
    //get node triangles
    std::set<int> grains;
    uint16_t tCount = node2TrianglePtr->getNumberOfFaces(i);
    int32_t* data = node2TrianglePtr->getFaceListPointer(i);

    //loop over triangles getting connected grains
    for(int j=0; j<tCount; j++)
    {
      int index = 2*data[j];
      grains.insert(m_SurfaceMeshFaceLabels[index+0]);
      grains.insert(m_SurfaceMeshFaceLabels[index+1]);
    }

    //save size
    if(grains.size()>4)
    {
      m_SurfaceMeshNodeType[i]=4;
    }
    else
    {
      m_SurfaceMeshNodeType[i]=grains.size();
    }

    //check if surface
    if( (*grains.begin())<0 )
    {
      m_SurfaceMeshNodeType[i]=m_SurfaceMeshNodeType[i]+10;
    }
  }

  notifyStatusMessage("Completed");
}

