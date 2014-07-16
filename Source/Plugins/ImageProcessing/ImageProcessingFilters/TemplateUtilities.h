/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _TemplateMacros_H_
#define _TemplateMacros_H_

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

namespace TemplateUtilities
{
  namespace DataContainerTypes
  {
    const QString Bool("bool");
    const QString Float("float");
    const QString Double("double");
    const QString Int8("int8_t");
    const QString UInt8("uint8_t");
    const QString Int16("int16_t");
    const QString UInt16("uint16_t");
    const QString Int32("int32_t");
    const QString UInt32("uint32_t");
    const QString Int64("int64_t");
    const QString UInt64("uint64_t");
  }

  namespace Errors
  {
    const int UnsupportedType(-401);
    const int MissingDataContainer(-402);
    const int MissingAttributeMatrix(-403);
    const int MissingArray(-404);
  }
}

//used in filter::execute method to run the correct version of a templated function
#define TEMPLATE_EXECUTE_FUNCTION(TEMPLATE_EXECUTE_FUNCTION_function, TEMPLATE_EXECUTE_FUNCTION_type, ...)\
  {\
    if( 0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::Bool) ) { TEMPLATE_EXECUTE_FUNCTION_function##<bool>( __VA_ARGS__); }\
    else if( 0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::Float) ) { TEMPLATE_EXECUTE_FUNCTION_function##<float>( __VA_ARGS__); }\
    else if( 0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::Double) ){ TEMPLATE_EXECUTE_FUNCTION_function##<double>( __VA_ARGS__); }\
    else if( 0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::Int8) ) { TEMPLATE_EXECUTE_FUNCTION_function##<int8_t>( __VA_ARGS__); }\
    else if( 0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::UInt8) ) { TEMPLATE_EXECUTE_FUNCTION_function##<uint8_t>( __VA_ARGS__); }\
    else if( 0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::Int16) ) { TEMPLATE_EXECUTE_FUNCTION_function##<int16_t>( __VA_ARGS__); }\
    else if( 0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::UInt16) ) { TEMPLATE_EXECUTE_FUNCTION_function##<uint16_t>( __VA_ARGS__); }\
    else if( 0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::Int32) ) { TEMPLATE_EXECUTE_FUNCTION_function##<int32_t>( __VA_ARGS__); }\
    else if( 0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::UInt32) ) { TEMPLATE_EXECUTE_FUNCTION_function##<uint32_t>( __VA_ARGS__); }\
    else if( 0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::Int64) ) { TEMPLATE_EXECUTE_FUNCTION_function##<int64_t>( __VA_ARGS__); }\
    else if( 0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::UInt64) ) { TEMPLATE_EXECUTE_FUNCTION_function##<uint64_t>( __VA_ARGS__); }\
  }

//used in place of 'DEFINE_REQUIRED_DATAARRAY_VARIABLE' in filter header
#define TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE(TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name)\
    DREAM3D_INSTANCE_STRING_PROPERTY(TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name##ArrayName);\
    private:\
    DataArray<bool>::WeakPointer m_TemplateBool##TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<float>::WeakPointer m_TemplateFloat##TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<double>::WeakPointer m_TemplateDouble##TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<int8_t>::WeakPointer m_TemplateInt8##TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<uint8_t>::WeakPointer m_TemplateUInt8##TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<int16_t>::WeakPointer m_TemplateInt16##TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<uint16_t>::WeakPointer m_TemplateUInt16##TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<int32_t>::WeakPointer m_TemplateInt32##TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<uint32_t>::WeakPointer m_TemplateUInt32##TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<int64_t>::WeakPointer m_TemplateInt64##TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<uint64_t>::WeakPointer m_TemplateUInt64##TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name##Ptr;\
    void* m_##TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name;

//used in place of 'DEFINE_CREATED_DATAARRAY_VARIABLE' in filter header
#define TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE(TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE_name)\
    private:\
    DataArray<bool>::WeakPointer m_TemplateBool##TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<float>::WeakPointer m_TemplateFloat##TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<double>::WeakPointer m_TemplateDouble##TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<int8_t>::WeakPointer m_TemplateInt8##TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<uint8_t>::WeakPointer m_TemplateUInt8##TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<int16_t>::WeakPointer m_TemplateInt16##TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<uint16_t>::WeakPointer m_TemplateUInt16##TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<int32_t>::WeakPointer m_TemplateInt32##TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<uint32_t>::WeakPointer m_TemplateUInt32##TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<int64_t>::WeakPointer m_TemplateInt64##TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE_name##Ptr;\
    DataArray<uint64_t>::WeakPointer m_TemplateUInt64##TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE_name##Ptr;\
    void* m_##TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE_name;


//used in preflight to check if array: exists, is compatible type, and has correct # componenets per voxel
#define TEMPLATE_GET_PREREQ_ARRAY(TEMPLATE_GET_PREREQ_ARRAY_arrayname, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims)\
  {\
    VolumeDataContainer* TEMPLATE_GET_PREREQ_ARRAY_m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##ArrayPath.getDataContainerName(), false);\
    if(getErrorCondition() < 0 || NULL == TEMPLATE_GET_PREREQ_ARRAY_m) {\
      QString TEMPLATE_GET_PREREQ_ARRAY_message = QObject::tr("The Data Container '%1' does not exist").arg(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##ArrayPath.getDataContainerName());\
      setErrorCondition(TemplateUtilities::Errors::MissingDataContainer);\
      notifyErrorMessage(getHumanLabel(), TEMPLATE_GET_PREREQ_ARRAY_message, getErrorCondition());\
      return;\
    }\
    AttributeMatrix::Pointer TEMPLATE_GET_PREREQ_ARRAY_cellAttrMat = TEMPLATE_GET_PREREQ_ARRAY_m->getPrereqAttributeMatrix<AbstractFilter>(this, m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##ArrayPath.getAttributeMatrixName(), TemplateUtilities::Errors::MissingAttributeMatrix);\
    if(getErrorCondition() < 0 || NULL == TEMPLATE_GET_PREREQ_ARRAY_cellAttrMat.get()) {\
      QString TEMPLATE_GET_PREREQ_ARRAY_message = QObject::tr("The Attribute Matrix '%1' does not exist").arg(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##ArrayPath.getAttributeMatrixName());\
      setErrorCondition(TemplateUtilities::Errors::MissingAttributeMatrix);\
      notifyErrorMessage(getHumanLabel(), TEMPLATE_GET_PREREQ_ARRAY_message, getErrorCondition());\
      return;\
    }\
    IDataArray::Pointer TEMPLATE_GET_PREREQ_ARRAY_p = TEMPLATE_GET_PREREQ_ARRAY_cellAttrMat->getAttributeArray(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##ArrayPath.getDataArrayName());\
    if (NULL == TEMPLATE_GET_PREREQ_ARRAY_p.get()) {\
      QString TEMPLATE_GET_PREREQ_ARRAY_message = QObject::tr("The input array '%1' was not found in the AttributeMatrix '%2'.").arg(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##ArrayName).arg(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##ArrayPath.getAttributeMatrixName());\
      setErrorCondition(TemplateUtilities::Errors::MissingArray);\
      notifyErrorMessage(getHumanLabel(), TEMPLATE_GET_PREREQ_ARRAY_message, getErrorCondition());\
      return;\
    }\
    QString TEMPLATE_GET_PREREQ_ARRAY_type = TEMPLATE_GET_PREREQ_ARRAY_p->getTypeAsString();\
    if( 0==TEMPLATE_GET_PREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Bool) ) {\
      m_TemplateBool##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
      if( NULL != m_TemplateBool##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateBool##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_GET_PREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Float) ) {\
      m_TemplateFloat##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
      if( NULL != m_TemplateFloat##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateFloat##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_GET_PREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Double) ){\
      m_TemplateDouble##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
      if( NULL != m_TemplateDouble##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateDouble##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_GET_PREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Int8) ) {\
      m_TemplateInt8##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
      if( NULL != m_TemplateInt8##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateInt8##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_GET_PREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::UInt8) ) {\
      m_TemplateUInt8##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
      if( NULL != m_TemplateUInt8##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateUInt8##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_GET_PREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Int16) ) {\
      m_TemplateInt16##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int16_t>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
      if( NULL != m_TemplateInt16##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateInt16##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_GET_PREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::UInt16) ) {\
      m_TemplateUInt16##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint16_t>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
      if( NULL != m_TemplateUInt16##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateUInt16##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_GET_PREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Int32) ) {\
      m_TemplateInt32##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
      if( NULL != m_TemplateInt32##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateInt32##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_GET_PREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::UInt32) ) {\
      m_TemplateUInt32##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
      if( NULL != m_TemplateUInt32##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateUInt32##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_GET_PREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Int64) ) {\
      m_TemplateInt64##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
      if( NULL != m_TemplateInt64##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateInt64##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_GET_PREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::UInt64) ) {\
      m_TemplateUInt64##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint64_t>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
      if( NULL != m_TemplateUInt64##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateUInt64##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else {\
      QString TEMPLATE_GET_PREREQ_ARRAY_typelist = TemplateUtilities::DataContainerTypes::Bool+", "+TemplateUtilities::DataContainerTypes::Float+", "+TemplateUtilities::DataContainerTypes::Double+", "+TemplateUtilities::DataContainerTypes::Int8+", "+TemplateUtilities::DataContainerTypes::UInt8+", "+TemplateUtilities::DataContainerTypes::Int16+", "+TemplateUtilities::DataContainerTypes::UInt16+", "+TemplateUtilities::DataContainerTypes::Int32+", "+TemplateUtilities::DataContainerTypes::UInt32+", "+TemplateUtilities::DataContainerTypes::Int64+", "+TemplateUtilities::DataContainerTypes::UInt64;\
      QString TEMPLATE_GET_PREREQ_ARRAY_message = QObject::tr("The input array %1 is of unsupported type '%2'. The following types are supported: %3").arg(TEMPLATE_GET_PREREQ_ARRAY_arraypath.getDataArrayName()).arg(TEMPLATE_GET_PREREQ_ARRAY_type).arg(TEMPLATE_GET_PREREQ_ARRAY_typelist);\
      setErrorCondition(TemplateUtilities::Errors::UnsupportedType);\
      notifyErrorMessage(getHumanLabel(), TEMPLATE_GET_PREREQ_ARRAY_message, getErrorCondition());\
    }\
  }

//used during preflight to create an array: at the specified path, of the specified type, and of the specified # components per voxel
#define TEMPLATE_CREATE_NONPREREQ_ARRAY(TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims, TEMPLATE_CREATE_NONPREREQ_ARRAY_type)\
  {\
    if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Bool) ) {\
      m_TemplateBool##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
      if( NULL != m_TemplateBool##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateBool##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Float) ) {\
      m_TemplateFloat##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
      if( NULL != m_TemplateFloat##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateFloat##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Double) ){\
      m_TemplateDouble##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
      if( NULL != m_TemplateDouble##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateDouble##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Int8) ) {\
      m_TemplateInt8##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter, int8_t>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
      if( NULL != m_TemplateInt8##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateInt8##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::UInt8) ) {\
      m_TemplateUInt8##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
      if( NULL != m_TemplateUInt8##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateUInt8##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Int16) ) {\
      m_TemplateInt16##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int16_t>, AbstractFilter, int16_t>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
      if( NULL != m_TemplateInt16##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateInt16##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::UInt16) ) {\
      m_TemplateUInt16##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint16_t>, AbstractFilter, uint16_t>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
      if( NULL != m_TemplateUInt16##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateUInt16##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Int32) ) {\
      m_TemplateInt32##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
      if( NULL != m_TemplateInt32##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateInt32##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::UInt32) ) {\
      m_TemplateUInt32##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
      if( NULL != m_TemplateUInt32##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateUInt32##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Int64) ) {\
      m_TemplateInt64##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter, int64_t>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
      if( NULL != m_TemplateInt64##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateInt64##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::UInt64) ) {\
      m_TemplateUInt64##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint64_t>, AbstractFilter, uint64_t>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
      if( NULL != m_TemplateUInt64##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock().get() )\
      { m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname = static_cast<void*>(m_TemplateUInt64##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock()->getPointer(0)); }\
    }\
    else {\
      QString TEMPLATE_CREATE_NONPREREQ_ARRAY_typelist = TemplateUtilities::DataContainerTypes::Bool+", "+TemplateUtilities::DataContainerTypes::Float+", "+TemplateUtilities::DataContainerTypes::Double+", "+TemplateUtilities::DataContainerTypes::Int8+", "+TemplateUtilities::DataContainerTypes::UInt8+", "+TemplateUtilities::DataContainerTypes::Int16+", "+TemplateUtilities::DataContainerTypes::UInt16+", "+TemplateUtilities::DataContainerTypes::Int32+", "+TemplateUtilities::DataContainerTypes::UInt32+", "+TemplateUtilities::DataContainerTypes::Int64+", "+TemplateUtilities::DataContainerTypes::UInt64;\
      QString TEMPLATE_CREATE_NONPREREQ_ARRAY_message = QObject::tr("The created array '%1' is of unsupported type '%2'. The following types are supported: %3").arg(TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath.getDataArrayName()).arg(TEMPLATE_CREATE_NONPREREQ_ARRAY_type).arg(TEMPLATE_CREATE_NONPREREQ_ARRAY_typelist);\
      setErrorCondition(TemplateUtilities::Errors::UnsupportedType);\
      notifyErrorMessage(getHumanLabel(), TEMPLATE_CREATE_NONPREREQ_ARRAY_message, getErrorCondition());\
    }\
  }

//used in preflight to check if appopriate type
#define TEMPLATE_CHECK_TYPE_SUPPORT(TEMPLATE_CHECK_TYPE_SUPPORT_arrayname, TEMPLATE_CHECK_TYPE_SUPPORT_type)\
  {\
    bool TEMPLATE_CHECK_TYPE_SUPPORT_supportedType = false;\
    if( 0==TEMPLATE_CHECK_TYPE_SUPPORT_type.compare(TemplateUtilities::DataContainerTypes::Bool) ) { TEMPLATE_CHECK_TYPE_SUPPORT_supportedType=true; }\
    else if( 0==TEMPLATE_CHECK_TYPE_SUPPORT_type.compare(TemplateUtilities::DataContainerTypes::Float) ) { TEMPLATE_CHECK_TYPE_SUPPORT_supportedType=true; }\
    else if( 0==TEMPLATE_CHECK_TYPE_SUPPORT_type.compare(TemplateUtilities::DataContainerTypes::Double) ){ TEMPLATE_CHECK_TYPE_SUPPORT_supportedType=true; }\
    else if( 0==TEMPLATE_CHECK_TYPE_SUPPORT_type.compare(TemplateUtilities::DataContainerTypes::Int8) ) { TEMPLATE_CHECK_TYPE_SUPPORT_supportedType=true; }\
    else if( 0==TEMPLATE_CHECK_TYPE_SUPPORT_type.compare(TemplateUtilities::DataContainerTypes::UInt8) ) { TEMPLATE_CHECK_TYPE_SUPPORT_supportedType=true; }\
    else if( 0==TEMPLATE_CHECK_TYPE_SUPPORT_type.compare(TemplateUtilities::DataContainerTypes::Int16) ) { TEMPLATE_CHECK_TYPE_SUPPORT_supportedType=true; }\
    else if( 0==TEMPLATE_CHECK_TYPE_SUPPORT_type.compare(TemplateUtilities::DataContainerTypes::UInt16) ) { TEMPLATE_CHECK_TYPE_SUPPORT_supportedType=true; }\
    else if( 0==TEMPLATE_CHECK_TYPE_SUPPORT_type.compare(TemplateUtilities::DataContainerTypes::Int32) ) { TEMPLATE_CHECK_TYPE_SUPPORT_supportedType=true; }\
    else if( 0==TEMPLATE_CHECK_TYPE_SUPPORT_type.compare(TemplateUtilities::DataContainerTypes::UInt32) ) { TEMPLATE_CHECK_TYPE_SUPPORT_supportedType=true; }\
    else if( 0==TEMPLATE_CHECK_TYPE_SUPPORT_type.compare(TemplateUtilities::DataContainerTypes::Int64) ) { TEMPLATE_CHECK_TYPE_SUPPORT_supportedType=true; }\
    else if( 0==TEMPLATE_CHECK_TYPE_SUPPORT_type.compare(TemplateUtilities::DataContainerTypes::UInt64) ) { TEMPLATE_CHECK_TYPE_SUPPORT_supportedType=true; }\
    if(!TEMPLATE_CHECK_TYPE_SUPPORT_supportedType)\
    {\
      QString TEMPLATE_CHECK_TYPE_SUPPORT_typelist = TemplateUtilities::DataContainerTypes::Bool+", "+TemplateUtilities::DataContainerTypes::Float+", "+TemplateUtilities::DataContainerTypes::Double+", "+TemplateUtilities::DataContainerTypes::Int8+", "+TemplateUtilities::DataContainerTypes::UInt8+", "+TemplateUtilities::DataContainerTypes::Int16+", "+TemplateUtilities::DataContainerTypes::UInt16+", "+TemplateUtilities::DataContainerTypes::Int32+", "+TemplateUtilities::DataContainerTypes::UInt32+", "+TemplateUtilities::DataContainerTypes::Int64+", "+TemplateUtilities::DataContainerTypes::UInt64;\
      QString TEMPLATE_CHECK_TYPE_SUPPORT_message = QObject::tr("The input array '%1' is of unsupported type '%2'. The following TEMPLATE_CHECK_TYPE_SUPPORT_types are supported: %3").arg(TEMPLATE_CHECK_TYPE_SUPPORT_arrayname).arg(TEMPLATE_CHECK_TYPE_SUPPORT_type).arg(TEMPLATE_CHECK_TYPE_SUPPORT_typelist);\
      setErrorCondition(TemplateUtilities::Errors::UnsupportedType);\
      notifyErrorMessage(getHumanLabel(), TEMPLATE_CHECK_TYPE_SUPPORT_message, getErrorCondition());\
    }\
  }

//used in execute to get the size of an array
#define TEMPLATE_GET_NUM_TUPLES(TEMPLATE_GET_NUM_TUPLES_arrayname, TEMPLATE_GET_NUM_TUPLES_num, TEMPLATE_GET_NUM_TUPLES_type)\
    if( 0==TEMPLATE_GET_NUM_TUPLES_type.compare(TemplateUtilities::DataContainerTypes::Bool) ) { TEMPLATE_GET_NUM_TUPLES_num=m_TemplateBool##TEMPLATE_GET_NUM_TUPLES_arrayname##Ptr.lock()->getNumberOfTuples(); }\
    else if( 0==TEMPLATE_GET_NUM_TUPLES_type.compare(TemplateUtilities::DataContainerTypes::Float) ) { TEMPLATE_GET_NUM_TUPLES_num=m_TemplateFloat##TEMPLATE_GET_NUM_TUPLES_arrayname##Ptr.lock()->getNumberOfTuples(); }\
    else if( 0==TEMPLATE_GET_NUM_TUPLES_type.compare(TemplateUtilities::DataContainerTypes::Double) ){ TEMPLATE_GET_NUM_TUPLES_num=m_TemplateDouble##TEMPLATE_GET_NUM_TUPLES_arrayname##Ptr.lock()->getNumberOfTuples(); }\
    else if( 0==TEMPLATE_GET_NUM_TUPLES_type.compare(TemplateUtilities::DataContainerTypes::Int8) ) { TEMPLATE_GET_NUM_TUPLES_num=m_TemplateInt8##TEMPLATE_GET_NUM_TUPLES_arrayname##Ptr.lock()->getNumberOfTuples(); }\
    else if( 0==TEMPLATE_GET_NUM_TUPLES_type.compare(TemplateUtilities::DataContainerTypes::UInt8) ) { TEMPLATE_GET_NUM_TUPLES_num=m_TemplateUInt8##TEMPLATE_GET_NUM_TUPLES_arrayname##Ptr.lock()->getNumberOfTuples(); }\
    else if( 0==TEMPLATE_GET_NUM_TUPLES_type.compare(TemplateUtilities::DataContainerTypes::Int16) ) { TEMPLATE_GET_NUM_TUPLES_num=m_TemplateInt16##TEMPLATE_GET_NUM_TUPLES_arrayname##Ptr.lock()->getNumberOfTuples(); }\
    else if( 0==TEMPLATE_GET_NUM_TUPLES_type.compare(TemplateUtilities::DataContainerTypes::UInt16) ) { TEMPLATE_GET_NUM_TUPLES_num=m_TemplateUInt16##TEMPLATE_GET_NUM_TUPLES_arrayname##Ptr.lock()->getNumberOfTuples(); }\
    else if( 0==TEMPLATE_GET_NUM_TUPLES_type.compare(TemplateUtilities::DataContainerTypes::Int32) ) { TEMPLATE_GET_NUM_TUPLES_num=m_TemplateInt32##TEMPLATE_GET_NUM_TUPLES_arrayname##Ptr.lock()->getNumberOfTuples(); }\
    else if( 0==TEMPLATE_GET_NUM_TUPLES_type.compare(TemplateUtilities::DataContainerTypes::UInt32) ) { TEMPLATE_GET_NUM_TUPLES_num=m_TemplateUInt32##TEMPLATE_GET_NUM_TUPLES_arrayname##Ptr.lock()->getNumberOfTuples(); }\
    else if( 0==TEMPLATE_GET_NUM_TUPLES_type.compare(TemplateUtilities::DataContainerTypes::Int64) ) { TEMPLATE_GET_NUM_TUPLES_num=m_TemplateInt64##TEMPLATE_GET_NUM_TUPLES_arrayname##Ptr.lock()->getNumberOfTuples(); }\
    else if( 0==TEMPLATE_GET_NUM_TUPLES_type.compare(TemplateUtilities::DataContainerTypes::UInt64) ) { TEMPLATE_GET_NUM_TUPLES_num=m_TemplateUInt64##TEMPLATE_GET_NUM_TUPLES_arrayname##Ptr.lock()->getNumberOfTuples(); }

#endif /* _TemplateMacros_H_ */
