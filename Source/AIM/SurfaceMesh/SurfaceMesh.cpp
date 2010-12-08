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
#include "AIM/Common/VTKFileUtils.h"

#include <MXA/Common/LogTime.h>
#include <MXA/Utilities/MXADir.h>



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
m_ErrorCondition(0),
m_BinaryVTKFile(false),
m_ConformalMesh(true)
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

#define USE_VTK_FILE_UTILS 1

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMesh::compute()
{
  CHECK_FOR_CANCELED(Surface Meshing)
  progressMessage(AIM_STRING("Running Surface Meshing"), 0 );

  std::string  NodesRawFile = m_OutputDirectory + MXADir::Separator + AIM::SurfaceMeshing::NodesRawFile;
  std::string  NodesFile = m_OutputDirectory + MXADir::Separator + AIM::SurfaceMeshing::NodesFile;
  std::string  TrianglesFile = m_OutputDirectory + MXADir::Separator + AIM::SurfaceMeshing::TrianglesFile;
  std::string  EdgesFile = m_OutputDirectory + MXADir::Separator + AIM::SurfaceMeshing::EdgesFile;
  std::string  EdgesFileIndex = m_OutputDirectory + MXADir::Separator + AIM::SurfaceMeshing::EdgesFileIndex;
  std::string  TrianglesFileIndex = m_OutputDirectory + MXADir::Separator + AIM::SurfaceMeshing::TrianglesFileIndex;
  std::string  VisualizationFile = m_OutputDirectory + MXADir::Separator + AIM::SurfaceMeshing::VisualizationFile;

  // Create the output directory if needed
  if (MXADir::exists(m_OutputDirectory) == false)
  {
    if (MXADir::mkdir(m_OutputDirectory, true) == false)
    {
#ifdef AIM_USE_QT
    this->m_Cancel = true;
#endif
    this->m_ErrorCondition = 1;
    CHECK_FOR_CANCELED(Surface Meshing)
    progressMessage(AIM_STRING("Could not create output directory."), 100 );
    return;
    }
  }

  int cNodeID = 0;
//  int cEdgeID = 0;
//  int fEdgeID = 0;
  int cTriID = 0;
//  int nFEdge = 0; // number of edges on the square...
  int nTriangle = 0; // number of triangles...
  int nEdge = 0; // number of triangles...
//  int nnEdge = 0; // number of triangles...
//  int npTriangle = 0; // number of triangles...
//  int ncTriangle = 0; // number of triangles...
  int nNodes = 0; // number of total nodes used...
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


  m = SurfaceMeshFunc::New();
//  int err = 0;
#if  USE_VTK_FILE_UTILS
  VTKFileUtils vtkreader;
  m_ZDim = vtkreader.readHeader(m.get(), m_InputFile);

#else
  m_ZDim = m->initialize_micro(m_InputFile, -1);
#endif
  std::stringstream ss;
  for (int i = 0; i < (m_ZDim - 1); i++)
  {
    ss.str("");
    ss << "Marching Cubes Between Layers " << i << " and " << i+1;
    progressMessage(AIM_STRING(ss.str().c_str()), (i*90/m_ZDim) );

    // initialize neighbors, possible nodes and squares of marching cubes of each layer...
#if  USE_VTK_FILE_UTILS
    m_ZDim = vtkreader.readZSlice(m.get(), i);
    if(m_ZDim == -1)
    {
      ss.str("");
      ss << "Error loading slice data from vtk file.";
      this->m_ErrorCondition = 1;
      progressMessage(AIM_STRING(ss.str().c_str()), 100 );
      return;
    }
#else
    m_ZDim = m->initialize_micro(m_InputFile, i);
#endif
    m->get_neighbor_list(i);
    m->initialize_nodes(i);
    m->initialize_squares(i);

    // find face edges of each square of marching cubes in each layer...
    nEdge = m->get_number_Edges(i);
    m->get_nodes_Edges(edgeTable_2d, nsTable_2d, i, nEdge);

    // find triangles and arrange the spins across each triangle...
    nTriangle = m->get_number_triangles();

    if (nTriangle > 0)
    {
      m->get_triangles(nTriangle);
      m->arrange_grainnames(nTriangle, i);
    }

    // assign new, cumulative node id...
    nNodes = m->assign_nodeID(cNodeID, i);

    // Output nodes and triangles...
    m->writeNodesFile(i, cNodeID, NodesFile);
    m->writeTrianglesFile(nTriangle, TrianglesFile, i, cTriID);
    cNodeID = nNodes;
    cTriID = cTriID + nTriangle;
  }

#ifdef AIM_USE_QT
  QString msg("Writing Surface Mesh File: ");
  msg.append(QString::fromStdString(VisualizationFile));
#else
  std::string msg("Writing Surface Mesh File: ");
  msg.append(VisualizationFile);
#endif

  progressMessage(AIM_STRING("Smoothing Boundaries"), 90 );
  m->smooth_boundaries(nNodes, cTriID, NodesFile, TrianglesFile);

  progressMessage(msg, 95 );
  m->writeVTKOutputFile(nNodes, cTriID, VisualizationFile, NodesFile, TrianglesFile, m_BinaryVTKFile, m_ConformalMesh);

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
  //  std::cout << message.toStdString() << std::endl;
#else
  std::cout << logTime() << progress << "% - "<< message << std::endl;
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

