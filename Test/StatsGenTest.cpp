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
#include <iostream>
#include <vector>
#include <string>
#include <map>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/StatsGen.h"


#include "UnitTestSupport.hpp"

void genHexMDFPlotData()
{
  StatsGen sg;
  int err = 0;
  int size = 2500;
  std::vector<float> e1s;
  std::vector<float> e2s;
  std::vector<float> e3s;
  std::vector<float> weights;
  std::vector<float> sigmas;
  std::vector<float> odf;
  static const size_t odfsize = 15552;
  odf.resize(odfsize);
  Texture::CalculateHexODFData<std::vector<float>, float>(e1s, e2s, e3s, weights, sigmas, true, odf);


  // These are the output vectors
  std::vector<float> x;
  std::vector<float> y;

  // These are the input vectors
  std::vector<float> angles;
  std::vector<float> axes;


  // Allocate a new vector to hold the mdf data
  std::vector<float> mdf(15552);
  // Calculate the MDF Data using the ODF data and the rows from the MDF Table model
  Texture::CalculateMDFData<std::vector<float>, HexagonalOps, float>(angles, axes, weights, odf, mdf);

  size = 1000;
  // Now generate the actual XY point data that gets plotted.
  err = sg.genHexMDFPlotData<std::vector<float>, float>(mdf, x, y, size);

  DREAM3D_REQUIRE(err >= 0)
}



/**
 * @brief This code is just here to make sure each of the methods in the StatsGen
 *  class gets compiled due to some compilers NOT compiling the templates unless
 *  they are explicitly instantiated.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST(genHexMDFPlotData())

  PRINT_TEST_SUMMARY();
  //return err;
#if 1
  StatsGen sg;


  float alpha = 1.0;
  float beta = 1.0;

  std::vector<float> x;
  std::vector<float> y;
  int size = 1000;
  err = sg.genBetaPlotData(alpha, beta, x, y, size);


  float avg = 1.0f;
  float stdDev = 0.5f;
  err = sg.genLogNormalPlotData(avg, stdDev, x, y, size);

  float k = 1.4f;
  err = sg.genPowerLawPlotData(alpha, k, beta, x, y, size);


  float mu = 1.0f;
  float sigma = 1.0f;
  float cutoff = 4.0f;
  float maxCutOff = 4.0f;
  float binstep = 0.75f;
  float max, min;

  err = sg.computeNumberOfBins(mu, sigma, cutoff, maxCutOff, binstep, max, min);

  std::vector<float> binsizes;
  err = sg.genCutOff(mu, sigma, cutoff, maxCutOff, binstep, x, y, max, size, binsizes);

  typedef std::vector<float> VectorF;
  VectorF odf;
  VectorF x1;
  VectorF y1;
  VectorF x2;
  VectorF y2;
  VectorF x3;
  VectorF y3;
  int npoints = 1000;
  err = sg.genCubicODFPlotData<VectorF, float>(odf, x1, y1, x2, y2, x3, y3, npoints);


  err = sg.genHexODFPlotData<VectorF, float>(odf, x1, y1, x2, y2, x3, y3, npoints);


  err = sg.genOrthoRhombicODFPlotData<VectorF, float>(odf, x1, y1, x2, y2, x3, y3, npoints);

  err = sg.genAxisODFPlotData<VectorF, float>(odf, x1, y1, x2, y2, x3, y3, npoints);

  std::vector<float> mdf;

  err = sg.genCubicMDFPlotData<VectorF, float>(mdf, x, y, npoints);

  err = sg.genHexMDFPlotData<VectorF, float>(mdf, x, y, npoints);

  return EXIT_SUCCESS;
#endif
}
