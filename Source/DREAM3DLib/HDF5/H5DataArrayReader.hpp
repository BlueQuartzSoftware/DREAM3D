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

#ifndef H5DATAARRAYREADER_HPP_
#define H5DATAARRAYREADER_HPP_

#include <hdf5.h>

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/Common/NeighborList.hpp"

class H5DataArrayReader
{
  public:
    virtual ~H5DataArrayReader()
    {
    }

    /**
     * @brief Reads data  into an IDataArray::Pointer
     * @param locId The hdf5 object id of the parent
     * @param datasetPath The path to the data set containing the attributes you want
     * @param dims The dimensions of the attribute
     * @return Boost shared pointer to the data
     */
    template<typename T>
    static IDataArray::Pointer readH5Dataset(hid_t locId, const std::string &datasetPath, const std::vector<hsize_t> &dims)
    {
      herr_t err = -1;
      IDataArray::Pointer ptr;
      size_t numElements = 1;
      for (size_t i = 0; i < dims.size(); ++i)
      {
        numElements *= dims[i];
      }
      ptr = DataArray<T>::CreateArray(numElements);
      if(dims.size() > 1)
      {
        ptr->SetNumberOfComponents(static_cast<int>(dims[1]));
      }
      ptr->SetName(datasetPath);

      T* data = (T*)(ptr->GetVoidPointer(0));
      err = H5Lite::readPointerDataset(locId, datasetPath, data);
      if(err < 0)
      {
        std::cout << "readH5Data read error: " << __FILE__ << "(" << __LINE__ << ")" << std::endl;
        ptr = IDataArray::NullPointer();
      }
      return ptr;
    }

    /**
     *
     * @param gid
     * @param name
     * @return
     */
    static IDataArray::Pointer readIDataArray(hid_t gid, const std::string &name)
    {

      herr_t err = -1;
      herr_t retErr = 1;
      hid_t typeId = -1;
      H5T_class_t attr_type;
      size_t attr_size;
      std::string res;

      std::vector<hsize_t> dims; //Reusable for the loop
      IDataArray::Pointer ptr = IDataArray::NullPointer();
      //std::cout << "Reading Attribute " << *iter << std::endl;
      typeId = H5Lite::getDatasetType(gid, name);
      err = H5Lite::getDatasetInfo(gid, name, dims, attr_type, attr_size);
      if(err < 0)
      {
        std::cout << "Error in getAttributeInfo method in readUserMetaData." << std::endl;
      }

      else
      {
        switch(attr_type)
        {
          case H5T_STRING:
            res.clear(); //Clear the string out first
            err = H5Lite::readStringDataset(gid, name, res);
            //        if(err >= 0)
            //        {
            //          IDataArray::Pointer attr = MXAAsciiStringData::Create(res);
            //          attr->setName(name);
            //          attributes[*iter] = attr;
            //        }
            break;
          case H5T_INTEGER:
            //std::cout << "User Meta Data Type is Integer" << std::endl;
            if(H5Tequal(typeId, H5T_STD_U8BE) || H5Tequal(typeId, H5T_STD_U8LE))
            {
              ptr = H5DataArrayReader::readH5Dataset<uint8_t>(gid, name, dims);
            }
            else if(H5Tequal(typeId, H5T_STD_U16BE) || H5Tequal(typeId, H5T_STD_U16LE))
            {
              ptr = H5DataArrayReader::readH5Dataset<uint16_t>(gid, name, dims);
            }
            else if(H5Tequal(typeId, H5T_STD_U32BE) || H5Tequal(typeId, H5T_STD_U32LE))
            {
              ptr = H5DataArrayReader::readH5Dataset<uint32_t>(gid, name, dims);
            }
            else if(H5Tequal(typeId, H5T_STD_U64BE) || H5Tequal(typeId, H5T_STD_U64LE))
            {
              ptr = H5DataArrayReader::readH5Dataset<uint64_t>(gid, name, dims);
            }
            else if(H5Tequal(typeId, H5T_STD_I8BE) || H5Tequal(typeId, H5T_STD_I8LE))
            {
              ptr = H5DataArrayReader::readH5Dataset<int8_t>(gid, name, dims);
            }
            else if(H5Tequal(typeId, H5T_STD_I16BE) || H5Tequal(typeId, H5T_STD_I16LE))
            {
              ptr = H5DataArrayReader::readH5Dataset<int16_t>(gid, name, dims);
            }
            else if(H5Tequal(typeId, H5T_STD_I32BE) || H5Tequal(typeId, H5T_STD_I32LE))
            {
              ptr = H5DataArrayReader::readH5Dataset<int32_t>(gid, name, dims);
            }
            else if(H5Tequal(typeId, H5T_STD_I64BE) || H5Tequal(typeId, H5T_STD_I64LE))
            {
              ptr = H5DataArrayReader::readH5Dataset<int64_t>(gid, name, dims);
            }
            else
            {
              std::cout << "Unknown Type: " << typeId << " at " << name << std::endl;
              err = -1;
              retErr = -1;
            }
            break;
          case H5T_FLOAT:
            if(attr_size == 4)
            {
              ptr = H5DataArrayReader::readH5Dataset<float32>(gid, name, dims);
            }
            else if(attr_size == 8)
            {
              ptr = H5DataArrayReader::readH5Dataset<float64>(gid, name, dims);
            }
            else
            {
              std::cout << "Unknown Floating point type" << std::endl;
              err = -1;
              retErr = -1;
            }
            break;
          default:
            std::cout << "Error: readUserMetaData() Unknown attribute type: " << attr_type << std::endl;
            H5Utilities::printHDFClassType(attr_type);
        }

        CloseH5T(typeId, err, retErr);
        //Close the H5A type Id that was retrieved during the loop
      }

      return ptr;
    }

    /**
     *
     * @param gid
     * @param name
     * @return
     */
    static IDataArray::Pointer readNeighborListData(hid_t gid, const std::string &name)
    {

      herr_t err = -1;
      herr_t retErr = 1;
      hid_t typeId = -1;
      H5T_class_t attr_type;
      size_t attr_size;
      std::string res;

      std::vector<hsize_t> dims; //Reusable for the loop
      IDataArray::Pointer iDataArray = IDataArray::NullPointer();
      //std::cout << "Reading Attribute " << *iter << std::endl;
      typeId = H5Lite::getDatasetType(gid, name);
      err = H5Lite::getDatasetInfo(gid, name, dims, attr_type, attr_size);
      if(err < 0)
      {
        std::cout << "Error in getAttributeInfo method in readUserMetaData." << std::endl;
      }

      else
      {
        switch(attr_type)
        {
          case H5T_STRING:
            res.clear(); //Clear the string out first
            err = H5Lite::readStringDataset(gid, name, res);
            //        if(err >= 0)
            //        {
            //          IDataArray::Pointer attr = MXAAsciiStringData::Create(res);
            //          attr->setName(name);
            //          attributes[*iter] = attr;
            //        }
            break;
          case H5T_INTEGER:
            //std::cout << "User Meta Data Type is Integer" << std::endl;
            if(H5Tequal(typeId, H5T_STD_U8BE) || H5Tequal(typeId, H5T_STD_U8LE))
            {
              NeighborList<uint8_t>::Pointer ptr = NeighborList<uint8_t>::New();
              ptr->SetName(name);
              if(ptr->readH5Data(gid) >= 0)
              {
                iDataArray = ptr;
              }
            }
            else if(H5Tequal(typeId, H5T_STD_U16BE) || H5Tequal(typeId, H5T_STD_U16LE))
            {
              NeighborList<uint16_t>::Pointer ptr = NeighborList<uint16_t>::New();
              ptr->SetName(name);
              if(ptr->readH5Data(gid) >= 0)
              {
                iDataArray = ptr;
              }
            }
            else if(H5Tequal(typeId, H5T_STD_U32BE) || H5Tequal(typeId, H5T_STD_U32LE))
            {
              NeighborList<uint32_t>::Pointer ptr = NeighborList<uint32_t>::New();
              ptr->SetName(name);
              if(ptr->readH5Data(gid) >= 0)
              {
                iDataArray = ptr;
              }
            }
            else if(H5Tequal(typeId, H5T_STD_U64BE) || H5Tequal(typeId, H5T_STD_U64LE))
            {
              NeighborList<uint64_t>::Pointer ptr = NeighborList<uint64_t>::New();
              ptr->SetName(name);
              if(ptr->readH5Data(gid) >= 0)
              {
                iDataArray = ptr;
              }
            }
            else if(H5Tequal(typeId, H5T_STD_I8BE) || H5Tequal(typeId, H5T_STD_I8LE))
            {
              NeighborList<int8_t>::Pointer ptr = NeighborList<int8_t>::New();
              ptr->SetName(name);
              if(ptr->readH5Data(gid) >= 0)
              {
                iDataArray = ptr;
              }
            }
            else if(H5Tequal(typeId, H5T_STD_I16BE) || H5Tequal(typeId, H5T_STD_I16LE))
            {
              NeighborList<int16_t>::Pointer ptr = NeighborList<int16_t>::New();
              ptr->SetName(name);
              if(ptr->readH5Data(gid) >= 0)
              {
                iDataArray = ptr;
              }
            }
            else if(H5Tequal(typeId, H5T_STD_I32BE) || H5Tequal(typeId, H5T_STD_I32LE))
            {
              NeighborList<int32_t>::Pointer ptr = NeighborList<int32_t>::New();
              ptr->SetName(name);
              if(ptr->readH5Data(gid) >= 0)
              {
                iDataArray = ptr;
              }
            }
            else if(H5Tequal(typeId, H5T_STD_I64BE) || H5Tequal(typeId, H5T_STD_I64LE))
            {
              NeighborList<int64_t>::Pointer ptr = NeighborList<int64_t>::New();
              ptr->SetName(name);
              if(ptr->readH5Data(gid) >= 0)
              {
                iDataArray = ptr;
              }
            }
            else
            {
              std::cout << "Unknown Type: " << typeId << " at " << name << std::endl;
              err = -1;
              retErr = -1;
            }
            break;
          case H5T_FLOAT:
            if(attr_size == 4)
            {
              NeighborList<float>::Pointer ptr = NeighborList<float>::New();
              ptr->SetName(name);
              if(ptr->readH5Data(gid) >= 0)
              {
                iDataArray = ptr;
              }
            }
            else if(attr_size == 8)
            {
              NeighborList<double>::Pointer ptr = NeighborList<double>::New();
              ptr->SetName(name);
              if(ptr->readH5Data(gid) >= 0)
              {
                iDataArray = ptr;
              }
            }
            else
            {
              std::cout << "Unknown Floating point type" << std::endl;
              err = -1;
              retErr = -1;
            }
            break;
          default:
            std::cout << "Error: readUserMetaData() Unknown attribute type: " << attr_type << std::endl;
            H5Utilities::printHDFClassType(attr_type);
        }

        CloseH5T(typeId, err, retErr);
        //Close the H5A type Id that was retrieved during the loop
      }
      return iDataArray;
    }

  protected:
    H5DataArrayReader()
    {
    }

  private:
    H5DataArrayReader(const H5DataArrayReader&); // Copy Constructor Not Implemented
    void operator=(const H5DataArrayReader&); // Operator '=' Not Implemented

};

#endif /* H5DATAARRAYREADER_HPP_ */
