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

#include <array>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <QtCore/QString>

/**
 * @file EbsdSEtGetMacros.h
 * @brief This file contains preprocessor macros that can be used to quickly
 * create instance variables, pointer types and other conveniences for a C++ class.
 */

#if defined(QT_CORE_LIB)
//-- Qt includes
#include <QtCore/QSharedPointer>
//#define RAW_PTR  data
#endif

//-- C++11 Includes
#include <memory>

#ifndef SHARED_IS_NULL
#define SHARED_IS_NULL(ptr)\
  (  (ptr).get() == nullptr )
#endif

/**
 * @brief Creates a static method that returns a nullptr pointer wrapped in a
 * std::shared_ptr<>
 * @param thisClass The name of the class.
 */
#define EBSD_NULL_SHARED_POINTER(thisClass)\
  static Pointer NullPointer(void)\
  { \
    return Pointer(static_cast<thisClass*>(nullptr));\
  }

#ifndef QT_SHARED_POINTERS
/**
 * @brief Creates some basic typedefs that can be used throughout the code to
 * reference the class.
 */
#define QT_SHARED_POINTERS(thisClass)\
  typedef thisClass                      Self;\
  typedef QSharedPointer< Self >        Pointer;\
  typedef QSharedPointer<const Self >  ConstPointer;\
  EBSD_NULL_SHARED_POINTER(thisClass)

#endif

/**
 * @brief Creates some basic typedefs that can be used throughout the code to
 * reference the class.
 */
#define EBSD_SHARED_POINTERS(thisClass)\
  typedef thisClass                      Self;\
  typedef std::shared_ptr<Self >        Pointer;\
  typedef std::shared_ptr<const Self >  ConstPointer;\
  typedef std::weak_ptr<thisClass > WeakPointer;\
  typedef std::weak_ptr<thisClass > ConstWeakPointer;\
  EBSD_NULL_SHARED_POINTER(thisClass)


#define EBSD_STATIC_NEW_SUPERCLASS(superclass, theclass)\
  static superclass::Pointer New()\
  {\
    superclass::Pointer shared_ptr (dynamic_cast<superclass*>(new theclass()) );\
    return shared_ptr;\
  }


/**
 * @brief Creates a typedef that points to the superclass of this class
 */
#define EBSD_SUPERCLASS_POINTER(SuperClass)\
  typedef SuperClass::Pointer SuperClass;


/**
 * @brief Creates a static "New" method that creates an instance of thisClass
 */
#define EBSD_NEW_SUPERCLASS(thisClass, SuperClass)                                                                                                                                                     \
  typedef SuperClass::Pointer SuperClass##Type;                                                                                                                                                        \
  static SuperClass##Type New##SuperClass(void)                                                                                                                                                        \
  {                                                                                                                                                                                                    \
    SuperClass##Type sharedPtr(new(thisClass));                                                                                                                                                        \
    return sharedPtr;                                                                                                                                                                                  \
  }

/**
 * @brief Implements a Static 'New' Method for a class
 */
#define EBSD_STATIC_NEW_MACRO(thisClass)                                                                                                                                                               \
  static Pointer New(void)                                                                                                                                                                             \
  {                                                                                                                                                                                                    \
    Pointer sharedPtr(new(thisClass));                                                                                                                                                                 \
    return sharedPtr;                                                                                                                                                                                  \
  }

#define EBSD_STATIC_NEW_MACRO_WITH_ARGS(thisClass, args)                                                                                                                                               \
  static Pointer New args                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    Pointer sharedPtr(new(thisClass));                                                                                                                                                                 \
    return sharedPtr;                                                                                                                                                                                  \
  }

/** Macro used to add standard methods to all classes, mainly type
 * information. */
#define EBSD_TYPE_MACRO(thisClass)                                                                                                                                                                     \
public:                                                                                                                                                                                                \
  const char* getNameOfClass() const                                                                                                                                                                   \
  {                                                                                                                                                                                                    \
    return #thisClass;                                                                                                                                                                                 \
  }                                                                                                                                                                                                    \
  static int IsTypeOf(const char* m_msgType)                                                                                                                                                           \
  {                                                                                                                                                                                                    \
    if(!strcmp(#thisClass, m_msgType))                                                                                                                                                                 \
    {                                                                                                                                                                                                  \
      return 1;                                                                                                                                                                                        \
    }                                                                                                                                                                                                  \
    return 0;                                                                                                                                                                                          \
  }                                                                                                                                                                                                    \
  virtual int IsA(const char* m_msgType)                                                                                                                                                               \
  {                                                                                                                                                                                                    \
    return this->thisClass::IsTypeOf(m_msgType);                                                                                                                                                       \
  }                                                                                                                                                                                                    \
  template <class Target, class Source> inline Target polymorphic_downcast(Source* x)                                                                                                                  \
  {                                                                                                                                                                                                    \
    if(dynamic_cast<Target>(x) != x)                                                                                                                                                                   \
    {                                                                                                                                                                                                  \
      return nullptr;                                                                                                                                                                                  \
    }                                                                                                                                                                                                  \
    return static_cast<Target>(x);                                                                                                                                                                     \
  }

#define EBSD_TYPE_MACRO_SUPER(thisClass, superclass)                                                                                                                                                   \
public:                                                                                                                                                                                                \
  virtual const char* getNameOfClass() const                                                                                                                                                           \
  {                                                                                                                                                                                                    \
    return #thisClass;                                                                                                                                                                                 \
  }                                                                                                                                                                                                    \
  static int IsTypeOf(const char* m_msgType)                                                                                                                                                           \
  {                                                                                                                                                                                                    \
    if(!strcmp(#thisClass, m_msgType))                                                                                                                                                                 \
    {                                                                                                                                                                                                  \
      return 1;                                                                                                                                                                                        \
    }                                                                                                                                                                                                  \
    return superclass::IsTypeOf(m_msgType);                                                                                                                                                            \
  }                                                                                                                                                                                                    \
  virtual int IsA(const char* m_msgType) override                                                                                                                                                      \
  {                                                                                                                                                                                                    \
    return this->thisClass::IsTypeOf(m_msgType);                                                                                                                                                       \
  }                                                                                                                                                                                                    \
  template <class Target, class Source> static Target polymorphic_downcast(Source* x)                                                                                                                  \
  {                                                                                                                                                                                                    \
    if(dynamic_cast<Target>(x) != x)                                                                                                                                                                   \
    {                                                                                                                                                                                                  \
      return nullptr;                                                                                                                                                                                  \
    }                                                                                                                                                                                                  \
    return static_cast<Target>(x);                                                                                                                                                                     \
  }

//------------------------------------------------------------------------------
// Macros for Properties
/**
* @brief Creates a QString constant for the Property so that the property
* can be retrieved by name.
*/
#define EBSD_PROPERTY_CONSTANT(prpty) \
  const QString prpty ( #prpty );

/**
* @brief Creates a "setter" method to set the property.
*/
#define EBSD_SET_PROPERTY(m_msgType, prpty)                                                                                                                                                            \
  void set##prpty(const m_msgType& value)                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    this->m_##prpty = value;                                                                                                                                                                           \
  }

/**
* @brief Creates a "getter" method to retrieve the value of the property.
*/
#define EBSD_GET_PROPERTY(m_msgType, prpty) \
  m_msgType get##prpty() { return m_##prpty; }



#define EBSD_VIRTUAL_INSTANCE_PROPERTY(m_msgType, prpty)\
  private:\
  m_msgType   m_##prpty;\
  public:\
  virtual EBSD_SET_PROPERTY(m_msgType, prpty)\
  virtual EBSD_GET_PROPERTY(m_msgType, prpty)


#define EBSD_INSTANCE_PROPERTY(m_msgType, prpty)\
  private:\
  m_msgType   m_##prpty;\
  public:\
  EBSD_SET_PROPERTY(m_msgType, prpty)\
  EBSD_GET_PROPERTY(m_msgType, prpty)

/**
 * @brief Creates a "setter" method to set the property.
 */
#define EBSD_SET_PTR_PROPERTY(m_msgType, prpty)                                                                                                                                                        \
  void set##prpty(m_msgType value)                                                                                                                                                                     \
  {                                                                                                                                                                                                    \
    this->m_##prpty = value;                                                                                                                                                                           \
  }

/**
 * @brief Creates a "getter" method to retrieve the value of the property.
 */
#define EBSD_GET_PTR_PROPERTY(m_msgType, prpty)                                                                                                                                                        \
  m_msgType get##prpty()                                                                                                                                                                               \
  {                                                                                                                                                                                                    \
    return m_##prpty;                                                                                                                                                                                  \
  }

#define EBSD_PTR_INSTANCE_PROPERTY(type, prpty)                                                                                                                                                        \
private:                                                                                                                                                                                               \
  type m_##prpty = nullptr;                                                                                                                                                                            \
  bool m_##prpty##Cleanup = true;                                                                                                                                                                      \
                                                                                                                                                                                                       \
public:                                                                                                                                                                                                \
  void set##prpty##PointerOwnership(bool owns)                                                                                                                                                         \
  {                                                                                                                                                                                                    \
    m_##prpty##Cleanup = owns;                                                                                                                                                                         \
  }                                                                                                                                                                                                    \
  bool get##prpty##PointerOwnership()                                                                                                                                                                  \
  {                                                                                                                                                                                                    \
    return m_##prpty##Cleanup;                                                                                                                                                                         \
  }                                                                                                                                                                                                    \
  EBSD_SET_PTR_PROPERTY(type, prpty)                                                                                                                                                                   \
  EBSD_GET_PTR_PROPERTY(type, prpty)

#define EBSD_SET_2DVECTOR_PROPERTY(type, prpty, varname)                                                                                                                                               \
  void set##prpty(const std::array<type, 2>& value)                                                                                                                                                    \
  {                                                                                                                                                                                                    \
    varname[0] = value[0];                                                                                                                                                                             \
    varname[1] = value[1];                                                                                                                                                                             \
  }                                                                                                                                                                                                    \
  void set##prpty(type value_0, type value_1)                                                                                                                                                          \
  {                                                                                                                                                                                                    \
    varname[0] = value_0;                                                                                                                                                                              \
    varname[1] = value_1;                                                                                                                                                                              \
  }

#define EBSD_GET_2DVECTOR_PROPERTY(type, prpty, varname)                                                                                                                                               \
  void get##prpty(std::array<type, 2>& value)                                                                                                                                                          \
  {                                                                                                                                                                                                    \
    value[0] = varname[0];                                                                                                                                                                             \
    value[1] = varname[1];                                                                                                                                                                             \
  }                                                                                                                                                                                                    \
  void get##prpty(type& value_0, type& value_1)                                                                                                                                                        \
  {                                                                                                                                                                                                    \
    value_0 = varname[0];                                                                                                                                                                              \
    value_1 = varname[1];                                                                                                                                                                              \
  }

#define EBSD_INSTANCE_2DVECTOR_PROPERTY(type, prpty)                                                                                                                                                   \
private:                                                                                                                                                                                               \
  std::array<type, 2> m_##prpty;                                                                                                                                                                       \
                                                                                                                                                                                                       \
public:                                                                                                                                                                                                \
  EBSD_SET_2DVECTOR_PROPERTY(type, prpty, m_##prpty)                                                                                                                                                   \
  EBSD_GET_2DVECTOR_PROPERTY(type, prpty, m_##prpty)

/**
* @brief Creates a "setter" method to set the property.
*/
#define EBSD_SET_STRING_PROPERTY( prpty, varname) \
  void set##prpty(const QString &value) { this->varname = value; }\
  void set##prpty(QString* value) { this->varname = *value; }

/**
* @brief Creates a "getter" method to retrieve the value of the property.
*/
#define EBSD_GET_STRING_PROPERTY( prpty, varname) \
  QString get##prpty() { return varname; }

/**
 * @brief Creates setters and getters in the form of 'setXXX()' and 'getXXX()' methods
 */
#define EBSD_INSTANCE_STRING_PROPERTY(prpty)\
  private:\
  QString      m_##prpty;\
  public:\
  EBSD_SET_STRING_PROPERTY(prpty,  m_##prpty)\
  EBSD_GET_STRING_PROPERTY(prpty,  m_##prpty)


#define EBSD_VIRTUAL_INSTANCE_STRING_PROPERTY(prpty)\
  private:\
  QString      m_##prpty;\
  public:\
  virtual EBSD_SET_STRING_PROPERTY(prpty,  m_##prpty)\
  virtual EBSD_GET_STRING_PROPERTY(prpty,  m_##prpty)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
/**
 * @brief Creates a "setter" method to set the property.
 */
#define EbsdHeader_SET_PROPERTY(HeaderType, m_msgType, prpty, key)                                                                                                                                     \
  void set##prpty(const m_msgType& value)                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    auto p = dynamic_cast<HeaderType*>(m_HeaderMap[key].get());                                                                                                                                        \
    if(nullptr != p)                                                                                                                                                                                   \
    {                                                                                                                                                                                                  \
      p->setValue(value);                                                                                                                                                                              \
    }                                                                                                                                                                                                  \
    else                                                                                                                                                                                               \
    {                                                                                                                                                                                                  \
      std::cout << "Setting Property '" << #prpty << "': Value for Key: " << key.toStdString() << " was null." << std::endl;                                                                           \
    }                                                                                                                                                                                                  \
  }

/**
 * @brief Creates a "getter" method to retrieve the value of the property.
 */
#define EbsdHeader_GET_PROPERTY(HeaderType, m_msgType, prpty, key)                                                                                                                                     \
  m_msgType get##prpty()                                                                                                                                                                               \
  {                                                                                                                                                                                                    \
    auto p = dynamic_cast<HeaderType*>(m_HeaderMap[key].get());                                                                                                                                        \
    if(nullptr != p)                                                                                                                                                                                   \
    {                                                                                                                                                                                                  \
      return p->getValue();                                                                                                                                                                            \
    }                                                                                                                                                                                                  \
    std::cout << "Getting Property '" << #prpty << "': Value for Key: " << key.toStdString() << " was null." << std::endl;                                                                             \
    return {};                                                                                                                                                                                         \
  }

#define EBSDHEADER_INSTANCE_PROPERTY(HeaderType, m_msgType, prpty, key)\
  public:\
  EbsdHeader_SET_PROPERTY(HeaderType, m_msgType, prpty, key)\
  EbsdHeader_GET_PROPERTY(HeaderType, m_msgType, prpty, key)

#define EBSD_POINTER_PROPERTY(name, var, type)                                                                                                                                                         \
private:                                                                                                                                                                                               \
  type* m_##var = nullptr;                                                                                                                                                                             \
  bool m_##var##Cleanup = true;                                                                                                                                                                        \
                                                                                                                                                                                                       \
protected:                                                                                                                                                                                             \
  void set##name##Pointer(type* f)                                                                                                                                                                     \
  {                                                                                                                                                                                                    \
    if(m_##var != nullptr && m_##var != f && m_##var##Cleanup)                                                                                                                                         \
    {                                                                                                                                                                                                  \
      deallocateArrayData(m_##var);                                                                                                                                                                    \
      m_##var = nullptr;                                                                                                                                                                               \
    }                                                                                                                                                                                                  \
    m_##var = f;                                                                                                                                                                                       \
  }                                                                                                                                                                                                    \
                                                                                                                                                                                                       \
public:                                                                                                                                                                                                \
  type* get##name##Pointer(bool releaseOwnership = false)                                                                                                                                              \
  {                                                                                                                                                                                                    \
    type* ptr = m_##var;                                                                                                                                                                               \
    if(releaseOwnership)                                                                                                                                                                               \
    {                                                                                                                                                                                                  \
      m_##var##Cleanup = false;                                                                                                                                                                        \
      m_##var = nullptr;                                                                                                                                                                               \
    }                                                                                                                                                                                                  \
    return ptr;                                                                                                                                                                                        \
  }                                                                                                                                                                                                    \
  bool get##name##Ownership()                                                                                                                                                                          \
  {                                                                                                                                                                                                    \
    return m_##var##Cleanup;                                                                                                                                                                           \
  }                                                                                                                                                                                                    \
  void release##name##Ownership()                                                                                                                                                                      \
  {                                                                                                                                                                                                    \
    m_##var##Cleanup = false;                                                                                                                                                                          \
    m_##var = nullptr;                                                                                                                                                                                 \
  }                                                                                                                                                                                                    \
  void free##name##Pointer()                                                                                                                                                                           \
  {                                                                                                                                                                                                    \
    if(nullptr != m_##var)                                                                                                                                                                             \
    {                                                                                                                                                                                                  \
      free(m_##var);                                                                                                                                                                                   \
      m_##var = nullptr;                                                                                                                                                                               \
      m_##var##Cleanup = true;                                                                                                                                                                         \
    }                                                                                                                                                                                                  \
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// These are simple over-rides from the boost distribution because we don't want the entire boost distribution just
// for a few boost headers
namespace Ebsd
{
  class bad_lexical_cast : public std::runtime_error
  {
    public:
      bad_lexical_cast(const QString& s)
        : std::runtime_error(s.toStdString())
      { }
  };

  class bad_any_cast : public std::runtime_error
  {
    public:
      bad_any_cast(const QString& s)
        : std::runtime_error(s.toStdString())
      { }
  };

  template<typename T>
  T lexical_cast(const QString& s)
  {
    std::istringstream i(s.toStdString());
    T x;
    if (!(i >> x))
    { throw bad_lexical_cast("convertToDouble(\"" + s + "\")"); }

    return x;
  }
}

