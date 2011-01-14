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

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"

#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/Constants.h"
#include "AIM/Common/Grain.h"
#include "AIM/Common/Voxel.h"
#include "AIM/Common/AIMRandomNG.h"
#include "AIM/ANG/AbstractAngDataLoader.h"
#include "AIM/Common/HDF5/H5ReconStatsWriter.h"


using namespace std;




class AIMCOMMON_EXPORT ReconstructionFunc
{

public:

    MXA_SHARED_POINTERS(ReconstructionFunc)
    MXA_STATIC_NEW_MACRO(ReconstructionFunc)
//    MXA_INSTANCE_PROPERTY(AngDirectoryPatterns::Pointer, DirectoryPattern);


  virtual ~ReconstructionFunc();

  // AngFileHelper::Pointer m_angFileHelper;

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
  AIM::Reconstruction::CrystalStructure crystruct;
  int alignmeth;
  int alreadyformed;


	AIMRandomNG rg;
	Voxel* voxels;
	Voxel* voxelstemp;
	vector<Grain> m_Grains;

	int **shifts;
	int **arr;
	int *graincounts;
	double **graincenters;
	double **grainmoments;
	double **quat_symm;

	vector<vector<double> > neighborhood;
	vector<vector<double> > neighborhoodfit;
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


  void initialize(int nX, int nY, int nZ,
                  double xRes, double yRes, double zRes,
                  bool v_mergetwinsoption,
                  bool v_mergecoloniesoption, int v_minallowedgrainsize,
                  double v_minseedconfidence, double v_downsamplefactor,
                  double v_minseedimagequality, double v_misorientationtolerance,
                  AIM::Reconstruction::CrystalStructure v_crystruct,
                  int v_alignmeth, bool v_alreadyformed);



//  void loadSlices(AbstractAngDataLoader::Pointer dataLoader);

	void find_border();
	int form_grains();
	void form_grains_sections();
	void remove_smallgrains();
	int renumber_grains1();
	int load_data(string);
	void assign_badpoints();
	void find_neighbors();
	void merge_containedgrains();
	int renumber_grains();
	int define_subgrains();
	int reburn_grains();
	void cleanup_data();
	void find_kernels();
	void homogenize_grains();
	void merge_twins();
	void merge_colonies();
	void characterize_twins();
	void characterize_colonies();
	int renumber_grains3();
	void find_euclidean_map ();
	void find_centroids ();
	void find_moments();
	void find_axes();
	void find_vectors(H5ReconStatsWriter::Pointer h5io);
	void find_centroids2D();
	void find_moments2D();
	void find_axes2D();
	void find_vectors2D(H5ReconStatsWriter::Pointer h5io);
	void find_eulerodf(H5ReconStatsWriter::Pointer h5io);
	void measure_misorientations(H5ReconStatsWriter::Pointer h5io);
	void find_colors();
	void find_convexities();
	void find_schmids();

  void write_graindata(const std::string &graindataFile);
	void align_sections(const std::string &filename );


  int volume_stats(H5ReconStatsWriter::Pointer h5io);
  int volume_stats2D(H5ReconStatsWriter::Pointer h5io);





	int writeVisualizationFile(const std::string &file); // DONE
  int writeIPFVizFile(const std::string &file);
  int writeDisorientationVizFile(const std::string &file); // DONE
  int writeImageQualityVizFile(const std::string &file); // DONE
  int writeSchmidFactorVizFile(const std::string &file); // DONE
  int writeDownSampledVizFile(const std::string &file);


	double getmisoquatcubic(double q1[5],double q2[5],double &n1,double &n2,double &n3);
	double getmisoquathexagonal(double q1[5],double q2[5],double &n1,double &n2,double &n3);

	double gamma(double);
	double find_xcoord(long);
	double find_ycoord(long);
	double find_zcoord(long);


  /* This is deprecated in favor of the HDF5 output file */
  void write_grains(const std::string &outputdir);

protected:
  ReconstructionFunc();

private:
	ReconstructionFunc(const ReconstructionFunc&);    // Copy Constructor Not Implemented
    void operator=(const ReconstructionFunc&);  // Operator '=' Not Implemented
};


#endif
