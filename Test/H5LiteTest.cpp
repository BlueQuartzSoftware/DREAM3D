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

#include <iostream>
#include <string>

//--MXA Includes
#include "MXA/MXA.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/StringUtils.h"

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

#include "UnitTestSupport.hpp"

#include "TestFileLocations.h"


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
void H5LiteTest();
//MXADataModel::Pointer createModelTemplate();
template<typename T>
herr_t testWritePointer1DArrayAttribute(hid_t file_id, const std::string &dsetName);



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  MXADir::remove(UnitTest::H5LiteTest::FileName);
  MXADir::remove(UnitTest::H5LiteTest::LargeFile);
#endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testWritePointer1DArrayAttribute(hid_t file_id, const std::string &dsetName)
{
  T value = 0x0;
  herr_t err = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.empty() == false);
  attributeKey = "1DArrayAttribute<" + attributeKey + ">";
  int32_t rank = 1;
  T data[DIM0];
  for(int i = 0; i < DIM0; ++i)
  {
    data[i] = (T)(i);
  }
  hsize_t dims[1];
  dims[0] = DIM0;

  err = H5Lite::writePointerAttribute<T>(file_id, dsetName, attributeKey, rank, dims, (T*)data);
  DREAM3D_REQUIRE(err >= 0);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testReadPointer1DArrayAttribute(hid_t file_id, const std::string &dsetName)
{
  T value = 0x0;
  herr_t err = -1;
  herr_t retErr = err;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.empty() == false);
  attributeKey = "1DArrayAttribute<" + attributeKey + ">";
  std::vector<T> referenceData(DIM0, 0);
  for(int i = 0; i < DIM0; ++i)
  {
    referenceData[i] = (T)(i);
  }

  H5T_class_t attr_type;
  size_t attr_size;
  hid_t typeId = -1;
  std::vector<hsize_t> dims;  //Reusable for the loop
  err = H5Lite::getAttributeInfo(file_id, dsetName, attributeKey, dims, attr_type, attr_size, typeId);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(dims.size() == 1);
  DREAM3D_REQUIRE(attr_size == sizeof(T));
  hid_t rank = 0;
  err = H5Lite::getAttributeNDims(file_id, dsetName, attributeKey, rank);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(rank == 1);
  CloseH5T(typeId, err, retErr); //Close the H5A type Id that was retrieved during the loop
  typename std::vector<T>::size_type numElements = 1;
  for (std::vector<uint64_t>::size_type i = 0; i < dims.size(); ++i)
  {
    numElements = numElements * dims[i];
  }
  std::vector<T> data(numElements, 0);
  err = H5Lite::readPointerAttribute<T>(file_id, dsetName, attributeKey, &(data.front() ) );
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE (data == referenceData);

  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testWritePointer2DArrayAttribute(hid_t file_id, const std::string &dsetName)
{
  //std::cout << DEBUG_OUT(logTime) << "testWritePointer2DArrayAttribute" << std::endl;
  T value = 0x0;
  herr_t err = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.empty() == false);

  attributeKey = "2DArrayAttribute<" + attributeKey + ">";
  int32_t rank = RANK_2D;
  T data[DIM0][DIM1];
  for(int i = 0; i < DIM0; ++i)
  {
    for (int j = 0; j < DIM1; ++j) {
      data[i][j] = (T)(i * j);
    }
  }
  hsize_t dims[RANK_2D];
  dims[0] = DIM0;
  dims[1] = DIM1;
  err = H5Lite::writePointerAttribute<T>(file_id, dsetName, attributeKey, rank, dims, (T*)data);
  DREAM3D_REQUIRE(err >= 0);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testReadPointer2DArrayAttribute(hid_t file_id, const std::string &dsetName)
{
  T value = 0x0;
  herr_t err = -1;
  herr_t retErr = err;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.empty() == false);
  attributeKey = "2DArrayAttribute<" + attributeKey + ">";
  T referenceData[DIM0][DIM1];
  for(int i = 0; i < DIM0; ++i)
  {
    for (int j = 0; j < DIM1; ++j) {
      referenceData[i][j] = (T)(i * j);
    }
  }
  H5T_class_t attr_type;
  size_t attr_size;
  hid_t typeId = -1;
  std::vector<hsize_t> dims;  //Reusable for the loop
  err = H5Lite::getAttributeInfo(file_id, dsetName, attributeKey, dims, attr_type, attr_size, typeId);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(dims.size() == 2);
  DREAM3D_REQUIRE(attr_size == sizeof(T));
  hid_t rank = 0;
  err = H5Lite::getAttributeNDims(file_id, dsetName, attributeKey, rank);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(rank == 2);

  CloseH5T(typeId, err, retErr); //Close the H5A type Id that was retrieved during the loop
  typename std::vector<T>::size_type numElements = 1;
  for (std::vector<uint64_t>::size_type i = 0; i < dims.size(); ++i)
  {
    numElements = numElements * dims[i];
  }
  std::vector<T> data(numElements, 0);
  err = H5Lite::readPointerAttribute<T>(file_id, dsetName, attributeKey, &(data.front() ) );
  DREAM3D_REQUIRE(err >= 0);

  DREAM3D_REQUIRE (::memcmp( &(data.front() ), referenceData, sizeof(T)*numElements) == 0);
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testWritePointer3DArrayAttribute(hid_t file_id, const std::string &dsetName)
{
  T value = 0x0;
  herr_t err = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.empty() == false);
  attributeKey = "3DArrayAttribute<" + attributeKey + ">";
  int32_t rank = RANK_3D;
  T data[DIM0][DIM1][DIM2];
  for(int i = 0; i < DIM0; ++i) {
    for (int j = 0; j < DIM1; ++j) {
      for (int k = 0; k < DIM2; ++k) {
        data[i][j][k] = (T)(i * j * k);
      }
    }
  }
  hsize_t dims[RANK_3D];
  dims[0] = DIM0;
  dims[1] = DIM1;
  dims[2] = DIM2;
  err = H5Lite::writePointerAttribute<T>(file_id, dsetName, attributeKey, rank, dims, (T*)data);
  DREAM3D_REQUIRE(err >= 0);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testReadPointer3DArrayAttribute(hid_t file_id, const std::string &dsetName)
{
  T value = 0x0;
  herr_t err = -1;
  herr_t retErr = err;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.empty() == false);
  attributeKey = "3DArrayAttribute<" + attributeKey + ">";
  T referenceData[DIM0][DIM1][DIM2];
  for(int i = 0; i < DIM0; ++i) {
    for (int j = 0; j < DIM1; ++j) {
      for (int k = 0; k < DIM2; ++k) {
        referenceData[i][j][k] = (T)(i * j * k);
      }
    }
  }
  H5T_class_t attr_type;
  size_t attr_size;
  hid_t typeId = -1;
  std::vector<hsize_t> dims;  //Reusable for the loop
  err = H5Lite::getAttributeInfo(file_id, dsetName, attributeKey, dims, attr_type, attr_size, typeId);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(dims.size() == 3);
  DREAM3D_REQUIRE(attr_size == sizeof(T));
  hid_t rank = 0;
  err = H5Lite::getAttributeNDims(file_id, dsetName, attributeKey, rank);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(rank == 3);
  CloseH5T(typeId, err, retErr); //Close the H5A type Id that was retrieved during the loop
  typename std::vector<T>::size_type numElements = 1;
  for (std::vector<uint64_t>::size_type i = 0; i < dims.size(); ++i)
  {
    numElements = numElements * dims[i];
  }
  std::vector<T> data(numElements, 0);
  err = H5Lite::readPointerAttribute<T>(file_id, dsetName, attributeKey, &(data.front() ) );
  DREAM3D_REQUIRE(err >= 0);

  DREAM3D_REQUIRE (::memcmp( &(data.front() ), referenceData, sizeof(T)*numElements) == 0);
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
herr_t testWriteVectorAttribute(hid_t file_id, std::string dsetName )
{
  T value = 0x0;
  herr_t err = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.empty() == false);
  attributeKey = "VectorAttribute<" + attributeKey + ">";

  int32_t numElements = DIM0;
  std::vector<hsize_t> dims (1, DIM0);

  /* Make dataset */
  std::vector<T> data (DIM0, 0);
  for (int i = 0; i < numElements; ++i) {
    data[i] = (T)(i);
  }
  //std::cout << "Attribute->Write: " << objName;
  err = H5Lite::writeVectorAttribute( file_id, dsetName, attributeKey, dims, data );
  DREAM3D_REQUIRE(err >= 0);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
herr_t testReadVectorAttribute(hid_t file_id, std::string dsetName )
{
  T value = 0x0;
  herr_t err = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.empty() == false);
  attributeKey = "VectorAttribute<" + attributeKey + ">";

  int32_t numElements = DIM0;
  std::vector<hsize_t> dims (1, DIM0);

  std::vector<T> data (DIM0, 0);
  for (int i = 0; i < numElements; ++i) {
    data[i] = (T)(i);
  }
  std::vector<T> rData(numElements, 0); //allocate and zero out the memory
  err = H5Lite::readVectorAttribute(file_id, dsetName, attributeKey, rData);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE( data == rData );
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testWriteScalarAttribute(hid_t file_id, const std::string &dsetName)
{
  T value = 0x0F;
  herr_t err = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.empty() == false);
  attributeKey = "ScalarAttribute<" + attributeKey + ">";
  err = H5Lite::writeScalarAttribute(file_id, dsetName, attributeKey, value);
  DREAM3D_REQUIRE(err >= 0);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testReadScalarAttribute(hid_t file_id, const std::string &dsetName)
{
  T value = 0x0F;
  T refValue = value;
  herr_t err = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.empty() == false);
  attributeKey = "ScalarAttribute<" + attributeKey + ">";

  err = H5Lite::readScalarAttribute(file_id, dsetName, attributeKey, value);
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
  dims[0] = DIM0; dims[1] = DIM1;

  /* Make dataset char */
  int32_t tSize = dims[0] * dims[1];
 // T data[dimx*dimy];
  std::vector<T> data(tSize);
  for (int32_t i = 0; i < tSize; ++i) {
    data[i] = static_cast<T>( i * 5);
  }

  std::string dsetName = H5Lite::HDFTypeForPrimitiveAsStr(value);
  dsetName = "Pointer2DArrayDataset<" + dsetName + ">";
  std::cout << "Running " << dsetName << " ... ";
  err = H5Lite::writePointerDataset( file_id, dsetName, rank, dims, &(data.front()) );
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

  std::cout << " Passed" << std::endl;
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

  std::vector<hsize_t> dims(RANK_1D, DIM0);
  /* Make dataset char */
  std::vector<T> data(DIM0, 0);
  for (int32_t i = 0; i < DIM0; ++i) {
    data[i] = static_cast<T>( i * 5);
  }

  std::string dsetName = H5Lite::HDFTypeForPrimitiveAsStr(value);
  dsetName = "VectorDataset<" + dsetName + ">";
  std::cout << "Running " << dsetName << " ... ";
  err = H5Lite::writeVectorDataset( file_id, dsetName, dims, data );
  DREAM3D_REQUIRE(err >= 0);

  std::cout << " Passed" << std::endl;
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
  for (int32_t i = 0; i < DIM0; ++i) {
    referenceData[i] = static_cast<T>( i * 5);
  }

  std::string dsetName = H5Lite::HDFTypeForPrimitiveAsStr(value);
  std::cout << "Running testReadVectorDataset <" + dsetName + "> ... ";
  dsetName = "VectorDataset<" + dsetName + ">";

  std::vector<T> data;
  err = H5Lite::readVectorDataset( file_id, dsetName, data );
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(data == referenceData);

  std::cout << " Passed" << std::endl;
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

  std::string dsetName = H5Lite::HDFTypeForPrimitiveAsStr(value);
  dsetName = "ScalarDataset<" + dsetName + ">";
  std::cout << "Running " << dsetName << " ... ";
  err = H5Lite::writeScalarDataset(file_id, dsetName, value );
  DREAM3D_REQUIRE(err >= 0);

  bool exists = H5Lite::datasetExists(file_id, dsetName);
  DREAM3D_REQUIRE_EQUAL(exists, true);

  exists = H5Lite::datasetExists(file_id, "DOES_NOT_EXIST");
  DREAM3D_REQUIRE_EQUAL(exists, false);

  std::cout << " Passed" << std::endl;
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

  std::string dsetName = H5Lite::HDFTypeForPrimitiveAsStr<T>(value);
  std::cout << "Running testReadScalarDataset <" + dsetName + "> ... ";
  dsetName = "ScalarDataset<" + dsetName + ">";

  err = H5Lite::readScalarDataset(file_id, dsetName, value );
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(refValue == value );

  std::cout << " Passed" << std::endl;
  return err;
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testMXAAttribute(hid_t file_id, const std::string &dsetName)
{
  T value = 0x0F;
  herr_t err = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.empty() == false);
  attributeKey = "MXAAttribute<" + attributeKey + ">";
  IMXAArray* array = MXAArrayTemplate<T>::New(10);
  IMXAArray::Pointer arrayPtr (array);
  T* p = static_cast<T*>(array->getVoidPointer(0));
  for (int var = 0; var < 10; ++var) {
    p[var] = static_cast<T>(var+65);
  }
  err = H5Lite::writeMXAAttribute(file_id, dsetName, attributeKey, array);
  DREAM3D_REQUIRE(err >= 0);

  // Now Read the Attribute back into an MXAArray object and test against the previous for equality
  IMXAArray* rArray = H5Lite::readMXAAttribute(file_id, dsetName, attributeKey);
  DREAM3D_REQUIRE (rArray != NULL);
  //hid_t t = rArray->getDataType();
  IMXAArray::Pointer rArrayPtr(rArray); // Let boost clean up the pointer
  T* r = static_cast<T*>(rArrayPtr->getVoidPointer(0));
//  for (int var = 0; var < 10; ++var) {
//    std::cout << "p=" << p[var] << "  r=" << (r[var]) << std::endl;
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

  std::string dsetName = H5Lite::HDFTypeForPrimitiveAsStr<T>(value);
  dsetName = "MXAArrayDataset<" + dsetName + ">";
  std::cout << "Running " << dsetName << " ... ";

  IMXAArray* array = MXAArrayTemplate<T>::New(10);
  IMXAArray::Pointer arrayPtr (array);
  T* p = static_cast<T*>(array->getVoidPointer(0));
  for (int var = 0; var < 10; ++var) {
    p[var] = static_cast<T>(var);
  }
  err = H5Lite::writeMXAArray(file_id, dsetName, array);
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
   IMXAArray* rArray = H5Lite::readMXAArray(file_id, dsetName);
   DREAM3D_REQUIRE (rArray != NULL);
   IMXAArray::Pointer rArrayPtr(rArray); // Let boost clean up the pointer
   T* r = static_cast<T*>(rArrayPtr->getVoidPointer(0));
 //  for (int var = 0; var < 10; ++var) {
 //    std::cout << "p=" << p[var] << "  r=" << (r[var]) << std::endl;
 //  }
   DREAM3D_REQUIRE( ::memcmp(r, p, sizeof(T) * 10) == 0);

  std::cout << " Passed" << std::endl;
  return err;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t testWriteStringDatasetAndAttributes(hid_t file_id)
{
  std::cout << "Running testWriteStringDatasetAndAttributes ......";
  herr_t err = 0;
  std::string dsetName ("std::string");
  std::string strData ("THIS IS THE DATA");
  err = H5Lite::writeStringDataset(file_id, dsetName, strData);
  DREAM3D_REQUIRE(err >= 0);
  // Write the Attributes
  std::string attributeKey ("std::string");
  std::string attrData ("THIS IS THE ATTRIBUTE DATA");
  err = H5Lite::writeStringAttribute(file_id, dsetName, attributeKey, attrData);
  DREAM3D_REQUIRE(err >= 0);

  const char* attrDataPtr = attrData.c_str();
  attributeKey = "c_string";
  err = H5Lite::writeStringAttribute(file_id, dsetName, attributeKey, attrData.size() + 1, attrDataPtr);

  std::map<std::string, std::string> attrMap;
  attrMap[attributeKey] = attrData;
  attributeKey = "std::string";
  attrMap[attributeKey] = attrData;

  // Dataset via pointer (c_str)
  const char* strPtr = strData.c_str();
  dsetName = "c_string";
  err = H5Lite::writeStringDataset(file_id, dsetName, strData.size() + 1, strPtr);
  DREAM3D_REQUIRE(err >= 0);

  err = H5Lite::writeStringAttributes(file_id, dsetName, attrMap);
  DREAM3D_REQUIRE(err >= 0);

  std::cout << " Passed" << std::endl;
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t HDF5Dump_datatype(hid_t dataset_id, std::ostream &iostr)
{
  herr_t status = 0;

  hid_t type_id = H5Dget_type(dataset_id);

  iostr << "    ";
  iostr << "DATATYPE \"";

  switch (H5Tget_class(type_id)) {
  case H5T_INTEGER:
    iostr << "Integer";
    break;

  case H5T_FLOAT:
    if (H5Tequal(type_id, H5T_IEEE_F32BE) ||
  H5Tequal(type_id, H5T_IEEE_F32LE) ||
  H5Tequal(type_id, H5T_NATIVE_FLOAT)) {
      // Float
      iostr << "Float";

    } else if (H5Tequal(type_id, H5T_IEEE_F64BE) ||
         H5Tequal(type_id, H5T_IEEE_F64LE) ||
         H5Tequal(type_id, H5T_NATIVE_DOUBLE) ||
         H5Tequal(type_id, H5T_NATIVE_LDOUBLE)) {
      // Double
      iostr << "Double";

    } else {
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

  iostr << "\"" << std::endl;

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
  std::vector<hsize_t> dims(2, 0);
  dims[0] = DIM0; dims[1] = DIM1;

  int32_t tSize = dims[0] * dims[1];
  std::vector<T> referenceData(tSize);
  for (int32_t i = 0; i < tSize; ++i) {
    referenceData[i] = static_cast<T>( i * 5);
  }
  // Generate the data set name
  std::string dsetName = H5Lite::HDFTypeForPrimitiveAsStr(value);
  std::cout << "Running testReadPointer2DArrayDataset<" << dsetName << "> ... ";
  dsetName = "Pointer2DArrayDataset<" + dsetName + ">";

  H5T_class_t attr_type;
  size_t attr_size;

  err = H5Lite::getDatasetInfo(file_id, dsetName, dims, attr_type, attr_size);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(dims.size() == 2);
  DREAM3D_REQUIRE(attr_size == sizeof(T));
  hid_t rank = 0;
  err = H5Lite::getDatasetNDims(file_id, dsetName, rank);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE(rank == 2);
  typename std::vector<T>::size_type numElements = 1;
  for (std::vector<hsize_t>::size_type i = 0; i < dims.size(); ++i)
  {
    numElements = numElements * dims[i];
  }


  herr_t retErr = 0;
  hid_t did = -1;
  /* Open the dataset. */
  if ( (did = H5Dopen( file_id, dsetName.c_str(), H5P_DEFAULT )) < 0 )
  {
     return -2;
  }
  /* Get an identifier for the datatype. */
  hid_t tid =  H5Dget_type( did );

  err = H5Dclose(did);
  if (err < 0)
  {
    std::cout << "File: " << __FILE__ << "(" << __LINE__ << "): " << "Error Closing Dataset." << std::endl;
    retErr = err;
  }
  if (retErr < 0)
  {
    return retErr;
  }

  DREAM3D_REQUIRE (tid > 0);
//  std::cout << H5Lite::StringForHDFType(tid) << std::endl;
  err = H5Tclose(tid);
  DREAM3D_REQUIRE(err >= 0);

  hid_t dsType = H5Lite::getDatasetType(file_id, dsetName);
  DREAM3D_REQUIRE (dsType > 0);
//  std::cout << H5Lite::StringForHDFType(dsType) << std::endl;


  err = H5Tclose(dsType);
  DREAM3D_REQUIRE(err >= 0);

  std::vector<T> data(numElements, 0);
  err = H5Lite::readPointerDataset(file_id, dsetName, &(data.front() ) );
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

  std::cout << " Passed" << std::endl;
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void printData(std::vector<T> &data, std::vector<T> &rData) {
 // std::cout << "Wrote/Read" << std::endl;
  for (typename std::vector<T>::iterator iter = data.begin(); iter < data.end(); ++iter ) {
    std::cout << *iter << " ";
  }
  std::cout << std::endl;

  for (typename std::vector<T>::iterator iter = rData.begin(); iter < rData.end(); ++iter ) {
    std::cout << *iter << " ";
  }

  std::cout << std::endl;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t testReadStringDatasetAndAttributes(hid_t file_id)
{
  std::cout << "Running testReadStringDatasetAndAttributes ......";
  herr_t err = 0;

  std::string refData ("THIS IS THE DATA");
  std::string refAttrData ("THIS IS THE ATTRIBUTE DATA");


  std::string dsetName ("std::string");
  std::string strData ("");
  err = H5Lite::readStringDataset(file_id, dsetName, strData);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE( refData.compare(strData) == 0);


  strData = "FooBarJunk";
  err = H5Lite::readStringDataset(file_id, dsetName, strData);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE( refData.compare(strData) == 0);


  // Write the Attributes
  std::string attributeKey ("std::string");
  std::string attrData ("");
  err = H5Lite::readStringAttribute(file_id, dsetName, attributeKey, attrData);
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE( refAttrData.compare(attrData) == 0);

  std::vector<uint8_t> attrDataPtr (refAttrData.size() + 1, 0);
  attributeKey = "c_string";
  err = H5Lite::readStringAttribute(file_id, dsetName, attributeKey, &(attrDataPtr.front() ) );
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE( refAttrData.compare(attrData) == 0);

  // Dataset via pointer (c_str)
  std::vector<uint8_t> strDataPtr (refData.size() + 1, 0);
  dsetName = "c_string";
  err = H5Lite::readStringDataset(file_id, dsetName, &(strDataPtr.front() ) );
  DREAM3D_REQUIRE(err >= 0);
  DREAM3D_REQUIRE( ::memcmp(&(strDataPtr.front()), refData.c_str(), refData.size() + 1) == 0 );


  std::cout << " Passed" << std::endl;
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5LiteTest()
{
 // herr_t err = -1;
  hid_t   file_id = 0;
  /* Create a new file using default properties. */
  file_id = H5Fcreate( UnitTest::H5LiteTest::FileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT );
  DREAM3D_REQUIRE(file_id > 0);
  //Create the Extra Groups
  hid_t sintGid = H5Gcreate(file_id, "Signed Int", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  hid_t uintGid = H5Gcreate(file_id, "Unsigned Int", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  hid_t pointer = H5Gcreate(file_id, "Pointer", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  herr_t err = H5Gclose(sintGid);
  if (err < 0)
  {
    std::cout << "Error closing Group sintGid" << std::endl;
    return;
  }
  err = H5Gclose(uintGid);
  if (err < 0)
  {
    std::cout << "Error closing Group uintGid" << std::endl;
    return;
  }
  err = H5Gclose(pointer);
  if (err < 0)
  {
    std::cout << "Error closing Group c99Gid" << std::endl;
    return;
  }

  // std::cout << logTime() << "----------- Testing Writing/Reading of Datasets using Raw Pointers -----------" << std::endl;
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
 // std::cout << logTime() << "Testing Complete" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestLargeFileSupport()
{
  // herr_t err = -1;
  hid_t file_id;
  /* Create a new file using default properties. */
  file_id = H5Fcreate(UnitTest::H5LiteTest::LargeFile.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  DREAM3D_REQUIRE(file_id > 0);
  std::vector<int > buffer(1000000); // Create a 4 MegaByte Buffer
  int32_t rank = 1;
  std::vector<hsize_t > dims(1, 1000000);
  std::string dsetName;
  for (int i = 0; i < 1000; ++i)
  {
    dsetName = "/" + StringUtils::numToString(i);
    H5Lite::writePointerDataset<int >(file_id, dsetName, rank, &(dims.front()), &(buffer.front()));
    std::cout << "Large File " << i << "/1000" << std::endl;

  }

  herr_t err = H5Fclose(file_id);
  DREAM3D_REQUIRE(err >= 0);

}

#define TYPE_DETECTION(m_msgType, check)\
{\
  m_msgType v = 0x00;\
  hid_t t = H5Lite::HDFTypeForPrimitive<m_msgType>(v);\
  DREAM3D_REQUIRE_EQUAL(t, check);\
}


template<typename T>
hid_t _testTypeName()
{
  T v = 0x00;
  return H5Lite::HDFTypeForPrimitive<T>(v);
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

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( TestTypeDetection() )
  DREAM3D_REGISTER_TEST( H5LiteTest() )
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )

  PRINT_TEST_SUMMARY();
  return err;
}

