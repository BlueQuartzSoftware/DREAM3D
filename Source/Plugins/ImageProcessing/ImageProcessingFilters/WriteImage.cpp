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
#include "WriteImage.h"

#include <QtCore/QString>
#include <QtCore/QFileInfo>

#include "itkImageFileWriter.h"
#include "itkRGBPixel.h"
#include "itkRGBAPixel.h"
#include "itkVectorImage.h"


#include "DREAM3DLib/Common/TemplateHelpers.hpp"

#include "ImageProcessing/ImageProcessingFilters/ItkBridge.h"


/**
 * @brief This is a private implementation for the filter that handles the actual algorithm implementation details
 * for us like figuring out if we can use this private implementation with the data array that is assigned.
 */
template<typename TInputType>
class WriteImagePrivate
{
  public:
    typedef DataArray<TInputType> DataArrayType;

    WriteImagePrivate() {}
    virtual ~WriteImagePrivate() {}

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
    void static Execute(WriteImage* filter, DataContainer::Pointer m, QString attrMatName, IDataArray::Pointer inputDataArray, QString outputFile)
    {
      typename DataArrayType::Pointer inputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(inputDataArray);

      // Get a Raw Pointer to the data
      TInputType* inputData = inputDataPtr->getPointer(0);
      //size_t numVoxels = inputDataPtr->getNumberOfTuples();

      //get utilities


      //get the total number of components of the data and switch accordingly
      int numComp = inputDataPtr->getNumberOfComponents();

      itk::ProcessObject::Pointer writerObject;

      if(1 == numComp) //scalar image
      {
        typedef ItkBridge<TInputType> ItkBridgeType;
        //define types and wrap input image
        typedef typename ItkBridgeType::ScalarImageType ImageType;
        typename ImageType::Pointer inputImage = ItkBridgeType::CreateItkWrapperForDataPointer(m, attrMatName, inputData);

        //create writer and execute
        typedef itk::ImageFileWriter<ImageType> WriterType;
        typename WriterType::Pointer writer = WriterType::New();
        writer->SetFileName( outputFile.toLatin1().constData() );
        writer->SetInput( inputImage );
        writerObject = writer;
      }
      else if(3 == numComp)//rgb image
      {
        //define types and wrap input image
        typedef ItkBridge<TInputType>                            ItkBridgeType;

        //define types and wrap input image
        typedef typename ItkBridgeType::RGBImageType ImageType;
        typedef typename itk::ImageFileWriter<ImageType> WriterType;
        //     typename ImageType::Pointer inputImage = ItkBridgeType::template Dream3DtoITKTemplate<ImageType>(m, attrMatName, inputData);
        typename ImageType::Pointer inputImage = ItkBridgeType::template Dream3DtoITKImportFilter<typename ImageType::PixelType>(m, attrMatName, inputData)->GetOutput();

        //create writer and execute
        typename WriterType::Pointer writer = WriterType::New();
        writer->SetFileName( outputFile.toLatin1().constData() );
        writer->SetInput( inputImage );
        writerObject = writer;
      }
      else if(4 == numComp)//rgba image
      {
        typedef ItkBridge<TInputType>                            ItkBridgeType;

        //define types and wrap input image
        typedef typename ItkBridgeType::RGBAImageType ImageType;
        typedef typename itk::ImageFileWriter<ImageType> WriterType;
        //     typename ImageType::Pointer inputImage = ItkBridgeType::template Dream3DtoITKTemplate<ImageType>(m, attrMatName, inputData);
        typename ImageType::Pointer inputImage = ItkBridgeType::template Dream3DtoITKImportFilter<typename ImageType::PixelType>(m, attrMatName, inputData)->GetOutput();

        //create writer and execute
        typename WriterType::Pointer writer = WriterType::New();
        writer->SetFileName( outputFile.toLatin1().constData() );
        writer->SetInput( inputImage );
        writerObject = writer;
      } /** else//vector image
      {
        //define types and wrap input image
        typedef itk::Image<itk::FixedArray<TInputType, numComp> >, ImageProcessing::ImageDimension> ImageType;
        typedef itk::ImageFileWriter<ImageType> WriterType;
        ImageType::Pointer inputImage = ItkBridgeType::Dream3DtoITKTemplate<ImageType>(m, attrMatName, inputData);

        //create writer and execute
        typename WriterType::Pointer writer = WriterType::New();
        writer->SetFileName( outputFile.toLatin1().constData() );
        writer->SetInput( inputImage );
        writerObject = writer;
      }*/

      try
      {
        writerObject->Update();
      }
      catch( itk::ExceptionObject& err )
      {
        filter->setErrorCondition(-5);
        QString ss = QObject::tr("Failed to write image. Error Message returned from ITK:\n   %1").arg(err.GetDescription());
        filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
      }
    }
  private:
    WriteImagePrivate(const WriteImagePrivate&); // Copy Constructor Not Implemented
    void operator=(const WriteImagePrivate&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteImage::WriteImage() :
  AbstractFilter(),
  m_SelectedCellArrayPath("", "", ""),
  m_OutputFileName(""),
  m_SelectedCellArrayArrayName(""),
  m_SelectedCellArray(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteImage::~WriteImage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImage::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Array to Write", "SelectedCellArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayPath(), false, ""));
  parameters.push_back(FileSystemFilterParameter::New("Output File Name", "OutputFileName", FilterParameterWidgetType::OutputFileWidget, getOutputFileName(), false, "", "*.tif", "TIFF"));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImage::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  setOutputFileName( reader->readString( "OutputFileName", getOutputFileName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WriteImage::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue( "SelectedCellArrayPath", getSelectedCellArrayPath() );
  writer->writeValue( "OutputFileName", getOutputFileName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImage::dataCheck()
{
  setErrorCondition(0);


  if(m_OutputFileName.isEmpty())
  {
    QString ss = QObject::tr("Output file name/path was not given");
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QFileInfo fi(m_OutputFileName);
  if(fi.suffix().compare("tif") != 0)
  {
    QString ss = QObject::tr("Image Stacks are only supported for TIFF formatted output");
    setErrorCondition(-101);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  //pass empty dimensions to allow any size
  QVector<size_t> compDims;
  m_SelectedCellArrayPtr = TemplateHelpers::GetPrereqArrayFromPath<AbstractFilter>()(this, getSelectedCellArrayPath(), compDims);
  if(NULL != m_SelectedCellArrayPtr.lock().get())
  {
    m_SelectedCellArray = m_SelectedCellArrayPtr.lock().get();
  }

  //make sure dims of selected array are appropriate
  if(1 == compDims.size())
  {
    if(1 == compDims[0]) //scalar
    {

    }
    else if (3 == compDims[0])//rgb
    {
      notifyWarningMessage(getHumanLabel(), "Warning: writing of rgb images is currenlty experimental (unstable behavoir may occur)", 0);
    }
    else if (4 == compDims[0])//rgba
    {
      notifyWarningMessage(getHumanLabel(), "Warning: writing of rgba images is currenlty experimental (unstable behavoir may occur)", 0);
    }
    else  //vector
    {
      //notifyWarningMessage(getHumanLabel(), "Warning: writing of vector images is currenlty experimental (unstable behavoir may occur)", 0);
      setErrorCondition(-102);
      notifyErrorMessage(getHumanLabel(), "Error: writing of vector images is currently not supported", getErrorCondition());
    }
  }
  else
  {
    QString message = QObject::tr("The selected array '%1' has unsupported dimensionality (%2)").arg(m_SelectedCellArrayPath.getDataArrayName()).arg(compDims.size());
    setErrorCondition(-101);
    notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImage::preflight()
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
void WriteImage::execute()
{
  //int err = 0;
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getSelectedCellArrayPath().getDataContainerName());
  QString attrMatName = getSelectedCellArrayPath().getAttributeMatrixName();

  //get input data
  IDataArray::Pointer inputData = m_SelectedCellArrayPtr.lock();

  if(WriteImagePrivate<int8_t>()(inputData))
  {
    WriteImagePrivate<int8_t>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<uint8_t>()(inputData) )
  {
    WriteImagePrivate<uint8_t>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<int16_t>()(inputData) )
  {
    WriteImagePrivate<int16_t>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<uint16_t>()(inputData) )
  {
    WriteImagePrivate<uint16_t>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<int32_t>()(inputData) )
  {
    WriteImagePrivate<int32_t>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<uint32_t>()(inputData) )
  {
    WriteImagePrivate<uint32_t>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<int64_t>()(inputData) )
  {
    WriteImagePrivate<int64_t>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<uint64_t>()(inputData) )
  {
    WriteImagePrivate<uint64_t>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<float>()(inputData) )
  {
    WriteImagePrivate<float>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<double>()(inputData) )
  {
    WriteImagePrivate<double>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else
  {
    setErrorCondition(-10001);
    QString ss = QObject::tr("A Supported DataArray type was not used for an input array.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }



  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer WriteImage::newFilterInstance(bool copyFilterParameters)
{
  WriteImage::Pointer filter = WriteImage::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteImage::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteImage::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteImage::getSubGroupName()
{return "IO";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteImage::getHumanLabel()
{return "Write Tiff Image Stack";}

