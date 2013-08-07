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




#include "EbsdLib/EbsdConstants.h"
#include "DREAM3DLib/Common/EbsdColoring.hpp"

#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestCrystalStructureTest()
{

  uint32_t chigh = Ebsd::CrystalStructure::Cubic_High;
  uint32_t clow = Ebsd::CrystalStructure::Cubic_Low;

  uint32_t hhigh = Ebsd::CrystalStructure::Hexagonal_High;
  uint32_t hlow = Ebsd::CrystalStructure::Hexagonal_Low;

  bool b = Ebsd::CrystalStructure::Cubic_High == chigh;
  DREAM3D_REQUIRE_EQUAL(b, true)

  b = Ebsd::CrystalStructure::Cubic_High == clow;
  DREAM3D_REQUIRE_EQUAL(b, false)

  b = Ebsd::CrystalStructure::Cubic_High == hlow;
  DREAM3D_REQUIRE_EQUAL(b, false)

   b = Ebsd::CrystalStructure::Hexagonal_High == hhigh;
  DREAM3D_REQUIRE_EQUAL(b, true)

  b = Ebsd::CrystalStructure::Hexagonal_High == hlow;
  DREAM3D_REQUIRE_EQUAL(b, false)

  b = Ebsd::CrystalStructure::Hexagonal_High == clow;
  DREAM3D_REQUIRE_EQUAL(b, false)

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestEbsdColoring()
{

  float phi1 = 207.1653;
  float phi = 44.2854;
  float phi2 = 146.5178;
  float ref0 = 0;
  float ref1 = 0;
  float ref2 = 1;

  uint8_t rgb[3];
  uint8_t hkl[3];

  EbsdColoring::GenerateCubicIPFColor(phi1, phi, phi2, ref0, ref1, ref2, rgb, hkl, true);

  std::cout << (int)(rgb[0]) << " " << (int)(rgb[1]) << " " << (int)(rgb[2]) << " " << std::endl;
}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( TestEbsdColoring() )

  DREAM3D_REGISTER_TEST( TestCrystalStructureTest() )

  PRINT_TEST_SUMMARY();
  return err;
}




