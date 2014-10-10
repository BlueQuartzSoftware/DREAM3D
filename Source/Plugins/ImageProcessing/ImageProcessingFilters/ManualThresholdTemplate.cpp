/* ============================================================================
 * Copyright (c) 2014 DREAM3D Consortium
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
 * Neither the names of any of the DREAM3D Consortium contributors
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
 *  This code was partially written under United States Air Force Contract number
 *                              FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ManualThresholdTemplate.h"

#include <string>

//thresholding filter
#include "itkBinaryThresholdImageFilter.h"

#include "DREAM3DLib/Common/TemplateHelpers.hpp"

// ImageProcessing Plugin
#include "ItkBridge.h"


/**
 * @brief This is a private implementation for the filter that handles the actual algorithm implementation details
 * for us like figuring out if we can use this private implementation with the data array that is assigned.
 */
template<typename PixelType>
class ManualThresholdTemplatePrivate
{
  public:
    typedef DataArray<PixelType> DataArrayType;

    ManualThresholdTemplatePrivate() {}
    virtual ~ManualThresholdTemplatePrivate() {}

    // -----------------------------------------------------------------------------
    // Determine if this is the proper type of an array to downcast from the IDataArray
    // -----------------------------------------------------------------------------
    bool operator()(IDataArray::Pointer p)
    {
      return (boost::dynamic_pointer_cast<DataArrayType>(p).get() != NULL);
    }

    // -----------------------------------------------------------------------------
    // This is the actual templated algorithm
    // -----------------------------------------------------------------------------
    void static Execute(ManualThresholdTemplate* filter, IDataArray::Pointer inputIDataArray, IDataArray::Pointer outputIDataArray, PixelType manParameter, VolumeDataContainer* m, QString attrMatName)
    {
      typename DataArrayType::Pointer inputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(inputIDataArray);
      typename DataArrayType::Pointer outputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(outputIDataArray);

      //convert arrays to correct type
      PixelType* inputData = static_cast<PixelType*>(inputDataPtr->getPointer(0));
      PixelType* outputData = static_cast<PixelType*>(outputDataPtr->getPointer(0));

      size_t numVoxels = inputDataPtr->getNumberOfTuples();

      typedef ItkBridge<PixelType> ItkBridgeType;

      //wrap input as itk image
      typedef itk::Image<PixelType, ImageProcessing::ImageDimension> ImageType;
      typename ImageType::Pointer inputImage = ItkBridgeType::CreateItkWrapperForDataPointer(m, attrMatName, inputData);

      //define threshold filters
      typedef itk::BinaryThresholdImageFilter <ImageType, ImageType> BinaryThresholdImageFilterType;

      //threshold
      typename BinaryThresholdImageFilterType::Pointer thresholdFilter = BinaryThresholdImageFilterType::New();
      thresholdFilter->SetInput(inputImage);
      thresholdFilter->SetLowerThreshold(manParameter);
      thresholdFilter->SetUpperThreshold(255);
      thresholdFilter->SetInsideValue(255);
      thresholdFilter->SetOutsideValue(0);
      thresholdFilter->GetOutput()->GetPixelContainer()->SetImportPointer(outputData, numVoxels, false);
      try
      {
        thresholdFilter->Update();
      }
      catch( itk::ExceptionObject & err )
      {
        filter->setErrorCondition(-5);
        QString ss = QObject::tr("Failed to execute itk::BinaryThresholdImageFilter filter. Error Message returned from ITK:\n   %1").arg(err.GetDescription());
        filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
      }
    }
  private:
    ManualThresholdTemplatePrivate(const ManualThresholdTemplatePrivate&); // Copy Constructor Not Implemented
    void operator=(const ManualThresholdTemplatePrivate&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ManualThresholdTemplate::ManualThresholdTemplate() :
  AbstractFilter(),
  m_SelectedCellArrayArrayPath("", "", ""),
  m_NewCellArrayName(""),
  m_SaveAsNewArray(true),
  m_ManualParameter(128),
  m_SelectedCellArrayArrayName(""),
  m_SelectedCellArray(NULL),
  m_NewCellArray(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ManualThresholdTemplate::~ManualThresholdTemplate()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ManualThresholdTemplate::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Array to Process", "SelectedCellArrayArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayArrayPath(), false, ""));
  QStringList linkedProps;
  linkedProps << "NewCellArrayName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Save As New Array", "SaveAsNewArray", getSaveAsNewArray(), linkedProps, false));
  parameters.push_back(FilterParameter::New("Created Array Name", "NewCellArrayName", FilterParameterWidgetType::StringWidget, getNewCellArrayName(), false, ""));
  parameters.push_back(FilterParameter::New("Threshold Value", "ManualParameter", FilterParameterWidgetType::IntWidget, getManualParameter(), false, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ManualThresholdTemplate::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayArrayPath( reader->readDataArrayPath( "SelectedCellArrayArrayPath", getSelectedCellArrayArrayPath() ) );
  setNewCellArrayName( reader->readString( "NewCellArrayName", getNewCellArrayName() ) );
  setSaveAsNewArray( reader->readValue( "SaveAsNewArray", getSaveAsNewArray() ) );
  setManualParameter( reader->readValue( "ManualParameter", getManualParameter() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ManualThresholdTemplate::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedCellArrayArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NewCellArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SaveAsNewArray)
  DREAM3D_FILTER_WRITE_PARAMETER(ManualParameter)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ManualThresholdTemplate::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  //check for required arrays
  QVector<size_t> compDims(1, 1);

  m_SelectedCellArrayPtr = TemplateHelpers::GetPrereqArrayFromPath<AbstractFilter, VolumeDataContainer>()(this, getSelectedCellArrayArrayPath(), compDims);
  if(NULL != m_SelectedCellArrayPtr.lock().get())
  {
    m_SelectedCellArray = m_SelectedCellArrayPtr.lock().get();
  }

  //configured created name / location
  if(m_SaveAsNewArray == false) { m_NewCellArrayName = "thisIsATempName"; }
  tempPath.update(getSelectedCellArrayArrayPath().getDataContainerName(), getSelectedCellArrayArrayPath().getAttributeMatrixName(), getNewCellArrayName() );

  // We can safely just get the pointers without checking if they are NULL because that was effectively done above in the GetPrereqArray call
  VolumeDataContainer* dc = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getSelectedCellArrayArrayPath().getDataContainerName() );
  AttributeMatrix::Pointer am = dc->getPrereqAttributeMatrix<AbstractFilter>(this, getSelectedCellArrayArrayPath().getAttributeMatrixName(), 80000);
  IDataArray::Pointer data = am->getExistingPrereqArray<IDataArray, AbstractFilter>(this, getSelectedCellArrayArrayPath().getDataArrayName(), 80000);

  m_NewCellArrayPtr = TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath, compDims, data);
  if( NULL != m_NewCellArrayPtr.lock().get() )
  {
    m_NewCellArray = m_NewCellArrayPtr.lock()->getVoidPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ManualThresholdTemplate::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename PixelType>
void filter(IDataArray::Pointer inputIDataArray, IDataArray::Pointer outputIDataArray, PixelType manParameter, VolumeDataContainer* m, QString attrMatName)
{
  typedef DataArray<PixelType> DataArrayType;
  typename DataArrayType::Pointer inputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(inputIDataArray);
  typename DataArrayType::Pointer outputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(outputIDataArray);

  //convert arrays to correct type
  PixelType* inputData = static_cast<PixelType*>(inputDataPtr->getPointer(0));
  PixelType* outputData = static_cast<PixelType*>(outputDataPtr->getPointer(0));

  size_t numVoxels = inputDataPtr->getNumberOfTuples();

  typedef ItkBridge<PixelType> ItkBridgeType;

  //wrap input as itk image
  typedef itk::Image<PixelType, ImageProcessing::ImageDimension> ImageType;
  typename ImageType::Pointer inputImage = ItkBridgeType::CreateItkWrapperForDataPointer(m, attrMatName, inputData);

  //define threshold filters
  typedef itk::BinaryThresholdImageFilter <ImageType, ImageType> BinaryThresholdImageFilterType;

  //threshold
  typename BinaryThresholdImageFilterType::Pointer thresholdFilter = BinaryThresholdImageFilterType::New();
  thresholdFilter->SetInput(inputImage);
  thresholdFilter->SetLowerThreshold(manParameter);
  thresholdFilter->SetUpperThreshold(255);
  thresholdFilter->SetInsideValue(255);
  thresholdFilter->SetOutsideValue(0);
  thresholdFilter->GetOutput()->GetPixelContainer()->SetImportPointer(outputData, numVoxels, false);
  thresholdFilter->Update();
}

/**example without itk
template<typename PixelType>
void filter(IDataArray::Pointer inputIDataArray, IDataArray::Pointer outputIDataArray, PixelType manParameter)
{
  typedef DataArray<PixelType> DataArrayType;
  typename DataArrayType::Pointer inputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(inputIDataArray);
  typename DataArrayType::Pointer outputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(outputIDataArray);

  //convert arrays to correct type
  PixelType* inputData = static_cast<PixelType*>(inputDataPtr->getPointer(0));
  PixelType* outputData = static_cast<PixelType*>(outputDataPtr->getPointer(0));

  size_t numVoxels = inputDataPtr->getNumberOfTuples();

  for(size_t i=0; i<numVoxels; i++) {
    if(inputData[i]>=manParameter) {
      outputData[i]=255;
    } else {
      outputData[i]=0;
    }
  }
}
*/

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ManualThresholdTemplate::execute()
{
  QString ss;
  dataCheck();
  if(getErrorCondition() < 0)
  {
    setErrorCondition(-10000);
    ss = QObject::tr("DataCheck did not pass during execute");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  //get volume container
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getSelectedCellArrayArrayPath().getDataContainerName());
  QString attrMatName = getSelectedCellArrayArrayPath().getAttributeMatrixName();

  //get input and output data
  IDataArray::Pointer inputData = m_SelectedCellArrayPtr.lock();
  IDataArray::Pointer outputData = m_NewCellArrayPtr.lock();

  //execute type dependant portion using a Private Implementation that takes care of figuring out if
  // we can work on the correct type and actually handling the algorithm execution. We pass in "this" so
  // that the private implementation can get access to the current object to pass up status notifications,
  // progress or handle "cancel" if needed.
  if(ManualThresholdTemplatePrivate<int8_t>()(inputData))
  {
    ManualThresholdTemplatePrivate<int8_t>::Execute(this, inputData, outputData, getManualParameter(), m, attrMatName);
  }
  else if(ManualThresholdTemplatePrivate<uint8_t>()(inputData) )
  {
    ManualThresholdTemplatePrivate<uint8_t>::Execute(this, inputData, outputData, getManualParameter(), m, attrMatName);
  }
  else if(ManualThresholdTemplatePrivate<int16_t>()(inputData) )
  {
    ManualThresholdTemplatePrivate<int16_t>::Execute(this, inputData, outputData, getManualParameter(), m, attrMatName);
  }
  else if(ManualThresholdTemplatePrivate<uint16_t>()(inputData) )
  {
    ManualThresholdTemplatePrivate<uint16_t>::Execute(this, inputData, outputData, getManualParameter(), m, attrMatName);
  }
  else if(ManualThresholdTemplatePrivate<int32_t>()(inputData) )
  {
    ManualThresholdTemplatePrivate<int32_t>::Execute(this, inputData, outputData, getManualParameter(), m, attrMatName);
  }
  else if(ManualThresholdTemplatePrivate<uint32_t>()(inputData) )
  {
    ManualThresholdTemplatePrivate<uint32_t>::Execute(this, inputData, outputData, getManualParameter(), m, attrMatName);
  }
  else if(ManualThresholdTemplatePrivate<int64_t>()(inputData) )
  {
    ManualThresholdTemplatePrivate<int64_t>::Execute(this, inputData, outputData, getManualParameter(), m, attrMatName);
  }
  else if(ManualThresholdTemplatePrivate<uint64_t>()(inputData) )
  {
    ManualThresholdTemplatePrivate<uint64_t>::Execute(this, inputData, outputData, getManualParameter(), m, attrMatName);
  }
  else if(ManualThresholdTemplatePrivate<float>()(inputData) )
  {
    ManualThresholdTemplatePrivate<float>::Execute(this, inputData, outputData, getManualParameter(), m, attrMatName);
  }
  else if(ManualThresholdTemplatePrivate<double>()(inputData) )
  {
    ManualThresholdTemplatePrivate<double>::Execute(this, inputData, outputData, getManualParameter(), m, attrMatName);
  }
  else
  {
    setErrorCondition(-10001);
    ss = QObject::tr("A Supported DataArray type was not used for an input array.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  //array name changing/cleanup
  AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(m_SelectedCellArrayArrayPath.getAttributeMatrixName());
  if(m_SaveAsNewArray == true)
  {
    attrMat->addAttributeArray(getNewCellArrayName(), outputData);
  }
  else
  {
    attrMat->removeAttributeArray(m_SelectedCellArrayArrayPath.getDataArrayName());
    outputData->setName(m_SelectedCellArrayArrayPath.getDataArrayName());
    attrMat->addAttributeArray(outputData->getName(), outputData);
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ManualThresholdTemplate::newFilterInstance(bool copyFilterParameters)
{
  ManualThresholdTemplate::Pointer filter = ManualThresholdTemplate::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ManualThresholdTemplate::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ManualThresholdTemplate::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ManualThresholdTemplate::getSubGroupName()
{return "Misc";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ManualThresholdTemplate::getHumanLabel()
{return "Threshold Image Template (Manual - Single Level)";}

