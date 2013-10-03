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
#ifndef MESHGENERATOR_H_
#define MESHGENERATOR_H_

#include <time.h>
#include <string>
#include <vector>

#include <DREAM3D/Common/AIMRandomNG.h>

#ifdef __cplusplus
extern "C"
{
#endif

class nodes
{
  public:
    void set_coords(float x, float y, float z)
    {
      xc = x;
      yc = y;
      zc = z;
    }
    void set_numgrains(int ngrains)
    {
      numgrains = ngrains;
    }
    void set_triplenode(int tnode)
    {
      triplenode = tnode;
    }
    void set_killed(int killed)
    {
      nodekilled = killed;
    }
    void set_newnumber(int newnumber)
    {
      newnodenumber = newnumber;
    }
    void set_surfnode(int snode)
    {
      surfnode = snode;
    }
    void set_finalnumber(int count)
    {
      finalnumber = count;
    }
    void set_onedge(int onedge)
    {
      edgenode = onedge;
    }
    void set_nodemoved(int moved)
    {
      nodemoved = moved;
    }
    float xc;
    float yc;
    float zc;
    int finalnumber;
    int edgenode;
    int nodemoved;
    int triplenode;
    int numgrains;
    int nodekilled;
    int newnodenumber;
    int surfnode;
};


class triangles
{
  public:
    void set_nodes_leftgrain(int node1, int node2, int node3)
    {
      firstnodeleft = node1;
      secondnodeleft = node2;
      thirdnodeleft = node3;
    }
    void set_nodes_rightgrain(int node1, int node3, int node2)
    {
      firstnoderight = node1;
      secondnoderight = node3;
      thirdnoderight = node2;
    }
    void set_avgmisorientation(float avgmisorient)
    {
      averagemisorientation = avgmisorient;
    }
    void set_grains(int grain1, int grain2)
    {
      leftgrain = grain1;
      rightgrain = grain2;
    }
    void set_normal(float nx, float ny, float nz)
    {
      normalx = nx;
      normaly = ny;
      normalz = nz;
    }
    void set_center(float xc, float yc, float zc)
    {
      xcenter = xc;
      ycenter = yc;
      zcenter = zc;
    }
    void set_vol_leftgrain(float leftvol)
    {
      leftvolume = leftvol;
    }
    void set_vol_rightgrain(float rightvol)
    {
      rightvolume = rightvol;
    }
    void set_onedge(int onedge)
    {
      edgetriangle = onedge;
    }
    void set_finalnumber(int count)
    {
      finalnumber = count;
    }
    void set_area(float area)
    {
      trianglearea = area;
    }
    void set_edgelengths(float length1, float length2, float length3)
    {
      edgelength1 = length1;
      edgelength2 = length2;
      edgelength3 = length3;
    }
    void set_killed(int killed)
    {
      trianglekilled = killed;
    }
    void set_averageedgelength(float avgdist)
    {
      averageedgelength = avgdist;
    }
    void set_maxedgelength(float maxdist)
    {
      maxedgelength = maxdist;
    }
    void set_tripletriangle(int ttriangle)
    {
      tripletriangle = ttriangle;
    }
    void set_facetD(float dval)
    {
      dvalue = dval;
    }
    int firstnodeleft;
    int secondnodeleft;
    int thirdnodeleft;
    int firstnoderight;
    int secondnoderight;
    int thirdnoderight;
    float trianglearea;
    float normalx;
    float normaly;
    float normalz;
    int finalnumber;
    int edgetriangle;
    float averageedgelength;
    float maxedgelength;
    int leftgrain;
    int rightgrain;
    float leftvolume;
    float rightvolume;
    float xcenter;
    float ycenter;
    float zcenter;
    float edgelength1;
    float edgelength2;
    float edgelength3;
    int trianglekilled;
    int tripletriangle;
    float dvalue;
    float averagemisorientation;
};


class elements
{
  public:
    void set_nodes(int n1, int n2, int n3, int n4)
    {
      node1 = n1;
      node2 = n2;
      node3 = n3;
      node4 = n4;
    }
    void set_grain(int gnum)
    {
      grainname = gnum;
    }
    void set_dihedralangles(float dihedral1, float dihedral2, float dihedral3, float dihedral4, float dihedral5, float dihedral6)
    {
      dihedralangle1 = dihedral1;
      dihedralangle2 = dihedral2;
      dihedralangle3 = dihedral3;
      dihedralangle4 = dihedral4;
      dihedralangle5 = dihedral5;
      dihedralangle6 = dihedral6;
    }
    void set_edgelengths(float length1, float length2, float length3, float length4, float length5, float length6)
    {
      edgelength1 = length1;
      edgelength2 = length2;
      edgelength3 = length3;
      edgelength4 = length4;
      edgelength5 = length5;
      edgelength6 = length6;
    }
    void set_killed(int killed)
    {
      elementkilled = killed;
    }
    void set_surfnodecount(int surfnodecount)
    {
      originalnodecount = surfnodecount;
    }
    void set_volume(float vol)
    {
      volume = vol;
    }
    void set_onedge(int onedge)
    {
      edgeelement = onedge;
    }
    int node1;
    int node2;
    int node3;
    int node4;
    int grainname;
    float dihedralangle1;
    float dihedralangle2;
    float dihedralangle3;
    float dihedralangle4;
    float dihedralangle5;
    float dihedralangle6;
    float edgelength1;
    float edgelength2;
    float edgelength3;
    float edgelength4;
    float edgelength5;
    float edgelength6;
    int elementkilled;
    int edgeelement;
    int originalnodecount;
    float volume;
};

class grains
{
  public:
    void set_centroid(float xcenter, float ycenter, float zcenter)
    {
      xcentroid = xcenter;
      ycentroid = ycenter;
      zcentroid = zcenter;
    }
    void set_onedge(int onedge)
    {
      edgegrain = onedge;
    }
    void set_bounds(float minx, float maxx, float miny, float maxy, float minz, float maxz)
    {
      xmin = minx;
      xmax = maxx;
      ymin = miny;
      ymax = maxy;
      zmin = minz;
      zmax = maxz;
    }
    void set_mindist(float mindist)
    {
      minimumdistance = mindist;
    }
    void set_maxedgelength(float maxlength)
    {
      maxedgelength = maxlength;
    }
    void set_volume(float totalvol)
    {
      volume = totalvol;
    }
    float xcentroid;
    float ycentroid;
    float zcentroid;
    int edgegrain;
    float xmin;
    float xmax;
    float ymin;
    float ymax;
    float zmin;
    float zmax;
    float minimumdistance;
    float volume;
    float maxedgelength;
};



#ifdef VOLUME_MESH_LIBRARY
int MeshGenerator_Main(const QString& readname1, const QString& readname2,
                       const QString& writename1, const QString& writename1a,
                       const QString& writename2, const QString& writename3,
                       float xDim, float yDim, float zDim,
                       float xRes, float yRes, float zRes,
                       int numGrains);
#endif



void read_nodes(QString inname1);
void read_triangles(QString inname3);
void locate_graincenters();
void delete_triangles();
void clean_triangles();
void create_finalnodesandtriangles();
void find_volumes();
void make_nodes(int iter);
void write_nodes();
void read_elements(int idea);
void measure_elements();
void improve_mesh();
void write_meshdata(QString outname1);
void write_dihedralangles(QString outname2);
void write_voxeldata(QString outname3);


#ifdef __cplusplus
}
#endif



#endif /* MESHGENERATOR_H_ */
