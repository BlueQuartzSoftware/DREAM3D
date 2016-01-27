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

#ifndef _ThresholdFilterHelperHelper_H_
#define _ThresholdFilterHelperHelper_H_


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/IDataArrayFilter.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

/**
 * @brief The ThresholdFilterHelper class
 */
class SIMPLib_EXPORT ThresholdFilterHelper : public IDataArrayFilter
{
  public:
    ThresholdFilterHelper(DREAM3D::Comparison::Enumeration compType,
                          double compValue,
                          BoolArrayType* output);

    virtual ~ThresholdFilterHelper();

    /**
     *
     */
    template<typename T>
    void filterDataLessThan(IDataArray* m_Input)
    {
      size_t m_NumValues = m_Input->getNumberOfTuples();
      T v = static_cast<T>(comparisonValue);
      T* data = IDataArray::SafeReinterpretCast<IDataArray*, DataArray<T>*, T*>(m_Input);
      for (size_t i = 0; i < m_NumValues; ++i)
      {
        bool b = (data[i] < v);
        m_Output->setValue(i, b);
      }
    }

    /**
     *
     */
    template<typename T>
    void filterDataGreaterThan(IDataArray* m_Input)
    {
      size_t m_NumValues = m_Input->getNumberOfTuples();
      T v = static_cast<T>(comparisonValue);
      T* data = IDataArray::SafeReinterpretCast<IDataArray*, DataArray<T>*, T*>(m_Input);
      for (size_t i = 0; i < m_NumValues; ++i)
      {
        bool b = (data[i] > v);
        m_Output->setValue(i, b);
      }
    }

    /**
     *
     */
    template<typename T>
    void filterDataEqualTo(IDataArray* m_Input)
    {
      size_t m_NumValues = m_Input->getNumberOfTuples();
      T v = static_cast<T>(comparisonValue);
      T* data = IDataArray::SafeReinterpretCast<IDataArray*, DataArray<T>*, T*>(m_Input);
      for (size_t i = 0; i < m_NumValues; ++i)
      {
        bool b = (data[i] == v);
        m_Output->setValue(i, b);
      }
    }


    /**
    * @brief execute
    * @param input
    * @param output
    * @return
    */
    int execute(IDataArray* input, IDataArray* output);

  private:
    DREAM3D::Comparison::Enumeration comparisonOperator;
    double comparisonValue;
    BoolArrayType* m_Output;


    ThresholdFilterHelper(const ThresholdFilterHelper&); // Copy Constructor Not Implemented
    void operator=(const ThresholdFilterHelper&); // Operator '=' Not Implemented
};


#endif /* _ThresholdFilterHelperHelper_H_ */

