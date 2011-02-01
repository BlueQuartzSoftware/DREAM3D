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

#include "AIM/Common/Constants.h"
#include "AIM/Common/AIMRandomNG.h"
#include "AIM/Common/MisorientationCalculations.h"
#include "AIM/Common/Texture.h"

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
    int GenCubicODF(T weights, T sigmas,
                    T &x001, T &y001, T &x011, T &y011, T &x111, T &y111,
                    int size, double randomWeight)
    {
      int TextureBins[AIM_TEXTURE_COUNT];
      static const size_t eighteenCubed = 5832;
      double odf[eighteenCubed];
      int err = 0;
	  double q1[5], qref[5];
      size_t bin, ea1bin, ea2bin, ea3bin;
      double ea1, ea2, ea3;
      double r1, r2, r3;
      double r1test, r2test, r3test;
      double h1, h2, h3;
      double sum, diff;
      double hmag, rmag, angle;
      double g[3][3];
      double x, y, z;
      double xpf, ypf;
      double degtorad = M_PI / 180.0;
      double totaldensity;
      double dim1 = 0;
      double dim2 = 0;
      double dim3 = 0;
      const double m_pi = 3.1415926535897;
      dim1 = 2*pow((0.75 * ((m_pi / 4.0) - sin((m_pi / 4.0)))), (1.0 / 3.0));
      dim2 = 2*pow((0.75 * ((m_pi / 4.0) - sin((m_pi / 4.0)))), (1.0 / 3.0));
      dim3 = 2*pow((0.75 * ((m_pi / 4.0) - sin((m_pi / 4.0)))), (1.0 / 3.0));

      AIMRandomNG rg;
      rg.RandomInit(2902239);
      x001.resize(size * 3);
      y001.resize(size * 3);
      x011.resize(size * 6);
      y011.resize(size * 6);
      x111.resize(size * 4);
      y111.resize(size * 4);
      for (int i = 0; i < 15; i++)
      {
        r1 = tan(Texture::Values[i][1]/2)*sin((Texture::Values[i][0]-Texture::Values[i][2])/2)/cos((Texture::Values[i][0]+Texture::Values[i][2])/ 2);
        r2 = tan(Texture::Values[i][1]/2)*cos((Texture::Values[i][0]-Texture::Values[i][2])/2)/cos((Texture::Values[i][0]+Texture::Values[i][2])/ 2);
        r3 = tan((Texture::Values[i][0]+Texture::Values[i][2])/2);
		MisorientationCalculations::getFZQuatCubic(r1, r2, r3);
        rmag = pow((r1 * r1 + r2 * r2 + r3 * r3), 0.5);
        angle = 2.0 * atan(rmag);
        h1 = pow(((3.0 / 4.0) * (angle - sin(angle))), (1.0 / 3.0)) * (r1 / rmag);
        h2 = pow(((3.0 / 4.0) * (angle - sin(angle))), (1.0 / 3.0)) * (r2 / rmag);
        h3 = pow(((3.0 / 4.0) * (angle - sin(angle))), (1.0 / 3.0)) * (r3 / rmag);
        if (angle == 0) h1 = 0.0, h2 = 0.0, h3 = 0.0;
        ea1bin = int((h1 + (dim1 / 2.0)) * 18.0 / dim1);
        ea2bin = int((h2 + (dim1 / 2.0)) * 18.0 / dim2);
        ea3bin = int((h3 + (dim1 / 2.0)) * 18.0 / dim3);
        if (ea1bin >= 18) ea1bin = 17;
        if (ea2bin >= 18) ea2bin = 17;
        if (ea3bin >= 18) ea3bin = 17;
        bin = (ea3bin * 18 * 18) + (ea2bin * 18) + (ea1bin);
        TextureBins[i] = bin;
      }
      double totalweight = 0;
      for (int i = 0; i < eighteenCubed; i++)
      {
        odf[i] = randomWeight / (eighteenCubed);
        totalweight = totalweight + randomWeight / (eighteenCubed);
      }
      for (int i = 0; i < Texture::Count; i++)
      {
        bin = TextureBins[i];
        odf[bin] = odf[bin] + (weights[i]);
        totalweight = totalweight + weights[i];
      }
      for (int i = 0; i < eighteenCubed; i++)
      {
        odf[i] = odf[i] / totalweight;
      }
      for (int i = 0; i < size; i++)
      {
        double random = rg.Random();
        int choose = 0;
        totaldensity = 0;
        for (int j = 0; j < eighteenCubed; j++)
        {
          double density = odf[j];
          totaldensity = totaldensity + density;
          if (random < totaldensity && random >= (totaldensity - density)) choose = j;
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
        r1 = tan(angle/2.0) * (h1 / hmag);
        r2 = tan(angle/2.0) * (h2 / hmag);
        r3 = tan(angle/2.0) * (h3 / hmag);
        sum = atan(r3);
        diff = atan2(r2,r1);
        ea1 = sum + diff;
        ea2 = 2. * atan2((r1 * cos(sum)),cos(diff));
        ea3 = sum - diff;
        r1test = tan(ea2/2)*sin((ea1-ea3)/2)/cos((ea1+ea3)/ 2);
        r2test = tan(ea2/2)*cos((ea1-ea3)/2)/cos((ea1+ea3)/ 2);
        r3test = tan((ea1+ea3)/2);
        g[0][0] = cos(ea1) * cos(ea3) - sin(ea1) * sin(ea3) * cos(ea2);
        g[1][0] = sin(ea1) * cos(ea3) + cos(ea1) * sin(ea3) * cos(ea2);
        g[2][0] = sin(ea3) * sin(ea2);
        g[0][1] = -cos(ea1) * sin(ea3) - sin(ea1) * cos(ea3) * cos(ea2);
        g[1][1] = -sin(ea1) * sin(ea3) + cos(ea1) * cos(ea3) * cos(ea2);
        g[2][1] = cos(ea3) * sin(ea2);
        g[0][2] = sin(ea1) * sin(ea2);
        g[1][2] = -cos(ea1) * sin(ea2);
        g[2][2] = cos(ea2);
        x = g[0][0];
        y = g[1][0];
        z = g[2][0];
        if (z < 0) z = -z;
        ypf = x - (x * (z / (z + 1)));
        xpf = y - (y * (z / (z + 1)));
        random = rg.Random();
        x001[3 * i] = xpf;
        y001[3 * i] = ypf;
        x = g[0][1];
        y = g[1][1];
        z = g[2][1];
        if (z < 0) z = -z;
        ypf = x - (x * (z / (z + 1)));
        xpf = y - (y * (z / (z + 1)));
        x001[3 * i + 1] = xpf;
        y001[3 * i + 1] = ypf;
        x = g[0][2];
        y = g[1][2];
        z = g[2][2];
        if (z < 0) z = -z;
        ypf = x - (x * (z / (z + 1)));
        xpf = y - (y * (z / (z + 1)));
        x001[3 * i + 2] = xpf;
        y001[3 * i + 2] = ypf;
        x = 0.707107 * (g[0][0] + g[0][1]);
        y = 0.707107 * (g[1][0] + g[1][1]);
        z = 0.707107 * (g[2][0] + g[2][1]);
        if (z < 0) z = -z;
        ypf = x - (x * (z / (z + 1)));
        xpf = y - (y * (z / (z + 1)));
        x011[6 * i] = xpf;
        y011[6 * i] = ypf;
        x = 0.707107 * (g[0][1] + g[0][2]);
        y = 0.707107 * (g[1][1] + g[1][2]);
        z = 0.707107 * (g[2][1] + g[2][2]);
        if (z < 0) z = -z;
        ypf = x - (x * (z / (z + 1)));
        xpf = y - (y * (z / (z + 1)));
        x011[6 * i + 1] = xpf;
        y011[6 * i + 1] = ypf;
        x = 0.707107 * (g[0][2] + g[0][0]);
        y = 0.707107 * (g[1][2] + g[1][0]);
        z = 0.707107 * (g[2][2] + g[2][0]);
        if (z < 0) z = -z;
        ypf = x - (x * (z / (z + 1)));
        xpf = y - (y * (z / (z + 1)));
        x011[6 * i + 2] = xpf;
        y011[6 * i + 2] = ypf;
        x = 0.707107 * (g[0][0] - g[0][1]);
        y = 0.707107 * (g[1][0] - g[1][1]);
        z = 0.707107 * (g[2][0] - g[2][1]);
        if (z < 0) z = -z;
        ypf = x - (x * (z / (z + 1)));
        xpf = y - (y * (z / (z + 1)));
        x011[6 * i + 3] = xpf;
        y011[6 * i + 3] = ypf;
        x = 0.707107 * (g[0][1] - g[0][2]);
        y = 0.707107 * (g[1][1] - g[1][2]);
        z = 0.707107 * (g[2][1] - g[2][2]);
        if (z < 0) z = -z;
        ypf = x - (x * (z / (z + 1)));
        xpf = y - (y * (z / (z + 1)));
        x011[6 * i + 4] = xpf;
        y011[6 * i + 4] = ypf;
        x = 0.707107 * (g[0][2] - g[0][0]);
        y = 0.707107 * (g[1][2] - g[1][0]);
        z = 0.707107 * (g[2][2] - g[2][0]);
        if (z < 0) z = -z;
        ypf = x - (x * (z / (z + 1)));
        xpf = y - (y * (z / (z + 1)));
        x011[6 * i + 5] = xpf;
        y011[6 * i + 5] = ypf;
        x = 0.57735 * (g[0][0] + g[0][1] + g[0][2]);
        y = 0.57735 * (g[1][0] + g[1][1] + g[1][2]);
        z = 0.57735 * (g[2][0] + g[2][1] + g[2][2]);
        if (z < 0) z = -z;
        ypf = x - (x * (z / (z + 1)));
        xpf = y - (y * (z / (z + 1)));
        x111[4 * i] = xpf;
        y111[4 * i] = ypf;
        x = 0.57735 * (g[0][0] + g[0][1] - g[0][2]);
        y = 0.57735 * (g[1][0] + g[1][1] - g[1][2]);
        z = 0.57735 * (g[2][0] + g[2][1] - g[2][2]);
        if (z < 0) z = -z;
        ypf = x - (x * (z / (z + 1)));
        xpf = y - (y * (z / (z + 1)));
        x111[4 * i + 1] = xpf;
        y111[4 * i + 1] = ypf;
        x = 0.57735 * (g[0][0] - g[0][1] + g[0][2]);
        y = 0.57735 * (g[1][0] - g[1][1] + g[1][2]);
        z = 0.57735 * (g[2][0] - g[2][1] + g[2][2]);
        if (z < 0) z = -z;
        ypf = x - (x * (z / (z + 1)));
        xpf = y - (y * (z / (z + 1)));
        x111[4 * i + 2] = xpf;
        y111[4 * i + 2] = ypf;
        x = 0.57735 * (-g[0][0] + g[0][1] + g[0][2]);
        y = 0.57735 * (-g[1][0] + g[1][1] + g[1][2]);
        z = 0.57735 * (-g[2][0] + g[2][1] + g[2][2]);
        if (z < 0) z = -z;
        ypf = x - (x * (z / (z + 1)));
        xpf = y - (y * (z / (z + 1)));
        x111[4 * i + 3] = xpf;
        y111[4 * i + 3] = ypf;
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
     * @return
     */
    int computeNumberOfBins(double mu, double sigma, double cutoff, double binstep, double &max, double &min);

    /**
     * @brief Calculates the number of bins using 32 bit floating point values
     * @param mu
     * @param sigma
     * @param cutoff
     * @param binstep
     * @return
     */
    int computeNumberOfBins(float mu, float sigma, float cutoff, float binstep, float &max, float &min);

    /**
     * @brief Generates the CutOff values
     * @param mu
     * @param sigma
     * @param cutoff
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
