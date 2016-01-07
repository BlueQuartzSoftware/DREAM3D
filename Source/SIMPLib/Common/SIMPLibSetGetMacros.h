/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
#ifndef _SIMPL_SETGETMACROS_H_
#define _SIMPL_SETGETMACROS_H_

#include <string.h>
#include <sstream>
#include <stdexcept>

/**
 * @file EbsdSEtGetMacros.h
 * @brief This file contains preprocessor macros that can be used to quickly
 * create instance variables, pointer types and other conveniences for a C++ class.
 */

#if defined(QT_CORE_LIB)
#include <QtCore/QString>
#include <QtCore/QtDebug>
//-- Qt includes
#include <QtCore/QSharedPointer>
//#define RAW_PTR  data
#endif

//-- C++11 Includes
#include <memory>


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
#define SIMPL_BENCHMARKS 0

#if SIMPL_BENCHMARKS


#define DEFINE_CLOCK unsigned long long int millis;

#define START_CLOCK millis = QDateTime::currentMSecsSinceEpoch();

#define END_CLOCK(message)\
  qDebug() << message << " Finish Time(ms): " << (QDateTime::currentMSecsSinceEpoch() - millis) ;


#else
#define DEFINE_CLOCK
#define START_CLOCK
#define END_CLOCK
#endif



#define SHARED_IS_NULL(ptr)\
  (  (ptr).get() == NULL )

/**
 * @brief Creates a static method that returns a NULL pointer wrapped in a
 * std::shared_ptr<>
 * @param thisClass The name of the class.
 */
#define SIMPL_NULL_SHARED_POINTER(thisClass)\
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
  SIMPL_NULL_SHARED_POINTER(thisClass)

#endif

/**
 * @brief Creates some basic typedefs that can be used throughout the code to
 * reference the class.
 */
#define SIMPL_SHARED_POINTERS(thisClass)\
  typedef thisClass                      Self;\
  typedef std::shared_ptr<Self >        Pointer;\
  typedef std::shared_ptr<const Self >  ConstPointer;\
  typedef std::weak_ptr<thisClass > WeakPointer;\
  typedef std::weak_ptr<thisClass > ConstWeakPointer;\
  SIMPL_NULL_SHARED_POINTER(thisClass)


#define SIMPL_STATIC_NEW_SUPERCLASS(superclass, theclass)\
  static superclass::Pointer New##superclass()\
  {\
    theclass* ptr = new theclass();\
    superclass::Pointer shared_ptr (dynamic_cast<superclass*>(ptr) );\
    return shared_ptr;\
  }

/**
 * @brief Creates a typedef that points to the superclass of this class
 */
#define SIMPL_SUPERCLASS_POINTER(SuperClass)\
  typedef SuperClass::Pointer SuperClass;


/**
 * @brief Creates a static "New" method that creates an instance of thisClass
 */
#define SIMPL_NEW_SUPERCLASS(thisClass, SuperClass)\
  typedef SuperClass::Pointer SuperClass##Type;\
  static SuperClass##Type New##SuperClass(void) \
  { \
    SuperClass##Type sharedPtr (new thisClass); \
    return sharedPtr; \
  }

/**
 * @brief Implements a Static 'New' Method for a class
 */
#define SIMPL_STATIC_NEW_MACRO(thisClass) \
  static Pointer New(void) \
  { \
    Pointer sharedPtr (new thisClass); \
    return sharedPtr; \
  }

#define SIMPL_STATIC_NEW_MACRO_WITH_ARGS(thisClass, args) \
  static Pointer New args \
  { \
    Pointer sharedPtr (new thisClass); \
    return sharedPtr; \
  }

/** Macro used to add standard methods to all classes, mainly type
 * information. */
#define SIMPL_TYPE_MACRO(thisClass) \
  public: \
  virtual const QString getNameOfClass() {return QString(#thisClass);}\
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


#define SIMPL_TYPE_MACRO_SUPER(thisClass,superclass) \
  public: \
  virtual const QString getNameOfClass() {return QString(#thisClass);}\
  static QString ClassName() {return QString(#thisClass);}\
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



#define SIMPL_CLASS_VERSION(vers)\
  virtual int getClassVersion() { return vers; }


//------------------------------------------------------------------------------
// Macros for Properties
/**
* @brief Creates a QString constant for the Property so that the property
* can be retrieved by name.
*/
#define SIMPL_PROPERTY_CONSTANT(prpty)\
  const QString prpty ( #prpty );

/**
* @brief Creates a "setter" method to set the property.
*/
#define SIMPL_SET_PROPERTY(type, prpty)\
  void set##prpty(type value) { this->m_##prpty = value; }

/**
* @brief Creates a "getter" method to retrieve the value of the property.
*/
#define SIMPL_GET_PROPERTY(type, prpty)\
  type get##prpty() const { return m_##prpty; }

/**
* @brief
*/
#define SIMPL_SET_FILTER_PARAMETER(type, prpty)\
  void set##prpty(type value) { this->m_##prpty = value; emit parametersChanged(); }

/**
* @brief
*/
#define SIMPL_VIRTUAL_INSTANCE_PROPERTY(type, prpty)\
  private:\
  type   m_##prpty;\
  public:\
  virtual SIMPL_SET_PROPERTY(type, prpty)\
  virtual SIMPL_GET_PROPERTY(type, prpty)


#define SIMPL_INSTANCE_PROPERTY(type, prpty)\
  private:\
  type   m_##prpty;\
  public:\
  SIMPL_SET_PROPERTY(type, prpty)\
  SIMPL_GET_PROPERTY(type, prpty)

#define SIMPL_PRIVATE_INSTANCE_PROPERTY(type, prpty)\
  private:\
  type   m_##prpty;\
  SIMPL_SET_PROPERTY(type, prpty)\
  public:\
  SIMPL_GET_PROPERTY(type, prpty)



#define SIMPL_PIMPL_PROPERTY_DECL(type, prpty)\
  public:\
  void set##prpty(type value);\
  type get##prpty() const;


#define SIMPL_PIMPL_PROPERTY_DEF(Class, type, prpty)\
  void Class::set##prpty(type value) {\
    Q_D(Class);\
    d->m_##prpty = value;\
  }\
  type Class::get##prpty() const {\
    Q_D(const Class);\
    return d->m_##prpty;\
  }


#define SIMPL_OVERLOAD_PROPERTY(type, prpty, overload)\
  private:\
  type m_##prpty;\
  public:\
  SIMPL_SET_PROPERTY(overload, prpty)\
  SIMPL_GET_PROPERTY(overload, prpty)

#define SIMPL_BOOL_PROPERTY(prpty)\
  private:\
  bool m_##prpty;\
  public:\
  SIMPL_SET_PROPERTY(bool, prpty)\
  bool is##prpty() { return m_##prpty; }





/**
* @brief
*/
#define SIMPL_FILTER_PARAMETER(type, prpty)\
  private:\
  type   m_##prpty;\
  public:\
  SIMPL_SET_PROPERTY(type, prpty)\
  SIMPL_GET_PROPERTY(type, prpty)

#define SIMPL_SET_2DVECTOR_PROPERTY(type, prpty, varname)\
  void set##prpty(type value[2]) {\
    varname[0] = value[0]; varname[1] = value[1]; }\
  void set##prpty(type value_0, type value_1) {\
    varname[0] = value_0; varname[1] = value_1; }

#define SIMPL_GET_2DVECTOR_PROPERTY(type, prpty, varname)\
  void get##prpty(type value[2]) {\
    value[0] = varname[0]; value[1] = varname[1]; }\
  void get##prpty(type &value_0, type &value_1) {\
    value_0 = varname[0]; value_1 = varname[1]; }


#define SIMPL_INSTANCE_VEC2_PROPERTY(type, prpty)\
  private:\
  type   m_##prpty[2];\
  public:\
  SIMPL_SET_2DVECTOR_PROPERTY(type, prpty, m_##prpty)\
  SIMPL_GET_2DVECTOR_PROPERTY(type, prpty, m_##prpty)


#define SIMPL_SET_VEC3_PROPERTY(type, prpty, varname)\
  void set##prpty(type value[3]) {\
    varname[0] = value[0]; varname[1] = value[1]; varname[2] = value[2]; }\
  void set##prpty(type value_0, type value_1, type value_2) {\
    varname[0] = value_0; varname[1] = value_1; varname[2] = value_2; }

#define SIMPL_GET_VEC3_PROPERTY(type, prpty, varname)\
  void get##prpty(type value[3]) {\
    value[0] = varname[0]; value[1] = varname[1]; value[2] = varname[2]; }\
  void get##prpty(type &value_0, type &value_1, type &value_2) {\
    value_0 = varname[0]; value_1 = varname[1]; value_2 = varname[2]; }


#define SIMPL_INSTANCE_VEC3_PROPERTY(type, prpty)\
  private:\
  type   m_##prpty[3];\
  public:\
  SIMPL_SET_VEC3_PROPERTY(type, prpty, m_##prpty)\
  SIMPL_GET_VEC3_PROPERTY(type, prpty, m_##prpty)



#define SIMPL_CONTAINER_TYPE(thisClass, container) \
  typedef container<thisClass >     ContainerT; \
  typedef std::shared_ptr< container<thisClass > > ContainerPType;


/**
* @brief Creates a "setter" method to set the property.
*/
#define SIMPL_SET_STRING_PROPERTY( prpty, varname) \
  void set##prpty(const QString &value) { this->varname = value; }

/**
* @brief Creates a "getter" method to retrieve the value of the property.
*/
#define SIMPL_GET_STRING_PROPERTY( prpty, varname) \
  QString get##prpty() const { return varname; }

/**
 * @brief Creates setters and getters in the form of 'setXXX()' and 'getXXX()' methods
 */
#define SIMPL_INSTANCE_STRING_PROPERTY(prpty)\
  private:\
  QString      m_##prpty;\
  public:\
  SIMPL_SET_STRING_PROPERTY(prpty,  m_##prpty)\
  SIMPL_GET_STRING_PROPERTY(prpty,  m_##prpty)


#define SIMPL_VIRTUAL_INSTANCE_STRING_PROPERTY(prpty)\
  private:\
  QString      m_##prpty;\
  public:\
  virtual SIMPL_SET_STRING_PROPERTY(prpty,  m_##prpty)\
  virtual SIMPL_GET_STRING_PROPERTY(prpty,  m_##prpty)


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#define SIMPL_DECLARE_ARRAY(Type, ptr, prpty )\
  private:\
  DataArray<Type>::Pointer m_##prpty;\
  Type* ptr;\
  public:\
  SIMPL_SET_PROPERTY(DataArray<Type>::Pointer, prpty)\
  SIMPL_GET_PROPERTY(DataArray<Type>::Pointer, prpty)



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
/**
 * @brief Creates a "setter" method to set the property.
 */
#define SIMPL_Header_SET_PROPERTY( HeaderType, type, prpty, key) \
  void set##prpty(type value) { \
    HeaderType* p = dynamic_cast<HeaderType*>(m_Headermap[key].get()); \
    if (NULL != p) { p->setValue(value); } else {\
      qDebug() << "Value for Key: " << key << " was null." ;} }

/**
 * @brief Creates a "getter" method to retrieve the value of the property.
 */
#define SIMPL_Header_GET_PROPERTY(HeaderType, type, prpty, key) \
  type get##prpty() { \
    HeaderType* p = dynamic_cast<HeaderType*>(m_Headermap[key].get());\
    if (NULL != p) { return p->getValue(); } else {\
      qDebug() << "Value for Key: " << key << " was null." ; return 0;} }


#define SIMPL_Header_INSTANCE_PROPERTY(HeaderType, type, prpty, key)\
  public:\
  SIMPL_Header_SET_PROPERTY(HeaderType, type, prpty, key)\
  SIMPL_Header_GET_PROPERTY(HeaderType, type, prpty, key)


#define SIMPL_POINTER_PROPERTY(name, var, type)\
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


/**
*
* */
#define SIMPL_FILTER_WRITE_PARAMETER(property)\
  writer->writeValue(#property, get##property() );




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#define DEFINE_DATAARRAY_VARIABLE(type, name)\
  private:\
  DataArray<type>::WeakPointer m_##name##Ptr;\
  type* m_##name;

#define DEFINE_REQUIRED_VARIABLE(type, name)\
  SIMPL_INSTANCE_STRING_PROPERTY(name##ArrayName);\
  private:\
  type::WeakPointer m_##name##Ptr;\
  type* m_##name;

//used in place of 'DEFINE_DATAARRAY_VARIABLE' in filter header
#define DEFINE_IDATAARRAY_VARIABLE(varName)\
  private:\
  IDataArray::WeakPointer m_##varName##Ptr;\
  void* m_##varName;

#define SIMPL_COPY_INSTANCEVAR(name)\
  filter->set##name(get##name());


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#define CREATE_INPUT_FILENAME(f, n)\
  QString f = m_InputDirectory + QDir::Separator + n;\
  f = QDir::toNativeSeparators(f);

#define CREATE_OUTPUT_FILENAME(f, n)\
  QString f = m_InputDirectory + QDir::Separator + n;\
  f = QDir::toNativeSeparators(f);

#define CHECK_FOR_CANCELED(FuncClass, Message, name)\
  if (this->getCancel() ) { \
    updatePipelineMessage(#Message);\
    updatePipelineProgress(0);\
    pipelineFinished();\
    return;}\


#define CHECK_FOR_ERROR(FuncClass, Message, err)\
  if(err < 0) {\
    setErrorCondition(err);\
    QString msg = QString(Message);\
    pipelineErrorMessage(msg.toLatin1().data());\
    updatePipelineProgress(0);\
    pipelineFinished();\
    return;   }


#define MAKE_OUTPUT_FILE_PATH(outpath, filename)\
  QString outpath = m_OutputDirectory + "/" + m_OutputFilePrefix + filename;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  /**
    * @brief Macro to silence compiler warnings for unused parameters in methods.
    */
#define SIMPL_NOT_USED(x)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// These are simple over-rides from the boost distribution because we don't want the entire boost distribution just
// for a few boost headers
  namespace SIMPL_
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




#endif /* SIMPL_SETGETMACROS_H_ */

