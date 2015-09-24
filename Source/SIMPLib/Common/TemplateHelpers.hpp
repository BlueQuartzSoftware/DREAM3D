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
#ifndef _TemplateHelpers_H_
#define _TemplateHelpers_H_

#include <boost/shared_ptr.hpp>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/AbstractFilter.h"

/*
* @brief This file contains a namespace and some C++ Functors that help to reduce the amout of code that filters
* need when the filter needs to determine if an IDataArray is of a certain primitive type.
*/

namespace TemplateHelpers
{

#define EXECUTE_FUNCTION_TEMPLATE(observableObj, templateName, inputData, ...)\
  if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(inputData))\
  {\
    templateName<float>(__VA_ARGS__);\
  }\
  else if(TemplateHelpers::CanDynamicCast<DoubleArrayType>()(inputData))\
  {\
    templateName<double>(__VA_ARGS__);\
  }\
  else if(TemplateHelpers::CanDynamicCast<Int8ArrayType>()(inputData))\
  {\
    templateName<int8_t>(__VA_ARGS__);\
  }\
  else if(TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(inputData))\
  {\
    templateName<uint8_t>(__VA_ARGS__);\
  }\
  else if(TemplateHelpers::CanDynamicCast<Int16ArrayType>()(inputData))\
  {\
    templateName<int16_t>(__VA_ARGS__);\
  }\
  else if(TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(inputData))\
  {\
    templateName<uint16_t>(__VA_ARGS__);\
  }\
  else if(TemplateHelpers::CanDynamicCast<Int32ArrayType>()(inputData))\
  {\
    templateName<int32_t>(__VA_ARGS__);\
  }\
  else if(TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(inputData))\
  {\
    templateName<uint32_t>(__VA_ARGS__);\
  }\
  else if(TemplateHelpers::CanDynamicCast<Int64ArrayType>()(inputData))\
  {\
    templateName<int64_t>(__VA_ARGS__);\
  }\
  else if(TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(inputData))\
  {\
    templateName<uint64_t>(__VA_ARGS__);\
  }\
  else if(TemplateHelpers::CanDynamicCast<BoolArrayType>()(inputData))\
  {\
    templateName<bool>(__VA_ARGS__);\
  }\
  else\
  {\
    observableObj->notifyErrorMessage(#templateName, "The input array was of unsupported type", TemplateHelpers::Errors::UnsupportedType);\
    return;\
  }

#define EXECUTE_TEMPLATE(observableObj, templateName, inputData, ...)\
  if(templateName<int8_t>()(inputData))\
  {\
    templateName<int8_t> pimpl;\
    pimpl.Execute(__VA_ARGS__);\
  }\
  else if(templateName<uint8_t>()(inputData) )\
  {\
    templateName<uint8_t> pimpl;\
    pimpl.Execute(__VA_ARGS__);\
  }\
  else if(templateName<int16_t>()(inputData) )\
  {\
    templateName<int16_t> pimpl;\
    pimpl.Execute(__VA_ARGS__);\
  }\
  else if(templateName<uint16_t>()(inputData) )\
  {\
    templateName<uint16_t> pimpl;\
    pimpl.Execute(__VA_ARGS__);\
  }\
  else if(templateName<int32_t>()(inputData) )\
  {\
    templateName<int32_t> pimpl;\
    pimpl.Execute(__VA_ARGS__);\
  }\
  else if(templateName<uint32_t>()(inputData) )\
  {\
    templateName<uint32_t> pimpl;\
    pimpl.Execute(__VA_ARGS__);\
  }\
  else if(templateName<int64_t>()(inputData) )\
  {\
    templateName<int64_t> pimpl;\
    pimpl.Execute(__VA_ARGS__);\
  }\
  else if(templateName<uint64_t>()(inputData) )\
  {\
    templateName<uint64_t> pimpl;\
    pimpl.Execute( __VA_ARGS__);\
  }\
  else if(templateName<float>()(inputData) )\
  {\
    templateName<float> pimpl;\
    pimpl.Execute(__VA_ARGS__);\
  }\
  else if(templateName<double>()(inputData) )\
  {\
    templateName<double> pimpl;\
    pimpl.Execute(__VA_ARGS__);\
  }\
  else if(templateName<bool>()(inputData) )\
  {\
    templateName<bool> pimpl;\
    pimpl.Execute(__VA_ARGS__);\
  }\
  else\
  {\
    observableObj->notifyErrorMessage(#templateName, "The input array was of unsupported type", TemplateHelpers::Errors::UnsupportedType);\
    return;\
  }

  /**
  * @brief Define from Error Codes
  */
  namespace Errors
  {
    const int UnsupportedType(-401);
    const int MissingDataContainer(-402);
    const int MissingAttributeMatrix(-403);
    const int MissingArray(-404);
  }

  /**
  * @brief This class (functor) simply returns true or false if the IDataArray can be downcast to a certain DataArray type
  * parameterized by the template parameter T.
  */
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


  /**
   * @brief The CreateNonPrereqArrayFromArrayType class will create a DataArray of the same type as another DataArray and attach it to
   * a supplied data container.
   */
  class CreateNonPrereqArrayFromArrayType
  {
    public:
      CreateNonPrereqArrayFromArrayType() {}
      ~CreateNonPrereqArrayFromArrayType() {}

      IDataArray::WeakPointer operator()(AbstractFilter* f, DataArrayPath arrayPath, QVector<size_t> compDims, IDataArray::Pointer sourceArrayType)
      {

        IDataArray::Pointer ptr = IDataArray::NullPointer();

        if(CanDynamicCast<FloatArrayType>()(sourceArrayType) )
        {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(f, arrayPath, 0, compDims);
        }
        else if(CanDynamicCast<DoubleArrayType>()(sourceArrayType) )
        {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(f, arrayPath, 0, compDims);
        }
        else if(CanDynamicCast<Int8ArrayType>()(sourceArrayType) )
        {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter, int8_t>(f, arrayPath, 0, compDims);
        }
        else if(CanDynamicCast<UInt8ArrayType>()(sourceArrayType) )
        {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(f, arrayPath, 0, compDims);
        }
        else if(CanDynamicCast<Int16ArrayType>()(sourceArrayType) )
        {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int16_t>, AbstractFilter, int16_t>(f, arrayPath, 0, compDims);
        }
        else if(CanDynamicCast<UInt16ArrayType>()(sourceArrayType) )
        {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint16_t>, AbstractFilter, uint16_t>(f, arrayPath, 0, compDims);
        }
        else if(CanDynamicCast<Int32ArrayType>()(sourceArrayType) )
        {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(f, arrayPath, 0, compDims);
        }
        else if(CanDynamicCast<UInt32ArrayType>()(sourceArrayType) )
        {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(f, arrayPath, 0, compDims);
        }
        else if(CanDynamicCast<Int64ArrayType>()(sourceArrayType) )
        {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter, int64_t>(f, arrayPath, 0, compDims);
        }
        else if(CanDynamicCast<UInt64ArrayType>()(sourceArrayType) )
        {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint64_t>, AbstractFilter, uint64_t>(f, arrayPath, 0, compDims);
        }
        else if(CanDynamicCast<BoolArrayType>()(sourceArrayType) )
        {
          ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<BoolArrayType, AbstractFilter, uint64_t>(f, arrayPath, 0, compDims);
        }
        else
        {
          QString msg = QObject::tr("The created array '%1' is of unsupported type. The following types are supported: %3").arg(arrayPath.getDataArrayName()).arg(DREAM3D::TypeNames::SupportedTypeList);
          f->setErrorCondition(Errors::UnsupportedType);
          f->notifyErrorMessage(f->getHumanLabel(), msg, f->getErrorCondition());
        }
        return ptr;
      }

  };


  /**
   * @brief The CreateNonPrereqArrayFromArrayType class will create a DataArray of the same type as another DataArray and attach it to
   * a supplied data container.
   */
  class CreateNonPrereqArrayFromTypeEnum
  {
    public:
      CreateNonPrereqArrayFromTypeEnum() {}
      ~CreateNonPrereqArrayFromTypeEnum() {}

      IDataArray::WeakPointer operator()(AbstractFilter* f, DataArrayPath arrayPath, QVector<size_t> compDims, int arrayType, double initValue)
      {
        IDataArray::Pointer ptr = IDataArray::NullPointer();

        switch(arrayType)
        {
          case DREAM3D::TypeEnums::Int8:
            ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter, int8_t>(f, arrayPath, static_cast<int8_t>(initValue), compDims);
            break;
          case DREAM3D::TypeEnums::UInt8:
            ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(f, arrayPath, static_cast<uint8_t>(initValue), compDims);
            break;
          case DREAM3D::TypeEnums::Int16:
            ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int16_t>, AbstractFilter, int16_t>(f, arrayPath, static_cast<int16_t>(initValue), compDims);
            break;
          case DREAM3D::TypeEnums::UInt16:
            ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint16_t>, AbstractFilter, uint16_t>(f, arrayPath, static_cast<uint16_t>(initValue), compDims);
            break;
          case DREAM3D::TypeEnums::Int32:
            ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(f, arrayPath, static_cast<int32_t>(initValue), compDims);
            break;
          case DREAM3D::TypeEnums::UInt32:
            ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(f, arrayPath, static_cast<uint32_t>(initValue), compDims);
            break;
          case DREAM3D::TypeEnums::Int64:
            ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter, int64_t>(f, arrayPath, static_cast<int64_t>(initValue), compDims);
            break;
          case DREAM3D::TypeEnums::UInt64:
            ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint64_t>, AbstractFilter, uint64_t>(f, arrayPath, static_cast<uint64_t>(initValue), compDims);
            break;
          case DREAM3D::TypeEnums::Float:
            ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(f, arrayPath, static_cast<float>(initValue), compDims);
            break;
          case DREAM3D::TypeEnums::Double:
            ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(f, arrayPath, static_cast<double>(initValue), compDims);
            break;
          case DREAM3D::TypeEnums::Bool:
            ptr = f->getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(f, arrayPath, static_cast<bool>(initValue), compDims);
            break;
          default:
            QString msg = QObject::tr("The created array '%1' is of unsupported type. The following types are supported: %3").arg(arrayPath.getDataArrayName()).arg(DREAM3D::TypeEnums::SupportedTypeList);
            f->setErrorCondition(Errors::UnsupportedType);
            f->notifyErrorMessage(f->getHumanLabel(), msg, f->getErrorCondition());
            break;
        }
        return ptr;
      }

  };




  /**
   * @brief The CreateArrayFromArrayType class will create a DataArray of the same type as another DataArray but not attach
   * it to any data container.
   */
  class CreateArrayFromArrayType
  {
    public:
      CreateArrayFromArrayType() {}
      ~CreateArrayFromArrayType() {}

      IDataArray::Pointer operator()(AbstractFilter* f, size_t numTuples, QVector<size_t> compDims, QString arrayName, bool allocate, IDataArray::Pointer sourceArrayType)
      {
        CreateArrayFromArrayType classInstance;
        QVector<size_t> tupleDims(1, numTuples);
        return classInstance(f, tupleDims, compDims, arrayName, allocate, sourceArrayType);
      }

      IDataArray::Pointer operator()(AbstractFilter* f, QVector<size_t> tupleDims, QVector<size_t> compDims, QString arrayName, bool allocate, IDataArray::Pointer sourceArrayType)
      {
        IDataArray::Pointer ptr = IDataArray::NullPointer();

        if(CanDynamicCast<FloatArrayType>()(sourceArrayType) )
        {
          ptr = FloatArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(CanDynamicCast<DoubleArrayType>()(sourceArrayType) )
        {
          ptr = DoubleArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(CanDynamicCast<Int8ArrayType>()(sourceArrayType) )
        {
          ptr = Int8ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(CanDynamicCast<UInt8ArrayType>()(sourceArrayType) )
        {
          ptr = UInt8ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(CanDynamicCast<Int16ArrayType>()(sourceArrayType) )
        {
          ptr = Int16ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(CanDynamicCast<UInt16ArrayType>()(sourceArrayType) )
        {
          ptr = UInt16ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(CanDynamicCast<Int32ArrayType>()(sourceArrayType) )
        {
          ptr = Int32ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(CanDynamicCast<UInt32ArrayType>()(sourceArrayType) )
        {
          ptr = UInt32ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(CanDynamicCast<Int64ArrayType>()(sourceArrayType) )
        {
          ptr = Int64ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(CanDynamicCast<UInt64ArrayType>()(sourceArrayType) )
        {
          ptr = UInt64ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(CanDynamicCast<BoolArrayType>()(sourceArrayType) )
        {
          ptr = BoolArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else
        {
          QString msg = QObject::tr("The created array is of unsupported type.");
          f->setErrorCondition(Errors::UnsupportedType);
          f->notifyErrorMessage(f->getHumanLabel(), msg, f->getErrorCondition());
        }
        return ptr;
      }
  };

  /**
   * @brief The CreateArrayFromType class will create a DataArray matching the type supplied as a QString.  Available types are listed
   * in Constants.h.
   */
  class CreateArrayFromType
  {
    public:
      CreateArrayFromType() {}
      ~CreateArrayFromType() {}

      IDataArray::Pointer operator()(AbstractFilter* f, size_t numTuples, QVector<size_t> compDims, QString arrayName, bool allocate, QString type)
      {
        CreateArrayFromType classInstance;
        QVector<size_t> tupleDims(1, numTuples);
        return classInstance(f, tupleDims, compDims, arrayName, allocate, type);
      }

      IDataArray::Pointer operator()(AbstractFilter* f, QVector<size_t> tupleDims, QVector<size_t> compDims, QString arrayName, bool allocate, QString type)
      {
        IDataArray::Pointer ptr = IDataArray::NullPointer();

        if(type.compare(DREAM3D::TypeNames::Float) == 0)
        {
          ptr = FloatArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(type.compare(DREAM3D::TypeNames::Double) == 0)
        {
          ptr = DoubleArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(type.compare(DREAM3D::TypeNames::Int8) == 0)
        {
          ptr = Int8ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(type.compare(DREAM3D::TypeNames::UInt8) == 0)
        {
          ptr = UInt8ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(type.compare(DREAM3D::TypeNames::Int16) == 0)
        {
          ptr = Int16ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(type.compare(DREAM3D::TypeNames::UInt16) == 0)
        {
          ptr = UInt16ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(type.compare(DREAM3D::TypeNames::Int32) == 0)
        {
          ptr = Int32ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(type.compare(DREAM3D::TypeNames::UInt32) == 0)
        {
          ptr = UInt32ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(type.compare(DREAM3D::TypeNames::Int64) == 0)
        {
          ptr = Int64ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else if(type.compare(DREAM3D::TypeNames::UInt64) == 0)
        {
          ptr = UInt64ArrayType::CreateArray(tupleDims, compDims, arrayName, allocate);
        }
        else
        {
          QString msg = QObject::tr("The created array is of unsupported type.");
          f->setErrorCondition(Errors::UnsupportedType);
          f->notifyErrorMessage(f->getHumanLabel(), msg, f->getErrorCondition());
        }
        return ptr;
      }
  };

  /**
  * @brief The GetPrereqArrayFromPath class will return a pointer to a DataArray of unknown type given the path.
  */
  template<typename FilterClass>
  class GetPrereqArrayFromPath
  {
    public:
      GetPrereqArrayFromPath() {}
      virtual ~GetPrereqArrayFromPath() {}

      IDataArray::WeakPointer operator()(FilterClass* f, DataArrayPath arrayPath, QVector<size_t>& compDims)
      {
        IDataArray::Pointer retPtr = IDataArray::NullPointer();
        DataContainer::Pointer volDataCntr = f->getDataContainerArray()->template getPrereqDataContainer<FilterClass>(f, arrayPath.getDataContainerName(), false);
        if(f->getErrorCondition() < 0 || NULL == volDataCntr)
        {
          QString ss = QObject::tr("The Data Container '%1' does not exist").arg(arrayPath.getDataContainerName());
          f->setErrorCondition(Errors::MissingDataContainer);
          f->notifyErrorMessage(f->getHumanLabel(), ss, f->getErrorCondition());
          return retPtr;
        }
        AttributeMatrix::Pointer cell_attr_matrix = volDataCntr->template getPrereqAttributeMatrix<FilterClass>(f, arrayPath.getAttributeMatrixName(), Errors::MissingAttributeMatrix);
        if(f->getErrorCondition() < 0 || NULL == cell_attr_matrix.get())
        {
          QString ss = QObject::tr("The Attribute Matrix '%1' does not exist").arg(arrayPath.getAttributeMatrixName());
          f->setErrorCondition(Errors::MissingAttributeMatrix);
          f->notifyErrorMessage(f->getHumanLabel(), ss, f->getErrorCondition());
          return retPtr;
        }
        IDataArray::Pointer templ_ptr = cell_attr_matrix->getAttributeArray(arrayPath.getDataArrayName());
        if(NULL == templ_ptr.get())
        {
          QString ss = QObject::tr("The input array '%1' was not found in the AttributeMatrix '%2'.").arg(arrayPath.getDataArrayName()).arg(arrayPath.getAttributeMatrixName());
          f->setErrorCondition(Errors::MissingArray);
          f->notifyErrorMessage(f->getHumanLabel(), ss, f->getErrorCondition());
          return retPtr;
        }
        if(compDims.isEmpty())
        {
          compDims = templ_ptr->getComponentDimensions();
        }

        IDataArray::Pointer i_data_array = cell_attr_matrix->getAttributeArray(arrayPath.getDataArrayName());
        if(CanDynamicCast<Int8ArrayType>()(i_data_array) )
        {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(CanDynamicCast<UInt8ArrayType>()(i_data_array) )
        {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(CanDynamicCast<Int16ArrayType>()(i_data_array) )
        {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<int16_t>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(CanDynamicCast<UInt16ArrayType>()(i_data_array) )
        {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<uint16_t>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(CanDynamicCast<Int32ArrayType>()(i_data_array) )
        {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(CanDynamicCast<UInt32ArrayType>()(i_data_array) )
        {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(CanDynamicCast<Int64ArrayType>()(i_data_array) )
        {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<int64_t>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(CanDynamicCast<UInt64ArrayType>()(i_data_array) )
        {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<uint64_t>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(CanDynamicCast<FloatArrayType>()(i_data_array) )
        {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(f, arrayPath, compDims);
        }
        else if(CanDynamicCast<DoubleArrayType>()(i_data_array) )
        {
          retPtr = f->getDataContainerArray()->template getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(f, arrayPath, compDims);
        }
        else
        {
          QString ss = QObject::tr("The input array %1 is of unsupported type '%2'. The following types are supported: %3").arg(arrayPath.getDataArrayName()).arg(  retPtr->getTypeAsString()).arg(DREAM3D::TypeNames::SupportedTypeList);
          f->setErrorCondition(Errors::UnsupportedType);
          f->notifyErrorMessage(f->getHumanLabel(), ss, f->getErrorCondition());
        }
        return retPtr;

      }


    private:
      GetPrereqArrayFromPath(const GetPrereqArrayFromPath&); // Copy Constructor Not Implemented
      void operator=(const GetPrereqArrayFromPath&); // Operator '=' Not Implemented
  };

}

#endif /* _TemplateHelpers_H_ */
