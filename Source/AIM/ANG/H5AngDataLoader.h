/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _H5ANGDATALOADER_H_
#define _H5ANGDATALOADER_H_

#include <MXA/Common/MXASetGetMacros.h>
#include "AIM/ANG/AbstractAngDataLoader.h"
#include "AIM/Common/AIMCommonConfiguration.h"
#include <AIM/Common/Voxel.h>
#include <AIM/ANG/AngDirectoryPatterns.h>

class AIMCOMMON_EXPORT H5AngDataLoader : public AbstractAngDataLoader
{
  public:
    MXA_SHARED_POINTERS(H5AngDataLoader)
    MXA_STATIC_NEW_SUPERCLASS(AbstractAngDataLoader, H5AngDataLoader)

    virtual ~H5AngDataLoader();

    MXA_INSTANCE_STRING_PROPERTY(Filename)
    MXA_INSTANCE_PROPERTY(int, ZStartIndex)
    MXA_INSTANCE_PROPERTY(int, ZEndIndex)
    MXA_INSTANCE_PROPERTY(bool, Cancel);

    int loadData(Voxel voxels[], int xpoints, int ypoints, int zpoints);
    int getSizeAndResolution(int &xpoints, int &ypoints, int &zpoints,
                                        double &xres, double &yres, double &zres);

  protected:
    H5AngDataLoader();
  private:
    H5AngDataLoader(const H5AngDataLoader&);    // Copy Constructor Not Implemented
    void operator=(const H5AngDataLoader&);  // Operator '=' Not Implemented

};

#endif /* _H5ANGDATALOADER_H_ */
