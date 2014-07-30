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
#if (_MSC_VER)
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "HoughCircles.h"

#include "itkHoughTransform2DCirclesImageFilter.h"

#include <QtCore/QString>

#include "ItkBridge.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HoughCircles::HoughCircles() :
  AbstractFilter(),
  m_SelectedCellArrayPath("", "", ""),
  m_NewCellArrayName(""),
  m_SaveAsNewArray(true),
  m_MinRadius(0),
  m_MaxRadius(0),
  m_NumberCircles(0),
  m_SelectedCellArrayArrayName(""),
  m_SelectedCellArray(NULL),
  m_NewCellArray(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HoughCircles::~HoughCircles()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HoughCircles::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Array to Process", "SelectedCellArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayPath(), false, ""));
  QStringList linkedProps;
  linkedProps << "NewCellArrayName";
  parameters.push_back(FilterParameter::NewConditional("Save As New Array", "SaveAsNewArray", FilterParameterWidgetType::LinkedBooleanWidget, getSaveAsNewArray(), false, linkedProps));

  parameters.push_back(FilterParameter::New("Created Array Name", "NewCellArrayName", FilterParameterWidgetType::StringWidget, getNewCellArrayName(), false, ""));
  parameters.push_back(FilterParameter::New("Minimum Radius", "MinRadius", FilterParameterWidgetType::DoubleWidget, getMinRadius(), false));
  parameters.push_back(FilterParameter::New("Maximum Radius", "MaxRadius", FilterParameterWidgetType::DoubleWidget, getMinRadius(), false));
  parameters.push_back(FilterParameter::New("Number of Circles", "NumberCircles", FilterParameterWidgetType::IntWidget, getNumberCircles(), false));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HoughCircles::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  setNewCellArrayName( reader->readString( "NewCellArrayName", getNewCellArrayName() ) );
  setSaveAsNewArray( reader->readValue( "SaveAsNewArray", getSaveAsNewArray() ) );
  setMinRadius( reader->readValue( "MinRadius", getMinRadius() ) );
  setMaxRadius( reader->readValue( "MaxRadius", getMaxRadius() ) );
  setNumberCircles( reader->readValue( "NumberCircles", getNumberCircles() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HoughCircles::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedCellArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NewCellArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SaveAsNewArray)
  DREAM3D_FILTER_WRITE_PARAMETER(MinRadius)
  DREAM3D_FILTER_WRITE_PARAMETER(MaxRadius)
  DREAM3D_FILTER_WRITE_PARAMETER(NumberCircles)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HoughCircles::dataCheck()
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
void HoughCircles::preflight()
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
void HoughCircles::execute()
{
  //int err = 0;
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getSelectedCellArrayPath().getDataContainerName());
  QString attrMatName = getSelectedCellArrayPath().getAttributeMatrixName();

  /* Place all your code to execute your filter here. */
  //get dimensions
  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
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

  size_t totalPoints = m_SelectedCellArrayPtr.lock()->getNumberOfTuples();
  for(int i = 0; i < totalPoints; ++i)
  {
    m_NewCellArray[i] = m_SelectedCellArray[i];
  }

  //wrap raw and processed image data as itk::images
  ImageProcessing::DefaultImageType::Pointer inputImage = ITKUtilitiesType::CreateItkWrapperForDataPointer(m, attrMatName, m_SelectedCellArray);
  ImageProcessing::DefaultImageType::Pointer outputImage = ITKUtilitiesType::CreateItkWrapperForDataPointer(m, attrMatName, m_NewCellArray);

  ImageProcessing::DefaultSliceType::IndexType localIndex;
  typedef itk::HoughTransform2DCirclesImageFilter<ImageProcessing::DefaultPixelType, ImageProcessing::FloatPixelType> HoughTransformFilterType;
  HoughTransformFilterType::Pointer houghFilter = HoughTransformFilterType::New();
  houghFilter->SetNumberOfCircles( m_NumberCircles );
  houghFilter->SetMinimumRadius( m_MinRadius );
  houghFilter->SetMaximumRadius( m_MaxRadius );
  /*optional parameters, these are the default values
  houghFilter->SetSweepAngle( 0 );
  houghFilter->SetSigmaGradient( 1 );
  houghFilter->SetVariance( 5 );
  houghFilter->SetDiscRadiusRatio( 10 );
  */

  //loop over slices
  for(int i = 0; i < dims[2]; ++i)
  {
    //extract slice and transform
    QString ss = QObject::tr("Hough Transforming Slice: %1").arg(i + 1);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    ImageProcessing::DefaultSliceType::Pointer inputSlice = ITKUtilitiesType::ExtractSlice(inputImage, ImageProcessing::ZSlice, i);
    houghFilter->SetInput( inputSlice );
    houghFilter->Update();
    ImageProcessing::FloatSliceType::Pointer localAccumulator = houghFilter->GetOutput();

    //find circles
    ss = QObject::tr("Finding Circles on Slice: %1").arg(i + 1);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    HoughTransformFilterType::CirclesListType circles = houghFilter->GetCircles( m_NumberCircles );

    //create blank slice of same dimensions
    ImageProcessing::DefaultSliceType::Pointer outputSlice = ImageProcessing::DefaultSliceType::New();
    ImageProcessing::DefaultSliceType::RegionType region;
    region.SetSize(inputSlice->GetLargestPossibleRegion().GetSize());
    region.SetIndex(inputSlice->GetLargestPossibleRegion().GetIndex());
    outputSlice->SetRegions( region );
    outputSlice->SetOrigin(inputSlice->GetOrigin());
    outputSlice->SetSpacing(inputSlice->GetSpacing());
    outputSlice->Allocate();
    outputSlice->FillBuffer(0);

    //loop over circles drawing on slice
    HoughTransformFilterType::CirclesListType::const_iterator itCircles = circles.begin();
    while( itCircles != circles.end() )
    {
      //std::cout << "Center: ";
      //std::cout << (*itCircles)->GetObjectToParentTransform()->GetOffset() << std::endl;
      //std::cout << "Radius: " << (*itCircles)->GetRadius()[0] << std::endl;

      for(double angle = 0; angle <= 2 * vnl_math::pi; angle += vnl_math::pi / 60.0 )
      {
        localIndex[0] = (long int)((*itCircles)->GetObjectToParentTransform()->GetOffset()[0]
                                   + (*itCircles)->GetRadius()[0] * vcl_cos(angle));
        localIndex[1] = (long int)((*itCircles)->GetObjectToParentTransform()->GetOffset()[1]
                                   + (*itCircles)->GetRadius()[0] * vcl_sin(angle));
        ImageProcessing::DefaultSliceType::RegionType outputRegion = outputSlice->GetLargestPossibleRegion();
        if( outputRegion.IsInside( localIndex ) )
        {
          outputSlice->SetPixel( localIndex, 255 );
        }
      }
      itCircles++;
    }

    //copy slice into output
    ITKUtilitiesType::SetSlice(outputImage, outputSlice, ImageProcessing::ZSlice, i);
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
AbstractFilter::Pointer HoughCircles::newFilterInstance(bool copyFilterParameters)
{
  HoughCircles::Pointer filter = HoughCircles::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString HoughCircles::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString HoughCircles::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString HoughCircles::getSubGroupName()
{return "Misc";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString HoughCircles::getHumanLabel()
{return "Hough Circle Detection";}

