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


#include <stdio.h>

#include <iostream>
#include <iomanip>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/OrientationMath/OrientationConverter.hpp"
#include "OrientationLib/SpaceGroupOps/CubicOps.h"

#include "TestPrintFunctions.h"

class OrientationConverterTest
{
  public:
    OrientationConverterTest(){}
    virtual ~OrientationConverterTest(){}



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
      eulers->setComponent(0, 0, 302.84f * SIMPLib::Constants::k_PiOver180);
      eulers->setComponent(0, 1, 51.282f * SIMPLib::Constants::k_PiOver180);
      eulers->setComponent(0, 2, 37.969f * SIMPLib::Constants::k_PiOver180);
      eulers->setComponent(1, 0, 45.0f * SIMPLib::Constants::k_PiOver180);
      eulers->setComponent(1, 1, 0.0f * SIMPLib::Constants::k_PiOver180);
      eulers->setComponent(1, 2, 0.0f * SIMPLib::Constants::k_PiOver180);

      float rad = 302.84 * SIMPLib::Constants::k_PiOver180;
      std::cout << "Rad: " << rad << std::endl;
      std::cout << "2Pi: " << SIMPLib::Constants::k_2Pi << std::endl;


      //std::cout << "Remainer (302.84/360): " << remainder(rad, SIMPLib::Constants::k_2Pi) << std::endl;
      std::cout << "fmod (5.28556 / 2Pi): " << fmod(rad, SIMPLib::Constants::k_2Pi) << std::endl;


      OrientationConverter<float>::Pointer ocEulers =  EulerConverter<float>::New();
      ocEulers->setInputData(eulers);
      ocEulers->convertRepresentationTo(OrientationConverter<float>::Quaternion);

      FloatArrayType::Pointer output = ocEulers->getOutputData();

      for(size_t i = 0; i < nTuples; i++)
      {
        float* ptr = output->getPointer(i * qStride);
        OrientationPrinters::Print_QU<float*, float>(ptr);
      }

    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void TestEulerAngle(float phi1, float phi, float phi2)
    {
    //  std::cout << "TESTING EULER ANGLE (Rad): " << phi1 << ", " << phi << ", " << phi2 << std::endl;

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
      converters[0] = EulerConverter<float>::New();
      converters[1] = OrientationMatrixConverter<float>::New();
      converters[2] = QuaternionConverter<float>::New();
      converters[3] = AxisAngleConverter<float>::New();
      converters[4] = RodriguesConverter<float>::New();
      converters[5] = HomochoricConverter<float>::New();
      //converters[6] = CubochoricConverter<float>::New();

      typedef  OrientationTransforms<FOrientArrayType, float> OrientationTransformType;
      OrientationTransformType::ResultType result;
      QVector<int> strides = OCType::GetComponentCounts();

      for(int t0 = 0; t0 < 1; t0++)
      {
        for(int t1 = 1; t1 < converters.size(); t1++)
        {
          if (t0 == t1) { continue; }

          converters[t0]->setInputData(eulers);
          converters[t0]->convertRepresentationTo(ocTypes[t1]);
          FloatArrayType::Pointer t1_output = converters[t0]->getOutputData();

          converters[t1]->setInputData(t1_output);
          converters[t1]->convertRepresentationTo(ocTypes[t0]);
          FloatArrayType::Pointer t0_output = converters[t1]->getOutputData();

          qStride = strides[t0];
          std::vector<float> delta(qStride, 0);
          for(size_t i = 0; i < nTuples; i++)
          {
            float* orig = eulers->getPointer(i * qStride);
            float* converted = t0_output->getPointer(i * qStride);
            //printf("%s -> %s -> %s\n", tStrings[t0].toLatin1().constData(), tStrings[t1].toLatin1().constData(), tStrings[t0].toLatin1().constData());
            for(size_t j = 0; j < qStride; j++)
            {
              delta[j] = std::abs(orig[j] - converted[j]);
              DREAM3D_REQUIRE(delta[j] < 1.0E6);
            }
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
            TestEulerAngle(p1 * SIMPLib::Constants::k_PiOver180, p * SIMPLib::Constants::k_PiOver180, p2 * SIMPLib::Constants::k_PiOver180);
          }
        }
      }



    }

    void operator()()
    {
      int err = 0;
      DREAM3D_REGISTER_TEST( TestEulerConversion() );
      DREAM3D_REGISTER_TEST( TestFilterDesign() );
    }

  private:
    OrientationConverterTest(const OrientationConverterTest&); // Copy Constructor Not Implemented
    void operator=(const OrientationConverterTest&); // Operator '=' Not Implemented
};
