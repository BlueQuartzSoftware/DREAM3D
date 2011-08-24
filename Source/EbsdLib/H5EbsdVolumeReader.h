/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef _H5EBSDVOLUMEREADER_H_
#define _H5EBSDVOLUMEREADER_H_

#include <vector>


#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdLibConfiguration.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/H5EbsdVolumeInfo.h"

#include "EbsdLib/QualityMetricFilter.h"


/**
 * @class AbstractAngDataLoader AbstractAngDataLoader.h EbsdLib/TSL/AbstractAngDataLoader.h
 * @brief  This class defines the C++ interface that subclasses must implement
 * in order to be able to load OIM data into the DREAM.3D programs.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date May 23, 2011
 * @version 1.0
 */
class EbsdLib_EXPORT H5EbsdVolumeReader : public H5EbsdVolumeInfo
{

  public:
    EBSD_SHARED_POINTERS(H5EbsdVolumeReader)
    EBSD_TYPE_MACRO(H5EbsdVolumeReader)
    EBSD_STATIC_NEW_MACRO(H5EbsdVolumeReader)

    virtual ~H5EbsdVolumeReader();

    EBSD_INSTANCE_PROPERTY(bool, Cancel)

    EBSD_INSTANCE_PROPERTY(Ebsd::RefFrameOrigin, RefFrameOrigin);
    EBSD_INSTANCE_PROPERTY(Ebsd::RefFrameZDir, RefFrameZDir);
	EBSD_INSTANCE_PROPERTY(bool, AxesFlipped);

    /**
     * @brief This is the actual starting slice that the user wants to start
     * from which may be different from the "ZStartIndex" which is saved in the
     * HDF5 file.
     */
    EBSD_INSTANCE_PROPERTY(int, SliceStart);

    /**
     * @brief This is the actual ending slice that the user wants to start
     * from which may be different from the "ZEndIndex" which is saved in the
     * HDF5 file.
     */
    EBSD_INSTANCE_PROPERTY(int, SliceEnd);

    /**
     * @brief This method does the actual loading of the OIM data from the data
     * source (files, streams, etc) into the data structures. Subclasses need to
     * fully implement this. This is a skeleton method that simply returns an error.
     * @param euler1s The first set of euler angles (phi1)
     * @param euler2s The second set of euler angles (Phi)
     * @param euler3s The third set of euler angles (phi2)
     * @param phases The phase index data
     * @param goodVoxels The boolean array of good voxels
     * @param xpoints The number of x voxels
     * @param ypoints The number of y voxels
     * @param zpoints The number of z voxels
     * @param filters The Quality Metric Filters
     * @return
     */
    virtual int loadData(float* euler1s, float* euler2s, float* euler3s,
                         int* phases, bool* goodVoxels,
                         int xpoints, int ypoints, int zpoints,
                         std::vector<QualityMetricFilter::Pointer> filters);

    /**
     *
     * @param filters
     * @param dataPointers
     * @param nPoints
     * @param dTypes
     * @return
     */
    virtual AIMArray<bool>::Pointer determineGoodVoxels( std::vector<QualityMetricFilter::Pointer> filters,
                                    std::vector<void*> dataPointers,
                                    size_t nPoints,
                                    std::vector<Ebsd::NumType> dTypes);

  protected:
    H5EbsdVolumeReader();

  private:
    H5EbsdVolumeReader(const H5EbsdVolumeReader&); // Copy Constructor Not Implemented
    void operator=(const H5EbsdVolumeReader&); // Operator '=' Not Implemented

};



#endif /* _H5EBSDVOLUMEREADER_H_  */
