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

#include <cstring>

#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QtDebug>

#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdHeaderEntry.h"


/**
 * @class AngHeaderEntry AngHeaderEntry.h EbsdLib/TSL/AngHeaderEntry.h
 * @brief Header entry that holds an integer or decimal type value
 *
 * @date Aug 8, 2011
 * @version 1.0
 */
template<typename T>
class EbsdLib_EXPORT AngHeaderEntry : public EbsdHeaderEntry
{

  public:
    EBSD_SHARED_POINTERS(AngHeaderEntry<T >)
    HEADERENTRY_NEW_SUPERCLASS(AngHeaderEntry<T>, EbsdHeaderEntry)

    ~AngHeaderEntry() override = default;

    QString getKey() override
    {
      return m_key;
    }
#if EbsdLib_HDF5_SUPPORT
    QString getHDFType() override
    {
      T value = static_cast<T>(0);
      return QString::fromStdString(H5Lite::HDFTypeForPrimitiveAsStr(value));
    }
#endif

    void parseValue(QByteArray& value) override
    {
      if (value[0] == ':') { value = value.mid(1); } // move past the ":" character
      QTextStream ss(&value);
      ss >> m_value;
    }
    void print(std::ostream& out) override
    {
      out << m_key.toStdString() << "  " << m_value << std::endl;
    }

    T getValue() { return m_value; }
    void setValue(T value) { m_value = value;}

  protected:
    AngHeaderEntry(const QString& key) :
      m_value(0),
      m_key(key)
    {
    }

    AngHeaderEntry() = default;

  private:
    T m_value;
    QString m_key;

  public:
    AngHeaderEntry(const AngHeaderEntry&) = delete; // Copy Constructor Not Implemented
    AngHeaderEntry(AngHeaderEntry&&) = delete;      // Move Constructor Not Implemented
    AngHeaderEntry& operator=(const AngHeaderEntry&) = delete; // Copy Assignment Not Implemented
    AngHeaderEntry& operator=(AngHeaderEntry&&) = delete;      // Move Assignment Not Implemented
};

/**
 * @class AngStringHeaderEntry AngStringHeaderEntry.h EbsdLib/TSL/AngHeaderEntry.h
 * @brief Header entry that holds a string type value
 *
 * @date Aug 1, 2011
 * @version 1.0
 */
class AngStringHeaderEntry : public EbsdHeaderEntry
{
  public:
    EBSD_SHARED_POINTERS(AngStringHeaderEntry)
    HEADERENTRY_NEW_SUPERCLASS(AngStringHeaderEntry, EbsdHeaderEntry)

    ~AngStringHeaderEntry() override = default;

    QString getKey() override
    {
      return m_key;
    }
    QString getHDFType() override
    {
      return "H5T_STRING";
    }

    void parseValue(QByteArray& value) override
    {
      if (value[0] == ':') { value = value.mid(1); } // move past the ":" character
      value = value.trimmed(); // remove leading/trailing white space
      m_value = QString(value);
    }
    void print(std::ostream& out) override
    {
      out << m_key.toStdString() << "  " << m_value.toStdString() << std::endl;
    }

    QString getValue() { return m_value; }
    void setValue(const QString& value)
    {
      m_value = value;
    }

  protected:
    AngStringHeaderEntry(QString key)
    : m_key(std::move(key))
    {
    }

    AngStringHeaderEntry() = default;

  private:
    QString m_value;
    QString m_key;

  public:
    AngStringHeaderEntry(const AngStringHeaderEntry&) = delete; // Copy Constructor Not Implemented
    AngStringHeaderEntry(AngStringHeaderEntry&&) = delete;      // Move Constructor Not Implemented
    AngStringHeaderEntry& operator=(const AngStringHeaderEntry&) = delete; // Copy Assignment Not Implemented
    AngStringHeaderEntry& operator=(AngStringHeaderEntry&&) = delete;      // Move Assignment Not Implemented
};
