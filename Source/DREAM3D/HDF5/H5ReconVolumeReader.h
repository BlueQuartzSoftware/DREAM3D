/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
#ifndef H5RECONVOLUMEREADER_H_
#define H5RECONVOLUMEREADER_H_

#include "hdf5.h"

#include <vector>
#include <string>

#include <boost/shared_array.hpp>

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"


#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Reconstruction/ReconstructionVoxel.h"


/*
 *
 */
class H5ReconVolumeReader
{
  public:
    MXA_SHARED_POINTERS(H5ReconVolumeReader)
    MXA_TYPE_MACRO(H5ReconVolumeReader)
    MXA_STATIC_NEW_MACRO(H5ReconVolumeReader)

    virtual ~H5ReconVolumeReader();

    MXA_INSTANCE_STRING_PROPERTY(Filename);

    int getSizeAndResolution(int volDims[3], float spacing[3]);

    int readVoxelData(boost::shared_array<ReconstructionVoxel> voxels,
                      std::vector<AIM::Reconstruction::CrystalStructure> &crystruct,
                      int totalpoints);

  protected:
    H5ReconVolumeReader();

  private:
    H5ReconVolumeReader(const H5ReconVolumeReader&); // Copy Constructor Not Implemented
    void operator=(const H5ReconVolumeReader&); // Operator '=' Not Implemented
};

#endif /* H5RECONVOLUMEREADER_H_ */
