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

#include "LogNormalOps.h"

#include "SIMPLib/Math/SIMPLibMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LogNormalOps::~LogNormalOps() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LogNormalOps::LogNormalOps() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LogNormalOps::calculateParameters(std::vector<float>& data, FloatArrayType::Pointer outputs)
{
  int err = 0;
  float avg = 0;
  float stddev = 0;
  if(data.size() > 1)
  {
    for(std::vector<float>::size_type i = 0; i < data.size(); i++)
    {
      avg = avg + log(data[i]);
    }
    avg = avg / float(data.size());
    for(std::vector<float>::size_type i = 0; i < data.size(); i++)
    {
      stddev = stddev + ((avg - log(data[i])) * (avg - log(data[i])));
    }
    stddev = stddev / float(data.size());
    stddev = sqrt(stddev);
  }
  else if(data.size() == 1)
  {
    avg = data[0];
    stddev = 0;
  }
  else
  {
    avg = 0;
    stddev = 0;
  }
  outputs->setValue(0, avg);
  outputs->setValue(1, stddev);
  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LogNormalOps::calculateCorrelatedParameters(std::vector<std::vector<float>>& data, VectorOfFloatArray outputs)
{
  int err = 0;
  float avg = 0;
  float stddev = 0;
  for(std::vector<float>::size_type i = 0; i < data.size(); i++)
  {
    avg = 0;
    stddev = 0;
    if(data[i].size() > 1)
    {
      for(std::vector<float>::size_type j = 0; j < data[i].size(); j++)
      {
        avg = avg + log(data[i][j]);
      }
      avg = avg / float(data[i].size());
      for(std::vector<float>::size_type j = 0; j < data[i].size(); j++)
      {
        stddev = stddev + ((avg - log(data[i][j])) * (avg - log(data[i][j])));
      }
      stddev = stddev / float(data[i].size());
      stddev = sqrt(stddev);
    }
    else if(data[i].size() == 1)
    {
      avg = data[i][0];
      stddev = 0;
    }
    else
    {
      avg = 0;
      stddev = 0;
    }
    outputs[0]->setValue(i, avg);
    outputs[1]->setValue(i, stddev);
  }
  return err;
}
