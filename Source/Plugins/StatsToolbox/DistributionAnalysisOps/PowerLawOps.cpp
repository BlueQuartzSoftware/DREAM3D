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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "PowerLawOps.h"
#include <limits>
#include <numeric>

#include "SIMPLib/Math/SIMPLibMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PowerLawOps::~PowerLawOps() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PowerLawOps::PowerLawOps() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PowerLawOps::calculateParameters(std::vector<float>& data, FloatArrayType::Pointer outputs)
{
  int err = 0;
  float alpha = 0;
  float min = std::numeric_limits<float>::max();
  if(data.size() > 1)
  {
    for(std::vector<float>::size_type i = 0; i < data.size(); i++)
    {
      if(data[i] < min)
      {
        min = data[i];
      }
    }
    for(std::vector<float>::size_type i = 0; i < data.size(); i++)
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
  outputs->setValue(0, alpha);
  outputs->setValue(1, min);
  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PowerLawOps::calculateCorrelatedParameters(std::vector<std::vector<float>>& data, VectorOfFloatArray outputs)
{
  int err = 0;
  float alpha = 0;
  float min;
  for(std::vector<float>::size_type i = 0; i < data.size(); i++)
  {
    if(data[i].size() > 1)
    {
      min = std::numeric_limits<float>::max();
      for(std::vector<float>::size_type j = 0; j < data[i].size(); j++)
      {
        if(data[i][j] < min)
        {
          min = data[i][j];
        }
      }
      for(std::vector<float>::size_type j = 0; j < data[i].size(); j++)
      {
        alpha = alpha + log(data[i][j] / min);
      }
      if(alpha != 0.0f)
      {
        alpha = 1.0f / alpha;
      }
      alpha = 1.0f + (alpha * data[i].size());
    }
    else
    {
      min = 0;
      alpha = 0;
    }
    outputs[0]->setValue(i, alpha);
    outputs[1]->setValue(i, min);
  }
  return err;
}

// -----------------------------------------------------------------------------
PowerLawOps::Pointer PowerLawOps::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
QString PowerLawOps::getNameOfClass() const
{
  return QString("PowerLawOps");
}

// -----------------------------------------------------------------------------
QString PowerLawOps::ClassName()
{
  return QString("PowerLawOps");
}

// -----------------------------------------------------------------------------
PowerLawOps::Pointer PowerLawOps::New()
{
  Pointer sharedPtr(new(PowerLawOps));
  return sharedPtr;
}
