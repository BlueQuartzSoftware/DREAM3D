
#ifndef _VolumeOpsFunc_H
#define _VolumeOpsFunc_H

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

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AIMArray.hpp"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/Grain.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"
#include "DREAM3DLib/Common/Observable.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/HDF5/H5StatsWriter.h"


/**
 * @class VolumeOpsFunc VolumeOpsFunc.h AIM/Common/VolumeOpsFunc.h
 * @brief This class holds data associated with the pipeline for the VolumeOps
 * plugin.
 * @author
 * @date
 * @version 1.0
 */
class VolumeOpsFunc
{
public:
    DREAM3D_SHARED_POINTERS(VolumeOpsFunc)
    DREAM3D_TYPE_MACRO(VolumeOpsFunc)
    DREAM3D_STATIC_NEW_MACRO(VolumeOpsFunc);

    virtual ~VolumeOpsFunc();

    typedef boost::shared_array<float>    SharedFloatArray;
    typedef boost::shared_array<int>      SharedIntArray;

	std::vector<Ebsd::CrystalStructure> crystruct;
    std::vector<DREAM3D::Reconstruction::PhaseType> phaseType;

    DECLARE_WRAPPED_ARRAY(grain_indicies_old, m_GrainIndicies_old, int)
    DECLARE_WRAPPED_ARRAY(phases_old, m_Phases_old, int);
    DECLARE_WRAPPED_ARRAY(euler1s_old, m_Euler1s_old, float);
    DECLARE_WRAPPED_ARRAY(euler2s_old, m_Euler2s_old, float);
    DECLARE_WRAPPED_ARRAY(euler3s_old, m_Euler3s_old, float);
    DECLARE_WRAPPED_ARRAY(grain_indicies, m_GrainIndicies, int)
    DECLARE_WRAPPED_ARRAY(phases, m_Phases, int);
    DECLARE_WRAPPED_ARRAY(euler1s, m_Euler1s, float);
    DECLARE_WRAPPED_ARRAY(euler2s, m_Euler2s, float);
    DECLARE_WRAPPED_ARRAY(euler3s, m_Euler3s, float);

    float resx;
    float resy;
    float resz;
    float resx_old;
    float resy_old;
    float resz_old;
    float sizex;
    float sizey;
    float sizez;
    float sizex_old;
    float sizey_old;
    float sizez_old;
    int xpoints;
    int ypoints;
    int zpoints;
    int xpoints_old;
    int ypoints_old;
    int zpoints_old;
    int totalpoints;
    int totalpoints_old;
	float xstart;
	float ystart;
	float zstart;

	void initializeAttributes();
	void manipulate_volume();


protected:
    VolumeOpsFunc();
    
private:
    std::vector<OrientationMath*> m_OrientationOps;
    CubicOps::Pointer m_CubicOps;
    HexagonalOps::Pointer m_HexOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

	VolumeOpsFunc(const VolumeOpsFunc& );
    void operator =(const VolumeOpsFunc& );
};


#endif
