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
#include "DREAM3DLib/SurfaceMeshingFilters/GenerateNodeTriangleConectivity.h"
#include "DREAM3DLib/Common/ManagedPointerArray.hpp"


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
    typedef Triangle                   TriangleType;
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
    Pointer relabelTriangles(Mesh::Pointer mesh, StructArray<Triangle>::Pointer masterTriangleListPtr, const std::vector<bool> &masterVisited)
    {
      size_t triangleIndex = *(m_Triangles.begin());
      int32_t newLabel = mesh->getMaxLabel() + 1;
      mesh->incrementMaxLabel();
      Triangle* masterTriangleList = masterTriangleListPtr->GetPointer(0);
      LabelVisitorInfo::Pointer p = LabelVisitorInfo::New(m_Label, triangleIndex);

      p->setPrimed(false);
      p->setNewLabel(newLabel);
      p->setRelabeled(true);
      p->m_Triangles = m_Triangles; // This will make a copy of the current set of triangles
      p->m_OriginalTriangleList = m_Triangles; // Make a copy that does NOT get updated
      bool seedIsSet = false;
      for (std::set<int32_t>::iterator triIter = p->m_Triangles.begin(); triIter != p->m_Triangles.end(); ++triIter )
      {
        Triangle& t = masterTriangleList[*triIter];
        if (t.nSpin[0] == m_Label) { t.nSpin[0] = newLabel; }
        if (t.nSpin[1] == m_Label) { t.nSpin[1] = newLabel; }
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
    void revertTriangleLabels(StructArray<Triangle>::Pointer masterTriangleListPtr)
    {
      Triangle* masterTriangleList = masterTriangleListPtr->GetPointer(0);

      if (m_Relabeled == true)
      {
        std::cout << "    Reverting Label " << m_NewLabel << " To " << m_Label << std::endl;
        for (std::set<int32_t>::iterator triIter = m_OriginalTriangleList.begin(); triIter != m_OriginalTriangleList.end(); ++triIter )
        {
          Triangle& t = masterTriangleList[*triIter];
          if (t.nSpin[0] == m_NewLabel) { t.nSpin[0] = m_Label; }
          if (t.nSpin[1] == m_NewLabel) { t.nSpin[1] = m_Label; }
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
  AbstractFilter(),
  m_SurfaceMeshUniqueEdgesArrayName(DREAM3D::CellData::SurfaceMeshUniqueEdges),
  m_SurfaceMeshNodeTrianglesArrayName(DREAM3D::CellData::SurfaceMeshNodeTriangles),
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
    if(sm->getNodes().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
      setErrorCondition(-384);
    }

    // We MUST have Triangles defined also.
    if(sm->getTriangles().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -385);
      setErrorCondition(-385);
    }

    if (sm->getCellData(m_SurfaceMeshUniqueEdgesArrayName).get() == NULL)
    {
      m_DoUniqueEdgesFilter = true;
    }

    if (sm->getCellData(m_SurfaceMeshNodeTrianglesArrayName).get() == NULL)
    {
      m_DoNodeTriangleConnectivityFilter = true;
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

  notifyStatusMessage("Generating Unique Edge Ids");
  // Generate the Unique Edges
  if (m_DoUniqueEdgesFilter == true)
  {
    GenerateUniqueEdges::Pointer conn = GenerateUniqueEdges::New();
    conn->setMessagePrefix(getMessagePrefix());
    conn->setObservers(getObservers());
    conn->setVoxelDataContainer(getVoxelDataContainer());
    conn->setSurfaceMeshDataContainer(getSurfaceMeshDataContainer());
    conn->setSolidMeshDataContainer(getSolidMeshDataContainer());
    conn->execute();
    if(conn->getErrorCondition() < 0)
    {
      setErrorCondition(-777);
      notifyErrorMessage("Error running the GenerateUniqueEdges Filter", getErrorCondition());
      return;
    }
  }

  // Get the edges array where each index of the array has a unique Vertex Id pair. The index of the array
  // is the same value as in the Struct Trianlge.e_id[]
  //  IDataArray::Pointer edgeConnPtr = m->getCellData(DREAM3D::CellData::SurfaceMeshUniqueEdges);
  //  Int32ArrayType* uniqueEdgeList = Int32ArrayType::SafePointerDownCast(edgeConnPtr.get());
  IDataArray::Pointer edgeTrianglePtr = m->getCellData(DREAM3D::CellData::SurfaceMeshEdgeTriangles);
  ManagedPointerArray<int>* edgeTriangles = ManagedPointerArray<int>::SafePointerDownCast(edgeTrianglePtr.get());

#if 1
  notifyStatusMessage("Generating Triangle List for each Node");
  // Generate the Node2Triangle Connectivity
  if (m_DoNodeTriangleConnectivityFilter == true)
  {
    GenerateNodeTriangleConectivity::Pointer conn = GenerateNodeTriangleConectivity::New();
    conn->setMessagePrefix(getMessagePrefix());
    conn->setObservers(getObservers());
    conn->setVoxelDataContainer(getVoxelDataContainer());
    conn->setSurfaceMeshDataContainer(getSurfaceMeshDataContainer());
    conn->setSolidMeshDataContainer(getSolidMeshDataContainer());
    conn->execute();
    if(conn->getErrorCondition() < 0)
    {
      setErrorCondition(-777);
      notifyErrorMessage("Error running the GenerateNodeTriangleConectivity Filter", getErrorCondition());
      return;
    }
  }
  // Get the array that has the each Node's triangle list
  IDataArray::Pointer nodeTrianglePtr = m->getCellData(DREAM3D::CellData::SurfaceMeshNodeTriangles);
  ManagedPointerArray<int>* nodeTriangles = ManagedPointerArray<int>::SafePointerDownCast(nodeTrianglePtr.get());
#endif

  debugPrintConnectivity();


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyTriangleWinding::findCommonEdge(Triangle& seed, Triangle& target)
{

}

// -----------------------------------------------------------------------------
// Groups the triangles according to which Grain they are a part of
// -----------------------------------------------------------------------------
void VerifyTriangleWinding::getLabelTriangleMap(LabelTriangleMapType &trianglesToLabelMap)
{
  StructArray<Triangle>::Pointer masterTriangleList = getSurfaceMeshDataContainer()->getTriangles();
  if(NULL == masterTriangleList.get())
  {
    setErrorCondition(-556);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Triangles", -556);
    return;
  }
  int ntri = masterTriangleList->GetNumberOfTuples();
  Triangle* triangles = masterTriangleList->GetPointer(0);

  // Loop over all the triangles and group them according to which grain/region they are a part of
  for(int t = 0; t < ntri; ++t)
  {
    Triangle& tri = triangles[t];
    trianglesToLabelMap[tri.nSpin[0]].insert(t);
    trianglesToLabelMap[tri.nSpin[1]].insert(t);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VerifyTriangleWinding::debugPrintConnectivity()
{
  int err = 0;
//  std::cout << "--------------------------------------------------------------" << std::endl;
  //  TriangleListType& masterTriangleList = *(mesh->triangles());  // Create a reference variable for better syntax
  //  NodeVector&       masterNodeList = *(mesh->nodes());
  Mesh::Pointer mesh = Mesh::New();

  // get the triangle definitions - use the pointer to the start of the Struct Array
  StructArray<Triangle>::Pointer masterTriangleList = getSurfaceMeshDataContainer()->getTriangles();
  if(NULL == masterTriangleList.get())
  {
    setErrorCondition(-556);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Triangles", -556);
    return getErrorCondition();
  }
  int numTriangles = masterTriangleList->GetNumberOfTuples();
  Triangle* triangles = masterTriangleList->GetPointer(0);

  StructArray<Node>::Pointer masterNodeListPtr = getSurfaceMeshDataContainer()->getNodes();
  if(NULL == masterNodeListPtr.get())
  {
    setErrorCondition(-555);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Nodes", -555);
    return getErrorCondition();
  }
  int numNodes = masterNodeListPtr->GetNumberOfTuples();
  StructArray<Node>& masterNodeList = *(masterNodeListPtr.get());

  int min = 268435457;
  int max = -268435457;
  // Set the min and Max labels in the Mesh class;
  for(int n = 0; n < numTriangles; ++n)
  {
    int l = triangles[n].nSpin[0];
    if (l < min) { min = l;}
    if (l > max) { max = l;}
    l = triangles[n].nSpin[1];
    if (l < min) { min = l;}
    if (l > max) { max = l;}
  }
  mesh->setMaxLabel(max);
  mesh->setMinLabel(min);



  //  std::cout << "Max int32_t: " << mesh->getMaxint32_t() << std::endl;
  //  std::cout << "Min int32_t: " << mesh->getMinint32_t() << std::endl;

  bool firstTime = true;

  // Get a grouping of triangles by grain ID
  LabelTriangleMapType trianglesToLabelMap;
  getLabelTriangleMap(trianglesToLabelMap);


  std::deque<LabelVisitorInfo::Pointer> labelObjectsToVisit;

  // Keeps a list of all the triangles that have been visited.
  std::vector<bool> masterVisited(masterTriangleList->GetNumberOfTuples(), false);

  STDEXT::hash_set<int32_t> labelsVisitedSet;
  STDEXT::hash_set<int32_t> labelsToVisitSet;
  int32_t triIndex = 0;

  bool firstLabel = true;

  // Get the first triangle in the list
  Triangle& triangle = triangles[triIndex];
  int32_t currentLabel = 0;

  //  if (inputs->addBoundBox == true)
  //  {
  //    currentLabel = t->nSpin[0];
  //    if (inputs->boundBoxValue == currentLabel)
  //    {
  //      currentLabel = t->nSpin[1];
  //    }
  //  }
  //  else
  {
    currentLabel = triangle.nSpin[0];
  }

  LabelVisitorInfo::Pointer ldo = LabelVisitorInfo::New(currentLabel, triIndex);
  labelObjectsToVisit.push_back(ldo);
  LabelVisitorInfo::Pointer curLdo = LabelVisitorInfo::NullPointer();
  labelsToVisitSet.insert(currentLabel);


  /* Make sure the winding is correct on the first triangle of the first label
   * that will be checked.
   */
  VectorType P(-1000.0, -1000.0, -1000.0);

  std::vector<int> tNodes = TriangleOps::getNodeIndices(triangle, currentLabel);
  VectorType normal = TriangleOps::computeNormal(masterNodeList[tNodes[0]], masterNodeList[tNodes[1]], masterNodeList[tNodes[2]]);
  //std::cout << "T" << triIndex <<" Normal: " << normal.x << ", " << normal.y << ", " << normal.z << std::endl;

  Plane plane(normal, masterNodeList[tNodes[0]]);
  Plane::CP where = plane.classify_point(P);
  //std::cout << "where: " << where << std::endl;
  if (Plane::CP_BACK == where)
  {
    TriangleOps::flipWinding(triangle);
  }

  float total = (float)(trianglesToLabelMap.size());
  float curPercent = 0.0;
  int progressIndex = 0;
  std::stringstream ss;

  // Start looping on all the int32_t values
  while (labelObjectsToVisit.empty() == false)
  {

    std::map<int32_t, size_t> neighborlabels;

    if ( (progressIndex/total * 100.0f) > (curPercent) )
    {
      ss.str("");
      ss << "Verifying Winding " << (progressIndex/total * 100.0f) << " Complete";
      notifyStatusMessage(ss.str());
      curPercent += 5.0f;
    }
    ++progressIndex;
    //std::cout << "Total: "<< total << "    "<< "progressIndex: " << progressIndex << std::endl;

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


    //std::cout << "==> Current int32_t: " << currentLabel << std::endl;
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
    std::set<size_t>::size_type nLabelTriStart = curLdo->m_Triangles.size();
    //std::cout << "    " << nLabelTriStart << " Triangles To Visit" << std::endl;


    STDEXT::hash_set<int> localVisited;
    std::deque<int> triangleDeque;
    triangle = triangles[triIndex];
    triangleDeque.push_back(triIndex);


    while (triangleDeque.empty() == false)
    {
      int32_t triangleIndex = triangleDeque.front();
      triangle = triangles[triangleIndex];
      //std::cout << " $ tIndex: " << triangleIndex << std::endl;

      std::vector<int32_t> adjTris = TriangleOps::findAdjacentTriangles(getSurfaceMeshDataContainer(), masterTriangleList, triangleIndex, currentLabel);
      for (TriangleListType::iterator adjTri = adjTris.begin(); adjTri != adjTris.end(); ++adjTri)
      {
        //std::cout << "  ^ AdjTri index: " << *adjTri << std::endl;
        if (masterVisited[*adjTri] == false)
        {
          //std::cout << "   * Checking Winding: " << *adjTri << std::endl;
          if (TriangleOps::verifyWinding(triangle, triangles[*adjTri], currentLabel) == true)
          {
            //std::cout << "Triangle winding flipped for triangle id = " << *adjTri << " Grain Id: " << currentLabel << std::endl;
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
      if (currentLabel != triangle.nSpin[0]) {
        neighborlabels.insert(std::pair<int32_t, int>(triangle.nSpin[0], triangleIndex));
      }
      if (currentLabel != triangle.nSpin[1]) {
        neighborlabels.insert(std::pair<int32_t, int>(triangle.nSpin[1], triangleIndex));
      }


      localVisited.insert(triangleIndex);
      masterVisited[triangleIndex] = true;
      curLdo->m_Triangles.erase(triangleIndex);
      triangleDeque.pop_front();
    }  // End of loop to visit all triangles in the 'currentLabel'

    // Find the Next label to push onto the end of the labels List, but ONLY if it is NOT
    // currently on the list and NOT currently on the label visited list.
    for (std::map<int32_t, size_t>::iterator neigh = neighborlabels.begin(); neigh != neighborlabels.end(); ++neigh )
    {
      int32_t triangleIndex = (*neigh).second;
      triangle = triangles[triangleIndex];

      if ( labelsToVisitSet.find(triangle.nSpin[0]) == labelsToVisitSet.end()
           && (labelsVisitedSet.find(triangle.nSpin[0]) == labelsVisitedSet.end() ) )
      {
        // Push the int32_t value into the "set"
        labelsToVisitSet.insert(triangle.nSpin[0]);
        LabelVisitorInfo::Pointer l = LabelVisitorInfo::New(triangle.nSpin[0], triangleIndex);
        labelObjectsToVisit.push_back(l);
       // std::cout << "    Adding Label " << triangle.nSpin[0] << " to visit starting at tIndex " << triangleIndex << std::endl;
      }
      if ( labelsToVisitSet.find(triangle.nSpin[1]) == labelsToVisitSet.end()
           && (labelsVisitedSet.find(triangle.nSpin[1]) == labelsVisitedSet.end() ) )
      {
        // Push the int32_t value into the "set"
        labelsToVisitSet.insert(triangle.nSpin[1]);
        LabelVisitorInfo::Pointer l = LabelVisitorInfo::New(triangle.nSpin[1], triangleIndex);
        labelObjectsToVisit.push_back(l);
      //  std::cout << "    Adding Label " << triangle.nSpin[1] << " to visit starting at tIndex " << triangleIndex << std::endl;
      }

    }

    std::set<size_t>::size_type nLabelTriEnd = curLdo->m_Triangles.size();
    if (nLabelTriStart == nLabelTriEnd )
    {
     // std::cout << "    !--!++! No new triangles visited for int32_t " << currentLabel << std::endl;
    }
    else if (nLabelTriEnd == 0)
    {
    //  std::cout << "    No Triangles remain to be visited for label " << std::endl;
      // If this currentLabel was the result of a "relabeling" then revert back to the original
      // label for those triangles.
      curLdo->revertTriangleLabels(masterTriangleList);
    }
    else
    {
      //std::cout << "    Not all triangles visited for label"  << std::endl;
      //std::cout << "    " << nLabelTriEnd << " Triangles Remaining to be visited" << std::endl;
      // At this point relabel the remaining triangles
      //      if (currentLabel == 28) {
      //        std::cout << "break" << std::endl;
      //      }
      LabelVisitorInfo::Pointer p = curLdo->relabelTriangles(mesh, masterTriangleList, masterVisited);
      labelObjectsToVisit.push_back(p);
      labelsToVisitSet.insert(p->getLabel());
      // Revert the current labelVisitorObject to its original label
      curLdo->revertTriangleLabels(masterTriangleList);
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


  std::set<int32_t> thelabels = TriangleOps::generateUniqueLabels(masterTriangleList);
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


