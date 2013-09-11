/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force, 
 * BlueQuartz Software nor the names of its contributors may be used to endorse 
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

// Broderick-Groeber Diagram Maker.cpp : Defines the entry point for the console application.
//


#include <stdlib.h>

#include <iostream>
#include <cmath>
#include <fstream>
#include <list>
#include <algorithm>
#include <numeric>
#include <sstream>

#include "MeshGenerator.h"

using namespace std;

namespace VolMesh {
nodes *originalnode;
nodes *finalnode;
triangles *originaltriangle;
triangles *finaltriangle;
elements *element;
grains *grain;
float *grainvol;
float ****zbuffer;
int ***voxels;
int *graincheck;
QVector< QVector<int > > nodesvector;
QVector< QVector<int > > trianglesvector;
QVector< QVector<int > > gvector;
QVector< QVector<int > > tvector;
QVector< QVector<int > > nnvector;
QVector< QVector<int > > elvector;
int dihedralanglesurf[180];
int dihedralanglebulk[180];
int volumesurf[25];
int volumebulk[25];
int badedge[1000000][2];
int numsurfnodes=0;
int numnodes=0;
int numtriangles=0;
int numelements=0;
int elementcount=0;
int numcycles=4;
float xdim=0.0f;
float ydim=0.0f;
float zdim=0.0f;
float xres=0.0f;
float yres=0.0f;
float zres=0.0f;
float globalxmin = 0.5f;
float globalymin = 0.5f;
float globalzmin = 0.5f;
float globalxmax = 48.5f;
float globalymax = 48.5f;
float globalzmax = 48.5f;
float pi = 3.1415926535897f;
float radtodegree = 180.0f/3.1415926535897f;
int numxvoxels=0;
int numyvoxels=0;
int numzvoxels=0;
int numgrains=0;
int numgoodsurfnodes = 0;
float globalvolmeshvol = 0.0f;
float globalsurfmeshvol = 0.0f;
float avgarea = 0.0f;
int32_t seeder = time(0);
DREAM3DRandom rg;
}


using namespace VolMesh;

#ifdef VOLUME_MESH_LIBRARY
int MeshGenerator_Main(const QString &readname1, const QString &readname2,
                    const QString &writename1, const QString &writename1a,
                    const QString &writename2, const QString &writename3,
                    float xDim, float yDim, float zDim,
                    float xRes, float yRes, float zRes,
                    int numGrains)
{

  xdim = xDim;
  ydim = yDim;
  zdim = zDim;
  xres = xRes;
  yres = yRes;
  zres = zRes;
  numgrains = numGrains;

#else
int main(int argc, char **argv)
{
  rg.RandomInit(seeder);
  string readname1;
  string readname2;
  string writename1;
  string writename1a;
  string writename2;
  string writename3;

  cout << "Enter the filename to read nodes from: ";
  cin >> readname1;
  cout << "Enter the filename to read triangles from: ";
  cin >> readname2;
  cout << "Enter the x dimension of the box:";
  cin >> xdim;
  cout << "Enter the y dimension of the box:";
  cin >> ydim;
  cout << "Enter the z dimension of the box:";
  cin >> zdim;
  cout << "Enter the x resolution of the box:";
  cin >> xres;
  cout << "Enter the y resolution of the box:";
  cin >> yres;
  cout << "Enter the z resolution of the box:";
  cin >> zres;
  cout << "Enter the number of grains in the box:";
  cin >> numgrains;
  cout << "Enter the filename to write triangle data to: ";
  cin >> writename1;
  cout << "Enter the filename to write mesh data to: ";
  cin >> writename2;
  cout << "Enter the filename to write dihedral data to: ";
  cin >> writename3;

  writename1 = "volumetric_mesh_v5_1.vtk";
  writename1a = "volumetric_mesh_v5_2.vtk";
  writename2 = "element_quality_measures_v5.txt";
  writename3 = "voxels_v5.txt";

#endif

  //  readname1 = "nodes_reconstructed_v1.txt";
  //  readname2 = "triangles_reconstructed_v1.txt";
  //  readname1 = "nodes_reconstructed_v2.txt";
  //  readname2 = "triangles_reconstructed_v2.txt";
  //  readname1 = "nodes_reconstructed_v3.txt";
  //  readname2 = "triangles_reconstructed_v3.txt";
  //  readname1 = "nodes_reconstructed_v5.txt";
  //  readname2 = "triangles_reconstructed_v5.txt";
  //  readname1 = "nodes.txt";
  //  readname2 = "triangles.txt";
  //  xdim = 48;
  //  ydim = 48;
  //  zdim = 48;
  //  xres = 0.25;
  //  yres = 0.25;
  //  zres = 0.25;
  numxvoxels = xdim / xres;
  numyvoxels = ydim / yres;
  numzvoxels = zdim / zres;
  //  numgrains = 50;


  ifstream inputFile1;
  inputFile1.open(readname1.c_str(), std::ios_base::binary);
  inputFile1 >> numsurfnodes;
  inputFile1.close();
  ifstream inputFile2;
  inputFile2.open(readname2.c_str(), std::ios_base::binary);
  inputFile2 >> numtriangles;
  inputFile2.close();

  gvector.resize(numsurfnodes * 100);
  tvector.resize(numsurfnodes * 100);
  nnvector.resize(numsurfnodes * 100);
  elvector.resize(numsurfnodes * 100);
  trianglesvector.resize(numgrains + 1);
  nodesvector.resize(numgrains + 1);
  voxels = new int **[numxvoxels];
  for (int a = 0; a < numxvoxels; a++)
  {
    voxels[a] = new int *[numyvoxels];
    for (int b = 0; b < numyvoxels; b++)
    {
      voxels[a][b] = new int[numzvoxels];
      for (int c = 0; c < numzvoxels; c++)
      {
        voxels[a][b][c] = 0;
      }
    }
  }
  zbuffer = new float ***[numgrains + 1];
  for (int a = 1; a < numgrains + 1; a++)
  {
    zbuffer[a] = new float **[6];
    for (int b = 0; b < 6; b++)
    {
      zbuffer[a][b] = new float *[500];
      for (int c = 0; c < 500; c++)
      {
        zbuffer[a][b][c] = new float[500];
        for (int d = 0; d < 500; d++)
        {
          float initval = 0;
          if (b == 0 || b == 2 || b == 4)
            initval = 10000000;
          if (b == 1 || b == 3 || b == 5)
            initval = 0;
          zbuffer[a][b][c][d] = initval;
        }
      }
    }
  }
  graincheck = new int[numgrains + 1];
  originalnode = new nodes[numsurfnodes];
  finalnode = new nodes[numsurfnodes * 100];
  originaltriangle = new triangles[numtriangles];
  finaltriangle = new triangles[numtriangles];
  grain = new grains[numgrains + 1];
  grainvol = new float[numgrains + 1];
  for (int iter = 0; iter < numgrains + 1; iter++)
  {
    grainvol[iter] = 0;
  }
  read_nodes(readname1);
  read_triangles(readname2);
  locate_graincenters();
  //delete_triangles();
  //clean_triangles();
  create_finalnodesandtriangles();
  find_volumes();
  for (int iter = 1; iter < numgrains + 1; iter++)
  {
    make_nodes(iter);
  }
  for (int idea = 0; idea < numcycles; idea++)
  {
    write_nodes();
    // int test = system("D:\\Groeber\\C++_Codes_2005\\MeshGenerator\\MeshGenerator\\qdelaunay < points.txt s i TO tets.txt");
    string filein;
    filein = "tets.txt";
    ifstream inputFile3;
    inputFile3.open(filein.c_str(), std::ios_base::binary);
    inputFile3 >> numelements;
    inputFile3.close();
    element = new elements[numelements];
    read_elements(idea);
    if (idea < (numcycles - 1))
      delete[] element;
  }
  measure_elements();
  // improve_mesh();
  // measure_elements();
  write_meshdata(writename1);
  write_dihedralangles(writename2);
  write_voxeldata(writename3);
  return 0;
}

void read_nodes(string inname1)
{
  ifstream inputFile;
  inputFile.open(inname1.c_str(), std::ios_base::binary);
  float x;
  float y;
  float z;
  int nodenum;
  int ngrains;
  inputFile >> numsurfnodes;
  int onedge = 0;
  int killed = 0;
  int ntnode = 0;
  int tnode = 1;
  for (int i = 0; i < numsurfnodes; i++)
  {
    onedge = 0;
    inputFile >> nodenum >> ngrains >> x >> y >> z;
    originalnode[nodenum].set_coords(x, y, z);
    originalnode[nodenum].set_killed(killed);
    originalnode[nodenum].set_numgrains(ngrains);
    originalnode[nodenum].set_triplenode(ntnode);
    if (ngrains > 2)
      originalnode[nodenum].set_triplenode(tnode);
  }
}

void read_triangles(string inname3)
{
  ifstream inputFile;
  inputFile.open(inname3.c_str(), std::ios_base::binary);
  int trianglenum;
  int node1;
  int node2;
  int node3;
  int edge1;
  int edge2;
  int edge3;
  int grain1;
  int grain2;
  int killed = 0;
  int ttriangle = 1;
  int nttriangle = 0;
  int onedge = 0;
  inputFile >> numtriangles;
  for (int i = 0; i < numtriangles; i++)
  {
    onedge = 0;
    inputFile >> trianglenum >> node1 >> node2 >> node3 >> edge1 >> edge2 >> edge3 >> grain1 >> grain2;
    if (grain1 < 0)
      grain1 = 0, onedge = 1;
    if (grain2 < 0)
      grain2 = 0, onedge = 1;
    float x1 = originalnode[node1].xc;
    float y1 = originalnode[node1].yc;
    float z1 = originalnode[node1].zc;
    float x2 = originalnode[node2].xc;
    float y2 = originalnode[node2].yc;
    float z2 = originalnode[node2].zc;
    float x3 = originalnode[node3].xc;
    float y3 = originalnode[node3].yc;
    float z3 = originalnode[node3].zc;
    float x12 = x2 - x1;
    float y12 = y2 - y1;
    float z12 = z2 - z1;
    float x13 = x3 - x1;
    float y13 = y3 - y1;
    float z13 = z3 - z1;
    float x21 = x1 - x2;
    float y21 = y1 - y2;
    float z21 = z1 - z2;
    float x23 = x3 - x2;
    float y23 = y3 - y2;
    float z23 = z3 - z2;
    float x31 = x1 - x3;
    float y31 = y1 - y3;
    float z31 = z1 - z3;
    float x32 = x2 - x3;
    float y32 = y2 - y3;
    float z32 = z2 - z3;
    float nx = (y12 * z13 - z12 * y13);
    float ny = (z12 * x13 - x12 * z13);
    float nz = (x12 * y13 - y12 * x13);
    float denom = powf((nx * nx + ny * ny + nz * nz), 0.5);
    nx = nx / denom;
    ny = ny / denom;
    nz = nz / denom;
    float dval = (x1 * nx + y1 * ny + z1 * nz);
    float area = denom / 2.0f;
    avgarea = avgarea + area;
    float denom12 = (x12 * x12) + (y12 * y12) + (z12 * z12);
    float denom13 = (x13 * x13) + (y13 * y13) + (z13 * z13);
    float denom21 = (x21 * x21) + (y21 * y21) + (z21 * z21);
    float denom23 = (x23 * x23) + (y23 * y23) + (z23 * z23);
    float denom31 = (x31 * x31) + (y31 * y31) + (z31 * z31);
    float denom32 = (x32 * x32) + (y32 * y32) + (z32 * z32);
    denom12 = powf(denom12, 0.5);
    denom13 = powf(denom13, 0.5);
    denom21 = powf(denom21, 0.5);
    denom23 = powf(denom23, 0.5);
    denom31 = powf(denom31, 0.5);
    denom32 = powf(denom32, 0.5);
    x12 = x12 / denom12;
    y12 = y12 / denom12;
    z12 = z12 / denom12;
    x13 = x13 / denom13;
    y13 = y13 / denom13;
    z13 = z13 / denom13;
    x21 = x21 / denom21;
    y21 = y21 / denom21;
    z21 = z21 / denom21;
    x23 = x23 / denom23;
    y23 = y23 / denom23;
    z23 = z23 / denom23;
    x31 = x31 / denom31;
    y31 = y31 / denom31;
    z31 = z31 / denom31;
    x32 = x32 / denom32;
    y32 = y32 / denom32;
    z32 = z32 / denom32;
    float angle1 = (x12 * x13) + (y12 * y13) + (z12 * z13);
    float angle2 = (x21 * x23) + (y21 * y23) + (z21 * z23);
    float angle3 = (x31 * x32) + (y31 * y32) + (z31 * z32);
    angle1 = acos(angle1);
    angle2 = acos(angle2);
    angle3 = acos(angle3);
    angle1 = angle1 * radtodegree;
    angle2 = angle2 * radtodegree;
    angle3 = angle3 * radtodegree;
    float xc = (originalnode[node1].xc + originalnode[node2].xc + originalnode[node3].xc) / 3.0f;
    float yc = (originalnode[node1].yc + originalnode[node2].yc + originalnode[node3].yc) / 3.0f;
    float zc = (originalnode[node1].zc + originalnode[node2].zc + originalnode[node3].zc) / 3.0f;
    float length1 = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
    length1 = powf(length1, 0.5);
    float length2 = ((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)) + ((z1 - z3) * (z1 - z3));
    length2 = powf(length2, 0.5);
    float length3 = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)) + ((z2 - z3) * (z2 - z3));
    length3 = powf(length3, 0.5);
    float avglength = (length1 + length2 + length3) / 3.0f;
    float maxlength = max(length1, length2);
    maxlength = max(maxlength, length3);
    trianglesvector[grain1].push_back(trianglenum);
    trianglesvector[grain2].push_back(trianglenum);
    nodesvector[grain1].push_back(node1);
    nodesvector[grain1].push_back(node2);
    nodesvector[grain1].push_back(node3);
    nodesvector[grain2].push_back(node1);
    nodesvector[grain2].push_back(node2);
    nodesvector[grain2].push_back(node3);
    tvector[node1].push_back(trianglenum);
    tvector[node2].push_back(trianglenum);
    tvector[node3].push_back(trianglenum);
    gvector[node1].push_back(grain1);
    gvector[node2].push_back(grain1);
    gvector[node3].push_back(grain1);
    gvector[node1].push_back(grain2);
    gvector[node2].push_back(grain2);
    gvector[node3].push_back(grain2);
    nnvector[node1].push_back(node2);
    nnvector[node1].push_back(node3);
    nnvector[node2].push_back(node1);
    nnvector[node2].push_back(node3);
    nnvector[node3].push_back(node1);
    nnvector[node3].push_back(node2);
    originaltriangle[trianglenum].set_nodes_leftgrain(node1, node2, node3);
    originaltriangle[trianglenum].set_nodes_rightgrain(node1, node3, node2);
    originaltriangle[trianglenum].set_grains(grain1, grain2);
    originaltriangle[trianglenum].set_normal(nx, ny, nz);
    originaltriangle[trianglenum].set_center(xc, yc, zc);
    originaltriangle[trianglenum].set_edgelengths(length1, length2, length3);
    originaltriangle[trianglenum].set_averageedgelength(avglength);
    originaltriangle[trianglenum].set_maxedgelength(maxlength);
    originaltriangle[trianglenum].set_killed(killed);
    originaltriangle[trianglenum].set_area(area);
    originaltriangle[trianglenum].set_facetD(dval);
    originaltriangle[trianglenum].set_tripletriangle(nttriangle);
    originaltriangle[trianglenum].set_onedge(onedge);
    if ((originalnode[node1].triplenode + originalnode[node2].triplenode + originalnode[node3].triplenode) >= 2)
      originaltriangle[trianglenum].set_tripletriangle(ttriangle);
  }
  for (int j = 0; j < numgrains + 1; j++)
  {
    vector<int>::iterator newend;
    sort(nodesvector[j].begin(), nodesvector[j].end());
    newend = unique(nodesvector[j].begin(), nodesvector[j].end());
    nodesvector[j].erase(newend, nodesvector[j].end());
    sort(trianglesvector[j].begin(), trianglesvector[j].end());
    newend = unique(trianglesvector[j].begin(), trianglesvector[j].end());
    trianglesvector[j].erase(newend, trianglesvector[j].end());
  }
  for (int j = 0; j < numsurfnodes; j++)
  {
    vector<int>::iterator newend;
    sort(tvector[j].begin(), tvector[j].end());
    newend = unique(tvector[j].begin(), tvector[j].end());
    tvector[j].erase(newend, tvector[j].end());
    sort(gvector[j].begin(), gvector[j].end());
    newend = unique(gvector[j].begin(), gvector[j].end());
    gvector[j].erase(newend, gvector[j].end());
    sort(nnvector[j].begin(), nnvector[j].end());
    newend = unique(nnvector[j].begin(), nnvector[j].end());
    nnvector[j].erase(newend, nnvector[j].end());
  }
  avgarea = avgarea / numtriangles;
  inputFile.close();
}

void locate_graincenters()
{
  float xcenter = 0;
  float ycenter = 0;
  float zcenter = 0;
  int onedge = 0;
  for (int i = 1; i < numgrains + 1; i++)
  {
    onedge = 0;
    xcenter = 0;
    ycenter = 0;
    zcenter = 0;
    int size = nodesvector[i].size();
    for (int j = 0; j < size; j++)
    {
      int snode = nodesvector[i][j];
      xcenter = xcenter + originalnode[snode].xc;
      ycenter = ycenter + originalnode[snode].yc;
      zcenter = zcenter + originalnode[snode].zc;
      if (originalnode[snode].edgenode == 1)
        onedge = 1;
    }
    xcenter = xcenter / size;
    ycenter = ycenter / size;
    zcenter = zcenter / size;
    grain[i].set_centroid(xcenter, ycenter, zcenter);
    grain[i].set_onedge(onedge);
  }
}
void delete_triangles()
{
  list<int> affectedtrianglelist;
  int count = 0;
  int keep = 0;
  int killed = 1;
  for (int i = 0; i < numtriangles; i++)
  {
    if (originaltriangle[i].trianglekilled != 1 && originaltriangle[i].edgetriangle != 1)
    {
      int node1 = originaltriangle[i].firstnodeleft;
      int node2 = originaltriangle[i].secondnodeleft;
      int node3 = originaltriangle[i].thirdnodeleft;
      int grain1 = originaltriangle[i].leftgrain;
      int grain2 = originaltriangle[i].rightgrain;
      for (int j = 0; j < numtriangles; j++)
      {
        int n1 = originaltriangle[j].firstnodeleft;
        int n2 = originaltriangle[j].secondnodeleft;
        int n3 = originaltriangle[j].thirdnodeleft;
        int g1 = originaltriangle[j].leftgrain;
        int g2 = originaltriangle[j].rightgrain;
        int sameboundary = 0;
        if (g1 == grain1 && g2 == grain2)
          sameboundary = 1;
        if (g1 == grain2 && g2 == grain1)
          sameboundary = 1;
        int shared = 0;
        if (n1 == node1)
          shared++;
        if (n1 == node2)
          shared++;
        if (n1 == node3)
          shared++;
        if (n2 == node1)
          shared++;
        if (n2 == node2)
          shared++;
        if (n2 == node3)
          shared++;
        if (n3 == node1)
          shared++;
        if (n3 == node2)
          shared++;
        if (n3 == node3)
          shared++;
        if (originaltriangle[j].trianglekilled != 1 && i != j && sameboundary == 1)
        {
          if (shared > 0)
          {
            affectedtrianglelist.push_back(j);
          }
        }
      }
      keep = 0;
      float centernx = originaltriangle[i].normalx;
      float centerny = originaltriangle[i].normaly;
      float centernz = originaltriangle[i].normalz;
      int edgeorder[3];
      float x1 = originalnode[node1].xc;
      float y1 = originalnode[node1].yc;
      float z1 = originalnode[node1].zc;
      float x2 = originalnode[node2].xc;
      float y2 = originalnode[node2].yc;
      float z2 = originalnode[node2].zc;
      float x3 = originalnode[node3].xc;
      float y3 = originalnode[node3].yc;
      float z3 = originalnode[node3].zc;
      float midedge1x = (x1 + x2) / 2;
      float midedge1y = (y1 + y2) / 2;
      float midedge1z = (z1 + z2) / 2;
      float midedge2x = (x1 + x3) / 2;
      float midedge2y = (y1 + y3) / 2;
      float midedge2z = (z1 + z3) / 2;
      float midedge3x = (x2 + x3) / 2;
      float midedge3y = (y2 + y3) / 2;
      float midedge3z = (z2 + z3) / 2;
      float edgelength1 = originaltriangle[i].edgelength1;
      float edgelength2 = originaltriangle[i].edgelength2;
      float edgelength3 = originaltriangle[i].edgelength3;
      if (edgelength1 <= edgelength2 && edgelength1 <= edgelength3)
        edgeorder[0] = 1;
      if (edgelength2 <= edgelength1 && edgelength2 <= edgelength3)
        edgeorder[0] = 2;
      if (edgelength3 <= edgelength1 && edgelength3 <= edgelength2)
        edgeorder[0] = 3;
      if (edgeorder[0] == 1 && edgelength2 <= edgelength3)
        edgeorder[1] = 2, edgeorder[2] = 3;
      if (edgeorder[0] == 1 && edgelength3 <= edgelength2)
        edgeorder[1] = 3, edgeorder[2] = 2;
      if (edgeorder[0] == 2 && edgelength1 <= edgelength3)
        edgeorder[1] = 1, edgeorder[2] = 3;
      if (edgeorder[0] == 2 && edgelength3 <= edgelength1)
        edgeorder[1] = 3, edgeorder[2] = 1;
      if (edgeorder[0] == 3 && edgelength1 <= edgelength2)
        edgeorder[1] = 1, edgeorder[2] = 2;
      if (edgeorder[0] == 3 && edgelength2 <= edgelength1)
        edgeorder[1] = 2, edgeorder[2] = 1;
      int size = 0;
      float avgmisorient = 0;
      while (!affectedtrianglelist.empty() && keep == 0)
      {
        int trianglenum = affectedtrianglelist.front();
        if (originaltriangle[trianglenum].trianglekilled != 1)
        {
          size++;
          int leftgrain1 = originaltriangle[i].leftgrain;
          int leftgrain2 = originaltriangle[trianglenum].leftgrain;
          int rightgrain2 = originaltriangle[trianglenum].rightgrain;
          int normalfix2 = 1;
          if (leftgrain1 == leftgrain2)
            normalfix2 = 1;
          if (leftgrain1 == rightgrain2)
            normalfix2 = -1;
          float nx = normalfix2 * originaltriangle[trianglenum].normalx;
          float ny = normalfix2 * originaltriangle[trianglenum].normaly;
          float nz = normalfix2 * originaltriangle[trianglenum].normalz;
          float angle = nx * centernx + ny * centerny + nz * centernz;
          angle = acos(angle);
          angle = 180.0f * angle / 3.1415926535897;
          angle = 180.0f - angle;
          if (angle < 165)
            keep = 1;
          avgmisorient = avgmisorient + angle;
        }
        affectedtrianglelist.pop_front();
      }
      avgmisorient = avgmisorient / size;
      originaltriangle[i].set_avgmisorientation(avgmisorient);
      if (avgmisorient <= 172.5)
        keep = 1;
      if (originaltriangle[i].trianglearea < (0.25 * avgarea))
        keep = 0;
      if (keep == 0)
      {
        count++;
        if (edgeorder[0] == 1)
        {
          originaltriangle[i].set_killed(killed);
          if (originalnode[node2].numgrains > originalnode[node1].numgrains)
          {
            int temp = node1;
            node1 = node2;
            node2 = temp;
          }
          originalnode[node1].set_coords(midedge1x, midedge1y, midedge1z);
          originalnode[node2].set_killed(killed);
          int size = tvector[node2].size();
          for (int l = 0; l < size; l++)
          {
            int firsttriangle = tvector[node2][l];
            if (originaltriangle[firsttriangle].trianglekilled != 1)
            {
              int affected = 0;
              int n1 = originaltriangle[firsttriangle].firstnodeleft;
              int n2 = originaltriangle[firsttriangle].secondnodeleft;
              int n3 = originaltriangle[firsttriangle].thirdnodeleft;
              if (n1 == node2)
                n1 = node1, affected = 1;
              if (n2 == node2)
                n2 = node1, affected = 1;
              if (n3 == node2)
                n3 = node1, affected = 1;
              originaltriangle[firsttriangle].set_nodes_leftgrain(n1, n2, n3);
              originaltriangle[firsttriangle].set_nodes_rightgrain(n1, n3, n2);
              if (n1 == n2 || n1 == n3 || n2 == n3)
                originaltriangle[firsttriangle].set_killed(killed);
              if (affected == 1 && originaltriangle[firsttriangle].trianglekilled != 1)
              {
                tvector[node1].push_back(firsttriangle);
              }
              float a1 = originalnode[n2].xc - originalnode[n1].xc;
              float a2 = originalnode[n2].yc - originalnode[n1].yc;
              float a3 = originalnode[n2].zc - originalnode[n1].zc;
              float b1 = originalnode[n3].xc - originalnode[n1].xc;
              float b2 = originalnode[n3].yc - originalnode[n1].yc;
              float b3 = originalnode[n3].zc - originalnode[n1].zc;
              float nx = (a2 * b3 - a3 * b2);
              float ny = (a3 * b1 - a1 * b3);
              float nz = (a1 * b2 - a2 * b1);
              float denom = powf((nx * nx + ny * ny + nz * nz), 0.5);
              nx = nx / denom;
              ny = ny / denom;
              nz = nz / denom;
              float area = denom / 2.0f;
              float xc = (originalnode[n1].xc + originalnode[n2].xc + originalnode[n3].xc) / 3.0f;
              float yc = (originalnode[n1].yc + originalnode[n2].yc + originalnode[n3].yc) / 3.0f;
              float zc = (originalnode[n1].zc + originalnode[n2].zc + originalnode[n3].zc) / 3.0f;
              float length1 = ((originalnode[n1].xc - originalnode[n2].xc) * (originalnode[n1].xc - originalnode[n2].xc)) + ((originalnode[n1].yc
                  - originalnode[n2].yc) * (originalnode[n1].yc - originalnode[n2].yc)) + ((originalnode[n1].zc - originalnode[n2].zc) * (originalnode[n1].zc
                  - originalnode[n2].zc));
              length1 = powf(length1, 0.5);
              float length2 = ((originalnode[n1].xc - originalnode[n3].xc) * (originalnode[n1].xc - originalnode[n3].xc)) + ((originalnode[n1].yc
                  - originalnode[n3].yc) * (originalnode[n1].yc - originalnode[n3].yc)) + ((originalnode[n1].zc - originalnode[n3].zc) * (originalnode[n1].zc
                  - originalnode[n3].zc));
              length2 = powf(length2, 0.5);
              float length3 = ((originalnode[n2].xc - originalnode[n3].xc) * (originalnode[n2].xc - originalnode[n3].xc)) + ((originalnode[n2].yc
                  - originalnode[n3].yc) * (originalnode[n2].yc - originalnode[n3].yc)) + ((originalnode[n2].zc - originalnode[n3].zc) * (originalnode[n2].zc
                  - originalnode[n3].zc));
              length3 = powf(length3, 0.5);
              float avglength = (length1 + length2 + length3) / 3.0f;
              float maxlength = length1;
              if (length2 > maxlength)
                maxlength = length2;
              if (length3 > maxlength)
                maxlength = length3;
              originaltriangle[firsttriangle].set_normal(nx, ny, nz);
              originaltriangle[firsttriangle].set_center(xc, yc, zc);
              originaltriangle[firsttriangle].set_area(area);
              originaltriangle[firsttriangle].set_averageedgelength(avglength);
              originaltriangle[firsttriangle].set_maxedgelength(maxlength);
            }
          }
          size = nnvector[node2].size();
          for (int l = 0; l < size; l++)
          {
            int firstneighbornode = nnvector[node2][l];
            nnvector[node1].push_back(firstneighbornode);
          }
        }
        if (edgeorder[0] == 2)
        {
          originaltriangle[i].set_killed(killed);
          if (originalnode[node3].numgrains > originalnode[node1].numgrains)
          {
            int temp = node1;
            node1 = node3;
            node3 = temp;
          }
          originalnode[node1].set_coords(midedge2x, midedge2y, midedge2z);
          originalnode[node3].set_killed(killed);
          int size = tvector[node3].size();
          for (int l = 0; l < size; l++)
          {
            int firsttriangle = tvector[node3][l];
            if (originaltriangle[firsttriangle].trianglekilled != 1)
            {
              int affected = 0;
              int n1 = originaltriangle[firsttriangle].firstnodeleft;
              int n2 = originaltriangle[firsttriangle].secondnodeleft;
              int n3 = originaltriangle[firsttriangle].thirdnodeleft;
              if (n1 == node3)
                n1 = node1, affected = 1;
              if (n2 == node3)
                n2 = node1, affected = 1;
              if (n3 == node3)
                n3 = node1, affected = 1;
              originaltriangle[firsttriangle].set_nodes_leftgrain(n1, n2, n3);
              originaltriangle[firsttriangle].set_nodes_rightgrain(n1, n3, n2);
              if (n1 == n2 || n1 == n3 || n2 == n3)
                originaltriangle[firsttriangle].set_killed(killed);
              if (affected == 1 && originaltriangle[firsttriangle].trianglekilled != 1)
              {
                tvector[node1].push_back(firsttriangle);
              }
              float a1 = originalnode[n2].xc - originalnode[n1].xc;
              float a2 = originalnode[n2].yc - originalnode[n1].yc;
              float a3 = originalnode[n2].zc - originalnode[n1].zc;
              float b1 = originalnode[n3].xc - originalnode[n1].xc;
              float b2 = originalnode[n3].yc - originalnode[n1].yc;
              float b3 = originalnode[n3].zc - originalnode[n1].zc;
              float nx = (a2 * b3 - a3 * b2);
              float ny = (a3 * b1 - a1 * b3);
              float nz = (a1 * b2 - a2 * b1);
              float denom = powf((nx * nx + ny * ny + nz * nz), 0.5);
              nx = nx / denom;
              ny = ny / denom;
              nz = nz / denom;
              float area = denom / 2.0f;
              float xc = (originalnode[n1].xc + originalnode[n2].xc + originalnode[n3].xc) / 3.0f;
              float yc = (originalnode[n1].yc + originalnode[n2].yc + originalnode[n3].yc) / 3.0f;
              float zc = (originalnode[n1].zc + originalnode[n2].zc + originalnode[n3].zc) / 3.0f;
              float length1 = ((originalnode[n1].xc - originalnode[n2].xc) * (originalnode[n1].xc - originalnode[n2].xc)) + ((originalnode[n1].yc
                  - originalnode[n2].yc) * (originalnode[n1].yc - originalnode[n2].yc)) + ((originalnode[n1].zc - originalnode[n2].zc) * (originalnode[n1].zc
                  - originalnode[n2].zc));
              length1 = powf(length1, 0.5);
              float length2 = ((originalnode[n1].xc - originalnode[n3].xc) * (originalnode[n1].xc - originalnode[n3].xc)) + ((originalnode[n1].yc
                  - originalnode[n3].yc) * (originalnode[n1].yc - originalnode[n3].yc)) + ((originalnode[n1].zc - originalnode[n3].zc) * (originalnode[n1].zc
                  - originalnode[n3].zc));
              length2 = powf(length2, 0.5);
              float length3 = ((originalnode[n2].xc - originalnode[n3].xc) * (originalnode[n2].xc - originalnode[n3].xc)) + ((originalnode[n2].yc
                  - originalnode[n3].yc) * (originalnode[n2].yc - originalnode[n3].yc)) + ((originalnode[n2].zc - originalnode[n3].zc) * (originalnode[n2].zc
                  - originalnode[n3].zc));
              length3 = powf(length3, 0.5);
              float avglength = (length1 + length2 + length3) / 3.0f;
              float maxlength = length1;
              if (length2 > maxlength)
                maxlength = length2;
              if (length3 > maxlength)
                maxlength = length3;
              originaltriangle[firsttriangle].set_normal(nx, ny, nz);
              originaltriangle[firsttriangle].set_center(xc, yc, zc);
              originaltriangle[firsttriangle].set_area(area);
              originaltriangle[firsttriangle].set_averageedgelength(avglength);
              originaltriangle[firsttriangle].set_maxedgelength(maxlength);
            }
          }
          size = nnvector[node3].size();
          for (int l = 0; l < size; l++)
          {
            int firstneighbornode = nnvector[node3][l];
            nnvector[node1].push_back(firstneighbornode);
          }
        }
        if (edgeorder[0] == 3)
        {
          originaltriangle[i].set_killed(killed);
          if (originalnode[node3].numgrains > originalnode[node2].numgrains)
          {
            int temp = node2;
            node2 = node3;
            node3 = temp;
          }
          originalnode[node2].set_coords(midedge3x, midedge3y, midedge3z);
          originalnode[node3].set_killed(killed);
          int size = tvector[node3].size();
          for (int l = 0; l < size; l++)
          {
            int firsttriangle = tvector[node3][l];
            if (originaltriangle[firsttriangle].trianglekilled != 1)
            {
              int affected = 0;
              int n1 = originaltriangle[firsttriangle].firstnodeleft;
              int n2 = originaltriangle[firsttriangle].secondnodeleft;
              int n3 = originaltriangle[firsttriangle].thirdnodeleft;
              if (n1 == node3)
                n1 = node2, affected = 1;
              if (n2 == node3)
                n2 = node2, affected = 1;
              if (n3 == node3)
                n3 = node2, affected = 1;
              originaltriangle[firsttriangle].set_nodes_leftgrain(n1, n2, n3);
              originaltriangle[firsttriangle].set_nodes_rightgrain(n1, n3, n2);
              if (n1 == n2 || n1 == n3 || n2 == n3)
                originaltriangle[firsttriangle].set_killed(killed);
              if (affected == 1 && originaltriangle[firsttriangle].trianglekilled != 1)
              {
                tvector[node2].push_back(firsttriangle);
              }
              float a1 = originalnode[n2].xc - originalnode[n1].xc;
              float a2 = originalnode[n2].yc - originalnode[n1].yc;
              float a3 = originalnode[n2].zc - originalnode[n1].zc;
              float b1 = originalnode[n3].xc - originalnode[n1].xc;
              float b2 = originalnode[n3].yc - originalnode[n1].yc;
              float b3 = originalnode[n3].zc - originalnode[n1].zc;
              float nx = (a2 * b3 - a3 * b2);
              float ny = (a3 * b1 - a1 * b3);
              float nz = (a1 * b2 - a2 * b1);
              float denom = powf((nx * nx + ny * ny + nz * nz), 0.5);
              nx = nx / denom;
              ny = ny / denom;
              nz = nz / denom;
              float area = denom / 2.0f;
              float xc = (originalnode[n1].xc + originalnode[n2].xc + originalnode[n3].xc) / 3.0f;
              float yc = (originalnode[n1].yc + originalnode[n2].yc + originalnode[n3].yc) / 3.0f;
              float zc = (originalnode[n1].zc + originalnode[n2].zc + originalnode[n3].zc) / 3.0f;
              float length1 = ((originalnode[n1].xc - originalnode[n2].xc) * (originalnode[n1].xc - originalnode[n2].xc)) + ((originalnode[n1].yc
                  - originalnode[n2].yc) * (originalnode[n1].yc - originalnode[n2].yc)) + ((originalnode[n1].zc - originalnode[n2].zc) * (originalnode[n1].zc
                  - originalnode[n2].zc));
              length1 = powf(length1, 0.5);
              float length2 = ((originalnode[n1].xc - originalnode[n3].xc) * (originalnode[n1].xc - originalnode[n3].xc)) + ((originalnode[n1].yc
                  - originalnode[n3].yc) * (originalnode[n1].yc - originalnode[n3].yc)) + ((originalnode[n1].zc - originalnode[n3].zc) * (originalnode[n1].zc
                  - originalnode[n3].zc));
              length2 = powf(length2, 0.5);
              float length3 = ((originalnode[n2].xc - originalnode[n3].xc) * (originalnode[n2].xc - originalnode[n3].xc)) + ((originalnode[n2].yc
                  - originalnode[n3].yc) * (originalnode[n2].yc - originalnode[n3].yc)) + ((originalnode[n2].zc - originalnode[n3].zc) * (originalnode[n2].zc
                  - originalnode[n3].zc));
              length3 = powf(length3, 0.5);
              float avglength = (length1 + length2 + length3) / 3.0f;
              float maxlength = length1;
              if (length2 > maxlength)
                maxlength = length2;
              if (length3 > maxlength)
                maxlength = length3;
              originaltriangle[firsttriangle].set_normal(nx, ny, nz);
              originaltriangle[firsttriangle].set_center(xc, yc, zc);
              originaltriangle[firsttriangle].set_area(area);
              originaltriangle[firsttriangle].set_averageedgelength(avglength);
              originaltriangle[firsttriangle].set_maxedgelength(maxlength);
            }
          }
          size = nnvector[node3].size();
          for (int l = 0; l < size; l++)
          {
            int firstneighbornode = nnvector[node3][l];
            nnvector[node2].push_back(firstneighbornode);
          }
        }
      }
    }
  }
}
void clean_triangles()
{
  float nx = 0;
  float ny = 0;
  float nz = 0;
  int killed = 1;
  int counter = 0;
  for (int i = 0; i < numsurfnodes; i++)
  {
    if (originalnode[i].nodekilled != 1 && originalnode[i].edgenode != 1)
    //    if(originalnode[i].nodekilled != 1)
    {
      nx = 0;
      ny = 0;
      nz = 0;
      counter = 0;
      int size = nnvector[i].size();
      for (int l = 0; l < size; l++)
      {
        int firstneighbornode = nnvector[i][l];
        if (originalnode[firstneighbornode].nodekilled != 1)
        {
          float x = originalnode[firstneighbornode].xc;
          float y = originalnode[firstneighbornode].yc;
          float z = originalnode[firstneighbornode].zc;
          nx = nx + x;
          ny = ny + y;
          nz = nz + z;
          counter++;
        }
      }
      if (counter > 0)
      {
        nx = nx / counter;
        ny = ny / counter;
        nz = nz / counter;
        originalnode[i].set_coords(nx, ny, nz);
      }
    }
  }
  int count = 1;
  while (count > 0)
  {
    count = 0;
    for (int j = 0; j < numtriangles; j++)
    {
      if (originaltriangle[j].trianglekilled != 1 && originaltriangle[j].edgetriangle != 1)
      {
        int node1 = originaltriangle[j].firstnodeleft;
        int node2 = originaltriangle[j].secondnodeleft;
        int node3 = originaltriangle[j].thirdnodeleft;
        float x1 = originalnode[node1].xc;
        float y1 = originalnode[node1].yc;
        float z1 = originalnode[node1].zc;
        float x2 = originalnode[node2].xc;
        float y2 = originalnode[node2].yc;
        float z2 = originalnode[node2].zc;
        float x3 = originalnode[node3].xc;
        float y3 = originalnode[node3].yc;
        float z3 = originalnode[node3].zc;
        float midedge1x = (x1 + x2) / 2;
        float midedge1y = (y1 + y2) / 2;
        float midedge1z = (z1 + z2) / 2;
        float midedge2x = (x1 + x3) / 2;
        float midedge2y = (y1 + y3) / 2;
        float midedge2z = (z1 + z3) / 2;
        float midedge3x = (x2 + x3) / 2;
        float midedge3y = (y2 + y3) / 2;
        float midedge3z = (z2 + z3) / 2;
        float x12 = x2 - x1;
        float y12 = y2 - y1;
        float z12 = z2 - z1;
        float x13 = x3 - x1;
        float y13 = y3 - y1;
        float z13 = z3 - z1;
        float x21 = x1 - x2;
        float y21 = y1 - y2;
        float z21 = z1 - z2;
        float x23 = x3 - x2;
        float y23 = y3 - y2;
        float z23 = z3 - z2;
        float x31 = x1 - x3;
        float y31 = y1 - y3;
        float z31 = z1 - z3;
        float x32 = x2 - x3;
        float y32 = y2 - y3;
        float z32 = z2 - z3;
        float denom12 = (x12 * x12) + (y12 * y12) + (z12 * z12);
        float denom13 = (x13 * x13) + (y13 * y13) + (z13 * z13);
        float denom21 = (x21 * x21) + (y21 * y21) + (z21 * z21);
        float denom23 = (x23 * x23) + (y23 * y23) + (z23 * z23);
        float denom31 = (x31 * x31) + (y31 * y31) + (z31 * z31);
        float denom32 = (x32 * x32) + (y32 * y32) + (z32 * z32);
        denom12 = powf(denom12, 0.5);
        denom13 = powf(denom13, 0.5);
        denom21 = powf(denom21, 0.5);
        denom23 = powf(denom23, 0.5);
        denom31 = powf(denom31, 0.5);
        denom32 = powf(denom32, 0.5);
        x12 = x12 / denom12;
        y12 = y12 / denom12;
        z12 = z12 / denom12;
        x13 = x13 / denom13;
        y13 = y13 / denom13;
        z13 = z13 / denom13;
        x21 = x21 / denom21;
        y21 = y21 / denom21;
        z21 = z21 / denom21;
        x23 = x23 / denom23;
        y23 = y23 / denom23;
        z23 = z23 / denom23;
        x31 = x31 / denom31;
        y31 = y31 / denom31;
        z31 = z31 / denom31;
        x32 = x32 / denom32;
        y32 = y32 / denom32;
        z32 = z32 / denom32;
        float angle1 = (x12 * x13) + (y12 * y13) + (z12 * z13);
        float angle2 = (x21 * x23) + (y21 * y23) + (z21 * z23);
        float angle3 = (x31 * x32) + (y31 * y32) + (z31 * z32);
        angle1 = acos(angle1);
        angle2 = acos(angle2);
        angle3 = acos(angle3);
        angle1 = 180.0f * angle1 / 3.1415926535897;
        angle2 = 180.0f * angle2 / 3.1415926535897;
        angle3 = 180.0f * angle3 / 3.1415926535897;
        int badangle = 0;
        if (angle1 < 20)
          badangle = 1;
        if (angle2 < 20)
          badangle = 2;
        if (angle3 < 20)
          badangle = 3;
        if (badangle == 1)
        {
          count++;
          if (originalnode[node3].numgrains > originalnode[node2].numgrains)
          {
            int temp = node2;
            node2 = node3;
            node3 = temp;
          }
          originalnode[node2].set_coords(midedge3x, midedge3y, midedge3z);
          originalnode[node3].set_killed(killed);
          int size = tvector[node3].size();
          for (int l = 0; l < size; l++)
          {
            int firsttriangle = tvector[node3][l];
            if (originaltriangle[firsttriangle].trianglekilled != 1)
            {
              int affected = 0;
              int n1 = originaltriangle[firsttriangle].firstnodeleft;
              int n2 = originaltriangle[firsttriangle].secondnodeleft;
              int n3 = originaltriangle[firsttriangle].thirdnodeleft;
              if (n1 == node3)
                n1 = node2, affected = 1;
              if (n2 == node3)
                n2 = node2, affected = 1;
              if (n3 == node3)
                n3 = node2, affected = 1;
              originaltriangle[firsttriangle].set_nodes_leftgrain(n1, n2, n3);
              originaltriangle[firsttriangle].set_nodes_rightgrain(n1, n3, n2);
              if (n1 == n2 || n1 == n3 || n2 == n3)
                originaltriangle[firsttriangle].set_killed(killed);
              if (affected == 1 && originaltriangle[firsttriangle].trianglekilled != 1)
              {
                tvector[node2].push_back(firsttriangle);
              }
              float a1 = originalnode[n2].xc - originalnode[n1].xc;
              float a2 = originalnode[n2].yc - originalnode[n1].yc;
              float a3 = originalnode[n2].zc - originalnode[n1].zc;
              float b1 = originalnode[n3].xc - originalnode[n1].xc;
              float b2 = originalnode[n3].yc - originalnode[n1].yc;
              float b3 = originalnode[n3].zc - originalnode[n1].zc;
              float nx = (a2 * b3 - a3 * b2);
              float ny = (a3 * b1 - a1 * b3);
              float nz = (a1 * b2 - a2 * b1);
              float denom = powf((nx * nx + ny * ny + nz * nz), 0.5);
              nx = nx / denom;
              ny = ny / denom;
              nz = nz / denom;
              float area = denom / 2.0f;
              float xc = (originalnode[n1].xc + originalnode[n2].xc + originalnode[n3].xc) / 3.0f;
              float yc = (originalnode[n1].yc + originalnode[n2].yc + originalnode[n3].yc) / 3.0f;
              float zc = (originalnode[n1].zc + originalnode[n2].zc + originalnode[n3].zc) / 3.0f;
              float length1 = ((originalnode[n1].xc - originalnode[n2].xc) * (originalnode[n1].xc - originalnode[n2].xc)) + ((originalnode[n1].yc
                  - originalnode[n2].yc) * (originalnode[n1].yc - originalnode[n2].yc)) + ((originalnode[n1].zc - originalnode[n2].zc) * (originalnode[n1].zc
                  - originalnode[n2].zc));
              length1 = powf(length1, 0.5);
              float length2 = ((originalnode[n1].xc - originalnode[n3].xc) * (originalnode[n1].xc - originalnode[n3].xc)) + ((originalnode[n1].yc
                  - originalnode[n3].yc) * (originalnode[n1].yc - originalnode[n3].yc)) + ((originalnode[n1].zc - originalnode[n3].zc) * (originalnode[n1].zc
                  - originalnode[n3].zc));
              length2 = powf(length2, 0.5);
              float length3 = ((originalnode[n2].xc - originalnode[n3].xc) * (originalnode[n2].xc - originalnode[n3].xc)) + ((originalnode[n2].yc
                  - originalnode[n3].yc) * (originalnode[n2].yc - originalnode[n3].yc)) + ((originalnode[n2].zc - originalnode[n3].zc) * (originalnode[n2].zc
                  - originalnode[n3].zc));
              length3 = powf(length3, 0.5);
              float avglength = (length1 + length2 + length3) / 3.0f;
              float maxlength = length1;
              if (length2 > maxlength)
                maxlength = length2;
              if (length3 > maxlength)
                maxlength = length3;
              originaltriangle[firsttriangle].set_normal(nx, ny, nz);
              originaltriangle[firsttriangle].set_center(xc, yc, zc);
              originaltriangle[firsttriangle].set_area(area);
              originaltriangle[firsttriangle].set_averageedgelength(avglength);
              originaltriangle[firsttriangle].set_maxedgelength(maxlength);
            }
          }
          size = nnvector[node3].size();
          for (int l = 0; l < size; l++)
          {
            int firstneighbornode = nnvector[node3][l];
            nnvector[node2].push_back(firstneighbornode);
          }
        }
        if (badangle == 2)
        {
          count++;
          if (originalnode[node3].numgrains > originalnode[node1].numgrains)
          {
            int temp = node1;
            node1 = node3;
            node3 = temp;
          }
          originalnode[node1].set_coords(midedge2x, midedge2y, midedge2z);
          originalnode[node3].set_killed(killed);
          int size = tvector[node3].size();
          for (int l = 0; l < size; l++)
          {
            int firsttriangle = tvector[node3][l];
            if (originaltriangle[firsttriangle].trianglekilled != 1)
            {
              int affected = 0;
              int n1 = originaltriangle[firsttriangle].firstnodeleft;
              int n2 = originaltriangle[firsttriangle].secondnodeleft;
              int n3 = originaltriangle[firsttriangle].thirdnodeleft;
              if (n1 == node3)
                n1 = node1, affected = 1;
              if (n2 == node3)
                n2 = node1, affected = 1;
              if (n3 == node3)
                n3 = node1, affected = 1;
              originaltriangle[firsttriangle].set_nodes_leftgrain(n1, n2, n3);
              originaltriangle[firsttriangle].set_nodes_rightgrain(n1, n3, n2);
              if (n1 == n2 || n1 == n3 || n2 == n3)
                originaltriangle[firsttriangle].set_killed(killed);
              if (affected == 1 && originaltriangle[firsttriangle].trianglekilled != 1)
              {
                tvector[node1].push_back(firsttriangle);
              }
              float a1 = originalnode[n2].xc - originalnode[n1].xc;
              float a2 = originalnode[n2].yc - originalnode[n1].yc;
              float a3 = originalnode[n2].zc - originalnode[n1].zc;
              float b1 = originalnode[n3].xc - originalnode[n1].xc;
              float b2 = originalnode[n3].yc - originalnode[n1].yc;
              float b3 = originalnode[n3].zc - originalnode[n1].zc;
              float nx = (a2 * b3 - a3 * b2);
              float ny = (a3 * b1 - a1 * b3);
              float nz = (a1 * b2 - a2 * b1);
              float denom = powf((nx * nx + ny * ny + nz * nz), 0.5);
              nx = nx / denom;
              ny = ny / denom;
              nz = nz / denom;
              float area = denom / 2.0f;
              float xc = (originalnode[n1].xc + originalnode[n2].xc + originalnode[n3].xc) / 3.0f;
              float yc = (originalnode[n1].yc + originalnode[n2].yc + originalnode[n3].yc) / 3.0f;
              float zc = (originalnode[n1].zc + originalnode[n2].zc + originalnode[n3].zc) / 3.0f;
              float length1 = ((originalnode[n1].xc - originalnode[n2].xc) * (originalnode[n1].xc - originalnode[n2].xc)) + ((originalnode[n1].yc
                  - originalnode[n2].yc) * (originalnode[n1].yc - originalnode[n2].yc)) + ((originalnode[n1].zc - originalnode[n2].zc) * (originalnode[n1].zc
                  - originalnode[n2].zc));
              length1 = powf(length1, 0.5);
              float length2 = ((originalnode[n1].xc - originalnode[n3].xc) * (originalnode[n1].xc - originalnode[n3].xc)) + ((originalnode[n1].yc
                  - originalnode[n3].yc) * (originalnode[n1].yc - originalnode[n3].yc)) + ((originalnode[n1].zc - originalnode[n3].zc) * (originalnode[n1].zc
                  - originalnode[n3].zc));
              length2 = powf(length2, 0.5);
              float length3 = ((originalnode[n2].xc - originalnode[n3].xc) * (originalnode[n2].xc - originalnode[n3].xc)) + ((originalnode[n2].yc
                  - originalnode[n3].yc) * (originalnode[n2].yc - originalnode[n3].yc)) + ((originalnode[n2].zc - originalnode[n3].zc) * (originalnode[n2].zc
                  - originalnode[n3].zc));
              length3 = powf(length3, 0.5);
              float avglength = (length1 + length2 + length3) / 3.0f;
              float maxlength = length1;
              if (length2 > maxlength)
                maxlength = length2;
              if (length3 > maxlength)
                maxlength = length3;
              originaltriangle[firsttriangle].set_normal(nx, ny, nz);
              originaltriangle[firsttriangle].set_center(xc, yc, zc);
              originaltriangle[firsttriangle].set_area(area);
              originaltriangle[firsttriangle].set_averageedgelength(avglength);
              originaltriangle[firsttriangle].set_maxedgelength(maxlength);
            }
          }
          size = nnvector[node3].size();
          for (int l = 0; l < size; l++)
          {
            int firstneighbornode = nnvector[node3][l];
            nnvector[node1].push_back(firstneighbornode);
          }
        }
        if (badangle == 3)
        {
          count++;
          if (originalnode[node2].numgrains > originalnode[node1].numgrains)
          {
            int temp = node1;
            node1 = node2;
            node2 = temp;
          }
          originalnode[node1].set_coords(midedge1x, midedge1y, midedge1z);
          originalnode[node2].set_killed(killed);
          int size = tvector[node2].size();
          for (int l = 0; l < size; l++)
          {
            int firsttriangle = tvector[node2][l];
            if (originaltriangle[firsttriangle].trianglekilled != 1)
            {
              int affected = 0;
              int n1 = originaltriangle[firsttriangle].firstnodeleft;
              int n2 = originaltriangle[firsttriangle].secondnodeleft;
              int n3 = originaltriangle[firsttriangle].thirdnodeleft;
              if (n1 == node2)
                n1 = node1, affected = 1;
              if (n2 == node2)
                n2 = node1, affected = 1;
              if (n3 == node2)
                n3 = node1, affected = 1;
              originaltriangle[firsttriangle].set_nodes_leftgrain(n1, n2, n3);
              originaltriangle[firsttriangle].set_nodes_rightgrain(n1, n3, n2);
              if (n1 == n2 || n1 == n3 || n2 == n3)
                originaltriangle[firsttriangle].set_killed(killed);
              if (affected == 1 && originaltriangle[firsttriangle].trianglekilled != 1)
              {
                tvector[node1].push_back(firsttriangle);
              }
              float a1 = originalnode[n2].xc - originalnode[n1].xc;
              float a2 = originalnode[n2].yc - originalnode[n1].yc;
              float a3 = originalnode[n2].zc - originalnode[n1].zc;
              float b1 = originalnode[n3].xc - originalnode[n1].xc;
              float b2 = originalnode[n3].yc - originalnode[n1].yc;
              float b3 = originalnode[n3].zc - originalnode[n1].zc;
              float nx = (a2 * b3 - a3 * b2);
              float ny = (a3 * b1 - a1 * b3);
              float nz = (a1 * b2 - a2 * b1);
              float denom = powf((nx * nx + ny * ny + nz * nz), 0.5);
              nx = nx / denom;
              ny = ny / denom;
              nz = nz / denom;
              float area = denom / 2.0f;
              float xc = (originalnode[n1].xc + originalnode[n2].xc + originalnode[n3].xc) / 3.0f;
              float yc = (originalnode[n1].yc + originalnode[n2].yc + originalnode[n3].yc) / 3.0f;
              float zc = (originalnode[n1].zc + originalnode[n2].zc + originalnode[n3].zc) / 3.0f;
              float length1 = ((originalnode[n1].xc - originalnode[n2].xc) * (originalnode[n1].xc - originalnode[n2].xc)) + ((originalnode[n1].yc
                  - originalnode[n2].yc) * (originalnode[n1].yc - originalnode[n2].yc)) + ((originalnode[n1].zc - originalnode[n2].zc) * (originalnode[n1].zc
                  - originalnode[n2].zc));
              length1 = powf(length1, 0.5);
              float length2 = ((originalnode[n1].xc - originalnode[n3].xc) * (originalnode[n1].xc - originalnode[n3].xc)) + ((originalnode[n1].yc
                  - originalnode[n3].yc) * (originalnode[n1].yc - originalnode[n3].yc)) + ((originalnode[n1].zc - originalnode[n3].zc) * (originalnode[n1].zc
                  - originalnode[n3].zc));
              length2 = powf(length2, 0.5);
              float length3 = ((originalnode[n2].xc - originalnode[n3].xc) * (originalnode[n2].xc - originalnode[n3].xc)) + ((originalnode[n2].yc
                  - originalnode[n3].yc) * (originalnode[n2].yc - originalnode[n3].yc)) + ((originalnode[n2].zc - originalnode[n3].zc) * (originalnode[n2].zc
                  - originalnode[n3].zc));
              length3 = powf(length3, 0.5);
              float avglength = (length1 + length2 + length3) / 3.0f;
              float maxlength = length1;
              if (length2 > maxlength)
                maxlength = length2;
              if (length3 > maxlength)
                maxlength = length3;
              originaltriangle[firsttriangle].set_normal(nx, ny, nz);
              originaltriangle[firsttriangle].set_center(xc, yc, zc);
              originaltriangle[firsttriangle].set_area(area);
              originaltriangle[firsttriangle].set_averageedgelength(avglength);
              originaltriangle[firsttriangle].set_maxedgelength(maxlength);
            }
          }
          size = nnvector[node2].size();
          for (int l = 0; l < size; l++)
          {
            int firstneighbornode = nnvector[node2][l];
            nnvector[node1].push_back(firstneighbornode);
          }
        }
      }
    }
  }
  /*  string fileout;
   fileout = "boundary angles ns.vtk";
   ofstream outputFile;
   outputFile.open(fileout.c_str(), std::ios_base::binary);
   float minangle = 10000000;
   int goodtriangles = 0;
   for(int i=0;i<numtriangles;i++)
   {
   if(originaltriangle[i].trianglekilled != 1)
   {
   goodtriangles++;
   }
   }
   outputFile << "# vtk DataFile Version 2.0" << endl;
   outputFile << "data set from FFT2dx_GB" << endl;
   outputFile << "ASCII" << endl;
   outputFile << "DATASET UNSTRUCTURED_GRID" << endl;
   outputFile << endl;
   outputFile << "POINTS " << numsurfnodes << " float" << endl;
   for(int i=0;i<numsurfnodes;i++)
   {
   outputFile << originalnode[i].xc << " " << originalnode[i].yc << " " << originalnode[i].zc << endl;
   }
   outputFile << endl;
   outputFile << "CELLS " << goodtriangles << " " << goodtriangles*4 << endl;
   for(int i=0;i<numtriangles;i++)
   {
   if(originaltriangle[i].trianglekilled != 1)
   {
   if(originaltriangle[i].leftgrain < originaltriangle[i].rightgrain)
   {
   outputFile << "3 " << originaltriangle[i].firstnodeleft << " " << originaltriangle[i].secondnodeleft << " " << originaltriangle[i].thirdnodeleft << endl;
   }
   if(originaltriangle[i].leftgrain > originaltriangle[i].rightgrain)
   {
   outputFile << "3 " << originaltriangle[i].firstnoderight << " " << originaltriangle[i].secondnoderight << " " << originaltriangle[i].thirdnoderight << endl;
   }
   }
   }
   outputFile << endl;
   outputFile << "CELL_TYPES " << goodtriangles << endl;
   for(int i=0;i<numtriangles;i++)
   {
   if(originaltriangle[i].trianglekilled != 1)
   {
   outputFile << "5" << endl;
   }
   }
   outputFile << endl;
   outputFile << "CELL_DATA " << goodtriangles << endl;
   outputFile << "SCALARS TriangleID int 1" << endl;
   outputFile << "LOOKUP_TABLE default" << endl;
   for(int i=0;i<numtriangles;i++)
   {
   if(originaltriangle[i].trianglekilled != 1)
   {
   outputFile << i << endl;
   }
   }
   outputFile << endl;
   outputFile << "SCALARS Dihedrals float" << endl;
   outputFile << "LOOKUP_TABLE default" << endl;
   for(int i=0;i<numtriangles;i++)
   {
   if(originaltriangle[i].trianglekilled != 1)
   {
   minangle = 10000000;
   int node1 = originaltriangle[i].firstnodeleft;
   int node2 = originaltriangle[i].secondnodeleft;
   int node3 = originaltriangle[i].thirdnodeleft;
   int grain1 = originaltriangle[i].leftgrain;
   int grain2 = originaltriangle[i].rightgrain;
   for(int j=0;j<numtriangles;j++)
   {
   int n1 = originaltriangle[j].firstnodeleft;
   int n2 = originaltriangle[j].secondnodeleft;
   int n3 = originaltriangle[j].thirdnodeleft;
   int g1 = originaltriangle[j].leftgrain;
   int g2 = originaltriangle[j].rightgrain;
   int sameboundary = 0;
   if(g1 == grain1 && g2 == grain2) sameboundary = 1;
   if(g1 == grain2 && g2 == grain1) sameboundary = 1;
   int shared = 0;
   if(n1 == node1) shared++;
   if(n1 == node2) shared++;
   if(n1 == node3) shared++;
   if(n2 == node1) shared++;
   if(n2 == node2) shared++;
   if(n2 == node3) shared++;
   if(n3 == node1) shared++;
   if(n3 == node2) shared++;
   if(n3 == node3) shared++;
   if(shared == 2)
   {
   int leftgrain1 = originaltriangle[i].leftgrain;
   int leftgrain2 = originaltriangle[j].leftgrain;
   int rightgrain1 = originaltriangle[i].rightgrain;
   int rightgrain2 = originaltriangle[j].rightgrain;
   int normalfix1 = 1;
   int normalfix2 = 1;
   if(leftgrain1 == leftgrain2) normalfix1 = 1, normalfix2 = 1;
   if(leftgrain1 == rightgrain2) normalfix1 = 1, normalfix2 = -1;
   if(rightgrain1 == leftgrain2) normalfix1 = -1, normalfix2 = 1;
   if(rightgrain1 == rightgrain2) normalfix1 = -1, normalfix2 = -1;
   int skip = 0;
   if(rightgrain1 != leftgrain2 && rightgrain1 != rightgrain2 && leftgrain1 != leftgrain2 && leftgrain1 != rightgrain2) skip = 1;
   if(skip != 1)
   {
   float nx1 = normalfix1*originaltriangle[i].normalx;
   float ny1 = normalfix1*originaltriangle[i].normaly;
   float nz1 = normalfix1*originaltriangle[i].normalz;
   float nx2 = normalfix2*originaltriangle[j].normalx;
   float ny2 = normalfix2*originaltriangle[j].normaly;
   float nz2 = normalfix2*originaltriangle[j].normalz;
   float angle = (nx1*nx2)+(ny1*ny2)+(nz1*nz2);
   if(angle > 1) angle = 1;
   if(angle < -1) angle = -1;
   angle = acos(angle);
   angle = 180*angle/3.1415926535897;
   angle = 180-angle;
   if(angle < minangle) minangle = angle;
   if(angle < 10)
   {
   int stop = 0;
   }
   }
   }
   }
   if(originaltriangle[i].edgetriangle == 1) minangle = minangle + 0.1;
   outputFile << minangle << endl;
   }
   }
   outputFile.close();*/
}
void create_finalnodesandtriangles()
{
  int count = 0;
  int killed = 0;
  int snode = 0;
  int notmoved = 0;
  for (int i = 0; i < numgrains + 1; i++)
  {
    trianglesvector[i].clear();
    nodesvector[i].clear();
  }
  for (int i = 0; i < numsurfnodes; i++)
  {
    gvector[i].clear();
    tvector[i].clear();
    nnvector[i].clear();
    if (originalnode[i].nodekilled != 1)
    {
      originalnode[i].set_finalnumber(count);
      float random1 = 2.0f * (rg.genrand_res53() - 0.5);
      float x = originalnode[i].xc;
      float y = originalnode[i].yc;
      float z = originalnode[i].zc;
      if (x != globalxmin && x != globalxmax)
        x = x + (random1 * 0.01f);
      if (y != globalymin && y != globalymax)
        y = y - (random1 * 0.01f);
      if (z != globalzmin && z != globalzmax)
        z = z + (random1 * 0.01f);
      int ngrains = originalnode[i].numgrains;
      int onedge = originalnode[i].edgenode;
      finalnode[count].set_coords(x, y, z);
      finalnode[count].set_killed(killed);
      finalnode[count].set_surfnode(snode);
      finalnode[count].set_onedge(onedge);
      finalnode[count].set_numgrains(ngrains);
      finalnode[count].set_nodemoved(notmoved);
      count++;
    }
  }
  numnodes = count;
  count = 0;
  for (int i = 0; i < numtriangles; i++)
  {
    if (originaltriangle[i].trianglekilled != 1)
    {
      originaltriangle[i].set_finalnumber(count);
      int firstnode = originaltriangle[i].firstnodeleft;
      firstnode = originalnode[firstnode].finalnumber;
      int secondnode = originaltriangle[i].secondnodeleft;
      secondnode = originalnode[secondnode].finalnumber;
      int thirdnode = originaltriangle[i].thirdnodeleft;
      thirdnode = originalnode[thirdnode].finalnumber;
      float trianglearea = originaltriangle[i].trianglearea;
      float normalx = originaltriangle[i].normalx;
      float normaly = originaltriangle[i].normaly;
      float normalz = originaltriangle[i].normalz;
      int edgetriangle = originaltriangle[i].edgetriangle;
      float averageedgelength = originaltriangle[i].averageedgelength;
      float maxedgelength = originaltriangle[i].maxedgelength;
      int leftgrain = originaltriangle[i].leftgrain;
      int rightgrain = originaltriangle[i].rightgrain;
      float leftvolume = originaltriangle[i].leftvolume;
      float rightvolume = originaltriangle[i].rightvolume;
      float xcenter = originaltriangle[i].xcenter;
      float ycenter = originaltriangle[i].ycenter;
      float zcenter = originaltriangle[i].zcenter;
      float edgelength1 = originaltriangle[i].edgelength1;
      float edgelength2 = originaltriangle[i].edgelength2;
      float edgelength3 = originaltriangle[i].edgelength3;
      int trianglekilled = originaltriangle[i].trianglekilled;
      int tripletriangle = originaltriangle[i].tripletriangle;
      float dvalue = originaltriangle[i].dvalue;
      float averagemisorientation = originaltriangle[i].averagemisorientation;
      finaltriangle[count].set_nodes_leftgrain(firstnode, secondnode, thirdnode);
      finaltriangle[count].set_nodes_rightgrain(firstnode, thirdnode, secondnode);
      finaltriangle[count].set_area(trianglearea);
      finaltriangle[count].set_normal(normalx, normaly, normalz);
      finaltriangle[count].set_onedge(edgetriangle);
      finaltriangle[count].set_averageedgelength(averageedgelength);
      finaltriangle[count].set_maxedgelength(maxedgelength);
      finaltriangle[count].set_grains(leftgrain, rightgrain);
      finaltriangle[count].set_vol_leftgrain(leftvolume);
      finaltriangle[count].set_vol_rightgrain(rightvolume);
      finaltriangle[count].set_center(xcenter, ycenter, zcenter);
      finaltriangle[count].set_edgelengths(edgelength1, edgelength2, edgelength3);
      finaltriangle[count].set_killed(trianglekilled);
      finaltriangle[count].set_tripletriangle(tripletriangle);
      finaltriangle[count].set_facetD(dvalue);
      finaltriangle[count].set_avgmisorientation(averagemisorientation);
      trianglesvector[leftgrain].push_back(count);
      trianglesvector[rightgrain].push_back(count);
      nodesvector[leftgrain].push_back(firstnode);
      nodesvector[leftgrain].push_back(secondnode);
      nodesvector[leftgrain].push_back(thirdnode);
      nodesvector[rightgrain].push_back(firstnode);
      nodesvector[rightgrain].push_back(secondnode);
      nodesvector[rightgrain].push_back(thirdnode);
      tvector[firstnode].push_back(count);
      tvector[secondnode].push_back(count);
      tvector[thirdnode].push_back(count);
      gvector[firstnode].push_back(leftgrain);
      gvector[secondnode].push_back(leftgrain);
      gvector[thirdnode].push_back(leftgrain);
      gvector[firstnode].push_back(rightgrain);
      gvector[secondnode].push_back(rightgrain);
      gvector[thirdnode].push_back(rightgrain);
      nnvector[firstnode].push_back(secondnode);
      nnvector[firstnode].push_back(thirdnode);
      nnvector[secondnode].push_back(firstnode);
      nnvector[secondnode].push_back(thirdnode);
      nnvector[thirdnode].push_back(firstnode);
      nnvector[thirdnode].push_back(secondnode);
      count++;
    }
  }
  numtriangles = count;
  for (int j = 0; j < numgrains + 1; j++)
  {
    vector<int>::iterator newend;
    sort(nodesvector[j].begin(), nodesvector[j].end());
    newend = unique(nodesvector[j].begin(), nodesvector[j].end());
    nodesvector[j].erase(newend, nodesvector[j].end());
    sort(trianglesvector[j].begin(), trianglesvector[j].end());
    newend = unique(trianglesvector[j].begin(), trianglesvector[j].end());
    trianglesvector[j].erase(newend, trianglesvector[j].end());
  }
  for (int j = 0; j < numnodes; j++)
  {
    vector<int>::iterator newend;
    sort(tvector[j].begin(), tvector[j].end());
    newend = unique(tvector[j].begin(), tvector[j].end());
    tvector[j].erase(newend, tvector[j].end());
    sort(gvector[j].begin(), gvector[j].end());
    newend = unique(gvector[j].begin(), gvector[j].end());
    gvector[j].erase(newend, gvector[j].end());
    sort(nnvector[j].begin(), nnvector[j].end());
    newend = unique(nnvector[j].begin(), nnvector[j].end());
    nnvector[j].erase(newend, nnvector[j].end());
  }
  string fileout;
  fileout = "surface mesh.vtk";
  ofstream outputFile;
  outputFile.open(fileout.c_str(), std::ios_base::binary);
//  float minangle = 10000000;
  outputFile << "# vtk DataFile Version 2.0" << endl;
  outputFile << "data set from FFT2dx_GB" << endl;
  outputFile << "ASCII" << endl;
  outputFile << "DATASET UNSTRUCTURED_GRID" << endl;
  outputFile << endl;
  outputFile << "POINTS " << numnodes << " float" << endl;
  for (int i = 0; i < numnodes; i++)
  {
    outputFile << finalnode[i].xc << " " << finalnode[i].yc << " " << finalnode[i].zc << endl;
  }
  outputFile << endl;
  outputFile << "CELLS " << numtriangles * 2 << " " << numtriangles * 2 * 4 << endl;
  for (int i = 0; i < numtriangles; i++)
  {
    outputFile << "3 " << finaltriangle[i].firstnodeleft << " " << finaltriangle[i].secondnodeleft << " " << finaltriangle[i].thirdnodeleft << endl;
    outputFile << "3 " << finaltriangle[i].firstnoderight << " " << finaltriangle[i].secondnoderight << " " << finaltriangle[i].thirdnoderight << endl;
  }
  outputFile << endl;
  outputFile << "CELL_TYPES " << numtriangles * 2 << endl;
  for (int i = 0; i < numtriangles; i++)
  {
    outputFile << "5" << endl;
    outputFile << "5" << endl;
  }
  outputFile << endl;
  outputFile << "CELL_DATA " << numtriangles * 2 << endl;
  outputFile << "SCALARS GrainID float" << endl;
  outputFile << "LOOKUP_TABLE default" << endl;
  for (int i = 0; i < numtriangles; i++)
  {
    outputFile << finaltriangle[i].leftgrain << endl;
    outputFile << finaltriangle[i].rightgrain << endl;
  }
  outputFile.close();
}
void find_volumes()
{
  float vol = 0;
  float totalvol = 0;
  int node1 = 0;
  int node2 = 0;
  int node3 = 0;
  float maxlength = 0;
  for (int i = 1; i < numgrains + 1; i++)
  {
    vol = 0;
    totalvol = 0;
    maxlength = 0;
    float minx = 1000000;
    float maxx = 0;
    float miny = 1000000;
    float maxy = 0;
    float minz = 1000000;
    float maxz = 0;
    int size = nodesvector[i].size();
    for (int j = 0; j < size; j++)
    {
      int firstnode = nodesvector[i][j];
      if (finalnode[firstnode].xc < minx)
        minx = finalnode[firstnode].xc;
      if (finalnode[firstnode].xc > maxx)
        maxx = finalnode[firstnode].xc;
      if (finalnode[firstnode].yc < miny)
        miny = finalnode[firstnode].yc;
      if (finalnode[firstnode].yc > maxy)
        maxy = finalnode[firstnode].yc;
      if (finalnode[firstnode].zc < minz)
        minz = finalnode[firstnode].zc;
      if (finalnode[firstnode].zc > maxz)
        maxz = finalnode[firstnode].zc;
    }
    grain[i].set_bounds(minx, maxx, miny, maxy, minz, maxz);
    size = trianglesvector[i].size();
    for (int j = 0; j < size; j++)
    {
      int firsttriangle = trianglesvector[i][j];
      if (finaltriangle[firsttriangle].trianglekilled != 1)
      {
        int leftgrain = finaltriangle[firsttriangle].leftgrain;
        int rightgrain = finaltriangle[firsttriangle].rightgrain;
        if (leftgrain == i)
        {
          node1 = finaltriangle[firsttriangle].firstnodeleft;
          node2 = finaltriangle[firsttriangle].secondnodeleft;
          node3 = finaltriangle[firsttriangle].thirdnodeleft;
        }
        if (rightgrain == i)
        {
          node1 = finaltriangle[firsttriangle].firstnoderight;
          node2 = finaltriangle[firsttriangle].secondnoderight;
          node3 = finaltriangle[firsttriangle].thirdnoderight;
        }
        float x1 = finalnode[node1].xc;
        float y1 = finalnode[node1].yc;
        float z1 = finalnode[node1].zc;
        float x2 = finalnode[node2].xc;
        float y2 = finalnode[node2].yc;
        float z2 = finalnode[node2].zc;
        float x3 = finalnode[node3].xc;
        float y3 = finalnode[node3].yc;
        float z3 = finalnode[node3].zc;
        float x4 = grain[i].xcentroid;
        float y4 = grain[i].ycentroid;
        float z4 = grain[i].zcentroid;
        float a1x = x2 - x1;
        float a1y = y2 - y1;
        float a1z = z2 - z1;
        float a2x = x3 - x2;
        float a2y = y3 - y2;
        float a2z = z3 - z2;
        float a3x = x4 - x3;
        float a3y = y4 - y3;
        float a3z = z4 - z3;
        float determinant = ((a1x * a2y * a3z) + (a1y * a2z * a3x) + (a2x * a3y * a1z)) - ((a3x * a2y * a1z) + (a2x * a1y * a3z) + (a2z * a3y * a1x));
        vol = determinant / 6.0f;
        if (leftgrain == i)
          finaltriangle[firsttriangle].set_vol_leftgrain(vol);
        if (rightgrain == i)
          finaltriangle[firsttriangle].set_vol_rightgrain(vol);
        totalvol = totalvol + vol;
        float tempmaxlength = finaltriangle[firsttriangle].maxedgelength;
        if (tempmaxlength > maxlength)
          maxlength = tempmaxlength;
      }
    }
    grain[i].set_volume(totalvol);
    grain[i].set_maxedgelength(maxlength);
    globalsurfmeshvol = globalsurfmeshvol + totalvol;
  }
}
void make_nodes(int iter)
{
//  int count = 0;
  int killed = 0;
//  int snode = 1;
  int vnode = 0;
  int notmoved = 0;
  float dist = 0.0f;
  int firsttriangle = 0;
  int node1, node2, node3;
  float x1, y1, z1, x2, y2, z2, x3, y3, z3;
  float resx = 1.0f / (500.0f);
  float resy = 1.0f / (500.0f);
  float resz = 1.0f / (500.0f);
//  float fraction[3];
//  char fileout[100] = "pointcheck";
//  char num[10];
//  char fileoutext[7] = ".vtk";
//  itoa(iter, num, 10);
//  strcat(fileout, num);
//  strcat(fileout, fileoutext);
  QTextStream ss;
  ss << "pointcheck" << iter << ".vtk";
  QString fileout = ss.str();

  //  ofstream outFile;
  //  outFile.open(fileout, std::ios_base::binary);
//  float grainmaxedgelength = grain[iter].maxedgelength;
  float minx = grain[iter].xmin;
  float maxx = grain[iter].xmax;
  float miny = grain[iter].ymin;
  float maxy = grain[iter].ymax;
  float minz = grain[iter].zmin;
  float maxz = grain[iter].zmax;
//  float centerx = grain[iter].xcentroid;
//  float centery = grain[iter].ycentroid;
//  float centerz = grain[iter].zcentroid;
  int size = trianglesvector[iter].size();
  for (int l = 0; l < size; l++)
  {
    firsttriangle = trianglesvector[iter][l];
    if (finaltriangle[firsttriangle].trianglekilled != 1)
    {
      node1 = finaltriangle[firsttriangle].firstnodeleft;
      node2 = finaltriangle[firsttriangle].secondnodeleft;
      node3 = finaltriangle[firsttriangle].thirdnodeleft;
      x1 = finalnode[node1].xc;
      y1 = finalnode[node1].yc;
      z1 = finalnode[node1].zc;
      x2 = finalnode[node2].xc;
      y2 = finalnode[node2].yc;
      z2 = finalnode[node2].zc;
      x3 = finalnode[node3].xc;
      y3 = finalnode[node3].yc;
      z3 = finalnode[node3].zc;
      x1 = (x1 - minx) / (maxx - minx);
      x2 = (x2 - minx) / (maxx - minx);
      x3 = (x3 - minx) / (maxx - minx);
      y1 = (y1 - miny) / (maxy - miny);
      y2 = (y2 - miny) / (maxy - miny);
      y3 = (y3 - miny) / (maxy - miny);
      z1 = (z1 - minz) / (maxz - minz);
      z2 = (z2 - minz) / (maxz - minz);
      z3 = (z3 - minz) / (maxz - minz);
      float xmin = x1;
      if (x2 < xmin)
        xmin = x2;
      if (x3 < xmin)
        xmin = x3;
      float ymin = y1;
      if (y2 < ymin)
        ymin = y2;
      if (y3 < ymin)
        ymin = y3;
      float zmin = z1;
      if (z2 < zmin)
        zmin = z2;
      if (z3 < zmin)
        zmin = z3;
      float xmax = x1;
      if (x2 > xmax)
        xmax = x2;
      if (x3 > xmax)
        xmax = x3;
      float ymax = y1;
      if (y2 > ymax)
        ymax = y2;
      if (y3 > ymax)
        ymax = y3;
      float zmax = z1;
      if (z2 > zmax)
        zmax = z2;
      if (z3 > zmax)
        zmax = z3;
      int xstart = (xmin / resx) - 1;
      int xstop = (xmax / resx) + 1;
      int ystart = (ymin / resy) - 1;
      int ystop = (ymax / resy) + 1;
      int zstart = (zmin / resz) - 1;
      int zstop = (zmax / resz) + 1;
      if (xstart < 0)
        xstart = 0;
      if (xstop > 500)
        xstop = 500;
      if (ystart < 0)
        ystart = 0;
      if (ystop > 500)
        ystop = 500;
      if (zstart < 0)
        zstart = 0;
      if (zstop > 500)
        zstop = 500;
      for (int i = xstart; i < xstop; i++)
      {
        for (int j = ystart; j < ystop; j++)
        {
          float x = (i * resx) + (resx / 2);
          float y = (j * resy) + (resy / 2);
          float e0x = x - x1;
          float e0y = y - y1;
          float e1x = x2 - x1;
          float e1y = y2 - y1;
          float e2x = x3 - x1;
          float e2y = y3 - y1;
          int in = 1;
          if (e1x == 0)
          {
            if (e2x == 0)
              in = 0;
            if (e2x != 0)
            {
              float u = e0x / e2x;
              if (u < 0 || u > 1)
                in = 0;
              if (e1y == 0)
                in = 0;
              float v = (e0y - (e2y * u)) / e1y;
              if (v < 0)
                in = 0;
              if ((u + v) > 1)
                in = 0;
            }
          }
          if (e1x != 0)
          {
            float d = (e2y * e1x) - (e2x * e1y);
            if (d == 0)
              in = 0;
            float u = ((e0y * e1x) - (e0x * e1y)) / d;
            if (u < 0 || u > 1)
              in = 0;
            float v = (e0x - (e2x * u)) / e1x;
            if (v < 0)
              in = 0;
            if ((u + v) > 1)
              in = 0;
          }
          if (in == 1)
          {
            float a = finaltriangle[firsttriangle].normalx;
            float b = finaltriangle[firsttriangle].normaly;
            float c = finaltriangle[firsttriangle].normalz;
            float d = finaltriangle[firsttriangle].dvalue;
            x = (x * (maxx - minx)) + minx;
            y = (y * (maxy - miny)) + miny;
            float zval = (d - (a * x) - (b * y)) / c;
            zval = (zval - minz) / (maxz - minz);
            if (zval < zbuffer[iter][0][i][j])
              zbuffer[iter][0][i][j] = zval;
            if (zval > zbuffer[iter][1][i][j])
              zbuffer[iter][1][i][j] = zval;
          }
        }
      }
      for (int i = xstart; i < xstop; i++)
      {
        for (int j = zstart; j < zstop; j++)
        {
          float x = (i * resx) + (resx / 2);
          float z = (j * resz) + (resz / 2);
          float e0x = x - x1;
          float e0z = z - z1;
          float e1x = x2 - x1;
          float e1z = z2 - z1;
          float e2x = x3 - x1;
          float e2z = z3 - z1;
          int in = 1;
          if (e1x == 0)
          {
            if (e2x == 0)
              in = 0;
            if (e2x != 0)
            {
              float u = e0x / e2x;
              if (u < 0 || u > 1)
                in = 0;
              if (e1z == 0)
                in = 0;
              float v = (e0z - (e2z * u)) / e1z;
              if (v < 0)
                in = 0;
              if ((u + v) > 1)
                in = 0;
            }
          }
          if (e1x != 0)
          {
            float d = (e2z * e1x) - (e2x * e1z);
            if (d == 0)
              in = 0;
            float u = ((e0z * e1x) - (e0x * e1z)) / d;
            if (u < 0 || u > 1)
              in = 0;
            float v = (e0x - (e2x * u)) / e1x;
            if (v < 0)
              in = 0;
            if ((u + v) > 1)
              in = 0;
          }
          if (in == 1)
          {
            float a = finaltriangle[firsttriangle].normalx;
            float b = finaltriangle[firsttriangle].normaly;
            float c = finaltriangle[firsttriangle].normalz;
            float d = finaltriangle[firsttriangle].dvalue;
            x = (x * (maxx - minx)) + minx;
            z = (z * (maxz - minz)) + minz;
            float yval = (d - (a * x) - (c * z)) / b;
            yval = (yval - miny) / (maxy - miny);
            if (yval < zbuffer[iter][2][i][j])
              zbuffer[iter][2][i][j] = yval;
            if (yval > zbuffer[iter][3][i][j])
              zbuffer[iter][3][i][j] = yval;
          }
        }
      }
      for (int i = ystart; i < ystop; i++)
      {
        for (int j = zstart; j < zstop; j++)
        {
          float y = (i * resy) + (resy / 2);
          float z = (j * resz) + (resz / 2);
          float e0y = y - y1;
          float e0z = z - z1;
          float e1y = y2 - y1;
          float e1z = z2 - z1;
          float e2y = y3 - y1;
          float e2z = z3 - z1;
          int in = 1;
          if (e1y == 0)
          {
            if (e2y == 0)
              in = 0;
            if (e2y != 0)
            {
              float u = e0y / e2y;
              if (u < 0 || u > 1)
                in = 0;
              if (e1z == 0)
                in = 0;
              float v = (e0z - (e2z * u)) / e1z;
              if (v < 0)
                in = 0;
              if ((u + v) > 1)
                in = 0;
            }
          }
          if (e1y != 0)
          {
            float d = (e2z * e1y) - (e2y * e1z);
            if (d == 0)
              in = 0;
            float u = ((e0z * e1y) - (e0y * e1z)) / d;
            if (u < 0 || u > 1)
              in = 0;
            float v = (e0y - (e2y * u)) / e1y;
            if (v < 0)
              in = 0;
            if ((u + v) > 1)
              in = 0;
          }
          if (in == 1)
          {
            float a = finaltriangle[firsttriangle].normalx;
            float b = finaltriangle[firsttriangle].normaly;
            float c = finaltriangle[firsttriangle].normalz;
            float d = finaltriangle[firsttriangle].dvalue;
            y = (y * (maxy - miny)) + miny;
            z = (z * (maxz - minz)) + minz;
            float xval = (d - (b * y) - (c * z)) / a;
            xval = (xval - minx) / (maxx - minx);
            if (xval < zbuffer[iter][4][i][j])
              zbuffer[iter][4][i][j] = xval;
            if (xval > zbuffer[iter][5][i][j])
              zbuffer[iter][5][i][j] = xval;
          }
        }
      }
    }
  }
  for (int i = 0; i < numxvoxels; i++)
  {
    for (int j = 0; j < numyvoxels; j++)
    {
      for (int k = 0; k < numzvoxels; k++)
      {
        float x = (double(i) * xres) + (xres / 2.0) + 0.5;
        float y = (double(j) * yres) + (yres / 2.0) + 0.5;
        float z = (double(k) * zres) + (zres / 2.0) + 0.5;
        x = (x - minx) / (maxx - minx);
        y = (y - miny) / (maxy - miny);
        z = (z - minz) / (maxz - minz);
        int ibin = double(x) / (1.0 / 500.0);
        int jbin = double(y) / (1.0 / 500.0);
        int kbin = double(z) / (1.0 / 500.0);
        if (ibin >= 0 && ibin < 500 && jbin >= 0 && jbin < 500 && kbin >= 0 && kbin < 500)
        {
          if (x >= zbuffer[iter][4][jbin][kbin] && x <= zbuffer[iter][5][jbin][kbin] && y >= zbuffer[iter][2][ibin][kbin] && y <= zbuffer[iter][3][ibin][kbin]
              && z >= zbuffer[iter][0][ibin][jbin] && z <= zbuffer[iter][1][ibin][jbin])
          {
            voxels[i][j][k] = iter;
          }
        }
      }
    }
  }
  int ngrains = 1;
  int grainnodecount = nodesvector[iter].size();
  while (grainnodecount < 3500)
  {
    float xp = rg.genrand_res53();
    float yp = rg.genrand_res53();
    float zp = rg.genrand_res53();
    int x = xp / resx;
    int y = yp / resy;
    int z = zp / resz;
    if (x >= 0 && x < 500 && y >= 0 && y < 500 && z >= 0 && z < 500)
    {
      if (xp > zbuffer[iter][4][y][z] && xp < zbuffer[iter][5][y][z] && yp > zbuffer[iter][2][x][z] && yp < zbuffer[iter][3][x][z] && zp
          > zbuffer[iter][0][x][y] && zp < zbuffer[iter][1][x][y])
      {
        dist = xp - zbuffer[iter][4][y][z];
        if ((zbuffer[iter][5][y][z] - xp) < dist)
          dist = (zbuffer[iter][5][y][z] - xp);
        if ((yp - zbuffer[iter][2][x][z]) < dist)
          dist = (yp - zbuffer[iter][2][x][z]);
        if ((zbuffer[iter][3][x][z] - yp) < dist)
          dist = (zbuffer[iter][3][x][z] - yp);
        if ((zp - zbuffer[iter][0][x][y]) < dist)
          dist = (zp - zbuffer[iter][0][x][y]);
        if ((zbuffer[iter][1][x][y] - zp) < dist)
          dist = (zbuffer[iter][1][x][y] - zp);
        float prob = 2.0 * (0.5 - dist);
        prob = powf(prob, 2);
        float random = rg.genrand_res53();
        if (random <= prob)
        {
          xp = (xp * (maxx - minx)) + minx;
          yp = (yp * (maxy - miny)) + miny;
          zp = (zp * (maxz - minz)) + minz;
          finalnode[numnodes].set_coords(xp, yp, zp);
          finalnode[numnodes].set_killed(killed);
          finalnode[numnodes].set_surfnode(iter);
          finalnode[numnodes].set_numgrains(ngrains);
          finalnode[numnodes].set_onedge(vnode);
          finalnode[numnodes].set_nodemoved(notmoved);
          gvector[numnodes].push_back(iter);
          nodesvector[iter].push_back(numnodes);
          numnodes++;
          grainnodecount++;
        }
      }
    }
  }
  /*  outFile << "# vtk DataFile Version 2.0" << endl;
   outFile << "data set from FFT2dx_GB" << endl;
   outFile << "ASCII" << endl;
   outFile << "DATASET UNSTRUCTURED_GRID" << endl;
   outFile << endl;
   outFile << "POINTS " << numsurfnodes+numnodes-numgoodsurfnodes << " float" << endl;
   for(int i=0;i<numsurfnodes;i++)
   {
   outFile << originalnode[i].xc << " " << originalnode[i].yc << " " << originalnode[i].zc << endl;
   }
   for(int i=numgoodsurfnodes;i<numnodes;i++)
   {
   outFile << node[i].xc << " " << node[i].yc << " " << node[i].zc << endl;
   }
   outFile << endl;
   outFile << "CELLS " << numtriangles+(numsurfnodes+numnodes-numgoodsurfnodes) << " " << (numtriangles*4)+((numsurfnodes+numnodes-numgoodsurfnodes)*2) << endl;
   for(int i=0;i<numsurfnodes;i++)
   {
   outFile << "1 " << i << endl;
   }
   for(int i=numgoodsurfnodes;i<numnodes;i++)
   {
   outFile << "1 " << i+numsurfnodes-numgoodsurfnodes << endl;
   }
   trianglelist = grain[iter].trianglelist;
   while(!trianglelist.empty())
   {
   int firsttriangle = trianglelist.front();
   outFile << "3 " << triangle[firsttriangle].firstnodeleft << " " << triangle[firsttriangle].secondnodeleft << " " << triangle[firsttriangle].thirdnodeleft << endl;
   trianglelist.pop_front();
   }
   outFile << endl;
   outFile << "CELL_TYPES " << numtriangles+(numsurfnodes+numnodes-numgoodsurfnodes) << endl;
   for(int i=0;i<numsurfnodes;i++)
   {
   outFile << "1" << endl;
   }
   for(int i=numgoodsurfnodes;i<numnodes;i++)
   {
   outFile << "1" << endl;
   }
   trianglelist = grain[iter].trianglelist;
   while(!trianglelist.empty())
   {
   int firsttriangle = trianglelist.front();
   outFile << "5" << endl;
   trianglelist.pop_front();
   }
   outFile << endl;
   outFile << "CELL_DATA " << numtriangles+(numsurfnodes+numnodes-numgoodsurfnodes) << endl;
   outFile << "SCALARS GrainID int 1" << endl;
   outFile << "LOOKUP_TABLE default" << endl;
   for(int i=0;i<numsurfnodes;i++)
   {
   outFile << "0" << endl;
   }
   for(int i=numgoodsurfnodes;i<numnodes;i++)
   {
   list<int> nglist = node[i].grainlist;
   int grainname = nglist.front();
   outFile << grainname << endl;
   }
   trianglelist = grain[iter].trianglelist;
   while(!trianglelist.empty())
   {
   int firsttriangle = trianglelist.front();
   outFile << iter << endl;
   trianglelist.pop_front();
   }
   outFile.close();*/
}
void write_nodes()
{
//  int goodfinalnodecount = 0;
  string filename;
  filename = "points.txt";
  ofstream outFile;
  outFile.open(filename.c_str(), std::ios_base::binary);
  outFile << 3 << endl;
  outFile << numnodes << endl;
  for (int i = 0; i < numnodes; i++)
  {
    nnvector[i].clear();
    if (finalnode[i].nodekilled != 1)
    {
      float x = finalnode[i].xc;
      float y = finalnode[i].yc;
      float z = finalnode[i].zc;
      outFile << x << " " << y << " " << z << endl;
    }
  }
  outFile.close();
}
void read_elements(int idea)
{
  int n1;
  int n2;
  int n3;
  int n4;
//  int gnum;
  int notkilled = 0;
  int killed = 1;
//  int checkcount = 0;
  int surfnodecount = 0;
//  int moved = 1;
//  int inside = 0;
//  float nx = 0;
//  float ny = 0;
//  float nz = 0;
  int onedge = 0;
  int pickedgrain = 0;
  int badtetcount = -1;
  float badvolume = 0;
  list<int> nglist;
  string filename = "tets.txt";
  ifstream inputFile;
  inputFile.open(filename.c_str(), std::ios_base::binary);
  inputFile >> numelements;
  elementcount = 0;
  for (int i = 0; i < numelements; i++)
  {
    inputFile >> n1 >> n2 >> n3 >> n4;
    int good12 = 0;
    int good13 = 0;
    int good14 = 0;
    int good23 = 0;
    int good24 = 0;
    int good34 = 0;
    int size1 = gvector[n1].size();
    int size2 = gvector[n2].size();
    int size3 = gvector[n3].size();
    int size4 = gvector[n4].size();
    if (idea < numcycles - 1)
    {
      for (int l = 0; l < size1; l++)
      {
        int gnum1 = gvector[n1][l];
        if (find(gvector[n2].begin(), gvector[n2].end(), gnum1) != gvector[n2].end())
          good12 = 1;
        if (find(gvector[n3].begin(), gvector[n3].end(), gnum1) != gvector[n3].end())
          good13 = 1;
        if (find(gvector[n4].begin(), gvector[n4].end(), gnum1) != gvector[n4].end())
          good14 = 1;
      }
      for (int l = 0; l < size2; l++)
      {
        int gnum2 = gvector[n2][l];
        if (find(gvector[n3].begin(), gvector[n3].end(), gnum2) != gvector[n3].end())
          good23 = 1;
        if (find(gvector[n4].begin(), gvector[n4].end(), gnum2) != gvector[n4].end())
          good24 = 1;
      }
      for (int l = 0; l < size3; l++)
      {
        int gnum3 = gvector[n3][l];
        if (find(gvector[n4].begin(), gvector[n4].end(), gnum3) != gvector[n4].end())
          good34 = 1;
      }
      if (good12 == 0)
      {
        if (size1 == 1 && size2 == 1)
        {
          int move = 0;
          float xp = -1;
          float yp = -1;
          float zp = -1;
          int intersected = 0;
          int gnum = gvector[n1][0];
          int size = trianglesvector[gnum].size();
          for (int l = 0; l < size; l++)
          {
            int firsttriangle = trianglesvector[gnum][l];
            int node1 = finaltriangle[firsttriangle].firstnodeleft;
            int node2 = finaltriangle[firsttriangle].secondnodeleft;
            int node3 = finaltriangle[firsttriangle].thirdnodeleft;
            float nx = finaltriangle[firsttriangle].normalx;
            float ny = finaltriangle[firsttriangle].normaly;
            float nz = finaltriangle[firsttriangle].normalz;
            float vpx = finalnode[node1].xc - finalnode[n1].xc;
            float vpy = finalnode[node1].yc - finalnode[n1].yc;
            float vpz = finalnode[node1].zc - finalnode[n1].zc;
            float ppx = finalnode[n2].xc - finalnode[n1].xc;
            float ppy = finalnode[n2].yc - finalnode[n1].yc;
            float ppz = finalnode[n2].zc - finalnode[n1].zc;
            float r = ((nx * vpx) + (ny * vpy) + (nz * vpz)) / ((nx * ppx) + (ny * ppy) + (nz * ppz));
            float p1x = finalnode[n1].xc + (r * (finalnode[n2].xc - finalnode[n1].xc));
            float p1y = finalnode[n1].yc + (r * (finalnode[n2].yc - finalnode[n1].yc));
            float p1z = finalnode[n1].zc + (r * (finalnode[n2].zc - finalnode[n1].zc));
            float wx = p1x - finalnode[node1].xc;
            float wy = p1y - finalnode[node1].yc;
            float wz = p1z - finalnode[node1].zc;
            float ux = finalnode[node2].xc - finalnode[node1].xc;
            float uy = finalnode[node2].yc - finalnode[node1].yc;
            float uz = finalnode[node2].zc - finalnode[node1].zc;
            float vx = finalnode[node3].xc - finalnode[node1].xc;
            float vy = finalnode[node3].yc - finalnode[node1].yc;
            float vz = finalnode[node3].zc - finalnode[node1].zc;
            float denominator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((ux * vx) + (uy * vy) + (uz * vz))) - (((ux * ux) + (uy * uy) + (uz * uz))
                * ((vx * vx) + (vy * vy) + (vz * vz)));
            float snumerator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((wx * vx) + (wy * vy) + (wz * vz))) - (((wx * ux) + (wy * uy) + (wz * uz)) * ((vx * vx)
                + (vy * vy) + (vz * vz)));
            float tnumerator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((wx * ux) + (wy * uy) + (wz * uz))) - (((wx * vx) + (wy * vy) + (wz * vz)) * ((ux * ux)
                + (uy * uy) + (uz * uz)));
            float s = snumerator / denominator;
            float t = tnumerator / denominator;
            int in = 1;
            if (s < 0 || s > 1)
              in = 0;
            if (t < 0 || t > 1)
              in = 0;
            if ((s + t) > 1)
              in = 0;
            if (in == 1 && r >= 0 && r <= 1)
            {
              xp = p1x;
              yp = p1y;
              zp = p1z;
              l = size;
              if (r <= 0.5)
                move = 1;
              if (r > 0.5)
                move = 2;
              intersected++;
            }
//            if (in == 0 && r >= 0 && r <= 1)
//            {
//              int stop = 0;
//            }
          }
          if (move == 1)
          {
            for (int l = 0; l < size2; l++)
            {
              int firstgrain = gvector[n2][l];
              if (find(gvector[n1].begin(), gvector[n1].end(), firstgrain) == gvector[n1].end())
                gvector[n1].push_back(firstgrain);
            }
            size1 = gvector[n1].size();
            good13 = 0;
            good14 = 0;
            for (int l = 0; l < size1; l++)
            {
              int gnum1 = gvector[n1][l];
              if (find(gvector[n3].begin(), gvector[n3].end(), gnum1) != gvector[n3].end())
                good13 = 1;
              if (find(gvector[n4].begin(), gvector[n4].end(), gnum1) != gvector[n4].end())
                good14 = 1;
            }
            finalnode[n1].set_coords(xp, yp, zp);
          }
          if (move == 2)
          {
            for (int l = 0; l < size1; l++)
            {
              int firstgrain = gvector[n1][l];
              if (find(gvector[n2].begin(), gvector[n2].end(), firstgrain) == gvector[n2].end())
                gvector[n2].push_back(firstgrain);
            }
            size2 = gvector[n2].size();
            good23 = 0;
            good24 = 0;
            for (int l = 0; l < size2; l++)
            {
              int gnum2 = gvector[n2][l];
              if (find(gvector[n3].begin(), gvector[n3].end(), gnum2) != gvector[n3].end())
                good23 = 1;
              if (find(gvector[n4].begin(), gvector[n4].end(), gnum2) != gvector[n4].end())
                good24 = 1;
            }
            finalnode[n2].set_coords(xp, yp, zp);
          }
        }
      }
      if (good13 == 0)
      {
        if (size1 == 1 && size3 == 1)
        {
          float xp = -1;
          float yp = -1;
          float zp = -1;
          int move = 0;
          int intersected = 0;
          int gnum = gvector[n1][0];
          int size = trianglesvector[gnum].size();
          for (int l = 0; l < size; l++)
          {
            int firsttriangle = trianglesvector[gnum][l];
            int node1 = finaltriangle[firsttriangle].firstnodeleft;
            int node2 = finaltriangle[firsttriangle].secondnodeleft;
            int node3 = finaltriangle[firsttriangle].thirdnodeleft;
            float nx = finaltriangle[firsttriangle].normalx;
            float ny = finaltriangle[firsttriangle].normaly;
            float nz = finaltriangle[firsttriangle].normalz;
            float vpx = finalnode[node1].xc - finalnode[n1].xc;
            float vpy = finalnode[node1].yc - finalnode[n1].yc;
            float vpz = finalnode[node1].zc - finalnode[n1].zc;
            float ppx = finalnode[n3].xc - finalnode[n1].xc;
            float ppy = finalnode[n3].yc - finalnode[n1].yc;
            float ppz = finalnode[n3].zc - finalnode[n1].zc;
            float r = ((nx * vpx) + (ny * vpy) + (nz * vpz)) / ((nx * ppx) + (ny * ppy) + (nz * ppz));
            float p1x = finalnode[n1].xc + (r * (finalnode[n3].xc - finalnode[n1].xc));
            float p1y = finalnode[n1].yc + (r * (finalnode[n3].yc - finalnode[n1].yc));
            float p1z = finalnode[n1].zc + (r * (finalnode[n3].zc - finalnode[n1].zc));
            float wx = p1x - finalnode[node1].xc;
            float wy = p1y - finalnode[node1].yc;
            float wz = p1z - finalnode[node1].zc;
            float ux = finalnode[node2].xc - finalnode[node1].xc;
            float uy = finalnode[node2].yc - finalnode[node1].yc;
            float uz = finalnode[node2].zc - finalnode[node1].zc;
            float vx = finalnode[node3].xc - finalnode[node1].xc;
            float vy = finalnode[node3].yc - finalnode[node1].yc;
            float vz = finalnode[node3].zc - finalnode[node1].zc;
            float denominator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((ux * vx) + (uy * vy) + (uz * vz))) - (((ux * ux) + (uy * uy) + (uz * uz))
                * ((vx * vx) + (vy * vy) + (vz * vz)));
            float snumerator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((wx * vx) + (wy * vy) + (wz * vz))) - (((wx * ux) + (wy * uy) + (wz * uz)) * ((vx * vx)
                + (vy * vy) + (vz * vz)));
            float tnumerator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((wx * ux) + (wy * uy) + (wz * uz))) - (((wx * vx) + (wy * vy) + (wz * vz)) * ((ux * ux)
                + (uy * uy) + (uz * uz)));
            float s = snumerator / denominator;
            float t = tnumerator / denominator;
            int in = 1;
            if (s < 0 || s > 1)
              in = 0;
            if (t < 0 || t > 1)
              in = 0;
            if ((s + t) > 1)
              in = 0;
            if (in == 1 && r >= 0 && r <= 1)
            {
              xp = p1x;
              yp = p1y;
              zp = p1z;
              if (r <= 0.5)
                move = 1;
              if (r <= 0.5)
                move = 2;
              l = size;
              intersected++;
            }
//            if (in == 0 && r >= 0 && r <= 1)
//            {
//              int stop = 0;
//            }
          }
          if (move == 1)
          {
            for (int l = 0; l < size3; l++)
            {
              int firstgrain = gvector[n3][l];
              if (find(gvector[n1].begin(), gvector[n1].end(), firstgrain) == gvector[n1].end())
                gvector[n1].push_back(firstgrain);
            }
            size1 = gvector[n1].size();
            good14 = 0;
            for (int l = 0; l < size1; l++)
            {
              int gnum1 = gvector[n1][l];
              if (find(gvector[n4].begin(), gvector[n4].end(), gnum1) != gvector[n4].end())
                good14 = 1;
            }
            finalnode[n1].set_coords(xp, yp, zp);
          }
          if (move == 2)
          {
            for (int l = 0; l < size1; l++)
            {
              int firstgrain = gvector[n1][l];
              if (find(gvector[n3].begin(), gvector[n3].end(), firstgrain) == gvector[n3].end())
                gvector[n3].push_back(firstgrain);
            }
            size3 = gvector[n3].size();
            good23 = 0;
            good34 = 0;
            for (int l = 0; l < size3; l++)
            {
              int gnum3 = gvector[n3][l];
              if (find(gvector[n2].begin(), gvector[n2].end(), gnum3) != gvector[n2].end())
                good23 = 1;
              if (find(gvector[n4].begin(), gvector[n4].end(), gnum3) != gvector[n4].end())
                good34 = 1;
            }
            finalnode[n3].set_coords(xp, yp, zp);
          }
        }
      }
      if (good14 == 0)
      {
        if (size1 == 1 && size4 == 1)
        {
          float xp = -1;
          float yp = -1;
          float zp = -1;
          int move = 0;
          int intersected = 0;
          int gnum = gvector[n1][0];
          int size = trianglesvector[gnum].size();
          for (int l = 0; l < size; l++)
          {
            int firsttriangle = trianglesvector[gnum][l];
            int node1 = finaltriangle[firsttriangle].firstnodeleft;
            int node2 = finaltriangle[firsttriangle].secondnodeleft;
            int node3 = finaltriangle[firsttriangle].thirdnodeleft;
            float nx = finaltriangle[firsttriangle].normalx;
            float ny = finaltriangle[firsttriangle].normaly;
            float nz = finaltriangle[firsttriangle].normalz;
            float vpx = finalnode[node1].xc - finalnode[n1].xc;
            float vpy = finalnode[node1].yc - finalnode[n1].yc;
            float vpz = finalnode[node1].zc - finalnode[n1].zc;
            float ppx = finalnode[n4].xc - finalnode[n1].xc;
            float ppy = finalnode[n4].yc - finalnode[n1].yc;
            float ppz = finalnode[n4].zc - finalnode[n1].zc;
            float r = ((nx * vpx) + (ny * vpy) + (nz * vpz)) / ((nx * ppx) + (ny * ppy) + (nz * ppz));
            float p1x = finalnode[n1].xc + (r * (finalnode[n4].xc - finalnode[n1].xc));
            float p1y = finalnode[n1].yc + (r * (finalnode[n4].yc - finalnode[n1].yc));
            float p1z = finalnode[n1].zc + (r * (finalnode[n4].zc - finalnode[n1].zc));
            float wx = p1x - finalnode[node1].xc;
            float wy = p1y - finalnode[node1].yc;
            float wz = p1z - finalnode[node1].zc;
            float ux = finalnode[node2].xc - finalnode[node1].xc;
            float uy = finalnode[node2].yc - finalnode[node1].yc;
            float uz = finalnode[node2].zc - finalnode[node1].zc;
            float vx = finalnode[node3].xc - finalnode[node1].xc;
            float vy = finalnode[node3].yc - finalnode[node1].yc;
            float vz = finalnode[node3].zc - finalnode[node1].zc;
            float denominator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((ux * vx) + (uy * vy) + (uz * vz))) - (((ux * ux) + (uy * uy) + (uz * uz))
                * ((vx * vx) + (vy * vy) + (vz * vz)));
            float snumerator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((wx * vx) + (wy * vy) + (wz * vz))) - (((wx * ux) + (wy * uy) + (wz * uz)) * ((vx * vx)
                + (vy * vy) + (vz * vz)));
            float tnumerator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((wx * ux) + (wy * uy) + (wz * uz))) - (((wx * vx) + (wy * vy) + (wz * vz)) * ((ux * ux)
                + (uy * uy) + (uz * uz)));
            float s = snumerator / denominator;
            float t = tnumerator / denominator;
            int in = 1;
            if (s < 0 || s > 1)
              in = 0;
            if (t < 0 || t > 1)
              in = 0;
            if ((s + t) > 1)
              in = 0;
            if (in == 1 && r >= 0 && r <= 1)
            {
              xp = p1x;
              yp = p1y;
              zp = p1z;
              l = size;
              if (r <= 0.5)
                move = 1;
              if (r <= 0.5)
                move = 2;
              intersected++;
            }
//            if (in == 0 && r >= 0 && r <= 1)
//            {
//              int stop = 0;
//            }
          }
          if (move == 1)
          {
            for (int l = 0; l < size4; l++)
            {
              int firstgrain = gvector[n4][l];
              if (find(gvector[n1].begin(), gvector[n1].end(), firstgrain) == gvector[n1].end())
                gvector[n1].push_back(firstgrain);
            }
            finalnode[n1].set_coords(xp, yp, zp);
          }
          if (move == 2)
          {
            for (int l = 0; l < size1; l++)
            {
              int firstgrain = gvector[n1][l];
              if (find(gvector[n4].begin(), gvector[n4].end(), firstgrain) == gvector[n4].end())
                gvector[n4].push_back(firstgrain);
            }
            size4 = gvector[n4].size();
            good24 = 0;
            good34 = 0;
            for (int l = 0; l < size4; l++)
            {
              int gnum4 = gvector[n4][l];
              if (find(gvector[n2].begin(), gvector[n2].end(), gnum4) != gvector[n2].end())
                good24 = 1;
              if (find(gvector[n3].begin(), gvector[n3].end(), gnum4) != gvector[n3].end())
                good34 = 1;
            }
            finalnode[n4].set_coords(xp, yp, zp);
          }
        }
      }
      if (good23 == 0)
      {
        if (size2 == 1 && size3 == 1)
        {
          float xp = -1;
          float yp = -1;
          float zp = -1;
          int move = 0;
          int intersected = 0;
          int gnum = gvector[n2][0];
          int size = trianglesvector[gnum].size();
          for (int l = 0; l < size; l++)
          {
            int firsttriangle = trianglesvector[gnum][l];
            int node1 = finaltriangle[firsttriangle].firstnodeleft;
            int node2 = finaltriangle[firsttriangle].secondnodeleft;
            int node3 = finaltriangle[firsttriangle].thirdnodeleft;
            float nx = finaltriangle[firsttriangle].normalx;
            float ny = finaltriangle[firsttriangle].normaly;
            float nz = finaltriangle[firsttriangle].normalz;
            float vpx = finalnode[node1].xc - finalnode[n2].xc;
            float vpy = finalnode[node1].yc - finalnode[n2].yc;
            float vpz = finalnode[node1].zc - finalnode[n2].zc;
            float ppx = finalnode[n3].xc - finalnode[n2].xc;
            float ppy = finalnode[n3].yc - finalnode[n2].yc;
            float ppz = finalnode[n3].zc - finalnode[n2].zc;
            float r = ((nx * vpx) + (ny * vpy) + (nz * vpz)) / ((nx * ppx) + (ny * ppy) + (nz * ppz));
            float p1x = finalnode[n2].xc + (r * (finalnode[n3].xc - finalnode[n2].xc));
            float p1y = finalnode[n2].yc + (r * (finalnode[n3].yc - finalnode[n2].yc));
            float p1z = finalnode[n2].zc + (r * (finalnode[n3].zc - finalnode[n2].zc));
            float wx = p1x - finalnode[node1].xc;
            float wy = p1y - finalnode[node1].yc;
            float wz = p1z - finalnode[node1].zc;
            float ux = finalnode[node2].xc - finalnode[node1].xc;
            float uy = finalnode[node2].yc - finalnode[node1].yc;
            float uz = finalnode[node2].zc - finalnode[node1].zc;
            float vx = finalnode[node3].xc - finalnode[node1].xc;
            float vy = finalnode[node3].yc - finalnode[node1].yc;
            float vz = finalnode[node3].zc - finalnode[node1].zc;
            float denominator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((ux * vx) + (uy * vy) + (uz * vz))) - (((ux * ux) + (uy * uy) + (uz * uz))
                * ((vx * vx) + (vy * vy) + (vz * vz)));
            float snumerator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((wx * vx) + (wy * vy) + (wz * vz))) - (((wx * ux) + (wy * uy) + (wz * uz)) * ((vx * vx)
                + (vy * vy) + (vz * vz)));
            float tnumerator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((wx * ux) + (wy * uy) + (wz * uz))) - (((wx * vx) + (wy * vy) + (wz * vz)) * ((ux * ux)
                + (uy * uy) + (uz * uz)));
            float s = snumerator / denominator;
            float t = tnumerator / denominator;
            int in = 1;
            if (s < 0 || s > 1)
              in = 0;
            if (t < 0 || t > 1)
              in = 0;
            if ((s + t) > 1)
              in = 0;
            if (in == 1 && r >= 0 && r <= 1)
            {
              xp = p1x;
              yp = p1y;
              zp = p1z;
              l = size;
              if (r <= 0.5)
                move = 1;
              if (r <= 0.5)
                move = 2;
              intersected++;
            }
//            if (in == 0 && r >= 0 && r <= 1)
//            {
//              int stop = 0;
//            }
          }
          if (move == 1)
          {
            for (int l = 0; l < size3; l++)
            {
              int firstgrain = gvector[n3][l];
              if (find(gvector[n2].begin(), gvector[n2].end(), firstgrain) == gvector[n2].end())
                gvector[n2].push_back(firstgrain);
            }
            size2 = gvector[n2].size();
            good24 = 0;
            for (int l = 0; l < size2; l++)
            {
              int gnum2 = gvector[n2][l];
              if (find(gvector[n4].begin(), gvector[n4].end(), gnum2) != gvector[n4].end())
                good24 = 1;
            }
            finalnode[n2].set_coords(xp, yp, zp);
          }
          if (move == 2)
          {
            for (int l = 0; l < size2; l++)
            {
              int firstgrain = gvector[n2][l];
              if (find(gvector[n3].begin(), gvector[n3].end(), firstgrain) == gvector[n3].end())
                gvector[n3].push_back(firstgrain);
            }
            size3 = gvector[n3].size();
            good34 = 0;
            for (int l = 0; l < size3; l++)
            {
              int gnum3 = gvector[n3][l];
              if (find(gvector[n4].begin(), gvector[n4].end(), gnum3) != gvector[n4].end())
                good34 = 1;
            }
            finalnode[n3].set_coords(xp, yp, zp);
          }
        }
      }
      if (good24 == 0)
      {
        if (size2 == 1 && size4 == 1)
        {
          float xp = -1;
          float yp = -1;
          float zp = -1;
          int move = 0;
          int intersected = 0;
          int gnum = gvector[n2][0];
          int size = trianglesvector[gnum].size();
          for (int l = 0; l < size; l++)
          {
            int firsttriangle = trianglesvector[gnum][l];
            int node1 = finaltriangle[firsttriangle].firstnodeleft;
            int node2 = finaltriangle[firsttriangle].secondnodeleft;
            int node3 = finaltriangle[firsttriangle].thirdnodeleft;
            float nx = finaltriangle[firsttriangle].normalx;
            float ny = finaltriangle[firsttriangle].normaly;
            float nz = finaltriangle[firsttriangle].normalz;
            float vpx = finalnode[node1].xc - finalnode[n2].xc;
            float vpy = finalnode[node1].yc - finalnode[n2].yc;
            float vpz = finalnode[node1].zc - finalnode[n2].zc;
            float ppx = finalnode[n4].xc - finalnode[n2].xc;
            float ppy = finalnode[n4].yc - finalnode[n2].yc;
            float ppz = finalnode[n4].zc - finalnode[n2].zc;
            float r = ((nx * vpx) + (ny * vpy) + (nz * vpz)) / ((nx * ppx) + (ny * ppy) + (nz * ppz));
            float p1x = finalnode[n2].xc + (r * (finalnode[n4].xc - finalnode[n2].xc));
            float p1y = finalnode[n2].yc + (r * (finalnode[n4].yc - finalnode[n2].yc));
            float p1z = finalnode[n2].zc + (r * (finalnode[n4].zc - finalnode[n2].zc));
            float wx = p1x - finalnode[node1].xc;
            float wy = p1y - finalnode[node1].yc;
            float wz = p1z - finalnode[node1].zc;
            float ux = finalnode[node2].xc - finalnode[node1].xc;
            float uy = finalnode[node2].yc - finalnode[node1].yc;
            float uz = finalnode[node2].zc - finalnode[node1].zc;
            float vx = finalnode[node3].xc - finalnode[node1].xc;
            float vy = finalnode[node3].yc - finalnode[node1].yc;
            float vz = finalnode[node3].zc - finalnode[node1].zc;
            float denominator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((ux * vx) + (uy * vy) + (uz * vz))) - (((ux * ux) + (uy * uy) + (uz * uz))
                * ((vx * vx) + (vy * vy) + (vz * vz)));
            float snumerator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((wx * vx) + (wy * vy) + (wz * vz))) - (((wx * ux) + (wy * uy) + (wz * uz)) * ((vx * vx)
                + (vy * vy) + (vz * vz)));
            float tnumerator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((wx * ux) + (wy * uy) + (wz * uz))) - (((wx * vx) + (wy * vy) + (wz * vz)) * ((ux * ux)
                + (uy * uy) + (uz * uz)));
            float s = snumerator / denominator;
            float t = tnumerator / denominator;
            int in = 1;
            if (s < 0 || s > 1)
              in = 0;
            if (t < 0 || t > 1)
              in = 0;
            if ((s + t) > 1)
              in = 0;
            if (in == 1 && r >= 0 && r <= 1)
            {
              xp = p1x;
              yp = p1y;
              zp = p1z;
              l = size;
              if (r <= 0.5)
                move = 1;
              if (r <= 0.5)
                move = 2;
              intersected++;
            }
//            if (in == 0 && r >= 0 && r <= 1)
//            {
//              int stop = 0;
//            }
          }
          if (move == 1)
          {
            for (int l = 0; l < size4; l++)
            {
              int firstgrain = gvector[n4][l];
              if (find(gvector[n2].begin(), gvector[n2].end(), firstgrain) == gvector[n2].end())
                gvector[n2].push_back(firstgrain);
            }
            finalnode[n2].set_coords(xp, yp, zp);
          }
          if (move == 2)
          {
            for (int l = 0; l < size2; l++)
            {
              int firstgrain = gvector[n2][l];
              if (find(gvector[n4].begin(), gvector[n4].end(), firstgrain) == gvector[n4].end())
                gvector[n4].push_back(firstgrain);
            }
            size4 = gvector[n4].size();
            good34 = 0;
            for (int l = 0; l < size4; l++)
            {
              int gnum4 = gvector[n4][l];
              if (find(gvector[n3].begin(), gvector[n3].end(), gnum4) != gvector[n3].end())
                good34 = 1;
            }
            finalnode[n4].set_coords(xp, yp, zp);
          }
        }
      }
      if (good34 == 0)
      {
        if (size3 == 1 && size4 == 1)
        {
          float xp = -1;
          float yp = -1;
          float zp = -1;
          int move = 0;
          int intersected = 0;
          int gnum = gvector[n3][0];
          int size = trianglesvector[gnum].size();
          for (int l = 0; l < size; l++)
          {
            int firsttriangle = trianglesvector[gnum][l];
            int node1 = finaltriangle[firsttriangle].firstnodeleft;
            int node2 = finaltriangle[firsttriangle].secondnodeleft;
            int node3 = finaltriangle[firsttriangle].thirdnodeleft;
            float nx = finaltriangle[firsttriangle].normalx;
            float ny = finaltriangle[firsttriangle].normaly;
            float nz = finaltriangle[firsttriangle].normalz;
            float vpx = finalnode[node1].xc - finalnode[n3].xc;
            float vpy = finalnode[node1].yc - finalnode[n3].yc;
            float vpz = finalnode[node1].zc - finalnode[n3].zc;
            float ppx = finalnode[n4].xc - finalnode[n3].xc;
            float ppy = finalnode[n4].yc - finalnode[n3].yc;
            float ppz = finalnode[n4].zc - finalnode[n3].zc;
            float r = ((nx * vpx) + (ny * vpy) + (nz * vpz)) / ((nx * ppx) + (ny * ppy) + (nz * ppz));
            float p1x = finalnode[n3].xc + (r * (finalnode[n4].xc - finalnode[n3].xc));
            float p1y = finalnode[n3].yc + (r * (finalnode[n4].yc - finalnode[n3].yc));
            float p1z = finalnode[n3].zc + (r * (finalnode[n4].zc - finalnode[n3].zc));
            float wx = p1x - finalnode[node1].xc;
            float wy = p1y - finalnode[node1].yc;
            float wz = p1z - finalnode[node1].zc;
            float ux = finalnode[node2].xc - finalnode[node1].xc;
            float uy = finalnode[node2].yc - finalnode[node1].yc;
            float uz = finalnode[node2].zc - finalnode[node1].zc;
            float vx = finalnode[node3].xc - finalnode[node1].xc;
            float vy = finalnode[node3].yc - finalnode[node1].yc;
            float vz = finalnode[node3].zc - finalnode[node1].zc;
            float denominator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((ux * vx) + (uy * vy) + (uz * vz))) - (((ux * ux) + (uy * uy) + (uz * uz))
                * ((vx * vx) + (vy * vy) + (vz * vz)));
            float snumerator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((wx * vx) + (wy * vy) + (wz * vz))) - (((wx * ux) + (wy * uy) + (wz * uz)) * ((vx * vx)
                + (vy * vy) + (vz * vz)));
            float tnumerator = (((ux * vx) + (uy * vy) + (uz * vz)) * ((wx * ux) + (wy * uy) + (wz * uz))) - (((wx * vx) + (wy * vy) + (wz * vz)) * ((ux * ux)
                + (uy * uy) + (uz * uz)));
            float s = snumerator / denominator;
            float t = tnumerator / denominator;
            int in = 1;
            if (s < 0 || s > 1)
              in = 0;
            if (t < 0 || t > 1)
              in = 0;
            if ((s + t) > 1)
              in = 0;
            if (in == 1 && r >= 0 && r <= 1)
            {
              xp = p1x;
              yp = p1y;
              zp = p1z;
              l = size;
              if (r <= 0.5)
                move = 1;
              if (r <= 0.5)
                move = 2;
              intersected++;
            }
//            if (in == 0 && r >= 0 && r <= 1)
//            {
//              int stop = 0;
//            }
          }
          if (move == 1)
          {
            for (int l = 0; l < size4; l++)
            {
              int firstgrain = gvector[n4][l];
              if (find(gvector[n3].begin(), gvector[n3].end(), firstgrain) == gvector[n3].end())
                gvector[n3].push_back(firstgrain);
            }
            finalnode[n3].set_coords(xp, yp, zp);
          }
          if (move == 2)
          {
            for (int l = 0; l < size3; l++)
            {
              int firstgrain = gvector[n3][l];
              if (find(gvector[n4].begin(), gvector[n4].end(), firstgrain) == gvector[n4].end())
                gvector[n4].push_back(firstgrain);
            }
            finalnode[n4].set_coords(xp, yp, zp);
          }
        }
      }
    }
    for (int j = 1; j < numgrains + 1; j++)
    {
      graincheck[j] = 0;
    }
    onedge = 0;
    pickedgrain = badtetcount;
    int surfnode1 = finalnode[n1].surfnode;
    int surfnode2 = finalnode[n2].surfnode;
    int surfnode3 = finalnode[n3].surfnode;
    int surfnode4 = finalnode[n4].surfnode;
    surfnodecount = surfnode1 + surfnode2 + surfnode3 + surfnode4;
    if (finalnode[n1].edgenode == 1 || finalnode[n2].edgenode == 1 || finalnode[n3].edgenode == 1 || finalnode[n4].edgenode == 1)
      onedge = 1;
    size1 = gvector[n1].size();
    size2 = gvector[n2].size();
    size3 = gvector[n3].size();
    size4 = gvector[n4].size();
    for (int l = 0; l < size1; l++)
    {
      int gnum = gvector[n1][l];
      graincheck[gnum]++;
      if (graincheck[gnum] == 4)
        pickedgrain = gnum;
    }
    for (int l = 0; l < size2; l++)
    {
      int gnum = gvector[n2][l];
      graincheck[gnum]++;
      if (graincheck[gnum] == 4)
        pickedgrain = gnum;
    }
    for (int l = 0; l < size3; l++)
    {
      int gnum = gvector[n3][l];
      graincheck[gnum]++;
      if (graincheck[gnum] == 4)
        pickedgrain = gnum;
    }
    for (int l = 0; l < size4; l++)
    {
      int gnum = gvector[n4][l];
      graincheck[gnum]++;
      if (graincheck[gnum] == 4)
        pickedgrain = gnum;
    }
    float x1 = finalnode[n1].xc;
    float y1 = finalnode[n1].yc;
    float z1 = finalnode[n1].zc;
    float x2 = finalnode[n2].xc;
    float y2 = finalnode[n2].yc;
    float z2 = finalnode[n2].zc;
    float x3 = finalnode[n3].xc;
    float y3 = finalnode[n3].yc;
    float z3 = finalnode[n3].zc;
    float x4 = finalnode[n4].xc;
    float y4 = finalnode[n4].yc;
    float z4 = finalnode[n4].zc;
//    float a1x = x1 - x3;
//    float a1y = y1 - y3;
//    float a1z = z1 - z3;
    float b1x = x1 - x2;
    float b1y = y1 - y2;
    float b1z = z1 - z2;
//    float n1x = (a1y * b1z) - (a1z * b1y);
//    float n1y = (a1z * b1x) - (a1x * b1z);
//    float n1z = (a1x * b1y) - (a1y * b1x);
    float a2x = x2 - x3;
    float a2y = y2 - y3;
    float a2z = z2 - z3;
//    float b2x = x2 - x4;
//    float b2y = y2 - y4;
//    float b2z = z2 - z4;
//    float n2x = (a2y * b2z) - (a2z * b2y);
//    float n2y = (a2z * b2x) - (a2x * b2z);
//    float n2z = (a2x * b2y) - (a2y * b2x);
//    float a3x = x3 - x1;
//    float a3y = y3 - y1;
//    float a3z = z3 - z1;
    float b3x = x3 - x4;
    float b3y = y3 - y4;
    float b3z = z3 - z4;
//    float n3x = (a3y * b3z) - (a3z * b3y);
//    float n3y = (a3z * b3x) - (a3x * b3z);
//    float n3z = (a3x * b3y) - (a3y * b3x);
//    float a4x = x4 - x1;
//    float a4y = y4 - y1;
//    float a4z = z4 - z1;
//    float b4x = x4 - x2;
//    float b4y = y4 - y2;
//    float b4z = z4 - z2;
//    float n4x = (a4y * b4z) - (a4z * b4y);
//    float n4y = (a4z * b4x) - (a4x * b4z);
//    float n4z = (a4x * b4y) - (a4y * b4x);
    float determinant = ((b1x * a2y * b3z) + (b1y * a2z * b3x) + (a2x * b3y * b1z)) - ((b3x * a2y * b1z) + (a2x * b1y * b3z) + (a2z * b3y * b1x));
    float absdeterminant = fabs(determinant);
    float vol = absdeterminant / 6.0;
    globalvolmeshvol = globalvolmeshvol + vol;
    element[elementcount].set_nodes(n1, n2, n3, n4);
    element[elementcount].set_killed(notkilled);
    element[elementcount].set_grain(pickedgrain);
    element[elementcount].set_surfnodecount(surfnodecount);
    element[elementcount].set_onedge(onedge);
    elvector[n1].push_back(elementcount);
    elvector[n2].push_back(elementcount);
    elvector[n3].push_back(elementcount);
    elvector[n4].push_back(elementcount);
    nnvector[n1].push_back(n2);
    nnvector[n1].push_back(n3);
    nnvector[n1].push_back(n4);
    nnvector[n2].push_back(n1);
    nnvector[n2].push_back(n3);
    nnvector[n2].push_back(n4);
    nnvector[n3].push_back(n1);
    nnvector[n3].push_back(n2);
    nnvector[n3].push_back(n4);
    nnvector[n4].push_back(n1);
    nnvector[n4].push_back(n2);
    nnvector[n4].push_back(n3);
    elementcount++;
    if (pickedgrain < 0)
    {
      badtetcount = badtetcount - 1;
      badvolume = badvolume + vol;
    }
  }
  for (int j = 0; j < numnodes; j++)
  {
    vector<int>::iterator newend;
    sort(elvector[j].begin(), elvector[j].end());
    newend = unique(elvector[j].begin(), elvector[j].end());
    elvector[j].erase(newend, elvector[j].end());
    sort(nnvector[j].begin(), nnvector[j].end());
    newend = unique(nnvector[j].begin(), nnvector[j].end());
    nnvector[j].erase(newend, nnvector[j].end());
  }
  for (int i = 0; i < numelements; i++)
  {
    int grainnum = element[i].grainname;
    int doit = 0;
    if (grainnum < 0)
    {
      for (int j = 1; j < numgrains + 1; j++)
      {
        graincheck[j] = 0;
      }
      int pickedgrain = -1;
      int n1 = element[i].node1;
      int n2 = element[i].node2;
      int n3 = element[i].node3;
      int n4 = element[i].node4;
      int size1 = gvector[n1].size();
      int size2 = gvector[n2].size();
      int size3 = gvector[n3].size();
      int size4 = gvector[n4].size();
      for (int l = 0; l < size1; l++)
      {
        int gnum = gvector[n1][l];
        graincheck[gnum]++;
        if (graincheck[gnum] == 4)
          pickedgrain = gnum;
      }
      for (int l = 0; l < size2; l++)
      {
        int gnum = gvector[n2][l];
        graincheck[gnum]++;
        if (graincheck[gnum] == 4)
          pickedgrain = gnum;
      }
      for (int l = 0; l < size3; l++)
      {
        int gnum = gvector[n3][l];
        graincheck[gnum]++;
        if (graincheck[gnum] == 4)
          pickedgrain = gnum;
      }
      for (int l = 0; l < size4; l++)
      {
        int gnum = gvector[n4][l];
        graincheck[gnum]++;
        if (graincheck[gnum] == 4)
          pickedgrain = gnum;
      }
      if (pickedgrain > 0)
      {
        element[i].set_grain(pickedgrain);
      }
      if (pickedgrain == -1 && doit == 1)
      {
        int newnode = n1;
        int maxsize = size1;
        if (size2 > maxsize)
          newnode = n2, maxsize = size2;
        if (size3 > maxsize)
          newnode = n3, maxsize = size3;
        if (size4 > maxsize)
          newnode = n4, maxsize = size4;
        if (newnode != n1)
          finalnode[n1].set_killed(killed);
        if (newnode != n2)
          finalnode[n2].set_killed(killed);
        if (newnode != n3)
          finalnode[n3].set_killed(killed);
        if (newnode != n4)
          finalnode[n4].set_killed(killed);
        int size = elvector[n1].size();
        for (int l = 0; l < size; l++)
        {
          int count1 = 0;
          int count2 = 0;
          int count3 = 0;
          int count4 = 0;
          int elem = elvector[n1][l];
          int node1 = element[elem].node1;
          int node2 = element[elem].node2;
          int node3 = element[elem].node3;
          int node4 = element[elem].node4;
          if (node1 == n1 || node1 == n2 || node1 == n3 || node1 == n4)
            count1++;
          if (node2 == n1 || node2 == n2 || node2 == n3 || node2 == n4)
            count2++;
          if (node3 == n1 || node3 == n2 || node3 == n3 || node3 == n4)
            count3++;
          if (node4 == n1 || node4 == n2 || node4 == n3 || node4 == n4)
            count4++;
          if ((count1 + count2 + count3 + count4) >= 2)
          {
            element[elem].set_killed(killed);
          }
          if ((count1 + count2 + count3 + count4) == 1)
          {
            if (count1 == 1)
              element[elem].set_nodes(newnode, node2, node3, node4);
            if (count2 == 1)
              element[elem].set_nodes(node1, newnode, node3, node4);
            if (count3 == 1)
              element[elem].set_nodes(node1, node2, newnode, node4);
            if (count4 == 1)
              element[elem].set_nodes(node1, node2, node3, newnode);
          }
        }
        size = elvector[n2].size();
        for (int l = 0; l < size; l++)
        {
          int count1 = 0;
          int count2 = 0;
          int count3 = 0;
          int count4 = 0;
          int elem = elvector[n2][l];
          int node1 = element[elem].node1;
          int node2 = element[elem].node2;
          int node3 = element[elem].node3;
          int node4 = element[elem].node4;
          if (node1 == n1 || node1 == n2 || node1 == n3 || node1 == n4)
            count1++;
          if (node2 == n1 || node2 == n2 || node2 == n3 || node2 == n4)
            count2++;
          if (node3 == n1 || node3 == n2 || node3 == n3 || node3 == n4)
            count3++;
          if (node4 == n1 || node4 == n2 || node4 == n3 || node4 == n4)
            count4++;
          if ((count1 + count2 + count3 + count4) >= 2)
          {
            element[elem].set_killed(killed);
          }
          if ((count1 + count2 + count3 + count4) == 1)
          {
            if (count1 == 1)
              element[elem].set_nodes(newnode, node2, node3, node4);
            if (count2 == 1)
              element[elem].set_nodes(node1, newnode, node3, node4);
            if (count3 == 1)
              element[elem].set_nodes(node1, node2, newnode, node4);
            if (count4 == 1)
              element[elem].set_nodes(node1, node2, node3, newnode);
          }
        }
        size = elvector[n3].size();
        for (int l = 0; l < size; l++)
        {
          int count1 = 0;
          int count2 = 0;
          int count3 = 0;
          int count4 = 0;
          int elem = elvector[n3][l];
          int node1 = element[elem].node1;
          int node2 = element[elem].node2;
          int node3 = element[elem].node3;
          int node4 = element[elem].node4;
          if (node1 == n1 || node1 == n2 || node1 == n3 || node1 == n4)
            count1++;
          if (node2 == n1 || node2 == n2 || node2 == n3 || node2 == n4)
            count2++;
          if (node3 == n1 || node3 == n2 || node3 == n3 || node3 == n4)
            count3++;
          if (node4 == n1 || node4 == n2 || node4 == n3 || node4 == n4)
            count4++;
          if ((count1 + count2 + count3 + count4) >= 2)
          {
            element[elem].set_killed(killed);
          }
          if ((count1 + count2 + count3 + count4) == 1)
          {
            if (count1 == 1)
              element[elem].set_nodes(newnode, node2, node3, node4);
            if (count2 == 1)
              element[elem].set_nodes(node1, newnode, node3, node4);
            if (count3 == 1)
              element[elem].set_nodes(node1, node2, newnode, node4);
            if (count4 == 1)
              element[elem].set_nodes(node1, node2, node3, newnode);
          }
        }
        size = elvector[n4].size();
        for (int l = 0; l < size; l++)
        {
          int count1 = 0;
          int count2 = 0;
          int count3 = 0;
          int count4 = 0;
          int elem = elvector[n4][l];
          int node1 = element[elem].node1;
          int node2 = element[elem].node2;
          int node3 = element[elem].node3;
          int node4 = element[elem].node4;
          if (node1 == n1 || node1 == n2 || node1 == n3 || node1 == n4)
            count1++;
          if (node2 == n1 || node2 == n2 || node2 == n3 || node2 == n4)
            count2++;
          if (node3 == n1 || node3 == n2 || node3 == n3 || node3 == n4)
            count3++;
          if (node4 == n1 || node4 == n2 || node4 == n3 || node4 == n4)
            count4++;
          if ((count1 + count2 + count3 + count4) >= 2)
          {
            element[elem].set_killed(killed);
          }
          if ((count1 + count2 + count3 + count4) == 1)
          {
            if (count1 == 1)
              element[elem].set_nodes(newnode, node2, node3, node4);
            if (count2 == 1)
              element[elem].set_nodes(node1, newnode, node3, node4);
            if (count3 == 1)
              element[elem].set_nodes(node1, node2, newnode, node4);
            if (count4 == 1)
              element[elem].set_nodes(node1, node2, node3, newnode);
          }
        }
      }
    }
  }
}
void measure_elements()
{
  float avgvol = 0;
  int count = 0;
  for (int i = 0; i < 180; i++)
  {
    dihedralanglesurf[i] = 0;
    dihedralanglebulk[i] = 0;
  }
  for (int m = 0; m < 25; m++)
  {
    volumesurf[m] = 0;
    volumebulk[m] = 0;
  }
  for (int j = 0; j < elementcount; j++)
  {
    int n1 = element[j].node1;
    int n2 = element[j].node2;
    int n3 = element[j].node3;
    int n4 = element[j].node4;
    float x1 = finalnode[n1].xc;
    float y1 = finalnode[n1].yc;
    float z1 = finalnode[n1].zc;
    float x2 = finalnode[n2].xc;
    float y2 = finalnode[n2].yc;
    float z2 = finalnode[n2].zc;
    float x3 = finalnode[n3].xc;
    float y3 = finalnode[n3].yc;
    float z3 = finalnode[n3].zc;
    float x4 = finalnode[n4].xc;
    float y4 = finalnode[n4].yc;
    float z4 = finalnode[n4].zc;
    float a1x = x1 - x2;
    float a1y = y1 - y2;
    float a1z = z1 - z2;
    float b1x = x1 - x3;
    float b1y = y1 - y3;
    float b1z = z1 - z3;
    float c1x = x1 - x4;
    float c1y = y1 - y4;
    float c1z = z1 - z4;
    float n1x = (a1y * b1z) - (a1z * b1y);
    float n1y = (a1z * b1x) - (a1x * b1z);
    float n1z = (a1x * b1y) - (a1y * b1x);
    float check = (c1x * n1x) + (c1y * n1y) + (c1z * n1z);
    if (check < 0)
    {
      element[j].set_nodes(n1, n3, n2, n4);
    }
  }
  int edgeelement = 0;
  int onedge = 0;
  for (int k = 0; k < elementcount; k++)
  {
    if (element[k].elementkilled != 1)
    {
      edgeelement = 0;
      onedge = 0;
      int n1 = element[k].node1;
      int n2 = element[k].node2;
      int n3 = element[k].node3;
      int n4 = element[k].node4;
      int grainname = element[k].grainname;
      float x1 = finalnode[n1].xc;
      float y1 = finalnode[n1].yc;
      float z1 = finalnode[n1].zc;
      float x2 = finalnode[n2].xc;
      float y2 = finalnode[n2].yc;
      float z2 = finalnode[n2].zc;
      float x3 = finalnode[n3].xc;
      float y3 = finalnode[n3].yc;
      float z3 = finalnode[n3].zc;
      float x4 = finalnode[n4].xc;
      float y4 = finalnode[n4].yc;
      float z4 = finalnode[n4].zc;
      float a1x = x1 - x3;
      float a1y = y1 - y3;
      float a1z = z1 - z3;
      float b1x = x1 - x2;
      float b1y = y1 - y2;
      float b1z = z1 - z2;
      float n1x = (a1y * b1z) - (a1z * b1y);
      float n1y = (a1z * b1x) - (a1x * b1z);
      float n1z = (a1x * b1y) - (a1y * b1x);
      float a2x = x2 - x3;
      float a2y = y2 - y3;
      float a2z = z2 - z3;
      float b2x = x2 - x4;
      float b2y = y2 - y4;
      float b2z = z2 - z4;
      float n2x = (a2y * b2z) - (a2z * b2y);
      float n2y = (a2z * b2x) - (a2x * b2z);
      float n2z = (a2x * b2y) - (a2y * b2x);
      float a3x = x3 - x1;
      float a3y = y3 - y1;
      float a3z = z3 - z1;
      float b3x = x3 - x4;
      float b3y = y3 - y4;
      float b3z = z3 - z4;
      float n3x = (a3y * b3z) - (a3z * b3y);
      float n3y = (a3z * b3x) - (a3x * b3z);
      float n3z = (a3x * b3y) - (a3y * b3x);
      float a4x = x4 - x1;
      float a4y = y4 - y1;
      float a4z = z4 - z1;
      float b4x = x4 - x2;
      float b4y = y4 - y2;
      float b4z = z4 - z2;
      float n4x = (a4y * b4z) - (a4z * b4y);
      float n4y = (a4z * b4x) - (a4x * b4z);
      float n4z = (a4x * b4y) - (a4y * b4x);
      float denom11 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
      float denom12 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
      denom11 = powf(denom11, 0.5);
      denom12 = powf(denom12, 0.5);
      float dihedral1 = ((n1x * n2x) + (n1y * n2y) + (n1z * n2z)) / (denom11 * denom12);
      if (dihedral1 < -1)
        dihedral1 = -1;
      if (dihedral1 > 1)
        dihedral1 = 1;
      dihedral1 = acos(dihedral1);
      float denom21 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
      float denom22 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
      denom21 = powf(denom21, 0.5);
      denom22 = powf(denom22, 0.5);
      float dihedral2 = ((n1x * n3x) + (n1y * n3y) + (n1z * n3z)) / (denom21 * denom22);
      if (dihedral2 < -1)
        dihedral2 = -1;
      if (dihedral2 > 1)
        dihedral2 = 1;
      dihedral2 = acos(dihedral2);
      float denom31 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
      float denom32 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
      denom31 = powf(denom31, 0.5);
      denom32 = powf(denom32, 0.5);
      float dihedral3 = ((n1x * n4x) + (n1y * n4y) + (n1z * n4z)) / (denom31 * denom32);
      if (dihedral3 < -1)
        dihedral3 = -1;
      if (dihedral3 > 1)
        dihedral3 = 1;
      dihedral3 = acos(dihedral3);
      float denom41 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
      float denom42 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
      denom41 = powf(denom41, 0.5);
      denom42 = powf(denom42, 0.5);
      float dihedral4 = ((n2x * n3x) + (n2y * n3y) + (n2z * n3z)) / (denom41 * denom42);
      if (dihedral4 < -1)
        dihedral4 = -1;
      if (dihedral4 > 1)
        dihedral4 = 1;
      dihedral4 = acos(dihedral4);
      float denom51 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
      float denom52 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
      denom51 = powf(denom51, 0.5);
      denom52 = powf(denom52, 0.5);
      float dihedral5 = ((n2x * n4x) + (n2y * n4y) + (n2z * n4z)) / (denom51 * denom52);
      if (dihedral5 < -1)
        dihedral5 = -1;
      if (dihedral5 > 1)
        dihedral5 = 1;
      dihedral5 = acos(dihedral5);
      float denom61 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
      float denom62 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
      denom61 = powf(denom61, 0.5);
      denom62 = powf(denom62, 0.5);
      float dihedral6 = ((n3x * n4x) + (n3y * n4y) + (n3z * n4z)) / (denom61 * denom62);
      if (dihedral6 < -1)
        dihedral6 = -1;
      if (dihedral6 > 1)
        dihedral6 = 1;
      dihedral6 = acos(dihedral6);
      dihedral1 = (180.0f / 3.1415926535897f) * dihedral1;
      dihedral2 = (180.0f / 3.1415926535897f) * dihedral2;
      dihedral3 = (180.0f / 3.1415926535897f) * dihedral3;
      dihedral4 = (180.0f / 3.1415926535897f) * dihedral4;
      dihedral5 = (180.0f / 3.1415926535897f) * dihedral5;
      dihedral6 = (180.0f / 3.1415926535897f) * dihedral6;
      dihedral1 = 180.0f - dihedral1;
      dihedral2 = 180.0f - dihedral2;
      dihedral3 = 180.0f - dihedral3;
      dihedral4 = 180.0f - dihedral4;
      dihedral5 = 180.0f - dihedral5;
      dihedral6 = 180.0f - dihedral6;
      int surfnodecount = element[k].originalnodecount;
      if (surfnodecount == 0)
      {
        dihedralanglebulk[int(dihedral1)]++;
        dihedralanglebulk[int(dihedral2)]++;
        dihedralanglebulk[int(dihedral3)]++;
        dihedralanglebulk[int(dihedral4)]++;
        dihedralanglebulk[int(dihedral5)]++;
        dihedralanglebulk[int(dihedral6)]++;
      }
      if (surfnodecount > 0)
      {
        dihedralanglesurf[int(dihedral1)]++;
        dihedralanglesurf[int(dihedral2)]++;
        dihedralanglesurf[int(dihedral3)]++;
        dihedralanglesurf[int(dihedral4)]++;
        dihedralanglesurf[int(dihedral5)]++;
        dihedralanglesurf[int(dihedral6)]++;
      }
      element[k].set_dihedralangles(dihedral1, dihedral2, dihedral3, dihedral4, dihedral5, dihedral6);
      float length1 = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
      float length2 = ((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)) + ((z1 - z3) * (z1 - z3));
      float length3 = ((x1 - x4) * (x1 - x4)) + ((y1 - y4) * (y1 - y4)) + ((z1 - z4) * (z1 - z4));
      float length4 = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)) + ((z2 - z3) * (z2 - z3));
      float length5 = ((x2 - x4) * (x2 - x4)) + ((y2 - y4) * (y2 - y4)) + ((z2 - z4) * (z2 - z4));
      float length6 = ((x3 - x4) * (x3 - x4)) + ((y3 - y4) * (y3 - y4)) + ((z3 - z4) * (z3 - z4));
      length1 = powf(length1, 0.5);
      length2 = powf(length2, 0.5);
      length3 = powf(length3, 0.5);
      length4 = powf(length4, 0.5);
      length5 = powf(length5, 0.5);
      length6 = powf(length6, 0.5);
      element[k].set_edgelengths(length1, length2, length3, length4, length5, length6);
      float determinant = ((b1x * a2y * b3z) + (b1y * a2z * b3x) + (a2x * b3y * b1z)) - ((b3x * a2y * b1z) + (a2x * b1y * b3z) + (a2z * b3y * b1x));
      float absdeterminant = fabs(determinant);
      float vol = absdeterminant / 6.0;
      element[k].set_volume(vol);
      if (grainname > 0)
        grainvol[grainname] = grainvol[grainname] + vol;
      avgvol = avgvol + vol;
      count++;
    }
  }
  avgvol = avgvol / double(count);
  float totalvol = 0;
  for (int i = 1; i < numgrains + 1; i++)
  {
//    float truevol = grain[i].volume;
   // float error = (truevol - grainvol[i]) / truevol;
    totalvol = totalvol + grainvol[i];
  }
  for (int l = 0; l < elementcount; l++)
  {
    if (element[l].elementkilled != 1)
    {
      int bin = 0;
      float volume = element[l].volume;
      float ratio = volume / avgvol;
      if (ratio < 1.0 / 10.0)
        bin = 0;
      if (ratio >= 1.0 / 10.0 && ratio < 1.0 / 9.0)
        bin = 1;
      if (ratio >= 1.0 / 9.0 && ratio < 1.0 / 8.0)
        bin = 2;
      if (ratio >= 1.0 / 8.0 && ratio < 1.0 / 7.0)
        bin = 3;
      if (ratio >= 1.0 / 7.0 && ratio < 1.0 / 6.0)
        bin = 4;
      if (ratio >= 1.0 / 6.0 && ratio < 1.0 / 5.0)
        bin = 5;
      if (ratio >= 1.0 / 5.0 && ratio < 1.0 / 4.0)
        bin = 6;
      if (ratio >= 1.0 / 4.0 && ratio < 1.0 / 3.0)
        bin = 7;
      if (ratio >= 1.0 / 3.0 && ratio < 1.0 / 2.0)
        bin = 8;
      if (ratio >= 1.0 / 2.0 && ratio < 1.0 / 1.0)
        bin = 9;
      if (ratio >= 1.0 / 1.0 && ratio < 2.0 / 1.0)
        bin = 10;
      if (ratio >= 2.0 / 1.0 && ratio < 3.0 / 1.0)
        bin = 11;
      if (ratio >= 3.0 / 1.0 && ratio < 4.0 / 1.0)
        bin = 12;
      if (ratio >= 4.0 / 1.0 && ratio < 5.0 / 1.0)
        bin = 13;
      if (ratio >= 5.0 / 1.0 && ratio < 6.0 / 1.0)
        bin = 14;
      if (ratio >= 6.0 / 1.0 && ratio < 7.0 / 1.0)
        bin = 15;
      if (ratio >= 7.0 / 1.0 && ratio < 8.0 / 1.0)
        bin = 16;
      if (ratio >= 8.0 / 1.0 && ratio < 9.0 / 1.0)
        bin = 17;
      if (ratio >= 9.0 / 1.0 && ratio < 10.0 / 1.0)
        bin = 18;
      if (ratio >= 10.0 / 1.0 && ratio < 11.0 / 1.0)
        bin = 19;
      if (ratio >= 11.0 / 1.0 && ratio < 12.0 / 1.0)
        bin = 20;
      if (ratio >= 12.0 / 1.0 && ratio < 13.0 / 1.0)
        bin = 21;
      if (ratio >= 13.0 / 1.0 && ratio < 14.0 / 1.0)
        bin = 22;
      if (ratio >= 14.0 / 1.0 && ratio < 15.0 / 1.0)
        bin = 23;
      if (ratio >= 15.0 / 1.0)
        bin = 24;
      int grainname = element[l].grainname;
      int onedge = grain[grainname].edgegrain;
      int surfnodecount = element[l].originalnodecount;
      if (onedge == 0 && surfnodecount == 0)
      {
        volumebulk[bin]++;
      }
      if (onedge == 0 && surfnodecount > 0)
      {
        volumesurf[bin]++;
      }
    }
  }
}
void improve_mesh()
{
  float minedgelength = 0;
  int edgetokill = 0;
//  int nodetokill = 0;
//  int nodetomove = 0;
  int killed = 1;
  int count = elementcount;
  int insidecount = elementcount;
  float nx = 0;
  float ny = 0;
  float nz = 0;
  while (count > 0)
  {
    count = 0;
    insidecount = 0;
    for (int i = 0; i < elementcount; i++)
    {
 //    int gnum = element[i].grainname;
//      int oedge = grain[gnum].edgegrain;
      if (element[i].elementkilled != 1)
      {
        insidecount++;
        int node1 = element[i].node1;
        int node2 = element[i].node2;
        int node3 = element[i].node3;
        int node4 = element[i].node4;
//        int numgrains1 = finalnode[node1].numgrains;
//        int numgrains2 = finalnode[node2].numgrains;
//        int numgrains3 = finalnode[node3].numgrains;
//        int numgrains4 = finalnode[node4].numgrains;
        float dihedralangle1 = element[i].dihedralangle1;
        float dihedralangle2 = element[i].dihedralangle2;
        float dihedralangle3 = element[i].dihedralangle3;
        float dihedralangle4 = element[i].dihedralangle4;
        float dihedralangle5 = element[i].dihedralangle5;
        float dihedralangle6 = element[i].dihedralangle6;
        if (dihedralangle1 < 10 || dihedralangle1 > 170 || dihedralangle2 < 10 || dihedralangle2 > 170 || dihedralangle3 < 10 || dihedralangle3 > 170
            || dihedralangle4 < 10 || dihedralangle4 > 170 || dihedralangle5 < 10 || dihedralangle5 > 170 || dihedralangle6 < 10 || dihedralangle6 > 170)
        //        if(dihedralangle1 < 15 || dihedralangle1 > 165 || dihedralangle2 < 15 || dihedralangle2 > 165 || dihedralangle3 < 15 || dihedralangle3 > 165 || dihedralangle4 < 15 || dihedralangle4 > 165 || dihedralangle5 < 15 || dihedralangle5 > 165 || dihedralangle6 < 15 || dihedralangle6 > 165)
        //        if(dihedralangle1 < 20 || dihedralangle1 > 160 || dihedralangle2 < 20 || dihedralangle2 > 160 || dihedralangle3 < 20 || dihedralangle3 > 160 || dihedralangle4 < 20 || dihedralangle4 > 160 || dihedralangle5 < 20 || dihedralangle5 > 160 || dihedralangle6 < 20 || dihedralangle6 > 160)
        {
          minedgelength = element[i].edgelength1;
          edgetokill = 1;
          if (minedgelength > element[i].edgelength2)
            minedgelength = element[i].edgelength2, edgetokill = 2;
          if (minedgelength > element[i].edgelength3)
            minedgelength = element[i].edgelength3, edgetokill = 3;
          if (minedgelength > element[i].edgelength4)
            minedgelength = element[i].edgelength4, edgetokill = 4;
          if (minedgelength > element[i].edgelength5)
            minedgelength = element[i].edgelength5, edgetokill = 5;
          if (minedgelength > element[i].edgelength6)
            minedgelength = element[i].edgelength6, edgetokill = 6;
          element[i].set_killed(killed);
          if (edgetokill == 1)
          {
            count++;
            if (finalnode[node2].numgrains > finalnode[node1].numgrains)
            {
              int temp = node1;
              node1 = node2;
              node2 = temp;
            }
            int affected = 0;
            float ecx = (finalnode[node1].xc + finalnode[node2].xc) / 2.0f;
            float ecy = (finalnode[node1].yc + finalnode[node2].yc) / 2.0f;
            float ecz = (finalnode[node1].zc + finalnode[node2].zc) / 2.0f;
            finalnode[node1].set_coords(ecx, ecy, ecz);
            finalnode[node2].set_killed(killed);
            int size = elvector[node2].size();
            ;
            for (int l = 0; l < size; l++)
            {
              affected = 0;
              int firstelement = elvector[node2][l];
              int n1 = element[firstelement].node1;
              int n2 = element[firstelement].node2;
              int n3 = element[firstelement].node3;
              int n4 = element[firstelement].node4;
              if (n1 == node2)
                n1 = node1, affected = 1;
              if (n2 == node2)
                n2 = node1, affected = 1;
              if (n3 == node2)
                n3 = node1, affected = 1;
              if (n4 == node2)
                n4 = node1, affected = 1;
              element[firstelement].set_nodes(n1, n2, n3, n4);
              if (n1 == n2 || n1 == n3 || n1 == n4 || n2 == n3 || n2 == n4 || n3 == n4)
                element[firstelement].set_killed(killed);
              if (element[firstelement].elementkilled != 1)
              {
                if (affected == 1)
                {
                  elvector[node1].push_back(firstelement);
                }
                float x1 = finalnode[n1].xc;
                float y1 = finalnode[n1].yc;
                float z1 = finalnode[n1].zc;
                float x2 = finalnode[n2].xc;
                float y2 = finalnode[n2].yc;
                float z2 = finalnode[n2].zc;
                float x3 = finalnode[n3].xc;
                float y3 = finalnode[n3].yc;
                float z3 = finalnode[n3].zc;
                float x4 = finalnode[n4].xc;
                float y4 = finalnode[n4].yc;
                float z4 = finalnode[n4].zc;
                float a1x = x1 - x3;
                float a1y = y1 - y3;
                float a1z = z1 - z3;
                float b1x = x1 - x2;
                float b1y = y1 - y2;
                float b1z = z1 - z2;
                float n1x = (a1y * b1z) - (a1z * b1y);
                float n1y = (a1z * b1x) - (a1x * b1z);
                float n1z = (a1x * b1y) - (a1y * b1x);
                float a2x = x2 - x3;
                float a2y = y2 - y3;
                float a2z = z2 - z3;
                float b2x = x2 - x4;
                float b2y = y2 - y4;
                float b2z = z2 - z4;
                float n2x = (a2y * b2z) - (a2z * b2y);
                float n2y = (a2z * b2x) - (a2x * b2z);
                float n2z = (a2x * b2y) - (a2y * b2x);
                float a3x = x3 - x1;
                float a3y = y3 - y1;
                float a3z = z3 - z1;
                float b3x = x3 - x4;
                float b3y = y3 - y4;
                float b3z = z3 - z4;
                float n3x = (a3y * b3z) - (a3z * b3y);
                float n3y = (a3z * b3x) - (a3x * b3z);
                float n3z = (a3x * b3y) - (a3y * b3x);
                float a4x = x4 - x1;
                float a4y = y4 - y1;
                float a4z = z4 - z1;
                float b4x = x4 - x2;
                float b4y = y4 - y2;
                float b4z = z4 - z2;
                float n4x = (a4y * b4z) - (a4z * b4y);
                float n4y = (a4z * b4x) - (a4x * b4z);
                float n4z = (a4x * b4y) - (a4y * b4x);
                float denom11 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom12 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                denom11 = powf(denom11, 0.5);
                denom12 = powf(denom12, 0.5);
                float dihedral1 = ((n1x * n2x) + (n1y * n2y) + (n1z * n2z)) / (denom11 * denom12);
                dihedral1 = acos(dihedral1);
                float denom21 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom22 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                denom21 = powf(denom21, 0.5);
                denom22 = powf(denom22, 0.5);
                float dihedral2 = ((n1x * n3x) + (n1y * n3y) + (n1z * n3z)) / (denom21 * denom22);
                dihedral2 = acos(dihedral2);
                float denom31 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom32 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom31 = powf(denom31, 0.5);
                denom32 = powf(denom32, 0.5);
                float dihedral3 = ((n1x * n4x) + (n1y * n4y) + (n1z * n4z)) / (denom31 * denom32);
                dihedral3 = acos(dihedral3);
                float denom41 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                float denom42 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                denom41 = powf(denom41, 0.5);
                denom42 = powf(denom42, 0.5);
                float dihedral4 = ((n2x * n3x) + (n2y * n3y) + (n2z * n3z)) / (denom41 * denom42);
                dihedral4 = acos(dihedral4);
                float denom51 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                float denom52 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom51 = powf(denom51, 0.5);
                denom52 = powf(denom52, 0.5);
                float dihedral5 = ((n2x * n4x) + (n2y * n4y) + (n2z * n4z)) / (denom51 * denom52);
                dihedral5 = acos(dihedral5);
                float denom61 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                float denom62 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom61 = powf(denom61, 0.5);
                denom62 = powf(denom62, 0.5);
                float dihedral6 = ((n3x * n4x) + (n3y * n4y) + (n3z * n4z)) / (denom61 * denom62);
                dihedral6 = acos(dihedral6);
                dihedral1 = (180.0f / 3.1415926535897f) * dihedral1;
                dihedral2 = (180.0f / 3.1415926535897f) * dihedral2;
                dihedral3 = (180.0f / 3.1415926535897f) * dihedral3;
                dihedral4 = (180.0f / 3.1415926535897f) * dihedral4;
                dihedral5 = (180.0f / 3.1415926535897f) * dihedral5;
                dihedral6 = (180.0f / 3.1415926535897f) * dihedral6;
                dihedral1 = 180.0f - dihedral1;
                dihedral2 = 180.0f - dihedral2;
                dihedral3 = 180.0f - dihedral3;
                dihedral4 = 180.0f - dihedral4;
                dihedral5 = 180.0f - dihedral5;
                dihedral6 = 180.0f - dihedral6;
                element[firstelement].set_dihedralangles(dihedral1, dihedral2, dihedral3, dihedral4, dihedral5, dihedral6);
                float length1 = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
                float length2 = ((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)) + ((z1 - z3) * (z1 - z3));
                float length3 = ((x1 - x4) * (x1 - x4)) + ((y1 - y4) * (y1 - y4)) + ((z1 - z4) * (z1 - z4));
                float length4 = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)) + ((z2 - z3) * (z2 - z3));
                float length5 = ((x2 - x4) * (x2 - x4)) + ((y2 - y4) * (y2 - y4)) + ((z2 - z4) * (z2 - z4));
                float length6 = ((x3 - x4) * (x3 - x4)) + ((y3 - y4) * (y3 - y4)) + ((z3 - z4) * (z3 - z4));
                length1 = powf(length1, 0.5);
                length2 = powf(length2, 0.5);
                length3 = powf(length3, 0.5);
                length4 = powf(length4, 0.5);
                length5 = powf(length5, 0.5);
                length6 = powf(length6, 0.5);
                element[firstelement].set_edgelengths(length1, length2, length3, length4, length5, length6);
              }
            }
            size = nnvector[node2].size();
            for (int l = 0; l < size; l++)
            {
              int firstneighbornode = nnvector[node2][l];
              nnvector[node1].push_back(firstneighbornode);
            }
            if (finalnode[node1].numgrains < 2)
            {
              nx = 0;
              ny = 0;
              nz = 0;
              int counter = 0;
              size = nnvector[node1].size();
              for (int l = 0; l < size; l++)
              {
                int firstneighbornode = nnvector[node1][l];
                if (finalnode[firstneighbornode].nodekilled != 1)
                {
                  float x = finalnode[firstneighbornode].xc;
                  float y = finalnode[firstneighbornode].yc;
                  float z = finalnode[firstneighbornode].zc;
                  nx = nx + x;
                  ny = ny + y;
                  nz = nz + z;
                  counter++;
                }
              }
              nx = nx / counter;
              ny = ny / counter;
              nz = nz / counter;
              //              node[node1].set_coords(nx,ny,nz);
              size = elvector[node1].size();
              for (int l = 0; l < size; l++)
              {
                int firstelement = elvector[node1][l];
                int n1 = element[firstelement].node1;
                int n2 = element[firstelement].node2;
                int n3 = element[firstelement].node3;
                int n4 = element[firstelement].node4;
                if (element[firstelement].elementkilled != 1)
                {
                  float x1 = finalnode[n1].xc;
                  float y1 = finalnode[n1].yc;
                  float z1 = finalnode[n1].zc;
                  float x2 = finalnode[n2].xc;
                  float y2 = finalnode[n2].yc;
                  float z2 = finalnode[n2].zc;
                  float x3 = finalnode[n3].xc;
                  float y3 = finalnode[n3].yc;
                  float z3 = finalnode[n3].zc;
                  float x4 = finalnode[n4].xc;
                  float y4 = finalnode[n4].yc;
                  float z4 = finalnode[n4].zc;
                  float a1x = x1 - x3;
                  float a1y = y1 - y3;
                  float a1z = z1 - z3;
                  float b1x = x1 - x2;
                  float b1y = y1 - y2;
                  float b1z = z1 - z2;
                  float n1x = (a1y * b1z) - (a1z * b1y);
                  float n1y = (a1z * b1x) - (a1x * b1z);
                  float n1z = (a1x * b1y) - (a1y * b1x);
                  float a2x = x2 - x3;
                  float a2y = y2 - y3;
                  float a2z = z2 - z3;
                  float b2x = x2 - x4;
                  float b2y = y2 - y4;
                  float b2z = z2 - z4;
                  float n2x = (a2y * b2z) - (a2z * b2y);
                  float n2y = (a2z * b2x) - (a2x * b2z);
                  float n2z = (a2x * b2y) - (a2y * b2x);
                  float a3x = x3 - x1;
                  float a3y = y3 - y1;
                  float a3z = z3 - z1;
                  float b3x = x3 - x4;
                  float b3y = y3 - y4;
                  float b3z = z3 - z4;
                  float n3x = (a3y * b3z) - (a3z * b3y);
                  float n3y = (a3z * b3x) - (a3x * b3z);
                  float n3z = (a3x * b3y) - (a3y * b3x);
                  float a4x = x4 - x1;
                  float a4y = y4 - y1;
                  float a4z = z4 - z1;
                  float b4x = x4 - x2;
                  float b4y = y4 - y2;
                  float b4z = z4 - z2;
                  float n4x = (a4y * b4z) - (a4z * b4y);
                  float n4y = (a4z * b4x) - (a4x * b4z);
                  float n4z = (a4x * b4y) - (a4y * b4x);
                  float denom11 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom12 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  denom11 = powf(denom11, 0.5);
                  denom12 = powf(denom12, 0.5);
                  float dihedral1 = ((n1x * n2x) + (n1y * n2y) + (n1z * n2z)) / (denom11 * denom12);
                  dihedral1 = acos(dihedral1);
                  float denom21 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom22 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  denom21 = powf(denom21, 0.5);
                  denom22 = powf(denom22, 0.5);
                  float dihedral2 = ((n1x * n3x) + (n1y * n3y) + (n1z * n3z)) / (denom21 * denom22);
                  dihedral2 = acos(dihedral2);
                  float denom31 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom32 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom31 = powf(denom31, 0.5);
                  denom32 = powf(denom32, 0.5);
                  float dihedral3 = ((n1x * n4x) + (n1y * n4y) + (n1z * n4z)) / (denom31 * denom32);
                  dihedral3 = acos(dihedral3);
                  float denom41 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  float denom42 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  denom41 = powf(denom41, 0.5);
                  denom42 = powf(denom42, 0.5);
                  float dihedral4 = ((n2x * n3x) + (n2y * n3y) + (n2z * n3z)) / (denom41 * denom42);
                  dihedral4 = acos(dihedral4);
                  float denom51 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  float denom52 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom51 = powf(denom51, 0.5);
                  denom52 = powf(denom52, 0.5);
                  float dihedral5 = ((n2x * n4x) + (n2y * n4y) + (n2z * n4z)) / (denom51 * denom52);
                  dihedral5 = acos(dihedral5);
                  float denom61 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  float denom62 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom61 = powf(denom61, 0.5);
                  denom62 = powf(denom62, 0.5);
                  float dihedral6 = ((n3x * n4x) + (n3y * n4y) + (n3z * n4z)) / (denom61 * denom62);
                  dihedral6 = acos(dihedral6);
                  dihedral1 = (180.0f / 3.1415926535897f) * dihedral1;
                  dihedral2 = (180.0f / 3.1415926535897f) * dihedral2;
                  dihedral3 = (180.0f / 3.1415926535897f) * dihedral3;
                  dihedral4 = (180.0f / 3.1415926535897f) * dihedral4;
                  dihedral5 = (180.0f / 3.1415926535897f) * dihedral5;
                  dihedral6 = (180.0f / 3.1415926535897f) * dihedral6;
                  dihedral1 = 180.0f - dihedral1;
                  dihedral2 = 180.0f - dihedral2;
                  dihedral3 = 180.0f - dihedral3;
                  dihedral4 = 180.0f - dihedral4;
                  dihedral5 = 180.0f - dihedral5;
                  dihedral6 = 180.0f - dihedral6;
                  element[firstelement].set_dihedralangles(dihedral1, dihedral2, dihedral3, dihedral4, dihedral5, dihedral6);
                  float length1 = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
                  float length2 = ((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)) + ((z1 - z3) * (z1 - z3));
                  float length3 = ((x1 - x4) * (x1 - x4)) + ((y1 - y4) * (y1 - y4)) + ((z1 - z4) * (z1 - z4));
                  float length4 = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)) + ((z2 - z3) * (z2 - z3));
                  float length5 = ((x2 - x4) * (x2 - x4)) + ((y2 - y4) * (y2 - y4)) + ((z2 - z4) * (z2 - z4));
                  float length6 = ((x3 - x4) * (x3 - x4)) + ((y3 - y4) * (y3 - y4)) + ((z3 - z4) * (z3 - z4));
                  length1 = powf(length1, 0.5);
                  length2 = powf(length2, 0.5);
                  length3 = powf(length3, 0.5);
                  length4 = powf(length4, 0.5);
                  length5 = powf(length5, 0.5);
                  length6 = powf(length6, 0.5);
                  element[firstelement].set_edgelengths(length1, length2, length3, length4, length5, length6);
                }
              }
            }
          }
          if (edgetokill == 2)
          {
            count++;
            if (finalnode[node3].numgrains > finalnode[node1].numgrains)
            {
              int temp = node1;
              node1 = node3;
              node3 = temp;
            }
            int affected = 0;
            float ecx = (finalnode[node1].xc + finalnode[node3].xc) / 2.0f;
            float ecy = (finalnode[node1].yc + finalnode[node3].yc) / 2.0f;
            float ecz = (finalnode[node1].zc + finalnode[node3].zc) / 2.0f;
            finalnode[node1].set_coords(ecx, ecy, ecz);
            finalnode[node3].set_killed(killed);
            int size = elvector[node3].size();
            ;
            for (int l = 0; l < size; l++)
            {
              affected = 0;
              int firstelement = elvector[node3][l];
              int n1 = element[firstelement].node1;
              int n2 = element[firstelement].node3;
              int n3 = element[firstelement].node3;
              int n4 = element[firstelement].node4;
              if (n1 == node3)
                n1 = node1, affected = 1;
              if (n2 == node3)
                n2 = node1, affected = 1;
              if (n3 == node3)
                n3 = node1, affected = 1;
              if (n4 == node3)
                n4 = node1, affected = 1;
              element[firstelement].set_nodes(n1, n2, n3, n4);
              if (n1 == n2 || n1 == n3 || n1 == n4 || n2 == n3 || n2 == n4 || n3 == n4)
                element[firstelement].set_killed(killed);
              if (element[firstelement].elementkilled != 1)
              {
                if (affected == 1)
                {
                  elvector[node1].push_back(firstelement);
                }
                float x1 = finalnode[n1].xc;
                float y1 = finalnode[n1].yc;
                float z1 = finalnode[n1].zc;
                float x2 = finalnode[n2].xc;
                float y2 = finalnode[n2].yc;
                float z2 = finalnode[n2].zc;
                float x3 = finalnode[n3].xc;
                float y3 = finalnode[n3].yc;
                float z3 = finalnode[n3].zc;
                float x4 = finalnode[n4].xc;
                float y4 = finalnode[n4].yc;
                float z4 = finalnode[n4].zc;
                float a1x = x1 - x3;
                float a1y = y1 - y3;
                float a1z = z1 - z3;
                float b1x = x1 - x2;
                float b1y = y1 - y2;
                float b1z = z1 - z2;
                float n1x = (a1y * b1z) - (a1z * b1y);
                float n1y = (a1z * b1x) - (a1x * b1z);
                float n1z = (a1x * b1y) - (a1y * b1x);
                float a2x = x2 - x3;
                float a2y = y2 - y3;
                float a2z = z2 - z3;
                float b2x = x2 - x4;
                float b2y = y2 - y4;
                float b2z = z2 - z4;
                float n2x = (a2y * b2z) - (a2z * b2y);
                float n2y = (a2z * b2x) - (a2x * b2z);
                float n2z = (a2x * b2y) - (a2y * b2x);
                float a3x = x3 - x1;
                float a3y = y3 - y1;
                float a3z = z3 - z1;
                float b3x = x3 - x4;
                float b3y = y3 - y4;
                float b3z = z3 - z4;
                float n3x = (a3y * b3z) - (a3z * b3y);
                float n3y = (a3z * b3x) - (a3x * b3z);
                float n3z = (a3x * b3y) - (a3y * b3x);
                float a4x = x4 - x1;
                float a4y = y4 - y1;
                float a4z = z4 - z1;
                float b4x = x4 - x2;
                float b4y = y4 - y2;
                float b4z = z4 - z2;
                float n4x = (a4y * b4z) - (a4z * b4y);
                float n4y = (a4z * b4x) - (a4x * b4z);
                float n4z = (a4x * b4y) - (a4y * b4x);
                float denom11 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom12 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                denom11 = powf(denom11, 0.5);
                denom12 = powf(denom12, 0.5);
                float dihedral1 = ((n1x * n2x) + (n1y * n2y) + (n1z * n2z)) / (denom11 * denom12);
                dihedral1 = acos(dihedral1);
                float denom21 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom22 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                denom21 = powf(denom21, 0.5);
                denom22 = powf(denom22, 0.5);
                float dihedral2 = ((n1x * n3x) + (n1y * n3y) + (n1z * n3z)) / (denom21 * denom22);
                dihedral2 = acos(dihedral2);
                float denom31 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom32 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom31 = powf(denom31, 0.5);
                denom32 = powf(denom32, 0.5);
                float dihedral3 = ((n1x * n4x) + (n1y * n4y) + (n1z * n4z)) / (denom31 * denom32);
                dihedral3 = acos(dihedral3);
                float denom41 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                float denom42 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                denom41 = powf(denom41, 0.5);
                denom42 = powf(denom42, 0.5);
                float dihedral4 = ((n2x * n3x) + (n2y * n3y) + (n2z * n3z)) / (denom41 * denom42);
                dihedral4 = acos(dihedral4);
                float denom51 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                float denom52 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom51 = powf(denom51, 0.5);
                denom52 = powf(denom52, 0.5);
                float dihedral5 = ((n2x * n4x) + (n2y * n4y) + (n2z * n4z)) / (denom51 * denom52);
                dihedral5 = acos(dihedral5);
                float denom61 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                float denom62 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom61 = powf(denom61, 0.5);
                denom62 = powf(denom62, 0.5);
                float dihedral6 = ((n3x * n4x) + (n3y * n4y) + (n3z * n4z)) / (denom61 * denom62);
                dihedral6 = acos(dihedral6);
                dihedral1 = (180.0f / 3.1415926535897f) * dihedral1;
                dihedral2 = (180.0f / 3.1415926535897f) * dihedral2;
                dihedral3 = (180.0f / 3.1415926535897f) * dihedral3;
                dihedral4 = (180.0f / 3.1415926535897f) * dihedral4;
                dihedral5 = (180.0f / 3.1415926535897f) * dihedral5;
                dihedral6 = (180.0f / 3.1415926535897f) * dihedral6;
                dihedral1 = 180.0f - dihedral1;
                dihedral2 = 180.0f - dihedral2;
                dihedral3 = 180.0f - dihedral3;
                dihedral4 = 180.0f - dihedral4;
                dihedral5 = 180.0f - dihedral5;
                dihedral6 = 180.0f - dihedral6;
                element[firstelement].set_dihedralangles(dihedral1, dihedral2, dihedral3, dihedral4, dihedral5, dihedral6);
                float length1 = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
                float length2 = ((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)) + ((z1 - z3) * (z1 - z3));
                float length3 = ((x1 - x4) * (x1 - x4)) + ((y1 - y4) * (y1 - y4)) + ((z1 - z4) * (z1 - z4));
                float length4 = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)) + ((z2 - z3) * (z2 - z3));
                float length5 = ((x2 - x4) * (x2 - x4)) + ((y2 - y4) * (y2 - y4)) + ((z2 - z4) * (z2 - z4));
                float length6 = ((x3 - x4) * (x3 - x4)) + ((y3 - y4) * (y3 - y4)) + ((z3 - z4) * (z3 - z4));
                length1 = powf(length1, 0.5);
                length2 = powf(length2, 0.5);
                length3 = powf(length3, 0.5);
                length4 = powf(length4, 0.5);
                length5 = powf(length5, 0.5);
                length6 = powf(length6, 0.5);
                element[firstelement].set_edgelengths(length1, length2, length3, length4, length5, length6);
              }
            }
            size = nnvector[node3].size();
            for (int l = 0; l < size; l++)
            {
              int firstneighbornode = nnvector[node3][l];
              nnvector[node1].push_back(firstneighbornode);
            }
            if (finalnode[node1].numgrains < 2)
            {
              nx = 0;
              ny = 0;
              nz = 0;
              int counter = 0;
              size = nnvector[node1].size();
              for (int l = 0; l < size; l++)
              {
                int firstneighbornode = nnvector[node1][l];
                if (finalnode[firstneighbornode].nodekilled != 1)
                {
                  float x = finalnode[firstneighbornode].xc;
                  float y = finalnode[firstneighbornode].yc;
                  float z = finalnode[firstneighbornode].zc;
                  nx = nx + x;
                  ny = ny + y;
                  nz = nz + z;
                  counter++;
                }
              }
              nx = nx / counter;
              ny = ny / counter;
              nz = nz / counter;
              //              node[node1].set_coords(nx,ny,nz);
              size = elvector[node1].size();
              for (int l = 0; l < size; l++)
              {
                int firstelement = elvector[node1][l];
                int n1 = element[firstelement].node1;
                int n2 = element[firstelement].node3;
                int n3 = element[firstelement].node3;
                int n4 = element[firstelement].node4;
                if (element[firstelement].elementkilled != 1)
                {
                  float x1 = finalnode[n1].xc;
                  float y1 = finalnode[n1].yc;
                  float z1 = finalnode[n1].zc;
                  float x2 = finalnode[n2].xc;
                  float y2 = finalnode[n2].yc;
                  float z2 = finalnode[n2].zc;
                  float x3 = finalnode[n3].xc;
                  float y3 = finalnode[n3].yc;
                  float z3 = finalnode[n3].zc;
                  float x4 = finalnode[n4].xc;
                  float y4 = finalnode[n4].yc;
                  float z4 = finalnode[n4].zc;
                  float a1x = x1 - x3;
                  float a1y = y1 - y3;
                  float a1z = z1 - z3;
                  float b1x = x1 - x2;
                  float b1y = y1 - y2;
                  float b1z = z1 - z2;
                  float n1x = (a1y * b1z) - (a1z * b1y);
                  float n1y = (a1z * b1x) - (a1x * b1z);
                  float n1z = (a1x * b1y) - (a1y * b1x);
                  float a2x = x2 - x3;
                  float a2y = y2 - y3;
                  float a2z = z2 - z3;
                  float b2x = x2 - x4;
                  float b2y = y2 - y4;
                  float b2z = z2 - z4;
                  float n2x = (a2y * b2z) - (a2z * b2y);
                  float n2y = (a2z * b2x) - (a2x * b2z);
                  float n2z = (a2x * b2y) - (a2y * b2x);
                  float a3x = x3 - x1;
                  float a3y = y3 - y1;
                  float a3z = z3 - z1;
                  float b3x = x3 - x4;
                  float b3y = y3 - y4;
                  float b3z = z3 - z4;
                  float n3x = (a3y * b3z) - (a3z * b3y);
                  float n3y = (a3z * b3x) - (a3x * b3z);
                  float n3z = (a3x * b3y) - (a3y * b3x);
                  float a4x = x4 - x1;
                  float a4y = y4 - y1;
                  float a4z = z4 - z1;
                  float b4x = x4 - x2;
                  float b4y = y4 - y2;
                  float b4z = z4 - z2;
                  float n4x = (a4y * b4z) - (a4z * b4y);
                  float n4y = (a4z * b4x) - (a4x * b4z);
                  float n4z = (a4x * b4y) - (a4y * b4x);
                  float denom11 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom12 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  denom11 = powf(denom11, 0.5);
                  denom12 = powf(denom12, 0.5);
                  float dihedral1 = ((n1x * n2x) + (n1y * n2y) + (n1z * n2z)) / (denom11 * denom12);
                  dihedral1 = acos(dihedral1);
                  float denom21 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom22 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  denom21 = powf(denom21, 0.5);
                  denom22 = powf(denom22, 0.5);
                  float dihedral2 = ((n1x * n3x) + (n1y * n3y) + (n1z * n3z)) / (denom21 * denom22);
                  dihedral2 = acos(dihedral2);
                  float denom31 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom32 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom31 = powf(denom31, 0.5);
                  denom32 = powf(denom32, 0.5);
                  float dihedral3 = ((n1x * n4x) + (n1y * n4y) + (n1z * n4z)) / (denom31 * denom32);
                  dihedral3 = acos(dihedral3);
                  float denom41 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  float denom42 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  denom41 = powf(denom41, 0.5);
                  denom42 = powf(denom42, 0.5);
                  float dihedral4 = ((n2x * n3x) + (n2y * n3y) + (n2z * n3z)) / (denom41 * denom42);
                  dihedral4 = acos(dihedral4);
                  float denom51 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  float denom52 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom51 = powf(denom51, 0.5);
                  denom52 = powf(denom52, 0.5);
                  float dihedral5 = ((n2x * n4x) + (n2y * n4y) + (n2z * n4z)) / (denom51 * denom52);
                  dihedral5 = acos(dihedral5);
                  float denom61 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  float denom62 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom61 = powf(denom61, 0.5);
                  denom62 = powf(denom62, 0.5);
                  float dihedral6 = ((n3x * n4x) + (n3y * n4y) + (n3z * n4z)) / (denom61 * denom62);
                  dihedral6 = acos(dihedral6);
                  dihedral1 = (180.0f / 3.1415926535897f) * dihedral1;
                  dihedral2 = (180.0f / 3.1415926535897f) * dihedral2;
                  dihedral3 = (180.0f / 3.1415926535897f) * dihedral3;
                  dihedral4 = (180.0f / 3.1415926535897f) * dihedral4;
                  dihedral5 = (180.0f / 3.1415926535897f) * dihedral5;
                  dihedral6 = (180.0f / 3.1415926535897f) * dihedral6;
                  dihedral1 = 180.0f - dihedral1;
                  dihedral2 = 180.0f - dihedral2;
                  dihedral3 = 180.0f - dihedral3;
                  dihedral4 = 180.0f - dihedral4;
                  dihedral5 = 180.0f - dihedral5;
                  dihedral6 = 180.0f - dihedral6;
                  element[firstelement].set_dihedralangles(dihedral1, dihedral2, dihedral3, dihedral4, dihedral5, dihedral6);
                  float length1 = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
                  float length2 = ((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)) + ((z1 - z3) * (z1 - z3));
                  float length3 = ((x1 - x4) * (x1 - x4)) + ((y1 - y4) * (y1 - y4)) + ((z1 - z4) * (z1 - z4));
                  float length4 = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)) + ((z2 - z3) * (z2 - z3));
                  float length5 = ((x2 - x4) * (x2 - x4)) + ((y2 - y4) * (y2 - y4)) + ((z2 - z4) * (z2 - z4));
                  float length6 = ((x3 - x4) * (x3 - x4)) + ((y3 - y4) * (y3 - y4)) + ((z3 - z4) * (z3 - z4));
                  length1 = powf(length1, 0.5);
                  length2 = powf(length2, 0.5);
                  length3 = powf(length3, 0.5);
                  length4 = powf(length4, 0.5);
                  length5 = powf(length5, 0.5);
                  length6 = powf(length6, 0.5);
                  element[firstelement].set_edgelengths(length1, length2, length3, length4, length5, length6);
                }
              }
            }
          }
          if (edgetokill == 3)
          {
            count++;
            if (finalnode[node4].numgrains > finalnode[node1].numgrains)
            {
              int temp = node1;
              node1 = node4;
              node4 = temp;
            }
            int affected = 0;
            float ecx = (finalnode[node1].xc + finalnode[node4].xc) / 2.0f;
            float ecy = (finalnode[node1].yc + finalnode[node4].yc) / 2.0f;
            float ecz = (finalnode[node1].zc + finalnode[node4].zc) / 2.0f;
            finalnode[node1].set_coords(ecx, ecy, ecz);
            finalnode[node4].set_killed(killed);
            int size = elvector[node4].size();
            ;
            for (int l = 0; l < size; l++)
            {
              affected = 0;
              int firstelement = elvector[node4][l];
              int n1 = element[firstelement].node1;
              int n2 = element[firstelement].node4;
              int n3 = element[firstelement].node3;
              int n4 = element[firstelement].node4;
              if (n1 == node4)
                n1 = node1, affected = 1;
              if (n2 == node4)
                n2 = node1, affected = 1;
              if (n3 == node4)
                n3 = node1, affected = 1;
              if (n4 == node4)
                n4 = node1, affected = 1;
              element[firstelement].set_nodes(n1, n2, n3, n4);
              if (n1 == n2 || n1 == n3 || n1 == n4 || n2 == n3 || n2 == n4 || n3 == n4)
                element[firstelement].set_killed(killed);
              if (element[firstelement].elementkilled != 1)
              {
                if (affected == 1)
                {
                  elvector[node1].push_back(firstelement);
                }
                float x1 = finalnode[n1].xc;
                float y1 = finalnode[n1].yc;
                float z1 = finalnode[n1].zc;
                float x2 = finalnode[n2].xc;
                float y2 = finalnode[n2].yc;
                float z2 = finalnode[n2].zc;
                float x3 = finalnode[n3].xc;
                float y3 = finalnode[n3].yc;
                float z3 = finalnode[n3].zc;
                float x4 = finalnode[n4].xc;
                float y4 = finalnode[n4].yc;
                float z4 = finalnode[n4].zc;
                float a1x = x1 - x3;
                float a1y = y1 - y3;
                float a1z = z1 - z3;
                float b1x = x1 - x2;
                float b1y = y1 - y2;
                float b1z = z1 - z2;
                float n1x = (a1y * b1z) - (a1z * b1y);
                float n1y = (a1z * b1x) - (a1x * b1z);
                float n1z = (a1x * b1y) - (a1y * b1x);
                float a2x = x2 - x3;
                float a2y = y2 - y3;
                float a2z = z2 - z3;
                float b2x = x2 - x4;
                float b2y = y2 - y4;
                float b2z = z2 - z4;
                float n2x = (a2y * b2z) - (a2z * b2y);
                float n2y = (a2z * b2x) - (a2x * b2z);
                float n2z = (a2x * b2y) - (a2y * b2x);
                float a3x = x3 - x1;
                float a3y = y3 - y1;
                float a3z = z3 - z1;
                float b3x = x3 - x4;
                float b3y = y3 - y4;
                float b3z = z3 - z4;
                float n3x = (a3y * b3z) - (a3z * b3y);
                float n3y = (a3z * b3x) - (a3x * b3z);
                float n3z = (a3x * b3y) - (a3y * b3x);
                float a4x = x4 - x1;
                float a4y = y4 - y1;
                float a4z = z4 - z1;
                float b4x = x4 - x2;
                float b4y = y4 - y2;
                float b4z = z4 - z2;
                float n4x = (a4y * b4z) - (a4z * b4y);
                float n4y = (a4z * b4x) - (a4x * b4z);
                float n4z = (a4x * b4y) - (a4y * b4x);
                float denom11 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom12 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                denom11 = powf(denom11, 0.5);
                denom12 = powf(denom12, 0.5);
                float dihedral1 = ((n1x * n2x) + (n1y * n2y) + (n1z * n2z)) / (denom11 * denom12);
                dihedral1 = acos(dihedral1);
                float denom21 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom22 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                denom21 = powf(denom21, 0.5);
                denom22 = powf(denom22, 0.5);
                float dihedral2 = ((n1x * n3x) + (n1y * n3y) + (n1z * n3z)) / (denom21 * denom22);
                dihedral2 = acos(dihedral2);
                float denom31 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom32 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom31 = powf(denom31, 0.5);
                denom32 = powf(denom32, 0.5);
                float dihedral3 = ((n1x * n4x) + (n1y * n4y) + (n1z * n4z)) / (denom31 * denom32);
                dihedral3 = acos(dihedral3);
                float denom41 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                float denom42 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                denom41 = powf(denom41, 0.5);
                denom42 = powf(denom42, 0.5);
                float dihedral4 = ((n2x * n3x) + (n2y * n3y) + (n2z * n3z)) / (denom41 * denom42);
                dihedral4 = acos(dihedral4);
                float denom51 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                float denom52 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom51 = powf(denom51, 0.5);
                denom52 = powf(denom52, 0.5);
                float dihedral5 = ((n2x * n4x) + (n2y * n4y) + (n2z * n4z)) / (denom51 * denom52);
                dihedral5 = acos(dihedral5);
                float denom61 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                float denom62 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom61 = powf(denom61, 0.5);
                denom62 = powf(denom62, 0.5);
                float dihedral6 = ((n3x * n4x) + (n3y * n4y) + (n3z * n4z)) / (denom61 * denom62);
                dihedral6 = acos(dihedral6);
                dihedral1 = (180.0f / 3.1415926535897f) * dihedral1;
                dihedral2 = (180.0f / 3.1415926535897f) * dihedral2;
                dihedral3 = (180.0f / 3.1415926535897f) * dihedral3;
                dihedral4 = (180.0f / 3.1415926535897f) * dihedral4;
                dihedral5 = (180.0f / 3.1415926535897f) * dihedral5;
                dihedral6 = (180.0f / 3.1415926535897f) * dihedral6;
                dihedral1 = 180.0f - dihedral1;
                dihedral2 = 180.0f - dihedral2;
                dihedral3 = 180.0f - dihedral3;
                dihedral4 = 180.0f - dihedral4;
                dihedral5 = 180.0f - dihedral5;
                dihedral6 = 180.0f - dihedral6;
                element[firstelement].set_dihedralangles(dihedral1, dihedral2, dihedral3, dihedral4, dihedral5, dihedral6);
                float length1 = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
                float length2 = ((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)) + ((z1 - z3) * (z1 - z3));
                float length3 = ((x1 - x4) * (x1 - x4)) + ((y1 - y4) * (y1 - y4)) + ((z1 - z4) * (z1 - z4));
                float length4 = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)) + ((z2 - z3) * (z2 - z3));
                float length5 = ((x2 - x4) * (x2 - x4)) + ((y2 - y4) * (y2 - y4)) + ((z2 - z4) * (z2 - z4));
                float length6 = ((x3 - x4) * (x3 - x4)) + ((y3 - y4) * (y3 - y4)) + ((z3 - z4) * (z3 - z4));
                length1 = powf(length1, 0.5);
                length2 = powf(length2, 0.5);
                length3 = powf(length3, 0.5);
                length4 = powf(length4, 0.5);
                length5 = powf(length5, 0.5);
                length6 = powf(length6, 0.5);
                element[firstelement].set_edgelengths(length1, length2, length3, length4, length5, length6);
              }
            }
            size = nnvector[node4].size();
            for (int l = 0; l < size; l++)
            {
              int firstneighbornode = nnvector[node4][l];
              nnvector[node1].push_back(firstneighbornode);
            }
            if (finalnode[node1].numgrains < 2)
            {
              nx = 0;
              ny = 0;
              nz = 0;
              int counter = 0;
              size = nnvector[node1].size();
              for (int l = 0; l < size; l++)
              {
                int firstneighbornode = nnvector[node1][l];
                if (finalnode[firstneighbornode].nodekilled != 1)
                {
                  float x = finalnode[firstneighbornode].xc;
                  float y = finalnode[firstneighbornode].yc;
                  float z = finalnode[firstneighbornode].zc;
                  nx = nx + x;
                  ny = ny + y;
                  nz = nz + z;
                  counter++;
                }
              }
              nx = nx / counter;
              ny = ny / counter;
              nz = nz / counter;
              //              node[node1].set_coords(nx,ny,nz);
              size = elvector[node1].size();
              for (int l = 0; l < size; l++)
              {
                int firstelement = elvector[node1][l];
                int n1 = element[firstelement].node1;
                int n2 = element[firstelement].node4;
                int n3 = element[firstelement].node3;
                int n4 = element[firstelement].node4;
                if (element[firstelement].elementkilled != 1)
                {
                  float x1 = finalnode[n1].xc;
                  float y1 = finalnode[n1].yc;
                  float z1 = finalnode[n1].zc;
                  float x2 = finalnode[n2].xc;
                  float y2 = finalnode[n2].yc;
                  float z2 = finalnode[n2].zc;
                  float x3 = finalnode[n3].xc;
                  float y3 = finalnode[n3].yc;
                  float z3 = finalnode[n3].zc;
                  float x4 = finalnode[n4].xc;
                  float y4 = finalnode[n4].yc;
                  float z4 = finalnode[n4].zc;
                  float a1x = x1 - x3;
                  float a1y = y1 - y3;
                  float a1z = z1 - z3;
                  float b1x = x1 - x2;
                  float b1y = y1 - y2;
                  float b1z = z1 - z2;
                  float n1x = (a1y * b1z) - (a1z * b1y);
                  float n1y = (a1z * b1x) - (a1x * b1z);
                  float n1z = (a1x * b1y) - (a1y * b1x);
                  float a2x = x2 - x3;
                  float a2y = y2 - y3;
                  float a2z = z2 - z3;
                  float b2x = x2 - x4;
                  float b2y = y2 - y4;
                  float b2z = z2 - z4;
                  float n2x = (a2y * b2z) - (a2z * b2y);
                  float n2y = (a2z * b2x) - (a2x * b2z);
                  float n2z = (a2x * b2y) - (a2y * b2x);
                  float a3x = x3 - x1;
                  float a3y = y3 - y1;
                  float a3z = z3 - z1;
                  float b3x = x3 - x4;
                  float b3y = y3 - y4;
                  float b3z = z3 - z4;
                  float n3x = (a3y * b3z) - (a3z * b3y);
                  float n3y = (a3z * b3x) - (a3x * b3z);
                  float n3z = (a3x * b3y) - (a3y * b3x);
                  float a4x = x4 - x1;
                  float a4y = y4 - y1;
                  float a4z = z4 - z1;
                  float b4x = x4 - x2;
                  float b4y = y4 - y2;
                  float b4z = z4 - z2;
                  float n4x = (a4y * b4z) - (a4z * b4y);
                  float n4y = (a4z * b4x) - (a4x * b4z);
                  float n4z = (a4x * b4y) - (a4y * b4x);
                  float denom11 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom12 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  denom11 = powf(denom11, 0.5);
                  denom12 = powf(denom12, 0.5);
                  float dihedral1 = ((n1x * n2x) + (n1y * n2y) + (n1z * n2z)) / (denom11 * denom12);
                  dihedral1 = acos(dihedral1);
                  float denom21 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom22 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  denom21 = powf(denom21, 0.5);
                  denom22 = powf(denom22, 0.5);
                  float dihedral2 = ((n1x * n3x) + (n1y * n3y) + (n1z * n3z)) / (denom21 * denom22);
                  dihedral2 = acos(dihedral2);
                  float denom31 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom32 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom31 = powf(denom31, 0.5);
                  denom32 = powf(denom32, 0.5);
                  float dihedral3 = ((n1x * n4x) + (n1y * n4y) + (n1z * n4z)) / (denom31 * denom32);
                  dihedral3 = acos(dihedral3);
                  float denom41 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  float denom42 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  denom41 = powf(denom41, 0.5);
                  denom42 = powf(denom42, 0.5);
                  float dihedral4 = ((n2x * n3x) + (n2y * n3y) + (n2z * n3z)) / (denom41 * denom42);
                  dihedral4 = acos(dihedral4);
                  float denom51 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  float denom52 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom51 = powf(denom51, 0.5);
                  denom52 = powf(denom52, 0.5);
                  float dihedral5 = ((n2x * n4x) + (n2y * n4y) + (n2z * n4z)) / (denom51 * denom52);
                  dihedral5 = acos(dihedral5);
                  float denom61 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  float denom62 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom61 = powf(denom61, 0.5);
                  denom62 = powf(denom62, 0.5);
                  float dihedral6 = ((n3x * n4x) + (n3y * n4y) + (n3z * n4z)) / (denom61 * denom62);
                  dihedral6 = acos(dihedral6);
                  dihedral1 = (180.0f / 3.1415926535897f) * dihedral1;
                  dihedral2 = (180.0f / 3.1415926535897f) * dihedral2;
                  dihedral3 = (180.0f / 3.1415926535897f) * dihedral3;
                  dihedral4 = (180.0f / 3.1415926535897f) * dihedral4;
                  dihedral5 = (180.0f / 3.1415926535897f) * dihedral5;
                  dihedral6 = (180.0f / 3.1415926535897f) * dihedral6;
                  dihedral1 = 180.0f - dihedral1;
                  dihedral2 = 180.0f - dihedral2;
                  dihedral3 = 180.0f - dihedral3;
                  dihedral4 = 180.0f - dihedral4;
                  dihedral5 = 180.0f - dihedral5;
                  dihedral6 = 180.0f - dihedral6;
                  element[firstelement].set_dihedralangles(dihedral1, dihedral2, dihedral3, dihedral4, dihedral5, dihedral6);
                  float length1 = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
                  float length2 = ((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)) + ((z1 - z3) * (z1 - z3));
                  float length3 = ((x1 - x4) * (x1 - x4)) + ((y1 - y4) * (y1 - y4)) + ((z1 - z4) * (z1 - z4));
                  float length4 = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)) + ((z2 - z3) * (z2 - z3));
                  float length5 = ((x2 - x4) * (x2 - x4)) + ((y2 - y4) * (y2 - y4)) + ((z2 - z4) * (z2 - z4));
                  float length6 = ((x3 - x4) * (x3 - x4)) + ((y3 - y4) * (y3 - y4)) + ((z3 - z4) * (z3 - z4));
                  length1 = powf(length1, 0.5);
                  length2 = powf(length2, 0.5);
                  length3 = powf(length3, 0.5);
                  length4 = powf(length4, 0.5);
                  length5 = powf(length5, 0.5);
                  length6 = powf(length6, 0.5);
                  element[firstelement].set_edgelengths(length1, length2, length3, length4, length5, length6);
                }
              }
            }
          }
          if (edgetokill == 4)
          {
            count++;
            if (finalnode[node3].numgrains > finalnode[node2].numgrains)
            {
              int temp = node2;
              node2 = node3;
              node3 = temp;
            }
            int affected = 0;
            float ecx = (finalnode[node2].xc + finalnode[node3].xc) / 2.0f;
            float ecy = (finalnode[node2].yc + finalnode[node3].yc) / 2.0f;
            float ecz = (finalnode[node2].zc + finalnode[node3].zc) / 2.0f;
            finalnode[node2].set_coords(ecx, ecy, ecz);
            finalnode[node3].set_killed(killed);
            int size = elvector[node3].size();
            ;
            for (int l = 0; l < size; l++)
            {
              affected = 0;
              int firstelement = elvector[node3][l];
              int n1 = element[firstelement].node2;
              int n2 = element[firstelement].node3;
              int n3 = element[firstelement].node3;
              int n4 = element[firstelement].node4;
              if (n1 == node3)
                n1 = node2, affected = 1;
              if (n2 == node3)
                n2 = node2, affected = 1;
              if (n3 == node3)
                n3 = node2, affected = 1;
              if (n4 == node3)
                n4 = node2, affected = 1;
              element[firstelement].set_nodes(n1, n2, n3, n4);
              if (n1 == n2 || n1 == n3 || n1 == n4 || n2 == n3 || n2 == n4 || n3 == n4)
                element[firstelement].set_killed(killed);
              if (element[firstelement].elementkilled != 1)
              {
                if (affected == 1)
                {
                  elvector[node2].push_back(firstelement);
                }
                float x1 = finalnode[n1].xc;
                float y1 = finalnode[n1].yc;
                float z1 = finalnode[n1].zc;
                float x2 = finalnode[n2].xc;
                float y2 = finalnode[n2].yc;
                float z2 = finalnode[n2].zc;
                float x3 = finalnode[n3].xc;
                float y3 = finalnode[n3].yc;
                float z3 = finalnode[n3].zc;
                float x4 = finalnode[n4].xc;
                float y4 = finalnode[n4].yc;
                float z4 = finalnode[n4].zc;
                float a1x = x1 - x3;
                float a1y = y1 - y3;
                float a1z = z1 - z3;
                float b1x = x1 - x2;
                float b1y = y1 - y2;
                float b1z = z1 - z2;
                float n1x = (a1y * b1z) - (a1z * b1y);
                float n1y = (a1z * b1x) - (a1x * b1z);
                float n1z = (a1x * b1y) - (a1y * b1x);
                float a2x = x2 - x3;
                float a2y = y2 - y3;
                float a2z = z2 - z3;
                float b2x = x2 - x4;
                float b2y = y2 - y4;
                float b2z = z2 - z4;
                float n2x = (a2y * b2z) - (a2z * b2y);
                float n2y = (a2z * b2x) - (a2x * b2z);
                float n2z = (a2x * b2y) - (a2y * b2x);
                float a3x = x3 - x1;
                float a3y = y3 - y1;
                float a3z = z3 - z1;
                float b3x = x3 - x4;
                float b3y = y3 - y4;
                float b3z = z3 - z4;
                float n3x = (a3y * b3z) - (a3z * b3y);
                float n3y = (a3z * b3x) - (a3x * b3z);
                float n3z = (a3x * b3y) - (a3y * b3x);
                float a4x = x4 - x1;
                float a4y = y4 - y1;
                float a4z = z4 - z1;
                float b4x = x4 - x2;
                float b4y = y4 - y2;
                float b4z = z4 - z2;
                float n4x = (a4y * b4z) - (a4z * b4y);
                float n4y = (a4z * b4x) - (a4x * b4z);
                float n4z = (a4x * b4y) - (a4y * b4x);
                float denom11 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom12 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                denom11 = powf(denom11, 0.5);
                denom12 = powf(denom12, 0.5);
                float dihedral1 = ((n1x * n2x) + (n1y * n2y) + (n1z * n2z)) / (denom11 * denom12);
                dihedral1 = acos(dihedral1);
                float denom21 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom22 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                denom21 = powf(denom21, 0.5);
                denom22 = powf(denom22, 0.5);
                float dihedral2 = ((n1x * n3x) + (n1y * n3y) + (n1z * n3z)) / (denom21 * denom22);
                dihedral2 = acos(dihedral2);
                float denom31 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom32 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom31 = powf(denom31, 0.5);
                denom32 = powf(denom32, 0.5);
                float dihedral3 = ((n1x * n4x) + (n1y * n4y) + (n1z * n4z)) / (denom31 * denom32);
                dihedral3 = acos(dihedral3);
                float denom41 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                float denom42 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                denom41 = powf(denom41, 0.5);
                denom42 = powf(denom42, 0.5);
                float dihedral4 = ((n2x * n3x) + (n2y * n3y) + (n2z * n3z)) / (denom41 * denom42);
                dihedral4 = acos(dihedral4);
                float denom51 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                float denom52 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom51 = powf(denom51, 0.5);
                denom52 = powf(denom52, 0.5);
                float dihedral5 = ((n2x * n4x) + (n2y * n4y) + (n2z * n4z)) / (denom51 * denom52);
                dihedral5 = acos(dihedral5);
                float denom61 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                float denom62 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom61 = powf(denom61, 0.5);
                denom62 = powf(denom62, 0.5);
                float dihedral6 = ((n3x * n4x) + (n3y * n4y) + (n3z * n4z)) / (denom61 * denom62);
                dihedral6 = acos(dihedral6);
                dihedral1 = (180.0f / 3.1415926535897f) * dihedral1;
                dihedral2 = (180.0f / 3.1415926535897f) * dihedral2;
                dihedral3 = (180.0f / 3.1415926535897f) * dihedral3;
                dihedral4 = (180.0f / 3.1415926535897f) * dihedral4;
                dihedral5 = (180.0f / 3.1415926535897f) * dihedral5;
                dihedral6 = (180.0f / 3.1415926535897f) * dihedral6;
                dihedral1 = 180.0f - dihedral1;
                dihedral2 = 180.0f - dihedral2;
                dihedral3 = 180.0f - dihedral3;
                dihedral4 = 180.0f - dihedral4;
                dihedral5 = 180.0f - dihedral5;
                dihedral6 = 180.0f - dihedral6;
                element[firstelement].set_dihedralangles(dihedral1, dihedral2, dihedral3, dihedral4, dihedral5, dihedral6);
                float length1 = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
                float length2 = ((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)) + ((z1 - z3) * (z1 - z3));
                float length3 = ((x1 - x4) * (x1 - x4)) + ((y1 - y4) * (y1 - y4)) + ((z1 - z4) * (z1 - z4));
                float length4 = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)) + ((z2 - z3) * (z2 - z3));
                float length5 = ((x2 - x4) * (x2 - x4)) + ((y2 - y4) * (y2 - y4)) + ((z2 - z4) * (z2 - z4));
                float length6 = ((x3 - x4) * (x3 - x4)) + ((y3 - y4) * (y3 - y4)) + ((z3 - z4) * (z3 - z4));
                length1 = powf(length1, 0.5);
                length2 = powf(length2, 0.5);
                length3 = powf(length3, 0.5);
                length4 = powf(length4, 0.5);
                length5 = powf(length5, 0.5);
                length6 = powf(length6, 0.5);
                element[firstelement].set_edgelengths(length1, length2, length3, length4, length5, length6);
              }
            }
            size = nnvector[node3].size();
            for (int l = 0; l < size; l++)
            {
              int firstneighbornode = nnvector[node3][l];
              nnvector[node2].push_back(firstneighbornode);
            }
            if (finalnode[node2].numgrains < 2)
            {
              nx = 0;
              ny = 0;
              nz = 0;
              int counter = 0;
              size = nnvector[node2].size();
              for (int l = 0; l < size; l++)
              {
                int firstneighbornode = nnvector[node2][l];
                if (finalnode[firstneighbornode].nodekilled != 1)
                {
                  float x = finalnode[firstneighbornode].xc;
                  float y = finalnode[firstneighbornode].yc;
                  float z = finalnode[firstneighbornode].zc;
                  nx = nx + x;
                  ny = ny + y;
                  nz = nz + z;
                  counter++;
                }
              }
              nx = nx / counter;
              ny = ny / counter;
              nz = nz / counter;
              //              node[node2].set_coords(nx,ny,nz);
              size = elvector[node2].size();
              for (int l = 0; l < size; l++)
              {
                int firstelement = elvector[node2][l];
                int n1 = element[firstelement].node2;
                int n2 = element[firstelement].node3;
                int n3 = element[firstelement].node3;
                int n4 = element[firstelement].node4;
                if (element[firstelement].elementkilled != 1)
                {
                  float x1 = finalnode[n1].xc;
                  float y1 = finalnode[n1].yc;
                  float z1 = finalnode[n1].zc;
                  float x2 = finalnode[n2].xc;
                  float y2 = finalnode[n2].yc;
                  float z2 = finalnode[n2].zc;
                  float x3 = finalnode[n3].xc;
                  float y3 = finalnode[n3].yc;
                  float z3 = finalnode[n3].zc;
                  float x4 = finalnode[n4].xc;
                  float y4 = finalnode[n4].yc;
                  float z4 = finalnode[n4].zc;
                  float a1x = x1 - x3;
                  float a1y = y1 - y3;
                  float a1z = z1 - z3;
                  float b1x = x1 - x2;
                  float b1y = y1 - y2;
                  float b1z = z1 - z2;
                  float n1x = (a1y * b1z) - (a1z * b1y);
                  float n1y = (a1z * b1x) - (a1x * b1z);
                  float n1z = (a1x * b1y) - (a1y * b1x);
                  float a2x = x2 - x3;
                  float a2y = y2 - y3;
                  float a2z = z2 - z3;
                  float b2x = x2 - x4;
                  float b2y = y2 - y4;
                  float b2z = z2 - z4;
                  float n2x = (a2y * b2z) - (a2z * b2y);
                  float n2y = (a2z * b2x) - (a2x * b2z);
                  float n2z = (a2x * b2y) - (a2y * b2x);
                  float a3x = x3 - x1;
                  float a3y = y3 - y1;
                  float a3z = z3 - z1;
                  float b3x = x3 - x4;
                  float b3y = y3 - y4;
                  float b3z = z3 - z4;
                  float n3x = (a3y * b3z) - (a3z * b3y);
                  float n3y = (a3z * b3x) - (a3x * b3z);
                  float n3z = (a3x * b3y) - (a3y * b3x);
                  float a4x = x4 - x1;
                  float a4y = y4 - y1;
                  float a4z = z4 - z1;
                  float b4x = x4 - x2;
                  float b4y = y4 - y2;
                  float b4z = z4 - z2;
                  float n4x = (a4y * b4z) - (a4z * b4y);
                  float n4y = (a4z * b4x) - (a4x * b4z);
                  float n4z = (a4x * b4y) - (a4y * b4x);
                  float denom11 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom12 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  denom11 = powf(denom11, 0.5);
                  denom12 = powf(denom12, 0.5);
                  float dihedral1 = ((n1x * n2x) + (n1y * n2y) + (n1z * n2z)) / (denom11 * denom12);
                  dihedral1 = acos(dihedral1);
                  float denom21 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom22 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  denom21 = powf(denom21, 0.5);
                  denom22 = powf(denom22, 0.5);
                  float dihedral2 = ((n1x * n3x) + (n1y * n3y) + (n1z * n3z)) / (denom21 * denom22);
                  dihedral2 = acos(dihedral2);
                  float denom31 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom32 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom31 = powf(denom31, 0.5);
                  denom32 = powf(denom32, 0.5);
                  float dihedral3 = ((n1x * n4x) + (n1y * n4y) + (n1z * n4z)) / (denom31 * denom32);
                  dihedral3 = acos(dihedral3);
                  float denom41 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  float denom42 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  denom41 = powf(denom41, 0.5);
                  denom42 = powf(denom42, 0.5);
                  float dihedral4 = ((n2x * n3x) + (n2y * n3y) + (n2z * n3z)) / (denom41 * denom42);
                  dihedral4 = acos(dihedral4);
                  float denom51 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  float denom52 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom51 = powf(denom51, 0.5);
                  denom52 = powf(denom52, 0.5);
                  float dihedral5 = ((n2x * n4x) + (n2y * n4y) + (n2z * n4z)) / (denom51 * denom52);
                  dihedral5 = acos(dihedral5);
                  float denom61 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  float denom62 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom61 = powf(denom61, 0.5);
                  denom62 = powf(denom62, 0.5);
                  float dihedral6 = ((n3x * n4x) + (n3y * n4y) + (n3z * n4z)) / (denom61 * denom62);
                  dihedral6 = acos(dihedral6);
                  dihedral1 = (180.0f / 3.1415926535897f) * dihedral1;
                  dihedral2 = (180.0f / 3.1415926535897f) * dihedral2;
                  dihedral3 = (180.0f / 3.1415926535897f) * dihedral3;
                  dihedral4 = (180.0f / 3.1415926535897f) * dihedral4;
                  dihedral5 = (180.0f / 3.1415926535897f) * dihedral5;
                  dihedral6 = (180.0f / 3.1415926535897f) * dihedral6;
                  dihedral1 = 180.0f - dihedral1;
                  dihedral2 = 180.0f - dihedral2;
                  dihedral3 = 180.0f - dihedral3;
                  dihedral4 = 180.0f - dihedral4;
                  dihedral5 = 180.0f - dihedral5;
                  dihedral6 = 180.0f - dihedral6;
                  element[firstelement].set_dihedralangles(dihedral1, dihedral2, dihedral3, dihedral4, dihedral5, dihedral6);
                  float length1 = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
                  float length2 = ((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)) + ((z1 - z3) * (z1 - z3));
                  float length3 = ((x1 - x4) * (x1 - x4)) + ((y1 - y4) * (y1 - y4)) + ((z1 - z4) * (z1 - z4));
                  float length4 = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)) + ((z2 - z3) * (z2 - z3));
                  float length5 = ((x2 - x4) * (x2 - x4)) + ((y2 - y4) * (y2 - y4)) + ((z2 - z4) * (z2 - z4));
                  float length6 = ((x3 - x4) * (x3 - x4)) + ((y3 - y4) * (y3 - y4)) + ((z3 - z4) * (z3 - z4));
                  length1 = powf(length1, 0.5);
                  length2 = powf(length2, 0.5);
                  length3 = powf(length3, 0.5);
                  length4 = powf(length4, 0.5);
                  length5 = powf(length5, 0.5);
                  length6 = powf(length6, 0.5);
                  element[firstelement].set_edgelengths(length1, length2, length3, length4, length5, length6);
                }
              }
            }
          }
          if (edgetokill == 5)
          {
            count++;
            if (finalnode[node4].numgrains > finalnode[node2].numgrains)
            {
              int temp = node2;
              node2 = node4;
              node4 = temp;
            }
            int affected = 0;
            float ecx = (finalnode[node2].xc + finalnode[node4].xc) / 2.0f;
            float ecy = (finalnode[node2].yc + finalnode[node4].yc) / 2.0f;
            float ecz = (finalnode[node2].zc + finalnode[node4].zc) / 2.0f;
            finalnode[node2].set_coords(ecx, ecy, ecz);
            finalnode[node4].set_killed(killed);
            int size = elvector[node4].size();
            ;
            for (int l = 0; l < size; l++)
            {
              affected = 0;
              int firstelement = elvector[node4][l];
              int n1 = element[firstelement].node2;
              int n2 = element[firstelement].node4;
              int n3 = element[firstelement].node3;
              int n4 = element[firstelement].node4;
              if (n1 == node4)
                n1 = node2, affected = 1;
              if (n2 == node4)
                n2 = node2, affected = 1;
              if (n3 == node4)
                n3 = node2, affected = 1;
              if (n4 == node4)
                n4 = node2, affected = 1;
              element[firstelement].set_nodes(n1, n2, n3, n4);
              if (n1 == n2 || n1 == n3 || n1 == n4 || n2 == n3 || n2 == n4 || n3 == n4)
                element[firstelement].set_killed(killed);
              if (element[firstelement].elementkilled != 1)
              {
                if (affected == 1)
                {
                  elvector[node2].push_back(firstelement);
                }
                float x1 = finalnode[n1].xc;
                float y1 = finalnode[n1].yc;
                float z1 = finalnode[n1].zc;
                float x2 = finalnode[n2].xc;
                float y2 = finalnode[n2].yc;
                float z2 = finalnode[n2].zc;
                float x3 = finalnode[n3].xc;
                float y3 = finalnode[n3].yc;
                float z3 = finalnode[n3].zc;
                float x4 = finalnode[n4].xc;
                float y4 = finalnode[n4].yc;
                float z4 = finalnode[n4].zc;
                float a1x = x1 - x3;
                float a1y = y1 - y3;
                float a1z = z1 - z3;
                float b1x = x1 - x2;
                float b1y = y1 - y2;
                float b1z = z1 - z2;
                float n1x = (a1y * b1z) - (a1z * b1y);
                float n1y = (a1z * b1x) - (a1x * b1z);
                float n1z = (a1x * b1y) - (a1y * b1x);
                float a2x = x2 - x3;
                float a2y = y2 - y3;
                float a2z = z2 - z3;
                float b2x = x2 - x4;
                float b2y = y2 - y4;
                float b2z = z2 - z4;
                float n2x = (a2y * b2z) - (a2z * b2y);
                float n2y = (a2z * b2x) - (a2x * b2z);
                float n2z = (a2x * b2y) - (a2y * b2x);
                float a3x = x3 - x1;
                float a3y = y3 - y1;
                float a3z = z3 - z1;
                float b3x = x3 - x4;
                float b3y = y3 - y4;
                float b3z = z3 - z4;
                float n3x = (a3y * b3z) - (a3z * b3y);
                float n3y = (a3z * b3x) - (a3x * b3z);
                float n3z = (a3x * b3y) - (a3y * b3x);
                float a4x = x4 - x1;
                float a4y = y4 - y1;
                float a4z = z4 - z1;
                float b4x = x4 - x2;
                float b4y = y4 - y2;
                float b4z = z4 - z2;
                float n4x = (a4y * b4z) - (a4z * b4y);
                float n4y = (a4z * b4x) - (a4x * b4z);
                float n4z = (a4x * b4y) - (a4y * b4x);
                float denom11 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom12 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                denom11 = powf(denom11, 0.5);
                denom12 = powf(denom12, 0.5);
                float dihedral1 = ((n1x * n2x) + (n1y * n2y) + (n1z * n2z)) / (denom11 * denom12);
                dihedral1 = acos(dihedral1);
                float denom21 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom22 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                denom21 = powf(denom21, 0.5);
                denom22 = powf(denom22, 0.5);
                float dihedral2 = ((n1x * n3x) + (n1y * n3y) + (n1z * n3z)) / (denom21 * denom22);
                dihedral2 = acos(dihedral2);
                float denom31 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom32 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom31 = powf(denom31, 0.5);
                denom32 = powf(denom32, 0.5);
                float dihedral3 = ((n1x * n4x) + (n1y * n4y) + (n1z * n4z)) / (denom31 * denom32);
                dihedral3 = acos(dihedral3);
                float denom41 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                float denom42 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                denom41 = powf(denom41, 0.5);
                denom42 = powf(denom42, 0.5);
                float dihedral4 = ((n2x * n3x) + (n2y * n3y) + (n2z * n3z)) / (denom41 * denom42);
                dihedral4 = acos(dihedral4);
                float denom51 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                float denom52 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom51 = powf(denom51, 0.5);
                denom52 = powf(denom52, 0.5);
                float dihedral5 = ((n2x * n4x) + (n2y * n4y) + (n2z * n4z)) / (denom51 * denom52);
                dihedral5 = acos(dihedral5);
                float denom61 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                float denom62 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom61 = powf(denom61, 0.5);
                denom62 = powf(denom62, 0.5);
                float dihedral6 = ((n3x * n4x) + (n3y * n4y) + (n3z * n4z)) / (denom61 * denom62);
                dihedral6 = acos(dihedral6);
                dihedral1 = (180.0f / 3.1415926535897f) * dihedral1;
                dihedral2 = (180.0f / 3.1415926535897f) * dihedral2;
                dihedral3 = (180.0f / 3.1415926535897f) * dihedral3;
                dihedral4 = (180.0f / 3.1415926535897f) * dihedral4;
                dihedral5 = (180.0f / 3.1415926535897f) * dihedral5;
                dihedral6 = (180.0f / 3.1415926535897f) * dihedral6;
                dihedral1 = 180.0f - dihedral1;
                dihedral2 = 180.0f - dihedral2;
                dihedral3 = 180.0f - dihedral3;
                dihedral4 = 180.0f - dihedral4;
                dihedral5 = 180.0f - dihedral5;
                dihedral6 = 180.0f - dihedral6;
                element[firstelement].set_dihedralangles(dihedral1, dihedral2, dihedral3, dihedral4, dihedral5, dihedral6);
                float length1 = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
                float length2 = ((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)) + ((z1 - z3) * (z1 - z3));
                float length3 = ((x1 - x4) * (x1 - x4)) + ((y1 - y4) * (y1 - y4)) + ((z1 - z4) * (z1 - z4));
                float length4 = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)) + ((z2 - z3) * (z2 - z3));
                float length5 = ((x2 - x4) * (x2 - x4)) + ((y2 - y4) * (y2 - y4)) + ((z2 - z4) * (z2 - z4));
                float length6 = ((x3 - x4) * (x3 - x4)) + ((y3 - y4) * (y3 - y4)) + ((z3 - z4) * (z3 - z4));
                length1 = powf(length1, 0.5);
                length2 = powf(length2, 0.5);
                length3 = powf(length3, 0.5);
                length4 = powf(length4, 0.5);
                length5 = powf(length5, 0.5);
                length6 = powf(length6, 0.5);
                element[firstelement].set_edgelengths(length1, length2, length3, length4, length5, length6);
              }
            }
            size = nnvector[node4].size();
            for (int l = 0; l < size; l++)
            {
              int firstneighbornode = nnvector[node4][l];
              nnvector[node2].push_back(firstneighbornode);
            }
            if (finalnode[node2].numgrains < 2)
            {
              nx = 0;
              ny = 0;
              nz = 0;
              int counter = 0;
              size = nnvector[node2].size();
              for (int l = 0; l < size; l++)
              {
                int firstneighbornode = nnvector[node2][l];
                if (finalnode[firstneighbornode].nodekilled != 1)
                {
                  float x = finalnode[firstneighbornode].xc;
                  float y = finalnode[firstneighbornode].yc;
                  float z = finalnode[firstneighbornode].zc;
                  nx = nx + x;
                  ny = ny + y;
                  nz = nz + z;
                  counter++;
                }
              }
              nx = nx / counter;
              ny = ny / counter;
              nz = nz / counter;
              //              node[node2].set_coords(nx,ny,nz);
              size = elvector[node2].size();
              for (int l = 0; l < size; l++)
              {
                int firstelement = elvector[node2][l];
                int n1 = element[firstelement].node2;
                int n2 = element[firstelement].node4;
                int n3 = element[firstelement].node3;
                int n4 = element[firstelement].node4;
                if (element[firstelement].elementkilled != 1)
                {
                  float x1 = finalnode[n1].xc;
                  float y1 = finalnode[n1].yc;
                  float z1 = finalnode[n1].zc;
                  float x2 = finalnode[n2].xc;
                  float y2 = finalnode[n2].yc;
                  float z2 = finalnode[n2].zc;
                  float x3 = finalnode[n3].xc;
                  float y3 = finalnode[n3].yc;
                  float z3 = finalnode[n3].zc;
                  float x4 = finalnode[n4].xc;
                  float y4 = finalnode[n4].yc;
                  float z4 = finalnode[n4].zc;
                  float a1x = x1 - x3;
                  float a1y = y1 - y3;
                  float a1z = z1 - z3;
                  float b1x = x1 - x2;
                  float b1y = y1 - y2;
                  float b1z = z1 - z2;
                  float n1x = (a1y * b1z) - (a1z * b1y);
                  float n1y = (a1z * b1x) - (a1x * b1z);
                  float n1z = (a1x * b1y) - (a1y * b1x);
                  float a2x = x2 - x3;
                  float a2y = y2 - y3;
                  float a2z = z2 - z3;
                  float b2x = x2 - x4;
                  float b2y = y2 - y4;
                  float b2z = z2 - z4;
                  float n2x = (a2y * b2z) - (a2z * b2y);
                  float n2y = (a2z * b2x) - (a2x * b2z);
                  float n2z = (a2x * b2y) - (a2y * b2x);
                  float a3x = x3 - x1;
                  float a3y = y3 - y1;
                  float a3z = z3 - z1;
                  float b3x = x3 - x4;
                  float b3y = y3 - y4;
                  float b3z = z3 - z4;
                  float n3x = (a3y * b3z) - (a3z * b3y);
                  float n3y = (a3z * b3x) - (a3x * b3z);
                  float n3z = (a3x * b3y) - (a3y * b3x);
                  float a4x = x4 - x1;
                  float a4y = y4 - y1;
                  float a4z = z4 - z1;
                  float b4x = x4 - x2;
                  float b4y = y4 - y2;
                  float b4z = z4 - z2;
                  float n4x = (a4y * b4z) - (a4z * b4y);
                  float n4y = (a4z * b4x) - (a4x * b4z);
                  float n4z = (a4x * b4y) - (a4y * b4x);
                  float denom11 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom12 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  denom11 = powf(denom11, 0.5);
                  denom12 = powf(denom12, 0.5);
                  float dihedral1 = ((n1x * n2x) + (n1y * n2y) + (n1z * n2z)) / (denom11 * denom12);
                  dihedral1 = acos(dihedral1);
                  float denom21 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom22 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  denom21 = powf(denom21, 0.5);
                  denom22 = powf(denom22, 0.5);
                  float dihedral2 = ((n1x * n3x) + (n1y * n3y) + (n1z * n3z)) / (denom21 * denom22);
                  dihedral2 = acos(dihedral2);
                  float denom31 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom32 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom31 = powf(denom31, 0.5);
                  denom32 = powf(denom32, 0.5);
                  float dihedral3 = ((n1x * n4x) + (n1y * n4y) + (n1z * n4z)) / (denom31 * denom32);
                  dihedral3 = acos(dihedral3);
                  float denom41 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  float denom42 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  denom41 = powf(denom41, 0.5);
                  denom42 = powf(denom42, 0.5);
                  float dihedral4 = ((n2x * n3x) + (n2y * n3y) + (n2z * n3z)) / (denom41 * denom42);
                  dihedral4 = acos(dihedral4);
                  float denom51 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  float denom52 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom51 = powf(denom51, 0.5);
                  denom52 = powf(denom52, 0.5);
                  float dihedral5 = ((n2x * n4x) + (n2y * n4y) + (n2z * n4z)) / (denom51 * denom52);
                  dihedral5 = acos(dihedral5);
                  float denom61 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  float denom62 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom61 = powf(denom61, 0.5);
                  denom62 = powf(denom62, 0.5);
                  float dihedral6 = ((n3x * n4x) + (n3y * n4y) + (n3z * n4z)) / (denom61 * denom62);
                  dihedral6 = acos(dihedral6);
                  dihedral1 = (180.0f / 3.1415926535897f) * dihedral1;
                  dihedral2 = (180.0f / 3.1415926535897f) * dihedral2;
                  dihedral3 = (180.0f / 3.1415926535897f) * dihedral3;
                  dihedral4 = (180.0f / 3.1415926535897f) * dihedral4;
                  dihedral5 = (180.0f / 3.1415926535897f) * dihedral5;
                  dihedral6 = (180.0f / 3.1415926535897f) * dihedral6;
                  dihedral1 = 180.0f - dihedral1;
                  dihedral2 = 180.0f - dihedral2;
                  dihedral3 = 180.0f - dihedral3;
                  dihedral4 = 180.0f - dihedral4;
                  dihedral5 = 180.0f - dihedral5;
                  dihedral6 = 180.0f - dihedral6;
                  element[firstelement].set_dihedralangles(dihedral1, dihedral2, dihedral3, dihedral4, dihedral5, dihedral6);
                  float length1 = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
                  float length2 = ((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)) + ((z1 - z3) * (z1 - z3));
                  float length3 = ((x1 - x4) * (x1 - x4)) + ((y1 - y4) * (y1 - y4)) + ((z1 - z4) * (z1 - z4));
                  float length4 = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)) + ((z2 - z3) * (z2 - z3));
                  float length5 = ((x2 - x4) * (x2 - x4)) + ((y2 - y4) * (y2 - y4)) + ((z2 - z4) * (z2 - z4));
                  float length6 = ((x3 - x4) * (x3 - x4)) + ((y3 - y4) * (y3 - y4)) + ((z3 - z4) * (z3 - z4));
                  length1 = powf(length1, 0.5);
                  length2 = powf(length2, 0.5);
                  length3 = powf(length3, 0.5);
                  length4 = powf(length4, 0.5);
                  length5 = powf(length5, 0.5);
                  length6 = powf(length6, 0.5);
                  element[firstelement].set_edgelengths(length1, length2, length3, length4, length5, length6);
                }
              }
            }
          }
          if (edgetokill == 6)
          {
            count++;
            if (finalnode[node4].numgrains > finalnode[node3].numgrains)
            {
              int temp = node3;
              node3 = node4;
              node4 = temp;
            }
            int affected = 0;
            float ecx = (finalnode[node3].xc + finalnode[node4].xc) / 2.0f;
            float ecy = (finalnode[node3].yc + finalnode[node4].yc) / 2.0f;
            float ecz = (finalnode[node3].zc + finalnode[node4].zc) / 2.0f;
            finalnode[node3].set_coords(ecx, ecy, ecz);
            finalnode[node4].set_killed(killed);
            int size = elvector[node4].size();
            ;
            for (int l = 0; l < size; l++)
            {
              affected = 0;
              int firstelement = elvector[node4][l];
              int n1 = element[firstelement].node3;
              int n2 = element[firstelement].node4;
              int n3 = element[firstelement].node3;
              int n4 = element[firstelement].node4;
              if (n1 == node4)
                n1 = node3, affected = 1;
              if (n2 == node4)
                n2 = node3, affected = 1;
              if (n3 == node4)
                n3 = node3, affected = 1;
              if (n4 == node4)
                n4 = node3, affected = 1;
              element[firstelement].set_nodes(n1, n2, n3, n4);
              if (n1 == n2 || n1 == n3 || n1 == n4 || n2 == n3 || n2 == n4 || n3 == n4)
                element[firstelement].set_killed(killed);
              if (element[firstelement].elementkilled != 1)
              {
                if (affected == 1)
                {
                  elvector[node3].push_back(firstelement);
                }
                float x1 = finalnode[n1].xc;
                float y1 = finalnode[n1].yc;
                float z1 = finalnode[n1].zc;
                float x2 = finalnode[n2].xc;
                float y2 = finalnode[n2].yc;
                float z2 = finalnode[n2].zc;
                float x3 = finalnode[n3].xc;
                float y3 = finalnode[n3].yc;
                float z3 = finalnode[n3].zc;
                float x4 = finalnode[n4].xc;
                float y4 = finalnode[n4].yc;
                float z4 = finalnode[n4].zc;
                float a1x = x1 - x3;
                float a1y = y1 - y3;
                float a1z = z1 - z3;
                float b1x = x1 - x2;
                float b1y = y1 - y2;
                float b1z = z1 - z2;
                float n1x = (a1y * b1z) - (a1z * b1y);
                float n1y = (a1z * b1x) - (a1x * b1z);
                float n1z = (a1x * b1y) - (a1y * b1x);
                float a2x = x2 - x3;
                float a2y = y2 - y3;
                float a2z = z2 - z3;
                float b2x = x2 - x4;
                float b2y = y2 - y4;
                float b2z = z2 - z4;
                float n2x = (a2y * b2z) - (a2z * b2y);
                float n2y = (a2z * b2x) - (a2x * b2z);
                float n2z = (a2x * b2y) - (a2y * b2x);
                float a3x = x3 - x1;
                float a3y = y3 - y1;
                float a3z = z3 - z1;
                float b3x = x3 - x4;
                float b3y = y3 - y4;
                float b3z = z3 - z4;
                float n3x = (a3y * b3z) - (a3z * b3y);
                float n3y = (a3z * b3x) - (a3x * b3z);
                float n3z = (a3x * b3y) - (a3y * b3x);
                float a4x = x4 - x1;
                float a4y = y4 - y1;
                float a4z = z4 - z1;
                float b4x = x4 - x2;
                float b4y = y4 - y2;
                float b4z = z4 - z2;
                float n4x = (a4y * b4z) - (a4z * b4y);
                float n4y = (a4z * b4x) - (a4x * b4z);
                float n4z = (a4x * b4y) - (a4y * b4x);
                float denom11 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom12 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                denom11 = powf(denom11, 0.5);
                denom12 = powf(denom12, 0.5);
                float dihedral1 = ((n1x * n2x) + (n1y * n2y) + (n1z * n2z)) / (denom11 * denom12);
                dihedral1 = acos(dihedral1);
                float denom21 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom22 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                denom21 = powf(denom21, 0.5);
                denom22 = powf(denom22, 0.5);
                float dihedral2 = ((n1x * n3x) + (n1y * n3y) + (n1z * n3z)) / (denom21 * denom22);
                dihedral2 = acos(dihedral2);
                float denom31 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                float denom32 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom31 = powf(denom31, 0.5);
                denom32 = powf(denom32, 0.5);
                float dihedral3 = ((n1x * n4x) + (n1y * n4y) + (n1z * n4z)) / (denom31 * denom32);
                dihedral3 = acos(dihedral3);
                float denom41 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                float denom42 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                denom41 = powf(denom41, 0.5);
                denom42 = powf(denom42, 0.5);
                float dihedral4 = ((n2x * n3x) + (n2y * n3y) + (n2z * n3z)) / (denom41 * denom42);
                dihedral4 = acos(dihedral4);
                float denom51 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                float denom52 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom51 = powf(denom51, 0.5);
                denom52 = powf(denom52, 0.5);
                float dihedral5 = ((n2x * n4x) + (n2y * n4y) + (n2z * n4z)) / (denom51 * denom52);
                dihedral5 = acos(dihedral5);
                float denom61 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                float denom62 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                denom61 = powf(denom61, 0.5);
                denom62 = powf(denom62, 0.5);
                float dihedral6 = ((n3x * n4x) + (n3y * n4y) + (n3z * n4z)) / (denom61 * denom62);
                dihedral6 = acos(dihedral6);
                dihedral1 = (180.0f / 3.1415926535897f) * dihedral1;
                dihedral2 = (180.0f / 3.1415926535897f) * dihedral2;
                dihedral3 = (180.0f / 3.1415926535897f) * dihedral3;
                dihedral4 = (180.0f / 3.1415926535897f) * dihedral4;
                dihedral5 = (180.0f / 3.1415926535897f) * dihedral5;
                dihedral6 = (180.0f / 3.1415926535897f) * dihedral6;
                dihedral1 = 180.0f - dihedral1;
                dihedral2 = 180.0f - dihedral2;
                dihedral3 = 180.0f - dihedral3;
                dihedral4 = 180.0f - dihedral4;
                dihedral5 = 180.0f - dihedral5;
                dihedral6 = 180.0f - dihedral6;
                element[firstelement].set_dihedralangles(dihedral1, dihedral2, dihedral3, dihedral4, dihedral5, dihedral6);
                float length1 = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
                float length2 = ((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)) + ((z1 - z3) * (z1 - z3));
                float length3 = ((x1 - x4) * (x1 - x4)) + ((y1 - y4) * (y1 - y4)) + ((z1 - z4) * (z1 - z4));
                float length4 = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)) + ((z2 - z3) * (z2 - z3));
                float length5 = ((x2 - x4) * (x2 - x4)) + ((y2 - y4) * (y2 - y4)) + ((z2 - z4) * (z2 - z4));
                float length6 = ((x3 - x4) * (x3 - x4)) + ((y3 - y4) * (y3 - y4)) + ((z3 - z4) * (z3 - z4));
                length1 = powf(length1, 0.5);
                length2 = powf(length2, 0.5);
                length3 = powf(length3, 0.5);
                length4 = powf(length4, 0.5);
                length5 = powf(length5, 0.5);
                length6 = powf(length6, 0.5);
                element[firstelement].set_edgelengths(length1, length2, length3, length4, length5, length6);
              }
            }
            size = nnvector[node4].size();
            for (int l = 0; l < size; l++)
            {
              int firstneighbornode = nnvector[node4][l];
              nnvector[node3].push_back(firstneighbornode);
            }
            if (finalnode[node3].numgrains < 2)
            {
              nx = 0;
              ny = 0;
              nz = 0;
              int counter = 0;
              size = nnvector[node3].size();
              for (int l = 0; l < size; l++)
              {
                int firstneighbornode = nnvector[node3][l];
                if (finalnode[firstneighbornode].nodekilled != 1)
                {
                  float x = finalnode[firstneighbornode].xc;
                  float y = finalnode[firstneighbornode].yc;
                  float z = finalnode[firstneighbornode].zc;
                  nx = nx + x;
                  ny = ny + y;
                  nz = nz + z;
                  counter++;
                }
              }
              nx = nx / counter;
              ny = ny / counter;
              nz = nz / counter;
              //              node[node3].set_coords(nx,ny,nz);
              size = elvector[node3].size();
              for (int l = 0; l < size; l++)
              {
                int firstelement = elvector[node3][l];
                int n1 = element[firstelement].node3;
                int n2 = element[firstelement].node4;
                int n3 = element[firstelement].node3;
                int n4 = element[firstelement].node4;
                if (element[firstelement].elementkilled != 1)
                {
                  float x1 = finalnode[n1].xc;
                  float y1 = finalnode[n1].yc;
                  float z1 = finalnode[n1].zc;
                  float x2 = finalnode[n2].xc;
                  float y2 = finalnode[n2].yc;
                  float z2 = finalnode[n2].zc;
                  float x3 = finalnode[n3].xc;
                  float y3 = finalnode[n3].yc;
                  float z3 = finalnode[n3].zc;
                  float x4 = finalnode[n4].xc;
                  float y4 = finalnode[n4].yc;
                  float z4 = finalnode[n4].zc;
                  float a1x = x1 - x3;
                  float a1y = y1 - y3;
                  float a1z = z1 - z3;
                  float b1x = x1 - x2;
                  float b1y = y1 - y2;
                  float b1z = z1 - z2;
                  float n1x = (a1y * b1z) - (a1z * b1y);
                  float n1y = (a1z * b1x) - (a1x * b1z);
                  float n1z = (a1x * b1y) - (a1y * b1x);
                  float a2x = x2 - x3;
                  float a2y = y2 - y3;
                  float a2z = z2 - z3;
                  float b2x = x2 - x4;
                  float b2y = y2 - y4;
                  float b2z = z2 - z4;
                  float n2x = (a2y * b2z) - (a2z * b2y);
                  float n2y = (a2z * b2x) - (a2x * b2z);
                  float n2z = (a2x * b2y) - (a2y * b2x);
                  float a3x = x3 - x1;
                  float a3y = y3 - y1;
                  float a3z = z3 - z1;
                  float b3x = x3 - x4;
                  float b3y = y3 - y4;
                  float b3z = z3 - z4;
                  float n3x = (a3y * b3z) - (a3z * b3y);
                  float n3y = (a3z * b3x) - (a3x * b3z);
                  float n3z = (a3x * b3y) - (a3y * b3x);
                  float a4x = x4 - x1;
                  float a4y = y4 - y1;
                  float a4z = z4 - z1;
                  float b4x = x4 - x2;
                  float b4y = y4 - y2;
                  float b4z = z4 - z2;
                  float n4x = (a4y * b4z) - (a4z * b4y);
                  float n4y = (a4z * b4x) - (a4x * b4z);
                  float n4z = (a4x * b4y) - (a4y * b4x);
                  float denom11 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom12 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  denom11 = powf(denom11, 0.5);
                  denom12 = powf(denom12, 0.5);
                  float dihedral1 = ((n1x * n2x) + (n1y * n2y) + (n1z * n2z)) / (denom11 * denom12);
                  dihedral1 = acos(dihedral1);
                  float denom21 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom22 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  denom21 = powf(denom21, 0.5);
                  denom22 = powf(denom22, 0.5);
                  float dihedral2 = ((n1x * n3x) + (n1y * n3y) + (n1z * n3z)) / (denom21 * denom22);
                  dihedral2 = acos(dihedral2);
                  float denom31 = (n1x * n1x) + (n1y * n1y) + (n1z * n1z);
                  float denom32 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom31 = powf(denom31, 0.5);
                  denom32 = powf(denom32, 0.5);
                  float dihedral3 = ((n1x * n4x) + (n1y * n4y) + (n1z * n4z)) / (denom31 * denom32);
                  dihedral3 = acos(dihedral3);
                  float denom41 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  float denom42 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  denom41 = powf(denom41, 0.5);
                  denom42 = powf(denom42, 0.5);
                  float dihedral4 = ((n2x * n3x) + (n2y * n3y) + (n2z * n3z)) / (denom41 * denom42);
                  dihedral4 = acos(dihedral4);
                  float denom51 = (n2x * n2x) + (n2y * n2y) + (n2z * n2z);
                  float denom52 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom51 = powf(denom51, 0.5);
                  denom52 = powf(denom52, 0.5);
                  float dihedral5 = ((n2x * n4x) + (n2y * n4y) + (n2z * n4z)) / (denom51 * denom52);
                  dihedral5 = acos(dihedral5);
                  float denom61 = (n3x * n3x) + (n3y * n3y) + (n3z * n3z);
                  float denom62 = (n4x * n4x) + (n4y * n4y) + (n4z * n4z);
                  denom61 = powf(denom61, 0.5);
                  denom62 = powf(denom62, 0.5);
                  float dihedral6 = ((n3x * n4x) + (n3y * n4y) + (n3z * n4z)) / (denom61 * denom62);
                  dihedral6 = acos(dihedral6);
                  dihedral1 = (180.0f / 3.1415926535897f) * dihedral1;
                  dihedral2 = (180.0f / 3.1415926535897f) * dihedral2;
                  dihedral3 = (180.0f / 3.1415926535897f) * dihedral3;
                  dihedral4 = (180.0f / 3.1415926535897f) * dihedral4;
                  dihedral5 = (180.0f / 3.1415926535897f) * dihedral5;
                  dihedral6 = (180.0f / 3.1415926535897f) * dihedral6;
                  dihedral1 = 180.0f - dihedral1;
                  dihedral2 = 180.0f - dihedral2;
                  dihedral3 = 180.0f - dihedral3;
                  dihedral4 = 180.0f - dihedral4;
                  dihedral5 = 180.0f - dihedral5;
                  dihedral6 = 180.0f - dihedral6;
                  element[firstelement].set_dihedralangles(dihedral1, dihedral2, dihedral3, dihedral4, dihedral5, dihedral6);
                  float length1 = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
                  float length2 = ((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)) + ((z1 - z3) * (z1 - z3));
                  float length3 = ((x1 - x4) * (x1 - x4)) + ((y1 - y4) * (y1 - y4)) + ((z1 - z4) * (z1 - z4));
                  float length4 = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)) + ((z2 - z3) * (z2 - z3));
                  float length5 = ((x2 - x4) * (x2 - x4)) + ((y2 - y4) * (y2 - y4)) + ((z2 - z4) * (z2 - z4));
                  float length6 = ((x3 - x4) * (x3 - x4)) + ((y3 - y4) * (y3 - y4)) + ((z3 - z4) * (z3 - z4));
                  length1 = powf(length1, 0.5);
                  length2 = powf(length2, 0.5);
                  length3 = powf(length3, 0.5);
                  length4 = powf(length4, 0.5);
                  length5 = powf(length5, 0.5);
                  length6 = powf(length6, 0.5);
                  element[firstelement].set_edgelengths(length1, length2, length3, length4, length5, length6);
                }
              }
            }
          }
        }
      }
    }
  }
}
void write_meshdata(string outname1)
{
  ofstream outFile;
  outFile.open(outname1.c_str(), std::ios_base::binary);
  int edgeelement = 0;
  outFile << "# vtk DataFile Version 2.0" << endl;
  outFile << "data set from FFT2dx_GB" << endl;
  outFile << "ASCII" << endl;
  outFile << "DATASET UNSTRUCTURED_GRID" << endl;
  outFile << endl;
  outFile << "POINTS " << numnodes << " float" << endl;
  for (int j = 0; j < numnodes; j++)
  {
    float x = finalnode[j].xc;
    float y = finalnode[j].yc;
    float z = finalnode[j].zc;
    outFile << x << " " << y << " " << z << endl;
  }
  int goodelementcount = 0;
  for (int i = 0; i < elementcount; i++)
  {
    if (element[i].elementkilled != 1)
    {
      goodelementcount++;
    }
  }
  outFile << endl;
  outFile << "CELLS " << goodelementcount + numnodes << " " << (goodelementcount * 5) + (numnodes * 2) << endl;
  for (int j = 0; j < numnodes; j++)
  {
    outFile << "1 " << j << endl;
  }
  for (int i = 0; i < elementcount; i++)
  {
    if (element[i].elementkilled != 1)
    {
      edgeelement = 0;
      int n1 = element[i].node1;
      int n2 = element[i].node2;
      int n3 = element[i].node3;
      int n4 = element[i].node4;
      outFile << "4 " << n1 << " " << n2 << " " << n3 << " " << n4 << endl;
    }
  }
  outFile << endl;
  outFile << "CELL_TYPES " << goodelementcount + numnodes << endl;
  for (int j = 0; j < numnodes; j++)
  {
    outFile << "1" << endl;
  }
  for (int i = 0; i < elementcount; i++)
  {
    if (element[i].elementkilled != 1)
    {
      outFile << "10" << endl;
    }
  }
  outFile << endl;
  outFile << "CELL_DATA " << goodelementcount + numnodes << endl;
  outFile << "SCALARS GrainID int 1" << endl;
  outFile << "LOOKUP_TABLE default" << endl;
  for (int j = 0; j < numnodes; j++)
  {
    int nodetype = finalnode[j].surfnode;
    outFile << nodetype << endl;
    //    if(gvector[j].size() == 1) outFile << gvector[j][0] << endl;
    //    if(gvector[j].size() != 1) outFile << "0" << endl;
  }
  for (int i = 0; i < elementcount; i++)
  {
    if (element[i].elementkilled != 1)
    {
      edgeelement = 0;
      int gname = element[i].grainname;
      outFile << gname << endl;
    }
  }
  outFile << endl;
  outFile << "SCALARS DihedralAngle float" << endl;
  outFile << "LOOKUP_TABLE default" << endl;
  for (int j = 0; j < numnodes; j++)
  {
    outFile << "0" << endl;
  }
  for (int i = 0; i < elementcount; i++)
  {
    if (element[i].elementkilled != 1)
    {
      edgeelement = 0;
//      int n1 = element[i].node1;
//      int n2 = element[i].node2;
//      int n3 = element[i].node3;
//      int n4 = element[i].node4;
//      float x1 = finalnode[n1].xc;
//      float y1 = finalnode[n1].yc;
//      float z1 = finalnode[n1].zc;
//      float x2 = finalnode[n2].xc;
//      float y2 = finalnode[n2].yc;
//      float z2 = finalnode[n2].zc;
//      float x3 = finalnode[n3].xc;
//      float y3 = finalnode[n3].yc;
//      float z3 = finalnode[n3].zc;
//      float x4 = finalnode[n4].xc;
//      float y4 = finalnode[n4].yc;
//      float z4 = finalnode[n4].zc;
//      int grainname = element[i].grainname;
//      int onedge = grain[grainname].edgegrain;
//      float xc = grain[grainname].xcentroid;
//      float yc = grain[grainname].ycentroid;
//      float zc = grain[grainname].zcentroid;
//      int denom = powf(double(numgrains), 0.333) + 1.0;
      float dihedral1 = element[i].dihedralangle1;
      float dihedral2 = element[i].dihedralangle2;
      float dihedral3 = element[i].dihedralangle3;
      float dihedral4 = element[i].dihedralangle4;
      float dihedral5 = element[i].dihedralangle5;
      float dihedral6 = element[i].dihedralangle6;
      float worstangle = dihedral1;
      if ((dihedral2) < worstangle)
        worstangle = (dihedral2);
      if ((dihedral3) < worstangle)
        worstangle = (dihedral3);
      if ((dihedral4) < worstangle)
        worstangle = (dihedral4);
      if ((dihedral5) < worstangle)
        worstangle = (dihedral5);
      if ((dihedral6) < worstangle)
        worstangle = (dihedral6);
      outFile << worstangle << endl;
    }
  }
  outFile.close();
}

void write_dihedralangles(string outname2)
{
  ofstream outFile;
  outFile.open(outname2.c_str(), std::ios_base::binary);
  for (int i = 0; i < 180; i++)
  {
    if (i < 25)
    {
      outFile << i << " " << dihedralanglesurf[i] << "  " << dihedralanglebulk[i] << "  " << volumesurf[i] << " " << volumebulk[i] << endl;
    }
    if (i >= 25)
    {
      outFile << i << " " << dihedralanglesurf[i] << "  " << dihedralanglebulk[i] << endl;
    }
  }
  outFile.close();
}
void write_voxeldata(string outname3)
{
  ofstream outFile;
  outFile.open(outname3.c_str(), std::ios_base::binary);
  outFile << "# vtk DataFile Version 2.0" << endl;
  outFile << "data set from FFT2dx_GB" << endl;
  outFile << "ASCII" << endl;
  outFile << "DATASET STRUCTURED_POINTS" << endl;
  outFile << "DIMENSIONS " << (numxvoxels) << " " << (numyvoxels) << " " << (numzvoxels) << endl;
  outFile << "ORIGIN 0.0 0.0 0.0" << endl;
  outFile << "SPACING " << xres << " " << yres << " " << zres << endl;
  outFile << "POINT_DATA " << (numxvoxels) * (numyvoxels) * (numzvoxels) << endl;
  outFile << endl;
  outFile << "SCALARS GrainID int 1" << endl;
  outFile << "LOOKUP_TABLE default" << endl;
  int counter = 0;
  for (int i = 0; i < numzvoxels; i++)
  {
    for (int j = 0; j < numyvoxels; j++)
    {
      for (int k = 0; k < numxvoxels; k++)
      {
        counter++;
        outFile << "       " << voxels[k][j][i];
        if (counter % 20 == 0)
          outFile << endl;
      }
    }
  }
  outFile.close();
}

