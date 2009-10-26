///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#ifndef ANGFILEHELPER_H_
#define ANGFILEHELPER_H_

#include <MXA/Common/MXASetGetMacros.h>
#include <AIM/Common/Voxel.h>
#include <AIM/ANG/AngDirectoryPatterns.h>


class AngFileHelper
{
  public:
    MXA_SHARED_POINTERS(AngFileHelper)
    MXA_STATIC_NEW_MACRO(AngFileHelper)
    virtual ~AngFileHelper();

    MXA_INSTANCE_PROPERTY_m(AngDirectoryPatterns::Pointer, DirectoryPattern);
    MXA_INSTANCE_PROPERTY_m(int, ZIndexStart)
    MXA_INSTANCE_PROPERTY_m(int, ZIndexEnd)
    MXA_INSTANCE_PROPERTY_m(bool, Cancel);

    void loadData(Voxel voxels[],
                  size_t xpoints, size_t ypoints, size_t zpoints,
                  double resz);

  protected:
    AngFileHelper();
  private:
    AngFileHelper(const AngFileHelper&);    // Copy Constructor Not Implemented
      void operator=(const AngFileHelper&);  // Operator '=' Not Implemented

};

#endif /* ANGFILEHELPER_H_ */
