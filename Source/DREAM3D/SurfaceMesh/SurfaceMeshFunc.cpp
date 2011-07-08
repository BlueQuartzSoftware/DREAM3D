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
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif

// Include this FIRST because there is a needed define for some compiles
// to expose some of the constants needed below
#include "DREAM3D/Common/AIMMath.h"

// C Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-- C++ STL
#include <vector>
#include <map>
#include <sstream>
#include <queue>

#include <MXA/Utilities/MXADir.h>
#include <MXA/Utilities/StringUtils.h>

using namespace std;

//const static float m_pi = M_PI;
//const static float m_OnepointThree = 1.33333333333f;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshFunc::SurfaceMeshFunc() :
    neigh(NULL), voxels(NULL), cSquare(NULL)
{

}

SurfaceMeshFunc::~SurfaceMeshFunc()
{
  delete[] neigh;
  delete[] voxels;
  delete[] cSquare;
}

void SurfaceMeshFunc::get_neighbor_list()
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
  float x, y, z;
  int start = NSP + 1;
  if (zID == 0) start = 1, numgrains = 0;
  // node id starts with 0....
  if (zID > 0)
  {
    for (i = 1; i <= NSP; i++)
    {
      id = 7 * (i - 1);
      oid = 7 * ((i + NSP) - 1);
      for (j = 0; j < 7; j++)
      {
        cVertex[id + j] = cVertex[oid + j];
      }
    }
  }
  int shift = (zID * NSP);
  for (i = start; i <= 2 * NSP; i++)
  {
    id = 7 * (i - 1);
    tsite = i;
    locale = tsite + shift;
    x = find_xcoord(locale);
    y = find_ycoord(locale);
    z = find_zcoord(locale);
    int grainid = voxels[tsite];
    if (grainid > numgrains) numgrains = grainid;
    cVertex[id].xc = x + (0.5 * xRes);
    cVertex[id].yc = y;
    cVertex[id].zc = z;
    cVertex[id].nodeKind = 0;
    cVertex[id].NodeID = -1;
    cVertex[id].point = tsite;
    cVertex[id + 1].xc = x;
    cVertex[id + 1].yc = y + (0.5 * yRes);
    cVertex[id + 1].zc = z;
    cVertex[id + 1].nodeKind = 0;
    cVertex[id + 1].NodeID = -1;
    cVertex[id + 1].point = tsite;
    cVertex[id + 2].xc = x;
    cVertex[id + 2].yc = y;
    cVertex[id + 2].zc = z + (0.5 * zRes);
    cVertex[id + 2].nodeKind = 0;
    cVertex[id + 2].NodeID = -1;
    cVertex[id + 2].point = tsite;
    cVertex[id + 3].xc = x + (0.5 * xRes);
    cVertex[id + 3].yc = y + (0.5 * yRes);
    cVertex[id + 3].zc = z;
    cVertex[id + 3].nodeKind = 0;
    cVertex[id + 3].NodeID = -1;
    cVertex[id + 3].point = tsite;
    cVertex[id + 4].xc = x + (0.5 * xRes);
    cVertex[id + 4].yc = y;
    cVertex[id + 4].zc = z + (0.5 * zRes);
    cVertex[id + 4].nodeKind = 0;
    cVertex[id + 4].NodeID = -1;
    cVertex[id + 4].point = tsite;
    cVertex[id + 5].xc = x;
    cVertex[id + 5].yc = y + (0.5 * yRes);
    cVertex[id + 5].zc = z + (0.5 * zRes);
    cVertex[id + 5].nodeKind = 0;
    cVertex[id + 5].NodeID = -1;
    cVertex[id + 5].point = tsite;
    cVertex[id + 6].xc = x + (0.5 * xRes);
    cVertex[id + 6].yc = y + (0.5 * yRes);
    cVertex[id + 6].zc = z + (0.5 * zRes);
    cVertex[id + 6].nodeKind = 0;
    cVertex[id + 6].NodeID = -1;
    cVertex[id + 6].point = tsite;
  }
}

float SurfaceMeshFunc::find_xcoord(int index)
{
  index = index - 1;
  float x = xRes * float(index % xDim);
  return x;
}
float SurfaceMeshFunc::find_ycoord(int index)
{
  index = index - 1;
  float y = yRes * float((index / xDim) % yDim);
  return y;
}
float SurfaceMeshFunc::find_zcoord(int index)
{
  index = index - 1;
  float z = zRes * float(index / (xDim * yDim));
  return z;
}
void SurfaceMeshFunc::initialize_squares(int zID)
{

  // Gather initial information on each square...
  int id;
  int i, j;
  int csite;
  // square id starts with 0....
  // notice that point at the surface will have the wrong values of node at the other end...
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
    for (int j = 0; j < 3; j++)
    {
      cSquare[id + j].nEdge = 0;
      cSquare[id + j].turnFC = 0;
      cSquare[id + j].FCnode = -1;
      cSquare[id + j].effect = 0;
    }
  }
}
int SurfaceMeshFunc::get_nodes_Edges(int eT2d[20][8], int NST2d[20][8], int zID)
{
  int j, k, m, ii;
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
  int tn1, tn2;
  int tnk;
  int pixIndex[2];
  int eid; // edge id...
  int tnode1, tnode2;
  int edgeCount; // number of edges for each square...
  int nodeID[2];
  int pixgrainname[2];
  eid = 0;
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
      cgrainname = voxels[csite];
      tNSite[0] = cSquare[k].site_id[0];
      tNSite[1] = cSquare[k].site_id[1];
      tNSite[2] = cSquare[k].site_id[2];
      tNSite[3] = cSquare[k].site_id[3];
      atBulk = 0;
      for (m = 0; m < 4; m++)
      {
        tsite = tNSite[m];
        tngrainname[m] = voxels[tsite];
        if (tngrainname[m] < 0) atBulk++;
      }
      edgeCount = 0;
      // Let's find the edges...
      if (atBulk != 4)
      { // coNSider the square inside the bulk only...
        cSquare[k].effect = 1;
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
              get_grainnames(cubeOrigin, sqOrder, pixIndex, pixgrainname);
              if (pixgrainname[0] > 0 || pixgrainname[1] > 0)
              {
                cEdge.resize(eid + 1);
                cEdge[eid].node_id[0] = nodeID[0]; // actual node ids for each edge...
                cEdge[eid].node_id[1] = nodeID[1];
                cEdge[eid].neigh_grainname[0] = pixgrainname[0];
                cEdge[eid].neigh_grainname[1] = pixgrainname[1];
                cEdge[eid].edgeKind = 2; // edges of the open loops are always binary...
                // triple lines only occurs inside the marching cubes...
                cSquare[k].edge_id[edgeCount] = eid;
                edgeCount++;
                eid++;
              }
              else
              {
                tn1 = nodeID[0];
                tn2 = nodeID[1];
                cVertex[tn1].nodeKind = -1; // extra nodes from meshing the surface of the box...
                cVertex[tn2].nodeKind = -1; // we don't need them...
              }
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
                  tnk = cVertex[tnode].nodeKind;
                  if (tnk != -1)
                  {
                    cVertex[tnode].nodeKind = 2;
                  }
                }
              }
            }
          }
        }
      }
      cSquare[k].nEdge = edgeCount;
    }
  }
  return (cEdge.size());
}

int SurfaceMeshFunc::get_square_index(int tNS[4])
{
  // identify each square configuration using binary bit...
  // returNS the unique decimal integer for each configuration...
  int aBit[6];
  int tempIndex;
  int subIndex;
  if (tNS[0] == tNS[1]) aBit[0] = 0;
  else aBit[0] = 1;
  if (tNS[1] == tNS[2]) aBit[1] = 0;
  else aBit[1] = 1;
  if (tNS[2] == tNS[3]) aBit[2] = 0;
  else aBit[2] = 1;
  if (tNS[3] == tNS[0]) aBit[3] = 0;
  else aBit[3] = 1;
  if (tNS[0] == tNS[2]) aBit[4] = 0;
  else aBit[4] = 1;
  if (tNS[1] == tNS[3]) aBit[5] = 0;
  else aBit[5] = 1;
  tempIndex = 8 * aBit[3] + 4 * aBit[2] + 2 * aBit[1] + 1 * aBit[0];
  if (tempIndex == 15)
  {
    subIndex = 2 * aBit[4] + 1 * aBit[5];
    if (subIndex != 0)
    {
      tempIndex = tempIndex + subIndex + 1;
    }
  }
  return tempIndex;
}

int SurfaceMeshFunc::treat_anomaly(int tNSt[4], int zID1)
{
  int i, j, k, ii;
  int csite, cgrainname;
  int NSite, ngrainname;
  int temp;
  int tempFlag;
  int min, minid;
  int numNeigh[4];
  min = 1000;
  minid = -1;
  for (k = 0; k < 4; k++)
  {
    numNeigh[k] = 0;
  }
  for (i = 0; i < 4; i++)
  {
    csite = tNSt[i];
    cgrainname = voxels[csite];
//    for (j = 1; j <= num_neigh; j++)
    for (j = 1; j <= 8; j++)
    {
      NSite = neigh[csite].neigh_id[j];
      ngrainname = voxels[NSite];
      if (cgrainname == ngrainname)
      {
        numNeigh[i] = numNeigh[i] + 1;
      }
    }
  }
  for (ii = 0; ii < 4; ii++)
  {
    temp = numNeigh[ii];
    if (temp < min)
    {
      min = temp;
      minid = ii;
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
      switch(tempIndex)
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
      switch(tempIndex)
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
      switch(tempIndex)
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

void SurfaceMeshFunc::get_grainnames(int cst, int ord, int pID[2], int *pgrainname)
{
  int i;
  int pixTemp, tempgrainname;
  for (i = 0; i < 2; i++)
  {
    pixTemp = pID[i];
    if (ord == 0)
    {
      switch(pixTemp)
      {
        case 0:
          tempgrainname = voxels[cst];
          pgrainname[i] = tempgrainname;
          break;
        case 1:
          tempgrainname = voxels[cst + 1];
          pgrainname[i] = tempgrainname;
          break;
        case 2:
          tempgrainname = voxels[cst + xDim + 1];
          pgrainname[i] = tempgrainname;
          break;
        case 3:
          tempgrainname = voxels[cst + xDim];
          pgrainname[i] = tempgrainname;
          break;
      }
    }
    else if (ord == 1)
    {
      switch(pixTemp)
      {
        case 0:
          tempgrainname = voxels[cst];
          pgrainname[i] = tempgrainname;
          break;
        case 1:
          tempgrainname = voxels[cst + 1];
          pgrainname[i] = tempgrainname;
          break;
        case 2:
          tempgrainname = voxels[cst + NSP + 1];
          pgrainname[i] = tempgrainname;
          break;
        case 3:
          tempgrainname = voxels[cst + NSP];
          pgrainname[i] = tempgrainname;
          break;
      }
    }
    else if (ord == 2)
    {
      switch(pixTemp)
      {
        case 0:
          tempgrainname = voxels[cst + xDim];
          pgrainname[i] = tempgrainname;
          break;
        case 1:
          tempgrainname = voxels[cst];
          pgrainname[i] = tempgrainname;
          break;
        case 2:
          tempgrainname = voxels[cst + NSP];
          pgrainname[i] = tempgrainname;
          break;
        case 3:
          tempgrainname = voxels[cst + NSP + xDim];
          pgrainname[i] = tempgrainname;
          break;
      }
    }
  }
}

int SurfaceMeshFunc::get_triangles()
{
  int i, ii, i1, i2;
  int sqID[6];
  int tsq; // current sq id...
  int tnE; // temp number of edges...
  int nFC; // number of FC turned on...
  int nE; // number of face edges...
  int eff; // all the squares effective?...
  int cubeFlag; // if 1, we can do marching cube; if 0, useless...
  int BCnode; // cube center node...
  int nkFlag;
  int tsqid1, tsqid2;
  int *arrayE;
  int fcid;
  int tindex;
  int nds, nburnt;
  int tsite1, tsite2;
  int tgrainname1, tgrainname2;
  int cgrainname, ngrainname;
  int arraygrainname[8];
  int tidIn, tidOut;
  int arrayFC[6];
  tidIn = 0;
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
      if (cSquare[tsq].FCnode != -1)
      {
        arrayFC[fcid] = cSquare[tsq].FCnode;
        fcid++;
      }
//      nFC = nFC + 1;
      nE = nE + cSquare[tsq].nEdge;
      eff = eff + cSquare[tsq].effect;
    }
    nFC = fcid;
    if (eff > 0) cubeFlag = 1;
    if (nFC >= 3)
    {
      // If number of face centers turned on is more than 2...
      // let's update the nodeKind of body center node...
      tsqid1 = sqID[0];
      tsqid2 = sqID[5];
      nkFlag = 0;
      // get spin numbers for 8 corners for each marching cube...
      for (int j = 0; j < 4; j++)
      {
        tsite1 = cSquare[tsqid1].site_id[j];
        tsite2 = cSquare[tsqid2].site_id[j];
        tgrainname1 = voxels[tsite1];
        tgrainname2 = voxels[tsite2];
        arraygrainname[j] = tgrainname1;
        arraygrainname[j + 4] = tgrainname2;
        if (tgrainname1 < 0 || tgrainname2 < 0) nkFlag++;
      }
      nds = 0; // number of different spins in each marching cube...
      nburnt = 0; // so nds = nodeKind of body center position...
      for (int k = 0; k < 8; k++)
      {
        // arraySpin contains no -1 before any of it is burnt...
        cgrainname = arraygrainname[k];
        if (cgrainname != -1)
        {
          nds++;
          arraygrainname[k] = -1; // burn...
          nburnt++;
          for (int kk = 0; kk < 8; kk++)
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
      // update nodeKind of body center node in the current marching cube...
      if (nkFlag > 0)
      {
        cVertex[BCnode].nodeKind = nds;
      }
      else
      {
        cVertex[BCnode].nodeKind = nds;
      }
    }
    // Checking the number of edges for loops in the cube...
    // if the current marching cube is a collection of 6 effective squares...and
    // the number of face edges at least 3...
    // when nE==2, it doen't happen
    // when nE==1, the edge will contribute for the neighboring marching cube...
    // when nE==0, it meaNS the cube is inside a grain...
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
        get_case0_triangles(i, arrayE, nE, tidIn, &tidOut);
        tidIn = tidOut;
      }
      else if (nFC == 2)
      {
        get_case2_triangles(i, arrayE, nE, arrayFC, nFC, tidIn, &tidOut);
        tidIn = tidOut;
      }
      else if (nFC > 2 && nFC <= 6)
      {
        get_caseM_triangles(i, arrayE, nE, arrayFC, nFC, tidIn, &tidOut, BCnode);
        tidIn = tidOut;
      }
      delete[] arrayE;
    }
  }
  cEdge.clear();
  return (cTriangle.size());
}

#define ADD_TRIANGLE(cTriangle, ctid, n0, n1, n2, label0, label1)\
  cTriangle.resize(ctid + 1);\
  cTriangle[ctid].node_id[0] = n0;\
  cTriangle[ctid].node_id[1] = n1;\
  cTriangle[ctid].node_id[2] = n2;\
  cTriangle[ctid].ngrainname[0] = label0;\
  cTriangle[ctid].ngrainname[1] = label1;\
  cTriangle[ctid].tIndex = ctid;\
  m3c::Edge::Pointer e0 = m3c::Edge::New(cTriangle[ctid].node_id[0], cTriangle[ctid].node_id[1]);\
  m3c::Edge::Pointer e = eMap[e0->getId()];\
  if (NULL == e.get()) { eMap[e0->getId()] = e0; }else{ e0 = e; }\
  e0->triangles.insert(ctid);\
  m3c::Edge::Pointer e1 = m3c::Edge::New(cTriangle[ctid].node_id[1], cTriangle[ctid].node_id[2]);\
  e = eMap[e1->getId()];\
  if (NULL == e.get()) { eMap[e1->getId()] = e1; }else{ e1 = e;}\
  e1->triangles.insert(ctid);\
  m3c::Edge::Pointer e2 = m3c::Edge::New(cTriangle[ctid].node_id[2], cTriangle[ctid].node_id[0]);\
  e = eMap[e2->getId()]; \
  if (NULL == e.get()){ eMap[e2->getId()] = e2; }else{ e2 = e; }\
  e2->triangles.insert(ctid);\
  cTriangle[ctid].edges[0] = e0;  cTriangle[ctid].edges[1] = e1;  cTriangle[ctid].edges[2] = e2;\
  labelTriangleMap[label0] = ctid;\
  labelTriangleMap[label1] = ctid;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshFunc::analyzeWinding()
{
  std::cout << "Done Reading Triangles File" << std::endl;
  std::cout << " Edge Count: " << eMap.size() << std::endl;
  std::cout << " Triangle Count: " << cTriangle.size() << std::endl;
  std::cout << "labelTriangleMap.size(): " << labelTriangleMap.size() << std::endl;

  float total = (float)(labelTriangleMap.size());
 // bool firstLabel = true;
  // Keeps a list of all the triangles that have been visited.
  std::vector<bool> masterVisited(cTriangle.size(), false);

  size_t size = cTriangle.size();
  for(size_t i = 0; i < size; ++i)
  {
    if (cTriangle[i].tIndex > size)
    {
      std::cout << "HEre" << std::endl;
    }
  }



  // Start with first triangle in the master list:
  int masterTriangleIndex = 0;
  Patch& t = cTriangle[masterTriangleIndex];
//  std::deque<int> labelsToVisit;

  // Get the first 2 labels to visit from the first triangle. We use these 2 labels
  // because this triangle shares these 2 labels and guarantees these 2 labels are
  // connected to each other.

  float curPercent = 0.0;
  int progressIndex = 0;
  for (LabelTriangleMapType::iterator cLabel = labelTriangleMap.begin(); cLabel != labelTriangleMap.end(); ++cLabel )
  {
    int currentLabel = cLabel->first;
  //  if (currentLabel != 1) { continue; }
    masterTriangleIndex = cLabel->second;
    t = cTriangle[masterTriangleIndex];

    if ( (progressIndex/total * 100.0f) > (curPercent) )
    {
      std::cout << "Verifying Winding: " << curPercent << "% Complete" << std::endl;
      curPercent += 5.0f;
    }
    ++progressIndex;
  //  std::cout << "Current Label: " << currentLabel << std::endl;
  //  int seedTriIndex = masterTriangleIndex;

    if (NULL == &t )
    {
      std::cout << "Could not find a triangle with the winding set. This should NOT happen" << std::endl;
      assert(1 == 0);
    }

    std::set<int> localVisited; // Keep track of which triangles have been visited
    std::deque<int> triangleDeque;
    triangleDeque.push_back(t.tIndex);

    while (triangleDeque.empty() == false)
    {
      t = cTriangle[triangleDeque.front()];
  //    std::cout << "tIndex = " << t->tIndex << std::endl;
      localVisited.insert(t.tIndex);
#if 0
      bool debug = ( t->tIndex == 163674 || t->tIndex == 163673 || t->tIndex == 163675 || t->tIndex == 163700);
      if (debug == true)
      {
        std::cout << "Debugging" << std::endl;
      } debug = false;
#endif
      std::vector<int> adjTris = findAdjacentTriangles(t, currentLabel);
      for ( std::vector<int>::iterator adjTri = adjTris.begin(); adjTri != adjTris.end(); ++adjTri )
      {
        //  std::cout << "  ^ AdjTri index: " << (*adjTri)->tIndex << std::endl;
        if (masterVisited[*adjTri] == false)
        {
          //   std::cout << "   * Checking Winding: " << (*adjTri)->tIndex << std::endl;
          t.verifyWinding( cTriangle[*adjTri], currentLabel);
        }


        if (localVisited.find(*adjTri) == localVisited.end()
          && find(triangleDeque.begin(), triangleDeque.end(), *adjTri) == triangleDeque.end())
        {
          // std::cout << "   # Adding to Deque: " << (*adjTri)->tIndex << std::endl;
          triangleDeque.push_back(*adjTri);
          localVisited.insert(*adjTri);
          masterVisited[*adjTri] = true;
        }
      }

      triangleDeque.pop_front();
    }

  }

  std::cout << "--------------------------------------------------------------" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<int> SurfaceMeshFunc::findAdjacentTriangles(Patch &triangle, int label)
{
  std::vector<int> adjacentTris;
  typedef m3c::Edge::Pointer EdgeType;
  // Get the 3 edges from the triangle


  // Iterate over the 3 Edges of the triangle
  for (int i = 0; i < 3; ++i)
  {
    // Get the current edge
    EdgeType e = triangle.edges[i];
    // Get the list of indices of the triangles that belong to that edge
    std::set<int> tIndices = e->triangles;
    // Iterate over the indices to find triangles that match the label and are NOT the current triangle index
    for (std::set<int>::iterator iter = tIndices.begin(); iter != tIndices.end(); ++iter )
    {
      Patch& t = cTriangle.at(*iter);
      if ( (t.ngrainname[0] == label || t.ngrainname[1] == label)
          && (t.tIndex != triangle.tIndex) )
      {
     //   std::cout << "    Found Adjacent Triangle: " << t->tIndex << std::endl;
        adjacentTris.push_back(t.tIndex);
      }
    }
  }

  return adjacentTris;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshFunc::get_case0_triangles(int site, int *ae, int nedge, int tin, int *tout)
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
                cEdge[ne].neigh_grainname[0] = ngrainname2;
                cEdge[ne].neigh_grainname[1] = ngrainname1;
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

      ADD_TRIANGLE(cTriangle, ctid, cEdge[te0].node_id[0], cEdge[te1].node_id[0], cEdge[te2].node_id[0], cEdge[te0].neigh_grainname[0], cEdge[te0].neigh_grainname[1] )
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
      ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[te0].neigh_grainname[0], cEdge[te0].neigh_grainname[1] )

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
          ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[ce].neigh_grainname[0], cEdge[ce].neigh_grainname[1] )

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
          ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[ce].neigh_grainname[0], cEdge[ce].neigh_grainname[1] )

          new_node0 = tv0;
          cnumT++;
          ctid++;
        }
      }
    }
    delete[] loop;
  }
  *tout = ctid;
  delete[] burnt;
  delete[] burnt_list;
  delete[] count;
}

void SurfaceMeshFunc::get_case2_triangles(int site, int *ae, int nedge, int *afc, int nfctr, int tin, int *tout)
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
        ADD_TRIANGLE(cTriangle, ctid, cEdge[te0].node_id[0], cEdge[te1].node_id[0], cEdge[te1].node_id[1], cEdge[te0].neigh_grainname[0], cEdge[te0].neigh_grainname[1] )

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
        ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[te0].neigh_grainname[0], cEdge[te0].neigh_grainname[1] )

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
            ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[ce].neigh_grainname[0], cEdge[ce].neigh_grainname[1] )

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
            ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[ce].neigh_grainname[0], cEdge[ce].neigh_grainname[1] )

            new_node0 = tv0;
            cnumT++;
            ctid++;
          }
        }
      }
      delete[] burnt_loop;
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
        ADD_TRIANGLE(cTriangle, ctid, cEdge[te0].node_id[0], cEdge[te1].node_id[0], cEdge[te2].node_id[0], cEdge[te0].neigh_grainname[0], cEdge[te0].neigh_grainname[1] )

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
        ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[te0].neigh_grainname[0], cEdge[te0].neigh_grainname[1] )

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
            ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[ce].neigh_grainname[0], cEdge[ce].neigh_grainname[1] )

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
            ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[ce].neigh_grainname[0], cEdge[ce].neigh_grainname[1] )

            new_node0 = tv0;
            cnumT++;
            ctid++;
          }
        }

      }
      delete[] burnt_loop;
    }
  }
  *tout = ctid;
  delete[] burnt;
  delete[] burnt_list;
  delete[] count;
}
void SurfaceMeshFunc::get_caseM_triangles(int site, int *ae, int nedge, int *afc, int nfctr, int tin, int *tout, int ccn)
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
        ADD_TRIANGLE(cTriangle, ctid, ccn, tn0, tn1, ts0, ts1)

        ctid++;
      }
      delete[] burnt_loop;
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
        ADD_TRIANGLE(cTriangle, ctid, cEdge[te0].node_id[0], cEdge[te1].node_id[0], cEdge[te2].node_id[0], cEdge[te0].neigh_grainname[0], cEdge[te0].neigh_grainname[1])

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
        ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[te0].neigh_grainname[0], cEdge[te0].neigh_grainname[1])

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
            ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[ce].neigh_grainname[0], cEdge[ce].neigh_grainname[1])

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
            ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[ce].neigh_grainname[0], cEdge[ce].neigh_grainname[1])

            cTriangle.resize(ctid + 1);
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
      delete[] burnt_loop;
    }
  }
  *tout = ctid;
  delete[] burnt;
  delete[] burnt_list;
  delete[] count;
}

void SurfaceMeshFunc::arrange_grainnames(int numT, int zID)
{
  int i, j;
  int cnode;
  int csite, kind;
  int tsite1[3], tsite2[3];
  int ngrainname1, ngrainname2;
  int tgrainname1[3], tgrainname2[3];
  float cx, cy, cz;
  float xSum, ySum, zSum;
  float vcoord[3][3];
  float u[3], w[3];
  float x, y, z;
  float a, b, c, d, length;
  float sidecheck;
  int shift = (zID * NSP);
  int locale;
  for (i = 0; i < numT; i++)
  { // for each triangle...
    xSum = 0.0;
    ySum = 0.0;
    zSum = 0.0;
    ngrainname1 = cTriangle[i].ngrainname[0];
    ngrainname2 = cTriangle[i].ngrainname[1];
    cTriangle[i].ngrainname[0] = -1;
    cTriangle[i].ngrainname[1] = -1;
    for (j = 0; j < 3; j++)
    { // for each node inside the triangle...
      tsite1[j] = -1;
      tsite2[j] = -1;
      tgrainname1[j] = -1;
      tgrainname2[j] = -1;
      cnode = cTriangle[i].node_id[j];
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
        tsite1[j] = csite;
        tsite2[j] = neigh[csite].neigh_id[1];
        tgrainname1[j] = voxels[tsite1[j]];
        tgrainname2[j] = voxels[tsite2[j]];
      }
      else if (kind == 1)
      {
        tsite1[j] = csite;
        tsite2[j] = neigh[csite].neigh_id[7];
        tgrainname1[j] = voxels[tsite1[j]];
        tgrainname2[j] = voxels[tsite2[j]];
      }
      else if (kind == 2)
      {
        tsite1[j] = csite;
        tsite2[j] = neigh[csite].neigh_id[18];
        tgrainname1[j] = voxels[tsite1[j]];
        tgrainname2[j] = voxels[tsite2[j]];
      }
    }
    // Getting the center of triangle...
    cx = xSum / 3.0;
    cy = ySum / 3.0;
    cz = zSum / 3.0;
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
    a = a / length;
    b = b / length;
    c = c / length;
    if (fabs(a) < 0.00001) a = 0.0;
    if (fabs(b) < 0.00001) b = 0.0;
    if (fabs(c) < 0.00001) c = 0.0;
    // update patch info...
    cTriangle[i].normal[0] = a;
    cTriangle[i].normal[1] = b;
    cTriangle[i].normal[2] = c;
    cTriangle[i].area = 0.5 * length;
    // determine which way normal should point...
    d = -(a * cx + b * cy + c * cz);
    for (int j = 0; j < 3; j++)
    {
      if (tsite1[j] != -1)
      {
        locale = tsite1[j] + shift;
        x = find_xcoord(locale);
        y = find_ycoord(locale);
        z = find_zcoord(locale);
        sidecheck = (a * x + b * y + c * z + d);
        if (sidecheck < -0.000001)
        {
          cTriangle[i].ngrainname[0] = tgrainname2[j];
          cTriangle[i].ngrainname[1] = tgrainname1[j];
        }
        else if (sidecheck > 0.000001)
        {
          cTriangle[i].ngrainname[0] = tgrainname1[j];
          cTriangle[i].ngrainname[1] = tgrainname2[j];
        }
      }
    }
    int k = 0;
    int index = 0;
    int testtsite = 0;
    //int sidechecked = 1;
    while (cTriangle[i].ngrainname[0] == -1 && k < 6)
    {
      while (tsite1[k] == -1)
      {
        k++;
      }
      if (k == 3) a = -a, b = -b, c = -c;
      if (k < 3) index = k;
      if (k >= 3) index = k - 3;
      if (a != 0 && (tsite1[index] % xDim + a) >= 0 && (tsite1[index] % xDim + a) < xDim) testtsite = tsite1[index] + (a * 1);
      if (b != 0 && ((tsite1[index] / xDim) % yDim + b) >= 0 && ((tsite1[index] / xDim) % yDim + b) < yDim) testtsite = tsite1[index] + (b * xDim);
      if (c != 0 && (tsite1[index] / (xDim * yDim) + c) >= 0 && (tsite1[index] / (xDim * yDim) + c) < 2) testtsite = tsite1[index] + (c * NSP);
      int gname = voxels[testtsite];
      if (gname == tgrainname1[index])
      {
        if (k < 3) cTriangle[i].ngrainname[0] = tgrainname1[index], cTriangle[i].ngrainname[1] = tgrainname2[index];
        if (k >= 3) cTriangle[i].ngrainname[0] = tgrainname2[index], cTriangle[i].ngrainname[1] = tgrainname1[index];
      }
      if (gname == tgrainname2[index])
      {
        if (k < 3) cTriangle[i].ngrainname[0] = tgrainname2[index], cTriangle[i].ngrainname[1] = tgrainname1[index];
        if (k >= 3) cTriangle[i].ngrainname[0] = tgrainname1[index], cTriangle[i].ngrainname[1] = tgrainname2[index];
      }
      if (gname != tgrainname1[index] && gname != tgrainname2[index]) k++;
    }
  }
}
int SurfaceMeshFunc::assign_nodeID(int nN)
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
    plane = i % 7;
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
int SurfaceMeshFunc::writeNodesFile(int zID, int cNodeID, const std::string &nodesFile)
{
//  int tID;
//  int nk;

  int count;
  count = 0;

  unsigned char data[32];
  int* tId = (int*)(&data[0]);
  int* nk = (int*)(&data[4]);
  float* vec3f = (float*)(&data[8]);
  size_t totalWritten = 0;
  FILE* f = NULL;

  // Create a new file if this is our first slice
  if (zID == 0)
  {
    f = fopen(nodesFile.c_str(), "wb");
    if (NULL == f)
    {
      return -1;
    }
  }
  // Append to existing file if we are on z>0 slice
  if (zID > 0)
  {
    f = fopen(nodesFile.c_str(), "ab");
    if (NULL == f)
    {
      return -1;
    }
  }
  int total = (7 * 2 * NSP);
  for (int k = 0; k < total; k++)
  {
    *tId = cVertex[k].NodeID;
    if (*tId > cNodeID - 1)
    {
      *nk = cVertex[k].nodeKind;
      vec3f[0] = cVertex[k].xc;
      vec3f[1] = cVertex[k].yc;
      vec3f[2] = cVertex[k].zc;
//      fprintf(f, "%d %d %f %f %f\n", tID, nk, x, y, z);
      totalWritten = fwrite(data, sizeof(unsigned char), 32, f);
    }
  }
  fclose(f);
  return 0;
}

// -----------------------------------------------------------------------------
//  Write a BINARY file which is only TEMP during the surface meshing
// -----------------------------------------------------------------------------
int SurfaceMeshFunc::writeTrianglesFile(int zID, int ctid, const std::string &trianglesFile, int nt)
{

  //int i;
  int tag;
  int end;
  // int newID;
  int n1, n2, n3;
  int data[6];

  tag = zID;
  end = nt;
  data[0] = ctid;
  FILE* f = NULL;

  // Create a new file if this is our first slice
  if (zID == 0)
  {
    f = fopen(trianglesFile.c_str(), "wb");
    if (NULL == f)
    {
      return -1;
    }
  }
  // Append to existing file if we are on z>0 slice
  if (zID > 0)
  {
    f = fopen(trianglesFile.c_str(), "ab");
    if (NULL == f)
    {
      return -1;
    }
  }
  size_t totalWritten = 0;
  //  outFile << nt <<endl;
  for (int i = 0; i < end; i++)
  {
    n1 = cTriangle[i].node_id[0];
    n2 = cTriangle[i].node_id[1];
    n3 = cTriangle[i].node_id[2];
    data[1] = cVertex[n1].NodeID;
    data[2] = cVertex[n2].NodeID;
    data[3] = cVertex[n3].NodeID;
    data[4] = cTriangle[i].ngrainname[0];
    data[5] = cTriangle[i].ngrainname[1];
    totalWritten = fwrite(data, sizeof(int), 6, f);
    if (totalWritten != 6)
    {
      std::cout << "Error Writing Triangles Temp File. Not enough elements written. Wrote " << totalWritten << " of 6." << std::endl;
      return -1;
    }

//    fprintf(f, "%d %d %d %d %d %d\n", newID, n1, n2, n3, s1, s2);
    data[0] = data[0] + 1;
  }
  fclose(f);
  return 0;
}

