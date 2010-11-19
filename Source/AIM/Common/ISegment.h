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

#ifndef _ISegment_H_
#define _ISegment_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include <vector>

#include "AIM/Common/AIMCommonConfiguration.h"

/**
* @class ISegment ISegment.h AIM/Common/ISegment.h
* @brief Support class for the MicroGen3D class
* @author Michael A. Jackson for BlueQuartz Software, Dr. Michael Groeber for USAFRL
* @date Nov 4, 2009
* @version 1.0
*/
class AIMCOMMON_EXPORT ISegment
{
public:
    ISegment();
    virtual ~ISegment();

    int n_id[2];
    int new_n_id[2];
    int edgeKind;
    int ngrainname[4];
    int burnt; // if not burnt it's -1...
    int eff; // flag for output, if it's 1 it will be used for output...

  private:

    ISegment(const ISegment&);    // Copy Constructor Not Implemented
      void operator=(const ISegment&);  // Operator '=' Not Implemented
};

#endif /* ISegment_H_ */
