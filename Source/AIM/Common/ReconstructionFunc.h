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
#ifndef _ReconstructionFunc_H
#define _ReconstructionFunc_H

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <cstddef>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <fstream>
#include <list>
#include <algorithm>
#include <numeric>

#include <MXA/MXATypes.h>
#include <MXA/Common/MXASetGetMacros.h>

#include <AIM/Common/Grain.h>
#include <AIM/Common/Voxel.h>
#include <AIM/Common/Bin.h>
#include <AIM/Common/AIMRandomNG.h>
#include <AIM/ANG/AngFileHelper.h>
#include <AIM/ANG/AngDirectoryPatterns.h>


using namespace std;




class ReconstructionFunc
{

public:

    MXA_SHARED_POINTERS(ReconstructionFunc)
    MXA_STATIC_NEW_MACRO(ReconstructionFunc)
    MXA_INSTANCE_PROPERTY(AngDirectoryPatterns::Pointer, DirectoryPattern);


  virtual ~ReconstructionFunc();

  AngFileHelper::Pointer m_angFileHelper;

  double sizex;
  double sizey;
  double sizez;

  double resx;
  double resy;
  double resz;

  double misorientationtolerance;
  double minseedconfidence;
  double minseedimagequality;
  double downsamplefactor;
  int minallowedgrainsize;
  int mergetwinsoption;
  int mergecoloniesoption;
  int crystruct;
  int alignmeth;
  int alreadyformed;


	AIMRandomNG rg;
	Voxel* voxels;
	Voxel* voxelstemp;
	vector<Grain> grains;
	Bin *eulerodf;
	Bin *axisodf;

	int **shifts;
	int **arr;
	int *graincounts;
	double **graincenters;
	double **grainmoments;
	double **quat_symm;

	vector<vector<int> > neighborhood;
	vector<vector<double> > svbovera;
	vector<vector<double> > svcovera;
	vector<vector<double> > svcoverb;
	vector<vector<double> > svschmid;
	vector<vector<double> > svomega3;

	int numseNbins;
	int numorients;
	int numeulers;
	int numgrains;
	double totalsurfacearea;
	int maxdiameter;
	int mindiameter;
	int cutoutxsize;
	int cutoutysize;
	int cmaxx;
	int cminx;
	int cmaxy;
	int cminy;
	int xpoints;
	int ypoints;
	int tempxpoints;
	int tempypoints;
	int zpoints;
	int totalpoints;
	int totaltemppoints;
	int numneighbins;
	double totalvol;
	double totalaxes;

    void initialize(int, int, double, bool, bool, int, double, double, double, double, int, int, bool);
	void loadSlices();
	void align_sections(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void find_border();
	int form_grains(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void form_grains_sections(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void remove_smallgrains();
	int renumber_grains1();
	void write_volume(string, string, string, string, string, string, bool, bool, bool, bool,double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	int load_data(string);
	void assign_badpoints(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void find_neighbors();
	void merge_containedgrains();
	int renumber_grains();
	int define_subgrains(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	int reburn_grains(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void cleanup_data();
	void find_kernels(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void homogenize_grains(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void merge_twins(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void merge_colonies(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void characterize_twins();
	void characterize_colonies();
	int renumber_grains3();
	void find_euclidean_map ();
	void find_centroids ();
	void find_moments();
	void find_axes();
	void find_vectors();
	void find_eulerodf();
	void measure_misorientations(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void find_colors(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void find_convexities();
	void find_schmids();
	void volume_stats(string,string,string);
	void write_axisodf(string);
	void write_eulerodf(string);
	void write_graindata(string);
	void write_grains(/* double quat_symmcubic[24][5],double quat_symmhex[12][5] */);
	double getmisoquatcubic(double,double q1[5],double q2[5],double &,double &,double &);
	double getmisoquathexagonal(double quat_symmhex[12][5],double,double q1[5],double q2[5],double &,double &,double &);
	double gamma(double);
	double find_xcoord(long);
	double find_ycoord(long);
	double find_zcoord(long);

protected:
  ReconstructionFunc();

private:
	ReconstructionFunc(const ReconstructionFunc&);    // Copy Constructor Not Implemented
    void operator=(const ReconstructionFunc&);  // Operator '=' Not Implemented
};


#endif
