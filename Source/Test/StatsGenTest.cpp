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


#include "StatsGenerator/StatsGen.cpp"
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
  StatsGen sg;

  int err = 0;
  double alpha = 1.0;
  double beta = 1.0;

  std::vector<double> x;
  std::vector<double> y;
  int size = 1000;
  err = sg.GenBetaPlotData(alpha, beta, x, y, size);


  double avg = 1.0;
  double stdDev = 0.5;
  err = sg.GenLogNormalPlotData(avg, stdDev, x, y, size);

  double k = 1.4;
  err = sg.GenPowerLawPlotData(alpha, k, beta, x, y, size);


  double mu = 1.0;
  double sigma = 1.0;
  double cutoff = 4.0;
  double binstep = 0.75;
  double max, min;

  err = sg.computeNumberOfBins(mu, sigma, cutoff, binstep, max, min);

  std::vector<double> binsizes;
  err = sg.GenCutOff(mu, sigma, cutoff, binstep, x, y, max, size, binsizes);


  std::vector<double> odf;
  std::vector<double> x1;
  std::vector<double> y1;
  std::vector<double> x2;
  std::vector<double> y2;
  std::vector<double> x3;
  std::vector<double> y3;
  int npoints = 1000;
  err = sg.GenCubicODFPlotData(odf, x1, y1, x2, y2, x3, y3, npoints);


  err = sg.GenHexODFPlotData(odf, x1, y1, x2, y2, x3, y3, npoints);


  err = sg.GenOrthoRhombicODFPlotData(odf, x1, y1, x2, y2, x3, y3, npoints);

  err = sg.GenAxisODFPlotData(odf, x1, y1, x2, y2, x3, y3, npoints);

  std::vector<double> mdf;

  err = sg.GenCubicMDFPlotData(mdf, x, y, npoints);

  err = sg.GenHexMDFPlotData(mdf, x, y, npoints);

  return EXIT_SUCCESS;
}
