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

#include "AutoThreshold.h"

//histogram calculation
#include "itkImageToHistogramFilter.h"

//histogram based selectors
#include "itkHistogramThresholdCalculator.h"
#include "itkHuangThresholdCalculator.h"
#include "itkIntermodesThresholdCalculator.h"
#include "itkIsoDataThresholdCalculator.h"
#include "itkKittlerIllingworthThresholdCalculator.h"
#include "itkLiThresholdCalculator.h"
#include "itkMaximumEntropyThresholdCalculator.h"
#include "itkMomentsThresholdCalculator.h"
#include "itkOtsuThresholdCalculator.h"
#include "itkRenyiEntropyThresholdCalculator.h"
#include "itkShanbhagThresholdCalculator.h"
#include "itkTriangleThresholdCalculator.h"
#include "itkYenThresholdCalculator.h"

//thresholding filter
#include "itkBinaryThresholdImageFilter.h"

#include "DREAM3DLib/Common/Constants.h"

#include "ItkBridge.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AutoThreshold::AutoThreshold() :
  AbstractFilter(),
  m_SelectedCellArrayPath("", "", ""),
  m_NewCellArrayName(""),
  m_SaveAsNewArray(true),
  m_Slice(false),
  m_Method(7),
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
AutoThreshold::~AutoThreshold()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AutoThreshold::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Array to Process", "SelectedCellArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayPath(), false, ""));
  QStringList linkedProps;
  linkedProps << "NewCellArrayName";
  parameters.push_back(FilterParameter::NewConditional("Save As New Array", "SaveAsNewArray", FilterParameterWidgetType::LinkedBooleanWidget, getSaveAsNewArray(), false, linkedProps));
  parameters.push_back(FilterParameter::New("Created Array Name", "NewCellArrayName", FilterParameterWidgetType::StringWidget, getNewCellArrayName(), false, ""));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Threshold Method");
    parameter->setPropertyName("Method");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    QVector<QString> choices;
    choices.push_back("Huang");
    choices.push_back("Intermodes");
    choices.push_back("IsoData");
    choices.push_back("Kittler Illingworth");
    choices.push_back("Li");
    choices.push_back("Maximum Entropy");
    choices.push_back("Moments");
    choices.push_back("Otsu");
    choices.push_back("Renyi Entropy");
    choices.push_back("Shanbhag");
    choices.push_back("Triangle");
    choices.push_back("Yen");
    parameter->setChoices(choices);
    parameter->setAdvanced(false);
    parameters.push_back(parameter);
  }
  parameters.push_back(FilterParameter::New("Slice at a Time", "Slice", FilterParameterWidgetType::BooleanWidget, getSlice(), false));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AutoThreshold::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  setNewCellArrayName( reader->readString( "NewCellArrayName", getNewCellArrayName() ) );
  setSaveAsNewArray( reader->readValue( "SaveAsNewArray", getSaveAsNewArray() ) );
  setSlice( reader->readValue( "Slice", getSlice() ) );
  setMethod( reader->readValue( "Method", getMethod() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AutoThreshold::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedCellArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NewCellArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SaveAsNewArray)
  DREAM3D_FILTER_WRITE_PARAMETER(Slice)
  DREAM3D_FILTER_WRITE_PARAMETER(Method)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AutoThreshold::dataCheck()
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
void AutoThreshold::preflight()
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
void AutoThreshold::execute()
{
  //int err = 0;
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getSelectedCellArrayPath().getDataContainerName());
  QString attrMatName = getSelectedCellArrayPath().getAttributeMatrixName();

  //get dims
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

  //wrap input as itk image
  ImageProcessing::DefaultImageType::Pointer inputImage = ITKUtilitiesType::CreateItkWrapperForDataPointer(m, attrMatName, m_SelectedCellArray);

  //define threshold filters
  typedef itk::BinaryThresholdImageFilter <ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType> BinaryThresholdImageFilterType;
  typedef itk::BinaryThresholdImageFilter <ImageProcessing::DefaultSliceType, ImageProcessing::DefaultSliceType> BinaryThresholdImageFilterType2D;

  //define hitogram generator (will make the same kind of histogram for 2 and 3d images
  typedef itk::Statistics::ImageToHistogramFilter<ImageProcessing::DefaultImageType> HistogramGenerator;

  //find threshold value w/ histogram
  itk::HistogramThresholdCalculator< HistogramGenerator::HistogramType, uint8_t >::Pointer calculator;

  typedef itk::HuangThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > HuangCalculatorType;
  typedef itk::IntermodesThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > IntermodesCalculatorType;
  typedef itk::IsoDataThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > IsoDataCalculatorType;
  typedef itk::KittlerIllingworthThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > KittlerIllingowrthCalculatorType;
  typedef itk::LiThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > LiCalculatorType;
  typedef itk::MaximumEntropyThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > MaximumEntropyCalculatorType;
  typedef itk::MomentsThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > MomentsCalculatorType;
  typedef itk::OtsuThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > OtsuCalculatorType;
  typedef itk::RenyiEntropyThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > RenyiEntropyCalculatorType;
  typedef itk::ShanbhagThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > ShanbhagCalculatorType;
  typedef itk::TriangleThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > TriangleCalculatorType;
  typedef itk::YenThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > YenCalculatorType;

  switch(m_Method)
  {
    case 0:
    {
      calculator = HuangCalculatorType::New();
    }
    break;

    case 1:
    {
      calculator = IntermodesCalculatorType::New();
    }
    break;

    case 2:
    {
      calculator = IsoDataCalculatorType::New();
    }
    break;

    case 3:
    {
      calculator = KittlerIllingowrthCalculatorType::New();
    }
    break;

    case 4:
    {
      calculator = LiCalculatorType::New();
    }
    break;

    case 5:
    {
      calculator = MaximumEntropyCalculatorType::New();
    }
    break;

    case 6:
    {
      calculator = MomentsCalculatorType::New();
    }
    break;

    case 7:
    {
      calculator = OtsuCalculatorType::New();
    }
    break;

    case 8:
    {
      calculator = RenyiEntropyCalculatorType::New();
    }
    break;

    case 9:
    {
      calculator = ShanbhagCalculatorType::New();
    }
    break;

    case 10:
    {
      calculator = TriangleCalculatorType::New();
    }
    break;

    case 11:
    {
      calculator = YenCalculatorType::New();
    }
    break;
  }

  if(m_Slice)
  {
    //define 2d histogram generator
    typedef itk::Statistics::ImageToHistogramFilter<ImageProcessing::DefaultSliceType> HistogramGenerator2D;
    HistogramGenerator2D::Pointer histogramFilter2D = HistogramGenerator2D::New();

    //specify number of bins / bounds
    typedef HistogramGenerator2D::HistogramSizeType SizeType;
    SizeType size( 1 );
    size[0] = 255;
    histogramFilter2D->SetHistogramSize( size );
    histogramFilter2D->SetMarginalScale( 10.0 );
    HistogramGenerator2D::HistogramMeasurementVectorType lowerBound( 1 );
    HistogramGenerator2D::HistogramMeasurementVectorType upperBound( 1 );
    lowerBound[0] = 0;
    upperBound[0] = 256;
    histogramFilter2D->SetHistogramBinMinimum( lowerBound );
    histogramFilter2D->SetHistogramBinMaximum( upperBound );

    //wrap output buffer as image
    ImageProcessing::DefaultImageType::Pointer outputImage = ITKUtilitiesType::CreateItkWrapperForDataPointer(m, attrMatName, m_NewCellArray);

    //loop over slices
    for(int i = 0; i < dims[2]; i++)
    {
      //get slice
      ImageProcessing::DefaultSliceType::Pointer slice = ITKUtilitiesType::ExtractSlice(inputImage, ImageProcessing::ZSlice, i);

      //find histogram
      histogramFilter2D->SetInput( slice );
      histogramFilter2D->Update();
      const HistogramGenerator::HistogramType* histogram = histogramFilter2D->GetOutput();

      //calculate threshold level
      calculator->SetInput(histogram);
      calculator->Update();
      const uint8_t thresholdValue = calculator->GetThreshold();

      //threshold
      BinaryThresholdImageFilterType2D::Pointer thresholdFilter = BinaryThresholdImageFilterType2D::New();
      thresholdFilter->SetInput(slice);
      thresholdFilter->SetLowerThreshold(thresholdValue);
      thresholdFilter->SetUpperThreshold(255);
      thresholdFilter->SetInsideValue(255);
      thresholdFilter->SetOutsideValue(0);
      thresholdFilter->Update();

      //copy back into volume
      ITKUtilitiesType::SetSlice(outputImage, thresholdFilter->GetOutput(), ImageProcessing::ZSlice, i);
    }
  }
  else
  {
    //specify number of bins / bounds
    HistogramGenerator::Pointer histogramFilter = HistogramGenerator::New();
    typedef HistogramGenerator::HistogramSizeType SizeType;
    SizeType size( 1 );
    size[0] = 255;
    histogramFilter->SetHistogramSize( size );
    histogramFilter->SetMarginalScale( 10.0 );
    HistogramGenerator::HistogramMeasurementVectorType lowerBound( 1 );
    HistogramGenerator::HistogramMeasurementVectorType upperBound( 1 );
    lowerBound[0] = 0;
    upperBound[0] = 256;
    histogramFilter->SetHistogramBinMinimum( lowerBound );
    histogramFilter->SetHistogramBinMaximum( upperBound );

    //find histogram
    histogramFilter->SetInput( inputImage );
    histogramFilter->Update();
    const HistogramGenerator::HistogramType* histogram = histogramFilter->GetOutput();

    //calculate threshold level
    calculator->SetInput(histogram);
    calculator->Update();
    const uint8_t thresholdValue = calculator->GetThreshold();

    //threshold
    BinaryThresholdImageFilterType::Pointer thresholdFilter = BinaryThresholdImageFilterType::New();
    thresholdFilter->SetInput(inputImage);
    thresholdFilter->SetLowerThreshold(thresholdValue);
    thresholdFilter->SetUpperThreshold(255);
    thresholdFilter->SetInsideValue(255);
    thresholdFilter->SetOutsideValue(0);
    thresholdFilter->GetOutput()->GetPixelContainer()->SetImportPointer(m_NewCellArray, m_NewCellArrayPtr.lock()->getNumberOfTuples(), false);
    thresholdFilter->Update();
  }

  //array name changing/cleanup
  if(m_SaveAsNewArray == false)
  {
    AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(m_SelectedCellArrayPath.getAttributeMatrixName());
    attrMat->removeAttributeArray(m_SelectedCellArrayPath.getDataArrayName());
    bool check = attrMat->renameAttributeArray(m_NewCellArrayName, m_SelectedCellArrayPath.getDataArrayName());
    if(check == false)
    {

    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AutoThreshold::newFilterInstance(bool copyFilterParameters)
{
  AutoThreshold::Pointer filter = AutoThreshold::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AutoThreshold::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AutoThreshold::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AutoThreshold::getSubGroupName()
{return "Misc";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AutoThreshold::getHumanLabel()
{return "Threshold Image (Auto)";}

