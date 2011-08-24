/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef H5CTFVOLUMEREADER_H_
#define H5CTFVOLUMEREADER_H_

//-- STL Includes
#include <vector>

//-- EbsdLib Includes
#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdLibConfiguration.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/H5EbsdVolumeReader.h"
#include "EbsdLib/QualityMetricFilter.h"

#include "EbsdLib/HKL/CtfPhase.h"


/**
 * @class H5CtfVolumeReader H5CtfVolumeReader.h Reconstruction/EbsdSupport/H5CtfVolumeReader.h
 * @brief This class loads EBSD data from an HDF5 based file.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date May 23, 2011
 * @version 1.0
 */
class EbsdLib_EXPORT H5CtfVolumeReader : public H5EbsdVolumeReader
{
  public:
    EBSD_SHARED_POINTERS(H5CtfVolumeReader)
    EBSD_STATIC_NEW_SUPERCLASS(H5EbsdVolumeReader, H5CtfVolumeReader)

    virtual ~H5CtfVolumeReader();

    /**
     * @brief This method does the actual loading of the OIM data from the data
     * source (files, streams, etc) into the data structures.
     * @param euler1s
     * @param euler2s
     * @param euler3s
     * @param phases
     * @param goodVoxels
     * @param xpoints
     * @param ypoints
     * @param zpoints
     * @param filters
     * @return
     */
    int loadData(float* euler1s, float* euler2s, float* euler3s,
                 int* phases, bool* goodVoxels,
                 int xpoints, int ypoints, int zpoints, Ebsd::RefFrameZDir ZDir,
                 std::vector<QualityMetricFilter::Pointer> filters);

    /**
     * @brief
     * @return
     */
    std::vector<CtfPhase::Pointer> getPhases();

  protected:
    H5CtfVolumeReader();

  private:
    std::vector<CtfPhase::Pointer> m_Phases;

    H5CtfVolumeReader(const H5CtfVolumeReader&); // Copy Constructor Not Implemented
    void operator=(const H5CtfVolumeReader&); // Operator '=' Not Implemented
};

#endif /* H5CTFVOLUMEREADER_H_ */
