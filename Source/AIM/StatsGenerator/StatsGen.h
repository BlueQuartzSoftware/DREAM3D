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

#ifndef _STATSGEN_H_
#define _STATSGEN_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <math.h>

/**
 * @class StatsGen StatsGen.h
 * @author Michael A. Groeber
 * @date Dec 16, 2010
 * @version 1.0
 */
template<typename T>
class StatsGen
{
  public:
    StatsGen();
    virtual ~StatsGen();

    int GenBeta(double p, double q, T &x, T &y, int size)
    {
      int err=0;
      double value, gammapq, gammap, gammaq, betain, betaout;
      x.resize(size);
      y.resize(size);
      value = p;
      gammap = gamma(value);
      value = q;
      gammaq = gamma(value);
      value = p+q;
      gammapq = gamma(value);
      for(int i=0;i<size;i++)
      {
        betain = (i*(1.0/double(size)))+((1.0/double(size))/2.0);
        betaout = (gammapq/(gammap*gammaq))*pow(betain,(p-1))*pow((1-betain),(q-1));
        x[i] = betain;
        y[i] = betaout*(1.0/double(size));
        if(betaout < 0) err = 1;
      }
      return err;
    }

#if 0
    int GenLogNormal(double m, double s, std::vector<float> &x, std::vector<float> &y, int size);
    int GenCutOff(double m, double s, double value, std::vector<int> &cutoffs);
#endif
    double gamma(double value);


  private:
    StatsGen(const StatsGen&); // Copy Constructor Not Implemented
    void operator=(const StatsGen&); // Operator '=' Not Implemented
};

#include "StatsGen.txx"

#endif /* _STATSGEN_H_ */
