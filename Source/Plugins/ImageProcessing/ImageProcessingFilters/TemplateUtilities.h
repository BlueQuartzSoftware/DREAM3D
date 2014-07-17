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
#define TEMPLATE_EXECUTE_FUNCTION(function, type, ...)\
  {\
    if(0==type.compare(TemplateUtilities::DataContainerTypes::Int8)) {\
      function##<int8_t>(__VA_ARGS__);\
    }\
    else if(0==type.compare(TemplateUtilities::DataContainerTypes::UInt8)) {\
      function##<uint8_t>(__VA_ARGS__);\
    }\
    else if(0==type.compare(TemplateUtilities::DataContainerTypes::Int16)) {\
      function##<int16_t>(__VA_ARGS__);\
    }\
    else if(0==type.compare(TemplateUtilities::DataContainerTypes::UInt16)) {\
      function##<uint16_t>(__VA_ARGS__);\
    }\
    else if(0==type.compare(TemplateUtilities::DataContainerTypes::Int32)) {\
      function##<int32_t>(__VA_ARGS__);\
    }\
    else if(0==type.compare(TemplateUtilities::DataContainerTypes::UInt32)) {\
      function##<uint32_t>(__VA_ARGS__);\
    }\
    else if(0==type.compare(TemplateUtilities::DataContainerTypes::Int64)) {\
      function##<int64_t>(__VA_ARGS__);\
    }\
    else if(0==type.compare(TemplateUtilities::DataContainerTypes::UInt64)) {\
      function##<uint64_t>(__VA_ARGS__);\
    }\
    else if(0==type.compare(TemplateUtilities::DataContainerTypes::Float)) {\
      function##<float>(__VA_ARGS__);\
    }\
    else if(0==type.compare(TemplateUtilities::DataContainerTypes::Double)) {\
      function##<double>(__VA_ARGS__);\
    }\
    else {\
      setErrorCondition(TemplateUtilities::Errors::UnsupportedType);\
      QString errMessage = QObject::tr("The unsupported type '%1' could not be used. The following types are supported: %2").arg(type).arg(TemplateUtilities::DataContainerTypes::SupportedTypeList);\
      notifyErrorMessage(getHumanLabel(), errMessage, getErrorCondition());\
      return;\
    }\
  }

//used in place of 'DEFINE_REQUIRED_DATAARRAY_VARIABLE' in filter header
#define TEMPLATE_DEFINE_REQUIRED_DATAARRAY_VARIABLE(varName)\
    DREAM3D_INSTANCE_STRING_PROPERTY(varName##ArrayName);\
    private:\
    IDataArray::WeakPointer m_##varName##Ptr;\
    void* m_##varName;

//used in place of 'DEFINE_CREATED_DATAARRAY_VARIABLE' in filter header
#define TEMPLATE_DEFINE_CREATED_DATAARRAY_VARIABLE(varName)\
    private:\
    IDataArray::WeakPointer m_##varName##Ptr;\
    void* m_##varName;

//used in preflight to check if array: exists, is compatible type, and has correct # componenets per voxel
#define TEMPLATE_GET_PREREQ_ARRAY(arrayName, arrayPath, dims)\
  {\
    VolumeDataContainer* volDataCntr = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, m_##arrayName##ArrayPath.getDataContainerName(), false);\
    if(getErrorCondition() < 0 || NULL == volDataCntr) {\
      QString volDataCntressage = QObject::tr("The Data Container '%1' does not exist").arg(m_##arrayName##ArrayPath.getDataContainerName());\
      setErrorCondition(TemplateUtilities::Errors::MissingDataContainer);\
      notifyErrorMessage(getHumanLabel(), volDataCntressage, getErrorCondition());\
      return;\
    }\
    AttributeMatrix::Pointer TEMPLATE_GET_PREREQ_ARRAY_cellAttrMat = volDataCntr->getPrereqAttributeMatrix<AbstractFilter>(this, m_##arrayName##ArrayPath.getAttributeMatrixName(), TemplateUtilities::Errors::MissingAttributeMatrix);\
    if(getErrorCondition() < 0 || NULL == TEMPLATE_GET_PREREQ_ARRAY_cellAttrMat.get()) {\
      QString volDataCntressage = QObject::tr("The Attribute Matrix '%1' does not exist").arg(m_##arrayName##ArrayPath.getAttributeMatrixName());\
      setErrorCondition(TemplateUtilities::Errors::MissingAttributeMatrix);\
      notifyErrorMessage(getHumanLabel(), volDataCntressage, getErrorCondition());\
      return;\
    }\
    IDataArray::Pointer TEMPLATE_GET_PREREQ_ARRAY_p = TEMPLATE_GET_PREREQ_ARRAY_cellAttrMat->getAttributeArray(m_##arrayName##ArrayPath.getDataArrayName());\
    if (NULL == TEMPLATE_GET_PREREQ_ARRAY_p.get()) {\
      QString volDataCntressage = QObject::tr("The input array '%1' was not found in the AttributeMatrix '%2'.").arg(m_##arrayName##ArrayName).arg(m_##arrayName##ArrayPath.getAttributeMatrixName());\
      setErrorCondition(TemplateUtilities::Errors::MissingArray);\
      notifyErrorMessage(getHumanLabel(), volDataCntressage, getErrorCondition());\
      return;\
    }\
    m_##arrayName##Ptr = volDataCntr->getAttributeMatrix(m_##arrayName##ArrayPath.getAttributeMatrixName())->getAttributeArray(getSelectedCellArrayArrayPath().getDataArrayName());\
    if(TemplateUtilities::IsSubclassOf<Int8ArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<UInt8ArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<Int16ArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int16_t>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<UInt16ArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint16_t>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<Int32ArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<UInt32ArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<Int64ArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<UInt64ArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint64_t>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<FloatArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateUtilities::IsSubclassOf<DoubleArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else{\
      QString volDataCntressage = QObject::tr("The input array %1 is of unsupported type '%2'. The following types are supported: %3").arg(arrayPath.getDataArrayName()).arg(m_##arrayName##Ptr.lock()->getTypeAsString()).arg(TemplateUtilities::DataContainerTypes::SupportedTypeList);\
      setErrorCondition(TemplateUtilities::Errors::UnsupportedType);\
      notifyErrorMessage(getHumanLabel(), volDataCntressage, getErrorCondition());\
      return;\
    }\
    if( NULL != m_##arrayName##Ptr.lock().get() ) {\
      m_##arrayName = m_##arrayName##Ptr.lock()->getVoidPointer(0);\
    }\
  }

//used during preflight to create an array: at the specified path, of the specified type, and of the specified # components per voxel
#define TEMPLATE_CREATE_NONPREREQ_ARRAY(arrayName, arrayPath, dims, type)\
  {\
    if( 0==type.compare(TemplateUtilities::DataContainerTypes::Float) ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, arrayPath, 0, dims);\
    }\
    else if( 0==type.compare(TemplateUtilities::DataContainerTypes::Double) ){\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, arrayPath, 0, dims);\
    }\
    else if( 0==type.compare(TemplateUtilities::DataContainerTypes::Int8) ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter, int8_t>(this, arrayPath, 0, dims);\
    }\
    else if( 0==type.compare(TemplateUtilities::DataContainerTypes::UInt8) ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, arrayPath, 0, dims);\
    }\
    else if( 0==type.compare(TemplateUtilities::DataContainerTypes::Int16) ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int16_t>, AbstractFilter, int16_t>(this, arrayPath, 0, dims);\
    }\
    else if( 0==type.compare(TemplateUtilities::DataContainerTypes::UInt16) ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint16_t>, AbstractFilter, uint16_t>(this, arrayPath, 0, dims);\
    }\
    else if( 0==type.compare(TemplateUtilities::DataContainerTypes::Int32) ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, arrayPath, 0, dims);\
    }\
    else if( 0==type.compare(TemplateUtilities::DataContainerTypes::UInt32) ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this, arrayPath, 0, dims);\
    }\
    else if( 0==type.compare(TemplateUtilities::DataContainerTypes::Int64) ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter, int64_t>(this, arrayPath, 0, dims);\
    }\
    else if( 0==type.compare(TemplateUtilities::DataContainerTypes::UInt64) ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint64_t>, AbstractFilter, uint64_t>(this, arrayPath, 0, dims);\
    }\
    else {\
      QString errMessage = QObject::tr("The created array '%1' is of unsupported type '%2'. The following types are supported: %3").arg(arrayPath.getDataArrayName()).arg(type).arg(TemplateUtilities::DataContainerTypes::SupportedTypeList);\
      setErrorCondition(TemplateUtilities::Errors::UnsupportedType);\
      notifyErrorMessage(getHumanLabel(), errMessage, getErrorCondition());\
    }\
    if( NULL != m_##arrayName##Ptr.lock().get() ) { \
        m_##arrayName = m_##arrayName##Ptr.lock()->getVoidPointer(0);\
    }\
  }

#endif /* _TemplateMacros_H_ */
