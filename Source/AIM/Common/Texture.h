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

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "MXA/MXATypes.h"
#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/AIMMath.h"
#include "AIM/Common/MisorientationCalculations.h"


#define AIM_TEXTURE_COUNT 14



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

    const static size_t Count;
    const static double Values[AIM_TEXTURE_COUNT][3];
    const static char*  Names[AIM_TEXTURE_COUNT];
    const static double Weights[AIM_TEXTURE_COUNT];
    const static double Sigmas[AIM_TEXTURE_COUNT];

    //FIXME: Implement a Hexagonal Crystal Structure Calculation
    //TODO: Testing


    /**
     * @brief This will calculate ODF data based on an array of weights that are
     * passed in and a Cubic Crystal Structure. The weights are from this class
     * "Weights" static variable but
     * can have user/programmer changed values. This is templated on the container
     * type that holds the data. Containers that adhere to the STL Vector API
     * should be usable. QVector falls into this category.
     * @param weights Array of weights. The only stipulation is the length of the
     * array is Texture::Count
     * @param randomWeight  The value of the weight to be applied to random orientations
     * @param normalize Should the ODF data be normalized by the totalWeight value
     * before returning.
     * @param odf (OUT) The ODF data that is generated from this function.
     * @param totalweight (OUT) The TotalWeight value that is also calculated
     */
    template<typename T>
    static void calculateCubicODFData(T weights, T sigmas, double randomWeight, bool normalize, T &odf, double &totalweight)
    {
      int TextureBins[AIM_TEXTURE_COUNT];
      static const size_t eighteenCubed = 5832;
      odf.resize(eighteenCubed);
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

      for (size_t i = 0; i < Texture::Count; i++)
      {
        tan_term = tan(Texture::Values[i][1]/2);
        sin_term = sin((Texture::Values[i][0]-Texture::Values[i][2])/2);
        cos_term1 = cos((Texture::Values[i][0]+Texture::Values[i][2])/2);
        cos_term2 = cos((Texture::Values[i][0]-Texture::Values[i][2])/2);

        r1 = tan_term * cos_term2 / cos_term1;
        r2 = tan_term * sin_term / cos_term1;
        r3 = tan((Texture::Values[i][0]+Texture::Values[i][2])/2);
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

      for (size_t i = 0; i < eighteenCubed; i++)
      {
        odf[i] = randomWeight / (eighteenCubed);
        totalweight = totalweight + randomWeight / (eighteenCubed);
      }
      for (size_t i = 0; i < Texture::Count; i++)
      {
        bin = TextureBins[i];
        odf[bin] = odf[bin] + (weights[i]);
        totalweight = totalweight + weights[i];
        bin1 = bin % 18;
        bin2 = (bin / 18) % 18;
        bin3 = bin / (18 * 18);
        for (int j = -sigmas[i]; j <= sigmas[i]; j++)
        {
          for (int k = -sigmas[i]; k <= sigmas[i]; k++)
          {
            for (int l = -sigmas[i]; l <= sigmas[i]; l++)
            {
              addbin1 = bin1 + j;
              addbin2 = bin2 + k;
              addbin3 = bin3 + l;
              if (addbin1 < 0) addbin1 = addbin1 + 18;
              if (addbin1 >= 18) addbin1 = addbin1 - 18;
              if (addbin2 < 0) addbin2 = addbin2 + 18;
              if (addbin2 >= 18) addbin2 = addbin2 - 18;
              if (addbin3 < 0) addbin3 = addbin3 + 18;
              if (addbin3 >= 18) addbin3 = addbin3 - 18;
              addbin = (addbin3 * 18 * 18) + (addbin2 * 18) + (addbin1);
              dist = pow((j * j + k * k + l * l), 0.5);
              fraction = 1.0 - ((dist / sigmas[i]) * (dist / sigmas[i]));
              if (fraction > 0.0)
              {
                odf[addbin] = odf[addbin] + (weights[i] * fraction);
                totalweight = totalweight + (weights[i] * fraction);
              }
            }
          }
        }
      }

      if (normalize == true)
      {
        // Normalize the odf
        for (size_t i = 0; i < eighteenCubed; i++)
        {
          odf[i] = odf[i] / totalweight;
        }
      }

    }




protected:
    Texture();

private:
    Texture(const Texture&); // Copy Constructor Not Implemented
    void operator=(const Texture&); // Operator '=' Not Implemented

};

#endif /* TEXTURE_H_ */
