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

/**
* @class Voxel Voxel.h AIM/Common/Voxel.h
* @brief Support class for the MicroGen3D class
* @author Michael A. Jackson for BlueQuartz Software, Dr. Michael Groeber for USAFRL
* @date Nov 4, 2009
* @version 1.0
*/
class Voxel
{
public:
    Voxel();
    virtual ~Voxel();

    int grainname;
    double confidence;
    double imagequality;
    int alreadychecked;
	int nearestneighbor;
	double nearestneighbordistance;
    double euler1;
    double euler2;
    double euler3;
    int neighbor;
	int numowners;
    double misorientation;
	double kernelmisorientation;
    int surfacevoxel;
	int unassigned;
	double quat[5];

  private:

    Voxel(const Voxel&);    // Copy Constructor Not Implemented
      void operator=(const Voxel&);  // Operator '=' Not Implemented
};

#endif /* VOXEL_H_ */
