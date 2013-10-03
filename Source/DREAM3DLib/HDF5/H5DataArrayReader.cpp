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

#include "H5DataArrayReader.h"

#include <vector>

#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"
#include "DREAM3DLib/DataArrays/StringDataArray.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5DataArrayReader::H5DataArrayReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5DataArrayReader::~H5DataArrayReader()
{

}

namespace Detail
{
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template<typename T>
  IDataArray::Pointer readH5Dataset(hid_t locId,
                                    const std::string& datasetPath,
                                    const std::vector<hsize_t>& dims)
  {
    herr_t err = -1;
    IDataArray::Pointer ptr;
    size_t numElements = 1;
    for (size_t i = 0; i < dims.size(); ++i)
    {
      numElements *= dims[i];
    }
    ptr = DataArray<T>::CreateArray(numElements, datasetPath);
    if(dims.size() > 1)
    {
      ptr->SetNumberOfComponents(static_cast<int>(dims[1]));
    }

    T* data = (T*)(ptr->GetVoidPointer(0));
    err = H5Lite::readPointerDataset(locId, datasetPath, data);
    if(err < 0)
    {
      std::cout << "readH5Data read error: " << __FILE__ << "(" << __LINE__ << ")" << std::endl;
      ptr = IDataArray::NullPointer();
    }
    return ptr;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer H5DataArrayReader::readStringDataArray(hid_t gid, const std::string& name, bool preflightOnly)
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
  if (typeId < 0)
  {
    return ptr;
  }
  err = H5Lite::getDatasetInfo(gid, name, dims, attr_type, attr_size);
  if(err < 0)
  {
    std::cout << "Error in getAttributeInfo method in readUserMetaData." << std::endl;
  }
  else
  {
    std::string classType;
    err = H5Lite::readStringAttribute(gid, name, DREAM3D::HDF5::ObjectType, classType);
    if (err < 0)
    {
      return ptr;
    }
    int numComp = 1;
    err = H5Lite::readScalarAttribute(gid, name, DREAM3D::HDF5::NumComponents, numComp);
    if (err < 0)
    {
      numComp = 1;
    }
    if(H5Tequal(typeId, H5T_STD_U8BE) || H5Tequal(typeId, H5T_STD_U8LE)
        || H5Tequal(typeId, H5T_STD_I8BE) || H5Tequal(typeId, H5T_STD_I8LE) )
    {
      if (preflightOnly == false)
      {
        IDataArray::Pointer bufferPtr = Detail::readH5Dataset<char>(gid, name, dims);
        const char* buf = reinterpret_cast<char*>(bufferPtr->GetVoidPointer(0));
        // count the number of 0x00 characters which are the 'null termination' of each string

        size_t size = bufferPtr->GetNumberOfTuples();
        size_t count = 0;
        for(size_t i = 0; i < size; ++i)
        {
          if(buf[i] == 0)
          {
            ++count;
          }
        }
        if (count > 0)
        {
          ptr = StringDataArray::CreateArray(count, name);
          StringDataArray* strArray = StringDataArray::SafePointerDownCast(ptr.get());
          size_t start = 0;
          size_t index = 0;
          for(size_t i = 0; i < size; ++i)
          {
            if(buf[i] == 0)
            {
              std::string str( &(buf[start]) );
              strArray->SetValue(index, str);
              ++index;
              start = i + 1;
            }
          }
        }
      }
      else // We are preflighting only so just create a StringDataArray of lenght 1
      {
        ptr = StringDataArray::CreateArray(1, name);
      }
    }

  }
  CloseH5T(typeId, err, retErr);
  return ptr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer H5DataArrayReader::readIDataArray(hid_t gid, const std::string& name, bool preflightOnly)
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
  if (typeId < 0)
  {
    return ptr;
  }
  err = H5Lite::getDatasetInfo(gid, name, dims, attr_type, attr_size);
  if(err < 0)
  {
    std::cout << "Error in getAttributeInfo method in readUserMetaData." << std::endl;
  }
  else
  {
    std::string classType;
    err = H5Lite::readStringAttribute(gid, name, DREAM3D::HDF5::ObjectType, classType);
    if (err < 0)
    {
      return ptr;
    }
    int numComp = 1;
    err = H5Lite::readScalarAttribute(gid, name, DREAM3D::HDF5::NumComponents, numComp);
    if (err < 0)
    {
      numComp = 1;
    }
    // Check to see if we are reading a bool array and if so read it and return
    if (classType.compare("DataArray<bool>") == 0)
    {
      if (preflightOnly == false) { ptr = Detail::readH5Dataset<bool>(gid, name, dims); }
      else { ptr = DataArray<bool>::CreateArray(1, numComp, name); }
      CloseH5T(typeId, err, retErr);
      return ptr; // <== Note early return here.
    }
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
          if (preflightOnly == false) { ptr = Detail::readH5Dataset<uint8_t>(gid, name, dims); }
          else { ptr = DataArray<uint8_t>::CreateArray(1, numComp, name); }
        }
        else if(H5Tequal(typeId, H5T_STD_U16BE) || H5Tequal(typeId, H5T_STD_U16LE))
        {
          if (preflightOnly == false) { ptr = Detail::readH5Dataset<uint16_t>(gid, name, dims); }
          else { ptr = DataArray<uint16_t>::CreateArray(1, numComp, name); }
        }
        else if(H5Tequal(typeId, H5T_STD_U32BE) || H5Tequal(typeId, H5T_STD_U32LE))
        {
          if (preflightOnly == false) { ptr = Detail::readH5Dataset<uint32_t>(gid, name, dims); }
          else { ptr = DataArray<uint32_t>::CreateArray(1, numComp, name); }
        }
        else if(H5Tequal(typeId, H5T_STD_U64BE) || H5Tequal(typeId, H5T_STD_U64LE))
        {
          if (preflightOnly == false) { ptr = Detail::readH5Dataset<uint64_t>(gid, name, dims); }
          else { ptr = DataArray<uint64_t>::CreateArray(1, numComp, name); }
        }
        else if(H5Tequal(typeId, H5T_STD_I8BE) || H5Tequal(typeId, H5T_STD_I8LE))
        {
          if (preflightOnly == false) { ptr = Detail::readH5Dataset<int8_t>(gid, name, dims); }
          else { ptr = DataArray<int8_t>::CreateArray(1, numComp, name); }
        }
        else if(H5Tequal(typeId, H5T_STD_I16BE) || H5Tequal(typeId, H5T_STD_I16LE))
        {
          if (preflightOnly == false) { ptr = Detail::readH5Dataset<int16_t>(gid, name, dims); }
          else { ptr = DataArray<int16_t>::CreateArray(1, numComp, name); }
        }
        else if(H5Tequal(typeId, H5T_STD_I32BE) || H5Tequal(typeId, H5T_STD_I32LE))
        {
          if (preflightOnly == false) { ptr = Detail::readH5Dataset<int32_t>(gid, name, dims); }
          else { ptr = DataArray<int32_t>::CreateArray(1, numComp, name); }
        }
        else if(H5Tequal(typeId, H5T_STD_I64BE) || H5Tequal(typeId, H5T_STD_I64LE))
        {
          if (preflightOnly == false) { ptr = Detail::readH5Dataset<int64_t>(gid, name, dims); }
          else { ptr = DataArray<int64_t>::CreateArray(1, numComp, name); }
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
          if (preflightOnly == false) { ptr = Detail::readH5Dataset<float>(gid, name, dims); }
          else { ptr = DataArray<float>::CreateArray(1, numComp, name); }
        }
        else if(attr_size == 8)
        {
          if (preflightOnly == false) { ptr = Detail::readH5Dataset<double>(gid, name, dims); }
          else { ptr = DataArray<double>::CreateArray(1, numComp, name); }
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer H5DataArrayReader::readNeighborListData(hid_t gid, const std::string& name, bool preflightOnly)
{

  herr_t err = -1;
  hid_t typeId = -1;
  H5T_class_t attr_type;
  size_t attr_size;
  std::string res;

  std::vector<hsize_t> dims; //Reusable for the loop
  IDataArray::Pointer iDataArray = IDataArray::NullPointer();
  //std::cout << "Reading Attribute " << *iter << std::endl;
  typeId = H5Lite::getDatasetType(gid, name);
  std::string keyname;
  err = H5Lite::readStringAttribute(gid, name, "Linked NumNeighbors Dataset", keyname);
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
          ptr->setNumNeighborsArrayName(keyname);
          if(false == preflightOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else if(H5Tequal(typeId, H5T_STD_U16BE) || H5Tequal(typeId, H5T_STD_U16LE))
        {
          NeighborList<uint16_t>::Pointer ptr = NeighborList<uint16_t>::New();
          ptr->SetName(name);
          ptr->setNumNeighborsArrayName(keyname);
          if(false == preflightOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else if(H5Tequal(typeId, H5T_STD_U32BE) || H5Tequal(typeId, H5T_STD_U32LE))
        {
          NeighborList<uint32_t>::Pointer ptr = NeighborList<uint32_t>::New();
          ptr->SetName(name);
          ptr->setNumNeighborsArrayName(keyname);
          if(false == preflightOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else if(H5Tequal(typeId, H5T_STD_U64BE) || H5Tequal(typeId, H5T_STD_U64LE))
        {
          NeighborList<uint64_t>::Pointer ptr = NeighborList<uint64_t>::New();
          ptr->SetName(name);
          ptr->setNumNeighborsArrayName(keyname);
          if(false == preflightOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else if(H5Tequal(typeId, H5T_STD_I8BE) || H5Tequal(typeId, H5T_STD_I8LE))
        {
          NeighborList<int8_t>::Pointer ptr = NeighborList<int8_t>::New();
          ptr->SetName(name);
          ptr->setNumNeighborsArrayName(keyname);
          if(false == preflightOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else if(H5Tequal(typeId, H5T_STD_I16BE) || H5Tequal(typeId, H5T_STD_I16LE))
        {
          NeighborList<int16_t>::Pointer ptr = NeighborList<int16_t>::New();
          ptr->SetName(name);
          ptr->setNumNeighborsArrayName(keyname);
          if(false == preflightOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else if(H5Tequal(typeId, H5T_STD_I32BE) || H5Tequal(typeId, H5T_STD_I32LE))
        {
          NeighborList<int32_t>::Pointer ptr = NeighborList<int32_t>::New();
          ptr->SetName(name);
          ptr->setNumNeighborsArrayName(keyname);
          if(false == preflightOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else if(H5Tequal(typeId, H5T_STD_I64BE) || H5Tequal(typeId, H5T_STD_I64LE))
        {
          NeighborList<int64_t>::Pointer ptr = NeighborList<int64_t>::New();
          ptr->SetName(name);
          ptr->setNumNeighborsArrayName(keyname);
          if(false == preflightOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else
        {
          std::cout << "Unknown Type: " << typeId << " at " << name << std::endl;
          err = -1;
        }
        break;
      case H5T_FLOAT:
        if(attr_size == 4)
        {
          NeighborList<float>::Pointer ptr = NeighborList<float>::New();
          ptr->SetName(name);
          ptr->setNumNeighborsArrayName(keyname);
          if(false == preflightOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else if(attr_size == 8)
        {
          NeighborList<double>::Pointer ptr = NeighborList<double>::New();
          ptr->SetName(name);
          ptr->setNumNeighborsArrayName(keyname);
          if(false == preflightOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else
        {
          std::cout << "Unknown Floating point type" << std::endl;
          err = -1;
        }
        break;
      default:
        std::cout << "Error: readUserMetaData() Unknown attribute type: " << attr_type << std::endl;
        H5Utilities::printHDFClassType(attr_type);
    }
    int retErr = 0;
    CloseH5T(typeId, err, retErr);
    //Close the H5A type Id that was retrieved during the loop
  }
  if (err < 0 )
  {
    iDataArray = IDataArray::NullPointer();
  }
  return iDataArray;
}
