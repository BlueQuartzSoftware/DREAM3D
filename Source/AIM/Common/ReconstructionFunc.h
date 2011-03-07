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
#include "AIM/Common/AIMArray.hpp"
#include "AIM/Common/Constants.h"
#include "AIM/Common/Grain.h"
#include "AIM/Common/Voxel.h"
#include "AIM/Common/AIMRandomNG.h"
#include "AIM/ANG/AbstractAngDataLoader.h"
#include "AIM/Common/HDF5/H5ReconStatsWriter.h"


using namespace std;

/**
 * @class ReconstructionFunc ReconstructionFunc.h AIM/Common/ReconstructionFunc.h
 * @brief This class holds all the various functions that are needed to reconstruct
 * a 3D representation from a stack of OIM Data.
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Michael A. Groeber (US Air Force Research Laboratory)
 * @version 2.0
 */
class AIMCOMMON_EXPORT ReconstructionFunc
{

  public:

    MXA_SHARED_POINTERS(ReconstructionFunc)
    MXA_STATIC_NEW_MACRO(ReconstructionFunc)

    virtual ~ReconstructionFunc();

    typedef AIMArray<double> DoubleArrayType;
    typedef AIMArray<int>    IntArrayType;

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
    double sizebinstepsize;
    int minallowedgrainsize;
    int mergetwinsoption;
    int mergecoloniesoption;
    AIM::Reconstruction::CrystalStructure crystruct;
    int alignmeth;
    int alreadyformed;


    AIMRandomNG rg;
    Voxel* voxels;
    Voxel* voxelstemp;
    std::vector<Grain::Pointer> m_Grains;

    int **shifts;
    int **arr;
    int *graincounts;
    double **quat_symm;

    vector<vector<double> > graincenters;
    vector<vector<double> > grainmoments;
    // vector<vector<double> > grainquats;
    DoubleArrayType::Pointer m_grainQuats;

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
 //   int numgrains;
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
                  double v_sizebinstepsize, AIM::Reconstruction::CrystalStructure v_crystruct,
                  int v_alignmeth, bool v_alreadyformed);


    void find_border();
    void form_grains();
    void form_grains_sections();
    void remove_smallgrains();
    int renumber_grains1();
    int load_data(const std::string &filename);
    void assign_badpoints();
    void find_neighbors();
    void define_neighborhood();
    void merge_containedgrains();
    void reorder_grains(const std::string &reconVisFile);
    int remove_smallgrains(int numgrains);
	  void fillin_sample();
    void cleanup_data();
    void find_grain_and_kernel_misorientations();
    void merge_twins();
    void merge_colonies();
    void characterize_twins();
    void characterize_colonies();
    void renumber_grains3();
    void find_euclidean_map();

    void find_vectors(H5ReconStatsWriter::Pointer h5io);
    void find_centroids();
    void find_moments();
    void find_axes();
    void find_colors();
    void find_schmids();

    void find_vectors2D(H5ReconStatsWriter::Pointer h5io);
    void find_centroids2D();
    void find_moments2D();
    void find_axes2D();

    void deformation_stats(const std::string &filename, const std::string &filename2);
    void write_graindata(const std::string &graindataFile);
    void align_sections(const std::string &filename);
    /* This is deprecated in favor of the HDF5 output file */
    void write_grains(const std::string &outputdir);


    void find_eulerodf(H5ReconStatsWriter::Pointer h5io);
    void measure_misorientations(H5ReconStatsWriter::Pointer h5io);
    int volume_stats(H5ReconStatsWriter::Pointer h5io);
    int volume_stats2D(H5ReconStatsWriter::Pointer h5io);

    double gamma(double x);
    double find_xcoord(size_t index);
    double find_ycoord(size_t index);
    double find_zcoord(size_t index);

  protected:
    ReconstructionFunc();

  private:
    ReconstructionFunc(const ReconstructionFunc&);    // Copy Constructor Not Implemented
    void operator=(const ReconstructionFunc&);  // Operator '=' Not Implemented
};


#endif
