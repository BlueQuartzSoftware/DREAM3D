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

#include <MXA/Utilities/MXAFileSystemPath.h>
#include <AIM/SurfaceMesh/Surface_Mesh_General_MCA_Layer.h>
#include <AIM/SurfaceMesh/Update_Edge_Tri.h>
#include <AIM/SurfaceMesh/smooth-grain3d.h>


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
m_DxFile(""),
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
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMesh::compute()
{
  CHECK_FOR_CANCELED(Surface Meshing)
  progressMessage(AIM_STRING("Running Surface Meshing"), 0 );

  m_ErrorCondition = SurfaceMesh_MCALayer(m_XDim, m_YDim, m_ZDim,
                                 m_OutputDirectory.c_str(),
                                 m_DxFile.c_str(),
                                 AIM::Representation::MeshStatFile.c_str(),
                                 AIM::Representation::NodesRawFile.c_str());

int SurfaceMesh_MCALayer( int xnum, int ynum, int znum,
                          const char* outputDir,
                          const char* dxFile,
                          const char* meshStatFile,
                          const char* nodesRawFile)
{
  int i;
  int err;

  int NS; // The number of sites(voxels) in the simulation box...
  int NSP;

  int cNodeID;
  int cEdgeID, fEdgeID;
  int cTriID;

  int nFEdge; // number of edges on the square...
  int nTriangle; // number of triangles...
  int nNodes; // number of total nodes used...
  int tnIEdge, nIEdge;

  struct neighbor *neigh; // contains nearest neighbor information...
  struct voxel *point; // contains voxel information...
  struct face *cSquare; // contains square information...
 // struct face *pSquare;
  struct node *cVertex; // contains node information...
  struct node *pVertex;
  struct segment *cFedge; // contains edges on square faces for open loops...
  struct isegment *cIedge;
  struct patch *cTriangle;
  FILE *f1, *f4;
  char trianglesFile[BUFSIZ];
  char edgeFile[BUFSIZ];
  char genericFilePath[BUFSIZ];
  char postfix[64] = TRIANGLES_RAW_TXT;
  char edgepostfix[64] = EDGES_RAW_TXT;
  // Edge edge and neighboring spin table...

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

  err = 0;
//  meshStatsFile = MESH_STAT_FILE;
//  nodesFile = NODES_RAW_FILE;

  NS = xnum * ynum * znum;
  NSP = xnum * ynum;

  neigh = (struct neighbor *) malloc((2 * NSP + 1) * sizeof(struct neighbor));
  point = (struct voxel *) malloc((NS + 1) * sizeof(struct voxel));
  cSquare = (struct face *) malloc(3 * 2 * NSP * sizeof(struct face));
  cVertex = (struct node *) malloc(7 * 2 * NSP * sizeof(struct node));
  pVertex = (struct node *) malloc(7 * 2 * NSP * sizeof(struct node));

  printf("\nReading microstucture...\n");
  err = initialize_micro(point, NS, xnum, ynum, znum, dxFile );
  if (err != 0)
  {
    return err;
  }

  memset(genericFilePath, 0, BUFSIZ); // Clear the string first
  sprintf(genericFilePath, "%s%s", outputDir,  nodesRawFile);
  if ((f1 = fopen(genericFilePath, "w")) == NULL)
  {
    printf("\nThe nodesFile file doesn't exist!\n");
    return 1;
  }

  fclose(f1);

  cNodeID = 0;
  cEdgeID = 0;
  fEdgeID = 0;
  cTriID = 0;

  for (i = 0; i < (znum - 1); i++)
  {

    printf("\nMarching Cubes between layers %5d and %5d\n", i + 1, i + 2);

    // initialize neighbors, possible nodes and squares of marching cubes of each layer...
    get_neighbor_list(neigh, NS, NSP, xnum, ynum, znum, i);
    initialize_nodes(point, cVertex, NSP, i);
    initialize_squares(neigh, cSquare, NS, NSP, i);

    // find face edges of each square of marching cubes in each layer...
    nFEdge = get_number_fEdges(cSquare, point, neigh, NSP, i);
    printf("\tTotal number of face edges: %d\n", nFEdge);
    cFedge = (struct segment *) malloc(nFEdge * sizeof(struct segment));
    get_nodes_fEdges(cSquare, point, neigh, cVertex, cFedge, edgeTable_2d, nsTable_2d, NS, NSP, xnum, i);

    // find triangles and arrange the spins across each triangle...
    nTriangle = get_number_triangles(point, cSquare, cVertex, cFedge, NS, NSP, xnum);
    printf("\tNumber of triangles: %d\n", nTriangle);

    if (nTriangle > 0)
    {
      cTriangle = (struct patch *) malloc(nTriangle * sizeof(struct patch));
      get_triangles(cTriangle, cSquare, cVertex, cFedge, NS, NSP, xnum);
      arrange_spins(point, cTriangle, cVertex, neigh, nTriangle, xnum, NSP, i);
    }

    // find unique inner edges...
    tnIEdge = 3 * nTriangle - nFEdge;
    if (tnIEdge > 0)
    {
      cIedge = (struct isegment *) malloc(tnIEdge * sizeof(struct isegment));
      tnIEdge = get_inner_edges(cVertex, cFedge, cIedge, cTriangle, nFEdge, nTriangle);
      printf("\tNumber of inner edges including duplicates: %d\n", tnIEdge);
      find_unique_inner_edges(cIedge, tnIEdge, &nIEdge);
      printf("\tNumber of unique inner edges: %d\n", nIEdge);
    }
    // copy the previous node and face edge information...
    if (i > 0)
    {
      copy_previous_nodes(cVertex, pVertex, NSP);
    }

    // assign new, cumulative node id...
    nNodes = assign_new_nodeID(cVertex, NSP, cNodeID);
    printf("\tNumber of nodes used: %d\n", nNodes - cNodeID);
    cNodeID = nNodes;
    printf("\tCurrent node is at: %d\n", cNodeID);

    // rewirte the edges and the triangles with new node ids...
    update_face_edges(cVertex, cFedge, nFEdge);

    if (nTriangle > 0 && tnIEdge > 0)
    {
      update_inner_edges(cVertex, cIedge, tnIEdge, nIEdge);
      update_current_triangles(cVertex, cTriangle, nTriangle);
    }

    // Output nodes and triangles...
    memset(genericFilePath, 0, BUFSIZ); // Clear the string first
    sprintf(genericFilePath, "%s%s", outputDir,  nodesRawFile);
    get_output_nodes(cVertex, NSP, i, znum, genericFilePath );

    memset(edgeFile, 0, BUFSIZ); // Clear the string first
    sprintf(edgeFile, "%s%d%s", outputDir,  i, edgepostfix);
    get_output_edges(cFedge, cIedge, nFEdge, tnIEdge, i, znum, cEdgeID, &fEdgeID, edgeFile);

    memset(trianglesFile, 0, BUFSIZ); // Clear the string first
    sprintf(trianglesFile, "%s%d%s", outputDir, i, postfix);
    get_output_triangles(cTriangle, nTriangle, trianglesFile, i, cTriID);
    cEdgeID = fEdgeID;
    cTriID = cTriID + nTriangle;

    // store cVertex information of bottom layer onto temporary vertex...
    copy_cNodes_2_pNodes(cVertex, pVertex, NSP);

    free(cFedge);

    if (nTriangle > 0 && tnIEdge > 0)
    {
      free(cIedge);
      free(cTriangle);
    }
  }

  free(neigh);
  free(point);
  free(cSquare);
  free(cVertex);
  free(pVertex);

  memset(genericFilePath, 0, BUFSIZ); // Clear the string first
  sprintf(genericFilePath, "%s%s", outputDir,  meshStatFile);

  if ((f4 = fopen(genericFilePath, "w")) == NULL)
  {
    printf("\nThe mesh_stat.txt file doesn't exist!\n");
    return 1;
  }

  fprintf(f4, "%d %d %d %d\n", cNodeID, cEdgeID, cTriID, znum - 2);

  fclose(f4);
  return err;
}


#ifndef SURFACE_MESH_LIBRARY
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  char* dxFile;
  int xnum, ynum, znum;
  int err;
  char* edgeTableFile;
  char* neighspinTableFile;
  char* meshStatFile;
  char* nodesRawFile;
  char* outputDir;

  edgeTableFile = EDGETABLE_2D_FILE;
  neighspinTableFile = NEIGHSPIN_TABLE_FILE;
  meshStatFile = MESH_STAT_FILE;
  nodesRawFile = NODES_RAW_FILE;
  outputDir = ".";

  err = 0;
  dxFile = (char*)malloc( BUFSIZ * sizeof(char));
  memset(dxFile, 0, BUFSIZ);


  printf("Enter the dimension of the microstructure:\n");
  scanf("%d %d %d", &xnum, &ynum, &znum);
  printf("%d %d %d\n", xnum, ynum, znum);

  printf("Enter the name of input microstructure dx file:\n");
  scanf("%s", dxFile);

  err = SurfaceMesh_MCALayer(xnum, ynum, znum,
                     outputDir,
                     dxFile,
                     meshStatFile, nodesRawFile);

  free(dxFile);
  return err;
}
#endif



  CHECK_FOR_CANCELED(Surface Meshing)
  progressMessage(AIM_STRING("Converting Slice Files to Single Nodes/Triangles Pair"), 40 );
  // Convert the output edge and triangle files into a single node/triangle file
  m_ErrorCondition = Update_Edge_Tri(AIM::Representation::MeshStatFile.c_str(),
                                     m_OutputDirectory.c_str(),
                                     AIM::Representation::EdgesFile.c_str(),
                                     AIM::Representation::TrianglesFile.c_str(),
                                     AIM::Representation::NodesFile.c_str(),
                                     AIM::Representation::NodesRawFile.c_str() );


  if (m_SmoothMesh == true)
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
  std::cout << message << std::endl;
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

