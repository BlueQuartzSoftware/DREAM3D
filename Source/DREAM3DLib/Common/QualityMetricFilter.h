/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#ifndef _QUALITYMETRICFILTER_H_
#define _QUALITYMETRICFILTER_H_

#include "EbsdLib/EbsdConstants.h"


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"


#include "DREAM3DLib/Common/DataArray.hpp"

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
class DREAM3DLib_EXPORT QualityMetricFilter
{
  public:
    DREAM3D_SHARED_POINTERS(QualityMetricFilter)
    DREAM3D_TYPE_MACRO(QualityMetricFilter)
    DREAM3D_STATIC_NEW_MACRO(QualityMetricFilter)

    virtual ~QualityMetricFilter();

    DREAM3D_INSTANCE_STRING_PROPERTY(FieldName)
    DREAM3D_INSTANCE_PROPERTY(float, FieldValue)
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldOperator)
    DREAM3D_INSTANCE_PROPERTY(int, FieldPhaseNumber)

    DREAM3D_INSTANCE_PROPERTY(void*, Input)
    DREAM3D_INSTANCE_PROPERTY(int32_t*, InputPhaseData)
    DREAM3D_INSTANCE_PROPERTY(size_t, NumValues)
    DREAM3D_INSTANCE_PROPERTY(Ebsd::NumType, DataType);
    DREAM3D_INSTANCE_PROPERTY(DataArray<bool>::Pointer, Output)

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
        bool b = ((data[i] < m_FieldValue || m_InputPhaseData[i] != m_FieldPhaseNumber) && m_InputPhaseData[i] != 0);
        m_Output->SetValue(i, b);
      }
    }

    template<typename T>
    void filterDataGreaterThan()
    {
      T* data = static_cast<T*>(m_Input);
      for (size_t i = 0; i < m_NumValues; ++i)
      {
        bool b = ((data[i] > m_FieldValue || m_InputPhaseData[i] != m_FieldPhaseNumber) && m_InputPhaseData[i] != 0);
        m_Output->SetValue(i, b);
      }
    }

    template<typename T>
    void filterDataEqualTo()
    {
      T* data = static_cast<T*>(m_Input);
      for (size_t i = 0; i < m_NumValues; ++i)
      {
        bool b = ((data[i] == m_FieldValue || m_InputPhaseData[i] != m_FieldPhaseNumber) && m_InputPhaseData[i] != 0);
        m_Output->SetValue(i, b);
      }
    }



    QualityMetricFilter(const QualityMetricFilter&); // Copy Constructor Not Implemented
    void operator=(const QualityMetricFilter&); // Operator '=' Not Implemented
};

#endif /* _QUALITYMETRICFILTER_H_ */
