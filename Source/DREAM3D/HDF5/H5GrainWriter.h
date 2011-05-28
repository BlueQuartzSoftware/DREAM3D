/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Michael A. Groeber (US Air Force Research Laboratory)
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

#ifndef _H5GRAINWRITER_H_
#define _H5GRAINWRITER_H_

#include "MXA/Common/MXASetGetMacros.h"

#include "AIM/Common/AIMCommonConfiguration.h"

class ReconstructionFunc;
class GrainGeneratorFunc;

/**
 * @class H5GrainWriter H5GrainWriter.h AIM/Common/HDF5/H5GrainWriter.h
 * @brief This class will write a .h5grain file from data provided by the
 * ReconstructionFunc class
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 19, 2011
 * @version 1.0
 */
class AIMCOMMON_EXPORT H5GrainWriter
{
  public:
    MXA_SHARED_POINTERS(H5GrainWriter)
    MXA_STATIC_NEW_MACRO(H5GrainWriter)
    MXA_TYPE_MACRO(H5GrainWriter)

    virtual ~H5GrainWriter();

    /**
     * @brief
     * @param hdfFile
     * @return
     */
    int writeHDF5GrainsFile(ReconstructionFunc* r, const std::string &hdfFile);

    /**
     * @brief
     * @param hdfFile
     * @return
     */
    int writeHDF5GrainsFile(GrainGeneratorFunc* r, const std::string &hdfFile);

  protected:
    H5GrainWriter();

  private:
    H5GrainWriter(const H5GrainWriter&); // Copy Constructor Not Implemented
    void operator=(const H5GrainWriter&); // Operator '=' Not Implemented
};

#endif /* _H5GRAINWRITER_H_ */
