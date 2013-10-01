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


#include <string.h>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

#include "EbsdLib/HKL/CtfReader.h"
#include "EbsdLib/HKL/H5CtfImporter.h"

#include "UnitTestSupport.hpp"
#include "EbsdTestFileLocation.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestCtfReader()
{
  CtfReader reader;
  reader.setFileName(UnitTest::CtfReaderTest::FileDir + UnitTest::CtfReaderTest::EuropeanInputFile1);
 // reader.setFileName("/Users/Shared/Data/HKL_Data/Oxford_Standard_Ni0.ctf");
  int err =  reader.readFile();
  DREAM3D_REQUIRE(err >= 0);

//  size_t nRows = reader.getNumberOfElements();

#if 0
  size_t x = reader.getXCells();
  size_t y = reader.getYCells();
  size_t z = reader.getZCells();
  if (z < 0) { z = 1; }

  float* xPtr = reader.getXPointer();

  for (size_t k = 0; k < z; ++k)
  {
    for (size_t j = 0; j < y; ++j)
    {
      for (size_t i = 0; i < x; ++i)
      {
//        if ( *xPtr < 0.1)
//        {
//          std::cout << i << "," << j << "," << k << std::endl;
//        }
        ++xPtr;
      }

    }
 //   std::cout << "Z Slice Done" << std::endl;
  }
#endif

#if 1

  float xstep = reader.getXStep();
  DREAM3D_REQUIRE(xstep == 0.5f);
  float ystep = reader.getYStep();
  DREAM3D_REQUIRE(ystep == 0.5f);
  DREAM3D_REQUIRE(reader.getNumPhases() == 1);
  CtfPhase::Pointer phase = reader.getPhaseVector().at(0);
  std::vector<float> dimensions = phase->getLatticeConstants();
  DREAM3D_REQUIRE(dimensions[0] >= 3.230f && dimensions[0] <= 3.232f)
  DREAM3D_REQUIRE(dimensions[1] >= 3.230f && dimensions[1] <= 3.232f)
  DREAM3D_REQUIRE(dimensions.size() == 6)
#endif

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( TestCtfReader() )


  PRINT_TEST_SUMMARY();
  return err;
}
