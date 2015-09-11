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


#ifndef _QUALITYMETRICFILTER_H_
#define _QUALITYMETRICFILTER_H_

#include "EbsdLib/EbsdConstants.h"


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"


#include "SIMPLib/DataArrays/DataArray.hpp"

namespace Detail
{
  namespace QualityMetricFilter
  {
    template<typename T>
    bool lessThan(T lhs, T rhs) { return (lhs < rhs); }

    template<typename T>
    bool greaterThan(T lhs, T rhs) { return (lhs > rhs); }

    template<typename T>
    bool equalTo(T lhs, T rhs) { return (lhs == rhs); }
  }
}


/*
 *
 */
class SIMPLib_EXPORT QualityMetricFilter
{
  public:
    SIMPL_SHARED_POINTERS(QualityMetricFilter)
    SIMPL_TYPE_MACRO(QualityMetricFilter)
    SIMPL_STATIC_NEW_MACRO(QualityMetricFilter)

    virtual ~QualityMetricFilter();

    SIMPL_INSTANCE_STRING_PROPERTY(FeatureName)
    SIMPL_INSTANCE_PROPERTY(float, FeatureValue)
    SIMPL_INSTANCE_STRING_PROPERTY(FeatureOperator)
    SIMPL_INSTANCE_PROPERTY(int, FeaturePhaseNumber)

    SIMPL_INSTANCE_PROPERTY(void*, Input)
    SIMPL_INSTANCE_PROPERTY(int32_t*, InputPhaseData)
    SIMPL_INSTANCE_PROPERTY(size_t, NumValues)
    SIMPL_INSTANCE_PROPERTY(Ebsd::NumType, DataType)
    SIMPL_INSTANCE_PROPERTY(DataArray<bool>::Pointer, Output)

    int filter();

  protected:
    QualityMetricFilter();

  private:

    template<typename T>
    void filterDataLessThan()
    {
      T* data = static_cast<T*>(m_Input);
      for (size_t i = 0; i < m_NumValues; ++i)
      {
        bool b = ((data[i] < m_FeatureValue || m_InputPhaseData[i] != m_FeaturePhaseNumber) && m_InputPhaseData[i] != 0);
        m_Output->SetValue(i, b);
      }
    }

    template<typename T>
    void filterDataGreaterThan()
    {
      T* data = static_cast<T*>(m_Input);
      for (size_t i = 0; i < m_NumValues; ++i)
      {
        bool b = ((data[i] > m_FeatureValue || m_InputPhaseData[i] != m_FeaturePhaseNumber) && m_InputPhaseData[i] != 0);
        m_Output->SetValue(i, b);
      }
    }

    template<typename T>
    void filterDataEqualTo()
    {
      T* data = static_cast<T*>(m_Input);
      for (size_t i = 0; i < m_NumValues; ++i)
      {
        bool b = ((data[i] == m_FeatureValue || m_InputPhaseData[i] != m_FeaturePhaseNumber) && m_InputPhaseData[i] != 0);
        m_Output->SetValue(i, b);
      }
    }



    QualityMetricFilter(const QualityMetricFilter&); // Copy Constructor Not Implemented
    void operator=(const QualityMetricFilter&); // Operator '=' Not Implemented
};

#endif /* _QUALITYMETRICFILTER_H_ */

