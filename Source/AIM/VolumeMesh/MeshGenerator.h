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
#ifndef MESHGENERATOR_H_
#define MESHGENERATOR_H_

#include <time.h>
#include <string>
#include <vector>

#include <AIM/Common/AIMRandomNG.h>

#ifdef __cplusplus
extern "C"
{
#endif

  class nodes
  {
   public:
    void set_coords(double x, double y, double z)
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
    double xc;
    double yc;
    double zc;
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
    void set_avgmisorientation(double avgmisorient)
    {
      averagemisorientation = avgmisorient;
    }
    void set_grains(int grain1, int grain2)
    {
      leftgrain = grain1;
      rightgrain = grain2;
    }
    void set_normal(double nx, double ny, double nz)
    {
      normalx = nx;
      normaly = ny;
      normalz = nz;
    }
    void set_center(double xc, double yc, double zc)
    {
      xcenter = xc;
      ycenter = yc;
      zcenter = zc;
    }
    void set_vol_leftgrain(double leftvol)
    {
      leftvolume = leftvol;
    }
    void set_vol_rightgrain(double rightvol)
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
    void set_area(double area)
    {
      trianglearea = area;
    }
    void set_edgelengths(double length1, double length2, double length3)
    {
      edgelength1 = length1;
      edgelength2 = length2;
      edgelength3 = length3;
    }
    void set_killed(int killed)
    {
      trianglekilled = killed;
    }
    void set_averageedgelength(double avgdist)
    {
      averageedgelength = avgdist;
    }
    void set_maxedgelength(double maxdist)
    {
      maxedgelength = maxdist;
    }
    void set_tripletriangle(int ttriangle)
    {
      tripletriangle = ttriangle;
    }
    void set_facetD(double dval)
    {
      dvalue = dval;
    }
    int firstnodeleft;
    int secondnodeleft;
    int thirdnodeleft;
    int firstnoderight;
    int secondnoderight;
    int thirdnoderight;
    double trianglearea;
    double normalx;
    double normaly;
    double normalz;
    int finalnumber;
    int edgetriangle;
    double averageedgelength;
    double maxedgelength;
    int leftgrain;
    int rightgrain;
    double leftvolume;
    double rightvolume;
    double xcenter;
    double ycenter;
    double zcenter;
    double edgelength1;
    double edgelength2;
    double edgelength3;
    int trianglekilled;
    int tripletriangle;
    double dvalue;
    double averagemisorientation;
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
    void set_dihedralangles(double dihedral1, double dihedral2, double dihedral3, double dihedral4, double dihedral5, double dihedral6)
    {
      dihedralangle1 = dihedral1;
      dihedralangle2 = dihedral2;
      dihedralangle3 = dihedral3;
      dihedralangle4 = dihedral4;
      dihedralangle5 = dihedral5;
      dihedralangle6 = dihedral6;
    }
    void set_edgelengths(double length1, double length2, double length3, double length4, double length5, double length6)
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
    void set_volume(double vol)
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
    double dihedralangle1;
    double dihedralangle2;
    double dihedralangle3;
    double dihedralangle4;
    double dihedralangle5;
    double dihedralangle6;
    double edgelength1;
    double edgelength2;
    double edgelength3;
    double edgelength4;
    double edgelength5;
    double edgelength6;
    int elementkilled;
    int edgeelement;
    int originalnodecount;
    double volume;
  };

  class grains
  {
   public:
    void set_centroid(double xcenter, double ycenter, double zcenter)
    {
      xcentroid = xcenter;
      ycentroid = ycenter;
      zcentroid = zcenter;
    }
    void set_onedge(int onedge)
    {
      edgegrain = onedge;
    }
    void set_bounds(double minx, double maxx, double miny, double maxy, double minz, double maxz)
    {
      xmin = minx;
      xmax = maxx;
      ymin = miny;
      ymax = maxy;
      zmin = minz;
      zmax = maxz;
    }
    void set_mindist(double mindist)
    {
      minimumdistance = mindist;
    }
    void set_maxedgelength(double maxlength)
    {
      maxedgelength = maxlength;
    }
    void set_volume(double totalvol)
    {
      volume = totalvol;
    }
    double xcentroid;
    double ycentroid;
    double zcentroid;
    int edgegrain;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;
    double minimumdistance;
    double volume;
    double maxedgelength;
  };



#ifdef VOLUME_MESH_LIBRARY
int MeshGenerator_Main(const std::string &readname1, const std::string &readname2,
                    const std::string &writename1, const std::string &writename1a,
                    const std::string &writename2, const std::string &writename3,
                    double xDim, double yDim, double zDim,
                    double xRes, double yRes, double zRes,
                    int numGrains);
#endif



  void read_nodes(std::string inname1);
  void read_triangles(std::string inname3);
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
  void write_meshdata(std::string outname1);
  void write_dihedralangles(std::string outname2);
  void write_voxeldata(std::string outname3);


#ifdef __cplusplus
}
#endif



#endif /* MESHGENERATOR_H_ */
