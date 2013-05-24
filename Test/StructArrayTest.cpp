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
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/SurfaceMeshDataContainer.h"
#include "DREAM3DLib/Common/SurfaceMeshStructs.h"
#include "DREAM3DLib/Common/StructArray.hpp"
#include "DREAM3DLib/Common/FilterParameter.h"


#include "UnitTestSupport.hpp"

#include "TestFileLocations.h"

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
InitializeTuple()
EraseTuples()
CopyTuple()
*  GetTypeSize()
*  GetNumberOfTuples()
*  GetSize()
*  GetNumberOfComponents()
*  GetVoidPointer()
*  GetPointer()

*  RawResize()
*  Resize()
*  operator[]
*/


typedef struct
{
  uint32_t pos[3];
} Vec3Int_t;

typedef StructArray<Vec3Int_t> Vec3IntList_t;
typedef Vec3IntList_t::Pointer Vec3IntListPointer_t;

const std::string kArrayName("TestArray");

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Vec3IntListPointer_t initializeStructArray()
{
  Vec3IntListPointer_t nodes = Vec3IntList_t::CreateArray(ARRAY_SIZE, kArrayName);
  size_t nTuples = nodes->GetNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(ARRAY_SIZE, nTuples)

  int nComp = nodes->GetNumberOfComponents();
  DREAM3D_REQUIRE_EQUAL(1, nComp)

  size_t size = nodes->GetSize();
  DREAM3D_REQUIRE_EQUAL(ARRAY_SIZE, size)

  int typeSize = nodes->GetTypeSize();
  DREAM3D_REQUIRE_EQUAL(typeSize, sizeof(Vec3Int_t));

  // Add some known data into the arrays
  for (size_t i = 0; i < nTuples; ++i)
  {
    Vec3Int_t* node = nodes->GetPointer(i);
    node->pos[0] = i+23;
    node->pos[1] = i+11;
    node->pos[2] = i+20;
  }

  std::string name = "foo";
  nodes->SetName(name);
  std::string retName = nodes->GetName();
  DREAM3D_REQUIRE_EQUAL(name, retName);

  std::string tStr = nodes->getTypeAsString();
  std::string tStrComp("struct");
  DREAM3D_REQUIRE_EQUAL(tStr, tStrComp)

  std::string fn = nodes->getFullNameOfClass();
  std::string fnComp("StructArray<struct>");
  DREAM3D_REQUIRE_EQUAL(fn, fnComp)


  return nodes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestOperatorOverloads()
{
  Vec3IntListPointer_t nodes = initializeStructArray();

  size_t size = nodes->GetSize();
  DREAM3D_REQUIRE_EQUAL(ARRAY_SIZE, size)

  // Check data using the operator[] overload
  Vec3IntList_t& vertices = *nodes; // Dereference the Shared_Pointer using the * operator which allows the [] operators
  for (size_t i = 0; i < size; ++i)
  {
    Vec3Int_t node = vertices[i]; // Uses the [] operator overload
    DREAM3D_REQUIRE_EQUAL(node.pos[0] , i+23);
    DREAM3D_REQUIRE_EQUAL(node.pos[1] , i+11);
    DREAM3D_REQUIRE_EQUAL(node.pos[2] , i+20);
  }

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestInitialization()
{
  Vec3IntListPointer_t nodes = initializeStructArray();

  size_t size = nodes->GetSize();
  DREAM3D_REQUIRE_EQUAL(ARRAY_SIZE, size)


  // Test Reinitializing the Array to Zero Size
  nodes->initialize();
  Vec3Int_t* ptr = reinterpret_cast<Vec3Int_t*>(nodes->GetVoidPointer(0));
  DREAM3D_REQUIRE_EQUAL(NULL, ptr);
  ptr = nodes->GetPointer(0);
  DREAM3D_REQUIRE_EQUAL(NULL, ptr);
  size_t nTuples = nodes->GetNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(INIT_SIZE, nTuples)
  size = nodes->GetSize();
  DREAM3D_REQUIRE_EQUAL(INIT_SIZE, size)

  // Try another test by initializing the struct array with a known value across all the structs
  nodes = initializeStructArray();
  size = nodes->GetSize();
  DREAM3D_REQUIRE_EQUAL(ARRAY_SIZE, size)

  Vec3Int_t test; // Create a struct with some know values
  test.pos[0] = 10;
  test.pos[1] = 20;
  test.pos[2] = 30;
  nodes->initializeWithValues(test); // init our struct array with this struct
  {
    // Check data using the operator[] overload
    Vec3IntList_t& vertices = *nodes; // Dereference the Shared_Pointer using the * operator which allows the [] operators
    for (size_t i = 0; i < size; ++i)
    {
      Vec3Int_t node = vertices[i]; // Uses the [] operator overload
      DREAM3D_REQUIRE_EQUAL(node.pos[0] , 10);
      DREAM3D_REQUIRE_EQUAL(node.pos[1] , 20);
      DREAM3D_REQUIRE_EQUAL(node.pos[2] , 30);
    }
  }

  // Next, initialize all the structs in the array with Zeros
  nodes->initializeWithZeros();
    // Check data using the operator[] overload
  {
    Vec3IntList_t& vertices = *nodes; // Dereference the Shared_Pointer using the * operator which allows the [] operators
    for (size_t i = 0; i < size; ++i)
    {
      Vec3Int_t node = vertices[i]; // Uses the [] operator overload
      DREAM3D_REQUIRE_EQUAL(node.pos[0] , 0);
      DREAM3D_REQUIRE_EQUAL(node.pos[1] , 0);
      DREAM3D_REQUIRE_EQUAL(node.pos[2] , 0);
    }
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestResizeArray()
{

  Vec3IntListPointer_t nodes = initializeStructArray();

  // Resize to a smaller Array
  nodes->Resize(RESIZE_SMALLER);
  size_t nTuples = nodes->GetNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(RESIZE_SMALLER, nTuples)
  size_t size = nodes->GetSize();
  DREAM3D_REQUIRE_EQUAL(RESIZE_SMALLER, size)


  // Now run through the data and check that the correct data got removed and our
  // original data is still in tact
  for (size_t i = 0; i < nTuples; ++i)
  {
    Vec3Int_t* node = nodes->GetPointer(i);
    DREAM3D_REQUIRE_EQUAL(node->pos[0] , i+23);
    DREAM3D_REQUIRE_EQUAL(node->pos[1] , i+11);
    DREAM3D_REQUIRE_EQUAL(node->pos[2] , i+20);
  }

  // Resize to a larger Array
  nodes->Resize(RESIZE_LARGER);
  nTuples = nodes->GetNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(RESIZE_LARGER, nTuples)
  size = nodes->GetSize();
  DREAM3D_REQUIRE_EQUAL(RESIZE_LARGER, size)
  // Assign some known Data to each of the structures in the Struct Array
  for (size_t i = 0; i < size; ++i)
  {
    Vec3Int_t* node = nodes->GetPointer(i);
    node->pos[0] = i+23;
    node->pos[1] = i+11;
    node->pos[2] = i+20;
  }
  // Now check the data that was assigned to each of the structures
  for (size_t i = 0; i < size; ++i)
  {
    Vec3Int_t* node = nodes->GetPointer(i);
    DREAM3D_REQUIRE_EQUAL(node->pos[0] , i+23);
    DREAM3D_REQUIRE_EQUAL(node->pos[1] , i+11);
    DREAM3D_REQUIRE_EQUAL(node->pos[2] , i+20);
  }


  // Resize to a Zero Array
  nodes->Resize(INIT_SIZE);
  Vec3Int_t* ptr = reinterpret_cast<Vec3Int_t*>(nodes->GetVoidPointer(0));
  DREAM3D_REQUIRE_EQUAL(NULL, ptr);
  ptr = nodes->GetPointer(0);
  DREAM3D_REQUIRE_EQUAL(NULL, ptr);
  nTuples = nodes->GetNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(INIT_SIZE, nTuples)
  size = nodes->GetSize();
  DREAM3D_REQUIRE_EQUAL(INIT_SIZE, size)


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestTupleCopy()
{
  // Create an Array of 10 Structs
  Vec3IntListPointer_t nodes = initializeStructArray();

  // Try to put in a range that is Too large
  int err = nodes->CopyTuple(0, ARRAY_SIZE+1);
  DREAM3D_REQUIRE_EQUAL(err, -1)
  err = nodes->CopyTuple(ARRAY_SIZE+1, 0);
  DREAM3D_REQUIRE_EQUAL(err, -1)

  // Copy the values from Index 0 to Index 1 in the Struct Array and Check values
  {
    uint32_t from = 0;
    uint32_t to = 1;
    err = nodes->CopyTuple(from, to);
    DREAM3D_REQUIRE_EQUAL(err, 0)
    Vec3Int_t* ptr0 = nodes->GetPointer(from);
    Vec3Int_t* ptr1 = nodes->GetPointer(to);
    DREAM3D_REQUIRE_EQUAL(ptr0->pos[0] , from + 23)
    DREAM3D_REQUIRE_EQUAL(ptr0->pos[1] , from + 11)
    DREAM3D_REQUIRE_EQUAL(ptr0->pos[2] , from + 20)

    DREAM3D_REQUIRE_EQUAL(ptr1->pos[0] , from + 23)
    DREAM3D_REQUIRE_EQUAL(ptr1->pos[1] , from + 11)
    DREAM3D_REQUIRE_EQUAL(ptr1->pos[2] , from + 20)
  }

// Test Copying from a Larger Index to a smaller Index
  {
    uint32_t from = 6;
    uint32_t to = 5;
    err = nodes->CopyTuple(from, to);
    DREAM3D_REQUIRE_EQUAL(err, 0)
    Vec3Int_t* ptr0 = nodes->GetPointer(from);
    Vec3Int_t* ptr1 = nodes->GetPointer(to);
    DREAM3D_REQUIRE_EQUAL(ptr0->pos[0] , from + 23)
    DREAM3D_REQUIRE_EQUAL(ptr0->pos[1] , from + 11)
    DREAM3D_REQUIRE_EQUAL(ptr0->pos[2] , from + 20)

    DREAM3D_REQUIRE_EQUAL(ptr1->pos[0] , from + 23)
    DREAM3D_REQUIRE_EQUAL(ptr1->pos[1] , from + 11)
    DREAM3D_REQUIRE_EQUAL(ptr1->pos[2] , from + 20)
  }

// Test copying to from an index to itself, which is basically a no-op
  {
    uint32_t from = 8;
    uint32_t to = 8;
    err = nodes->CopyTuple(from, to);
    DREAM3D_REQUIRE_EQUAL(err, 0)
    Vec3Int_t* ptr0 = nodes->GetPointer(from);
    Vec3Int_t* ptr1 = nodes->GetPointer(to);
    DREAM3D_REQUIRE_EQUAL(ptr0->pos[0] , from + 23)
    DREAM3D_REQUIRE_EQUAL(ptr0->pos[1] , from + 11)
    DREAM3D_REQUIRE_EQUAL(ptr0->pos[2] , from + 20)

    DREAM3D_REQUIRE_EQUAL(ptr1->pos[0] , from + 23)
    DREAM3D_REQUIRE_EQUAL(ptr1->pos[1] , from + 11)
    DREAM3D_REQUIRE_EQUAL(ptr1->pos[2] , from + 20)
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestTupleErase()
{
  // Create an Array of 10 Structs
  Vec3IntListPointer_t nodes = initializeStructArray();
  std::vector<size_t> idxs(ARRAY_SIZE + 1, 0);
  // Try to erase more indices than there are in the struct array
  int err = nodes->EraseTuples(idxs);
  DREAM3D_REQUIRE_EQUAL(err, 0)
  DREAM3D_REQUIRE_EQUAL(nodes->GetNumberOfTuples(), 0);

  idxs.resize(0);
  nodes = initializeStructArray();
  err = nodes->EraseTuples(idxs);
  DREAM3D_REQUIRE_EQUAL(err, 0)
  DREAM3D_REQUIRE_EQUAL(nodes->GetNumberOfTuples(), ARRAY_SIZE);

  //Erase the first Element
  idxs.push_back(0);
  err = nodes->EraseTuples(idxs);
  DREAM3D_REQUIRE_EQUAL(err, 0)
  size_t nTuples = nodes->GetNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(nTuples, ARRAY_SIZE-1);
  for (size_t i = 0; i < nTuples; ++i)
  {
    Vec3Int_t* node = nodes->GetPointer(i);
    DREAM3D_REQUIRE_EQUAL(node->pos[0] , i+1+23);
    DREAM3D_REQUIRE_EQUAL(node->pos[1] , i+1+11);
    DREAM3D_REQUIRE_EQUAL(node->pos[2] , i+1+20);
  }

  // Remove the structs located at the even indices
  nodes = initializeStructArray();
  idxs.push_back(2);
  idxs.push_back(4);
  idxs.push_back(6);
  idxs.push_back(8);
  err = nodes->EraseTuples(idxs);
  DREAM3D_REQUIRE_EQUAL(err, 0)
  nTuples = nodes->GetNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(nTuples, ARRAY_SIZE-5);
  for (size_t i = 0; i < nTuples; ++i)
  {
    Vec3Int_t* node = nodes->GetPointer(i);
    DREAM3D_REQUIRE_EQUAL(node->pos[0] , i*2+1+23);
    DREAM3D_REQUIRE_EQUAL(node->pos[1] , i*2+1+11);
    DREAM3D_REQUIRE_EQUAL(node->pos[2] , i*2+1+20);
  }

  // Try to erase an index that is larger than the number of tuples in the array
  idxs.resize(0);
  idxs.push_back(ARRAY_SIZE-1);
  err = nodes->EraseTuples(idxs);
  DREAM3D_REQUIRE_EQUAL(err, -100)

  // Remove the last Struct from the Array
  nodes = initializeStructArray();
  idxs.resize(0);
  idxs.push_back(ARRAY_SIZE-1);
  err = nodes->EraseTuples(idxs);
  DREAM3D_REQUIRE_EQUAL(err, 0)
  nTuples = nodes->GetNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(nTuples, ARRAY_SIZE-1);
  for (size_t i = 0; i < nTuples; ++i)
  {
    Vec3Int_t* node = nodes->GetPointer(i);
    DREAM3D_REQUIRE_EQUAL(node->pos[0] , i+23);
    DREAM3D_REQUIRE_EQUAL(node->pos[1] , i+11);
    DREAM3D_REQUIRE_EQUAL(node->pos[2] , i+20);
  }
}


// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;

  #if !REMOVE_TEST_FILES
    DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  #endif

  DREAM3D_REGISTER_TEST( TestInitialization() )
  DREAM3D_REGISTER_TEST( TestResizeArray() )
  DREAM3D_REGISTER_TEST( TestInitialization() )
  DREAM3D_REGISTER_TEST( TestTupleCopy() )
  DREAM3D_REGISTER_TEST( TestTupleErase() )

  #if REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  #endif


  PRINT_TEST_SUMMARY();
  return err;
}

