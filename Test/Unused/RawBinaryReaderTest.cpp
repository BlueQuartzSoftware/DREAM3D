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



#include <stdlib.h>
#include <stdio.h>

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/ScopedFileMonitor.hpp"
#include "SIMPLib/IOFilters/RawBinaryReader.h"


#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"

/* Testing Notes:
 *
 *  testCase1: This tests when the file size is equal to the allocated size, and checks to see if the data read is the same as the data written.
 *
 *  testCase2: This tests when the file size is smaller than the allocated size. (Reading past the end of the file)
 *
 *  testCase3: This tests when the file size is larger than the allocated size and there is junk at the end of the file.
 *
 *  testCase4: This tests when the file size is larger than the allocated size and there is junk at the beginning of the file.
 *
 *  testCase5: This tests when the file size is larger than the allocated size and there is junk at the beginning and end of the file.
 *
 *  testCase6: This tests when skipHeaderBytes equals the file size
 */


/** we are going to use a fairly large array size because we want to exercise the
 * possibilty that we can not write the data or read the data from the file in one
 * step in the filter
 */

#define X_DIM      100
#define Y_DIM      100
#define Z_DIM      1000
#define ARRAY_SIZE  (X_DIM * Y_DIM * Z_DIM)

#define RBRT_FILE_NOT_OPEN -1000
#define RBRT_FILE_TOO_SMALL -1010
#define RBRT_READ_EOF       -1030
#define RBRT_NO_ERROR       0

namespace Detail
{
  enum NumType
  {
    Int8 = 0,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Int64,
    UInt64,
    Float,
    Double,
    UnknownNumType
  };
  enum Endian
  {
    Little = 0,
    Big
  };

  enum JunkPlacement
  {
    None = 0,
    Start,
    End,
    Both
  };
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(UnitTest::RawBinaryReaderTest::OutputFile);
#endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
bool createAndWriteToFile(T* dataArray, size_t dataSize, T* junkArray, size_t junkSize, Detail::JunkPlacement junkPlacement)
{
  /* Any of these combinations will return failure:
   *      If the junkArray has values and junkPlacement is set to NONE
   *      If junkArray is NULL and junkPlacement is not set to NONE
   *      If junkPlacement is set to an invalid value
   *    If the dataArray is NULL
   */
  if ( (NULL != junkArray && junkPlacement <= 0) || (NULL == junkArray && junkPlacement > 0)
       || junkPlacement < 0 || dataArray == NULL)
  {
    return false;
  }

  // Create the output file to dump some data into
  FILE* f = fopen(UnitTest::RawBinaryReaderTest::OutputFile.toLatin1().data(), "wb");

  // If junkPlacement is set to START or BOTH, write junk to file
  size_t numWritten = 0;
  if (junkPlacement == Detail::Start || junkPlacement == Detail::Both)
  {
    while(1)
    {
      numWritten += fwrite(junkArray, sizeof(T), junkSize, f);
      if (numWritten == junkSize)
      {
        break;
      }
      junkArray = junkArray + numWritten;
    }
  }

  // Write the data to the file
  numWritten = 0;
  T* dataArrayFront = dataArray;
  while(1)
  {
    numWritten += fwrite(dataArray, sizeof(T), dataSize, f);
    if (numWritten == dataSize)
    {
      break;
    }
    dataArray = dataArray + numWritten;
  }

  // Reset the pointer back to the front of the array, and set dataArrayFront to NULL
  dataArrayFront = NULL;

  // If junkPlacement is set to END or BOTH, write junk to file
  numWritten = 0;
  if (junkPlacement == Detail::End || junkPlacement == Detail::Both)
  {
    while(1)
    {
      numWritten += fwrite(junkArray, sizeof(T), junkSize, f);
      if (numWritten == junkSize)
      {
        break;
      }
      junkArray = junkArray + numWritten;
    }
  }

  // Close the file
  fclose(f);

  // Return successful
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RawBinaryReader::Pointer createRawBinaryReaderFilter(int scalarType, size_t N, int skipBytes)
{
  RawBinaryReader::Pointer filt = RawBinaryReader::New();
  filt->setInputFile(UnitTest::RawBinaryReaderTest::OutputFile);
  filt->setScalarType(scalarType);
  filt->setEndian(Detail::Little);
  filt->setDimensionality(3);
  filt->setNumberOfComponents(N);
  filt->setSkipHeaderBytes(skipBytes);
  IntVec3Widget_t dims;
  dims.x = X_DIM;
  dims.y = Y_DIM;
  dims.z = Z_DIM;
  filt->setDimensions(dims);
  FloatVec3Widget_t origin;
  origin.x = origin.y = origin.z = 1.0f;
  filt->setOrigin(origin);
  FloatVec3Widget_t spacing;
  spacing.x = spacing.y = spacing.z = 0.55f;
  filt->setResolution(spacing);
  filt->setOutputArrayName("Test_Array");

  return filt;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// testCase1: This tests when the file size is equal to the allocated size, and checks to see if the data read is the same as the data written.
template<typename T, size_t N>
int testCase1_Execute(const QString& name, int scalarType)
{
  int err = 0;
  int dataArraySize = ARRAY_SIZE * N;
  int junkArraySize = 0;
  int skipHeaderBytes = 0;
  qDebug() << "Testing case 1: " << name << " with num comps " << N;


  // Allocate an array, and get the dataArray from that array
  boost::shared_array<T> array(new T[dataArraySize]); // This makes sure our allocated array is deleted when we leave
  T* dataArray = array.get();

  // Write some data into the data array
  for(size_t i = 0; i < dataArraySize; ++i)
  {
    dataArray[i] = static_cast<T>(i);
  }

  // Create junkArray and set it to NULL because there is no junk in this test case
  T* junkArray = NULL;

  // Create the file and write to it.  If any of the information is wrong, the result will be false
  bool result = createAndWriteToFile(dataArray, dataArraySize, junkArray, junkArraySize, Detail::None);

  // Test to make sure that the file was created and written to successfully
  DREAM3D_REQUIRED(result, == , true)

  // Create the data container
  VolumeDataContainer::Pointer m = VolumeDataContainer::New();
  m->setName(DREAM3D::Defaults::DataContainerName);
  DataContainerArray::Pointer dca = DataContainerArray::New();
  dca->pushBack(m);

  // Create the filter, passing in the skipHeaderBytes
  RawBinaryReader::Pointer filt = createRawBinaryReaderFilter(scalarType, N, skipHeaderBytes);
  filt->setDataContainerArray(dca);

  // Preflight, get the error condition, and check that there are no errors
  filt->preflight();
  err = filt->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)

  // Execute the filter, check that there are no errors, and compare the data
  filt->execute();
  DREAM3D_REQUIRED(err, >= , 0)

  IDataArray::Pointer iData = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray("Test_Array");
  T* data = reinterpret_cast<T*>(iData->getVoidPointer(0));
  T d, p;
  for(size_t i = 0; i < dataArraySize; ++i)
  {
    d = data[i];
    p = dataArray[i];
    DREAM3D_REQUIRE_EQUAL(d, p)
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void testCase1_TestPrimitives(const QString& name, int scalarType)
{
  testCase1_Execute<T, 1>(name, scalarType);
  testCase1_Execute<T, 2>(name, scalarType);
  testCase1_Execute<T, 3>(name, scalarType);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void testCase1()
{
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QDir dir(UnitTest::RawBinaryReaderTest::TestDir);
  if(!dir.mkpath("."))
  {
    qDebug() << "Error creating parent path '" << UnitTest::RawBinaryReaderTest::TestDir << "'";
    return;
  }


  testCase1_TestPrimitives<int8_t>("int8_t", Detail::Int8);
  testCase1_TestPrimitives<uint8_t>("uint8_t", Detail::UInt8);
  testCase1_TestPrimitives<int16_t>("int16_t", Detail::Int16);
  testCase1_TestPrimitives<uint16_t>("uint16_t", Detail::UInt16);
  testCase1_TestPrimitives<int32_t>("int32_t", Detail::Int32);
  testCase1_TestPrimitives<uint32_t>("uint32_t", Detail::UInt32);
  testCase1_TestPrimitives<int64_t>("int64_t", Detail::Int64);
  testCase1_TestPrimitives<uint64_t>("uint64_t", Detail::UInt64);
  testCase1_TestPrimitives<float>("float", Detail::Float);
  testCase1_TestPrimitives<double>("double", Detail::Double);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// testCase2: This tests when the file size is smaller than the allocated size. (Reading past the end of the file)
template<typename T, size_t N>
void testCase2_Execute(const QString& name, int scalarType)
{
  int err = 0;
  int dataArraySize = ARRAY_SIZE * N / 2;   // We don't care what is written...we just need the data array size to be less than the file size
  int junkArraySize = 0;
  int skipHeaderBytes = junkArraySize * sizeof(T);
  qDebug() << "Testing case 2: " << name << " with num comps " << N;

  // Allocate an array, and get the dataArray from that array
  boost::shared_array<T> array(new T[dataArraySize]); // This makes sure our allocated array is deleted when we leave
  T* dataArray = array.get();

  // Write some data into the data array
  for(size_t i = 0; i < dataArraySize; ++i)
  {
    dataArray[i] = static_cast<T>(i);
  }

  // Create junkArray and set it to NULL because there is no junk in this test case
  T* junkArray = NULL;

  // Create the file and write to it.  If any of the information is wrong, the result will be false
  bool result = createAndWriteToFile(dataArray, dataArraySize, junkArray, junkArraySize, Detail::None);

  // Test to make sure that the file was created and written to successfully
  DREAM3D_REQUIRED(result, == , true)

  // Create the data container
  VolumeDataContainer::Pointer m = VolumeDataContainer::New();
  m->setName(DREAM3D::Defaults::DataContainerName);
  DataContainerArray::Pointer dca = DataContainerArray::New();
  dca->pushBack(m);

  // Create the filter, passing in the skipHeaderBytes
  RawBinaryReader::Pointer filt = createRawBinaryReaderFilter(scalarType, N, skipHeaderBytes);
  filt->setDataContainerArray(dca);

  // Preflight, get error condition, and check that the "file is too small" error is returned
  filt->preflight();
  err = filt->getErrorCondition();
  DREAM3D_REQUIRED(err, == , RBRT_FILE_TOO_SMALL)

  // Execute, get error condition, and check that the "file is too small" error is returned
  filt->execute();
  err = filt->getErrorCondition();
  DREAM3D_REQUIRED(err, == , RBRT_FILE_TOO_SMALL)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void testCase2_TestPrimitives(const QString& name, int scalarType)
{
  testCase2_Execute<T, 1>(name, scalarType);
  testCase2_Execute<T, 2>(name, scalarType);
  testCase2_Execute<T, 3>(name, scalarType);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void testCase2()
{
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QDir dir(UnitTest::RawBinaryReaderTest::TestDir);
  if(!dir.mkpath("."))
  {
    qDebug() << "Error creating parent path '" << UnitTest::RawBinaryReaderTest::TestDir << "'";
    return;
  }



  testCase2_TestPrimitives<int8_t>("int8_t", Detail::Int8);
  testCase2_TestPrimitives<uint8_t>("uint8_t", Detail::UInt8);
  testCase2_TestPrimitives<int16_t>("int16_t", Detail::Int16);
  testCase2_TestPrimitives<uint16_t>("uint16_t", Detail::UInt16);
  testCase2_TestPrimitives<int32_t>("int32_t", Detail::Int32);
  testCase2_TestPrimitives<uint32_t>("uint32_t", Detail::UInt32);
  testCase2_TestPrimitives<int64_t>("int64_t", Detail::Int64);
  testCase2_TestPrimitives<uint64_t>("uint64_t", Detail::UInt64);
  testCase2_TestPrimitives<float>("float", Detail::Float);
  testCase2_TestPrimitives<double>("double", Detail::Double);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// testCase3: This tests when the file size is larger than the allocated size and there is junk at the end of the file.
template<typename T, size_t N>
void testCase3_Execute(const QString& name, int scalarType)
{
  int dataArraySize = ARRAY_SIZE * N;
  int junkArraySize = 10;
  int skipHeaderBytes = 0;
  int err = 0;
  qDebug() << "Testing case 3: " << name << " with num comps " << N;

  // Allocate an array, and get the dataArray from that array
  boost::shared_array<T> array(new T[dataArraySize]); // This makes sure our allocated array is deleted when we leave
  T* dataArray = array.get();

  // Write some data into the data array
  for(size_t i = 0; i < dataArraySize; ++i)
  {
    dataArray[i] = static_cast<T>(i);
  }

  // Create junkArray
  T* junkArray = new T[junkArraySize];

  // Write a pattern into junkArray
  for(size_t i = 0; i < junkArraySize; ++i)
  {
    junkArray[i] = (unsigned)0xAB;
  }

  // Create the file and write to it.  If any of the information is wrong, the result will be false
  bool result = createAndWriteToFile(dataArray, dataArraySize, junkArray, junkArraySize, Detail::End);

  // Test to make sure that the file was created and written to successfully
  DREAM3D_REQUIRED(result, == , true)

  // Create the data container
  VolumeDataContainer::Pointer m = VolumeDataContainer::New();
  m->setName(DREAM3D::Defaults::DataContainerName);
  DataContainerArray::Pointer dca = DataContainerArray::New();
  dca->pushBack(m);

  // Create the filter, passing in the skipHeaderBytes
  RawBinaryReader::Pointer filt = createRawBinaryReaderFilter(scalarType, N, skipHeaderBytes);
  filt->setDataContainerArray(dca);

  // Preflight, get the error condition, and check that there are no errors
  filt->preflight();
  err = filt->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)

  // Execute, get the error condition, check that there are no errors, and compare the data
  filt->execute();
  err = filt->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)

  IDataArray::Pointer iData = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray("Test_Array");
  T* readData = reinterpret_cast<T*>(iData->getVoidPointer(0));
  T d, p;
  for(size_t i = 0; i < dataArraySize; ++i)
  {
    d = readData[i];
    p = dataArray[i];
    DREAM3D_REQUIRE_EQUAL(d, p)
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void testCase3_TestPrimitives(const QString& name, int scalarType)
{
  testCase3_Execute<T, 1>(name, scalarType);
  testCase3_Execute<T, 2>(name, scalarType);
  testCase3_Execute<T, 3>(name, scalarType);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void testCase3()
{
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QDir dir(UnitTest::RawBinaryReaderTest::TestDir);
  if(!dir.mkpath("."))
  {
    qDebug() << "Error creating parent path '" << UnitTest::RawBinaryReaderTest::TestDir << "'";
    return;
  }


  testCase3_TestPrimitives<int8_t>("int8_t", Detail::Int8);
  testCase3_TestPrimitives<uint8_t>("uint8_t", Detail::UInt8);
  testCase3_TestPrimitives<int16_t>("int16_t", Detail::Int16);
  testCase3_TestPrimitives<uint16_t>("uint16_t", Detail::UInt16);
  testCase3_TestPrimitives<int32_t>("int32_t", Detail::Int32);
  testCase3_TestPrimitives<uint32_t>("uint32_t", Detail::UInt32);
  testCase3_TestPrimitives<int64_t>("int64_t", Detail::Int64);
  testCase3_TestPrimitives<uint64_t>("uint64_t", Detail::UInt64);
  testCase3_TestPrimitives<float>("float", Detail::Float);
  testCase3_TestPrimitives<double>("double", Detail::Double);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// testCase4: This tests when the file size is larger than the allocated size and there is junk at the beginning of the file.
template<typename T, size_t N>
void testCase4_Execute(const QString& name, int scalarType)
{
  int dataArraySize = ARRAY_SIZE * N;
  int junkArraySize = 5;
  int skipHeaderBytes = junkArraySize * sizeof(T);
  int err = 0;
  qDebug() << "Testing case 4: " << name << " with num comps " << N;

  // Allocate an array, and get the dataArray from that array
  boost::shared_array<T> array(new T[dataArraySize]); // This makes sure our allocated array is deleted when we leave
  T* dataArray = array.get();

  // Write some data into the data array
  for(size_t i = 0; i < dataArraySize; ++i)
  {
    dataArray[i] = static_cast<T>(i);
  }

  // Create junkArray
  T* junkArray = new T[junkArraySize];

  // Write a pattern into junkArray
  for(size_t i = 0; i < junkArraySize; ++i)
  {
    junkArray[i] = (unsigned)0xAB;
  }


  // Create the file and write to it.  If any of the information is wrong, the result will be false
  bool result = createAndWriteToFile(dataArray, dataArraySize, junkArray, junkArraySize, Detail::Start);

  // Test to make sure that the file was created and written to successfully
  DREAM3D_REQUIRED(result, == , true)

  // Create the data container
  VolumeDataContainer::Pointer m = VolumeDataContainer::New();
  m->setName(DREAM3D::Defaults::DataContainerName);
  DataContainerArray::Pointer dca = DataContainerArray::New();
  dca->pushBack(m);

  // Create the filter, passing in the skipHeaderBytes
  RawBinaryReader::Pointer filt = createRawBinaryReaderFilter(scalarType, N, skipHeaderBytes);
  filt->setDataContainerArray(dca);

  // Preflight, get error condition, and check that there are no errors
  filt->preflight();
  err = filt->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)

  // Execute, get error condition, check that there are no errors, and compare the data
  filt->execute();
  err = filt->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)

  IDataArray::Pointer iData = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray("Test_Array");
  T* data = reinterpret_cast<T*>(iData->getVoidPointer(0));
  T d, p;
  for(size_t i = 0; i < dataArraySize; ++i)
  {
    d = data[i];
    p = dataArray[i];
    DREAM3D_REQUIRE_EQUAL(d, p)
  }

  /*
   * SUBTEST: Test when skipHeaderBytes is larger than expected
   */

  // Create another data container
  VolumeDataContainer::Pointer m2 = VolumeDataContainer::New();
  m2->setName(DREAM3D::Defaults::DataContainerName);
  DataContainerArray::Pointer dca2 = DataContainerArray::New();
  dca2->pushBack(m2);

  // Create another filter, passing in the skipHeaderBytes + 1
  RawBinaryReader::Pointer filt2 = createRawBinaryReaderFilter(scalarType, N, skipHeaderBytes + 1);
  filt2->setDataContainerArray(dca2);

  // Preflight, get error condition, and check that there are errors
  filt2->preflight();
  err = filt2->getErrorCondition();
  DREAM3D_REQUIRED(err, < , 0)

  // Execute, get error condition, and check that the "file too small" error occurred
  filt2->execute();
  err = filt2->getErrorCondition();
  DREAM3D_REQUIRED(err, == , RBRT_FILE_TOO_SMALL)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void testCase4_TestPrimitives(const QString& name, int scalarType)
{
  testCase4_Execute<T, 1>(name, scalarType);
  testCase4_Execute<T, 2>(name, scalarType);
  testCase4_Execute<T, 3>(name, scalarType);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void testCase4()
{
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QDir dir(UnitTest::RawBinaryReaderTest::TestDir);
  if(!dir.mkpath("."))
  {
    qDebug() << "Error creating parent path '" << UnitTest::RawBinaryReaderTest::TestDir << "'";
    return;
  }


  testCase4_TestPrimitives<int8_t>("int8_t", Detail::Int8);
  testCase4_TestPrimitives<uint8_t>("uint8_t", Detail::UInt8);
  testCase4_TestPrimitives<int16_t>("int16_t", Detail::Int16);
  testCase4_TestPrimitives<uint16_t>("uint16_t", Detail::UInt16);
  testCase4_TestPrimitives<int32_t>("int32_t", Detail::Int32);
  testCase4_TestPrimitives<uint32_t>("uint32_t", Detail::UInt32);
  testCase4_TestPrimitives<int64_t>("int64_t", Detail::Int64);
  testCase4_TestPrimitives<uint64_t>("uint64_t", Detail::UInt64);
  testCase4_TestPrimitives<float>("float", Detail::Float);
  testCase4_TestPrimitives<double>("double", Detail::Double);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// testCase5: This tests when the file size is larger than the allocated size and there is junk both at the beginning and end of the file.
template<typename T, size_t N>
void testCase5_Execute(const QString& name, int scalarType)
{
  int dataArraySize = ARRAY_SIZE * N;
  int junkArraySize = 10;
  int skipHeaderBytes = junkArraySize * sizeof(T);
  int err = 0;
  qDebug() << "Testing case 5: " << name << " with num comps " << N;

  // Allocate an array, and get the dataArray from that array
  boost::shared_array<T> array(new T[dataArraySize]); // This makes sure our allocated array is deleted when we leave
  T* dataArray = array.get();

  // Write some data into the data array
  for(size_t i = 0; i < dataArraySize; ++i)
  {
    dataArray[i] = static_cast<T>(i);
  }

  // Create junkArray
  T* junkArray = new T[junkArraySize];

  // Write a pattern into junkArray
  for(size_t i = 0; i < junkArraySize; ++i)
  {
    junkArray[i] = (unsigned)0xAB;
  }


  // Create the file and write to it.  If any of the information is wrong, the result will be false
  bool result = createAndWriteToFile(dataArray, dataArraySize, junkArray, junkArraySize, Detail::Both);

  // Test to make sure that the file was created and written to successfully
  DREAM3D_REQUIRED(result, == , true)

  // Create the data container
  VolumeDataContainer::Pointer m = VolumeDataContainer::New();
  m->setName(DREAM3D::Defaults::DataContainerName);
  DataContainerArray::Pointer dca = DataContainerArray::New();
  dca->pushBack(m);

  // Create the filter, passing in the skipHeaderBytes
  RawBinaryReader::Pointer filt = createRawBinaryReaderFilter(scalarType, N, skipHeaderBytes);
  filt->setDataContainerArray(dca);

  // Preflight, get error condition, and check that there are no errors
  filt->preflight();
  err = filt->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)

  // Execute, get error condition, check that there are no errors, and compare the data
  filt->execute();
  err = filt->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)

  IDataArray::Pointer iData = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray("Test_Array");
  T* data = reinterpret_cast<T*>(iData->getVoidPointer(0));
  T d, p;
  for(size_t i = 0; i < dataArraySize; ++i)
  {
    d = data[i];
    p = dataArray[i];
    DREAM3D_REQUIRE_EQUAL(d, p)
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void testCase5_TestPrimitives(const QString& name, int scalarType)
{
  testCase5_Execute<T, 1>(name, scalarType);
  testCase5_Execute<T, 2>(name, scalarType);
  testCase5_Execute<T, 3>(name, scalarType);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void testCase5()
{
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QDir dir(UnitTest::RawBinaryReaderTest::TestDir);
  if(!dir.mkpath("."))
  {
    qDebug() << "Error creating parent path '" << UnitTest::RawBinaryReaderTest::TestDir << "'";
    return;
  }


  testCase5_TestPrimitives<int8_t>("int8_t", Detail::Int8);
  testCase5_TestPrimitives<uint8_t>("uint8_t", Detail::UInt8);
  testCase5_TestPrimitives<int16_t>("int16_t", Detail::Int16);
  testCase5_TestPrimitives<uint16_t>("uint16_t", Detail::UInt16);
  testCase5_TestPrimitives<int32_t>("int32_t", Detail::Int32);
  testCase5_TestPrimitives<uint32_t>("uint32_t", Detail::UInt32);
  testCase5_TestPrimitives<int64_t>("int64_t", Detail::Int64);
  testCase5_TestPrimitives<uint64_t>("uint64_t", Detail::UInt64);
  testCase5_TestPrimitives<float>("float", Detail::Float);
  testCase5_TestPrimitives<double>("double", Detail::Double);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// testCase6: This tests when skipHeaderBytes equals the file size
template<typename T, size_t N>
void testCase6_Execute(const QString& name, int scalarType)
{
  int dataArraySize = 0;
  int junkArraySize = ARRAY_SIZE * N;
  int skipHeaderBytes = junkArraySize * sizeof(T);
  int err = 0;
  qDebug() << "Testing case 6: " << name << " with num comps " << N;

  // Allocate an array, and get the dataArray from that array
  boost::shared_array<T> array(new T[dataArraySize]); // This makes sure our allocated array is deleted when we leave
  T* dataArray = array.get();

  // Write some data into the data array
  for(size_t i = 0; i < dataArraySize; ++i)
  {
    dataArray[i] = static_cast<T>(i);
  }

  // Create junkArray
  T* junkArray = new T[junkArraySize];

  // Write a pattern into junkArray
  for(size_t i = 0; i < junkArraySize; ++i)
  {
    junkArray[i] = (unsigned)0xAB;
  }


  // Create the file and write to it.  If any of the information is wrong, the result will be false
  bool result = createAndWriteToFile(dataArray, dataArraySize, junkArray, junkArraySize, Detail::Start);

  // Test to make sure that the file was created and written to successfully
  DREAM3D_REQUIRED(result, == , true)

  // Create the data container
  VolumeDataContainer::Pointer m = VolumeDataContainer::New();
  m->setName(DREAM3D::Defaults::DataContainerName);
  DataContainerArray::Pointer dca = DataContainerArray::New();
  dca->pushBack(m);

  // Create the filter, passing in the skipHeaderBytes
  RawBinaryReader::Pointer filt = createRawBinaryReaderFilter(scalarType, N, skipHeaderBytes);
  filt->setDataContainerArray(dca);

  // Preflight, get error condition, and check that the "file too small" error has occurred
  filt->preflight();
  err = filt->getErrorCondition();
  DREAM3D_REQUIRED(err, == , RBRT_FILE_TOO_SMALL)

  // Execute, get error condition, and check that there are errors
  filt->execute();
  err = filt->getErrorCondition();
  DREAM3D_REQUIRED(err, < , 0)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void testCase6_TestPrimitives(const QString& name, int scalarType)
{
  testCase6_Execute<T, 1>(name, scalarType);
  testCase6_Execute<T, 2>(name, scalarType);
  testCase6_Execute<T, 3>(name, scalarType);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void testCase6()
{
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QDir dir(UnitTest::RawBinaryReaderTest::TestDir);
  if(!dir.mkpath("."))
  {
    qDebug() << "Error creating parent path '" << UnitTest::RawBinaryReaderTest::TestDir << "'";
    return;
  }


  testCase6_TestPrimitives<int8_t>("int8_t", Detail::Int8);
  testCase6_TestPrimitives<uint8_t>("uint8_t", Detail::UInt8);
  testCase6_TestPrimitives<int16_t>("int16_t", Detail::Int16);
  testCase6_TestPrimitives<uint16_t>("uint16_t", Detail::UInt16);
  testCase6_TestPrimitives<int32_t>("int32_t", Detail::Int32);
  testCase6_TestPrimitives<uint32_t>("uint32_t", Detail::UInt32);
  testCase6_TestPrimitives<int64_t>("int64_t", Detail::Int64);
  testCase6_TestPrimitives<uint64_t>("uint64_t", Detail::UInt64);
  testCase6_TestPrimitives<float>("float", Detail::Float);
  testCase6_TestPrimitives<double>("double", Detail::Double);
}

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;


#if !REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif


  DREAM3D_REGISTER_TEST( testCase1() )
  DREAM3D_REGISTER_TEST( testCase2() )
  DREAM3D_REGISTER_TEST( testCase3() )
  DREAM3D_REGISTER_TEST( testCase4() )
  DREAM3D_REGISTER_TEST( testCase5() )
  DREAM3D_REGISTER_TEST( testCase6() )

#if REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif


  PRINT_TEST_SUMMARY();
  return err;
}
