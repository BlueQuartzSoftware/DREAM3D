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


#include <QtCore/QDir>
#include <QtCore/QFile>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/ManagedArrayOfArrays.hpp"


#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"


#define NUM_ELEMENTS  10

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
template<typename T>
void __TestManagedArray()
{

  typename ManagedArrayOfArrays<T>::Pointer mArray = ManagedArrayOfArrays<T>::CreateArray(NUM_ELEMENTS, "test");

  // Allocate some memory for each array
  for(size_t i = 0; i < NUM_ELEMENTS; ++i)
  {
    typename ManagedArrayOfArrays<T>::Data_t& ptr = *(mArray->getPointer(i));
    // First make sure we have a properly initialized Data structure
    DREAM3D_REQUIRE_EQUAL(ptr.count, 0)
    DREAM3D_REQUIRE_EQUAL(ptr.data, NULL)

    ptr.count = i;
    ptr.data = (T*)(malloc(sizeof(T) * ptr.count));
  }

  for(size_t i = 0; i < NUM_ELEMENTS; ++i)
  {
    typename ManagedArrayOfArrays<T>::Data_t& ptr = *(mArray->getPointer(i));
    DREAM3D_REQUIRE_EQUAL(ptr.count, i)
    DREAM3D_REQUIRE_NE(ptr.data, NULL)
  }

  // Forcibly set this to NULL so that if something goes wrong the debugger will catch it here.
  mArray = ManagedArrayOfArrays<T>::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestManagedPointerArray()
{
  __TestManagedArray<uint8_t>();
  __TestManagedArray<int8_t>();
  __TestManagedArray<uint16_t>();
  __TestManagedArray<int16_t>();
  __TestManagedArray<uint32_t>();
  __TestManagedArray<int32_t>();
  __TestManagedArray<uint64_t>();
  __TestManagedArray<int64_t>();
  __TestManagedArray<float>();
  __TestManagedArray<double>();
  __TestManagedArray<bool>();

  return EXIT_SUCCESS;
}



// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( TestManagedPointerArray() )

  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  PRINT_TEST_SUMMARY();
  return err;
}


