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


#include "H5DataArrayReader.h"

#include <vector>

#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/DataArrays/StringDataArray.hpp"

#define MIKESTEMP 1

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
                                    const QString& datasetPath,
                                    const QVector<size_t>& tDims,
                                    const QVector<size_t>& cDims)
  {
    herr_t err = -1;
    IDataArray::Pointer ptr;

    ptr = DataArray<T>::CreateArray(tDims, cDims, datasetPath);

    T* data = (T*)(ptr->getVoidPointer(0));
    err = QH5Lite::readPointerDataset(locId, datasetPath, data);
    if(err < 0)
    {
      qDebug() << "readH5Data read error: " << __FILE__ << "(" << __LINE__ << ")" ;
      ptr = IDataArray::NullPointer();
    }
    return ptr;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5DataArrayReader::ReadRequiredAttributes(hid_t gid, const QString& name, QString& objType, int& version, QVector<size_t>& tDims, QVector<size_t>& cDims)
{
  int err = 0;
  int retErr = 0;
  err = QH5Lite::readStringAttribute(gid, name, DREAM3D::HDF5::ObjectType, objType);
  if (err < 0)
  {
    retErr = err;
  }

  err = QH5Lite::readScalarAttribute(gid, name, DREAM3D::HDF5::DataArrayVersion, version);
  if(err < 0)
  {
    retErr = err;
    version = 1;
  }

  // Read the tuple dimensions as an attribute
  err = QH5Lite::readVectorAttribute(gid, name, DREAM3D::HDF5::TupleDimensions, tDims);
  if (err < 0)
  {
    retErr = err;
    qDebug() << "Missing TupleDimensions for Array with Name: " << name;
  }

  // Read the component dimensions as  an attribute
  err = QH5Lite::readVectorAttribute(gid, name, DREAM3D::HDF5::ComponentDimensions, cDims);
  if (err < 0)
  {
    retErr = err;
    qDebug() << "Missing ComponentDimensions for Array with Name: " << name;
  }

  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer H5DataArrayReader::ReadStringDataArray(hid_t gid, const QString& name, bool metaDataOnly)
{
  herr_t err = -1;
  //herr_t retErr = 1;
  hid_t typeId = -1;
  H5T_class_t attr_type;
  size_t attr_size;
  QString res;

  QVector<hsize_t> dims; //Reusable for the loop
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
    err = H5Tclose(typeId);
    return ptr;
  }


  QString classType;
  int version = 0;
  QVector<size_t> tDims;
  QVector<size_t> cDims;

  err = ReadRequiredAttributes(gid, name, classType, version, tDims, cDims);
  if(err < 0)
  {
    err = H5Tclose(typeId);
    return ptr;
  }


  //Sanity Check the combination of the Tuple and Component Dims. They should match in aggregate what we got from the getDatasetInfo above.
  qint32 offset = 0;
  for(qint32 i = 0; i < tDims.size(); i++)
  {
    if(dims.at(offset) != tDims.at(i))
    {
      qDebug() << "Tuple Dimension " << i << " did not equal the matching slot in the HDF5 Dataset Dimensions." << dims.at(offset) << " Versus " << tDims.at(i);
      err = H5Tclose(typeId);
      return ptr;
    }
    offset++;
  }
  // Strings are stored as variable length arrays so trying to match the component
  // dimensions does not make sense.
  StringDataArray::Pointer strTemp = StringDataArray::CreateArray(dims[0], name);

  err = QH5Lite::readVectorOfStringDataset(gid, name, strTemp->getStringArray());
  if(err < 0)
  {
    err = H5Tclose(typeId);
    return ptr;
  }

  ptr = strTemp;

  return ptr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer H5DataArrayReader::ReadIDataArray(hid_t gid, const QString& name, bool metaDataOnly)
{

  herr_t err = -1;
  //herr_t retErr = 1;
  hid_t typeId = -1;
  H5T_class_t attr_type;
  size_t attr_size;
  QString res;

  QVector<hsize_t> dims; //Reusable for the loop
  IDataArray::Pointer ptr = IDataArray::NullPointer();
  //qDebug() << "Reading Attribute " << *iter ;
  typeId = QH5Lite::getDatasetType(gid, name);
  if (typeId < 0)
  {
    return ptr;
  }
  // Get the HDF5 DataSet information. the dimensions will be the combined Tuple Dims and the Data Array Componenet dimes
  err = QH5Lite::getDatasetInfo(gid, name, dims, attr_type, attr_size);
  if(err < 0)
  {
    qDebug() << "Error in getAttributeInfo method in readUserMetaData." ;
  }
  else
  {
    QString classType;
    int version = 0;
    QVector<size_t> tDims;
    QVector<size_t> cDims;


    err = ReadRequiredAttributes(gid, name, classType, version, tDims, cDims);
    if(err < 0)
    {
      return ptr;
    }


    // Check to see if we are reading a bool array and if so read it and return
    if (classType.compare("DataArray<bool>") == 0)
    {
      if (metaDataOnly == false)
      {
        ptr = Detail::readH5Dataset<bool>(gid, name, tDims, cDims);
      }
      else
      {
        ptr = DataArray<bool>::CreateArray(tDims, cDims, name, false);
      }
      err = H5Tclose(typeId);
      return ptr; // <== Note early return here.
    }
    switch(attr_type)
    {
      case H5T_STRING:
        res.clear(); //Clear the string out first
        err = QH5Lite::readStringDataset(gid, name, res);
        //        if(err >= 0)
        //        {
        //          IDataArray::Pointer attr = MXAAsciiStringData::Create(res);
        //          attr->setName(name);
        //          attributes[*iter] = attr;
        //        }
        break;
      case H5T_INTEGER:
        //qDebug() << "User Meta Data Type is Integer" ;
        if(H5Tequal(typeId, H5T_STD_U8BE) || H5Tequal(typeId, H5T_STD_U8LE))
        {
          if (metaDataOnly == false)
          {
            ptr = Detail::readH5Dataset<uint8_t>(gid, name, tDims, cDims);
          }
          else
          {
            ptr = DataArray<uint8_t>::CreateArray(tDims, cDims, name, false);
          }
        }
        else if(H5Tequal(typeId, H5T_STD_U16BE) || H5Tequal(typeId, H5T_STD_U16LE))
        {
          if (metaDataOnly == false)
          {
            ptr = Detail::readH5Dataset<uint16_t>(gid, name, tDims, cDims);
          }
          else
          {
            ptr = DataArray<uint16_t>::CreateArray(tDims, cDims, name, false);
          }
        }
        else if(H5Tequal(typeId, H5T_STD_U32BE) || H5Tequal(typeId, H5T_STD_U32LE))
        {
          if (metaDataOnly == false)
          {
            ptr = Detail::readH5Dataset<uint32_t>(gid, name, tDims, cDims);
          }
          else
          {
            ptr = DataArray<uint32_t>::CreateArray(tDims, cDims, name, false);
          }
        }
        else if(H5Tequal(typeId, H5T_STD_U64BE) || H5Tequal(typeId, H5T_STD_U64LE))
        {
          if (metaDataOnly == false)
          {
            ptr = Detail::readH5Dataset<uint64_t>(gid, name, tDims, cDims);
          }
          else
          {
            ptr = DataArray<uint64_t>::CreateArray(tDims, cDims, name, false);
          }
        }
        else if(H5Tequal(typeId, H5T_STD_I8BE) || H5Tequal(typeId, H5T_STD_I8LE))
        {
          if (metaDataOnly == false)
          {
            ptr = Detail::readH5Dataset<int8_t>(gid, name, tDims, cDims);
          }
          else
          {
            ptr = DataArray<int8_t>::CreateArray(tDims, cDims, name, false);
          }
        }
        else if(H5Tequal(typeId, H5T_STD_I16BE) || H5Tequal(typeId, H5T_STD_I16LE))
        {
          if (metaDataOnly == false)
          {
            ptr = Detail::readH5Dataset<int16_t>(gid, name, tDims, cDims);
          }
          else
          {
            ptr = DataArray<int16_t>::CreateArray(tDims, cDims, name, false);
          }
        }
        else if(H5Tequal(typeId, H5T_STD_I32BE) || H5Tequal(typeId, H5T_STD_I32LE))
        {
          if (metaDataOnly == false)
          {
            ptr = Detail::readH5Dataset<int32_t>(gid, name, tDims, cDims);
          }
          else
          {
            ptr = DataArray<int32_t>::CreateArray(tDims, cDims, name, false);
          }
        }
        else if(H5Tequal(typeId, H5T_STD_I64BE) || H5Tequal(typeId, H5T_STD_I64LE))
        {
          if (metaDataOnly == false)
          {
            ptr = Detail::readH5Dataset<int64_t>(gid, name, tDims, cDims);
          }
          else
          {
            ptr = DataArray<int64_t>::CreateArray(tDims, cDims, name, false);
          }
        }
        else
        {
          qDebug() << "Unknown Type: " << typeId << " at " << name ;
          err = -1;
        }
        break;
      case H5T_FLOAT:
        if(attr_size == 4)
        {
          if (metaDataOnly == false)
          {
            ptr = Detail::readH5Dataset<float>(gid, name, tDims, cDims);
          }
          else
          {
            ptr = DataArray<float>::CreateArray(tDims, cDims, name, false);
          }
        }
        else if(attr_size == 8)
        {
          if (metaDataOnly == false)
          {
            ptr = Detail::readH5Dataset<double>(gid, name, tDims, cDims);
          }
          else
          {
            ptr = DataArray<double>::CreateArray(tDims, cDims, name, false);
          }
        }
        else
        {
          qDebug() << "Unknown Floating point type" ;
          err = -1;
        }
        break;
      default:
        qDebug() << "Error: readUserMetaData() Unknown attribute type: " << attr_type ;
        QH5Utilities::printHDFClassType(attr_type);
    }

    err = H5Tclose(typeId);
    //Close the H5A type Id that was retrieved during the loop
  }

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer H5DataArrayReader::ReadNeighborListData(hid_t gid, const QString& name, bool metaDataOnly)
{

  herr_t err = -1;
  hid_t typeId = -1;
  H5T_class_t attr_type;
  size_t attr_size;
  QString res;

  QVector<hsize_t> dims; //Reusable for the loop
  IDataArray::Pointer iDataArray = IDataArray::NullPointer();
  //qDebug() << "Reading Attribute " << *iter ;
  typeId = QH5Lite::getDatasetType(gid, name);
  err = QH5Lite::getDatasetInfo(gid, name, dims, attr_type, attr_size);
  if(err < 0)
  {
    qDebug() << "Error in getAttributeInfo method in readUserMetaData." ;
  }
  else
  {
    QString classType;
    int version = 0;
    QVector<size_t> tDims;
    QVector<size_t> cDims;

    err = ReadRequiredAttributes(gid, name, classType, version, tDims, cDims);
    if(err < 0)
    {
      return iDataArray;
    }


    switch(attr_type)
    {
      case H5T_STRING:
        res.clear(); //Clear the string out first
        err = QH5Lite::readStringDataset(gid, name, res);
        break;
      case H5T_INTEGER:
        if(H5Tequal(typeId, H5T_STD_U8BE) || H5Tequal(typeId, H5T_STD_U8LE))
        {
          NeighborList<uint8_t>::Pointer ptr = NeighborList<uint8_t>::CreateArray(tDims, cDims, name, false);
          if(false == metaDataOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else if(H5Tequal(typeId, H5T_STD_U16BE) || H5Tequal(typeId, H5T_STD_U16LE))
        {
          NeighborList<uint16_t>::Pointer ptr = NeighborList<uint16_t>::CreateArray(tDims, cDims, name, false);
          if(false == metaDataOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else if(H5Tequal(typeId, H5T_STD_U32BE) || H5Tequal(typeId, H5T_STD_U32LE))
        {
          NeighborList<uint32_t>::Pointer ptr = NeighborList<uint32_t>::CreateArray(tDims, cDims, name, false);
          if(false == metaDataOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else if(H5Tequal(typeId, H5T_STD_U64BE) || H5Tequal(typeId, H5T_STD_U64LE))
        {
          NeighborList<uint64_t>::Pointer ptr = NeighborList<uint64_t>::CreateArray(tDims, cDims, name, false);
          if(false == metaDataOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else if(H5Tequal(typeId, H5T_STD_I8BE) || H5Tequal(typeId, H5T_STD_I8LE))
        {
          NeighborList<int8_t>::Pointer ptr = NeighborList<int8_t>::CreateArray(tDims, cDims, name, false);
          if(false == metaDataOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else if(H5Tequal(typeId, H5T_STD_I16BE) || H5Tequal(typeId, H5T_STD_I16LE))
        {
          NeighborList<int16_t>::Pointer ptr = NeighborList<int16_t>::CreateArray(tDims, cDims, name, false);
          if(false == metaDataOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else if(H5Tequal(typeId, H5T_STD_I32BE) || H5Tequal(typeId, H5T_STD_I32LE))
        {
          NeighborList<int32_t>::Pointer ptr = NeighborList<int32_t>::CreateArray(tDims, cDims, name, false);
          if(false == metaDataOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else if(H5Tequal(typeId, H5T_STD_I64BE) || H5Tequal(typeId, H5T_STD_I64LE))
        {
          NeighborList<int64_t>::Pointer ptr = NeighborList<int64_t>::CreateArray(tDims, cDims, name, false);
          if(false == metaDataOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else
        {
          qDebug() << "Unknown Type: " << typeId << " at " << name ;
          err = -1;
        }
        break;
      case H5T_FLOAT:
        if(attr_size == 4)
        {
          NeighborList<float>::Pointer ptr = NeighborList<float>::CreateArray(tDims, cDims, name, false);
          if(false == metaDataOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else if(attr_size == 8)
        {
          NeighborList<double>::Pointer ptr = NeighborList<double>::CreateArray(tDims, cDims, name, false);
          if(false == metaDataOnly)
          {
            ptr->readH5Data(gid);
          }
          iDataArray = ptr;
        }
        else
        {
          qDebug() << "Unknown Floating point type" ;
          err = -1;
        }
        break;
      default:
        qDebug() << "Error: readUserMetaData() Unknown attribute type: " << attr_type ;
        QH5Utilities::printHDFClassType(attr_type);
    }

    err = H5Tclose(typeId);
    //Close the H5A type Id that was retrieved during the loop
  }
  if (err < 0 )
  {
    iDataArray = IDataArray::NullPointer();
  }
  return iDataArray;
}
