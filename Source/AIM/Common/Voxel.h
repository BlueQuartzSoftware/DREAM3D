///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VOXEL_H_
#define _VOXEL_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include <vector>
/*
 *
 */
class Voxel
{
public:
    Voxel();
    virtual ~Voxel();

    int grainname;
    double confidence;
    int alreadychecked;
    double xc;
    double yc;
    double zc;
    double euler1;
    double euler2;
    double euler3;
    int hasneighbor;
    int neighbor;
    double misorientation;
    int surfacevoxel;
    int neighbor1;
    int neighbor2;
    int neighbor3;
    int neighbor4;
    int neighbor5;
    int neighbor6;
    int available;
    int available90;
    int inside;
    std::vector<int> voxneighlist;

  private:

    Voxel(const Voxel&);    // Copy Constructor Not Implemented
      void operator=(const Voxel&);  // Operator '=' Not Implemented
};

#endif /* VOXEL_H_ */
