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

/**
 * @brief main.cpp This is mainly a test to make sure that the Texture.h
 * file will compile using strict STL containers
 * @param argc
 * @param argv
 * @return
 */
#include <iostream>
#include <vector>


#include "DREAM3DLib/Common/Texture.hpp"
#include "DREAM3DLib/Common/StatsGen.h"

#define POPULATE_DATA(i, e1, e2, e3, w, s)\
  e1s[i] = e1;\
  e2s[i] = e2;\
  e3s[i] = e3;\
  weights[i] = w;\
  sigmas[i] = s;



int main(int argc, char **argv)
{
  std::vector<float> odf;
  std::vector<float> e1s(2);
  std::vector<float> e2s(2);
  std::vector<float> e3s(2);
  std::vector<float> weights(2);
  std::vector<float> sigmas(2);

  POPULATE_DATA(0, 35, 45, 0, 1.0, 1.0)
  POPULATE_DATA(1, 59, 37, 63, 1.0, 1.0)
  // Resize the ODF vector properly for Cubic
  odf.resize(5832);

  // Calculate the ODF Data
  Texture::CalculateCubicODFData<std::vector<float>, float>(e1s, e2s, e3s, weights, sigmas, true, odf);


  std::vector<float > x001;
  std::vector<float > y001;
  std::vector<float > x011;
  std::vector<float > y011;
  std::vector<float > x111;
  std::vector<float > y111;

  StatsGen sg;
  int size = 1000;
  int err = 0;
  err = sg.genCubicODFPlotData<std::vector<float>, float>(odf, x001, y001, x011, y011, x111, y111, size);
  if (err == 1)
  {
    //TODO: Present Error Message
    return 1;
  }


  return 0;
}

