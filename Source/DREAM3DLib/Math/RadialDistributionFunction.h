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
#ifndef _RadialDistributionFunction_H_
#define _RadialDistributionFunction_H_

#include <vector>

#include "DREAM3DLib/DREAM3DLib.h"

/**
 * @brief The RadialDistributionFunction class can generate different types of distributions
 * for a Radial Distribution Function
 */
class DREAM3DLib_EXPORT RadialDistributionFunction
{
  public:

    virtual ~RadialDistributionFunction();

    /**
     * @brief GenerateRandomDistribution
     * @param minDistance The minimum distance between objects
     * @param maxDistance The maximum distance between objects
     * @param numBins The number of bins to generate
     * @return An array of values that are the frequency values for the histogram
     */
    static std::vector<float> GenerateRandomDistribution(float minDistance, float maxDistance, int numBins, std::vector<float> boxdims, std::vector<float> boxres);

  protected:
    RadialDistributionFunction();

  private:
    RadialDistributionFunction(const RadialDistributionFunction&); // Copy Constructor Not Implemented
    void operator=(const RadialDistributionFunction&); // Operator '=' Not Implemented
};


#endif /* _RadialDistributionFunction_H_ */
