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

/**
 * @brief main.cpp This is mainly a test to make sure that the Texture.h
 * file will compile using strict STL containers
 * @param argc
 * @param argv
 * @return
 */
#include <iostream>
#include <vector>

#include "SIMPLib/Math/SIMPLibMath.h"

#include "OrientationLib/Texture/StatsGen.hpp"
#include "OrientationLib/Texture/Texture.hpp"

#include "TestPrintFunctions.h"

#define POPULATE_DATA(i, e1, e2, e3, w, s)                                                                                                                                                             \
  e1s[i] = e1;                                                                                                                                                                                         \
  e2s[i] = e2;                                                                                                                                                                                         \
  e3s[i] = e3;                                                                                                                                                                                         \
  weights[i] = w;                                                                                                                                                                                      \
  sigmas[i] = s;

class ODFTest
{
public:
  ODFTest()
  {
  }
  virtual ~ODFTest()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template<typename T>
  void Print_Coord(const T* om)
  {
    printf("Coord:% 3.16f % 3.16f % 3.16f\n", om[0], om[1], om[2] );
  }

  void CubicODFTest()
  {
    // Resize the ODF vector properly for Cubic
    std::vector<float> odf(CubicOps::k_OdfSize);
    std::vector<float> e1s(2);
    std::vector<float> e2s(2);
    std::vector<float> e3s(2);
    std::vector<float> weights(2);
    std::vector<float> sigmas(2);

    POPULATE_DATA(0, 35, 45, 0, 1000.0, 2.0)
    POPULATE_DATA(1, 59, 37, 63, 1000.0, 1.0)

    // Calculate the ODF Data

    size_t numEntries = e1s.size();
    Texture::CalculateCubicODFData(&(e1s.front()), &(e2s.front()), &(e3s.front()), &(weights.front()), &(sigmas.front()), true, &(odf.front()), numEntries);

    size_t npoints = 1000;
    std::vector<float> x001(npoints * 3);
    std::vector<float> y001(npoints * 3);
    std::vector<float> x011(npoints * 6);
    std::vector<float> y011(npoints * 6);
    std::vector<float> x111(npoints * 4);
    std::vector<float> y111(npoints * 4);
  }

  void TestRotation()
  {
    float phi1 = 0.0f * SIMPLib::Constants::k_PiOver180;
    float PHI = 180.0f;
    float phi2 = 0.0f;

    float ga[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    FOrientArrayType om(9, 0.0);
    FOrientTransformsType::eu2om(FOrientArrayType(phi1, PHI, phi2), om);
    om.toGMatrix(ga);

    float coordsRotated[3] = {0.0f, 0.0f, 0.0f};
    float coords[3] = {0.0f, 0.0f, 0.0f};
    float xc = -0.0;
    float yc = -5.0;
    float zc = 0.0;
    coords[0] = coords[0] - xc;
    coords[1] = coords[1] - yc;
    coords[2] = coords[2] - zc;
    MatrixMath::Multiply3x3with3x1(ga, coords, coordsRotated);

    Print_Coord<float>(coords);
    Print_Coord<float>(coordsRotated);

  }




  void operator()()
  {

    TestRotation();
    CubicODFTest();

    int err = 0;

    if(err == 1)
    {
      // TODO: Present Error Message
      return;
    }
  }

private:
  ODFTest(const ODFTest&);        // Copy Constructor Not Implemented
  void operator=(const ODFTest&); // Operator '=' Not Implemented
};
