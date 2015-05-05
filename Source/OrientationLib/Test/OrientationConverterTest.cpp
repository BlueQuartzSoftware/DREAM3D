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
void TestEulerConversion()
{
  size_t nTuples = 2;
  int qStride = 4;
  QVector<size_t> cDims(1, 3);
  FloatArrayType::Pointer eulers = FloatArrayType::CreateArray(nTuples, cDims, "Eulers");
  // Initialize the Eulers with some values
  eulers->setComponent(0, 0, DREAM3D::Constants::k_PiOver2);
  eulers->setComponent(0, 1, 0.0f);
  eulers->setComponent(0, 2, 0.0f);
  eulers->setComponent(1, 0, DREAM3D::Constants::k_PiOver4);
  eulers->setComponent(1, 1, 0.0f);
  eulers->setComponent(1, 2, 0.0f);

  OrientationConverter<float>::Pointer ocEulers =  EulerConvertor<float>::New();
  ocEulers->setInputData(eulers);
  ocEulers->convertRepresentationTo(OrientationConverter<float>::Quaternion);

  FloatArrayType::Pointer output = ocEulers->getOutputData();

  for(size_t i = 0; i < nTuples; i++)
  {
    float* ptr = output->getPointer(i * qStride);
    Print_QU(ptr);
  }

  ocEulers->convertRepresentationTo(OrientationConverter<float>::AxisAngle);
  output = ocEulers->getOutputData();

  for(size_t i = 0; i < nTuples; i++)
  {
    float* ptr = output->getPointer(i * qStride);
    Print_QU(ptr);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestFilterDesign()
{

  size_t nTuples = 2;
  int qStride = 4;
  QVector<size_t> cDims(1, 3);
  FloatArrayType::Pointer eulers = FloatArrayType::CreateArray(nTuples, cDims, "Eulers");
  // Initialize the Eulers with some values
  eulers->setComponent(0, 0, DREAM3D::Constants::k_PiOver2);
  eulers->setComponent(0, 1, 0.0f);
  eulers->setComponent(0, 2, 0.0f);
  eulers->setComponent(1, 0, DREAM3D::Constants::k_PiOver4);
  eulers->setComponent(1, 1, 0.0f);
  eulers->setComponent(1, 2, 0.0f);




  typedef OrientationConverter<float> OCType;
  QVector<OCType::Pointer> converters(7);

  converters[0] = EulerConvertor<float>::New();
  converters[1] = OrientationMatrixConvertor<float>::New();
  converters[2] = QuaternionConvertor<float>::New();
  converters[3] = AxisAngleConvertor<float>::New();
  converters[4] = RodriguesConvertor<float>::New();
  converters[5] = HomochoricConvertor<float>::New();
  converters[6] = CubochoricConvertor<float>::New();

  QVector<OCType::OrientationType> ocTypes(7);
  ocTypes[0] = OCType::Euler;
  ocTypes[1] = OCType::OrientationMatrix;
  ocTypes[2] = OCType::Quaternion;
  ocTypes[3] = OCType::AxisAngle;
  ocTypes[4] = OCType::Rodrigues;
  ocTypes[5] = OCType::Homochoric;
  ocTypes[6] = OCType::Cubochoric;


  int selectionIndex = 0;
  int outputIndex = 3;
  converters[selectionIndex]->setInputData(eulers);
  converters[selectionIndex]->convertRepresentationTo(ocTypes[outputIndex]);

  FloatArrayType::Pointer output = converters[selectionIndex]->getOutputData();

  for(size_t i = 0; i < nTuples; i++)
  {
    float* ptr = output->getPointer(i * qStride);
    Print_QU(ptr);
  }


}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{

  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( TestEulerConversion() );
  DREAM3D_REGISTER_TEST( TestFilterDesign() );



  return err;
}
