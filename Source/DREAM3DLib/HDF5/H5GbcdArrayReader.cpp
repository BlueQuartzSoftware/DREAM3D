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

#include "H5GbcdArrayReader.h"

#include <vector>

#include "H5Support/QH5Utilities.h"

#include "DREAM3DLib/Common/GbcdDataArray.hpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5GbcdArrayReader::H5GbcdArrayReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5GbcdArrayReader::~H5GbcdArrayReader()
{

}

namespace Detail
{
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template<typename T>
  IDataArray::Pointer readGbcdDataset(hid_t locId,
                                    const QString &datasetPath,
                                    const std::vector<hsize_t> &dims)
  {
    herr_t err = -1;
    IDataArray::Pointer ptr;
    size_t gDims[5] = {dims[0], dims[1], dims[2], dims[3], dims[4] };
    // Allocate an array of memory based on the dimensions
    ptr = GbcdDataArray<T>::CreateArray(gDims, datasetPath, true);
    // Get the pointer to the actual memory location
    T* data = (T*)(ptr->GetVoidPointer(0));
    // Read the file into that memory location
    err = QH5Lite::readPointerDataset(locId, datasetPath, data);
    if(err < 0)
    {
      qDebug() << "H5GbcdArrayReader::readH5Data read error: " << __FILE__ << "(" << __LINE__ << ")" ;
      ptr = IDataArray::NullPointer();
    }
    return ptr;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer H5GbcdArrayReader::readIDataArray(hid_t gid, const QString &name, bool preflightOnly)
{

  herr_t err = -1;
  herr_t retErr = 1;
  hid_t typeId = -1;
  H5T_class_t attr_type;
  size_t attr_size;
  QString res;

  std::vector<hsize_t> dims; //Reusable for the loop
  IDataArray::Pointer ptr = IDataArray::NullPointer();
  //qDebug() << "Reading Attribute " << *iter ;
  typeId = QH5Lite::getDatasetType(gid, name);
  if (typeId < 0)
  {
    return ptr;
  }
  err = QH5Lite::getDatasetInfo(gid, name, dims, attr_type, attr_size);
  if(err < 0)
  {
    qDebug() << "Error in getAttributeInfo method in readUserMetaData." ;
  }
  else
  {
    QString classType;
    err = QH5Lite::readStringAttribute(gid, name, DREAM3D::HDF5::ObjectType, classType);
    if (err < 0)
    {
      return ptr;
    }

    // Check to see if we are reading a bool array and if so read it and return
    if (classType.compare("GbcdDataArray<bool>") == 0)
    {
      if (preflightOnly == false) ptr = Detail::readGbcdDataset<bool>(gid, name, dims);
      else ptr = GbcdDataArray<bool>::CreateArray(dims[0], dims[1], dims[2], dims[3], dims[4], name, !preflightOnly);
      CloseH5T(typeId, err, retErr);
      return ptr; // <== Note early return here.
    }
    switch(attr_type)
    {

      case H5T_INTEGER:
        //qDebug() << "User Meta Data Type is Integer" ;
        if(H5Tequal(typeId, H5T_STD_U8BE) || H5Tequal(typeId, H5T_STD_U8LE))
        {
          if (preflightOnly == false) ptr = Detail::readGbcdDataset<uint8_t>(gid, name, dims);
          else ptr = GbcdDataArray<uint8_t>::CreateArray(dims[0], dims[1], dims[2], dims[3], dims[4], name, !preflightOnly);
        }
        else if(H5Tequal(typeId, H5T_STD_U16BE) || H5Tequal(typeId, H5T_STD_U16LE))
        {
          if (preflightOnly == false) ptr = Detail::readGbcdDataset<uint16_t>(gid, name, dims);
          else ptr = GbcdDataArray<uint16_t>::CreateArray(dims[0], dims[1], dims[2], dims[3], dims[4], name, !preflightOnly);
        }
        else if(H5Tequal(typeId, H5T_STD_U32BE) || H5Tequal(typeId, H5T_STD_U32LE))
        {
          if (preflightOnly == false) ptr = Detail::readGbcdDataset<uint32_t>(gid, name, dims);
          else ptr = GbcdDataArray<uint32_t>::CreateArray(dims[0], dims[1], dims[2], dims[3], dims[4], name, !preflightOnly);
        }
        else if(H5Tequal(typeId, H5T_STD_U64BE) || H5Tequal(typeId, H5T_STD_U64LE))
        {
          if (preflightOnly == false) ptr = Detail::readGbcdDataset<uint64_t>(gid, name, dims);
          else ptr = GbcdDataArray<uint64_t>::CreateArray(dims[0], dims[1], dims[2], dims[3], dims[4], name, !preflightOnly);
        }
        else if(H5Tequal(typeId, H5T_STD_I8BE) || H5Tequal(typeId, H5T_STD_I8LE))
        {
          if (preflightOnly == false) ptr = Detail::readGbcdDataset<int8_t>(gid, name, dims);
          else ptr = GbcdDataArray<int8_t>::CreateArray(dims[0], dims[1], dims[2], dims[3], dims[4], name, !preflightOnly);
        }
        else if(H5Tequal(typeId, H5T_STD_I16BE) || H5Tequal(typeId, H5T_STD_I16LE))
        {
          if (preflightOnly == false) ptr = Detail::readGbcdDataset<int16_t>(gid, name, dims);
          else ptr = GbcdDataArray<int16_t>::CreateArray(dims[0], dims[1], dims[2], dims[3], dims[4], name, !preflightOnly);
        }
        else if(H5Tequal(typeId, H5T_STD_I32BE) || H5Tequal(typeId, H5T_STD_I32LE))
        {
          if (preflightOnly == false) ptr = Detail::readGbcdDataset<int32_t>(gid, name, dims);
          else ptr = GbcdDataArray<int32_t>::CreateArray(dims[0], dims[1], dims[2], dims[3], dims[4], name, !preflightOnly);
        }
        else if(H5Tequal(typeId, H5T_STD_I64BE) || H5Tequal(typeId, H5T_STD_I64LE))
        {
          if (preflightOnly == false) ptr = Detail::readGbcdDataset<int64_t>(gid, name, dims);
          else ptr = GbcdDataArray<int64_t>::CreateArray(dims[0], dims[1], dims[2], dims[3], dims[4], name, !preflightOnly);
        }
        else
        {
          qDebug() << "Unknown Type: " << typeId << " at " << name ;
          err = -1;
          retErr = -1;
        }
        break;
      case H5T_FLOAT:
        if(attr_size == 4)
        {
          if (preflightOnly == false) ptr = Detail::readGbcdDataset<float>(gid, name, dims);
          else ptr = GbcdDataArray<float>::CreateArray(dims[0], dims[1], dims[2], dims[3], dims[4], name, !preflightOnly);
        }
        else if(attr_size == 8)
        {
          if (preflightOnly == false) ptr = Detail::readGbcdDataset<double>(gid, name, dims);
          else ptr = GbcdDataArray<double>::CreateArray(dims[0], dims[1], dims[2], dims[3], dims[4], name, !preflightOnly);
        }
        else
        {
          qDebug() << "Unknown Floating point type" ;
          err = -1;
          retErr = -1;
        }
        break;
      default:
        qDebug() << "Error: readUserMetaData() Unknown attribute type: " << attr_type ;
        QH5Utilities::printHDFClassType(attr_type);
    }

    CloseH5T(typeId, err, retErr);
    //Close the H5A type Id that was retrieved during the loop
  }

  return ptr;
}
