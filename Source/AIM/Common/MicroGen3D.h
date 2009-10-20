#ifndef MicroGen3D_H
#define MicroGen3D_H

#include <vector>
#include <assert.h>
#include <stdio.h>
#include "stdafx.h"
#include <string>
#include <iostream>
#include <cmath>
#include <fstream>
#include <list>
#include <time.h>
#include <stdlib.h>
#include <algorithm>
#include <numeric>

using namespace std;

typedef   signed long int32;     
typedef unsigned long uint32;     

class TRandomMotherOfAll
{
public:
  void RandomInit(uint32);
  double Random();
  double x[5];                         
};

class voxels
{
public:
	int grainname;
	double confidence;
	int alreadychecked;
	double xc;
	double yc;
	double zc;
	double euler1;
	double euler2;
	double euler3;
	int hasneighbor;
	int neighbor;
	double misorientation;
	int surfacevoxel;
	int neighbor1;
	int neighbor2;
	int neighbor3;
	int neighbor4;
	int neighbor5;
	int neighbor6;
	int available;
	int available90;
	int inside;
	vector<int> voxneighlist;
};

class grains
{
public:
	int numvoxels;
	int numneighbors;
	int newgrainname;
	vector<int> neighborlist;
	int gotsizemerged;
	int gotcontainedmerged;
	int gottwinmerged;
	double avgeuler1;
	double avgeuler2;
	double avgeuler3;
	double centroidx;
	double centroidy;
	double centroidz;
	int surfacegrain;
	double Ixx;
	double Iyy;
	double Izz;
	double Ixy;
	double Iyz;
	double Ixz;
	double axis1;
	double axis2;
	double axis3;
	double axis1x;
	double axis1y;
	double axis1z;
	double axis2x;
	double axis2y;
	double axis2z;
	double axis3x;
	double axis3y;
	double axis3z;
	vector<double> misorientationlist;
	double averagemisorientation;
	int twinnewnumberbeenset;
	int twinnewnumber;
	double red;
	double green;
	double blue;
	double schmidfactor;
	double convexity;
	double euler1;
	double euler2;
	double euler3;
	double oeuler1;
	double oeuler2;
	double oeuler3;
	int grainname;
	int neighnum;
	double volume;
	double Nvalue;
	double nserror;
	double x;
	double y;
	double z;
	double xc;
	double yc;
	double zc;
	int currentsize;
	int initsize;
	int temponsurf;
	int tempneighnum;
	double radius1;
	double radius2;
	double radius3;
	double lowanglefraction;
	double grainrank;
	double grainmicrorank;
	double picked;
	double frozen;
};

class bins
{
public:
	double euler1;
	double euler2;
	double euler3;
	double dprobability;
	double sprobability;
	double Nprobability;
	double Nvalue;
	double bctotprobability;
	double diameter;
	double rad;
	double bctotal;
	double difference;
	double height;
	double binrank;
};

class orients
{
public:
	double rad1x;
	double rad1y;
	double rad1z;
	double rad2x;
	double rad2y;
	double rad2z;
	double rad3x;
	double rad3y;
	double rad3z;
};


class MicroGen3D
{

public:
  MicroGen3D();
  virtual ~MicroGen3D();

	TRandomMotherOfAll rg;
  Voxel* voxels;

	voxels voxel[1000];
	grains tempgrain[1000];
	grains grain[1000];
	grains goodgrain[1000];
	grains packedgrain[1000];
	int gsizes[1000];
	int gremovals[1000];
	bins diambin[1000];
	bins boverabin[1000];
	bins coverabin[1000];
	bins coverbbin[1000];
	bins seNbin[1000];
	int eulercount[100][100][100];
	bins eulerbin[1000];
	orients orient[1000];
	voxels gridfine[1000];
	voxels gridcourse[1000];
	bins actualmisobin[1000];
	bins simmisobin[1000];
	bins actualmicrobin[1000];
	bins simmicrobin[1000];
	double svn[1000][20];
	double svs[20][20];
	double nsdist[20][20];
	vector<vector<int> > voxelvector;
	vector<vector<int> > neighborvector;

	int numdiambins;
	int numshapebins;
	int numseNbins;
	int numsizebins;
	int numorients;
	int numeulers;
	int nummisobins;
	int nummicrobins;
	int misoiter;
	int nummisomoves;
	int nummicromoves;
	int numgrains;
	int shapeclass;
	double overlapallowed;
	int overlapassignment;
	long double sizex;
	long double sizey;
	long double sizez;
	long xpoints;
	long ypoints;
	long zpoints;
	long double resx;
	long double resy;
	long double resz;
	long double resx1;
	long double resy1;
	long double resz1;
	int numneighbins;
	int nummicros;
	int crystruct;

	double misorientationtolerance;
	double minseedconfidence;
	int minallowedgrainsize;
	int mergetwinsoption;
	int alreadyformed;
	int totalpoints;
	double bcent[250000][5];
	double eulerrank[180][180][180];


	void loadSlices();
	int form_grains();
	void remove_smallgrains();
	int renumber_grains1();
	void write_volume1(string);
	void load_data(string);
	void assign_badpoints();
	void find_neighbors();
	void merge_containedgrains();
	int renumber_grains2();
	void homogenize_grains();
	void merge_twins();
	void characterize_twins();
	int renumber_grains3();
	void find_goodneighbors();
	void find_centroids ();
	void find_moments();
	void find_axes();
	void find_vectors();
	void measure_misorientations();
	void find_colors();
	void find_convexities();
	void volume_stats(string, string, string, string, string, string, string, string, string);
	void write_volume2(string);
	void create_visualization(string);
	void write_grains(string);
	void find_boundarycenters(string);
	void write_axisorientations(string);
	void write_eulerangles(string);
	void loadVolData(string, int);
	void loadboveraData(string, int);
	void loadcoveraData(string, int);
	void loadcoverbData(string, int);
	void loadNData(string, int);
	void loadorientData(string, int);
	void loadeulerData(string, int);
	void generate_grains(int);
	void assign_eulers(int);
	void loadSVNData(string);
	void loadSVSData(string);
	void make_points(int);
	void fill_gaps(int);
	void writeCube(string, int);
	void write_grains(string, int);
	void find_boundarycenters1(string,int);
	void loadMisoData(string);
	void loadMicroData(string);
	void measure_misorientations1(int);
	void rank_misobins(int);
	void count_misorientations(int);
	int rank_microbins(int);
	void rank_grains1(int);
	void identify_grains1(int,int);
	void move_grains1(int);
	void rank_grains2(int);
	void identify_grains2(int,int);
	void move_grains2(int);
	void freeze_grains(int);
	void write_volume(string);
	double getmisoquat(double ,double,double ,double ,double ,double ,double,double ,double &,double &,double &);
	double gamma(double);

};


#endif