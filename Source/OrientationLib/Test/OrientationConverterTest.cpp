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

#include <stdio.h>

#include <iostream>
#include <iomanip>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/Math/OrientationConverter.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void Print_QU(const T& om, typename QuaternionMath<float>::Order layout = QuaternionMath<float>::QuaternionVectorScalar)
{
  if(layout == QuaternionMath<float>::QuaternionVectorScalar)
  {
    printf("Quaternion (<vector>scalar)      :   <% 3.6f   % 3.6f    % 3.6f>    % 3.6f\n", om[0], om[1], om[2], om[3] );
  }

  else if(layout == QuaternionMath<float>::QuaternionScalarVector)
  {
    printf("Quaternion (scalar<vector>)      :   % 3.6f   <% 3.6f    % 3.6f    % 3.6f>\n", om[0], om[1], om[2], om[3] );
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void Print_EU(const T& om)
{
  printf("Euler angles [rad]               :   % 3.6f    % 3.6f    % 3.6f\n", om[0], om[1], om[2] );
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestEulerConversion()
{
  size_t nTuples = 2;
  int qStride = 4;
  QVector<size_t> cDims(1, 3);
  FloatArrayType::Pointer eulers = FloatArrayType::CreateArray(nTuples, cDims, "Eulers");
  // Initialize the Eulers with some values
  eulers->setComponent(0, 0, 302.84f * DREAM3D::Constants::k_PiOver180);
  eulers->setComponent(0, 1, 51.282f * DREAM3D::Constants::k_PiOver180);
  eulers->setComponent(0, 2, 37.969f * DREAM3D::Constants::k_PiOver180);
  eulers->setComponent(1, 0, 45.0f * DREAM3D::Constants::k_PiOver180);
  eulers->setComponent(1, 1, 0.0f * DREAM3D::Constants::k_PiOver180);
  eulers->setComponent(1, 2, 0.0f * DREAM3D::Constants::k_PiOver180);

  float rad = 302.84 * DREAM3D::Constants::k_PiOver180;
  std::cout << "Rad: " << rad << std::endl;
  std::cout << "2Pi: " << DREAM3D::Constants::k_2Pi << std::endl;


  //std::cout << "Remainer (302.84/360): " << remainder(rad, DREAM3D::Constants::k_2Pi) << std::endl;
  std::cout << "fmod (5.28556 / 2Pi): " << fmod(rad, DREAM3D::Constants::k_2Pi) << std::endl;


  OrientationConverter<float>::Pointer ocEulers =  EulerConvertor<float>::New();
  ocEulers->setInputData(eulers);
  ocEulers->convertRepresentationTo(OrientationConverter<float>::Quaternion);

  FloatArrayType::Pointer output = ocEulers->getOutputData();

  for(size_t i = 0; i < nTuples; i++)
  {
    float* ptr = output->getPointer(i * qStride);
    Print_QU(ptr);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool closeEnough(const float& a, const float& b,
                 const float& epsilon = std::numeric_limits<float>::epsilon())
{
  return (epsilon > std::abs(a - b));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestEulerAngle(float phi1, float phi, float phi2)
{
  std::cout << "TESTING EULER ANGLE (Rad): " << phi1 << ", " << phi << ", " << phi2 << std::endl;

  size_t nTuples = 1;
  int qStride = 4;
  QVector<size_t> cDims(1, 3);
  FloatArrayType::Pointer eulers = FloatArrayType::CreateArray(nTuples, cDims, "Eulers");
  // Initialize the Eulers with some values
  eulers->setComponent(0, 0, phi1);
  eulers->setComponent(0, 1, phi);
  eulers->setComponent(0, 2, phi2);

  typedef OrientationConverter<float> OCType;
  QVector<OCType::OrientationType> ocTypes = OCType::GetOrientationTypes();
  QVector<QString> tStrings = OCType::GetOrientationTypeStrings();
  QVector<OCType::Pointer> converters(6);
  converters[0] = EulerConvertor<float>::New();
  converters[1] = OrientationMatrixConvertor<float>::New();
  converters[2] = QuaternionConvertor<float>::New();
  converters[3] = AxisAngleConvertor<float>::New();
  converters[4] = RodriguesConvertor<float>::New();
  converters[5] = HomochoricConvertor<float>::New();
  //converters[6] = CubochoricConvertor<float>::New();

  typedef  OrientationTransforms<FloatOrientationArray_t, float> OrientationTransformType;
  OrientationTransformType::ResultType result;
  QVector<int> strides = OCType::GetComponentCounts();

  for(int t0 = 0; t0 < 1; t0++)
  {
    for(int t1 = 0; t1 < converters.size(); t1++)
    {
      if (t0 == t1) { continue; }
      // std::cout << "   Converting " << tStrings[t0].toStdString() << "->"<< tStrings[t1].toStdString() << "->" << tStrings[t0].toStdString() << std::endl;
      converters[t0]->setInputData(eulers);
      converters[t0]->convertRepresentationTo(ocTypes[t1]);
      FloatArrayType::Pointer t1_output = converters[t0]->getOutputData();

      converters[t1]->setInputData(t1_output);
      converters[t1]->convertRepresentationTo(ocTypes[t0]);
      FloatArrayType::Pointer t0_output = converters[t1]->getOutputData();

      converters[t0]->printRepresentation(t0_output->getPointer(0));
      printf("\n");
      converters[t0]->setInputData(t0_output);
      converters[t0]->sanityCheckInputData();
      converters[t0]->printRepresentation(t0_output->getPointer(0));
      printf("\n");

      qStride = strides[t0];
      bool test = false;
      float delta[qStride];
      for(size_t i = 0; i < nTuples; i++)
      {
        float* orig = eulers->getPointer(i * qStride);
        float* converted = t0_output->getPointer(i * qStride);
        float* o = t1_output->getPointer(i * strides[t1]);

        converters[t0]->printRepresentation(orig);
        printf("\n");
        converters[t1]->printRepresentation(o);
        delta[0] = std::abs(orig[0] - converted[0]);
        delta[1] = std::abs(orig[1] - converted[1]);
        delta[2] = std::abs(orig[2] - converted[2]);

        printf("%s -> %s -> %s\t New Euler: %0.8f, %0.8f, %0.8f\tDelta: %0.8f, %0.8f, %0.8f\n", tStrings[t0].toLatin1().constData(), tStrings[t1].toLatin1().constData(), tStrings[t0].toLatin1().constData(), converted[0], converted[1], converted[2], delta[0], delta[1], delta[2]);


//        std::cout << std::setprecision(8) << "   Euler: " << converted[0] << ", " << converted[1] << ", " << converted[2];
//        std::cout << std::setprecision(8) << "   Delta: " <<  << ", " << std::abs(orig[1] - converted[1]) << ", " << std::abs(orig[2] - converted[2]) << std::endl;
#if 0
        test = closeEnough(orig[0], converted[0], 1.0E-6);
        DREAM3D_REQUIRE_EQUAL(test, true)
        test = closeEnough(orig[1], converted[1], 1.0E-6);
        DREAM3D_REQUIRE_EQUAL(test, true)
        test = closeEnough(orig[1], converted[1], 1.0E-6);
        DREAM3D_REQUIRE_EQUAL(test, true)
#endif

      }
    }

  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestFilterDesign()
{

  float numSteps = 4.0;
  float phi1Inc = 360.0 / numSteps;
  float phiInc = 180.0 / numSteps;
  float phi2Inc = 360.0 / numSteps;


  for(float p2 = 0.0; p2 < 361.0; p2 = p2 + phi2Inc)
  {
    for(float p = 0.0; p < 181.0; p = p + phiInc)
    {
      for(float p1 = 0.0; p1 < 361.0; p1 = p1 + phi1Inc)
      {
//        std::cout << "TESTING EULER ANGLE (Degrees): " << p1 << ", " << p << ", " << p2 << std::endl;
        TestEulerAngle(p1 * DREAM3D::Constants::k_PiOver180, p * DREAM3D::Constants::k_PiOver180, p2 * DREAM3D::Constants::k_PiOver180);
      }
    }
  }



}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{

  int err = EXIT_SUCCESS;

  // DREAM3D_REGISTER_TEST( TestEulerConversion() );
  DREAM3D_REGISTER_TEST( TestFilterDesign() );

  return err;
}
