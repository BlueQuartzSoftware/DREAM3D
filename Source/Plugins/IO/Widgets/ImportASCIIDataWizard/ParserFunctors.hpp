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

#ifndef _ParserFunctors_hpp_
#define _ParserFunctors_hpp_

#include <QtCore/QByteArray>

#include "SIMPLib/SIMPLib.h"

namespace ParserErrorMessages
{
  const QString ValueOutOfRange = "Value is out of range for the specified data type.";
  const QString CouldNotConvert = "Value could not be converted to the specified data type.";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class ParserFunctor
{
public:
  virtual ~ParserFunctor() {}

  struct ErrorObject
  {
    bool ok;
    QString errorMessage;
  };
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Int8Functor : public ParserFunctor
{
  public:
    virtual ~Int8Functor() {}

    int8_t operator() (const QString &token, ErrorObject &obj)
    {
      int16_t value = token.toInt(&obj.ok, 0);
      if (!obj.ok)
      {
        // Try double
        double dValue = token.toDouble(&obj.ok);
        if (obj.ok)
        {
          // If it converts to a double, cast it to the correct type
          value = static_cast<int8_t>(dValue);
        }
        else
        {
          obj.errorMessage = ParserErrorMessages::CouldNotConvert;
        }
      }
      else if(value > std::numeric_limits<int8_t>().max() || value < std::numeric_limits<int8_t>().min())
      {
          // This value is out of range, so return a conversion error
          obj.ok = false;
          obj.errorMessage = ParserErrorMessages::ValueOutOfRange;
      }
      else
      {
        int8_t realValue = static_cast<int8_t>(value);
        return realValue;
      }
      return value;
    }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class UInt8Functor : public ParserFunctor
{
public:
  virtual ~UInt8Functor() {}

  uint8_t operator() (const QString &token, ErrorObject &obj)
  {
    uint16_t value = token.toUInt(&obj.ok);
    if (!obj.ok)
    {
      if (token.isEmpty() == false && token[0] == '-')
      {
        // This is a negative value, so fail
        obj.errorMessage = ParserErrorMessages::ValueOutOfRange;
      }
      else
      {
        // Try double
        double dValue = token.toDouble(&obj.ok);
        if (obj.ok)
        {
          // If it converts to a double, cast it to the correct type
          value = static_cast<uint8_t>(dValue);
        }
        else
        {
          obj.errorMessage = ParserErrorMessages::CouldNotConvert;
        }
      }
    }
    else if (value > std::numeric_limits<uint8_t>().max())
    {
      obj.ok = false;
      obj.errorMessage = ParserErrorMessages::ValueOutOfRange;
    }
    return value;
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Int16Functor : public ParserFunctor
{
public:
  virtual ~Int16Functor() {}

  int16_t operator() (const QString &token, ErrorObject &obj)
  {
    int32_t value = token.toInt(&obj.ok);
    if (!obj.ok)
    {
      // Try double
      double dValue = token.toDouble(&obj.ok);
      if (obj.ok)
      {
        // If it converts to a double, cast it to the correct type
        value = static_cast<int16_t>(dValue);
      }
      else
      {
        obj.errorMessage = ParserErrorMessages::CouldNotConvert;
      }
    }
    else if (value > std::numeric_limits<int16_t>().max() || value < std::numeric_limits<int16_t>().min())
    {
      // This value is out of range, so return a conversion error
      obj.ok = false;
      obj.errorMessage = ParserErrorMessages::ValueOutOfRange;
    }
    else
    {
      int16_t realValue = static_cast<int16_t>(value);
      return realValue;
    }
    return value;
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class UInt16Functor : public ParserFunctor
{
public:
  virtual ~UInt16Functor() {}

  uint16_t operator() (const QString &token, ErrorObject &obj)
  {
    uint32_t value = token.toUInt(&obj.ok);
    if (!obj.ok)
    {
      if (token.isEmpty() == false && token[0] == '-')
      {
        // This is a negative value, so fail
        obj.errorMessage = ParserErrorMessages::ValueOutOfRange;
      }
      else
      {
        // Try double
        double dValue = token.toDouble(&obj.ok);
        if (obj.ok)
        {
          // If it converts to a double, cast it to the correct type
          value = static_cast<uint16_t>(dValue);
        }
        else
        {
          obj.errorMessage = ParserErrorMessages::CouldNotConvert;
        }
      }
    }
    else if (value > std::numeric_limits<uint16_t>().max())
    {
      // This value is out of range, so return a conversion error
      obj.ok = false;
      obj.errorMessage = ParserErrorMessages::ValueOutOfRange;
    }
    else
    {
      uint16_t realValue = static_cast<uint16_t>(value);
      return realValue;
    }
    return value;
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Int32Functor : public ParserFunctor
{
public:
  virtual ~Int32Functor() {}

  int32_t operator()(const QString &token, ErrorObject &obj)
  {
    int64_t value = token.toLongLong(&obj.ok);
    if (!obj.ok)
    {
      // Try double
      double dValue = token.toDouble(&obj.ok);
      if (obj.ok)
      {
        // If it converts to a double, cast it to the correct type
        value = static_cast<int32_t>(dValue);
      }
      else
      {
        obj.errorMessage = ParserErrorMessages::CouldNotConvert;
      }
    }
    else if (value > std::numeric_limits<int32_t>().max() || value < std::numeric_limits<int32_t>().min())
    {
      // This value is out of range, so return a conversion error
      obj.ok = false;
      obj.errorMessage = ParserErrorMessages::ValueOutOfRange;
    }
    else
    {
      int32_t realValue = static_cast<int32_t>(value);
      return realValue;
    }
    return value;
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class UInt32Functor : public ParserFunctor
{
public:
  virtual ~UInt32Functor() {}

  uint32_t operator() (const QString &token, ErrorObject &obj)
  {
    uint64_t value = token.toULongLong(&obj.ok);
    if (!obj.ok)
    {
      if (token.isEmpty() == false && token[0] == '-')
      {
        // This is a negative value, so fail
        obj.errorMessage = ParserErrorMessages::ValueOutOfRange;
      }
      else
      {
        // Try double
        double dValue = token.toDouble(&obj.ok);
        if (obj.ok)
        {
          // If it converts to a double, cast it to the correct type
          value = static_cast<uint32_t>(dValue);
        }
        else
        {
          obj.errorMessage = ParserErrorMessages::CouldNotConvert;
        }
      }
    }
    else if (value > std::numeric_limits<uint32_t>().max())
    {
      // This value is out of range, so return a conversion error
      obj.ok = false;
      obj.errorMessage = ParserErrorMessages::ValueOutOfRange;
    }
    else
    {
      uint32_t realValue = static_cast<uint32_t>(value);
      return realValue;
    }
    return value;
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Int64Functor : public ParserFunctor
{
public:
  virtual ~Int64Functor() {}

  int64_t operator() (const QString &token, ErrorObject &obj)
  {
    int64_t value = token.toLongLong(&obj.ok);
    if (!obj.ok)
    {
      if (token.contains('.'))
      {
        double doubleValue = token.toDouble(&obj.ok);
        if (!obj.ok)
        {
          obj.errorMessage = ParserErrorMessages::CouldNotConvert;
        }
        else
        {
          value = static_cast<int64_t>(doubleValue);
        }
      }
      else
      {
        obj.errorMessage = ParserErrorMessages::CouldNotConvert;
      }
    }
    return value;
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class UInt64Functor : public ParserFunctor
{
public:
  virtual ~UInt64Functor() {}

  uint64_t operator() (const QString &token, ErrorObject &obj)
  {
    uint64_t value = token.toULongLong(&obj.ok);
    if (!obj.ok)
    {
      if (token.contains('.'))
      {
        double doubleValue = token.toDouble(&obj.ok);
        if (!obj.ok)
        {
          obj.errorMessage = ParserErrorMessages::CouldNotConvert;
        }
        else
        {
          value = static_cast<uint64_t>(doubleValue);
        }
      }
      else if (token.isEmpty() == false && token[0] == '-')
      {
        obj.errorMessage = ParserErrorMessages::ValueOutOfRange;
      }
      else
      {
        obj.errorMessage = ParserErrorMessages::CouldNotConvert;
      }
    }
    return value;
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class FloatFunctor : public ParserFunctor
{
public:
  virtual ~FloatFunctor() {}

  float operator() (const QString &token, ErrorObject &obj)
  {
    float value = token.toFloat(&obj.ok);
    return value;
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DoubleFunctor : public ParserFunctor
{
public:
  virtual ~DoubleFunctor() {}

  double operator() (const QString &token, ErrorObject &obj)
  {
    double value = token.toDouble(&obj.ok);
    return value;
  }
};

#endif /* PARSERFUNCTORS_HPP_ */

