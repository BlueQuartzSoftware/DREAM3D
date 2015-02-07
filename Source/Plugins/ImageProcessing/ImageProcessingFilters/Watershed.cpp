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
#include "Watershed.h"

#include <QtCore/QString>

#include "DREAM3DLib/Common/Constants.h"

#include "ItkBridge.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "itkWatershedImageFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Watershed::Watershed() :
  m_SelectedCellArrayPath("", "", ""),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_Threshold(0.005f),
  m_Level(0.5f),
  m_SelectedCellArrayArrayName(""),
  m_SelectedCellArray(NULL),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Watershed::~Watershed()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Watershed::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Image Data Array", "SelectedCellArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Feature Ids Array", "FeatureIdsArrayName", FilterParameterWidgetType::StringWidget, getFeatureIdsArrayName(), false, ""));
  parameters.push_back(FilterParameter::New("Threshold", "Threshold", FilterParameterWidgetType::DoubleWidget, getThreshold(), false));
  parameters.push_back(FilterParameter::New("Level", "Level", FilterParameterWidgetType::DoubleWidget, getLevel(), false));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Watershed::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  setFeatureIdsArrayName( reader->readString( "FeatureIdsArrayName", getFeatureIdsArrayName() ) );
  setThreshold( reader->readValue( "Threshold", getThreshold() ) );
  setLevel( reader->readValue( "Level", getLevel() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int Watershed::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue( "SelectedCellArrayPath", getSelectedCellArrayPath() );
  writer->writeValue( "FeatureIdsArrayName", getFeatureIdsArrayName() );
  writer->writeValue( "Threshold", getThreshold() );
  writer->writeValue( "Level", getLevel() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Watershed::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  QVector<size_t> dims(1, 1);
  m_SelectedCellArrayPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<ImageProcessing::DefaultPixelType>, AbstractFilter>(this, getSelectedCellArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SelectedCellArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SelectedCellArray = m_SelectedCellArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getSelectedCellArrayPath().getDataContainerName())->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0 || NULL == image.get()) { return; }

  tempPath.update(getSelectedCellArrayPath().getDataContainerName(), getSelectedCellArrayPath().getAttributeMatrixName(), getFeatureIdsArrayName() );
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Watershed::preflight()
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
void Watershed::execute()
{
  //int err = 0;
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getSelectedCellArrayPath().getDataContainerName());
  QString attrMatName = getSelectedCellArrayPath().getAttributeMatrixName();

  //wrap m_RawImageData as itk::image
  ImageProcessing::DefaultImageType::Pointer inputImage = ITKUtilitiesType::CreateItkWrapperForDataPointer(m, attrMatName, m_SelectedCellArray);

  //create gradient magnitude filter
  notifyStatusMessage(getHumanLabel(), "Calculating Gradient Magnitude");
  typedef itk::GradientMagnitudeImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType >  GradientMagnitudeImageFilterType;
  GradientMagnitudeImageFilterType::Pointer gradientMagnitudeImageFilter = GradientMagnitudeImageFilterType::New();
  gradientMagnitudeImageFilter->SetInput(inputImage);
  gradientMagnitudeImageFilter->Update();

  //watershed image
  notifyStatusMessage(getHumanLabel(), "Watershedding");
  typedef itk::WatershedImageFilter<ImageProcessing::DefaultImageType> WatershedFilterType;
  WatershedFilterType::Pointer watershed = WatershedFilterType::New();
  watershed->SetThreshold(m_Threshold);
  watershed->SetLevel(m_Level);
  watershed->SetInput(gradientMagnitudeImageFilter->GetOutput());

  //execute filter
  try
  {
    watershed->Update();
  }
  catch( itk::ExceptionObject& err )
  {
    setErrorCondition(-5);
    QString ss = QObject::tr("Failed to execute itk::GradientMagnitudeImageFilter filter. Error Message returned from ITK:\n   %1").arg(err.GetDescription());
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  //get output and copy to grainids
  typedef itk::Image<unsigned long, ImageProcessing::ImageDimension>   WatershedImageType;
  WatershedImageType::Pointer output = watershed->GetOutput();
  WatershedImageType::RegionType filterRegion = output->GetLargestPossibleRegion();
  typedef itk::ImageRegionConstIterator<itk::Image<unsigned long, ImageProcessing::ImageDimension> > WatershedIteratorType;
  WatershedIteratorType it(output, filterRegion);
  it.GoToBegin();
  int index = 0;
  while(!it.IsAtEnd())
  {
    m_FeatureIds[index] = it.Get();
    ++it;
    ++index;
  }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer Watershed::newFilterInstance(bool copyFilterParameters)
{
  Watershed::Pointer filter = Watershed::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Watershed::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Watershed::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Watershed::getSubGroupName()
{return "Misc";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Watershed::getHumanLabel()
{return "Watershed Filter";}

