/* ============================================================================
 * Copyright (c) 2012 Dr. Anthony Rollet (Carnegie-Mellon University)
 * Copyright (c) 2012 Dr. Sukbin Lee (Carnegie-Mellon University)
 * Copyright (c) 2012 Dr. Jason Gruber (Carnegie-Mellon University)
 * Copyright (c) 2012 Dr. Stephen Sintay (Carnegie-Mellon University)
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
 * Neither the name of Anthony Rollet, Sukbin Lee, Jason Gruber, Stephen Sintay
 * nor the names of its contributors may be used to endorse
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

// Written by Jason Gruber jgruber@andrew.cmu.edu
//   many mods by Steve Sintay, ADR
//  added an option to turn off edge-of-model constraints
//  ADR, Nov 09; found another place to turn off dc/dt, Dec 09
//  ADR, Mar 10; added option to constrain quad points
//  ADR, May 10; added node info to output to track
//               problems with surface pull-in along GBs
//               changed the way in which constraints are added, and used
//  ADR, June 10; added edge info to compute TJ smoothing forces
//      created a new class for edges, but do not need it, in fact
//      created a function of line curvature for computing forces
//      also added option to restart from an INP
//       Feb 11; debugging surface node constraint
//  look for mobility values etc, and perhaps tweak them
// Integrated into DREAM3D as MovingFiniteElementSmoothing.{cpp|h} by
// Michael A. Jackson as part of SAIC Prime contract N00173-07-C-2068
#include "MovingFiniteElementSmoothing.h"

#include <iomanip>
#include <limits>

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/SurfaceMeshStructs.h"
#include "DREAM3DLib/Common/StructArray.hpp"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/SurfaceMeshingFilters/MeshFunctions.h"
#include "DREAM3DLib/SurfaceMeshingFilters/MeshLinearAlgebra.h"

#define ENABLE_MFE_SMOOTHING_RESTART_FILE 0

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class pvector
{
  public:
    double coefft[3];
};

typedef struct
{
    int triplenn1;
    int triplenn2;
} TripleNN;


void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ")
{
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);

  // Find first "non-delimiter".
  std::string::size_type pos = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos)
  {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));

    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);

    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
inline int delta(int i, int j)
{
  return (i == j);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename Node, typename T>
void LineCurvatureDirection(pvector& v, Node& n0, Node& n1, Node& n2)
//  returns vector along which curvature points at node n1
{
  v.coefft[0] = (2. * n1.pos[0]) - n2.pos[0] - n0.pos[0];
  v.coefft[1] = (2. * n1.pos[1]) - n2.pos[1] - n0.pos[1];
  v.coefft[2] = (2. * n1.pos[2]) - n2.pos[2] - n0.pos[2];
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename Node, typename T>
double AngleLineCurvature(Node& n0, Node& n1, Node& n2)
//  returns angle approx. to curvature at node n1
{
  T v1[3], v2[3];
  v1[0] = n0.pos[0] - n1.pos[0];
  v1[1] = n0.pos[1] - n1.pos[1];
  v1[2] = n0.pos[2] - n1.pos[2];
  v2[0] = n1.pos[0] - n2.pos[0];
  v2[1] = n1.pos[1] - n2.pos[1];
  v2[2] = n1.pos[2] - n2.pos[2];
  T d1 = NodeFunctions<Node, T>::Distance(n0, n1);
  T d2 = NodeFunctions<Node, T>::Distance(n1, n2);
  T product = sqrt((v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]) / d1 / d2);
  if(product > 1.) product = 1.;
  if(product < -1.) product = -1.;
  T angle = acos(product);
  return 2. * angle / (d1 + d2);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MovingFiniteElementSmoothing::MovingFiniteElementSmoothing() :
SurfaceMeshFilter(),
m_SurfaceMeshNodeTypeArrayName(DREAM3D::VertexData::SurfaceMeshNodeType),
m_IterationSteps(1),
m_NodeConstraints(true),
m_ConstrainSurfaceNodes(true),
m_ConstrainQuadPoints(true),
m_SmoothTripleLines(true),
m_SurfaceMeshNodeType(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MovingFiniteElementSmoothing::~MovingFiniteElementSmoothing()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MovingFiniteElementSmoothing::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Iteration Steps");
    option->setPropertyName("IterationSteps");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Apply Node Contraints");
    option->setPropertyName("NodeConstraints");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Constrain Surface Nodes");
    option->setPropertyName("ConstrainSurfaceNodes");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Constrain Quad Points");
    option->setPropertyName("ConstrainQuadPoints");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Smooth Triple Lines");
    option->setPropertyName("SmoothTripleLines");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MovingFiniteElementSmoothing::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MovingFiniteElementSmoothing::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("IterationSteps", getIterationSteps());
  writer->writeValue("ConstrainNodes", getNodeConstraints());
  writer->writeValue("ConstrainSurfaceNodes", getConstrainSurfaceNodes());
  writer->writeValue("ConstrainQuadPoints", getConstrainQuadPoints());
  writer->writeValue("SmoothTripleLines", getSmoothTripleLines());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MovingFiniteElementSmoothing::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;

  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", getErrorCondition());
  }
  else
  {
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

    if (getErrorCondition() >= 0)
    {
      // Check for Node Type Array
      int size = sm->getVertices()->GetNumberOfTuples();
      GET_PREREQ_DATA(sm, DREAM3D, VertexData, SurfaceMeshNodeType, ss, -390, int8_t, Int8ArrayType, size, 1)
    }


    if ( getConstrainQuadPoints() == true || getSmoothTripleLines() == true )
    {
      IDataArray::Pointer edges = sm->getVertexData(DREAM3D::EdgeData::SurfaceMeshEdges);
      if(edges.get() == NULL)
      {
        addErrorMessage(getHumanLabel(), "Constraining Quad Points or Triples lines requires Edges array", -385);
        setErrorCondition(-385);
      }
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MovingFiniteElementSmoothing::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
   * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MovingFiniteElementSmoothing::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);

  // This needs to get run so that our private pointers are set correctly.
  dataCheck(false, 0, 0, 0);
  if(getErrorCondition() < 0)
  {
    return;
  }


  SurfaceMeshDataContainer* m = getSurfaceMeshDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMesh DataContainer Object was NULL", -999);
    return;
  }
  DREAM3D::SurfaceMesh::VertListPointer_t floatNodesPtr = m->getVertices();
  if(NULL == floatNodesPtr.get())
  {
    setErrorCondition(-555);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Nodes", -555);
    return;
  }

  DREAM3D::SurfaceMesh::FaceListPointer_t trianglesPtr = m->getFaces();
  if(NULL == trianglesPtr.get())
  {
    setErrorCondition(-556);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Triangles", -556);
    return;
  }

  setErrorCondition(0);
  /* Place all your code to execute your filter here. */
  DREAM3D::SurfaceMesh::Vert_t* nodesF = floatNodesPtr->GetPointer(0); // Get the pointer to the from of the array so we can use [] notation

  DREAM3D::SurfaceMesh::Face_t* triangles = trianglesPtr->GetPointer(0); // Get the pointer to the from of the array so we can use [] notation

  // Data variables
  int numberNodes = floatNodesPtr->GetNumberOfTuples();
  int ntri = trianglesPtr->GetNumberOfTuples();


  StructArray<DREAM3D::SurfaceMesh::VertD_t>::Pointer nodesDPtr = StructArray<DREAM3D::SurfaceMesh::VertD_t>::CreateArray(numberNodes, "MFE_Double_Nodes");
  nodesDPtr->initializeWithZeros();
  DREAM3D::SurfaceMesh::VertD_t* nodes = nodesDPtr->GetPointer(0);

  // Copy the nodes from the 32 bit floating point to the 64 bit floating point
  for(int n = 0; n < numberNodes; ++n)
  {
    nodes[n].pos[0] = nodesF[n].pos[0];
    nodes[n].pos[1] = nodesF[n].pos[1];
    nodes[n].pos[2] = nodesF[n].pos[2];
  }

  bool isVerbose = true;

  // We need a few more arrays to support the code from CMU:
  StructArray<TripleNN>::Pointer triplennPtr = StructArray<TripleNN>::CreateArray(numberNodes, "TripleLine_Neighbors_Or_IDS");
  triplennPtr->initializeWithZeros();
  TripleNN* triplenn = triplennPtr->GetPointer(0);

//  std::vector<std::string> data;
#if 0
  // read in nodes/triangles
  std::ifstream input1(nodesFile.c_str());
  std::ifstream input2(triangleFile.c_str());
  std::ofstream velocities;
#endif
  std::string outputNodesFile("/tmp/outputNodesFile.txt");

  if(isVerbose)
  {
    std::cout << "Number of nodes: " << numberNodes << std::endl;
    std::cout << "Number of triangles: " << ntri << std::endl;
    std::cout << std::endl;
  }


  std::vector<int> tid(ntri);
  std::vector<int> nodeConstraint(numberNodes); //  0=unconstrained, 1=constrained in X, 2= in Y, 4= in Z

  //Read the nodes
#if 0
  int nodeType;
  if (isVerbose) std::cout << "reading nodes " << std::endl;
  for (int i = 0; i < nnod; i++)
  {
    input1 >> id(nodes[i]) >> nodeType >> nodes[i][0] >> nodes[i][1] >> nodes[i][2];
    //  added a junk entry, 25 vi 09; changed to keep node type, 30 xii 09
    triplenn1(nodes[i]) = 0;
    triplenn2(nodes[i]) = 0;
    type(nodes[i]) = nodeType;// duplicating info for now
  }
  if (isVerbose) std::cout << "end reading nodes" << std::endl;
  input1.close();
#endif

  if(getCancel() == true)
  {
    return;
  }



  #if 1
#if __APPLE__
  #warning Smoothing Triple Lines are NOT working because the code needs updated.
#endif
  if(m_SmoothTripleLines == true)
  {
    m_SmoothTripleLines= false;
  }
  #else

  if(m_SmoothTripleLines == true)
  {
    IDataArray::Pointer edgesDataArray = m->getPointData(DREAM3D::CellData::SurfaceMeshEdges);
    IDataArray::Pointer iEdgesDataArray = m->getPointData(DREAM3D::CellData::SurfaceMeshInternalEdges);
    if((edgesDataArray.get() == NULL || iEdgesDataArray.get() == NULL) && m_SmoothTripleLines == true)
    {
      setErrorCondition(-596);
      notifyErrorMessage("Either the Edges or Internal Edges array was NULL which means those arrays have not been created and you have selected to smooth triple lines. Disable the smoothing of triple lines.", -556);
      return;
    }

    StructArray<DREAM3D::SurfaceMesh::Edge_t> *edgesStructArray = StructArray<DREAM3D::SurfaceMesh::Edge_t>::SafePointerDownCast(edgesDataArray.get());
    DREAM3D::SurfaceMesh::Edge_t* edges = edgesStructArray->GetPointer(0);

    StructArray<DREAM3D::SurfaceMesh::Edge_t> *internalEdgesStructArray = StructArray<DREAM3D::SurfaceMesh::Edge_t>::SafePointerDownCast(iEdgesDataArray.get());
    DREAM3D::SurfaceMesh::Edge_t* iEdges = internalEdgesStructArray->GetPointer(0);

    //  Read the edges, if we are going to smooth them explicitly
#if 0
    if (isVerbose) std::cout << "reading edges " << std::endl;
    std::ifstream input3(edgesFile.c_str());
    if (input3.is_open() == false)
    {
      if (isVerbose)
      {
        std::cout << "You have asked to Smooth Triple Lines but have not provided an Edges file." << std::endl;
      }
      return -1;
    }
#endif
    int edgeCount = edgesStructArray->GetNumberOfTuples();
    int iEdgeCount = internalEdgesStructArray->GetNumberOfTuples();
    int nedges = edgeCount + iEdgeCount;
    //  input3 >> nedges;
    //  don't actually need the structure. just the information
    if(isVerbose) std::cout << "Number of edges = " << nedges << std::endl;

    int tedges = 0; // triple line edge count

    for (int i = 0; i < nedges; i++)
    {
      int nid[2] =
      { -1, -1 }, ekind = -1;
      if(i < edgeCount) // Grab from the 'Segment' array
      {
        ekind = edges[i].edgeKind;
        nid[0] = edges[i].verts[0];
        nid[1] = edges[i].verts[1];
      }
      else // Grab from the 'ISegment' Array
      {
        ekind = iEdges[i - edgeCount].edgeKind;
        nid[0] = edges[i - edgeCount].verts[0];
        nid[1] = edges[i - edgeCount].verts[1];
      }

      //input3 >> junk >> nid[0] >> nid[1] >> ekind >> spin1 >> spin2 >> spin3 >> spin4;
      if(ekind == 3)
      {
        // std::cout << std::endl << "edge#  ID1  ID2  " << i << " " << nid[0] << " " << nid[1] << std::endl ;
        tedges++;
        //  now to assign NN nodes at any node on a TJ, but not quads
        if(m_SurfaceMeshNodeType[nid[0]] == 3 || m_SurfaceMeshNodeType[nid[0]] == 13)
        {
          if(triplenn[nid[0]].triplenn1 == 0)
          {
            triplenn[nid[0]].triplenn1 = nid[1];
          }
          else
          {
            if(triplenn[nid[1]].triplenn2 != 0)
            {
              if(isVerbose)
              {
                std::cout << "triplenn[nid[1]].triplenn2 != 0 was TRUE. This is bad" << std::endl;
                setErrorCondition(-666);
                notifyErrorMessage("triplenn[nid[1]].triplenn2 != 0 was TRUE. This is bad", -666);
              }
              return;
            }
            else
            {
              triplenn[nid[0]].triplenn2 = nid[1];
            }
          }
        }
        //      if ( nodes[nid[0]] == 0 ) ;
        if(m_SurfaceMeshNodeType[nid[1]] == 3 || m_SurfaceMeshNodeType[nid[1]] == 13)
        {
          if(triplenn[nid[1]].triplenn1 == 0)
          {
            triplenn[nid[1]].triplenn1 = nid[0];
          }
          else
          {
            if(triplenn[nid[1]].triplenn2 != 0)
            {
              if(isVerbose)
              {
                std::cout << "triplenn[nid[1]].triplenn2 != 0 was TRUE. This is bad" << std::endl;
                setErrorCondition(-666);
                notifyErrorMessage("triplenn[nid[1]].triplenn2 != 0 was TRUE. This is bad", -666);
              }
              return;
            }
            else
            {
              triplenn[nid[1]].triplenn2 = nid[0];
            }
          }
        }
      }
    }
    if(isVerbose) std::cout << "... completed 1st reading edges" << std::endl;
    //  destroy(edges) ;
    //input3.close();
    //  now let's check the entries
    /*
     for ( int i = 0 ; i < nnod ; i++ ) {
     if ( type(nodes[i]) == 3 ||  type(nodes[i]) == 13 ) {
     std::cout << "node ID, NN1, NN2: " << id(nodes[i]) << " " << triplenn1(nodes[i])
     << " " << triplenn2(nodes[i]) << std::endl ;
     }
     }  */
  }
#endif

  if(getCancel() == true)
  {
    return;
  }

  int final = m_IterationSteps;
  int begin = 1;
  //  for time stepping

#if ENABLE_MFE_SMOOTHING_RESTART_FILE
  int nnodRe, nnTriRe, stepsRe;
  if(restartFile.empty() == false)
  {
    //  we have a requested re-start - node positions will be read from the INP file
    std::ifstream restartInput(restartFile.c_str());

    restartInput >> nnodRe >> nnTriRe;
    if (isVerbose) std::cout << "Check number of nodes, triangles in .inp: " << nnodRe << ", " << nnTriRe << std::endl;
    if(nnodRe != nnod)
    {
      if (isVerbose) std::cout << "Discrepancy in the number of nodes, must stop! " << std::endl;
      return -1;
    }
    else
    { // we are OK to read in
      if (isVerbose) std::cout << "reading nodes from restart INP " << std::endl;
      for (int i = 0; i < nnod; i++)
      {
        input1 >> id(nodes[i]) >> nodes[i][0] >> nodes[i][1] >> nodes[i][2];
      }
      if (isVerbose) std::cout << "end reading nodes from restart INP " << std::endl;
    }
    restartInput.close();
    if (isVerbose) std::cout << "What step are we re-starting at? " << std::endl;
    std::cin >> stepsRe;
    if(stepsRe >= final)
    {
      if (isVerbose) std::cout << "Final time less than re-start time, must stop! " << std::endl;
      return -1;
    }
    else
    {
      begin = stepsRe;
    }
  }
#endif

  if(getCancel() == true)
  {
    return;
  }

#if 0
  //Read the triangles
  if (isVerbose) std::cout << "reading triangles: " << std::endl;
  for (int i = 0; i < ntri; i++)
  {
    //  char type[3];
    int nid[3];
    //  int reg1, reg2;
    input2 >> tid[i] >> nid[0] >> nid[1] >> nid[2] >> junk >> junk >> junk >> triangles[i].region1 >> triangles[i].region2;

    triangles[i](0) = &nodes[nid[0]];
    triangles[i](1) = &nodes[nid[1]];
    triangles[i](2) = &nodes[nid[2]];
    // 	bool ok[3] = {false,false,false};
    // 	for (int j=0; j<nnod; j++) {
    // // 	  if (id[j]==nid[0]) {triangles[i](0) = &nodes[j]; triangles[i].nids[0]= nid[0]; ok[0] = true;}
    // // 	  else if (id[j]==nid[1]) {triangles[i](1) = &nodes[j]; triangles[i].nids[1]= nid[1];ok[1] = true;}
    // // 	  else if (id[j]==nid[2]) {triangles[i](2) = &nodes[j]; triangles[i].nids[2]= nid[2];ok[2] = true;}
    // 	  if (id(nodes[j])==nid[0]) {triangles[i](0) = &nodes[j];  ok[0] = true;}
    // 	  else if (id(nodes[j])==nid[1]) {triangles[i](1) = &nodes[j]; ok[1] = true;}
    // 	  else if (id(nodes[j])==nid[2]) {triangles[i](2) = &nodes[j]; ok[2] = true;}
    // 	  if (ok[0] && ok[1] && ok[2]) break;
    // 	}
  }
  if (isVerbose) std::cout << "end reading triangles" << std::endl;
  input2.close();

  if (getCancel() == true)
  {
    return;
  }
#endif

  // Find the minimum and maximum dimension of the data
  double min[3] =
  { std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max() };
  double max[3] =
  { std::numeric_limits<double>::min(), std::numeric_limits<double>::min(), std::numeric_limits<double>::min() };


  for (int i = 0; i < numberNodes; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      if(nodes[i].pos[j] < min[j])
      {
        min[j] = nodes[i].pos[j];
      }
      if(nodes[i].pos[j] > max[j])
      {
        max[j] = nodes[i].pos[j];
      }
    }
  }
  if(isVerbose)
  {
    std::cout << "Model Dimensions as Min, Max: " << std::endl;

    //  for(int i=0; i<3; i++)
    std::cout << "X (0): " << min[0] << " " << max[0] << std::endl;
    std::cout << "Y (1): " << min[1] << " " << max[1] << std::endl;
    std::cout << "Z (2): " << min[2] << " " << max[2] << std::endl;
  }
  //Allocate vectors and matricies
  int n_size = 3 * numberNodes;
  MFE::Vector<double> x(n_size), F(n_size);
  MFE::SMatrix<double> K(n_size, n_size);

  //Allocate constants for solving linear equations
  const double epsilon = 1.0; // change this if quality force too
  // high, low
  const double dt = (40.0e-6) * (10 / max[1]);
  // time step, change if mesh moves too much, little
  const double small = 1.0e-12;
  const double large = 1.0e+50;
  const double one12th = 1.0 / 12.0;
  const double tolerance = 1.0e-5;
  // Tolerance for nodes that are
  // near the RVE boundary

  double A_scale, Q_scale, TJ_scale;
  // Prefactors for quality, curvature and triple line forces
  //   don't make these two values too far different
  //  larger values should increase velocities

  // Variables for logging of quality progress
  double Q_max, Q_sum, Q_ave, Q_max_ave;
  double A, Q;
  int hist_count = 10;

  std::vector<double> Q_max_hist(hist_count);

  Q_ave = 2.9;
  Q_max_ave = 10;

  double Dihedral, Dihedral_min, Dihedral_max, Dihedral_sum, Dihedral_ave;
  //  for measuring dihedral angles, Mar 2010

  //  now we determine constraints
  for (int r = 0; r < numberNodes; r++)
  {
    nodeConstraint[r] = 0;
    if(m_NodeConstraints == true)
    { // only do this if we want the constraint on surfaces
      if(fabs(nodes[r].pos[0] - max[0]) < tolerance || fabs(nodes[r].pos[0] - min[0]) < tolerance) nodeConstraint[r] += 1;
      if(fabs(nodes[r].pos[1] - max[1]) < tolerance || fabs(nodes[r].pos[1] - min[1]) < tolerance) nodeConstraint[r] += 2;
      if(fabs(nodes[r].pos[2] - max[2]) < tolerance || fabs(nodes[r].pos[2] - min[2]) < tolerance) nodeConstraint[r] += 4;
      // debug
      if(m_SurfaceMeshNodeType[r] > 10 && nodeConstraint[r] == 0)
      {
        if(isVerbose)
        {
          std::cout << "ID " << r << " Coords: " << nodes[r].pos[0] << " " << nodes[r].pos[1] << " " << nodes[r].pos[2] << std::endl;
          std::cout << " Coord. Diffs. for X: " << fabs(nodes[r].pos[0] - max[0]) << " " << fabs(nodes[r].pos[0] - min[0]) << std::endl;
          std::cout << " Coord. Diffs. for Y: " << fabs(nodes[r].pos[1] - max[1]) << " " << fabs(nodes[r].pos[1] - min[1]) << std::endl;
          std::cout << " Coord. Diffs. for Z: " << fabs(nodes[r].pos[2] - max[2]) << " " << fabs(nodes[r].pos[2] - min[2]) << std::endl;
          std::cout << "Type, Constraint: " << m_SurfaceMeshNodeType[r] << " " << nodeConstraint[r] << std::endl;
          std::cout << "Check whether there is a problem with applying constraints ... " << std::endl;
        }
      }
      //  constraint for surface nodes
      if(m_ConstrainSurfaceNodes == true && nodeConstraint[r] != 0) { nodeConstraint[r] = 7; }
      //  totally constrain a surface node (temporary fix for connectivity issues in bounded box meshes)

    }
    if(m_SurfaceMeshNodeType[r] == DREAM3D::SurfaceMesh::NodeType::QuadPoint && m_ConstrainQuadPoints == true) { nodeConstraint[r] = 7; }
    //  constraint for quad point nodes is in all 3 coords.
  }

  // update loop
  for (size_t updates = begin; updates <= static_cast<size_t>(final); ++updates)
  {
    if(getCancel() == true)
    {
      return;
    }

    // changed arg index
    if(isVerbose)
    {
      std::cout << "Update loop: " << updates << std::endl;
    }
    ss.str("");
    ss << "Iteration: " << updates;
    notifyStatusMessage(ss.str());

    Dihedral_min = 180.;
    Dihedral_max = 0.;
    Dihedral_ave = 0.;

    TJ_scale = 19000.0; //  arbitrary choice for triple line smoothing !
    A_scale = 4000.0;
    //  designed to ramp up the weight attached to the Quality forces
    Q_scale = 500.0 + 50.0 * (double)updates / (double)final;
    // 		WAS: 	Q_scale = 1000;
    //  14 may 10: discovered that ramping up the quality forces so high
    //    leads to UNsmoothing of the mesh!

    // compute triangle contributions to K and F
    ntri = trianglesPtr->GetNumberOfTuples();
    Q_max = 0.;
    Q_sum = 0.;
    Dihedral_sum = 0.;
    Dihedral_min = 180.;
    Dihedral_max = -1.; //  added may 10, ADR
    double LDistance, deltaLDistance;

    typedef NodeFunctions<DREAM3D::SurfaceMesh::VertD_t, double> NodeFunctionsType;
    typedef TriangleFunctions<DREAM3D::SurfaceMesh::VertD_t, double> TriangleFunctionsType;
    // Loop through each of the triangles
    for (int t = 0; t < ntri; t++)
    {
      DREAM3D::SurfaceMesh::Face_t& rtri = triangles[t];
      MFE::Vector<double> n(3);
      n = TriangleFunctionsType::normal(nodes[rtri.verts[0]], nodes[rtri.verts[1]], nodes[rtri.verts[2]]);
      A = TriangleFunctionsType::area(nodes[rtri.verts[0]], nodes[rtri.verts[1]], nodes[rtri.verts[2]]); //  current Area
      Q = TriangleFunctionsType::circularity(nodes[rtri.verts[0]], nodes[rtri.verts[1]], nodes[rtri.verts[2]], A); //  current quality
      if(Q > 100.) {
        if(isVerbose)
        {
          std::cout << "Warning, tri no. " << t << " has Q= " << Q << std::endl;
        }
      }
      Q_sum += Q;
      if(Q > Q_max) { Q_max = Q; }

      Dihedral = TriangleFunctionsType::MinDihedral(nodes[rtri.verts[0]], nodes[rtri.verts[1]], nodes[rtri.verts[2]]);
      // debug
      //      std::cout << "triangle, min dihedral: " << t << ", " << Dihedral*180/PI << std::endl;
      Dihedral_sum += Dihedral;
      if(Dihedral < Dihedral_min) Dihedral_min = Dihedral;
      if(Dihedral > Dihedral_max) Dihedral_max = Dihedral;

      for (int n0 = 0; n0 < 3; n0++)
      { // for each of 3 nodes on the t^th triangle
        int i = rtri.verts[n0];
        DREAM3D::SurfaceMesh::VertD_t& node_i = nodes[i];
        for (int j = 0; j < 3; j++)
        { //  for each of the three coordinates of the node
          if(m_SmoothTripleLines == true && (m_SurfaceMeshNodeType[i] == 3 || m_SurfaceMeshNodeType[i] == 13))
          {
            //  if we are smoothing triple lines, and we have a TJ node
            LDistance = NodeFunctionsType::Distance(node_i, nodes[triplenn[i].triplenn1]) + NodeFunctionsType::Distance(nodes[triplenn[i].triplenn2], nodes[i]);
          }
          node_i.pos[j] += small;
          double Anew = TriangleFunctionsType::area(nodes[rtri.verts[0]], nodes[rtri.verts[1]], nodes[rtri.verts[2]]); //  current Area
          double Qnew = TriangleFunctionsType::circularity(nodes[rtri.verts[0]], nodes[rtri.verts[1]], nodes[rtri.verts[2]], Anew);
          if(m_SmoothTripleLines == true && (m_SurfaceMeshNodeType[i] == 3 || m_SurfaceMeshNodeType[i] == 13))
          {
            //  if we are smoothing triple lines, and we have a TJ node
            deltaLDistance = NodeFunctionsType::Distance(node_i, nodes[triplenn[i].triplenn1]) + NodeFunctionsType::Distance(nodes[triplenn[i].triplenn2], nodes[i])
                - LDistance; // change in line length
            F[3 * i + j] -= TJ_scale * deltaLDistance;
          }
          node_i.pos[j] -= small;
          double arg = (A_scale * (Anew - A) + Q_scale * (Qnew - Q) * A) / small;
          F[3 * i + j] -= arg;
        }
        for (int n1 = 0; n1 < 3; n1++)
        { //  for each of 3 nodes
          int h = rtri.verts[n1];
          for (int k = 0; k < 3; k++)
          {
            for (int j = 0; j < 3; j++)
            {
              K[3 * h + k][3 * i + j] += one12th * (1.0 + delta(i, h)) * n[j] * n[k] * A;
            }
          }
        }
      }
    }


    // add epsilon to the diagonal
    for (int r = 0; r < numberNodes; r++)
    {
      for (int s = 0; s < 3; s++)
      {
        K[3 * r + s][3 * r + s] += epsilon;
      }
    }

    // apply boundary conditions
    for (int r = 0; r < numberNodes; r++)
    {
      if(m_NodeConstraints == true || m_ConstrainQuadPoints == true)
      { // only do this if we want the constraint
        if(nodeConstraint[r] % 2 != 0)
        {
          K[3 * r][3 * r] = large;
        } // X
        if((nodeConstraint[r] / 2) % 2 != 0)
        {
          K[3 * r + 1][3 * r + 1] = large;
        } // Y
        if(nodeConstraint[r] / 4 != 0)
        {
          K[3 * r + 2][3 * r + 2] = large;
        } // Z
        //  changed  12 v 10, ADR
      }
    }

    // solve for node velocities
    int iterations = MFE::CR(K, x, F, 4000, 1.0e-5);
    if(isVerbose) std::cout << iterations << " iterations ... " << std::endl;

    //Update the quality information
    if(updates - 1 < Q_max_hist.size())
    {
      Q_max_hist[updates - 1] = Q_max;
    }
    else
    {

      //Update the history of Q_max
      for (size_t i = 0; i < Q_max_hist.size() - 1; i++)
      {
        //		std::cout << i << " "<< Q_max_hist[i] << " " << Q_max_hist[i+1] << std::endl;
        Q_max_hist[i] = Q_max_hist[i + 1];
      }

      Q_max_hist[Q_max_hist.size() - 1] = Q_max;

      //Compute the rolling average of the Q_max
      Q_max_ave = 0;
      for (size_t i = 0; i < Q_max_hist.size(); i++)
      {
        Q_max_ave += Q_max_hist[i];
      }
      Q_max_ave /= Q_max_hist.size();
    }
    Q_ave = Q_sum / ntri;

    // 	for(int i=0; i<Q_max_hist.size(); i++)
    // 	  std::cout<<"Q_Max history ... "<<Q_max_hist[i]<<std::endl;
    if(isVerbose)
    {
      std::cout << "Maximum circularity quality ... " << Q_max << "   - high is bad, low is good" << std::endl;
      std::cout << "Rolling Average circularity quality ... " << Q_max_ave << std::endl;
      std::cout << "Average circularity quality ... " << Q_ave << std::endl;
    }
    Dihedral_ave = Dihedral_sum / (float)ntri;
    if(isVerbose)
    {
      std::cout << "Smallest min. dihedral angle ... " << Dihedral_min * 180. / M_PI << "   - low is bad, high is good" << std::endl;
      std::cout << "Average min. dihedral angle ... " << Dihedral_ave * 180. / M_PI << std::endl;
      std::cout << "Largest min. dihedral angle ... " << Dihedral_max * 180. / M_PI << std::endl;

      std::cout << std::endl;
    }
#if 0
    //Output velocities for examination
    std::ostringstream iter_stream;
    std::string iter_string;

    // write the iteration to a string
    iter_stream << updates;
    iter_string = iter_stream.str();

    // extract the basename from the provided filename
    std::string infile = outputNodesFile;
    std::vector<std::string> tokens;
    std::string delimiters = "."; // Only a period
    std::string basename;
    tokenize(infile, tokens, delimiters);

    if(tokens.size() > 2)
    {
      if(isVerbose) std::cout << "WARNING: multiple \".\" in file name: " << infile << std::endl;
      if(isVerbose) std::cout << "\t Using: ." << tokens.front() << "As basename" << std::endl;
      basename = tokens.front();
    }
    else if(tokens.size() <= 2)
    {
      basename = tokens.front();
    }

    // put it all back together
    std::string iterFileName;
    if(updates < 9)
    {
      iterFileName = basename + "_0" + iter_string + ".inp";
    }
    else
    {
      iterFileName = basename + "_" + iter_string + ".inp";
    }
#endif

    // update node positions
    for (int r = 0; r < numberNodes; r++)
    {
      //	  velocityfile << r << " ";
      for (int s = 0; s < 3; s++)
      {
        double bc_dt = dt;
        if(m_NodeConstraints == true)
        { // only do this if we want the constraint
          /*
           if( (fabs(nodes[r][s] - max[s]) < tolerance)
           || (fabs(nodes[r][s] - min[s]) < tolerance)) bc_dt = 0.0 ;
           */
          if(s == 0 && nodeConstraint[r] % 2 != 0) bc_dt = 0.0; // X
          if(s == 1 && (nodeConstraint[r] / 2) % 2 != 0) bc_dt = 0.0; // Y
          if(s == 2 && nodeConstraint[r] / 4 != 0) bc_dt = 0.0; // Z
          //  changed  12 v 10, ADR
        }

        if(fabs(dt * x[3 * r + s]) > 1.0)
        {
          nodes[r].pos[s] += 0.0;
        }
        else if(fabs(dt * x[3 * r + s]) < 1.0)
        {
          nodes[r].pos[s] += bc_dt * x[3 * r + s];
        }
        //		velocityfile  << std::scientific << std::setw(4)
        //			  << std::setprecision(4) << F[3*r+s] << "\t"<< x[3*r+s] <<"\t";
      }
      //	  velocityfile << std::endl;
    }
    //	velocityfile.close();

#if 0
    if(!((updates) % 10))
    {
      // Open the outputfile
      std::ofstream inpfile;
      inpfile.open(iterFileName.c_str());
      if(!inpfile)
      {
        if(isVerbose) std::cout << "Failed to open: " << iterFileName << std::endl;
        return;
      }

      //      inpfile << nnod <<" "<< ntri <<" 0 2 0"<< std::endl ;
      inpfile << nnod << " " << ntri << " 2 2 0" << std::endl; //  now that we have NODE properties
      for (int i = 0; i < nnod; i++)
      {
        inpfile << i << " " << nodes[i].pos[0] << " " << nodes[i].pos[1] << " " << nodes[i].pos[2] << std::endl;
      }

      for (int i = 0; i < ntri; i++)
      {
        //		inpfile<<i<<" "<<triangles[i].region1>>triangles[i].region2<<" tri ";
        inpfile << i << " 0 tri ";
        inpfile << triangles[i].verts[0] << " ";
        inpfile << triangles[i].verts[1] << " ";
        inpfile << triangles[i].verts[2] << std::endl;
        //inpfile<<triangles[i].region1<<" ";
        //inpfile<<triangles[i].region2<<std::endl;
      }

      //  information on nodes
      inpfile << "2 1 1" << std::endl;
      inpfile << "Node_Type, none" << std::endl;
      inpfile << "Node_Constraint, none" << std::endl;
      for (int i = 0; i < nnod; i++)
      {
        inpfile << i << " " << nodes[i].nodeKind << " " << nodeConstraint[i] << std::endl;
      }

      //  information on elements (triangles)
      inpfile << "2 1 1" << std::endl;
      inpfile << "minID, none" << std::endl;
      inpfile << "maxID, none" << std::endl;
      for (int i = 0; i < ntri; i++) // ADR  25 jun 09
      {
        inpfile << i << " " << triangles[i].nSpin[0] << " " << triangles[i].nSpin[1] << std::endl;
      }
      inpfile.close();
    }
#endif
  }

#if 0
  std::ofstream inpfile;
  inpfile.open(outputNodesFile.c_str());
  if(!inpfile)
  {
    if(isVerbose) std::cout << "Failed to open nodes output: " << outputNodesFile << std::endl;
    return;
  }

  //   std::ofstream output(argv[2]);
  inpfile << nnod << std::endl;
  for (int i = 0; i < nnod; i++)
  {
    inpfile << std::setw(6) << std::fixed << "\t" << i << "\t" << nodes[i].nodeKind << "\t" << nodes[i].pos[0] << "\t" << nodes[i].pos[1] << "\t"
        << nodes[i].pos[2] << std::endl;
  }
  inpfile.close();
#endif

  if(isVerbose)
  {
    std::cout << "Finished the smoothing " << std::endl;
    std::cout << "You can combine the nodes and triangles to do further analysis or smoothing" << std::endl;
    // std::cout << "Look for " << outputNodesFile << std::endl;
  }

  // Copy the nodes from the 64 bit floating point to the 32 bit floating point
  for(int n = 0; n < numberNodes; ++n)
  {
    nodesF[n].pos[0] = nodes[n].pos[0];
    nodesF[n].pos[1] = nodes[n].pos[1];
    nodesF[n].pos[2] = nodes[n].pos[2];
  }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

