///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, 2010 Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, 2010 Michael Groeber, US Air Force Research Laboratory
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

#include <boost/shared_array.hpp>

#include <MXA/MXATypes.h>
#include <MXA/Common/MXASetGetMacros.h>

#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/AIMArray.hpp"
#include "AIM/Common/Constants.h"
#include "AIM/Common/Grain.h"
#include "AIM/Common/Voxel.h"
#include "AIM/Common/AIMRandomNG.h"
#include "AIM/Common/HDF5/H5ReconStatsWriter.h"
#include "AIM/Common/HDF5/H5ReconStatsReader.h"

typedef boost::shared_array<double>    DoubleArray;


/**
 * @class GrainGeneratorFunc GrainGeneratorFunc.h AIM/Common/GrainGeneratorFunc.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class AIMCOMMON_EXPORT GrainGeneratorFunc
{
public:
    MXA_SHARED_POINTERS(GrainGeneratorFunc)MXA_STATIC_NEW_MACRO(GrainGeneratorFunc)virtual ~GrainGeneratorFunc();


    double resx;
    double resy;
    double resz;
    double sizex;
    double sizey;
    double sizez;
    double misorientationtolerance;
    std::vector<AIM::Reconstruction::CrystalStructure> crystruct;
    AIMRandomNG rg;

    std::vector<DoubleArray> actualodf;

    std::vector<DoubleArray> simodf;
    std::vector<DoubleArray> axisodf;
    Voxel* voxels;
    std::vector<DoubleArray> actualmdf;
    std::vector<DoubleArray> simmdf;
    std::vector<DoubleArray> actualmicrotex;
    std::vector<DoubleArray> simmicrotex;

//    int* graincounts;
    std::vector<DoubleArray> graincenters;
    std::vector<DoubleArray> grainmoments;

//    Grain *precipitates;
    std::vector<Grain::Pointer> precipitates;
//    int* psizes;

    std::vector<Grain::Pointer> m_Grains;

    std::vector<int> activegrainlist;
    std::vector<int> precipitateorder;
    std::vector<int> takencheck;
    std::vector<std::vector<double> > grainsizedist;
    std::vector<std::vector<double> > simgrainsizedist;
    std::vector<std::vector<std::vector<double> > > bovera;
    std::vector<std::vector<std::vector<double> > > covera;
    std::vector<std::vector<std::vector<double> > > coverb;
    std::vector<std::vector<std::vector<double> > > neighborhood;
    std::vector<std::vector<std::vector<double> > > omega3;
    std::vector<std::vector<std::vector<double> > > neighbordist;
    std::vector<std::vector<std::vector<double> > > neighborhoodfit;
    std::vector<std::vector<std::vector<double> > > svbovera;
    std::vector<std::vector<std::vector<double> > > svcovera;
    std::vector<std::vector<std::vector<double> > > svcoverb;
    std::vector<std::vector<std::vector<double> > > svschmid;
    std::vector<std::vector<std::vector<double> > > svomega3;
    std::vector<std::vector<std::vector<int> > > boundaries;

    double machineepsilon;
    double maxrealnumber;
    double minrealnumber;
    int numorients;
    int numeulers;
    int resdiff;
    std::vector<double> unbiasedvol;
    std::vector<double> totalsurfacearea;
    int numgrains;
    int numextragrains;
    int shapeclass;
    std::vector<double> phasefraction;
    std::vector<double> binstepsize;
    std::vector<double> grainsizediststep;
    std::vector<double> maxdiameter;
    std::vector<double> mindiameter;
    std::vector<double> avgdiam;
    std::vector<double> sddiam;
    std::vector<int> numdiameterbins;
    int worstgrain;
    std::vector<double> totalaxes;
    int32_t xpoints;
    int32_t ypoints;
    int32_t zpoints;
    int totalpoints;
    double totalvol;
    int numneighbins;
    double volcheck;
    int ownercheck;
    double currentfillingerror, oldfillingerror;
    double currentneighborhooderror, oldneighborhooderror;
    double currentsizedisterror, oldsizedisterror;
    double fillingerrorweight;
    double neighborhooderrorweight;
    double sizedisterrorweight;

    void initialize(int32_t m_NumGrains, int32_t m_ShapeClass,
                    double m_XResolution, double m_YResolution, double m_ZResolution,
                    double m_fillingerrorweight, double m_neighborhooderrorweight,
                    double m_sizedisterrorweight,
                    int32_t m_Precipitates, double m_FractionPrecipitates,
                    std::vector<AIM::Reconstruction::CrystalStructure> m_CrystalStructure);
    void initializeArrays();
    void initialize2();

    void write_eulerangles(const std::string & filename);
    void write_graindata(const std::string & filename);
    void writeCube(const std::string & filename, int numgrains);
    int readReconStatsData(H5ReconStatsReader::Pointer h5io);
    int readAxisOrientationData(H5ReconStatsReader::Pointer h5io);
    int readODFData(H5ReconStatsReader::Pointer h5io);
    int readMisorientationData(H5ReconStatsReader::Pointer h5io);
    int readMicroTextureData(H5ReconStatsReader::Pointer h5io);
    void read_structure(const std::string & filename);
    int pack_grains(const std::string & filename, int numgrains);
    void generate_grain(int, int);
    void assign_eulers(int);
    void insert_grain(size_t grainNum);
    void add_grain(size_t grainNum);
    void remove_grain(size_t grainNum);
    double costcheck_remove(size_t grainNum);
    double costcheck_add(size_t grainNum);
    void determine_neighbors();
    double check_neighborhooderror(int gadd, int gremove);
    double check_sizedisterror(int gadd, int gremove);
    int assign_voxels(int numgrains);
    void fill_gaps(int numgrains);
	int adjust_boundaries(int numgrains);
    void find_neighbors();
    void swapOutOrientation(int & badtrycount, int & numbins, double currentodferror, double currentmdferror);
    void switchOrientations(int & badtrycount, int & numbins, double currentodferror, double currentmdferror);
    void MC_LoopBody1(int phase, size_t neighbor, int j, std::vector<double>* misolist, std::vector<double>* neighborsurfarealist, double &mdfchange);
    void MC_LoopBody2(int phase, size_t neighbor, int j, std::vector<double>* misolist, std::vector<double>* neighborsurfarealist);
    void matchCrystallography(const std::string & ErrorFile, H5ReconStatsWriter::Pointer h5io);
    void measure_misorientations();
    void find_centroids();
    void find_moments();
    void find_axes();
    void find_vectors(H5ReconStatsWriter::Pointer h5io);
    int volume_stats(H5ReconStatsWriter::Pointer h5io);
    double gamma(double);
    double erf(double);
    double erfc(double);
    double gammastirf(double);
    double lngamma(double, double& );
    double incompletebeta(double, double, double);
    double incompletebetafe(double, double, double, double, double);
    double incompletebetafe2(double, double, double, double, double);
    double incompletebetaps(double, double, double, double);
    double find_xcoord(long );
    double find_ycoord(long );
    double find_zcoord(long );
protected:
    GrainGeneratorFunc();
private:
    GrainGeneratorFunc(const GrainGeneratorFunc& );
    void operator =(const GrainGeneratorFunc& );
};


#endif
