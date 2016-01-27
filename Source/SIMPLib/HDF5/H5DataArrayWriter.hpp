/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#ifndef _H5DataArrayWriter_H_
#define _H5DataArrayWriter_H_

#include <hdf5.h>

#include <QtCore/QString>

#include "H5Support/QH5Lite.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
//#include "SIMPLib/DataArrays/DataArray.hpp"


/**
 * @class H5DataArrayWriter H5DataArrayWriter.h DREAM3DLib/HDF5/H5DataArrayWriter.h
 * @brief This class handles writing of DataArray<T> objects to an HDF5 file
 *
 * @date Jan 22, 2012
 * @version 1.0
 */
class H5DataArrayWriter
{
  public:
    virtual ~H5DataArrayWriter() {}

    /**
     * @brief writeDataArrayAttributes
     * @param gid
     * @param dataArray
     * @param tDims
     * @param cDims
     * @return
     */
    template<typename IDataArrayType>
    static int writeDataArrayAttributes(hid_t gid, IDataArrayType* dataArray, QVector<size_t> tDims, QVector<size_t> cDims)
    {
      int err = QH5Lite::writeScalarAttribute(gid, dataArray->getName(), DREAM3D::HDF5::DataArrayVersion, dataArray->getClassVersion());
      if(err < 0)
      {
        return -604;
      }
      err = QH5Lite::writeStringAttribute(gid, dataArray->getName(), DREAM3D::HDF5::ObjectType, dataArray->getFullNameOfClass());
      if(err < 0)
      {
        return -607;
      }


      // Write the tuple dimensions as an attribute
      hsize_t size = tDims.size();
      err = QH5Lite::writePointerAttribute(gid, dataArray->getName(), DREAM3D::HDF5::TupleDimensions, 1, &size, tDims.data());
      if (err < 0)
      {
        return -609;
      }

      // write the component dimensions as  an attribute
      size = cDims.size();
      err = QH5Lite::writePointerAttribute(gid, dataArray->getName(), DREAM3D::HDF5::ComponentDimensions, 1, &size, cDims.data());
      if (err < 0)
      {
        return -610;
      }

      QString str = QString("x=%1").arg(tDims[0]);
      if(tDims.size() > 1) { str = str + QString(",y=%1").arg(tDims[1]); }
      if(tDims.size() > 2) { str = str + QString(",z=%1").arg(tDims[2]); }
      err = QH5Lite::writeStringAttribute(gid, dataArray->getName(), DREAM3D::HDF5::AxisDimensions, str);
      if (err < 0)
      {
        return -611;
      }
      return err;
    }

    /**
     * @brief writeDataArray
     * @param gid
     * @param dataArray
     * @param tDims
     * @return
     */
    template<class T>
    static int writeDataArray(hid_t gid, T* dataArray, QVector<size_t> tDims)
    {
      int err = 0;

      QVector<size_t> cDims = dataArray->getComponentDimensions();
      hsize_t h5Rank = tDims.size() + cDims.size();

      QVector<hsize_t> h5Dims(tDims.size() + cDims.size());

#if 1
      /*** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      *  VERY IMPORTANT. MAKE SURE YOU UNDERSTAND THIS NEXT SECTION FULLY BEFORE CHANGING ANYTHING.
      * HDF5 wants the dimensions written from slowest to fastest, so "C" style or ZYX style. DREAM3D stores
      * the dimensions in the opposite order (XYZ) (even though the data is laid out in memory correctly). So using the
      * Small IN100 data set as an example the NCOLS(189) NROWS(201) and NSLICES(117) are stored in the tDims vector as
      * [189,201,117] BUT we need to reverse the order so that HDF5 can properly interpret the arrays and also properly
      * load data into the HDFView program. These next few loops will reverse the order in addition to calculating the
      * total number of elements in the array
      */
      qint32 count = tDims.size() - 1;
      for (int i = count; i >= 0; i--)
      {
        //     std::cout << "  " << tDims[i] << std::endl;
        h5Dims[count - i] = tDims[i];
      }
      //  std::cout << "  Comp Dims: " << std::endl;
      count = cDims.size() - 1;
      for (int i = count; i >= 0; i--)
      {
        //    std::cout << "  " << cDims[i] << std::endl;
        h5Dims[count - i + tDims.size()] = cDims[i];
      }

#else
      //  std::cout << dataArray->getName().toStdString() << " Tuple Dims " << std::endl;
      for (int i = 0; i < tDims.size(); i++)
      {
        //    std::cout << i <<  "  " << tDims[i] << std::endl;
        h5Dims[i] = tDims[i];
      }
      //  std::cout << "  Comp Dims: " << std::endl;
      for(int i = 0; i < cDims.size(); i++)
      {
        //   std::cout << (i+tDims.size()) << "  " << cDims[i] << std::endl;
        h5Dims[i + tDims.size()] = cDims[i];
      }
#endif
      if (QH5Lite::datasetExists(gid, dataArray->getName()) == false)
      {
        err = QH5Lite::writePointerDataset(gid, dataArray->getName(), h5Rank, h5Dims.data(), dataArray->getPointer(0));
        if(err < 0)
        {
          return err;
        }
      }
      else
      {
        err = QH5Lite::replacePointerDataset(gid, dataArray->getName(), h5Rank, h5Dims.data(), dataArray->getPointer(0));
        if(err < 0)
        {
          return err;
        }
      }

      err = writeDataArrayAttributes<T>(gid, dataArray, tDims, cDims);

      return err;
    }


    /**
     * @brief writeDataArray
     * @param gid
     * @param dataArray
     * @param tDims
     * @return
     */
    template<class T>
    static int writeStringDataArray(hid_t gid, T* dataArray)
    {
      int err = 0;

      std::vector<std::string> data(dataArray->getNumberOfTuples());
      for(int i = 0; i < data.size(); i++)
      {
        data[i] = dataArray->getValue(i).toStdString();
      }

      err = H5Lite::writeVectorOfStringsDataset(gid, dataArray->getName().toStdString(), data);
      QVector<size_t> tDims(1, dataArray->getNumberOfTuples());
      QVector<size_t> cDims(1, 1);
      err = writeDataArrayAttributes<T>(gid, dataArray, tDims, cDims);

      return err;
    }


  protected:
    H5DataArrayWriter() {}

  private:
    H5DataArrayWriter(const H5DataArrayWriter&); // Copy Constructor Not Implemented
    void operator=(const H5DataArrayWriter&); // Operator '=' Not Implemented
};




/**
 * @class H5DataArrayWriter H5DataArrayWriter.h DREAM3DLib/HDF5/H5DataArrayWriter.h
 * @brief This class handles writing of DataArray<T> objects to an HDF5 file
 *
 * @date Jan 22, 2012
 * @version 1.0
 */
template<typename T>
class H5GBCDArrayWriter
{
  public:
    virtual ~H5GBCDArrayWriter() {}

    static int writeArray(hid_t gid, const QString& name, size_t* gbcdDims, T* data, const QString& className)
    {
      int32_t rank = 5;
      hsize_t dims[5] = {gbcdDims[0], gbcdDims[1], gbcdDims[2], gbcdDims[3], gbcdDims[4],};
      int err = 0;
      if (QH5Lite::datasetExists(gid, name) == false)
      {
        err = QH5Lite::writePointerDataset(gid, name, rank, dims, data);
        if(err < 0)
        {
          return err;
        }
      }

      err = QH5Lite::writeStringAttribute(gid, name, DREAM3D::HDF5::ObjectType, className);
      if(err < 0)
      {
        return err;
      }
      return err;
    }


  protected:
    H5GBCDArrayWriter() {}

  private:
    H5GBCDArrayWriter(const H5GBCDArrayWriter&); // Copy Constructor Not Implemented
    void operator=(const H5GBCDArrayWriter&); // Operator '=' Not Implemented
};


#endif /* _H5DataArrayWriter_H_ */

