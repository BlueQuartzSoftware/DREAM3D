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
#include "SobelEdge.h"

#include <QtCore/QString>

#include "ItkBridge.h"
#include "itkSobelEdgeDetectionImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SobelEdge::SobelEdge() :
  AbstractFilter(),
  m_SelectedCellArrayPath("", "", ""),
  m_NewCellArrayName(""),
  m_SaveAsNewArray(true),
  m_Slice(false),
  m_SelectedCellArrayArrayName(""),
  m_SelectedCellArray(NULL),
  m_NewCellArray(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SobelEdge::~SobelEdge()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SobelEdge::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Array to Process", "SelectedCellArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayPath(), false, ""));
  QStringList linkedProps;
  linkedProps << "NewCellArrayName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Save As New Array", "SaveAsNewArray", getSaveAsNewArray(), linkedProps, false));

  parameters.push_back(FilterParameter::New("Created Array Name", "NewCellArrayName", FilterParameterWidgetType::StringWidget, getNewCellArrayName(), false, ""));
  parameters.push_back(FilterParameter::New("Slice at a Time", "Slice", FilterParameterWidgetType::BooleanWidget, getSlice(), false));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SobelEdge::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  setNewCellArrayName( reader->readString( "NewCellArrayName", getNewCellArrayName() ) );
  setSaveAsNewArray( reader->readValue( "SaveAsNewArray", getSaveAsNewArray() ) );
  setSlice( reader->readValue( "Slice", getSlice() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SobelEdge::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedCellArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NewCellArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SaveAsNewArray)
  DREAM3D_FILTER_WRITE_PARAMETER(Slice)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SobelEdge::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  QVector<size_t> dims(1, 1);
  m_SelectedCellArrayPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<ImageProcessing::DefaultPixelType>, AbstractFilter>(this, getSelectedCellArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SelectedCellArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SelectedCellArray = m_SelectedCellArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_SaveAsNewArray == false) { m_NewCellArrayName = "thisIsATempName"; }
  tempPath.update(getSelectedCellArrayPath().getDataContainerName(), getSelectedCellArrayPath().getAttributeMatrixName(), getNewCellArrayName() );
  m_NewCellArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<ImageProcessing::DefaultPixelType>, AbstractFilter, ImageProcessing::DefaultPixelType>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NewCellArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NewCellArray = m_NewCellArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SobelEdge::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.

  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
  setErrorCondition(0xABABABAB);
  QString ss = QObject::tr("Filter is NOT updated for IGeometry Redesign. A Programmer needs to check this filter. Please report this to the DREAM3D developers.");
  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SobelEdge::execute()
{
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getSelectedCellArrayPath().getDataContainerName());
  QString attrMatName = getSelectedCellArrayPath().getAttributeMatrixName();

  //wrap m_RawImageData as itk::image
  ImageProcessing::DefaultImageType::Pointer inputImage = ITKUtilitiesType::CreateItkWrapperForDataPointer(m, attrMatName, m_SelectedCellArray);

  if(m_Slice)
  {
    //wrap output array
    ImageProcessing::DefaultImageType::Pointer outputImage = ITKUtilitiesType::CreateItkWrapperForDataPointer(m, attrMatName, m_NewCellArray);

    //get dimensions
    size_t udims[3] = {0, 0, 0};
    /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
    typedef int32_t DimType;
#else
    typedef int64_t DimType;
#endif
    DimType dims[3] =
    {
      static_cast<DimType>(udims[0]),
      static_cast<DimType>(udims[1]),
      static_cast<DimType>(udims[2]),
    };

    //create edge filter
    typedef itk::SobelEdgeDetectionImageFilter<ImageProcessing::DefaultSliceType, ImageProcessing::FloatSliceType> SobelFilterType;
    SobelFilterType::Pointer sobelFilter = SobelFilterType::New();

    //convert result back to uint8
    typedef itk::RescaleIntensityImageFilter<ImageProcessing::FloatSliceType, ImageProcessing::DefaultSliceType> RescaleImageType;
    RescaleImageType::Pointer rescaleFilter = RescaleImageType::New();
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);

    //loop over slices applying filters
    for(int i = 0; i < dims[2]; ++i)
    {
      QString ss = QObject::tr("Finding Edges On Slice: %1").arg(i + 1);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

      //get slice
      ImageProcessing::DefaultSliceType::Pointer inputSlice = ITKUtilitiesType::ExtractSlice(inputImage, ImageProcessing::ZSlice, i);

      //run filters
      sobelFilter->SetInput(inputSlice);
      rescaleFilter->SetInput(sobelFilter->GetOutput());


      //execute filters
      try
      {
        sobelFilter->Update();
        rescaleFilter->Update();
      }
      catch( itk::ExceptionObject& err )
      {
        setErrorCondition(-5);
        QString ss = QObject::tr("Failed to execute itk::SobelEdgeDetectionImageFilter filter. Error Message returned from ITK:\n   %1").arg(err.GetDescription());
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      }

      //copy into volume
      ITKUtilitiesType::SetSlice(outputImage, rescaleFilter->GetOutput(), ImageProcessing::ZSlice, i);
    }
  }
  else
  {
    //create edge filter
    typedef itk::SobelEdgeDetectionImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType> SobelFilterType;
    SobelFilterType::Pointer sobelFilter = SobelFilterType::New();
    sobelFilter->SetInput(inputImage);

    //convert result back to uint8
    typedef itk::RescaleIntensityImageFilter<ImageProcessing::FloatImageType, ImageProcessing::DefaultImageType> RescaleImageType;
    RescaleImageType::Pointer rescaleFilter = RescaleImageType::New();
    rescaleFilter->SetInput(sobelFilter->GetOutput());
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);

    //have filter write to dream3d array instead of creating its own buffer
    ITKUtilitiesType::SetITKFilterOutput(rescaleFilter->GetOutput(), m_NewCellArrayPtr.lock());

    //execute filters
    try
    {
      sobelFilter->Update();
      rescaleFilter->Update();
    }
    catch( itk::ExceptionObject& err )
    {
      setErrorCondition(-5);
      QString ss = QObject::tr("Failed to execute itk::SobelEdgeDetectionImageFilter filter. Error Message returned from ITK:\n   %1").arg(err.GetDescription());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

  }

  //array name changing/cleanup
  if(m_SaveAsNewArray == false)
  {
    AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(m_SelectedCellArrayPath.getAttributeMatrixName());
    attrMat->removeAttributeArray(m_SelectedCellArrayPath.getDataArrayName());
    attrMat->renameAttributeArray(m_NewCellArrayName, m_SelectedCellArrayPath.getDataArrayName());
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SobelEdge::newFilterInstance(bool copyFilterParameters)
{
  SobelEdge::Pointer filter = SobelEdge::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SobelEdge::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SobelEdge::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SobelEdge::getSubGroupName()
{return "Misc";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SobelEdge::getHumanLabel()
{return "Sobel Edge Detector";}

