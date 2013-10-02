/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <stdlib.h>

#include <stdio.h>

#include <iostream>
#include <string>

#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Utilities/IO/AngleFileLoader.h"


#include "UnitTestSupport.hpp"

#include "TestFileLocations.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  MXADir::remove(UnitTest::AngleFileLoaderTest::OutputFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void makeTestFile(const std::string delim, const std::string &outputFile)
{
  int count = 1000;
  float e0, e1, e2;

  FILE* f = fopen(outputFile.c_str(), "wb");

  fprintf(f, "%d\n", count);

  for(int i = 0; i < count; ++i)
  {
    e0 = static_cast<float>(i) * 0.1;
    e1 = static_cast<float>(i) * 0.25;
    e2 = static_cast<float>(i) * 0.58;
    fprintf(f, "%0.6f%s%0.6f%s%0.6f\n", e0, delim.c_str(), e1, delim.c_str(), e2);
  }
  fclose(f);
}




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestLoadingSpaceDelimited()
{
  // Create a Teset File
   makeTestFile(" ", UnitTest::AngleFileLoaderTest::OutputFile);

  AngleFileLoader::Pointer reader = AngleFileLoader::New();
  reader->setInputFile(UnitTest::AngleFileLoaderTest::OutputFile);
  reader->setDelimiter(" ");
  reader->setAngleRepresentation(AngleFileLoader::EulerAngles);
  reader->loadData();
  int err = reader->getErrorCode();
  DREAM3D_REQUIRE_EQUAL(err, 0)



}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestLoadingCommaDelimited()
{
  // Create a Teset File
   makeTestFile(", ", UnitTest::AngleFileLoaderTest::OutputFile);

  AngleFileLoader::Pointer reader = AngleFileLoader::New();
  reader->setInputFile(UnitTest::AngleFileLoaderTest::OutputFile);
  reader->setDelimiter(",");
  reader->setAngleRepresentation(AngleFileLoader::EulerAngles);
  reader->loadData();
  int err = reader->getErrorCode();
  DREAM3D_REQUIRE_EQUAL(err, 0)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestLoadingSemiColonDelimited()
{
  // Create a Teset File
   makeTestFile(";", UnitTest::AngleFileLoaderTest::OutputFile);

  AngleFileLoader::Pointer reader = AngleFileLoader::New();
  reader->setInputFile(UnitTest::AngleFileLoaderTest::OutputFile);
  reader->setDelimiter(";");
  reader->setAngleRepresentation(AngleFileLoader::EulerAngles);
  reader->loadData();
  int err = reader->getErrorCode();
  DREAM3D_REQUIRE_EQUAL(err, 0)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestLoadingTabDelimited()
{
  // Create a Teset File
   makeTestFile("\t", UnitTest::AngleFileLoaderTest::OutputFile);

  AngleFileLoader::Pointer reader = AngleFileLoader::New();
  reader->setInputFile(UnitTest::AngleFileLoaderTest::OutputFile);
  reader->setDelimiter("\t");
  reader->setAngleRepresentation(AngleFileLoader::EulerAngles);
  reader->loadData();
  int err = reader->getErrorCode();
  DREAM3D_REQUIRE_EQUAL(err, 0)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{

  int err = EXIT_SUCCESS;

  MXADir::mkdir(UnitTest::AngleFileLoaderTest::TestDir, true);

#if !REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
    #endif

      DREAM3D_REGISTER_TEST( TestLoadingSpaceDelimited() )
      DREAM3D_REGISTER_TEST( TestLoadingCommaDelimited() )
      DREAM3D_REGISTER_TEST( TestLoadingSemiColonDelimited() )
      DREAM3D_REGISTER_TEST( TestLoadingTabDelimited() )

    #if REMOVE_TEST_FILES
      DREAM3D_REGISTER_TEST( RemoveTestFiles() )
    #endif


      PRINT_TEST_SUMMARY();
  return err;
}
