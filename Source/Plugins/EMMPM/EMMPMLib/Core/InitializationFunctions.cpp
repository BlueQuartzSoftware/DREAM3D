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

#include "InitializationFunctions.h"
//-- C Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//-- Boost Includes
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>


//-- EMMMPM Lib Includes
#include "EMMPMLib/Core/EMMPM.h"
#include "EMMPMLib/Common/MSVCDefines.h"
#include "EMMPMLib/Common/EMMPM_Math.h"
#include "EMMPMLib/Common/EMTime.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InitializationFunction::InitializationFunction()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InitializationFunction::~InitializationFunction()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializationFunction::initialize(EMMPM_Data::Pointer data)
{

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BasicInitialization::BasicInitialization()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BasicInitialization::~BasicInitialization()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BasicInitialization::initialize(EMMPM_Data::Pointer data)
{
  //FIXME: This needs to be adapted for vector images (dims > 1)
  unsigned int i, k, l;
  real_t mu, sigma;
  char msgbuff[256];
  unsigned int rows = data->rows;
  unsigned int cols = data->columns;
  unsigned int classes = data->classes;
  unsigned char* y = data->y;
  size_t total;

  total = rows * cols;

  memset(msgbuff, 0, 256);

  /* Initialization of parameter estimation */
  mu = 0;
  sigma = 0;
  for (i = 0; i < total; i++)
  {
    mu += y[i];
  }

  mu /= (rows * cols);

  for (i = 0; i < total; i++)
  {
    sigma += (y[i] - mu) * (y[i] - mu);
  }

  sigma /= (rows * cols);
  sigma = sqrt((real_t)sigma);

  if (classes % 2 == 0)
  {
    for (k = 0; k < classes / 2; k++)
    {
      data->mean[classes / 2 + k] = mu + (k + 1) * sigma / 2;
      data->mean[classes / 2 - 1 - k] = mu - (k + 1) * sigma / 2;
    }
  }
  else
  {
    data->mean[classes / 2] = mu;
    for (k = 0; k < classes / 2; k++)
    {
      data->mean[classes / 2 + 1 + k] = mu + (k + 1) * sigma / 2;
      data->mean[classes / 2 - 1 - k] = mu - (k + 1) * sigma / 2;
    }
  }

  for (l = 0; l < classes; l++)
  {
    data->variance[l] = 20.0;
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UserDefinedAreasInitialization::UserDefinedAreasInitialization()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UserDefinedAreasInitialization::~UserDefinedAreasInitialization()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UserDefinedAreasInitialization::initialize(EMMPM_Data::Pointer data)
{

  unsigned int i, j;
  size_t index;
  int c, l;
  real_t mu = 0.0;
  unsigned int cols = data->columns;
  char msgbuff[256];
  unsigned char* y = data->y;

  if (data->dims != 1)
  {
    printf("User Defined Initialization ONLY works with GrayScale images and not vector images.\n  %s(%d)", __FILE__, __LINE__);
    exit(1);
  }

  memset(msgbuff, 0, 256);

  for (c = 0; c < data->classes; c++)
  {
    int x1 = data->initCoords[c][0];
    int y1 = data->initCoords[c][1];
    int x2 = data->initCoords[c][2];
    int y2 = data->initCoords[c][3];
    mu = 0;
    snprintf(msgbuff, 256, "m[%d] Coords: %d %d %d %d", c, x1, y1, x2, y2);
    for (i = data->initCoords[c][1]; i < data->initCoords[c][3]; i++)
    {
      for (j = data->initCoords[c][0]; j < data->initCoords[c][2]; j++)
      {
        index = (cols * i) + j;
        mu += y[index];
      }
    }

    mu /= (y2 - y1) * (x2 - x1);
    data->mean[c] = mu;
    snprintf(msgbuff, 256, "m[%d]=%f", c, mu);
  }

  for (l = 0; l < data->classes; l++)
  {
    data->variance[l] = 20.0;
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
XtArrayInitialization::XtArrayInitialization()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
XtArrayInitialization::~XtArrayInitialization()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void XtArrayInitialization::initialize(EMMPM_Data::Pointer data)
{
  size_t total;

  total = data->rows * data->columns;

  const float rangeMin = 0.0f;
  const float rangeMax = 1.0f;
  typedef boost::uniform_real<> NumberDistribution;
  typedef boost::mt19937 RandomNumberGenerator;
  typedef boost::variate_generator<RandomNumberGenerator&,
          NumberDistribution> Generator;

  NumberDistribution distribution(rangeMin, rangeMax);
  RandomNumberGenerator generator;
  Generator numberGenerator(generator, distribution);
  generator.seed(EMMPM_getMilliSeconds()); // seed with the current time

  /* Initialize classification of each pixel randomly with a uniform disribution */
  for (size_t i = 0; i < total; i++)
  {
    data->xt[i] = numberGenerator() * data->classes;
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GradientVariablesInitialization::GradientVariablesInitialization()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GradientVariablesInitialization::~GradientVariablesInitialization()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GradientVariablesInitialization::initialize(EMMPM_Data::Pointer data)
{
  size_t ijd, ij, ijd1;

  size_t nsCols = data->columns - 1;
  size_t nsRows = data->rows;
  size_t ewCols = data->columns;
  size_t ewRows = data->rows - 1;
  size_t swCols = data->columns - 1;
  size_t swRows = data->rows - 1;
  size_t nwCols = data->columns - 1;
  size_t nwRows = data->rows - 1;

  int dims = data->dims;
  real_t x;

  /* Allocate for edge images */

  data->ns = (real_t*)malloc(nsCols * nsRows * sizeof(real_t));
  if (data->ns == nullptr) { return; }
  data->ew = (real_t*)malloc(ewCols * ewRows * sizeof(real_t));
  if (data->ew == nullptr) { return; }
  data->sw = (real_t*)malloc(swCols * swRows * sizeof(real_t));
  if (data->sw == nullptr) { return; }
  data->nw = (real_t*)malloc(nwCols * nwRows * sizeof(real_t));
  if (data->nw == nullptr) { return; }

  /* Do edge detection for gradient penalty*/
  for (uint32_t i = 0; i < data->rows; i++)
  {
    for (uint32_t j = 0; j < nwCols; j++)
    {
      x = 0;
      for (int32_t d = 0; d < dims; d++)
      {
        ijd = (dims * nwCols * i) + (dims * j) + d;
        ijd1 = (dims * nwCols * (i)) + (dims * (j + 1)) + d;
        x += (data->y[ijd] - data->y[ijd1]) * (data->y[ijd] - data->y[ijd1]);
      }
      ij = (nwCols * i) + j;
      data->ns[ij] = data->beta_e * atan((10 - sqrt(x)) / 5);
    }
  }
  for (uint32_t i = 0; i < nwRows; i++)
  {
    for (uint32_t j = 0; j < data->columns; j++)
    {
      x = 0;
      for (int32_t d = 0; d < dims; d++)
      {
        ijd = (dims * data->columns * i) + (dims * j) + d;
        ijd1 = (dims * data->columns * (i + 1)) + (dims * (j)) + d;
        x += (data->y[ijd] - data->y[ijd1]) * (data->y[ijd] - data->y[ijd1]);
      }
      ij = (data->columns * i) + j;
      data->ew[ij] = data->beta_e * atan((10 - sqrt(x)) / 5);
    }
  }
  nwCols = data->columns - 1;
  nwRows = data->rows - 1;
  for (uint32_t i = 0; i < nwRows; i++)
  {
    for (uint32_t j = 0; j < nwCols; j++)
    {
      x = 0;
      for (uint32_t d = 0; d < data->dims; d++)
      {
        ijd = (dims * data->columns * i) + (dims * j) + d;
        ijd1 = (dims * data->columns * (i + 1)) + (dims * (j + 1)) + d;
        x += (data->y[ijd] - data->y[ijd1]) * (data->y[ijd] - data->y[ijd1]);
      }
      ij = (nwCols * i) + j;
      data->sw[ij] = data->beta_e * atan((10 - sqrt(0.5 * x)) / 5);
      x = 0;
      for (uint32_t d = 0; d < data->dims; d++)
      {
        ijd = (dims * data->columns * (i + 1)) + (dims * (j)) + d;
        ijd1 = (dims * data->columns * (i)) + (dims * (j + 1)) + d;
        x += (data->y[ijd] - data->y[ijd1]) * (data->y[ijd] - data->y[ijd1]);
      }
      ij = (nwCols * i) + j;
      data->nw[ij] = data->beta_e * atan((10 - sqrt(0.5 * x)) / 5);
    }
  }

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CurvatureInitialization::CurvatureInitialization()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CurvatureInitialization::~CurvatureInitialization()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CurvatureInitialization::initialize(EMMPM_Data::Pointer data)
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CurvatureInitialization::initCurvatureVariables(EMMPM_Data::Pointer data)
{
  int l, lij;
  unsigned int i, j;

  data->ccost = (real_t*)malloc(data->classes * data->rows * data->columns * sizeof(real_t));
  if (data->ccost == nullptr) { return; }

  /* Initialize Curve Costs to zero */
  for (l = 0; l < data->classes; l++)
  {
    for (i = 0; i < data->rows; i++)
    {
      for (j = 0; j < data->columns; j++)
      {
        {
          lij = (data->columns * data->rows * l) + (data->columns * i) + j;
          data->ccost[lij] = 0;
        }
      }
    }
  }
}

