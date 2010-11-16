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


#include <MXA/Common/MXASetGetMacros.h>
#include <AIM/Common/Voxel.h>
#include <AIM/ANG/AngDirectoryPatterns.h>


class AngFileHelper
{
  public:
    MXA_SHARED_POINTERS(AngFileHelper)
    MXA_STATIC_NEW_MACRO(AngFileHelper)
    virtual ~AngFileHelper();

    MXA_INSTANCE_PROPERTY(AngDirectoryPatterns::Pointer, DirectoryPattern);
    MXA_INSTANCE_PROPERTY(int, ZIndexStart)
    MXA_INSTANCE_PROPERTY(int, ZIndexEnd)
    MXA_INSTANCE_PROPERTY(bool, Cancel);

    void loadData(Voxel voxels[], int xpoints, int ypoints, int zpoints, int slice);

  protected:
    AngFileHelper();
  private:
    AngFileHelper(const AngFileHelper&);    // Copy Constructor Not Implemented
      void operator=(const AngFileHelper&);  // Operator '=' Not Implemented

};

#endif /* ANGFILEHELPER_H_ */
