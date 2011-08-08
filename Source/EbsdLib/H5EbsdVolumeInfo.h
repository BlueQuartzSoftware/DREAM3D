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

#ifndef H5EBSDVOLUMEREADER_H_
#define H5EBSDVOLUMEREADER_H_

#include <string>

#include "EbsdLib/EbsdLibTypes.h"
#include "EbsdLib/EbsdSetGetMacros.h"

/**
 * @class H5EbsdVolumeInfo H5EbsdVolumeInfo.h EbsdLib/H5EbsdVolumeInfo.h
 * @brief This is a higher level class that only reads the volume header information
 * from the .h5ebsd file. The actual loading of the data is left to subclasses.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Aug 5, 2011
 * @version $Revision$
 */
class  EbsdLib_EXPORT H5EbsdVolumeInfo
{
  public:
    EBSD_SHARED_POINTERS(H5EbsdVolumeInfo)
    EBSD_TYPE_MACRO(H5EbsdVolumeInfo)
    EBSD_STATIC_NEW_MACRO(H5EbsdVolumeInfo)

    virtual ~H5EbsdVolumeInfo();

    EBSD_INSTANCE_STRING_PROPERTY(Filename);


    virtual int readVolumeInfo();

    virtual void invalidateCache();

    /**
     * @brief This method gathers the number of points in each of the 3 axis directions
     * and also the resolution in each of the axis directions where each of the
     * arguments are "output" parameters, ie, they will be over written with the
     * values from the data source.
     * @param xDim The number of points along the X Axis
     * @param yDim The number of points along the Y Axis
     * @param zDim The number of points along the Z Axis
     * @param xRes The resolution along the X Axis
     * @param yRes The resolution along the Y Axis
     * @param zRes The resolution along the Z Axis
     */
    virtual int getDimsAndResolution(int &xDim, int &yDim, int &zDim,
                                     float &xRes, float &yRes, float &zRes);

    virtual int getDims(int &xDim, int &yDim, int &zDim);
    virtual int getResolution(float &xRes, float &yRes, float &zRes);

    virtual std::string getManufacturer();

    virtual int getNumSlices();
    virtual int getZStart();
    virtual int getZEnd();

    virtual int getNumPhases();

  protected:
    H5EbsdVolumeInfo();

  private:
    bool m_ValuesAreCached;
    int m_XDim;
    int m_YDim;
    int m_ZDim;
    float m_XRes;
    float m_YRes;
    float m_ZRes;
    int m_ZStart;
    int m_ZEnd;
    std::string m_Manufacturer;
    int m_NumPhases;



    H5EbsdVolumeInfo(const H5EbsdVolumeInfo&); //Not Implemented
    void operator=(const H5EbsdVolumeInfo&); //Not Implemented

};

#endif /* H5EBSDVOLUMEREADER_H_ */
