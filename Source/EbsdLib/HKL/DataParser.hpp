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

#include "EbsdLib/EbsdSetGetMacros.h"

class DataParser
{
  public:
    EBSD_SHARED_POINTERS(DataParser)
    EBSD_TYPE_MACRO(DataParser)

    virtual ~DataParser() = default;

    virtual bool allocateArray(size_t numberOfElements) { (void)(numberOfElements); return false;}
    virtual void* getVoidPointer() { return nullptr; }
    virtual void  setVoidPointer(void* p) {}

    EBSD_INSTANCE_PROPERTY(bool, ManageMemory)
    EBSD_INSTANCE_PROPERTY(size_t, Size)
    EBSD_INSTANCE_STRING_PROPERTY(ColumnName)
    EBSD_INSTANCE_PROPERTY(int, ColumnIndex)


    virtual void parse(const QByteArray& token, size_t index) {}
  protected:
    DataParser()
    : m_ManageMemory(false)
    , m_Size(0)
    , m_ColumnName("")
    , m_ColumnIndex(0)
    {
    }

  public:
    DataParser(const DataParser&) = delete;            // Copy Constructor Not Implemented
    DataParser(DataParser&&) = delete;                 // Move Constructor Not Implemented
    DataParser& operator=(const DataParser&) = delete; // Copy Assignment Not Implemented
    DataParser& operator=(DataParser&&) = delete;      // Move Assignment Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Int32Parser : public DataParser
{
  public:
    EBSD_SHARED_POINTERS(Int32Parser)
    EBSD_TYPE_MACRO_SUPER(Int32Parser, DataParser)
    static Pointer New(int32_t* ptr, size_t size, const QString& name, int index)
    {
      Pointer sharedPtr (new Int32Parser(ptr, size, name, index));
      return sharedPtr;
    }

    ~Int32Parser() override
    {
      if(m_Ptr != nullptr && getManageMemory())
      {
#if defined ( SIMPL_USE_SSE ) && defined ( __SSE2__ )
        _mm_free(m_Ptr );
#else
        free(m_Ptr);
#endif
        m_Ptr = nullptr;
      }
    }

    void setPtr(int32_t* value)
    {
      this->m_Ptr = value;
    }
    int32_t* getPtr()
    {
      return m_Ptr;
    }

    bool allocateArray(size_t numberOfElements) override
    {
#if defined ( SIMPL_USE_SSE ) && defined ( __SSE2__ )
      m_Ptr = static_cast<int32_t*>( _mm_malloc (numberOfElements * sizeof(T), 16) );
#else
      m_Ptr = static_cast<int32_t*>(malloc(sizeof(int32_t) * numberOfElements));
#endif
      return (m_Ptr != nullptr);
    }

    void* getVoidPointer() override
    {
      return reinterpret_cast<void*>(m_Ptr);
    }
    void setVoidPointer(void* p) override
    {
      m_Ptr = reinterpret_cast<int32_t*>(p);
    }

    int32_t* getPointer(size_t offset) { return m_Ptr + offset; }

    void parse(const QByteArray& token, size_t index) override
    {
      Q_ASSERT(index < getSize());
      bool ok = false;
      m_Ptr[index] = token.toInt(&ok, 10);
    }

  protected:
    Int32Parser(int32_t* ptr, size_t size, const QString& name, int index) :
      m_Ptr(ptr)
    {
      setManageMemory(true);
      setSize(size);
      setColumnName(name);
      setColumnIndex(index);
    }

  private:
    int32_t* m_Ptr;

  public:
    Int32Parser(const Int32Parser&) = delete;            // Copy Constructor Not Implemented
    Int32Parser(Int32Parser&&) = delete;                 // Move Constructor Not Implemented
    Int32Parser& operator=(const Int32Parser&) = delete; // Copy Assignment Not Implemented
    Int32Parser& operator=(Int32Parser&&) = delete;      // Move Assignment Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class FloatParser : public DataParser
{
  public:
    EBSD_SHARED_POINTERS(FloatParser)
    EBSD_TYPE_MACRO_SUPER(FloatParser, DataParser)
    static Pointer New(float* ptr, size_t size, const QString& name, int index)
    {
      Pointer sharedPtr (new FloatParser(ptr, size, name, index));
      return sharedPtr;
    }

    ~FloatParser() override
    {
      if(m_Ptr != nullptr && getManageMemory())
      {
#if defined ( SIMPL_USE_SSE ) && defined ( __SSE2__ )
        _mm_free(m_Ptr );
#else
        free(m_Ptr);
#endif
        m_Ptr = nullptr;
      }
    }

    void setPtr(float* value)
    {
      this->m_Ptr = value;
    }
    float* getPtr()
    {
      return m_Ptr;
    }

    bool allocateArray(size_t numberOfElements) override
    {
#if defined ( SIMPL_USE_SSE ) && defined ( __SSE2__ )
      m_Ptr = static_cast<float*>( _mm_malloc (numberOfElements * sizeof(T), 16) );
#else
      m_Ptr = static_cast<float*>(malloc(sizeof(float) * numberOfElements));
#endif
      return (m_Ptr != nullptr);
    }

    void* getVoidPointer() override
    {
      return reinterpret_cast<void*>(m_Ptr);
    }
    void setVoidPointer(void* p) override
    {
      m_Ptr = reinterpret_cast<float*>(p);
    }

    float* getPointer(size_t offset) { return m_Ptr + offset; }

    void parse(const QByteArray& token, size_t index) override
    {
      bool ok = false;
      m_Ptr[index] = token.toFloat(&ok);
    }

  protected:
    FloatParser(float* ptr, size_t size, const QString& name, int index) :
      m_Ptr(ptr)
    {
      setManageMemory(true);
      setSize(size);
      setColumnName(name);
      setColumnIndex(index);
    }

  private:
    float* m_Ptr;

  public:
    FloatParser(const FloatParser&) = delete;            // Copy Constructor Not Implemented
    FloatParser(FloatParser&&) = delete;                 // Move Constructor Not Implemented
    FloatParser& operator=(const FloatParser&) = delete; // Copy Assignment Not Implemented
    FloatParser& operator=(FloatParser&&) = delete;      // Move Assignment Not Implemented
};



