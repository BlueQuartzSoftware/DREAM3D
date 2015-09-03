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


#include "QualityMetricFilter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QualityMetricFilter::QualityMetricFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QualityMetricFilter::~QualityMetricFilter()
{
}




#define FILTER_DATA(m_msgType) \
  if (m_FeatureOperator.compare("<") == 0) filterDataLessThan<m_msgType>();\
  else if (m_FeatureOperator.compare(">") == 0) filterDataGreaterThan<m_msgType>();\
  else if (m_FeatureOperator.compare("=") == 0) filterDataEqualTo<m_msgType>();


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QualityMetricFilter::filter()
{
  int err = 0;
  if (m_Output.get() == NULL)
  {
    m_Output = DataArray<bool>::CreateArray(0, getFeatureName());
  }
  m_Output->Resize(m_NumValues);
  m_Output->initializeWithZeros();

  if (m_DataType == Ebsd::Int8)
  {
    FILTER_DATA(int8_t);
  }
  if (m_DataType == Ebsd::UInt8)
  {
    FILTER_DATA(uint8_t);
  }
  if (m_DataType == Ebsd::Int16)
  {
    FILTER_DATA(int16_t);
  }
  if (m_DataType == Ebsd::UInt16)
  {
    FILTER_DATA(uint16_t);
  }
  if (m_DataType == Ebsd::Int32)
  {
    FILTER_DATA(int32_t);
  }
  if (m_DataType == Ebsd::UInt32)
  {
    FILTER_DATA(uint32_t);
  }
  if (m_DataType == Ebsd::Int64)
  {
    FILTER_DATA(int64_t);
  }
  if (m_DataType == Ebsd::UInt64)
  {
    FILTER_DATA(uint64_t);
  }
  if (m_DataType == Ebsd::Float)
  {
    FILTER_DATA(float);
  }
  if (m_DataType == Ebsd::Double)
  {
    FILTER_DATA(double);
  }


  return err;
}
