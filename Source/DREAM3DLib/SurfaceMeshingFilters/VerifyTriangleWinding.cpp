/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "VerifyTriangleWinding.h"

#include <string>
#include <iostream>
#include <queue>
#include <deque>
#include <limits>

#ifdef _MSC_VER
#include <hash_set>
// using namespace stdext;
#define STDEXT stdext
#else
#include <ext/hash_set>
// using namespace __gnu_cxx;
#define STDEXT __gnu_cxx
#endif


#include "DREAM3DLib/Common/MatrixMath.h"
#include "DREAM3DLib/SurfaceMeshingFilters/util/Vector3.h"
#include "DREAM3DLib/SurfaceMeshingFilters/util/Plane.h"
#include "DREAM3DLib/SurfaceMeshingFilters/util/TriangleOps.h"
#include "DREAM3DLib/SurfaceMeshingFilters/GenerateUniqueEdges.h"
#include "DREAM3DLib/SurfaceMeshingFilters/MeshVertLinks.hpp"
#include "DREAM3DLib/SurfaceMeshingFilters/MeshTriangleNeighbors.hpp"
#include "DREAM3DLib/SurfaceMeshingFilters/ReverseTriangleWinding.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Mesh
{
  public:

    DREAM3D_SHARED_POINTERS(Mesh)
    DREAM3D_STATIC_NEW_MACRO(Mesh)
    virtual ~Mesh(){}
    int getMaxLabel() {return -1;}
    int getMinLabel() {return -1;}
    void setMaxLabel(int32_t l) { m_MaxLabel = l; }
    void setMinLabel(int32_t l) { m_MinLabel = l; }
    void incrementMaxLabel() { m_MaxLabel++; }

  protected:
    Mesh() {}
  private:
    int32_t  m_MinLabel;
    int32_t  m_MaxLabel;

    Mesh(const Mesh&); // Copy Constructor Not Implemented
    void operator=(const Mesh&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class LabelVisitorInfo
{
  public:
    //   typedef Mesh<int32_t>::Pointer                MeshPtrType;
    typedef SurfaceMesh::DataStructures::Face_t                   TriangleType;
    typedef std::vector<int32_t>      TriangleListType;

    DREAM3D_SHARED_POINTERS(LabelVisitorInfo)
    static Pointer New(int32_t label, size_t tIndex)
    {
      Pointer sharedPtr (new LabelVisitorInfo(label, tIndex));
      return sharedPtr;
    }

    virtual ~LabelVisitorInfo() {}

  private:

    int32_t m_Label;

  public:

    int32_t getLabel()
    {
      if (m_Relabeled) return m_NewLabel;
      return m_Label;
    }

    DREAM3D_INSTANCE_PROPERTY(size_t, StartIndex)
    DREAM3D_INSTANCE_PROPERTY(bool, Primed)
    DREAM3D_INSTANCE_PROPERTY(int32_t, NewLabel)
    DREAM3D_INSTANCE_PROPERTY(bool, Relabeled)

    std::set<int32_t>  m_Triangles;
    std::set<int32_t>  m_OriginalTriangleList;

    /**
 *
 * @param mesh
 * @param masterVisited
 * @return
 */
    Pointer relabelTriangles(Mesh::Pointer mesh,
                             DataArray<int32_t>* masterTriangleListPtr,
                             const std::vector<bool> &masterVisited)
    {
      size_t triangleIndex = *(m_Triangles.begin());
      int32_t newLabel = mesh->getMaxLabel() + 1;
      mesh->incrementMaxLabel();
      int32_t* masterTriangleList = masterTriangleListPtr->GetPointer(0);
      LabelVisitorInfo::Pointer p = LabelVisitorInfo::New(m_Label, triangleIndex);

      p->setPrimed(false);
      p->setNewLabel(newLabel);
      p->setRelabeled(true);
      p->m_Triangles = m_Triangles; // This will make a copy of the current set of triangles
      p->m_OriginalTriangleList = m_Triangles; // Make a copy that does NOT get updated
      bool seedIsSet = false;
      for (std::set<int32_t>::iterator triIter = p->m_Triangles.begin(); triIter != p->m_Triangles.end(); ++triIter )
      {
        if (masterTriangleList[ (*triIter) * 2] == m_Label) { masterTriangleList[ (*triIter) * 2] = newLabel; }
        if (masterTriangleList[ (*triIter) * 2 + 1] == m_Label) { masterTriangleList[ (*triIter) * 2 + 1] = newLabel; }
        if (masterVisited[*triIter]) { p->setStartIndex(*triIter); seedIsSet = true; }
      }
      if (seedIsSet) {
        p->setPrimed(true);
      }

      return p;
    }

    /**
     *
     * @param mesh
     */
    void revertTriangleLabels(DataArray<int32_t>* masterTriangleListPtr)
    {
      int32_t* masterTriangleList = masterTriangleListPtr->GetPointer(0);

      if (m_Relabeled == true)
      {
        //  std::cout << "    Reverting Label " << m_NewLabel << " To " << m_Label << std::endl;
        for (std::set<int32_t>::iterator triIter = m_OriginalTriangleList.begin(); triIter != m_OriginalTriangleList.end(); ++triIter )
        {
          if (masterTriangleList[(*triIter) * 2] == m_NewLabel) { masterTriangleList[(*triIter) * 2] = m_Label; }
          if (masterTriangleList[(*triIter) * 2 + 1] == m_NewLabel) { masterTriangleList[(*triIter) * 2 + 1] = m_Label; }
        }
        m_Relabeled = false;
        m_NewLabel = m_Label;
        m_Primed = false;
      }
    }



  protected:
    LabelVisitorInfo(int32_t label, size_t tIndex) :
      m_Label(label),
      m_StartIndex(tIndex),
      m_Primed(false),
      m_NewLabel(label),
      m_Relabeled(false)
    {
    }

  private:
    LabelVisitorInfo(const LabelVisitorInfo&); // Copy Constructor Not Implemented
    void operator=(const LabelVisitorInfo&);   // Operator '=' Not Implemented
};




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VerifyTriangleWinding::VerifyTriangleWinding() :
  SurfaceMeshFilter(),
  m_SurfaceMeshUniqueEdgesArrayName(DREAM3D::EdgeData::SurfaceMeshUniqueEdges),
  m_SurfaceMeshNodeTrianglesArrayName(DREAM3D::PointData::SurfaceMeshNodeTriangles),
  m_DoUniqueEdgesFilter(false),
  m_DoNodeTriangleConnectivityFilter(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VerifyTriangleWinding::~VerifyTriangleWinding()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyTriangleWinding::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyTriangleWinding::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyTriangleWinding::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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

    if (sm->getFaceData(DREAM3D::FaceData::SurfaceMeshTriangleLabels).get() == NULL)
    {
      setErrorCondition(-386);
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangle Grain Id Labels", getErrorCondition());
    }

    if (sm->getEdgeData(m_SurfaceMeshUniqueEdgesArrayName).get() == NULL)
    {
      m_DoUniqueEdgesFilter = true;
    } else
    {
      m_DoUniqueEdgesFilter = false;
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyTriangleWinding::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyTriangleWinding::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceMeshDataContainer* m = getSurfaceMeshDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMeshDataContainer DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  dataCheck(false, 1, 1, 1);

  //  Generate the Unique Edges
  if (m_DoUniqueEdgesFilter == true)
  {
    // There was no Edge connectivity before this filter so delete it when we are done with it
    GenerateUniqueEdges::Pointer conn = GenerateUniqueEdges::New();
    ss.str("");
    ss << getMessagePrefix() << " |->Generating Unique Edge Ids |->";
    conn->setMessagePrefix(ss.str());
    conn->setObservers(getObservers());
    conn->setSurfaceMeshUniqueEdgesArrayName(getSurfaceMeshUniqueEdgesArrayName());
    conn->setVoxelDataContainer(getVoxelDataContainer());
    conn->setSurfaceMeshDataContainer(getSurfaceMeshDataContainer());
    conn->setSolidMeshDataContainer(getSolidMeshDataContainer());
    conn->execute();
    if(conn->getErrorCondition() < 0)
    {
      setErrorCondition(conn->getErrorCondition());
      return;
    }
  }
  if (getCancel() == true) { return; }

  notifyStatusMessage("Generating Triangle List for each Node");
  // Make sure the Triangle Connectivity is created because the FindNRing algorithm needs this and will
  // assert if the data is NOT in the SurfaceMesh Data Container
  bool clearMeshVertLinks = false;
  MeshVertLinks::Pointer vertLinks = getSurfaceMeshDataContainer()->getMeshVertLinks();
  if (NULL == vertLinks.get())
  {
    clearMeshVertLinks = true; // This was not explicitly set in the pipeline so we are going to clear it when the filter is complete
    getSurfaceMeshDataContainer()->buildMeshVertLinks();
  }
  if (getCancel() == true) { return; }
  bool clearTriangleNeighbors = false;
  MeshTriangleNeighbors::Pointer triangleNeighbors = getSurfaceMeshDataContainer()->getMeshTriangleNeighborLists();
  if (NULL == triangleNeighbors)
  {
    clearTriangleNeighbors = true;
    getSurfaceMeshDataContainer()->buildMeshTriangleNeighborLists();
  }

  // Execute the actual verification step.
  notifyStatusMessage("Generating Connectivity Complete. Starting Analysis");
  verifyTriangleWinding();



  // Clean up any arrays that were designated as temp
  if (m_DoUniqueEdgesFilter == true)
  {
    IDataArray::Pointer removedConnectviity = getSurfaceMeshDataContainer()->removeEdgeData(m_SurfaceMeshUniqueEdgesArrayName);
    BOOST_ASSERT(removedConnectviity.get() != NULL);
  }
  if (clearMeshVertLinks == true)
  {
    getSurfaceMeshDataContainer()->removeMeshVertLinks();
  }
  if (clearTriangleNeighbors == true)
  {
    getSurfaceMeshDataContainer()->removeMeshTriangleNeighborLists();
  }
  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
// Groups the triangles according to which Grain they are a part of
// -----------------------------------------------------------------------------
void VerifyTriangleWinding::getLabelTriangleMap(LabelTriangleMapType &trianglesToLabelMap)
{
  SurfaceMesh::DataStructures::FaceList_t::Pointer masterTriangleList = getSurfaceMeshDataContainer()->getFaces();
  if(NULL == masterTriangleList.get())
  {
    setErrorCondition(-556);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Triangles", -556);
    return;
  }

  IDataArray::Pointer flPtr = getSurfaceMeshDataContainer()->getFaceData(DREAM3D::FaceData::SurfaceMeshTriangleLabels);
  DataArray<int32_t>* faceLabelsPtr = DataArray<int32_t>::SafePointerDownCast(flPtr.get());
  int32_t* faceLabels = faceLabelsPtr->GetPointer(0);

  int ntri = masterTriangleList->GetNumberOfTuples();
  //SurfaceMesh::DataStructures::Face_t* triangles = masterTriangleList->GetPointer(0);

  // Loop over all the triangles and group them according to which grain/region they are a part of
  for(int t = 0; t < ntri; ++t)
  {
    int32_t* label = faceLabels + t*2;
    trianglesToLabelMap[label[0]].insert(t);
    trianglesToLabelMap[label[1]].insert(t);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t VerifyTriangleWinding::getSeedTriangle(int32_t label, std::set<int32_t> &triangleIndices)
{

  SurfaceMesh::DataStructures::Vert_t* verts = getSurfaceMeshDataContainer()->getVertices()->GetPointer(0);
  SurfaceMesh::DataStructures::Face_t* triangles = getSurfaceMeshDataContainer()->getFaces()->GetPointer(0);

  IDataArray::Pointer flPtr = getSurfaceMeshDataContainer()->getFaceData(DREAM3D::FaceData::SurfaceMeshTriangleLabels);
  DataArray<int32_t>* faceLabelsPtr = DataArray<int32_t>::SafePointerDownCast(flPtr.get());
  int32_t* faceLabels = faceLabelsPtr->GetPointer(0);

  float xMax = std::numeric_limits<float>::min();
  float avgX = 0.0f;
  int32_t seedTriangleIdx = 0;

  for (std::set<int32_t>::iterator iter = triangleIndices.begin(); iter != triangleIndices.end(); ++iter)
  {
    int32_t i = *iter;
    avgX  = (verts[triangles[i].verts[0]].pos[0] + verts[triangles[i].verts[1]].pos[0] + verts[triangles[i].verts[2]].pos[0])/3.0;
    if (avgX > xMax)
    {
      xMax = avgX;
      seedTriangleIdx = i;
    }
    // m_Centroids[i*3 + 1] = (verts[triangles[i].verts[0]].pos[1] + verts[triangles[i].verts[1]].pos[1] + verts[triangles[i].verts[2]].pos[1])/3.0;
    // m_Centroids[i*3 + 2]  = (verts[triangles[i].verts[0]].pos[2] + verts[triangles[i].verts[1]].pos[2] + verts[triangles[i].verts[2]].pos[2])/3.0;
  }
  // Now we have the "right most" triangle based on x component of the centroid of the triangles for this label.
  // Lets now figure out if the normal points generally in the positive or negative X direction.

  int32_t* faceLabel = faceLabels + seedTriangleIdx*2;
  VectorType normal;
  if (faceLabel[0] == label)
  {
    normal = TriangleOps::computeNormal(verts[triangles[seedTriangleIdx].verts[0]], verts[triangles[seedTriangleIdx].verts[1]], verts[triangles[seedTriangleIdx].verts[2]]);
  }
  else
  {
    normal = TriangleOps::computeNormal(verts[triangles[seedTriangleIdx].verts[2]], verts[triangles[seedTriangleIdx].verts[1]], verts[triangles[seedTriangleIdx].verts[0]]);
  }

  if (normal.x < 0.0f)
  {
    ReverseTriangleWinding::Pointer reverse = ReverseTriangleWinding::New();
    reverse->setSurfaceMeshDataContainer(getSurfaceMeshDataContainer());
    reverse->setObservers(getObservers());
    reverse->setMessagePrefix(getMessagePrefix());
    reverse->execute();
    if (reverse->getErrorCondition() < 0)
    {
      setErrorCondition(reverse->getErrorCondition());
      notifyErrorMessage("Error Reversing the Triangle Winding", -801);
      return -1;
    }
    if (faceLabel[0] == label)
    {
      normal = TriangleOps::computeNormal(verts[triangles[seedTriangleIdx].verts[0]], verts[triangles[seedTriangleIdx].verts[1]], verts[triangles[seedTriangleIdx].verts[2]]);
    }
    else
    {
      normal = TriangleOps::computeNormal(verts[triangles[seedTriangleIdx].verts[2]], verts[triangles[seedTriangleIdx].verts[1]], verts[triangles[seedTriangleIdx].verts[0]]);
    }
    if (normal.x < 0.0f)
    {
      notifyErrorMessage("Error After attempted triangle winding reversal. Triangle normal is still oriented in wrong direction.", -802);
      seedTriangleIdx = -1;
    }
  }

  return seedTriangleIdx;
}

#define PRINT_FACE(index, triangles)\
  std::cout << " " << triangles[index].verts[0] << " " << triangles[index].verts[1] << " " << triangles[index].verts[2] << std::endl;\
  std::cout << "Triangle ID: " << index << std::endl;\
  std::cout << "Triangle.labels[0] " << triangles[index].labels[0] << std::endl;\
  std::cout << "Triangle.labels[1] " << triangles[index].labels[1] << std::endl;\

#define PRINT_VERT(index)\
  std::cout << index << " " << verts[index].pos[0] << " " << verts[index].pos[1] << " " << verts[index].pos[2] << std::endl;

#define PRINT_NORMAL(i, nodes, triangles)\
{ VectorType normal = TriangleOps::computeNormal(nodes[triangles[i].verts[0]], nodes[triangles[i].verts[1]], nodes[triangles[i].verts[2]]);\
  std::cout << normal.x << " " << normal.y << " " << normal.z << std::endl;}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VerifyTriangleWinding::verifyTriangleWinding()
{
  int err = 0;
  //  std::cout << "--------------------------------------------------------------" << std::endl;
  //  TriangleListType& masterTriangleList = *(mesh->triangles());  // Create a reference variable for better syntax
  //  NodeVector&       masterNodeList = *(mesh->nodes());


  // get the triangle definitions - use the pointer to the start of the Struct Array
  SurfaceMesh::DataStructures::FaceList_t::Pointer masterTriangleList = getSurfaceMeshDataContainer()->getFaces();
  if(NULL == masterTriangleList.get())
  {
    setErrorCondition(-556);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Triangles", -556);
    return getErrorCondition();
  }
  SurfaceMesh::DataStructures::Face_t* triangles = masterTriangleList->GetPointer(0);
  IDataArray::Pointer flPtr = getSurfaceMeshDataContainer()->getFaceData(DREAM3D::FaceData::SurfaceMeshTriangleLabels);
  DataArray<int32_t>* faceLabelsPtr = DataArray<int32_t>::SafePointerDownCast(flPtr.get());
  int32_t* faceLabels = faceLabelsPtr->GetPointer(0);

  int numTriangles = masterTriangleList->GetNumberOfTuples();

  SurfaceMesh::DataStructures::VertList_t::Pointer masterNodeListPtr = getSurfaceMeshDataContainer()->getVertices();
  if(NULL == masterNodeListPtr.get())
  {
    setErrorCondition(-555);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Nodes", -555);
    return getErrorCondition();
  }

  Mesh::Pointer mesh = Mesh::New();
  int min = std::numeric_limits<int32_t>::max();
  int max = std::numeric_limits<int32_t>::min();
  // Set the min and Max labels in the Mesh class;
  for(int n = 0; n < numTriangles; ++n)
  {
    int l = faceLabels[n*2];
    if (l < min) { min = l;}
    if (l > max) { max = l;}
    l = faceLabels[n*2 + 1];
    if (l < min) { min = l;}
    if (l > max) { max = l;}
  }
  mesh->setMaxLabel(max);
  mesh->setMinLabel(min);

  // Get a grouping of triangles by grain ID
  LabelTriangleMapType trianglesToLabelMap;
  getLabelTriangleMap(trianglesToLabelMap);
  //  for(LabelTriangleMapType::iterator iter = trianglesToLabelMap.begin(); iter != trianglesToLabelMap.end(); ++iter)
  //  {
  //    std::cout << "Grain: " << (*iter).first << "   Triangle Count: " << (*iter).second.size() << std::endl;
  //  }


  int32_t currentLabel = 0;

  // Find the first Non Zero Grain Id (Label). This is going to be our starting grain.
  for(LabelTriangleMapType::iterator iter = trianglesToLabelMap.begin(); iter != trianglesToLabelMap.end(); ++iter)
  {
    if ( (*iter).first > 0)
    {
      currentLabel = (*iter).first;
      break;
    }
  }

  std::deque<LabelVisitorInfo::Pointer> labelObjectsToVisit;

  // Keeps a list of all the triangles that have been visited.
  std::vector<bool> masterVisited(masterTriangleList->GetNumberOfTuples(), false);

  STDEXT::hash_set<int32_t> labelsVisitedSet;
  STDEXT::hash_set<int32_t> labelsToVisitSet;
  int32_t triIndex = 0;

  bool firstLabel = false;

  // Get the first triangle in the list
  // SurfaceMesh::DataStructures::Face_t& triangle = triangles[triIndex];


  // Now that we have the starting Grain, lets try and get a seed triangle that is oriented in the proper direction.
  /* Make sure the winding is correct on the first triangle of the first label that will be checked. */
  triIndex = getSeedTriangle(currentLabel, trianglesToLabelMap[currentLabel]);
  if (getErrorCondition() < 0 || triIndex < 0)
  {
    return -1;
  }

  LabelVisitorInfo::Pointer ldo = LabelVisitorInfo::New(currentLabel, triIndex);
  labelObjectsToVisit.push_back(ldo);
  LabelVisitorInfo::Pointer curLdo = LabelVisitorInfo::NullPointer();
  labelsToVisitSet.insert(currentLabel);

  float total = (float)(trianglesToLabelMap.size());
  float curPercent = 0.0;
  int progressIndex = 0;
  std::stringstream ss;

  // Start looping on all the Triangle Labels (Grain Ids) values
  while (labelObjectsToVisit.empty() == false)
  {
    if (getCancel() == true) { return -1; }
    if ( (progressIndex/total * 100.0f) > (curPercent) )
    {
      ss.str("");
      ss << static_cast<int>(progressIndex/total * 100.0f) << "% Complete";
      notifyStatusMessage(ss.str());
      curPercent += 5.0f;
    }
    ++progressIndex;

    std::map<int32_t, int32_t> neighborlabels;

    curLdo = labelObjectsToVisit.front();
    labelObjectsToVisit.pop_front();
    currentLabel = curLdo->getLabel();
    labelsToVisitSet.erase(currentLabel);
    labelsVisitedSet.insert(currentLabel);
    if (curLdo->getPrimed() == false && curLdo->getRelabeled() == false) {
      curLdo->m_Triangles = trianglesToLabelMap[currentLabel];
      curLdo->setPrimed(true);
    }
    else if (curLdo->getPrimed() == false && curLdo->getRelabeled() == true)
    {
      //    std::cout << "    Reint32_ting could not find a starting index. Using first index in triangle set" << std::endl;
      curLdo->setStartIndex( *(curLdo->m_Triangles.begin()) );
      curLdo->setPrimed(true);
    }

    if (firstLabel == true)
    {
      triIndex = 0;
      firstLabel = false;
    }
    else
    {
      triIndex = curLdo->getStartIndex();
    }
    // Get the number of triangles remaining to be visited for the current label
    std::set<int32_t>::size_type nLabelTriStart = curLdo->m_Triangles.size();


    STDEXT::hash_set<int> localVisited;
    std::deque<int> triangleDeque;
    //SurfaceMesh::DataStructures::Face_t& triangle = triangles[triIndex];
    triangleDeque.push_back(triIndex);

    while (triangleDeque.empty() == false)
    {
      int32_t triangleIndex = triangleDeque.front();
      SurfaceMesh::DataStructures::Face_t& triangle = triangles[triangleIndex];
      int32_t* faceLabel = faceLabels + (triangleIndex * 2); // Here we are getting a new pointer offset from the start of the labels array

      //   std::cout << " $ tIndex: " << triangleIndex << std::endl;

      std::vector<int32_t> adjTris = TriangleOps::findAdjacentTriangles(getSurfaceMeshDataContainer(), triangleIndex, currentLabel);
      for (TriangleListType::iterator adjTri = adjTris.begin(); adjTri != adjTris.end(); ++adjTri)
      {
        // std::cout << "  ^ AdjTri index: " << *adjTri << std::endl;
        if (masterVisited[*adjTri] == false)
        {
          //    std::cout << "   * Checking Winding: " << *adjTri << std::endl;
          if (TriangleOps::verifyWinding(triangle, triangles[*adjTri], faceLabel, faceLabels + (*adjTri * 2), currentLabel) == true)
          {
         //   std::cout << "Triangle winding flipped for triangle id = " << *adjTri << " Grain Id: " << currentLabel << std::endl;
          }
        }

        if (localVisited.find(*adjTri) == localVisited.end()
            && find(triangleDeque.begin(), triangleDeque.end(), *adjTri) == triangleDeque.end())
        {
          //std::cout << "   # Adding to Deque: " << *adjTri << std::endl;
          triangleDeque.push_back(*adjTri);
          localVisited.insert(*adjTri);
          masterVisited[*adjTri] = true;
        }
      }

      // Just add the neighbor label to a set so we end up with a list of unique
      // labels that are neighbors to the current label
      if (currentLabel != faceLabel[0]) {
        neighborlabels.insert(std::pair<int32_t, int32_t>(faceLabel[0], triangleIndex));
      }
      if (currentLabel != faceLabel[1]) {
        neighborlabels.insert(std::pair<int32_t, int32_t>(faceLabel[1], triangleIndex));
      }


      localVisited.insert(triangleIndex);
      masterVisited[triangleIndex] = true;
      curLdo->m_Triangles.erase(triangleIndex);
      triangleDeque.pop_front();
    }  // End of loop to visit all triangles in the 'currentLabel'

    // Find the Next label to push onto the end of the labels List, but ONLY if it is NOT
    // currently on the list and NOT currently on the label visited list.
    for (std::map<int32_t, int32_t>::iterator neigh = neighborlabels.begin(); neigh != neighborlabels.end(); ++neigh )
    {
      int32_t triangleIndex = (*neigh).second;
      int32_t* triangleLabel = faceLabels + triangleIndex*2;

      if ( labelsToVisitSet.find(triangleLabel[0]) == labelsToVisitSet.end()
           && (labelsVisitedSet.find(triangleLabel[0]) == labelsVisitedSet.end() ) )
      {
        // Push the int32_t value into the "set"
        labelsToVisitSet.insert(triangleLabel[0]);
        LabelVisitorInfo::Pointer l = LabelVisitorInfo::New(triangleLabel[0], triangleIndex);
        labelObjectsToVisit.push_back(l);
        // std::cout << "    Adding Label " << triangle.labels[0] << " to visit starting at tIndex " << triangleIndex << std::endl;
      }
      if ( labelsToVisitSet.find(triangleLabel[1]) == labelsToVisitSet.end()
           && (labelsVisitedSet.find(triangleLabel[1]) == labelsVisitedSet.end() ) )
      {
        // Push the int32_t value into the "set"
        labelsToVisitSet.insert(triangleLabel[1]);
        LabelVisitorInfo::Pointer l = LabelVisitorInfo::New(triangleLabel[1], triangleIndex);
        labelObjectsToVisit.push_back(l);
        //  std::cout << "    Adding Label " << triangle.labels[1] << " to visit starting at tIndex " << triangleIndex << std::endl;
      }

    }

    std::set<int32_t>::size_type nLabelTriEnd = curLdo->m_Triangles.size();
    if (nLabelTriStart == nLabelTriEnd )
    {
      // std::cout << "    !--!++! No new triangles visited for int32_t " << currentLabel << std::endl;
    }
    else if (nLabelTriEnd == 0)
    {
      //  std::cout << "    No Triangles remain to be visited for label " << std::endl;
      // If this currentLabel was the result of a "relabeling" then revert back to the original
      // label for those triangles.
      curLdo->revertTriangleLabels(faceLabelsPtr);
    }
    else
    {
      //std::cout << "    Not all triangles visited for label"  << std::endl;
      //std::cout << "    " << nLabelTriEnd << " Triangles Remaining to be visited" << std::endl;
      // At this point relabel the remaining triangles
      //      if (currentLabel == 28) {
      //        std::cout << "break" << std::endl;
      //      }
      LabelVisitorInfo::Pointer p = curLdo->relabelTriangles(mesh, faceLabelsPtr, masterVisited);
      labelObjectsToVisit.push_back(p);
      labelsToVisitSet.insert(p->getLabel());
      // Revert the current labelVisitorObject to its original label
      curLdo->revertTriangleLabels(faceLabelsPtr);
      //std::cout << "    New label " << mesh->getMaxLabel() << " added to visit" << std::endl;
    }

  } // End of Loop over each int32_t

  //std::cout << "Total: "<< total << std::endl;
  //std::cout << "labelsToVisitSet.size() :" << labelsToVisitSet.size() << std::endl;
  if (labelsToVisitSet.size() != 0) {
    //std::cout << " labelsToVisitSet:" << std::endl;
    for (STDEXT::hash_set<int32_t>::iterator iter = labelsToVisitSet.begin(); iter != labelsToVisitSet.end(); ++iter )
    {
      //std::cout << "  int32_t: " << *iter << std::endl;
    }
  }
  //std::cout << "labelsVisited.size() :" << labelsVisitedSet.size() << std::endl;


  std::set<int32_t> thelabels = TriangleOps::generateUniqueLabels(faceLabelsPtr);
  for (STDEXT::hash_set<int32_t>::iterator iter = labelsVisitedSet.begin(); iter != labelsVisitedSet.end(); ++iter )
  {
    thelabels.erase(*iter);
  }

  //std::cout << "labels NOT visited:" << std::endl;
  for (std::set<int32_t>::iterator iter = thelabels.begin(); iter != thelabels.end(); ++iter )
  {
    //std::cout << "  int32_t: " << *iter << std::endl;
  }

  // std::cout << "--------------------------------------------------------------" << std::endl;
  return err;
}


