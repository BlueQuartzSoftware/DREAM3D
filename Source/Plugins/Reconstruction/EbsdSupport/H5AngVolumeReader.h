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
#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/TSL/AngDirectoryPatterns.h"
#include "EbsdLib/TSL/AngPhase.h"

//-- AIM Common Includes
#include "DREAM3D/DREAM3DConfiguration.h"

#include "Reconstruction/EbsdSupport/H5EbsdVolumeReader.h"
#include "Reconstruction/ReconstructionFunc.h"
#include "Reconstruction/QualityMetricFilter.h"


/**
 * @class H5AngVolumeReader H5AngVolumeReader.h Reconstruction/EbsdSupport/H5AngVolumeReader.h
 * @brief This class loads OIM data from an HDF5 based file.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date May 23, 2011
 * @version 1.0
 */
class H5AngVolumeReader : public H5EbsdVolumeReader
{
  public:
    MXA_SHARED_POINTERS(H5AngVolumeReader)
    MXA_STATIC_NEW_SUPERCLASS(H5EbsdVolumeReader, H5AngVolumeReader)

    virtual ~H5AngVolumeReader();

    int loadData(ReconstructionFunc* m, std::vector<QualityMetricFilter::Pointer> filters);

    std::vector<AngPhase::Pointer> getPhases();

  protected:
    H5AngVolumeReader();

  private:
    std::vector<AngPhase::Pointer> m_Phases;

    H5AngVolumeReader(const H5AngVolumeReader&);    // Copy Constructor Not Implemented
    void operator=(const H5AngVolumeReader&);  // Operator '=' Not Implemented

};

#endif /* _H5ANGDATALOADER_H_ */
