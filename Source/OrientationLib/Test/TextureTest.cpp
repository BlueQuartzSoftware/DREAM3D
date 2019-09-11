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



#include <iostream>
#include <vector>
#include <string>

#include "OrientationLib/Texture/Texture.hpp"
#include "OrientationLib/LaueOps/CubicOps.h"


/**
 * @brief These tests are just here to make sure the code compiles. The tests will
 * not actually work or would probably produced undefined results.
 * @param argc
 * @param argv
 * @return
 */
class TextureTest
{
  public:
    TextureTest() = default;
    ~TextureTest() = default;

    void operator()()
    {
      QVector<float> e1s;
      QVector<float> e2s;
      QVector<float> e3s;
      QVector<float> weights;
      QVector<float> sigmas;
      QVector<float> odf;

      CubicOps cubicOps;
      HexagonalOps hexagonalOps;
      OrthoRhombicOps orthOps;

      size_t numEntries = e1s.size();
      odf.resize(cubicOps.getODFSize());
      Texture::CalculateCubicODFData(e1s.data(), e2s.data(), e3s.data(), weights.data(), sigmas.data(), true, odf.data(), numEntries);
      odf.resize(hexagonalOps.getODFSize());
      Texture::CalculateHexODFData(e1s.data(), e2s.data(), e3s.data(), weights.data(), sigmas.data(), true,
                                   odf.data(), numEntries);


      odf.resize(orthOps.getODFSize());
      Texture::CalculateOrthoRhombicODFData(e1s.data(), e2s.data(), e3s.data(), weights.data(), sigmas.data(), true,
                                            odf.data(), numEntries);


      //int size = 1000;
      // Now generate the actual XY point data that gets plotted.
      // These are the output vectors
      QVector<float> angles;
      QVector<float> axes;
      QVector<float> mdf(CubicOps::k_MdfSize);

      Texture::CalculateMDFData<float, CubicOps>(angles.data(), axes.data(), weights.data(), odf.data(), mdf.data(), angles.size());
    }

  public:
    TextureTest(const TextureTest&) = delete;            // Copy Constructor Not Implemented
    TextureTest(TextureTest&&) = delete;                 // Move Constructor Not Implemented
    TextureTest& operator=(const TextureTest&) = delete; // Copy Assignment Not Implemented
    TextureTest& operator=(TextureTest&&) = delete;      // Move Assignment Not Implemented
};
