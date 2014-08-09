/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DecimateSurfaceMesh.h"


#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wredeclared-class-member"
#endif


#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/multiset_of.hpp>

#ifdef __clang__
#pragma clang diagnostic pop
#endif


#include <Eigen/Core>
#include <Eigen/LU>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/SurfaceMeshingFilters/MeshVertLinks.hpp"
#include "DREAM3DLib/SurfaceMeshingFilters/util/TriangleOps.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DecimateSurfaceMesh::DecimateSurfaceMesh() :
  AbstractFilter(),
  m_SurfaceMeshFaceNormalsArrayName(DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshFaceNormals(NULL),
  m_TrianglesKeep(25)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DecimateSurfaceMesh::~DecimateSurfaceMesh()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DecimateSurfaceMesh::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Triangles to Keep");
    option->setPropertyName("TrianglesKeep");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("%");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void DecimateSurfaceMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setTrianglesKeep( reader->readValue("TrianglesKeep", getTrianglesKeep()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DecimateSurfaceMesh::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("TrianglesKeep", getTrianglesKeep() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

void DecimateSurfaceMesh::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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

    GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshFaceNormals, ss, -387, double, DoubleArrayType, fields, 3)
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DecimateSurfaceMesh::preflight()
{
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
}

class MergeCost
{
  public:
    double cost;
    double vx;
    double vy;
    double vz;

    MergeCost() {};
    MergeCost(double c, double x, double y, double z) : cost(c), vx(x), vy(y), vz(z) {}
    MergeCost(const MergeCost&);
    ~MergeCost();
    bool operator< (const MergeCost&) const;
    void operator= (const MergeCost& other);
};

MergeCost::~MergeCost(void){}

MergeCost::MergeCost(const MergeCost& other)
{
  cost=other.cost;
  vx=other.vx;
  vy=other.vy;
  vz=other.vz;
}

bool MergeCost::operator< (const MergeCost& other) const
{
  return this->cost<other.cost;
}

void MergeCost::operator= (const MergeCost& other)
{
  this->cost=other.cost;
  this->vx=other.vx;
  this->vy=other.vy;
  this->vz=other.vz;
}

MergeCost evaluatePair(std::pair<int, int> nPair, double* q_mat, DREAM3D::SurfaceMesh::Vert_t* nodes)
{
  //calculate q_bar and fill matricies
  Eigen::Matrix4d q_bar, dQ_bar;
  int index1 = 10*nPair.first;
  int index2 = 10*nPair.second;
  double qvalues[10];
  for(int j=0; j<10; j++) {
    qvalues[j]=q_mat[index1+j]+q_mat[index2+j];
  }
  q_bar << qvalues[0], qvalues[1], qvalues[2], qvalues[3],
           qvalues[1], qvalues[4], qvalues[5], qvalues[6],
           qvalues[2], qvalues[5], qvalues[7], qvalues[8],
           qvalues[3], qvalues[6], qvalues[8], qvalues[9];

  dQ_bar << qvalues[0], qvalues[1], qvalues[2], qvalues[3],
            qvalues[1], qvalues[4], qvalues[5], qvalues[6],
            qvalues[2], qvalues[5], qvalues[7], qvalues[8],
            0, 0, 0, 1;

  //find best contraction position
  Eigen::Vector4d v_bar;
  Eigen::FullPivLU<Eigen::Matrix4d> lu(dQ_bar);
  if(lu.isInvertible()) {
    Eigen::Matrix4d MInv = dQ_bar.inverse();
    v_bar << MInv(0,3),
             MInv(1,3),
             MInv(2,3),
             1;
  } else {//choose midpoint if not invertible
    v_bar << (nodes[nPair.first].pos[0]+nodes[nPair.second].pos[0])/2,
             (nodes[nPair.first].pos[1]+nodes[nPair.second].pos[1])/2,
             (nodes[nPair.first].pos[2]+nodes[nPair.second].pos[2])/2,
             1;
  }

  return MergeCost(v_bar.dot(q_bar*v_bar), v_bar(0), v_bar(1), v_bar(2));
}

void update_kp(std::set<int> triNums, DREAM3D::SurfaceMesh::Face_t* triangles, DREAM3D::SurfaceMesh::Vert_t* nodes, double* k_p)
{
  /**
   *  kp
   *  a^2 ab ac ad
   *  ab b^2 bc bd
   *  ac bc c^2 cd
   *  ad bd cd d^2
   */
  //loop over affected triangles
  for(std::set<int>::iterator triIter = triNums.begin(); triIter!=triNums.end(); triIter++) {
    //calculate triangle normal
    VectorType normal = TriangleOps::computeNormal(nodes[triangles[*triIter].verts[0]], nodes[triangles[*triIter].verts[1]], nodes[triangles[*triIter].verts[2]]);
    double a = normal.x;
    double b = normal.y;
    double c = normal.z;

    //find plane
    double d = -( a*nodes[triangles[*triIter].verts[0]].pos[0] + b*nodes[triangles[*triIter].verts[0]].pos[1] + c*nodes[triangles[*triIter].verts[0]].pos[2] );
    int index = 10*(*triIter);

    //fill values
    k_p[index+0] = a*a;
    k_p[index+1] = a*b;
    k_p[index+2] = a*c;
    k_p[index+3] = a*d;
    k_p[index+4] = b*b;
    k_p[index+5] = b*c;
    k_p[index+6] = b*d;
    k_p[index+7] = c*c;
    k_p[index+8] = c*d;
    k_p[index+9] = d*d;
  }
}

void update_qmat(std::set<int> nodeNums, std::map<int, std::set<int> > node2Triangles, double* k_p, double* q_mat)
{
 //loop over verticies computing Q matricies
  for(std::set<int>::iterator nodeIter=nodeNums.begin(); nodeIter!=nodeNums.end(); ++nodeIter) {
    //zero qmat
    int index = 10*(*nodeIter);
    for(int i=0; i<10; i++) {
      q_mat[index+i]=0;
    }

    //get triangles associated with node
    std::set<int> triangleList = node2Triangles.find(*nodeIter)->second;

    //loop over triangles accumulating k_p's
    for(std::set<int>::iterator triIter=triangleList.begin(); triIter!=triangleList.end(); ++triIter) {
      int kpIndex = 10*(*triIter);
      for(int i=0; i<10; i++) {
        q_mat[index+i]=q_mat[index+i]+k_p[kpIndex+i];
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DecimateSurfaceMesh::execute()
{
  int err = 0;
  setErrorCondition(err);

  setErrorCondition(0);
  if(getErrorCondition() < 0)
  {
    return;
  }

  std::stringstream ss;

  //get mesh
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  dataCheckSurfaceMesh(false, 0, sm->getNumFaceTuples(), 0);
  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = sm->getVertices();
  DREAM3D::SurfaceMesh::FaceListPointer_t trianglesPtr = sm->getFaces();
  DREAM3D::SurfaceMesh::Vert_t* nodes = nodesPtr->GetPointer(0);
  DREAM3D::SurfaceMesh::Face_t* triangles = trianglesPtr->GetPointer(0);
  size_t numberTriangles = trianglesPtr->GetNumberOfTuples();
  size_t numberNodes = nodesPtr->GetNumberOfTuples();

  //loop over all triangle generating k_p matrix
  /**
   *  kp
   *  a^2 ab ac ad
   *  ab b^2 bc bd
   *  ac bc c^2 cd
   *  ad bd cd d^2
   */
  double* k_p = new double[10*numberTriangles];
  for(int i=0; i<numberTriangles; i++) {
    double a, b, c, d;
    int index;
    a = m_SurfaceMeshFaceNormals[i*3+0];
    b = m_SurfaceMeshFaceNormals[i*3+1];
    c = m_SurfaceMeshFaceNormals[i*3+2];
    d = -( a*nodes[triangles[i].verts[0]].pos[0] + b*nodes[triangles[i].verts[0]].pos[1] + c*nodes[triangles[i].verts[0]].pos[2] );
    index = 10*i;
    k_p[index+0] = a*a;
    k_p[index+1] = a*b;
    k_p[index+2] = a*c;
    k_p[index+3] = a*d;
    k_p[index+4] = b*b;
    k_p[index+5] = b*c;
    k_p[index+6] = b*d;
    k_p[index+7] = c*c;
    k_p[index+8] = c*d;
    k_p[index+9] = d*d;
  }

  //get node to triangle connectivity
  notifyStatusMessage("Building Node->Triangle Map");
  MeshVertLinks::Pointer node2TrianglePtr =sm->getMeshVertLinks();
  if (node2TrianglePtr.get() == NULL) {
    sm->buildMeshVertLinks();
    node2TrianglePtr =sm->getMeshVertLinks();
  }

  //loop over verticies computing Q matricies and converting node to triangle connectivity
  double* q_mat = new double[10*numberNodes];
  std::fill(q_mat, q_mat+10*numberNodes, 0);
  std::map<int, std::set<int> > nodeToTriangle;
  for(int i=0; i<numberNodes; i++) {
    uint16_t tCount = node2TrianglePtr->getNumberOfFaces(i);
    int32_t* data = node2TrianglePtr->getFaceListPointer(i);
    int index=10*i;
    int kpIndex;
    std::set<int> triangleSet;
    for(int j=0; j<tCount; j++) {
      kpIndex = 10*data[j];
      for(int k=0; k<10; k++) {
        q_mat[index+k]=q_mat[index+k]+k_p[kpIndex+k];
      }
      triangleSet.insert(data[j]);
    }
    nodeToTriangle.insert(std::pair<int, std::set<int> >(i,triangleSet));
  }

  //find valid pairs: I used a threshold of 0 for simplicity->only connected nodes can be contracted
  notifyStatusMessage("Finding Valid Pairs");
  typedef boost::bimap< boost::bimaps::set_of< std::pair<int,int> >, boost::bimaps::multiset_of< MergeCost > > bimap_type;
  typedef bimap_type::value_type pair_type;
  bimap_type validPairs;

  for(int i=0; i<numberNodes; i++) {
    //loop over triangles
    uint16_t tCount = node2TrianglePtr->getNumberOfFaces(i);
    int32_t* data = node2TrianglePtr->getFaceListPointer(i);
    for(int j=0; j<tCount; j++)
    {
      //loop over verticies
      for(int k=0; k<3; k++) {
        //add if condition is met
        if(i<triangles[data[j]].verts[k]) {
          validPairs.insert(pair_type(std::pair<int,int>(i, triangles[data[j]].verts[k]), evaluatePair(std::pair<int,int>(i, triangles[data[j]].verts[k]), q_mat, nodes)));
        }
      }
    }
  }

  //build list of nodes / triangles to keep
  std::set<int> goodNodes;
  for(int i=0; i<numberNodes; i++) {
    goodNodes.insert(i);
  }
  std::set<int> goodTriangles;
  for(int i=0; i<numberTriangles; i++) {
    goodTriangles.insert(i);
  }

  //contract best pair iteratively
  notifyStatusMessage("Contracting Pairs");
  int trianglesToCollapse = (1.0f-m_TrianglesKeep/100.0)*numberTriangles;
  if(trianglesToCollapse==numberTriangles) trianglesToCollapse--;
  int triangleIncrement = trianglesToCollapse/100;
  int triangleCounter=0;
  int progress=0;
  int trianglesCollapsed=0;

  //iteratively merge best pair until target reached
  while( trianglesToCollapse>trianglesCollapsed && validPairs.size()!=0 )
    {
    //update progress
    if(triangleCounter>=triangleIncrement) {
      triangleCounter=0;
      progress++;
      ss.str("");
      ss<<"Contracting Pairs: "<<progress<<"%";
      notifyStatusMessage(ss.str());
    }

    //get best pair and remove from list
    boost::bimaps::relation::structured_pair<boost::bimaps::tags::tagged<const MergeCost, boost::bimaps::relation::member_at::right>, boost::bimaps::tags::tagged<const std::pair<int, int>, boost::bimaps::relation::member_at::left>, mpl_::na, boost::bimaps::relation::mirror_layout> bestPair = *(validPairs.right.begin());
    validPairs.right.erase(validPairs.right.begin());
    int movedNode = bestPair.second.first;
    int removedNode = bestPair.second.second;

    //get connectivity of removed node and stop tracking
    std::map<int, std::set<int> >::iterator mapIter = nodeToTriangle.find(removedNode);
    if(mapIter!=nodeToTriangle.end()) {
      std::set<int> removedNodeTriangles = mapIter->second;
      nodeToTriangle.erase(mapIter);

      //get connectivity of moved node
      mapIter = nodeToTriangle.find(movedNode);
      if(mapIter!=nodeToTriangle.end()) {
        std::set<int> movedNodeTriangles = mapIter->second;

        //update position of moved node
        nodes[movedNode].pos[0]=bestPair.first.vx;
        nodes[movedNode].pos[1]=bestPair.first.vy;
        nodes[movedNode].pos[2]=bestPair.first.vz;

        //mark bad node for later deletion
        goodNodes.erase(removedNode);

        //loop over removed node's triangles updating node id and finding collased triangles
        std::set<int> collapsedTriangles;
        for(std::set<int>::iterator iter = removedNodeTriangles.begin(); iter!=removedNodeTriangles.end(); ++iter) {
          for(int j=0; j<3; j++) {
            if(triangles[(*iter)].verts[j]==removedNode) {
              triangles[(*iter)].verts[j]=movedNode;
            } else if(triangles[(*iter)].verts[j]==movedNode) {
                collapsedTriangles.insert((*iter));
            }
          }
        }
        trianglesCollapsed=trianglesCollapsed+collapsedTriangles.size();

        //remove collapsed triangles from list of both nodes
        for(std::set<int>::iterator iter = collapsedTriangles.begin(); iter!=collapsedTriangles.end(); ++iter) {
          removedNodeTriangles.erase((*iter));
          movedNodeTriangles.erase((*iter));
          triangleCounter++;
        }

        //loop over good (uncollapsed) triangles of removed node finding changed pairs
        std::set<std::pair<std::pair<int, int>, std::pair<int, int> >  > changedPairs;//node id changed
        for(std::set<int>::iterator iter = removedNodeTriangles.begin(); iter!=removedNodeTriangles.end(); ++iter) {
          int triNum = *iter;
          int node1=triangles[triNum].verts[0];
          int node2=triangles[triNum].verts[1];
          int node3=triangles[triNum].verts[2];
          if(movedNode==node1) {
            changedPairs.insert(std::pair< std::pair<int, int>,std::pair<int, int> >( std::pair<int, int>(std::min(removedNode, node2), std::max(removedNode, node2)), std::pair<int, int>(std::min(movedNode, node2), std::max(movedNode, node2))));
            changedPairs.insert(std::pair< std::pair<int, int>,std::pair<int, int> >( std::pair<int, int>(std::min(removedNode, node3), std::max(removedNode, node3)), std::pair<int, int>(std::min(movedNode, node3), std::max(movedNode, node3))));
          } else if (movedNode==node2) {
            changedPairs.insert(std::pair< std::pair<int, int>,std::pair<int, int> >( std::pair<int, int>(std::min(removedNode, node1), std::max(removedNode, node1)), std::pair<int, int>(std::min(movedNode, node1), std::max(movedNode, node1))));
            changedPairs.insert(std::pair< std::pair<int, int>,std::pair<int, int> >( std::pair<int, int>(std::min(removedNode, node3), std::max(removedNode, node3)), std::pair<int, int>(std::min(movedNode, node3), std::max(movedNode, node3))));
          } else {//removedNode==node3
            changedPairs.insert(std::pair< std::pair<int, int>,std::pair<int, int> >( std::pair<int, int>(std::min(removedNode, node1), std::max(removedNode, node1)), std::pair<int, int>(std::min(movedNode, node1), std::max(movedNode, node1))));
            changedPairs.insert(std::pair< std::pair<int, int>,std::pair<int, int> >( std::pair<int, int>(std::min(removedNode, node2), std::max(removedNode, node2)), std::pair<int, int>(std::min(movedNode, node2), std::max(movedNode, node2))));
          }
        }

        //loop over triangles checking for collapsed triangles and building list of updated pairs
        std::set<std::pair<int, int> > updatedPairs;//cost change only
        for(std::set<int>::iterator iter = movedNodeTriangles.begin(); iter!=movedNodeTriangles.end(); ++iter) {
          int triNum = *iter;
          int node1=triangles[triNum].verts[0];
          int node2=triangles[triNum].verts[1];
          int node3=triangles[triNum].verts[2];
          if(node1==movedNode) {
            updatedPairs.insert(std::pair<int, int>(std::min(movedNode, node2), std::max(movedNode, node2)));
            updatedPairs.insert(std::pair<int, int>(std::min(movedNode, node3), std::max(movedNode, node3)));
          } else if (node2==movedNode) {
            updatedPairs.insert(std::pair<int, int>(std::min(movedNode, node1), std::max(movedNode, node1)));
            updatedPairs.insert(std::pair<int, int>(std::min(movedNode, node3), std::max(movedNode, node3)));
          } else {//node3==movedNode
            updatedPairs.insert(std::pair<int, int>(std::min(movedNode, node1), std::max(movedNode, node1)));
            updatedPairs.insert(std::pair<int, int>(std::min(movedNode, node2), std::max(movedNode, node2)));
          }
        }

        //update connectivity of moved node
        movedNodeTriangles.insert(removedNodeTriangles.begin(), removedNodeTriangles.end());
        mapIter->second=movedNodeTriangles;

        //make sure moved node isn't orphan
        if(movedNodeTriangles.size()==0) {
          nodeToTriangle.erase(mapIter);
        }

        //loop over collapsed triangles resolving conflicts
        for(std::set<int>::iterator iter=collapsedTriangles.begin(); iter!=collapsedTriangles.end(); ++iter) {
          //get node / triangle
          int triNum = *iter;
          int node1=triangles[triNum].verts[0];
          int node2=triangles[triNum].verts[1];
          int node3=triangles[triNum].verts[2];

          //nodes have been updated so 2/3 nodes are now the moved node, find 3rd node
          int otherNode;
          if(node1!=movedNode) {otherNode=node1;}
          else if(node2!=movedNode) {otherNode=node2;}
          else {otherNode=node3;}

          //pairs being switched from other node & removed node ---> other node & moved node are redundant (if they exist)
          std::set<std::pair< std::pair<int, int>,std::pair<int, int> > >::iterator cPairIter = changedPairs.find(std::pair< std::pair<int, int>,std::pair<int, int> >(std::pair<int, int>(std::min(removedNode, otherNode), std::max(removedNode, otherNode)), std::pair<int, int>(std::min(movedNode, otherNode), std::max(movedNode, otherNode))));
          if(cPairIter!=changedPairs.end()) changedPairs.erase(cPairIter);

          //collapse will create a redundant pair, remove pair associated with removed node
          changedPairs.erase(std::pair< std::pair<int, int>,std::pair<int, int> >( std::pair<int, int>(std::min(removedNode, otherNode), std::max(removedNode, otherNode)), std::pair<int, int>(std::min(movedNode, otherNode), std::max(movedNode, otherNode))));
          validPairs.left.erase(std::pair<int,int>(std::min(removedNode, otherNode), std::max(removedNode, otherNode)));

          //collapsed triangles may be the only one containing the remaining pair
          ///updatedPairs.insert(std::pair<int,int>(std::min(otherNode, movedNode), std::max(otherNode, movedNode)));

          //collapsed triangles need to be removed from connectivity of other node
          mapIter = nodeToTriangle.find(otherNode);
          if(mapIter!=nodeToTriangle.end()) {
            mapIter->second.erase(triNum);

            //check for orphaned nodes (dont belong to any triangles)
            if((mapIter->second).size()==0) {
              nodeToTriangle.erase(mapIter);
              updatedPairs.erase(std::pair<int,int>(std::min(otherNode, movedNode), std::max(otherNode, movedNode)));
              validPairs.left.erase(std::pair<int,int>(std::min(otherNode, movedNode), std::max(otherNode, movedNode)));
              goodNodes.erase(otherNode);
            }
          }

          //collapsed triangles are eliminated, mark for later deletion
          goodTriangles.erase((*iter));
        }
        changedPairs.erase(std::pair<std::pair<int,int>,std::pair<int,int> >(std::pair<int,int>(movedNode, removedNode), std::pair<int,int>(movedNode, movedNode)));

        //update kp of changed triangles and q_mat of affected nodes
        update_kp(movedNodeTriangles, triangles, nodes, k_p);

        //update q_mat of moved node
        int index=10*movedNode;
        for(int j=0; j<10; j++) {
          q_mat[index+j]=0;
        }
        for(std::set<int>::iterator iter=movedNodeTriangles.begin(); iter!=movedNodeTriangles.end(); ++iter) {
          int kpIndex=10*(*iter);
          for(int j=0; j<10; j++) {
            q_mat[index+j]=q_mat[index+j]+k_p[kpIndex+j];
          }
        }

        for(std::set< std::pair<std::pair<int, int>,std::pair<int, int> > >::iterator iter=changedPairs.begin(); iter!=changedPairs.end(); ++iter) {
          validPairs.left.erase((*iter).first);
          validPairs.insert(pair_type((*iter).second, evaluatePair((*iter).second, q_mat, nodes)));
        }
        for(std::set<std::pair<int, int> >::iterator iter=updatedPairs.begin(); iter!=updatedPairs.end(); ++iter) {
          validPairs.left.erase((*iter));
          validPairs.insert(pair_type((*iter), evaluatePair((*iter), q_mat, nodes)));
        }
      }
    }
  }

  delete[] k_p;
  delete[] q_mat;

  //create new face + vertex containers
  DREAM3D::SurfaceMesh::FaceListPointer_t newTrianglesPtr = DREAM3D::SurfaceMesh::FaceList_t::CreateArray(goodTriangles.size(), DREAM3D::FaceData::SurfaceMeshFaces);
  DREAM3D::SurfaceMesh::VertListPointer_t newNodesPtr = DREAM3D::SurfaceMesh::VertList_t::CreateArray(goodNodes.size(), DREAM3D::VertexData::SurfaceMeshNodes);
  DREAM3D::SurfaceMesh::Face_t* newTriangles = newTrianglesPtr.get()->GetPointer(0);
  DREAM3D::SurfaceMesh::Vert_t* newNodes = newNodesPtr.get()->GetPointer(0);

  //fill new nodes + build map
  notifyStatusMessage("Mapping old->new nodes");
  int index=0;
  std::map<int,int> nodeMap;
  for(std::set<int>::iterator it=goodNodes.begin(); it!=goodNodes.end(); ++it) {
    nodeMap[*it]=index;
    newNodes[index]=nodes[*it];
    index++;
  }

  //fill new triangles
  notifyStatusMessage("Mapping old->new triangles");
  std::list<std::string> faceArrayNames = sm->getFaceArrayNameList();
  index=0;
  for(std::set<int>::iterator it=goodTriangles.begin(); it!=goodTriangles.end(); ++it) {
    newTriangles[index]=triangles[*it];
    for(int i=0; i<3; i++) {
      newTriangles[index].verts[i]=nodeMap.find(newTriangles[index].verts[i])->second;
    }

    //copy triangle value
    for (std::list<std::string>::iterator iter = faceArrayNames.begin(); iter != faceArrayNames.end(); ++iter)
    {
      std::string name = *iter;
      IDataArray::Pointer p = sm->getFaceData(*iter);
      p->CopyTuple(*it, index);
    }
    index++;
  }
  //shorten face arrays
  for (std::list<std::string>::iterator iter = faceArrayNames.begin(); iter != faceArrayNames.end(); ++iter)
  {
    std::string name = *iter;
    IDataArray::Pointer p = sm->getFaceData(*iter);
    err = p->Resize(goodTriangles.size());
  }

  //remove node and edge arrays
  std::list<std::string> vertexArrayNames = sm->getPointArrayNameList();
  for (std::list<std::string>::iterator iter = vertexArrayNames.begin(); iter != vertexArrayNames.end(); ++iter)
  {
    std::string name = *iter;
    sm->removeVertexData(name);
  }
  std::list<std::string> edgeArrayNames = sm->getEdgeArrayNameList();
  for (std::list<std::string>::iterator iter = edgeArrayNames.begin(); iter != edgeArrayNames.end(); ++iter)
  {
    std::string name = *iter;
    sm->removeEdgeData(name);
  }

  //update faces + vertexes
  sm->removeFaceData(DREAM3D::FaceData::SurfaceMeshFaces);
  sm->removeVertexData(DREAM3D::VertexData::SurfaceMeshNodes);
  sm->setFaces(newTrianglesPtr);
  sm->setVertices(newNodesPtr);

  //remove old connectivity
  sm->removeMeshVertLinks();

  notifyStatusMessage("Completed");
}

