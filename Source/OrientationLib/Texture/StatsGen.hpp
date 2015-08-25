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


#ifndef _STATSGEN_H_
#define _STATSGEN_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"
#include "OrientationLib/Texture/Texture.hpp"


/**
 * @brief This class contains static functions to generate ODF and MDF data as X,Y points. This data can be discretized
 * onto a regular grid which would result in standard ODF Pole Figures and a regular 2D MDF plot.
 * @class StatsGen StatsGen.h StatsGenerator/StatsGen.h
 * @author Michael A. Groeber US Air Force Research Laboratory
 * @date Dec 16, 2010
 * @version 1.2
 */
class StatsGen
{
  public:

    virtual ~StatsGen() {}

    /**
     * @brief Generates  XY Scatter plot data for a Beta Distribution function
     * @param alpha Alpha Term in the Beta Distribution function
     * @param beta Beta term in the Beta Distribution function
     * @param x X Values (Output)
     * @param y Y Value (Output)
     * @param size Number of points in the scatter plot
     * @return Error Code. 0 is NO ERROR.
     */
    template<typename Vector>
    static int GenBetaPlotData(float alpha, float beta, Vector& x, Vector& y, int size)
    {
      int err = 0;
      float total = 0;
      float betain, betaout;
      x.resize(size);
      y.resize(size);
      for (int i = 0; i < size; i++)
      {
        betain = (i * (1.0 / float(size))) + ((1.0 / float(size)) / 2.0);
        betaout = powf(betain, (alpha - 1)) * powf((1 - betain), (beta - 1));
        x[i] = betain;
        y[i] = betaout;
        total = total + betaout;
        if (betaout < 0) { err = 1; }
      }
      for (int i = 0; i < size; i++)
      {
        y[i] = y[i] / total;
      }
      return err;
    }

    /**
    * @brief GenLogNormalPlotData Generates Log Normal plot data
    * @param avg The Average
    * @param stdDev The standard deviation
    * @param x The generated X values. Must conform to the STL vector API
    * @param y The generated Y values. Must conform to the STL vector API
    * @param size The number of values to create
    * @return
    */
    template<typename Vector>
    static int GenLogNormalPlotData(float avg, float stdDev, Vector& x, Vector& y, int size)
    {
      int err = 0;
      float lognormin, lognormout, max, min;
      float s2 = powf(stdDev, 2);
      float root2pi = powf( (float)(M_2PI), 0.5);
      x.resize(size);
      y.resize(size);
      min = exp(avg - (5 * stdDev));
      max = exp(avg + (5 * stdDev));
      for (int i = 0; i < size; i++)
      {
        lognormin = (i * ((max - min) / float(size))) + (((max - min) / float(size)) / 2.0) + min;
        lognormout = (1.0 / (lognormin * stdDev * root2pi)) * exp(-((log(lognormin) - avg) * (log(lognormin) - avg)) / (2 * s2));
        x[i] = lognormin;
        y[i] = lognormout * ((max - min) / float(size));
        if (lognormout < 0) { err = 1; }
      }
      return err;
    }

    /**
     * @brief GenPowerLawPlotData
     * @param alpha
     * @param k
     * @param beta
     * @param x
     * @param y
     * @param size
     * @return
     */
    template<typename Vector>
    static int GenPowerLawPlotData(float alpha, float k, float beta, Vector& x, Vector& y, int size)
    {
      int err = 0;
      float in, out, max, min;
      x.resize(size);
      y.resize(size);
      min = 0;
      max = 3;
      for (int i = 0; i < size; i++)
      {
        in = (i * ((max - min) / float(size))) + (((max - min) / float(size)) / 2.0) + min;
        out = alpha * powf(in, k) + beta;
        x[i] = in;
        y[i] = out;
        if (out < 0) { err = 1; }
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
    static int ComputeNumberOfBins(float mu, float sigma, float minCutOff, float maxCutOff,
                                   float binstep, float& max, float& min)
    {
      min = expf(mu - (minCutOff * sigma));
      max = expf(mu + (maxCutOff * sigma));
      return static_cast<int>((max - min) / binstep) + 1;
    }

    /**
     * @brief Generates the CutOff values
     * @param mu Must be (0.0, N] where N should be less than ?? (20)
     * @param sigma
     * @param cutoff
     * @param binstep
     * @param x Type that adheres to the QVector API
     * @param y Type that adheres to the QVector API
     * @param yMax
     * @param numsizebins
     * @param binsizes Type that adheres to the QVector API
     * @return
     */
    template<typename J, typename Vector>
    static int GenCutOff(J mu, J sigma, J minCutOff, J maxCutOff, J binstep, Vector& x, Vector& y, J yMax, int& numsizebins, Vector& binsizes)
    {
      J max, min;
      numsizebins = StatsGen::ComputeNumberOfBins(mu, sigma, minCutOff, maxCutOff, binstep, max, min);
      if(numsizebins < 0)
      {
        numsizebins = 0;
        return -1;
      }
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
     * @brief  This method will generate ODF data for 3 scatter plots which are the
     * <001>, <011> and <111> directions.
     * @param odf Pointer to ODF bin data which has been sized to CubicOps::k_OdfSize
     * @param eulers Euler angles to be generated. This memory must already be preallocated.
     * @param npoints The number of points for the Scatter Plot which is at least the number of elements used in the allocation of the various output arrays.
     */
    template<typename T>
    static int GenCubicODFPlotData(const T* odf, T* eulers, size_t npoints)
    {
      uint64_t m_Seed = QDateTime::currentMSecsSinceEpoch();
      DREAM3D_RANDOMNG_NEW_SEEDED(m_Seed);
      int err = 0;
      int choose;
      T totaldensity;
      T random, density;

      CubicOps ops;
      T td1;
      //float* odfPtr = &(odf.front());
      for (size_t i = 0; i < npoints; i++)
      {
        m_Seed++;
        random = rg.genrand_res53();
        choose = 0;
        totaldensity = 0;
        for (int j = 0; j < CubicOps::k_OdfSize; j++)
        {
          density = odf[j];
          td1 = totaldensity;
          totaldensity = totaldensity + density;
          if (random < totaldensity && random >= td1) { choose = static_cast<int> (j); break; }
        }
        FOrientArrayType eu = ops.determineEulerAngles(m_Seed, choose);
        eulers[3 * i + 0] = eu[0];
        eulers[3 * i + 1] = eu[1];
        eulers[3 * i + 2] = eu[2];
      }
      return err;
    }

    /**
     * @brief  This method will generate ODF data for a Hexagonal material and
     * generate 3 scatter plots which are the
     * <001>, <011> and <111> directions. The method is a C++ Template which the parameter
     * type is a QVector conforming class type that holds the data.
     * QVector falls into this category. The input data for the
     * euler angles is in Columnar fashion instead of row major format.
     * @param odf [input] The ODF data
     * @param eulers Euler angles to be generated. This memory must already be preallocated.
     * @param size The number of points for the Scatter Plot
     */
    template<typename T>
    static int GenHexODFPlotData(T* odf, T* eulers, int npoints)
    {
      uint64_t m_Seed = QDateTime::currentMSecsSinceEpoch();
      DREAM3D_RANDOMNG_NEW_SEEDED(m_Seed);
      int err = 0;
      int choose;
      float totaldensity;
      float random, density;
      HexagonalOps ops;

      float td1;
      for (int i = 0; i < npoints; i++)
      {
        m_Seed++;
        random = rg.genrand_res53();
        choose = 0;
        totaldensity = 0;
        for (int j = 0; j < HexagonalOps::k_OdfSize; j++)
        {
          density = odf[j];
          td1 = totaldensity;
          totaldensity = totaldensity + density;
          if (random < totaldensity && random >= td1) { choose = static_cast<int> (j); break; }
        }
        FOrientArrayType eu = ops.determineEulerAngles(m_Seed, choose);
        eulers[3 * i + 0] = eu[0];
        eulers[3 * i + 1] = eu[1];
        eulers[3 * i + 2] = eu[2];
      }
      return err;
    }

    /**
     * @brief  This method will generate ODF data for a OrthoRhombic material and
     * generate 3 scatter plots which are the
     * <001>, <011> and <111> directions. The method is a C++ Template which the parameter
     * type is a QVector conforming class type that holds the data.
     * QVector falls into this category. The input data for the
     * euler angles is in Columnar fashion instead of row major format.
     * @param odf The ODF Data
     * @param eulers Euler angles to be generated. This memory must already be preallocated.
     * @param size The number of points for the Scatter Plot
     */
    template<typename T>
    static int GenOrthoRhombicODFPlotData(T* odf, T* eulers, int npoints)
    {
      uint64_t m_Seed = QDateTime::currentMSecsSinceEpoch();
      DREAM3D_RANDOMNG_NEW_SEEDED(m_Seed);
      int err = 0;
      int choose;
      float totaldensity;
      float random, density;
      OrthoRhombicOps ops;

      float td1;
      for (int i = 0; i < npoints; i++)
      {
        m_Seed++;
        random = rg.genrand_res53();
        choose = 0;
        totaldensity = 0;
        for (int j = 0; j < OrthoRhombicOps::k_OdfSize; j++)
        {
          density = odf[j];
          td1 = totaldensity;
          totaldensity = totaldensity + density;
          if (random < totaldensity && random >= td1)
          {
            choose = static_cast<int> (j);
            break;
          }
        }
        FOrientArrayType eu = ops.determineEulerAngles(m_Seed, choose);
        eulers[3 * i + 0] = eu[0];
        eulers[3 * i + 1] = eu[1];
        eulers[3 * i + 2] = eu[2];
      }
      return err;
    }

    /**
     * @brief  This method will generate ODF data for a OrthoRhombic material and
     * generate 3 scatter plots which are the
     * <001>, <011> and <111> directions. The method is a C++ Template which the parameter
     * type is a QVector conforming class type that holds the data.
     * QVector falls into this category. The input data for the
     * euler angles is in Columnar fashion instead of row major format.
     * @param e1s The first euler angles (input)
     * @param e2s The second euler angles (input)
     * @param e3s The third euler angles (input)
     * @param weights Array of weights values. (input)
     * @param sigmas Array of sigma values. (input)
     * @param xA X Values of the A axis PF Scatter plot (Output). This memory must already be preallocated.
     * @param yA Y Values of the A axis PF Scatter plot (Output). This memory must already be preallocated.
     * @param xB X Values of the B axis PF Scatter plot (Output). This memory must already be preallocated.
     * @param yB Y Values of the B axis PF Scatter plot (Output). This memory must already be preallocated.
     * @param xC X Values of the C axis PF Scatter plot (Output). This memory must already be preallocated.
     * @param yC Y Values of the C axis PF Scatter plot (Output). This memory must already be preallocated.
     * @param size The number of points for the Scatter Plot
     */
    template<typename T>
    static int GenAxisODFPlotData(T* odf, T* eulers, int npoints)
    {
      uint64_t m_Seed = QDateTime::currentMSecsSinceEpoch();
      DREAM3D_RANDOMNG_NEW_SEEDED(m_Seed);
      int err = 0;
      int choose;
      float totaldensity;
      float random, density;
      OrthoRhombicOps ops;

      float td1;
      for (int i = 0; i < npoints; i++)
      {
        m_Seed++;
        random = rg.genrand_res53();
        choose = 0;
        totaldensity = 0;
        for (int j = 0; j < OrthoRhombicOps::k_OdfSize; j++)
        {
          density = odf[j];
          td1 = totaldensity;
          totaldensity = totaldensity + density;
          if (random < totaldensity && random >= td1)
          {
            choose = static_cast<int> (j);
            break;
          }
        }
        FOrientArrayType eu = ops.determineEulerAngles(m_Seed, choose);
        eulers[3 * i + 0] = eu[0];
        eulers[3 * i + 1] = eu[1];
        eulers[3 * i + 2] = eu[2];
      }
      return err;
    }

    /**
     * @brief  This method will generate MDF data for a Cubic material and
     * generate 1 XY scatter plots.
     * @param mdf [input] This is the input MDF data which is already computed and of lenght CubicOps::k_MdfSize
     * @param x [output] X Values of the Scatter plot. This memory must already be preallocated.
     * @param y [outout] Y Values of the Scatter plot. This memory must already be preallocated.
     * @param npoints The number of XY points for the Scatter Plot
     * @param size The number of samples of the MDF to take
     */
    template<typename T>
    static int GenCubicMDFPlotData(T* mdf, T* xval, T* yval, int npoints, int size)
    {
      float radtodeg = 180.0f / float(M_PI);

      uint64_t m_Seed = QDateTime::currentMSecsSinceEpoch();
      DREAM3D_RANDOMNG_NEW_SEEDED(m_Seed);

      int err = 0;
      float density;
      float totaldensity;
      int choose = 0;
      float random;
      float w;

      CubicOps ops;

      for (int i = 0; i < npoints; i++)
      {
        yval[i] = 0;
      }

      float td1;
      for (int i = 0; i < size; i++)
      {
        m_Seed++;
        random = rg.genrand_res53();
        choose = 0;
        totaldensity = 0;
        for (int j = 0; j < CubicOps::k_MdfSize; j++)
        {
          density = mdf[j];
          td1 = totaldensity;
          totaldensity = totaldensity + density;
          if (random < totaldensity && random >= td1)
          {
            choose = static_cast<int> (j);
            break;
          }
        }
        FOrientArrayType rod = ops.determineRodriguesVector(m_Seed, choose);
        FOrientArrayType ax(4, 0.0);
        FOrientTransformsType::ro2ax(rod, ax);

        w = ax[3] * radtodeg;
        yval[int(w / 5.0)]++;
      }
      for (int i = 0; i < npoints; i++)
      {
        xval[i] = i * 5.0 + 2.5;
        yval[i] = yval[i] / float(size);
      }
      return err;
    }

    /**
     * @brief  This method will generate MDF data for a Hexagonal material and
     * generate 1 XY scatter plots.
     * @param mdf [input] This is the input MDF data which is already computed and of lenght CubicOps::k_MdfSize. This memory must already be preallocated.
     * @param x [output] X Values of the Scatter plot. This memory must already be preallocated.
     * @param y [outout] Y Values of the Scatter plot. This memory must already be preallocated.
     * @param npoints The number of XY points for the Scatter Plot
     * @param size The number of samples of the MDF to take
     */
    template<typename T>
    static int GenHexMDFPlotData(T* mdf, T* xval, T* yval, int npoints, int size)
    {
      float radtodeg = 180.0f / float(M_PI);

      uint64_t m_Seed = QDateTime::currentMSecsSinceEpoch();
      DREAM3D_RANDOMNG_NEW_SEEDED(m_Seed);

      int err = 0;
      int choose = 0;
      float density;
      float totaldensity;
      float random;
      HexagonalOps ops;

      for (int i = 0; i < npoints; i++)
      {
        yval[i] = 0;
      }
      //    float ra1, ra2, ra3, rb1, rb2, rb3, rc1, rc2, rc3;
      float td1;
      for (int i = 0; i < size; i++)
      {
        m_Seed++;
        random = rg.genrand_res53();
        choose = 0;
        totaldensity = 0;
        for (size_t j = 0; j < HexagonalOps::k_MdfSize; j++)
        {
          density = mdf[j];
          td1 = totaldensity;
          totaldensity = totaldensity + density;
          if (random < totaldensity && random >= td1) { choose = static_cast<int> (j); break; }
        }
        FOrientArrayType rod = ops.determineRodriguesVector(m_Seed, choose);
        FOrientArrayType ax(4, 0.0);
        FOrientTransformsType::ro2ax(rod, ax);

        float w = ax[3] * radtodeg;
        size_t index = static_cast<size_t>(w / 5.0f);
        yval[index]++;
      }
      for (int i = 0; i < npoints; i++)
      {
        xval[i] = i * 5.0 + 2.5;
        yval[i] = yval[i] / float(size);
      }
      return err;
    }

  protected:
    StatsGen() {}

  private:
    StatsGen(const StatsGen&); // Copy Constructor Not Implemented
    void operator=(const StatsGen&); // Operator '=' Not Implemented
};

#endif /* _STATSGEN_H_ */

