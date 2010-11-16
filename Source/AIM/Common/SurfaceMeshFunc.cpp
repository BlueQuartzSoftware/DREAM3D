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
  cSquare(NULL)
{

}

SurfaceMeshFunc::~SurfaceMeshFunc()
{
  delete []neigh;
  delete []point;
  delete []cSquare;
}

int SurfaceMeshFunc::initialize_micro(string filename, int zID)
{
	int i, j, k, l;
	int id;
	int tgrainname;
	int surfacevoxel;
	double tempx, tempy, tempz;
	int xnum, ynum, znum;
	double xres, yres, zres;
	int col, row, plane;
	const unsigned int size ( 1024 );
	char buf [ size ];
	std::string word;
	if(zID == -1)
	{
		in.open(filename.c_str() );
		bool headerdone = false;
		while(headerdone == false)
		{
			in.getline( buf, size );
			std::string line = buf;
			in >> word;
			if(word == "LOOKUP_TABLE")
			{
				headerdone = true;
				in >> word;
			}
			if(word == "DIMENSIONS")
			{
				in >> xnum >> ynum >> znum;
				NS = xnum * ynum * znum;
				NSP = xnum * ynum;
				xDim = xnum;
				yDim = ynum;
				zDim = znum;
				neigh = new Neighbor[2 * NSP + 1];
				point = new Voxel[2*NSP + 1];
				cSquare = new Face[3 * 2 * NSP];
				cVertex = new Node[2*7*NSP];
			}
			if(word == "SPACING")
			{
				in >> xres >> yres >> zres;
				xRes = xres;
				yRes = yres;
				zRes = zres;
			}
		}
	}
	if(zID >= 0)
	{
		int shift = (zID*NSP);
		int start = NSP+1;
		if(zID == 0) start = 1;
		if(zID > 0)
		{
		  for (i = 1; i <= NSP; i++)
		  {
			point[i] = point[i+NSP];
		  }
		}
		for(i=start;i<=(2*NSP);i++)
		{
			in >> tgrainname;
			if(tgrainname <= 0) tgrainname = -3;
			col = (i+shift-1)%xDim;
			row = ((i+shift-1)/xDim)%yDim;
			plane = (i+shift-1)/(xDim*yDim);
			point[i].grainname = tgrainname;
			if(col == 0 || col == (xDim-1) || row == 0 || row == (yDim-1) || plane == 0 || plane == (zDim-1)) point[i].grainname = -3;
		}
		point[0].grainname = 0; // Point 0 is a garbage...
	}
//	in.close();
	return zDim;
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
    site_id = ii;
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
  int i, j;
  int id, oid;
  int tsite, locale;
  double x, y, z;
  int start = NSP+1;
  if(zID == 0) start = 1;
  // node id starts with 0....
  if(zID > 0)
  {
	  for (i = 1; i <= NSP; i++)
	  {
	    id = 7 * (i - 1);
		oid = 7 * ((i+NSP) - 1);
		for(j=0;j<7;j++)
		{
			cVertex[id+j] = cVertex[oid+j];
		}
	  }
  }
  int shift = (zID*NSP);
  for (i = start; i <= 2 * NSP; i++)
  {
    id = 7 * (i - 1);
    tsite = i;
	locale = tsite+shift;
    x = find_xcoord(locale);
    y = find_ycoord(locale);
    z = find_zcoord(locale);
	cVertex[id].xc = x + (0.5*xRes);
	cVertex[id].yc = y;
	cVertex[id].zc = z;
	cVertex[id].nodeKind = 0;
	cVertex[id].NodeID = -1;
	cVertex[id].point = tsite;
	cVertex[id+1].xc = x;
	cVertex[id+1].yc = y + (0.5*yRes);
	cVertex[id+1].zc = z;
	cVertex[id+1].nodeKind = 0;
	cVertex[id+1].NodeID = -1;
	cVertex[id+1].point = tsite;
	cVertex[id+2].xc = x;
	cVertex[id+2].yc = y;
	cVertex[id+2].zc = z + (0.5*zRes);
	cVertex[id+2].nodeKind = 0;
	cVertex[id+2].NodeID = -1;
	cVertex[id+2].point = tsite;
	cVertex[id+3].xc = x + (0.5*xRes);
	cVertex[id+3].yc = y + (0.5*yRes);
	cVertex[id+3].zc = z;
	cVertex[id+3].nodeKind = 0;
	cVertex[id+3].NodeID = -1;
	cVertex[id+3].point = tsite;
	cVertex[id+4].xc = x + (0.5*xRes);
	cVertex[id+4].yc = y;
	cVertex[id+4].zc = z + (0.5*zRes);
	cVertex[id+4].nodeKind = 0;
	cVertex[id+4].NodeID = -1;
	cVertex[id+4].point = tsite;
	cVertex[id+5].xc = x;
	cVertex[id+5].yc = y + (0.5*yRes);
	cVertex[id+5].zc = z + (0.5*zRes);
	cVertex[id+5].nodeKind = 0;
	cVertex[id+5].NodeID = -1;
	cVertex[id+5].point = tsite;
	cVertex[id+6].xc = x + (0.5*xRes);
	cVertex[id+6].yc = y + (0.5*yRes);
	cVertex[id+6].zc = z + (0.5*zRes);
	cVertex[id+6].nodeKind = 0;
	cVertex[id+6].NodeID = -1;
	cVertex[id+6].point = tsite;
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
  int i, j;
  int csite;
  // square id starts with 0....
  // notice that voxels at the surface will have the wrong values of node at the other end...
  // since it includes periodic boundary condition...
  // but, since the structure surrounded by ghost layer of grainname -3, it's OK...
  for (i = 1; i <= 2 * NSP; i++)
  {
    id = 3 * (i - 1);
    csite = i;
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
int SurfaceMeshFunc::get_number_Edges(int zID)
{
  int  k, m;
  int csite, cgrainname, tsite;
  int tNSite[4];
  int tngrainname[4];
  int atBulk; // if it's 0, the square is at Bulk...
  int sqIndex;
  int anFlag;
  int quot, rmd;
  int sumEdge;
  int numCEdge;
  sumEdge = 0;
  for (k = 0; k < (3 * 2 * NSP); k++)
  { // for each square on upper layer...
    quot = k / (3 * NSP);
    rmd = k % 3;
    if (quot == 0 || (quot == 1 && rmd == 0))
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
      // Let's count the number of edges...
      if (atBulk > 0)
      { // coNSider the square iNSide the bulk only..
	    sqIndex = 0;
		sqIndex = get_square_index(tngrainname);
		cSquare[k].effect = 1;
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

void SurfaceMeshFunc::get_nodes_Edges(int eT2d[20][8], int NST2d[20][8], int zID, int nedge)
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
  cEdge = new Segment[nedge];
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
			  if(pixgrainname[0] > 0 || pixgrainname[1] > 0)
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
					}
					else
					{
					  tnode = nodeID[ii];
					  cVertex[tnode].nodeKind = 2;
					}
				  }
				  cEdge[eid].node_id[0] = tnode1; // actual node ids for each edge...
				  cEdge[eid].node_id[1] = tnode2;
				  cEdge[eid].neigh_grainname[0] = pixgrainname[0];
				  cEdge[eid].neigh_grainname[1] = pixgrainname[1];
				  cEdge[eid].edgeKind = 2; // edges of the open loops are always binary...
				  // triple lines only occurs iNSide the marching cubes...
				  // mark where each face edge belong to, upper, middle or bottom of the layer...
				  if (quot == 0)
				  {
					if (rmd1 == 0)
					{
					  cEdge[eid].where = 0;
					}
					else
					{
					  cEdge[eid].where = 1;
					}
				  }
				  else
				  { //when quot==1
					if (rmd1 == 0)
					{
					  cEdge[eid].where = 2;
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
    }
  }
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
    cid = csite;
    cgrainname = point[csite].grainname;
    for (j = 1; j <= num_neigh; j++)
    {
		NSite = neigh[cid].neigh_id[j];
		if(NSite <= 0 || NSite > (2*NSP)) ngrainname = -3;
		if(NSite > 0 && NSite <= (2*NSP)) ngrainname = point[NSite].grainname;
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
  int tnE; // temp number of edges...
  int nFC; // number of FC turned on...
  int tFC;
  int nE; // number of face edges...
  int eff; // all the squares effective?...
  int cubeFlag; // if 1, we can do marching cube; if 0, useless...
  int *arrayE;
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
    nE = 0;
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
      nE = nE + cSquare[tsq].nEdge;
      eff = eff + cSquare[tsq].effect;
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
	  cVertex[BCnode].nodeKind = nds + 10;
    }
    // Checking the number of edges for loops in the cube...
    // if the current marching cube is a collection of 6 effective squares...and
    // the number of face edges at least 3...
    // when nE==2, it doen't happen
    // when nE==1, the edge will contribute for the neighboring marching cube...
    // when nE==0, it meaNS the cube is iNSide a grain...
    if (cubeFlag == 1 && nE > 2)
    {
      // Make edge array for each marching cube...
      arrayE = new int[nE];
      tindex = 0;
      for (i1 = 0; i1 < 6; i1++)
      {
        tsq = sqID[i1];
        tnE = cSquare[tsq].nEdge;
        if (tnE != 0)
        {
          for (i2 = 0; i2 < tnE; i2++)
          {
            arrayE[tindex] = cSquare[tsq].edge_id[i2];
            tindex++;
          }
        }
      }
      // CoNSider each case as Z. Wu's paper...
      if (nFC == 0)
      { // when there's no face center
        nTri0 = nTri0 + get_number_case0_triangles(arrayE, nE);
      }
      else if (nFC == 2)
      {
        nTri2 = nTri2 + get_number_case2_triangles(arrayE, nE, arrayFC, nFC);
      }
      else if (nFC > 2 && nFC <= 6)
      {
        nTriM = nTriM + get_number_caseM_triangles(arrayE, nE, arrayFC, nFC);
      }
      delete [] arrayE;
    }
  }
  nTri = nTri0 + nTri2 + nTriM;
  return (nTri);
}

int SurfaceMeshFunc::get_number_case0_triangles(int *ae, int nedge)
{
  int ii, i, j, jj, k, kk, k1;
  int loopID;
  int tail, head, coin;
  int ce, ne;
  int cgrainname1, cgrainname2, cnode1, cnode2;
  int ngrainname1, ngrainname2, nnode1, nnode2;
  int nucleus, chaser;
  int grainnameFlag, nodeFlag, flip;
  int bflag, nbflag;
  int *burnt;
  int *burnt_list;
  int *count;
  int numN, numTri, tnumTri;
  int lid;
  burnt = new int[nedge];
  burnt_list = new int[nedge];
  // initialize burn flags for face edges...
  for (ii = 0; ii < nedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }
  loopID = 1;
  tail = 0;
  head = 0;
  for (i = 0; i < nedge; i++)
  {
    ce = ae[i];
    bflag = burnt[i];
    if (bflag == 0)
    {
      nucleus = ce;
      burnt[i] = loopID;
      burnt_list[tail] = ce;
	  coin = 1;
      while (coin)
      {
        chaser = burnt_list[tail];
		cgrainname1 = cEdge[chaser].neigh_grainname[0];
        cgrainname2 = cEdge[chaser].neigh_grainname[1];
		cnode1 = cEdge[chaser].node_id[0];
        cnode2 = cEdge[chaser].node_id[1];
        for (j = 0; j < nedge; j++)
        {
          ne = ae[j];
          nbflag = burnt[j];
          if (nbflag == 0)
          {
			ngrainname1 = cEdge[ne].neigh_grainname[0];
			ngrainname2 = cEdge[ne].neigh_grainname[1];
            nnode1 = cEdge[ne].node_id[0];
            nnode2 = cEdge[ne].node_id[1];
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
              burnt_list[head] = ne;
              burnt[j] = loopID;
              if (flip == 1)
              {
				cEdge[ne].node_id[0] = nnode2;
                cEdge[ne].node_id[1] = nnode1;
              }
            }
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
      }
    }
  }
  count = new int[loopID];
  for (k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }
  for (k = 1; k < loopID; k++)
  {
    for (kk = 0; kk < nedge; kk++)
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
  }
  delete [] burnt;
  delete [] burnt_list;
  delete [] count;
  return (numTri);
}

int SurfaceMeshFunc::get_number_case2_triangles(int *ae,  int nedge, int *afc, int nfctr)
{
  int ii, i, j, k, kk, k1, n, i1, j1;
  int loopID;
  int tail, head, coin;
  int ce, ne;
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
  burnt = new int[nedge];
  burnt_list = new int[nedge];
  // initialize burn flags for face edges...
  for (ii = 0; ii < nedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }
  loopID = 1;
  tail = 0;
  head = 0;
  for (i = 0; i < nedge; i++)
  {
    ce = ae[i];
    bflag = burnt[i];
    if (bflag == 0)
    {
      nucleus = ce;
      burnt[i] = loopID;
      burnt_list[tail] = ce;
	  coin = 1;
      while (coin)
      {
        chaser = burnt_list[tail];
		cgrainname1 = cEdge[chaser].neigh_grainname[0];
        cgrainname2 = cEdge[chaser].neigh_grainname[1];
		cnode1 = cEdge[chaser].node_id[0];
        cnode2 = cEdge[chaser].node_id[1];
        for (j = 0; j < nedge; j++)
        {
          ne = ae[j];
          nbflag = burnt[j];
          if (nbflag == 0)
          {
			ngrainname1 = cEdge[ne].neigh_grainname[0];
			ngrainname2 = cEdge[ne].neigh_grainname[1];
            nnode1 = cEdge[ne].node_id[0];
            nnode2 = cEdge[ne].node_id[1];
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
              burnt_list[head] = ne;
              burnt[j] = loopID;
            }
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
      }
    }
  }
  count = new int[loopID];
  for (k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }
  for (k = 1; k < loopID; k++)
  {
    for (kk = 0; kk < nedge; kk++)
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
    to = to + numN;
    from = to - numN;
    burnt_loop = new int[numN + 2];
    for (i1 = from; i1 < to; i1++)
    {
      ce = burnt_list[i1];
	  cnode1 = cEdge[ce].node_id[0];
      cnode2 = cEdge[ce].node_id[1];
      if (start == cnode1)
      {
        openL = 1; // loop is open...
        startEdge = ce;
        flip = 0;
      }
      else if (start == cnode2)
      {
        openL = 1;
        startEdge = ce;
        flip = 1;
      }
    }
    if (openL == 1)
    { // if current loop is an open loop...
      if (flip == 1)
      {
        tnode = cEdge[startEdge].node_id[0];
        tgrainname = cEdge[startEdge].neigh_grainname[0];
        cEdge[startEdge].node_id[0] = cEdge[startEdge].node_id[1];
        cEdge[startEdge].node_id[1] = tnode;
        cEdge[startEdge].neigh_grainname[0] = cEdge[startEdge].neigh_grainname[1];
        cEdge[startEdge].neigh_grainname[1] = tgrainname;
      }
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cEdge[startEdge].node_id[1];
      chaser = startEdge;
      while (index < numN)
      {
        for (n = from; n < to; n++)
        {
          ce = burnt_list[n];
          cnode1 = cEdge[ce].node_id[0];
          cnode2 = cEdge[ce].node_id[1];
          if ((ce != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = ce;
            index++;
          }
          else if ((ce != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = ce;
            index++;
            // flip...
            tnode = cEdge[ce].node_id[0];
            tgrainname = cEdge[ce].neigh_grainname[0];
            cEdge[ce].node_id[0] = cEdge[ce].node_id[1];
            cEdge[ce].node_id[1] = tnode;
            cEdge[ce].neigh_grainname[0] = cEdge[ce].neigh_grainname[1];
            cEdge[ce].neigh_grainname[1] = tgrainname;
          }
        }
        chaser = burnt_loop[index - 1];
        endNode = cEdge[chaser].node_id[1];
      }
      if ((numN + 1) == 3)
      {
        numTri = numTri + 1;
      }
      else if ((numN + 1) > 3)
      {
        numTri = numTri + ((numN + 1) - 2);
      }
      delete [] burnt_loop;
    }
    else
    { // if current loop is a closed one....i.e., openL = 0...
      startEdge = burnt_list[from];
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cEdge[startEdge].node_id[1];
      chaser = startEdge;
      while (index < numN)
      {
        for (n = from; n < to; n++)
        {
          ce = burnt_list[n];
          cnode1 = cEdge[ce].node_id[0];
          cnode2 = cEdge[ce].node_id[1];
          if ((ce != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = ce;
            index++;
          }
          else if ((ce != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = ce;
            index++;
            // flip...
            tnode = cEdge[ce].node_id[0];
            tgrainname = cEdge[ce].neigh_grainname[0];
            cEdge[ce].node_id[0] = cEdge[ce].node_id[1];
            cEdge[ce].node_id[1] = tnode;
            cEdge[ce].neigh_grainname[0] = cEdge[ce].neigh_grainname[1];
            cEdge[ce].neigh_grainname[1] = tgrainname;
          }
        }
        chaser = burnt_loop[index - 1];
        endNode = cEdge[chaser].node_id[1];
      }
      if (numN == 3)
      {
        numTri = numTri + 1;
      }
      else if (numN > 3)
      {
        numTri = numTri + (numN - 2);
      }
      delete [] burnt_loop;
    }
  }
  delete [] burnt;
  delete [] burnt_list;
  delete [] count;
  return (numTri);
}
int SurfaceMeshFunc::get_number_caseM_triangles(int *ae, int nedge, int *afc, int nfctr)
{
  int ii, i, j, k, kk, k1, n, i1, j1, n1;
  int loopID;
  int tail, head, coin;
  int ce, ne;
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
  int *burnt_loop;
  int openL; // if a loop is an open loop, it's 1; if closed, it's 0...
  int startEdge;
  int endNode;
  int index;
  burnt = new int[nedge];
  burnt_list = new int[nedge];
  // initialize burn flags for face edges...
  for (ii = 0; ii < nedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }
  loopID = 1;
  tail = 0;
  head = 0;
  for (i = 0; i < nedge; i++)
  {
    ce = ae[i];
    bflag = burnt[i];
    if (bflag == 0)
    {
      nucleus = ce;
      burnt[i] = loopID;
      burnt_list[tail] = ce;
	  coin = 1;
      while (coin)
      {
        chaser = burnt_list[tail];
        cgrainname1 = cEdge[chaser].neigh_grainname[0];
        cgrainname2 = cEdge[chaser].neigh_grainname[1];
        cnode1 = cEdge[chaser].node_id[0];
        cnode2 = cEdge[chaser].node_id[1];
        for (j = 0; j < nedge; j++)
        {
          ne = ae[j];
          nbflag = burnt[j];
          if (nbflag == 0)
          {
            ngrainname1 = cEdge[ne].neigh_grainname[0];
            ngrainname2 = cEdge[ne].neigh_grainname[1];
            nnode1 = cEdge[ne].node_id[0];
            nnode2 = cEdge[ne].node_id[1];
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
              burnt_list[head] = ne;
              burnt[j] = loopID;
            }
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
      }
    }
  }
  count = new int[loopID];
  for (k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }
  for (k = 1; k < loopID; k++)
  {
    for (kk = 0; kk < nedge; kk++)
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
  for (j1 = 1; j1 < loopID; j1++)
  {
    openL = 0; // current loop is closed...
    numN = count[j1];
    to = to + numN;
    from = to - numN;
    burnt_loop = new int[(numN + 2)];
    for (i1 = from; i1 < to; i1++)
    {
      ce = burnt_list[i1];
      cnode1 = cEdge[ce].node_id[0];
      cnode2 = cEdge[ce].node_id[1];
      for (n1 = 0; n1 < nfctr; n1++)
      {
        start = afc[n1];
        if (start == cnode1)
        {
          openL = 1; // loop is open...
          startEdge = ce;
          flip = 0;
        }
        else if (start == cnode2)
        {
          openL = 1;
          startEdge = ce;
          flip = 1;
        }
      }
    }
    if (openL == 1)
    { // if current loop is an open loop...
      if (flip == 1)
      {
        tnode = cEdge[startEdge].node_id[0];
        tgrainname = cEdge[startEdge].neigh_grainname[0];
        cEdge[startEdge].node_id[0] = cEdge[startEdge].node_id[1];
        cEdge[startEdge].node_id[1] = tnode;
        cEdge[startEdge].neigh_grainname[0] = cEdge[startEdge].neigh_grainname[1];
        cEdge[startEdge].neigh_grainname[1] = tgrainname;
      }
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cEdge[startEdge].node_id[1];
      chaser = startEdge;
      while (index < numN)
      {
        for (n = from; n < to; n++)
        {
          ce = burnt_list[n];
          cnode1 = cEdge[ce].node_id[0];
          cnode2 = cEdge[ce].node_id[1];
          if ((ce != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = ce;
            index++;
          }
          else if ((ce != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = ce;
            index++;
            // flip...
            tnode = cEdge[ce].node_id[0];
            tgrainname = cEdge[ce].neigh_grainname[0];
            cEdge[ce].node_id[0] = cEdge[ce].node_id[1];
            cEdge[ce].node_id[1] = tnode;
            cEdge[ce].neigh_grainname[0] = cEdge[ce].neigh_grainname[1];
            cEdge[ce].neigh_grainname[1] = tgrainname;
          }
        }
        chaser = burnt_loop[index - 1];
        endNode = cEdge[chaser].node_id[1];
      }
      if ((numN + 2) == 3)
      {
        numTri = numTri + 1;
      }
      else if ((numN + 2) > 3)
      {
        numTri = numTri + ((numN + 2) - 2);
      }
      delete [] burnt_loop;
    }
    else
    { // if current loop is a closed one....i.e., openL = 0...
      startEdge = burnt_list[from];
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cEdge[startEdge].node_id[1];
      chaser = startEdge;
      while (index < numN)
      {
        for (n = from; n < to; n++)
        {
          ce = burnt_list[n];
          cnode1 = cEdge[ce].node_id[0];
          cnode2 = cEdge[ce].node_id[1];
          if ((ce != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = ce;
            index++;
          }
          else if ((ce != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = ce;
            index++;
            // flip...
            tnode = cEdge[ce].node_id[0];
            tgrainname = cEdge[ce].neigh_grainname[0];
            cEdge[ce].node_id[0] = cEdge[ce].node_id[1];
            cEdge[ce].node_id[1] = tnode;
            cEdge[ce].neigh_grainname[0] = cEdge[ce].neigh_grainname[1];
            cEdge[ce].neigh_grainname[1] = tgrainname;
          }
        }
        chaser = burnt_loop[index - 1];
        endNode = cEdge[chaser].node_id[1];
      }
      if (numN == 3)
      {
        numTri = numTri + 1;
      }
      else if (numN > 3)
      {
        numTri = numTri + (numN - 2);
      }
      delete [] burnt_loop;
    }
  }
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
  int tnE; // temp number of edges...
  int nFC; // number of FC turned on...
  int tFC;
  int nE; // number of face edges...
  int eff; // all the squares effective?...
  int cubeFlag; // if 1, we can do marching cube; if 0, useless...
  int bodyCtr; // cube center node...
  int *arrayE;
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
    nE = 0;
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
      nE = nE + cSquare[tsq].nEdge;
      eff = eff + cSquare[tsq].effect;
    }
    if (eff > 0)
    {
      cubeFlag = 1;
    }
    // Checking the number of edges for loops in the cube...
    // if the current marching cube is a collection of 6 effective squares...and
    // the number of face edges at least 3...
    // when nE==2, it doen't happen
    // when nE==1, the edge will contribute for the neighboring marching cube...
    // when nE==0, it meaNS the cube is iNSide a grain...
    if (cubeFlag == 1 && nE > 2)
    {
      // Make edge array for each marching cube...
      arrayE = new int[nE];
      tindex = 0;
      for (i1 = 0; i1 < 6; i1++)
      {
        tsq = sqID[i1];
        tnE = cSquare[tsq].nEdge;
        if (tnE != 0)
        {
          for (i2 = 0; i2 < tnE; i2++)
          {
            arrayE[tindex] = cSquare[tsq].edge_id[i2];
            tindex++;
          }
        }
      }
      // CoNSider each case as Z. Wu's paper...
      if (nFC == 0)
      { // when there's no face center
        get_case0_triangles(arrayE, nE, tidIn, &tidOut);
        tidIn = tidOut;
      }
      else if (nFC == 2)
      {
        get_case2_triangles(arrayE, nE, arrayFC, nFC, tidIn, &tidOut);
        tidIn = tidOut;
      }
      else if (nFC > 2 && nFC <= 6)
      {
        get_caseM_triangles(arrayE, nE, arrayFC, nFC, tidIn, &tidOut, bodyCtr);
        tidIn = tidOut;
      }
      delete [] arrayE;
    }
  }
  delete [] cEdge;
  return 0;
}

void SurfaceMeshFunc::get_case0_triangles(int *ae, int nedge, int tin, int *tout)
{
  int ii, i, j, jj, k, kk, k1, mm;
  int loopID;
  int tail, head, coin;
  int ce, ne;
  int cgrainname1, cgrainname2, cnode1, cnode2;
  int ngrainname1, ngrainname2, nnode1, nnode2;
  int nucleus, chaser;
  int grainnameFlag, nodeFlag, flip;
  int bflag, nbflag;
  int *burnt;
  int *burnt_list;
  int *count;
  int numN, sumN;
  int from;
  int *loop;
  int ctid;
  int front, back;
  int te0, te1, te2, tv0, tcVertex, tv2;
  int numT, cnumT, new_node0, new_node1;
  burnt = new int[nedge];
  burnt_list = new int[nedge];
  // initialize burn flags for face edges...
  for (ii = 0; ii < nedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }
  loopID = 1;
  tail = 0;
  head = 0;
  for (i = 0; i < nedge; i++)
  {
    ce = ae[i];
    bflag = burnt[i];
    if (bflag == 0)
    {
      nucleus = ce;
      burnt[i] = loopID;
      burnt_list[tail] = ce;
	  coin = 1;
      while (coin)
      {
        chaser = burnt_list[tail];
        cgrainname1 = cEdge[chaser].neigh_grainname[0];
        cgrainname2 = cEdge[chaser].neigh_grainname[1];
        cnode1 = cEdge[chaser].node_id[0];
        cnode2 = cEdge[chaser].node_id[1];
        for (j = 0; j < nedge; j++)
        {
          ne = ae[j];
          nbflag = burnt[j];
          if (nbflag == 0)
          {
            ngrainname1 = cEdge[ne].neigh_grainname[0];
            ngrainname2 = cEdge[ne].neigh_grainname[1];
            nnode1 = cEdge[ne].node_id[0];
            nnode2 = cEdge[ne].node_id[1];
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
              burnt_list[head] = ne;
              burnt[j] = loopID;
              if (flip == 1)
              {
                //cEdge[nedge].neigh_grainname[0] = ngrainname2;
                //cEdge[nedge].neigh_grainname[1] = ngrainname1;
                cEdge[ne].node_id[0] = nnode2;
                cEdge[ne].node_id[1] = nnode1;
              }
            }
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
      }
    }
  }
  count = new int[loopID];
  for (k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }
  for (k = 1; k < loopID; k++)
  {
    for (kk = 0; kk < nedge; kk++)
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
      cTriangle[ctid].node_id[0] = cEdge[te0].node_id[0];
      cTriangle[ctid].node_id[1] = cEdge[te1].node_id[0];
      cTriangle[ctid].node_id[2] = cEdge[te2].node_id[0];
      cTriangle[ctid].ngrainname[0] = cEdge[te0].neigh_grainname[0];
      cTriangle[ctid].ngrainname[1] = cEdge[te0].neigh_grainname[1];
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
      tv0 = cEdge[te0].node_id[0];
      tcVertex = cEdge[te0].node_id[1];
      tv2 = cEdge[te1].node_id[0];
      cTriangle[ctid].node_id[0] = tv0;
      cTriangle[ctid].node_id[1] = tcVertex;
      cTriangle[ctid].node_id[2] = tv2;
      cTriangle[ctid].ngrainname[0] = cEdge[te0].neigh_grainname[0];
      cTriangle[ctid].ngrainname[1] = cEdge[te0].neigh_grainname[1];
      new_node0 = tv2;
      new_node1 = tcVertex;
      cnumT++;
      ctid++;
      while (cnumT < numT)
      {
        if ((cnumT % 2) != 0)
        {
          front = front + 1;
          ce = loop[front];
          tv0 = cEdge[ce].node_id[0];
          tcVertex = cEdge[ce].node_id[1];
          tv2 = new_node0;
          cTriangle[ctid].node_id[0] = tv0;
          cTriangle[ctid].node_id[1] = tcVertex;
          cTriangle[ctid].node_id[2] = tv2;
          cTriangle[ctid].ngrainname[0] = cEdge[ce].neigh_grainname[0];
          cTriangle[ctid].ngrainname[1] = cEdge[ce].neigh_grainname[1];
          new_node0 = tcVertex;
          cnumT++;
          ctid++;
        }
        else
        {
          back = back - 1;
          ce = loop[back];
          tv0 = cEdge[ce].node_id[0];
          tcVertex = cEdge[ce].node_id[1];
          tv2 = new_node0;
          cTriangle[ctid].node_id[0] = tv0;
          cTriangle[ctid].node_id[1] = tcVertex;
          cTriangle[ctid].node_id[2] = tv2;
          cTriangle[ctid].ngrainname[0] = cEdge[ce].neigh_grainname[0];
          cTriangle[ctid].ngrainname[1] = cEdge[ce].neigh_grainname[1];
          new_node0 = tv0;
          cnumT++;
          ctid++;
        }
      }
    }
    delete [] loop;
  }
  *tout = ctid;
  delete [] burnt;
  delete [] burnt_list;
  delete [] count;
}

void SurfaceMeshFunc::get_case2_triangles(int *ae, int nedge, int *afc, int nfctr, int tin, int *tout)
{
  int ii, i, j, k, kk, k1, n, i1, j1;
  int loopID;
  int tail, head, coin;
  int ce, ne;
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
  int ctid;
  int front, back;
  int te0, te1, te2, tv0, tcVertex, tv2;
  int numT, cnumT, new_node0, new_node1;
  burnt = new int[nedge];
  burnt_list = new int[nedge];
  // initialize burn flags for face edges...
  for (ii = 0; ii < nedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }
  loopID = 1;
  tail = 0;
  head = 0;
  for (i = 0; i < nedge; i++)
  {
    ce = ae[i];
    bflag = burnt[i];
    if (bflag == 0)
    {
      nucleus = ce;
      burnt[i] = loopID;
      burnt_list[tail] = ce;
	  coin = 1;
      while (coin)
      {
        chaser = burnt_list[tail];
        cgrainname1 = cEdge[chaser].neigh_grainname[0];
        cgrainname2 = cEdge[chaser].neigh_grainname[1];
        cnode1 = cEdge[chaser].node_id[0];
        cnode2 = cEdge[chaser].node_id[1];
        for (j = 0; j < nedge; j++)
        {
          ne = ae[j];
          nbflag = burnt[j];
          if (nbflag == 0)
          {
            ngrainname1 = cEdge[ne].neigh_grainname[0];
            ngrainname2 = cEdge[ne].neigh_grainname[1];
            nnode1 = cEdge[ne].node_id[0];
            nnode2 = cEdge[ne].node_id[1];
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
              burnt_list[head] = ne;
              burnt[j] = loopID;
            }
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
      }
    }
  }
  count = new int[loopID];
  for (k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }
  for (k = 1; k < loopID; k++)
  {
    for (kk = 0; kk < nedge; kk++)
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
      ce = burnt_list[i1];
      cnode1 = cEdge[ce].node_id[0];
      cnode2 = cEdge[ce].node_id[1];
      if (start == cnode1)
      {
        openL = 1; // loop is open...
        startEdge = ce;
        flip = 0;
      }
      else if (start == cnode2)
      {
        openL = 1;
        startEdge = ce;
        flip = 1;
      }
    }
    if (openL == 1)
    { // if current loop is an open loop...
      if (flip == 1)
      {
        tnode = cEdge[startEdge].node_id[0];
        tgrainname = cEdge[startEdge].neigh_grainname[0];
        cEdge[startEdge].node_id[0] = cEdge[startEdge].node_id[1];
        cEdge[startEdge].node_id[1] = tnode;
        cEdge[startEdge].neigh_grainname[0] = cEdge[startEdge].neigh_grainname[1];
        cEdge[startEdge].neigh_grainname[1] = tgrainname;
      }
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cEdge[startEdge].node_id[1];
      chaser = startEdge;
      while (index < numN)
      {
        for (n = from; n < to; n++)
        {
          ce = burnt_list[n];
          cnode1 = cEdge[ce].node_id[0];
          cnode2 = cEdge[ce].node_id[1];
          if ((ce != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = ce;
            index++;
          }
          else if ((ce != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = ce;
            index++;
            // flip...
            tnode = cEdge[ce].node_id[0];
            tgrainname = cEdge[ce].neigh_grainname[0];
            cEdge[ce].node_id[0] = cEdge[ce].node_id[1];
            cEdge[ce].node_id[1] = tnode;
            cEdge[ce].neigh_grainname[0] = cEdge[ce].neigh_grainname[1];
            cEdge[ce].neigh_grainname[1] = tgrainname;
          }
        }
        chaser = burnt_loop[index - 1];
        endNode = cEdge[chaser].node_id[1];
      }
      if (numN == 2)
      {
        te0 = burnt_loop[0];
        te1 = burnt_loop[1];
        cTriangle[ctid].node_id[0] = cEdge[te0].node_id[0];
        cTriangle[ctid].node_id[1] = cEdge[te1].node_id[0];
        cTriangle[ctid].node_id[2] = cEdge[te1].node_id[1];
        cTriangle[ctid].ngrainname[0] = cEdge[te0].neigh_grainname[0];
        cTriangle[ctid].ngrainname[1] = cEdge[te0].neigh_grainname[1];
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
        tv0 = cEdge[te0].node_id[0];
        tcVertex = cEdge[te0].node_id[1];
        tv2 = cEdge[te1].node_id[1];
        cTriangle[ctid].node_id[0] = tv0;
        cTriangle[ctid].node_id[1] = tcVertex;
        cTriangle[ctid].node_id[2] = tv2;
        cTriangle[ctid].ngrainname[0] = cEdge[te0].neigh_grainname[0];
        cTriangle[ctid].ngrainname[1] = cEdge[te0].neigh_grainname[1];
        new_node0 = tv2;
        new_node1 = tcVertex;
        cnumT++;
        ctid++;
        while (cnumT < numT)
        {
          if ((cnumT % 2) != 0)
          {
            front = front + 1;
            ce = burnt_loop[front];
            tv0 = cEdge[ce].node_id[0];
            tcVertex = cEdge[ce].node_id[1];
            tv2 = new_node0;
            cTriangle[ctid].node_id[0] = tv0;
            cTriangle[ctid].node_id[1] = tcVertex;
            cTriangle[ctid].node_id[2] = tv2;
            cTriangle[ctid].ngrainname[0] = cEdge[ce].neigh_grainname[0];
            cTriangle[ctid].ngrainname[1] = cEdge[ce].neigh_grainname[1];
            new_node0 = tcVertex;
            cnumT++;
            ctid++;
          }
          else
          {
            back = back - 1;
            ce = burnt_loop[back];
            tv0 = cEdge[ce].node_id[0];
            tcVertex = cEdge[ce].node_id[1];
            tv2 = new_node0;
            cTriangle[ctid].node_id[0] = tv0;
            cTriangle[ctid].node_id[1] = tcVertex;
            cTriangle[ctid].node_id[2] = tv2;
            cTriangle[ctid].ngrainname[0] = cEdge[ce].neigh_grainname[0];
            cTriangle[ctid].ngrainname[1] = cEdge[ce].neigh_grainname[1];
            new_node0 = tv0;
            cnumT++;
            ctid++;
          }
        }
      }
      delete [] burnt_loop;
    }
    else
    { // if current loop is a closed one....i.e., openL = 0...
      startEdge = burnt_list[from];
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cEdge[startEdge].node_id[1];
      chaser = startEdge;
      while (index < numN)
      {
        for (n = from; n < to; n++)
        {
          ce = burnt_list[n];
          cnode1 = cEdge[ce].node_id[0];
          cnode2 = cEdge[ce].node_id[1];
          if ((ce != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = ce;
            index++;
          }
          else if ((ce != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = ce;
            index++;
            // flip...
            tnode = cEdge[ce].node_id[0];
            tgrainname = cEdge[ce].neigh_grainname[0];
            cEdge[ce].node_id[0] = cEdge[ce].node_id[1];
            cEdge[ce].node_id[1] = tnode;
            cEdge[ce].neigh_grainname[0] = cEdge[ce].neigh_grainname[1];
            cEdge[ce].neigh_grainname[1] = tgrainname;
          }
        }
        chaser = burnt_loop[index - 1];
        endNode = cEdge[chaser].node_id[1];
      }
      if (numN == 3)
      {
        te0 = burnt_loop[0];
        te1 = burnt_loop[1];
        te2 = burnt_loop[2];
        cTriangle[ctid].node_id[0] = cEdge[te0].node_id[0];
        cTriangle[ctid].node_id[1] = cEdge[te1].node_id[0];
        cTriangle[ctid].node_id[2] = cEdge[te2].node_id[0];
        cTriangle[ctid].ngrainname[0] = cEdge[te0].neigh_grainname[0];
        cTriangle[ctid].ngrainname[1] = cEdge[te0].neigh_grainname[1];
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
        tv0 = cEdge[te0].node_id[0];
        tcVertex = cEdge[te0].node_id[1];
        tv2 = cEdge[te1].node_id[0];
        cTriangle[ctid].node_id[0] = tv0;
        cTriangle[ctid].node_id[1] = tcVertex;
        cTriangle[ctid].node_id[2] = tv2;
        cTriangle[ctid].ngrainname[0] = cEdge[te0].neigh_grainname[0];
        cTriangle[ctid].ngrainname[1] = cEdge[te0].neigh_grainname[1];
        new_node0 = tv2;
        new_node1 = tcVertex;
        cnumT++;
        ctid++;
        while (cnumT < numT)
        {
          if ((cnumT % 2) != 0)
          {
            front = front + 1;
            ce = burnt_loop[front];
            tv0 = cEdge[ce].node_id[0];
            tcVertex = cEdge[ce].node_id[1];
            tv2 = new_node0;
            cTriangle[ctid].node_id[0] = tv0;
            cTriangle[ctid].node_id[1] = tcVertex;
            cTriangle[ctid].node_id[2] = tv2;
            cTriangle[ctid].ngrainname[0] = cEdge[ce].neigh_grainname[0];
            cTriangle[ctid].ngrainname[1] = cEdge[ce].neigh_grainname[1];
            new_node0 = tcVertex;
            cnumT++;
            ctid++;
          }
          else
          {
            back = back - 1;
            ce = burnt_loop[back];
            tv0 = cEdge[ce].node_id[0];
            tcVertex = cEdge[ce].node_id[1];
            tv2 = new_node0;
            cTriangle[ctid].node_id[0] = tv0;
            cTriangle[ctid].node_id[1] = tcVertex;
            cTriangle[ctid].node_id[2] = tv2;
            cTriangle[ctid].ngrainname[0] = cEdge[ce].neigh_grainname[0];
            cTriangle[ctid].ngrainname[1] = cEdge[ce].neigh_grainname[1];
            new_node0 = tv0;
            cnumT++;
            ctid++;
          }
        }

      }
      delete [] burnt_loop;
    }
  }
  *tout = ctid;
  delete [] burnt;
  delete [] burnt_list;
  delete [] count;
}
void SurfaceMeshFunc::get_caseM_triangles(int *ae, int nedge, int *afc, int nfctr, int tin, int *tout, int ccn)
{
  int ii, i, j, k, kk, k1, n, i1, j1, n1, iii;
  int loopID;
  int tail, head, coin;
  int ce, ne;
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
  int ctid;
  int front, back;
  int te0, te1, te2, tv0, tcVertex, tv2, ts0, ts1;
  int numT, cnumT, new_node0, new_node1;
  burnt = new int[nedge];
  burnt_list = new int[nedge];
  // initialize burn flags for face edges...
  for (ii = 0; ii < nedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }
  loopID = 1;
  tail = 0;
  head = 0;
  for (i = 0; i < nedge; i++)
  {
    ce = ae[i];
    bflag = burnt[i];
    if (bflag == 0)
    {
      nucleus = ce;
      burnt[i] = loopID;
      burnt_list[tail] = ce;
	  coin = 1;
      while (coin)
      {
        chaser = burnt_list[tail];
        cgrainname1 = cEdge[chaser].neigh_grainname[0];
        cgrainname2 = cEdge[chaser].neigh_grainname[1];
        cnode1 = cEdge[chaser].node_id[0];
        cnode2 = cEdge[chaser].node_id[1];
        for (j = 0; j < nedge; j++)
        {
          ne = ae[j];
          nbflag = burnt[j];
          if (nbflag == 0)
          {
            ngrainname1 = cEdge[ne].neigh_grainname[0];
            ngrainname2 = cEdge[ne].neigh_grainname[1];
            nnode1 = cEdge[ne].node_id[0];
            nnode2 = cEdge[ne].node_id[1];
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
              burnt_list[head] = ne;
              burnt[j] = loopID;
            }
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
      }
    }
  }
  count = new int[loopID];
  for (k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }
  for (k = 1; k < loopID; k++)
  {
    for (kk = 0; kk < nedge; kk++)
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
      ce = burnt_list[i1];
      cnode1 = cEdge[ce].node_id[0];
      cnode2 = cEdge[ce].node_id[1];
      for (n1 = 0; n1 < nfctr; n1++)
      {
        start = afc[n1];
        if (start == cnode1)
        {
          openL = 1; // loop is open...
          startEdge = ce;
          flip = 0;
        }
        else if (start == cnode2)
        {
          openL = 1;
          startEdge = ce;
          flip = 1;
        }
      }
    }
    if (openL == 1)
    { // if current loop is an open loop...
      if (flip == 1)
      {
        tnode = cEdge[startEdge].node_id[0];
        tgrainname = cEdge[startEdge].neigh_grainname[0];
        cEdge[startEdge].node_id[0] = cEdge[startEdge].node_id[1];
        cEdge[startEdge].node_id[1] = tnode;
        cEdge[startEdge].neigh_grainname[0] = cEdge[startEdge].neigh_grainname[1];
        cEdge[startEdge].neigh_grainname[1] = tgrainname;
      }
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cEdge[startEdge].node_id[1];
      chaser = startEdge;
      while (index < numN)
      {
        for (n = from; n < to; n++)
        {
          ce = burnt_list[n];
          cnode1 = cEdge[ce].node_id[0];
          cnode2 = cEdge[ce].node_id[1];
          if ((ce != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = ce;
            index++;
          }
          else if ((ce != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = ce;
            index++;
            // flip...
            tnode = cEdge[ce].node_id[0];
            tgrainname = cEdge[ce].neigh_grainname[0];
            cEdge[ce].node_id[0] = cEdge[ce].node_id[1];
            cEdge[ce].node_id[1] = tnode;
            cEdge[ce].neigh_grainname[0] = cEdge[ce].neigh_grainname[1];
            cEdge[ce].neigh_grainname[1] = tgrainname;
          }
        }
        chaser = burnt_loop[index - 1];
        endNode = cEdge[chaser].node_id[1];
      }
      //triangulation...
      for (iii = 0; iii < numN; iii++)
      {
        ce = burnt_loop[iii];
        tn0 = cEdge[ce].node_id[0];
        tn1 = cEdge[ce].node_id[1];
        ts0 = cEdge[ce].neigh_grainname[0];
        ts1 = cEdge[ce].neigh_grainname[1];
        cTriangle[ctid].node_id[0] = ccn;
        cTriangle[ctid].node_id[1] = tn0;
        cTriangle[ctid].node_id[2] = tn1;
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
      endNode = cEdge[startEdge].node_id[1];
      chaser = startEdge;
      while (index < numN)
      {
        for (n = from; n < to; n++)
        {
          ce = burnt_list[n];
          cnode1 = cEdge[ce].node_id[0];
          cnode2 = cEdge[ce].node_id[1];
          if ((ce != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = ce;
            index++;
          }
          else if ((ce != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = ce;
            index++;
            // flip...
            tnode = cEdge[ce].node_id[0];
            tgrainname = cEdge[ce].neigh_grainname[0];
            cEdge[ce].node_id[0] = cEdge[ce].node_id[1];
            cEdge[ce].node_id[1] = tnode;
            cEdge[ce].neigh_grainname[0] = cEdge[ce].neigh_grainname[1];
            cEdge[ce].neigh_grainname[1] = tgrainname;
          }
        }
        chaser = burnt_loop[index - 1];
        endNode = cEdge[chaser].node_id[1];
      }
      if (numN == 3)
      {
        te0 = burnt_loop[0];
        te1 = burnt_loop[1];
        te2 = burnt_loop[2];
        cTriangle[ctid].node_id[0] = cEdge[te0].node_id[0];
        cTriangle[ctid].node_id[1] = cEdge[te1].node_id[0];
        cTriangle[ctid].node_id[2] = cEdge[te2].node_id[0];
        cTriangle[ctid].ngrainname[0] = cEdge[te0].neigh_grainname[0];
        cTriangle[ctid].ngrainname[1] = cEdge[te0].neigh_grainname[1];
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
        tv0 = cEdge[te0].node_id[0];
        tcVertex = cEdge[te0].node_id[1];
        tv2 = cEdge[te1].node_id[0];
        cTriangle[ctid].node_id[0] = tv0;
        cTriangle[ctid].node_id[1] = tcVertex;
        cTriangle[ctid].node_id[2] = tv2;
        cTriangle[ctid].ngrainname[0] = cEdge[te0].neigh_grainname[0];
        cTriangle[ctid].ngrainname[1] = cEdge[te0].neigh_grainname[1];
        new_node0 = tv2;
        new_node1 = tcVertex;
        cnumT++;
        ctid++;
        while (cnumT < numT)
        {
          if ((cnumT % 2) != 0)
          {
            front = front + 1;
            ce = burnt_loop[front];
            tv0 = cEdge[ce].node_id[0];
            tcVertex = cEdge[ce].node_id[1];
            tv2 = new_node0;
            cTriangle[ctid].node_id[0] = tv0;
            cTriangle[ctid].node_id[1] = tcVertex;
            cTriangle[ctid].node_id[2] = tv2;
            cTriangle[ctid].ngrainname[0] = cEdge[ce].neigh_grainname[0];
            cTriangle[ctid].ngrainname[1] = cEdge[ce].neigh_grainname[1];
            new_node0 = tcVertex;
            cnumT++;
            ctid++;
          }
          else
          {
            back = back - 1;
            ce = burnt_loop[back];
            tv0 = cEdge[ce].node_id[0];
            tcVertex = cEdge[ce].node_id[1];
            tv2 = new_node0;
            cTriangle[ctid].node_id[0] = tv0;
            cTriangle[ctid].node_id[1] = tcVertex;
            cTriangle[ctid].node_id[2] = tv2;
            cTriangle[ctid].ngrainname[0] = cEdge[ce].neigh_grainname[0];
            cTriangle[ctid].ngrainname[1] = cEdge[ce].neigh_grainname[1];
            new_node0 = tv0;
            cnumT++;
            ctid++;
          }
        }
      }
      delete [] burnt_loop;
    }
  }
  *tout = ctid;
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
    for (j = 0; j < 3; j++)
    { // for each node iNSide the triangle...
      cnode = cTriangle[i].node_id[j];
      cst_index = cnode / 7 + 1;
      csite = cnode / 7 + 1;
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
  }
}
int SurfaceMeshFunc::assign_nodeID(int nN, int zID)
{
  int i;
  int nid;
  int nkind;
  int cnid;
  int plane;
  nid = nN;
  for (i = 0; i < (7 * 2 * NSP); i++)
  {
    nkind = cVertex[i].nodeKind;
    cnid = cVertex[i].NodeID;
	plane = i%7;
    if (nkind != 0 && cnid == -1)
    {
      cVertex[i].NodeID = nid;
      nid++;
    }
  }
  return (nid);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void SurfaceMeshFunc::get_output_nodes(int zID, int cNodeID, string NodesFile)
void SurfaceMeshFunc::writeNodesFile(int zID, int cNodeID, const std::string &nodesFile)
{
//  int index;
//  int i, j, k;
//  int finish;
  int tID;
  int nk;
  int czid;
  double x, y, z;
//  int start, end;
  int count;
  vector<int > order;
 // end = -1;
  count = 0;
  czid = zID;
  FILE* f = NULL;

  // Create a new file if this is our first slice
  if (zID == 0)
  {
    f = fopen(nodesFile.c_str(), "w");
  }
  // Append to existing file if we are on z>0 slice
  if (zID > 0)
  {
    f = fopen(nodesFile.c_str(), "a");
  }
  //  if(zID == 0) outFile.open(nodesFile.c_str());
  //  if(zID > 0) outFile.open(nodesFile.c_str(), ios::app);
  for (int k = 0; k < (7 * 2 * NSP); k++)
  {
    tID = cVertex[k].NodeID;
    if (tID > cNodeID - 1)
    {
      nk = cVertex[k].nodeKind;
      x = cVertex[k].xc;
      y = cVertex[k].yc;
      z = cVertex[k].zc;
      fprintf(f, "%d %d %f %f %f\n", tID, nk, x, y, z);
   //   outFile << tID << "	" << nk << "	" << x << "	" << y << "	" << z << endl;
    }
  }
  fclose(f);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshFunc::writeTrianglesFile (int nt, const std::string &trianglesFile, int zID, int ctid)
{
  //int i;
  int tag;
  int end;
  int newID;
  int n1, n2, n3, s1, s2;
  //  double x1, x2, x3;
  //  double y1, y2, y3;
  //  double z1, z2, z3;
  tag = zID;
  end = nt;
  newID = ctid;
  FILE* f = NULL;

  // Create a new file if this is our first slice
  if (zID == 0)
  {
    f = fopen(trianglesFile.c_str(), "w");
  }
  // Append to existing file if we are on z>0 slice
  if (zID > 0)
  {
    f = fopen(trianglesFile.c_str(), "a");
  }
  //  outFile << nt <<endl;
  for (int i = 0; i < end; i++)
  {
    n1 = cTriangle[i].node_id[0];
    n2 = cTriangle[i].node_id[1];
    n3 = cTriangle[i].node_id[2];
    n1 = cVertex[n1].NodeID;
    n2 = cVertex[n2].NodeID;
    n3 = cVertex[n3].NodeID;
    s1 = cTriangle[i].ngrainname[0];
    s2 = cTriangle[i].ngrainname[1];
    fprintf(f, "%d %d %d %d %d %d\n", newID, n1, n2, n3, s1, s2);
//    outFile << newID << "	" << n1 << "	" << n2 << "	" << n3 << "	" << s1 << "	" << s2 << endl;
    newID++;
  }
  fclose(f);
  if (end > 0) delete[] cTriangle;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshFunc::writeVTKOutputFile (int nNodes, int nTriangles,
                                  const std::string &VisualizationFile,
                                  const std::string &NodesFile,
                                  const std::string &TrianglesFile,
                                  bool binaryFile,
                                  bool conformalMesh)
{
#if 1
  FILE* vtkFile = NULL;
  FILE* nodes = NULL;
  FILE* tris = NULL;


  vtkFile = fopen(VisualizationFile.c_str(), "w");
  if (NULL == vtkFile)
  {
    std::cout << "Error Creating VTK Visualization File '" << VisualizationFile << "'" << std::endl;
    return;
  }
  nodes = fopen(NodesFile.c_str(), "r");
  if (NULL == nodes)
  {
    std::cout << "Error Opening Nodes file '" << NodesFile << "'" << std::endl;
    fclose(vtkFile);
    return;
  }
  tris = fopen(TrianglesFile.c_str(), "r");
  if (NULL == tris)
  {
    std::cout << "Error Opening Triangles file'" << TrianglesFile << "'" << std::endl;
    fclose(vtkFile);
    fclose(nodes);
    return;
  }

  fprintf(vtkFile, "# vtk DataFile Version 2.0\n");
  fprintf(vtkFile,  "data set from FFT2dx_GB\n");
	fprintf(vtkFile,  "ASCII\n");
	fprintf(vtkFile,  "DATASET UNSTRUCTURED_GRID\n");
	fprintf(vtkFile,  "POINTS %d float\n", nNodes);
	int nodenum;
	int nodetype;
	float x;
	float y;
	float z;
	// Read in the nodes one line at a time and write them out to the vtk file
	for(int i=0;i<nNodes;i++)
	{
		fscanf(nodes, "%d %d %f %f %f\n", &nodenum, &nodetype, &x, &y, &z);
		fprintf(vtkFile, "%f %f %f\n", x, y, z);
	}
	//inputFile1.close();
	fclose(nodes);
	nodes = NULL;

	// Write the triangle indices into the vtk File
	int trianglenum;
	int node1;
	int node2;
	int node3;


  fprintf(vtkFile, "CELLS %d %d\n", nTriangles, (nTriangles*4));
  // Stoe the Grain Ids so we don't have to re-read the triangles file again
	int* grainIds = (int*)malloc(nTriangles * 2 * 4);
	size_t index = 0;
	for(int i=0;i<nTriangles;i++)
	{
	  // Read a line from the Triangles File
	  fscanf(tris, "%d %d %d %d %d %d", &trianglenum, &node1, &node2, &node3, &grainIds[index], &grainIds[index+1]);

    if (grainIds[index] < grainIds[index+1])
    {
      fprintf(vtkFile, "3 %d %d %d\n", node1, node2, node3);
    }
    else
    {
      fprintf(vtkFile, "3 %d %d %d\n", node3, node2, node1);
    }

	  index = index + 2;
	}
	fclose(tris);
	tris = NULL;

	// Write the CELL_TYPES into the file
	fprintf(vtkFile, "\n");
	fprintf(vtkFile, "CELL_TYPES %d\n", nTriangles);
  for(int i=0;i<nTriangles;i++)
  {
    fprintf(vtkFile, "5\n");
  }


  // Write the GrainId Data to teh file
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_DATA %d\n", nTriangles);
  fprintf(vtkFile, "SCALARS GrainID int\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");
  index = 0;
  for (int i = 0; i < nTriangles; i++)
  {
    if (grainIds[index] < grainIds[index + 1])
    {
      fprintf(vtkFile, "%d\n", grainIds[index]);
    }
    else
    {
      fprintf(vtkFile, "%d\n", grainIds[index + 1]);
    }
    index = index + 2;
  }
  // Free the memory
  free(grainIds);
  // Close the input and output files
  fclose(vtkFile);

#else
  ofstream outFile;
  outFile.open(VisualizationFile.c_str());
  ifstream inputFile1;
  inputFile1.open(NodesFile.c_str());
  ifstream inputFile2;
  inputFile2.open(TrianglesFile.c_str());
  outFile << "# vtk DataFile Version 2.0" << endl;
  outFile << "data set from FFT2dx_GB" << endl;
  outFile << "ASCII" << endl;
  outFile << "DATASET UNSTRUCTURED_GRID" << endl;
  outFile << endl;
  outFile << "POINTS " << nNodes << " float" << endl;
  int nodenum;
  int nodetype;
  double x;
  double y;
  double z;
  for(int i=0;i<nNodes;i++)
  {
    inputFile1 >> nodenum >> nodetype >> x >> y >> z;
    outFile << x << " " << y << " " << z << endl;
  }
  inputFile1.close();
  int trianglenum;
  int node1;
  int node2;
  int node3;
//  int edge1;
//  int edge2;
//  int edge3;
  int grain1;
  int grain2;
  outFile << endl;
  outFile << "CELLS " << nTriangles << " " << (nTriangles*4) << endl;
  for(int i=0;i<nTriangles;i++)
  {
    inputFile2 >> trianglenum >> node1 >> node2 >> node3 >> grain1 >> grain2;
    if(grain1 < grain2) outFile << "3 " << node1 << " " << node2 << " " << node3 << endl;
    if(grain1 > grain2) outFile << "3 " << node3 << " " << node2 << " " << node1 << endl;
  }
  inputFile2.close();
  outFile << endl;
  outFile << "CELL_TYPES " << nTriangles << endl;
  for(int i=0;i<nTriangles;i++)
  {
    outFile << "5" << endl;
  }
  inputFile2.open(TrianglesFile.c_str());
  outFile << endl;
  outFile << "CELL_DATA " << nTriangles << endl;
  outFile << "SCALARS GrainID float" << endl;
  outFile << "LOOKUP_TABLE default" << endl;
  for(int i=0;i<nTriangles;i++)
  {
    inputFile2 >> trianglenum >> node1 >> node2 >> node3 >> grain1 >> grain2;
    if(grain1 < grain2) outFile << grain1 << endl;
    if(grain1 > grain2) outFile << grain2 << endl;
  }
  inputFile2.close();
  outFile.close();
#endif
}
