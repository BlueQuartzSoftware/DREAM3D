/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "RadialDistributionFunction.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"
#include "math.h"
#include <fstream>
#include <iostream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RdfData::RdfData() :
  m_MinDistance(0.0f),
  m_MaxDistance(0.0f),
  m_DistributionType(DREAM3D::StringConstants::UnknownDistribution)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RdfData::~RdfData()
{

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RadialDistributionFunction::RadialDistributionFunction()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RadialDistributionFunction::~RadialDistributionFunction()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<float> RadialDistributionFunction::GenerateRandomDistribution(float minDistance, float maxDistance, int numBins, std::vector<float> boxdims, std::vector<float> boxres)
{
  std::vector<float> freq(numBins, 0);
  std::vector<float> randomCentroids;
  std::vector<std::vector<float> > distancelist;
  int32_t largeNumber = 1000;
  int32_t numDistances = largeNumber * (largeNumber - 1);

  // boxdims are the dimensions of the box in microns
  // boxres is the resoultion of the box in microns
  int32_t xpoints = boxdims[0] / boxres[0];
  int32_t ypoints = boxdims[1] / boxres[1];
  int32_t zpoints = boxdims[2] / boxres[2];
  int32_t bin;
  int32_t totalpoints = xpoints * ypoints * zpoints;

  float x, y, z;
  float xn, yn, zn;
  float xc, yc, zc;
  float r;

  size_t featureOwnerIdx = 0;
  size_t column, row, plane;


  float stepsize = (maxDistance - minDistance) / numBins;
  float maxBoxDistance = sqrtf((boxdims[0] * boxdims[0]) + (boxdims[1] * boxdims[1]) + (boxdims[2] * boxdims[2]));
  int32_t current_num_bins = ceil((maxBoxDistance - minDistance) / (stepsize));

  freq.resize(current_num_bins + 1);


  SIMPL_RANDOMNG_NEW();

  randomCentroids.resize(largeNumber * 3);

  //Generating all of the random points and storing their coordinates in randomCentroids
  for (int32_t i = 0; i < largeNumber; i++)
  {
    featureOwnerIdx = static_cast<size_t>(rg.genrand_res53() * totalpoints);

    column = featureOwnerIdx % xpoints;
    row = int(featureOwnerIdx / xpoints) % ypoints;
    plane = featureOwnerIdx / (xpoints * ypoints);

    xc = static_cast<float>(column * boxres[0]) ;
    yc = static_cast<float>(row * boxres[1]);
    zc = static_cast<float>(plane * boxres[2]);

    randomCentroids[3 * i] = xc;
    randomCentroids[3 * i + 1] = yc;
    randomCentroids[3 * i + 2] = zc;

  }

  distancelist.resize(largeNumber);


//Calculating all of the distances and storing them in the distnace list
  for (int32_t i = 1; i < largeNumber; i++)
  {

    x = randomCentroids[3 * i];
    y = randomCentroids[3 * i + 1];
    z = randomCentroids[3 * i + 2];

    for (int32_t j = i + 1; j < largeNumber; j++)
    {

      xn = randomCentroids[3 * j];
      yn = randomCentroids[3 * j + 1];
      zn = randomCentroids[3 * j + 2];

      r = sqrtf((x - xn) * (x - xn) + (y - yn) * (y - yn) + (z - zn) * (z - zn));

      distancelist[i].push_back(r);
      distancelist[j].push_back(r);

    }

  }

  //bin up the distance list
  for (int32_t i = 0; i < largeNumber; i++)
  {
    for (size_t j = 0; j < distancelist[i].size(); j++)
    {
      bin = (distancelist[i][j] - minDistance) / stepsize;

      if (distancelist[i][j] < minDistance)
      {
        bin = -1;
      }
      freq[bin + 1]++;

    }
  }

  for (int32_t i = 0; i < current_num_bins + 1; i++)
  {
    freq[i] = freq[i] / (numDistances);
  }


  return freq;

}
