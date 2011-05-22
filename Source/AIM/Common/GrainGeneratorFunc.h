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
#include "AIM/Common/GrainGeneratorVoxel.h"
#include "AIM/Common/AIMRandomNG.h"
#include "AIM/Common/HDF5/H5ReconStatsWriter.h"
#include "AIM/Common/HDF5/H5ReconStatsReader.h"
#include "AIM/Common/OrientationMath.h"
#include "AIM/Common/OrientationOps/CubicOps.h"
#include "AIM/Common/OrientationOps/HexagonalOps.h"
#include "AIM/Common/OrientationOps/OrthoRhombicOps.h"




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
    MXA_SHARED_POINTERS(GrainGeneratorFunc)
    MXA_STATIC_NEW_MACRO(GrainGeneratorFunc)

    virtual ~GrainGeneratorFunc();

    typedef boost::shared_array<float>    SharedFloatArray;
    typedef boost::shared_array<int>      SharedIntArray;

	bool periodic_boundaries;
	
	float sizex;
    float sizey;
    float sizez;

    unsigned long long int GGseed;
    
	float resx;
    float resy;
    float resz;
    float packingresx;
    float packingresy;
    float packingresz;

    int xpoints;
    int ypoints;
    int zpoints;
    int totalpoints;
    int packingxpoints;
    int packingypoints;
    int packingzpoints;
    int packingtotalpoints;

    float misorientationtolerance;

    std::vector<AIM::Reconstruction::CrystalStructure> crystruct;
    std::vector<AIM::Reconstruction::PhaseType>        phaseType;
    std::vector<float>                                 pptFractions;

    boost::shared_array<GrainGeneratorVoxel> voxels;

    std::vector<Grain::Pointer> m_Grains;
	std::vector<float> sizetest;

	std::vector<float> grainowners;
	std::vector<float> goalgrainowners;
	
	std::vector<SharedFloatArray> actualodf;
    std::vector<SharedFloatArray> simodf;
    std::vector<SharedFloatArray> axisodf;

    std::vector<SharedFloatArray> actualmdf;
    std::vector<SharedFloatArray> simmdf;
    std::vector<SharedFloatArray> actualmicrotex;
    std::vector<SharedFloatArray> simmicrotex;

    std::vector<SharedFloatArray> graincenters;
    std::vector<SharedFloatArray> grainmoments;

	std::vector<int> primaryphases;
	std::vector<double> primaryphasefractions;
	std::vector<int> precipitatephases;
	std::vector<double> precipitatephasefractions;

    std::vector<int> takencheck;
    std::vector<std::vector<std::vector<float> > > bovera;
    std::vector<std::vector<std::vector<float> > > covera;
    std::vector<std::vector<std::vector<float> > > coverb;
    std::vector<std::vector<std::vector<float> > > omega3;
    std::vector<std::vector<std::vector<float> > > neighborparams;
    std::vector<std::vector<float> > grainsizedist;
    std::vector<std::vector<float> > simgrainsizedist;
    std::vector<std::vector<std::vector<float> > > neighbordist;
    std::vector<std::vector<std::vector<float> > > simneighbordist;
    std::vector<std::vector<std::vector<int> > > boundaries;

    float machineepsilon;
    float maxrealnumber;
    float minrealnumber;
    int numorients;
    int numeulers;
    int resdiff;
    std::vector<float> unbiasedvol;
    std::vector<float> totalsurfacearea;
    int numgrains;
    int shapeclass;
    std::vector<float> phasefraction;
    std::vector<float> binstepsize;
    std::vector<float> grainsizediststep;
    std::vector<float> maxdiameter;
    std::vector<float> mindiameter;
    std::vector<float> avgdiam;
    std::vector<float> sddiam;
    std::vector<int> numdiameterbins;
    int worstgrain;
    std::vector<float> totalaxes;

    float totalvol;
    float totalprecipvol;
    int numneighbins;
    float volcheck;
    int ownercheck;
    float currentfillingerror, oldfillingerror;
    float currentneighborhooderror, oldneighborhooderror;
    float currentsizedisterror, oldsizedisterror;
    float fillingerrorweight;
    float neighborhooderrorweight;
    float sizedisterrorweight;

#if 0
    void initialize(int32_t m_NumGrains, int32_t m_ShapeClass,
                    float m_XResolution, float m_YResolution, float m_ZResolution,
                    float m_fillingerrorweight, float m_neighborhooderrorweight, float m_sizedisterrorweight,
                    int32_t m_Precipitates, float m_FractionPrecipitates,
                    std::vector<AIM::Reconstruction::CrystalStructure> m_CrystalStructure,
                    std::vector<AIM::Reconstruction::PhaseType> m_PhaseType);

#endif
	void initializeArrays(std::vector<AIM::Reconstruction::CrystalStructure> structures);
    void initialize_packinggrid();

    void write_eulerangles(const std::string & filename);
    void write_graindata(const std::string & filename);
    int readReconStatsData(H5ReconStatsReader::Pointer h5io);
    int readAxisOrientationData(H5ReconStatsReader::Pointer h5io);
    int readODFData(H5ReconStatsReader::Pointer h5io);
    int readMisorientationData(H5ReconStatsReader::Pointer h5io);
    int readMicroTextureData(H5ReconStatsReader::Pointer h5io);

    int pack_grains(const std::string & filename, int numgrains);
    void generate_grain(int, int);
    void assign_eulers(int);
    void insert_grain(size_t grainNum);
    void move_grain(size_t grainNum, float xc, float yc, float zc);
    void add_grain(size_t grainNum);
    void remove_grain(size_t grainNum);
    void determine_neighbors(size_t grainNum, int add);
	void compare_1Ddistributions(std::vector<float>, std::vector<float>, float &sqrerror);
	void compare_2Ddistributions(std::vector<std::vector<float> >, std::vector<std::vector<float> >, float &sqrerror);
	void compare_3Ddistributions(std::vector<std::vector<std::vector<float> > >, std::vector<std::vector<std::vector<float> > >, float &sqrerror);
    float check_neighborhooderror(int gadd, int gremove);
    float check_sizedisterror(int gadd, int gremove);
    float check_fillingerror(int gadd, int gremove);
    int assign_voxels(int numgrains);
    void fill_gaps(int numgrains);
	int adjust_boundaries(int numgrains);
    int place_precipitates(int numgrains);
    void find_neighbors();
    void swapOutOrientation(int & badtrycount, int & numbins, float currentodferror, float currentmdferror);
    void switchOrientations(int & badtrycount, int & numbins, float currentodferror, float currentmdferror);
    void MC_LoopBody1(int phase, size_t neighbor, int j, std::vector<float>* misolist, std::vector<float>* neighborsurfarealist, float &mdfchange);
    void MC_LoopBody2(int phase, size_t neighbor, int j, std::vector<float>* misolist, std::vector<float>* neighborsurfarealist);
    void matchCrystallography(const std::string & ErrorFile, H5ReconStatsWriter::Pointer h5io);
    void measure_misorientations();
    void find_centroids();
    void find_moments();
    void find_axes();
    void find_vectors(H5ReconStatsWriter::Pointer h5io);
    int volume_stats(H5ReconStatsWriter::Pointer h5io);
    float gamma(float);
    float erf(float);
    float erfc(float);
    float gammastirf(float);
    float lngamma(float, float& );
    float incompletebeta(float, float, float);
    float incompletebetafe(float, float, float, float, float);
    float incompletebetafe2(float, float, float, float, float);
    float incompletebetaps(float, float, float, float);
    float find_xcoord(long long int index);
    float find_ycoord(long long int index);
    float find_zcoord(long long int index);
protected:
    GrainGeneratorFunc();
private:
    std::vector<OrientationMath*>    m_OrientatioOps;
    OrientationMath::Pointer                m_CubicOps;
    OrientationMath::Pointer            m_HexOps;
    OrientationMath::Pointer         m_OrthoOps;

    GrainGeneratorFunc(const GrainGeneratorFunc& );
    void operator =(const GrainGeneratorFunc& );
};


#endif
