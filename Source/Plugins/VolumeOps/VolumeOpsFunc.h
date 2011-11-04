/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
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

//    typedef boost::shared_array<float>    SharedFloatArray;
//    typedef boost::shared_array<int>      SharedIntArray;

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

    void initialize();

protected:
    VolumeOpsFunc();



private:
    std::vector<OrientationMath*> m_OrientationOps;
    CubicOps::Pointer             m_CubicOps;
    HexagonalOps::Pointer         m_HexOps;
    OrthoRhombicOps::Pointer      m_OrthoOps;

	VolumeOpsFunc(const VolumeOpsFunc& );
    void operator =(const VolumeOpsFunc& );
};


#endif
