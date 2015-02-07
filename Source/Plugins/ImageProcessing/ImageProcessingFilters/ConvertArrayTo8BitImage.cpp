/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2013 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ConvertArrayTo8BitImage.h"

#include "DREAM3DLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertArrayTo8BitImage::ConvertArrayTo8BitImage() :
  AbstractFilter(),
  m_SelectedArrayPath("", "", ""),
  m_NewArrayArrayName(""),
  m_NewArray(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertArrayTo8BitImage::~ConvertArrayTo8BitImage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertArrayTo8BitImage::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Array To Convert", "SelectedArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedArrayPath(), false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("New Array Name", "NewArrayArrayName", FilterParameterWidgetType::StringWidget, getNewArrayArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertArrayTo8BitImage::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewArrayArrayName(reader->readString("NewArrayArrayName", getNewArrayArrayName() ) );
  setSelectedArrayPath( reader->readDataArrayPath( "SelectedArrayPath", getSelectedArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ConvertArrayTo8BitImage::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(NewArrayArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertArrayTo8BitImage::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  if(m_SelectedArrayPath.isEmpty() == true)
  {
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), "An array from the DataContainer must be selected.", getErrorCondition());
  }
  else
  {
    IDataArray::Pointer inputData = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName())->getAttributeMatrix(m_SelectedArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedArrayPath.getDataArrayName());
    if (NULL == inputData.get())
    {
      QString ss = QObject::tr("Data array '%1' does not exist in the DataContainer. Was it spelled correctly?").arg(m_SelectedArrayPath.getDataArrayName());
      setErrorCondition(-11001);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    else
    {
      if(inputData->getNumberOfComponents() > 1)
      {
        QString ss = QObject::tr("Data Array '%1' cannot have more than 1 component").arg(m_SelectedArrayPath.getDataArrayName());
        setErrorCondition(-11002);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
      QVector<size_t> dims(1, 1);
      tempPath.update(m_SelectedArrayPath.getDataContainerName(), m_SelectedArrayPath.getAttributeMatrixName(), getNewArrayArrayName() );
      m_NewArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
      if( NULL != m_NewArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
      { m_NewArray = m_NewArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
      if(getErrorCondition() < 0) { return; }

      ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getSelectedArrayPath().getDataContainerName())->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
      if(getErrorCondition() < 0 || NULL == image.get()) { return; }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertArrayTo8BitImage::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void scaleArray(IDataArray::Pointer inputData, uint8_t* newArray)
{
  DataArray<T>* inputArray = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == inputArray)
  {
    return;
  }

  T* inputArrayPtr = inputArray->getPointer(0);
  size_t numPoints = inputArray->getNumberOfTuples();

  float min = 1000000.0f;
  float max = 0.0f;
  float value;
  float scaledValue;
  for (size_t i = 0; i < numPoints; i++)
  {
    value = inputArrayPtr[i];
    if(value > max) { max = value; }
    if(value < min) { min = value; }
  }

  for (size_t i = 0; i < numPoints; i++)
  {
    scaledValue = (inputArrayPtr[i] - min) / (max - min);
    if(scaledValue < 0.0) scaledValue = 0.0f;
    else if(scaledValue > 1.0f) scaledValue = 1.0f;
    scaledValue *= 255.0f;
    newArray[i] = static_cast<uint8_t>(scaledValue);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertArrayTo8BitImage::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName());

  QString ss;

  IDataArray::Pointer inputData = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName())->getAttributeMatrix(m_SelectedArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedArrayPath.getDataArrayName());

  QString dType = inputData->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();
  if (dType.compare("int8_t") == 0)
  {
    scaleArray<int8_t>(inputData, m_NewArray);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    scaleArray<uint8_t>(inputData, m_NewArray);
  }
  else if (dType.compare("int16_t") == 0)
  {
    scaleArray<int16_t>(inputData, m_NewArray);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    scaleArray<uint16_t>(inputData, m_NewArray);
  }
  else if (dType.compare("int32_t") == 0)
  {
    scaleArray<int32_t>(inputData, m_NewArray);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    scaleArray<uint32_t>(inputData, m_NewArray);
  }
  else if (dType.compare("int64_t") == 0)
  {
    scaleArray<int64_t>(inputData, m_NewArray);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    scaleArray<uint64_t>(inputData, m_NewArray);
  }
  else if (dType.compare("float") == 0)
  {
    scaleArray<float>(inputData, m_NewArray);
  }
  else if (dType.compare("double") == 0)
  {
    scaleArray<double>(inputData, m_NewArray);
  }
  else if (dType.compare("bool") == 0)
  {
    scaleArray<bool>(inputData, m_NewArray);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ConvertArrayTo8BitImage::newFilterInstance(bool copyFilterParameters)
{
  ConvertArrayTo8BitImage::Pointer filter = ConvertArrayTo8BitImage::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertArrayTo8BitImage::getCompiledLibraryName()
{ return ImageProcessing::ImageProcessingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertArrayTo8BitImage::getGroupName()
{ return "ImageProcessing"; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertArrayTo8BitImage::getSubGroupName()
{ return "Misc"; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertArrayTo8BitImage::getHumanLabel()
{ return "Convert Array To 8 Bit Image"; }

