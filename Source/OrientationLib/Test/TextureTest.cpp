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
#include "OrientationLib/Texture/StatsGen.hpp"
#include "OrientationLib/LaueOps/CubicLowOps.h"
#include "OrientationLib/LaueOps/CubicOps.h"
#include "OrientationLib/LaueOps/HexagonalLowOps.h"
#include "OrientationLib/LaueOps/HexagonalOps.h"
#include "OrientationLib/LaueOps/MonoclinicOps.h"
#include "OrientationLib/LaueOps/OrthoRhombicOps.h"
#include "OrientationLib/LaueOps/TetragonalLowOps.h"
#include "OrientationLib/LaueOps/TetragonalOps.h"
#include "OrientationLib/LaueOps/TriclinicOps.h"
#include "OrientationLib/LaueOps/TrigonalLowOps.h"
#include "OrientationLib/LaueOps/TrigonalOps.h"

#include "UnitTestSupport.hpp"

#include "OrientationLibTestFileLocations.h"

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

    template <class LaueOps>
    void TestTextureMdf()
    {
      LaueOps ops;
      std::cout << "======================================================" << std::endl;
      std::cout << ops.getNameOfClass().toStdString() << " MDF Plot Values" << std::endl;

      std::vector<float> odf;

      int size = 10000;
      std::vector<float> e1s;
      std::vector<float> e2s;
      std::vector<float> e3s;
      std::vector<float> sigmas;
      std::vector<float> angles;
      std::vector<float> axes;
      std::vector<float> weights;
      size_t numEntries = static_cast<size_t>(e1s.size());
      std::cout << "   Generating ODF....." << std::endl;
      Texture::CalculateODFData<float, LaueOps, std::vector<float>>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);

      // Allocate a new vector to hold the mdf data
      std::vector<float> mdf;

      std::cout << "   Generating MDF....." << std::endl;

      // Calculate the MDF Data using the ODF data and the rows from the MDF Table model
      Texture::CalculateMDFData<float, LaueOps, std::vector<float>>(angles, axes, weights, odf, mdf, static_cast<size_t>(angles.size()));
      // Now generate the actual XY point data that gets plotted.
      // These are the output vectors

      int npoints = 36;
      std::vector<float> x(npoints);
      std::vector<float> y(npoints);
      std::cout << "   Generating MDF Plot Data....." << std::endl;

      int32_t err = StatsGen::GenMDFPlotData<float, LaueOps, std::vector<float>>(mdf, x, y, size);
      if(err < 0)
      {
        std::cout << "Error Generating MDF Plot Valeus" << std::endl;
        return;
      }

      std::cout << "    npoints: " << x.size() << std::endl;
      for(size_t i = 0; i < x.size(); i++)
      {
        std::cout << x[i] << ", " << y[i] << std::endl;
      }
    }

    void TestMdfGeneration()
    {
      TestTextureMdf<CubicLowOps>();
      TestTextureMdf<CubicOps>();
      TestTextureMdf<HexagonalLowOps>();
      TestTextureMdf<HexagonalOps>();
      TestTextureMdf<MonoclinicOps>();
      TestTextureMdf<OrthoRhombicOps>();
      TestTextureMdf<TetragonalLowOps>();
      TestTextureMdf<TetragonalOps>();
      TestTextureMdf<TriclinicOps>();
      TestTextureMdf<TrigonalLowOps>();
      TestTextureMdf<TrigonalOps>();
    }

    template <class LaueOps>
    void TestTextureOdf()
    {
      std::vector<float> e1s;
      std::vector<float> e2s;
      std::vector<float> e3s;
      std::vector<float> weights;
      std::vector<float> sigmas;
      std::vector<float> odf;

      LaueOps ops;
      size_t numEntries = e1s.size();
      Texture::CalculateODFData<float, LaueOps, std::vector<float>>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
    }

    void TestOdfGeneration()
    {
      TestTextureOdf<CubicLowOps>();
      TestTextureOdf<CubicOps>();
      TestTextureOdf<HexagonalLowOps>();
      TestTextureOdf<HexagonalOps>();
      TestTextureOdf<MonoclinicOps>();
      TestTextureOdf<OrthoRhombicOps>();
      TestTextureOdf<TetragonalLowOps>();
      TestTextureOdf<TetragonalOps>();
      TestTextureOdf<TriclinicOps>();
      TestTextureOdf<TrigonalLowOps>();
      TestTextureOdf<TrigonalOps>();
    }

    void operator()()
    {
      std::cout << "#### TextureTest Starting ####" << std::endl;

      int err = EXIT_SUCCESS;
      DREAM3D_REGISTER_TEST(TestOdfGeneration())
      DREAM3D_REGISTER_TEST(TestMdfGeneration())
    }

  public:
    TextureTest(const TextureTest&) = delete;            // Copy Constructor Not Implemented
    TextureTest(TextureTest&&) = delete;                 // Move Constructor Not Implemented
    TextureTest& operator=(const TextureTest&) = delete; // Copy Assignment Not Implemented
    TextureTest& operator=(TextureTest&&) = delete;      // Move Assignment Not Implemented
};
