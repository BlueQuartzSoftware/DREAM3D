#ifndef _MicroGen3D_H
#define _MicroGen3D_

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




class MicroGen3D
{

public:

    MXA_SHARED_POINTERS(MicroGen3D)
    MXA_STATIC_NEW_MACRO(MicroGen3D)


  virtual ~MicroGen3D();

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
	Bin* diambin;
	Bin* boverabin;
	Bin* coverabin;
	Bin* coverbbin;
	Bin* seNbin;
	
	Bin* eulerbin;
	Orient* orient;
	Voxel* gridfine;
	Voxel* gridcourse;
	Bin* actualmisobin;
	Bin* simmisobin;
	Bin* actualmicrobin;
	Bin* simmicrobin;
	double svn[1000][20];
	double svs[20][20];
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

protected:
  MicroGen3D();

private:
	MicroGen3D(const MicroGen3D&);    // Copy Constructor Not Implemented
    void operator=(const MicroGen3D&);  // Operator '=' Not Implemented
};


#endif
