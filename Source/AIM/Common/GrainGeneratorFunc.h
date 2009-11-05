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
#ifndef _GrainGeneratorFunc_H
#define _GrainGeneratorFunc_H

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




class GrainGeneratorFunc
{

public:

    MXA_SHARED_POINTERS(GrainGeneratorFunc)
    MXA_STATIC_NEW_MACRO(GrainGeneratorFunc)


  virtual ~GrainGeneratorFunc();

void initialize(int32 v_NumG, int32 v_ShapeClass, double stepX, double stepY, double stepZ,
                  double v_OverAllow,int32 v_OverMethod,int32 v_crystruct);

void initialize2(int32 xpoints, int32 ypoints, int32 zpoints);

  AngFileHelper::Pointer m_angFileHelper;


  double resx;
  double resy;
  double resz;

  double misorientationtolerance;
  int crystruct;


	AIMRandomNG rg;
	Grain* grains;
	Grain* packedgrain;
	int* gsizes;
	int* gremovals;

	Bin* eulerbin;
	Orient* orient;
	Voxel* gridfine;
	Voxel* gridcourse;
	Bin* actualmisobin;
	Bin* simmisobin;
	Bin* actualmicrobin;
	Bin* simmicrobin;

	vector<int> grainorder;
	vector<vector<int> > voxelsvector;
	vector<vector<int> > neighborvector;
	vector<vector<double> > bovera;
	vector<vector<double> > covera;
	vector<vector<double> > coverb;
	vector<vector<double> > svn;
	vector<vector<double> > svs;
	vector<vector<double> > svshape;
	vector<vector<int> > nsdist;
	

	int numorients;
	int numeulers;
	int nummisobins;
	int nummicrobins;
	int misoiter;
	int nummicros;
	int nummisomoves;
	int nummicromoves;
	int numgrains;
	int shapeclass;
	int maxdiameter;
	int mindiameter;
	double overlapallowed;
	int overlapassignment;
	double avgdiam;
	double sddiam;
	int numdiameters;


	int32 xpoints;
	int32 ypoints;
	int32 zpoints;
	int32 xpoints1;
	int32 ypoints1;
	int32 zpoints1;
  int totalpoints;
  int totalpoints1;

	double resx1;
	double resy1;
	double resz1;
	int numneighbins;

	/** @brief The number of boundary center objects needed */
	size_t bcentSize;

// sizes are needed. Dont't change
  int eulercount[36][36][36];

	void write_volume(string);
	void loadStatsData(string);
	void loadorientData(string, int);
	void loadeulerData(string, int);
	void generate_grains(int);
	void assign_eulers(int);
	void make_points(int);
	void fill_gaps(int);
	void find_neighbors();
	void writeCube(string, int);
	void write_grains(string, int);
	void find_boundarycenters1(string,int);
	void loadMisoData(string);
	void loadMicroData(string);
	void measure_misorientations();
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
	double getmisoquat(double ,double,double ,double ,double ,double ,double,double ,double &,double &,double &);
	double gamma(double);

protected:
  GrainGeneratorFunc();

private:
	GrainGeneratorFunc(const GrainGeneratorFunc&);    // Copy Constructor Not Implemented
    void operator=(const GrainGeneratorFunc&);  // Operator '=' Not Implemented
};


#endif
