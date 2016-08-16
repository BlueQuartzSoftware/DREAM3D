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

#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Geometry/MeshStructs.h"
#include "SIMPLib/DataArrays/StructArray.hpp"

#include "SurfaceMeshing/SurfaceMeshingFilters/MeshFunctions.h"
#include "SurfaceMeshing/SurfaceMeshingFilters/MeshLinearAlgebra.h"

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
  if(product > 1.) { product = 1.; }
  if(product < -1.) { product = -1.; }
  T angle = acos(product);
  return 2. * angle / (d1 + d2);
}

// Include the MOC generated file for this class
#include "moc_MovingFiniteElementSmoothing.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MovingFiniteElementSmoothing::MovingFiniteElementSmoothing() :
  SurfaceMeshFilter(),
  m_IterationSteps(1),
  m_NodeConstraints(true),
  m_ConstrainSurfaceNodes(true),
  m_ConstrainQuadPoints(true),
  m_SmoothTripleLines(true),
  m_SurfaceMeshNodeTypeArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, SIMPL::VertexData::SurfaceMeshNodeType),
  m_SurfaceMeshNodeTypeArrayName(SIMPL::VertexData::SurfaceMeshNodeType),
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
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Iteration Steps", IterationSteps, FilterParameter::Uncategorized, MovingFiniteElementSmoothing));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Apply Node Contraints", NodeConstraints, FilterParameter::Uncategorized, MovingFiniteElementSmoothing));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Constrain Surface Nodes", ConstrainSurfaceNodes, FilterParameter::Uncategorized, MovingFiniteElementSmoothing));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Constrain Quad Points", ConstrainQuadPoints, FilterParameter::Uncategorized, MovingFiniteElementSmoothing));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Smooth Triple Lines", SmoothTripleLines, FilterParameter::Uncategorized, MovingFiniteElementSmoothing));
  parameters.push_back(SeparatorFilterParameter::New("Required Information", FilterParameter::Uncategorized));
  parameters.push_back(DataArraySelectionFilterParameter::New("SurfaceMeshNodeType", "SurfaceMeshNodeTypeArrayPath", getSurfaceMeshNodeTypeArrayPath(), FilterParameter::Uncategorized, SIMPL_BIND_SETTER(MovingFiniteElementSmoothing, this, SurfaceMeshNodeTypeArrayPath), SIMPL_BIND_GETTER(MovingFiniteElementSmoothing, this, SurfaceMeshNodeTypeArrayPath)));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MovingFiniteElementSmoothing::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshNodeTypeArrayPath(reader->readDataArrayPath("SurfaceMeshNodeTypeArrayPath", getSurfaceMeshNodeTypeArrayPath() ) );
  setIterationSteps( reader->readValue("IterationSteps", getIterationSteps()) );
  setNodeConstraints( reader->readValue("NodeConstraints", false) );
  setConstrainSurfaceNodes( reader->readValue("ConstrainSurfaceNodes", false) );
  setConstrainQuadPoints( reader->readValue("ConstrainQuadPoints", false) );
  setSmoothTripleLines( reader->readValue("SmoothTripleLines", false) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MovingFiniteElementSmoothing::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MovingFiniteElementSmoothing::dataCheck()
{
  setErrorCondition(0);

  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getSurfaceMeshNodeTypeArrayPath().getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

  // We MUST have Nodes
  if(sm->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
  }

  // We MUST have Triangles defined also.
  if(sm->getFaces().get() == NULL)
  {
    setErrorCondition(-385);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
  }

  if (getErrorCondition() >= 0)
  {
    // Check for Node Type Array
    //int size = sm->getVertices()->getNumberOfTuples();
    QVector<size_t> dims(1, 1);
    m_SurfaceMeshNodeTypePtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, getSurfaceMeshNodeTypeArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshNodeTypePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshNodeType = m_SurfaceMeshNodeTypePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  if ( getConstrainQuadPoints() == true || getSmoothTripleLines() == true )
  {
    if(sm->getEdges().get() == NULL)
    {
      setErrorCondition(-385);
      notifyErrorMessage(getHumanLabel(), "Constraining Quad Points or Triples lines requires Edges array", getErrorCondition());
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MovingFiniteElementSmoothing::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);

  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
  setErrorCondition(0xABABABAB);
  QString ss = QObject::tr("Filter is NOT updated for IGeometry Redesign. A Programmer needs to check this filter. Please report this to the DREAM3D developers.");
  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MovingFiniteElementSmoothing::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshNodeTypeArrayPath().getDataContainerName());

  VertexArray::Pointer floatNodesPtr = sm->getVertices();
  FaceArray::Pointer trianglesPtr = sm->getFaces();

  setErrorCondition(0);
  /* Place all your code to execute your filter here. */
  VertexArray::Vert_t* nodesF = floatNodesPtr->getPointer(0); // Get the pointer to the from of the array so we can use [] notation
  FaceArray::Face_t* triangles = trianglesPtr->getPointer(0); // Get the pointer to the from of the array so we can use [] notation

  // Data variables
  int numberNodes = floatNodesPtr->getNumberOfTuples();
  int ntri = trianglesPtr->getNumberOfTuples();

  StructArray<VertexArray::VertD_t>::Pointer nodesDPtr = StructArray<VertexArray::VertD_t>::CreateArray(numberNodes, "MFE_Double_Nodes");
  nodesDPtr->initializeWithZeros();
  VertexArray::VertD_t* nodes = nodesDPtr->getPointer(0);

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
  TripleNN* triplenn = triplennPtr->getPointer(0);

//  QVector<QString> data;
#if 0
  // read in nodes/triangles
  std::ifstream input1(nodesFile.toLatin1().data());
  std::ifstream input2(triangleFile.toLatin1().data());
  std::ofstream velocities;
#endif
  QString outputNodesFile("/tmp/outputNodesFile.txt");

  if(isVerbose)
  {
    qDebug() << "Number of nodes: " << numberNodes << "\n";
    qDebug() << "Number of triangles: " << ntri << "\n";
    qDebug() << "\n";
  }

  QVector<int> tid(ntri);
  QVector<int> nodeConstraint(numberNodes); //  0=unconstrained, 1=constrained in X, 2= in Y, 4= in Z

  //Read the nodes
#if 0
  int nodeType;
  if (isVerbose) { qDebug() << "reading nodes " << "\n"; }
  for (int i = 0; i < nnod; i++)
  {
    input1 >> id(nodes[i]) >> nodeType >> nodes[i][0] >> nodes[i][1] >> nodes[i][2];
    //  added a junk entry, 25 vi 09; changed to keep node type, 30 xii 09
    triplenn1(nodes[i]) = 0;
    triplenn2(nodes[i]) = 0;
    type(nodes[i]) = nodeType;// duplicating info for now
  }
  if (isVerbose) { qDebug() << "end reading nodes" << "\n"; }
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
    m_SmoothTripleLines = false;
  }
#else

  if(m_SmoothTripleLines == true)
  {
    IDataArray::Pointer edgesDataArray = m->getPointData(SIMPL::CellData::SurfaceMeshEdges);
    IDataArray::Pointer iEdgesDataArray = m->getPointData(SIMPL::CellData::SurfaceMeshInternalEdges);
    if((edgesDataArray.get() == NULL || iEdgesDataArray.get() == NULL) && m_SmoothTripleLines == true)
    {
      setErrorCondition(-596);
      notifyErrorMessage(getHumanLabel(), "Either the Edges or Internal Edges array was NULL which means those arrays have not been created and you have selected to smooth triple lines. Disable the smoothing of triple lines.", -556);
      return;
    }

    StructArray<EdgeArray::Edge_t>* edgesStructArray = StructArray<EdgeArray::Edge_t>::SafePointerDownCast(edgesDataArray.get());
    EdgeArray::Edge_t* edges = edgesStructArray->getPointer(0);

    StructArray<EdgeArray::Edge_t>* internalEdgesStructArray = StructArray<EdgeArray::Edge_t>::SafePointerDownCast(iEdgesDataArray.get());
    EdgeArray::Edge_t* iEdges = internalEdgesStructArray->getPointer(0);

    //  Read the edges, if we are going to smooth them explicitly
#if 0
    if (isVerbose) { qDebug() << "reading edges " << "\n"; }
    std::ifstream input3(edgesFile.toLatin1().data());
    if (input3.is_open() == false)
    {
      if (isVerbose)
      {
        qDebug() << "You have asked to Smooth Triple Lines but have not provided an Edges file." << "\n";
      }
      return -1;
    }
#endif
    int edgeCount = edgesStructArray->getNumberOfTuples();
    int iEdgeCount = internalEdgesStructArray->getNumberOfTuples();
    int nedges = edgeCount + iEdgeCount;
    //  input3 >> nedges;
    //  don't actually need the structure. just the information
    if(isVerbose) { qDebug() << "Number of edges = " << nedges << "\n"; }

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
        // qDebug() << "\n" << "edge#  ID1  ID2  " << i << " " << nid[0] << " " << nid[1] << "\n" ;
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
                qDebug() << "triplenn[nid[1]].triplenn2 != 0 was TRUE. This is bad" << "\n";
                setErrorCondition(-666);
                notifyErrorMessage(getHumanLabel(), "triplenn[nid[1]].triplenn2 != 0 was TRUE. This is bad", -666);
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
                qDebug() << "triplenn[nid[1]].triplenn2 != 0 was TRUE. This is bad" << "\n";
                setErrorCondition(-666);
                notifyErrorMessage(getHumanLabel(), "triplenn[nid[1]].triplenn2 != 0 was TRUE. This is bad", -666);
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
    if(isVerbose) { qDebug() << "... completed 1st reading edges" << "\n"; }
    //  destroy(edges) ;
    //input3.close();
    //  now let's check the entries
    /*
     for ( int i = 0 ; i < nnod ; i++ ) {
     if ( type(nodes[i]) == 3 ||  type(nodes[i]) == 13 ) {
     qDebug() << "node ID, NN1, NN2: " << id(nodes[i]) << " " << triplenn1(nodes[i])
     << " " << triplenn2(nodes[i]) << "\n" ;
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
  if(restartFile.isEmpty() == false)
  {
    //  we have a requested re-start - node positions will be read from the INP file
    std::ifstream restartInput(restartFile.toLatin1().data());

    restartInput >> nnodRe >> nnTriRe;
    if (isVerbose) { qDebug() << "Check number of nodes, triangles in .inp: " << nnodRe << ", " << nnTriRe << "\n"; }
    if(nnodRe != nnod)
    {
      if (isVerbose) { qDebug() << "Discrepancy in the number of nodes, must stop! " << "\n"; }
      return -1;
    }
    else
    {
      // we are OK to read in
      if (isVerbose) { qDebug() << "reading nodes from restart INP " << "\n"; }
      for (int i = 0; i < nnod; i++)
      {
        input1 >> id(nodes[i]) >> nodes[i][0] >> nodes[i][1] >> nodes[i][2];
      }
      if (isVerbose) { qDebug() << "end reading nodes from restart INP " << "\n"; }
    }
    restartInput.close();
    if (isVerbose) { qDebug() << "What step are we re-starting at? " << "\n"; }
    std::cin >> stepsRe;
    if(stepsRe >= final)
    {
      if (isVerbose) { qDebug() << "Final time less than re-start time, must stop! " << "\n"; }
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
  if (isVerbose) { qDebug() << "reading triangles: " << "\n"; }
  for (int i = 0; i < ntri; i++)
  {
    //  char type[3];
    int nid[3];
    //  int reg1, reg2;
    input2 >> tid[i] >> nid[0] >> nid[1] >> nid[2] >> junk >> junk >> junk >> triangles[i].region1 >> triangles[i].region2;

    triangles[i](0) = &nodes[nid[0]];
    triangles[i](1) = &nodes[nid[1]];
    triangles[i](2) = &nodes[nid[2]];
    //  bool ok[3] = {false,false,false};
    //  for (int j=0; j<nnod; j++) {
    // //     if (id[j]==nid[0]) {triangles[i](0) = &nodes[j]; triangles[i].nids[0]= nid[0]; ok[0] = true;}
    // //     else if (id[j]==nid[1]) {triangles[i](1) = &nodes[j]; triangles[i].nids[1]= nid[1];ok[1] = true;}
    // //     else if (id[j]==nid[2]) {triangles[i](2) = &nodes[j]; triangles[i].nids[2]= nid[2];ok[2] = true;}
    //    if (id(nodes[j])==nid[0]) {triangles[i](0) = &nodes[j];  ok[0] = true;}
    //    else if (id(nodes[j])==nid[1]) {triangles[i](1) = &nodes[j]; ok[1] = true;}
    //    else if (id(nodes[j])==nid[2]) {triangles[i](2) = &nodes[j]; ok[2] = true;}
    //    if (ok[0] && ok[1] && ok[2]) break;
    //  }
  }
  if (isVerbose) { qDebug() << "end reading triangles" << "\n"; }
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
    qDebug() << "Model Dimensions as Min, Max: " << "\n";

    //  for(int i=0; i<3; i++)
    qDebug() << "X (0): " << min[0] << " " << max[0] << "\n";
    qDebug() << "Y (1): " << min[1] << " " << max[1] << "\n";
    qDebug() << "Z (2): " << min[2] << " " << max[2] << "\n";
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

  QVector<double> Q_max_hist(hist_count);

  Q_ave = 2.9;
  Q_max_ave = 10;

  double Dihedral, Dihedral_min, Dihedral_max, Dihedral_sum, Dihedral_ave;
  //  for measuring dihedral angles, Mar 2010

  //  now we determine constraints
  for (int r = 0; r < numberNodes; r++)
  {
    nodeConstraint[r] = 0;
    if(m_NodeConstraints == true)
    {
      // only do this if we want the constraint on surfaces
      if(fabs(nodes[r].pos[0] - max[0]) < tolerance || fabs(nodes[r].pos[0] - min[0]) < tolerance) { nodeConstraint[r] += 1; }
      if(fabs(nodes[r].pos[1] - max[1]) < tolerance || fabs(nodes[r].pos[1] - min[1]) < tolerance) { nodeConstraint[r] += 2; }
      if(fabs(nodes[r].pos[2] - max[2]) < tolerance || fabs(nodes[r].pos[2] - min[2]) < tolerance) { nodeConstraint[r] += 4; }
      // debug
      if(m_SurfaceMeshNodeType[r] > 10 && nodeConstraint[r] == 0)
      {
        if(isVerbose)
        {
          qDebug() << "ID " << r << " Coords: " << nodes[r].pos[0] << " " << nodes[r].pos[1] << " " << nodes[r].pos[2] << "\n";
          qDebug() << " Coord. Diffs. for X: " << fabs(nodes[r].pos[0] - max[0]) << " " << fabs(nodes[r].pos[0] - min[0]) << "\n";
          qDebug() << " Coord. Diffs. for Y: " << fabs(nodes[r].pos[1] - max[1]) << " " << fabs(nodes[r].pos[1] - min[1]) << "\n";
          qDebug() << " Coord. Diffs. for Z: " << fabs(nodes[r].pos[2] - max[2]) << " " << fabs(nodes[r].pos[2] - min[2]) << "\n";
          qDebug() << "Type, Constraint: " << m_SurfaceMeshNodeType[r] << " " << nodeConstraint[r] << "\n";
          qDebug() << "Check whether there is a problem with applying constraints ... " << "\n";
        }
      }
      //  constraint for surface nodes
      if(m_ConstrainSurfaceNodes == true && nodeConstraint[r] != 0) { nodeConstraint[r] = 7; }
      //  totally constrain a surface node (temporary fix for connectivity issues in bounded box meshes)

    }
    if(m_SurfaceMeshNodeType[r] == SIMPL::SurfaceMesh::NodeType::QuadPoint && m_ConstrainQuadPoints == true) { nodeConstraint[r] = 7; }
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
      qDebug() << "Update loop: " << updates << "\n";
    }
    QString ss = QObject::tr("Iteration: %1").arg(updates);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

    Dihedral_min = 180.;
    Dihedral_max = 0.;
    Dihedral_ave = 0.;

    TJ_scale = 19000.0; //  arbitrary choice for triple line smoothing !
    A_scale = 4000.0;
    //  designed to ramp up the weight attached to the Quality forces
    Q_scale = 500.0 + 50.0 * (double)updates / (double)final;
    //    WAS:  Q_scale = 1000;
    //  14 may 10: discovered that ramping up the quality forces so high
    //    leads to UNsmoothing of the mesh!

    // compute triangle contributions to K and F
    ntri = trianglesPtr->getNumberOfTuples();
    Q_max = 0.;
    Q_sum = 0.;
    Dihedral_sum = 0.;
    Dihedral_min = 180.;
    Dihedral_max = -1.; //  added may 10, ADR
    double LDistance, deltaLDistance;

    typedef NodeFunctions<VertexArray::VertD_t, double> NodeFunctionsType;
    typedef TriangleFunctions<VertexArray::VertD_t, double> TriangleFunctionsType;
    // Loop through each of the triangles
    for (int t = 0; t < ntri; t++)
    {
      FaceArray::Face_t& rtri = triangles[t];
      MFE::Vector<double> n(3);
      n = TriangleFunctionsType::normal(nodes[rtri.verts[0]], nodes[rtri.verts[1]], nodes[rtri.verts[2]]);
      A = TriangleFunctionsType::area(nodes[rtri.verts[0]], nodes[rtri.verts[1]], nodes[rtri.verts[2]]); //  current Area
      Q = TriangleFunctionsType::circularity(nodes[rtri.verts[0]], nodes[rtri.verts[1]], nodes[rtri.verts[2]], A); //  current quality
      if(Q > 100.)
      {
        if(isVerbose)
        {
          qDebug() << "Warning, tri no. " << t << " has Q= " << Q << "\n";
        }
      }
      Q_sum += Q;
      if(Q > Q_max) { Q_max = Q; }

      Dihedral = TriangleFunctionsType::MinDihedral(nodes[rtri.verts[0]], nodes[rtri.verts[1]], nodes[rtri.verts[2]]);
      // debug
      //      qDebug() << "triangle, min dihedral: " << t << ", " << Dihedral*180/PI << "\n";
      Dihedral_sum += Dihedral;
      if(Dihedral < Dihedral_min) { Dihedral_min = Dihedral; }
      if(Dihedral > Dihedral_max) { Dihedral_max = Dihedral; }

      for (int n0 = 0; n0 < 3; n0++)
      {
        // for each of 3 nodes on the t^th triangle
        int i = rtri.verts[n0];
        VertexArray::VertD_t& node_i = nodes[i];
        for (int j = 0; j < 3; j++)
        {
          //  for each of the three coordinates of the node
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
        {
          //  for each of 3 nodes
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
      {
        // only do this if we want the constraint
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
    if(isVerbose) { qDebug() << iterations << " iterations ... " << "\n"; }

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
        //    qDebug() << i << " "<< Q_max_hist[i] << " " << Q_max_hist[i+1] << "\n";
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

    //  for(int i=0; i<Q_max_hist.size(); i++)
    //    std::cout<<"Q_Max history ... "<<Q_max_hist[i]<<std::endl;
    if(isVerbose)
    {
      qDebug() << "Maximum circularity quality ... " << Q_max << "   - high is bad, low is good" << "\n";
      qDebug() << "Rolling Average circularity quality ... " << Q_max_ave << "\n";
      qDebug() << "Average circularity quality ... " << Q_ave << "\n";
    }
    Dihedral_ave = Dihedral_sum / (float)ntri;
    if(isVerbose)
    {
      qDebug() << "Smallest min. dihedral angle ... " << Dihedral_min * 180. / M_PI << "   - low is bad, high is good" << "\n";
      qDebug() << "Average min. dihedral angle ... " << Dihedral_ave * 180. / M_PI << "\n";
      qDebug() << "Largest min. dihedral angle ... " << Dihedral_max * 180. / M_PI << "\n";

      qDebug() << "\n";
    }
#if 0
    //Output velocities for examination
    std::ostringstream iter_stream;
    QString iter_string;

    // write the iteration to a string
    iter_stream << updates;
    iter_string = iter_stream;

    // extract the basename from the provided filename
    QString infile = outputNodesFile;
    QVector<QString> tokens;
    QString delimiters = "."; // Only a period
    QString basename;
    tokenize(infile, tokens, delimiters);

    if(tokens.size() > 2)
    {
      if(isVerbose) { qDebug() << "WARNING: multiple \".\" in file name: " << infile << "\n"; }
      if(isVerbose) { qDebug() << "\t Using: ." << tokens.front() << "As basename" << "\n"; }
      basename = tokens.front();
    }
    else if(tokens.size() <= 2)
    {
      basename = tokens.front();
    }

    // put it all back together
    QString iterFileName;
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
      //    velocityfile << r << " ";
      for (int s = 0; s < 3; s++)
      {
        double bc_dt = dt;
        if(m_NodeConstraints == true)
        {
          // only do this if we want the constraint
          /*
           if( (fabs(nodes[r][s] - max[s]) < tolerance)
           || (fabs(nodes[r][s] - min[s]) < tolerance)) bc_dt = 0.0 ;
           */
          if(s == 0 && nodeConstraint[r] % 2 != 0) { bc_dt = 0.0; } // X
          if(s == 1 && (nodeConstraint[r] / 2) % 2 != 0) { bc_dt = 0.0; } // Y
          if(s == 2 && nodeConstraint[r] / 4 != 0) { bc_dt = 0.0; } // Z
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
        //    velocityfile  << std::scientific << QSetw(4)
        //        << QSetprecision(4) << F[3*r+s] << "\t"<< x[3*r+s] <<"\t";
      }
      //    velocityfile << "\n";
    }
    //  velocityfile.close();

#if 0
    if(!((updates) % 10))
    {
      // Open the outputfile
      std::ofstream inpfile;
      inpfile.open(iterFileName.toLatin1().data());
      if(!inpfile)
      {
        if(isVerbose) { qDebug() << "Failed to open: " << iterFileName << "\n"; }
        return;
      }

      //      inpfile << nnod <<" "<< ntri <<" 0 2 0"<< "\n" ;
      inpfile << nnod << " " << ntri << " 2 2 0" << "\n"; //  now that we have NODE properties
      for (int i = 0; i < nnod; i++)
      {
        inpfile << i << " " << nodes[i].pos[0] << " " << nodes[i].pos[1] << " " << nodes[i].pos[2] << "\n";
      }

      for (int i = 0; i < ntri; i++)
      {
        //    inpfile<<i<<" "<<triangles[i].region1>>triangles[i].region2<<" tri ";
        inpfile << i << " 0 tri ";
        inpfile << triangles[i].verts[0] << " ";
        inpfile << triangles[i].verts[1] << " ";
        inpfile << triangles[i].verts[2] << "\n";
        //inpfile<<triangles[i].region1<<" ";
        //inpfile<<triangles[i].region2<<std::endl;
      }

      //  information on nodes
      inpfile << "2 1 1" << "\n";
      inpfile << "Node_Type, none" << "\n";
      inpfile << "Node_Constraint, none" << "\n";
      for (int i = 0; i < nnod; i++)
      {
        inpfile << i << " " << nodes[i].nodeKind << " " << nodeConstraint[i] << "\n";
      }

      //  information on elements (triangles)
      inpfile << "2 1 1" << "\n";
      inpfile << "minID, none" << "\n";
      inpfile << "maxID, none" << "\n";
      for (int i = 0; i < ntri; i++) // ADR  25 jun 09
      {
        inpfile << i << " " << triangles[i].nSpin[0] << " " << triangles[i].nSpin[1] << "\n";
      }
      inpfile.close();
    }
#endif
  }

#if 0
  std::ofstream inpfile;
  inpfile.open(outputNodesFile.toLatin1().data());
  if(!inpfile)
  {
    if(isVerbose) { qDebug() << "Failed to open nodes output: " << outputNodesFile << "\n"; }
    return;
  }

  //   std::ofstream output(argv[2]);
  inpfile << nnod << "\n";
  for (int i = 0; i < nnod; i++)
  {
    inpfile << QSetw(6) << std::fixed << "\t" << i << "\t" << nodes[i].nodeKind << "\t" << nodes[i].pos[0] << "\t" << nodes[i].pos[1] << "\t"
            << nodes[i].pos[2] << "\n";
  }
  inpfile.close();
#endif

  if(isVerbose)
  {
    qDebug() << "Finished the smoothing " << "\n";
    qDebug() << "You can combine the nodes and triangles to do further analysis or smoothing" << "\n";
    // qDebug() << "Look for " << outputNodesFile << "\n";
  }

  // Copy the nodes from the 64 bit floating point to the 32 bit floating point
  for(int n = 0; n < numberNodes; ++n)
  {
    nodesF[n].pos[0] = nodes[n].pos[0];
    nodesF[n].pos[1] = nodes[n].pos[1];
    nodesF[n].pos[2] = nodes[n].pos[2];
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MovingFiniteElementSmoothing::newFilterInstance(bool copyFilterParameters)
{
  MovingFiniteElementSmoothing::Pointer filter = MovingFiniteElementSmoothing::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MovingFiniteElementSmoothing::getCompiledLibraryName()
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MovingFiniteElementSmoothing::getBrandingString()
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MovingFiniteElementSmoothing::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MovingFiniteElementSmoothing::getGroupName()
{ return SIMPL::FilterGroups::SurfaceMeshingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MovingFiniteElementSmoothing::getSubGroupName()
{ return SIMPL::FilterSubGroups::SmoothingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MovingFiniteElementSmoothing::getHumanLabel()
{ return "Moving Finite Element Smoothing"; }

