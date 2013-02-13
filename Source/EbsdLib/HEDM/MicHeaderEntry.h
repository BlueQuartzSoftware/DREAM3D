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




#include <string.h>
#include <iostream>

#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdHeaderEntry.h"


/**
 * @class MicHeaderEntry MicHeaderEntry.h EbsdLib/HEDM/MicHeaderEntry.h
 * @brief Header entry that holds an integer or decimal type value
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Aug 8, 2011
 * @version 1.0
 */
template<typename T>
class EbsdLib_EXPORT MicHeaderEntry : public EbsdHeaderEntry
{

  public:
    EBSD_SHARED_POINTERS(MicHeaderEntry<T >)
    HEADERENTRY_NEW_SUPERCLASS(MicHeaderEntry<T>, EbsdHeaderEntry)

    virtual ~MicHeaderEntry() {}

    std::string getKey() { return m_key; }
    std::string getHDFType() { T value = static_cast<T>(0); return H5Lite::HDFTypeForPrimitiveAsStr(value); }
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
    MicHeaderEntry(const std::string &key) :
      m_value(0),
      m_key(key)
    {
    }

    MicHeaderEntry() {}

  private:
    T m_value;
    std::string m_key;

    MicHeaderEntry(const MicHeaderEntry&); // Copy Constructor Not Implemented
    void operator=(const MicHeaderEntry&); // Operator '=' Not Implemented
};

/**
 * @class MicStringHeaderEntry MicStringHeaderEntry.h EbsdLib/HEDM/MicHeaderEntry.h
 * @brief Header entry that holds a string type value
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Aug 1, 2011
 * @version 1.0
 */
class MicStringHeaderEntry : public EbsdHeaderEntry
{
  public:
    EBSD_SHARED_POINTERS(MicStringHeaderEntry)
    HEADERENTRY_NEW_SUPERCLASS(MicStringHeaderEntry, EbsdHeaderEntry)

    virtual ~MicStringHeaderEntry() {}

    std::string getKey() { return m_key; }
    std::string getHDFType() { return "H5T_STRING"; }

    void parseValue(char* value, size_t start, size_t length)
    {
      if (value[start] == ':') { ++start; } // move past the ":" character
      while(value[start] < 33)
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
    MicStringHeaderEntry(const std::string &key) :
      m_key(key)
    {
    }

    MicStringHeaderEntry() {}

  private:
    std::string m_value;
    std::string m_key;

    MicStringHeaderEntry(const MicStringHeaderEntry&); // Copy Constructor Not Implemented
    void operator=(const MicStringHeaderEntry&); // Operator '=' Not Implemented
};








