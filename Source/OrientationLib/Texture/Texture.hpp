/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#pragma once

#include <fstream>
#include <vector>
#include <chrono>
#include <random>

#include <QtCore/QString>

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Math/SIMPLibRandom.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationLib/Core/Orientation.hpp"
#include "OrientationLib/Core/OrientationTransformation.hpp"
#include "OrientationLib/Core/Quaternion.hpp"
#include "OrientationLib/LaueOps/CubicOps.h"
#include "OrientationLib/LaueOps/HexagonalOps.h"
#include "OrientationLib/LaueOps/LaueOps.h"
#include "OrientationLib/LaueOps/OrthoRhombicOps.h"

/**
 * @brief This class holds default data for Orientation Distribution Function (ODF)
 * and Misorientation Distribution Functions (MDF)
 * calculations that the DREAM3D package will perform.
 *
 */

class Texture
{
public:
  virtual ~Texture() = default;

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
   * @param numEntries (OUT) The TotalWeight value that is also calculated
   */
  template <typename T, class LaueOps, class Container>
  static void CalculateODFData(Container& e1s, Container& e2s, Container& e3s, Container& weights, Container& sigmas, bool normalize, Container& odf, size_t numEntries)
  {
    LaueOps ops;
    std::array<size_t, 3> odfNumBins = ops.getOdfNumBins();
    odf.resize(ops.getODFSize());
    Int32ArrayType::Pointer textureBins = Int32ArrayType::CreateArray(numEntries, "TextureBins", true);
    int32_t* TextureBins = textureBins->getPointer(0);

    float addweight = 0;
    float totaladdweight = 0;
    float totalweight = float(ops.getODFSize());
    int bin, addbin;
    int bin1, bin2, bin3;
    int addbin1, addbin2, addbin3;
    float dist, fraction;
    // Use double precision for the calculations
    for(size_t i = 0; i < numEntries; i++)
    {
      OrientationF eu(e1s[i], e2s[i], e3s[i]);
      OrientationD rod = OrientationTransformation::eu2ro<OrientationF, OrientationD>(eu);

      rod = ops.getODFFZRod(rod);
      bin = ops.getOdfBin(rod);
      TextureBins[i] = static_cast<int>(bin);
    }

    for(int i = 0; i < ops.getODFSize(); i++)
    {
      odf[i] = 0;
    }
    for(size_t i = 0; i < numEntries; i++)
    {
      bin = TextureBins[i];
      bin1 = bin % odfNumBins[0];
      bin2 = (bin / odfNumBins[0]) % odfNumBins[1];
      bin3 = bin / (odfNumBins[0] * odfNumBins[1]);
      for(int j = -sigmas[i]; j <= sigmas[i]; j++)
      {
        int jsqrd = j * j;
        for(int k = -sigmas[i]; k <= sigmas[i]; k++)
        {
          int ksqrd = k * k;
          for(int l = -sigmas[i]; l <= sigmas[i]; l++)
          {
            int lsqrd = l * l;
            addbin1 = bin1 + int(j);
            addbin2 = bin2 + int(k);
            addbin3 = bin3 + int(l);
            int good = 1;
            if(addbin1 < 0)
            {
              good = 0;
            }
            if(addbin1 >= odfNumBins[0])
            {
              good = 0;
            }
            if(addbin2 < 0)
            {
              good = 0;
            }
            if(addbin2 >= odfNumBins[1])
            {
              good = 0;
            }
            if(addbin3 < 0)
            {
              good = 0;
            }
            if(addbin3 >= odfNumBins[2])
            {
              good = 0;
            }
            addbin = (addbin3 * odfNumBins[0] * odfNumBins[1]) + (addbin2 * odfNumBins[0]) + (addbin1);
            dist = powf((jsqrd + ksqrd + lsqrd), 0.5);
            fraction = 1.0 - (double(dist / int(sigmas[i])) * double(dist / int(sigmas[i])));
            if(dist <= int(sigmas[i]) && good == 1)
            {
              addweight = (weights[i] * fraction);
              if(sigmas[i] == 0.0)
              {
                addweight = weights[i];
              }
              odf[addbin] = odf[addbin] + addweight;
              totaladdweight = totaladdweight + addweight;
            }
          }
        }
      }
    }
    // These next loops *look* like they can be parallelized but the arrays are not large enough
    // to see any benefit so don't go down that road. std::transform is also slower than the
    // manual loops that are coded.
    if(totaladdweight > totalweight)
    {
      float scale = (totaladdweight / totalweight);
      for(int i = 0; i < ops.getODFSize(); i++)
      {
        odf[i] = odf[i] / scale;
      }
    }
    else
    {
      float remainingweight = totalweight - totaladdweight;
      float background = remainingweight / static_cast<float>(ops.getODFSize());
      for(int i = 0; i < ops.getODFSize(); i++)
      {
        odf[i] += background;
      }
    }
    if(normalize)
    {
      // Normalize the odf
      for(int i = 0; i < ops.getODFSize(); i++)
      {
        odf[i] = odf[i] / totalweight;
      }
    }
  }

  /**
   * @brief CalculateMDFData Calculates MDF (Misorientation Distribution Function) data
   * @param angles The angles
   * @param axes The axes
   * @param weights The weights
   * @param odf The ODF which has been already computed and sized correctly in another function
   * @param mdf [output] The MDF array to store the data which has been preallocated already
   * @param numEntries The number of elemnts in teh Angles/Axes/Weights arrays which should all the be same size or at least
   * the value passed here is the minium size of all the arrays. The sizes of the ODF and MDF arrays are
   * determined by calling the getODFSize and getMDFSize functions of the parameterized LaueOps class.
   */
  template <typename T, class LaueOps, class Container>
  static void CalculateMDFData(Container& angles, Container& axes, Container& weights, Container& odf, Container& mdf, size_t numEntries)
  {

    LaueOps orientationOps;
    const int odfsize = orientationOps.getODFSize();
    const int mdfsize = orientationOps.getMDFSize();
    mdf.resize(orientationOps.getMDFSize());
    // uint64_t m_Seed = QDateTime::currentMSecsSinceEpoch();

    // Create a Random Number generator
    std::random_device randomDevice;           // Will be used to obtain a seed for the random number engine
    std::mt19937_64 generator(randomDevice()); // Standard mersenne_twister_engine seeded with rd()
    std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
    generator.seed(seed);
    std::uniform_real_distribution<> distribution(0.0, 1.0);

    std::array<double, 3> randx3;

    int mbin;

    int choose1, choose2;

    float totaldensity;

    float random1, random2, density;

    for(int i = 0; i < mdfsize; i++)
    {
      mdf[i] = 0.0;
    }
    int remainingcount = 10000;
    int aSize = static_cast<int>(numEntries);
    for(int i = 0; i < aSize; i++)
    {
      OrientationD ax(axes[3 * i], axes[3 * i + 1], axes[3 * i + 2], angles[i]);
      OrientationD rod = OrientationTransformation::ax2ro<OrientationD, OrientationD>(ax);

      rod = orientationOps.getMDFFZRod(rod);
      mbin = orientationOps.getMisoBin(rod);
      mdf[mbin] = -1 * static_cast<int>((weights[i] / static_cast<float>(mdfsize)) * 10000.0);
      remainingcount = remainingcount + mdf[mbin];
    }

    for(int i = 0; i < remainingcount; i++)
    {

      random1 = distribution(generator);
      random2 = distribution(generator);
      choose1 = 0;
      choose2 = 0;

      totaldensity = 0;
      for(int j = 0; j < odfsize; j++)
      {
        density = odf[j];
        float d = totaldensity;
        totaldensity = totaldensity + density;
        if(random1 >= d && random1 < totaldensity)
        {
          choose1 = static_cast<int>(j);
        }
        if(random2 >= d && random2 < totaldensity)
        {
          choose2 = static_cast<int>(j);
        }
      }

      randx3[0] = distribution(generator);
      randx3[1] = distribution(generator);
      randx3[2] = distribution(generator);
      OrientationD eu = orientationOps.determineEulerAngles(randx3.data(), choose1);
      QuatType q1 = OrientationTransformation::eu2qu<OrientationD, QuatType>(eu);

      randx3[0] = distribution(generator);
      randx3[1] = distribution(generator);
      randx3[2] = distribution(generator);
      eu = orientationOps.determineEulerAngles(randx3.data(), choose2);
      QuatType q2 = OrientationTransformation::eu2qu<OrientationD, QuatType>(eu);
      OrientationD ax = orientationOps.calculateMisorientation(q1, q2);
      OrientationD ro = OrientationTransformation::ax2ro<OrientationD, OrientationD>(ax);

      ro = orientationOps.getMDFFZRod(ro); // <==== THIS IS NOT IMPELMENTED FOR ALL LAUE CLASSES
      mbin = orientationOps.getMisoBin(ro);
      if(mdf[mbin] >= 0)
      {
        mdf[mbin]++;
      }
      if(mdf[mbin] < 0)
      {
        i = i - 1;
      }
    }
    for(int i = 0; i < mdfsize; i++)
    {
      if(mdf[i] < 0)
      {
        mdf[i] = -mdf[i];
      }
      mdf[i] = mdf[i] / 10000.0;
    }
  }

protected:
  Texture() = default;

public:
  Texture(const Texture&) = delete;            // Copy Constructor Not Implemented
  Texture(Texture&&) = delete;                 // Move Constructor Not Implemented
  Texture& operator=(const Texture&) = delete; // Copy Assignment Not Implemented
  Texture& operator=(Texture&&) = delete;      // Move Assignment Not Implemented
};
