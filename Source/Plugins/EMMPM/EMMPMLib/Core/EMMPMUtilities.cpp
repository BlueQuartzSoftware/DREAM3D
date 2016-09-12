/*
The Original EM/MPM algorithm was developed by Mary L. Comer and is distributed
under the BSD License.
Copyright (c) <2010>, <Mary L. Comer>
All rights reserved.

[1] Comer, Mary L., and Delp, Edward J.,  ÒThe EM/MPM Algorithm for Segmentation
of Textured Images: Analysis and Further Experimental Results,Ó IEEE Transactions
on Image Processing, Vol. 9, No. 10, October 2000, pp. 1731-1744.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

Neither the name of <Mary L. Comer> nor the names of its contributors may be
used to endorse or promote products derived from this software without specific
prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "EMMPMLib/EMMPMLibTypes.h"
#include "EMMPMLib/Core/EMMPM_Constants.h"
#include "EMMPMLib/Core/EMMPM_Data.h"
#include "EMMPMLib/Common/EMMPM_Math.h"
#include "EMMPMLib/Core/InitializationFunctions.h"
#include "EMMPMLib/Core/EMMPMUtilities.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMUtilities::ConvertInputImageToWorkingImage(EMMPM_Data::Pointer data)
{
  uint8_t* dst;
  int i;
  int j;
  int d;
  size_t index = 0;
  int width;
  int height;
  int dims;

  if (data->inputImageChannels == 0)
  {
    printf("The number of input color channels was 0\n. Exiting Program.\n");
    return;
  }

  if (data->inputImageChannels != data->dims)
  {
    printf("The number of image channels does not match the number of vector dimensions\nExiting Program.\n");
    return;
  }


  /* Copy input image to y[][] */
  width = data->columns;
  height = data->rows;
  dims = data->dims;
  dst = data->inputImage;


  for (j = 0; j < height; j++)
  {
    for (i = 0; i < width; i++)
    {
      for (d = 0; d < dims; d++)
      {
        index = (j * width * dims) + (i * dims) + d;
        data->y[index] = *dst;
        ++dst;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMUtilities::ConvertXtToOutputImage(EMMPM_Data::Pointer data)
{
  size_t index;
  unsigned int i, j, d;
  unsigned char* raster;
  int l, ld;
  size_t gtindex = 0;
  size_t* classCounts = nullptr;
  size_t x = 0;

  real_t mu = 0.0;
  real_t sigma = 0.0;
  real_t twoSigSqrd = 0.0f;
  real_t constant = 0.0;
  real_t variance = 0.0;

  float sqrt2pi = sqrtf(2.0f * (float)M_PI);
  size_t histIdx = 0;
  float pixelWeight = 0.0;
  size_t totalPixels = 0;

  // Initialize all the counts to Zero
  classCounts = (size_t*)malloc(data->classes * sizeof(size_t));
  memset(classCounts, 0, data->classes * sizeof(size_t));

  if (data->outputImage == nullptr)
  {
    data->allocateOutputImage();
  }
  raster = data->outputImage;
  index = 0;
  totalPixels = data->rows * data->columns;
  unsigned int rows = data->rows;
  unsigned int columns = data->columns;
  unsigned int ixCol = 0;
  unsigned int* colorTable = data->colorTable;

  for (i = 0; i < rows; i++)
  {
    ixCol = i * columns;
    for (j = 0; j < columns; j++)
    {
      gtindex = data->xt[ ixCol + j ];
      classCounts[gtindex]++;
      raster[index++] = colorTable[gtindex];
    }
  }
  // Now we have the counts for the number of pixels of each class.
  // The "classes" loop could be its own threaded Task at this point
  //printf("=============================\n");
  for (d = 0; d < data->dims; d++)
  {
    for (l = 0; l < data->classes; ++l)
    {
      pixelWeight = (float)(classCounts[l]) / (float)(totalPixels);
      ld = data->dims * l + d;
      mu = data->mean[ld];
      variance = data->variance[ld];
      sigma = sqrt( data->variance[ld] ); // Standard Deviation is the Square Root of the Variance
      twoSigSqrd = variance * 2.0f; // variance is StdDev Squared, so just use the Variance value
      constant = 1.0f / (sigma * sqrt2pi);
      //printf("Class %d: Sigma %f  Peak Height: %f\n", l, sig, (constant * pixelWeight));
      for (x = 0; x < 256; ++x)
      {
        histIdx = (256 * data->classes * d) + (256 * l) + x;
        data->histograms[histIdx] = pixelWeight * constant * exp(-1.0f * ((x - mu) * (x - mu)) / (twoSigSqrd));
      }
    }
  }


  free(classCounts);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMUtilities::ZeroMeanVariance(int nClasses, size_t nDims, real_t* mu, real_t* var, real_t* N)
{
  size_t ld = 0;
  /* Reset model parameters to zero */
  for (int l = 0; l < nClasses; l++)
  {
    for (size_t d = 0; d < nDims; d++)
    {
      ld = nDims * l + d;
      mu[ld] = 0;
      var[ld] = 0;
    }
    N[l] = 0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EMMPMUtilities::isStoppingConditionLessThanTolerance(EMMPM_Data::Pointer data)
{
  real_t muDeltaSum = 0.0;
  real_t varDeltaSum = 0.0;

  real_t* mu = data->mean;
  real_t* prevMu = data->prev_mu;
  real_t* var = data->variance;
  real_t* prevVar = data->prev_variance;

  int nClasses = data->classes;
  size_t nDims = data->dims;

  size_t ld = 0;

  for (int l = 0; l < nClasses; l++)
  {
    for (size_t d = 0; d < nDims; d++)
    {
      ld = nDims * l + d;
      muDeltaSum += (mu[ld] - prevMu[ld]) * (mu[ld] - prevMu[ld]);
      varDeltaSum += (var[ld] - prevVar[ld]) * (var[ld] - prevVar[ld]);
    }
  }
  data->currentMSE = muDeltaSum + varDeltaSum;

  if (data->useStoppingThreshold == 0)
  {
    return false;
  }
  else if (data->currentMSE < data->stoppingThreshold)
  {
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMUtilities::copyCurrentMeanVarianceValues(EMMPM_Data::Pointer data)
{
  ::memcpy(data->prev_mu, data->mean, data->classes * data->dims * sizeof(real_t));
  ::memcpy(data->prev_variance, data->variance, data->classes * data->dims * sizeof(real_t));
}

// -----------------------------------------------------------------------------
// This class can not be easily parallelized due to the summation of the
// data->variance[ld] variable.
// -----------------------------------------------------------------------------
class EstimateMeans
{
  public:
    EstimateMeans(EMMPM_Data* dPtr, size_t l)
    {
      this->l = l;
      this->data = dPtr;
    }
    virtual ~EstimateMeans() {}

    void calc(int rowStart, int rowEnd, int colStart, int colEnd) const
    {
      int dims = data->dims;
      int rows = data->rows;
      int cols = data->columns;
      int32_t k_, k2_, lij, ld, ijd, k_temp, k2_temp;
      real_t* m = data->mean;
      unsigned char* y = data->y;
      real_t* probs = data->probs;
      real_t* N = data->N;

      k_temp = (cols * rows * l);
      for (int r = rowStart; r < rowEnd; r++)
      {
        k_ = k_temp + (cols * r);
        k2_temp = (dims * cols * r);
        for (int c = colStart; c < colEnd; c++)
        {
          k2_ = k2_temp + ( dims * c);
          lij = k_ + c;
          N[l] += probs[lij]; // denominator of (20)
          for (int d = 0; d < dims; d++)
          {
            ld = dims * l + d;
            ijd = k2_ + d;
            m[ld] += y[ijd] * probs[lij]; // numerator of (20)
          }
        }
      }
      if (N[l] != 0)
      {
        for (int d = 0; d < dims; d++)
        {
          ld = dims * l + d;
          m[ld] = m[ld] / N[l];
        }
      }
    }

  private:
    size_t l;
    EMMPM_Data* data;
};

/* This class can not be easily parallelized due to the summation of the
 * data->variance[ld] variable.
 */
class EstimateVariance
{
  public:
    EstimateVariance(EMMPM_Data* dPtr, size_t l)
    {
      this->l = l;
      this->data = dPtr;
    }
    virtual ~EstimateVariance() {}

    void calc(int rowStart, int rowEnd, int colStart, int colEnd) const
    {
      int dims = data->dims;
      int rows = data->rows;
      int cols = data->columns;
      int32_t k_, k2_, lij, ld, ijd, k_temp, k2_temp;
      real_t* m = data->mean;
      unsigned char* y = data->y;
      real_t* probs = data->probs;
      real_t* N = data->N;
      real_t res = 0.0f;
      real_t* v = data->variance;
      int32_t dimsXl = dims * l;

      k_temp = (cols * rows * l);
      for (int r = rowStart; r < rowEnd; r++)
      {
        k_ = k_temp + (cols * r);
        k2_temp = (dims * cols * r);
        for (int c = colStart; c < colEnd; c++)
        {
          k2_ = k2_temp + ( dims * c);
          // numerator of (21)
          lij = k_ + c;
          for (int d = 0; d < dims; d++)
          {
            ld = dimsXl + d;
            ijd = k2_ + d;
            res = y[ijd] - m[ld];
            res = res * res; // Square to get the Variance
            v[ld] += (res) * probs[lij];
          }
        }
      }

      if(N[l] != 0)
      {
        for (int d = 0; d < dims; d++)
        {
          ld = dims * l + d;
          v[ld] = v[ld] / N[l];
        }
      }
    }

  private:
    size_t l;
    EMMPM_Data* data;
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMUtilities::UpdateMeansAndVariances(EMMPM_Data::Pointer dt)
{
  EMMPM_Data* data = dt.get();

  size_t l;
// size_t dims = data->dims;
  size_t rows = data->rows;
  size_t cols = data->columns;
  size_t classes = data->classes;


  /*** Some efficiency was sacrificed for readability below ***/
  /* Update estimates for mean of each class - (Maximization) */
  for (l = 0; l < classes; l++)
  {
    EstimateMeans estimateMeans(data, l);
    estimateMeans.calc(0, rows, 0, cols);
  }

  // Eq. (20)}
  /* Update estimates of variance of each class */
  for (l = 0; l < classes; l++)
  {
    EstimateVariance estimateVariance(data, l);
    estimateVariance.calc(0, rows, 0, cols);
  }

  // Make sure we don't fall below some minimum variance.
  for (l = 0; l < classes; l++)
  {
    if(data->variance[l] < data->min_variance[l])
    {
      data->variance[l] = data->min_variance[l];
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMUtilities::MonitorMeansAndVariances(EMMPM_Data::Pointer dt)
{
  EMMPM_Data* data = dt.get();

  size_t l, d;
  size_t classes = data->classes;
  size_t dims = data->dims;
  char msgbuff[256];
  memset(msgbuff, 0, 256);

  printf("Class\tDim\tMu\tVariance(StdDev^2)\n");

  for (l = 0; l < classes; l++)
  {
    for (d = 0; d < dims; d++)
    {
      printf("%d\t%d\t%3.3f\t%3.3f\n", (int)l, (int)d, data->mean[l], data->variance[l]);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMUtilities::RemoveZeroProbClasses(EMMPM_Data::Pointer dt)
{
  EMMPM_Data* data = dt.get();

  size_t kk, l, dd, ld, l1d, i, j, ij;
  size_t dims = data->dims;
  size_t rows = data->rows;
  size_t cols = data->columns;
  size_t classes = data->classes;

  for (kk = 0; kk < classes; kk++)
  {
    if (data->N[kk] == 0)
    {
      for (l = kk; l < classes - 1; l++)
      {
        /* Move other classes to fill the gap */
        data->N[l] = data->N[l + 1];
        for (dd = 0; dd < dims; dd++)
        {
          ld = dims * l + dd;
          l1d = (dims * (l + 1)) + dd;
          data->mean[ld] = data->mean[l1d];
          data->variance[ld] = data->variance[l1d];
        }
      }
      for (i = 0; i < rows; i++)
      {
        for (j = 0; j < cols; j++)
        {
          ij = (cols * i) + j;
          if (data->xt[ij] > kk) { data->xt[ij]--; }
        }
      }
      classes = classes - 1; // push the eliminated class into the last class
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMUtilities::ComputeEntropy(real_t** *probs, unsigned char** output,
                                    unsigned int rows, unsigned int cols, unsigned int classes)
{
  unsigned int l, i, j;
  real_t entr;

  for(i = 0; i < rows; i++)
  {
    for(j = 0; j < cols; j++)
    {
      entr = 0;
      /* Compute entropy of each pixel */
      for(l = 0; l < classes; l++)
      {
        if(probs[l][i][j] > 0)
        { entr -= probs[l][i][j] * (log10(probs[l][i][j]) / log10(2.0f)); }
      }
      output[i][j] = (unsigned char)(entr + 0.5);
    }
  }

}
