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

#ifndef DREAM3DSETGETMACROS_H_
#define DREAM3DSETGETMACROS_H_

#include <string.h>

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

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

//-- Boost Includes
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>



#define COPY_ARRAY_3(var, obj)\
  var[0] = obj->var[0];var[1] = obj->var[1];var[2] = obj->var[2];

#define COPY_ARRAY_4(var, obj)\
  var[0] = obj->var[0];var[1] = obj->var[1];\
  var[2] = obj->var[2]; var[3] = obj->var[3];

#define COPY_ARRAY_5(var, obj)\
  var[0] = obj->var[0];var[1] = obj->var[1];\
  var[2] = obj->var[2]; var[3] = obj->var[3];\
  var[4] = obj->var[4];


/**
 * This will perform a deep copy of the content of the shared vector from
 * target to destination. This is NOT just a simple pointer copy.
 */
#define DEEP_COPY_SHARED_VECTOR(sharedPtr, obj, VType, m_msgType)\
  if (NULL != sharedPtr.get())\
{\
  sharedPtr = VType(static_cast<std::vector<m_msgType>*>(NULL));\
  }\
  if (NULL != obj->sharedPtr.get())\
{\
  sharedPtr = VType(new std::vector<m_msgType>(*(obj->sharedPtr.get())));\
  }



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#define DREAM3D_BENCHMARKS 0

#if DREAM3D_BENCHMARKS
#include "MXA/Common/LogTime.h"

#define DEFINE_CLOCK unsigned long long int millis;

#define START_CLOCK millis = MXA::getMilliSeconds();

#define END_CLOCK(message)\
  std::cout << message << " Finish Time(ms): " << (MXA::getMilliSeconds() - millis) << std::endl;


#else
#define DEFINE_CLOCK
#define START_CLOCK
#define END_CLOCK
#endif



#define SHARED_IS_NULL(ptr)\
  (  (ptr).get() == NULL )

/**
 * @brief Creates a static method that returns a NULL pointer wrapped in a
 * boost::shared_ptr<>
 * @param thisClass The name of the class.
 */
#define DREAM3D_NULL_SHARED_POINTER(thisClass)\
  static Pointer NullPointer(void)\
  { \
    return Pointer(static_cast<thisClass*>(NULL));\
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
  DREAM3D_NULL_SHARED_POINTER(thisClass)

#endif

/**
 * @brief Creates some basic typedefs that can be used throughout the code to
 * reference the class.
 */
#define DREAM3D_SHARED_POINTERS(thisClass)\
  typedef thisClass                      Self;\
  typedef boost::shared_ptr<Self >        Pointer;\
  typedef boost::shared_ptr<const Self >  ConstPointer;\
  typedef boost::weak_ptr<thisClass > WeakPointer;\
  typedef boost::weak_ptr<thisClass > ConstWeakPointer;\
  DREAM3D_NULL_SHARED_POINTER(thisClass)


#define DREAM3D_STATIC_NEW_SUPERCLASS(superclass, theclass)\
  static superclass::Pointer New##superclass()\
  {\
    theclass* ptr = new theclass();\
    superclass::Pointer shared_ptr (dynamic_cast<superclass*>(ptr) );\
    return shared_ptr;\
  }

/**
 * @brief Creates a typedef that points to the superclass of this class
 */
#define DREAM3D_SUPERCLASS_POINTER(SuperClass)\
  typedef SuperClass::Pointer SuperClass;


/**
 * @brief Creates a static "New" method that creates an instance of thisClass
 */
#define DREAM3D_NEW_SUPERCLASS(thisClass, SuperClass)\
  typedef SuperClass::Pointer SuperClass##Type;\
  static SuperClass##Type New##SuperClass(void) \
{ \
  SuperClass##Type sharedPtr (new thisClass); \
  return sharedPtr; \
}

/**
 * @brief Implements a Static 'New' Method for a class
 */
#define DREAM3D_STATIC_NEW_MACRO(thisClass) \
static Pointer New(void) \
{ \
  Pointer sharedPtr (new thisClass); \
  return sharedPtr; \
}

#define DREAM3D_STATIC_NEW_MACRO_WITH_ARGS(thisClass, args) \
static Pointer New args \
{ \
  Pointer sharedPtr (new thisClass); \
  return sharedPtr; \
}

/** Macro used to add standard methods to all classes, mainly type
 * information. */
#define DREAM3D_TYPE_MACRO(thisClass) \
  public: \
  virtual const std::string getNameOfClass() {return std::string(#thisClass);}\
  static int IsTypeOf(const char *type) \
  { \
    if ( !strcmp(#thisClass,type) ) \
      { \
      return 1; \
      } \
    return 0; \
  } \
  virtual int IsA(const char *type) \
  { \
    return this->thisClass::IsTypeOf(type); \
  } \
  template <class Source, class Target>\
  inline Target SafeObjectDownCast(Source x) { \
      if( dynamic_cast<Target>(x) != x ) { \
        return NULL;\
      }\
      return static_cast<Target>(x);\
  }


#define DREAM3D_TYPE_MACRO_SUPER(thisClass,superclass) \
  public: \
  virtual const std::string getNameOfClass() {return std::string(#thisClass);}\
  static std::string ClassName() {return std::string(#thisClass);}\
  static int IsTypeOf(const char *type) \
  { \
    if ( !strcmp(#thisClass,type) ) \
      { \
      return 1; \
      } \
    return superclass::IsTypeOf(type); \
  } \
  virtual int IsA(const char *type) \
  { \
    return this->thisClass::IsTypeOf(type); \
  } \
  template <class Source, class Target>\
  static Target SafeObjectDownCast(Source x) { \
      if( dynamic_cast<Target>(x) != x ) { \
        return NULL;\
      }\
      return static_cast<Target>(x);\
  }\
  static thisClass* SafePointerDownCast(superclass* s) {\
    return SafeObjectDownCast<superclass*, thisClass*>(s);\
  }



//------------------------------------------------------------------------------
// Macros for Properties
/**
* @brief Creates a std::string constant for the Property so that the property
* can be retrieved by name.
*/
#define DREAM3D_PROPERTY_CONSTANT(prpty) \
  const std::string prpty ( #prpty );

/**
* @brief Creates a "setter" method to set the property.
*/
#define DREAM3D_SET_PROPERTY(type, prpty) \
  void set##prpty(type value) { this->m_##prpty = value; }

/**
* @brief Creates a "getter" method to retrieve the value of the property.
*/
#define DREAM3D_GET_PROPERTY(type, prpty) \
  type get##prpty() { return m_##prpty; }



#define DREAM3D_VIRTUAL_INSTANCE_PROPERTY(type, prpty)\
  private:\
      type   m_##prpty;\
  public:\
    virtual DREAM3D_SET_PROPERTY(type, prpty)\
    virtual DREAM3D_GET_PROPERTY(type, prpty)


#define DREAM3D_INSTANCE_PROPERTY(type, prpty)\
  private:\
      type   m_##prpty;\
  public:\
    DREAM3D_SET_PROPERTY(type, prpty)\
    DREAM3D_GET_PROPERTY(type, prpty)



#define DREAM3D_SET_2DVECTOR_PROPERTY(type, prpty, varname)\
  void set##prpty(type value[2]) {\
      varname[0] = value[0]; varname[1] = value[1]; }\
  void set##prpty(type value_0, type value_1) {\
      varname[0] = value_0; varname[1] = value_1; }

#define DREAM3D_GET_2DVECTOR_PROPERTY(type, prpty, varname)\
  void get##prpty(type value[2]) {\
      value[0] = varname[0]; value[1] = varname[1]; }\
  void get##prpty(type &value_0, type &value_1) {\
      value_0 = varname[0]; value_1 = varname[1]; }


#define DREAM3D_INSTANCE_VEC2_PROPERTY(type, prpty)\
  private:\
    type   m_##prpty[2];\
  public:\
    DREAM3D_SET_2DVECTOR_PROPERTY(type, prpty, m_##prpty)\
    DREAM3D_GET_2DVECTOR_PROPERTY(type, prpty, m_##prpty)


#define DREAM3D_SET_VEC3_PROPERTY(type, prpty, varname)\
  void set##prpty(type value[3]) {\
      varname[0] = value[0]; varname[1] = value[1]; varname[2] = value[2]; }\
  void set##prpty(type value_0, type value_1, type value_2) {\
      varname[0] = value_0; varname[1] = value_1; varname[2] = value_2; }

#define DREAM3D_GET_VEC3_PROPERTY(type, prpty, varname)\
  void get##prpty(type value[3]) {\
      value[0] = varname[0]; value[1] = varname[1]; value[2] = varname[2]; }\
  void get##prpty(type &value_0, type &value_1, type &value_2) {\
      value_0 = varname[0]; value_1 = varname[1]; value_2 = varname[2]; }


#define DREAM3D_INSTANCE_VEC3_PROPERTY(type, prpty)\
  private:\
    type   m_##prpty[3];\
  public:\
    DREAM3D_SET_VEC3_PROPERTY(type, prpty, m_##prpty)\
    DREAM3D_GET_VEC3_PROPERTY(type, prpty, m_##prpty)



#define DREAM3D_CONTAINER_TYPE(thisClass, container) \
    typedef container<thisClass >     ContainerT; \
    typedef boost::shared_ptr< container<thisClass > > ContainerPType;


/**
* @brief Creates a "setter" method to set the property.
*/
#define DREAM3D_SET_STRING_PROPERTY( prpty, varname) \
  void set##prpty(const std::string &value) { this->varname = value; }

/**
* @brief Creates a "getter" method to retrieve the value of the property.
*/
#define DREAM3D_GET_STRING_PROPERTY( prpty, varname) \
  std::string get##prpty() { return varname; }

/**
 * @brief Creates setters and getters in the form of 'setXXX()' and 'getXXX()' methods
 */
#define DREAM3D_INSTANCE_STRING_PROPERTY(prpty)\
  private:\
  std::string      m_##prpty;\
  public:\
  DREAM3D_SET_STRING_PROPERTY(prpty,  m_##prpty)\
  DREAM3D_GET_STRING_PROPERTY(prpty,  m_##prpty)


#define DREAM3D_VIRTUAL_INSTANCE_STRING_PROPERTY(prpty)\
  private:\
  std::string      m_##prpty;\
  public:\
    virtual DREAM3D_SET_STRING_PROPERTY(prpty,  m_##prpty)\
    virtual DREAM3D_GET_STRING_PROPERTY(prpty,  m_##prpty)


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#define DREAM3D_DECLARE_ARRAY(Type, ptr, prpty )\
  private:\
  DataArray<Type>::Pointer m_##prpty;\
  Type* ptr;\
  public:\
  DREAM3D_SET_PROPERTY(DataArray<Type>::Pointer, prpty)\
  DREAM3D_GET_PROPERTY(DataArray<Type>::Pointer, prpty)



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
/**
 * @brief Creates a "setter" method to set the property.
 */
#define DREAM3DHeader_SET_PROPERTY( HeaderType, type, prpty, key) \
  void set##prpty(type value) { \
    HeaderType* p = dynamic_cast<HeaderType*>(m_Headermap[key].get()); \
    if (NULL != p) { p->setValue(value); } else {\
      std::cout << "Value for Key: " << key << " was null." << std::endl;} }

/**
 * @brief Creates a "getter" method to retrieve the value of the property.
 */
#define DREAM3DHeader_GET_PROPERTY(HeaderType, type, prpty, key) \
  type get##prpty() { \
    HeaderType* p = dynamic_cast<HeaderType*>(m_Headermap[key].get());\
    if (NULL != p) { return p->getValue(); } else {\
      std::cout << "Value for Key: " << key << " was null." << std::endl; return 0;} }


#define DREAM3DHeader_INSTANCE_PROPERTY(HeaderType, type, prpty, key)\
  public:\
    DREAM3DHeader_SET_PROPERTY(HeaderType, type, prpty, key)\
    DREAM3DHeader_GET_PROPERTY(HeaderType, type, prpty, key)


#define DREAM3D_POINTER_PROPERTY(name, var, type)\
private:\
  type* m_##var;\
public:\
type* get##name##Pointer() { return m_##var; }\
void set##name##Pointer(type* f)\
  {\
    if (m_##var != NULL && m_##var != f)\
    {\
      deallocateArrayData(m_##var);\
      m_##var = NULL;\
    }\
    m_##var = f;\
  }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#define CREATE_INPUT_FILENAME(f, n)\
    std::string f = m_InputDirectory + MXADir::Separator + n;\
    f = MXADir::toNativeSeparators(f);

#define CREATE_OUTPUT_FILENAME(f, n)\
    std::string f = m_InputDirectory + MXADir::Separator + n;\
    f = MXADir::toNativeSeparators(f);

#define CHECK_FOR_CANCELED(FuncClass, Message, name)\
    if (this->getCancel() ) { \
              updatePipelineMessage(#Message);\
              updatePipelineProgress(0);\
              pipelineFinished();\
      return;}\


#define CHECK_FOR_ERROR(FuncClass, Message, err)\
    if(err < 0) {\
      setErrorCondition(err);\
      std::string msg = std::string(Message);\
      pipelineErrorMessage(msg.c_str());\
      updatePipelineProgress(0);\
      pipelineFinished();\
      return;   }


#define MAKE_OUTPUT_FILE_PATH(outpath, filename)\
    std::string outpath = m_OutputDirectory + MXADir::Separator + m_OutputFilePrefix + filename;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// These are simple over-rides from the boost distribution because we don't want the entire boost distribution just
// for a few boost headers
namespace DREAM3D
{
  class bad_lexical_cast : public std::runtime_error {
  public:
    bad_lexical_cast(const std::string& s)
      : std::runtime_error(s)
    { }
  };

  class bad_any_cast : public std::runtime_error {
  public:
    bad_any_cast(const std::string& s)
      : std::runtime_error(s)
    { }
  };

  template<typename T>
  T lexical_cast(const std::string &s)
  {
    std::istringstream i(s);
    T x;
    if (!(i >> x))
      throw bad_lexical_cast("convertToDouble(\"" + s + "\")");

    return x;
  }
}




#endif /* DREAM3DSETGETMACROS_H_ */
