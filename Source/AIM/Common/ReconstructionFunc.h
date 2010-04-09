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

#include <MXA/Common/MXATypes.h>
#include <MXA/Common/MXASetGetMacros.h>

#include <AIM/Common/Grain.h>
#include <AIM/Common/Voxel.h>
#include <AIM/Common/Bin.h>
#include <AIM/Common/AIMRandomNG.h>
#include <AIM/ANG/AngFileHelper.h>


using namespace std;




class ReconstructionFunc
{

public:

    MXA_SHARED_POINTERS(ReconstructionFunc)
    MXA_STATIC_NEW_MACRO(ReconstructionFunc)


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
  int minallowedgrainsize;
  int mergetwinsoption;
  int mergecoloniesoption;
  int crystruct;
  int alreadyformed;


	AIMRandomNG rg;
	Voxel* voxels;
	Voxel* tempvoxels;
	Grain* grains;
	Bin *eulerodf;
	Bin *axisodf;

	int **shifts;
	int **arr;
	double **graincenters;
	double **grainmoments;
	double **quat_symm;

	vector<vector<int> > neighborvector;

	int numseNbins;
	int numorients;
	int numeulers;
	int nummisobins;
	int nummicrobins;
	int numgrains;
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
	int nummicros;
	double totalvol;
	double totalaxes;

    void initialize(double stepX, double stepY, double stepZ,
      int andNumCols, int angNumRows, int angSlices,
      bool, bool, int, double, double, double, int, bool);
	void find_cutout(string angFName, int andNumCols, int angNumRows, double, double);
	void loadSlices(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void align_sections(int slice,double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	int form_grains(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void remove_smallgrains();
	int renumber_grains1();
	void write_volume(string);
	int load_data(string);
	void assign_badpoints();
	void find_neighbors();
	void merge_containedgrains();
	int renumber_grains2();
	void homogenize_grains(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void merge_twins(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void merge_colonies(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void characterize_twins();
	void characterize_colonies();
	int renumber_grains3();
	void find_goodneighbors();
	void find_centroids ();
	void find_moments();
	void find_axes();
	void find_vectors();
	void find_eulerodf();
	void find_axisodf();
	void measure_misorientations(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void find_colors(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void find_convexities();
	void volume_stats(string,string,string);
	void write_axisodf(string);
	void write_eulerodf(string);
	void write_graindata(string);
	double getmisoquatcubic(double,double ,double ,double ,double ,double,double ,double &,double &,double &);
	double getmisoquathexagonal(double quat_symmhex[12][5],double,double ,double ,double ,double ,double,double ,double &,double &,double &);
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
