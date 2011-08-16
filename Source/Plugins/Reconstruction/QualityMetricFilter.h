/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _QUALITYMETRICFILTER_H_
#define _QUALITYMETRICFILTER_H_


#include "MXA/Common/MXASetGetMacros.h"
#include "EbsdLib/EbsdConstants.h"
#include "DREAM3D/Common/AIMArray.hpp"

namespace Detail {
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
class QualityMetricFilter
{
  public:
    MXA_SHARED_POINTERS(QualityMetricFilter)
    MXA_TYPE_MACRO(QualityMetricFilter)
    MXA_STATIC_NEW_MACRO(QualityMetricFilter)

    virtual ~QualityMetricFilter();

    MXA_INSTANCE_STRING_PROPERTY(FieldName)
    MXA_INSTANCE_PROPERTY(float, FieldValue)
    MXA_INSTANCE_STRING_PROPERTY(FieldOperator)

    MXA_INSTANCE_PROPERTY(void*, Input)
    MXA_INSTANCE_PROPERTY(size_t, NumValues)
    MXA_INSTANCE_PROPERTY(Ebsd::NumType, DataType);
    MXA_INSTANCE_PROPERTY(AIMArray<bool>::Pointer, Output)

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
        bool b = (data[i] < m_FieldValue);
        m_Output->SetValue(i, b);
      }
    }

    template<typename T>
    void filterDataGreaterThan()
    {
      T* data = static_cast<T*>(m_Input);
      for (size_t i = 0; i < m_NumValues; ++i)
      {
        bool b = (data[i] > m_FieldValue);
        m_Output->SetValue(i, b);
      }
    }

    template<typename T>
    void filterDataEqualTo()
    {
      T* data = static_cast<T*>(m_Input);
      for (size_t i = 0; i < m_NumValues; ++i)
      {
        bool b = (data[i] == m_FieldValue);
        m_Output->SetValue(i, b);
      }
    }



    QualityMetricFilter(const QualityMetricFilter&); // Copy Constructor Not Implemented
    void operator=(const QualityMetricFilter&); // Operator '=' Not Implemented
};

#endif /* _QUALITYMETRICFILTER_H_ */
