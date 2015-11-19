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

#ifndef _GenericDataParser_hpp_
#define _GenericDataParser_hpp_

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

class GenericDataParser
{
  public:
    SIMPL_SHARED_POINTERS(GenericDataParser)
    SIMPL_TYPE_MACRO(GenericDataParser)

    virtual ~GenericDataParser() {}

    SIMPL_INSTANCE_STRING_PROPERTY(ColumnName)
    SIMPL_INSTANCE_PROPERTY(int, ColumnIndex)
    SIMPL_VIRTUAL_INSTANCE_PROPERTY(IDataArray::Pointer, DataArray)

    virtual IDataArray::Pointer initializeNewDataArray(size_t numTuples, const QString& name, bool allocate) { return IDataArray::NullPointer(); }

    virtual void parse(const QByteArray& token, size_t index) {}

  protected:
    GenericDataParser() {}

  private:
    GenericDataParser(const GenericDataParser&); // Copy Constructor Not Implemented
    void operator=(const GenericDataParser&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class GenericInt32Parser : public GenericDataParser
{
  public:
    SIMPL_SHARED_POINTERS(GenericInt32Parser)
    SIMPL_TYPE_MACRO(GenericInt32Parser)
    static Pointer New(Int32ArrayType::Pointer ptr, const QString& name, int colIndex)
    {
      Pointer sharedPtr (new GenericInt32Parser(ptr, name, colIndex));
      return sharedPtr;
    }

    virtual ~GenericInt32Parser()
    {
    }

    void setDataArray(IDataArray::Pointer value)
    {
      GenericDataParser::setDataArray(value);
      m_Ptr = std::dynamic_pointer_cast<Int32ArrayType>(value);
    }

    IDataArray::Pointer initializeNewDataArray(size_t numTuples, const QString& name, bool allocate)
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
    GenericInt32Parser(Int32ArrayType::Pointer ptr, const QString& name, int index)
    {
      setColumnName(name);
      setColumnIndex(index);
      setDataArray(ptr);
      m_Ptr = ptr;
    }

  private:
    Int32ArrayType::Pointer m_Ptr;

    GenericInt32Parser(const GenericInt32Parser&); // Copy Constructor Not Implemented
    void operator=(const GenericInt32Parser&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class GenericFloatParser : public GenericDataParser
{
  public:
    SIMPL_SHARED_POINTERS(GenericFloatParser)
    SIMPL_TYPE_MACRO(GenericFloatParser)
    static Pointer New(FloatArrayType::Pointer ptr, const QString& name, int colIndex)
    {
      Pointer sharedPtr (new GenericFloatParser(ptr, name, colIndex));
      return sharedPtr;
    }

    virtual ~GenericFloatParser()
    {
    }

    void setDataArray(IDataArray::Pointer value)
    {
      GenericDataParser::setDataArray(value);
      m_Ptr = std::dynamic_pointer_cast<FloatArrayType>(value);


    }

    IDataArray::Pointer initializeNewDataArray(size_t numTuples, const QString& name, bool allocate)
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
    GenericFloatParser(FloatArrayType::Pointer ptr, const QString& name, int index)
    {
      setColumnName(name);
      setColumnIndex(index);
      setDataArray(ptr);
      m_Ptr = ptr;
    }

  private:
    FloatArrayType::Pointer m_Ptr;

    GenericFloatParser(const GenericFloatParser&); // Copy Constructor Not Implemented
    void operator=(const GenericFloatParser&); // Operator '=' Not ImplementedOperator '=' Not Implemented
};


#endif /* GenericDataParser_HPP_ */

