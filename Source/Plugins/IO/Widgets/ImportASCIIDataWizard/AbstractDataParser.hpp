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
#ifndef _AbstractDataParser_hpp_
#define _AbstractDataParser_hpp_

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "ParserFunctors.hpp"

class AbstractDataParser
{
  public:
    SIMPL_SHARED_POINTERS(AbstractDataParser)
    SIMPL_TYPE_MACRO(AbstractDataParser)

    virtual ~AbstractDataParser() {}

    SIMPL_INSTANCE_STRING_PROPERTY(ColumnName)
    SIMPL_INSTANCE_PROPERTY(int, ColumnIndex)
    SIMPL_VIRTUAL_INSTANCE_PROPERTY(IDataArray::Pointer, DataArray)

    virtual IDataArray::Pointer initializeNewDataArray(size_t numTuples, const QString &name, bool allocate){ return IDataArray::NullPointer(); }

    virtual ParserFunctor::ErrorObject parse(const QString& token, size_t index) = 0;

  protected:
    AbstractDataParser() {}

  private:
    AbstractDataParser(const AbstractDataParser&); // Copy Constructor Not Implemented
    void operator=(const AbstractDataParser&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T, class F>
class Parser : public AbstractDataParser
{
public:
  typedef Parser<T, F> SelfType;

  SIMPL_SHARED_POINTERS(SelfType)
  SIMPL_TYPE_MACRO(SelfType)

  static Pointer New(typename DataArray<T>::Pointer ptr, const QString& name, int colIndex)
  {
    Pointer sharedPtr(new Parser(ptr, name, colIndex));
    return sharedPtr;
  }

  virtual ~Parser()
  {
  }

  static IDataArray::Pointer InitializeNewDataArray(size_t numTuples, const QString &name, bool allocate)
  {
    typename DataArray<T>::Pointer array = DataArray<T>::CreateArray(numTuples, name, allocate);
    if (allocate) { array->initializeWithZeros(); }
    return array;
  }

  void setDataArray(IDataArray::Pointer value)
  {
    AbstractDataParser::setDataArray(value);
    m_Ptr = std::dynamic_pointer_cast<DataArray<T> >(value);
  }

  virtual ParserFunctor::ErrorObject parse(const QString& token, size_t index)
  {
    ParserFunctor::ErrorObject obj;
    (*m_Ptr)[index] = F()(token, obj);
    return obj;
  }

protected:
  Parser(typename DataArray<T>::Pointer ptr, const QString& name, int index)
  {
    setColumnName(name);
    setColumnIndex(index);
    setDataArray(ptr);
    m_Ptr = ptr;
  }

private:
  typename DataArray<T>::Pointer m_Ptr;

  Parser(const Parser&); // Copy Constructor Not Implemented
  void operator=(const Parser&); // Operator '=' Not Implemented
};

typedef Parser<int8_t, Int8Functor>  Int8ParserType;
typedef Parser<uint8_t, UInt8Functor>  UInt8ParserType;

typedef Parser<int16_t, Int16Functor>  Int16ParserType;
typedef Parser<uint16_t, UInt16Functor>  UInt16ParserType;

typedef Parser<int32_t, Int32Functor>  Int32ParserType;
typedef Parser<uint32_t, UInt32Functor>  UInt32ParserType;

typedef Parser<int64_t, Int64Functor>  Int64ParserType;
typedef Parser<uint64_t, UInt64Functor>  UInt64ParserType;

typedef Parser<float, FloatFunctor>  FloatParserType;
typedef Parser<double, DoubleFunctor>  DoubleParserType;

#endif /* DATAPARSER_HPP_ */

