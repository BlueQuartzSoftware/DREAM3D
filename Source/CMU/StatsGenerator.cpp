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

#include "StatsGenerator.h"
#include "StatsGenerator/StatsGen.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenerator::StatsGenerator()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenerator::~StatsGenerator()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenerator::computeBinsAndCutOffs( std::vector<double> &binsizes,
                           std::vector<double> &xCo,
                           std::vector<double> &yCo,
                           double &xMax, double &yMax,
                           std::vector<double> &x,
                           std::vector<double> &y)
{

  double mu = 1.0;
  double sigma = 1.0;
  double cutOff = 1.0;
  double binStepSize = 1.0;
  int err = 0;
  mu = m_Mu;

  sigma = m_Sigma;

  cutOff = m_SigmaCutOff;

  binStepSize = m_BinStepSize;


  int size = 100;

  StatsGen sg;
  err = sg.GenLogNormal<std::vector<double> > (mu, sigma, x, y, size);
  if (err == 1)
  {
    //TODO: Present Error Message
    return -1;
  }

//  double xMax = std::numeric_limits<double >::min();
//  double yMax = std::numeric_limits<double >::min();
  for (int i = 0; i < size; ++i)
  {
    //   std::cout << x[i] << "  " << y[i] << std::endl;
    if (x[i] > xMax)
    {
      xMax = x[i];
    }
    if (y[i] > yMax)
    {
      yMax = y[i];
    }
  }

  xCo.clear();
  yCo.clear();
  int numsizebins = 1;
  binsizes.clear();
  // QwtArray<int> numgrains;
  err = sg.GenCutOff<double, std::vector<double> > (mu, sigma, cutOff, binStepSize, xCo, yCo, yMax, numsizebins, binsizes);

  return 0;

}
