/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include <array>

#include "UnitTestSupport.hpp"

#include "OrientationLibTestFileLocations.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "OrientationLib/Core/Orientation.hpp"
#include "OrientationLib/Core/OrientationMath.h"
#include "OrientationLib/Core/OrientationTransformation.hpp"

class OrientationMathTest
{
public:
  OrientationMathTest() = default;
  virtual ~OrientationMathTest() = default;

  template <typename AxisAngleType, typename CoordinateType>
  CoordinateType TransformPoint(const AxisAngleType& axisAngle, float deg, const CoordinateType& inCoord)
  {
    OrientationF om = OrientationTransformation::ax2om<OrientationF, OrientationF>(axisAngle);
    return OrientationMath::TransformCoordinate<OrientationF, CoordinateType>(om, inCoord);
  }

  // -----------------------------------------------------------------------------
  void Test()
  {

    using CoordinateType = std::array<float, 3>;
    float deg = 45.0f;
    OrientationF axis = {0.0f, 1.0f, 0.0f, 0.0f};    // <001> @ 0 deg
    axis[3] = deg * SIMPLib::Constants::k_PiOver180; // Convert to Radians
    CoordinateType inCoord = {10.0f, 0.0f, 0.0f};
    CoordinateType outCoord = TransformPoint<OrientationF, CoordinateType>(axis, deg, inCoord);
    printf("%f %f %f\n", outCoord[0], outCoord[1], outCoord[2]);

    /* ====================================================== */
    OrientationF om = OrientationTransformation::ax2om<OrientationF, OrientationF>(axis);
    float rotMat[3][3];
    om.toGMatrix(rotMat);

    MatrixMath::Multiply3x3with3x1(rotMat, inCoord.data(), outCoord.data());

    printf("%f %f %f\n", outCoord[0], outCoord[1], outCoord[2]);

    CoordinateType outCoord2 = OrientationMath::TransformCoordinate<OrientationF, CoordinateType>(om, inCoord);

    printf("%f %f %f\n", outCoord2[0], outCoord2[1], outCoord2[2]);

    std::cout << "" << std::endl;
  }

  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### OrientationMathTest Starting ####" << std::endl;
    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(Test());
  }

public:
  OrientationMathTest(const OrientationMathTest&) = delete;            // Copy Constructor Not Implemented
  OrientationMathTest(OrientationMathTest&&) = delete;                 // Move Constructor Not Implemented
  OrientationMathTest& operator=(const OrientationMathTest&) = delete; // Copy Assignment Not Implemented
  OrientationMathTest& operator=(OrientationMathTest&&) = delete;      // Move Assignment Not Implemented
};
