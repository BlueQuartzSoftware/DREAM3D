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
#ifndef ANGFILEHELPER_H_
#define ANGFILEHELPER_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include "MXA/Common/MXASetGetMacros.h"

#include "ANG/AngConstants.h"
#include "ANG/AngDirectoryPatterns.h"

#include "AIM/ANG/AbstractAngDataLoader.h"
#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/Voxel.h"



class AIMCOMMON_EXPORT AngDataLoader : public AbstractAngDataLoader
{
  public:
    MXA_SHARED_POINTERS(AngDataLoader)
    MXA_STATIC_NEW_SUPERCLASS(AbstractAngDataLoader, AngDataLoader)

    virtual ~AngDataLoader();

    MXA_INSTANCE_PROPERTY(AngDirectoryPatterns::Pointer, DirectoryPattern);
    MXA_INSTANCE_PROPERTY(int, ZStartIndex)
    MXA_INSTANCE_PROPERTY(int, ZEndIndex)
    MXA_INSTANCE_PROPERTY(float, ZResolution)
    MXA_INSTANCE_PROPERTY(Ang::Orientation, Orientation)
    MXA_INSTANCE_PROPERTY(bool, Cancel);

    int getSizeAndResolution(int &xpoints, int &ypoints, int &zpoints,
                                       double &xres, double &yres, double &zres);

    int loadData(Voxel voxels[], int xpoints, int ypoints, int zpoints);

  protected:
    AngDataLoader();
  private:
    AngDataLoader(const AngDataLoader&);    // Copy Constructor Not Implemented
    void operator=(const AngDataLoader&);  // Operator '=' Not Implemented

};

#endif /* ANGFILEHELPER_H_ */
