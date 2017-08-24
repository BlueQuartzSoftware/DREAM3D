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

/* acvmpm.c */

/* Modified by Joel Dumke on 1/30/09 */

/* Heavily modified from the original by Michael A. Jackson for BlueQuartz Software
 * and funded by the Air Force Research Laboratory, Wright-Patterson AFB.
 */
#include "MPMCalculation.h"

//-- C Includes
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-- C++ includes
#include <random>
#include <chrono>

#include "EMMPMLib/Common/EMMPM_Math.h"
#include "EMMPMLib/Common/EMTime.h"
#include "EMMPMLib/Common/MSVCDefines.h"
#include "EMMPMLib/Core/EMMPM.h"
#include "EMMPMLib/Core/EMMPMUtilities.h"

#define USE_TBB_TASK_GROUP 0
#ifdef EMMPM_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>

#endif

#define COMPUTE_C_CLIQUE(C, x, y, ci, cj)                                                                                                                                                              \
  if((x) < 0 || (x) >= colEnd || (y) < 0 || (y) >= rowEnd)                                                                                                                                             \
  {                                                                                                                                                                                                    \
    C[ci][cj] = classes;                                                                                                                                                                               \
  }                                                                                                                                                                                                    \
  else                                                                                                                                                                                                 \
  {                                                                                                                                                                                                    \
    ij = (cols * (y)) + (x);                                                                                                                                                                           \
    C[ci][cj] = xt[ij];                                                                                                                                                                                \
  }

/**
 * @class ParallelCalcLoop ParallelCalcLoop.h EMMPM/Curvature/ParallelCalcLoop.h
 * @brief This class can calculate the parts of the MPM loop in parallel
 *
 * @date March 11, 2012
 * @version 1.0
 */
class ParallelMPMLoop
{
public:
#if USE_TBB_TASK_GROUP
  ParallelCalcLoop(EMMPM_Data* dPtr, real_t* ykPtr, real_t* rnd, int rowStart, int rowEnd, int colStart, int colEnd)
  : m_RowStart(rowStart)
  , m_RowEnd(rowEnd)
  , m_ColStart(colStart)
  , m_ColEnd(colEnd)
  ,
#else
  ParallelMPMLoop(EMMPM_Data* dPtr, real_t* ykPtr, real_t* rnd)
  :
#endif

      data(dPtr)
  , yk(ykPtr)
  , rnd(rnd)
  {
  }
  virtual ~ParallelMPMLoop()
  {
  }

  void calc(int rowStart, int rowEnd, int colStart, int colEnd) const
  {
    // uint64_t millis = EMMPM_getMilliSeconds();
    //  int l;
    real_t prior;
    int32_t ij, lij;
    int rows = data->rows;
    int cols = data->columns;
    int classes = data->classes;

    real_t xrnd, current;
    real_t post[EMMPM_MAX_CLASSES], sum, edge;

    size_t nsCols = data->columns - 1;
    size_t ewCols = data->columns;
    size_t swCols = data->columns - 1;
    size_t nwCols = data->columns - 1;

    unsigned char* xt = data->xt;
    real_t* probs = data->probs;
    real_t* ccost = data->ccost;
    real_t* ns = data->ns;
    real_t* ew = data->ew;
    real_t* sw = data->sw;
    real_t* nw = data->nw;
    real_t curvature_value = (real_t)0.0;

    int C[3][3]; // This is the Clique for the current Pixel
                 //      --------- X -----
                 //      |   | 0 | 1 | 2 |
                 //      -----------------
                 //   Y  | 0 |   |   |   |
                 //      -----------------
                 //      | 1 |   | P |   |
                 //      -----------------
                 //      | 2 |   |   |   |
                 //
                 // When we calculate the "C" matrix if the pixel value for the specific index of
                 // the clique would be off the image then a value = number of classes is
                 // used for the C[i][j]. That way we can figure out if we are off the image

    std::stringstream ss;
    unsigned int cSize = classes + 1;
    real_t* coupling = data->couplingBeta;

    for(int32_t y = rowStart; y < rowEnd; y++)
    {
      for(int32_t x = colStart; x < colEnd; x++)
      {

        /* -------------  */
        COMPUTE_C_CLIQUE(C, x - 1, y - 1, 0, 0);
        COMPUTE_C_CLIQUE(C, x, y - 1, 1, 0);
        COMPUTE_C_CLIQUE(C, x + 1, y - 1, 2, 0);
        COMPUTE_C_CLIQUE(C, x - 1, y, 0, 1);
        COMPUTE_C_CLIQUE(C, x + 1, y, 2, 1);
        COMPUTE_C_CLIQUE(C, x - 1, y + 1, 0, 2);
        COMPUTE_C_CLIQUE(C, x, y + 1, 1, 2);
        COMPUTE_C_CLIQUE(C, x + 1, y + 1, 2, 2);

#if 0
          if (y == rowStart + 1 && x == colStart + 1)
          {
            ss << "------------------------------" << std::endl;
            ss << "|" << C[0][0] << "\t" << C[1][0] << "\t" << C[2][0] << std::endl;
            ss << "|" << C[0][1] << "\t" << C[1][1] << "\t" << C[2][1] << std::endl;
            ss << "|" << C[0][2] << "\t" << C[1][2] << "\t" << C[2][2] << std::endl;
            ss << "------------------------------" << std::endl;
            std::cout << ss.str() << std::endl;
          }
#endif

        ij = (cols * y) + x;
        sum = 0;
        for(int l = 0; l < classes; ++l)
        {
          prior = 0;
          edge = 0;

          prior += coupling[(cSize * l) + C[0][0]];
          prior += coupling[(cSize * l) + C[1][0]];
          prior += coupling[(cSize * l) + C[2][0]];
          prior += coupling[(cSize * l) + C[0][1]];
          prior += coupling[(cSize * l) + C[2][1]];
          prior += coupling[(cSize * l) + C[0][2]];
          prior += coupling[(cSize * l) + C[1][2]];
          prior += coupling[(cSize * l) + C[2][2]];

#if 0
            if (y == rowStart + 1 && x == colStart + 1)
            {
              std::cout << "Class: " << l << "\t prior: " << prior << std::endl;
            }
#endif

          // now check for the gradient penalty. If our current class is NOT equal
          // to the class at index[i][j] AND the value of C[i][j] does NOT equal
          // to the Number of Classes then add in the gradient penalty.
          if(data->useGradientPenalty)
          {
            if(C[0][0] != l && C[0][0] != classes)
            {
              edge += sw[(swCols * (y - 1)) + x - 1];
            }
            if(C[1][0] != l && C[1][0] != classes)
            {
              edge += ew[(ewCols * (y - 1)) + x];
            }
            if(C[2][0] != l && C[2][0] != classes)
            {
              edge += nw[(nwCols * (y - 1)) + x];
            }
            if(C[0][1] != l && C[0][1] != classes)
            {
              edge += ns[(nsCols * y) + x - 1];
            }
            if(C[2][1] != l && C[2][1] != classes)
            {
              edge += ns[(nsCols * y) + x];
            }
            if(C[0][2] != l && C[0][2] != classes)
            {
              edge += nw[(nwCols * y) + x - 1];
            }
            if(C[1][2] != l && C[1][2] != classes)
            {
              edge += ew[(ewCols * y) + x];
            }
            if(C[2][2] != l && C[2][2] != classes)
            {
              edge += sw[(swCols * y) + x];
            }
          }

          lij = (cols * rows * l) + (cols * y) + x;
          curvature_value = 0.0;
          if(data->useCurvaturePenalty)
          {
            curvature_value = data->beta_c * ccost[lij];
          }
          real_t arg = data->workingKappa * (yk[lij] - (prior) - (edge) - (curvature_value)-data->w_gamma[l]);
          post[l] = expf(arg);
          sum += post[l];
        }

        xrnd = rnd[ij];
        current = 0.0;

        for(int l = 0; l < classes; l++)
        {
          lij = (cols * rows * l) + ij;
          real_t arg = post[l] / sum;
          if((xrnd >= current) && (xrnd <= (current + arg)))
          {
            xt[ij] = l;
            probs[lij] += 1.0;
          }
          current += arg;
        }
#if 0
          Dont even THINK about using this code...
          This classifys the pixel based on the largest
          in magnitude  probability
          real_t max = 0.0;
          int maxClass = 0;
          for (int l = 0; l < classes; l++)
          {
            lij = (cols * rows * l) + ij;
            //real_t arg = post[l] / sum;
            if (probs[lij] > max)
            {
              max = probs[lij];
              maxClass = l;
            }
          }
          //Assign class based on Maximum probability
          xt[ij] = maxClass;
#endif
      }
    }
    //  std::cout << "     --" << EMMPM_getMilliSeconds() - millis << "--" << std::endl;
  }

#if EMMPM_USE_PARALLEL_ALGORITHMS
#if USE_TBB_TASK_GROUP
  void operator()() const
  {
    calc(m_RowStart, m_RowEnd, m_ColStart, m_ColEnd);
  }
#else
  void operator()(const tbb::blocked_range2d<int>& r) const
  {
    calc(r.rows().begin(), r.rows().end(), r.cols().begin(), r.cols().end());
  }
#endif
#endif

private:
#if USE_TBB_TASK_GROUP
  int m_RowStart;
  int m_RowEnd;
  int m_ColStart;
  int m_ColEnd;
#endif
  const EMMPM_Data* data;
  const real_t* yk;
  const real_t* rnd;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MPMCalculation::MPMCalculation()
: Observable()
, m_StatsDelegate(nullptr)
, m_ErrorCondition(0)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MPMCalculation::~MPMCalculation()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MPMCalculation::execute()
{
  EMMPM_Data* data = m_Data.get();

  real_t* yk;
  real_t sqrt2pi, con[EMMPM_MAX_CLASSES];
  real_t post[EMMPM_MAX_CLASSES];

  // int k, l;
  // unsigned int i, j, d;
  size_t ld, ijd, lij;
  unsigned int dims = data->dims;
  unsigned int rows = data->rows;
  unsigned int cols = data->columns;
  unsigned int classes = data->classes;

  //  int rowEnd = rows/2;
  unsigned char* y = data->y;
  real_t* probs = data->probs;
  real_t* m = data->mean;
  real_t* v = data->variance;

  char msgbuff[256];
  float totalLoops;

  int currentLoopCount;

  //  real_t curvature_value = (real_t)0.0;

  memset(post, 0, EMMPM_MAX_CLASSES * sizeof(real_t));
  memset(con, 0, EMMPM_MAX_CLASSES * sizeof(real_t));

  totalLoops = (float)(data->emIterations * data->mpmIterations + data->mpmIterations);
  memset(msgbuff, 0, 256);
  data->progress++;

  yk = (real_t*)malloc(cols * rows * classes * sizeof(real_t));

  sqrt2pi = sqrt(2.0 * M_PI);

  for(uint32_t l = 0; l < classes; l++)
  {
    con[l] = 0;
    for(uint32_t d = 0; d < dims; d++)
    {
      ld = dims * l + d;
      con[l] += -log(sqrt2pi * sqrt(v[ld]));
    }
  }

  for(uint32_t i = 0; i < rows; i++)
  {
    for(uint32_t j = 0; j < cols; j++)
    {
      for(uint32_t l = 0; l < classes; l++)
      {
        lij = (cols * rows * l) + (cols * i) + j;
        probs[lij] = 0;
        yk[lij] = con[l];
        for(uint32_t d = 0; d < dims; d++)
        {
          ld = dims * l + d;
          ijd = (dims * cols * i) + (dims * j) + d;
          yk[lij] += ((y[ijd] - m[ld]) * (y[ijd] - m[ld]) / (-2.0 * v[ld]));
        }
      }
    }
  }

  const double rangeMin = 0.0;
  const double rangeMax = 1.0;

  std::random_device randomDevice;           // Will be used to obtain a seed for the random number engine
  std::mt19937_64 generator(randomDevice()); // Standard mersenne_twister_engine seeded with rd()
  std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
  generator.seed(seed);
  std::uniform_real_distribution<> distribution(rangeMin, rangeMax);

  // Generate all the numbers up front
  size_t total = rows * cols;
  std::vector<real_t> rndNumbers(total);
  real_t* rndNumbersPtr = &(rndNumbers.front());
  for(size_t i = 0; i < total; ++i)
  {
    rndNumbersPtr[i] = distribution(generator); // Work directly with the pointer for speed.
  }

  // unsigned long long int millis = EMMPM_getMilliSeconds();
  // std::cout << "------------------------------------------------" << std::endl;
  /* Perform the MPM loops */
  for(int32_t k = 0; k < data->mpmIterations; k++)
  {

    data->currentMPMLoop = k;
    if(data->cancel)
    {
      data->progress = 100.0;
      break;
    }
    data->inside_mpm_loop = 1;

#if EMMPM_USE_PARALLEL_ALGORITHMS
    tbb::task_scheduler_init init;
    int threads = init.default_num_threads();
#if USE_TBB_TASK_GROUP
    tbb::task_group* g = new tbb::task_group;
    unsigned int rowIncrement = rows / threads;
    unsigned int rowStop = 0 + rowIncrement;
    unsigned int rowStart = 0;
    for(int t = 0; t < threads; ++t)
    {
      g->run(ParallelCalcLoop(data, yk, &(rndNumbers.front()), rowStart, rowStop, 0, cols));
      rowStart = rowStop;
      rowStop = rowStop + rowIncrement;
      if(rowStop >= rows)
      {
        rowStop = rows;
      }
    }
    g->wait();
    delete g;
#else
    tbb::parallel_for(tbb::blocked_range2d<int>(0, rows, rows / threads, 0, cols, cols), ParallelMPMLoop(data, yk, &(rndNumbers.front())), tbb::simple_partitioner());
#endif

#else
    ParallelMPMLoop pcl(data, yk, &(rndNumbers.front()));
    pcl.calc(0, rows, 0, cols);
#endif

    // std::cout << "Counter: " << counter << std::endl;
    EMMPMUtilities::ConvertXtToOutputImage(getData());

    data->currentMPMLoop = k;
    QString ss = QString("MPM Loop %1").arg(k);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

    currentLoopCount = data->mpmIterations * data->currentEMLoop + data->currentMPMLoop;
    data->progress = currentLoopCount / totalLoops * 100.0;

    // notify("", data->progress, UpdateProgressValue);
    if(m_StatsDelegate != nullptr)
    {
      m_StatsDelegate->reportProgress(m_Data);
    }
  }
#if 0
#if EMMPM_USE_PARALLEL_ALGORITHMS
  std::cout << "Parrallel MPM Loop Time to Complete:";
#else
  std::cout << "Serial MPM Loop Time To Complete: ";
#endif
  std::cout << (EMMPM_getMilliSeconds() - millis) << std::endl;
#endif

  data->inside_mpm_loop = 0;

  if(!data->cancel)
  {
    /* Normalize probabilities */
    for(uint32_t i = 0; i < data->rows; i++)
    {
      for(uint32_t j = 0; j < data->columns; j++)
      {
        for(uint32_t l = 0; l < classes; l++)
        {
          lij = (cols * rows * l) + (cols * i) + j;
          data->probs[lij] = data->probs[lij] / (real_t)data->mpmIterations;
        }
      }
    }
  }

  /* Clean Up Memory */
  free(yk);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MPMCalculation::getHumanLabel()
{
  return "MPMCalculation";
}
