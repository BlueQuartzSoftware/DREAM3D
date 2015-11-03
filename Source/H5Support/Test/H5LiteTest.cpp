///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream>
#include <string>

#include <QtCore/QFile>
#include <QtCore/QtDebug>
#include <QtCore/QString>
#include <QtCore/QByteArray>


#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"

#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "H5SupportTestFileLocations.h"


#define DSET0_NAME "2D int32_t array"
#define DSET1_NAME "dataset char"
#define DSET2_NAME "dataset short"
#define DSET3_NAME "dataset int"
#define DSET4_NAME "dataset long"
#define DSET5_NAME "dataset float"
#define DSET6_NAME "dataset double"
#define DSET7_NAME "dataset string"

#define DIM 6
#define DIM0 4
#define DIM1 3
#define DIM2 2

#define RANK_1D 1
#define RANK_2D 2
#define RANK_3D 3



#define H5ATTRIBUTE_INSTANCE(m_msgType, Suffix, key)\
  Suffix##H5Attribute::Pointer m_msgType##_attr = Suffix##H5Attribute::New();\
  m_msgType##_attr->setKey(key);


//TODO: Test the Read/Write IMXAArray methods

/* Methods to test
 *
 * HDFTypeFromString
 * HDFTypeForPrimitiveAsStr
 * HDFTypeForPrimitive
 * writeVectorDataset - DONE
 * writePointerDataset - DONE
 * writeScalarDataset - DONE
 * writeStringDataset - DONE
 * writeStringDataset - DONE
 * writePointerAttribute - DONE
 * writeVectorAttribute - DONE
 * writeStringAttribute - DONE
 * writeStringAttribute - DONE
 * writeStringAttributes - DONE
 * writeScalarAttribute - DONE
 * readPointerDataset - DONE
 * readVectorDataset - DONE
 * readScalarDataset - DONE
 * readStringDataset - DONE
 * readStringDataset - DONE
 * readVectorAttribute - DONE
 * readScalarAttribute - DONE
 * readPointerAttribute - DONE
 * readStringAttribute - DONE
 * readStringAttribute - DONE
 * getAttributeNDims - DONE
 * getDatasetNDims - DONE
 * getDatasetType
 * getDatasetInfo - DONE
 * getAttributeInfo - DONE
 */


herr_t testMakeStringDataset(hid_t file_id);
herr_t testMakeStringAttribute(hid_t file_id);
void QH5LiteTest();
//MXADataModel::Pointer createModelTemplate();
template<typename T>
herr_t testWritePointer1DArrayAttribute(hid_t file_id, const QString& dsetName);



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(UnitTest::H5LiteTest::FileName);
  QFile::remove(UnitTest::H5LiteTest::LargeFile);
#endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testWritePointer1DArrayAttribute(hid_t file_id, const QString& dsetName)
{
  T value = 0x0;
  herr_t err = -1;
  QString attributeKey = QH5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.isEmpty() == false);
  attributeKey = "1DArrayAttribute<" + attributeKey + ">";
  int32_t rank = 1;
  T data[DIM0];
  for(int i = 0; i < DIM0; ++i)
  {
    data[i] = (T)(i);
  }
  hsize_t dims[1];
  dims[0] = DIM0;

  err = QH5Lite::writePointerAttribute<T>(file_id, dsetName, attributeKey, rank, dims, (T*)data);
  DREAM3D_REQUIRE(err >= 0);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testReadPointer1DArrayAttribute(hid_t file_id, const QString& dsetName)
{
  T value = 0x0;
  herr_t err = -1;
  herr_t retErr = err;
  QString attributeKey = QH5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.isEmpty() == false);
  attributeKey = "1DArrayAttribute<" + attributeKey + ">";
  QVector<T> referenceData(DIM0, 0);
  for(int i = 0; i < DIM0; ++i)
  {
    referenceData[i] = (T)(i);
  }

  H5T_class_t attr_type;
  size_t attr_size;
  hid_t typeId = -1;
  QVector<hsize_t> dims;  //Reusable for the loop
  err = QH5Lite::getAttributeInfo(file_id, dsetName, attributeKey, dims, attr_type, attr_size, typeId);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(dims.size() == 1);
  DREAM3D_REQUIRE(attr_size == sizeof(T));
  hid_t rank = 0;
  err = QH5Lite::getAttributeNDims(file_id, dsetName, attributeKey, rank);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(rank == 1);
  CloseH5T(typeId, err, retErr); //Close the H5A type Id that was retrieved during the loop
  typename QVector<T>::size_type numElements = 1;
  for (QVector<uint64_t>::size_type i = 0; i < dims.size(); ++i)
  {
    numElements = numElements * dims[i];
  }
  QVector<T> data(numElements, 0);
  err = QH5Lite::readPointerAttribute<T>(file_id, dsetName, attributeKey, data.data() );
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE (data == referenceData);

  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testWritePointer2DArrayAttribute(hid_t file_id, const QString& dsetName)
{
  //qDebug() << DEBUG_OUT(logTime) << "testWritePointer2DArrayAttribute" << "\n";
  T value = 0x0;
  herr_t err = -1;
  QString attributeKey = QH5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.isEmpty() == false);

  attributeKey = "2DArrayAttribute<" + attributeKey + ">";
  int32_t rank = RANK_2D;
  T data[DIM0][DIM1];
  for(int i = 0; i < DIM0; ++i)
  {
    for (int j = 0; j < DIM1; ++j)
    {
      data[i][j] = (T)(i * j);
    }
  }
  hsize_t dims[RANK_2D];
  dims[0] = DIM0;
  dims[1] = DIM1;
  err = QH5Lite::writePointerAttribute<T>(file_id, dsetName, attributeKey, rank, dims, (T*)data);
  DREAM3D_REQUIRE(err >= 0);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testReadPointer2DArrayAttribute(hid_t file_id, const QString& dsetName)
{
  T value = 0x0;
  herr_t err = -1;
  herr_t retErr = err;
  QString attributeKey = QH5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.isEmpty() == false);
  attributeKey = "2DArrayAttribute<" + attributeKey + ">";
  T referenceData[DIM0][DIM1];
  for(int i = 0; i < DIM0; ++i)
  {
    for (int j = 0; j < DIM1; ++j)
    {
      referenceData[i][j] = (T)(i * j);
    }
  }
  H5T_class_t attr_type;
  size_t attr_size;
  hid_t typeId = -1;
  QVector<hsize_t> dims;  //Reusable for the loop
  err = QH5Lite::getAttributeInfo(file_id, dsetName, attributeKey, dims, attr_type, attr_size, typeId);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(dims.size() == 2);
  DREAM3D_REQUIRE(attr_size == sizeof(T));
  hid_t rank = 0;
  err = QH5Lite::getAttributeNDims(file_id, dsetName, attributeKey, rank);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(rank == 2);

  CloseH5T(typeId, err, retErr); //Close the H5A type Id that was retrieved during the loop
  typename QVector<T>::size_type numElements = 1;
  for (QVector<uint64_t>::size_type i = 0; i < dims.size(); ++i)
  {
    numElements = numElements * dims[i];
  }
  QVector<T> data(numElements, 0);
  err = QH5Lite::readPointerAttribute<T>(file_id, dsetName, attributeKey, data.data() );
  DREAM3D_REQUIRE(err >= 0);

  DREAM3D_REQUIRE (::memcmp( data.data(), referenceData, sizeof(T)*numElements) == 0);
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testWritePointer3DArrayAttribute(hid_t file_id, const QString& dsetName)
{
  T value = 0x0;
  herr_t err = -1;
  QString attributeKey = QH5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.isEmpty() == false);
  attributeKey = "3DArrayAttribute<" + attributeKey + ">";
  int32_t rank = RANK_3D;
  T data[DIM0][DIM1][DIM2];
  for(int i = 0; i < DIM0; ++i)
  {
    for (int j = 0; j < DIM1; ++j)
    {
      for (int k = 0; k < DIM2; ++k)
      {
        data[i][j][k] = (T)(i * j * k);
      }
    }
  }
  hsize_t dims[RANK_3D];
  dims[0] = DIM0;
  dims[1] = DIM1;
  dims[2] = DIM2;
  err = QH5Lite::writePointerAttribute<T>(file_id, dsetName, attributeKey, rank, dims, (T*)data);
  DREAM3D_REQUIRE(err >= 0);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testReadPointer3DArrayAttribute(hid_t file_id, const QString& dsetName)
{
  T value = 0x0;
  herr_t err = -1;
  herr_t retErr = err;
  QString attributeKey = QH5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.isEmpty() == false);
  attributeKey = "3DArrayAttribute<" + attributeKey + ">";
  T referenceData[DIM0][DIM1][DIM2];
  for(int i = 0; i < DIM0; ++i)
  {
    for (int j = 0; j < DIM1; ++j)
    {
      for (int k = 0; k < DIM2; ++k)
      {
        referenceData[i][j][k] = (T)(i * j * k);
      }
    }
  }
  H5T_class_t attr_type;
  size_t attr_size;
  hid_t typeId = -1;
  QVector<hsize_t> dims;  //Reusable for the loop
  err = QH5Lite::getAttributeInfo(file_id, dsetName, attributeKey, dims, attr_type, attr_size, typeId);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(dims.size() == 3);
  DREAM3D_REQUIRE(attr_size == sizeof(T));
  hid_t rank = 0;
  err = QH5Lite::getAttributeNDims(file_id, dsetName, attributeKey, rank);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(rank == 3);
  CloseH5T(typeId, err, retErr); //Close the H5A type Id that was retrieved during the loop
  typename QVector<T>::size_type numElements = 1;
  for (QVector<uint64_t>::size_type i = 0; i < dims.size(); ++i)
  {
    numElements = numElements * dims[i];
  }
  QVector<T> data(numElements, 0);
  err = QH5Lite::readPointerAttribute<T>(file_id, dsetName, attributeKey, data.data() );
  DREAM3D_REQUIRE(err >= 0);

  DREAM3D_REQUIRE (::memcmp( data.data(), referenceData, sizeof(T)*numElements) == 0);
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
herr_t testWriteVectorAttribute(hid_t file_id, QString dsetName )
{
  T value = 0x0;
  herr_t err = -1;
  QString attributeKey = QH5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.isEmpty() == false);
  attributeKey = "VectorAttribute<" + attributeKey + ">";

  int32_t numElements = DIM0;
  QVector<hsize_t> dims (1, DIM0);

  /* Make dataset */
  QVector<T> data (DIM0, 0);
  for (int i = 0; i < numElements; ++i)
  {
    data[i] = (T)(i);
  }
  //qDebug() << "Attribute->Write: " << objName;
  err = QH5Lite::writeVectorAttribute( file_id, dsetName, attributeKey, dims, data );
  DREAM3D_REQUIRE(err >= 0);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
herr_t testReadVectorAttribute(hid_t file_id, QString dsetName )
{
  T value = 0x0;
  herr_t err = -1;
  QString attributeKey = QH5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.isEmpty() == false);
  attributeKey = "VectorAttribute<" + attributeKey + ">";

  int32_t numElements = DIM0;
  QVector<hsize_t> dims (1, DIM0);

  QVector<T> data (DIM0, 0);
  for (int i = 0; i < numElements; ++i)
  {
    data[i] = (T)(i);
  }
  QVector<T> rData(numElements, 0); //allocate and zero out the memory
  err = QH5Lite::readVectorAttribute(file_id, dsetName, attributeKey, rData);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE( data == rData );
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testWriteScalarAttribute(hid_t file_id, const QString& dsetName)
{
  T value = 0x0F;
  herr_t err = -1;
  QString attributeKey = QH5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.isEmpty() == false);
  attributeKey = "ScalarAttribute<" + attributeKey + ">";
  err = QH5Lite::writeScalarAttribute(file_id, dsetName, attributeKey, value);
  DREAM3D_REQUIRE(err >= 0);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testReadScalarAttribute(hid_t file_id, const QString& dsetName)
{
  T value = 0x0F;
  T refValue = value;
  herr_t err = -1;
  QString attributeKey = QH5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.isEmpty() == false);
  attributeKey = "ScalarAttribute<" + attributeKey + ">";

  err = QH5Lite::readScalarAttribute(file_id, dsetName, attributeKey, value);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(refValue == value);
  return err;
}


// -----------------------------------------------------------------------------
//  Uses Raw Pointers to save data to the data file
// -----------------------------------------------------------------------------
template <typename T>
herr_t testWritePointer2DArrayDataset(hid_t file_id)
{

  T value = 0x0;
  herr_t err = 1;
  int32_t rank = 2;
  // Create the Dimensions
  hsize_t dims[2];
  dims[0] = DIM0;
  dims[1] = DIM1;

  /* Make dataset char */
  int32_t tSize = dims[0] * dims[1];
// T data[dimx*dimy];
  QVector<T> data(tSize);
  for (int32_t i = 0; i < tSize; ++i)
  {
    data[i] = static_cast<T>( i * 5);
  }

  QString dsetName = QH5Lite::HDFTypeForPrimitiveAsStr(value);
  dsetName = "Pointer2DArrayDataset<" + dsetName + ">";
  qDebug() << "Running " << dsetName << " ... ";
  err = QH5Lite::writePointerDataset( file_id, dsetName, rank, dims, &(data.front()) );
  DREAM3D_REQUIRE(err >= 0);
  err = testWritePointer1DArrayAttribute<int8_t>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<uint8_t>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<int16_t>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<uint16_t>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<int32_t>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<uint32_t>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<int64_t>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<uint64_t>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<float32>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<float64>(file_id, dsetName);

  err = testWritePointer2DArrayAttribute<int8_t>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<uint8_t>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<int16_t>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<uint16_t>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<int32_t>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<uint32_t>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<int64_t>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<uint64_t>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<float32>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<float64>(file_id, dsetName);

  err = testWritePointer3DArrayAttribute<int8_t>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<uint8_t>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<int16_t>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<uint16_t>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<int32_t>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<uint32_t>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<int64_t>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<uint64_t>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<float32>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<float64>(file_id, dsetName);

  err = testWriteVectorAttribute<int8_t>(file_id, dsetName);
  err = testWriteVectorAttribute<uint8_t>(file_id, dsetName);
  err = testWriteVectorAttribute<int16_t>(file_id, dsetName);
  err = testWriteVectorAttribute<uint16_t>(file_id, dsetName);
  err = testWriteVectorAttribute<int32_t>(file_id, dsetName);
  err = testWriteVectorAttribute<uint32_t>(file_id, dsetName);
  err = testWriteVectorAttribute<int64_t>(file_id, dsetName);
  err = testWriteVectorAttribute<uint64_t>(file_id, dsetName);
  err = testWriteVectorAttribute<float32>(file_id, dsetName);
  err = testWriteVectorAttribute<float64>(file_id, dsetName);

  err = testWriteScalarAttribute<int8_t>(file_id, dsetName);
  err = testWriteScalarAttribute<uint8_t>(file_id, dsetName);
  err = testWriteScalarAttribute<int16_t>(file_id, dsetName);
  err = testWriteScalarAttribute<uint16_t>(file_id, dsetName);
  err = testWriteScalarAttribute<int32_t>(file_id, dsetName);
  err = testWriteScalarAttribute<uint32_t>(file_id, dsetName);
  err = testWriteScalarAttribute<int64_t>(file_id, dsetName);
  err = testWriteScalarAttribute<uint64_t>(file_id, dsetName);
  err = testWriteScalarAttribute<float32>(file_id, dsetName);
  err = testWriteScalarAttribute<float64>(file_id, dsetName);

  qDebug() << " Passed" << "\n";
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testWriteVectorDataset(hid_t file_id)
{
  T value = 0x0;
  herr_t err = 1;

  QVector<hsize_t> dims(RANK_1D, DIM0);
  /* Make dataset char */
  QVector<T> data(DIM0, 0);
  for (int32_t i = 0; i < DIM0; ++i)
  {
    data[i] = static_cast<T>( i * 5);
  }

  QString dsetName = QH5Lite::HDFTypeForPrimitiveAsStr(value);
  dsetName = "VectorDataset<" + dsetName + ">";
  qDebug() << "Running " << dsetName << " ... ";
  err = QH5Lite::writeVectorDataset<T>( file_id, dsetName, dims, data );
  DREAM3D_REQUIRE(err >= 0);

  qDebug() << " Passed" << "\n";
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testReadVectorDataset(hid_t file_id)
{
  T value = 0x0;
  herr_t err = 1;

  std::vector<hsize_t> dims(RANK_1D, DIM0);
  std::vector<T> referenceData(DIM0, 0);
  for (int32_t i = 0; i < DIM0; ++i)
  {
    referenceData[i] = static_cast<T>( i * 5);
  }

  QString dsetName = QH5Lite::HDFTypeForPrimitiveAsStr(value);
  qDebug() << "Running testReadVectorDataset <" + dsetName + "> ... ";
  dsetName = "VectorDataset<" + dsetName + ">";

  std::vector<T> data;
  err = QH5Lite::readVectorDataset( file_id, dsetName, data );
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(data == referenceData);

  qDebug() << " Passed" << "\n";
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testWriteScalarDataset(hid_t file_id)
{
  T value = 0x0F;
  herr_t err = 1;

  QString dsetName = QH5Lite::HDFTypeForPrimitiveAsStr(value);
  dsetName = "ScalarDataset<" + dsetName + ">";
  qDebug() << "Running " << dsetName << " ... ";
  err = QH5Lite::writeScalarDataset(file_id, dsetName, value );
  DREAM3D_REQUIRE(err >= 0);

  bool exists = QH5Lite::datasetExists(file_id, dsetName);
  DREAM3D_REQUIRE_EQUAL(exists, true);

  exists = QH5Lite::datasetExists(file_id, QString("DOES_NOT_EXIST") );
  DREAM3D_REQUIRE_EQUAL(exists, false);

  qDebug() << " Passed" << "\n";
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testReadScalarDataset(hid_t file_id)
{
  T value = 0x0F;
  T refValue = value;
  herr_t err = 1;

  QString dsetName = QH5Lite::HDFTypeForPrimitiveAsStr<T>(value);
  qDebug() << "Running testReadScalarDataset <" + dsetName + "> ... ";
  dsetName = "ScalarDataset<" + dsetName + ">";

  err = QH5Lite::readScalarDataset(file_id, dsetName, value );
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(refValue == value );

  qDebug() << " Passed" << "\n";
  return err;
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testMXAAttribute(hid_t file_id, const QString& dsetName)
{
  T value = 0x0F;
  herr_t err = -1;
  QString attributeKey = QH5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.isEmpty() == false);
  attributeKey = "MXAAttribute<" + attributeKey + ">";
  IMXAArray* array = MXAArrayTemplate<T>::New(10);
  IMXAArray::Pointer arrayPtr (array);
  T* p = static_cast<T*>(array->getVoidPointer(0));
  for (int var = 0; var < 10; ++var)
  {
    p[var] = static_cast<T>(var + 65);
  }
  err = QH5Lite::writeMXAAttribute(file_id, dsetName, attributeKey, array);
  DREAM3D_REQUIRE(err >= 0);

  // Now Read the Attribute back into an MXAArray object and test against the previous for equality
  IMXAArray* rArray = QH5Lite::readMXAAttribute(file_id, dsetName, attributeKey);
  DREAM3D_REQUIRE (rArray != NULL);
  //hid_t t = rArray->getDataType();
  IMXAArray::Pointer rArrayPtr(rArray); // Let boost clean up the pointer
  T* r = static_cast<T*>(rArrayPtr->getVoidPointer(0));
//  for (int var = 0; var < 10; ++var) {
//    qDebug() << "p=" << p[var] << "  r=" << (r[var]) << "\n";
//  }
  DREAM3D_REQUIRE( ::memcmp(r, p, sizeof(T) * 10) == 0);



  AbstractH5Attribute::Pointer ptr = H5Attribute::ReadH5Attribute(file_id, dsetName, attributeKey);
  DREAM3D_REQUIRE(ptr.get() != NULL);
  r = static_cast<T*>(ptr->getAttributeValue()->getVoidPointer(0));
  DREAM3D_REQUIRE( ::memcmp(r, p, sizeof(T) * 10) == 0);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testWriteMXAArray(hid_t file_id)
{
  herr_t err = 1;
  T value = 0x0F;

  QString dsetName = QH5Lite::HDFTypeForPrimitiveAsStr<T>(value);
  dsetName = "MXAArrayDataset<" + dsetName + ">";
  qDebug() << "Running " << dsetName << " ... ";

  IMXAArray* array = MXAArrayTemplate<T>::New(10);
  IMXAArray::Pointer arrayPtr (array);
  T* p = static_cast<T*>(array->getVoidPointer(0));
  for (int var = 0; var < 10; ++var)
  {
    p[var] = static_cast<T>(var);
  }
  err = QH5Lite::writeMXAArray(file_id, dsetName, array);
  DREAM3D_REQUIRE(err >= 0);

  DREAM3D_REQUIRE ( testMXAAttribute<int8_t>(file_id, dsetName) >= 0 );
  DREAM3D_REQUIRE ( testMXAAttribute<uint8_t>(file_id, dsetName) >= 0 );
  DREAM3D_REQUIRE ( testMXAAttribute<int16_t>(file_id, dsetName) >= 0 );
  DREAM3D_REQUIRE ( testMXAAttribute<uint16_t>(file_id, dsetName) >= 0 );
  DREAM3D_REQUIRE ( testMXAAttribute<int32_t>(file_id, dsetName) >= 0 );
  DREAM3D_REQUIRE ( testMXAAttribute<uint32_t>(file_id, dsetName) >= 0 );
  DREAM3D_REQUIRE ( testMXAAttribute<int64_t>(file_id, dsetName) >= 0 );
  DREAM3D_REQUIRE ( testMXAAttribute<uint64_t>(file_id, dsetName) >= 0 );
  DREAM3D_REQUIRE ( testMXAAttribute<float32>(file_id, dsetName) >= 0 );
  DREAM3D_REQUIRE ( testMXAAttribute<float64>(file_id, dsetName) >= 0 );

  // Now Read the Attribute back into an MXAArray object and test against the previous for equality
  IMXAArray* rArray = QH5Lite::readMXAArray(file_id, dsetName);
  DREAM3D_REQUIRE (rArray != NULL);
  IMXAArray::Pointer rArrayPtr(rArray); // Let boost clean up the pointer
  T* r = static_cast<T*>(rArrayPtr->getVoidPointer(0));
//  for (int var = 0; var < 10; ++var) {
//    qDebug() << "p=" << p[var] << "  r=" << (r[var]) << "\n";
//  }
  DREAM3D_REQUIRE( ::memcmp(r, p, sizeof(T) * 10) == 0);

  qDebug() << " Passed" << "\n";
  return err;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t testWriteStringDatasetAndAttributes(hid_t file_id)
{
  qDebug() << "Running testWriteStringDatasetAndAttributes ......";
  herr_t err = 0;
  QString dsetName ("QString");
  QString dsetNameBytes("c_string");
  QString attributeKey("QString");

  QString strData ("THIS IS THE DATA");
  QByteArray strDataBytes("THIS IS THE DATA AS BYTES");
  QString attrData("THIS IS THE ATTRIBUTE DATA");
  QByteArray attrDataBytes("THIS IS THE ATTRIBUTE DATA AS BYTES");


  // Write a String Data set using a QString as the data source
  err = QH5Lite::writeStringDataset(file_id, dsetName, strData);
  DREAM3D_REQUIRE(err >= 0);

  // Over Write the data from Above but with different values
  strData = "THIS IS XXX DATA";
  err = QH5Lite::writeStringDataset(file_id, dsetName, strData);
  DREAM3D_REQUIRE(err >= 0);

  // Over Write the data from Above but with longer string
  strData = "THIS IS LONGER DATA";
  err = QH5Lite::writeStringDataset(file_id, dsetName, strData);
  DREAM3D_REQUIRE(err >= 0);

  // Over Write the data from Above but with shorter string
  strData = "THIS IS LESS DATA";
  err = QH5Lite::writeStringDataset(file_id, dsetName, strData);
  DREAM3D_REQUIRE(err >= 0);

  // Over Write the data from Above but with even shorter string
  strData = "Even LESS DATA";
  err = QH5Lite::writeStringDataset(file_id, dsetName, strData);
  DREAM3D_REQUIRE(err >= 0);

  // Over Write the data from Above but with even shorter string
  strData = "THIS IS THE DATA";
  err = QH5Lite::writeStringDataset(file_id, dsetName, strData);
  DREAM3D_REQUIRE(err >= 0);

  // Write a String attribute using a QString as the data source
  err = QH5Lite::writeStringAttribute(file_id, dsetName, attributeKey, attrData);
  DREAM3D_REQUIRE(err >= 0);

  attributeKey = "c_string";
  err = QH5Lite::writeStringAttribute(file_id, dsetName, attributeKey, attrDataBytes.size() + 1, attrDataBytes.data());


// Write a String data set using char* as data source
  err = QH5Lite::writeStringDataset(file_id, dsetNameBytes, strDataBytes.size() + 1, strDataBytes.data());
  DREAM3D_REQUIRE(err >= 0);

  // Write a bunch of Attributes using a Map structure.
  QMap<QString, QString> attrMap;
  attrMap[attributeKey] = attrData;
  attributeKey = "QString";
  attrMap[attributeKey] = attrData;

  err = QH5Lite::writeStringAttributes(file_id, dsetNameBytes, attrMap);
  DREAM3D_REQUIRE(err >= 0);

  qDebug() << " Passed";
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t HDF5Dump_datatype(hid_t dataset_id, std::ostream& iostr)
{
  herr_t status = 0;

  hid_t type_id = H5Dget_type(dataset_id);

  iostr << "    ";
  iostr << "DATATYPE \"";

  switch (H5Tget_class(type_id))
  {
    case H5T_INTEGER:
      iostr << "Integer";
      break;

    case H5T_FLOAT:
      if (H5Tequal(type_id, H5T_IEEE_F32BE) ||
          H5Tequal(type_id, H5T_IEEE_F32LE) ||
          H5Tequal(type_id, H5T_NATIVE_FLOAT))
      {
        // Float
        iostr << "Float";

      }
      else if (H5Tequal(type_id, H5T_IEEE_F64BE) ||
               H5Tequal(type_id, H5T_IEEE_F64LE) ||
               H5Tequal(type_id, H5T_NATIVE_DOUBLE) ||
               H5Tequal(type_id, H5T_NATIVE_LDOUBLE))
      {
        // Double
        iostr << "Double";

      }
      else
      {
        iostr << "Undefined HDF5 float.";
      }
      break;

    case H5T_STRING:
      iostr << "String - Unsupported";
      break;

    case H5T_COMPOUND:
      iostr << "Compound - Unsupported";
      break;

    default:
      iostr << "Unsupported or unknown data type";
      break;
  }

  iostr << "\"" << "\n";

  status = H5Tclose(type_id);

  return status;
}


// -----------------------------------------------------------------------------
//  Uses Raw Pointers to read data from the data file
// -----------------------------------------------------------------------------
template <typename T>
herr_t testReadPointer2DArrayDataset(hid_t file_id)
{
  // Create the reference data
  T value = 0x0;
  herr_t err = 1;
  QVector<hsize_t> dims(2, 0);
  dims[0] = DIM0;
  dims[1] = DIM1;

  int32_t tSize = dims[0] * dims[1];
  QVector<T> referenceData(tSize);
  for (int32_t i = 0; i < tSize; ++i)
  {
    referenceData[i] = static_cast<T>( i * 5);
  }
  // Generate the data set name
  QString dsetName = QH5Lite::HDFTypeForPrimitiveAsStr(value);
  qDebug() << "Running testReadPointer2DArrayDataset<" << dsetName << "> ... ";
  dsetName = "Pointer2DArrayDataset<" + dsetName + ">";

  H5T_class_t attr_type;
  size_t attr_size;

  err = QH5Lite::getDatasetInfo(file_id, "FOOBAR", dims, attr_type, attr_size);
  DREAM3D_REQUIRE(err < 0);

  err = QH5Lite::getDatasetInfo(file_id, dsetName, dims, attr_type, attr_size);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(dims.size() == 2);
  DREAM3D_REQUIRE(attr_size == sizeof(T));
  hid_t rank = 0;
  err = QH5Lite::getDatasetNDims(file_id, dsetName, rank);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(rank == 2);
  typename QVector<T>::size_type numElements = 1;
  for (QVector<hsize_t>::size_type i = 0; i < dims.size(); ++i)
  {
    numElements = numElements * dims[i];
  }


  herr_t retErr = 0;
  hid_t did = -1;
  /* Open the dataset. */
  if ( (did = H5Dopen( file_id, dsetName.toLatin1().data(), H5P_DEFAULT )) < 0 )
  {
    return -2;
  }
  /* Get an identifier for the datatype. */
  hid_t tid =  H5Dget_type( did );

  err = H5Dclose(did);
  if (err < 0)
  {
    qDebug() << "File: " << __FILE__ << "(" << __LINE__ << "): " << "Error Closing Dataset." << "\n";
    retErr = err;
  }
  if (retErr < 0)
  {
    return retErr;
  }

  DREAM3D_REQUIRE (tid > 0);
//  qDebug() << QH5Lite::StringForHDFType(tid) << "\n";
  err = H5Tclose(tid);
  DREAM3D_REQUIRE(err >= 0);

  hid_t dsType = QH5Lite::getDatasetType(file_id, dsetName);
  DREAM3D_REQUIRE (dsType > 0);
//  qDebug() << QH5Lite::StringForHDFType(dsType) << "\n";


  err = H5Tclose(dsType);
  DREAM3D_REQUIRE(err >= 0);

  QVector<T> data(numElements, 0);
  err = QH5Lite::readPointerDataset(file_id, dsetName, data.data() );
  DREAM3D_REQUIRE(err >= 0);
  // Compare the data...
  DREAM3D_REQUIRE(data == referenceData);

  //Read all the attributes
  err = testReadPointer1DArrayAttribute<int8_t>(file_id, dsetName);
  err = testReadPointer1DArrayAttribute<uint8_t>(file_id, dsetName);
  err = testReadPointer1DArrayAttribute<int16_t>(file_id, dsetName);
  err = testReadPointer1DArrayAttribute<uint16_t>(file_id, dsetName);
  err = testReadPointer1DArrayAttribute<int32_t>(file_id, dsetName);
  err = testReadPointer1DArrayAttribute<uint32_t>(file_id, dsetName);
  err = testReadPointer1DArrayAttribute<int64_t>(file_id, dsetName);
  err = testReadPointer1DArrayAttribute<uint64_t>(file_id, dsetName);
  err = testReadPointer1DArrayAttribute<float32>(file_id, dsetName);
  err = testReadPointer1DArrayAttribute<float64>(file_id, dsetName);

  err = testReadPointer2DArrayAttribute<int8_t>(file_id, dsetName);
  err = testReadPointer2DArrayAttribute<uint8_t>(file_id, dsetName);
  err = testReadPointer2DArrayAttribute<int16_t>(file_id, dsetName);
  err = testReadPointer2DArrayAttribute<uint16_t>(file_id, dsetName);
  err = testReadPointer2DArrayAttribute<int32_t>(file_id, dsetName);
  err = testReadPointer2DArrayAttribute<uint32_t>(file_id, dsetName);
  err = testReadPointer2DArrayAttribute<int64_t>(file_id, dsetName);
  err = testReadPointer2DArrayAttribute<uint64_t>(file_id, dsetName);
  err = testReadPointer2DArrayAttribute<float32>(file_id, dsetName);
  err = testReadPointer2DArrayAttribute<float64>(file_id, dsetName);

  err = testReadPointer3DArrayAttribute<int8_t>(file_id, dsetName);
  err = testReadPointer3DArrayAttribute<uint8_t>(file_id, dsetName);
  err = testReadPointer3DArrayAttribute<int16_t>(file_id, dsetName);
  err = testReadPointer3DArrayAttribute<uint16_t>(file_id, dsetName);
  err = testReadPointer3DArrayAttribute<int32_t>(file_id, dsetName);
  err = testReadPointer3DArrayAttribute<uint32_t>(file_id, dsetName);
  err = testReadPointer3DArrayAttribute<int64_t>(file_id, dsetName);
  err = testReadPointer3DArrayAttribute<uint64_t>(file_id, dsetName);
  err = testReadPointer3DArrayAttribute<float32>(file_id, dsetName);
  err = testReadPointer3DArrayAttribute<float64>(file_id, dsetName);
//
  err = testReadVectorAttribute<int8_t>(file_id, dsetName);
  err = testReadVectorAttribute<uint8_t>(file_id, dsetName);
  err = testReadVectorAttribute<int16_t>(file_id, dsetName);
  err = testReadVectorAttribute<uint16_t>(file_id, dsetName);
  err = testReadVectorAttribute<int32_t>(file_id, dsetName);
  err = testReadVectorAttribute<uint32_t>(file_id, dsetName);
  err = testReadVectorAttribute<int64_t>(file_id, dsetName);
  err = testReadVectorAttribute<uint64_t>(file_id, dsetName);
  err = testReadVectorAttribute<float32>(file_id, dsetName);
  err = testReadVectorAttribute<float64>(file_id, dsetName);

  err = testReadScalarAttribute<int8_t>(file_id, dsetName);
  err = testReadScalarAttribute<uint8_t>(file_id, dsetName);
  err = testReadScalarAttribute<int16_t>(file_id, dsetName);
  err = testReadScalarAttribute<uint16_t>(file_id, dsetName);
  err = testReadScalarAttribute<int32_t>(file_id, dsetName);
  err = testReadScalarAttribute<uint32_t>(file_id, dsetName);
  err = testReadScalarAttribute<int64_t>(file_id, dsetName);
  err = testReadScalarAttribute<uint64_t>(file_id, dsetName);
  err = testReadScalarAttribute<float32>(file_id, dsetName);
  err = testReadScalarAttribute<float64>(file_id, dsetName);

  qDebug() << " Passed" << "\n";
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void printData(QVector<T>& data, QVector<T>& rData)
{
// qDebug() << "Wrote/Read" << "\n";
  for (typename QVector<T>::iterator iter = data.begin(); iter < data.end(); ++iter )
  {
    qDebug() << *iter << " ";
  }
  qDebug() << "\n";

  for (typename QVector<T>::iterator iter = rData.begin(); iter < rData.end(); ++iter )
  {
    qDebug() << *iter << " ";
  }

  qDebug() << "\n";

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t testReadStringDatasetAndAttributes(hid_t file_id)
{
  qDebug() << "Running testReadStringDatasetAndAttributes ......";
  herr_t err = 0;

  QString dsetName ("QString");
  QString dsetNameBytes("c_string");
  QString attributeKey ("QString");

  QString refData ("THIS IS THE DATA");
  QByteArray refDataBytes("THIS IS THE DATA AS BYTES");
  QString refAttrData ("THIS IS THE ATTRIBUTE DATA");
  QByteArray refAttrDataBytes("THIS IS THE ATTRIBUTE DATA AS BYTES");


  QString strData ("");
  // Read the string as a QString from the file
  err = QH5Lite::readStringDataset(file_id, dsetName, strData);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE( refData.compare(strData) == 0);

  // Read the string again this time passing in a QString that has data in it which should get cleared
  strData = "FooBarJunk";
  err = QH5Lite::readStringDataset(file_id, dsetName, strData);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE( refData.compare(strData) == 0);

  // Read the Attributes
  QString attrData ("");
  // Read the Attribute as a QString and compare to the reference
  err = QH5Lite::readStringAttribute(file_id, dsetName, attributeKey, attrData);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE( refAttrData.compare(attrData) == 0);

  // Read the attribute as a byte array (US ASCII NULL TERM)
  QByteArray attrDataPtr(refAttrDataBytes.size(), 0); // Create a null terminated string initialized to all zeros
  attributeKey = "c_string";
  err = QH5Lite::readStringAttribute(file_id, dsetName, attributeKey, attrDataPtr.data() );
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE( ::memcmp(attrDataPtr.data(), refAttrDataBytes.data(), refAttrDataBytes.size()) == 0 );

  // Read a string data set which was written as a QByteArray into a QByteArray
  QByteArray strDataBytes(refDataBytes.size(), 0);
  err = QH5Lite::readStringDataset(file_id, dsetNameBytes, strDataBytes.data());
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE( strDataBytes == refDataBytes);

  QString mapString;
  err = QH5Lite::readStringAttribute(file_id, dsetNameBytes, dsetNameBytes, mapString);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE( refAttrData.compare(mapString) == 0);

  err = QH5Lite::readStringAttribute(file_id, dsetNameBytes, attributeKey, mapString);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE( refAttrData.compare(mapString) == 0);

  qDebug() << " Passed" << "\n";
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QH5LiteTest()
{
// herr_t err = -1;
  hid_t   file_id = 0;
  /* Create a new file using default properties. */
  file_id = H5Fcreate( UnitTest::H5LiteTest::FileName.toLatin1().data(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT );
  DREAM3D_REQUIRE(file_id > 0);
  //Create the Extra Groups
  hid_t sintGid = H5Gcreate(file_id, "Signed Int", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  hid_t uintGid = H5Gcreate(file_id, "Unsigned Int", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  hid_t pointer = H5Gcreate(file_id, "Pointer", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  herr_t err = H5Gclose(sintGid);
  if (err < 0)
  {
    qDebug() << "Error closing Group sintGid" << "\n";
    return;
  }
  err = H5Gclose(uintGid);
  if (err < 0)
  {
    qDebug() << "Error closing Group uintGid" << "\n";
    return;
  }
  err = H5Gclose(pointer);
  if (err < 0)
  {
    qDebug() << "Error closing Group c99Gid" << "\n";
    return;
  }

  // qDebug() << logTime() << "----------- Testing Writing/Reading of Datasets using Raw Pointers -----------" << "\n";
  DREAM3D_REQUIRE ( testWritePointer2DArrayDataset<int8_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWritePointer2DArrayDataset<uint8_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWritePointer2DArrayDataset<int16_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWritePointer2DArrayDataset<uint16_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWritePointer2DArrayDataset<int32_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWritePointer2DArrayDataset<uint32_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWritePointer2DArrayDataset<int64_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWritePointer2DArrayDataset<uint64_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWritePointer2DArrayDataset<float32>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWritePointer2DArrayDataset<float64>(file_id) >= 0);

  DREAM3D_REQUIRE ( testWriteVectorDataset<int8_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteVectorDataset<uint8_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteVectorDataset<int16_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteVectorDataset<uint16_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteVectorDataset<int32_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteVectorDataset<uint32_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteVectorDataset<int64_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteVectorDataset<uint64_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteVectorDataset<float32>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteVectorDataset<float64>(file_id) >= 0);

  DREAM3D_REQUIRE ( testWriteScalarDataset<int8_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteScalarDataset<uint8_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteScalarDataset<int16_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteScalarDataset<uint16_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteScalarDataset<int32_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteScalarDataset<uint32_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteScalarDataset<int64_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteScalarDataset<uint64_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteScalarDataset<float32>(file_id) >= 0);
  DREAM3D_REQUIRE ( testWriteScalarDataset<float64>(file_id) >= 0);

  DREAM3D_REQUIRE ( testWriteStringDatasetAndAttributes(file_id) >= 0);

//   DREAM3D_REQUIRE ( testWriteMXAArray<int8_t>(file_id) >= 0);
//   DREAM3D_REQUIRE ( testWriteMXAArray<uint8_t>(file_id) >= 0);
//   DREAM3D_REQUIRE ( testWriteMXAArray<int16_t>(file_id) >= 0);
//   DREAM3D_REQUIRE ( testWriteMXAArray<uint16_t>(file_id) >= 0);
//   DREAM3D_REQUIRE ( testWriteMXAArray<int32_t>(file_id) >= 0);
//   DREAM3D_REQUIRE ( testWriteMXAArray<uint32_t>(file_id) >= 0);
//   DREAM3D_REQUIRE ( testWriteMXAArray<int64_t>(file_id) >= 0);
//   DREAM3D_REQUIRE ( testWriteMXAArray<uint64_t>(file_id) >= 0);
//   DREAM3D_REQUIRE ( testWriteMXAArray<float32>(file_id) >= 0);
//   DREAM3D_REQUIRE ( testWriteMXAArray<float64>(file_id) >= 0);


//  // ******************* Test Reading Data *************************************
  DREAM3D_REQUIRE ( testReadPointer2DArrayDataset<int8_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadPointer2DArrayDataset<uint8_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadPointer2DArrayDataset<int16_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadPointer2DArrayDataset<uint16_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadPointer2DArrayDataset<int32_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadPointer2DArrayDataset<uint32_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadPointer2DArrayDataset<int64_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadPointer2DArrayDataset<uint64_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadPointer2DArrayDataset<float32>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadPointer2DArrayDataset<float64>(file_id) >= 0);


  DREAM3D_REQUIRE ( testReadVectorDataset<int8_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadVectorDataset<uint8_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadVectorDataset<int16_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadVectorDataset<uint16_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadVectorDataset<int32_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadVectorDataset<uint32_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadVectorDataset<int64_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadVectorDataset<uint64_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadVectorDataset<float32>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadVectorDataset<float64>(file_id) >= 0);
//
  DREAM3D_REQUIRE ( testReadScalarDataset<int8_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadScalarDataset<uint8_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadScalarDataset<int16_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadScalarDataset<uint16_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadScalarDataset<int32_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadScalarDataset<uint32_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadScalarDataset<int64_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadScalarDataset<uint64_t>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadScalarDataset<float32>(file_id) >= 0);
  DREAM3D_REQUIRE ( testReadScalarDataset<float64>(file_id) >= 0);

  DREAM3D_REQUIRE ( testReadStringDatasetAndAttributes(file_id) >= 0);

  /* Close the file. */
  H5Fclose( file_id );
// qDebug() << logTime() << "Testing Complete" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestLargeFileSupport()
{
  // herr_t err = -1;
  hid_t file_id;
  /* Create a new file using default properties. */
  file_id = H5Fcreate(UnitTest::H5LiteTest::LargeFile.toLatin1().data(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  DREAM3D_REQUIRE(file_id > 0);
  QVector<int > buffer(1000000); // Create a 4 MegaByte Buffer
  int32_t rank = 1;
  QVector<hsize_t > dims(1, 1000000);
  QString dsetName;
  for (int i = 0; i < 1000; ++i)
  {
    dsetName = "/" + QString::number(i);
    QH5Lite::writePointerDataset<int >(file_id, dsetName, rank, &(dims.front()), &(buffer.front()));
    qDebug() << "Large File " << i << "/1000" << "\n";

  }

  herr_t err = H5Fclose(file_id);
  DREAM3D_REQUIRE(err >= 0);

}

#define TYPE_DETECTION(m_msgType, check)\
  {\
    m_msgType v = 0x00;\
    hid_t t = QH5Lite::HDFTypeForPrimitive<m_msgType>(v);\
    DREAM3D_REQUIRE_EQUAL(t, check);\
  }


template<typename T>
hid_t _testTypeName()
{
  T v = 0x00;
  return QH5Lite::HDFTypeForPrimitive<T>(v);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestTypeDetection()
{

  DREAM3D_REQUIRE_EQUAL(_testTypeName<int8_t>(), H5T_NATIVE_INT8)
#if CMP_TYPE_CHAR_IS_SIGNED
  DREAM3D_REQUIRE_EQUAL(_testTypeName<char>(), H5T_NATIVE_INT8)
#else
  DREAM3D_REQUIRE_EQUAL(_testTypeName<char>(), H5T_NATIVE_UINT8)
#endif
  DREAM3D_REQUIRE_EQUAL(_testTypeName<signed char>(), H5T_NATIVE_INT8)
  DREAM3D_REQUIRE_EQUAL(_testTypeName<unsigned char>(), H5T_NATIVE_UINT8)
  DREAM3D_REQUIRE_EQUAL(_testTypeName<uint8_t>(), H5T_NATIVE_UINT8)

//  TYPE_DETECTION(uint8_t, H5T_NATIVE_UINT8);
//
//  TYPE_DETECTION(char, H5T_NATIVE_INT8);
//  TYPE_DETECTION(signed char, H5T_NATIVE_INT8);


}

#if 0
class WriteString
{
  public:
    WriteString (hid_t dataset, hid_t datatype
                 , hid_t dataspace, hid_t memspace)
      : m_dataset (dataset), m_datatype (datatype)
      , m_dataspace (dataspace), m_memspace (memspace)
      , m_pos () {}

  private:
    hid_t m_dataset;
    hid_t m_datatype;
    hid_t m_dataspace;
    hid_t m_memspace;
    int m_pos;
//...

  public:
    void operator ()(std::vector<std::string>::value_type const& v)
    {
      // Select the file position, 1 record at position 'pos'
      hsize_t count[] = { 1 } ;
      hsize_t offset[] = { m_pos++ } ;
      H5Sselect_hyperslab( m_dataspace
                           , H5S_SELECT_SET
                           , offset
                           , NULL
                           , count
                           , NULL );

      const char* s = v.c_str ();
      H5Dwrite (m_dataset
                , m_datatype
                , m_memspace
                , m_dataspace
                , H5P_DEFAULT
                , &s );
    }
};

void writeVector (hid_t group, std::vector<std::string> const& v)
{
  hsize_t     dims[] = { v.size ()  } ;
  hid_t dataspace = H5Screate_simple(sizeof(dims) / sizeof(*dims), dims, NULL);

  dims[0] = 1;
  hid_t memspace = H5Screate_simple(sizeof(dims) / sizeof(*dims), dims, NULL);

  hid_t datatype = H5Tcopy (H5T_C_S1);
  H5Tset_size (datatype, H5T_VARIABLE);

  hid_t dataset = H5Dcreate(group, "files", datatype, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

  //
  // Select the "memory" to be written out - just 1 record.
  hsize_t offset[] = { 0 } ;
  hsize_t count[] = { 1 } ;
  H5Sselect_hyperslab( memspace, H5S_SELECT_SET, offset, NULL, count, NULL );

  std::for_each (v.begin (), v.end (), WriteString (dataset, datatype, dataspace, memspace));

  H5Dclose (dataset);
  H5Sclose (dataspace);
  H5Sclose (memspace);
  H5Tclose (datatype);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestVLength()
{
  std::string data_set_name("Variable Length String Array");
  std::vector<std::string> strings;
  strings.push_back("A Test");
  strings.push_back("Another Test with \n a new line");
  strings.push_back("Some other string thing");

  hid_t file_id = H5Utilities::createFile(UnitTest::H5LiteTest::VLengthFile.toStdString());

  writeVector(file_id, strings);

  H5Utilities::closeFile(file_id);

  file_id = -1;

}

#else

class WriteString
{
  public:
    WriteString(hid_t dataset, hid_t datatype, hid_t dataspace, hid_t memspace)
      : m_dataset(dataset), m_datatype(datatype), m_dataspace(dataspace), m_memspace(memspace), m_pos() {}

  private:
    hid_t m_dataset;
    hid_t m_datatype;
    hid_t m_dataspace;
    hid_t m_memspace;
    int m_pos;
    //...

  public:
    void operator ()(std::vector<std::string>::value_type const& v)
    {
      // Select the file position, 1 record at position 'pos'
      hsize_t count[] = { 1 };
      hsize_t offset[] = { static_cast<hsize_t>(m_pos++) };
      H5Sselect_hyperslab(m_dataspace, H5S_SELECT_SET, offset, NULL, count, NULL);

      const char* s = v.c_str();
      H5Dwrite(m_dataset, m_datatype, m_memspace, m_dataspace, H5P_DEFAULT, &s);
    }
};

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
void TestVLengStringReadWrite()
{

  {
    hid_t file_id = H5Utilities::createFile(UnitTest::H5LiteTest::VLengthFile.toStdString());

    std::vector<std::string> strings;
    strings.push_back("Testing 1 2 3");
    strings.push_back("String with a\n   newline");
    strings.push_back("Some other String");
    strings.push_back("Titanium");
    strings.push_back("Nickel");

    herr_t err = H5Lite::writeVectorOfStringsDataset(file_id, "VlenStrings", strings);
    DREAM3D_REQUIRE(err >= 0)

    H5Utilities::closeFile(file_id);
  }

  {

    hid_t file_id = H5Utilities::openFile(UnitTest::H5LiteTest::VLengthFile.toStdString(), true);

    std::vector<std::string> data;
    H5Lite::readVectorOfStringDataset(file_id, "VlenStrings", data);
    DREAM3D_REQUIRE(data.size() == 5)
  }

  {
    hid_t file_id = QH5Utilities::createFile(UnitTest::H5LiteTest::VLengthFile);

    QVector<QString> strings;
    strings.push_back("Testing 1 2 3");
    strings.push_back("String with a\n   newline");
    strings.push_back("Some other String");
    strings.push_back("Titanium");
    strings.push_back("Nickel");

    herr_t err = QH5Lite::writeVectorOfStringsDataset(file_id, "VlenStrings", strings);
    DREAM3D_REQUIRE(err >= 0)

    QH5Utilities::closeFile(file_id);
  }

  {

    hid_t file_id = QH5Utilities::openFile(UnitTest::H5LiteTest::VLengthFile, true);

    QVector<QString> data;
    QH5Lite::readVectorOfStringDataset(file_id, "VlenStrings", data);
    DREAM3D_REQUIRE(data.size() == 5)
#if 0
    /*
    * Open file and dataset.
    */

    hid_t dset = H5Dopen(file_id, "VlenStrings", H5P_DEFAULT);

    /*
    * Get the datatype.
    */
    hid_t filetype = H5Dget_type(dset);

    hsize_t dims[1] = { 0 };
    /*
    * Get dataspace and allocate memory for read buffer.
    */
    hid_t space = H5Dget_space(dset);
    int ndims = H5Sget_simple_extent_dims(space, dims, NULL);
    std::vector<char*> rdata(dims[0]);
    for (int i = 0; i < 5; i++)
    {
      rdata[i] = NULL;
    }

    /*
    * Create the memory datatype.
    */
    hid_t memtype = H5Tcopy(H5T_C_S1);
    herr_t status = H5Tset_size(memtype, H5T_VARIABLE);

    /*
    * Read the data.
    */
    status = H5Dread(dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, &(rdata.front()));

    /*
    * Output the data to the screen.
    */
    for (int i = 0; i < dims[0]; i++)
    {
      // printf("%s[%d]: %s\n", "VlenStrings", i, rdata[i].p);
      QString str = QString::fromLocal8Bit(rdata[i]);
      qDebug() << str;
    }

    /*
    * Close and release resources.  Note that H5Dvlen_reclaim works
    * for variable-length strings as well as variable-length arrays.
    * Also note that we must still free the array of pointers stored
    * in rdata, as H5Tvlen_reclaim only frees the data these point to.
    */
    status = H5Dvlen_reclaim(memtype, space, H5P_DEFAULT, &(rdata.front()));
    // free(rdata);
    status = H5Dclose(dset);
    status = H5Sclose(space);
    status = H5Tclose(filetype);
    status = H5Tclose(memtype);
#endif

    QH5Utilities::closeFile(file_id);
  }

}

#endif


// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST (TestVLengStringReadWrite() )

  DREAM3D_REGISTER_TEST( TestTypeDetection() )
  DREAM3D_REGISTER_TEST( QH5LiteTest() )
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )

  PRINT_TEST_SUMMARY();
  return err;
}

