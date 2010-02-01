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

    void initialize(double , double stepY, double stepZ,
      int32, int32 angNumRows, int32 angSlices,
      bool, int32, double, double, int32, bool);


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
	Grain* grains;
	Orient *eulerodf;
	Orient *axisodf;

	int **shifts;

	vector<vector<int> > neighborvector;

	int numseNbins;
	int numorients;
	int numeulers;
	int nummisobins;
	int nummicrobins;
	int numgrains;
	int maxdiameter;
	int mindiameter;
	int32 xpoints;
	int32 ypoints;
	int32 zpoints;
	int totalpoints;
	int numneighbins;
	int nummicros;
	double totalvol;

	void loadSlices();
	void align_sections(string);
	int form_grains();
	void remove_smallgrains();
	int renumber_grains1();
	void write_volume(string);
	int load_data(string);
	void assign_badpoints();
	void find_neighbors();
	void merge_containedgrains();
	int renumber_grains2();
	void homogenize_grains(double quat_symm[24][5]);
	void merge_twins();
	void characterize_twins();
	int renumber_grains3();
	void find_goodneighbors();
	void find_centroids ();
	void find_moments();
	void find_axes();
	void find_vectors();
	void find_eulerodf();
	void find_axisodf();
	void measure_misorientations();
	void find_colors();
	void find_convexities();
	void volume_stats(string,string,string);
	void create_dxfile(string);
	void write_axisodf(string);
	void write_eulerodf(string);
	void write_graindata(string);
	double getmisoquat(double ,double,double ,double ,double ,double ,double,double ,double &,double &,double &);
	double gamma(double);

protected:
  ReconstructionFunc();

private:
	ReconstructionFunc(const ReconstructionFunc&);    // Copy Constructor Not Implemented
    void operator=(const ReconstructionFunc&);  // Operator '=' Not Implemented
};


#endif
