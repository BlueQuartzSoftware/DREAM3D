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
#include <utility>

#if EbsdLib_HDF5_SUPPORT
#include "H5Support/QH5Lite.h"
#endif

#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QtDebug>

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdHeaderEntry.h"

#if 0
/**
 * @class CtfHeaderEntry CtfHeaderEntry.h EbsdLib/HKL/CtfHeaderEntry.h
 * @brief Header entry that holds an integer or decimal type value
 *
 * @date Aug 8, 2011
 * @version 1.0
 */
template<typename T>
class EbsdLib_EXPORT CtfHeaderEntry : public EbsdHeaderEntry
{

  public:
    EBSD_SHARED_POINTERS(CtfHeaderEntry<T >)
    HEADERENTRY_NEW_SUPERCLASS(CtfHeaderEntry<T>, EbsdHeaderEntry)
    EBSD_TYPE_MACRO_SUPER(CtfHeaderEntry, EbsdHeaderEntry)

    ~CtfHeaderEntry() = default;

    /**
     * @brief getKey
     * @return
     */
    QString getKey() override
    {
      return m_key;
    }

#if EbsdLib_HDF5_SUPPORT
    QString getHDFType() override
    {
      T value = static_cast<T>(0);
      return QH5Lite::HDFTypeForPrimitiveAsStr(value);
    }
#endif
    /**
     * @brief parseValue
     * @param value
     */
    void parseValue(QByteArray& bytes) override
    {
      // Simple Naieve filter to remove European style decimals that use a comma
      for(char& v : bytes)
      {
        if(v == ',')
        {
          v = '.';
        }
      }
      QTextStream ss(&bytes);
      ss >> m_Value;
    }

    /**
     * @brief print
     * @param out
     */
    void print(std::ostream& out) override
    {
      out << m_key.toStdString() << "  " << m_Value << std::endl;
    }

    /**
     * @brief getValue
     * @return
     */
    T getValue()
    {
      return m_Value;
    }

    /**
     * @brief setValue
     * @param value
     */
    void setValue(T value)
    {
      m_Value = value;
    }

  protected:
    CtfHeaderEntry(QString key)
    : m_Value(0)
    , m_key(std::move(key))
    {
    }

    CtfHeaderEntry() = default;

  private:
    T m_Value;
    QString m_key;

  public:
    CtfHeaderEntry(const CtfHeaderEntry&) = delete; // Copy Constructor Not Implemented
    CtfHeaderEntry(CtfHeaderEntry&&) = delete;      // Move Constructor Not Implemented
    CtfHeaderEntry& operator=(const CtfHeaderEntry&) = delete; // Copy Assignment Not Implemented
    CtfHeaderEntry& operator=(CtfHeaderEntry&&) = delete;      // Move Assignment Not Implemented
};
#endif

/**
 * @class CtfStringHeaderEntry CtfHeaderEntry.h EbsdLib/HKL/CtfHeaderEntry.h
 * @brief Header entry that holds a string type value
 *
 * @date Aug 1, 2011
 * @version 1.0
 */
class CtfStringHeaderEntry : public EbsdHeaderEntry
{
  public:
    EBSD_SHARED_POINTERS(CtfStringHeaderEntry)
    HEADERENTRY_NEW_SUPERCLASS(CtfStringHeaderEntry, EbsdHeaderEntry)

    ~CtfStringHeaderEntry() override = default;

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
      m_Value = QString(value);
    }

    void print(std::ostream& out) override
    {
      out << m_key.toStdString() << "  " << m_Value.toStdString() << std::endl;
    }

    QString getValue()
    {
      return m_Value;
    }
    void setValue(const QString& value)
    {
      m_Value = value;
    }

  protected:
    CtfStringHeaderEntry(QString key)
    : m_key(std::move(key))
    {
    }

    CtfStringHeaderEntry() = default;

  private:
    QString m_Value;
    QString m_key;

  public:
    CtfStringHeaderEntry(const CtfStringHeaderEntry&) = delete; // Copy Constructor Not Implemented
    CtfStringHeaderEntry(CtfStringHeaderEntry&&) = delete;      // Move Constructor Not Implemented
    CtfStringHeaderEntry& operator=(const CtfStringHeaderEntry&) = delete; // Copy Assignment Not Implemented
    CtfStringHeaderEntry& operator=(CtfStringHeaderEntry&&) = delete;      // Move Assignment Not Implemented
};

/**
 * @class CtfInt32HeaderEntry CtfInt32HeaderEntry.h EbsdLib/HKL/CtfInt32HeaderEntry.h
 * @brief Header entry that holds an integer or decimal type value
 *
 * @date Aug 8, 2011
 * @version 1.0
 */

class EbsdLib_EXPORT CtfInt32HeaderEntry : public EbsdHeaderEntry
{

public:
  EBSD_SHARED_POINTERS(CtfInt32HeaderEntry)
  HEADERENTRY_NEW_SUPERCLASS(CtfInt32HeaderEntry, EbsdHeaderEntry)
  EBSD_TYPE_MACRO_SUPER(CtfInt32HeaderEntry, EbsdHeaderEntry)

  ~CtfInt32HeaderEntry() override = default;

  /**
   * @brief getKey
   * @return
   */
  QString getKey() override
  {
    return m_key;
  }

#if EbsdLib_HDF5_SUPPORT
  QString getHDFType() override
  {
    int32_t value = static_cast<int32_t>(0);
    return QH5Lite::HDFTypeForPrimitiveAsStr(value);
  }
#endif
  /**
   * @brief parseValue
   * @param value
   */
  void parseValue(QByteArray& bytes) override
  {
    // Simple Naieve filter to remove European style decimals that use a comma
    for(char& v : bytes)
    {
      if(v == ',')
      {
        v = '.';
      }
    }
    bool ok = false;
    m_Value = QString(bytes).toInt(&ok);
    if(!ok)
    {
      m_Value = -1;
    }
  }

  /**
   * @brief print
   * @param out
   */
  void print(std::ostream& out) override
  {
    out << m_key.toStdString() << "  " << m_Value << std::endl;
  }

  /**
   * @brief getValue
   * @return
   */
  int32_t getValue()
  {
    return m_Value;
  }

  /**
   * @brief setValue
   * @param value
   */
  void setValue(int32_t value)
  {
    m_Value = value;
  }

protected:
  CtfInt32HeaderEntry(const QString& key)
  : m_key(key)
  {
  }

  CtfInt32HeaderEntry() = default;

private:
  int32_t m_Value = 0;
  QString m_key;

public:
  CtfInt32HeaderEntry(const CtfInt32HeaderEntry&) = delete;            // Copy Constructor Not Implemented
  CtfInt32HeaderEntry(CtfInt32HeaderEntry&&) = delete;                 // Move Constructor Not Implemented
  CtfInt32HeaderEntry& operator=(const CtfInt32HeaderEntry&) = delete; // Copy Assignment Not Implemented
  CtfInt32HeaderEntry& operator=(CtfInt32HeaderEntry&&) = delete;      // Move Assignment Not Implemented
};

/**
 * @class CtfFloatHeaderEntry CtfFloatHeaderEntry.h EbsdLib/HKL/CtfFloatHeaderEntry.h
 * @brief Header entry that holds an integer or decimal type value
 *
 * @date Aug 8, 2011
 * @version 1.0
 */

class EbsdLib_EXPORT CtfFloatHeaderEntry : public EbsdHeaderEntry
{

public:
  EBSD_SHARED_POINTERS(CtfFloatHeaderEntry)
  HEADERENTRY_NEW_SUPERCLASS(CtfFloatHeaderEntry, EbsdHeaderEntry)
  EBSD_TYPE_MACRO_SUPER(CtfFloatHeaderEntry, EbsdHeaderEntry)

  ~CtfFloatHeaderEntry() override = default;

  /**
   * @brief getKey
   * @return
   */
  QString getKey() override
  {
    return m_key;
  }

#if EbsdLib_HDF5_SUPPORT
  QString getHDFType() override
  {
    float value = static_cast<float>(0);
    return QH5Lite::HDFTypeForPrimitiveAsStr(value);
  }
#endif
  /**
   * @brief parseValue
   * @param value
   */
  void parseValue(QByteArray& bytes) override
  {
    // Simple Naieve filter to remove European style decimals that use a comma
    for(char& v : bytes)
    {
      if(v == ',')
      {
        v = '.';
      }
    }
    bool ok = false;
    m_Value = QString(bytes).toFloat(&ok);
    if(!ok)
    {
      m_Value = -1.0f;
    }
  }

  /**
   * @brief print
   * @param out
   */
  void print(std::ostream& out) override
  {
    out << m_key.toStdString() << "  " << m_Value << std::endl;
  }

  /**
   * @brief getValue
   * @return
   */
  float getValue()
  {
    return m_Value;
  }

  /**
   * @brief setValue
   * @param value
   */
  void setValue(float value)
  {
    m_Value = value;
  }

protected:
  CtfFloatHeaderEntry(const QString& key)
  : m_key(key)
  {
  }

  CtfFloatHeaderEntry() = default;

private:
  float m_Value = 0.0f;
  QString m_key;

public:
  CtfFloatHeaderEntry(const CtfFloatHeaderEntry&) = delete;            // Copy Constructor Not Implemented
  CtfFloatHeaderEntry(CtfFloatHeaderEntry&&) = delete;                 // Move Constructor Not Implemented
  CtfFloatHeaderEntry& operator=(const CtfFloatHeaderEntry&) = delete; // Copy Assignment Not Implemented
  CtfFloatHeaderEntry& operator=(CtfFloatHeaderEntry&&) = delete;      // Move Assignment Not Implemented
};
