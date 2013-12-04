/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "ConvertData.h"

#include <stdio.h>


namespace Detail
{
  enum NumType
  {
    Int8 = 0,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Int64,
    UInt64,
    Float,
    Double,
    UnknownNumType
  };
}

namespace Detail
{
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template<typename T>
  void ConvertData(T* ptr, VolumeDataContainer* m, int32_t scalarType, const QString attributeMatrixName, const QString& name)
  {
    int numberOfComponents = ptr->GetNumberOfComponents();
    int voxels = ptr->getNumberOfTuples();
    size_t size = ptr->GetSize();


    QVector<int> dims(1, numberOfComponents);
    if (scalarType == Detail::Int8)
    {
      Int8ArrayType::Pointer p = Int8ArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->GetName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->SetValue(v, ptr->GetValue(v) );
      }
    }
    else if (scalarType == Detail::UInt8)
    {
      UInt8ArrayType::Pointer p = UInt8ArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->GetName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->SetValue(v, ptr->GetValue(v) );
      }
    }
    else if (scalarType == Detail::Int16)
    {
      Int16ArrayType::Pointer p = Int16ArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->GetName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->SetValue(v, ptr->GetValue(v) );
      }
    }
    else if (scalarType == Detail::UInt16)
    {
      UInt16ArrayType::Pointer p = UInt16ArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->GetName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->SetValue(v, ptr->GetValue(v) );
      }
    }
    else if (scalarType == Detail::Int32)
    {
      Int32ArrayType::Pointer p = Int32ArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->GetName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->SetValue(v, ptr->GetValue(v) );
      }
    }
    else if (scalarType == Detail::UInt32)
    {
      UInt32ArrayType::Pointer p = UInt32ArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->GetName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->SetValue(v, ptr->GetValue(v) );
      }
    }
    else if (scalarType == Detail::Int64)
    {
      Int64ArrayType::Pointer p = Int64ArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->GetName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->SetValue(v, ptr->GetValue(v) );
      }
    }
    else if (scalarType == Detail::UInt64)
    {
      UInt64ArrayType::Pointer p = UInt64ArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->GetName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->SetValue(v, ptr->GetValue(v) );
      }
    }
    else if (scalarType == Detail::Float)
    {
      FloatArrayType::Pointer p = FloatArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->GetName(), p);
    }
    else if (scalarType == Detail::Double)
    {
      DoubleArrayType::Pointer p = DoubleArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->GetName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->SetValue(v, ptr->GetValue(v) );
      }
    }


  }

} // End Namespace Detail

#define CHECK_AND_CONVERT(Type, DataContainer, ScalarType, Array, AttributeMatrixName, OutputName)\
  if(false == completed) {\
    Type* Type##Ptr = Type::SafePointerDownCast(Array.get());\
    if (NULL != Type##Ptr) {\
      Detail::ConvertData<Type>(Type##Ptr, DataContainer, ScalarType, AttributeMatrixName, OutputName);\
      completed = true;\
    }\
  }



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertData::ConvertData() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_ScalarType(0),
  m_OutputArrayName(""),
  m_SelectedCellArrayName("")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertData::~ConvertData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::setupFilterParameters()
{
  FilterParameterVector parameters;
  /* Place all your option initialization code here */

  /* To Display a Combobox with a list of current Voxel Cell Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Volume Cell Array Name");
    option->setPropertyName("SelectedCellArrayName");
    option->setWidgetType(FilterParameter::VolumeCellArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    parameters.push_back(option);
  }
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Scalar Type");
    parameter->setPropertyName("ScalarType");
    parameter->setWidgetType(FilterParameter::ChoiceWidget);
    parameter->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("signed   int 8  bit");
    choices.push_back("unsigned int 8  bit");
    choices.push_back("signed   int 16 bit");
    choices.push_back("unsigned int 16 bit");
    choices.push_back("signed   int 32 bit");
    choices.push_back("unsigned int 32 bit");
    choices.push_back("signed   int 64 bit");
    choices.push_back("unsigned int 64 bit");
    choices.push_back("       Float 32 bit");
    choices.push_back("      Double 64 bit");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output Array Name");
    option->setPropertyName("OutputArrayName");
    option->setWidgetType(FilterParameter::StringWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setSelectedCellArrayName( reader->readString( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  setScalarType( reader->readValue("ScalarType", getScalarType()) );
  setOutputArrayName( reader->readString( "OutputArrayName", getOutputArrayName() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ConvertData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  writer->writeValue("ScalarType", getScalarType() );
  writer->writeValue("OutputArrayName", getOutputArrayName() );
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::dataCheck(bool preflight)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  QString ss;
  if(m_SelectedCellArrayName.isEmpty() == true)
  {
    ss = QObject::tr("The Input Voxel Cell Array Name is blank (empty) and a value must be filled in for the pipeline to complete.");
    setErrorCondition(-397);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(m_OutputArrayName.isEmpty() == true)
  {
    ss = QObject::tr("The Output Array Name is blank (empty) and a value must be filled in for the pipeline to complete.");
    setErrorCondition(-398);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  int numberOfComponents = 0;
  IDataArray::Pointer iArray = cellAttrMat->getAttributeArray(m_SelectedCellArrayName);
  if (NULL != iArray)
  {
    numberOfComponents = iArray->GetNumberOfComponents();
  }

  if (true == preflight)
  {
    IDataArray::Pointer p = IDataArray::NullPointer();
    QVector<int> dims(1, numberOfComponents);
    int64_t voxels = cellAttrMat->getNumTuples();
    if (m_ScalarType == Detail::Int8)
    {
      p = Int8ArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::UInt8)
    {
      p = UInt8ArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::Int16)
    {
      p = Int16ArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::UInt16)
    {
      p = UInt16ArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::Int32)
    {
      p = Int32ArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::UInt32)
    {
      p = UInt32ArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::Int64)
    {
      p = Int64ArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::UInt64)
    {
      p = UInt64ArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::Float)
    {
      p = FloatArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::Double)
    {
      p = DoubleArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    }
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(p->GetName(), p);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::preflight()
{
  dataCheck(true);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::execute()
{
  int err = 0;
  setErrorCondition(err);

  dataCheck(false);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  IDataArray::Pointer iArray = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(m_SelectedCellArrayName);
  bool completed = false;

  CHECK_AND_CONVERT(UInt8ArrayType, m, m_ScalarType, iArray, getCellAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(Int8ArrayType, m, m_ScalarType, iArray, getCellAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(UInt16ArrayType, m, m_ScalarType, iArray, getCellAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(Int16ArrayType, m, m_ScalarType, iArray, getCellAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(UInt32ArrayType, m, m_ScalarType, iArray, getCellAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(Int32ArrayType, m, m_ScalarType, iArray, getCellAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(UInt64ArrayType, m, m_ScalarType, iArray, getCellAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(Int64ArrayType, m, m_ScalarType, iArray, getCellAttributeMatrixName(), m_OutputArrayName)

  CHECK_AND_CONVERT(FloatArrayType, m, m_ScalarType, iArray, getCellAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(DoubleArrayType, m, m_ScalarType, iArray, getCellAttributeMatrixName(), m_OutputArrayName)

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
