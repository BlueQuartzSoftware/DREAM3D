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

#include <iostream>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QVector>
#include <QtCore/QString>

#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"
#include "DREAM3DLib/Common/GbcdDataArray.hpp"

#include "UnitTestSupport.hpp"

#include "TestFileLocations.h"

#define NUM_ELEMENTS  10
#define NUM_COMPONENTS 2
#define NUM_TUPLES     5

#define NUM_ELEMENTS_2   20
#define NUM_COMPONENTS_2 2
#define NUM_TUPLES_2     10

#define NUM_ELEMENTS_3   16
#define NUM_COMPONENTS_3 2
#define NUM_TUPLES_3     8

#define NUM_ELEMENTS_4   16
#define NUM_COMPONENTS_4 4
#define NUM_TUPLES_4     4

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(UnitTest::DataArrayTest::TestFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void __TestCopyTuples()
{
  int err = 0;
  typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(NUM_ELEMENTS_2, "TestCopyTuples");
  DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);

  array->SetNumberOfComponents(NUM_COMPONENTS_2);
  for(size_t i = 0; i < NUM_TUPLES_2; ++i)
  {
    array->SetComponent(i, 0, static_cast<T>(i));
    array->SetComponent(i, 1, static_cast<T>(i));
  }

  err = array->CopyTuple(0, 1);
  DREAM3D_REQUIRE_EQUAL(0, err);
  err = array->CopyTuple(3, 2);
  DREAM3D_REQUIRE_EQUAL(0, err);
  err = array->CopyTuple(4, 5);
  DREAM3D_REQUIRE_EQUAL(0, err);
  err = array->CopyTuple(8, 9);
  DREAM3D_REQUIRE_EQUAL(0, err);
  err = array->CopyTuple(18, 19);
  DREAM3D_REQUIRE_EQUAL(-1, err);

  DREAM3D_REQUIRE_EQUAL(array->GetComponent(1, 0), 0);
  DREAM3D_REQUIRE_EQUAL(array->GetComponent(1, 1), 0);
  DREAM3D_REQUIRE_EQUAL(array->GetComponent(2, 0), 3);
  DREAM3D_REQUIRE_EQUAL(array->GetComponent(2, 1), 3);
  DREAM3D_REQUIRE_EQUAL(array->GetComponent(5, 0), 4);
  DREAM3D_REQUIRE_EQUAL(array->GetComponent(5, 1), 4);
  DREAM3D_REQUIRE_EQUAL(array->GetComponent(9, 0), 8);
  DREAM3D_REQUIRE_EQUAL(array->GetComponent(9, 1), 8);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestCopyTuples()
{
  __TestCopyTuples<int8_t>();
  __TestCopyTuples<uint8_t>();
  __TestCopyTuples<int16_t>();
  __TestCopyTuples<uint16_t>();
  __TestCopyTuples<int32_t>();
  __TestCopyTuples<uint32_t>();
  __TestCopyTuples<int64_t>();
  __TestCopyTuples<uint64_t>();
  __TestCopyTuples<float>();
  __TestCopyTuples<double>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void __TestEraseElements()
{
  // Test dropping of front elements only
  {
    typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(NUM_ELEMENTS, "Test1");
    DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);
    for(size_t i = 0; i < NUM_ELEMENTS; ++i)
    {
      array->SetComponent(i, 0, static_cast<T>(i) );
    }

    QVector<size_t> eraseElements;
    eraseElements.push_back(0);
    eraseElements.push_back(1);

    array->EraseTuples(eraseElements);

    DREAM3D_REQUIRE_EQUAL(array->GetValue(0), 2);
    DREAM3D_REQUIRE_EQUAL(array->GetValue(1), 3);
    DREAM3D_REQUIRE_EQUAL(array->GetValue(2), 4);
  }

  // Test Dropping of internal elements
  {
    typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(NUM_ELEMENTS_2, "Test2");
    DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);
    array->SetNumberOfComponents(NUM_COMPONENTS_2);
    for(size_t i = 0; i < NUM_TUPLES_2; ++i)
    {
      array->SetComponent(i, 0, static_cast<T>(i));
      array->SetComponent(i, 1, static_cast<T>(i));
    }

    QVector<size_t> eraseElements;
    eraseElements.push_back(3);
    eraseElements.push_back(6);
    eraseElements.push_back(8);

    array->EraseTuples(eraseElements);

    DREAM3D_REQUIRE_EQUAL(array->GetComponent(3, 0), 4);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(3, 1), 4);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(5, 0), 7);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(5, 1), 7);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(6, 0), 9);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(6, 1), 9);
  }

  // Test Dropping of internal elements
  {
    typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(NUM_ELEMENTS_2, "Test3");
    DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);
    array->SetNumberOfComponents(NUM_COMPONENTS_2);
    for(size_t i = 0; i < NUM_TUPLES_2; ++i)
    {
      array->SetComponent(i, 0, static_cast<T>(i));
      array->SetComponent(i, 1, static_cast<T>(i));
    }

    QVector<size_t> eraseElements;
    eraseElements.push_back(3);
    eraseElements.push_back(6);
    eraseElements.push_back(9);
    array->EraseTuples(eraseElements);

    DREAM3D_REQUIRE_EQUAL(array->GetComponent(3, 0), 4);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(3, 1), 4);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(5, 0), 7);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(5, 1), 7);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(6, 0), 8);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(6, 1), 8);
  }

  // Test Dropping of internal continuous elements
  {
    typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(NUM_ELEMENTS_2, "Test4");
    DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);
    array->SetNumberOfComponents(NUM_COMPONENTS_2);
    for(size_t i = 0; i < NUM_TUPLES_2; ++i)
    {
      array->SetComponent(i, 0, static_cast<T>(i));
      array->SetComponent(i, 1, static_cast<T>(i));
    }

    QVector<size_t> eraseElements;
    eraseElements.push_back(3);
    eraseElements.push_back(4);
    eraseElements.push_back(5);
    array->EraseTuples(eraseElements);

    DREAM3D_REQUIRE_EQUAL(array->GetComponent(3, 0), 6);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(3, 1), 6);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(4, 0), 7);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(4, 1), 7);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(5, 0), 8);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(5, 1), 8);
  }

  // Test Dropping of Front and Back Elements
  {
    typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(NUM_ELEMENTS_2, "Test5");
    DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);
    array->SetNumberOfComponents(NUM_COMPONENTS_2);
    for(size_t i = 0; i < NUM_TUPLES_2; ++i)
    {
      array->SetComponent(i, 0, static_cast<T>(i));
      array->SetComponent(i, 1, static_cast<T>(i));
    }

    QVector<size_t> eraseElements;
    eraseElements.push_back(0);
    eraseElements.push_back(9);

    array->EraseTuples(eraseElements);

    DREAM3D_REQUIRE_EQUAL(array->GetComponent(0, 0), 1);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(0, 1), 1);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(7, 0), 8);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(7, 1), 8);
  }

  // Test Dropping of Back Elements
  {
    typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(NUM_ELEMENTS_2, "Test6");
    DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);
    array->SetNumberOfComponents(NUM_COMPONENTS_2);
    for(size_t i = 0; i < NUM_TUPLES_2; ++i)
    {
      array->SetComponent(i, 0, static_cast<T>(i));
      array->SetComponent(i, 1, static_cast<T>(i));
    }

    QVector<size_t> eraseElements;
    eraseElements.push_back(7);
    eraseElements.push_back(8);
    eraseElements.push_back(9);
    array->EraseTuples(eraseElements);

    DREAM3D_REQUIRE_EQUAL(array->GetComponent(4, 0), 4);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(4, 1), 4);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(5, 0), 5);
    DREAM3D_REQUIRE_EQUAL(array->GetComponent(5, 1), 5);
  }

  // Test Dropping of indices larger than the number of tuples
  {
    typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(NUM_TUPLES_2, NUM_COMPONENTS_2, "Test6");
    DREAM3D_REQUIRE_EQUAL(array->isAllocated(), true);
    for(size_t i = 0; i < NUM_TUPLES_2; ++i)
    {
      array->SetComponent(i, 0, static_cast<T>(i));
      array->SetComponent(i, 1, static_cast<T>(i));
    }

    QVector<size_t> eraseElements;
    eraseElements.push_back(10);
    int err = array->EraseTuples(eraseElements);
    DREAM3D_REQUIRE_EQUAL(err , -100)

        eraseElements.clear();
    err = array->EraseTuples(eraseElements);
    DREAM3D_REQUIRE_EQUAL(err , 0)

        eraseElements.resize(20);
    err = array->EraseTuples(eraseElements);
    DREAM3D_REQUIRE_EQUAL(err , 0)
        size_t nTuples = array->getNumberOfTuples();
    DREAM3D_REQUIRE_EQUAL(nTuples, 0)
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestEraseElements()
{
  __TestEraseElements<int8_t>();
  __TestEraseElements<uint8_t>();
  __TestEraseElements<int16_t>();
  __TestEraseElements<uint16_t>();
  __TestEraseElements<int32_t>();
  __TestEraseElements<uint32_t>();
  __TestEraseElements<int64_t>();
  __TestEraseElements<uint64_t>();
  __TestEraseElements<float>();
  __TestEraseElements<double>();
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestDataArray()
{
  int32_t* ptr = NULL;
  {
    Int32ArrayType::Pointer d = Int32ArrayType::CreateArray(0, "Test7");
    DREAM3D_REQUIRE_EQUAL(0, d->GetSize());
    DREAM3D_REQUIRE_EQUAL(0, d->getNumberOfTuples());
    ptr = d->getPointer(0);
    DREAM3D_REQUIRE_EQUAL(ptr, 0);
    DREAM3D_REQUIRE_EQUAL(d->isAllocated(), false);
  }

  {
    Int32ArrayType::Pointer int32Array = Int32ArrayType::CreateArray(NUM_ELEMENTS, "Test8");
    ptr = int32Array->getPointer(0);
    DREAM3D_REQUIRE_EQUAL(int32Array->isAllocated(), true);
    DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS, int32Array->getNumberOfTuples());
    DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS, int32Array->GetSize());
    int32Array->SetNumberOfComponents(NUM_COMPONENTS);
    DREAM3D_REQUIRE_EQUAL(NUM_TUPLES, int32Array->getNumberOfTuples());
    DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS, int32Array->GetSize());

    for (int i = 0; i < NUM_TUPLES; ++i)
    {
      for (int c = 0; c < NUM_COMPONENTS; ++c)
      {
        int32Array->SetComponent(i, c, i + c);
      }
    }

    // Resize Larger
    int32Array->Resize(NUM_TUPLES_2);
    DREAM3D_REQUIRE_EQUAL(NUM_TUPLES_2, int32Array->getNumberOfTuples());
    DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS_2, int32Array->GetSize());
    DREAM3D_REQUIRE_EQUAL(int32Array->isAllocated(), true);

    // This should have saved our data so lets look at the data and compare it
    for (int i = 0; i < NUM_TUPLES; ++i)
    {
      for (int c = 0; c < NUM_COMPONENTS; ++c)
      {
        DREAM3D_REQUIRE_EQUAL( (int32Array->GetComponent(i, c)), (i+c))
      }
    }

    // Resize Smaller - Which should have still save some of our data
    int32Array->Resize(NUM_TUPLES_3);
    DREAM3D_REQUIRE_EQUAL(NUM_TUPLES_3, int32Array->getNumberOfTuples());
    DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS_3, int32Array->GetSize());
    DREAM3D_REQUIRE_EQUAL(int32Array->isAllocated(), true);

    // This should have saved our data so lets look at the data and compare it
    for (int i = 0; i < NUM_TUPLES; ++i)
    {
      for (int c = 0; c < NUM_COMPONENTS; ++c)
      {
        DREAM3D_REQUIRE_EQUAL( (int32Array->GetComponent(i, c)), (i+c))
      }
    }

    // Change number of components
    int32Array->SetNumberOfComponents(NUM_COMPONENTS_4);
    DREAM3D_REQUIRE_EQUAL(NUM_TUPLES_4, int32Array->getNumberOfTuples());
    DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS_4, int32Array->GetSize());

    double temp = 9999;
    int32Array->InitializeTuple(0, temp );
    for (int c = 0; c < NUM_COMPONENTS; ++c)
    {
      DREAM3D_REQUIRE_EQUAL( (int32Array->GetComponent(0, c)), (9999))
    }


    ptr = int32Array->getPointer(0);
  }

}

// -----------------------------------------------------------------------------
//  Test the NeighborList Class
// -----------------------------------------------------------------------------
template<typename T>
void __TestNeighborList()
{
  typename NeighborList<T>::Pointer n = NeighborList<T>::New();
  n->SetName("Test");

  for(int i = 0; i < 4; ++i) {
    for(T j = 0; j < (T)(i+4); ++j) {
      n->addEntry(i, static_cast<T>(j*i+3) );
    }
  }

  typename NeighborList<T>::SharedVectorType v;
  for(int i = 0; i < 4; ++i)
  {
    v = n->getList(i);
    DREAM3D_REQUIRE_NE(v.get(), 0);
  }

  // Remove the front 2 elements and test
  QVector<size_t> eraseElements;
  eraseElements.push_back(0);
  eraseElements.push_back(1);

  n->EraseTuples(eraseElements);
  for(int i = 0; i < 2; ++i)
  {
    v = n->getList(i);
    DREAM3D_REQUIRE_NE(v.get(), 0);
    DREAM3D_REQUIRE_EQUAL(v->size(), static_cast<size_t>(i+2+4) );
    for(T j = 0; j < (T)(i+4+2); ++j) {
      DREAM3D_REQUIRE_EQUAL(v->at(j), j*(i+2)+3);
    }
  }

  // Reset and erase the back 2 "Tuples"
  n->clearAllLists();
  for(int i = 0; i < 4; ++i) {
    for(T j = 0; j < (T)(i+4); ++j) {
      n->addEntry(i, j*i+3);
    }
  }
  eraseElements.clear();
  eraseElements.push_back(2);
  eraseElements.push_back(3);
  n->EraseTuples(eraseElements);
  for(int i = 0; i < 2; ++i)
  {
    v = n->getList(i);
    DREAM3D_REQUIRE_NE(v.get(), 0);
    DREAM3D_REQUIRE_EQUAL(v->size(), static_cast<size_t>(i+4) );
    for(T j = 0; j < (T)(i+4); ++j) {
      DREAM3D_REQUIRE_EQUAL(v->at(j), j*i+3);
    }
  }

  // Reset and erase the back 2 "Tuples"
  n->clearAllLists();
  for(int i = 0; i < 4; ++i) {
    for(T j = 0; j < (T)(i+4); ++j) {
      n->addEntry(i, j*i+3);
    }
  }
  eraseElements.clear();
  eraseElements.push_back(1);
  eraseElements.push_back(2);
  n->EraseTuples(eraseElements);
  int i = 0;
  v = n->getList(i);
  DREAM3D_REQUIRE_NE(v.get(), 0);
  DREAM3D_REQUIRE_EQUAL(v->size(), static_cast<size_t>(i+4) );
  for(T j = 0; j < (T)(i+4); ++j) {
    DREAM3D_REQUIRE_EQUAL(v->at(j), j*i+3);
  }
  i = 1;
  v = n->getList(i);
  DREAM3D_REQUIRE_NE(v.get(), 0);
  i=3;
  DREAM3D_REQUIRE_EQUAL(v->size(), static_cast<size_t>(i+4) );
  for(T j = 0; j < (T)(i+4); ++j) {
    DREAM3D_REQUIRE_EQUAL(v->at(j), j*i+3);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestNeighborList()
{
  __TestNeighborList<int8_t>();
  __TestNeighborList<uint8_t>();
  __TestNeighborList<int16_t>();
  __TestNeighborList<uint16_t>();
  __TestNeighborList<int32_t>();
  __TestNeighborList<uint32_t>();
  __TestNeighborList<int64_t>();
  __TestNeighborList<uint64_t>();
  __TestNeighborList<float>();
  __TestNeighborList<double>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestGbcdDataArray()
{

  size_t dims[5] = {60,29,60,60,29};
  size_t size = dims[0]*dims[1]*dims[2]*dims[3]*dims[4];
  GBCDFloatArrayType::Pointer m_GBCD = GBCDFloatArrayType::CreateArray(dims, "GBCD Data", false);

  size_t retDims[5];
  m_GBCD->GetGbcdDimension(retDims);
  DREAM3D_REQUIRE_EQUAL(retDims[0], dims[0])
  DREAM3D_REQUIRE_EQUAL(retDims[1], dims[1])
  DREAM3D_REQUIRE_EQUAL(retDims[2], dims[2])
  DREAM3D_REQUIRE_EQUAL(retDims[3], dims[3])
  DREAM3D_REQUIRE_EQUAL(retDims[4], dims[4])
  DREAM3D_REQUIRE_EQUAL(false, m_GBCD->isAllocated())

  DREAM3D_REQUIRE_EQUAL(size, m_GBCD->GetSize())

  // This will test writing an array that has not been allocated which should return an
  // negative error code.
  {
    hid_t fid = QH5Utilities::createFile(UnitTest::DataArrayTest::TestFile);
    DREAM3D_REQUIRE(fid > 0)
        int err = m_GBCD->writeH5Data(fid);
    DREAM3D_REQUIRE(err < 0)
        QH5Utilities::closeFile(fid);
  }

  // Now allocate all the memory we need
  m_GBCD->Allocate();
  DREAM3D_REQUIRE_EQUAL(true, m_GBCD->isAllocated())
      float* ptr = m_GBCD->getPointer(0);
  size_t idx = 0;
  // Now dump some actual data in the arrays
  for(size_t i = 0; i < dims[0]; ++i)
  {
    for(size_t j = 0; j < dims[1]; ++j)
    {
      for(size_t k = 0; k < dims[2]; ++k)
      {
        for(size_t l = 0; l < dims[3]; ++l)
        {
          for(size_t m = 0; m < dims[4]; ++m)
          {
            ptr[idx] = static_cast<float>(i+j+k+l+m);
            idx++;
          }
        }
      }
    }
  }


  // This will test writing an array that has been allocated which should return an
  // Zero or Positive error code.
  {
    hid_t fid = QH5Utilities::createFile(UnitTest::DataArrayTest::TestFile);
    DREAM3D_REQUIRE(fid > 0)
        int err = m_GBCD->writeH5Data(fid);
    DREAM3D_REQUIRE(err >= 0)
        QH5Utilities::closeFile(fid);
  }

  {
    hid_t fid = QH5Utilities::openFile(UnitTest::DataArrayTest::TestFile, true);
    DREAM3D_REQUIRE(fid > 0)
        m_GBCD->initializeWithZeros();
    int err = m_GBCD->readH5Data(fid);
    DREAM3D_REQUIRE(err >= 0)
        QH5Utilities::closeFile(fid);
    ptr = m_GBCD->getPointer(0);
    idx = 0;
    // Now dump some actual data in the arrays
    for(size_t i = 0; i < dims[0]; ++i)
    {
      for(size_t j = 0; j < dims[1]; ++j)
      {
        for(size_t k = 0; k < dims[2]; ++k)
        {
          for(size_t l = 0; l < dims[3]; ++l)
          {
            for(size_t m = 0; m < dims[4]; ++m)
            {
              DREAM3D_REQUIRE(ptr[idx] == i+j+k+l+m)
              idx++;
            }
          }
        }
      }
    }

  }

}

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;

  QDir dir(UnitTest::DataArrayTest::TestDir);
dir.mkpath(".");

#if !REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
    #endif

      DREAM3D_REGISTER_TEST( TestDataArray() )
      DREAM3D_REGISTER_TEST( TestEraseElements() )
      DREAM3D_REGISTER_TEST( TestCopyTuples() )
      DREAM3D_REGISTER_TEST( TestNeighborList() )
      DREAM3D_REGISTER_TEST( TestGbcdDataArray() )

    #if REMOVE_TEST_FILES
      DREAM3D_REGISTER_TEST( RemoveTestFiles() )
    #endif


      PRINT_TEST_SUMMARY();
  return err;
}

