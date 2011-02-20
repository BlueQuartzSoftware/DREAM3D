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

#include "AIM/Common/AIMMath.h"
#include "AIM/Common/Constants.h"
#include "AIM/Common/AIMRandomNG.h"

#include "AIM/Common/Texture.h"
#include "MXA/Common/LogTime.h"

/**
 * @class StatsGen StatsGen.h AIM/StatsGenerator/StatsGen.h
 * @author Michael A. Groeber US Air Force Research Laboratory
 * @date Dec 16, 2010
 * @version 1.0
 */
class StatsGen
{
  public:
    StatsGen();
    virtual ~StatsGen();

    template<typename T>
    int GenBeta(double alpha, double beta, T &x, T &y, int size)
    {
      int err = 0;
      double value, gammapq, gammap, gammaq, betain, betaout;
      x.resize(size);
      y.resize(size);
      value = alpha;
      gammap = gamma(value);
      value = beta;
      gammaq = gamma(value);
      value = alpha + beta;
      gammapq = gamma(value);
      for (int i = 0; i < size; i++)
      {
        betain = (i * (1.0 / double(size))) + ((1.0 / double(size)) / 2.0);
        betaout = (gammapq / (gammap * gammaq)) * pow(betain, (alpha - 1)) * pow((1 - betain), (beta - 1));
        x[i] = betain;
        y[i] = betaout * (1.0 / double(size));
        if (betaout < 0)
          err = 1;
      }
      return err;
    }


    template<typename T>
    int GenCubicODFPlotData(T weights, T sigmas,
                    T &x001, T &y001, T &x011, T &y011, T &x111, T &y111,
                    int size, double randomWeight)
    {
      static const size_t eighteenCubed = 5832;
      double totalweight = 0;
      T odf;
      odf.resize(eighteenCubed);
      Texture::calculateCubicODFData(weights, sigmas, randomWeight, true, odf, totalweight);

      AIMRandomNG rg;
      /* Get a seed value based off the system clock. The issue is that this will
       * be a 64 bit unsigned integer where the high 32 bits will basically not
       * change where as the lower 32 bits will. The following lines of code will
       * pull off the low 32 bits from the number. This operation depends on most
       * significant byte ordering which is different between Big Endian and
       * Little Endian machines. For Big endian machines the Most Significant Byte
       * (MSB) is the first 32 bits. For Little Endian machines the MSB is the
       * second 32 bits.
       */
      unsigned long long int seed = MXA::getMilliSeconds();
      unsigned int* seedPtr = reinterpret_cast<unsigned int*>(&seed);
#if CMP_WORDS_BIGENDIAN
      rg.RandomInit(seedPtr[1]);
#else
      rg.RandomInit(seedPtr[0]);
#endif
      int err = 0;
      int choose;
      double g[3][3];
      double x, y, z;
      double xpf, ypf;
      double xpfa, ypfa;
      double totaldensity;
      double hmag;
      double angle;
      double r1, r2, r3;
      double h1, h2, h3;
      double n1, n2, n3;
      double random, tan_angle, density, cos_angle, sin_angle;

      double dim1 = 2*pow((0.75 * ((M_PI / 4.0) - sin((M_PI / 4.0)))), (1.0 / 3.0));
      double dim2 = 2*pow((0.75 * ((M_PI / 4.0) - sin((M_PI / 4.0)))), (1.0 / 3.0));
      double dim3 = 2*pow((0.75 * ((M_PI / 4.0) - sin((M_PI / 4.0)))), (1.0 / 3.0));

      x001.resize(size * 3 * 4);
      y001.resize(size * 3 * 4);
      x011.resize(size * 6 * 4);
      y011.resize(size * 6 * 4);
      x111.resize(size * 4 * 4);
      y111.resize(size * 4 * 4);

      for (int i = 0; i < size; i++)
      {
        random = rg.Random();
        choose = 0;

        totaldensity = 0;
        for (size_t j = 0; j < eighteenCubed; j++)
        {
          density = odf[j];
          totaldensity = totaldensity + density;
          if (random < totaldensity && random >= (totaldensity - density)) choose = static_cast<int>(j);
        }
        h1 = choose % 18;
        h2 = (choose / 18) % 18;
        h3 = choose / (18 * 18);
        random = rg.Random();
        h1 = ((dim1 / 18.0) * h1) + ((dim1 / 18.0) * random) - (dim1 / 2.0);
        random = rg.Random();
        h2 = ((dim2 / 18.0) * h2) + ((dim2 / 18.0) * random) - (dim2 / 2.0);
        random = rg.Random();
        h3 = ((dim3 / 18.0) * h3) + ((dim3 / 18.0) * random) - (dim3 / 2.0);
        hmag = pow((h1 * h1 + h2 * h2 + h3 * h3), 0.5);
        angle = pow((8 * hmag * hmag * hmag), (1.0 / 3.0));
        tan_angle = tan(angle/2.0);
		n1 = h1/hmag;
		n2 = h2/hmag;
		n3 = h3/hmag;
        r1 = tan_angle * n1;
        r2 = tan_angle * n2;
        r3 = tan_angle * n3;
		cos_angle = cos(angle);
		sin_angle = sin(angle);
		g[0][0] = cos_angle + n1*n1*(1-cos_angle);
		g[0][1] = n1*n2*(1-cos_angle) - n3*sin_angle;
		g[0][2] = n1*n3*(1-cos_angle) + n2*sin_angle;
		g[1][0] = n1*n2*(1-cos_angle) + n3*sin_angle;
		g[1][1] = cos_angle + n2*n2*(1-cos_angle);
		g[1][2] = n2*n3*(1-cos_angle) - n1*sin_angle;
		g[2][0] = n1*n3*(1-cos_angle) - n2*sin_angle;
		g[2][1] = n2*n3*(1-cos_angle) + n1*sin_angle;
		g[2][2] = cos_angle + n3*n3*(1-cos_angle);
        x = g[0][0];
        y = g[1][0];
        z = g[2][0];
        if (z < 0) z = -z;
        xpf = y - (y * (z / (z + 1)));
        ypf = x - (x * (z / (z + 1)));
        random = rg.Random();
        x001[12 * i] = xpf;
        y001[12 * i] = ypf;
		xpfa = -xpf;
		ypfa = ypf;
        x001[12 * i + 1] = xpfa;
        y001[12 * i + 1] = ypfa;
		xpfa = xpf;
		ypfa = -ypf;
        x001[12 * i + 2] = xpfa;
        y001[12 * i + 2] = ypfa;
		xpfa = -xpf;
		ypfa = -ypf;
        x001[12 * i + 3] = xpfa;
        y001[12 * i + 3] = ypfa;
        x = g[0][1];
        y = g[1][1];
        z = g[2][1];
        if (z < 0) z = -z;
        xpf = y - (y * (z / (z + 1)));
        ypf = x - (x * (z / (z + 1)));
        x001[12 * i + 4] = xpf;
        y001[12 * i + 4] = ypf;
		xpfa = -xpf;
		ypfa = ypf;
        x001[12 * i + 5] = xpfa;
        y001[12 * i + 5] = ypfa;
		xpfa = xpf;
		ypfa = -ypf;
        x001[12 * i + 6] = xpfa;
        y001[12 * i + 6] = ypfa;
		xpfa = -xpf;
		ypfa = -ypf;
        x001[12 * i + 7] = xpfa;
        y001[12 * i + 7] = ypfa;
        x = g[0][2];
        y = g[1][2];
        z = g[2][2];
        if (z < 0) z = -z;
        xpf = y - (y * (z / (z + 1)));
        ypf = x - (x * (z / (z + 1)));
        x001[12 * i + 8] = xpf;
        y001[12 * i + 8] = ypf;
		xpfa = -xpf;
		ypfa = ypf;
        x001[12 * i + 9] = xpfa;
        y001[12 * i + 9] = ypfa;
		xpfa = xpf;
		ypfa = -ypf;
        x001[12 * i + 10] = xpfa;
        y001[12 * i + 10] = ypfa;
		xpfa = -xpf;
		ypfa = -ypf;
        x001[12 * i + 11] = xpfa;
        y001[12 * i + 11] = ypfa;
        x = 0.707107 * (g[0][0] + g[0][1]);
        y = 0.707107 * (g[1][0] + g[1][1]);
        z = 0.707107 * (g[2][0] + g[2][1]);
        if (z < 0) z = -z;
        xpf = y - (y * (z / (z + 1)));
        ypf = x - (x * (z / (z + 1)));
        x011[24 * i] = xpf;
        y011[24 * i] = ypf;
		xpfa = -xpf;
		ypfa = ypf;
        x011[24 * i + 1] = xpfa;
        y011[24 * i + 1] = ypfa;
		xpfa = xpf;
		ypfa = -ypf;
        x011[24 * i + 2] = xpfa;
        y011[24 * i + 2] = ypfa;
		xpfa = -xpf;
		ypfa = -ypf;
        x011[24 * i + 3] = xpfa;
        y011[24 * i + 3] = ypfa;
        x = 0.707107 * (g[0][1] + g[0][2]);
        y = 0.707107 * (g[1][1] + g[1][2]);
        z = 0.707107 * (g[2][1] + g[2][2]);
        if (z < 0) z = -z;
        xpf = y - (y * (z / (z + 1)));
        ypf = x - (x * (z / (z + 1)));
        x011[24 * i + 4] = xpf;
        y011[24 * i + 4] = ypf;
		xpfa = -xpf;
		ypfa = ypf;
        x011[24 * i + 5] = xpfa;
        y011[24 * i + 5] = ypfa;
		xpfa = xpf;
		ypfa = -ypf;
        x011[24 * i + 6] = xpfa;
        y011[24 * i + 6] = ypfa;
		xpfa = -xpf;
		ypfa = -ypf;
        x011[24 * i + 7] = xpfa;
        y011[24 * i + 7] = ypfa;
        x = 0.707107 * (g[0][2] + g[0][0]);
        y = 0.707107 * (g[1][2] + g[1][0]);
        z = 0.707107 * (g[2][2] + g[2][0]);
        if (z < 0) z = -z;
        xpf = y - (y * (z / (z + 1)));
        ypf = x - (x * (z / (z + 1)));
        x011[24 * i + 8] = xpf;
        y011[24 * i + 8] = ypf;
		xpfa = -xpf;
		ypfa = ypf;
        x011[24 * i + 9] = xpfa;
        y011[24 * i + 9] = ypfa;
		xpfa = xpf;
		ypfa = -ypf;
        x011[24 * i + 10] = xpfa;
        y011[24 * i + 10] = ypfa;
		xpfa = -xpf;
		ypfa = -ypf;
        x011[24 * i + 11] = xpfa;
        y011[24 * i + 11] = ypfa;
        x = 0.707107 * (g[0][0] - g[0][1]);
        y = 0.707107 * (g[1][0] - g[1][1]);
        z = 0.707107 * (g[2][0] - g[2][1]);
        if (z < 0) z = -z;
        xpf = y - (y * (z / (z + 1)));
        ypf = x - (x * (z / (z + 1)));
        x011[24 * i + 12] = xpf;
        y011[24 * i + 12] = ypf;
		xpfa = -xpf;
		ypfa = ypf;
        x011[24 * i + 13] = xpfa;
        y011[24 * i + 13] = ypfa;
		xpfa = xpf;
		ypfa = -ypf;
        x011[24 * i + 14] = xpfa;
        y011[24 * i + 14] = ypfa;
		xpfa = -xpf;
		ypfa = -ypf;
        x011[24 * i + 15] = xpfa;
        y011[24 * i + 15] = ypfa;
        x = 0.707107 * (g[0][1] - g[0][2]);
        y = 0.707107 * (g[1][1] - g[1][2]);
        z = 0.707107 * (g[2][1] - g[2][2]);
        if (z < 0) z = -z;
        xpf = y - (y * (z / (z + 1)));
        ypf = x - (x * (z / (z + 1)));
        x011[24 * i + 16] = xpf;
        y011[24 * i + 16] = ypf;
		xpfa = -xpf;
		ypfa = ypf;
        x011[24 * i + 17] = xpfa;
        y011[24 * i + 17] = ypfa;
		xpfa = xpf;
		ypfa = -ypf;
        x011[24 * i + 18] = xpfa;
        y011[24 * i + 18] = ypfa;
		xpfa = -xpf;
		ypfa = -ypf;
        x011[24 * i + 19] = xpfa;
        y011[24 * i + 19] = ypfa;
        x = 0.707107 * (g[0][2] - g[0][0]);
        y = 0.707107 * (g[1][2] - g[1][0]);
        z = 0.707107 * (g[2][2] - g[2][0]);
        if (z < 0) z = -z;
        xpf = y - (y * (z / (z + 1)));
        ypf = x - (x * (z / (z + 1)));
        x011[24 * i + 20] = xpf;
        y011[24 * i + 20] = ypf;
		xpfa = -xpf;
		ypfa = ypf;
        x011[24 * i + 21] = xpfa;
        y011[24 * i + 21] = ypfa;
		xpfa = xpf;
		ypfa = -ypf;
        x011[24 * i + 22] = xpfa;
        y011[24 * i + 22] = ypfa;
		xpfa = -xpf;
		ypfa = -ypf;
        x011[24 * i + 23] = xpfa;
        y011[24 * i + 23] = ypfa;
        x = 0.57735 * (g[0][0] + g[0][1] + g[0][2]);
        y = 0.57735 * (g[1][0] + g[1][1] + g[1][2]);
        z = 0.57735 * (g[2][0] + g[2][1] + g[2][2]);
        if (z < 0) z = -z;
        xpf = y - (y * (z / (z + 1)));
        ypf = x - (x * (z / (z + 1)));
        x111[16 * i] = xpf;
        y111[16 * i] = ypf;
		xpfa = -xpf;
		ypfa = ypf;
        x111[16 * i + 1] = xpfa;
        y111[16 * i + 1] = ypfa;
		xpfa = xpf;
		ypfa = -ypf;
        x111[16 * i + 2] = xpfa;
        y111[16 * i + 2] = ypfa;
		xpfa = -xpf;
		ypfa = -ypf;
        x111[16 * i + 3] = xpfa;
        y111[16 * i + 3] = ypfa;
        x = 0.57735 * (g[0][0] + g[0][1] - g[0][2]);
        y = 0.57735 * (g[1][0] + g[1][1] - g[1][2]);
        z = 0.57735 * (g[2][0] + g[2][1] - g[2][2]);
        if (z < 0) z = -z;
        xpf = y - (y * (z / (z + 1)));
        ypf = x - (x * (z / (z + 1)));
        x111[16 * i + 4] = xpf;
        y111[16 * i + 4] = ypf;
		xpfa = -xpf;
		ypfa = ypf;
        x111[16 * i + 5] = xpfa;
        y111[16 * i + 5] = ypfa;
		xpfa = xpf;
		ypfa = -ypf;
        x111[16 * i + 6] = xpfa;
        y111[16 * i + 6] = ypfa;
		xpfa = -xpf;
		ypfa = -ypf;
        x111[16 * i + 7] = xpfa;
        y111[16 * i + 7] = ypfa;
        x = 0.57735 * (g[0][0] - g[0][1] + g[0][2]);
        y = 0.57735 * (g[1][0] - g[1][1] + g[1][2]);
        z = 0.57735 * (g[2][0] - g[2][1] + g[2][2]);
        if (z < 0) z = -z;
        xpf = y - (y * (z / (z + 1)));
        ypf = x - (x * (z / (z + 1)));
        x111[16 * i + 8] = xpf;
        y111[16 * i + 8] = ypf;
		xpfa = -xpf;
		ypfa = ypf;
        x111[16 * i + 9] = xpfa;
        y111[16 * i + 9] = ypfa;
		xpfa = xpf;
		ypfa = -ypf;
        x111[16 * i + 10] = xpfa;
        y111[16 * i + 10] = ypfa;
		xpfa = -xpf;
		ypfa = -ypf;
        x111[16 * i + 11] = xpfa;
        y111[16 * i + 11] = ypfa;
        x = 0.57735 * (-g[0][0] + g[0][1] + g[0][2]);
        y = 0.57735 * (-g[1][0] + g[1][1] + g[1][2]);
        z = 0.57735 * (-g[2][0] + g[2][1] + g[2][2]);
        if (z < 0) z = -z;
        xpf = y - (y * (z / (z + 1)));
        ypf = x - (x * (z / (z + 1)));
        x111[16 * i + 12] = xpf;
        y111[16 * i + 12] = ypf;
		xpfa = -xpf;
		ypfa = ypf;
        x111[16 * i + 13] = xpfa;
        y111[16 * i + 13] = ypfa;
		xpfa = xpf;
		ypfa = -ypf;
        x111[16 * i + 14] = xpfa;
        y111[16 * i + 14] = ypfa;
		xpfa = -xpf;
		ypfa = -ypf;
        x111[16 * i + 15] = xpfa;
        y111[16 * i + 15] = ypfa;
      }
      return err;
    }

    template<typename T>
    int GenLogNormal(double avg, double stdDev, T &x, T &y, int size)
    {
      int err = 0;
      double lognormin, lognormout, max, min;
      double s2 = pow(stdDev, 2);
      double root2pi = pow((2.0 * 3.1415926535897), 0.5);
      x.resize(size);
      y.resize(size);
      min = exp(avg - (5 * stdDev));
      max = exp(avg + (5 * stdDev));
      for (int i = 0; i < size; i++)
      {
        lognormin = (i * ((max - min) / double(size))) + (((max - min) / double(size)) / 2.0) + min;
        lognormout = (1.0 / (lognormin * stdDev * root2pi)) * exp(-((log(lognormin) - avg) * (log(lognormin) - avg)) / (2 * s2));
        x[i] = lognormin;
        y[i] = lognormout * ((max - min) / double(size));
        if (lognormout < 0)
          err = 1;
      }
      return err;
    }

    template<typename T>
    int GenPowerLaw(double alpha, double k, double beta, T &x, T &y, int size)
    {
      int err = 0;
      double in, out, max, min;
      x.resize(size);
      y.resize(size);
      min = 0;
      max = 5;
      for (int i = 0; i < size; i++)
      {
        in = (i * ((max - min) / double(size))) + (((max - min) / double(size)) / 2.0) + min;
        out = alpha * pow(in, k) + beta;
        x[i] = in;
        y[i] = out;
        if (out < 0)
          err = 1;
      }
      return err;
    }

    /**
     * @brief Calculates the number of bins using 64 bit floating point values
     * @param mu
     * @param sigma
     * @param cutoff
     * @param binstep
     * @param max (out)
     * @param min (out)
     * @return
     */
    int computeNumberOfBins(double mu, double sigma, double cutoff, double binstep, double &max, double &min);

    /**
     * @brief Calculates the number of bins using 32 bit floating point values
     * @param mu
     * @param sigma
     * @param cutoff
     * @param binstep
     * @param max (out)
     * @param min (out)
     * @return
     */
    int computeNumberOfBins(float mu, float sigma, float cutoff, float binstep, float &max, float &min);

    /**
     * @brief Generates the CutOff values
     * @param mu
     * @param sigma
     * @param cutoff
     * @param binstep
     * @param x
     * @param y
     * @param yMax
     * @param numsizebins
     * @param binsizes
     * @return
     */
    template<typename J, typename T>
    int GenCutOff(J mu, J sigma, J cutoff, J binstep, T &x, T &y, J yMax, int &numsizebins, T &binsizes)
    {
      J max, min;
      numsizebins = computeNumberOfBins(mu, sigma, cutoff, binstep, max, min);
      int err = 0;
      x.resize(2);
      y.resize(2);

      x[0] = min;
      x[1] = max;

      y[0] = 0.0;
      y[1] = yMax;

      binsizes.resize(numsizebins);
      for (int i = 0; i < numsizebins; i++)
      {
        binsizes[i] = min + (i * binstep);
      }
      return err;
    }

    /**
     * @brief
     * @param value
     * @return
     */
    double gamma(double value);

  private:
    StatsGen(const StatsGen&); // Copy Constructor Not Implemented
    void operator=(const StatsGen&); // Operator '=' Not Implemented
};

#endif /* _STATSGEN_H_ */
