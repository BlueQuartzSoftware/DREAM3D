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
  void ConvertData(T* ptr, QVector<size_t> dims, VolumeDataContainer* m, int32_t scalarType, const QString attributeMatrixName, const QString& name)
  {
    int voxels = ptr->getNumberOfTuples();
    size_t size = ptr->getSize();

    if (scalarType == Detail::Int8)
    {
      Int8ArrayType::Pointer p = Int8ArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->setValue(v, ptr->getValue(v) );
      }
    }
    else if (scalarType == Detail::UInt8)
    {
      UInt8ArrayType::Pointer p = UInt8ArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->setValue(v, ptr->getValue(v) );
      }
    }
    else if (scalarType == Detail::Int16)
    {
      Int16ArrayType::Pointer p = Int16ArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->setValue(v, ptr->getValue(v) );
      }
    }
    else if (scalarType == Detail::UInt16)
    {
      UInt16ArrayType::Pointer p = UInt16ArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->setValue(v, ptr->getValue(v) );
      }
    }
    else if (scalarType == Detail::Int32)
    {
      Int32ArrayType::Pointer p = Int32ArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->setValue(v, ptr->getValue(v) );
      }
    }
    else if (scalarType == Detail::UInt32)
    {
      UInt32ArrayType::Pointer p = UInt32ArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->setValue(v, ptr->getValue(v) );
      }
    }
    else if (scalarType == Detail::Int64)
    {
      Int64ArrayType::Pointer p = Int64ArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->setValue(v, ptr->getValue(v) );
      }
    }
    else if (scalarType == Detail::UInt64)
    {
      UInt64ArrayType::Pointer p = UInt64ArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->setValue(v, ptr->getValue(v) );
      }
    }
    else if (scalarType == Detail::Float)
    {
      FloatArrayType::Pointer p = FloatArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
    }
    else if (scalarType == Detail::Double)
    {
      DoubleArrayType::Pointer p = DoubleArrayType::CreateArray(voxels, dims, name);
      m->getAttributeMatrix(attributeMatrixName)->addAttributeArray(p->getName(), p);
      for(size_t v = 0; v < size; ++v)
      {
        p->setValue(v, ptr->getValue(v) );
      }
    }
  }
} // End Namespace Detail

#define CHECK_AND_CONVERT(Type, DataContainer, ScalarType, Array, AttributeMatrixName, OutputName)\
  if(false == completed) {\
    Type* Type##Ptr = Type::SafePointerDownCast(Array.get());\
    if (NULL != Type##Ptr) {\
      QVector<size_t> dims = Array->getComponentDimensions();\
      Detail::ConvertData<Type>(Type##Ptr, dims, DataContainer, ScalarType, AttributeMatrixName, OutputName);\
      completed = true;\
    }\
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertData::ConvertData() :
  AbstractFilter(),
  m_ScalarType(0),
  m_OutputArrayName(""),
  m_SelectedCellArrayPath("", "", "")
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
  parameters.push_back(FilterParameter::New("Attribute Array Name", "SelectedCellArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayPath(), false));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Scalar Type");
    parameter->setPropertyName("ScalarType");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    ////parameter->setValueType("unsigned int");
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
  parameters.push_back(FilterParameter::New("Output Array Name", "OutputArrayName", FilterParameterWidgetType::StringWidget, getOutputArrayName(), false));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  setScalarType( reader->readValue("ScalarType", getScalarType()) );
  setOutputArrayName( reader->readString( "OutputArrayName", getOutputArrayName() ) );
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
  writer->writeValue("SelectedCellArrayPath", getSelectedCellArrayPath() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::dataCheck(bool preflight)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, m_SelectedCellArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, m_SelectedCellArrayPath.getAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }

  QString ss;
  if(m_SelectedCellArrayPath.isEmpty() == true)
  {
    ss = QObject::tr("The Input Cell Array Name is blank (empty) and a value must be filled in for the pipeline to complete.");
    setErrorCondition(-397);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(m_OutputArrayName.isEmpty() == true)
  {
    ss = QObject::tr("The Output Array Name is blank (empty) and a value must be filled in for the pipeline to complete.");
    setErrorCondition(-398);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  int numberOfComponents = 0;
  if (true == preflight)
  {
    //  IDataArray::Pointer  = IDataArray::NullPointer();
    IDataArray::Pointer p = cellAttrMat->getAttributeArray(m_SelectedCellArrayPath.getDataArrayName());
    if (NULL == p.get())
    {
      ss = QObject::tr("The input array '%1' was not found in the AttributeMatrix '%2'.").arg(m_SelectedCellArrayPath.getDataArrayName()).arg(m_SelectedCellArrayPath.getAttributeMatrixName());
      setErrorCondition(-398);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    numberOfComponents = p->getNumberOfComponents();
    QVector<size_t> dims = p->getComponentDimensions();
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
    cellAttrMat->addAttributeArray(p->getName(), p);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck(true);
  emit preflightExecuted();
  setInPreflight(false);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertData::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck(false);
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_SelectedCellArrayPath.getDataContainerName());

  IDataArray::Pointer iArray = m->getAttributeMatrix(m_SelectedCellArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedCellArrayPath.getDataArrayName());
  bool completed = false;

  CHECK_AND_CONVERT(UInt8ArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(Int8ArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(UInt16ArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(Int16ArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(UInt32ArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(Int32ArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(UInt64ArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(Int64ArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(FloatArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)
  CHECK_AND_CONVERT(DoubleArrayType, m, m_ScalarType, iArray, m_SelectedCellArrayPath.getAttributeMatrixName(), m_OutputArrayName)

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ConvertData::newFilterInstance(bool copyFilterParameters)
{
  /*
  * ScalarType
  * OutputArrayName
  * SelectedCellArrayName
  */
  ConvertData::Pointer filter = ConvertData::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}
