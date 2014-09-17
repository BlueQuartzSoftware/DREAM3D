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
#include "GrayToRGB.h"

//thresholding filter
#include "itkComposeImageFilter.h"

#include "DREAM3DLib/Common/TemplateHelpers.hpp"

// ImageProcessing Plugin
#include "ItkBridge.h"


/**
 * @brief This is a private implementation for the filter that handles the actual algorithm implementation details
 * for us like figuring out if we can use this private implementation with the data array that is assigned.
 */
template<typename PixelType>
class GrayToRGBPrivate
{
  public:
    typedef DataArray<PixelType> DataArrayType;

    GrayToRGBPrivate() {}
    virtual ~GrayToRGBPrivate() {}

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
    void static Execute(GrayToRGB* filter, IDataArray::Pointer redInputIDataArray, IDataArray::Pointer greenInputIDataArray, IDataArray::Pointer blueInputIDataArray, IDataArray::Pointer outputIDataArray, VolumeDataContainer* m, QString attrMatName)
    {
      typename DataArrayType::Pointer redInputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(redInputIDataArray);
      typename DataArrayType::Pointer greenInputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(greenInputIDataArray);
      typename DataArrayType::Pointer blueInputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(blueInputIDataArray);
      typename DataArrayType::Pointer outputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(outputIDataArray);

      //convert arrays to correct type
      PixelType* redData = static_cast<PixelType*>(redInputDataPtr->getPointer(0));
      PixelType* greenData = static_cast<PixelType*>(greenInputDataPtr->getPointer(0));
      PixelType* blueData = static_cast<PixelType*>(blueInputDataPtr->getPointer(0));
      PixelType* outputData = static_cast<PixelType*>(outputDataPtr->getPointer(0));

      size_t numVoxels = redInputDataPtr->getNumberOfTuples();

      typedef ItkBridge<PixelType> ItkBridgeType;

      //wrap inputs as itk images
      typedef itk::Image<PixelType, ImageProcessing::ImageDimension> ImageType;
      typename ImageType::Pointer redImage = ItkBridgeType::CreateItkWrapperForDataPointer(m, attrMatName, redData);
      typename ImageType::Pointer greenImage = ItkBridgeType::CreateItkWrapperForDataPointer(m, attrMatName, greenData);
      typename ImageType::Pointer blueImage = ItkBridgeType::CreateItkWrapperForDataPointer(m, attrMatName, blueData);

      //define threshold filters
      typedef itk::ComposeImageFilter<typename ItkBridgeType::ScalarImageType, typename ItkBridgeType::RGBImageType> ComposeRGBType;

      //threshold
      typename ComposeRGBType::Pointer composeRGB = ComposeRGBType::New();
      composeRGB->SetInput(0, redImage);
      composeRGB->SetInput(1, greenImage);
      composeRGB->SetInput(2, blueImage);
      composeRGB->GetOutput()->GetPixelContainer()->SetImportPointer(reinterpret_cast<typename ItkBridgeType::RGBImageType::PixelType*>(outputData), numVoxels, false);

      try
      {
        composeRGB->Update();
      }
      catch( itk::ExceptionObject & err )
      {
        filter->setErrorCondition(-5);
        QString ss = QObject::tr("Failed to convert image. Error Message returned from ITK:\n   %1").arg(err.GetDescription());
        filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
      }
    }
  private:
    GrayToRGBPrivate(const GrayToRGBPrivate&); // Copy Constructor Not Implemented
    void operator=(const GrayToRGBPrivate&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrayToRGB::GrayToRGB() :
  AbstractFilter(),
  m_RedArrayPath("", "", ""),
  m_GreenArrayPath("", "", ""),
  m_BlueArrayPath("", "", ""),
  m_NewCellArrayName(""),
  m_Red(NULL),
  m_Green(NULL),
  m_Blue(NULL),
  m_NewCellArray(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrayToRGB::~GrayToRGB()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrayToRGB::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Red Channel", "RedArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getRedArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Green Channel", "GreenArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getGreenArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Blue Channel", "BlueArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getBlueArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Created Array Name", "NewCellArrayName", FilterParameterWidgetType::StringWidget, getNewCellArrayName(), false, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrayToRGB::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setRedArrayPath( reader->readDataArrayPath( "RedArrayPath", getRedArrayPath() ) );
  setGreenArrayPath( reader->readDataArrayPath( "GreenArrayPath", getGreenArrayPath() ) );
  setBlueArrayPath( reader->readDataArrayPath( "BlueArrayPath", getBlueArrayPath() ) );
  setNewCellArrayName( reader->readString( "NewCellArrayName", getNewCellArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GrayToRGB::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(RedArrayPath)
      DREAM3D_FILTER_WRITE_PARAMETER(GreenArrayPath)
      DREAM3D_FILTER_WRITE_PARAMETER(BlueArrayPath)
      DREAM3D_FILTER_WRITE_PARAMETER(NewCellArrayName)
      writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrayToRGB::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  //check for required arrays
  QVector<size_t> compDims(1, 1);
  m_RedPtr = TemplateHelpers::GetPrereqArrayFromPath<AbstractFilter, VolumeDataContainer>()(this, getRedArrayPath(), compDims);
  if(NULL != m_RedPtr.lock().get())
  {
    m_Red = m_RedPtr.lock().get();
  }
  m_GreenPtr = TemplateHelpers::GetPrereqArrayFromPath<AbstractFilter, VolumeDataContainer>()(this, getGreenArrayPath(), compDims);
  if(NULL != m_GreenPtr.lock().get())
  {
    m_Green = m_GreenPtr.lock().get();
  }
  m_BluePtr = TemplateHelpers::GetPrereqArrayFromPath<AbstractFilter, VolumeDataContainer>()(this, getBlueArrayPath(), compDims);
  if(NULL != m_BluePtr.lock().get())
  {
    m_Blue = m_BluePtr.lock().get();
  }

  //configured created name / location
  tempPath.update(getRedArrayPath().getDataContainerName(), getRedArrayPath().getAttributeMatrixName(), getNewCellArrayName() );


  VolumeDataContainer* redDC = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getRedArrayPath().getDataContainerName() );
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer redAM = redDC->getPrereqAttributeMatrix<AbstractFilter>(this, getRedArrayPath().getAttributeMatrixName(), 80000);
  if(getErrorCondition() < 0) { return; }
  IDataArray::Pointer redArrayptr = redAM->getExistingPrereqArray<IDataArray, AbstractFilter>(this, getRedArrayPath().getDataArrayName(), 80000);
  if(getErrorCondition() < 0) { return; }

  //create new array of same type
  compDims[0]=3;
  m_NewCellArrayPtr = TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath, compDims, redArrayptr);
  if( NULL != m_NewCellArrayPtr.lock().get() )
  {
    m_NewCellArray = m_NewCellArrayPtr.lock()->getVoidPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrayToRGB::preflight()
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
void GrayToRGB::execute()
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
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getRedArrayPath().getDataContainerName());
  QString attrMatName = getRedArrayPath().getAttributeMatrixName();

  //get input and output data
  IDataArray::Pointer redData = m_RedPtr.lock();
  IDataArray::Pointer greenData = m_GreenPtr.lock();
  IDataArray::Pointer blueData = m_BluePtr.lock();
  IDataArray::Pointer outputData = m_NewCellArrayPtr.lock();

  //execute type dependant portion using a Private Implementation that takes care of figuring out if
  // we can work on the correct type and actually handling the algorithm execution. We pass in "this" so
  // that the private implementation can get access to the current object to pass up status notifications,
  // progress or handle "cancel" if needed.
  if(GrayToRGBPrivate<int8_t>()(redData))
  {
    GrayToRGBPrivate<int8_t>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<uint8_t>()(redData) )
  {
    GrayToRGBPrivate<uint8_t>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<int16_t>()(redData) )
  {
    GrayToRGBPrivate<int16_t>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<uint16_t>()(redData) )
  {
    GrayToRGBPrivate<uint16_t>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<int32_t>()(redData) )
  {
    GrayToRGBPrivate<int32_t>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<uint32_t>()(redData) )
  {
    GrayToRGBPrivate<uint32_t>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<int64_t>()(redData) )
  {
    GrayToRGBPrivate<int64_t>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<uint64_t>()(redData) )
  {
    GrayToRGBPrivate<uint64_t>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<float>()(redData) )
  {
    GrayToRGBPrivate<float>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<double>()(redData) )
  {
    GrayToRGBPrivate<double>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else
  {
    setErrorCondition(-10001);
    ss = QObject::tr("A Supported DataArray type was not used for an input array.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  //array name changing/cleanup
  AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(m_RedArrayPath.getAttributeMatrixName());
  attrMat->addAttributeArray(getNewCellArrayName(), outputData);


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GrayToRGB::newFilterInstance(bool copyFilterParameters)
{
  GrayToRGB::Pointer filter = GrayToRGB::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GrayToRGB::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GrayToRGB::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GrayToRGB::getSubGroupName()
{return "Misc";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GrayToRGB::getHumanLabel()
{return "Gray to RGB (Merge Channels)";}

