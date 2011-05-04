/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <vector>
#include "MXA/MXATypes.h"
#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/AIMMath.h"
#include "AIM/Common/MisorientationCalculations.h"

/**
 * @class Texture Texture.h AIM/Common/Texture.h
 * @brief This class holds default data for Orientation Distribution Function
 * calculations that the AIMRepresentation package will perform.
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Micharl A. Groeber for US Air Force Research Laboratory
 * @date Feb 1, 2011
 * @version 1.0
 */

class AIMCOMMON_EXPORT Texture
{
  public:

  virtual ~Texture();

#if 0
  const static size_t Count;
  const static double Values[AIM_TEXTURE_COUNT][3];
  const static char* Names[AIM_TEXTURE_COUNT];
  const static double Weights[AIM_TEXTURE_COUNT];
  const static double Sigmas[AIM_TEXTURE_COUNT];

#endif
  /**
   * @brief This will calculate ODF data based on an array of weights that are
   * passed in and a Cubic Crystal Structure. This is templated on the container
   * type that holds the data. Containers that adhere to the STL Vector API
   * should be usable. QVector falls into this category. The input data for the
   * euler angles is in Columnar fashion instead of row major format.
   * @param e1s The first euler angles
   * @param e2s The second euler angles
   * @param e3s The third euler angles
   * @param weights Array of weights values.
   * @param sigmas Array of sigma values.
   * @param normalize Should the ODF data be normalized by the totalWeight value
   * before returning.
   * @param odf (OUT) The ODF data that is generated from this function.
   * @param totalweight (OUT) The TotalWeight value that is also calculated
   */
  template<typename T>
  static void calculateCubicODFData(T e1s, T e2s, T e3s, T weights, T sigmas,
      bool normalize, T &odf, double &totalweight)
  {
    double randomWeight = 1.0;
    int *TextureBins;
    TextureBins = new int[weights.size()];
    static const size_t odfsize = 5832;
    double degtorad = M_PI/180.0;
    double addweight = 0;
    odf.resize(odfsize);
    size_t ea1bin, ea2bin, ea3bin;
    size_t bin, addbin;
    size_t bin1, bin2, bin3;
    size_t addbin1, addbin2, addbin3;
    double dist, fraction;
    double rmag, angle;
    double r1, r2, r3;
    double h1, h2, h3;
    double dim1 = 2*pow((0.75 * ((M_PI / 4.0) - sin((M_PI / 4.0)))), (1.0 / 3.0));
    double dim2 = 2*pow((0.75 * ((M_PI / 4.0) - sin((M_PI / 4.0)))), (1.0 / 3.0));
    double dim3 = 2*pow((0.75 * ((M_PI / 4.0) - sin((M_PI / 4.0)))), (1.0 / 3.0));
    double tan_term = 0.0;
    double sin_term = 0.0;
    double cos_term1 = 0.0;
    double cos_term2 = 0.0;
    double hTmp = 0.0;

    for (typename T::size_type i = 0; i < e1s.size(); i++)
    {
      tan_term = tan(e2s[i]*degtorad/2);
      sin_term = sin((e1s[i]*degtorad-e3s[i]*degtorad)/2);
      cos_term1 = cos((e1s[i]*degtorad+e3s[i]*degtorad)/2);
      cos_term2 = cos((e1s[i]*degtorad-e3s[i]*degtorad)/2);

      r1 = tan_term * cos_term2 / cos_term1;
      r2 = tan_term * sin_term / cos_term1;
      r3 = tan((e1s[i]*degtorad+e3s[i]*degtorad)/2);
      MisorientationCalculations::getFZRodCubic(r1, r2, r3);
      rmag = pow((r1 * r1 + r2 * r2 + r3 * r3), 0.5);
      angle = 2.0 * atan(rmag);
      hTmp = pow(((3.0 / 4.0) * (angle - sin(angle))), (1.0 / 3.0));
      h1 = hTmp * (r1 / rmag);
      h2 = hTmp * (r2 / rmag);
      h3 = hTmp * (r3 / rmag);
      if (angle == 0) h1 = 0.0, h2 = 0.0, h3 = 0.0;
      ea1bin = int((h1 + (dim1 / 2.0)) * 18.0 / dim1);
      ea2bin = int((h2 + (dim1 / 2.0)) * 18.0 / dim2);
      ea3bin = int((h3 + (dim1 / 2.0)) * 18.0 / dim3);
      if (ea1bin >= 18) ea1bin = 17;
      if (ea2bin >= 18) ea2bin = 17;
      if (ea3bin >= 18) ea3bin = 17;
      bin = (ea3bin * 18 * 18) + (ea2bin * 18) + (ea1bin);
      TextureBins[i] = static_cast<int>(bin);
    }

    for (size_t i = 0; i < odfsize; i++)
    {
      odf[i] = 0.0;
    }
    for (typename T::size_type i = 0; i < weights.size(); i++)
    {
      bin = TextureBins[i];
      bin1 = bin % 18;
      bin2 = (bin / 18) % 18;
      bin3 = bin / (18 * 18);
      for (double j = -sigmas[i]; j <= sigmas[i]; j++)
      {
        double jsqrd = j*j;
        for (double k = -sigmas[i]; k <= sigmas[i]; k++)
        {
          double ksqrd = k*k;
          for (double l = -sigmas[i]; l <= sigmas[i]; l++)
          {
            double lsqrd = l*l;
            addbin1 = bin1 + static_cast<size_t>(j);
            addbin2 = bin2 + static_cast<size_t>(k);
            addbin3 = bin3 + static_cast<size_t>(l);
            if (addbin1 < 0) addbin1 = addbin1 + 18;
            if (addbin1 >= 18) addbin1 = addbin1 - 18;
            if (addbin2 < 0) addbin2 = addbin2 + 18;
            if (addbin2 >= 18) addbin2 = addbin2 - 18;
            if (addbin3 < 0) addbin3 = addbin3 + 18;
            if (addbin3 >= 18) addbin3 = addbin3 - 18;
            addbin = (addbin3 * 18 * 18) + (addbin2 * 18) + (addbin1);
            dist = pow((jsqrd +ksqrd + lsqrd), 0.5);
            fraction = 1.0 - ((dist / sigmas[i]) * (dist / sigmas[i]));
            if (fraction > 0.0)
            {
              addweight = (weights[i] * fraction);
              odf[addbin] = odf[addbin]+addweight;
            }
          }
        }
      }
    }
    double remainingweight = double(odfsize);
    double remainingcount = 0;
    for (size_t i = 0; i < odfsize; i++)
    {
      remainingweight = remainingweight-odf[i];
      if(odf[i] == 0.0) remainingcount++;
    }
    for (size_t i = 0; i < odfsize; i++)
    {
      if(odf[i] == 0.0) odf[i] = remainingweight/remainingcount;
    }
    if (normalize == true)
    {
      // Normalize the odf
      for (size_t i = 0; i < odfsize; i++)
      {
        odf[i] = odf[i] / double(odfsize);
      }
    }

  }

  /**
   * @brief This will calculate ODF data based on an array of weights that are
   * passed in and a Hexagonal Crystal Structure. This is templated on the container
   * type that holds the data. Containers that adhere to the STL Vector API
   * should be usable. QVector falls into this category. The input data for the
   * euler angles is in Columnar fashion instead of row major format.
   * @param e1s The first euler angles
   * @param e2s The second euler angles
   * @param e3s The third euler angles
   * @param weights Array of weights values.
   * @param sigmas Array of sigma values.
   * @param normalize Should the ODF data be normalized by the totalWeight value
   * before returning.
   * @param odf (OUT) The ODF data that is generated from this function.
   * @param totalweight (OUT) The TotalWeight value that is also calculated
   */
  template<typename T>
  static void calculateHexODFData(T e1s, T e2s, T e3s, T weights, T sigmas,
      bool normalize, T &odf, double &totalweight)
  {
    double randomWeight = 1.0;
    int *TextureBins;
    TextureBins = new int[weights.size()];
    static const size_t odfsize = 15552;
    double degtorad = M_PI/180.0;
    double addweight = 0;
    odf.resize(odfsize);
    size_t ea1bin, ea2bin, ea3bin;
    size_t bin, addbin;
    size_t bin1, bin2, bin3;
    size_t addbin1, addbin2, addbin3;
    double dist, fraction;
    double rmag, angle;
    double r1, r2, r3;
    double h1, h2, h3;
    double dim1 = 2*pow((0.75 * ((M_PI / 2.0) - sin((M_PI / 2.0)))), (1.0 / 3.0));
    double dim2 = 2*pow((0.75 * ((M_PI / 2.0) - sin((M_PI / 2.0)))), (1.0 / 3.0));
    double dim3 = 2*pow((0.75 * ((M_PI / 6.0) - sin((M_PI / 6.0)))), (1.0 / 3.0));
    double tan_term = 0.0;
    double sin_term = 0.0;
    double cos_term1 = 0.0;
    double cos_term2 = 0.0;
    double hTmp = 0.0;

    for (typename T::size_type i = 0; i < e1s.size(); i++)
    {
      tan_term = tan(e2s[i]*degtorad/2);
      sin_term = sin((e1s[i]*degtorad-e3s[i]*degtorad)/2);
      cos_term1 = cos((e1s[i]*degtorad+e3s[i]*degtorad)/2);
      cos_term2 = cos((e1s[i]*degtorad-e3s[i]*degtorad)/2);

      r1 = tan_term * cos_term2 / cos_term1;
      r2 = tan_term * sin_term / cos_term1;
      r3 = tan((e1s[i]*degtorad+e3s[i]*degtorad)/2);
      MisorientationCalculations::getFZRodHexagonal(r1, r2, r3);
      rmag = pow((r1 * r1 + r2 * r2 + r3 * r3), 0.5);
      angle = 2.0 * atan(rmag);
      hTmp = pow(((3.0 / 4.0) * (angle - sin(angle))), (1.0 / 3.0));
      h1 = hTmp * (r1 / rmag);
      h2 = hTmp * (r2 / rmag);
      h3 = hTmp * (r3 / rmag);
      if (angle == 0) h1 = 0.0, h2 = 0.0, h3 = 0.0;
      ea1bin = int((h1 + (dim1 / 2.0)) * 36.0 / dim1);
      ea2bin = int((h2 + (dim1 / 2.0)) * 36.0 / dim2);
      ea3bin = int((h3 + (dim1 / 2.0)) * 12.0 / dim3);
      if (ea1bin >= 36) ea1bin = 35;
      if (ea2bin >= 36) ea2bin = 35;
      if (ea3bin >= 12) ea3bin = 11;
      bin = (ea3bin * 36 * 36) + (ea2bin * 36) + (ea1bin);
      TextureBins[i] = static_cast<int>(bin);
    }

    for (size_t i = 0; i < odfsize; i++)
    {
      odf[i] = 0.0;
    }
    for (typename T::size_type i = 0; i < e1s.size(); i++)
    {
      bin = TextureBins[i];
      bin1 = bin % 36;
      bin2 = (bin / 36) % 36;
      bin3 = bin / (36 * 36);
      for (int j = -sigmas[i]; j <= sigmas[i]; j++)
      {
        for (int k = -sigmas[i]; k <= sigmas[i]; k++)
        {
          for (int l = -sigmas[i]; l <= sigmas[i]; l++)
          {
            addbin1 = bin1 + j;
            addbin2 = bin2 + k;
            addbin3 = bin3 + l;
            if (addbin1 < 0) addbin1 = addbin1 + 36;
            if (addbin1 >= 36) addbin1 = addbin1 - 36;
            if (addbin2 < 0) addbin2 = addbin2 + 36;
            if (addbin2 >= 36) addbin2 = addbin2 - 36;
            if (addbin3 < 0) addbin3 = addbin3 + 12;
            if (addbin3 >= 12) addbin3 = addbin3 - 12;
            addbin = (addbin3 * 36 * 36) + (addbin2 * 36) + (addbin1);
            dist = pow((j * j + k * k + l * l), 0.5);
            fraction = 1.0 - ((dist / sigmas[i]) * (dist / sigmas[i]));
            if (fraction > 0.0)
            {
              addweight = (weights[i] * fraction);
              odf[addbin] = odf[addbin]+addweight;
            }
          }
        }
      }
    }
    double remainingweight = double(odfsize);
    double remainingcount = 0;
    for (size_t i = 0; i < odfsize; i++)
    {
      remainingweight = remainingweight-odf[i];
      if(odf[i] == 0.0) remainingcount++;
    }
    for (size_t i = 0; i < odfsize; i++)
    {
      if(odf[i] == 0.0) odf[i] = remainingweight/remainingcount;
    }
    if (normalize == true)
    {
      // Normalize the odf
      for (size_t i = 0; i < odfsize; i++)
      {
        odf[i] = odf[i] / double(odfsize);
      }
    }

  }

  /**
   * @brief This will calculate ODF data based on an array of weights that are
   * passed in and a OrthoRhombic Crystal Structure. This is templated on the container
   * type that holds the data. Containers that adhere to the STL Vector API
   * should be usable. QVector falls into this category. The input data for the
   * euler angles is in Columnar fashion instead of row major format.
   * @param e1s The first euler angles
   * @param e2s The second euler angles
   * @param e3s The third euler angles
   * @param weights Array of weights values.
   * @param sigmas Array of sigma values.
   * @param normalize Should the ODF data be normalized by the totalWeight value
   * before returning.
   * @param odf (OUT) The ODF data that is generated from this function.
   * @param totalweight (OUT) The TotalWeight value that is also calculated
   */
  template<typename T>
  static void calculateOrthoRhombicODFData(T e1s, T e2s, T e3s, T weights, T sigmas,
      bool normalize, T &odf, double &totalweight)
  {
    double randomWeight = 1.0;
    int *TextureBins;
    TextureBins = new int[weights.size()];
    static const size_t odfsize = 46656;
    double degtorad = M_PI/180.0;
    double addweight = 0;
    odf.resize(odfsize);
    size_t ea1bin, ea2bin, ea3bin;
    size_t bin, addbin;
    size_t bin1, bin2, bin3;
    size_t addbin1, addbin2, addbin3;
    double dist, fraction;
    double rmag, angle;
    double r1, r2, r3;
    double h1, h2, h3;
    double dim1 = 2*pow((0.75 * ((M_PI / 2.0) - sin((M_PI / 2.0)))), (1.0 / 3.0));
    double dim2 = 2*pow((0.75 * ((M_PI / 2.0) - sin((M_PI / 2.0)))), (1.0 / 3.0));
    double dim3 = 2*pow((0.75 * ((M_PI / 2.0) - sin((M_PI / 2.0)))), (1.0 / 3.0));
    double tan_term = 0.0;
    double sin_term = 0.0;
    double cos_term1 = 0.0;
    double cos_term2 = 0.0;
    double hTmp = 0.0;

    for (typename T::size_type i = 0; i < e1s.size(); i++)
    {
      tan_term = tan(e2s[i]*degtorad/2);
      sin_term = sin((e1s[i]*degtorad-e3s[i]*degtorad)/2);
      cos_term1 = cos((e1s[i]*degtorad+e3s[i]*degtorad)/2);
      cos_term2 = cos((e1s[i]*degtorad-e3s[i]*degtorad)/2);

      r1 = tan_term * cos_term2 / cos_term1;
      r2 = tan_term * sin_term / cos_term1;
      r3 = tan((e1s[i]*degtorad+e3s[i]*degtorad)/2);
      MisorientationCalculations::getFZRodOrtho(r1, r2, r3);
      rmag = pow((r1 * r1 + r2 * r2 + r3 * r3), 0.5);
      angle = 2.0 * atan(rmag);
      hTmp = pow(((3.0 / 4.0) * (angle - sin(angle))), (1.0 / 3.0));
      h1 = hTmp * (r1 / rmag);
      h2 = hTmp * (r2 / rmag);
      h3 = hTmp * (r3 / rmag);
      if (angle == 0) h1 = 0.0, h2 = 0.0, h3 = 0.0;
      ea1bin = int((h1 + (dim1 / 2.0)) * 36.0 / dim1);
      ea2bin = int((h2 + (dim1 / 2.0)) * 36.0 / dim2);
      ea3bin = int((h3 + (dim1 / 2.0)) * 36.0 / dim3);
      if (ea1bin >= 36) ea1bin = 35;
      if (ea2bin >= 36) ea2bin = 35;
      if (ea3bin >= 36) ea3bin = 35;
      bin = (ea3bin * 36 * 36) + (ea2bin * 36) + (ea1bin);
      TextureBins[i] = static_cast<int>(bin);
    }

    for (size_t i = 0; i < odfsize; i++)
    {
      odf[i] = randomWeight / (odfsize);
      totalweight = totalweight + randomWeight / (odfsize);
    }
    for (typename T::size_type i = 0; i < e1s.size(); i++)
    {
      bin = TextureBins[i];
      addweight = (odf[bin] * (weights[i])) - odf[bin];
      odf[bin] = odf[bin] * (weights[i]);
      totalweight = totalweight + addweight;
      bin1 = bin % 36;
      bin2 = (bin / 36) % 36;
      bin3 = bin / (36 * 36);
      for (int j = -sigmas[i]; j <= sigmas[i]; j++)
      {
        for (int k = -sigmas[i]; k <= sigmas[i]; k++)
        {
          for (int l = -sigmas[i]; l <= sigmas[i]; l++)
          {
            addbin1 = bin1 + j;
            addbin2 = bin2 + k;
            addbin3 = bin3 + l;
            if (addbin1 < 0) addbin1 = addbin1 + 36;
            if (addbin1 >= 36) addbin1 = addbin1 - 36;
            if (addbin2 < 0) addbin2 = addbin2 + 36;
            if (addbin2 >= 36) addbin2 = addbin2 - 36;
            if (addbin3 < 0) addbin3 = addbin3 + 36;
            if (addbin3 >= 36) addbin3 = addbin3 - 36;
            addbin = (addbin3 * 36 * 36) + (addbin2 * 36) + (addbin1);
            dist = pow((j * j + k * k + l * l), 0.5);
            fraction = 1.0 - ((dist / sigmas[i]) * (dist / sigmas[i]));
            if (fraction > 0.0)
            {
              addweight = (odf[addbin] * (weights[i] * fraction)) - odf[addbin];
              odf[addbin] = odf[addbin] * (weights[i] * fraction);
              totalweight = totalweight + addweight;
            }
          }
        }
      }
    }

    if (normalize == true)
    {
      double test;
      // Normalize the odf
      for (size_t i = 0; i < odfsize; i++)
      {
        odf[i] = odf[i] / totalweight;
      }
    }

  }

  template<typename T>
  static void calculateCubicMDFData(T angles, T axes, T weights, T odf, T &mdf)
  {
    static const size_t odfsize = 5832;
    static const size_t mdfsize = 5832;
    double totalweight = 0;
    double radtodeg = 180.0/M_PI;

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
    unsigned int* seedPtr = reinterpret_cast<unsigned int*> (&seed);
#if CMP_WORDS_BIGENDIAN
    rg.RandomInit(seedPtr[1]);
#else
    rg.RandomInit(seedPtr[0]);
#endif
    int err = 0;
    int mbin;
    double w = 0;
    int choose1, choose2;
    double q1[5], q2[5];
    double totaldensity;
    double hmag;
    double angle;
    double denom;
    double r1, r2, r3;
    double h1, h2, h3;
    double n1, n2, n3;
    double random, random1, random2, tan_angle, density, cos_angle, sin_angle;

    double dim1 = 2 * pow((0.75 * ((M_PI / 4.0) - sin((M_PI / 4.0)))), (1.0 / 3.0));
    double dim2 = 2 * pow((0.75 * ((M_PI / 4.0) - sin((M_PI / 4.0)))), (1.0 / 3.0));
    double dim3 = 2 * pow((0.75 * ((M_PI / 4.0) - sin((M_PI / 4.0)))), (1.0 / 3.0));

    for (int i = 0; i < mdfsize; i++)
    {
      mdf[i] = 0.0;
    }
    int remainingcount = 10000;
    for (int i = 0; i < angles.size(); i++)
    {
      denom = pow((axes[3*i]*axes[3*i]+axes[3*i+1]*axes[3*i+1]+axes[3*i+2]*axes[3*i+2]),0.5);
      n1 = axes[3*i]/denom;
      n2 = axes[3*i+1]/denom;
      n3 = axes[3*i+2]/denom;
      r1 = n1 * pow(((3.0 / 4.0) * (angles[i] - sin(angles[i]))), (1.0 / 3.0));
      r2 = n2 * pow(((3.0 / 4.0) * (angles[i] - sin(angles[i]))), (1.0 / 3.0));
      r3 = n3 * pow(((3.0 / 4.0) * (angles[i] - sin(angles[i]))), (1.0 / 3.0));
      mbin = MisorientationCalculations::getMisoBinCubic(r1,r2,r3);
      mdf[mbin] = -int((weights[i]/double(mdfsize))*10000.0);
      remainingcount = remainingcount+mdf[mbin];
    }
    for (int i = 0; i < remainingcount; i++)
    {
      random1 = rg.Random();
      random2 = rg.Random();
      choose1 = 0;
      choose2 = 0;

      totaldensity = 0;
      for (size_t j = 0; j < odfsize; j++)
      {
        density = odf[j];
        totaldensity = totaldensity + density;
        if (random1 < totaldensity && random1 >= (totaldensity - density)) choose1 = static_cast<int> (j);
        if (random2 < totaldensity && random2 >= (totaldensity - density)) choose2 = static_cast<int> (j);
      }
      h1 = choose1 % 18;
      h2 = (choose1 / 18) % 18;
      h3 = choose1 / (18 * 18);
      random = rg.Random();
      h1 = ((dim1 / 18.0) * h1) + ((dim1 / 18.0) * random) - (dim1 / 2.0);
      random = rg.Random();
      h2 = ((dim2 / 18.0) * h2) + ((dim2 / 18.0) * random) - (dim2 / 2.0);
      random = rg.Random();
      h3 = ((dim3 / 18.0) * h3) + ((dim3 / 18.0) * random) - (dim3 / 2.0);
      hmag = pow((h1 * h1 + h2 * h2 + h3 * h3), 0.5);
      angle = pow((8 * hmag * hmag * hmag), (1.0 / 3.0));
      n1 = h1 / hmag;
      n2 = h2 / hmag;
      n3 = h3 / hmag;
      cos_angle = cos(angle/2.0);
      sin_angle = sin(angle/2.0);
      q1[0] = 1;
      q1[1] = sin_angle * n1;
      q1[2] = sin_angle * n2;
      q1[3] = sin_angle * n3;
      q1[4] = cos_angle;
      h1 = choose2 % 18;
      h2 = (choose2 / 18) % 18;
      h3 = choose2 / (18 * 18);
      random = rg.Random();
      h1 = ((dim1 / 18.0) * h1) + ((dim1 / 18.0) * random) - (dim1 / 2.0);
      random = rg.Random();
      h2 = ((dim2 / 18.0) * h2) + ((dim2 / 18.0) * random) - (dim2 / 2.0);
      random = rg.Random();
      h3 = ((dim3 / 18.0) * h3) + ((dim3 / 18.0) * random) - (dim3 / 2.0);
      hmag = pow((h1 * h1 + h2 * h2 + h3 * h3), 0.5);
      angle = pow((8 * hmag * hmag * hmag), (1.0 / 3.0));
      n1 = h1 / hmag;
      n2 = h2 / hmag;
      n3 = h3 / hmag;
      cos_angle = cos(angle/2.0);
      sin_angle = sin(angle/2.0);
      q2[0] = 1;
      q2[1] = sin_angle * n1;
      q2[2] = sin_angle * n2;
      q2[3] = sin_angle * n3;
      q2[4] = cos_angle;
      w = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
      w = w/radtodeg;
      r1 = n1 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
      r2 = n2 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
      r3 = n3 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
      mbin = MisorientationCalculations::getMisoBinCubic(r1,r2,r3);
      if(mdf[mbin] >= 0) mdf[mbin]++;
      if(mdf[mbin] < 0) i = i-1;
    }
	double test;
    for (int i = 0; i < mdfsize; i++)
    {
      if(mdf[i] < 0) mdf[i] = -mdf[i];
      mdf[i] = mdf[i]/10000.0;
    }

  }

  template<typename T>
  static void calculateHexMDFData(T angles, T axes, T weights, T odf, T &mdf)
  {
    static const size_t odfsize = 15552;
    static const size_t mdfsize = 15552;
    double totalweight = 0;
    double radtodeg = 180.0/M_PI;

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
    unsigned int* seedPtr = reinterpret_cast<unsigned int*> (&seed);
#if CMP_WORDS_BIGENDIAN
    rg.RandomInit(seedPtr[1]);
#else
    rg.RandomInit(seedPtr[0]);
#endif
    int err = 0;
    int mbin;
    int choose1, choose2;
    double w = 0;
    double q1[5], q2[5];
    double totaldensity;
    double hmag;
    double angle;
    double denom;
    double r1, r2, r3;
    double h1, h2, h3;
    double n1, n2, n3;
    double random, random1, random2, tan_angle, density, cos_angle, sin_angle;

    double dim1 = 2 * pow((0.75 * ((M_PI / 4.0) - sin((M_PI / 4.0)))), (1.0 / 3.0));
    double dim2 = 2 * pow((0.75 * ((M_PI / 4.0) - sin((M_PI / 4.0)))), (1.0 / 3.0));
    double dim3 = 2 * pow((0.75 * ((M_PI / 4.0) - sin((M_PI / 4.0)))), (1.0 / 3.0));

    for (int i = 0; i < mdfsize; i++)
    {
      mdf[i] = 0.0;
    }
    int remainingcount = 10000;
    for (int i = 0; i < angles.size(); i++)
    {
      denom = pow((axes[3*i]*axes[3*i]+axes[3*i+1]*axes[3*i+1]+axes[3*i+2]*axes[3*i+2]),0.5);
      n1 = axes[3*i]/denom;
      n2 = axes[3*i+1]/denom;
      n3 = axes[3*i+2]/denom;
      r1 = n1 * pow(((3.0 / 4.0) * (angles[i] - sin(angles[i]))), (1.0 / 3.0));
      r2 = n2 * pow(((3.0 / 4.0) * (angles[i] - sin(angles[i]))), (1.0 / 3.0));
      r3 = n3 * pow(((3.0 / 4.0) * (angles[i] - sin(angles[i]))), (1.0 / 3.0));
      mbin = MisorientationCalculations::getMisoBinCubic(r1,r2,r3);
      mdf[mbin] = -int((weights[i]/double(mdfsize))*10000.0);
      remainingcount = remainingcount+mdf[mbin];
    }
    for (int i = 0; i < remainingcount; i++)
    {
      random1 = rg.Random();
      random2 = rg.Random();
      choose1 = 0;
      choose2 = 0;

      totaldensity = 0;
      for (size_t j = 0; j < odfsize; j++)
      {
        density = odf[j];
        totaldensity = totaldensity + density;
        if (random1 < totaldensity && random1 >= (totaldensity - density)) choose1 = static_cast<int> (j);
        if (random2 < totaldensity && random2 >= (totaldensity - density)) choose2 = static_cast<int> (j);
      }
      h1 = choose1 % 18;
      h2 = (choose1 / 18) % 18;
      h3 = choose1 / (18 * 18);
      random = rg.Random();
      h1 = ((dim1 / 18.0) * h1) + ((dim1 / 18.0) * random) - (dim1 / 2.0);
      random = rg.Random();
      h2 = ((dim2 / 18.0) * h2) + ((dim2 / 18.0) * random) - (dim2 / 2.0);
      random = rg.Random();
      h3 = ((dim3 / 18.0) * h3) + ((dim3 / 18.0) * random) - (dim3 / 2.0);
      hmag = pow((h1 * h1 + h2 * h2 + h3 * h3), 0.5);
      angle = pow((8 * hmag * hmag * hmag), (1.0 / 3.0));
      n1 = h1 / hmag;
      n2 = h2 / hmag;
      n3 = h3 / hmag;
      cos_angle = cos(angle/2.0);
      sin_angle = sin(angle/2.0);
      q1[0] = 1;
      q1[1] = sin_angle * n1;
      q1[2] = sin_angle * n2;
      q1[3] = sin_angle * n3;
      q1[4] = cos_angle;
      h1 = choose2 % 18;
      h2 = (choose2 / 18) % 18;
      h3 = choose2 / (18 * 18);
      random = rg.Random();
      h1 = ((dim1 / 18.0) * h1) + ((dim1 / 18.0) * random) - (dim1 / 2.0);
      random = rg.Random();
      h2 = ((dim2 / 18.0) * h2) + ((dim2 / 18.0) * random) - (dim2 / 2.0);
      random = rg.Random();
      h3 = ((dim3 / 18.0) * h3) + ((dim3 / 18.0) * random) - (dim3 / 2.0);
      hmag = pow((h1 * h1 + h2 * h2 + h3 * h3), 0.5);
      angle = pow((8 * hmag * hmag * hmag), (1.0 / 3.0));
      n1 = h1 / hmag;
      n2 = h2 / hmag;
      n3 = h3 / hmag;
      cos_angle = cos(angle/2.0);
      sin_angle = sin(angle/2.0);
      q2[0] = 1;
      q2[1] = sin_angle * n1;
      q2[2] = sin_angle * n2;
      q2[3] = sin_angle * n3;
      q2[4] = cos_angle;
      w = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
      w = w/radtodeg;
      r1 = n1 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
      r2 = n2 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
      r3 = n3 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
      mbin = MisorientationCalculations::getMisoBinHexagonal(r1,r2,r3);
      if(mdf[mbin] >= 0) mdf[mbin]++;
      if(mdf[mbin] < 0) i = i-1;
    }
    for (int i = 0; i < mdfsize; i++)
    {
      if(mdf[i] < 0) mdf[i] = -mdf[i];
      mdf[i] = mdf[i]/10000.0;
    }
  }

  protected:
  Texture();

  private:
  Texture(const Texture&); // Copy Constructor Not Implemented
  void operator=(const Texture&); // Operator '=' Not Implemented

};

#endif /* TEXTURE_H_ */
