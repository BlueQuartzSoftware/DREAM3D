/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef _H5DataArrayWriter_H_
#define _H5DataArrayWriter_H_

#include <hdf5.h>

#include <string>

#include "H5Support/H5Lite.h"
//#include "DREAM3DLib/HDF5/VTKH5Constants.h"
#include "DREAM3DLib/Common/Constants.h"


/**
 * @class H5DataArrayWriter H5DataArrayWriter.h DREAM3DLib/HDF5/H5DataArrayWriter.h
 * @brief This class handles writing of DataArray<T> objects to an HDF5 file
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 22, 2012
 * @version 1.0
 */
template<typename T>
class H5DataArrayWriter
{
  public:
    virtual ~H5DataArrayWriter() {}

    static int writeArray(hid_t gid, const std::string& name, size_t numTuples, int numComp, T* data, const std::string& className)
    {
      int32_t rank = 0;
      if(numComp == 1)
      {
        rank = 1;
      }
      else
      {
        rank = 2;
      }

      hsize_t dims[2] =
      { numTuples, numComp };
      int err = 0;
      if (H5Lite::datasetExists(gid, name) == false)
      {
        err = H5Lite::writePointerDataset(gid, name, rank, dims, data);
        if(err < 0)
        {
          return err;
        }
      }
      err = H5Lite::writeScalarAttribute(gid, name, DREAM3D::HDF5::NumComponents, numComp);
      if(err < 0)
      {
        return err;
      }

      err = H5Lite::writeStringAttribute(gid, name, DREAM3D::HDF5::ObjectType, className);
      if(err < 0)
      {
        return err;
      }
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
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 22, 2012
 * @version 1.0
 */
template<typename T>
class H5GBCDArrayWriter
{
  public:
    virtual ~H5GBCDArrayWriter() {}

    static int writeArray(hid_t gid, const std::string& name, size_t* gbcdDims, T* data, const std::string& className)
    {
      int32_t rank = 5;
      hsize_t dims[5] = {gbcdDims[0], gbcdDims[1], gbcdDims[2], gbcdDims[3], gbcdDims[4],};
      int err = 0;
      if (H5Lite::datasetExists(gid, name) == false)
      {
        err = H5Lite::writePointerDataset(gid, name, rank, dims, data);
        if(err < 0)
        {
          return err;
        }
      }

      err = H5Lite::writeStringAttribute(gid, name, DREAM3D::HDF5::ObjectType, className);
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
