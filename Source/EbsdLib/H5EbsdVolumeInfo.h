/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef _h5ebsdvolumeinfo_h_
#define _h5ebsdvolumeinfo_h_

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QSet>

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdConstants.h"

/**
 * @class H5EbsdVolumeInfo H5EbsdVolumeInfo.h EbsdLib/H5EbsdVolumeInfo.h
 * @brief This is a higher level class that only reads the volume header information
 * from the .h5ebsd file. The actual loading of the data is left to subclasses.
 *
 * @date Aug 5, 2011
 * @version 1.0
 */
class  EbsdLib_EXPORT H5EbsdVolumeInfo
{
  public:
    EBSD_SHARED_POINTERS(H5EbsdVolumeInfo)
    EBSD_TYPE_MACRO(H5EbsdVolumeInfo)
    EBSD_STATIC_NEW_MACRO(H5EbsdVolumeInfo)

    virtual ~H5EbsdVolumeInfo();


    /**
     * @brief These get filled out if there are errors. Negative values are error codes
     */
    EBSD_INSTANCE_PROPERTY(int, ErrorCode)

    EBSD_INSTANCE_STRING_PROPERTY(ErrorMessage)


    EBSD_INSTANCE_STRING_PROPERTY(FileName)

    /**
     * @brief Reads all the volume header values
     * @return Error condition if any of the reads fail
     */
    virtual int readVolumeInfo();

    /**
     * @brief Marks the cached values as being invalid which will force a full
     * read of the volume information the next time one of the cache values is
     * requested.
     */
    virtual void invalidateCache();

    /**
     * @brief Returns the file version of the H5Ebsd file. This is an attribute attached
     * to the root "/" data set.
     * @return
     */
    virtual uint32_t getFileVersion();

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
     * @return Error condition
     */
    virtual int getDimsAndResolution(int64_t& xDim, int64_t& yDim, int64_t& zDim,
                                     float& xRes, float& yRes, float& zRes);

    /**
     * @brief This method gathers the number of points in each of the 3 axis directions
     * where each of the
     * arguments are "output" parameters, ie, they will be over written with the
     * values from the data source.
     * @param xDim The number of points along the X Axis
     * @param yDim The number of points along the Y Axis
     * @param zDim The number of points along the Z Axis
     * @return Error condition
     */
    virtual int getDims(int64_t& xDim, int64_t& yDim, int64_t& zDim);

    /**
     * @brief This method gathers the resolution in each of the axis directions where each of the
     * arguments are "output" parameters, ie, they will be over written with the
     * values from the data source.
     * @param xRes The resolution along the X Axis
     * @param yRes The resolution along the Y Axis
     * @param zRes The resolution along the Z Axis
     * @return Error condition
     */
    virtual int getResolution(float& xRes, float& yRes, float& zRes);

    /**
     * @brief Returns the Manufacturer of the EBSD system
     * @return The Manufacturer string
     */
    virtual QString getManufacturer();

    /**
     * @brief Convenience method that returns the number of Z Slices of data
     */
    virtual int getNumSlices();

    /**
     * @brief Returns the index of the first slice of EBSD data
     */
    virtual int getZStart();

    /**
     * @brief Returns the index of the last slice of EBSD data.
     */
    virtual int getZEnd();

    /**
     * @brief Returns the number of phases contained in the EBSD data set.
     */
    virtual int getNumPhases();

    virtual uint32_t getStackingOrder();

    virtual float getSampleTransformationAngle();
    virtual QVector<float> getSampleTransformationAxis();
    virtual float getEulerTransformationAngle();
    virtual QVector<float> getEulerTransformationAxis();

    /**
     * @brief updateToLatestVersion This will apply various attribute and data set updates to bring the
     * file up to current API standards
     */
    virtual int updateToLatestVersion();

    /**
     * @brief getDataArrayNames
     * @return
     */
    virtual QSet<QString> getDataArrayNames();

    //virtual QSet<QString> readMetaDataArrayNames();


  protected:
    H5EbsdVolumeInfo();

  private:
    bool m_ValuesAreCached;
    uint32_t m_FileVersion;
    int m_XDim;
    int m_YDim;
    int m_ZDim;
    float m_XRes;
    float m_YRes;
    float m_ZRes;
    int m_ZStart;
    int m_ZEnd;
    uint32_t m_StackingOrder;
    int m_NumPhases;
    float m_SampleTransformationAngle;
    QVector<float> m_SampleTransformationAxis;
    float m_EulerTransformationAngle;
    QVector<float> m_EulerTransformationAxis;

    QSet<QString> m_DataArrayNames;
//    QSet<QString> m_MetaDataArrayNames;

    QString m_Manufacturer;

    H5EbsdVolumeInfo(const H5EbsdVolumeInfo&); //Not Implemented
    void operator=(const H5EbsdVolumeInfo&); //Not Implemented

};

#endif /* H5EBSDVOLUMEREADER_H_ */

