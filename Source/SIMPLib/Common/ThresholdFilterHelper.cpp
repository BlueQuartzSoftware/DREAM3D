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

#include "ThresholdFilterHelper.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ThresholdFilterHelper::ThresholdFilterHelper(DREAM3D::Comparison::Enumeration compType, double compValue, BoolArrayType* output) :
  comparisonOperator(compType),
  comparisonValue(compValue),
  m_Output(output)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ThresholdFilterHelper::~ThresholdFilterHelper()
{

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#define FILTER_DATA_HELPER(dType, ops, Type) \
  if (dType.compare(#Type) == 0) {\
    if (ops == DREAM3D::Comparison::Operator_LessThan) filterDataLessThan<Type>(input);\
    else if (ops == DREAM3D::Comparison::Operator_GreaterThan) filterDataGreaterThan<Type>(input);\
    else if (ops == DREAM3D::Comparison::Operator_Equal) filterDataEqualTo<Type>(input);\
    return 1;\
  }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ThresholdFilterHelper::execute(IDataArray* input, IDataArray* output)
{
  if (NULL == input)
  {
    return -1;
  }
  m_Output->initializeWithZeros();
  QString dType = input->getTypeAsString();

  FILTER_DATA_HELPER(dType, comparisonOperator, float);
  FILTER_DATA_HELPER(dType, comparisonOperator, double);

  FILTER_DATA_HELPER(dType, comparisonOperator, int8_t);
  FILTER_DATA_HELPER(dType, comparisonOperator, uint8_t);
# if CMP_TYPE_CHAR_IS_SIGNED
  FILTER_DATA_HELPER(dType, comparisonOperator, char);
#else
  FILTER_DATA_HELPER(dType, comparisonOperator, char);
#endif
  FILTER_DATA_HELPER(dType, comparisonOperator, signed char);
  FILTER_DATA_HELPER(dType, comparisonOperator, unsigned char);


  FILTER_DATA_HELPER(dType, comparisonOperator, int16_t);
  FILTER_DATA_HELPER(dType, comparisonOperator, short);
  FILTER_DATA_HELPER(dType, comparisonOperator, signed short);
  FILTER_DATA_HELPER(dType, comparisonOperator, uint16_t);
  FILTER_DATA_HELPER(dType, comparisonOperator, unsigned short);


  FILTER_DATA_HELPER(dType, comparisonOperator, int32_t);
  FILTER_DATA_HELPER(dType, comparisonOperator, uint32_t);
#if (CMP_SIZEOF_INT == 4)
  FILTER_DATA_HELPER(dType, comparisonOperator, int);
  FILTER_DATA_HELPER(dType, comparisonOperator, signed int);
  FILTER_DATA_HELPER(dType, comparisonOperator, unsigned int);
#endif


#if (CMP_SIZEOF_LONG == 4)
  FILTER_DATA_HELPER(dType, comparisonOperator, long int);
  FILTER_DATA_HELPER(dType, comparisonOperator, signed long int);
  FILTER_DATA_HELPER(dType, comparisonOperator, unsigned long int);
#elif (CMP_SIZEOF_LONG == 8)
  FILTER_DATA_HELPER(dType, comparisonOperator, long int);
  FILTER_DATA_HELPER(dType, comparisonOperator, signed long int);
  FILTER_DATA_HELPER(dType, comparisonOperator, unsigned long int);
#endif


#if (CMP_SIZEOF_LONG_LONG == 8)
  FILTER_DATA_HELPER(dType, comparisonOperator, long long int);
  FILTER_DATA_HELPER(dType, comparisonOperator, signed long long int);
  FILTER_DATA_HELPER(dType, comparisonOperator, unsigned long long int);
#endif
  FILTER_DATA_HELPER(dType, comparisonOperator, int64_t);
  FILTER_DATA_HELPER(dType, comparisonOperator, uint64_t);

  FILTER_DATA_HELPER(dType, comparisonOperator, bool);

  return -1;
}
