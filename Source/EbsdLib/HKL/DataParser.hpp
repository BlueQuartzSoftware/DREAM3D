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
#ifndef DATAPARSER_HPP_
#define DATAPARSER_HPP_

#include <QtCore/QString>

#include "EbsdLib/EbsdSetGetMacros.h"

class DataParser
{
  public:
    EBSD_SHARED_POINTERS(DataParser)
    EBSD_TYPE_MACRO(DataParser)

    virtual ~DataParser() {}

    virtual void parse(const QString &token, size_t offset)=0;
  protected:
    DataParser(){}

  private:
    DataParser(const DataParser&); // Copy Constructor Not Implemented
    void operator=(const DataParser&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Int32Parser : public DataParser
{
  public:
    EBSD_SHARED_POINTERS(Int32Parser)
    EBSD_TYPE_MACRO(Int32Parser)
    static Pointer New(int32_t* ptr, size_t size, const QString &name)
    {
      Pointer sharedPtr (new Int32Parser(ptr, size, name));
      return sharedPtr;
    }

    virtual ~Int32Parser() {}

    EBSD_INSTANCE_PROPERTY(int32_t*, Ptr)
    EBSD_INSTANCE_PROPERTY(size_t, Size)
    EBSD_INSTANCE_STRING_PROPERTY(ColumnName)

    virtual void parse(const QString &token, size_t offset)
    {
      int value;
      sscanf( token.c_str(), "%d", &value);
      m_Ptr[offset] = value;
    }
  protected:
    Int32Parser(int32_t* ptr, size_t size, const QString &name) :
      m_Ptr(ptr),
      m_Size(size),
      m_ColumnName(name)
    {
     // std::cout << m_ColumnName << " Pointer: " << m_Ptr << std::endl;
    }

  private:


    Int32Parser(const Int32Parser&); // Copy Constructor Not Implemented
    void operator=(const Int32Parser&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class FloatParser : public DataParser
{
  public:
    EBSD_SHARED_POINTERS(FloatParser)
    EBSD_TYPE_MACRO(FloatParser)
    static Pointer New(float* ptr, size_t size, const QString &name)
    {
      Pointer sharedPtr (new FloatParser(ptr, size, name));
      return sharedPtr;
    }

    virtual ~FloatParser() {}

    EBSD_INSTANCE_PROPERTY(float*, Ptr)
    EBSD_INSTANCE_PROPERTY(size_t, Size)
    EBSD_INSTANCE_STRING_PROPERTY(ColumnName)

    virtual void parse(const QString &token, size_t offset)
    {
      float value;
      sscanf( token.c_str(), "%f", &value);
      m_Ptr[offset] = value;
      //printf("%s: %f\n", m_ColumnName.c_str(), value);
    }
  protected:
    FloatParser(float* ptr, size_t size, const QString &name) :
      m_Ptr(ptr),
      m_Size(size),
      m_ColumnName(name)
    {
     // std::cout << m_ColumnName << " Pointer: " << m_Ptr << std::endl;
    }
  private:

    FloatParser(const FloatParser&); // Copy Constructor Not Implemented
    void operator=(const FloatParser&); // Operator '=' Not Implemented
};


#endif /* DATAPARSER_HPP_ */
