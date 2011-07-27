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
#include <map>

#include <boost/shared_array.hpp>

#include <MXA/MXATypes.h>
#include <MXA/Common/MXASetGetMacros.h>

#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/AIMArray.hpp"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/Grain.h"
#include "DREAM3D/Common/AIMRandomNG.h"

#include "DREAM3D/Common/OrientationMath.h"
#include "DREAM3D/Common/OrientationOps/CubicOps.h"
#include "DREAM3D/Common/OrientationOps/HexagonalOps.h"
#include "DREAM3D/Common/OrientationOps/OrthoRhombicOps.h"

#include "DREAM3D/Common/ShapeOps/ShapeOps.h"
#include "DREAM3D/Common/ShapeOps/CubeOctohedronOps.h"
#include "DREAM3D/Common/ShapeOps/CylinderOps.h"
#include "DREAM3D/Common/ShapeOps/EllipsoidOps.h"
#include "DREAM3D/Common/ShapeOps/SuperEllipsoidOps.h"

#include "DREAM3D/HDF5/H5ReconStatsWriter.h"
#include "DREAM3D/HDF5/H5ReconStatsReader.h"



/**
 * @class GrainGeneratorFunc GrainGeneratorFunc.h AIM/Common/GrainGeneratorFunc.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class GrainGeneratorFunc
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
    std::vector<AIM::SyntheticBuilder::ShapeType>      shapeTypes;

    DECLARE_WRAPPED_ARRAY(grain_indicies, m_GrainIndicies, int)
    DECLARE_WRAPPED_ARRAY(phases, m_Phases, int);
    DECLARE_WRAPPED_ARRAY(euler1s, m_Euler1s, float);
    DECLARE_WRAPPED_ARRAY(euler2s, m_Euler2s, float);
    DECLARE_WRAPPED_ARRAY(euler3s, m_Euler3s, float);
    DECLARE_WRAPPED_ARRAY(neighbors, m_Neighbors, int);
    DECLARE_WRAPPED_ARRAY(surfacevoxels, m_SurfaceVoxels, float);
    DECLARE_WRAPPED_ARRAY(quats, m_Quats, float); // n x 5 array

    std::vector<Grain::Pointer> m_Grains;
	  std::vector<float> sizetest;

	  std::vector<std::vector<std::vector<int> > > grainowners;
	  std::vector<std::vector<std::vector<int> > > goalgrainowners;

	  std::vector<SharedFloatArray> actualodf;
    std::vector<SharedFloatArray> simodf;
    std::vector<SharedFloatArray> axisodf;

    std::vector<SharedFloatArray> actualmdf;
    std::vector<SharedFloatArray> simmdf;
    std::vector<SharedFloatArray> actualmicrotex;
    std::vector<SharedFloatArray> simmicrotex;

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
    int numprimarygrains;
    std::vector<float> phasefraction;
    std::vector<float> binstepsize;
    std::vector<float> grainsizediststep;
    std::vector<float> maxdiameter;
    std::vector<float> mindiameter;
    std::vector<float> avgdiam;
    std::vector<float> sddiam;
    std::vector<int> numdiameterbins;
    int worstgrain;
//    std::vector<float> totalaxes;

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
	void initializeAttributes();
	  void initializeArrays(std::vector<AIM::Reconstruction::CrystalStructure> structures);
    void initialize_packinggrid();

    void write_eulerangles(const std::string & filename);
    int readReconStatsData(H5ReconStatsReader::Pointer h5io);
    int readAxisOrientationData(H5ReconStatsReader::Pointer h5io);
    int readODFData(H5ReconStatsReader::Pointer h5io);
    int readMisorientationData(H5ReconStatsReader::Pointer h5io);

    void pack_grains();
    void generate_grain(int, int);
    void assign_eulers();
    void insert_grain(size_t grainNum);
    void insert_precipitate(size_t grainNum);
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
    void assign_voxels();
    void fill_gaps();
    void fillin_precipitates();
	void adjust_boundaries();
    void place_precipitates();
    void find_neighbors();
    void swapOutOrientation(int & badtrycount, int & numbins, float currentodferror, float currentmdferror);
    void switchOrientations(int & badtrycount, int & numbins, float currentodferror, float currentmdferror);
    void MC_LoopBody1(int phase, size_t neighbor, int j, std::vector<float>* misolist, std::vector<float>* neighborsurfarealist, float &mdfchange);
    void MC_LoopBody2(int phase, size_t neighbor, int j, std::vector<float>* misolist, std::vector<float>* neighborsurfarealist);
    void matchCrystallography();
    void measure_misorientations();
	void write_graindata(const std::string & filename);
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

    int writePhFormat(const std::string &file);

protected:
    GrainGeneratorFunc();
private:
    std::vector<OrientationMath*>    m_OrientatioOps;
    OrientationMath::Pointer         m_CubicOps;
    OrientationMath::Pointer         m_HexOps;
    OrientationMath::Pointer         m_OrthoOps;

    std::map<AIM::SyntheticBuilder::ShapeType, DREAM3D::ShapeOps*>           m_ShapeOps;
    DREAM3D::ShapeOps::Pointer                m_UnknownShapeOps;
    DREAM3D::ShapeOps::Pointer                m_CubicOctohedronOps;
    DREAM3D::ShapeOps::Pointer                m_CylinderOps;
    DREAM3D::ShapeOps::Pointer                m_EllipsoidOps;
    DREAM3D::ShapeOps::Pointer                m_SuprtEllipsoidOps;


    GrainGeneratorFunc(const GrainGeneratorFunc& );
    void operator =(const GrainGeneratorFunc& );
};


#endif
