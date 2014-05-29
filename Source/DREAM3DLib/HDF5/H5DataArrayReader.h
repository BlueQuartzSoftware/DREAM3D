/* ============================================================================
 * Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2014 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef H5DATAARRAYREADER_HPP_
#define H5DATAARRAYREADER_HPP_

#include <hdf5.h>

#include <QtCore/QString>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

/**
 * @class H5DataArrayReader H5DataArrayReader.h DREAM3DLib/HDF5/H5DataArrayReader.h
 * @brief This class handles reading DataArray<T> objects from an HDF5 file
 * @author Michael A. Jackson for BlueQuartz Software
 * @date May 27, 2014
 * @version 1.0
 */
class DREAM3DLib_EXPORT H5DataArrayReader
{
  public:
    virtual ~H5DataArrayReader();


    /**
     * @brief readRequiredAttributes Reads the required attributes from an HDF5 Data set
     * @param objType The type (subclass) of IDataArray that is stored in the HDF5 file
     * @param version The version of the DataArray class
     * @param tDims The Tuple Dimensions of the data array
     * @param cDims The Component Dimensions of the data array
     * @return
     */
    static int ReadRequiredAttributes(hid_t gid, const QString &name, QString &objType, int &version, QVector<size_t> &tDims, QVector<size_t> &cDims);

    /**
     * @brief ReadIDataArray Reads an IDataArray subclass from the HDF5 file
     * @param gid The HDF5 Group to read the data array from
     * @param name The name of the data set
     * @param metaDataOnly Read just the meta data about the DataArray or actually read all the data
     * @return
     */
    static IDataArray::Pointer ReadIDataArray(hid_t gid, const QString& name, bool metaDataOnly = false);

    /**
     * @brief ReadNeighborListData
     * @param gid The HDF5 Group to read the data array from
     * @param name The name of the data set
     * @param metaDataOnly Read just the meta data about the DataArray or actually read all the data
     * @return
     */
    static IDataArray::Pointer ReadNeighborListData(hid_t gid, const QString& name, bool metaDataOnly = false);

    /**
     * @brief readStringDataArray
     * @param gid The HDF5 Group to read the data array from
     * @param name The name of the data set
     * @param metaDataOnly Read just the meta data about the DataArray or actually read all the data
     * @return
     */
    static IDataArray::Pointer ReadStringDataArray(hid_t gid, const QString& name, bool metaDataOnly = false);


  protected:
    H5DataArrayReader();

  private:
    H5DataArrayReader(const H5DataArrayReader&); // Copy Constructor Not Implemented
    void operator=(const H5DataArrayReader&); // Operator '=' Not Implemented

};

#endif /* H5DATAARRAYREADER_HPP_ */

