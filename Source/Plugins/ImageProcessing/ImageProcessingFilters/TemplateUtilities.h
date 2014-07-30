/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _TemplateUtilities_H_
#define _TemplateUtilities_H_

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/DataArrayPath.h"

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
    const QString SupportedTypeList(TypeNames::Int8 + ", " + TypeNames::UInt8 + ", " + TypeNames::Int16 + ", " + TypeNames::UInt16 + ", " + TypeNames::Int32 + ", " + TypeNames::UInt32 + ", " + TypeNames::Int64 + ", " + TypeNames::UInt64 + ", " + TypeNames::Float + ", " + TypeNames::Double);
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
  class CanDynamicCast
  {
    public:
      CanDynamicCast() {}
      virtual ~CanDynamicCast() {}
      bool operator()(IDataArray::Pointer p)
      {
        return (boost::dynamic_pointer_cast<T>(p).get() != NULL);
      }
  };
}

namespace TemplateHelper
{

  class CreateNonPrereqArrayFromArrayType
  {
    public:
      CreateNonPrereqArrayFromArrayType(){}
      ~CreateNonPrereqArrayFromArrayType(){}

      IDataArray::WeakPointer operator()(AbstractFilter* f, DataArrayPath arrayPath, QVector<size_t> compDims, IDataArray::Pointer sourceArrayType)
      {

        IDataArray::Pointer ptr = IDataArray::NullPointer();

        if(TemplateConstants::CanDynamicCast<FloatArrayType>()(sourceArrayType) ) {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(f, arrayPath, 0, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<DoubleArrayType>()(sourceArrayType) ) {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(f, arrayPath, 0, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<Int8ArrayType>()(sourceArrayType) ) {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter, int8_t>(f, arrayPath, 0, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<UInt8ArrayType>()(sourceArrayType) ) {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(f, arrayPath, 0, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<Int16ArrayType>()(sourceArrayType) ) {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int16_t>, AbstractFilter, int16_t>(f, arrayPath, 0, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<UInt16ArrayType>()(sourceArrayType) ) {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint16_t>, AbstractFilter, uint16_t>(f, arrayPath, 0, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<Int32ArrayType>()(sourceArrayType) ) {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(f, arrayPath, 0, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<UInt32ArrayType>()(sourceArrayType) ) {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(f, arrayPath, 0, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<Int64ArrayType>()(sourceArrayType) ) {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter, int64_t>(f, arrayPath, 0, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<UInt64ArrayType>()(sourceArrayType) ) {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint64_t>, AbstractFilter, uint64_t>(f, arrayPath, 0, compDims);
        }
        else {
          QString TEMPLATE_CREATE_NONPREREQ_ARRAY_message = QObject::tr("The created array '%1' is of unsupported type. The following types are supported: %3").arg(arrayPath.getDataArrayName()).arg(TemplateConstants::TypeNames::SupportedTypeList);
          f->setErrorCondition(TemplateConstants::Errors::UnsupportedType);
          f->notifyErrorMessage(f->getHumanLabel(), TEMPLATE_CREATE_NONPREREQ_ARRAY_message, f->getErrorCondition());\
        }
        return ptr;
      }

  };

  template<typename FilterClass, typename DataContainerClass>
  class GetPrereqArrayFromPath
  {
    public:
      GetPrereqArrayFromPath(){}
      virtual ~GetPrereqArrayFromPath(){}

      IDataArray::WeakPointer operator()(FilterClass* f, DataArrayPath arrayPath, QVector<size_t> compDims)
      {
        IDataArray::Pointer retPtr = IDataArray::NullPointer();
        DataContainerClass* volDataCntr = f->getDataContainerArray()->template getPrereqDataContainer<DataContainerClass, FilterClass>(f, arrayPath.getDataContainerName(), false);
        if(f->getErrorCondition() < 0 || NULL == volDataCntr) {
          QString ss = QObject::tr("The Data Container '%1' does not exist").arg(arrayPath.getDataContainerName());
          f->setErrorCondition(TemplateConstants::Errors::MissingDataContainer);
          f->notifyErrorMessage(f->getHumanLabel(), ss, f->getErrorCondition());
          return retPtr;
        }
        AttributeMatrix::Pointer cell_attr_matrix = volDataCntr->template getPrereqAttributeMatrix<FilterClass>(f, arrayPath.getAttributeMatrixName(), TemplateConstants::Errors::MissingAttributeMatrix);
        if(f->getErrorCondition() < 0 || NULL == cell_attr_matrix.get()) {
          QString ss = QObject::tr("The Attribute Matrix '%1' does not exist").arg(arrayPath.getAttributeMatrixName());
          f->setErrorCondition(TemplateConstants::Errors::MissingAttributeMatrix);
          f->notifyErrorMessage(f->getHumanLabel(), ss, f->getErrorCondition());
          return retPtr;
        }
        IDataArray::Pointer templ_ptr = cell_attr_matrix->getAttributeArray(arrayPath.getDataArrayName());
        if(NULL == templ_ptr.get()) {
          QString ss = QObject::tr("The input array '%1' was not found in the AttributeMatrix '%2'.").arg(arrayPath.getDataArrayName()).arg(arrayPath.getAttributeMatrixName());
          f->setErrorCondition(TemplateConstants::Errors::MissingArray);
          f->notifyErrorMessage(f->getHumanLabel(), ss, f->getErrorCondition());
          return retPtr;
        }
        if(compDims.isEmpty()) {
          compDims = templ_ptr->getComponentDimensions();
        }

        IDataArray::Pointer i_data_array = cell_attr_matrix->getAttributeArray(arrayPath.getDataArrayName());
        if(TemplateConstants::CanDynamicCast<Int8ArrayType>()(i_data_array) ) {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<UInt8ArrayType>()(i_data_array) ) {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<Int16ArrayType>()(i_data_array) ) {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<int16_t>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<UInt16ArrayType>()(i_data_array) ) {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<uint16_t>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<Int32ArrayType>()(i_data_array) ) {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<UInt32ArrayType>()(i_data_array) ) {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<Int64ArrayType>()(i_data_array) ) {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<UInt64ArrayType>()(i_data_array) ) {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<uint64_t>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<FloatArrayType>()(i_data_array) ) {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(TemplateConstants::CanDynamicCast<DoubleArrayType>()(i_data_array) ) {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(f, arrayPath, compDims);
        }
        else
        {
          QString ss = QObject::tr("The input array %1 is of unsupported type '%2'. The following types are supported: %3").arg(arrayPath.getDataArrayName()).arg(  retPtr->getTypeAsString()).arg(TemplateConstants::TypeNames::SupportedTypeList);
          f->setErrorCondition(TemplateConstants::Errors::UnsupportedType);
          f->notifyErrorMessage(f->getHumanLabel(), ss, f->getErrorCondition());
        }
        return retPtr;

      }


    private:
      GetPrereqArrayFromPath(const GetPrereqArrayFromPath&); // Copy Constructor Not Implemented
      void operator=(const GetPrereqArrayFromPath&); // Operator '=' Not Implemented
  };


  class TemplateUtilities
  {
    public:
      TemplateUtilities() {}
      virtual ~TemplateUtilities();

      QString static getTypeNameFromType(int type)
      {
        if(TemplateConstants::Bool == type)
        {
          return TemplateConstants::TypeNames::Bool;
        }
        else if(TemplateConstants::Int8 == type)
        {
          return TemplateConstants::TypeNames::Int8;
        }
        else if(TemplateConstants::UInt8 == type)
        {
          return TemplateConstants::TypeNames::UInt8;
        }
        else if(TemplateConstants::Int16 == type)
        {
          return TemplateConstants::TypeNames::Int16;
        }
        else if(TemplateConstants::UInt16 == type)
        {
          return TemplateConstants::TypeNames::UInt16;
        }
        else if(TemplateConstants::Int32 == type)
        {
          return TemplateConstants::TypeNames::Int32;
        }
        else if(TemplateConstants::UInt32 == type)
        {
          return TemplateConstants::TypeNames::UInt32;
        }
        else if(TemplateConstants::Int64 == type)
        {
          return TemplateConstants::TypeNames::Int64;
        }
        else if(TemplateConstants::UInt64 == type)
        {
          return TemplateConstants::TypeNames::UInt64;
        }
        else if(TemplateConstants::Float == type)
        {
          return TemplateConstants::TypeNames::Float;
        }
        else if(TemplateConstants::Double == type)
        {
          return TemplateConstants::TypeNames::Double;
        }
        else
        {
          return TemplateConstants::TypeNames::UnknownType;
        }
      }

      int static getTypeFromTypeName(QString type)
      {
        if(0 == type.compare(TemplateConstants::TypeNames::Bool))
        {
          return TemplateConstants::Bool;
        }
        else if(0 == type.compare(TemplateConstants::TypeNames::Int8))
        {
          return TemplateConstants::Int8;
        }
        else if(0 == type.compare(TemplateConstants::TypeNames::UInt8))
        {
          return TemplateConstants::UInt8;
        }
        else if(0 == type.compare(TemplateConstants::TypeNames::Int16))
        {
          return TemplateConstants::Int16;
        }
        else if(0 == type.compare(TemplateConstants::TypeNames::UInt16))
        {
          return TemplateConstants::UInt16;
        }
        else if(0 == type.compare(TemplateConstants::TypeNames::Int32))
        {
          return TemplateConstants::Int32;
        }
        else if(0 == type.compare(TemplateConstants::TypeNames::UInt32))
        {
          return TemplateConstants::UInt32;
        }
        else if(0 == type.compare(TemplateConstants::TypeNames::Int64))
        {
          return TemplateConstants::Int64;
        }
        else if(0 == type.compare(TemplateConstants::TypeNames::UInt64))
        {
          return TemplateConstants::UInt64;
        }
        else if(0 == type.compare(TemplateConstants::TypeNames::Float))
        {
          return TemplateConstants::Float;
        }
        else if(0 == type.compare(TemplateConstants::TypeNames::Double))
        {
          return TemplateConstants::Double;
        }
        else
        {
          return TemplateConstants::UnknownType;
        }
      }

    private:
      TemplateUtilities(const TemplateUtilities&); // Copy Constructor Not Implemented
      void operator=(const TemplateUtilities&); // Operator '=' Not Implemented
  };


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



#endif /* _TemplateUtilities_H_ */
