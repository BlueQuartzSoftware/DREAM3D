/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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

/**
 * @brief main.cpp This is mainly a test to make sure that the Texture.h
 * file will compile using strict STL containers
 * @param argc
 * @param argv
 * @return
 */
#include <iostream>
#include <vector>


#include "AIM/Common/Texture.h"
#include "AIM/StatsGenerator/StatsGen.h"

int main(int argc, char **argv)
{
  static const size_t eighteenCubed = 5832;
  double totalweight = 0.0;
  std::vector<double> odf;
  std::vector<double> weights(Texture::Count);
  std::vector<double> sigmas(Texture::Count);
  double randomWeight = 1.0;

  //Copy the weights into the vector
  for (size_t i = 0; i < Texture::Count; ++i)
  {
    weights[i] = Texture::Weights[i];
    sigmas[i] = Texture::Sigmas[i];
  }
  // Resize the ODF vector properly for Cubic
  odf.resize(eighteenCubed);

  // Calculate the ODF Data
  Texture::calculateCubicODFData(weights, sigmas, randomWeight, true, odf, totalweight);


  std::vector<double > x001;
  std::vector<double > y001;
  std::vector<double > x011;
  std::vector<double > y011;
  std::vector<double > x111;
  std::vector<double > y111;

  StatsGen sg;
  int size = 1000;
  int err = 0;
  err = sg.GenCubicODFPlotData(weights, sigmas, x001, y001, x011, y011, x111, y111, size, randomWeight);
  if (err == 1)
  {
    //TODO: Present Error Message
    return 1;
  }


  return 0;
}

