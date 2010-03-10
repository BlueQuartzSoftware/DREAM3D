////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD LiceNSe: http://www.opeNSource.org/liceNSes/bsd-liceNSe.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////

#include "SurfaceMeshFunc.h"

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <vector>


const static double m_pi = 3.1415926535897;
const static double m_OnePointThree = 1.33333333333;

#include <sstream>

#include <MXA/Utilities/StringUtils.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

using namespace std;

SurfaceMeshFunc::SurfaceMeshFunc() :
  neigh(NULL),
  point(NULL),
  cSquare(NULL),
  cVertex(NULL),
  pVertex(NULL)
{

}

SurfaceMeshFunc::~SurfaceMeshFunc()
{
  delete []neigh;
  delete []point;
  delete []cSquare;
  delete []cVertex;
  delete []pVertex;


}

void SurfaceMeshFunc::initialize(int xnum,int ynum, int znum, double xres, double yres, double zres)
{

  NS = xnum * ynum * znum;
  NSP = xnum * ynum;

  xDim = xnum;
  yDim = ynum;
  zDim = znum;
  xRes = xres;
  yRes = yres;
  zRes = zres;

  neigh = new Neighbor[2 * NSP + 1];
  point = new Voxel[NS + 1];
  cSquare = new Face[3 * 2 * NSP];
  cVertex = new Node[7 * 2 * NSP];
  pVertex = new Node[7 * 2 * NSP];


}

void SurfaceMeshFunc::initialize_micro(string filename)
{

  int i, j, k, l;
  int id;
  int tgrainname;
  string dummy;
  double tempx, tempy, tempz;
  ifstream inputFile;
  inputFile.open(filename.c_str());
  // Let's get rid of the 9 header lines in the xxx.dx produced by 3d coarsening code...
  for (k = 0; k < 9; k++)
  {
	getline(inputFile,dummy,'\n');
  }

  point[0].grainname = 0; // Point 0 is a garbage...

  // Let's input the grainname numbers for each site from micro.input...
  for (i = 1; i <= NS; i++)
  {
	int col = (i-1)%xDim;
	int row = ((i-1)/xDim)%yDim;
	int plane = (i-1)/(xDim*yDim);
    inputFile >> tgrainname;
    point[i].grainname = tgrainname;
	if(col == 0 || col == (xDim-1) || row == 0 || row == (yDim-1) || plane == 0 || plane == (zDim-1)) point[i].grainname = -3;
  }

  inputFile.close();

  // Let's fill out the coordinate of each voxel. Remember that x coordinate goes fastest...
  for (l = 0; l <= NS - xDim * yDim; l = l + xDim * yDim)
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


        if (tempx > (double) (xDim - 2) || tempy > (double) (yDim - 2) || tempz > (double) (zDim - 2))
        {
          point[id].surfacevoxel = 1;
        }
        else
        {
          point[id].surfacevoxel = 0;
        }

      }
    }
  }
}

void SurfaceMeshFunc::get_neighbor_list(int zID)
{
  // NSP = number of sites in a plane of xDim by yDim...
  // neigh[][] = 2 dimeNSional array storing its site number and neighbors...
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

  for (ii = 1; ii <= 2 * NSP; ii++)
  {

    site_id = zID * NSP + ii;
    neigh[ii].csiteid = site_id;

    k = (site_id - 1) / NSP;
    k = k * NSP;

    r = (site_id - 1) % NSP;
    j = r / xDim;
    j = j * xDim;

    i = r % xDim + 1;

    //same plane...
    neigh[ii].neigh_id[1] = k + j + i % xDim + 1;
    neigh[ii].neigh_id[2] = k + (j - xDim + NSP) % NSP + i % xDim + 1;
    neigh[ii].neigh_id[3] = k + (j - xDim + NSP) % NSP + i;
    neigh[ii].neigh_id[4] = k + (j - xDim + NSP) % NSP + (i - 2 + xDim) % xDim + 1;

    neigh[ii].neigh_id[5] = k + j + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[6] = k + (j + xDim) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[7] = k + (j + xDim) % NSP + i;
    neigh[ii].neigh_id[8] = k + (j + xDim) % NSP + i % xDim + 1;

    //upper plane...
    //move the plane up and use the same scheme...
    neigh[ii].neigh_id[9] = (k - NSP + NS) % NS + j + i;
    neigh[ii].neigh_id[10] = (k - NSP + NS) % NS + j + i % xDim + 1;
    neigh[ii].neigh_id[11] = (k - NSP + NS) % NS + (j - xDim + NSP) % NSP + i % xDim + 1;

    neigh[ii].neigh_id[12] = (k - NSP + NS) % NS + (j - xDim + NSP) % NSP + i;
    neigh[ii].neigh_id[13] = (k - NSP + NS) % NS + (j - xDim + NSP) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[14] = (k - NSP + NS) % NS + j + (i - 2 + xDim) % xDim + 1;

    neigh[ii].neigh_id[15] = (k - NSP + NS) % NS + (j + xDim) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[16] = (k - NSP + NS) % NS + (j + xDim) % NSP + i;
    neigh[ii].neigh_id[17] = (k - NSP + NS) % NS + (j + xDim) % NSP + i % xDim + 1;

    //lower plane...
    neigh[ii].neigh_id[18] = (k + NSP) % NS + j + i;
    neigh[ii].neigh_id[19] = (k + NSP) % NS + j + i % xDim + 1;
    neigh[ii].neigh_id[20] = (k + NSP) % NS + (j - xDim + NSP) % NSP + i % xDim + 1;

    neigh[ii].neigh_id[21] = (k + NSP) % NS + (j - xDim + NSP) % NSP + i;
    neigh[ii].neigh_id[22] = (k + NSP) % NS + (j - xDim + NSP) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[23] = (k + NSP) % NS + j + (i - 2 + xDim) % xDim + 1;

    neigh[ii].neigh_id[24] = (k + NSP) % NS + (j + xDim) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[25] = (k + NSP) % NS + (j + xDim) % NSP + i;
    neigh[ii].neigh_id[26] = (k + NSP) % NS + (j + xDim) % NSP + i % xDim + 1;

  }
}

void SurfaceMeshFunc::initialize_nodes(int zID)
{

  // Finds the coordinates of nodes...

  int i;
  int id;

  int tsite;
  double x, y, z;

  // node id starts with 0....
  for (i = 1; i <= 2 * NSP; i++)
  {

    id = 7 * (i - 1);
    tsite = zID * NSP + i;

    x = find_xcoord(tsite);
    y = find_ycoord(tsite);
    z = find_zcoord(tsite);

    cVertex[id].xc = x + (0.5*xRes);
    cVertex[id].yc = y;
    cVertex[id].zc = z;
    cVertex[id].nodeKind = 0;
    cVertex[id].newNodeID = -1;

    cVertex[id + 1].xc = x;
    cVertex[id + 1].yc = y + (0.5*yRes);
    cVertex[id + 1].zc = z;
    cVertex[id + 1].nodeKind = 0;
    cVertex[id + 1].newNodeID = -1;

    cVertex[id + 2].xc = x;
    cVertex[id + 2].yc = y;
    cVertex[id + 2].zc = z + (0.5*zRes);
    cVertex[id + 2].nodeKind = 0;
    cVertex[id + 2].newNodeID = -1;

   cVertex [id + 3].xc = x + (0.5*xRes);
   cVertex [id + 3].yc = y + (0.5*yRes);
   cVertex [id + 3].zc = z;
   cVertex [id + 3].nodeKind = 0;
   cVertex [id + 3].newNodeID = -1;

   cVertex [id + 4].xc = x + (0.5*xRes);
   cVertex [id + 4].yc = y;
   cVertex [id + 4].zc = z + (0.5*zRes);
   cVertex [id + 4].nodeKind = 0;
   cVertex [id + 4].newNodeID = -1;

   cVertex [id + 5].xc = x;
   cVertex [id + 5].yc = y + (0.5*yRes);
   cVertex [id + 5].zc = z + (0.5*zRes);
   cVertex [id + 5].nodeKind = 0;
   cVertex [id + 5].newNodeID = -1;

    cVertex[id + 6].xc = x + (0.5*xRes);
    cVertex[id + 6].yc = y + (0.5*yRes);
    cVertex[id + 6].zc = z + (0.5*zRes);
    cVertex[id + 6].nodeKind = 0;
    cVertex[id + 6].newNodeID = -1;

  }

}

double SurfaceMeshFunc::find_xcoord(long index)
{
	index = index-1;
	double x = xRes*double(index%xDim);
	return x;
}
double SurfaceMeshFunc::find_ycoord(long index)
{
	index = index-1;
	double y = yRes*double((index/xDim)%yDim);
	return y;
}
double SurfaceMeshFunc::find_zcoord(long index)
{
	index = index-1;
	double z = zRes*double(index/(xDim*yDim));
	return z;
}
void SurfaceMeshFunc::initialize_squares(int zID)
{

  // Gather initial information on each square...
  int id;
 // int sqid;
  int i, j;
  int csite;

  // square id starts with 0....
  // notice that voxels at the surface will have the wrong values of node at the other end...
  // since it includes periodic boundary condition...
  // but, since the structure surrounded by ghost layer of grainname -3, it's OK...
  for (i = 1; i <= 2 * NSP; i++)
  {

    id = 3 * (i - 1);
    csite = zID * NSP + i;

    // store 4 corners of square...
    cSquare[id].site_id[0] = csite;
    cSquare[id].site_id[1] = neigh[i].neigh_id[1];
    cSquare[id].site_id[2] = neigh[i].neigh_id[8];
    cSquare[id].site_id[3] = neigh[i].neigh_id[7];

    cSquare[id + 1].site_id[0] = csite;
    cSquare[id + 1].site_id[1] = neigh[i].neigh_id[1];
    cSquare[id + 1].site_id[2] = neigh[i].neigh_id[19];
    cSquare[id + 1].site_id[3] = neigh[i].neigh_id[18];

    cSquare[id + 2].site_id[0] = neigh[i].neigh_id[7];
    cSquare[id + 2].site_id[1] = csite;
    cSquare[id + 2].site_id[2] = neigh[i].neigh_id[18];
    cSquare[id + 2].site_id[3] = neigh[i].neigh_id[25];

    // initialize node, edge...-1 is dummy initial value...
    for (j = 0; j < 4; j++)
    {
      cSquare[id].edge_id[j] = -1;
      cSquare[id + 1].edge_id[j] = -1;
      cSquare[id + 2].edge_id[j] = -1;
    }

    cSquare[id].nEdge = 0;
    cSquare[id].turnFC = 0;
    cSquare[id].FCnode = -1;
    cSquare[id].effect = 0;

    cSquare[id + 1].nEdge = 0;
    cSquare[id + 1].turnFC = 0;
    cSquare[id + 1].FCnode = -1;
    cSquare[id + 1].effect = 0;

    cSquare[id + 2].nEdge = 0;
    cSquare[id + 2].turnFC = 0;
    cSquare[id + 2].FCnode = -1;
    cSquare[id + 2].effect = 0;
  }

}

int SurfaceMeshFunc::get_number_fEdges(int zID)
{
  int  k, m;
  int csite, cgrainname, tsite;
  int tNSite[4];
  int tngrainname[4];
  int atBulk; // if it's 0, the square is at Bulk...
  int sqIndex;
  int anFlag;
  int quot, rmd, rmd1;
  int sumEdge;
  int numCEdge;
  sumEdge = 0;
  for (k = 0; k < (3 * 2 * NSP); k++)
  { // for each square on upper layer...
    quot = k / (3 * NSP);
    rmd = k % (3 * NSP);
    rmd1 = rmd % 3;
    if (quot == 0 || (quot == 1 && rmd1 == 0))
    {
      csite = cSquare[k].site_id[0];
      cgrainname = point[csite].grainname;
      tNSite[0] = cSquare[k].site_id[0];
      tNSite[1] = cSquare[k].site_id[1];
      tNSite[2] = cSquare[k].site_id[2];
      tNSite[3] = cSquare[k].site_id[3];
      atBulk = 0;
      for (m = 0; m < 4; m++)
      {
        tsite = tNSite[m];
        tngrainname[m] = point[tsite].grainname;
        if (tngrainname[m] > 0)
        {
          atBulk++;
        }
      }
      if (atBulk > 0)
      {
        cSquare[k].effect = 1; // mark current square as effective one that can be "marching cubed"...
      }
      // Let's count the number of edges...
//      if (cgrainname > -10 && atBulk == 0)
      if (atBulk > 0)
      { // coNSider the square iNSide the bulk only..
        sqIndex = get_square_index(tngrainname);
        if (sqIndex == 15)
        {
          anFlag = treat_anomaly(tNSite, zID);
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
        sumEdge = sumEdge + numCEdge;
      }
    }
  }
  return sumEdge;
}

void SurfaceMeshFunc::get_nodes_fEdges(int eT2d[20][8], int NST2d[20][8], int zID, int nFEdge)
{
  int  j, k, m, ii;
  int csite, cgrainname, tsite;
  int cubeOrigin; // stores the site id of 0th corner of each cube...
  int sqOrder; // stores the 0th, 1st and 2nd square...
  int tNSite[4];
  int tngrainname[4];
  int atBulk; // if it's 0, the square is at Bulk...
  int quot, rmd, rmd1;
  int sqIndex;
  int anFlag;
  int nodeIndex[2];
  int tnode;
  int pixIndex[2];
  int eid; // edge id...
  int tnode1, tnode2;
  int edgeCount; // number of edges for each square...
  int nodeID[2];
  int pixgrainname[2];
  eid = 0;
  cFedge = new Segment[nFEdge];
  for (k = 0; k < 3 * 2 * NSP; k++)
  { // for each square...
    quot = k / (3 * NSP);
    rmd = k % (3 * NSP);
    rmd1 = rmd % 3;
    if (quot == 0 || (quot == 1 && rmd1 == 0))
    {
      cubeOrigin = k / 3 + 1;
      sqOrder = k % 3;
      csite = cSquare[k].site_id[0];
      cgrainname = point[csite].grainname;
      tNSite[0] = cSquare[k].site_id[0];
      tNSite[1] = cSquare[k].site_id[1];
      tNSite[2] = cSquare[k].site_id[2];
      tNSite[3] = cSquare[k].site_id[3];
      atBulk = 0;
      for (m = 0; m < 4; m++)
      {
        tsite = tNSite[m];
        tngrainname[m] = point[tsite].grainname;
        if (tngrainname[m] > 0)
        {
          atBulk++;
        }
      }
      edgeCount = 0;
      // Let's find the edges...
//      if (cgrainname > -10 && atBulk == 0)
      if (atBulk > 0)
      { // coNSider the square iNSide the bulk only...
        sqIndex = get_square_index(tngrainname);
        if (sqIndex == 15)
        {
          anFlag = treat_anomaly(tNSite, zID);
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
              pixIndex[0] = NST2d[sqIndex][j];
              pixIndex[1] = NST2d[sqIndex][j + 1];
              get_nodes(cubeOrigin, sqOrder, nodeIndex, nodeID);
              tnode1 = nodeID[0];
              tnode2 = nodeID[1];
			  get_grainnames(tngrainname,pixIndex,pixgrainname);
			  if(pixgrainname[0] > 0 || pixgrainname[1] >0)
			  {
				  // Categorize the node...if it's triple junction or not...
				  for (ii = 0; ii < 2; ii++)
				  {
					if (nodeIndex[ii] == 4)
					{
					  if (sqIndex == 7 || sqIndex == 11 || sqIndex == 13 || sqIndex == 14)
					  {
						tnode = nodeID[ii];
						cVertex[tnode].nodeKind = 3;
						cSquare[k].turnFC = 1;
						cSquare[k].FCnode = tnode;
					  }
					  else if (sqIndex == 19)
					  {
						tnode = nodeID[ii];
						cVertex[tnode].nodeKind = 4;
						cSquare[k].turnFC = 1;
						cSquare[k].FCnode = tnode;
					  }
					  else
					  {
						// do nothing...
					  }
					}
					else
					{
					  tnode = nodeID[ii];
					  cVertex[tnode].nodeKind = 2;
					}
				  }

				  cFedge[eid].node_id[0] = tnode1; // actual node ids for each edge...
				  cFedge[eid].node_id[1] = tnode2;
				  cFedge[eid].neigh_grainname[0] = pixgrainname[0];
				  cFedge[eid].neigh_grainname[1] = pixgrainname[1];
				  //if(pixgrainname[0]==pixgrainname[1]){
				  //printf("wrong %10d   %3d   %10d %10d   %10d %10d\n",
				  //eid, sqIndex, pixIndex[0], pixIndex[1], pixgrainname[0], pixgrainname[1]);
				  //}

				  cFedge[eid].segKind = 2; // edges of the open loops are always binary...
				  // triple lines only occurs iNSide the marching cubes...

				  // mark where each face edge belong to, upper, middle or bottom of the layer...
				  if (quot == 0)
				  {

					if (rmd1 == 0)
					{
					  cFedge[eid].where = 0;
					}
					else
					{
					  cFedge[eid].where = 1;
					}

				  }
				  else
				  { //when quot==1

					if (rmd1 == 0)
					{
					  cFedge[eid].where = 2;
					}
					else
					{
					  // do nothing...
					}
				  }

				  cSquare[k].edge_id[edgeCount] = eid;

				  edgeCount++;
				  eid++;
			  }
            }
          }
        }

      }

      cSquare[k].nEdge = edgeCount;
      //printf("%3d", edgeCount);
    }

  }

  //printf("\n");

  //printf("\nnum of edges: %d\n", eid);

}

int SurfaceMeshFunc::get_square_index(int tNS[4])
{

  // identify each square configuration using binary bit...
  // returNS the unique decimal integer for each configuration...

  int aBit[6];
  int tempIndex;
  int subIndex;

  if (tNS[0] == tNS[1])
  {
    aBit[0] = 0;
  }
  else
  {
    aBit[0] = 1;
  }
  if (tNS[1] == tNS[2])
  {
    aBit[1] = 0;
  }
  else
  {
    aBit[1] = 1;
  }
  if (tNS[2] == tNS[3])
  {
    aBit[2] = 0;
  }
  else
  {
    aBit[2] = 1;
  }
  if (tNS[3] == tNS[0])
  {
    aBit[3] = 0;
  }
  else
  {
    aBit[3] = 1;
  }
  if (tNS[0] == tNS[2])
  {
    aBit[4] = 0;
  }
  else
  {
    aBit[4] = 1;
  }
  if (tNS[1] == tNS[3])
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

int SurfaceMeshFunc::treat_anomaly(int tNSt[4], int zID1)
{

  int i, j, k, ii;

  int cid;

  int csite, cgrainname;
  int NSite, ngrainname;

  int temp;
  int tempFlag;

  int min, minid;

  int numNeigh[4];
  min = 27;
  minid = -1;

  for (k = 0; k < 4; k++)
  {
    numNeigh[k] = 0;
  }

  for (i = 0; i < 4; i++)
  {

    csite = tNSt[i];
    cid = csite - zID1 * NSP;
    cgrainname = point[csite].grainname;

    for (j = 1; j <= num_neigh; j++)
    {
      NSite = neigh[cid].neigh_id[j];
      ngrainname = point[NSite].grainname;
      if (cgrainname == ngrainname)
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

void SurfaceMeshFunc::get_nodes(int cst, int ord, int nidx[2], int *nid)
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
          nid[ii] = 7 * (cst + xDim - 1);
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
          nid[ii] = 7 * (cst + NSP - 1);
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
          nid[ii] = 7 * (cst + NSP - 1) + 1;
          break;
        case 3:
          nid[ii] = 7 * (cst + xDim - 1) + 2;
          break;
        case 4:
          nid[ii] = 7 * (cst - 1) + 5;
          break;
      }
    }
  }
}

void SurfaceMeshFunc::get_grainnames(int NSPneigh[4], int pID[2], int *pgrainname)
{

  int i;
  int pixTemp, tempgrainname;

  for (i = 0; i < 2; i++)
  {

    pixTemp = pID[i];

    switch (pixTemp)
    {
      case 0:
        tempgrainname = NSPneigh[0];
        pgrainname[i] = tempgrainname;
        break;
      case 1:
        tempgrainname = NSPneigh[1];
        pgrainname[i] = tempgrainname;
        break;
      case 2:
        tempgrainname = NSPneigh[2];
        pgrainname[i] = tempgrainname;
        break;
      case 3:
        tempgrainname = NSPneigh[3];
        pgrainname[i] = tempgrainname;
        break;
    }

    //printf("%7d %2d ", nid[i], tempIndex);
  }

}

int SurfaceMeshFunc::get_number_triangles()
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
  int tsqid1, tsqid2;
  int tsite1, tsite2;
  int cgrainname, ngrainname;
  int nburnt;
  int nds; // number of different grainname of upper square...
  int BCnode;
  int fcid;
  int tindex;
  int nTri, nTri0, nTri2, nTriM; // # triangles when fcenter is 0, 2 and more than 2...
  int arrayFC[6];
  int arraygrainname[8]; // stores 8 corner grainnames for center node kind...
  nTri0 = 0;
  nTri2 = 0;
  nTriM = 0;

  //printf("%d\n", NSP);

  for (i = 1; i <= NSP; i++)
  {

    cubeFlag = 0;

    // for each marching cube, collect faces...
    sqID[0] = 3 * (i - 1);
    sqID[1] = 3 * (i - 1) + 1;
    sqID[2] = 3 * (i - 1) + 2;
    sqID[3] = 3 * i + 2;
    sqID[4] = 3 * (i + xDim - 1) + 1;
    sqID[5] = 3 * (i + NSP - 1);

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
      tFC = cSquare[tsq].turnFC;
      if (tFC == 1)
      {
        arrayFC[fcid] = cSquare[tsq].FCnode;
        fcid++;
      }
      nFC = nFC + tFC;
      nFE = nFE + cSquare[tsq].nEdge;
      eff = eff + cSquare[tsq].effect;
    }

    if (fcid != nFC)
    {
      printf("Something's wrong with counting face centers...\n");
    }

    if (eff > 0)
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
        tsite1 = cSquare[tsqid1].site_id[j];
        tsite2 = cSquare[tsqid2].site_id[j];
        arraygrainname[j] = point[tsite1].grainname;
        arraygrainname[j + 4] = point[tsite2].grainname;
      }

      nds = 0;
      nburnt = 0;

      for (k = 0; k < 8; k++)
      {

        cgrainname = arraygrainname[k];

        if (cgrainname != -1)
        {

          nds++;
          arraygrainname[k] = -1; // burn...
          nburnt++;

          for (kk = 0; kk < 8; kk++)
          {
            ngrainname = arraygrainname[kk];
            if (cgrainname == ngrainname)
            {
              arraygrainname[kk] = -1; //burn...
              nburnt++;
            }
          }
        }
      }

      if (nburnt != 8)
      {
        printf("wrong burnt: %5d\n", nburnt);
      }

      cVertex[BCnode].nodeKind = nds + 10;
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
      //printf("\tcube iNSide grain\n");
    }
    else
    {
      //printf("\twhat?\n");
    }

    // if the current marching cube is a collection of 6 effective squares...and
    // the number of face edges at least 3...
    // when nFE==2, it doen't happen
    // when nFE==1, the edge will contribute for the neighboring marching cube...
    // when nFE==0, it meaNS the cube is iNSide a grain...
    if (cubeFlag == 1 && nFE > 2)
    {

      // Make edge array for each marching cube...
      arrayFE = new int[nFE];
      tindex = 0;
      for (i1 = 0; i1 < 6; i1++)
      {
        tsq = sqID[i1];
        tnfe = cSquare[tsq].nEdge;
        if (tnfe != 0)
        {
          for (i2 = 0; i2 < tnfe; i2++)
          {
            arrayFE[tindex] = cSquare[tsq].edge_id[i2];
            tindex++;
          }
        }
      }

      if (tindex != nFE)
      {
        printf("Something wrong with counting number of edges for marching cube...\n");
      }

      // CoNSider each case as Z. Wu's paper...
      if (nFC == 0)
      { // when there's no face center

        nTri0 = nTri0 + get_number_case0_triangles(arrayFE, nFE);

      }
      else if (nFC == 2)
      {

        nTri2 = nTri2 + get_number_case2_triangles(arrayFE, nFE, arrayFC, nFC);
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
         s0 = cSquare[tsq].site_id[0];
         s1 = cSquare[tsq].site_id[1];
         s2 = cSquare[tsq].site_id[2];
         s3 = cSquare[tsq].site_id[3];
         printf("%2d %2d %2d %2d   ", point[s0].grainname, point[s1].grainname, point[s2].grainname, point[s3].grainname);
         }
         printf("\n");
         */

      }
      else if (nFC > 2 && nFC <= 6)
      {

        nTriM = nTriM + get_number_caseM_triangles(arrayFE, nFE, arrayFC, nFC);
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
         s0 = cSquare[tsq].site_id[0];
         s1 = cSquare[tsq].site_id[1];
         s2 = cSquare[tsq].site_id[2];
         s3 = cSquare[tsq].site_id[3];
         printf("%2d %2d %2d %2d   ", point[s0].grainname, point[s1].grainname, point[s2].grainname, point[s3].grainname);
         }
         printf("\n");
         */

      }
      else
      {

        //printf("Somthing's wrong in counting face centers turned on...\n");
        //printf("%5d %10d\n\n", nFC, i);
      }

      delete [] arrayFE;
    }
  }

  //printf("\tNumber of triangles for each case:");
  //printf("%5d %5d %5d\n", nTri0, nTri2, nTriM);

  nTri = nTri0 + nTri2 + nTriM;
  return (nTri);
}

int SurfaceMeshFunc::get_number_case0_triangles(int *afe, int nfedge)
{

  int ii, i, j, jj, k, kk, k1;

  int loopID;
  int tail, head, coin;

  int cedge, nedge;
  int cgrainname1, cgrainname2, cnode1, cnode2;
  int ngrainname1, ngrainname2, nnode1, nnode2;
 // int tgrainname, tnode;
  int nucleus, chaser;
//  int  end;

  int grainnameFlag, nodeFlag, flip;

  int bflag, nbflag;
  int *burnt;
  int *burnt_list;

  int *count;
  int numN, numTri, tnumTri;

//  int tn1, tn2;
  int lid;

  burnt = new int[nfedge];
  burnt_list = new int[nfedge];

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

        cgrainname1 = cFedge[chaser].neigh_grainname[0];
        cgrainname2 = cFedge[chaser].neigh_grainname[1];
        cnode1 = cFedge[chaser].node_id[0];
        cnode2 = cFedge[chaser].node_id[1];

        for (j = 0; j < nfedge; j++)
        {

          nedge = afe[j];
          nbflag = burnt[j];

          if (nbflag == 0)
          {

            ngrainname1 = cFedge[nedge].neigh_grainname[0];
            ngrainname2 = cFedge[nedge].neigh_grainname[1];
            nnode1 = cFedge[nedge].node_id[0];
            nnode2 = cFedge[nedge].node_id[1];

            // checking if neighbor edge has same neighboring grainnames...
            if (((cgrainname1 == ngrainname1) && (cgrainname2 == ngrainname2)) || ((cgrainname1 == ngrainname2) && (cgrainname2 == ngrainname1)))
            {
              grainnameFlag = 1;
            }
            else
            {
              grainnameFlag = 0;
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

            if (grainnameFlag == 1 && nodeFlag == 1)
            {

              head = head + 1;
              burnt_list[head] = nedge;
              burnt[j] = loopID;

              if (flip == 1)
              {
                //cFedge[nedge].neigh_grainname[0] = ngrainname2;
                //cFedge[nedge].neigh_grainname[1] = ngrainname1;
                cFedge[nedge].node_id[0] = nnode2;
                cFedge[nedge].node_id[1] = nnode1;
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

  count = new int[loopID];

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
//        printf("%5d   %3d   %5d %5d   %5d %5d\n", cedge, lid, cFedge[cedge].node_id[0], cFedge[cedge].node_id[1], cFedge[cedge].neigh_grainname[0], cFedge[cedge].neigh_grainname[1]);
      }

//      printf("\n");

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
   cFedge[cedge].node_id[0], cFedge[cedge].node_id[1], cFedge[cedge].neigh_grainname[0], cFedge[cedge].neigh_grainname[1]);
   }
   printf("\n");
   */

  delete [] burnt;
  delete [] burnt_list;
  delete [] count;

  return (numTri);

}

int SurfaceMeshFunc::get_number_case2_triangles(int *afe,  int nfedge, int *afc, int nfctr)
{

  int ii, i, j, k, kk, k1, n, i1, j1;

  int loopID;
  int tail, head, coin;

  int cedge, nedge;
  int cgrainname1, cgrainname2, cnode1, cnode2;
  int ngrainname1, ngrainname2, nnode1, nnode2;
  int tgrainname, tnode;
  int nucleus, chaser;
  int start, end;
  int from, to;

  int grainnameFlag, nodeFlag, flip;

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

  burnt = new int[nfedge];
  burnt_list = new int[nfedge];


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

        cgrainname1 = cFedge[chaser].neigh_grainname[0];
        cgrainname2 = cFedge[chaser].neigh_grainname[1];
        cnode1 = cFedge[chaser].node_id[0];
        cnode2 = cFedge[chaser].node_id[1];

        for (j = 0; j < nfedge; j++)
        {

          nedge = afe[j];
          nbflag = burnt[j];

          if (nbflag == 0)
          {

            ngrainname1 = cFedge[nedge].neigh_grainname[0];
            ngrainname2 = cFedge[nedge].neigh_grainname[1];
            nnode1 = cFedge[nedge].node_id[0];
            nnode2 = cFedge[nedge].node_id[1];

            // checking if neighbor edge has same neighboring grainnames...
            if (((cgrainname1 == ngrainname1) && (cgrainname2 == ngrainname2)) || ((cgrainname1 == ngrainname2) && (cgrainname2 == ngrainname1)))
            {
              grainnameFlag = 1;
            }
            else
            {
              grainnameFlag = 0;
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

            if (grainnameFlag == 1 && nodeFlag == 1)
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

  count = new int[loopID];

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

    burnt_loop = new int[numN + 2];

    for (i1 = from; i1 < to; i1++)
    {

      cedge = burnt_list[i1];
      cnode1 = cFedge[cedge].node_id[0];
      cnode2 = cFedge[cedge].node_id[1];

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

    //printf("   start edge & node: %5d %5d", startEdge, cFedge[startEdge].node_id[0]);
    //printf("   open flag: %2d", openL);
    //printf("   flip flag: %2d", flip);
    //printf("\n");

    if (openL == 1)
    { // if current loop is an open loop...

      if (flip == 1)
      {
        tnode = cFedge[startEdge].node_id[0];
        tgrainname = cFedge[startEdge].neigh_grainname[0];
        cFedge[startEdge].node_id[0] = cFedge[startEdge].node_id[1];
        cFedge[startEdge].node_id[1] = tnode;
        cFedge[startEdge].neigh_grainname[0] = cFedge[startEdge].neigh_grainname[1];
        cFedge[startEdge].neigh_grainname[1] = tgrainname;
      }
      else
      {
        // do nothing...
      }

      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cFedge[startEdge].node_id[1];
      chaser = startEdge;

      do
      {

        for (n = from; n < to; n++)
        {

          cedge = burnt_list[n];
          cnode1 = cFedge[cedge].node_id[0];
          cnode2 = cFedge[cedge].node_id[1];
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
            tnode = cFedge[cedge].node_id[0];
            tgrainname = cFedge[cedge].neigh_grainname[0];
            cFedge[cedge].node_id[0] = cFedge[cedge].node_id[1];
            cFedge[cedge].node_id[1] = tnode;
            cFedge[cedge].neigh_grainname[0] = cFedge[cedge].neigh_grainname[1];
            cFedge[cedge].neigh_grainname[1] = tgrainname;
          }
          else
          {
            // do nothing...
          }

        }

        chaser = burnt_loop[index - 1];
        endNode = cFedge[chaser].node_id[1];

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

      delete [] burnt_loop;

    }
    else
    { // if current loop is a closed one....i.e., openL = 0...

      startEdge = burnt_list[from];
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cFedge[startEdge].node_id[1];
      chaser = startEdge;

      do
      {

        for (n = from; n < to; n++)
        {

          cedge = burnt_list[n];
          cnode1 = cFedge[cedge].node_id[0];
          cnode2 = cFedge[cedge].node_id[1];
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
            tnode = cFedge[cedge].node_id[0];
            tgrainname = cFedge[cedge].neigh_grainname[0];
            cFedge[cedge].node_id[0] = cFedge[cedge].node_id[1];
            cFedge[cedge].node_id[1] = tnode;
            cFedge[cedge].neigh_grainname[0] = cFedge[cedge].neigh_grainname[1];
            cFedge[cedge].neigh_grainname[1] = tgrainname;
          }
          else
          {
            // do nothing...
          }

        }

        chaser = burnt_loop[index - 1];
        endNode = cFedge[chaser].node_id[1];

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

      delete [] burnt_loop;
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
   cFedge[cedge].node_id[0], cFedge[cedge].node_id[1], cFedge[cedge].neigh_grainname[0], cFedge[cedge].neigh_grainname[1]);
   }
   printf("\n");
   */

  delete [] burnt;
  delete [] burnt_list;
  delete [] count;

  return (numTri);

}

int SurfaceMeshFunc::get_number_caseM_triangles(int *afe, int nfedge, int *afc, int nfctr)
{

  int ii, i, j, k, kk, k1, n, i1, j1, n1;

  int loopID;
  int tail, head, coin;

  int cedge, nedge;
  int cgrainname1, cgrainname2, cnode1, cnode2;
  int ngrainname1, ngrainname2, nnode1, nnode2;
  int tgrainname, tnode;
  int nucleus, chaser;
  int start;
  int from, to;

  int grainnameFlag, nodeFlag, flip;

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

  burnt = new int[nfedge];
  burnt_list = new int[nfedge];

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

        cgrainname1 = cFedge[chaser].neigh_grainname[0];
        cgrainname2 = cFedge[chaser].neigh_grainname[1];
        cnode1 = cFedge[chaser].node_id[0];
        cnode2 = cFedge[chaser].node_id[1];

        for (j = 0; j < nfedge; j++)
        {

          nedge = afe[j];
          nbflag = burnt[j];

          if (nbflag == 0)
          {

            ngrainname1 = cFedge[nedge].neigh_grainname[0];
            ngrainname2 = cFedge[nedge].neigh_grainname[1];
            nnode1 = cFedge[nedge].node_id[0];
            nnode2 = cFedge[nedge].node_id[1];

            // checking if neighbor edge has same neighboring grainnames...
            if (((cgrainname1 == ngrainname1) && (cgrainname2 == ngrainname2)) || ((cgrainname1 == ngrainname2) && (cgrainname2 == ngrainname1)))
            {
              grainnameFlag = 1;
            }
            else
            {
              grainnameFlag = 0;
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

            if (grainnameFlag == 1 && nodeFlag == 1)
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

  count = new int[loopID];

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

    burnt_loop = new int[(numN + 2)];

    for (i1 = from; i1 < to; i1++)
    {

      cedge = burnt_list[i1];
      cnode1 = cFedge[cedge].node_id[0];
      cnode2 = cFedge[cedge].node_id[1];

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

    //printf("   start edge & node: %5d %5d", startEdge, cFedge[startEdge].node_id[0]);
    //printf("   open flag: %2d", openL);
    //printf("   flip flag: %2d", flip);
    //printf("\n");


    if (openL == 1)
    { // if current loop is an open loop...

      if (flip == 1)
      {
        tnode = cFedge[startEdge].node_id[0];
        tgrainname = cFedge[startEdge].neigh_grainname[0];
        cFedge[startEdge].node_id[0] = cFedge[startEdge].node_id[1];
        cFedge[startEdge].node_id[1] = tnode;
        cFedge[startEdge].neigh_grainname[0] = cFedge[startEdge].neigh_grainname[1];
        cFedge[startEdge].neigh_grainname[1] = tgrainname;
      }
      else
      {
        // do nothing...
      }

      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cFedge[startEdge].node_id[1];
      chaser = startEdge;

      do
      {

        for (n = from; n < to; n++)
        {

          cedge = burnt_list[n];
          cnode1 = cFedge[cedge].node_id[0];
          cnode2 = cFedge[cedge].node_id[1];
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
            tnode = cFedge[cedge].node_id[0];
            tgrainname = cFedge[cedge].neigh_grainname[0];
            cFedge[cedge].node_id[0] = cFedge[cedge].node_id[1];
            cFedge[cedge].node_id[1] = tnode;
            cFedge[cedge].neigh_grainname[0] = cFedge[cedge].neigh_grainname[1];
            cFedge[cedge].neigh_grainname[1] = tgrainname;
          }
          else
          {
            // do nothing...
          }

        }

        chaser = burnt_loop[index - 1];
        endNode = cFedge[chaser].node_id[1];

      } while (index < numN);

      /*
       u = 0;
       for(u=0; u<index; u++){
       temp = burnt_loop[u];
       printf("%5d   %3d   %5d %5d   %5d %5d\n", temp, j1,
       cFedge[temp].node_id[0], cFedge[temp].node_id[1], cFedge[temp].neigh_grainname[0], cFedge[temp].neigh_grainname[1]);
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

      delete [] burnt_loop;

    }
    else
    { // if current loop is a closed one....i.e., openL = 0...

      startEdge = burnt_list[from];
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cFedge[startEdge].node_id[1];
      chaser = startEdge;

      do
      {

        for (n = from; n < to; n++)
        {

          cedge = burnt_list[n];
          cnode1 = cFedge[cedge].node_id[0];
          cnode2 = cFedge[cedge].node_id[1];
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
            tnode = cFedge[cedge].node_id[0];
            tgrainname = cFedge[cedge].neigh_grainname[0];
            cFedge[cedge].node_id[0] = cFedge[cedge].node_id[1];
            cFedge[cedge].node_id[1] = tnode;
            cFedge[cedge].neigh_grainname[0] = cFedge[cedge].neigh_grainname[1];
            cFedge[cedge].neigh_grainname[1] = tgrainname;
          }
          else
          {
            // do nothing...
          }

        }

        chaser = burnt_loop[index - 1];
        endNode = cFedge[chaser].node_id[1];

      } while (index < numN);

      /*
       u = 0;
       for(u=0; u<index; u++){
       temp = burnt_loop[u];
       printf("%5d   %3d   %5d %5d   %5d %5d\n", temp, j1,
       cFedge[temp].node_id[0], cFedge[temp].node_id[1], cFedge[temp].neigh_grainname[0], cFedge[temp].neigh_grainname[1]);
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

      delete [] burnt_loop;
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
   cFedge[cedge].node_id[0], cFedge[cedge].node_id[1], cFedge[cedge].neigh_grainname[0], cFedge[cedge].neigh_grainname[1]);
   }
   printf("\n");
   */

  delete [] burnt;
  delete [] burnt_list;
  delete [] count;

  return (numTri);

}

int SurfaceMeshFunc::get_triangles(int nTriangle)
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
  arrayFC = new int[6];
  tidIn = 0;
  cTriangle = new Patch[nTriangle];

  for (i = 1; i <= NSP; i++)
  {

    cubeFlag = 0;

    // for each marching cube, collect faces...
    sqID[0] = 3 * (i - 1);
    sqID[1] = 3 * (i - 1) + 1;
    sqID[2] = 3 * (i - 1) + 2;
    sqID[3] = 3 * i + 2;
    sqID[4] = 3 * (i + xDim - 1) + 1;
    sqID[5] = 3 * (i + NSP - 1);

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
      tFC = cSquare[tsq].turnFC;
      if (tFC == 1)
      {
        arrayFC[fcid] = cSquare[tsq].FCnode;
        fcid++;
      }
      nFC = nFC + tFC;
      nFE = nFE + cSquare[tsq].nEdge;
      eff = eff + cSquare[tsq].effect;
    }

    if (fcid != nFC)
    {
      printf("Something's wrong with counting face centers...\n");
    }

    if (eff > 0)
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
      //printf("\tcube iNSide grain\n");
    }
    else
    {
      printf("\twhat?\n");
    }

    // if the current marching cube is a collection of 6 effective squares...and
    // the number of face edges at least 3...
    // when nFE==2, it doen't happen
    // when nFE==1, the edge will contribute for the neighboring marching cube...
    // when nFE==0, it meaNS the cube is iNSide a grain...
    if (cubeFlag == 1 && nFE > 2)
    {

      // Make edge array for each marching cube...
      arrayFE = new int[nFE];
      tindex = 0;
      for (i1 = 0; i1 < 6; i1++)
      {
        tsq = sqID[i1];
        tnfe = cSquare[tsq].nEdge;
        if (tnfe != 0)
        {
          for (i2 = 0; i2 < tnfe; i2++)
          {
            arrayFE[tindex] = cSquare[tsq].edge_id[i2];
            tindex++;
          }
        }
      }

      if (tindex != nFE)
      {
        printf("Something wrong with counting number of edges for marching cube...\n");
      }

      // CoNSider each case as Z. Wu's paper...
      if (nFC == 0)
      { // when there's no face center

        //printf("%5d ", tidIn);
        get_case0_triangles(arrayFE, nFE, tidIn, &tidOut);
        tidIn = tidOut;
        //printf("%5d\n", tidIn);

      }
      else if (nFC == 2)
      {

        //printf("%5d ", tidIn);
        get_case2_triangles(arrayFE, nFE, arrayFC, nFC, tidIn, &tidOut);
        tidIn = tidOut;
        //printf("%5d\n", tidIn);

      }
      else if (nFC > 2 && nFC <= 6)
      {

        get_caseM_triangles(arrayFE, nFE, arrayFC, nFC, tidIn, &tidOut, bodyCtr);
        tidIn = tidOut;

      }
      else
      {

        //printf("Somthing's wrong in counting face centers turned on...\n");
        //printf("%5d %10d\n\n", nFC, i);
      }

      delete [] arrayFE;

    }

  }

  //printf("%5d\n", tidIn);

  return 0;
}

void SurfaceMeshFunc::get_case0_triangles(int *afe, int nfedge, int tin, int *tout)
{

  int ii, i, j, jj, k, kk, k1, mm;

  int loopID;
  int tail, head, coin;

  int cedge, nedge;
  int cgrainname1, cgrainname2, cnode1, cnode2;
  int ngrainname1, ngrainname2, nnode1, nnode2;
//  int tgrainname, tnode;
  int nucleus, chaser;
//  int start, end;

  int grainnameFlag, nodeFlag, flip;

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
  int te0, te1, te2, tv0, tcVertex, tv2;
  int numT, cnumT, new_node0, new_node1;

  burnt = new int[nfedge];
  burnt_list = new int[nfedge];


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

        cgrainname1 = cFedge[chaser].neigh_grainname[0];
        cgrainname2 = cFedge[chaser].neigh_grainname[1];
        cnode1 = cFedge[chaser].node_id[0];
        cnode2 = cFedge[chaser].node_id[1];

        for (j = 0; j < nfedge; j++)
        {

          nedge = afe[j];
          nbflag = burnt[j];

          if (nbflag == 0)
          {

            ngrainname1 = cFedge[nedge].neigh_grainname[0];
            ngrainname2 = cFedge[nedge].neigh_grainname[1];
            nnode1 = cFedge[nedge].node_id[0];
            nnode2 = cFedge[nedge].node_id[1];

            // checking if neighbor edge has same neighboring grainnames...
            if (((cgrainname1 == ngrainname1) && (cgrainname2 == ngrainname2)) || ((cgrainname1 == ngrainname2) && (cgrainname2 == ngrainname1)))
            {
              grainnameFlag = 1;
            }
            else
            {
              grainnameFlag = 0;
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

            if (grainnameFlag == 1 && nodeFlag == 1)
            {

              head = head + 1;
              burnt_list[head] = nedge;
              burnt[j] = loopID;

              if (flip == 1)
              {
                //cFedge[nedge].neigh_grainname[0] = ngrainname2;
                //cFedge[nedge].neigh_grainname[1] = ngrainname1;
                cFedge[nedge].node_id[0] = nnode2;
                cFedge[nedge].node_id[1] = nnode1;
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

  count = new int[loopID];

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

    loop = new int[numN];

    for (mm = 0; mm < numN; mm++)
    {
      loop[mm] = burnt_list[from + mm];
    }

    if (numN == 3)
    {

      te0 = loop[0];
      te1 = loop[1];
      te2 = loop[2];
      cTriangle[ctid].v_id[0] = cFedge[te0].node_id[0];
      cTriangle[ctid].v_id[1] = cFedge[te1].node_id[0];
      cTriangle[ctid].v_id[2] = cFedge[te2].node_id[0];
      cTriangle[ctid].ngrainname[0] = cFedge[te0].neigh_grainname[0];
      cTriangle[ctid].ngrainname[1] = cFedge[te0].neigh_grainname[1];
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
      tv0 = cFedge[te0].node_id[0];
      tcVertex = cFedge[te0].node_id[1];
      tv2 = cFedge[te1].node_id[0];
      cTriangle[ctid].v_id[0] = tv0;
      cTriangle[ctid].v_id[1] = tcVertex;
      cTriangle[ctid].v_id[2] = tv2;
      cTriangle[ctid].ngrainname[0] = cFedge[te0].neigh_grainname[0];
      cTriangle[ctid].ngrainname[1] = cFedge[te0].neigh_grainname[1];
      new_node0 = tv2;
      new_node1 = tcVertex;

      cnumT++;
      ctid++;

      do
      {

        if ((cnumT % 2) != 0)
        {

          front = front + 1;
          ce = loop[front];
          tv0 = cFedge[ce].node_id[0];
          tcVertex = cFedge[ce].node_id[1];
          tv2 = new_node0;
          cTriangle[ctid].v_id[0] = tv0;
          cTriangle[ctid].v_id[1] = tcVertex;
          cTriangle[ctid].v_id[2] = tv2;
          cTriangle[ctid].ngrainname[0] = cFedge[ce].neigh_grainname[0];
          cTriangle[ctid].ngrainname[1] = cFedge[ce].neigh_grainname[1];
          new_node0 = tcVertex;
          cnumT++;
          ctid++;

        }
        else
        {

          back = back - 1;
          ce = loop[back];
          tv0 = cFedge[ce].node_id[0];
          tcVertex = cFedge[ce].node_id[1];
          tv2 = new_node0;
          cTriangle[ctid].v_id[0] = tv0;
          cTriangle[ctid].v_id[1] = tcVertex;
          cTriangle[ctid].v_id[2] = tv2;
          cTriangle[ctid].ngrainname[0] = cFedge[ce].neigh_grainname[0];
          cTriangle[ctid].ngrainname[1] = cFedge[ce].neigh_grainname[1];
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

    delete [] loop;

  }

  *tout = ctid;
  //printf("%10d\n", ctid);

  delete [] burnt;
  delete [] burnt_list;
  delete [] count;

}

void SurfaceMeshFunc::get_case2_triangles(int *afe, int nfedge, int *afc, int nfctr, int tin, int *tout)
{

  int ii, i, j, k, kk, k1, n, i1, j1;

  int loopID;
  int tail, head, coin;

  int cedge, nedge;
  int cgrainname1, cgrainname2, cnode1, cnode2;
  int ngrainname1, ngrainname2, nnode1, nnode2;
  int tgrainname, tnode;
  int nucleus, chaser;
  int start, end;
  int from, to;

  int grainnameFlag, nodeFlag, flip;

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
  int te0, te1, te2, tv0, tcVertex, tv2;
  int numT, cnumT, new_node0, new_node1;

  burnt = new int[nfedge];
  burnt_list = new int[nfedge];


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

        cgrainname1 = cFedge[chaser].neigh_grainname[0];
        cgrainname2 = cFedge[chaser].neigh_grainname[1];
        cnode1 = cFedge[chaser].node_id[0];
        cnode2 = cFedge[chaser].node_id[1];

        for (j = 0; j < nfedge; j++)
        {

          nedge = afe[j];
          nbflag = burnt[j];

          if (nbflag == 0)
          {

            ngrainname1 = cFedge[nedge].neigh_grainname[0];
            ngrainname2 = cFedge[nedge].neigh_grainname[1];
            nnode1 = cFedge[nedge].node_id[0];
            nnode2 = cFedge[nedge].node_id[1];

            // checking if neighbor edge has same neighboring grainnames...
            if (((cgrainname1 == ngrainname1) && (cgrainname2 == ngrainname2)) || ((cgrainname1 == ngrainname2) && (cgrainname2 == ngrainname1)))
            {
              grainnameFlag = 1;
            }
            else
            {
              grainnameFlag = 0;
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

            if (grainnameFlag == 1 && nodeFlag == 1)
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

  count = new int[loopID];

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

    burnt_loop = new int[numN];

    for (i1 = from; i1 < to; i1++)
    {

      cedge = burnt_list[i1];
      cnode1 = cFedge[cedge].node_id[0];
      cnode2 = cFedge[cedge].node_id[1];

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

    //printf("   start edge & node: %5d %5d", startEdge, cFedge[startEdge].node_id[0]);
    //printf("   open flag: %2d", openL);
    //printf("   flip flag: %2d", flip);
    //printf("\n");

    if (openL == 1)
    { // if current loop is an open loop...

      if (flip == 1)
      {
        tnode = cFedge[startEdge].node_id[0];
        tgrainname = cFedge[startEdge].neigh_grainname[0];
        cFedge[startEdge].node_id[0] = cFedge[startEdge].node_id[1];
        cFedge[startEdge].node_id[1] = tnode;
        cFedge[startEdge].neigh_grainname[0] = cFedge[startEdge].neigh_grainname[1];
        cFedge[startEdge].neigh_grainname[1] = tgrainname;
      }
      else
      {
        // do nothing...
      }

      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cFedge[startEdge].node_id[1];
      chaser = startEdge;

      do
      {

        for (n = from; n < to; n++)
        {

          cedge = burnt_list[n];
          cnode1 = cFedge[cedge].node_id[0];
          cnode2 = cFedge[cedge].node_id[1];
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
            tnode = cFedge[cedge].node_id[0];
            tgrainname = cFedge[cedge].neigh_grainname[0];
            cFedge[cedge].node_id[0] = cFedge[cedge].node_id[1];
            cFedge[cedge].node_id[1] = tnode;
            cFedge[cedge].neigh_grainname[0] = cFedge[cedge].neigh_grainname[1];
            cFedge[cedge].neigh_grainname[1] = tgrainname;
          }
          else
          {
            // do nothing...
          }

        }

        chaser = burnt_loop[index - 1];
        endNode = cFedge[chaser].node_id[1];

      } while (index < numN);

      if (numN == 2)
      {

        te0 = burnt_loop[0];
        te1 = burnt_loop[1];
        cTriangle[ctid].v_id[0] = cFedge[te0].node_id[0];
        cTriangle[ctid].v_id[1] = cFedge[te1].node_id[0];
        cTriangle[ctid].v_id[2] = cFedge[te1].node_id[1];
        cTriangle[ctid].ngrainname[0] = cFedge[te0].neigh_grainname[0];
        cTriangle[ctid].ngrainname[1] = cFedge[te0].neigh_grainname[1];
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
        tv0 = cFedge[te0].node_id[0];
        tcVertex = cFedge[te0].node_id[1];
        tv2 = cFedge[te1].node_id[1];
        cTriangle[ctid].v_id[0] = tv0;
        cTriangle[ctid].v_id[1] = tcVertex;
        cTriangle[ctid].v_id[2] = tv2;
        cTriangle[ctid].ngrainname[0] = cFedge[te0].neigh_grainname[0];
        cTriangle[ctid].ngrainname[1] = cFedge[te0].neigh_grainname[1];
        new_node0 = tv2;
        new_node1 = tcVertex;

        cnumT++;
        ctid++;

        do
        {

          if ((cnumT % 2) != 0)
          {

            front = front + 1;
            ce = burnt_loop[front];
            tv0 = cFedge[ce].node_id[0];
            tcVertex = cFedge[ce].node_id[1];
            tv2 = new_node0;
            cTriangle[ctid].v_id[0] = tv0;
            cTriangle[ctid].v_id[1] = tcVertex;
            cTriangle[ctid].v_id[2] = tv2;
            cTriangle[ctid].ngrainname[0] = cFedge[ce].neigh_grainname[0];
            cTriangle[ctid].ngrainname[1] = cFedge[ce].neigh_grainname[1];
            new_node0 = tcVertex;
            cnumT++;
            ctid++;

          }
          else
          {

            back = back - 1;
            ce = burnt_loop[back];
            tv0 = cFedge[ce].node_id[0];
            tcVertex = cFedge[ce].node_id[1];
            tv2 = new_node0;
            cTriangle[ctid].v_id[0] = tv0;
            cTriangle[ctid].v_id[1] = tcVertex;
            cTriangle[ctid].v_id[2] = tv2;
            cTriangle[ctid].ngrainname[0] = cFedge[ce].neigh_grainname[0];
            cTriangle[ctid].ngrainname[1] = cFedge[ce].neigh_grainname[1];
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
       temp1 = cFedge[temp].node_id[0];
       temp2 = cFedge[temp].node_id[1];
       printf("%5d   %5d %5d\n", temp, temp1, temp2);
       }
       printf("\n");
       */

      delete [] burnt_loop;

    }
    else
    { // if current loop is a closed one....i.e., openL = 0...

      startEdge = burnt_list[from];
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cFedge[startEdge].node_id[1];
      chaser = startEdge;

      do
      {

        for (n = from; n < to; n++)
        {

          cedge = burnt_list[n];
          cnode1 = cFedge[cedge].node_id[0];
          cnode2 = cFedge[cedge].node_id[1];
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
            tnode = cFedge[cedge].node_id[0];
            tgrainname = cFedge[cedge].neigh_grainname[0];
            cFedge[cedge].node_id[0] = cFedge[cedge].node_id[1];
            cFedge[cedge].node_id[1] = tnode;
            cFedge[cedge].neigh_grainname[0] = cFedge[cedge].neigh_grainname[1];
            cFedge[cedge].neigh_grainname[1] = tgrainname;
          }
          else
          {
            // do nothing...
          }

        }

        chaser = burnt_loop[index - 1];
        endNode = cFedge[chaser].node_id[1];

      } while (index < numN);

      if (numN == 3)
      {

        te0 = burnt_loop[0];
        te1 = burnt_loop[1];
        te2 = burnt_loop[2];
        cTriangle[ctid].v_id[0] = cFedge[te0].node_id[0];
        cTriangle[ctid].v_id[1] = cFedge[te1].node_id[0];
        cTriangle[ctid].v_id[2] = cFedge[te2].node_id[0];
        cTriangle[ctid].ngrainname[0] = cFedge[te0].neigh_grainname[0];
        cTriangle[ctid].ngrainname[1] = cFedge[te0].neigh_grainname[1];
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
        tv0 = cFedge[te0].node_id[0];
        tcVertex = cFedge[te0].node_id[1];
        tv2 = cFedge[te1].node_id[0];
        cTriangle[ctid].v_id[0] = tv0;
        cTriangle[ctid].v_id[1] = tcVertex;
        cTriangle[ctid].v_id[2] = tv2;
        cTriangle[ctid].ngrainname[0] = cFedge[te0].neigh_grainname[0];
        cTriangle[ctid].ngrainname[1] = cFedge[te0].neigh_grainname[1];
        new_node0 = tv2;
        new_node1 = tcVertex;

        cnumT++;
        ctid++;

        do
        {

          if ((cnumT % 2) != 0)
          {

            front = front + 1;
            ce = burnt_loop[front];
            tv0 = cFedge[ce].node_id[0];
            tcVertex = cFedge[ce].node_id[1];
            tv2 = new_node0;
            cTriangle[ctid].v_id[0] = tv0;
            cTriangle[ctid].v_id[1] = tcVertex;
            cTriangle[ctid].v_id[2] = tv2;
            cTriangle[ctid].ngrainname[0] = cFedge[ce].neigh_grainname[0];
            cTriangle[ctid].ngrainname[1] = cFedge[ce].neigh_grainname[1];
            new_node0 = tcVertex;
            cnumT++;
            ctid++;

          }
          else
          {

            back = back - 1;
            ce = burnt_loop[back];
            tv0 = cFedge[ce].node_id[0];
            tcVertex = cFedge[ce].node_id[1];
            tv2 = new_node0;
            cTriangle[ctid].v_id[0] = tv0;
            cTriangle[ctid].v_id[1] = tcVertex;
            cTriangle[ctid].v_id[2] = tv2;
            cTriangle[ctid].ngrainname[0] = cFedge[ce].neigh_grainname[0];
            cTriangle[ctid].ngrainname[1] = cFedge[ce].neigh_grainname[1];
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
       ttemp1 = cFedge[ttemp].node_id[0];
       ttemp2 = cFedge[ttemp].node_id[1];
       printf("%5d   %5d %5d\n", ttemp, ttemp1, ttemp2);
       }
       printf("\n");
       */

      delete [] burnt_loop;

    }

  }

  *tout = ctid;
  //printf("%10d ", ctid);

  delete [] burnt;
  delete [] burnt_list;
  delete [] count;

}

void SurfaceMeshFunc::get_caseM_triangles(int *afe, int nfedge, int *afc, int nfctr, int tin, int *tout, int ccn)
{

  int ii, i, j, k, kk, k1, n, i1, j1, n1, iii;

  int loopID;
  int tail, head, coin;

  int cedge, nedge;
  int cgrainname1, cgrainname2, cnode1, cnode2;
  int ngrainname1, ngrainname2, nnode1, nnode2;
  int tgrainname, tnode;
  int nucleus, chaser;
  int start;
  int from, to;

  int grainnameFlag, nodeFlag, flip;

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
  int te0, te1, te2, tv0, tcVertex, tv2, ts0, ts1;
  int numT, cnumT, new_node0, new_node1;

  burnt = new int[nfedge];
  burnt_list = new int[nfedge];


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

        cgrainname1 = cFedge[chaser].neigh_grainname[0];
        cgrainname2 = cFedge[chaser].neigh_grainname[1];
        cnode1 = cFedge[chaser].node_id[0];
        cnode2 = cFedge[chaser].node_id[1];

        for (j = 0; j < nfedge; j++)
        {

          nedge = afe[j];
          nbflag = burnt[j];

          if (nbflag == 0)
          {

            ngrainname1 = cFedge[nedge].neigh_grainname[0];
            ngrainname2 = cFedge[nedge].neigh_grainname[1];
            nnode1 = cFedge[nedge].node_id[0];
            nnode2 = cFedge[nedge].node_id[1];

            // checking if neighbor edge has same neighboring grainnames...
            if (((cgrainname1 == ngrainname1) && (cgrainname2 == ngrainname2)) || ((cgrainname1 == ngrainname2) && (cgrainname2 == ngrainname1)))
            {
              grainnameFlag = 1;
            }
            else
            {
              grainnameFlag = 0;
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

            if (grainnameFlag == 1 && nodeFlag == 1)
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

  count = new int[loopID];

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

    burnt_loop = new int[numN];

    for (i1 = from; i1 < to; i1++)
    {

      cedge = burnt_list[i1];
      cnode1 = cFedge[cedge].node_id[0];
      cnode2 = cFedge[cedge].node_id[1];

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
        tnode = cFedge[startEdge].node_id[0];
        tgrainname = cFedge[startEdge].neigh_grainname[0];
        cFedge[startEdge].node_id[0] = cFedge[startEdge].node_id[1];
        cFedge[startEdge].node_id[1] = tnode;
        cFedge[startEdge].neigh_grainname[0] = cFedge[startEdge].neigh_grainname[1];
        cFedge[startEdge].neigh_grainname[1] = tgrainname;
      }
      else
      {
        // do nothing...
      }

      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cFedge[startEdge].node_id[1];
      chaser = startEdge;

      do
      {

        for (n = from; n < to; n++)
        {

          cedge = burnt_list[n];
          cnode1 = cFedge[cedge].node_id[0];
          cnode2 = cFedge[cedge].node_id[1];
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
            tnode = cFedge[cedge].node_id[0];
            tgrainname = cFedge[cedge].neigh_grainname[0];
            cFedge[cedge].node_id[0] = cFedge[cedge].node_id[1];
            cFedge[cedge].node_id[1] = tnode;
            cFedge[cedge].neigh_grainname[0] = cFedge[cedge].neigh_grainname[1];
            cFedge[cedge].neigh_grainname[1] = tgrainname;
          }
          else
          {
            // do nothing...
          }

        }

        chaser = burnt_loop[index - 1];
        endNode = cFedge[chaser].node_id[1];

      } while (index < numN);

      /*
       u = 0;
       for(u=0; u<index; u++){
       temp = burnt_loop[u];
       printf("%5d   %3d   %5d %5d   %5d %5d\n", temp, j1,
       cFedge[temp].node_id[0], cFedge[temp].node_id[1], cFedge[temp].neigh_grainname[0], cFedge[temp].neigh_grainname[1]);
       }
       */

      //triangulation...
      for (iii = 0; iii < numN; iii++)
      {
        ce = burnt_loop[iii];
        tn0 = cFedge[ce].node_id[0];
        tn1 = cFedge[ce].node_id[1];
        ts0 = cFedge[ce].neigh_grainname[0];
        ts1 = cFedge[ce].neigh_grainname[1];
        cTriangle[ctid].v_id[0] = ccn;
        cTriangle[ctid].v_id[1] = tn0;
        cTriangle[ctid].v_id[2] = tn1;
        cTriangle[ctid].ngrainname[0] = ts0;
        cTriangle[ctid].ngrainname[1] = ts1;
        ctid++;
      }

      delete [] burnt_loop;

    }
    else
    { // if current loop is a closed one....i.e., openL = 0...

      startEdge = burnt_list[from];
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cFedge[startEdge].node_id[1];
      chaser = startEdge;

      do
      {

        for (n = from; n < to; n++)
        {

          cedge = burnt_list[n];
          cnode1 = cFedge[cedge].node_id[0];
          cnode2 = cFedge[cedge].node_id[1];
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
            tnode = cFedge[cedge].node_id[0];
            tgrainname = cFedge[cedge].neigh_grainname[0];
            cFedge[cedge].node_id[0] = cFedge[cedge].node_id[1];
            cFedge[cedge].node_id[1] = tnode;
            cFedge[cedge].neigh_grainname[0] = cFedge[cedge].neigh_grainname[1];
            cFedge[cedge].neigh_grainname[1] = tgrainname;
          }
          else
          {
            // do nothing...
          }

        }

        chaser = burnt_loop[index - 1];
        endNode = cFedge[chaser].node_id[1];

      } while (index < numN);

      /*
       u = 0;
       for(u=0; u<index; u++){
       temp = burnt_loop[u];
       printf("%5d   %3d   %5d %5d   %5d %5d\n", temp, j1,
       cFedge[temp].node_id[0], cFedge[temp].node_id[1], cFedge[temp].neigh_grainname[0], cFedge[temp].neigh_grainname[1]);
       }
       */

      if (numN == 3)
      {

        te0 = burnt_loop[0];
        te1 = burnt_loop[1];
        te2 = burnt_loop[2];
        cTriangle[ctid].v_id[0] = cFedge[te0].node_id[0];
        cTriangle[ctid].v_id[1] = cFedge[te1].node_id[0];
        cTriangle[ctid].v_id[2] = cFedge[te2].node_id[0];
        cTriangle[ctid].ngrainname[0] = cFedge[te0].neigh_grainname[0];
        cTriangle[ctid].ngrainname[1] = cFedge[te0].neigh_grainname[1];
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
        tv0 = cFedge[te0].node_id[0];
        tcVertex = cFedge[te0].node_id[1];
        tv2 = cFedge[te1].node_id[0];
        cTriangle[ctid].v_id[0] = tv0;
        cTriangle[ctid].v_id[1] = tcVertex;
        cTriangle[ctid].v_id[2] = tv2;
        cTriangle[ctid].ngrainname[0] = cFedge[te0].neigh_grainname[0];
        cTriangle[ctid].ngrainname[1] = cFedge[te0].neigh_grainname[1];
        new_node0 = tv2;
        new_node1 = tcVertex;

        cnumT++;
        ctid++;

        do
        {

          if ((cnumT % 2) != 0)
          {

            front = front + 1;
            ce = burnt_loop[front];
            tv0 = cFedge[ce].node_id[0];
            tcVertex = cFedge[ce].node_id[1];
            tv2 = new_node0;
            cTriangle[ctid].v_id[0] = tv0;
            cTriangle[ctid].v_id[1] = tcVertex;
            cTriangle[ctid].v_id[2] = tv2;
            cTriangle[ctid].ngrainname[0] = cFedge[ce].neigh_grainname[0];
            cTriangle[ctid].ngrainname[1] = cFedge[ce].neigh_grainname[1];
            new_node0 = tcVertex;
            cnumT++;
            ctid++;

          }
          else
          {

            back = back - 1;
            ce = burnt_loop[back];
            tv0 = cFedge[ce].node_id[0];
            tcVertex = cFedge[ce].node_id[1];
            tv2 = new_node0;
            cTriangle[ctid].v_id[0] = tv0;
            cTriangle[ctid].v_id[1] = tcVertex;
            cTriangle[ctid].v_id[2] = tv2;
            cTriangle[ctid].ngrainname[0] = cFedge[ce].neigh_grainname[0];
            cTriangle[ctid].ngrainname[1] = cFedge[ce].neigh_grainname[1];
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

      delete [] burnt_loop;
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
   cFedge[cedge].node_id[0], cFedge[cedge].node_id[1], cFedge[cedge].neigh_grainname[0], cFedge[cedge].neigh_grainname[1]);
   }
   printf("\n");
   */

  delete [] burnt;
  delete [] burnt_list;
  delete [] count;

}

void SurfaceMeshFunc::arrange_grainnames(int numT, int zID)
{

  int i, j, k;

  int cnode;
  int csite, kind;
  int cst_index;
  int tsite1, tsite2;
  int ngrainname1, ngrainname2;
  int tgrainname1, tgrainname2;

  double cx, cy, cz;
  double xSum, ySum, zSum;
  double xSum1, ySum1, zSum1;
  double xSum2, ySum2, zSum2;

  int nEnode;

  double ctr1[3];
  double ctr2[3];

  double ncVertex[3];
  double tcVertex[3];
  double tv2[3];

  double vcoord[3][3];
  double u[3];
  double w[3];

  double a, b, c, length;
  double cs1, cs2;
  double length1, length2; // length of the vectors...
  double dotP1, dotP2;
  double theta1, theta2;

  double tneigh[3];

  for (i = 0; i < numT; i++)
  { // for each triangle...

    ngrainname1 = cTriangle[i].ngrainname[0];
    ngrainname2 = cTriangle[i].ngrainname[1];

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

    //printf("%3d %3d\t", ngrainname1, ngrainname2);

    for (j = 0; j < 3; j++)
    { // for each node iNSide the triangle...

      cnode = cTriangle[i].v_id[j];
      cst_index = cnode / 7 + 1;
      csite = cnode / 7 + 1 + NSP * zID;
      kind = cnode % 7;

      xSum = xSum + cVertex[cnode].xc;
      ySum = ySum + cVertex[cnode].yc;
      zSum = zSum + cVertex[cnode].zc;

      vcoord[j][0] = cVertex[cnode].xc;
      vcoord[j][1] = cVertex[cnode].yc;
      vcoord[j][2] = cVertex[cnode].zc;

      if (kind == 0)
      {

        nEnode++;
        tsite1 = csite;
        tsite2 = neigh[cst_index].neigh_id[1];
        tgrainname1 = point[tsite1].grainname;
        tgrainname2 = point[tsite2].grainname;

      }
      else if (kind == 1)
      {

        nEnode++;
        tsite1 = csite;
        tsite2 = neigh[cst_index].neigh_id[7];
        tgrainname1 = point[tsite1].grainname;
        tgrainname2 = point[tsite2].grainname;

      }
      else if (kind == 2)
      {

        nEnode++;
        tsite1 = csite;
        tsite2 = neigh[cst_index].neigh_id[18];
        tgrainname1 = point[tsite1].grainname;
        tgrainname2 = point[tsite2].grainname;

      }
      else
      {

        tgrainname1 = -1;
        tgrainname2 = -1;
      }

      //printf("%3d %3d\t", tgrainname1, tgrainname2);

      if (tgrainname1 == ngrainname1)
      {

        xSum1 = xSum1 + find_xcoord(tsite1);
        ySum1 = ySum1 + find_ycoord(tsite1);
        zSum1 = zSum1 + find_zcoord(tsite1);

        xSum2 = xSum2 + find_xcoord(tsite2);
        ySum2 = ySum2 + find_ycoord(tsite2);
        zSum2 = zSum2 + find_zcoord(tsite2);

      }
      else if (tgrainname2 == ngrainname1)
      {

        xSum1 = xSum1 + find_xcoord(tsite2);
        ySum1 = ySum1 + find_ycoord(tsite2);
        zSum1 = zSum1 + find_zcoord(tsite2);

        xSum2 = xSum2 + find_xcoord(tsite1);
        ySum2 = ySum2 + find_ycoord(tsite1);
        zSum2 = zSum2 + find_zcoord(tsite1);

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

    tcVertex[0] = ctr1[0] - cx;
    tcVertex[1] = ctr1[1] - cy;
    tcVertex[2] = ctr1[2] - cz;

    tv2[0] = ctr2[0] - cx;
    tv2[1] = ctr2[1] - cy;
    tv2[2] = ctr2[2] - cz;

    length1 = sqrt(tcVertex[0] * tcVertex[0] + tcVertex[1] * tcVertex[1] + tcVertex[2] * tcVertex[2]);
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
    tneigh[0] = a / length;
    tneigh[1] = b / length;
    tneigh[2] = c / length;

    for (k = 0; k < 3; k++)
    {
      if (fabs(tneigh[k]) < 0.0001)
      {
        tneigh[k] = 0.0;
      }
    }

    // update patch info...
    cTriangle[i].normal[0] = tneigh[0];
    cTriangle[i].normal[1] = tneigh[1];
    cTriangle[i].normal[2] = tneigh[2];
    cTriangle[i].area = 0.5 * length;

    // normals to triangle...
    ncVertex[0] = a;
    ncVertex[1] = b;
    ncVertex[2] = c;

    // Let's arrange the grainname order...
    // Getting angles between vectors...
    dotP1 = tcVertex[0] * ncVertex[0] + tcVertex[1] * ncVertex[1] + tcVertex[2] * ncVertex[2];
    dotP2 = tv2[0] * ncVertex[0] + tv2[1] * ncVertex[1] + tv2[2] * ncVertex[2];

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

    theta1 = 180.0 / M_PI * acos(cs1);
    theta2 = 180.0 / M_PI * acos(cs2);

    // update neighboring grainnames...
    if (theta1 < theta2)
    {
      cTriangle[i].ngrainname[0] = ngrainname1;
      cTriangle[i].ngrainname[1] = ngrainname2;
    }
    else
    {
      cTriangle[i].ngrainname[0] = ngrainname2;
      cTriangle[i].ngrainname[1] = ngrainname1;
    }

    //printf("%10.5f %10.5f\n", theta1, theta2);
  }

}

int SurfaceMeshFunc::get_inner_edges(int nfe, int nT, int tnIEdge)
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
  cIedge = new ISegment[tnIEdge];
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
      n1 = cTriangle[i].v_id[index1];
      n2 = cTriangle[i].v_id[index2];

      // Find nodeKind...
      cnk1 = cVertex[n1].nodeKind;
      cnk2 = cVertex[n2].nodeKind;

      if ((cnk1 > 10) || (cnk2 > 10))
      { // edges connected to body centers...
        cTriangle[i].edgePlace[j] = 1;
      }
      else if (((cnk1 > 2) && (cnk1 <= 10)) && ((cnk2 > 2) && (cnk2 <= 10)))
      {
        cTriangle[i].edgePlace[j] = 1;
      }
      else
      {
        cTriangle[i].edgePlace[j] = -1;
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

    cn1 = cFedge[ii].node_id[0];
    cn2 = cFedge[ii].node_id[1];

    for (jj = 0; jj < nT; jj++)
    {

      for (kk = 0; kk < 3; kk++)
      {

        bflag = cTriangle[jj].edgePlace[kk];

        if (bflag == -1)
        {

          index1 = kk;
          index2 = kk + 1;

          if (index1 == 2)
          {
            index2 = 0;
          }

          // Find nodes...
          n1 = cTriangle[jj].v_id[index1];
          n2 = cTriangle[jj].v_id[index2];

          if ((cn1 == n1 && cn2 == n2) || (cn1 == n2 && cn2 == n1))
          {
            cTriangle[jj].edgePlace[kk] = 0;
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

    ts1 = cTriangle[i1].ngrainname[0];
    ts2 = cTriangle[i1].ngrainname[1];

    for (j1 = 0; j1 < 3; j1++)
    {

      index1 = j1;
      index2 = j1 + 1;

      if (index1 == 2)
      {
        index2 = 0;
      }

      bflag = cTriangle[i1].edgePlace[index1];

      if (bflag != 0)
      {

        cIedge[index].n_id[0] = cTriangle[i1].v_id[index1];
        cIedge[index].n_id[1] = cTriangle[i1].v_id[index2];
        cIedge[index].ngrainname[0] = ts1;
        cIedge[index].ngrainname[1] = ts2;
        cIedge[index].ngrainname[2] = 0;
        cIedge[index].ngrainname[3] = 0;
        cIedge[index].edgeKind = 2;
        cIedge[index].burnt = -1;
        cIedge[index].eff = -1;
        index++;

        cTriangle[i1].edgePlace[index1] = 1;
      }

    }

  }

  return (index);

}

void SurfaceMeshFunc::find_unique_inner_edges(int nie, int *nEff)
{

  int i, j, k;

  int cedge, nedge;
  int bflag, nbflag;

  int cn1, cn2;
  int cNS1, cNS2, cNS;

  int nn1, nn2;
  int nNS1, nNS2;

  int numCN; // number of current neigh grainnames...
  int newCN1, newCN2; // flag for new neigh, if it's 0, add the neigh...

  int count;
  count = 0;

  //printf("\t\t%d\n", nie);

  for (i = 0; i < nie; i++)
  {

    cedge = i;
    bflag = cIedge[cedge].burnt;

    //if(i%10000==0){
    //printf("\t\t%d\n", i);
    //}

    if (bflag == -1)
    {

      cIedge[cedge].eff = 1; // If this one is a unique one, mark it.
      count++;
      cIedge[cedge].burnt = 1; // burn current edge...

      cn1 = cIedge[cedge].n_id[0]; // get current nodes...
      cn2 = cIedge[cedge].n_id[1];
      cNS1 = cIedge[cedge].ngrainname[0]; // get current neigh grainnames...
      cNS2 = cIedge[cedge].ngrainname[1];

      for (j = 0; j < nie; j++)
      {

        nedge = j;
        nbflag = cIedge[nedge].burnt;

        if (nbflag == -1)
        {

          nn1 = cIedge[nedge].n_id[0]; // get neigh nodes...
          nn2 = cIedge[nedge].n_id[1];
          nNS1 = cIedge[nedge].ngrainname[0]; // get neigh's neigh grainnames...
          nNS2 = cIedge[nedge].ngrainname[1];

          if ((cn1 == nn1 && cn2 == nn2) || (cn1 == nn2 && cn2 == nn1))
          { // if they are the same nodes...

            cIedge[nedge].burnt = 1; // if they are the same nodes, burn it...

            numCN = cIedge[cedge].edgeKind;
            newCN1 = 0;
            newCN2 = 0;

            for (k = 0; k < numCN; k++)
            { // remember nNS1 is different from nNS2...

              cNS = cIedge[cedge].ngrainname[k];

              if (cNS == nNS1)
              {
                newCN1++;
              }

              if (cNS == nNS2)
              {
                newCN2++;
              }
            }

            if (newCN1 == 0)
            {
              cIedge[cedge].ngrainname[numCN] = nNS1;
              numCN++;
            }

            if (newCN2 == 0)
            {
              cIedge[cedge].ngrainname[numCN] = nNS2;
              numCN++;
            }

            cIedge[cedge].edgeKind = numCN;
          }

        }

      }

    }

  }

  *nEff = count;

}

void SurfaceMeshFunc::copy_previous_nodes()
{

  int i, j;
  int start, end;

  int pid, cid;

  int tempID;

  start = NSP + 1;
  end = 2 * NSP;

  for (i = start; i <= end; i++)
  {

    for (j = 0; j < 7; j++)
    {

      pid = (i - 1) * 7 + j;
      tempID = pVertex[pid].newNodeID;

      if (tempID != -1)
      {
        cid = pid - 7 * NSP;
        cVertex[cid].newNodeID = tempID;
      }

    }

  }
}

int SurfaceMeshFunc::assign_new_nodeID(int nN)
{

  int i;
  int numN;
  int newnid;
  int nkind;
  int tid;

  numN = 7 * 2 * NSP;
  newnid = nN;

  for (i = 0; i < numN; i++)
  {

    nkind = cVertex[i].nodeKind;
    tid = cVertex[i].newNodeID;

    if (nkind != 0 && tid == -1)
    {
      cVertex[i].newNodeID = newnid;
      newnid++;
    }
  }

  return (newnid);

}

void SurfaceMeshFunc::update_face_edges(int nfe)
{

  int i, j;
  int oldID, newID;

  for (i = 0; i < nfe; i++)
  {

    for (j = 0; j < 2; j++)
    {
      oldID = cFedge[i].node_id[j];
      newID = cVertex[oldID].newNodeID;
      cFedge[i].new_n_id[j] = newID;
    }
  }

}

void SurfaceMeshFunc::update_inner_edges(int tnie, int nie)
{

  int i, j;
  int oldID, newID;
  int count;
  int uflag;

  count = 0;

  for (i = 0; i < tnie; i++)
  {

    uflag = cIedge[i].eff;

    if (uflag == 1)
    {

      for (j = 0; j < 2; j++)
      {
        oldID = cIedge[i].n_id[j];
        newID = cVertex[oldID].newNodeID;
        cIedge[i].new_n_id[j] = newID;
      }

      count++;
    }
  }

  if (count != nie)
  {
    printf("\t\tSomething's wrong in counting unique inner edges!!!\n");
  }

}

void SurfaceMeshFunc::update_current_triangles(int nT)
{

  int i, j;
  int oldID, newID;

  for (i = 0; i < nT; i++)
  {

    for (j = 0; j < 3; j++)
    {

      oldID = cTriangle[i].v_id[j];
      newID = cVertex[oldID].newNodeID;
      cTriangle[i].new_v_id[j] = newID;
    }
  }

}

void SurfaceMeshFunc::get_output_nodes(int zID, string NodesRawFile)
{

  int index;

  int i, j, k;
  int finish;
  int tID;
  int nk;
  int czid;
  double x, y, z;

  int start, end;
  int count;

  vector<int> order;

  end = -1;
  count = 0;

  czid = zID;

  if (czid == (zDim - 2))
  {
    finish = 7 * 2 * NSP;
  }
  else
  {
    finish = 7 * NSP;
  }

  // counting number of nodes to be output...
  for (j = 0; j < finish; j++)
  {
    tID = cVertex[j].newNodeID;
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

  tempcVertex = new Node[count];
//  order.resize(count);
  // rearrange the order...
  for (k = 0; k < finish; k++)
  {
    tID = cVertex[k].newNodeID;
    nk = cVertex[k].nodeKind;
    x = cVertex[k].xc;
    y = cVertex[k].yc;
    z = cVertex[k].zc;
    if (tID != -1)
    {
      index = tID - start;
//      order[index] = k;
      tempcVertex[index].newNodeID = tID;
      tempcVertex[index].nodeKind = nk;
      tempcVertex[index].xc = x;
      tempcVertex[index].yc = y;
      tempcVertex[index].zc = z;
    }
  }

ofstream outFile;
outFile.open(NodesRawFile.c_str(), ios::app);

  for (i = 0; i < count; i++)
  {
//       int cur = order[i];
//	   tID = cVertex[cur].newNodeID;
//	   nk = cVertex[cur].nodeKind;
//	   x = cVertex[cur].xc;
//	   y = cVertex[cur].yc;
//	   z = cVertex[cur].zc;
    tID = tempcVertex[i].newNodeID;
    nk= tempcVertex[i].nodeKind;
    x = tempcVertex[i].xc;
    y = tempcVertex[i].yc;
    z = tempcVertex[i].zc;

    outFile << tID << "	" << czid << "	" << nk << "	" << x << "	" << y << "	" << z << endl;
  }

  outFile.close();
  if(count > 0) delete [] tempcVertex;
}

void SurfaceMeshFunc::get_output_edges(int nfe, int tnie, int zID, int ceid, int *feid, string EdgesFileIndex)
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

  std::string ZIndex;
  std::string extension = ".txt";

  string EdgesFileIndexCur;
  EdgesFileIndexCur = EdgesFileIndex;
  ZIndex = StringUtils::numToString(zID);

  EdgesFileIndexCur.append(ZIndex);
  EdgesFileIndexCur.append(extension);

  ofstream outFile;
  outFile.open(EdgesFileIndexCur.c_str());

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
    flag1 = cFedge[ii].where;
    if (flag1 < flagLimit)
    {
      count++;
    }
  }

  for (jj = 0; jj < tnie; jj++)
  {
    flag2 = cIedge[jj].eff;
    if (flag2 == 1)
    {
      count++;
    }
  }




  outFile << count << endl;

  for (i = 0; i < nfe; i++)
  {

    flag1 = cFedge[i].where;

    if (flag1 < flagLimit)
    {
      n1 = cFedge[i].new_n_id[0];
      n2 = cFedge[i].new_n_id[1];
      sk = cFedge[i].segKind;
      s1 = cFedge[i].neigh_grainname[0];
      s2 = cFedge[i].neigh_grainname[1];
      outFile << index << "	" << czid << "	" << 0 << "	" << n1 << "	" << n2 << "	" << sk << "	" << s1 << "	" << s2 << "	" << dummy << "	" << dummy << endl;
      index++;
    }
  }

  for (j = 0; j < tnie; j++)
  {

    flag2 = cIedge[j].eff;

    if (flag2 == 1)
    {
      n1 = cIedge[j].new_n_id[0];
      n2 = cIedge[j].new_n_id[1];
      ek = cIedge[j].edgeKind;
      s1 = cIedge[j].ngrainname[0];
      s2 = cIedge[j].ngrainname[1];
      s3 = cIedge[j].ngrainname[2];
      s4 = cIedge[j].ngrainname[3];
      outFile << index << "	" << czid << "	" << 1 << "	" << n1 << "	" << n2 << "	" << ek << "	" << s1 << "	" << s2 << "	" << s3 << "	" << s4 << endl;
      index++;
    }
  }

  outFile.close();
  if(nfe > 0) delete [] cFedge;
  if(tnie > 0) delete [] cIedge;
  *feid = index;
}

void SurfaceMeshFunc::get_output_triangles (int nt, string TrianglesFileIndex, int zID, int ctid)
{
  int i;
  int tag;
  int end;
  int newID;

  int n1, n2, n3, s1, s2;

  tag = zID;
  end = nt;
  newID = ctid;

  std::string ZIndex;
  std::string extension = ".txt";

  string TrianglesFileIndexCur;
  TrianglesFileIndexCur = TrianglesFileIndex;

  ZIndex = StringUtils::numToString(zID);
  TrianglesFileIndexCur.append(ZIndex);
  TrianglesFileIndexCur.append(extension);

  ofstream outFile;
  outFile.open(TrianglesFileIndexCur.c_str());

  outFile << nt <<endl;

  for(i=0; i<end; i++){

    n1 = cTriangle[i].new_v_id[0];
    n2 = cTriangle[i].new_v_id[1];
    n3 = cTriangle[i].new_v_id[2];
    s1 = cTriangle[i].ngrainname[0];
    s2 = cTriangle[i].ngrainname[1];

    outFile << newID << "	" << tag << "	" << n1 << "	" << n2 << "	" << n3 << "	" << s1 << "	" << s2 << endl;

    newID++;
  }

  outFile.close();
  if(end > 0) delete [] cTriangle;
}

void SurfaceMeshFunc::copy_cNodes_2_pNodes()
{

  int i, j;

  int end;
  int nid;

  int tempID;
  int nk;

  end = 2 * NSP;

  for (i = 1; i <= end; i++)
  {

    for (j = 0; j < 7; j++)
    {

      nid = (i - 1) * 7 + j;
      tempID = cVertex[nid].newNodeID;
      nk = cVertex[nid].nodeKind;

      pVertex[nid].newNodeID = tempID;
      pVertex[nid].nodeKind = nk;
    }

  }
}
void SurfaceMeshFunc::UET_get_number_current_edges (int *nfe, int *nie, int zID, string EdgesFileIndex)
{
  int i;

  int eid;
  int ek;
  int feFlag, ieFlag;
  int tnfe, tnie;
  int layerIndex, kindIndex;
  int n1, n2;
  int s1, s2, s3, s4;

  int tne;

  std::string ZIndex;
  std::string extension = ".txt";

  string EdgesFileIndexCur;
  EdgesFileIndexCur = EdgesFileIndex;
  ZIndex = StringUtils::numToString(zID);
  EdgesFileIndexCur.append(ZIndex);
  EdgesFileIndexCur.append(extension);

  ifstream inputFile;
  inputFile.open(EdgesFileIndexCur.c_str());

  feFlag = 0;
  ieFlag = 1;

  tnfe = 0;
  tnie = 0;


  inputFile >> tne;

  for(i=0; i<tne; i++){

    inputFile >> eid >> layerIndex >> kindIndex >> n1 >> n2 >> ek >> s1 >> s2 >> s3 >> s4;

    if(kindIndex==feFlag){
      tnfe++;
    }else if(kindIndex==ieFlag){
      tnie++;
    }else{
      // do nothing...
    }

  }

  inputFile.close();

  *nfe = tnfe;
  *nie = tnie;

}
void SurfaceMeshFunc::UET_read_current_edges (int nFEdge, int nIEdge, int zID, string EdgesFileIndex)
{

  int i;

  int feid;
  int ieid;

  int eid;
  int ek;
  int feFlag, ieFlag;
  int layerIndex, kindIndex;
  int n1, n2;
  int s1, s2, s3, s4;

  int tne;

  std::string ZIndex;
  std::string extension = ".txt";

  string EdgesFileIndexCur;
  EdgesFileIndexCur = EdgesFileIndex;
  ZIndex = StringUtils::numToString(zID);
  EdgesFileIndexCur.append(ZIndex);
  EdgesFileIndexCur.append(extension);

  ifstream inputFile;
  inputFile.open(EdgesFileIndexCur.c_str());

  feFlag = 0;
  ieFlag = 1;

  feid = 0;
  ieid = 0;

  inputFile >> tne;

  Fedge = new Segment[nFEdge];
  Iedge = new Segment[nIEdge];

  for(i=0; i<tne; i++){

    inputFile >> eid >> layerIndex >> kindIndex >> n1 >> n2 >> ek >> s1 >> s2 >> s3 >> s4;

    if(kindIndex==feFlag){
      Fedge[feid].edgeID = eid;
      Fedge[feid].n_id[0] = n1;
      Fedge[feid].n_id[1] = n2;
      Fedge[feid].edgeKind = ek;
      Fedge[feid].nSpin[0] = s1;
      Fedge[feid].nSpin[1] = s2;
      Fedge[feid].nSpin[2] = s3;
      Fedge[feid].nSpin[3] = s4;
      feid++;
    }else if(kindIndex==ieFlag){
      Iedge[ieid].edgeID = eid;
      Iedge[ieid].n_id[0] = n1;
      Iedge[ieid].n_id[1] = n2;
      Iedge[ieid].edgeKind = ek;
      Iedge[ieid].nSpin[0] = s1;
      Iedge[ieid].nSpin[1] = s2;
      Iedge[ieid].nSpin[2] = s3;
      Iedge[ieid].nSpin[3] = s4;
      ieid++;
    }else{
      // do nothing...
    }

  }

  inputFile.close();
}
void SurfaceMeshFunc::UET_get_number_previous_current_triangles(int *nPT, int *nCT, int zID, string TrianglesFileIndex)
{

  int tnpt, tnct;

  std::string ZIndex;
  std::string extension = ".txt";

  string TrianglesFileIndexCur1;
  TrianglesFileIndexCur1 = TrianglesFileIndex;
  ZIndex = StringUtils::numToString(zID);
  TrianglesFileIndexCur1.append(ZIndex);
  TrianglesFileIndexCur1.append(extension);

  ifstream inputFile;
  inputFile.open(TrianglesFileIndexCur1.c_str());

  inputFile >> tnct;

  inputFile.close();

  if (zID > 0)
  {
    string TrianglesFileIndexCur2;
    TrianglesFileIndexCur2 = TrianglesFileIndex;
    ZIndex = StringUtils::numToString(zID-1);
    TrianglesFileIndexCur2.append(ZIndex);
    TrianglesFileIndexCur2.append(extension);

    ifstream inputFile2;
    inputFile2.open(TrianglesFileIndexCur2.c_str());
    inputFile2 >> tnpt;

    inputFile.close();

  }
  else
  {
    tnpt = 0;
  }

  *nPT = tnpt;
  *nCT = tnct;
}



void SurfaceMeshFunc::UET_read_current_triangles (int ncTriangle, int zID, string TrianglesFileIndex)
{

  int i;

  //int cFlag;
  int tid;
  int index;
  int n1, n2, n3, s1, s2;
  int cid;

  int tnt;

 // cFlag = zID;
  cid = 0;

  std::string ZIndex;
  std::string extension = ".txt";

  string TrianglesFileIndexCur;
  TrianglesFileIndexCur = TrianglesFileIndex;
  ZIndex = StringUtils::numToString(zID);
  TrianglesFileIndexCur.append(ZIndex);
  TrianglesFileIndexCur.append(extension);

  ifstream inputFile;
  inputFile.open(TrianglesFileIndexCur.c_str());

  inputFile >> tnt;

  cTriangle = new Patch [ncTriangle];


  for(i=0; i<tnt; i++){

    inputFile >> tid >> index >> n1 >> n2 >> n3 >> s1 >> s2;

    cTriangle[cid].triID = tid;
    cTriangle[cid].v_id[0] = n1;
    cTriangle[cid].v_id[1] = n2;
    cTriangle[cid].v_id[2] = n3;
    cTriangle[cid].e_id[0] = -1;
    cTriangle[cid].e_id[1] = -1;
    cTriangle[cid].e_id[2] = -1;
    cTriangle[cid].nSpin[0] = s1;
    cTriangle[cid].nSpin[1] = s2;
    cid++;
  }

  inputFile.close();
}
void SurfaceMeshFunc::UET_update_iEdges_triangles (int nIEdge, int ncTriangle)
{

  int i, j, k;

  int cn1, cn2, nn1, nn2;
  int teid;
  int tSpin[2];

  int type = 0;
  int index1, index2;
  int bflag;

  for(i=0; i<nIEdge; i++)
  {

    cn1 = Iedge[i].n_id[0];
    cn2 = Iedge[i].n_id[1];
    teid = Iedge[i].edgeID;

    for(j=0; j<ncTriangle; j++)
	{

      for(k=0; k<3; k++)
	  {  // for each edge of the current triangle...

		bflag = cTriangle[j].e_id[k];

		if(bflag==-1)
		{
		  index1 = k;
		  if(index1==2)
		  {
			  index2 = 0;
		  }
		  else
		  {
			index2 = k + 1;
		  }

		  // Find nodes...
		  nn1 = cTriangle[j].v_id[index1];
		  nn2 = cTriangle[j].v_id[index2];
		  if( (cn1==nn1 && cn2==nn2) || (cn1==nn2 && cn2==nn1))
		  {
			cTriangle[j].e_id[k] = teid;
			tSpin[0] = cTriangle[j].nSpin[0];
			tSpin[1] = cTriangle[j].nSpin[1];
			UET_update_edge_neigh_spins (type, i, tSpin);
		  }
	     }
        }
      }
    }
}
void SurfaceMeshFunc::UET_update_fEdges_triangles (int nFEdge, int npTriangle, int ncTriangle, int zID){

  int i, j, k, ii, jj, kk;

  int cn1, cn2, nn1, nn2;
  int teid;
  int tSpin[2];

  int index1, index2;
  int bflag;

  int type = 1;
  int zFlag;
  zFlag = zID;

  for(i=0; i<nFEdge; i++){

    cn1 = Fedge[i].n_id[0];
    cn2 = Fedge[i].n_id[1];
    teid = Fedge[i].edgeID;

    for(j=0; j<ncTriangle; j++){

      for(k=0; k<3; k++){  // for each edge of the current triangle...

	bflag = cTriangle[j].e_id[k];

	if(bflag==-1){

	  index1 = k;

	  if(index1==2){
	    index2 = 0;
	  }else{
	    index2 = k + 1;
	  }

	  // Find nodes...
	  nn1 = cTriangle[j].v_id[index1];
	  nn2 = cTriangle[j].v_id[index2];

	  if( (cn1==nn1 && cn2==nn2) || (cn1==nn2 && cn2==nn1)){
	    cTriangle[j].e_id[k] = teid;
	    tSpin[0] = cTriangle[j].nSpin[0];
	    tSpin[1] = cTriangle[j].nSpin[1];
	    UET_update_edge_neigh_spins (type, i, tSpin);
	  }

	}

      }
    }

  }


  if(zFlag>0){

    for(ii=0; ii<nFEdge; ii++){

      cn1 = Fedge[ii].n_id[0];
      cn2 = Fedge[ii].n_id[1];
      teid = Fedge[ii].edgeID;

      for(jj=0; jj<npTriangle; jj++){

	for(kk=0; kk<3; kk++){  // for each edge of the current triangle...

	  bflag = pTriangle[jj].e_id[kk];

	  if(bflag==-1){

	    index1 = kk;

	    if(index1==2){
	      index2 = 0;
	    }else{
	      index2 = kk + 1;
	    }

	    // Find nodes...
	    nn1 = pTriangle[jj].v_id[index1];
	    nn2 = pTriangle[jj].v_id[index2];

	    if( (cn1==nn1 && cn2==nn2) || (cn1==nn2 && cn2==nn1)){
	      pTriangle[jj].e_id[kk] = teid;
	      tSpin[0] = pTriangle[jj].nSpin[0];
	      tSpin[1] = pTriangle[jj].nSpin[1];
	      UET_update_edge_neigh_spins (type, ii, tSpin);
	    }

	  }

	}
      }
    }

  }

}



void SurfaceMeshFunc::UET_update_edge_neigh_spins (int type, int eid, int tsp[2]){

  int i;
  int index;
  int numCN, newCN1, newCN2;

  int cns;
  int nns1, nns2;

  index = eid;
  nns1 = tsp[0];
  nns2 = tsp[1];
  if(type == 0)
  {
	  numCN = Iedge[index].edgeKind;
  }
  if(type == 1)
  {
	  numCN = Fedge[index].edgeKind;
  }
  newCN1 = 0;
  newCN2 = 0;

  for(i=0; i<numCN; i++){   // remember nns1 is different from nns2...

    if(type == 0) cns = Iedge[index].nSpin[i];
    if(type == 1) cns = Fedge[index].nSpin[i];

    if(cns==nns1){
      newCN1++;
    }

    if(cns==nns2){
      newCN2++;
    }
  }

  if(newCN1==0)
  {
    if(type == 0) Iedge[index].nSpin[numCN] = nns1;
    if(type == 1) Fedge[index].nSpin[numCN] = nns1;
    numCN++;
  }

  if(newCN2==0){
    if(type == 0) Iedge[index].nSpin[numCN] = nns2;
    if(type == 1) Fedge[index].nSpin[numCN] = nns2;
    numCN++;
  }

  if(type == 0) Iedge[index].edgeKind = numCN;
  if(type == 1) Fedge[index].edgeKind = numCN;

}
void SurfaceMeshFunc::UET_copy_triangles (int npTriangle)
{

  int i;

  pTriangle = new Patch[npTriangle];

  for(i=0; i<npTriangle; i++){

    pTriangle[i].triID = cTriangle[i].triID;
    pTriangle[i].v_id[0] = cTriangle[i].v_id[0];
    pTriangle[i].v_id[1] = cTriangle[i].v_id[1];
    pTriangle[i].v_id[2] = cTriangle[i].v_id[2];
    pTriangle[i].e_id[0] = cTriangle[i].e_id[0];
    pTriangle[i].e_id[1] = cTriangle[i].e_id[1];
    pTriangle[i].e_id[2] = cTriangle[i].e_id[2];
    pTriangle[i].nSpin[0] = cTriangle[i].nSpin[0];
    pTriangle[i].nSpin[1] = cTriangle[i].nSpin[1];
  }

}
void SurfaceMeshFunc::UET_get_output_edges (int nFEdge, int nIEdge, string EdgesFile)
{

  int i, j;

  ofstream outFile;
  outFile.open(EdgesFile.c_str(), ios::app);

  for(i=0; i<nFEdge; i++)
  {
      outFile << Fedge[i].edgeID << "	" << Fedge[i].n_id[0] << "	" << Fedge[i].n_id[1] << "	" << Fedge[i].edgeKind << "	" << Fedge[i].nSpin[0] << "	" << Fedge[i].nSpin[1] << "	" << Fedge[i].nSpin[2] << "	" << Fedge[i].nSpin[3] << endl;
  }

  for(j=0; j<nIEdge; j++)
  {
	  outFile << Iedge[j].edgeID << "	" << Iedge[j].n_id[0] << "	" << Iedge[j].n_id[1] << "	" << Iedge[j].edgeKind << "	" << Iedge[j].nSpin[0] << "	" << Iedge[j].nSpin[1] << "	" << Iedge[j].nSpin[2] << "	" << Iedge[j].nSpin[3] << endl;
  }

  outFile.close();
}
void SurfaceMeshFunc::UET_get_output_triangles (int type, int nTriangle, string TrianglesFile)
{

  int i;

  ofstream outFile;
  outFile.open(TrianglesFile.c_str(), ios::app);

  for(i=0; i<nTriangle; i++)
  {
    if(type == 0)
	{
	  outFile << pTriangle[i].triID << "	" << pTriangle[i].v_id[0] << "	" << pTriangle[i].v_id[1] << "	" << pTriangle[i].v_id[2] << "	" << pTriangle[i].e_id[0] << "	" << pTriangle[i].e_id[1] << "	" << pTriangle[i].e_id[2] << "	" << pTriangle[i].nSpin[0] << "	" << pTriangle[i].nSpin[1] << endl;
	}
    if(type == 1)
	{
	  outFile << cTriangle[i].triID << "	" << cTriangle[i].v_id[0] << "	" << cTriangle[i].v_id[1] << "	" << cTriangle[i].v_id[2] << "	" << cTriangle[i].e_id[0] << "	" << cTriangle[i].e_id[1] << "	" << cTriangle[i].e_id[2] << "	" << cTriangle[i].nSpin[0] << "	" << cTriangle[i].nSpin[1] << endl;
	}
  }

  outFile.close();
}
void SurfaceMeshFunc::UET_get_output_nodes (int nNodes, string NodesFile, string NodesRawFile)
{

  int i;

  int id, dummy, nk;
  double x, y, z;

  ifstream inputFile;
  inputFile.open(NodesRawFile.c_str());
  ofstream outFile;
  outFile.open(NodesFile.c_str(), ios::app);

  for(i=0; i<nNodes; i++)
  {

    inputFile >> id >> dummy >> nk >> x >> y >> z;
    outFile << id << "	" << nk << "	" << x << "	" << y << "	" << z << endl;
  }

  inputFile.close();
  outFile.close();
}
void SurfaceMeshFunc::UET_create_vtk (string VisualizationFile, string NodesFile, string TrianglesFile)
{
	ofstream outFile;
	outFile.open(VisualizationFile.c_str(), ios::app);

	ifstream inputFile1;
	inputFile1.open(NodesFile.c_str());

	ifstream inputFile2;
	inputFile2.open(TrianglesFile.c_str());

	int numnodes=0;
	int numtriangles=0;

	inputFile1 >> numnodes;
	inputFile2 >> numtriangles;

	outFile << "# vtk DataFile Version 2.0" << endl;
	outFile << "data set from FFT2dx_GB" << endl;
	outFile << "ASCII" << endl;
	outFile << "DATASET UNSTRUCTURED_GRID" << endl;
	outFile << endl;
	outFile << "POINTS " << numnodes << " float" << endl;

	int nodenum;
	int nodetype;
	double x;
	double y;
	double z;
	for(int i=0;i<numnodes;i++)
	{
		inputFile1 >> nodenum >> nodetype >> x >> y >> z;
		outFile << x << "	" << y << "	" << z << endl;
	}
	inputFile1.close();

	int trianglenum;
	int node1;
	int node2;
	int node3;
	int edge1;
	int edge2;
	int edge3;
	int grain1;
	int grain2;
	outFile << endl;
	outFile << "CELLS " << 2*numtriangles << " " << (2*numtriangles*4) << endl;
	for(int i=0;i<numtriangles;i++)
	{
		inputFile2 >> trianglenum >> node1 >> node2 >> node3 >> edge1 >> edge2 >> edge3 >> grain1 >> grain2;
		outFile << "3	" << node1 << "	" << node2 << "	" << node3 << endl;
		outFile << "3	" << node1 << "	" << node2 << "	" << node3 << endl;
	}
	inputFile2.close();

	outFile << endl;
	outFile << "CELL_TYPES " << 2*numtriangles << endl;
	for(int i=0;i<numtriangles;i++)
	{
		outFile << "5" << endl;
		outFile << "5" << endl;
	}

	inputFile2.open(TrianglesFile.c_str());
	inputFile2 >> numtriangles;
	outFile << endl;
	outFile << "CELL_DATA " << 2*numtriangles << endl;
	outFile << "SCALARS GrainID float" << endl;
	outFile << "LOOKUP_TABLE default" << endl;
	for(int i=0;i<numtriangles;i++)
	{
		inputFile2 >> trianglenum >> node1 >> node2 >> node3 >> edge1 >> edge2 >> edge3 >> grain1 >> grain2;
		outFile << grain1 << endl;
		outFile << grain2 << endl;
	}
	inputFile2.close();
	outFile.close();
}
