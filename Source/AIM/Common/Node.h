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

#ifndef _Node_H_
#define _Node_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include <vector>

#include "AIM/Common/AIMCommonConfiguration.h"

/**
* @class Node Node.h AIM/Common/Node.h
* @brief Support class for the MicroGen3D class
* @author Michael A. Jackson for BlueQuartz Software, Dr. Michael Groeber for USAFRL
* @date Nov 4, 2009
* @version 1.0
*/
class AIMCOMMON_EXPORT Node
{
public:
    Node();
    virtual ~Node();

    int nodeKind; // 2 for binary, 3 for triple, and so on...
    double xc;
    double yc;
    double zc;
    int point;
    int NodeID; // newID for used nodes; if not used, it's -1...
};

#endif /* Node_H_ */
