/*
*****************************************************************************************
Update_Edge_Tri.c

Sukbin Lee
sukbin@andrew.cmu.edu

*****************************************************************************************
*/
#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif

#include "Update_Edge_Tri.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif
#define num_neigh 26

// constants for procedure ran3
#define MBIG 1000000000
#define MSEED 161803398
#define MZ 0
#define FAC (1.0/MBIG)


#define EDGES_RAW_POSTFIX     "_edges_raw.txt"
#define TRIANGLES_RAW_POSTFIX "_triangles_raw.txt"

// Output Files
#define EDGES_FILE         "edges.txt"
#define TRIANGLES_FILE     "triangles.txt"
#define NODES_FILE         "nodes.txt"

// Input Files
#define NODES_RAW_FILE     "nodes_raw.txt"
#define MESH_STAT_FILE     "mesh_stat.txt"

// data structures...
struct segment{
  int edgeID;
  int n_id[2];
  int edgeKind;
  int nSpin[4];
};

struct patch{
  int triID;
  int v_id[3];
  int e_id[3];
  int nSpin[2];
};



// function prototypes...
int UET_read_mesh_stat (int *nN, int *nE, int *nT, int *nMCL, const char* meshStatFile);

int UET_get_number_current_edges (int *nfe, int *nie, char* edgesFile);
int UET_get_number_previous_current_triangles (int *nPT, int *nCT, char* trianglesFile1, char* trianglesFile2, int zID);
int UET_read_current_edges (struct segment *fe, struct segment *ie, char* edgesFile);
int UET_read_current_triangles (struct patch *ct, char* trianglesFile);


void UET_update_iEdges_triangles (struct segment *ie, struct patch *ct, int nie, int nCT);
void UET_update_fEdges_triangles (struct segment *fe, struct patch *pt, struct patch *ct,
			      int nfe, int nPT, int nCT, int zID);
void UET_update_edge_neigh_spins (struct segment *e, int eid, int tsp[2]);


void UET_copy_triangles (struct patch *ct, struct patch *pt, int nPT);


int UET_get_output_edges (struct segment *fe, struct segment *ie, int nfe, int nie, const char* edgesFile);
int UET_get_output_triangles (struct patch *t, int nT, char* trianglesFile);
int UET_get_output_nodes (int nN, const char* nodesFile, const char* nodesRawFile);


int Update_Edge_Tri(const char* meshStatFile,
                    const char* outputDir,
                    const char* edgesFile,
                    const char* trianglesFile,
                    const char* nodesFile,
                    const char* nodesRawFile)
{
  int i;

  FILE *f1, *f2, *f3;
  int err;
  int nMCLayer;
  int numN;
  int numE, numFE, numIE;
  int numT, numPT, numCT;
  int end;
  char trianglesFile1[BUFSIZ];
  char trianglesFile2[BUFSIZ];
  char edgeFile[BUFSIZ];
  char genericFilePath[BUFSIZ];
  char genericFilePath2[BUFSIZ];
  char postfix[64] = TRIANGLES_RAW_POSTFIX;
  char edgepostfix[64] = EDGES_RAW_POSTFIX;

  struct segment *fEdge;
  struct segment *iEdge;
  struct patch *pTriangle;
  struct patch *cTriangle;

  err = 0;

  memset(genericFilePath, 0, BUFSIZ); // Clear the string first
  sprintf(genericFilePath, "%s%s", outputDir,  meshStatFile);
  err = UET_read_mesh_stat (&numN, &numE, &numT, &nMCLayer, genericFilePath);
  printf("\nThe total number of nodes, edges and triangles: %10d %10d %10d\n", numN, numE, numT);
  end = nMCLayer;  // triangles are tagged from 0 to znum-2 for locations to which they belong...

  pTriangle = (struct patch *)malloc(1*sizeof(struct patch));


  // Creating new or Erasing Current output files

  memset(genericFilePath, 0, BUFSIZ); // Clear the string first
  sprintf(genericFilePath, "%s%s", outputDir,  edgesFile);
  if ((f1 = fopen(genericFilePath, "w")) == NULL)
  {
    printf("\nCannot create output file: '%s'\n", genericFilePath);
    return 1;
  }
  fprintf(f1, "%d\n", numE);
  fclose(f1);

  memset(genericFilePath, 0, BUFSIZ); // Clear the string first
  sprintf(genericFilePath, "%s%s", outputDir,  trianglesFile);
  if ((f2 = fopen(genericFilePath, "w")) == NULL)
  {
    printf("\nCannot create output file: '%s'\n", genericFilePath);
    return 1;
  }
  fprintf(f2, "%d\n", numT);
  fclose(f2);

  memset(genericFilePath, 0, BUFSIZ); // Clear the string first
  sprintf(genericFilePath, "%s%s", outputDir,  nodesFile);
  if ((f3 = fopen(genericFilePath, "w")) == NULL)
  {
    printf("\nCannot create output file: '%s'\n", genericFilePath);
    return 1;
  }
  fprintf(f3, "%d\n", numN);
  fclose(f3);

  // For each layer...
  for (i = 0; i <= end; i++)
  {
    printf("\nworking on layer %4d of %4d layers\n", i, end);

    // get numbers of face edges, inner edges, previous triangles and current triangles...
    printf("\tgetting number of edges and triangles...\n");

    memset(edgeFile, 0, BUFSIZ); // Clear the string first
    sprintf(edgeFile, "%s%d%s", outputDir,  i, edgepostfix);
    err = UET_get_number_current_edges (&numFE, &numIE, edgeFile);

    memset(trianglesFile1, 0, BUFSIZ);
    memset(trianglesFile2, 0, BUFSIZ);
    sprintf(trianglesFile1, "%s%d%s", outputDir, i-1, postfix);
    sprintf(trianglesFile2, "%s%d%s", outputDir, i, postfix);
    err = UET_get_number_previous_current_triangles (&numPT, &numCT, trianglesFile1, trianglesFile2, i);
    printf("\tnumber of face edges and inner edges:     %10d %10d\n", numFE, numIE);
    printf("\tnumber of previous and current triangles: %10d %10d\n", numPT, numCT);

    fEdge = (struct segment *)malloc(numFE*sizeof(struct segment));
    iEdge = (struct segment *)malloc(numIE*sizeof(struct segment));
    //pTriangle = (struct patch *)malloc(numPT*sizeof(struct patch));
    cTriangle = (struct patch *)malloc(numCT*sizeof(struct patch));

    // read edge and triangle information...
    printf("\treading edges and triangles...\n");
    memset(edgeFile, 0, BUFSIZ); // Clear the string first
    sprintf(edgeFile, "%s%d%s", outputDir,  i, edgepostfix);
    err = UET_read_current_edges (fEdge, iEdge, edgeFile);

    memset(trianglesFile1, 0, BUFSIZ); // Clear the string first
    sprintf(trianglesFile1, "%s%d%s", outputDir,  i, postfix);
    err = UET_read_current_triangles (cTriangle, trianglesFile1);

    // update edges and triangles...
    printf("\tupdating edges...\n");
    UET_update_iEdges_triangles (iEdge, cTriangle, numIE, numCT);
    printf("\tupdating triangles...\n");
    UET_update_fEdges_triangles (fEdge, pTriangle, cTriangle, numFE, numPT, numCT, i);

    // print out edge information...
    printf("\tgetting outputs...\n");
    err = UET_get_output_edges (fEdge, iEdge, numFE, numIE, edgesFile);

    // print out triangle information...
    if (i > 0)
    {
      if (i == end)
      {
        memset(genericFilePath, 0, BUFSIZ);
        sprintf(genericFilePath, "%s%s", outputDir, trianglesFile);
        err = UET_get_output_triangles(pTriangle, numPT, genericFilePath);
        err = UET_get_output_triangles(cTriangle, numCT, genericFilePath);
      }
      else
      {
        err = UET_get_output_triangles(pTriangle, numPT, genericFilePath);
      }
    }

    // make cTriangle into pTriangle...
    free(pTriangle);
    numPT = numCT;
    pTriangle = (struct patch *)malloc(numPT*sizeof(struct patch));
    UET_copy_triangles (cTriangle, pTriangle, numPT);

    free(fEdge);
    free(iEdge);
    free(cTriangle);
//TODO: This is a kludge at this point. We just check for any error. If the error happens way
    // up in the code who knows what will really happen at this point.
    if (err == 1)
    {
      printf("Error Updating Nodes and Triangles File\n");
      return err;
    }
  }

  printf("\nupdating nodes...\n");


  memset(genericFilePath, 0, BUFSIZ);
  sprintf(genericFilePath, "%s%s", outputDir, nodesFile);
  memset(genericFilePath2, 0, BUFSIZ);
  sprintf(genericFilePath2, "%s%s", outputDir, nodesRawFile);
  err = UET_get_output_nodes (numN, genericFilePath, genericFilePath2);
  if (err == 1)
  {
    printf("Error Updating Nodes and Triangles File\n");
    return err;
  }


  return err;
}



#ifndef UPDATE_EDGE_TRI_LIBRARY
// Main function starts...
int main(int argc, char **argv)
{

  char edgesFile[64] = EDGES_FILE;
  char trianglesFile[64] = TRIANGLES_FILE;
  char nodesFile[64] = NODES_FILE;
  char outputDir[2] = "./";
  char meshStatFile[64] = MESH_STAT_FILE;
  char nodesRawFile[64] = NODES_RAW_FILE;
  int err;

  err =  Update_Edge_Tri(meshStatFile, outputDir, edgesFile, trianglesFile, nodesFile, nodesRawFile);

  return err;
}
#endif


// Functions...
int UET_read_mesh_stat (int *nN, int *nE, int *nT, int *nMCL, const char* meshStatFile)
{

  FILE *f;

  int tnN, tnE, tnT, tnmcl;

  if ((f = fopen(meshStatFile, "r")) == NULL)
  {
    printf("\nThe input file doesn't exist: '%s'\n", meshStatFile);
    return 1;
  }

  fscanf(f, "%d %d %d %d", &tnN, &tnE, &tnT, &tnmcl);

  *nN = tnN;
  *nE = tnE;
  *nT = tnT;
  *nMCL = tnmcl;

  fclose(f);
  return 0;
}



int UET_get_number_current_edges (int *nfe, int *nie, char* buf)
{
  int i;

  FILE *f;

  int eid;
  int ek;
  int feFlag, ieFlag;
  int tnfe, tnie;
  int layerIndex, kindIndex;
  int n1, n2;
  int s1, s2, s3, s4;

  int tne;

  feFlag = 0;
  ieFlag = 1;

  tnfe = 0;
  tnie = 0;

  if( (f=fopen(buf, "r")) == NULL){
    printf("\nThe input file doesn't exist: '%s'\n", buf);
    return 1;
  }

  fscanf(f, "%d", &tne);

  for(i=0; i<tne; i++){

    fscanf(f, "%d %d %d %d %d %d %d %d %d %d",
	   &eid, &layerIndex, &kindIndex, &n1, &n2, &ek, &s1, &s2, &s3, &s4);

    if(kindIndex==feFlag){
      tnfe++;
    }else if(kindIndex==ieFlag){
      tnie++;
    }else{
      // do nothing...
    }

  }

  fclose(f);

  *nfe = tnfe;
  *nie = tnie;

return 0;
}



int UET_read_current_edges (struct segment *fe, struct segment *ie, char* buf)
{

  int i;

  FILE *f;

  int feid;
  int ieid;

  int eid;
  int ek;
  int feFlag, ieFlag;
  int layerIndex, kindIndex;
  int n1, n2;
  int s1, s2, s3, s4;

  int tne;

  feFlag = 0;
  ieFlag = 1;

  feid = 0;
  ieid = 0;

  if( (f=fopen(buf, "r")) == NULL){
    printf("\nThe input file doesn't exist: '%s'\n", buf);
    return 1;
  }

  fscanf(f, "%d", &tne);

  for(i=0; i<tne; i++){

    fscanf(f, "%d %d %d %d %d %d %d %d %d %d",
	   &eid, &layerIndex, &kindIndex, &n1, &n2, &ek, &s1, &s2, &s3, &s4);

    if(kindIndex==feFlag){
      fe[feid].edgeID = eid;
      fe[feid].n_id[0] = n1;
      fe[feid].n_id[1] = n2;
      fe[feid].edgeKind = ek;
      fe[feid].nSpin[0] = s1;
      fe[feid].nSpin[1] = s2;
      fe[feid].nSpin[2] = s3;
      fe[feid].nSpin[3] = s4;
      feid++;
    }else if(kindIndex==ieFlag){
      ie[ieid].edgeID = eid;
      ie[ieid].n_id[0] = n1;
      ie[ieid].n_id[1] = n2;
      ie[ieid].edgeKind = ek;
      ie[ieid].nSpin[0] = s1;
      ie[ieid].nSpin[1] = s2;
      ie[ieid].nSpin[2] = s3;
      ie[ieid].nSpin[3] = s4;
      ieid++;
    }else{
      // do nothing...
    }

  }

  fclose(f);
  return 0;
}





int UET_get_number_previous_current_triangles(int *nPT, int *nCT, char* buf1, char* buf2, int zID)
{

  FILE *f1, *f2;

  int tnpt, tnct;

  if ((f2 = fopen(buf2, "r")) == NULL)
  {
    printf("\nThe input file doesn't exist: '%s'\n", buf2);
    return 1;
  }

  fscanf(f2, "%d", &tnct);

  fclose(f2);

  if (zID > 0)
  {
    if ((f1 = fopen(buf1, "r")) == NULL)
    {
      printf("\nThe input file doesn't exist: '%s'\n", buf1);
      return 1;
    }

    fscanf(f1, "%d", &tnpt);

    fclose(f1);

  }
  else
  {
    tnpt = 0;
  }

  *nPT = tnpt;
  *nCT = tnct;
  return 0;
}



int UET_read_current_triangles (struct patch *ct, char* buf)
{

  int i;

  FILE *f;

  //int cFlag;
  int tid;
  int index;
  int n1, n2, n3, s1, s2;
  int cid;

  int tnt;

 // cFlag = zID;
  cid = 0;

  if( (f=fopen(buf, "r")) == NULL){
    printf("\nThe input file doesn't exist: '%s'\n", buf);
    return 1;
  }

  fscanf(f, "%d", &tnt);

  for(i=0; i<tnt; i++){

    fscanf(f, "%d %d %d %d %d %d %d",
	   &tid, &index, &n1, &n2, &n3, &s1, &s2);

    ct[cid].triID = tid;
    ct[cid].v_id[0] = n1;
    ct[cid].v_id[1] = n2;
    ct[cid].v_id[2] = n3;
    ct[cid].e_id[0] = -1;
    ct[cid].e_id[1] = -1;
    ct[cid].e_id[2] = -1;
    ct[cid].nSpin[0] = s1;
    ct[cid].nSpin[1] = s2;
    cid++;
  }

  fclose(f);
  return 0;
}



void UET_update_iEdges_triangles (struct segment *ie, struct patch *ct, int nie, int nCT){

  int i, j, k;

  int cn1, cn2, nn1, nn2;
  int teid;
  int tSpin[2];

  int index1, index2;
  int bflag;

  for(i=0; i<nie; i++){

    cn1 = ie[i].n_id[0];
    cn2 = ie[i].n_id[1];
    teid = ie[i].edgeID;

    for(j=0; j<nCT; j++){

      for(k=0; k<3; k++){  // for each edge of the current triangle...

	bflag = ct[j].e_id[k];

	if(bflag==-1){

	  index1 = k;

	  if(index1==2){
	    index2 = 0;
	  }else{
	    index2 = k + 1;
	  }

	  // Find nodes...
	  nn1 = ct[j].v_id[index1];
	  nn2 = ct[j].v_id[index2];

	  if( (cn1==nn1 && cn2==nn2) || (cn1==nn2 && cn2==nn1)){
	    ct[j].e_id[k] = teid;
	    tSpin[0] = ct[j].nSpin[0];
	    tSpin[1] = ct[j].nSpin[1];
	    UET_update_edge_neigh_spins (ie, i, tSpin);
	  }
	}
      }
    }
  }

}



void UET_update_fEdges_triangles (struct segment *fe, struct patch *pt, struct patch *ct,
			      int nfe, int nPT, int nCT, int zID){

  int i, j, k, ii, jj, kk;

  int cn1, cn2, nn1, nn2;
  int teid;
  int tSpin[2];

  int index1, index2;
  int bflag;

  int zFlag;
  zFlag = zID;

  for(i=0; i<nfe; i++){

    cn1 = fe[i].n_id[0];
    cn2 = fe[i].n_id[1];
    teid = fe[i].edgeID;

    for(j=0; j<nCT; j++){

      for(k=0; k<3; k++){  // for each edge of the current triangle...

	bflag = ct[j].e_id[k];

	if(bflag==-1){

	  index1 = k;

	  if(index1==2){
	    index2 = 0;
	  }else{
	    index2 = k + 1;
	  }

	  // Find nodes...
	  nn1 = ct[j].v_id[index1];
	  nn2 = ct[j].v_id[index2];

	  if( (cn1==nn1 && cn2==nn2) || (cn1==nn2 && cn2==nn1)){
	    ct[j].e_id[k] = teid;
	    tSpin[0] = ct[j].nSpin[0];
	    tSpin[1] = ct[j].nSpin[1];
	    UET_update_edge_neigh_spins (fe, i, tSpin);
	  }

	}

      }
    }

  }


  if(zFlag>0){

    for(ii=0; ii<nfe; ii++){

      cn1 = fe[ii].n_id[0];
      cn2 = fe[ii].n_id[1];
      teid = fe[ii].edgeID;

      for(jj=0; jj<nPT; jj++){

	for(kk=0; kk<3; kk++){  // for each edge of the current triangle...

	  bflag = pt[jj].e_id[kk];

	  if(bflag==-1){

	    index1 = kk;

	    if(index1==2){
	      index2 = 0;
	    }else{
	      index2 = kk + 1;
	    }

	    // Find nodes...
	    nn1 = pt[jj].v_id[index1];
	    nn2 = pt[jj].v_id[index2];

	    if( (cn1==nn1 && cn2==nn2) || (cn1==nn2 && cn2==nn1)){
	      pt[jj].e_id[kk] = teid;
	      tSpin[0] = pt[jj].nSpin[0];
	      tSpin[1] = pt[jj].nSpin[1];
	      UET_update_edge_neigh_spins (fe, ii, tSpin);
	    }

	  }

	}
      }
    }

  }

}



void UET_update_edge_neigh_spins (struct segment *e, int eid, int tsp[2]){

  int i;
  int index;
  int numCN, newCN1, newCN2;

  int cns;
  int nns1, nns2;

  index = eid;
  nns1 = tsp[0];
  nns2 = tsp[1];
  numCN = e[index].edgeKind;
  newCN1 = 0;
  newCN2 = 0;

  for(i=0; i<numCN; i++){   // remember nns1 is different from nns2...

    cns = e[index].nSpin[i];

    if(cns==nns1){
      newCN1++;
    }

    if(cns==nns2){
      newCN2++;
    }
  }

  if(newCN1==0){
    e[index].nSpin[numCN] = nns1;
    numCN++;
  }

  if(newCN2==0){
    e[index].nSpin[numCN] = nns2;
    numCN++;
  }

  e[index].edgeKind = numCN;

}



void UET_copy_triangles (struct patch *ct, struct patch *pt, int nPT)
{

  int i;

  for(i=0; i<nPT; i++){

    pt[i].triID = ct[i].triID;
    pt[i].v_id[0] = ct[i].v_id[0];
    pt[i].v_id[1] = ct[i].v_id[1];
    pt[i].v_id[2] = ct[i].v_id[2];
    pt[i].e_id[0] = ct[i].e_id[0];
    pt[i].e_id[1] = ct[i].e_id[1];
    pt[i].e_id[2] = ct[i].e_id[2];
    pt[i].nSpin[0] = ct[i].nSpin[0];
    pt[i].nSpin[1] = ct[i].nSpin[1];
  }

}



int UET_get_output_edges (struct segment *fe, struct segment *ie, int nfe, int nie, const char* edgesFile)
{

  int i, j;

  FILE *f;

  if( (f=fopen(edgesFile, "a+")) == NULL){
    printf("\nCannot create output file '%s'\n", edgesFile);
    return 1;
  }

  for(i=0; i<nfe; i++){
    fprintf(f, "%10d   %8d %8d   %3d   %5d %5d %5d %5d\n",
	    fe[i].edgeID, fe[i].n_id[0], fe[i].n_id[1], fe[i].edgeKind,
	    fe[i].nSpin[0], fe[i].nSpin[1], fe[i].nSpin[2], fe[i].nSpin[3]);
  }

  for(j=0; j<nie; j++){
    fprintf(f, "%10d   %8d %8d   %3d   %5d %5d %5d %5d\n",
	    ie[j].edgeID, ie[j].n_id[0], ie[j].n_id[1], ie[j].edgeKind,
	    ie[j].nSpin[0], ie[j].nSpin[1], ie[j].nSpin[2], ie[j].nSpin[3]);
  }

  fclose(f);
  return 0;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int UET_get_output_triangles (struct patch *t, int nT, char* trianglesFile)
{

  int i;

  FILE *f;

  if( (f=fopen(trianglesFile, "a+")) == NULL){
    printf("\nCannot create output file '%s'\n", trianglesFile);
    return 1;
  }

  for(i=0; i<nT; i++){

    fprintf(f, "%10d     %8d %8d %8d     %8d %8d %8d     %5d %5d \n",
	    t[i].triID, t[i].v_id[0], t[i].v_id[1], t[i].v_id[2],
	    t[i].e_id[0], t[i].e_id[1], t[i].e_id[2], t[i].nSpin[0], t[i].nSpin[1]);
  }

  fclose(f);
  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int UET_get_output_nodes (int nN, const char* nodesFile, const char* nodesRawFile)
{

  int i;

  int id, dummy, nk;
  double x, y, z;

  FILE *f, *f1;

  if( (f=fopen(nodesRawFile, "r")) == NULL){
    printf("\nCannot create output file '%s'\n", nodesRawFile);
    return 1;
  }

  if( (f1=fopen(nodesFile, "a+")) == NULL){
    printf("\nCannot create output file '%s'\n", nodesFile);
    return 1;
  }

  for(i=0; i<nN; i++){

    fscanf(f, "%d %d %d %lf %lf %lf", &id, &dummy, &nk, &x, &y, &z);
    fprintf(f1, "%10d    %3d    %12.6f %12.6f %12.6f\n", id, nk, x, y, z);
  }

  fclose(f);
  fclose(f1);
  return 0;
}
