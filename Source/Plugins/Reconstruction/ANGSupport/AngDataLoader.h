/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
#ifndef ANGFILEHELPER_H_
#define ANGFILEHELPER_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include "MXA/Common/MXASetGetMacros.h"

#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/TSL/AngDirectoryPatterns.h"

#include "DREAM3D/DREAM3DConfiguration.h"
#include "AbstractAngDataLoader.h"
#include "Reconstruction/ReconstructionFunc.h"


/**
 * @class AngDataLoader AngDataLoader.h AIM/ANG/AngDataLoader.h
 * @brief This class loads OIM data from TSL .ang files.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date May 23, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT AngDataLoader : public AbstractAngDataLoader
{
  public:
    MXA_SHARED_POINTERS(AngDataLoader)
    MXA_STATIC_NEW_SUPERCLASS(AbstractAngDataLoader, AngDataLoader)

    virtual ~AngDataLoader();

    MXA_INSTANCE_PROPERTY(AngDirectoryPatterns::Pointer, DirectoryPattern);
    MXA_INSTANCE_PROPERTY(int, ZStartIndex)
    MXA_INSTANCE_PROPERTY(int, ZEndIndex)
    MXA_INSTANCE_PROPERTY(float, ZResolution)
    MXA_INSTANCE_PROPERTY(Ebsd::Ang::Orientation, Orientation)
    MXA_INSTANCE_PROPERTY(bool, Cancel);

    int getSizeAndResolution(int &xpoints, int &ypoints, int &zpoints,
                                       float &xres, float &yres, float &zres);

    int loadData(ReconstructionFunc* m);

  protected:
    AngDataLoader();
  private:
    AngDataLoader(const AngDataLoader&);    // Copy Constructor Not Implemented
    void operator=(const AngDataLoader&);  // Operator '=' Not Implemented

};

#endif /* ANGFILEHELPER_H_ */
