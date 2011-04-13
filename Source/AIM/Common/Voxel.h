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

#ifndef _VOXEL_H_
#define _VOXEL_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include <vector>

#include "MXA/Common/MXASetGetMacros.h"

#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/Constants.h"

/**
* @class Voxel Voxel.h AIM/Common/Voxel.h
* @brief Support class for the MicroGen3D class
* @author Michael A. Jackson for BlueQuartz Software, Dr. Michael Groeber for USAFRL
* @date Nov 4, 2009
* @version 1.0
*/
class AIMCOMMON_EXPORT Voxel
{
  public:
    MXA_SHARED_POINTERS(Voxel);
    MXA_STATIC_NEW_MACRO(Voxel);
    MXA_TYPE_MACRO(Voxel);
    Voxel();
    virtual ~Voxel();

    /**
     * @brief Performs a Depp copy of the Grain object by making copies of the contained
     * data and not just copying the pointers to the data
     * @param grain Another Grain object to copy from.
     */
    void deepCopy(Voxel* voxel);

    int grainname;
    double confidence;
    double imagequality;
    double imagequality2;
   	double ellipfunc;
    int alreadychecked;
    int nearestneighbor[3];
    double nearestneighbordistance[3];
    double euler1;
    double euler2;
    double euler3;
    float phase;
    int neighbor;
    int numowners;
    double grainmisorientation;
    double misorientationgradient;
    double kernelmisorientation;
    int surfacevoxel;
    int unassigned;
    double quat[5];

    IntVectorType grainlist;
    DoubleVectorType ellipfunclist;
    IntVectorType neighborlist;

  protected:


  private:
    Voxel(const Voxel&); // Copy Constructor Not Implemented
    void operator=(const Voxel&); // Operator '=' Not Implemented
};

#endif /* VOXEL_H_ */
