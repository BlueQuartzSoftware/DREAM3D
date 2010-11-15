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

#ifndef _Patch_H_
#define _Patch_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include <vector>

/**
* @class Patch Patch.h AIM/Common/Patch.h
* @brief Support class for the MicroGen3D class
* @author Michael A. Jackson for BlueQuartz Software, Dr. Michael Groeber for USAFRL
* @date Nov 4, 2009
* @version 1.0
*/
class Patch
{
public:
    Patch();
    virtual ~Patch();

    int node_id[3]; // stores three new node id for vertices of the triangles...
    int ngrainname[2]; // neighboring two grainnames...
    int edgePlace[3]; // if it's 0, face edges; if 1, inner edges...
    double normal[3];
    double area;
	int triID;
	int e_id[3];

  private:

    Patch(const Patch&);    // Copy Constructor Not Implemented
      void operator=(const Patch&);  // Operator '=' Not Implemented
};

#endif /* Patch_H_ */
