/*
 * AngFileHelper.h
 *
 *  Created on: Oct 21, 2009
 *      Author: mjackson
 */

#ifndef ANGFILEHELPER_H_
#define ANGFILEHELPER_H_

#include <MXA/Common/MXASetGetMacros.h>
#include <AIM/Common/Voxel.h>
#include <AIM/ANG/AngDirectoryPatterns.h>

/*
 *
 */
class AngFileHelper
{
  public:
    MXA_SHARED_POINTERS(AngFileHelper)
    MXA_STATIC_NEW_MACRO(AngFileHelper)
    virtual ~AngFileHelper();

    MXA_INSTANCE_PROPERTY_m(AngDirectoryPatterns::Pointer, DirectoryPattern);
    MXA_INSTANCE_PROPERTY_m(int, ZIndexStart)
    MXA_INSTANCE_PROPERTY_m(int, ZIndexEnd)

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
