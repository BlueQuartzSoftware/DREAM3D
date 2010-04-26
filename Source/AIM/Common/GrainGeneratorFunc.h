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
#include <AIM/Common/AIMRandomNG.h>
#include <AIM/ANG/AngFileHelper.h>


using namespace std;




class GrainGeneratorFunc
{

public:

    MXA_SHARED_POINTERS(GrainGeneratorFunc)
    MXA_STATIC_NEW_MACRO(GrainGeneratorFunc)


  virtual ~GrainGeneratorFunc();

void initialize(int32, int32 , double , double , double ,
                  int32 ,int32 ,int32, int32, double);

void initialize2();

AngFileHelper::Pointer m_angFileHelper;


  double resx;
  double resy;
  double resz;

  double misorientationtolerance;
  int crystruct;


	AIMRandomNG rg;
	Grain* grains;
	Grain* precipitates;
	int* gsizes;
	int* psizes;
	int* gremovals;

	Bin* actualodf;
	Bin* simodf;
	Bin* axisodf;
	Bin* precipaxisodf;
	Voxel* gridfine;
	Voxel* gridcourse;
	Bin* actualmdf;
	Bin* simmdf;
	Bin* actualmicrotex;
	Bin* simmicrotex;

	vector<int> grainorder;
	vector<int> precipitateorder;
	vector<int> takencheck;
	vector<vector<int> > neighborvector;
	vector<vector<double> > bovera;
	vector<vector<double> > covera;
	vector<vector<double> > coverb;
	vector<vector<double> > precipbovera;
	vector<vector<double> > precipcovera;
	vector<vector<double> > precipcoverb;
	vector<vector<double> > svn;
	vector<vector<double> > svs;
	vector<vector<double> > svomega3;
	vector<vector<double> > precipsvomega3;
	vector<vector<int> > nsdist;
	

	int numorients;
	int numeulers;
	int resdiff;
	double totalsurfacearea;
	int numgrains;
	int numprecipitates;
	int shapeclass;
	int preciptype;
	int maxdiameter;
	int mindiameter;
	int maxprecipdiameter;
	int minprecipdiameter;
	int32 overlapallowed;
	int overlapassignment;
	double fractionprecip;
	double avgdiam;
	double sddiam;
	double avgprecipdiam;
	double sdprecipdiam;
	int numdiameters;
	int numprecipdiameters;


	int32 xpoints;
	int32 ypoints;
	int32 zpoints;
	int32 xpoints1;
	int32 ypoints1;
	int32 zpoints1;
	int totalpoints;
	int totalpoints1;
	double totalvol;

	double resx1;
	double resy1;
	double resz1;
	int numneighbins;

	void write_eulerangles(string);
	void loadStatsData(string);
	void loadorientData(string);
	void loadeulerData(string);
	void generate_grains(int);
	void assign_eulers(int);
	void pack_grains(int);
	void fill_gaps(int);
	int create_precipitates();
	void insert_precipitates(int);
	void read_structure(string);
	void find_neighbors();
	void writeCube(string, int);
	void loadMisoData(string);
	void loadMicroData(string);
	void matchCrystallography(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	void measure_misorientations(double quat_symmcubic[24][5],double quat_symmhex[12][5]);
	double getmisoquatcubic(double ,double ,double ,double ,double,double ,double &,double &,double &);
	double getmisoquathexagonal(double quat_symmhex[12][5], double ,double ,double ,double ,double,double ,double &,double &,double &);
	double gamma(double);
	double find_xcoord(long);
	double find_ycoord(long);
	double find_zcoord(long);

protected:
  GrainGeneratorFunc();

private:
	GrainGeneratorFunc(const GrainGeneratorFunc&);    // Copy Constructor Not Implemented
    void operator=(const GrainGeneratorFunc&);  // Operator '=' Not Implemented
};


#endif
