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

#ifndef _Face_H_
#define _Face_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include <vector>

/**
* @class Face Face.h AIM/Common/Face.h
* @brief Support class for the MicroGen3D class
* @author Michael A. Jackson for BlueQuartz Software, Dr. Michael Groeber for USAFRL
* @date Nov 4, 2009
* @version 1.0
*/
class Face
{
public:
    Face();
    virtual ~Face();

    int site_id[4]; // stores 4 sites at the corners of each square...
    int edge_id[4]; // stores edge id turned on...others will have dummy -1...
    int nEdge; // number of edges on the square...
    int turnFC; // if 1, face center is on..., else it's 0
    int FCnode; // face center node...if not, it's -1...
    int effect; // 0 if the square is useless; 1 is good...

  private:

    Face(const Face&);    // Copy Constructor Not Implemented
      void operator=(const Face&);  // Operator '=' Not Implemented
};

#endif /* Face_H_ */
