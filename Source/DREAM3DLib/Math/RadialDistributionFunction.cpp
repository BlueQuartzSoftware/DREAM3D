/* ============================================================================
 * Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson, BlueQuartz Software nor the names of
 * its contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "RadialDistributionFunction.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "math.h"

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
  std::vector<float> randomCentroids(largeNumber*3);
  int32_t largeNumber = 1000;

  int32_t xpoints;
  int32_t ypoints;
  int32_t zpoints;

  size_t featureOwnderIdx = 0;

  float stepsize = (maxDistance-minDistance)/numBins;
  float maxBoxDistance = sqrtf((boxdims[0]*boxdims[0]) + (boxdims[1]*boxdims[1]) + (boxdims[2]*boxdims[2]))

  DREAM3D_RANDOMNG_NEW();

  for (size_t i = 0; i < largeNumber; i++)
  {
       featureOwnderIdx = static_cast<size_t>(rg.genrand_res53() * xpoints * ypoints * zpoints);

       column = featureOwnersIdx % xpoints;
       row = int(featureOwnersIdx / xpoints) % ypoints;
       plane = featureOwnersIdx / (xpoints * ypoints);

       xc = static_cast<float>(column * boxres[0]) ;
       yc = static_cast<float>(row * boxres[1]);
       zc = static_cast<float>(plane * boxres[2]);

       randomCentroids[3*i] = xc;
       randomCentroids[3*i+1] = yc;
       randomCentroids[3*i+2] = zc;

  }










  return freq;
}
