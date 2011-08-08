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

#ifndef _AbstractAngReader_h_
#define _AbstractAngReader_h_

#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdLibConfiguration.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/H5EbsdVolumeInfo.h"



class ReconstructionFunc;

/**
 * @class AbstractAngDataLoader AbstractAngDataLoader.h AIM/ANG/AbstractAngDataLoader.h
 * @brief  This class defines the C++ interface that subclasses must implement
 * in order to be able to load OIM data into the DREAM.3D programs.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date May 23, 2011
 * @version 1.0
 */
class H5EbsdVolumeReader : public H5EbsdVolumeInfo
{

  public:
    EBSD_SHARED_POINTERS(H5EbsdVolumeReader)
    EBSD_TYPE_MACRO(H5EbsdVolumeReader)
    EBSD_STATIC_NEW_MACRO(H5EbsdVolumeReader)

    virtual ~H5EbsdVolumeReader();

    EBSD_INSTANCE_PROPERTY(bool, Cancel)

    EBSD_INSTANCE_PROPERTY(Ebsd::Orientation, Orientation)
    EBSD_INSTANCE_PROPERTY(int, SliceStart);
    EBSD_INSTANCE_PROPERTY(int, SliceEnd);


    /**
     * @brief This method does the actual loading of the OIM data from the data
     * source (files, streams, etc) into the Reconstruction Module data structures.
     * @param m
     */
    virtual int loadData(ReconstructionFunc* m);


  protected:
    H5EbsdVolumeReader();

  private:
    H5EbsdVolumeReader(const H5EbsdVolumeReader&); // Copy Constructor Not Implemented
    void operator=(const H5EbsdVolumeReader&); // Operator '=' Not Implemented

};



#endif /* _AbstractAngReader_h_  */
