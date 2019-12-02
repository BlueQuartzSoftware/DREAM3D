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

#pragma once

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/DataArrays/DataArray.hpp"

class GenericDataParser
{
  public:
    using Self = GenericDataParser;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();
    Pointer NullPointer()
    {
      return Pointer(static_cast<Self*>(nullptr));
    }

    /**
     * @brief Returns the name of the class for GenericDataParser
     */
    /**
     * @brief Returns the name of the class for GenericDataParser
     */
    QString getNameOfClass() const
    {
      return QString("GenericDataParser");
    }

    /**
     * @brief Returns the name of the class for GenericDataParser
     */
    QString ClassName()
    {
      return QString("GenericDataParser");
    }

    virtual ~GenericDataParser() = default;

    // -----------------------------------------------------------------------------
    void GenericDataParser::setColumnName(const QString& value)
    {
      m_ColumnName = value;
    }

    // -----------------------------------------------------------------------------
    QString GenericDataParser::getColumnName() const
    {
      return m_ColumnName;
    }

    // -----------------------------------------------------------------------------
    void setColumnIndex(int value)
    {
      m_ColumnIndex = value;
    }

    // -----------------------------------------------------------------------------
    int getColumnIndex() const
    {
      return m_ColumnIndex;
    }

    virtual void setDataArray(IDataArray::Pointer value) { m_DataArray = value; }
    virtual IDataArray::Pointer getDataArray() { return m_DataArray; }
    
    virtual IDataArray::Pointer initializeNewDataArray(size_t numTuples, const QString& name, bool allocate) { return IDataArray::NullPointer(); }

    virtual void parse(const QByteArray& token, size_t index) {}

  protected:
    GenericDataParser() {}

  private:
    IDataArray::Pointer m_DataArray;
    
    GenericDataParser(const GenericDataParser&); // Copy Constructor Not Implemented
    void operator=(const GenericDataParser&);    // Move assignment Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class GenericInt32Parser : public GenericDataParser
{
  public:
    using Self = GenericInt32Parser;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();
    Pointer NullPointer()
    {
      return Pointer(static_cast<Self*>(nullptr));
    }

    /**
     * @brief Returns the name of the class for GenericInt32Parser
     */
    /**
     * @brief Returns the name of the class for GenericInt32Parser
     */
    QString getNameOfClass() const
    {
      return QString("GenericInt32Parser");
    }

    /**
     * @brief Returns the name of the class for GenericInt32Parser
     */
    QString ClassName()
    {
      return QString("GenericInt32Parser");
    }

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
    void operator=(const GenericInt32Parser&);     // Move assignment Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class GenericFloatParser : public GenericDataParser
{
  public:
    using Self = GenericFloatParser;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();
    Pointer NullPointer()
    {
      return Pointer(static_cast<Self*>(nullptr));
    }

    /**
     * @brief Returns the name of the class for GenericFloatParser
     */
    /**
     * @brief Returns the name of the class for GenericFloatParser
     */
    QString getNameOfClass() const
    {
      return QString("GenericFloatParser");
    }

    /**
     * @brief Returns the name of the class for GenericFloatParser
     */
    QString ClassName()
    {
      return QString("GenericFloatParser");
    }

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
    QString m_ColumnName = {};

    int m_ColumnIndex = {};

    FloatArrayType::Pointer m_Ptr;

    GenericFloatParser(const GenericFloatParser&); // Copy Constructor Not Implemented
    void operator=(const GenericFloatParser&);     // Move assignment Not ImplementedOperator '=' Not Implemented
};



