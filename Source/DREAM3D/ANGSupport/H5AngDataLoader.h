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

#ifndef _H5ANGDATALOADER_H_
#define _H5ANGDATALOADER_H_

#include <vector>

#include "MXA/Common/MXASetGetMacros.h"

//-- TSL Ang Reader Includes
#include "TSLLib/AngConstants.h"
#include "TSLLib/AngDirectoryPatterns.h"
#include "TSLLib/AngPhase.h"

//-- AIM Common Includes
#include "DREAM3D/ANGSupport/AbstractAngDataLoader.h"
#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Reconstruction/ReconstructionVoxel.h"



/**
 * @class H5AngDataLoader H5AngDataLoader.h AIM/ANG/H5AngDataLoader.h
 * @brief This class loads OIM data from an HDF5 based file.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date May 23, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT H5AngDataLoader : public AbstractAngDataLoader
{
  public:
    MXA_SHARED_POINTERS(H5AngDataLoader)
    MXA_STATIC_NEW_SUPERCLASS(AbstractAngDataLoader, H5AngDataLoader)

    virtual ~H5AngDataLoader();

    MXA_INSTANCE_STRING_PROPERTY(Filename)
    MXA_INSTANCE_PROPERTY(int, ZStartIndex)
    MXA_INSTANCE_PROPERTY(int, ZEndIndex)
    MXA_INSTANCE_PROPERTY(bool, Cancel)
    MXA_INSTANCE_PROPERTY(Ang::Orientation, Orientation)

    int loadData(ReconstructionVoxel voxels[], int xpoints, int ypoints, int zpoints);
    int getSizeAndResolution(int &xpoints, int &ypoints, int &zpoints,
                             float &xres, float &yres, float &zres);

    int readZHeader(int &zStart, int &zEnd, float &zRes);
    std::vector<AngPhase::Pointer> getPhases();

  protected:
    H5AngDataLoader();
  private:
    std::vector<AngPhase::Pointer> m_Phases;

    H5AngDataLoader(const H5AngDataLoader&);    // Copy Constructor Not Implemented
    void operator=(const H5AngDataLoader&);  // Operator '=' Not Implemented

};

#endif /* _H5ANGDATALOADER_H_ */
