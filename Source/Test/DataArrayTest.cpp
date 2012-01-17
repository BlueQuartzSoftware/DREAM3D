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
#include "DREAM3DLib/Common/DataArray.hpp"


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

#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestEraseElements()
{
  Int32ArrayType::Pointer int32Array = Int32ArrayType::CreateArray(NUM_ELEMENTS);
  for(size_t i = 0; i < NUM_ELEMENTS; ++i)
  {
    int32Array->SetComponent(i, 0, i);
  }

  std::vector<size_t> eraseElements;
  eraseElements.push_back(0);
  eraseElements.push_back(1);

  int32Array->EraseTuples(eraseElements);

  DREAM3D_REQUIRE_EQUAL(int32Array->GetValue(0), 2);
  DREAM3D_REQUIRE_EQUAL(int32Array->GetValue(1), 3);
  DREAM3D_REQUIRE_EQUAL(int32Array->GetValue(2), 4);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestDataArray()
{
  int32_t* ptr = NULL;
  {

  Int32ArrayType::Pointer d = Int32ArrayType::CreateArray(0);
  DREAM3D_REQUIRE_EQUAL(0, d->GetSize() );
  DREAM3D_REQUIRE_EQUAL(0, d->GetNumberOfTuples() );
    ptr = d->GetPointer(0);
  }


  {

  Int32ArrayType::Pointer int32Array = Int32ArrayType::CreateArray(NUM_ELEMENTS);
  ptr = int32Array->GetPointer(0);
  DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS , int32Array->GetNumberOfTuples());
  DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS , int32Array->GetSize());
  int32Array->SetNumberOfComponents(NUM_COMPONENTS);
  DREAM3D_REQUIRE_EQUAL(NUM_TUPLES , int32Array->GetNumberOfTuples());
  DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS , int32Array->GetSize());

  for(int i = 0; i < NUM_TUPLES; ++i)
  {
    for(int c = 0; c < NUM_COMPONENTS; ++c)
    {
      int32Array->SetComponent(i, c, i+c);
    }
  }


  // Resize Larger
  int32Array->Resize(NUM_TUPLES_2);
  DREAM3D_REQUIRE_EQUAL(NUM_TUPLES_2 , int32Array->GetNumberOfTuples());
  DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS_2 , int32Array->GetSize());

  // This should have saved our data so lets look at the data and compare it
  for(int i = 0; i < NUM_TUPLES; ++i)
  {
    for(int c = 0; c < NUM_COMPONENTS; ++c)
    {
      DREAM3D_REQUIRE_EQUAL( (int32Array->GetComponent(i, c)), (i+c) )
    }
  }



  // Resize Smaller - Which should have still save some of our data
  int32Array->Resize(NUM_TUPLES_3);
  DREAM3D_REQUIRE_EQUAL(NUM_TUPLES_3 , int32Array->GetNumberOfTuples());
  DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS_3 , int32Array->GetSize());

  // This should have saved our data so lets look at the data and compare it
  for(int i = 0; i < NUM_TUPLES; ++i)
  {
    for(int c = 0; c < NUM_COMPONENTS; ++c)
    {
      DREAM3D_REQUIRE_EQUAL( (int32Array->GetComponent(i, c)), (i+c) )
    }
  }


  // Change number of components
  int32Array->SetNumberOfComponents(NUM_COMPONENTS_4);
  DREAM3D_REQUIRE_EQUAL(NUM_TUPLES_4 , int32Array->GetNumberOfTuples());
  DREAM3D_REQUIRE_EQUAL(NUM_ELEMENTS_4 , int32Array->GetSize());

   ptr = int32Array->GetPointer(0);
  }


}

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( TestDataArray() );
  DREAM3D_REGISTER_TEST( TestEraseElements() );

  PRINT_TEST_SUMMARY();
  return err;
}

