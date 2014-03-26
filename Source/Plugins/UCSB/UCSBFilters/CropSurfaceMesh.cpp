/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "CropSurfaceMesh.h"

#include <map>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "TranslateSurfaceMesh.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropSurfaceMesh::CropSurfaceMesh() :
  AbstractFilter(),
  m_XMin(0),
  m_YMin(0),
  m_ZMin(0),
  m_XMax(0),
  m_YMax(0),
  m_ZMax(0),
  m_UpdateOrigin(true)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropSurfaceMesh::~CropSurfaceMesh()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropSurfaceMesh::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("X Min (Voxels)");
    option->setPropertyName("XMin");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Column");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Y Min (Voxels)");
    option->setPropertyName("YMin");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Row");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Z Min (Voxels)");
    option->setPropertyName("ZMin");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Plane");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("X Max (Voxels)");
    option->setPropertyName("XMax");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Column");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Y Max (Voxels)");
    option->setPropertyName("YMax");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Row");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Z Max (Voxels)");
    option->setPropertyName("ZMax");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Plane");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Update Origin");
    option->setPropertyName("UpdateOrigin");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void CropSurfaceMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setXMin( reader->readValue("XMin", getXMin()) );
  setYMin( reader->readValue("YMin", getYMin()) );
  setZMin( reader->readValue("ZMin", getZMin()) );
  setXMax( reader->readValue("XMax", getXMax()) );
  setYMax( reader->readValue("YMax", getYMax()) );
  setZMax( reader->readValue("ZMax", getZMax()) );
  setUpdateOrigin( reader->readValue("UpdateOrigin", false) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CropSurfaceMesh::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("XMin", getXMin() );
  writer->writeValue("YMin", getYMin() );
  writer->writeValue("ZMin", getZMin() );
  writer->writeValue("XMax", getXMax() );
  writer->writeValue("YMax", getYMax() );
  writer->writeValue("ZMax", getZMax() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropSurfaceMesh::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
     addErrorMessage(getHumanLabel(), "The DataContainer Object was NULL", -999);
    return;
  }
}

void CropSurfaceMesh::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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

    // We MUST have Triangles defined also.
    if(sm->getFaces().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -385);
      setErrorCondition(-385);
    }
    else
    {
    }
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropSurfaceMesh::preflight()
{
  dataCheck(true, 1, 1, 1);
  dataCheckSurfaceMesh(true, 1, 1, 1);

  VoxelDataContainer* m = getVoxelDataContainer();
  setErrorCondition(0);
  std::stringstream ss;
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  if (getXMax() < getXMin())
  {
    ss.str("");
    ss << "X Max (" << getXMax() << ") less than X Min (" << getXMin() << ")";
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5555);
  }
  if (getYMax() < getYMin())
  {
    ss.str("");
    ss << "Y Max (" << getYMax() << ") less than Y Min (" << getYMin() << ")";
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5555);
  }
  if (getZMax() < getZMin())
  {
    ss.str("");
    ss << "Z Max (" << getZMax() << ") less than Z Min (" << getZMin() << ")";
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5555);
  }
  if (getXMin() < 0)
  {
    ss.str("");
    ss << "X Min (" << getXMin() << ") less than 0";
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5555);
  }
  if (getYMin() < 0)
  {
    ss.str("");
    ss << "Y Min (" << getYMin() << ") less than 0";
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5555);
  }
  if (getZMin() < 0)
  {
    ss.str("");
    ss <<"Z Min (" << getZMin() << ") less than 0";
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5555);
  }
  if (getXMax() > (static_cast<int64_t>(m->getXPoints())-1))
  {
    ss.str("");
    ss << "The X Max you entered of " << getXMax() << " is greater than your Max X Point of " << static_cast<int64_t>(m->getXPoints())-1;
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5555);
  }
  if (getYMax() > (static_cast<int64_t>(m->getYPoints())-1))
  {
    ss.str("");
    ss << "The Y Max you entered of " << getYMax() << " is greater than your Max Y Point of " << static_cast<int64_t>(m->getYPoints())-1;
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5556);
  }
  if (getZMax() > (static_cast<int64_t>(m->getZPoints())-1))
  {
    ss.str("");
    ss << "The Z Max you entered of " << getZMax() << ") greater than your Max Z Point of " << static_cast<int64_t>(m->getZPoints())-1;
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5557);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropSurfaceMesh::execute()
{
  int err = 0;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  setErrorCondition(0);
  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if(getErrorCondition() < 0)
  {
    return;
  }

  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]), static_cast<DimType>(udims[1]), static_cast<DimType>(udims[2]), };

  // Check to see if the dims have actually changed.
  if(dims[0] == (m_XMax - m_XMin) && dims[1] == (m_YMax - m_YMin) && dims[2] == (m_ZMax - m_ZMin))
  {
    return;
  }

  std::stringstream ss;
  // Check to make sure the new dimensions are not "out of bounds" and warn the user if they are
  if (dims[0] <= m_XMax)
  {
    ss << "A Maximum value of " << m_XMax << " has been entered for the Max X which is larger than the input volume X Dimension of " << dims[0]
       << " This may lead to junk data being filled into the extra space.";
    notifyWarningMessage(ss.str(), -950);
  }
  if (dims[1] <= m_YMax)
  {
    ss.str("");
    ss << "A Maximum value of " << m_YMax << " has been entered for the Max Y which is larger than the input volume Y Dimension of " << dims[1]
       << " This may lead to junk data being filled into the extra space.";
    notifyWarningMessage(ss.str(), -950);
  }
  if (dims[2] <= m_ZMax)
  {
    ss.str("");
    ss << "A Maximum value of " << m_ZMax << " has been entered for the Max Z which is larger than the input volume Z Dimension of " << dims[2]
       << " This may lead to junk data being filled into the extra space.";
    notifyWarningMessage(ss.str(), -950);
  }

  int m_XP = ( (m_XMax - m_XMin)+1 );
  int m_YP = ( (m_YMax - m_YMin)+1 );
  int m_ZP = ( (m_ZMax - m_ZMin)+1 );

  //get mesh
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  dataCheckSurfaceMesh(false, 0, sm->getNumFaceTuples(), 0);
  DREAM3D::SurfaceMesh::VertListPointer_t oldNodesPtr = sm->getVertices();
  DREAM3D::SurfaceMesh::FaceListPointer_t oldTrianglesPtr = sm->getFaces();
  DREAM3D::SurfaceMesh::Vert_t* oldNodes = oldNodesPtr->GetPointer(0);
  DREAM3D::SurfaceMesh::Face_t* oldTriangles = oldTrianglesPtr->GetPointer(0);
  size_t numberTriangles = oldTrianglesPtr->GetNumberOfTuples();
  size_t numberNodes = oldNodesPtr->GetNumberOfTuples();

  //convert bounds from #voxel to position
  float voxelResolution[3] = {0.0f, 0.0f, 0.0f};
  m->getResolution(voxelResolution);
  float sampleOrigin[3] = {0.0f, 0.0f, 0.0f};
  m->getOrigin(sampleOrigin);
  float sampleBounds[3][2];
  sampleBounds[0][0]=m_XMin*voxelResolution[0]+sampleOrigin[0];
  sampleBounds[0][1]=(m_XMax+1)*voxelResolution[0]+sampleOrigin[0];
  sampleBounds[1][0]=m_YMin*voxelResolution[1]+sampleOrigin[1];
  sampleBounds[1][1]=(m_YMax+1)*voxelResolution[1]+sampleOrigin[1];
  sampleBounds[2][0]=m_ZMin*voxelResolution[2]+sampleOrigin[2];
  sampleBounds[2][1]=(m_ZMax+1)*voxelResolution[2]+sampleOrigin[2];

  //find good nodes / triangles
  std::vector<bool> goodTrianglesList(numberTriangles);
  std::vector<int> goodNodesList(3*numberTriangles, -1);
  int goodTriangles=0;
  for(int i=0; i<numberTriangles; i++)
  {
    goodTrianglesList[i]=false;
    int inBoundsNodes=0;

    //check each node
    for(int j=0; j<3; j++)
    {
      for(int k=0; k<3; k++)
      {
        if(oldNodes[oldTriangles[i].verts[j]].pos[k]>=sampleBounds[k][0] && oldNodes[oldTriangles[i].verts[j]].pos[k]<=sampleBounds[k][1])
        {
          inBoundsNodes++;
        }
      }
    }

    //triangle is good if all nodes are in bounds for x, y, and z
    if(9==inBoundsNodes)
    {
      goodTrianglesList[i]=true;
      goodTriangles++;
      for(int j=0; j<3; j++)
      {
        goodNodesList[3*i+j]=oldTriangles[i].verts[j];
      }
    }
  }

  //sort node list + remove duplicates
  std::sort(goodNodesList.begin(), goodNodesList.end());
  std::vector<int>::iterator it = std::unique(goodNodesList.begin(), goodNodesList.end());
  goodNodesList.resize( std::distance(goodNodesList.begin(),it) );

  //need to remove the -1 from bad nodes
  goodNodesList.erase(goodNodesList.begin(),goodNodesList.begin()+1);

  //build map of old to new nodes
  notifyStatusMessage("Building Map");
  int goodNodes=0;
  std::map<int,int> nodeMap;
  for(it=goodNodesList.begin(); it!=goodNodesList.end(); ++it)
  {
    nodeMap[*it]=goodNodes;
    goodNodes++;
  }

  //create new face + vertex containers
  DREAM3D::SurfaceMesh::FaceListPointer_t newTrianglesPtr = DREAM3D::SurfaceMesh::FaceList_t::CreateArray(goodTriangles, "New_SurfaceMeshFaces");
  DREAM3D::SurfaceMesh::VertListPointer_t newNodesPtr = DREAM3D::SurfaceMesh::VertList_t::CreateArray(goodNodes, DREAM3D::VertexData::SurfaceMeshNodes);
  DREAM3D::SurfaceMesh::Face_t* newTriangles = newTrianglesPtr.get()->GetPointer(0);
  DREAM3D::SurfaceMesh::Vert_t* newNodes = newNodesPtr.get()->GetPointer(0);

  //copy old node ids over for new triangle array
  notifyStatusMessage("Copying Data");
  std::map<int,int>::iterator mapIt;

  int index=0;

  //update face arrays
  std::list<std::string> faceArrayNames = sm->getFaceArrayNameList();
  for(int i=0; i<numberTriangles; i++)
  {
    if(goodTrianglesList[i])
    {
      newTriangles[index]=oldTriangles[i];
      for(int j=0; j<3; j++)
      {
        mapIt=nodeMap.find(oldTriangles[i].verts[j]);
        if(mapIt!=nodeMap.end())
        {
          newTriangles[index].verts[j]=mapIt->second;
        }
      }
      for (std::list<std::string>::iterator iter = faceArrayNames.begin(); iter != faceArrayNames.end(); ++iter)
      {
        std::string name = *iter;
        IDataArray::Pointer p = sm->getFaceData(*iter);
        p->CopyTuple(i, index);//since goodNodeList was sorted low->high there shouldn't be any conflicts
      }
      index++;
    }
  }
  for (std::list<std::string>::iterator iter = faceArrayNames.begin(); iter != faceArrayNames.end(); ++iter)
  {
    std::string name = *iter;
    IDataArray::Pointer p = sm->getFaceData(*iter);
    err = p->Resize(goodTriangles);
  }

  //update node arrays
  std::list<std::string> nodeArrayNames = sm->getPointArrayNameList();
  for(it=goodNodesList.begin(); it!=goodNodesList.end(); ++it)
  {
    int index_new=std::distance(goodNodesList.begin(), it);
    newNodes[index_new]=oldNodes[*it];
    for (std::list<std::string>::iterator iter = nodeArrayNames.begin(); iter != nodeArrayNames.end(); ++iter)
    {
      std::string name = *iter;
      IDataArray::Pointer p = sm->getVertexData(*iter);
      p->CopyTuple(*it, index_new);
    }
  }
  for (std::list<std::string>::iterator iter = nodeArrayNames.begin(); iter != nodeArrayNames.end(); ++iter)
  {
    std::string name = *iter;
    IDataArray::Pointer p = sm->getVertexData(*iter);
    err = p->Resize(goodNodes);
  }

  //remove other arrays for now
  std::list<std::string> edgeArrayNames = sm->getEdgeArrayNameList();
  for (std::list<std::string>::iterator iter = edgeArrayNames.begin(); iter != edgeArrayNames.end(); ++iter)
  {
    std::string name = *iter;
    sm->removeEdgeData(name);
  }
  std::list<std::string> fieldArrayNames = sm->getFieldArrayNameList();
  for (std::list<std::string>::iterator iter = fieldArrayNames.begin(); iter != fieldArrayNames.end(); ++iter)
  {
    std::string name = *iter;
    sm->removeFieldData(name);
  }

  //update faces + vertexes
  sm->removeFaceData(DREAM3D::FaceData::SurfaceMeshFaces);
  sm->removeVertexData(DREAM3D::VertexData::SurfaceMeshNodes);
  sm->setFaces(newTrianglesPtr);
  sm->setVertices(newNodesPtr);

  if(m_UpdateOrigin == true)
  {
    TranslateSurfaceMesh::Pointer translate = TranslateSurfaceMesh::New();
    translate->setSurfaceMeshDataContainer(sm);
    translate->setMessagePrefix(getMessagePrefix());
    translate->setXShift(-m_XMin * voxelResolution[0]-sampleOrigin[0]);
    translate->setYShift(-m_YMin * voxelResolution[1]-sampleOrigin[1]);
    translate->setZShift(-m_ZMin * voxelResolution[2]-sampleOrigin[2]);
    translate->execute();
    setErrorCondition(translate->getErrorCondition());
    addErrorMessages(translate->getPipelineMessages());
  }

  notifyStatusMessage("Completed");
}

