/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _TemplateMacros_H_
#define _TemplateMacros_H_

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
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
    const QString SupportedTypeList(TemplateUtilities::DataContainerTypes::Int8+", "+TemplateUtilities::DataContainerTypes::UInt8+", "+TemplateUtilities::DataContainerTypes::Int16+", "+TemplateUtilities::DataContainerTypes::UInt16+", "+TemplateUtilities::DataContainerTypes::Int32+", "+TemplateUtilities::DataContainerTypes::UInt32+", "+TemplateUtilities::DataContainerTypes::Int64+", "+TemplateUtilities::DataContainerTypes::UInt64+", "+TemplateUtilities::DataContainerTypes::Float+", "+TemplateUtilities::DataContainerTypes::Double);
  }

  namespace Errors
  {
    const int UnsupportedType(-401);
    const int MissingDataContainer(-402);
    const int MissingAttributeMatrix(-403);
    const int MissingArray(-404);
  }

  template<typename T>
  class IsSubclassOf
  {
    public:
      IsSubclassOf() {}
      virtual ~IsSubclassOf() {}
      bool operator()(IDataArray::Pointer p)
      {
        return (boost::dynamic_pointer_cast<T>(p).get() != NULL);
      }
  };
}

//used in filter::execute method to run the correct version of a templated function
#define TEMPLATE_EXECUTE_FUNCTION(TEMPLATE_EXECUTE_FUNCTION_function, TEMPLATE_EXECUTE_FUNCTION_type, ...)\
  {\
    if(0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::Int8)) {\
      TEMPLATE_EXECUTE_FUNCTION_function##<int8_t>(__VA_ARGS__);\
    }\
    else if(0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::UInt8)) {\
      TEMPLATE_EXECUTE_FUNCTION_function##<uint8_t>(__VA_ARGS__);\
    }\
    else if(0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::Int16)) {\
      TEMPLATE_EXECUTE_FUNCTION_function##<int16_t>(__VA_ARGS__);\
    }\
    else if(0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::UInt16)) {\
      TEMPLATE_EXECUTE_FUNCTION_function##<uint16_t>(__VA_ARGS__);\
    }\
    else if(0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::Int32)) {\
      TEMPLATE_EXECUTE_FUNCTION_function##<int32_t>(__VA_ARGS__);\
    }\
    else if(0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::UInt32)) {\
      TEMPLATE_EXECUTE_FUNCTION_function##<uint32_t>(__VA_ARGS__);\
    }\
    else if(0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::Int64)) {\
      TEMPLATE_EXECUTE_FUNCTION_function##<int64_t>(__VA_ARGS__);\
    }\
    else if(0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::UInt64)) {\
      TEMPLATE_EXECUTE_FUNCTION_function##<uint64_t>(__VA_ARGS__);\
    }\
    else if(0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::Float)) {\
      TEMPLATE_EXECUTE_FUNCTION_function##<float>(__VA_ARGS__);\
    }\
    else if(0==TEMPLATE_EXECUTE_FUNCTION_type.compare(TemplateUtilities::DataContainerTypes::Double)) {\
      TEMPLATE_EXECUTE_FUNCTION_function##<double>(__VA_ARGS__);\
    }\
    else {\
      setErrorCondition(TemplateUtilities::Errors::UnsupportedType);\
      QString TEMPLATE_EXECUTE_FUNCTION_message = QObject::tr("The unsupported type '%1' could not be used. The following types are supported: %2").arg(TEMPLATE_EXECUTE_FUNCTION_type).arg(TemplateUtilities::DataContainerTypes::SupportedTypeList);\
      notifyErrorMessage(getHumanLabel(), TEMPLATE_EXECUTE_FUNCTION_message, getErrorCondition());\
      return;\
    }\
  }

//used in place of 'DEFINE_REQUIRED_DATAARRAY_VARIABLE' in filter header
#define TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE(TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name)\
    DREAM3D_INSTANCE_STRING_PROPERTY(TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name##ArrayName);\
    private:\
    IDataArray::WeakPointer m_##TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name##Ptr;\
    void* m_##TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE_name;

//used in place of 'DEFINE_CREATED_DATAARRAY_VARIABLE' in filter header
#define TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE(TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE_name)\
    private:\
    IDataArray::WeakPointer m_##TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE_name##Ptr;\
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
    m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = TEMPLATE_GET_PREREQ_ARRAY_m->getAttributeMatrix(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##ArrayPath.getAttributeMatrixName())->getAttributeArray(getSelectedCellArrayArrayPath().getDataArrayName());\
    if(TemplateUtilities::IsSubclassOf<Int8ArrayType>()(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock())) {\
      m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<UInt8ArrayType>()(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock())) {\
      m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<Int16ArrayType>()(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock())) {\
      m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int16_t>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<UInt16ArrayType>()(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock())) {\
      m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint16_t>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<Int32ArrayType>()(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock())) {\
      m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<UInt32ArrayType>()(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock())) {\
      m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<Int64ArrayType>()(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock())) {\
      m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<UInt64ArrayType>()(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock())) {\
      m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint64_t>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<FloatArrayType>()(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock())) {\
      m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<DoubleArrayType>()(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock())) {\
      m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, TEMPLATE_GET_PREREQ_ARRAY_arraypath, TEMPLATE_GET_PREREQ_ARRAY_dims);\
    }\
    else{\
      QString TEMPLATE_GET_PREREQ_ARRAY_message = QObject::tr("The input array %1 is of unsupported type '%2'. The following types are supported: %3").arg(TEMPLATE_GET_PREREQ_ARRAY_arraypath.getDataArrayName()).arg(m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock()->getTypeAsString()).arg(TemplateUtilities::DataContainerTypes::SupportedTypeList);\
      setErrorCondition(TemplateUtilities::Errors::UnsupportedType);\
      notifyErrorMessage(getHumanLabel(), TEMPLATE_GET_PREREQ_ARRAY_message, getErrorCondition());\
      return;\
    }\
    if( NULL != m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock().get() ) {\
      m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname = m_##TEMPLATE_GET_PREREQ_ARRAY_arrayname##Ptr.lock()->getVoidPointer(0);\
    }\
  }

//used during preflight to create an array: at the specified path, of the specified type, and of the specified # components per voxel
#define TEMPLATE_CREATE_NONPREREQ_ARRAY(TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims, TEMPLATE_CREATE_NONPREREQ_ARRAY_type)\
  {\
    if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Float) ) {\
      m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Double) ){\
      m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Int8) ) {\
      m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter, int8_t>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::UInt8) ) {\
      m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Int16) ) {\
      m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int16_t>, AbstractFilter, int16_t>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::UInt16) ) {\
      m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint16_t>, AbstractFilter, uint16_t>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Int32) ) {\
      m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::UInt32) ) {\
      m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::Int64) ) {\
      m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter, int64_t>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
    }\
    else if( 0==TEMPLATE_CREATE_NONPREREQ_ARRAY_type.compare(TemplateUtilities::DataContainerTypes::UInt64) ) {\
      m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint64_t>, AbstractFilter, uint64_t>(this, TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath, 0, TEMPLATE_CREATE_NONPREREQ_ARRAY_dims);\
    }\
    else {\
      QString TEMPLATE_CREATE_NONPREREQ_ARRAY_message = QObject::tr("The created array '%1' is of unsupported type '%2'. The following types are supported: %3").arg(TEMPLATE_CREATE_NONPREREQ_ARRAY_arraypath.getDataArrayName()).arg(TEMPLATE_CREATE_NONPREREQ_ARRAY_type).arg(TemplateUtilities::DataContainerTypes::SupportedTypeList);\
      setErrorCondition(TemplateUtilities::Errors::UnsupportedType);\
      notifyErrorMessage(getHumanLabel(), TEMPLATE_CREATE_NONPREREQ_ARRAY_message, getErrorCondition());\
    }\
    if( NULL != m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock().get() ) { \
        m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname = m_##TEMPLATE_CREATE_NONPREREQ_ARRAY_arrayname##Ptr.lock()->getVoidPointer(0);\
    }\
  }

#endif /* _TemplateMacros_H_ */
