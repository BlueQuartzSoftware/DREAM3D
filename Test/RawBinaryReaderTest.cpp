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


#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>

#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/MXAFileInfo.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/ScopedFileMonitor.hpp"
#include "DREAM3DLib/IOFilters/RawBinaryReader.h"


#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"


/** we are going to use a fairly large array size because we want to exercise the
 * possibilty that we can not write the data or read the data from the file in one
 * step in the filter
 */

#define X_DIM      100
#define Y_DIM      100
#define Z_DIM      1000
#define ARRAY_SIZE  (X_DIM * Y_DIM * Z_DIM)

namespace Detail
{
  enum NumType {
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
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  MXADir::remove(UnitTest::RawBinaryReaderTest::OutputFile);
#endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T, size_t N>
int testComponents(const std::string &name, int scalarType)
{
  int err = 0;
  std::cout << "Testing " << name << " with num comps " << N << std::endl;


  // Allocate an array
  boost::shared_array<T> array(new T[ARRAY_SIZE * N]); // This makes sure our allocated array is deleted when we leave
  T* ptr = array.get();
  // Write some data into the array
  for(size_t i = 0; i < ARRAY_SIZE * N; ++i)
  {
    ptr[i] = static_cast<T>(i);
  }

  {
    // Create the output file to dump some data into
    FILE* f = fopen(UnitTest::RawBinaryReaderTest::OutputFile.c_str(), "wb");
    DREAM3D_REQUIRE(f != NULL)
        ScopedFileMonitor monitor(f); // This makes sure the file gets closed if we go out of scope

    size_t numWritten = 0;
    while(1)
    {
      //std::cout << "write pass..." << std::endl;
      numWritten += fwrite(ptr, sizeof(T), ARRAY_SIZE * N, f);
      if (numWritten == ARRAY_SIZE * N)
      {
        break;
      }
      ptr = ptr + numWritten;
    }
  }
  // Reset the pointer back to the front of the array
  ptr = array.get();

  // Now that the temp file with some data is written we need to read it back up and test for equality
  // First we need a Voxel Data Container
  VoxelDataContainer::Pointer m = VoxelDataContainer::New();
  // Now we need the filter
  RawBinaryReader::Pointer filt = RawBinaryReader::New();
  filt->setInputFile(UnitTest::RawBinaryReaderTest::OutputFile);
  filt->setScalarType(scalarType);
  filt->setEndian(Detail::Little);
  filt->setDimensionality(3);
  filt->setNumberOfComponents(N);
  IntVec3Widget_t dims;
  dims.x = X_DIM;
  dims.y = Y_DIM;
  dims.z = Z_DIM;
  filt->setDimensions(dims);
  FloatVec3Widget_t origin;
  origin.x = origin.y = origin.z = 1.0;
  filt->setOrigin(origin);
  FloatVec3Widget_t spacing;
  spacing.x = spacing.y = spacing.z = 0.55;
  filt->setSpacing(spacing);
  filt->setOutputArrayName("Test_Array");

  filt->setVoxelDataContainer(m.get());
  filt->execute();

  IDataArray::Pointer iData = m->getCellData("Test_Array");
  T* data = reinterpret_cast<T*>(iData->GetVoidPointer(0));

  // Now we need to compare the arrays to make sure we read up the right values
  T d, p;
  for(size_t i = 0; i < ARRAY_SIZE * N; ++i)
  {
    d = data[i];
    p = ptr[i];
    DREAM3D_REQUIRE_EQUAL(d, p)
//    if (d != p)
//    {
//      std::cout << "Comparison failed d=" << d << std::endl;
//      std::cout << "                  p=" << p << std::endl;
//      std::cout << "              index=" << i << std::endl;
//      std::cout << "              dims: " << X_DIM << " " << Y_DIM << " " << Z_DIM << std::endl;
//      std::cout << "         Num Comps: " << N << std::endl;
//    }
  }
  return err;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
int testPrimitive(const std::string &name, int scalarType)
{
  int err = 0;
  testComponents<T, 1>(name, scalarType);
  testComponents<T, 2>(name, scalarType);
  testComponents<T, 3>(name, scalarType);
  return err;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReaderTest()
{
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  if(!MXADir::mkdir(UnitTest::RawBinaryReaderTest::TestDir, true))
  {
    std::cout << "Error creating parent path '" << UnitTest::RawBinaryReaderTest::TestDir << "'";
    return;
  }


  testPrimitive<int8_t>("int8_t", Detail::Int8);
  testPrimitive<uint8_t>("uint8_t", Detail::UInt8);
  testPrimitive<int16_t>("int16_t", Detail::Int16);
  testPrimitive<uint16_t>("uint16_t", Detail::UInt16);
  testPrimitive<int32_t>("int32_t", Detail::Int32);
  testPrimitive<uint32_t>("uint32_t", Detail::UInt32);
  testPrimitive<int64_t>("int64_t", Detail::Int64);
  testPrimitive<uint64_t>("uint64_t", Detail::UInt64);
  testPrimitive<float>("float", Detail::Float);
  testPrimitive<double>("double", Detail::Double);
}

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( ReaderTest() )


      PRINT_TEST_SUMMARY();
  return err;
}

