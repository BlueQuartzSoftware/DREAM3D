/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "PowerLawOps.h"
#include <limits>
#include <numeric>

#include "DREAM3DLib/Math/DREAM3DMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PowerLawOps::~PowerLawOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PowerLawOps::PowerLawOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PowerLawOps::calculateParameters(QVector<float>& data, FloatArrayType::Pointer outputs)
{
  int err = 0;
  float alpha = 0;
  float min = std::numeric_limits<float>::max();
  if(data.size() > 1)
  {
    for(size_t i = 0; i < data.size(); i++)
    {
      if (data[i] < min) { min = data[i]; }
    }
    for(size_t i = 0; i < data.size(); i++)
    {
      alpha = alpha + log(data[i] / min);
    }
    alpha = 1.0f / alpha;
    alpha = 1 + (alpha * data.size());
  }
  else
  {
    min = 0;
    alpha = 0;
  }
  outputs->SetValue(0, alpha);
  outputs->SetValue(1, min);
  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PowerLawOps::calculateCorrelatedParameters(QVector<QVector<float> >& data, VectorOfFloatArray outputs)
{
  int err = 0;
  float alpha = 0;
  float min;
  for(size_t i = 0; i < data.size(); i++)
  {
    if(data[i].size() > 1)
    {
      min = std::numeric_limits<float>::max();
      for(size_t j = 0; j < data[i].size(); j++)
      {
        if (data[i][j] < min) { min = data[i][j]; }
      }
      for(size_t j = 0; j < data[i].size(); j++)
      {
        alpha = alpha + log(data[i][j] / min);
      }
      if(alpha != 0.0f) { alpha = 1.0f / alpha; }
      alpha = 1.0f + (alpha * data[i].size());
    }
    else
    {
      min = 0;
      alpha = 0;
    }
    outputs[0]->SetValue(i, alpha);
    outputs[1]->SetValue(i, min);
  }
  return err;
}

