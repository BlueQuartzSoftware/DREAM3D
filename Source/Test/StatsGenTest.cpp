/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "UnitTestSupport.hpp"

#include "StatsGenerator/StatsGen.cpp"

void GenHexMDFPlotData()
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
  float totalweight = 0;
  odf.resize(odfsize);
  Texture::calculateHexODFData(e1s, e2s, e3s, weights, sigmas, true, odf, totalweight);


  // These are the output vectors
  std::vector<float> x;
  std::vector<float> y;

  // These are the input vectors
  std::vector<float> angles;
  std::vector<float> axes;


  // Allocate a new vector to hold the mdf data
  std::vector<float> mdf(15552);
  // Calculate the MDF Data using the ODF data and the rows from the MDF Table model
  Texture::calculateMDFData<std::vector<float>, HexagonalOps>(angles, axes, weights, odf, mdf);

  size = 1000;
  // Now generate the actual XY point data that gets plotted.
  err = sg.GenHexMDFPlotData(mdf, x, y, size);

  MXA_REQUIRE(err >= 0)
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
  MXA_REGISTER_TEST(GenHexMDFPlotData());

  PRINT_TEST_SUMMARY();
  return err;
#if 0
  StatsGen sg;

  int err = 0;
  float alpha = 1.0;
  float beta = 1.0;

  std::vector<float> x;
  std::vector<float> y;
  int size = 1000;
  err = sg.GenBetaPlotData(alpha, beta, x, y, size);


  float avg = 1.0;
  float stdDev = 0.5;
  err = sg.GenLogNormalPlotData(avg, stdDev, x, y, size);

  float k = 1.4;
  err = sg.GenPowerLawPlotData(alpha, k, beta, x, y, size);


  float mu = 1.0;
  float sigma = 1.0;
  float cutoff = 4.0;
  float binstep = 0.75;
  float max, min;

  err = sg.computeNumberOfBins(mu, sigma, cutoff, binstep, max, min);

  std::vector<float> binsizes;
  err = sg.GenCutOff(mu, sigma, cutoff, binstep, x, y, max, size, binsizes);


  std::vector<float> odf;
  std::vector<float> x1;
  std::vector<float> y1;
  std::vector<float> x2;
  std::vector<float> y2;
  std::vector<float> x3;
  std::vector<float> y3;
  int npoints = 1000;
  err = sg.GenCubicODFPlotData(odf, x1, y1, x2, y2, x3, y3, npoints);


  err = sg.GenHexODFPlotData(odf, x1, y1, x2, y2, x3, y3, npoints);


  err = sg.GenOrthoRhombicODFPlotData(odf, x1, y1, x2, y2, x3, y3, npoints);

  err = sg.GenAxisODFPlotData(odf, x1, y1, x2, y2, x3, y3, npoints);

  std::vector<float> mdf;

  err = sg.GenCubicMDFPlotData(mdf, x, y, npoints);

  err = sg.GenHexMDFPlotData(mdf, x, y, npoints);

  return EXIT_SUCCESS;
#endif
}
