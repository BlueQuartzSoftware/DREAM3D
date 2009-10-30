/*
 *****************************************************************************************
 Surface_Mesh_General_MCA_Layer.c

 After Z. Wu's approach.
 First, all the squares are treated such that 2d marching squared edges are identified.
 Next, do the 3d stuff...

 The program gives 2 output files. 1) node_raw.txt and 2) patch_raw.txt

 Sukbin Lee
 sukbin@andrew.cmu.edu

 *****************************************************************************************
 */


#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "Surface_Mesh_General_MCA_Layer.h"

/**
 * @brief
 * @param xnum
 * @param ynum
 * @param znum
 * @param outputDir The absolute path to where the output files will be created
 * @param nodesFile The absolute path to the nodes input file
 * @param dxFile The absolute path to the dx file
 * @param edgeTableFile The Absolute Path to the EdgeTable File.
 * @param neighspinTableFile The Absolute Path to the NeighborSpinTable File
 * @param meshStatsFile The Absolute Path to the stats file
 * @return
 */
int SurfaceMesh_MCALayer( int xnum, int ynum, int znum,
                          const char* outputDir,
                          const char* dxFile,
                          const char* edgeTableFile,
                          const char* neighspinTableFile)
{
  int i;
  int err;
  // Edge edge and neighboring spin table...
  int edgeTable_2d[20][8];
  int nsTable_2d[20][8];

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
  struct segment *cFedge; // cotains edges on square faces for open loops...
  struct isegment *cIedge;
  struct patch *cTriangle;
  FILE *f1, *f4;
  char trianglesFile[BUFSIZ];
  char edgeFile[BUFSIZ];
  char genericFilePath[BUFSIZ];
  char* meshStatsFile;
  char* nodesFile;
  char postfix[64] = "_triangles_raw.txt";
  char edgepostfix[64] = "_edges_raw.txt";

  meshStatsFile = MESH_STAT_FILE;
  nodesFile = NODES_RAW_FILE;
  err = 0;
  NS = xnum * ynum * znum;
  NSP = xnum * ynum;

  neigh = (struct neighbor *) malloc((2 * NSP + 1) * sizeof(struct neighbor));
  point = (struct voxel *) malloc((NS + 1) * sizeof(struct voxel));
  cSquare = (struct face *) malloc(3 * 2 * NSP * sizeof(struct face));
  cVertex = (struct node *) malloc(7 * 2 * NSP * sizeof(struct node));
  pVertex = (struct node *) malloc(7 * 2 * NSP * sizeof(struct node));

  printf("\nReading microstucture...\n");
  initialize_micro(point, NS, xnum, ynum, znum, dxFile );

  printf("\nReading edge and triangle tables...\n");
  read_edge_neighspin_table(edgeTable_2d, nsTable_2d, edgeTableFile, neighspinTableFile);


  if ((f1 = fopen(nodesFile, "w")) == NULL)
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
    sprintf(genericFilePath, "%s%s", outputDir,  nodesFile);
    get_output_nodes(cVertex, NSP, i, znum, genericFilePath );



    memset(edgeFile, 0, BUFSIZ); // Clear the string first
    sprintf(edgeFile, "%s%d%s", outputDir,  i, edgepostfix);
    get_output_edges(cFedge, cIedge, nFEdge, tnIEdge, i, znum, cEdgeID, &fEdgeID, edgeFile);


    memset(trianglesFile, 0, BUFSIZ); // Clear the string first
    sprintf(trianglesFile, "%s%d%s", outputDir, cTriID, postfix);

    get_output_triangles(cTriangle, nTriangle, i, cTriID, trianglesFile);
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
  sprintf(genericFilePath, "%s%s", outputDir,  meshStatsFile);

  if ((f4 = fopen(meshStatsFile, "w")) == NULL)
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
  char* outputDir;
  
  edgeTableFile = EDGETABLE_2D_FILE;
  neighspinTableFile = NEIGHSPIN_TABLE_FILE;
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
                     dxFile, edgeTableFile,
                     neighspinTableFile );

  free(dxFile);
  return err;
}
#endif

// Functions...
void initialize_micro(struct voxel *p, int ns, int xDim, int yDim, int zDim, const char* name)
{

  FILE* f;

  int i, j, k, l;
  int id;

  int tspin;


  char datStr[255];

  double tempx, tempy, tempz;

  if ((f = fopen(name, "r")) == NULL)
  {
    printf("\nThe input file doesn't exist!\n");
    exit(1);
  }

  // Let's get rid of the 9 header lines in the xxx.dx produced by 3d coarsening code...
  for (k = 0; k < 9; k++)
  {
    fgets(datStr, 255, f);
  }

  p[0].spin = 0; // Point 0 is a garbage...
  p[0].coord[0] = 0.0;
  p[0].coord[1] = 0.0;
  p[0].coord[2] = 0.0;

  // Let's input the spin numbers for each site from micro.input...
  for (i = 1; i <= ns; i++)
  {
    fscanf(f, "%d", &tspin);
    if (tspin == 0)
    {
      tspin = -3;
    }
    p[i].spin = tspin;
  }

  fclose(f);

  // Let's fill out the coordinate of each voxel. Remember that x coordinate goes fastest...
  for (l = 0; l <= ns - xDim * yDim; l = l + xDim * yDim)
  {
    for (k = 0; k <= xDim * yDim - xDim; k = k + xDim)
    {
      for (j = 1; j <= xDim; j++)
      {
        id = l + k + j;
        tempx = (double) (j - 1);
        tempy = (double) (k / xDim);
        tempz = (double) (l / (xDim * yDim));
        //printf("%10d %6.3f %6.3f %6.3f\n", id, tempx, tempy, tempz);

        p[id].coord[0] = tempx;
        p[id].coord[1] = tempy;
        p[id].coord[2] = tempz;

        if (tempx > (double) (xDim - 2) || tempy > (double) (yDim - 2) || tempz > (double) (zDim - 2))
        {
          p[id].atSurf = 1;
        }
        else
        {
          p[id].atSurf = 0;
        }

      }
    }
  }

}

void read_edge_neighspin_table(int eT2d[20][8], int nsT2d[20][8],
                               const char* edgeTableFile, const char* neighspinTableFile)
{

  FILE *f1, *f2;

  int j, k, dummy;

  // Read edge table in 2d...
  if ((f1 = fopen(edgeTableFile, "r")) == NULL)
  {
    printf("\nThe edgeTableFile input file doesn't exist!\n");
    exit(1);
  }

  if ((f2 = fopen(neighspinTableFile, "r")) == NULL)
  {
    printf("\nThe neighspinTableFile input file doesn't exist!\n");
    exit(1);
  }

  for (j = 0; j < 20; j++)
  {

    fscanf(f1, "%d", &dummy);
    fscanf(f2, "%d", &dummy);

    for (k = 0; k < 8; k++)
    {
      fscanf(f1, "%d", &eT2d[j][k]);
      fscanf(f2, "%d", &nsT2d[j][k]);
    }
  }

  fclose(f1);
  fclose(f2);
}

void get_neighbor_list(struct neighbor *n, int ns, int nsp, int xDim, int yDim, int zDim, int zID)
{
  // nsp = number of sites in a plane of xDim by yDim...
  // n[][] = 2 dimensional array storing its site number and neighbors...
  // site_id = id number for each site...starting from 1 to xDim*yDim*zDim....
  //
  // I assumed the square lattice...so the order of neighbors as follows...
  //
  //    4   3   2         13  12  11          22  21  20
  //    5 site  1         14   9  10          23  18  19
  //    6   7   8         15  16  17          24  25  26
  //
  //    in-plane          upper plane         lower plane

  int ii; // loop index...
  int i, j, k, r; // position indices...
  int site_id; // id number for each site...

  for (ii = 1; ii <= 2 * nsp; ii++)
  {

    site_id = zID * nsp + ii;
    n[ii].csiteid = site_id;

    k = (site_id - 1) / nsp;
    k = k * nsp;

    r = (site_id - 1) % nsp;
    j = r / xDim;
    j = j * xDim;

    i = r % xDim + 1;

    //same plane...
    n[ii].neigh_id[1] = k + j + i % xDim + 1;
    n[ii].neigh_id[2] = k + (j - xDim + nsp) % nsp + i % xDim + 1;
    n[ii].neigh_id[3] = k + (j - xDim + nsp) % nsp + i;
    n[ii].neigh_id[4] = k + (j - xDim + nsp) % nsp + (i - 2 + xDim) % xDim + 1;

    n[ii].neigh_id[5] = k + j + (i - 2 + xDim) % xDim + 1;
    n[ii].neigh_id[6] = k + (j + xDim) % nsp + (i - 2 + xDim) % xDim + 1;
    n[ii].neigh_id[7] = k + (j + xDim) % nsp + i;
    n[ii].neigh_id[8] = k + (j + xDim) % nsp + i % xDim + 1;

    //upper plane...
    //move the plane up and use the same scheme...
    n[ii].neigh_id[9] = (k - nsp + ns) % ns + j + i;
    n[ii].neigh_id[10] = (k - nsp + ns) % ns + j + i % xDim + 1;
    n[ii].neigh_id[11] = (k - nsp + ns) % ns + (j - xDim + nsp) % nsp + i % xDim + 1;

    n[ii].neigh_id[12] = (k - nsp + ns) % ns + (j - xDim + nsp) % nsp + i;
    n[ii].neigh_id[13] = (k - nsp + ns) % ns + (j - xDim + nsp) % nsp + (i - 2 + xDim) % xDim + 1;
    n[ii].neigh_id[14] = (k - nsp + ns) % ns + j + (i - 2 + xDim) % xDim + 1;

    n[ii].neigh_id[15] = (k - nsp + ns) % ns + (j + xDim) % nsp + (i - 2 + xDim) % xDim + 1;
    n[ii].neigh_id[16] = (k - nsp + ns) % ns + (j + xDim) % nsp + i;
    n[ii].neigh_id[17] = (k - nsp + ns) % ns + (j + xDim) % nsp + i % xDim + 1;

    //lower plane...
    n[ii].neigh_id[18] = (k + nsp) % ns + j + i;
    n[ii].neigh_id[19] = (k + nsp) % ns + j + i % xDim + 1;
    n[ii].neigh_id[20] = (k + nsp) % ns + (j - xDim + nsp) % nsp + i % xDim + 1;

    n[ii].neigh_id[21] = (k + nsp) % ns + (j - xDim + nsp) % nsp + i;
    n[ii].neigh_id[22] = (k + nsp) % ns + (j - xDim + nsp) % nsp + (i - 2 + xDim) % xDim + 1;
    n[ii].neigh_id[23] = (k + nsp) % ns + j + (i - 2 + xDim) % xDim + 1;

    n[ii].neigh_id[24] = (k + nsp) % ns + (j + xDim) % nsp + (i - 2 + xDim) % xDim + 1;
    n[ii].neigh_id[25] = (k + nsp) % ns + (j + xDim) % nsp + i;
    n[ii].neigh_id[26] = (k + nsp) % ns + (j + xDim) % nsp + i % xDim + 1;

  }

  /*
   int i2;

   for( i2=1; i2<=nsp; i2++){
   printf("site %d\n\n ", n[i2].csiteid);
   printf("%d  %d  %d\n",   n[i2].neigh_id[4], n[i2].neigh_id[3], n[i2].neigh_id[2]);
   printf("%d  %d  %d\n",   n[i2].neigh_id[5], n[i2].csiteid    , n[i2].neigh_id[1]);
   printf("%d  %d  %d\n\n", n[i2].neigh_id[6], n[i2].neigh_id[7], n[i2].neigh_id[8]);

   printf("%d  %d  %d\n",   n[i2].neigh_id[13], n[i2].neigh_id[12],n[i2].neigh_id[11]);
   printf("%d  %d  %d\n",   n[i2].neigh_id[14], n[i2].neigh_id[9], n[i2].neigh_id[10]);
   printf("%d  %d  %d\n\n", n[i2].neigh_id[15], n[i2].neigh_id[16],n[i2].neigh_id[17]);

   printf("%d  %d  %d\n",   n[i2].neigh_id[22], n[i2].neigh_id[21], n[i2].neigh_id[20]);
   printf("%d  %d  %d\n",   n[i2].neigh_id[23], n[i2].neigh_id[18], n[i2].neigh_id[19]);
   printf("%d  %d  %d\n\n", n[i2].neigh_id[24], n[i2].neigh_id[25], n[i2].neigh_id[26]);
   }
   */

}

void initialize_nodes(struct voxel *p, struct node *v, int nsp, int zID)
{

  // Finds the coordinates of nodes...

  int i;
  int id;

  int tsite;
  double x, y, z;

  // node id starts with 0....
  for (i = 1; i <= 2 * nsp; i++)
  {

    id = 7 * (i - 1);
    tsite = zID * nsp + i;

    x = p[tsite].coord[0];
    y = p[tsite].coord[1];
    z = p[tsite].coord[2];
    //printf("%f %f %f\n", x, y, z);

    v[id].coord[0] = x + 0.5;
    v[id].coord[1] = y;
    v[id].coord[2] = z;
    v[id].nodeKind = 0;
    v[id].newNodeID = -1;

    v[id + 1].coord[0] = x;
    v[id + 1].coord[1] = y + 0.5;
    v[id + 1].coord[2] = z;
    v[id + 1].nodeKind = 0;
    v[id + 1].newNodeID = -1;

    v[id + 2].coord[0] = x;
    v[id + 2].coord[1] = y;
    v[id + 2].coord[2] = z + 0.5;
    v[id + 2].nodeKind = 0;
    v[id + 2].newNodeID = -1;

    v[id + 3].coord[0] = x + 0.5;
    v[id + 3].coord[1] = y + 0.5;
    v[id + 3].coord[2] = z;
    v[id + 3].nodeKind = 0;
    v[id + 3].newNodeID = -1;

    v[id + 4].coord[0] = x + 0.5;
    v[id + 4].coord[1] = y;
    v[id + 4].coord[2] = z + 0.5;
    v[id + 4].nodeKind = 0;
    v[id + 4].newNodeID = -1;

    v[id + 5].coord[0] = x;
    v[id + 5].coord[1] = y + 0.5;
    v[id + 5].coord[2] = z + 0.5;
    v[id + 5].nodeKind = 0;
    v[id + 5].newNodeID = -1;

    v[id + 6].coord[0] = x + 0.5;
    v[id + 6].coord[1] = y + 0.5;
    v[id + 6].coord[2] = z + 0.5;
    v[id + 6].nodeKind = 0;
    v[id + 6].newNodeID = -1;

    //printf("\n%10d %5.2f %5.2f %5.2f\n", i, x, y, z);
    //printf("%10d %5.2f %5.2f %5.2f\n", id,   v[id].coord[0], v[id].coord[1], v[id].coord[2]);
    //printf("%10d %5.2f %5.2f %5.2f\n", id+1, v[id+1].coord[0], v[id+1].coord[1], v[id+1].coord[2]);
    //printf("%10d %5.2f %5.2f %5.2f\n", id+2, v[id+2].coord[0], v[id+2].coord[1], v[id+2].coord[2]);
    //printf("%10d %5.2f %5.2f %5.2f\n", id+3, v[id+3].coord[0], v[id+3].coord[1], v[id+3].coord[2]);
    //printf("%10d %5.2f %5.2f %5.2f\n", id+4, v[id+4].coord[0], v[id+4].coord[1], v[id+4].coord[2]);
    //printf("%10d %5.2f %5.2f %5.2f\n", id+5, v[id+5].coord[0], v[id+5].coord[1], v[id+5].coord[2]);
    //printf("%10d %5.2f %5.2f %5.2f\n", id+6, v[id+6].coord[0], v[id+6].coord[1], v[id+6].coord[2]);
  }

}

void initialize_squares(struct neighbor *n, struct face *sq, int ns, int nsp, int zID)
{

  // Gather initial information on each square...
  int id;
 // int sqid;
  int i, j;
  int csite;

  // square id starts with 0....
  // notice that voxels at the surface will have the wrong values of node at the other end...
  // since it includes periodic boundary condition...
  // but, since the structure surrounded by ghost layer of spin -3, it's OK...
  for (i = 1; i <= 2 * nsp; i++)
  {

    id = 3 * (i - 1);
    csite = zID * nsp + i;

    // store 4 corners of square...
    sq[id].site_id[0] = csite;
    sq[id].site_id[1] = n[i].neigh_id[1];
    sq[id].site_id[2] = n[i].neigh_id[8];
    sq[id].site_id[3] = n[i].neigh_id[7];

    sq[id + 1].site_id[0] = csite;
    sq[id + 1].site_id[1] = n[i].neigh_id[1];
    sq[id + 1].site_id[2] = n[i].neigh_id[19];
    sq[id + 1].site_id[3] = n[i].neigh_id[18];

    sq[id + 2].site_id[0] = n[i].neigh_id[7];
    sq[id + 2].site_id[1] = csite;
    sq[id + 2].site_id[2] = n[i].neigh_id[18];
    sq[id + 2].site_id[3] = n[i].neigh_id[25];

    // initialize node, edge...-1 is dummy initial value...
    for (j = 0; j < 4; j++)
    {
      sq[id].edge_id[j] = -1;
      sq[id + 1].edge_id[j] = -1;
      sq[id + 2].edge_id[j] = -1;
    }

    sq[id].nEdge = 0;
    sq[id].turnFC = 0;
    sq[id].FCnode = -1;
    sq[id].effect = 0;

    sq[id + 1].nEdge = 0;
    sq[id + 1].turnFC = 0;
    sq[id + 1].FCnode = -1;
    sq[id + 1].effect = 0;

    sq[id + 2].nEdge = 0;
    sq[id + 2].turnFC = 0;
    sq[id + 2].FCnode = -1;
    sq[id + 2].effect = 0;
  }

}

int get_number_fEdges(struct face *sq, struct voxel *p, struct neighbor *n, int nsp, int zID)
{

  int  k, m;

  int csite, cspin, tsite;

  int tnsite[4];
  int tnspin[4];
  int atBulk; // if it's 0, the square is at Bulk...

  int sqIndex;
  int anFlag;

  int quot, rmd, rmd1;

  int sumEdge;
  int numCEdge;

  sumEdge = 0;

  for (k = 0; k < (3 * 2 * nsp); k++)
  { // for each square on upper layer...

    quot = k / (3 * nsp);
    rmd = k % (3 * nsp);
    rmd1 = rmd % 3;

    if (quot == 0 || (quot == 1 && rmd1 == 0))
    {

      csite = sq[k].site_id[0];
      cspin = p[csite].spin;

      tnsite[0] = sq[k].site_id[0];
      tnsite[1] = sq[k].site_id[1];
      tnsite[2] = sq[k].site_id[2];
      tnsite[3] = sq[k].site_id[3];
      atBulk = 0;

      for (m = 0; m < 4; m++)
      {
        tsite = tnsite[m];
        tnspin[m] = p[tsite].spin;
        if (tnspin[m] <= 0)
        {
          atBulk++;
        }
      }

      if (atBulk == 0)
      {
        sq[k].effect = 1; // mark current square as effective one that can be "marching cubed"...
      }

      // Let's count the number of edges...
      if (cspin > 0 && atBulk == 0)
      { // consider the square inside the bulk only...

        sqIndex = get_square_index(tnspin);

        if (sqIndex == 15)
        {
          anFlag = treat_anomaly(tnsite, p, n, nsp, zID);
          sqIndex = sqIndex + anFlag;
        }

        if (sqIndex == 0)
        {
          numCEdge = 0;
        }
        else if (sqIndex == 19)
        {
          numCEdge = 4;
        }
        else if (sqIndex == 15 || sqIndex == 16 || sqIndex == 17 || sqIndex == 18)
        {
          numCEdge = 2;
        }
        else if (sqIndex == 7 || sqIndex == 11 || sqIndex == 13 || sqIndex == 14)
        {
          numCEdge = 3;
        }
        else
        {
          numCEdge = 1;
        }

        //if(sqIndex!=0){
        //printf("\t%5d %5d     %3d\n", tnspin[0], tnspin[1], sqIndex);
        //printf("\t%5d %5d\n\n", tnspin[3], tnspin[2]);
        //printf("\t%5d %5d     %3d\n", tnsite[0], tnsite[1], sqIndex);
        //printf("\t%5d %5d\n\n", tnsite[3], tnsite[2]);
        //}

        sumEdge = sumEdge + numCEdge;

      }

    }

  }

  //printf("\tnum of edges: %d\n", sumEdge);
  return sumEdge;

}

void get_nodes_fEdges(struct face *sq,
                      struct voxel *p,
                      struct neighbor *n,
                      struct node *v,
                      struct segment *e,
                      int eT2d[20][8],
                      int nsT2d[20][8],
                      int ns,
                      int nsp,
                      int xDim,
                      int zID)
{

  int  j, k, m, ii;

  int csite, cspin, tsite;
  int cubeOrigin; // stores the site id of 0th corner of each cube...
  int sqOrder; // stores the 0th, 1st and 2nd square...

  int tnsite[4];
  int tnspin[4];
  int atBulk; // if it's 0, the square is at Bulk...

  int quot, rmd, rmd1;

  int sqIndex;
  int anFlag;

  int nodeIndex[2];
  int *nodeID;
  int tnode;
  int pixIndex[2];
  int *pixSpin;
  int eid; // edge id...
  int tnode1, tnode2;
  int edgeCount; // number of edges for each square...

  nodeID = (int *) malloc(2 * sizeof(int));
  pixSpin = (int *) malloc(2 * sizeof(int));

  eid = 0;

  for (k = 0; k < 3 * 2 * nsp; k++)
  { // for each square...

    quot = k / (3 * nsp);
    rmd = k % (3 * nsp);
    rmd1 = rmd % 3;

    if (quot == 0 || (quot == 1 && rmd1 == 0))
    {

      //printf("%5d", k);

      cubeOrigin = k / 3 + 1;
      sqOrder = k % 3;

      csite = sq[k].site_id[0];
      cspin = p[csite].spin;

      tnsite[0] = sq[k].site_id[0];
      tnsite[1] = sq[k].site_id[1];
      tnsite[2] = sq[k].site_id[2];
      tnsite[3] = sq[k].site_id[3];
      atBulk = 0;

      for (m = 0; m < 4; m++)
      {
        tsite = tnsite[m];
        tnspin[m] = p[tsite].spin;
        if (tnspin[m] <= 0)
        {
          atBulk++;
        }
      }

      edgeCount = 0;

      // Let's find the edges...
      if (cspin > 0 && atBulk == 0)
      { // consider the square inside the bulk only...

        sqIndex = get_square_index(tnspin);

        if (sqIndex == 15)
        {
          anFlag = treat_anomaly(tnsite, p, n, nsp, zID);
          sqIndex = sqIndex + anFlag;
        }

        if (sqIndex != 0)
        {

          for (j = 0; j < 8; j = j + 2)
          {

            if (eT2d[sqIndex][j] != -1)
            {

              nodeIndex[0] = eT2d[sqIndex][j];
              nodeIndex[1] = eT2d[sqIndex][j + 1];
              pixIndex[0] = nsT2d[sqIndex][j];
              pixIndex[1] = nsT2d[sqIndex][j + 1];

              get_nodes(cubeOrigin, sqOrder, nodeIndex, nodeID, nsp, xDim);

              tnode1 = nodeID[0];
              tnode2 = nodeID[1];

              //printf("%5d %5d    ", tnode1, tnode2);

              e[eid].node_id[0] = tnode1; // actual node ids for each edge...
              e[eid].node_id[1] = tnode2;

              // Categorize the node...if it's triple junction or not...
              for (ii = 0; ii < 2; ii++)
              {

                if (nodeIndex[ii] == 4)
                {

                  if (sqIndex == 7 || sqIndex == 11 || sqIndex == 13 || sqIndex == 14)
                  {
                    tnode = nodeID[ii];
                    v[tnode].nodeKind = 3;
                    sq[k].turnFC = 1;
                    sq[k].FCnode = tnode;
                  }
                  else if (sqIndex == 19)
                  {
                    tnode = nodeID[ii];
                    v[tnode].nodeKind = 4;
                    sq[k].turnFC = 1;
                    sq[k].FCnode = tnode;
                  }
                  else
                  {
                    // do nothing...
                  }

                }
                else
                {
                  tnode = nodeID[ii];
                  v[tnode].nodeKind = 2;
                }
              }

              // Get the grain spin across the edges...
              get_spins(tnspin, pixIndex, pixSpin);

              e[eid].neigh_spin[0] = pixSpin[0];
              e[eid].neigh_spin[1] = pixSpin[1];
              //if(pixSpin[0]==pixSpin[1]){
              //printf("wrong %10d   %3d   %10d %10d   %10d %10d\n",
              //eid, sqIndex, pixIndex[0], pixIndex[1], pixSpin[0], pixSpin[1]);
              //}

              e[eid].segKind = 2; // edges of the open loops are always binary...
              // triple lines only occurs inside the marching cubes...

              // mark where each face edge belong to, upper, middle or bottom of the layer...
              if (quot == 0)
              {

                if (rmd1 == 0)
                {
                  e[eid].where = 0;
                }
                else
                {
                  e[eid].where = 1;
                }

              }
              else
              { //when quot==1

                if (rmd1 == 0)
                {
                  e[eid].where = 2;
                }
                else
                {
                  // do nothing...
                }
              }

              sq[k].edge_id[edgeCount] = eid;

              edgeCount++;
              eid++;
            }
          }
        }

      }

      sq[k].nEdge = edgeCount;
      //printf("%3d", edgeCount);
    }

  }

  //printf("\n");

  //printf("\nnum of edges: %d\n", eid);

}

int get_square_index(int tns[4])
{

  // identify each square configuration using binary bit...
  // returns the unique decimal integer for each configuration...

  int aBit[6];
  int tempIndex;
  int subIndex;

  if (tns[0] == tns[1])
  {
    aBit[0] = 0;
  }
  else
  {
    aBit[0] = 1;
  }
  if (tns[1] == tns[2])
  {
    aBit[1] = 0;
  }
  else
  {
    aBit[1] = 1;
  }
  if (tns[2] == tns[3])
  {
    aBit[2] = 0;
  }
  else
  {
    aBit[2] = 1;
  }
  if (tns[3] == tns[0])
  {
    aBit[3] = 0;
  }
  else
  {
    aBit[3] = 1;
  }
  if (tns[0] == tns[2])
  {
    aBit[4] = 0;
  }
  else
  {
    aBit[4] = 1;
  }
  if (tns[1] == tns[3])
  {
    aBit[5] = 0;
  }
  else
  {
    aBit[5] = 1;
  }

  tempIndex = 8 * aBit[3] + 4 * aBit[2] + 2 * aBit[1] + 1 * aBit[0];

  if (tempIndex == 15)
  {
    subIndex = 2 * aBit[4] + 1 * aBit[5];
    if (subIndex == 0)
    {
      tempIndex = tempIndex;
    }
    else
    {
      tempIndex = tempIndex + subIndex + 1;
    }
  }

  return tempIndex;
}

int treat_anomaly(int tnst[4], struct voxel *p1, struct neighbor *n1, int nsp1, int zID1)
{

  int i, j, k, ii;

  int cid;

  int csite, cspin;
  int nsite, nspin;

  int temp;
  int tempFlag;

  int min, minid;

  int numNeigh[4];
  min = 0;
  minid = -1;

  for (k = 0; k < 4; k++)
  {
    numNeigh[k] = 0;
  }

  for (i = 0; i < 4; i++)
  {

    csite = tnst[i];
    cid = csite - zID1 * nsp1;
    cspin = p1[csite].spin;

    for (j = 1; j <= num_neigh; j++)
    {
      nsite = n1[cid].neigh_id[j];
      nspin = p1[nsite].spin;
      if (cspin == nspin)
      {
        numNeigh[i] = numNeigh[i] + 1;
      }
    }
  }

  for (ii = 0; ii < 4; ii++)
  {

    temp = numNeigh[ii];

    if (temp <= min)
    {
      min = temp;
      minid = ii;
    }
    else
    {
      min = min;
      minid = minid;
    }
  }

  if (minid == 1 || minid == 3)
  { // we can reverse this...let's try this later...
    tempFlag = 0;
  }
  else
  {
    tempFlag = 1;
  }

  return tempFlag;
}

void get_nodes(int cst, int ord, int nidx[2], int *nid, int nsp1, int xDim1)
{

  int ii;
  int tempIndex;

  for (ii = 0; ii < 2; ii++)
  {

    tempIndex = nidx[ii];

    if (ord == 0)
    { // if the square index is 0 for corner site...

      switch (tempIndex)
      {
        case 0:
          nid[ii] = 7 * (cst - 1);
          break;
        case 1:
          nid[ii] = 7 * cst + 1;
          break;
        case 2:
          nid[ii] = 7 * (cst + xDim1 - 1);
          break;
        case 3:
          nid[ii] = 7 * (cst - 1) + 1;
          break;
        case 4:
          nid[ii] = 7 * (cst - 1) + 3;
          break;
      }

    }
    else if (ord == 1)
    { // if the square index is 1...

      switch (tempIndex)
      {
        case 0:
          nid[ii] = 7 * (cst - 1);
          break;
        case 1:
          nid[ii] = 7 * cst + 2;
          break;
        case 2:
          nid[ii] = 7 * (cst + nsp1 - 1);
          break;
        case 3:
          nid[ii] = 7 * (cst - 1) + 2;
          break;
        case 4:
          nid[ii] = 7 * (cst - 1) + 4;
          break;
      }

    }
    else
    { // if the square index is 2...

      switch (tempIndex)
      {
        case 0:
          nid[ii] = 7 * (cst - 1) + 1;
          break;
        case 1:
          nid[ii] = 7 * (cst - 1) + 2;
          break;
        case 2:
          nid[ii] = 7 * (cst + nsp1 - 1) + 1;
          break;
        case 3:
          nid[ii] = 7 * (cst + xDim1 - 1) + 2;
          break;
        case 4:
          nid[ii] = 7 * (cst - 1) + 5;
          break;
      }

    }
    //printf("%10d %7d %2d\t", cst, nid[ii], tempIndex);
  }

  //printf("\n");

}

void get_spins(int nSpn[4], int pID[2], int *pSpin)
{

  int i;
  int pixTemp, tempSpin;

  for (i = 0; i < 2; i++)
  {

    pixTemp = pID[i];

    switch (pixTemp)
    {
      case 0:
        tempSpin = nSpn[0];
        pSpin[i] = tempSpin;
        break;
      case 1:
        tempSpin = nSpn[1];
        pSpin[i] = tempSpin;
        break;
      case 2:
        tempSpin = nSpn[2];
        pSpin[i] = tempSpin;
        break;
      case 3:
        tempSpin = nSpn[3];
        pSpin[i] = tempSpin;
        break;
    }

    //printf("%7d %2d ", nid[i], tempIndex);
  }

}

int get_number_triangles(struct voxel *p, struct face *sq, struct node *v, struct segment *e, int ns, int nsp, int xDim)
{

  int i, ii, i1, i2, j, k, kk;

  int sqID[6];
  int tsq; // current sq id...
  int tnfe; // temp number of edges...

  int nFC; // number of FC turned on...
  int tFC;
  int nFE; // number of face edges...
  int eff; // all the squares effective?...
  int cubeFlag; // if 1, we can do marching cube; if 0, useless...

  int *arrayFE;
  int *arrayFC;
  int *arraySpin;

  int tsqid1, tsqid2;
  int tsite1, tsite2;
  int cspin, nspin;
  int nburnt;
  int nds; // number of different spin of upper square...
  int BCnode;

  int fcid;
  int tindex;

  int nTri, nTri0, nTri2, nTriM; // # triangles when fcenter is 0, 2 and more than 2...

  arrayFC = (int *) malloc(6 * sizeof(int));
  arraySpin = (int *) malloc(8 * sizeof(int)); // stores 8 corner spins for center node kind...

  nTri0 = 0;
  nTri2 = 0;
  nTriM = 0;

  //printf("%d\n", nsp);

  for (i = 1; i <= nsp; i++)
  {

    cubeFlag = 0;

    // for each marching cube, collect faces...
    sqID[0] = 3 * (i - 1);
    sqID[1] = 3 * (i - 1) + 1;
    sqID[2] = 3 * (i - 1) + 2;
    sqID[3] = 3 * i + 2;
    sqID[4] = 3 * (i + xDim - 1) + 1;
    sqID[5] = 3 * (i + nsp - 1);

    BCnode = 7 * (i - 1) + 6;

    nFC = 0;
    nFE = 0;
    eff = 0;

    //initialize face center array...
    for (ii = 0; ii < 6; ii++)
    {
      arrayFC[ii] = -1;
    }

    // Count the number of face center turned on and face edges...
    fcid = 0;
    ii = 0;
    for (ii = 0; ii < 6; ii++)
    {
      tsq = sqID[ii];
      tFC = sq[tsq].turnFC;
      if (tFC == 1)
      {
        arrayFC[fcid] = sq[tsq].FCnode;
        fcid++;
      }
      nFC = nFC + tFC;
      nFE = nFE + sq[tsq].nEdge;
      eff = eff + sq[tsq].effect;
    }

    if (fcid != nFC)
    {
      printf("Something's wrong with counting face centers...\n");
    }

    if (eff == 6)
    {
      cubeFlag = 1;
    }

    if (nFC >= 3)
    {
      // If number of face centers turned on is more than 2...
      // let's update the nodeKind of body center node...
      tsqid1 = sqID[0];
      tsqid2 = sqID[5];

      for (j = 0; j < 4; j++)
      {
        tsite1 = sq[tsqid1].site_id[j];
        tsite2 = sq[tsqid2].site_id[j];
        arraySpin[j] = p[tsite1].spin;
        arraySpin[j + 4] = p[tsite2].spin;
      }

      nds = 0;
      nburnt = 0;

      for (k = 0; k < 8; k++)
      {

        cspin = arraySpin[k];

        if (cspin != -1)
        {

          nds++;
          arraySpin[k] = -1; // burn...
          nburnt++;

          for (kk = 0; kk < 8; kk++)
          {
            nspin = arraySpin[kk];
            if (cspin == nspin)
            {
              arraySpin[kk] = -1; //burn...
              nburnt++;
            }
          }
        }
      }

      if (nburnt != 8)
      {
        printf("wrong burnt: %5d\n", nburnt);
      }

      v[BCnode].nodeKind = nds + 10;
      //printf("%5d\n", nds);


    }

    // Checking the number of edges for loops in the cube...
    if (nFE >= 3)
    {
      //printf("%4d  CF:%4d\n", nFE, cubeFlag);
    }
    else if (nFE > 0 && nFE < 3)
    {
      //printf("\t? %5d %3d   CF:%4d\n", i, nFE, cubeFlag);  // We have to ignore this case...
    }
    else if (nFE == 0)
    {
      //printf("\tcube inside grain\n");
    }
    else
    {
      //printf("\twhat?\n");
    }

    // if the current marching cube is a collection of 6 effective squares...and
    // the number of face edges at least 3...
    // when nFE==2, it doen't happen
    // when nFE==1, the edge will contribute for the neighboring marching cube...
    // when nFE==0, it means the cube is inside a grain...
    if (cubeFlag == 1 && nFE > 2)
    {

      // Make edge array for each marching cube...
      arrayFE = (int *) malloc(nFE * sizeof(int));
      tindex = 0;
      for (i1 = 0; i1 < 6; i1++)
      {
        tsq = sqID[i1];
        tnfe = sq[tsq].nEdge;
        if (tnfe != 0)
        {
          for (i2 = 0; i2 < tnfe; i2++)
          {
            arrayFE[tindex] = sq[tsq].edge_id[i2];
            tindex++;
          }
        }
      }

      if (tindex != nFE)
      {
        printf("Something wrong with counting number of edges for marching cube...\n");
      }

      // Consider each case as Z. Wu's paper...
      if (nFC == 0)
      { // when there's no face center

        nTri0 = nTri0 + get_number_case0_triangles(arrayFE, v, e, nFE);

      }
      else if (nFC == 2)
      {

        nTri2 = nTri2 + get_number_case2_triangles(arrayFE, v, e, nFE, arrayFC, nFC);
        /*
         int m;
         int s1, s2, s3, s0;
         for(m=0; m<6; m++){
         printf("%5d ", arrayFC[m]);
         }
         printf("  ");
         m = 0;
         for(m=0; m<6; m++){
         tsq = sqID[m];
         s0 = sq[tsq].site_id[0];
         s1 = sq[tsq].site_id[1];
         s2 = sq[tsq].site_id[2];
         s3 = sq[tsq].site_id[3];
         printf("%2d %2d %2d %2d   ", p[s0].spin, p[s1].spin, p[s2].spin, p[s3].spin);
         }
         printf("\n");
         */

      }
      else if (nFC > 2 && nFC <= 6)
      {

        nTriM = nTriM + get_number_caseM_triangles(arrayFE, v, e, nFE, arrayFC, nFC);
        /*
         int m;
         int s1, s2, s3, s0;
         for(m=0; m<6; m++){
         printf("%5d ", arrayFC[m]);
         }
         printf("  ");
         m = 0;
         for(m=0; m<6; m++){
         tsq = sqID[m];
         s0 = sq[tsq].site_id[0];
         s1 = sq[tsq].site_id[1];
         s2 = sq[tsq].site_id[2];
         s3 = sq[tsq].site_id[3];
         printf("%2d %2d %2d %2d   ", p[s0].spin, p[s1].spin, p[s2].spin, p[s3].spin);
         }
         printf("\n");
         */

      }
      else
      {

        //printf("Somthing's wrong in counting face centers turned on...\n");
        //printf("%5d %10d\n\n", nFC, i);
      }

      free(arrayFE);

    }

  }

  //printf("\tNumber of triangles for each case:");
  //printf("%5d %5d %5d\n", nTri0, nTri2, nTriM);

  nTri = nTri0 + nTri2 + nTriM;

  free(arraySpin);

  return (nTri);
}

int get_number_case0_triangles(int *afe, struct node *v1, struct segment *e1, int nfedge)
{

  int ii, i, j, jj, k, kk, k1;

  int loopID;
  int tail, head, coin;

  int cedge, nedge;
  int cspin1, cspin2, cnode1, cnode2;
  int nspin1, nspin2, nnode1, nnode2;
 // int tspin, tnode;
  int nucleus, chaser;
//  int  end;

  int spinFlag, nodeFlag, flip;

  int bflag, nbflag;
  int *burnt;
  int *burnt_list;

  int *count;
  int numN, numTri, tnumTri;

//  int tn1, tn2;
  int lid;

  burnt = (int *) malloc(nfedge * sizeof(int));
  burnt_list = (int *) malloc(nfedge * sizeof(int));

  // initialize burn flags for face edges...
  for (ii = 0; ii < nfedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }

  loopID = 1;
  tail = 0;
  head = 0;

  for (i = 0; i < nfedge; i++)
  {

    cedge = afe[i];
    bflag = burnt[i];

    if (bflag == 0)
    {

      nucleus = cedge;
      burnt[i] = loopID;
      burnt_list[tail] = cedge;

      do
      {

        chaser = burnt_list[tail];

        cspin1 = e1[chaser].neigh_spin[0];
        cspin2 = e1[chaser].neigh_spin[1];
        cnode1 = e1[chaser].node_id[0];
        cnode2 = e1[chaser].node_id[1];

        for (j = 0; j < nfedge; j++)
        {

          nedge = afe[j];
          nbflag = burnt[j];

          if (nbflag == 0)
          {

            nspin1 = e1[nedge].neigh_spin[0];
            nspin2 = e1[nedge].neigh_spin[1];
            nnode1 = e1[nedge].node_id[0];
            nnode2 = e1[nedge].node_id[1];

            // checking if neighbor edge has same neighboring spins...
            if (((cspin1 == nspin1) && (cspin2 == nspin2)) || ((cspin1 == nspin2) && (cspin2 == nspin1)))
            {
              spinFlag = 1;
            }
            else
            {
              spinFlag = 0;
            }

            // checking if neighor egde is contiguous...
            if ((cnode2 == nnode1) && (cnode1 != nnode2))
            {
              nodeFlag = 1;
              flip = 0;
            }
            else if ((cnode2 == nnode2) && (cnode1 != nnode1))
            {
              nodeFlag = 1;
              flip = 1;
            }
            else
            {
              nodeFlag = 0;
              flip = 0;
            }

            if (spinFlag == 1 && nodeFlag == 1)
            {

              head = head + 1;
              burnt_list[head] = nedge;
              burnt[j] = loopID;

              if (flip == 1)
              {
                //e1[nedge].neigh_spin[0] = nspin2;
                //e1[nedge].neigh_spin[1] = nspin1;
                e1[nedge].node_id[0] = nnode2;
                e1[nedge].node_id[1] = nnode1;
              }

            }
            else
            {
              // do nothing...
            }

          }
          else
          {
            // do nothing...
          }

        }

        if (tail == head)
        {
          coin = 0;
          tail = tail + 1;
          head = tail;
          loopID++;
        }
        else
        {
          tail = tail + 1;
          coin = 1;
        }

      } while (coin);

    }

  }

  count = (int *) malloc(loopID * sizeof(int));

  for (k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }

  for (k = 1; k < loopID; k++)
  {
    for (kk = 0; kk < nfedge; kk++)
    {
      if (k == burnt[kk])
      {
        count[k] = count[k] + 1;
      }
    }
  }

  numTri = 0;

  for (jj = 1; jj < loopID; jj++)
  {

    numN = count[jj];

    if (numN == 3)
    {
      tnumTri = 1;
      numTri = numTri + tnumTri;
    }
    else if (numN > 3)
    {
      tnumTri = numN - 2;
      numTri = numTri + tnumTri;
    }
    else
    {
      // do nothing...
      printf("Something wrong in counting # triangles...%d %d\n", numN, nfedge);

      k = 0;
      kk = 0;
      for (k = 0; k < nfedge; k++)
      {
        cedge = burnt_list[k];
        for (kk = 0; kk < nfedge; kk++)
        {
          if (cedge == afe[kk])
          {
            lid = burnt[kk];
          }
        }
        printf("%5d   %3d   %5d %5d   %5d %5d\n", cedge, lid, e1[cedge].node_id[0], e1[cedge].node_id[1], e1[cedge].neigh_spin[0], e1[cedge].neigh_spin[1]);
      }

      printf("\n");

    }

  }

  /*
   int lid;
   k = 0;
   kk = 0;
   for(k=0; k<nfedge; k++){
   cedge = burnt_list[k];
   for(kk=0; kk<nfedge; kk++){
   if(cedge==afe[kk]){
   lid = burnt[kk];
   }
   }
   printf("%5d   %3d   %5d %5d   %5d %5d\n", cedge, lid,
   e1[cedge].node_id[0], e1[cedge].node_id[1], e1[cedge].neigh_spin[0], e1[cedge].neigh_spin[1]);
   }
   printf("\n");
   */

  free(burnt);
  free(burnt_list);
  free(count);

  return (numTri);

}

int get_number_case2_triangles(int *afe, struct node *v1, struct segment *e1, int nfedge, int *afc, int nfctr)
{

  int ii, i, j, k, kk, k1, n, i1, j1;

  int loopID;
  int tail, head, coin;

  int cedge, nedge;
  int cspin1, cspin2, cnode1, cnode2;
  int nspin1, nspin2, nnode1, nnode2;
  int tspin, tnode;
  int nucleus, chaser;
  int start, end;
  int from, to;

  int spinFlag, nodeFlag, flip;

  int bflag, nbflag;
  int *burnt;
  int *burnt_list;

  int *count;
  int numN, numTri;

  int *burnt_loop;

  int openL; // if a loop is an open loop, it's 1; if closed, it's 0...
  int startEdge;
  int endNode;
  int index;

  burnt = (int *) malloc(nfedge * sizeof(int));
  burnt_list = (int *) malloc(nfedge * sizeof(int));


  // initialize burn flags for face edges...
  for (ii = 0; ii < nfedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }

  loopID = 1;
  tail = 0;
  head = 0;

  for (i = 0; i < nfedge; i++)
  {

    cedge = afe[i];
    bflag = burnt[i];

    if (bflag == 0)
    {

      nucleus = cedge;
      burnt[i] = loopID;
      burnt_list[tail] = cedge;

      do
      {

        chaser = burnt_list[tail];

        cspin1 = e1[chaser].neigh_spin[0];
        cspin2 = e1[chaser].neigh_spin[1];
        cnode1 = e1[chaser].node_id[0];
        cnode2 = e1[chaser].node_id[1];

        for (j = 0; j < nfedge; j++)
        {

          nedge = afe[j];
          nbflag = burnt[j];

          if (nbflag == 0)
          {

            nspin1 = e1[nedge].neigh_spin[0];
            nspin2 = e1[nedge].neigh_spin[1];
            nnode1 = e1[nedge].node_id[0];
            nnode2 = e1[nedge].node_id[1];

            // checking if neighbor edge has same neighboring spins...
            if (((cspin1 == nspin1) && (cspin2 == nspin2)) || ((cspin1 == nspin2) && (cspin2 == nspin1)))
            {
              spinFlag = 1;
            }
            else
            {
              spinFlag = 0;
            }

            // checking if neighor egde is contiguous...
            if ((cnode1 == nnode1) && (cnode2 != nnode2))
            {
              nodeFlag = 1;
            }
            else if ((cnode1 == nnode2) && (cnode2 != nnode1))
            {
              nodeFlag = 1;
            }
            else if ((cnode2 == nnode1) && (cnode1 != nnode2))
            {
              nodeFlag = 1;
            }
            else if ((cnode2 == nnode2) && (cnode1 != nnode1))
            {
              nodeFlag = 1;
            }
            else
            {
              nodeFlag = 0;
            }

            if (spinFlag == 1 && nodeFlag == 1)
            {

              head = head + 1;
              burnt_list[head] = nedge;
              burnt[j] = loopID;

            }
            else
            {
              // do nothing...
            }

          }
          else
          {
            // do nothing...
          }

        }

        if (tail == head)
        {
          coin = 0;
          tail = tail + 1;
          head = tail;
          loopID++;
        }
        else
        {
          tail = tail + 1;
          coin = 1;
        }

      } while (coin);

    }

  }

  count = (int *) malloc(loopID * sizeof(int));

  for (k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }

  for (k = 1; k < loopID; k++)
  {
    for (kk = 0; kk < nfedge; kk++)
    {
      if (k == burnt[kk])
      {
        count[k] = count[k] + 1;
      }
    }
  }

  // Let's make complete loops...
  numTri = 0;
  start = afc[0];
  end = afc[1];
  to = 0;
  from = 0;
  j1 = 1;

  for (j1 = 1; j1 < loopID; j1++)
  {

    openL = 0; // current loop is closed...

    numN = count[j1];
    //printf("%5d\n", numN);

    to = to + numN;
    from = to - numN;

    //printf("%5d %5d   ", from, to);

    burnt_loop = (int *) malloc((numN + 2) * sizeof(int));

    for (i1 = from; i1 < to; i1++)
    {

      cedge = burnt_list[i1];
      cnode1 = e1[cedge].node_id[0];
      cnode2 = e1[cedge].node_id[1];

      if (start == cnode1)
      {
        openL = 1; // loop is open...
        startEdge = cedge;
        flip = 0;
      }
      else if (start == cnode2)
      {
        openL = 1;
        startEdge = cedge;
        flip = 1;
      }
      else
      {
        // do nothing...
      }

    }

    //printf("   start edge & node: %5d %5d", startEdge, e1[startEdge].node_id[0]);
    //printf("   open flag: %2d", openL);
    //printf("   flip flag: %2d", flip);
    //printf("\n");

    if (openL == 1)
    { // if current loop is an open loop...

      if (flip == 1)
      {
        tnode = e1[startEdge].node_id[0];
        tspin = e1[startEdge].neigh_spin[0];
        e1[startEdge].node_id[0] = e1[startEdge].node_id[1];
        e1[startEdge].node_id[1] = tnode;
        e1[startEdge].neigh_spin[0] = e1[startEdge].neigh_spin[1];
        e1[startEdge].neigh_spin[1] = tspin;
      }
      else
      {
        // do nothing...
      }

      burnt_loop[0] = startEdge;
      index = 1;
      endNode = e1[startEdge].node_id[1];
      chaser = startEdge;

      do
      {

        for (n = from; n < to; n++)
        {

          cedge = burnt_list[n];
          cnode1 = e1[cedge].node_id[0];
          cnode2 = e1[cedge].node_id[1];
          if ((cedge != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = cedge;
            index++;
          }
          else if ((cedge != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = cedge;
            index++;
            // flip...
            tnode = e1[cedge].node_id[0];
            tspin = e1[cedge].neigh_spin[0];
            e1[cedge].node_id[0] = e1[cedge].node_id[1];
            e1[cedge].node_id[1] = tnode;
            e1[cedge].neigh_spin[0] = e1[cedge].neigh_spin[1];
            e1[cedge].neigh_spin[1] = tspin;
          }
          else
          {
            // do nothing...
          }

        }

        chaser = burnt_loop[index - 1];
        endNode = e1[chaser].node_id[1];

      } while (index < numN);

      if ((numN + 1) == 3)
      {
        numTri = numTri + 1;
      }
      else if ((numN + 1) > 3)
      {
        numTri = numTri + ((numN + 1) - 2);
      }
      else
      {
        //do nothing...
      }

      free(burnt_loop);

    }
    else
    { // if current loop is a closed one....i.e., openL = 0...

      startEdge = burnt_list[from];
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = e1[startEdge].node_id[1];
      chaser = startEdge;

      do
      {

        for (n = from; n < to; n++)
        {

          cedge = burnt_list[n];
          cnode1 = e1[cedge].node_id[0];
          cnode2 = e1[cedge].node_id[1];
          if ((cedge != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = cedge;
            index++;
          }
          else if ((cedge != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = cedge;
            index++;
            // flip...
            tnode = e1[cedge].node_id[0];
            tspin = e1[cedge].neigh_spin[0];
            e1[cedge].node_id[0] = e1[cedge].node_id[1];
            e1[cedge].node_id[1] = tnode;
            e1[cedge].neigh_spin[0] = e1[cedge].neigh_spin[1];
            e1[cedge].neigh_spin[1] = tspin;
          }
          else
          {
            // do nothing...
          }

        }

        chaser = burnt_loop[index - 1];
        endNode = e1[chaser].node_id[1];

      } while (index < numN);

      if (numN == 3)
      {
        numTri = numTri + 1;
      }
      else if (numN > 3)
      {
        numTri = numTri + (numN - 2);
      }
      else
      {
        //do nothing...
      }

      free(burnt_loop);
    }

  }

  /*
   int lid;
   k = 0;
   kk = 0;
   printf("%5d %5d\n", afc[0], afc[1]);
   for(k=0; k<nfedge; k++){
   cedge = burnt_list[k];
   for(kk=0; kk<nfedge; kk++){
   if(cedge==afe[kk]){
   lid = burnt[kk];
   }
   }
   printf("%5d   %3d   %5d %5d   %5d %5d\n", cedge, lid,
   e1[cedge].node_id[0], e1[cedge].node_id[1], e1[cedge].neigh_spin[0], e1[cedge].neigh_spin[1]);
   }
   printf("\n");
   */

  free(burnt);
  free(burnt_list);
  free(count);

  return (numTri);

}

int get_number_caseM_triangles(int *afe, struct node *v1, struct segment *e1, int nfedge, int *afc, int nfctr)
{

  int ii, i, j, k, kk, k1, n, i1, j1, n1;

  int loopID;
  int tail, head, coin;

  int cedge, nedge;
  int cspin1, cspin2, cnode1, cnode2;
  int nspin1, nspin2, nnode1, nnode2;
  int tspin, tnode;
  int nucleus, chaser;
  int start;
  int from, to;

  int spinFlag, nodeFlag, flip;

  int bflag, nbflag;
  int *burnt;
  int *burnt_list;


  int *count;
  int numN, numTri;

  //int tn1, tn2;

  int *burnt_loop;

  int openL; // if a loop is an open loop, it's 1; if closed, it's 0...
  int startEdge;
  int endNode;
  int index;

 // int temp;

  burnt = (int *) malloc(nfedge * sizeof(int));
  burnt_list = (int *) malloc(nfedge * sizeof(int));

  // initialize burn flags for face edges...
  for (ii = 0; ii < nfedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }

  loopID = 1;
  tail = 0;
  head = 0;

  for (i = 0; i < nfedge; i++)
  {

    cedge = afe[i];
    bflag = burnt[i];

    if (bflag == 0)
    {

      nucleus = cedge;
      burnt[i] = loopID;
      burnt_list[tail] = cedge;

      do
      {

        chaser = burnt_list[tail];

        cspin1 = e1[chaser].neigh_spin[0];
        cspin2 = e1[chaser].neigh_spin[1];
        cnode1 = e1[chaser].node_id[0];
        cnode2 = e1[chaser].node_id[1];

        for (j = 0; j < nfedge; j++)
        {

          nedge = afe[j];
          nbflag = burnt[j];

          if (nbflag == 0)
          {

            nspin1 = e1[nedge].neigh_spin[0];
            nspin2 = e1[nedge].neigh_spin[1];
            nnode1 = e1[nedge].node_id[0];
            nnode2 = e1[nedge].node_id[1];

            // checking if neighbor edge has same neighboring spins...
            if (((cspin1 == nspin1) && (cspin2 == nspin2)) || ((cspin1 == nspin2) && (cspin2 == nspin1)))
            {
              spinFlag = 1;
            }
            else
            {
              spinFlag = 0;
            }

            // checking if neighor egde is contiguous...
            if ((cnode1 == nnode1) && (cnode2 != nnode2))
            {
              nodeFlag = 1;
            }
            else if ((cnode1 == nnode2) && (cnode2 != nnode1))
            {
              nodeFlag = 1;
            }
            else if ((cnode2 == nnode1) && (cnode1 != nnode2))
            {
              nodeFlag = 1;
            }
            else if ((cnode2 == nnode2) && (cnode1 != nnode1))
            {
              nodeFlag = 1;
            }
            else
            {
              nodeFlag = 0;
            }

            if (spinFlag == 1 && nodeFlag == 1)
            {

              head = head + 1;
              burnt_list[head] = nedge;
              burnt[j] = loopID;

            }
            else
            {
              // do nothing...
            }

          }
          else
          {
            // do nothing...
          }

        }

        if (tail == head)
        {
          coin = 0;
          tail = tail + 1;
          head = tail;
          loopID++;
        }
        else
        {
          tail = tail + 1;
          coin = 1;
        }

      } while (coin);

    }

  }

  count = (int *) malloc(loopID * sizeof(int));

  for (k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }

  for (k = 1; k < loopID; k++)
  {
    for (kk = 0; kk < nfedge; kk++)
    {
      if (k == burnt[kk])
      {
        count[k] = count[k] + 1;
      }
    }
  }

  // Let's make complete loops...
  numTri = 0;
  to = 0;
  from = 0;
  j1 = 1;

  /*
   u = 0;
   printf("\n");
   for(u=0; u<nfctr; u++){
   printf("%5d ", afc[u]);
   }
   printf("\n");
   */

  for (j1 = 1; j1 < loopID; j1++)
  {

    openL = 0; // current loop is closed...

    numN = count[j1];
    //printf("%5d\n", numN);

    to = to + numN;
    from = to - numN;

    //printf("%5d %5d   ", from, to);

    burnt_loop = (int *) malloc((numN + 2) * sizeof(int));

    for (i1 = from; i1 < to; i1++)
    {

      cedge = burnt_list[i1];
      cnode1 = e1[cedge].node_id[0];
      cnode2 = e1[cedge].node_id[1];

      for (n1 = 0; n1 < nfctr; n1++)
      {

        start = afc[n1];

        if (start == cnode1)
        {
          openL = 1; // loop is open...
          startEdge = cedge;
          flip = 0;
        }
        else if (start == cnode2)
        {
          openL = 1;
          startEdge = cedge;
          flip = 1;
        }
        else
        {
          // do nothing...
        }

      }

    }

    //printf("   start edge & node: %5d %5d", startEdge, e1[startEdge].node_id[0]);
    //printf("   open flag: %2d", openL);
    //printf("   flip flag: %2d", flip);
    //printf("\n");


    if (openL == 1)
    { // if current loop is an open loop...

      if (flip == 1)
      {
        tnode = e1[startEdge].node_id[0];
        tspin = e1[startEdge].neigh_spin[0];
        e1[startEdge].node_id[0] = e1[startEdge].node_id[1];
        e1[startEdge].node_id[1] = tnode;
        e1[startEdge].neigh_spin[0] = e1[startEdge].neigh_spin[1];
        e1[startEdge].neigh_spin[1] = tspin;
      }
      else
      {
        // do nothing...
      }

      burnt_loop[0] = startEdge;
      index = 1;
      endNode = e1[startEdge].node_id[1];
      chaser = startEdge;

      do
      {

        for (n = from; n < to; n++)
        {

          cedge = burnt_list[n];
          cnode1 = e1[cedge].node_id[0];
          cnode2 = e1[cedge].node_id[1];
          if ((cedge != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = cedge;
            index++;
          }
          else if ((cedge != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = cedge;
            index++;
            // flip...
            tnode = e1[cedge].node_id[0];
            tspin = e1[cedge].neigh_spin[0];
            e1[cedge].node_id[0] = e1[cedge].node_id[1];
            e1[cedge].node_id[1] = tnode;
            e1[cedge].neigh_spin[0] = e1[cedge].neigh_spin[1];
            e1[cedge].neigh_spin[1] = tspin;
          }
          else
          {
            // do nothing...
          }

        }

        chaser = burnt_loop[index - 1];
        endNode = e1[chaser].node_id[1];

      } while (index < numN);

      /*
       u = 0;
       for(u=0; u<index; u++){
       temp = burnt_loop[u];
       printf("%5d   %3d   %5d %5d   %5d %5d\n", temp, j1,
       e1[temp].node_id[0], e1[temp].node_id[1], e1[temp].neigh_spin[0], e1[temp].neigh_spin[1]);
       }
       */

      if ((numN + 2) == 3)
      {
        numTri = numTri + 1;
      }
      else if ((numN + 2) > 3)
      {
        numTri = numTri + ((numN + 2) - 2);
      }
      else
      {
        //do nothing...
      }

      free(burnt_loop);

    }
    else
    { // if current loop is a closed one....i.e., openL = 0...

      startEdge = burnt_list[from];
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = e1[startEdge].node_id[1];
      chaser = startEdge;

      do
      {

        for (n = from; n < to; n++)
        {

          cedge = burnt_list[n];
          cnode1 = e1[cedge].node_id[0];
          cnode2 = e1[cedge].node_id[1];
          if ((cedge != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = cedge;
            index++;
          }
          else if ((cedge != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = cedge;
            index++;
            // flip...
            tnode = e1[cedge].node_id[0];
            tspin = e1[cedge].neigh_spin[0];
            e1[cedge].node_id[0] = e1[cedge].node_id[1];
            e1[cedge].node_id[1] = tnode;
            e1[cedge].neigh_spin[0] = e1[cedge].neigh_spin[1];
            e1[cedge].neigh_spin[1] = tspin;
          }
          else
          {
            // do nothing...
          }

        }

        chaser = burnt_loop[index - 1];
        endNode = e1[chaser].node_id[1];

      } while (index < numN);

      /*
       u = 0;
       for(u=0; u<index; u++){
       temp = burnt_loop[u];
       printf("%5d   %3d   %5d %5d   %5d %5d\n", temp, j1,
       e1[temp].node_id[0], e1[temp].node_id[1], e1[temp].neigh_spin[0], e1[temp].neigh_spin[1]);
       }
       */

      if (numN == 3)
      {
        numTri = numTri + 1;
      }
      else if (numN > 3)
      {
        numTri = numTri + (numN - 2);
      }
      else
      {
        //do nothing...
      }

      free(burnt_loop);
    }

  }

  /*
   int lid;
   int u;
   k = 0;
   kk = 0;
   for(u=0; u<nfctr; u++){
   printf("%5d ", afc[u]);
   }
   printf("\n");
   for(k=0; k<nfedge; k++){
   cedge = burnt_list[k];
   for(kk=0; kk<nfedge; kk++){
   if(cedge==afe[kk]){
   lid = burnt[kk];
   }
   }
   printf("%5d   %3d   %5d %5d   %5d %5d\n", cedge, lid,
   e1[cedge].node_id[0], e1[cedge].node_id[1], e1[cedge].neigh_spin[0], e1[cedge].neigh_spin[1]);
   }
   printf("\n");
   */

  free(burnt);
  free(burnt_list);
  free(count);

  return (numTri);

}

int get_triangles(struct patch *t, struct face *sq, struct node *v, struct segment *e, int ns, int nsp, int xDim)
{

  int i, ii, i1, i2;

  int sqID[6];
  int tsq; // current sq id...
  int tnfe; // temp number of edges...

  int nFC; // number of FC turned on...
  int tFC;
  int nFE; // number of face edges...
  int eff; // all the squares effective?...
  int cubeFlag; // if 1, we can do marching cube; if 0, useless...
  int bodyCtr; // cube center node...

  int *arrayFE;
  int *arrayFC;

  int fcid;
  int tindex;
  int tidIn, tidOut;
  arrayFC = (int *) malloc(6 * sizeof(int));
  tidIn = 0;

  for (i = 1; i <= nsp; i++)
  {

    cubeFlag = 0;

    // for each marching cube, collect faces...
    sqID[0] = 3 * (i - 1);
    sqID[1] = 3 * (i - 1) + 1;
    sqID[2] = 3 * (i - 1) + 2;
    sqID[3] = 3 * i + 2;
    sqID[4] = 3 * (i + xDim - 1) + 1;
    sqID[5] = 3 * (i + nsp - 1);

    nFC = 0;
    nFE = 0;
    eff = 0;
    bodyCtr = 7 * (i - 1) + 6;

    //initialize face center array...
    for (ii = 0; ii < 6; ii++)
    {
      arrayFC[ii] = -1;
    }

    // Count the number of face center turned on and face edges...
    fcid = 0;
    ii = 0;
    for (ii = 0; ii < 6; ii++)
    {
      tsq = sqID[ii];
      tFC = sq[tsq].turnFC;
      if (tFC == 1)
      {
        arrayFC[fcid] = sq[tsq].FCnode;
        fcid++;
      }
      nFC = nFC + tFC;
      nFE = nFE + sq[tsq].nEdge;
      eff = eff + sq[tsq].effect;
    }

    if (fcid != nFC)
    {
      printf("Something's wrong with counting face centers...\n");
    }

    if (eff == 6)
    {
      cubeFlag = 1;
    }

    // Checking the number of edges for loops in the cube...
    if (nFE >= 3)
    {
      //printf("%4d", nFE);
    }
    else if (nFE > 0 && nFE < 3)
    {
      //printf("\t? %5d %3d\n", i, nFE);  // We have to ignore this case...
    }
    else if (nFE == 0)
    {
      //printf("\tcube inside grain\n");
    }
    else
    {
      printf("\twhat?\n");
    }

    // if the current marching cube is a collection of 6 effective squares...and
    // the number of face edges at least 3...
    // when nFE==2, it doen't happen
    // when nFE==1, the edge will contribute for the neighboring marching cube...
    // when nFE==0, it means the cube is inside a grain...
    if (cubeFlag == 1 && nFE > 2)
    {

      // Make edge array for each marching cube...
      arrayFE = (int *) malloc(nFE * sizeof(int));
      tindex = 0;
      for (i1 = 0; i1 < 6; i1++)
      {
        tsq = sqID[i1];
        tnfe = sq[tsq].nEdge;
        if (tnfe != 0)
        {
          for (i2 = 0; i2 < tnfe; i2++)
          {
            arrayFE[tindex] = sq[tsq].edge_id[i2];
            tindex++;
          }
        }
      }

      if (tindex != nFE)
      {
        printf("Something wrong with counting number of edges for marching cube...\n");
      }

      // Consider each case as Z. Wu's paper...
      if (nFC == 0)
      { // when there's no face center

        //printf("%5d ", tidIn);
        get_case0_triangles(t, arrayFE, v, e, nFE, tidIn, &tidOut);
        tidIn = tidOut;
        //printf("%5d\n", tidIn);

      }
      else if (nFC == 2)
      {

        //printf("%5d ", tidIn);
        get_case2_triangles(t, arrayFE, v, e, nFE, arrayFC, nFC, tidIn, &tidOut);
        tidIn = tidOut;
        //printf("%5d\n", tidIn);

      }
      else if (nFC > 2 && nFC <= 6)
      {

        get_caseM_triangles(t, arrayFE, v, e, nFE, arrayFC, nFC, tidIn, &tidOut, bodyCtr);
        tidIn = tidOut;

      }
      else
      {

        //printf("Somthing's wrong in counting face centers turned on...\n");
        //printf("%5d %10d\n\n", nFC, i);
      }

      free(arrayFE);

    }

  }

  //printf("%5d\n", tidIn);

  return 0;
}

void get_case0_triangles(struct patch *t1, int *afe, struct node *v1, struct segment *e1, int nfedge, int tin, int *tout)
{

  int ii, i, j, jj, k, kk, k1, mm;

  int loopID;
  int tail, head, coin;

  int cedge, nedge;
  int cspin1, cspin2, cnode1, cnode2;
  int nspin1, nspin2, nnode1, nnode2;
//  int tspin, tnode;
  int nucleus, chaser;
//  int start, end;

  int spinFlag, nodeFlag, flip;

  int bflag, nbflag;
  int *burnt;
  int *burnt_list;


  int *count;
  int numN, sumN;
  int from;

 // int te, tn1, tn2;

  int *loop;
  int ctid;
  int front, back;
  int ce;
  int te0, te1, te2, tv0, tv1, tv2;
  int numT, cnumT, new_node0, new_node1;

  burnt = (int *) malloc(nfedge * sizeof(int));
  burnt_list = (int *) malloc(nfedge * sizeof(int));


  // initialize burn flags for face edges...
  for (ii = 0; ii < nfedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }

  loopID = 1;
  tail = 0;
  head = 0;

  for (i = 0; i < nfedge; i++)
  {

    cedge = afe[i];
    bflag = burnt[i];

    if (bflag == 0)
    {

      nucleus = cedge;
      burnt[i] = loopID;
      burnt_list[tail] = cedge;

      do
      {

        chaser = burnt_list[tail];

        cspin1 = e1[chaser].neigh_spin[0];
        cspin2 = e1[chaser].neigh_spin[1];
        cnode1 = e1[chaser].node_id[0];
        cnode2 = e1[chaser].node_id[1];

        for (j = 0; j < nfedge; j++)
        {

          nedge = afe[j];
          nbflag = burnt[j];

          if (nbflag == 0)
          {

            nspin1 = e1[nedge].neigh_spin[0];
            nspin2 = e1[nedge].neigh_spin[1];
            nnode1 = e1[nedge].node_id[0];
            nnode2 = e1[nedge].node_id[1];

            // checking if neighbor edge has same neighboring spins...
            if (((cspin1 == nspin1) && (cspin2 == nspin2)) || ((cspin1 == nspin2) && (cspin2 == nspin1)))
            {
              spinFlag = 1;
            }
            else
            {
              spinFlag = 0;
            }

            // checking if neighor egde is contiguous...
            if ((cnode2 == nnode1) && (cnode1 != nnode2))
            {
              nodeFlag = 1;
              flip = 0;
            }
            else if ((cnode2 == nnode2) && (cnode1 != nnode1))
            {
              nodeFlag = 1;
              flip = 1;
            }
            else
            {
              nodeFlag = 0;
              flip = 0;
            }

            if (spinFlag == 1 && nodeFlag == 1)
            {

              head = head + 1;
              burnt_list[head] = nedge;
              burnt[j] = loopID;

              if (flip == 1)
              {
                //e1[nedge].neigh_spin[0] = nspin2;
                //e1[nedge].neigh_spin[1] = nspin1;
                e1[nedge].node_id[0] = nnode2;
                e1[nedge].node_id[1] = nnode1;
              }

            }
            else
            {
              // do nothing...
            }

          }
          else
          {
            // do nothing...
          }

        }

        if (tail == head)
        {
          coin = 0;
          tail = tail + 1;
          head = tail;
          loopID++;
        }
        else
        {
          tail = tail + 1;
          coin = 1;
        }

      } while (coin);

    }

  }

  count = (int *) malloc(loopID * sizeof(int));

  for (k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }

  for (k = 1; k < loopID; k++)
  {
    for (kk = 0; kk < nfedge; kk++)
    {
      if (k == burnt[kk])
      {
        count[k] = count[k] + 1;
      }
    }
  }

  // Let's do triangulation...
  sumN = 0;
  ctid = tin;

  for (jj = 1; jj < loopID; jj++)
  {

    numN = count[jj];
    sumN = sumN + numN;
    from = sumN - numN;

    loop = (int *) malloc(numN * sizeof(int));

    for (mm = 0; mm < numN; mm++)
    {
      loop[mm] = burnt_list[from + mm];
    }

    if (numN == 3)
    {

      te0 = loop[0];
      te1 = loop[1];
      te2 = loop[2];
      t1[ctid].v_id[0] = e1[te0].node_id[0];
      t1[ctid].v_id[1] = e1[te1].node_id[0];
      t1[ctid].v_id[2] = e1[te2].node_id[0];
      t1[ctid].nSpin[0] = e1[te0].neigh_spin[0];
      t1[ctid].nSpin[1] = e1[te0].neigh_spin[1];
      ctid++;

    }
    else if (numN > 3)
    {

      numT = numN - 2;
      cnumT = 0;
      front = 0;
      back = numN - 1;

      te0 = loop[front];
      te1 = loop[back];
      tv0 = e1[te0].node_id[0];
      tv1 = e1[te0].node_id[1];
      tv2 = e1[te1].node_id[0];
      t1[ctid].v_id[0] = tv0;
      t1[ctid].v_id[1] = tv1;
      t1[ctid].v_id[2] = tv2;
      t1[ctid].nSpin[0] = e1[te0].neigh_spin[0];
      t1[ctid].nSpin[1] = e1[te0].neigh_spin[1];
      new_node0 = tv2;
      new_node1 = tv1;

      cnumT++;
      ctid++;

      do
      {

        if ((cnumT % 2) != 0)
        {

          front = front + 1;
          ce = loop[front];
          tv0 = e1[ce].node_id[0];
          tv1 = e1[ce].node_id[1];
          tv2 = new_node0;
          t1[ctid].v_id[0] = tv0;
          t1[ctid].v_id[1] = tv1;
          t1[ctid].v_id[2] = tv2;
          t1[ctid].nSpin[0] = e1[ce].neigh_spin[0];
          t1[ctid].nSpin[1] = e1[ce].neigh_spin[1];
          new_node0 = tv1;
          cnumT++;
          ctid++;

        }
        else
        {

          back = back - 1;
          ce = loop[back];
          tv0 = e1[ce].node_id[0];
          tv1 = e1[ce].node_id[1];
          tv2 = new_node0;
          t1[ctid].v_id[0] = tv0;
          t1[ctid].v_id[1] = tv1;
          t1[ctid].v_id[2] = tv2;
          t1[ctid].nSpin[0] = e1[ce].neigh_spin[0];
          t1[ctid].nSpin[1] = e1[ce].neigh_spin[1];
          new_node0 = tv0;
          cnumT++;
          ctid++;
        }

      } while (cnumT < numT);

    }
    else
    {
      // do nothing...
    }

    free(loop);

  }

  *tout = ctid;
  //printf("%10d\n", ctid);

  free(burnt);
  free(burnt_list);
  free(count);

}

void get_case2_triangles(struct patch *t1, int *afe, struct node *v1, struct segment *e1, int nfedge, int *afc, int nfctr, int tin, int *tout)
{

  int ii, i, j, k, kk, k1, n, i1, j1;

  int loopID;
  int tail, head, coin;

  int cedge, nedge;
  int cspin1, cspin2, cnode1, cnode2;
  int nspin1, nspin2, nnode1, nnode2;
  int tspin, tnode;
  int nucleus, chaser;
  int start, end;
  int from, to;

  int spinFlag, nodeFlag, flip;

  int bflag, nbflag;
  int *burnt;
  int *burnt_list;


  int *count;
  int numN, numTri;

 // int te, tn1, tn2;

  int *burnt_loop;

  int openL; // if a loop is an open loop, it's 1; if closed, it's 0...
  int startEdge;
  int endNode;
  int index;

  //int sumN;

  int ctid;
  int front, back;
  int ce;
  int te0, te1, te2, tv0, tv1, tv2;
  int numT, cnumT, new_node0, new_node1;

  burnt = (int *) malloc(nfedge * sizeof(int));
  burnt_list = (int *) malloc(nfedge * sizeof(int));


  // initialize burn flags for face edges...
  for (ii = 0; ii < nfedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }

  loopID = 1;
  tail = 0;
  head = 0;

  for (i = 0; i < nfedge; i++)
  {

    cedge = afe[i];
    bflag = burnt[i];

    if (bflag == 0)
    {

      nucleus = cedge;
      burnt[i] = loopID;
      burnt_list[tail] = cedge;

      do
      {

        chaser = burnt_list[tail];

        cspin1 = e1[chaser].neigh_spin[0];
        cspin2 = e1[chaser].neigh_spin[1];
        cnode1 = e1[chaser].node_id[0];
        cnode2 = e1[chaser].node_id[1];

        for (j = 0; j < nfedge; j++)
        {

          nedge = afe[j];
          nbflag = burnt[j];

          if (nbflag == 0)
          {

            nspin1 = e1[nedge].neigh_spin[0];
            nspin2 = e1[nedge].neigh_spin[1];
            nnode1 = e1[nedge].node_id[0];
            nnode2 = e1[nedge].node_id[1];

            // checking if neighbor edge has same neighboring spins...
            if (((cspin1 == nspin1) && (cspin2 == nspin2)) || ((cspin1 == nspin2) && (cspin2 == nspin1)))
            {
              spinFlag = 1;
            }
            else
            {
              spinFlag = 0;
            }

            // checking if neighor egde is contiguous...
            if ((cnode1 == nnode1) && (cnode2 != nnode2))
            {
              nodeFlag = 1;
            }
            else if ((cnode1 == nnode2) && (cnode2 != nnode1))
            {
              nodeFlag = 1;
            }
            else if ((cnode2 == nnode1) && (cnode1 != nnode2))
            {
              nodeFlag = 1;
            }
            else if ((cnode2 == nnode2) && (cnode1 != nnode1))
            {
              nodeFlag = 1;
            }
            else
            {
              nodeFlag = 0;
            }

            if (spinFlag == 1 && nodeFlag == 1)
            {

              head = head + 1;
              burnt_list[head] = nedge;
              burnt[j] = loopID;

            }
            else
            {
              // do nothing...
            }

          }
          else
          {
            // do nothing...
          }

        }

        if (tail == head)
        {
          coin = 0;
          tail = tail + 1;
          head = tail;
          loopID++;
        }
        else
        {
          tail = tail + 1;
          coin = 1;
        }

      } while (coin);

    }

  }

  count = (int *) malloc(loopID * sizeof(int));

  for (k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }

  for (k = 1; k < loopID; k++)
  {
    for (kk = 0; kk < nfedge; kk++)
    {
      if (k == burnt[kk])
      {
        count[k] = count[k] + 1;
      }
    }
  }

  // Let's make complete loops...
  numTri = 0;
  start = afc[0];
  end = afc[1];
  //printf("\n\n");
  //printf("%10d %10d\n", start, end);
  to = 0;
  from = 0;
  j1 = 1;

  // Let's do triangulation...
  ctid = tin;

  for (j1 = 1; j1 < loopID; j1++)
  {

    openL = 0; // current loop is closed...

    numN = count[j1];

    to = to + numN;
    from = to - numN;

    burnt_loop = (int *) malloc(numN * sizeof(int));

    for (i1 = from; i1 < to; i1++)
    {

      cedge = burnt_list[i1];
      cnode1 = e1[cedge].node_id[0];
      cnode2 = e1[cedge].node_id[1];

      if (start == cnode1)
      {
        openL = 1; // loop is open...
        startEdge = cedge;
        flip = 0;
      }
      else if (start == cnode2)
      {
        openL = 1;
        startEdge = cedge;
        flip = 1;
      }
      else
      {
        // do nothing...
      }

    }

    //printf("   start edge & node: %5d %5d", startEdge, e1[startEdge].node_id[0]);
    //printf("   open flag: %2d", openL);
    //printf("   flip flag: %2d", flip);
    //printf("\n");

    if (openL == 1)
    { // if current loop is an open loop...

      if (flip == 1)
      {
        tnode = e1[startEdge].node_id[0];
        tspin = e1[startEdge].neigh_spin[0];
        e1[startEdge].node_id[0] = e1[startEdge].node_id[1];
        e1[startEdge].node_id[1] = tnode;
        e1[startEdge].neigh_spin[0] = e1[startEdge].neigh_spin[1];
        e1[startEdge].neigh_spin[1] = tspin;
      }
      else
      {
        // do nothing...
      }

      burnt_loop[0] = startEdge;
      index = 1;
      endNode = e1[startEdge].node_id[1];
      chaser = startEdge;

      do
      {

        for (n = from; n < to; n++)
        {

          cedge = burnt_list[n];
          cnode1 = e1[cedge].node_id[0];
          cnode2 = e1[cedge].node_id[1];
          if ((cedge != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = cedge;
            index++;
          }
          else if ((cedge != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = cedge;
            index++;
            // flip...
            tnode = e1[cedge].node_id[0];
            tspin = e1[cedge].neigh_spin[0];
            e1[cedge].node_id[0] = e1[cedge].node_id[1];
            e1[cedge].node_id[1] = tnode;
            e1[cedge].neigh_spin[0] = e1[cedge].neigh_spin[1];
            e1[cedge].neigh_spin[1] = tspin;
          }
          else
          {
            // do nothing...
          }

        }

        chaser = burnt_loop[index - 1];
        endNode = e1[chaser].node_id[1];

      } while (index < numN);

      if (numN == 2)
      {

        te0 = burnt_loop[0];
        te1 = burnt_loop[1];
        t1[ctid].v_id[0] = e1[te0].node_id[0];
        t1[ctid].v_id[1] = e1[te1].node_id[0];
        t1[ctid].v_id[2] = e1[te1].node_id[1];
        t1[ctid].nSpin[0] = e1[te0].neigh_spin[0];
        t1[ctid].nSpin[1] = e1[te0].neigh_spin[1];
        ctid++;

      }
      else if (numN > 2)
      {

        numT = numN - 1; // Note that it's open loop...
        cnumT = 0;
        front = 0;
        back = numN;

        te0 = burnt_loop[front];
        te1 = burnt_loop[back - 1];
        tv0 = e1[te0].node_id[0];
        tv1 = e1[te0].node_id[1];
        tv2 = e1[te1].node_id[1];
        t1[ctid].v_id[0] = tv0;
        t1[ctid].v_id[1] = tv1;
        t1[ctid].v_id[2] = tv2;
        t1[ctid].nSpin[0] = e1[te0].neigh_spin[0];
        t1[ctid].nSpin[1] = e1[te0].neigh_spin[1];
        new_node0 = tv2;
        new_node1 = tv1;

        cnumT++;
        ctid++;

        do
        {

          if ((cnumT % 2) != 0)
          {

            front = front + 1;
            ce = burnt_loop[front];
            tv0 = e1[ce].node_id[0];
            tv1 = e1[ce].node_id[1];
            tv2 = new_node0;
            t1[ctid].v_id[0] = tv0;
            t1[ctid].v_id[1] = tv1;
            t1[ctid].v_id[2] = tv2;
            t1[ctid].nSpin[0] = e1[ce].neigh_spin[0];
            t1[ctid].nSpin[1] = e1[ce].neigh_spin[1];
            new_node0 = tv1;
            cnumT++;
            ctid++;

          }
          else
          {

            back = back - 1;
            ce = burnt_loop[back];
            tv0 = e1[ce].node_id[0];
            tv1 = e1[ce].node_id[1];
            tv2 = new_node0;
            t1[ctid].v_id[0] = tv0;
            t1[ctid].v_id[1] = tv1;
            t1[ctid].v_id[2] = tv2;
            t1[ctid].nSpin[0] = e1[ce].neigh_spin[0];
            t1[ctid].nSpin[1] = e1[ce].neigh_spin[1];
            new_node0 = tv0;
            cnumT++;
            ctid++;
          }

        } while (cnumT < numT);

      }
      else
      {
        // do nothing...
      }

      /*
       int s;
       int temp, temp1, temp2;
       for(s=0; s<numN; s++){
       temp = burnt_loop[s];
       temp1 = e1[temp].node_id[0];
       temp2 = e1[temp].node_id[1];
       printf("%5d   %5d %5d\n", temp, temp1, temp2);
       }
       printf("\n");
       */

      free(burnt_loop);

    }
    else
    { // if current loop is a closed one....i.e., openL = 0...

      startEdge = burnt_list[from];
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = e1[startEdge].node_id[1];
      chaser = startEdge;

      do
      {

        for (n = from; n < to; n++)
        {

          cedge = burnt_list[n];
          cnode1 = e1[cedge].node_id[0];
          cnode2 = e1[cedge].node_id[1];
          if ((cedge != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = cedge;
            index++;
          }
          else if ((cedge != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = cedge;
            index++;
            // flip...
            tnode = e1[cedge].node_id[0];
            tspin = e1[cedge].neigh_spin[0];
            e1[cedge].node_id[0] = e1[cedge].node_id[1];
            e1[cedge].node_id[1] = tnode;
            e1[cedge].neigh_spin[0] = e1[cedge].neigh_spin[1];
            e1[cedge].neigh_spin[1] = tspin;
          }
          else
          {
            // do nothing...
          }

        }

        chaser = burnt_loop[index - 1];
        endNode = e1[chaser].node_id[1];

      } while (index < numN);

      if (numN == 3)
      {

        te0 = burnt_loop[0];
        te1 = burnt_loop[1];
        te2 = burnt_loop[2];
        t1[ctid].v_id[0] = e1[te0].node_id[0];
        t1[ctid].v_id[1] = e1[te1].node_id[0];
        t1[ctid].v_id[2] = e1[te2].node_id[0];
        t1[ctid].nSpin[0] = e1[te0].neigh_spin[0];
        t1[ctid].nSpin[1] = e1[te0].neigh_spin[1];
        ctid++;

      }
      else if (numN > 3)
      {

        numT = numN - 2;
        cnumT = 0;
        front = 0;
        back = numN - 1;

        te0 = burnt_loop[front];
        te1 = burnt_loop[back];
        tv0 = e1[te0].node_id[0];
        tv1 = e1[te0].node_id[1];
        tv2 = e1[te1].node_id[0];
        t1[ctid].v_id[0] = tv0;
        t1[ctid].v_id[1] = tv1;
        t1[ctid].v_id[2] = tv2;
        t1[ctid].nSpin[0] = e1[te0].neigh_spin[0];
        t1[ctid].nSpin[1] = e1[te0].neigh_spin[1];
        new_node0 = tv2;
        new_node1 = tv1;

        cnumT++;
        ctid++;

        do
        {

          if ((cnumT % 2) != 0)
          {

            front = front + 1;
            ce = burnt_loop[front];
            tv0 = e1[ce].node_id[0];
            tv1 = e1[ce].node_id[1];
            tv2 = new_node0;
            t1[ctid].v_id[0] = tv0;
            t1[ctid].v_id[1] = tv1;
            t1[ctid].v_id[2] = tv2;
            t1[ctid].nSpin[0] = e1[ce].neigh_spin[0];
            t1[ctid].nSpin[1] = e1[ce].neigh_spin[1];
            new_node0 = tv1;
            cnumT++;
            ctid++;

          }
          else
          {

            back = back - 1;
            ce = burnt_loop[back];
            tv0 = e1[ce].node_id[0];
            tv1 = e1[ce].node_id[1];
            tv2 = new_node0;
            t1[ctid].v_id[0] = tv0;
            t1[ctid].v_id[1] = tv1;
            t1[ctid].v_id[2] = tv2;
            t1[ctid].nSpin[0] = e1[ce].neigh_spin[0];
            t1[ctid].nSpin[1] = e1[ce].neigh_spin[1];
            new_node0 = tv0;
            cnumT++;
            ctid++;
          }

        } while (cnumT < numT);

      }
      else
      {
        // do nothing...
      }

      /*
       int s1;
       int ttemp, ttemp1, ttemp2;
       for(s1=0; s1<numN; s1++){
       ttemp = burnt_loop[s1];
       ttemp1 = e1[ttemp].node_id[0];
       ttemp2 = e1[ttemp].node_id[1];
       printf("%5d   %5d %5d\n", ttemp, ttemp1, ttemp2);
       }
       printf("\n");
       */

      free(burnt_loop);

    }

  }

  *tout = ctid;
  //printf("%10d ", ctid);

  free(burnt);
  free(burnt_list);
  free(count);

}

void get_caseM_triangles(struct patch *t1, int *afe, struct node *v1, struct segment *e1, int nfedge, int *afc, int nfctr, int tin, int *tout, int ccn)
{

  int ii, i, j, k, kk, k1, n, i1, j1, n1, iii;

  int loopID;
  int tail, head, coin;

  int cedge, nedge;
  int cspin1, cspin2, cnode1, cnode2;
  int nspin1, nspin2, nnode1, nnode2;
  int tspin, tnode;
  int nucleus, chaser;
  int start;
  int from, to;

  int spinFlag, nodeFlag, flip;

  int bflag, nbflag;
  int *burnt;
  int *burnt_list;


  int *count;
  int numN, numTri;

  int tn0, tn1;

  int *burnt_loop;

  int openL; // if a loop is an open loop, it's 1; if closed, it's 0...
  int startEdge;
  int endNode;
  int index;

//  int u, temp;
//
//  int sumN;

  int ctid;
  int front, back;
  int ce;
  int te0, te1, te2, tv0, tv1, tv2, ts0, ts1;
  int numT, cnumT, new_node0, new_node1;

  burnt = (int *) malloc(nfedge * sizeof(int));
  burnt_list = (int *) malloc(nfedge * sizeof(int));


  // initialize burn flags for face edges...
  for (ii = 0; ii < nfedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }

  loopID = 1;
  tail = 0;
  head = 0;

  for (i = 0; i < nfedge; i++)
  {

    cedge = afe[i];
    bflag = burnt[i];

    if (bflag == 0)
    {

      nucleus = cedge;
      burnt[i] = loopID;
      burnt_list[tail] = cedge;

      do
      {

        chaser = burnt_list[tail];

        cspin1 = e1[chaser].neigh_spin[0];
        cspin2 = e1[chaser].neigh_spin[1];
        cnode1 = e1[chaser].node_id[0];
        cnode2 = e1[chaser].node_id[1];

        for (j = 0; j < nfedge; j++)
        {

          nedge = afe[j];
          nbflag = burnt[j];

          if (nbflag == 0)
          {

            nspin1 = e1[nedge].neigh_spin[0];
            nspin2 = e1[nedge].neigh_spin[1];
            nnode1 = e1[nedge].node_id[0];
            nnode2 = e1[nedge].node_id[1];

            // checking if neighbor edge has same neighboring spins...
            if (((cspin1 == nspin1) && (cspin2 == nspin2)) || ((cspin1 == nspin2) && (cspin2 == nspin1)))
            {
              spinFlag = 1;
            }
            else
            {
              spinFlag = 0;
            }

            // checking if neighor egde is contiguous...
            if ((cnode1 == nnode1) && (cnode2 != nnode2))
            {
              nodeFlag = 1;
            }
            else if ((cnode1 == nnode2) && (cnode2 != nnode1))
            {
              nodeFlag = 1;
            }
            else if ((cnode2 == nnode1) && (cnode1 != nnode2))
            {
              nodeFlag = 1;
            }
            else if ((cnode2 == nnode2) && (cnode1 != nnode1))
            {
              nodeFlag = 1;
            }
            else
            {
              nodeFlag = 0;
            }

            if (spinFlag == 1 && nodeFlag == 1)
            {

              head = head + 1;
              burnt_list[head] = nedge;
              burnt[j] = loopID;

            }
            else
            {
              // do nothing...
            }

          }
          else
          {
            // do nothing...
          }

        }

        if (tail == head)
        {
          coin = 0;
          tail = tail + 1;
          head = tail;
          loopID++;
        }
        else
        {
          tail = tail + 1;
          coin = 1;
        }

      } while (coin);

    }

  }

  count = (int *) malloc(loopID * sizeof(int));

  for (k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }

  for (k = 1; k < loopID; k++)
  {
    for (kk = 0; kk < nfedge; kk++)
    {
      if (k == burnt[kk])
      {
        count[k] = count[k] + 1;
      }
    }
  }

  // Let's make complete loops...
  numTri = 0;
  to = 0;
  from = 0;
  j1 = 1;

  ctid = tin;

  for (j1 = 1; j1 < loopID; j1++)
  {

    openL = 0; // current loop is closed...

    numN = count[j1];

    to = to + numN;
    from = to - numN;

    burnt_loop = (int *) malloc(numN * sizeof(int));

    for (i1 = from; i1 < to; i1++)
    {

      cedge = burnt_list[i1];
      cnode1 = e1[cedge].node_id[0];
      cnode2 = e1[cedge].node_id[1];

      for (n1 = 0; n1 < nfctr; n1++)
      {

        start = afc[n1];

        if (start == cnode1)
        {
          openL = 1; // loop is open...
          startEdge = cedge;
          flip = 0;
        }
        else if (start == cnode2)
        {
          openL = 1;
          startEdge = cedge;
          flip = 1;
        }
        else
        {
          // do nothing...
        }
      }

    }

    if (openL == 1)
    { // if current loop is an open loop...

      if (flip == 1)
      {
        tnode = e1[startEdge].node_id[0];
        tspin = e1[startEdge].neigh_spin[0];
        e1[startEdge].node_id[0] = e1[startEdge].node_id[1];
        e1[startEdge].node_id[1] = tnode;
        e1[startEdge].neigh_spin[0] = e1[startEdge].neigh_spin[1];
        e1[startEdge].neigh_spin[1] = tspin;
      }
      else
      {
        // do nothing...
      }

      burnt_loop[0] = startEdge;
      index = 1;
      endNode = e1[startEdge].node_id[1];
      chaser = startEdge;

      do
      {

        for (n = from; n < to; n++)
        {

          cedge = burnt_list[n];
          cnode1 = e1[cedge].node_id[0];
          cnode2 = e1[cedge].node_id[1];
          if ((cedge != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = cedge;
            index++;
          }
          else if ((cedge != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = cedge;
            index++;
            // flip...
            tnode = e1[cedge].node_id[0];
            tspin = e1[cedge].neigh_spin[0];
            e1[cedge].node_id[0] = e1[cedge].node_id[1];
            e1[cedge].node_id[1] = tnode;
            e1[cedge].neigh_spin[0] = e1[cedge].neigh_spin[1];
            e1[cedge].neigh_spin[1] = tspin;
          }
          else
          {
            // do nothing...
          }

        }

        chaser = burnt_loop[index - 1];
        endNode = e1[chaser].node_id[1];

      } while (index < numN);

      /*
       u = 0;
       for(u=0; u<index; u++){
       temp = burnt_loop[u];
       printf("%5d   %3d   %5d %5d   %5d %5d\n", temp, j1,
       e1[temp].node_id[0], e1[temp].node_id[1], e1[temp].neigh_spin[0], e1[temp].neigh_spin[1]);
       }
       */

      //triangulation...
      for (iii = 0; iii < numN; iii++)
      {
        ce = burnt_loop[iii];
        tn0 = e1[ce].node_id[0];
        tn1 = e1[ce].node_id[1];
        ts0 = e1[ce].neigh_spin[0];
        ts1 = e1[ce].neigh_spin[1];
        t1[ctid].v_id[0] = ccn;
        t1[ctid].v_id[1] = tn0;
        t1[ctid].v_id[2] = tn1;
        t1[ctid].nSpin[0] = ts0;
        t1[ctid].nSpin[1] = ts1;
        ctid++;
      }

      free(burnt_loop);

    }
    else
    { // if current loop is a closed one....i.e., openL = 0...

      startEdge = burnt_list[from];
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = e1[startEdge].node_id[1];
      chaser = startEdge;

      do
      {

        for (n = from; n < to; n++)
        {

          cedge = burnt_list[n];
          cnode1 = e1[cedge].node_id[0];
          cnode2 = e1[cedge].node_id[1];
          if ((cedge != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = cedge;
            index++;
          }
          else if ((cedge != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = cedge;
            index++;
            // flip...
            tnode = e1[cedge].node_id[0];
            tspin = e1[cedge].neigh_spin[0];
            e1[cedge].node_id[0] = e1[cedge].node_id[1];
            e1[cedge].node_id[1] = tnode;
            e1[cedge].neigh_spin[0] = e1[cedge].neigh_spin[1];
            e1[cedge].neigh_spin[1] = tspin;
          }
          else
          {
            // do nothing...
          }

        }

        chaser = burnt_loop[index - 1];
        endNode = e1[chaser].node_id[1];

      } while (index < numN);

      /*
       u = 0;
       for(u=0; u<index; u++){
       temp = burnt_loop[u];
       printf("%5d   %3d   %5d %5d   %5d %5d\n", temp, j1,
       e1[temp].node_id[0], e1[temp].node_id[1], e1[temp].neigh_spin[0], e1[temp].neigh_spin[1]);
       }
       */

      if (numN == 3)
      {

        te0 = burnt_loop[0];
        te1 = burnt_loop[1];
        te2 = burnt_loop[2];
        t1[ctid].v_id[0] = e1[te0].node_id[0];
        t1[ctid].v_id[1] = e1[te1].node_id[0];
        t1[ctid].v_id[2] = e1[te2].node_id[0];
        t1[ctid].nSpin[0] = e1[te0].neigh_spin[0];
        t1[ctid].nSpin[1] = e1[te0].neigh_spin[1];
        ctid++;

      }
      else if (numN > 3)
      {

        numT = numN - 2;
        cnumT = 0;
        front = 0;
        back = numN - 1;

        te0 = burnt_loop[front];
        te1 = burnt_loop[back];
        tv0 = e1[te0].node_id[0];
        tv1 = e1[te0].node_id[1];
        tv2 = e1[te1].node_id[0];
        t1[ctid].v_id[0] = tv0;
        t1[ctid].v_id[1] = tv1;
        t1[ctid].v_id[2] = tv2;
        t1[ctid].nSpin[0] = e1[te0].neigh_spin[0];
        t1[ctid].nSpin[1] = e1[te0].neigh_spin[1];
        new_node0 = tv2;
        new_node1 = tv1;

        cnumT++;
        ctid++;

        do
        {

          if ((cnumT % 2) != 0)
          {

            front = front + 1;
            ce = burnt_loop[front];
            tv0 = e1[ce].node_id[0];
            tv1 = e1[ce].node_id[1];
            tv2 = new_node0;
            t1[ctid].v_id[0] = tv0;
            t1[ctid].v_id[1] = tv1;
            t1[ctid].v_id[2] = tv2;
            t1[ctid].nSpin[0] = e1[ce].neigh_spin[0];
            t1[ctid].nSpin[1] = e1[ce].neigh_spin[1];
            new_node0 = tv1;
            cnumT++;
            ctid++;

          }
          else
          {

            back = back - 1;
            ce = burnt_loop[back];
            tv0 = e1[ce].node_id[0];
            tv1 = e1[ce].node_id[1];
            tv2 = new_node0;
            t1[ctid].v_id[0] = tv0;
            t1[ctid].v_id[1] = tv1;
            t1[ctid].v_id[2] = tv2;
            t1[ctid].nSpin[0] = e1[ce].neigh_spin[0];
            t1[ctid].nSpin[1] = e1[ce].neigh_spin[1];
            new_node0 = tv0;
            cnumT++;
            ctid++;
          }

        } while (cnumT < numT);

      }
      else
      {
        // do nothing...
      }

      free(burnt_loop);
    }

  }

  *tout = ctid;

  /*
   int lid;
   int u;
   k = 0;
   kk = 0;
   for(u=0; u<nfctr; u++){
   printf("%5d ", afc[u]);
   }
   printf("\n");
   for(k=0; k<nfedge; k++){
   cedge = burnt_list[k];
   for(kk=0; kk<nfedge; kk++){
   if(cedge==afe[kk]){
   lid = burnt[kk];
   }
   }
   printf("%5d   %3d   %5d %5d   %5d %5d\n", cedge, lid,
   e1[cedge].node_id[0], e1[cedge].node_id[1], e1[cedge].neigh_spin[0], e1[cedge].neigh_spin[1]);
   }
   printf("\n");
   */

  free(burnt);
  free(burnt_list);
  free(count);

}

void arrange_spins(struct voxel *p, struct patch *t, struct node *v, struct neighbor *n, int numT, int xDim, int nsp, int zID)
{

  int i, j, k;

  int cnode;
  int csite, kind;
  int cst_index;
  int tsite1, tsite2;
  int nspin1, nspin2;
  int tspin1, tspin2;

  double cx, cy, cz;
  double xSum, ySum, zSum;
  double xSum1, ySum1, zSum1;
  double xSum2, ySum2, zSum2;

  int nEnode;

  double ctr1[3];
  double ctr2[3];

  double nv[3];
  double tv1[3];
  double tv2[3];

  double vcoord[3][3];
  double u[3];
  double w[3];

  double a, b, c, length;
  double cs1, cs2;
  double length1, length2; // length of the vectors...
  double dotP1, dotP2;
  double theta1, theta2;

  double tn[3];

  for (i = 0; i < numT; i++)
  { // for each triangle...

    nspin1 = t[i].nSpin[0];
    nspin2 = t[i].nSpin[1];

    xSum = 0.0;
    ySum = 0.0;
    zSum = 0.0;

    xSum1 = 0.0;
    ySum1 = 0.0;
    zSum1 = 0.0;

    xSum2 = 0.0;
    ySum2 = 0.0;
    zSum2 = 0.0;

    nEnode = 0;

    //printf("%3d %3d\t", nspin1, nspin2);

    for (j = 0; j < 3; j++)
    { // for each node inside the triangle...

      cnode = t[i].v_id[j];
      cst_index = cnode / 7 + 1;
      csite = cnode / 7 + 1 + nsp * zID;
      kind = cnode % 7;

      xSum = xSum + v[cnode].coord[0];
      ySum = ySum + v[cnode].coord[1];
      zSum = zSum + v[cnode].coord[2];

      vcoord[j][0] = v[cnode].coord[0];
      vcoord[j][1] = v[cnode].coord[1];
      vcoord[j][2] = v[cnode].coord[2];

      if (kind == 0)
      {

        nEnode++;
        tsite1 = csite;
        tsite2 = n[cst_index].neigh_id[1];
        tspin1 = p[tsite1].spin;
        tspin2 = p[tsite2].spin;

      }
      else if (kind == 1)
      {

        nEnode++;
        tsite1 = csite;
        tsite2 = n[cst_index].neigh_id[7];
        tspin1 = p[tsite1].spin;
        tspin2 = p[tsite2].spin;

      }
      else if (kind == 2)
      {

        nEnode++;
        tsite1 = csite;
        tsite2 = n[cst_index].neigh_id[18];
        tspin1 = p[tsite1].spin;
        tspin2 = p[tsite2].spin;

      }
      else
      {

        tspin1 = -1;
        tspin2 = -1;
      }

      //printf("%3d %3d\t", tspin1, tspin2);

      if (tspin1 == nspin1)
      {

        xSum1 = xSum1 + p[tsite1].coord[0];
        ySum1 = ySum1 + p[tsite1].coord[1];
        zSum1 = zSum1 + p[tsite1].coord[2];

        xSum2 = xSum2 + p[tsite2].coord[0];
        ySum2 = ySum2 + p[tsite2].coord[1];
        zSum2 = zSum2 + p[tsite2].coord[2];

      }
      else if (tspin2 == nspin1)
      {

        xSum1 = xSum1 + p[tsite2].coord[0];
        ySum1 = ySum1 + p[tsite2].coord[1];
        zSum1 = zSum1 + p[tsite2].coord[2];

        xSum2 = xSum2 + p[tsite1].coord[0];
        ySum2 = ySum2 + p[tsite1].coord[1];
        zSum2 = zSum2 + p[tsite1].coord[2];

      }
      else
      {

        // do nothing...
      }

    }

    // Getting the vectors from center of triangle to each center of mass...
    cx = xSum / 3.0;
    cy = ySum / 3.0;
    cz = zSum / 3.0;

    ctr1[0] = xSum1 / (double) nEnode;
    ctr1[1] = ySum1 / (double) nEnode;
    ctr1[2] = zSum1 / (double) nEnode;

    ctr2[0] = xSum2 / (double) nEnode;
    ctr2[1] = ySum2 / (double) nEnode;
    ctr2[2] = zSum2 / (double) nEnode;

    tv1[0] = ctr1[0] - cx;
    tv1[1] = ctr1[1] - cy;
    tv1[2] = ctr1[2] - cz;

    tv2[0] = ctr2[0] - cx;
    tv2[1] = ctr2[1] - cy;
    tv2[2] = ctr2[2] - cz;

    length1 = sqrt(tv1[0] * tv1[0] + tv1[1] * tv1[1] + tv1[2] * tv1[2]);
    length2 = sqrt(tv2[0] * tv2[0] + tv2[1] * tv2[1] + tv2[2] * tv2[2]);

    // Getting normal vector of the triangle...(Right-hand Rule!!!)
    // Getting 2 edge vectors of triangle originating from vertex 0...
    u[0] = vcoord[1][0] - vcoord[0][0];
    u[1] = vcoord[1][1] - vcoord[0][1];
    u[2] = vcoord[1][2] - vcoord[0][2];

    w[0] = vcoord[2][0] - vcoord[0][0];
    w[1] = vcoord[2][1] - vcoord[0][1];
    w[2] = vcoord[2][2] - vcoord[0][2];

    // Cross product, UxW = aX + bY + cZ...
    // Note that UxW follows the same direction of triangle loop...
    a = u[1] * w[2] - u[2] * w[1];
    b = u[2] * w[0] - u[0] * w[2];
    c = u[0] * w[1] - u[1] * w[0];

    length = sqrt(a * a + b * b + c * c);
    tn[0] = a / length;
    tn[1] = b / length;
    tn[2] = c / length;

    for (k = 0; k < 3; k++)
    {
      if (fabs(tn[k]) < 0.0001)
      {
        tn[k] = 0.0;
      }
    }

    // update patch info...
    t[i].normal[0] = tn[0];
    t[i].normal[1] = tn[1];
    t[i].normal[2] = tn[2];
    t[i].area = 0.5 * length;

    // normals to triangle...
    nv[0] = a;
    nv[1] = b;
    nv[2] = c;

    // Let's arrange the spin order...
    // Getting angles between vectors...
    dotP1 = tv1[0] * nv[0] + tv1[1] * nv[1] + tv1[2] * nv[2];
    dotP2 = tv2[0] * nv[0] + tv2[1] * nv[1] + tv2[2] * nv[2];

    cs1 = dotP1 / (length * length1);
    cs2 = dotP2 / (length * length2);

    if (cs1 > 1.0)
    {
      cs1 = 1.0;
    }
    else if (cs1 < -1.0)
    {
      cs1 = -1.0;
    }

    if (cs2 > 1.0)
    {
      cs2 = 1.0;
    }
    else if (cs2 < -1.0)
    {
      cs2 = -1.0;
    }

    theta1 = 180.0 / PI * acos(cs1);
    theta2 = 180.0 / PI * acos(cs2);

    // update neighboring spins...
    if (theta1 < theta2)
    {
      t[i].nSpin[0] = nspin1;
      t[i].nSpin[1] = nspin2;
    }
    else
    {
      t[i].nSpin[0] = nspin2;
      t[i].nSpin[1] = nspin1;
    }

    //printf("%10.5f %10.5f\n", theta1, theta2);
  }

}

int get_inner_edges(struct node *v, struct segment *fe, struct isegment *ie, struct patch *t, int nfe, int nT)
{

  int i, j, ii, jj, kk, i1, j1;

  int index;
  int index1, index2;
  int n1, n2;
  int cn1, cn2;
  int cnk1, cnk2; // nodeKind of current nodes...
  int ts1, ts2;

  int bflag;

  // Collect the edges from body centers and between face centers...

  //printf("\tmarking edges from body centers and between face centers...\n");
  for (i = 0; i < nT; i++)
  { // for each triangle...

    for (j = 0; j < 3; j++)
    { // for each edge of the current triangle...

      index1 = j;
      index2 = j + 1;

      if (index1 == 2)
      {
        index2 = 0;
      }

      // Find nodes...
      n1 = t[i].v_id[index1];
      n2 = t[i].v_id[index2];

      // Find nodeKind...
      cnk1 = v[n1].nodeKind;
      cnk2 = v[n2].nodeKind;

      if ((cnk1 > 10) || (cnk2 > 10))
      { // edges connected to body centers...
        t[i].edgePlace[j] = 1;
      }
      else if (((cnk1 > 2) && (cnk1 <= 10)) && ((cnk2 > 2) && (cnk2 <= 10)))
      {
        t[i].edgePlace[j] = 1;
      }
      else
      {
        t[i].edgePlace[j] = -1;
      }

    }

  }

  // mark face edges...
  //printf("\tmarking face edges (this will take some time)...\n");
  //printf("\t\t%d\n", nfe);
  for (ii = 0; ii < nfe; ii++)
  {

    //if(ii%10000==0){
    //printf("\t\t%d\n", ii);
    //}

    cn1 = fe[ii].node_id[0];
    cn2 = fe[ii].node_id[1];

    for (jj = 0; jj < nT; jj++)
    {

      for (kk = 0; kk < 3; kk++)
      {

        bflag = t[jj].edgePlace[kk];

        if (bflag == -1)
        {

          index1 = kk;
          index2 = kk + 1;

          if (index1 == 2)
          {
            index2 = 0;
          }

          // Find nodes...
          n1 = t[jj].v_id[index1];
          n2 = t[jj].v_id[index2];

          if ((cn1 == n1 && cn2 == n2) || (cn1 == n2 && cn2 == n1))
          {
            t[jj].edgePlace[kk] = 0;
          }

        }

      }

    }
  }

  // mark -1 as inner edges too...
  index = 0;
  //printf("\tupdating inner edges including duplicates...\n");
  for (i1 = 0; i1 < nT; i1++)
  {

    ts1 = t[i1].nSpin[0];
    ts2 = t[i1].nSpin[1];

    for (j1 = 0; j1 < 3; j1++)
    {

      index1 = j1;
      index2 = j1 + 1;

      if (index1 == 2)
      {
        index2 = 0;
      }

      bflag = t[i1].edgePlace[index1];

      if (bflag != 0)
      {

        ie[index].n_id[0] = t[i1].v_id[index1];
        ie[index].n_id[1] = t[i1].v_id[index2];
        ie[index].nSpin[0] = ts1;
        ie[index].nSpin[1] = ts2;
        ie[index].nSpin[2] = 0;
        ie[index].nSpin[3] = 0;
        ie[index].edgeKind = 2;
        ie[index].burnt = -1;
        ie[index].eff = -1;
        index++;

        t[i1].edgePlace[index1] = 1;
      }

    }

  }

  return (index);

}

void find_unique_inner_edges(struct isegment *ie, int nie, int *nEff)
{

  int i, j, k;

  int cedge, nedge;
  int bflag, nbflag;

  int cn1, cn2;
  int cns1, cns2, cns;

  int nn1, nn2;
  int nns1, nns2;

  int numCN; // number of current neigh spins...
  int newCN1, newCN2; // flag for new neigh, if it's 0, add the neigh...

  int count;
  count = 0;

  //printf("\t\t%d\n", nie);

  for (i = 0; i < nie; i++)
  {

    cedge = i;
    bflag = ie[cedge].burnt;

    //if(i%10000==0){
    //printf("\t\t%d\n", i);
    //}

    if (bflag == -1)
    {

      ie[cedge].eff = 1; // If this one is a unique one, mark it.
      count++;
      ie[cedge].burnt = 1; // burn current edge...

      cn1 = ie[cedge].n_id[0]; // get current nodes...
      cn2 = ie[cedge].n_id[1];
      cns1 = ie[cedge].nSpin[0]; // get current neigh spins...
      cns2 = ie[cedge].nSpin[1];

      for (j = 0; j < nie; j++)
      {

        nedge = j;
        nbflag = ie[nedge].burnt;

        if (nbflag == -1)
        {

          nn1 = ie[nedge].n_id[0]; // get neigh nodes...
          nn2 = ie[nedge].n_id[1];
          nns1 = ie[nedge].nSpin[0]; // get neigh's neigh spins...
          nns2 = ie[nedge].nSpin[1];

          if ((cn1 == nn1 && cn2 == nn2) || (cn1 == nn2 && cn2 == nn1))
          { // if they are the same nodes...

            ie[nedge].burnt = 1; // if they are the same nodes, burn it...

            numCN = ie[cedge].edgeKind;
            newCN1 = 0;
            newCN2 = 0;

            for (k = 0; k < numCN; k++)
            { // remember nns1 is different from nns2...

              cns = ie[cedge].nSpin[k];

              if (cns == nns1)
              {
                newCN1++;
              }

              if (cns == nns2)
              {
                newCN2++;
              }
            }

            if (newCN1 == 0)
            {
              ie[cedge].nSpin[numCN] = nns1;
              numCN++;
            }

            if (newCN2 == 0)
            {
              ie[cedge].nSpin[numCN] = nns2;
              numCN++;
            }

            ie[cedge].edgeKind = numCN;
          }

        }

      }

    }

  }

  *nEff = count;

}

void copy_previous_nodes(struct node *cv, struct node *pv, int nsp)
{

  int i, j;
  int start, end;

  int pid, cid;

  int tempID;

  start = nsp + 1;
  end = 2 * nsp;

  for (i = start; i <= end; i++)
  {

    for (j = 0; j < 7; j++)
    {

      pid = (i - 1) * 7 + j;
      tempID = pv[pid].newNodeID;

      if (tempID != -1)
      {
        cid = pid - 7 * nsp;
        cv[cid].newNodeID = tempID;
      }

    }

  }

}

int assign_new_nodeID(struct node *v, int nsp, int nN)
{

  int i;
  int numN;
  int newnid;
  int nkind;
  int tid;

  numN = 7 * 2 * nsp;
  newnid = nN;

  for (i = 0; i < numN; i++)
  {

    nkind = v[i].nodeKind;
    tid = v[i].newNodeID;

    if (nkind != 0 && tid == -1)
    {
      v[i].newNodeID = newnid;
      newnid++;
    }
  }

  return (newnid);

}

void update_face_edges(struct node *v, struct segment *fe, int nfe)
{

  int i, j;
  int oldID, newID;

  for (i = 0; i < nfe; i++)
  {

    for (j = 0; j < 2; j++)
    {
      oldID = fe[i].node_id[j];
      newID = v[oldID].newNodeID;
      fe[i].new_n_id[j] = newID;
    }
  }

}

void update_inner_edges(struct node *v, struct isegment *ie, int tnie, int nie)
{

  int i, j;
  int oldID, newID;
  int count;
  int uflag;

  count = 0;

  for (i = 0; i < tnie; i++)
  {

    uflag = ie[i].eff;

    if (uflag == 1)
    {

      for (j = 0; j < 2; j++)
      {
        oldID = ie[i].n_id[j];
        newID = v[oldID].newNodeID;
        ie[i].new_n_id[j] = newID;
      }

      count++;
    }
  }

  if (count != nie)
  {
    printf("\t\tSomething's wrong in counting unique inner edges!!!\n");
  }

}

void update_current_triangles(struct node *v, struct patch *t, int nT)
{

  int i, j;
  int oldID, newID;

  for (i = 0; i < nT; i++)
  {

    for (j = 0; j < 3; j++)
    {

      oldID = t[i].v_id[j];
      newID = v[oldID].newNodeID;
      t[i].new_v_id[j] = newID;
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int get_output_nodes(struct node *v, int nsp, int zID, int zDim, char* nodesFile)
{

  struct node *tempv;
  int index;

  int i, j, k;
  int finish;
  int tID;
  int nk;
  int czid;
  double x, y, z;

  int start, end;
  int count;
  FILE *f;

  end = -1;
  count = 0;

  czid = zID;

  if (czid == (zDim - 2))
  {
    finish = 7 * 2 * nsp;
  }
  else
  {
    finish = 7 * nsp;
  }

  // counting number of nodes to be output...
  for (j = 0; j < finish; j++)
  {
    tID = v[j].newNodeID;
    if (tID != -1)
    {
      count++;
      if (tID > end)
      {
        end = tID;
      }
      else
      {
        end = end;
      }
    }
  }

  start = end - count + 1;

  tempv = (struct node *) malloc(count * sizeof(struct node));

  // rearrange the order...
  for (k = 0; k < finish; k++)
  {
    tID = v[k].newNodeID;
    nk = v[k].nodeKind;
    x = v[k].coord[0];
    y = v[k].coord[1];
    z = v[k].coord[2];
    if (tID != -1)
    {
      index = tID - start;
      tempv[index].newNodeID = tID;
      tempv[index].nodeKind = nk;
      tempv[index].coord[0] = x;
      tempv[index].coord[1] = y;
      tempv[index].coord[2] = z;
    }
  }



  if ((f = fopen(nodesFile, "a+")) == NULL)
  {
    printf("\nThe nodesFile file doesn't exist!\n");
    return 1;
  }

  for (i = 0; i < count; i++)
  {

    tID = tempv[i].newNodeID;
    nk = tempv[i].nodeKind;
    x = tempv[i].coord[0];
    y = tempv[i].coord[1];
    z = tempv[i].coord[2];

    fprintf(f, "%10d    %5d    %3d    %10.5f %10.5f %10.5f\n", tID, czid, nk, x, y, z);
  }

  fclose(f);

  free(tempv);
  return 0;
}

int get_output_edges(struct segment *fe, struct isegment *ie, int nfe,
                      int tnie, int zID, int zDim, int ceid, int *feid,
                      char* edgeFile)
{
  int i, j, ii, jj;
  int flagLimit;
  int flag1, flag2;

  int czid, maxZ;

  int n1, n2;
  int sk, ek;
  int s1, s2, s3, s4;
  int dummy;

  int index;
  int count;
  FILE *f;


  maxZ = zDim - 2;
  czid = zID;
  dummy = 0;
  index = ceid;

  if (czid < maxZ)
  {
    flagLimit = 2;
  }
  else if (czid == maxZ)
  {
    flagLimit = 3;
  }
  else
  {
    // do nothing...
  }

  count = 0;

  for (ii = 0; ii < nfe; ii++)
  {
    flag1 = fe[ii].where;
    if (flag1 < flagLimit)
    {
      count++;
    }
  }

  for (jj = 0; jj < tnie; jj++)
  {
    flag2 = ie[jj].eff;
    if (flag2 == 1)
    {
      count++;
    }
  }



  if ((f = fopen(edgeFile, "w")) == NULL)
  {
    printf("\nThe edgeFile file doesn't exist!\n");
    return 1;
  }

  fprintf(f, "%d\n", count);

  for (i = 0; i < nfe; i++)
  {

    flag1 = fe[i].where;

    if (flag1 < flagLimit)
    {
      n1 = fe[i].new_n_id[0];
      n2 = fe[i].new_n_id[1];
      sk = fe[i].segKind;
      s1 = fe[i].neigh_spin[0];
      s2 = fe[i].neigh_spin[1];
      fprintf(f, "%10d    %3d    %3d    %8d %8d    %3d    %5d %5d %5d %5d\n", index, czid, 0, n1, n2, sk, s1, s2, dummy, dummy);
      index++;
    }
  }

  for (j = 0; j < tnie; j++)
  {

    flag2 = ie[j].eff;

    if (flag2 == 1)
    {
      n1 = ie[j].new_n_id[0];
      n2 = ie[j].new_n_id[1];
      ek = ie[j].edgeKind;
      s1 = ie[j].nSpin[0];
      s2 = ie[j].nSpin[1];
      s3 = ie[j].nSpin[2];
      s4 = ie[j].nSpin[3];
      fprintf(f, "%10d    %3d    %3d    %8d %8d    %3d    %5d %5d %5d %5d\n", index, czid, 1, n1, n2, ek, s1, s2, s3, s4);
      index++;
    }
  }

  fclose(f);

  *feid = index;
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int get_output_triangles(struct patch *t, int nt, int zID, int ctid, char* trianglesFile)
{
  int i;
  int tag;
  int end;
  int newID;

  int n1, n2, n3, s1, s2;
  FILE *f;

  tag = zID;
  end = nt;
  newID = ctid;

  if ((f = fopen(trianglesFile, "w")) == NULL)
  {
    printf("\nThe trianglesFile file doesn't exist!\n");
    return 1;
  }

  fprintf(f, "%d\n", end);

  for (i = 0; i < end; i++)
  {

    n1 = t[i].new_v_id[0];
    n2 = t[i].new_v_id[1];
    n3 = t[i].new_v_id[2];
    s1 = t[i].nSpin[0];
    s2 = t[i].nSpin[1];

    fprintf(f, "%10d    %5d    %10d %10d %10d    %6d %6d\n", newID, tag, n1, n2, n3, s1, s2);

    newID++;
  }

  fclose(f);
  return 0;
}

void copy_cNodes_2_pNodes(struct node *cv, struct node *pv, int nsp)
{

  int i, j;

  int end;
  int nid;

  int tempID;
  int nk;

  end = 2 * nsp;

  for (i = 1; i <= end; i++)
  {

    for (j = 0; j < 7; j++)
    {

      nid = (i - 1) * 7 + j;
      tempID = cv[nid].newNodeID;
      nk = cv[nid].nodeKind;

      pv[nid].newNodeID = tempID;
      pv[nid].nodeKind = nk;
    }

  }

}
