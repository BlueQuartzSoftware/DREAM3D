/* ============================================================================
 * Copyright (c) 2015 BlueQuartz Softwae, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
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
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "OrientationLibTestFileLocations.h"


#include "OrientationLib/SpaceGroupOps/SO3Sampler.h"
#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"
#include "OrientationLib/Utilities/ModifiedLambertProjection3D.hpp"

typedef OrientationTransforms<DOrientArrayType, double> OrientationTransformsType;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  // QFile::remove();
#endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SO3CountTest()
{
  SO3Sampler::Pointer sampler = SO3Sampler::New();
  SO3Sampler::OrientationListArrayType orientations = sampler->SampleRFZ(10, 32);
  DREAM3D_REQUIRE_EQUAL(361, orientations.size());

  orientations = sampler->SampleRFZ(100, 32);
  DREAM3D_REQUIRE_EQUAL(333227, orientations.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsideCubicFZTest()
{

  SO3Sampler::Pointer sampler = SO3Sampler::New();
  DOrientArrayType rod(4);

  DOrientArrayType cu( -0.3217544095666538,  0.2145029397111025, -0.4290058794222050);
  OrientationTransformsType::cu2ro(cu, rod);
  bool inside = sampler->insideCubicFZ(rod.data(), 4);
  DREAM3D_REQUIRE_EQUAL(inside, false);

  cu = DOrientArrayType(-0.42900587942220514, -0.21450293971110265, 0.42900587942220514);
  OrientationTransformsType::cu2ro(cu, rod);
  inside = sampler->insideCubicFZ(rod.data(), 4);
  DREAM3D_REQUIRE_EQUAL(inside, true);


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestPyramid()
{

  DOrientArrayType xyz(0,0,1);

  int pSection = ModifiedLambertProjection3D<DOrientArrayType, double>::GetPyramid(xyz);
  DREAM3D_REQUIRE_EQUAL(pSection, 1)

      xyz = DOrientArrayType(0,0,-1);
  pSection = ModifiedLambertProjection3D<DOrientArrayType, double>::GetPyramid(xyz);
  DREAM3D_REQUIRE_EQUAL(pSection, 2)

      xyz = DOrientArrayType(1,0,0);
  pSection = ModifiedLambertProjection3D<DOrientArrayType, double>::GetPyramid(xyz);
  DREAM3D_REQUIRE_EQUAL(pSection, 3)

      xyz = DOrientArrayType(-1, 0, 0);
  pSection = ModifiedLambertProjection3D<DOrientArrayType, double>::GetPyramid(xyz);
  DREAM3D_REQUIRE_EQUAL(pSection, 4)

      xyz = DOrientArrayType(0,1,0);
  pSection = ModifiedLambertProjection3D<DOrientArrayType, double>::GetPyramid(xyz);
  DREAM3D_REQUIRE_EQUAL(pSection, 5)

      xyz = DOrientArrayType(0,-1, 0);
  pSection = ModifiedLambertProjection3D<DOrientArrayType, double>::GetPyramid(xyz);
  DREAM3D_REQUIRE_EQUAL(pSection, 6)

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( InsideCubicFZTest() )
  DREAM3D_REGISTER_TEST( TestPyramid() )
  DREAM3D_REGISTER_TEST( SO3CountTest() )
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  PRINT_TEST_SUMMARY();

  return err;
}
