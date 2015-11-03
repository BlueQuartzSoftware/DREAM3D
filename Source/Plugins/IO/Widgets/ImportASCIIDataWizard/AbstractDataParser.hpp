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

namespace IO
{

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

    virtual void parse(const QByteArray& token, size_t index) {}

  protected:
    AbstractDataParser() {}

  private:
    AbstractDataParser(const AbstractDataParser&); // Copy Constructor Not Implemented
    void operator=(const AbstractDataParser&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Int32Parser : public AbstractDataParser
{
  public:
    SIMPL_SHARED_POINTERS(Int32Parser)
    SIMPL_TYPE_MACRO(Int32Parser)
    static Pointer New(Int32ArrayType::Pointer ptr, const QString& name, int colIndex)
    {
      Pointer sharedPtr (new Int32Parser(ptr, name, colIndex));
      return sharedPtr;
    }

    virtual ~Int32Parser()
    {
    }

    void setDataArray(IDataArray::Pointer value)
    {
      AbstractDataParser::setDataArray(value);
      m_Ptr = boost::dynamic_pointer_cast<Int32ArrayType>(value);
    }

    IDataArray::Pointer initializeNewDataArray(size_t numTuples, const QString &name, bool allocate)
    {
      Int32ArrayType::Pointer array = Int32ArrayType::CreateArray(numTuples, name, allocate);
      if (allocate) { array->initializeWithZeros(); }
      return array;
    }

    virtual void parse(const QByteArray& token, size_t index)
    {
      bool ok = false;
      (*m_Ptr)[index] = token.toInt(&ok, 10);
    }

  protected:
    Int32Parser(Int32ArrayType::Pointer ptr, const QString& name, int index)
    {
      setColumnName(name);
      setColumnIndex(index);
      setDataArray(ptr);
      m_Ptr = ptr;
    }

  private:
    Int32ArrayType::Pointer m_Ptr;

    Int32Parser(const Int32Parser&); // Copy Constructor Not Implemented
    void operator=(const Int32Parser&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class FloatParser : public AbstractDataParser
{
  public:
    SIMPL_SHARED_POINTERS(FloatParser)
    SIMPL_TYPE_MACRO(FloatParser)
    static Pointer New(FloatArrayType::Pointer ptr, const QString& name, int colIndex)
    {
      Pointer sharedPtr (new FloatParser(ptr, name, colIndex));
      return sharedPtr;
    }

    virtual ~FloatParser()
    {
    }

    void setDataArray(IDataArray::Pointer value)
    {
      AbstractDataParser::setDataArray(value);
      m_Ptr = boost::dynamic_pointer_cast<FloatArrayType>(value);
    }

    IDataArray::Pointer initializeNewDataArray(size_t numTuples, const QString &name, bool allocate)
    {
      FloatArrayType::Pointer array = FloatArrayType::CreateArray(numTuples, name, allocate);
      if (allocate) { array->initializeWithZeros(); }
      return array;
    }

    virtual void parse(const QByteArray& token, size_t index)
    {
      bool ok = false;
      (*m_Ptr)[index] = token.toFloat(&ok);
    }

  protected:
    FloatParser(FloatArrayType::Pointer ptr, const QString& name, int index)
    {
      setColumnName(name);
      setColumnIndex(index);
      setDataArray(ptr);
      m_Ptr = ptr;
    }

  private:
    FloatArrayType::Pointer m_Ptr;

    FloatParser(const FloatParser&); // Copy Constructor Not Implemented
    void operator=(const FloatParser&); // Operator '=' Not ImplementedOperator '=' Not Implemented
};

}

#endif /* DATAPARSER_HPP_ */

