/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <stdlib.h>
#include <iostream>
#include <vector>

#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/IO/PhWriter.hpp"
#include "DREAM3DLib/IO/PhReader.h"

#include "UnitTestSupport.hpp"

#define REMOVE_TEST_FILES 0

namespace Detail
{
  const std::string TestFile("PhIOTest.ph");
  static const int XSize = 5;
  static const int YSize = 4;
  static const int ZSize = 3;
  static const int Offset = 200;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  MXADir::remove(PhIOTest::TestFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestPhWriter()
{
  int size = Detail::XSize * Detail::YSize * Detail::ZSize;
  std::vector<int> grain_indices(size);
  for (int i = 0; i < size; ++i)
  {
    grain_indices[i] = i + Detail::Offset;
  }
  int nx = Detail::XSize;
  int ny = Detail::YSize;
  int nz = Detail::ZSize;


  PhWriter<int>::Pointer writer = PhWriter<int>::New();
  int err = writer->writeFile(Detail::TestFile, &(grain_indices.front()), nx, ny, nz);
  DREAM3D_REQUIRE_EQUAL(err, 0);
  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestPhReader()
{

  PhReader::Pointer reader = PhReader::New();
  int nx = 0;
  int ny = 0;
  int nz = 0;
  std::vector<int> grain_indices;
  int err = reader->readFile(Detail::TestFile, grain_indices, nx, ny, nz);
  DREAM3D_REQUIRE_EQUAL(err, 0);
  DREAM3D_REQUIRE_EQUAL(nx, Detail::XSize);
  DREAM3D_REQUIRE_EQUAL(ny, Detail::YSize);
  DREAM3D_REQUIRE_EQUAL(nz, Detail::ZSize);
  int size = Detail::XSize * Detail::YSize * Detail::ZSize;

  for (int i = 0; i < size; ++i)
  {
    DREAM3D_REQUIRE_EQUAL( (i+Detail::Offset), grain_indices[i] );
  }


  return EXIT_SUCCESS;
}


// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( TestPhWriter() );
  DREAM3D_REGISTER_TEST( TestPhReader() );

  DREAM3D_REGISTER_TEST( RemoveTestFiles() );
  PRINT_TEST_SUMMARY();
  return err;
}





