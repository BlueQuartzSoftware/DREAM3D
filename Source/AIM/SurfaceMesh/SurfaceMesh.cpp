///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////

#include "SurfaceMesh.h"

#include <MXA/Common/LogTime.h>
#include <MXA/Utilities/MXAFileSystemPath.h>


#ifdef AIM_USE_QT
#define CHECK_FOR_CANCELED(AClass)\
  if (this->m_Cancel) { \
  QString msg = #AClass; \
  msg += " was Canceled"; \
  emit updateMessage(msg);\
  emit updateProgress(0);\
  return;}

#else
#define CHECK_FOR_CANCELED(AClass)\
  ;
#endif



#if AIM_USE_QT

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMesh::Pointer SurfaceMesh::New( QObject* parent)
{
  Pointer sharedPtr(new SurfaceMesh(parent));
  return sharedPtr;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMesh::SurfaceMesh(
#if AIM_USE_QT
QObject* parent
#endif
) :
#if AIM_USE_QT
QThread(parent),
#endif
m_InputDirectory("."),
m_InputFile(""),
m_OutputDirectory(""),
m_SmoothMesh(false),
m_SmoothIterations(0),
m_SmoothFileOutputIncrement(0),
m_SmoothLockQuadPoints(false),
m_ErrorCondition(0)
#if AIM_USE_QT
,
m_Cancel(false)
#endif
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMesh::~SurfaceMesh()
{
}

#if AIM_USE_QT
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMesh::run()
{
  compute();
  m = SurfaceMeshFunc::NullPointer();  // Clean up the memory
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMesh::compute()
{
  CHECK_FOR_CANCELED(Surface Meshing)
  progressMessage(AIM_STRING("Running Surface Meshing"), 0 );

  std::string  NodesRawFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::NodesRawFile;
  std::string  NodesFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::NodesFile;
  std::string  TrianglesFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::TrianglesFile;
  std::string  EdgesFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::EdgesFile;
  std::string  EdgesFileIndex = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::EdgesFileIndex;
  std::string  TrianglesFileIndex = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::TrianglesFileIndex;
  std::string  VisualizationFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::VisualizationFile;

  m = SurfaceMeshFunc::New();
//  int err = 0;
  m_ZDim = m->initialize_micro(m_InputFile);


  int cNodeID = 0;
  int cEdgeID = 0;
  int fEdgeID = 0;
  int cTriID = 0;
  int nFEdge = 0; // number of edges on the square...
  int nTriangle = 0; // number of triangles...
//  int nEdge = 0; // number of edges...
  int npTriangle = 0; // number of triangles...
  int ncTriangle = 0; // number of triangles...
  int nNodes = 0; // number of total nodes used...
  int tnIEdge = 0;
  int nIEdge = 0;
  int edgeTable_2d[20][8] = {
	{ -1, -1, -1, -1, -1, -1, -1, -1},
	{ -1, -1, -1, -1, -1, -1, -1, -1},
	{ -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0, 1, -1, -1, -1, -1, -1, -1},
	{ -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0, 2, -1, -1, -1, -1, -1, -1},
	{ 1, 2, -1, -1, -1, -1, -1, -1},
	{ 0, 4, 2, 4, 1, 4, -1, -1},
	{ -1, -1, -1, -1, -1, -1, -1, -1},
	{ 3, 0, -1, -1, -1, -1, -1, -1},
	{ 3, 1, -1, -1, -1, -1, -1, -1},
	{ 3, 4, 0, 4, 1, 4, -1, -1},
	{ 2, 3, -1, -1, -1, -1, -1, -1},
	{ 3, 4, 0, 4, 2, 4, -1, -1},
	{ 3, 4, 1, 4, 2, 4, -1, -1},
	{ 3, 0, 1, 2, -1, -1, -1, -1},
	{ 0, 1, 2, 3, -1, -1, -1, -1},
	{ 0, 1, 2, 3, -1, -1, -1, -1},
	{ 3, 0, 1, 2, -1, -1, -1, -1},
	{ 3, 4, 1, 4, 0, 4, 2, 4}
	};

	int nsTable_2d[20][8] = {
	  {-1, -1, -1, -1, -1, -1, -1, -1},
	  {-1, -1, -1, -1, -1, -1, -1, -1},
	  {-1, -1, -1, -1, -1, -1, -1, -1},
	  {1, 0, -1, -1, -1, -1, -1, -1},
	  {-1, -1, -1, -1, -1, -1, -1, -1 },
	  {1, 0, -1, -1, -1, -1, -1, -1, },
	  {2, 1, -1, -1, -1, -1, -1, -1},
	  {1, 0, 3, 2, 2, 1, -1, -1},
	  {-1, -1, -1, -1, -1, -1, -1, -1},
	  {0, 3, -1, -1, -1, -1, -1, -1},
	  {0, 3, -1, -1, -1, -1, -1, -1},
	  {0, 3, 1, 0, 2, 1, -1, -1},
	  {3, 2, -1, -1, -1, -1, -1, -1},
	  {0, 3, 1, 0, 3, 2, -1, -1},
	  {0, 3, 2, 1, 3, 2, -1, -1},
	  {0, 3, 2, 1, -1, -1, -1, -1},
	  {1, 0, 3, 2, -1, -1, -1, -1},
	  {1, 0, 3, 2, -1, -1, -1, -1},
	  {0, 3, 2, 1, -1, -1, -1, -1},
	  {0, 3, 2, 1, 1,  0, 3, 2 }
	};

  for (int i = 0; i < (m_ZDim - 1); i++)
  {

    progressMessage(AIM_STRING("Marching Cubes Between Layers "), (i) );

    // initialize neighbors, possible nodes and squares of marching cubes of each layer...
    m->get_neighbor_list(i);
    m->initialize_nodes(i);
    m->initialize_squares(i);
    m->initialize_VertEdgemap();

    // find face edges of each square of marching cubes in each layer...
    nFEdge = m->get_number_fEdges(i);
    m->get_nodes_fEdges(edgeTable_2d, nsTable_2d, i, nFEdge);

    // find triangles and arrange the spins across each triangle...
    nTriangle = m->get_number_triangles();

    if (nTriangle > 0)
    {
      m->get_triangles(nTriangle);
      m->arrange_grainnames(nTriangle, i);
    }

    // find unique inner edges...
    tnIEdge = 3 * nTriangle - nFEdge;
    if (tnIEdge > 0)
    {
      tnIEdge = m->get_inner_edges(nFEdge, nTriangle, tnIEdge);
      m->find_unique_inner_edges(tnIEdge, &nIEdge);
    }
    // copy the previous node and face edge information...
    if (i > 0)
    {
      m->copy_previous_nodes();
    }

    // assign new, cumulative node id...
    nNodes = m->assign_new_nodeID(cNodeID);
    cNodeID = nNodes;

    // rewrite the edges and the triangles with new node ids...
    m->update_face_edges(nFEdge);

    if (nTriangle > 0 && tnIEdge > 0)
    {
      m->update_inner_edges(tnIEdge, nIEdge);
      m->update_current_triangles(nTriangle);
    }

    // Output nodes and triangles...
    m->get_output_nodes(i, NodesRawFile);

    m->get_output_edges(nFEdge, tnIEdge, i, cEdgeID, &fEdgeID, EdgesFileIndex);

    m->get_output_triangles(nTriangle, TrianglesFileIndex, i, cTriID);
    cEdgeID = fEdgeID;
    cTriID = cTriID + nTriangle;

    // store cVertex information of bottom layer onto temporary vertex...
    m->copy_cNodes_2_pNodes();
  }

  CHECK_FOR_CANCELED(Surface Meshing)
  progressMessage(AIM_STRING("Converting Slice Files to Single Nodes/Triangles Pair"), 40 );

  // Convert the output edge and triangle files into a single node/triangle file
//  pTriangle = (struct patch *)malloc(1*sizeof(struct patch));
  // Creating new or Erasing Current output files

  ofstream outFile;
  outFile.open(EdgesFile.c_str(), ios::app);
  outFile << cEdgeID << endl;
  outFile.close();

  outFile.open(TrianglesFile.c_str(), ios::app);
  outFile << cTriID << endl;
  outFile.close();

  outFile.open(NodesFile.c_str(), ios::app);
  outFile << cNodeID << endl;
  outFile.close();

  // For each layer...
  for (int i = 0; i <= (m_ZDim-2); i++)
  {

    // get numbers of face edges, inner edges, previous triangles and current triangles...
    m->UET_get_number_current_edges (&nFEdge, &nIEdge, i, EdgesFileIndex);
    m->UET_get_number_previous_current_triangles (&npTriangle, &ncTriangle, i, TrianglesFileIndex);

    // read edge and triangle information...
    m->UET_read_current_edges (nFEdge, nIEdge, i, EdgesFileIndex);
    m->UET_read_current_triangles (ncTriangle, i, TrianglesFileIndex);

    // update edges and triangles...
    m->UET_update_iEdges_triangles (nIEdge, ncTriangle);
    m->UET_update_fEdges_triangles (nFEdge, npTriangle, ncTriangle, i);

    // print out edge information...
    m->UET_get_output_edges (nFEdge, nIEdge, EdgesFile);

    // print out triangle information...
    if (i > 0)
    {
      int type = 0;
      if (i == (m_ZDim-2))
      {
	    type = 0;
        m->UET_get_output_triangles(type, npTriangle, TrianglesFile);
	    type = 1;
        m->UET_get_output_triangles(type, ncTriangle, TrianglesFile);
      }
      else
      {
	    type = 0;
        m->UET_get_output_triangles(type, npTriangle, TrianglesFile);
      }
    }

    // make cTriangle into pTriangle...
	npTriangle = ncTriangle;
    m->UET_copy_triangles (npTriangle);
  }

  m->UET_get_output_nodes (nNodes, NodesFile, NodesRawFile);
  m->UET_create_vtk(VisualizationFile, NodesFile, TrianglesFile);

/*  if (m_SmoothMesh == true)
  {
    //TODO: Run the smoothing algorithm
    CHECK_FOR_CANCELED(Surface Meshing)
    progressMessage(AIM_STRING("Smoothing Surface Meshing"), 70 );
    int lockquads = 0;
    if (m_SmoothLockQuadPoints) { lockquads = 1; }
    m_ErrorCondition = SmoothGrain3D(AIM::Representation::NodesFile,
                                     AIM::Representation::TrianglesFile,
                                      m_OutputDirectory,
                                      m_SmoothIterations,
                                      m_SmoothFileOutputIncrement,
                                      lockquads);

  }
*/
  progressMessage(AIM_STRING("Surface Meshing Complete"), 100 );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMesh::progressMessage(AIM_STRING message, int progress)
{
#ifdef AIM_USE_QT
  emit updateMessage(QString(message));
  emit updateProgress(progress);
  std::cout << message.toStdString() << " " << progress << std::endl;
#else
  std::cout << logTime() << "{" << progress << "} " << message << std::endl;
#endif
}

#ifdef AIM_USE_QT
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMesh::on_CancelWorker()
{
  // std::cout << "SurfaceMesh::cancelWorker()" << std::endl;
  this->m_Cancel = true;
}
#endif

