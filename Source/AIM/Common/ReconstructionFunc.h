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
#include <AIM/Common/Orient.h>
#include <AIM/Common/AIMRandomNG.h>
#include <AIM/ANG/AngFileHelper.h>


using namespace std;




class ReconstructionFunc
{

public:

    MXA_SHARED_POINTERS(ReconstructionFunc)
    MXA_STATIC_NEW_MACRO(ReconstructionFunc)


  virtual ~ReconstructionFunc();

    void initialize(double stepX, double stepY, double stepZ,
      int32 angNumCols, int32 angNumRows, int32 angSlices,
      bool v_mergetwinsoption,
      int32 v_minallowedgrainsize,
      double v_minseedconfidence,
      double v_misorientationtolerance,
      int32 v_crystruct,
      bool v_alreadyformed);


  AngFileHelper::Pointer m_angFileHelper;

  double sizex;
  double sizey;
  double sizez;

  double resx;
  double resy;
  double resz;

  double misorientationtolerance;
  double minseedconfidence;
  int minallowedgrainsize;
  int mergetwinsoption;
  int crystruct;
  int alreadyformed;


	AIMRandomNG rg;
	Voxel* voxels;
	Grain* tempgrain;
	Grain* grains;
	Grain* goodgrain;
	Grain* packedgrain;
	int* gsizes;
	int* gremovals;
	Bin* seNbin;

	Bin* eulerbin;
	Orient* orient;
	Voxel* gridfine;
	Voxel* gridcourse;
	Bin* actualmisobin;
	Bin* simmisobin;
	Bin* actualmicrobin;
	Bin* simmicrobin;
	double nsdist[20][20];

	vector<vector<int> > voxelsvector;
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
	int maxdiameter;
	int mindiameter;
	double overlapallowed;
	int overlapassignment;


	int32 xpoints;
	int32 ypoints;
	int32 zpoints;
  int totalpoints;

	double resx1;
	double resy1;
	double resz1;
	int numneighbins;
	int nummicros;

	/** @brief The number of boundary center objects needed */
	size_t bcentSize;

// sizes are needed. Dont't change
	double eulerrank[180][180][180];

// sizes are needed. Dont't change
  int eulercount[100][100][100];

	void loadSlices();
	int form_grains();
	void remove_smallgrains();
	int renumber_grains1();
	void write_volume(string);
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
	void volume_stats(string,string,string);
	void create_visualization(string);
	void write_grains(string);
	void find_boundarycenters(string);
	void write_axisorientations(string);
	void write_eulerangles(string);
	double getmisoquat(double ,double,double ,double ,double ,double ,double,double ,double &,double &,double &);
	double gamma(double);

protected:
  ReconstructionFunc();

private:
	ReconstructionFunc(const ReconstructionFunc&);    // Copy Constructor Not Implemented
    void operator=(const ReconstructionFunc&);  // Operator '=' Not Implemented
};


#endif
