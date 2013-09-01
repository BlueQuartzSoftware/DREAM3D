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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <vector>

#include <QtCore/QDir>
#include <QtCore/QFile>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

#include "DREAM3DLib/Common/ModifiedLambertProjection.h"
#include "DREAM3DLib/Common/ModifiedLambertProjectionArray.h"

#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(UnitTest::DxIOTest::TestFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestLambertWrite()
{
  FloatArrayType::Pointer xyzCoords = FloatArrayType::CreateArray(0, "XYZ Coords");
  int lambertDim = 22;
  float sphereRadius = 1.0;
  size_t gbcdDims[3] = { 40, 20, 40};
  // Allocate a typical GBCD Data Array
  std::vector<ModifiedLambertProjection::Pointer> lamberts(gbcdDims[0] * gbcdDims[1] * gbcdDims[2]);
  size_t count = lamberts.size();
  for(size_t i = 0; i < count; ++i)
  {
    lamberts[i] = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyzCoords.get(), lambertDim, sphereRadius);
    lamberts[i]->getNorthSquare()->initializeWithZeros();
    lamberts[i]->getSouthSquare()->initializeWithValues(1.0);
  }

  // Create something to hold all the GBCD Data
  ModifiedLambertProjectionArray::Pointer gbcdData = ModifiedLambertProjectionArray::New();
  gbcdData->setModifiedLambertProjectionArray(lamberts);


  // Now open an HDF5 File for writing
  hid_t fid = H5Utilities::createFile(UnitTest::LambertRWTest::TestFile);
  DREAM3D_REQUIRE(fid > 0)

  int err = gbcdData->writeH5Data(fid);
  DREAM3D_REQUIRE(err >= 0)

  H5Utilities::closeFile(fid);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestLambertRead()
{

}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{

  int err = EXIT_SUCCESS;

#if !REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif

  DREAM3D_REGISTER_TEST( TestLambertWrite() )
  DREAM3D_REGISTER_TEST( TestLambertRead() )

#if REMOVE_TEST_FILES
//  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif

  PRINT_TEST_SUMMARY();
  return err;
}






