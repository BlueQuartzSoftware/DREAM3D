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

#include <iostream>
#include <string>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Geometry/MeshStructs.h"
#include "SIMPLib/DataArrays/StringDataArray.hpp"
#include "SIMPLib/FilterParameters/FilterParameter.h"


#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "DREAM3DTestFileLocations.h"

#define ARRAY_SIZE 10
#define RESIZE_SMALLER 5
#define RESIZE_LARGER 8
#define INIT_SIZE 0

/**
Funtions to test:
*  getTypeAsString()
*  getFullNameOfClass()
*  setName()
*  getName()
*  Allocate()
*  initialize()
*  initializeWithZeros()
*  initializeWithValues()
initializeTuple()
eraseTuples()
copyTuple()
*  getTypeSize()
*  getNumberOfTuples()
*  getSize()
*  getNumberOfComponents()
*  getVoidPointer()
*  getPointer()

*  RawResize()
*  Resize()
*  operator[]
*/

namespace
{
 static const QString _0("zero");
  static const QString _1("one");
 static const QString _2("two");
 static const QString _3("three");
 static const QString _4("four");
 static const QString _5("five");
 static const QString _6("six");
 static const QString _7("seven");
 static const QString _8("eight");
 static const QString _9("nine");
}


const QString kArrayName("Test Strings");

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StringDataArray::Pointer initializeStringDataArray()
{
  StringDataArray::Pointer data = StringDataArray::CreateArray(ARRAY_SIZE, kArrayName);

  size_t nTuples = data->getNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(ARRAY_SIZE, nTuples)
  bool isAllocated = data->isAllocated();
  DREAM3D_REQUIRE_EQUAL(isAllocated, true)

  int nComp = data->getNumberOfComponents();
  DREAM3D_REQUIRE_EQUAL(1, nComp)

  size_t size = data->getSize();
  DREAM3D_REQUIRE_EQUAL(ARRAY_SIZE, size)

  data->setValue(0, ::_0);
  data->setValue(1, ::_1);
  data->setValue(2, ::_2);
  data->setValue(3, ::_3);
  data->setValue(4, ::_4);
  data->setValue(5, ::_5);
  data->setValue(6, ::_6);
  data->setValue(7, ::_7);
  data->setValue(8, ::_8);
  data->setValue(9, ::_9);


  QString name = kArrayName;
  QString retName = data->getName();

  DREAM3D_REQUIRE_EQUAL(name, retName);

  QString tStr = data->getTypeAsString();
  QString tStrComp("StringDataArray");
  DREAM3D_REQUIRE_EQUAL(tStr, tStrComp)

  QString fn = data->getFullNameOfClass();
  QString fnComp("StringDataArray");
  DREAM3D_REQUIRE_EQUAL(fn, fnComp)


  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestOperatorOverloads()
{


}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestInitialization()
{
  StringDataArray::Pointer nodes = initializeStringDataArray();

  size_t size = nodes->getSize();
  DREAM3D_REQUIRE_EQUAL(ARRAY_SIZE, size)


  // Test Reinitializing the Array to Zero Size
  nodes->initialize();

  size_t nTuples = nodes->getNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(INIT_SIZE, nTuples)
  size = nodes->getSize();
  DREAM3D_REQUIRE_EQUAL(INIT_SIZE, size)

  // Try another test by initializing the struct array with a known value across all the structs
  nodes = initializeStringDataArray();
  size = nodes->getSize();
  DREAM3D_REQUIRE_EQUAL(ARRAY_SIZE, size)

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestResizeArray()
{

  StringDataArray::Pointer nodes = initializeStringDataArray();

  // Resize to a smaller Array
  nodes->resize(RESIZE_SMALLER);
  size_t nTuples = nodes->getNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(RESIZE_SMALLER, nTuples)
  size_t size = nodes->getSize();
  DREAM3D_REQUIRE_EQUAL(RESIZE_SMALLER, size)


  // Now run through the data and check that the correct data got removed and our
  // original data is still in tact
   DREAM3D_REQUIRE_EQUAL(nodes->getValue(0), ::_0);
  DREAM3D_REQUIRE_EQUAL(nodes->getValue(1), ::_1);
  DREAM3D_REQUIRE_EQUAL(nodes->getValue(2), ::_2);
  DREAM3D_REQUIRE_EQUAL(nodes->getValue(3), ::_3);
  DREAM3D_REQUIRE_EQUAL(nodes->getValue(4), ::_4);


  // Resize to a larger Array
  nodes->resize(RESIZE_LARGER);
  nTuples = nodes->getNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(RESIZE_LARGER, nTuples)
  size = nodes->getSize();
  DREAM3D_REQUIRE_EQUAL(RESIZE_LARGER, size)


  // Resize to a Zero Array
  nodes->resize(INIT_SIZE);
  nTuples = nodes->getNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(INIT_SIZE, nTuples)
  size = nodes->getSize();
  DREAM3D_REQUIRE_EQUAL(INIT_SIZE, size)


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestTupleCopy()
{
  // Create an Array of 10 Structs
  StringDataArray::Pointer nodes = initializeStringDataArray();

  // Try to put in a range that is Too large
  int err = nodes->copyTuple(0, ARRAY_SIZE + 1);
  DREAM3D_REQUIRE_EQUAL(err, -1)
  err = nodes->copyTuple(ARRAY_SIZE + 1, 0);
  DREAM3D_REQUIRE_EQUAL(err, -1)

  // Copy the values from Index 0 to Index 1 in the Struct Array and Check values
  {
    uint32_t from = 0;
    uint32_t to = 1;
    err = nodes->copyTuple(from, to);
    DREAM3D_REQUIRE_EQUAL(err, 0)
    QString ptr0 = nodes->getValue(from);
    QString ptr1 = nodes->getValue(to);
    DREAM3D_REQUIRE_EQUAL(ptr0, ptr1);
  }

  // Test Copying from a Larger Index to a smaller Index
  {
    uint32_t from = 6;
    uint32_t to = 5;
    err = nodes->copyTuple(from, to);
    DREAM3D_REQUIRE_EQUAL(err, 0)
    QString ptr0 = nodes->getValue(from);
    QString ptr1 = nodes->getValue(to);
    DREAM3D_REQUIRE_EQUAL(ptr0, ptr1);
  }

  // Test copying to from an index to itself, which is basically a no-op
  {
    uint32_t from = 8;
    uint32_t to = 8;
    err = nodes->copyTuple(from, to);
    DREAM3D_REQUIRE_EQUAL(err, 0)
    QString ptr0 = nodes->getValue(from);
    QString ptr1 = nodes->getValue(to);
    DREAM3D_REQUIRE_EQUAL(ptr0, ptr1);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestDataCopy()
{
  // Create an Array of 10 Structs
  StringDataArray::Pointer nodes = initializeStringDataArray();
  StringDataArray::Pointer copy = initializeStringDataArray();

  nodes->resize(ARRAY_SIZE * 2);
  bool didCopy = nodes->copyData(ARRAY_SIZE, copy);
  DREAM3D_REQUIRE_EQUAL(didCopy, true)
  DREAM3D_REQUIRE_EQUAL(nodes->getNumberOfTuples(), ARRAY_SIZE * 2);

  for(size_t i = 0; i < ARRAY_SIZE; i++)
  {
    QString ptr0 = nodes->getValue(i);
    QString ptr1 = nodes->getValue(ARRAY_SIZE + i);

    DREAM3D_REQUIRE_EQUAL(ptr0, ptr1);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestTupleErase()
{
  // Create an Array of 10 Structs
  StringDataArray::Pointer nodes = initializeStringDataArray();
  QVector<size_t> idxs(ARRAY_SIZE + 1, 0);
  // Try to erase more indices than there are in the struct array
  int err = nodes->eraseTuples(idxs);
  DREAM3D_REQUIRE_EQUAL(err, 0)
  DREAM3D_REQUIRE_EQUAL(nodes->getNumberOfTuples(), 0);

  idxs.resize(0);
  nodes = initializeStringDataArray();
  err = nodes->eraseTuples(idxs);
  DREAM3D_REQUIRE_EQUAL(err, 0)
  DREAM3D_REQUIRE_EQUAL(nodes->getNumberOfTuples(), ARRAY_SIZE);

  //Erase the first Element
  idxs.push_back(0);
  err = nodes->eraseTuples(idxs);
  DREAM3D_REQUIRE_EQUAL(err, 0)
  size_t nTuples = nodes->getNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(nTuples, ARRAY_SIZE - 1);
  for (size_t i = 0; i < nTuples; ++i)
  {
    QString node = nodes->getValue(i);
//    DREAM3D_REQUIRE_EQUAL(node->pos[0] , i + 1 + 23);
//    DREAM3D_REQUIRE_EQUAL(node->pos[1] , i + 1 + 11);
//    DREAM3D_REQUIRE_EQUAL(node->pos[2] , i + 1 + 20);
  }

  // Remove the structs located at the even indices
  nodes = initializeStringDataArray();
  idxs.push_back(2);
  idxs.push_back(4);
  idxs.push_back(6);
  idxs.push_back(8);
  err = nodes->eraseTuples(idxs);
  DREAM3D_REQUIRE_EQUAL(err, 0)
  nTuples = nodes->getNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(nTuples, ARRAY_SIZE - 5);
  for (size_t i = 0; i < nTuples; ++i)
  {
    QString node = nodes->getValue(i);
//    DREAM3D_REQUIRE_EQUAL(node->pos[0] , i * 2 + 1 + 23);
//    DREAM3D_REQUIRE_EQUAL(node->pos[1] , i * 2 + 1 + 11);
//    DREAM3D_REQUIRE_EQUAL(node->pos[2] , i * 2 + 1 + 20);
  }

  // Try to erase an index that is larger than the number of tuples in the array
  idxs.resize(0);
  idxs.push_back(ARRAY_SIZE - 1);
  err = nodes->eraseTuples(idxs);
  DREAM3D_REQUIRE_EQUAL(err, -100)

  // Remove the last Struct from the Array
  nodes = initializeStringDataArray();
  idxs.resize(0);
  idxs.push_back(ARRAY_SIZE - 1);
  err = nodes->eraseTuples(idxs);
  DREAM3D_REQUIRE_EQUAL(err, 0)
  nTuples = nodes->getNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(nTuples, ARRAY_SIZE - 1);
  for (size_t i = 0; i < nTuples; ++i)
  {
    QString node = nodes->getValue(i);
//    DREAM3D_REQUIRE_EQUAL(node->pos[0] , i + 23);
//    DREAM3D_REQUIRE_EQUAL(node->pos[1] , i + 11);
//    DREAM3D_REQUIRE_EQUAL(node->pos[2] , i + 20);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestDeepCopyArray()
{
  StringDataArray::Pointer nodes = initializeStringDataArray();
  StringDataArray::Pointer copy = boost::dynamic_pointer_cast<StringDataArray>(nodes->deepCopy());

  DREAM3D_REQUIRE_EQUAL(nodes->getSize(), copy->getSize());
  for(size_t i = 0; i < ARRAY_SIZE; i++)
  {
    DREAM3D_REQUIRE_EQUAL(nodes->getValue(i), copy->getValue(i));
  }

}


// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;
  if(argc < 0) { return EXIT_FAILURE; }

#if !REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif
  DREAM3D_REGISTER_TEST( TestDataCopy() )
  DREAM3D_REGISTER_TEST( TestInitialization() )
  DREAM3D_REGISTER_TEST( TestResizeArray() )
  DREAM3D_REGISTER_TEST( TestInitialization() )
  DREAM3D_REGISTER_TEST( TestTupleCopy() )
  DREAM3D_REGISTER_TEST( TestTupleErase() )
  DREAM3D_REGISTER_TEST( TestDeepCopyArray() )

#if REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif


  PRINT_TEST_SUMMARY();
  return err;
}

