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

namespace TemplateConstants
{
  namespace TypeNames
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
    const QString UnknownType("UnknownType");
    const QString SupportedTypeList(TypeNames::Int8+", "+TypeNames::UInt8+", "+TypeNames::Int16+", "+TypeNames::UInt16+", "+TypeNames::Int32+", "+TypeNames::UInt32+", "+TypeNames::Int64+", "+TypeNames::UInt64+", "+TypeNames::Float+", "+TypeNames::Double);
  }

  enum Types
  {
    UnknownType = 0,
    Bool,
    Int8,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Int64,
    UInt64,
    Float,
    Double
  };

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

class TemplateUtilities
{
  public:
    TemplateUtilities(){}
    virtual ~TemplateUtilities();

    QString static getTypeNameFromType(int type)
    {
      if(TemplateConstants::Bool==type) {
        return TemplateConstants::TypeNames::Bool;
      } else if(TemplateConstants::Int8==type) {
        return TemplateConstants::TypeNames::Int8;
      } else if(TemplateConstants::UInt8==type) {
        return TemplateConstants::TypeNames::UInt8;
      } else if(TemplateConstants::Int16==type) {
        return TemplateConstants::TypeNames::Int16;
      } else if(TemplateConstants::UInt16==type) {
        return TemplateConstants::TypeNames::UInt16;
      } else if(TemplateConstants::Int32==type) {
        return TemplateConstants::TypeNames::Int32;
      } else if(TemplateConstants::UInt32==type) {
        return TemplateConstants::TypeNames::UInt32;
      } else if(TemplateConstants::Int64==type) {
        return TemplateConstants::TypeNames::Int64;
      } else if(TemplateConstants::UInt64==type) {
        return TemplateConstants::TypeNames::UInt64;
      } else if(TemplateConstants::Float==type) {
        return TemplateConstants::TypeNames::Float;
      } else if(TemplateConstants::Double==type) {
        return TemplateConstants::TypeNames::Double;
      } else {
        return TemplateConstants::TypeNames::UnknownType;
      }
    }

    int static getTypeFromTypeName(QString type)
    {
      if(0==type.compare(TemplateConstants::TypeNames::Bool)) {
        return TemplateConstants::Bool;
      } else if(0==type.compare(TemplateConstants::TypeNames::Int8)) {
        return TemplateConstants::Int8;
      } else if(0==type.compare(TemplateConstants::TypeNames::UInt8)) {
        return TemplateConstants::UInt8;
      } else if(0==type.compare(TemplateConstants::TypeNames::Int16)) {
        return TemplateConstants::Int16;
      } else if(0==type.compare(TemplateConstants::TypeNames::UInt16)) {
        return TemplateConstants::UInt16;
      } else if(0==type.compare(TemplateConstants::TypeNames::Int32)) {
        return TemplateConstants::Int32;
      } else if(0==type.compare(TemplateConstants::TypeNames::UInt32)) {
        return TemplateConstants::UInt32;
      } else if(0==type.compare(TemplateConstants::TypeNames::Int64)) {
        return TemplateConstants::Int64;
      } else if(0==type.compare(TemplateConstants::TypeNames::UInt64)) {
        return TemplateConstants::UInt64;
      } else if(0==type.compare(TemplateConstants::TypeNames::Float)) {
        return TemplateConstants::Float;
      } else if(0==type.compare(TemplateConstants::TypeNames::Double)) {
        return TemplateConstants::Double;
      } else {
        return TemplateConstants::UnknownType;
      }
    }

  private:
    TemplateUtilities(const TemplateUtilities&); // Copy Constructor Not Implemented
    void operator=(const TemplateUtilities&); // Operator '=' Not Implemented
};

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
    VolumeDataContainer* volDataCntr = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, arrayPath.getDataContainerName(), false);\
    if(getErrorCondition() < 0 || NULL == volDataCntr) {\
      QString TEMPLATE_GET_PREREQ_ARRAY_message = QObject::tr("The Data Container '%1' does not exist").arg(arrayPath.getDataContainerName());\
      setErrorCondition(TemplateConstants::Errors::MissingDataContainer);\
      notifyErrorMessage(getHumanLabel(), TEMPLATE_GET_PREREQ_ARRAY_message, getErrorCondition());\
      return;\
    }\
    AttributeMatrix::Pointer TEMPLATE_GET_PREREQ_ARRAY_cellAttrMat = volDataCntr->getPrereqAttributeMatrix<AbstractFilter>(this, arrayPath.getAttributeMatrixName(), TemplateConstants::Errors::MissingAttributeMatrix);\
    if(getErrorCondition() < 0 || NULL == TEMPLATE_GET_PREREQ_ARRAY_cellAttrMat.get()) {\
      QString TEMPLATE_GET_PREREQ_ARRAY_message = QObject::tr("The Attribute Matrix '%1' does not exist").arg(arrayPath.getAttributeMatrixName());\
      setErrorCondition(TemplateConstants::Errors::MissingAttributeMatrix);\
      notifyErrorMessage(getHumanLabel(), TEMPLATE_GET_PREREQ_ARRAY_message, getErrorCondition());\
      return;\
    }\
    IDataArray::Pointer TEMPLATE_GET_PREREQ_ARRAY_p = TEMPLATE_GET_PREREQ_ARRAY_cellAttrMat->getAttributeArray(arrayPath.getDataArrayName());\
    if(NULL == TEMPLATE_GET_PREREQ_ARRAY_p.get()) {\
      QString TEMPLATE_GET_PREREQ_ARRAY_message = QObject::tr("The input array '%1' was not found in the AttributeMatrix '%2'.").arg(m_##arrayName##ArrayName).arg(arrayPath.getAttributeMatrixName());\
      setErrorCondition(TemplateConstants::Errors::MissingArray);\
      notifyErrorMessage(getHumanLabel(), TEMPLATE_GET_PREREQ_ARRAY_message, getErrorCondition());\
      return;\
    }\
    if(dims.isEmpty()) {\
      dims.swap(TEMPLATE_GET_PREREQ_ARRAY_p->getComponentDimensions());\
    }\
    m_##arrayName##Ptr = volDataCntr->getAttributeMatrix(arrayPath.getAttributeMatrixName())->getAttributeArray(arrayPath.getDataArrayName());\
    if(TemplateConstants::IsSubclassOf<Int8ArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateConstants::IsSubclassOf<UInt8ArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateConstants::IsSubclassOf<Int16ArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int16_t>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateConstants::IsSubclassOf<UInt16ArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint16_t>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateConstants::IsSubclassOf<Int32ArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateConstants::IsSubclassOf<UInt32ArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateConstants::IsSubclassOf<Int64ArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateConstants::IsSubclassOf<UInt64ArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint64_t>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateConstants::IsSubclassOf<FloatArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else if(TemplateConstants::IsSubclassOf<DoubleArrayType>()(m_##arrayName##Ptr.lock())) {\
      m_##arrayName##Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, arrayPath, dims);\
    }\
    else{\
      QString TEMPLATE_GET_PREREQ_ARRAY_message = QObject::tr("The input array %1 is of unsupported type '%2'. The following types are supported: %3").arg(arrayPath.getDataArrayName()).arg(m_##arrayName##Ptr.lock()->getTypeAsString()).arg(TemplateConstants::TypeNames::SupportedTypeList);\
      setErrorCondition(TemplateConstants::Errors::UnsupportedType);\
      notifyErrorMessage(getHumanLabel(), TEMPLATE_GET_PREREQ_ARRAY_message, getErrorCondition());\
      return;\
    }\
    if( NULL != m_##arrayName##Ptr.lock().get() ) {\
      m_##arrayName = m_##arrayName##Ptr.lock()->getVoidPointer(0);\
    }\
  }

//used during preflight to create an array: at the specified path, of the specified type, and of the specified # components per voxel
#define TEMPLATE_CREATE_NONPREREQ_ARRAY(arrayName, arrayPath, dims, type)\
  {\
    if( TemplateConstants::Float==type ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, arrayPath, 0, dims);\
    }\
    else if( TemplateConstants::Double==type ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, arrayPath, 0, dims);\
    }\
    else if( TemplateConstants::Int8==type ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter, int8_t>(this, arrayPath, 0, dims);\
    }\
    else if( TemplateConstants::UInt8==type ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, arrayPath, 0, dims);\
    }\
    else if( TemplateConstants::Int16==type ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int16_t>, AbstractFilter, int16_t>(this, arrayPath, 0, dims);\
    }\
    else if( TemplateConstants::UInt16==type ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint16_t>, AbstractFilter, uint16_t>(this, arrayPath, 0, dims);\
    }\
    else if( TemplateConstants::Int32==type ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, arrayPath, 0, dims);\
    }\
    else if( TemplateConstants::UInt32==type ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this, arrayPath, 0, dims);\
    }\
    else if( TemplateConstants::Int64==type ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter, int64_t>(this, arrayPath, 0, dims);\
    }\
    else if( TemplateConstants::UInt64==type ) {\
      m_##arrayName##Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint64_t>, AbstractFilter, uint64_t>(this, arrayPath, 0, dims);\
    }\
    else {\
      QString TEMPLATE_CREATE_NONPREREQ_ARRAY_message = QObject::tr("The created array '%1' is of unsupported type '%2'. The following types are supported: %3").arg(arrayPath.getDataArrayName()).arg(type).arg(TemplateConstants::TypeNames::SupportedTypeList);\
      setErrorCondition(TemplateConstants::Errors::UnsupportedType);\
      notifyErrorMessage(getHumanLabel(), TEMPLATE_CREATE_NONPREREQ_ARRAY_message, getErrorCondition());\
    }\
    if( NULL != m_##arrayName##Ptr.lock().get() ) { \
        m_##arrayName = m_##arrayName##Ptr.lock()->getVoidPointer(0);\
    }\
  }

#endif /* _TemplateUtilities_H_ */
