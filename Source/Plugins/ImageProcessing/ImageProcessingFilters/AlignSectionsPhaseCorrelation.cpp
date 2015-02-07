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
#include "AlignSectionsPhaseCorrelation.h"

#include <QtCore/QString>
#include <fstream>

#include "ImageProcessing/ImageProcessingConstants.h"
#include "ItkBridge.h"

#include "itkFFTNormalizedCorrelationImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"

/**
* @brief This is a private implementation for the filter that handles the actual algorithm implementation details
* for us like figuring out if we can use this private implementation with the data array that is assigned.
*/
template<typename PixelType>
class AlignSectionsPhaseCorrelationPrivate
{
  public:
    typedef DataArray<PixelType> DataArrayType;

    AlignSectionsPhaseCorrelationPrivate() {}
    virtual ~AlignSectionsPhaseCorrelationPrivate() {}

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
    void static Execute(AlignSectionsPhaseCorrelation* filter, IDataArray::Pointer inputArray, DataContainer::Pointer m, QString attrMatName, QVector<int>& xshifts, QVector<int>& yshifts)
    {
      //get volume dimensions
      size_t udims[3] = {0, 0, 0};
      m->getGeometryAs<ImageGeom>()->getDimensions(udims);
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
      int voxelsPerSlice = dims[0] * dims[1];

      //convert array to correct type
      typename DataArrayType::Pointer inputArrayPtr = boost::dynamic_pointer_cast<DataArrayType>(inputArray);
      PixelType* inputData = static_cast<PixelType*>(inputArrayPtr->getPointer(0));

      //define types
      typedef ItkBridge<PixelType> ItkBridgeType;
      typedef ItkBridgeType::ScalarImageType ImageType;
      typedef ItkBridgeType::ScalarSliceImageType SliceType;
      typedef itk::Image<float, SliceType::ImageDimension> FloatSliceType;
      typedef itk::FFTNormalizedCorrelationImageFilter<SliceType, FloatSliceType> CorrelationType;
      typedef itk::MinimumMaximumImageCalculator <FloatSliceType> MinMaxType;

      //wrap input  as itk image
      size_t numVoxels = inputArrayPtr->getNumberOfTuples();
      typename ImageType::Pointer inputImage = ItkBridgeType::CreateItkWrapperForDataPointer(m, attrMatName, inputData);

      //open output file if needed
      std::ofstream outFile;
      if (filter->getWriteAlignmentShifts() == true)
      {
        outFile.open(filter->getAlignmentShiftFileName().toLatin1().data());
      }

      //loop over slices computing shifts
      for (DimType iter = 1; iter < dims[2]; iter++)
      {
        //update progress
        QString ss = QObject::tr("Aligning Sections - Determining Shifts - %1 Percent Complete").arg(((float)iter / dims[2]) * 100);
        filter->notifyStatusMessage(filter->getMessagePrefix(), filter->getHumanLabel(), ss);

        //extract slices
        int sliceNum = static_cast<int>( (dims[2] - 1) - iter );
        SliceType::Pointer fixedImage = ItkBridgeType::ExtractSlice(inputImage, ImageProcessing::ZSlice, sliceNum);
        SliceType::Pointer movingImage = ItkBridgeType::ExtractSlice(inputImage, ImageProcessing::ZSlice, sliceNum + 1);

        //fft correlation
        CorrelationType::Pointer correlation = CorrelationType::New();
        correlation->SetFixedImage(fixedImage);
        correlation->SetMovingImage(movingImage);
        correlation->SetRequiredNumberOfOverlappingPixels(0.1 * voxelsPerSlice);

        //find peak
        MinMaxType::Pointer minmax = MinMaxType::New();
        minmax->SetImage(correlation->GetOutput());

        //update pipeline
        try
        {
          minmax->Compute();
        }
        catch( itk::ExceptionObject& err )
        {
          filter->setErrorCondition(-5);
          QString ss = QObject::tr("Failed to convert image. Error Message returned from ITK:\n   %1").arg(err.GetDescription());
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }

        //convert from max location to shift
        FloatSliceType::IndexType maximumLocation = minmax->GetIndexOfMaximum();
        int sliceXShift = maximumLocation[0] - dims[0] + 1;
        int sliceYShift = maximumLocation[1] - dims[1] + 1;

        //save shift
        xshifts[iter] = xshifts[iter - 1] + sliceXShift;
        yshifts[iter] = yshifts[iter - 1] + sliceYShift;
        if (filter->getWriteAlignmentShifts() == true)
        {
          outFile << sliceNum << " " << sliceNum + 1 << " " << sliceXShift << " " << sliceYShift << " " << xshifts[iter] << " " << yshifts[iter] << "\n";
        }
      }
    }
  private:
    AlignSectionsPhaseCorrelationPrivate(const AlignSectionsPhaseCorrelationPrivate&); // Copy Constructor Not Implemented
    void operator=(const AlignSectionsPhaseCorrelationPrivate&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsPhaseCorrelation::AlignSectionsPhaseCorrelation() :
  AlignSections(),
  m_SelectedCellArrayPath("", "", ""),
  m_SelectedCellArray(NULL)
/* DO NOT FORGET TO INITIALIZE ALL YOUR DREAM3D Filter Parameters HERE */
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsPhaseCorrelation::~AlignSectionsPhaseCorrelation()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsPhaseCorrelation::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Input Array", "SelectedCellArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayPath(), false, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsPhaseCorrelation::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AlignSectionsPhaseCorrelation::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedCellArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsPhaseCorrelation::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  //check for required arrays
  QVector<size_t> compDims(1, 1);
  m_SelectedCellArrayPtr = TemplateHelpers::GetPrereqArrayFromPath<AbstractFilter>()(this, getSelectedCellArrayPath(), compDims);
  if(NULL != m_SelectedCellArrayPtr.lock().get())
  {
    m_SelectedCellArray = m_SelectedCellArrayPtr.lock().get();
  }
  if(getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getSelectedCellArrayPath().getDataContainerName())->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0 || NULL == image.get()) { return; }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsPhaseCorrelation::preflight()
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
const QString AlignSectionsPhaseCorrelation::getCompiledLibraryName()
{
  return ImageProcessing::ImageProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsPhaseCorrelation::getGroupName()
{
  return "ImageProcessing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsPhaseCorrelation::getHumanLabel()
{
  return "AlignSectionsPhaseCorrelation";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsPhaseCorrelation::getSubGroupName()
{
  return "Misc";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsPhaseCorrelation::execute()
{
  int err = 0;
  // typically run your dataCheck function to make sure you can get that far and all your variables are initialized
  dataCheck();
  // Check to make sure you made it through the data check. Errors would have been reported already so if something
  // happens to fail in the dataCheck() then we simply return
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  /* Place all your code to execute your filter here. */
  AlignSections::execute();

  /* If some error occurs this code snippet can report the error up the call chain*/
  if (err < 0)
  {
    QString ss = QObject::tr("Some error message");
    setErrorCondition(-99999999);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

void AlignSectionsPhaseCorrelation::find_shifts(QVector<int>& xshifts, QVector<int>& yshifts)
{
  //get volume container
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getSelectedCellArrayPath().getDataContainerName());
  QString attrMatName = getSelectedCellArrayPath().getAttributeMatrixName();

  //get input data
  IDataArray::Pointer inputData = m_SelectedCellArrayPtr.lock();

  //execute type dependant portion using a Private Implementation that takes care of figuring out if
  // we can work on the correct type and actually handling the algorithm execution. We pass in "this" so
  // that the private implementation can get access to the current object to pass up status notifications,
  // progress or handle "cancel" if needed.
  if(AlignSectionsPhaseCorrelationPrivate<int8_t>()(inputData))
  {
    AlignSectionsPhaseCorrelationPrivate<int8_t>::Execute(this, inputData, m, attrMatName, xshifts, yshifts);
  }
  else if(AlignSectionsPhaseCorrelationPrivate<uint8_t>()(inputData) )
  {
    AlignSectionsPhaseCorrelationPrivate<uint8_t>::Execute(this, inputData, m, attrMatName, xshifts, yshifts);
  }
  else if(AlignSectionsPhaseCorrelationPrivate<int16_t>()(inputData) )
  {
    AlignSectionsPhaseCorrelationPrivate<int16_t>::Execute(this, inputData, m, attrMatName, xshifts, yshifts);
  }
  else if(AlignSectionsPhaseCorrelationPrivate<uint16_t>()(inputData) )
  {
    AlignSectionsPhaseCorrelationPrivate<uint16_t>::Execute(this, inputData, m, attrMatName, xshifts, yshifts);
  }
  else if(AlignSectionsPhaseCorrelationPrivate<int32_t>()(inputData) )
  {
    AlignSectionsPhaseCorrelationPrivate<int32_t>::Execute(this, inputData, m, attrMatName, xshifts, yshifts);
  }
  else if(AlignSectionsPhaseCorrelationPrivate<uint32_t>()(inputData) )
  {
    AlignSectionsPhaseCorrelationPrivate<uint32_t>::Execute(this, inputData, m, attrMatName, xshifts, yshifts);
  }
  else if(AlignSectionsPhaseCorrelationPrivate<int64_t>()(inputData) )
  {
    AlignSectionsPhaseCorrelationPrivate<int64_t>::Execute(this, inputData, m, attrMatName, xshifts, yshifts);
  }
  else if(AlignSectionsPhaseCorrelationPrivate<uint64_t>()(inputData) )
  {
    AlignSectionsPhaseCorrelationPrivate<uint64_t>::Execute(this, inputData, m, attrMatName, xshifts, yshifts);
  }
  else if(AlignSectionsPhaseCorrelationPrivate<float>()(inputData) )
  {
    AlignSectionsPhaseCorrelationPrivate<float>::Execute(this, inputData, m, attrMatName, xshifts, yshifts);
  }
  else if(AlignSectionsPhaseCorrelationPrivate<double>()(inputData) )
  {
    AlignSectionsPhaseCorrelationPrivate<double>::Execute(this, inputData, m, attrMatName, xshifts, yshifts);
  }
  else
  {
    setErrorCondition(-10001);
    QString ss = QObject::tr("A Supported DataArray type was not used for an input array.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AlignSectionsPhaseCorrelation::newFilterInstance(bool copyFilterParameters)
{
  /*
  * write code to optionally copy the filter parameters from the current filter into the new instance
  */
  AlignSectionsPhaseCorrelation::Pointer filter = AlignSectionsPhaseCorrelation::New();
  if(true == copyFilterParameters)
  {
    /* If the filter uses all the standard Filter Parameter Widgets you can probabaly get
     * away with using this method to copy the filter parameters from the current instance
     * into the new instance
     */
    copyFilterParameterInstanceVariables(filter.get());
    /* If your filter is using a lot of custom FilterParameterWidgets @see ReadH5Ebsd then you
     * may need to copy each filter parameter explicitly plus any other instance variables that
     * are needed into the new instance. Here is some example code from ReadH5Ebsd
     */
    //    DREAM3D_COPY_INSTANCEVAR(OutputFile)
    //    DREAM3D_COPY_INSTANCEVAR(ZStartIndex)
    //    DREAM3D_COPY_INSTANCEVAR(ZEndIndex)
    //    DREAM3D_COPY_INSTANCEVAR(ZResolution)
  }
  return filter;
}

