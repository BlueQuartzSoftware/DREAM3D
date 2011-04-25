/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
///////////////////////////////////////////////////////////////////////////////
// This code was partly written under US Air Force Contract FA8650-07-D-5800
///////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <string.h>

#include "ANG/AngSetGetMacros.h"
#include "ANG/TSLAngLibConfiguration.h"

/**
 * @brief Creates a static "New" method that creates an instance of thisClass
 */
#define HEADERENTRY_NEW_SUPERCLASS(thisClass, SuperClass)\
  typedef SuperClass::Pointer SuperClass##Type;\
  static SuperClass##Type New##SuperClass(const std::string &key) \
{ \
  SuperClass##Type sharedPtr (new thisClass(key)); \
  return sharedPtr; \
}\

/**
 *
 */
class TSLANG_EXPORT HeaderEntry
{
  public:
    ANG_SHARED_POINTERS(HeaderEntry);
    virtual ~HeaderEntry() {}
    virtual std::string getKey() = 0;
    virtual void parseValue(char* value, size_t start, size_t length) = 0;
    virtual void print(std::ostream &out) = 0;

  protected:
    HeaderEntry() {}


  private:
    HeaderEntry(const HeaderEntry&); // Copy Constructor Not Implemented
    void operator=(const HeaderEntry&); // Operator '=' Not Implemented

};


template<typename T>
class TSLANG_EXPORT AngHeaderEntry : public HeaderEntry
{

  public:
    ANG_SHARED_POINTERS(AngHeaderEntry<T >);
    HEADERENTRY_NEW_SUPERCLASS(AngHeaderEntry<T>, HeaderEntry);

    virtual ~AngHeaderEntry() {}

    std::string getKey() { return m_key; }
    void parseValue(char* value, size_t start, size_t length)
    {
      if (value[start] == ':') { ++start; } // move past the ":" character
      std::string data( &(value[start]), strlen(value) - start);
      std::stringstream ss(data);
      ss >> m_value;
    }
    void print(std::ostream &out) {
      out << m_key << "  " << m_value << std::endl;
    }

    T getValue() { return m_value; }
    void setValue(T value) { m_value = value;}

  protected:
    AngHeaderEntry(const std::string &key) :
      m_value(0),
      m_key(key)
    {
    }

    AngHeaderEntry() {}

  private:
    T m_value;
    std::string m_key;

    AngHeaderEntry(const AngHeaderEntry&); // Copy Constructor Not Implemented
    void operator=(const AngHeaderEntry&); // Operator '=' Not Implemented
};


class AngStringHeaderEntry : public HeaderEntry
{
  public:
    ANG_SHARED_POINTERS(AngStringHeaderEntry);
    HEADERENTRY_NEW_SUPERCLASS(AngStringHeaderEntry, HeaderEntry);

    virtual ~AngStringHeaderEntry() {}

    std::string getKey() { return m_key; }
    void parseValue(char* value, size_t start, size_t length)
    {
      if (value[start] == ':') { ++start; } // move past the ":" character
      while(value[start] == ' ')
      {
        ++start;
      }
      std::string data( &(value[start]), strlen(value) - start);
      m_value = data;
    }
    void print(std::ostream &out) {
      out << m_key << "  " << m_value << std::endl;
    }

    std::string getValue() { return m_value; }
    void setValue(const std::string &value)
    {
      m_value = value;
    }

  protected:
    AngStringHeaderEntry(const std::string &key) :
      m_key(key)
    {
    }

    AngStringHeaderEntry() {}

  private:
    std::string m_value;
    std::string m_key;

    AngStringHeaderEntry(const AngStringHeaderEntry&); // Copy Constructor Not Implemented
    void operator=(const AngStringHeaderEntry&); // Operator '=' Not Implemented
};








