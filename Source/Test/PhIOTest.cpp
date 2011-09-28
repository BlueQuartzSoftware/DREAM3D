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

#define REMOVE_TEST_FILES 1

namespace PhIOTest
{
  const std::string PhIOTestFile("PhIOTest.ph");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  MXADir::remove(PhIOTest::PhIOTestFile);
#endif
}


int TestPhWriter()
{
  std::vector<int> grain_indices(27);
  for (int i = 0; i < 27; ++i)
  {
    grain_indices[i] = i + 100;
  }
  int xpoints = 3;
  int ypoints = 3;
  int zpoints = 3;


  PhWriter::Pointer writer = PhWriter::New();
  int err = writer->writeFile(PhIOTest::PhIOTestFile, grain_indices, xpoints, ypoints, zpoints);
  MXA_REQUIRE_EQUAL(err, 0);
  return EXIT_SUCCESS;
}

int TestPhReader()
{

  PhReader::Pointer reader = PhReader::New();
  int nx = 0;
  int ny = 0;
  int nz = 0;
  std::vector<int> grain_indices;
  int err = reader->readFile(PhIOTest::PhIOTestFile, grain_indices, nx, ny, nz);
  MXA_REQUIRE_EQUAL(err, 0);
  MXA_REQUIRE_EQUAL(nx, 3);
  MXA_REQUIRE_EQUAL(ny, 3);
  MXA_REQUIRE_EQUAL(nz, 3);

  for (int i = 0; i < 27; ++i)
  {
    MXA_REQUIRE_EQUAL( (i+100), grain_indices[i] );
  }


  return EXIT_SUCCESS;
}


// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
  int err = EXIT_SUCCESS;

  MXA_REGISTER_TEST( TestPhWriter() );
  MXA_REGISTER_TEST( TestPhReader() );

  MXA_REGISTER_TEST( RemoveTestFiles() );
  PRINT_TEST_SUMMARY();
  return err;
}





