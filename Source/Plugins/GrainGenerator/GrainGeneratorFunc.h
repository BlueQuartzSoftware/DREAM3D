/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef _GrainGeneratorFunc_H
#define _GrainGeneratorFunc_H

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif



#include <vector>
#include <map>


#include <boost/shared_array.hpp>

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/DREAM3DLib.h"

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/AIMArray.hpp"
#include "DREAM3DLib/Common/Grain.h"
#include "DREAM3DLib/Common/Observable.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/ShapeOps/ShapeOps.h"

/**
 * @class GrainGeneratorFunc GrainGeneratorFunc.h AIM/Common/GrainGeneratorFunc.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class GrainGeneratorFunc : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS(GrainGeneratorFunc)
    DREAM3D_STATIC_NEW_MACRO(GrainGeneratorFunc)

    virtual ~GrainGeneratorFunc();

    typedef boost::shared_array<float> SharedFloatArray;
    typedef boost::shared_array<int> SharedIntArray;

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

    std::vector<Ebsd::CrystalStructure> crystruct;
    std::vector<DREAM3D::Reconstruction::PhaseType> phaseType;
    std::vector<float> pptFractions;
    std::vector<DREAM3D::SyntheticBuilder::ShapeType> shapeTypes;

    // Voxel Attribute Arrays
    DECLARE_WRAPPED_ARRAY(grain_indicies, m_GrainIndicies, int)
    DECLARE_WRAPPED_ARRAY(ellipfuncs, m_Ellipfuncs, float)
    DECLARE_WRAPPED_ARRAY(phases, m_Phases, int)
    DECLARE_WRAPPED_ARRAY(euler1s, m_Euler1s, float)
    DECLARE_WRAPPED_ARRAY(euler2s, m_Euler2s, float)
    DECLARE_WRAPPED_ARRAY(euler3s, m_Euler3s, float)
    DECLARE_WRAPPED_ARRAY(surfacevoxels, m_SurfaceVoxels, char)


    std::vector<std::vector<std::vector<int> > > grainowners;

    std::vector<Grain::Pointer> m_Grains;
    std::vector<int> gsizes;
    std::vector<int> newnames;
    std::vector<float> sizetest;

    // These use the typedefs because the length of each array is determined by
    // the cystal structure of the phase. The lengths for each type of crystal
    // structure are known and constant
    std::vector<SharedFloatArray> actualodf;
    std::vector<SharedFloatArray> simodf;
    std::vector<SharedFloatArray> actualmdf;
    std::vector<SharedFloatArray> simmdf;
    std::vector<SharedFloatArray> axisodf;

    std::vector<int> primaryphases;
    std::vector<float> primaryphasefractions;
    std::vector<int> precipitatephases;
    std::vector<float> precipitatephasefractions;

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

//    float machineepsilon;
//    float maxrealnumber;
//    float minrealnumber;
    int numorients;
    int numeulers;
    int resdiff;
    std::vector<float> unbiasedvol;
    DECLARE_WRAPPED_ARRAY(totalsurfacearea, m_TotalSurfaceArea, float);
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

    float totalvol;
    float totalprecipvol;
    int numneighbins;
    float volcheck;
    int ownercheck;
    float fillingerror, oldfillingerror;
    float currentneighborhooderror, oldneighborhooderror;
    float currentsizedisterror, oldsizedisterror;
    float neighborhooderrorweight;


    std::vector<OrientationMath*> m_OrientatioOps;
    std::map<DREAM3D::SyntheticBuilder::ShapeType, DREAM3D::ShapeOps*> m_ShapeOps;

  protected:
    GrainGeneratorFunc();
  private:

    OrientationMath::Pointer m_CubicOps;
    OrientationMath::Pointer m_HexOps;
    OrientationMath::Pointer m_OrthoOps;

    DREAM3D::ShapeOps::Pointer m_UnknownShapeOps;
    DREAM3D::ShapeOps::Pointer m_CubicOctohedronOps;
    DREAM3D::ShapeOps::Pointer m_CylinderOps;
    DREAM3D::ShapeOps::Pointer m_EllipsoidOps;
    DREAM3D::ShapeOps::Pointer m_SuprtEllipsoidOps;

    GrainGeneratorFunc(const GrainGeneratorFunc&);
    void operator =(const GrainGeneratorFunc&);
};

#endif
