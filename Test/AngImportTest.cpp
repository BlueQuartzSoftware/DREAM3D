/* ============================================================================
 * Copyright (c) 2013, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2013, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#include <string.h>

#include "MXA/Utilities/MXADir.h"

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/TSL/H5AngImporter.h"

#include "UnitTestSupport.hpp"
#include "EbsdTestFileLocation.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  MXADir::remove(EbsdImportTest::H5EbsdOutputFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestMissingHeaders()
{

  AngReader reader;
  reader.setFileName(EbsdImportTest::MissingHeader1);
  int err = reader.readHeaderOnly();
  // It should read through this header just fine
  DREAM3D_REQUIRE(err > 0)

  int value = reader.getNumEvenCols();
  DREAM3D_REQUIRE_EQUAL(value, -1)

  value = reader.getNumOddCols();
  DREAM3D_REQUIRE_EQUAL(value, -1)

  value = reader.getNumRows();
  DREAM3D_REQUIRE_EQUAL(value, -1)

  float step = reader.getXStep();
  DREAM3D_REQUIRE_EQUAL(step, 0.0f)

  step = reader.getYStep();
  DREAM3D_REQUIRE_EQUAL(step, 0.0f)

  err = reader.readFile();
  std::cout << reader.getErrorMessage() << std::endl;
  DREAM3D_REQUIRED(err, ==, -200)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestMissingGrid()
{

  AngReader reader;
  reader.setFileName(EbsdImportTest::GridMissing);
  int err = reader.readHeaderOnly();
  // It should read through this header just fine
  DREAM3D_REQUIRE(err > 0)



  err = reader.readFile();
  std::cout << reader.getErrorMessage() << std::endl;
  DREAM3D_REQUIRED(err, ==, -300)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestHexGrid()
{

  AngReader reader;
  reader.setFileName(EbsdImportTest::HexHeader);
  int err = reader.readHeaderOnly();
  // It should read through this header just fine
  DREAM3D_REQUIRE(err > 0)



  err = reader.readFile();
  std::cout << reader.getErrorMessage() << std::endl;
  DREAM3D_REQUIRED(err, ==, -400)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestShortFile()
{
   AngReader reader;
  reader.setFileName(EbsdImportTest::ShortFile);
  int err = reader.readFile();
  // It should read through this header just fine but die when reading the file because there is not enough data being read
  std::cout << reader.getErrorMessage() << std::endl;
  DREAM3D_REQUIRED(err, <, 0)
}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{

  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( TestMissingHeaders() )
  DREAM3D_REGISTER_TEST( TestHexGrid() )
  DREAM3D_REGISTER_TEST( TestMissingGrid() )
  DREAM3D_REGISTER_TEST( TestShortFile() )

  DREAM3D_REGISTER_TEST( RemoveTestFiles() )

  PRINT_TEST_SUMMARY();
  return err;
}

