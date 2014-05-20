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
#include "FlattenImage.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif


#include "DREAM3DLib/Math/DREAM3DMath.h"

class FlattenImageImpl
{

  public:
    FlattenImageImpl(unsigned char* data, unsigned char* newdata, float Rfactor, float Gfactor, float Bfactor, size_t comp) :
      m_ImageData(data),
      m_FlatImageData(newdata),
      convRFactor(Rfactor),
      convGFactor(Gfactor),
      convBFactor(Bfactor),
      numComp(comp)
    {}
    virtual ~FlattenImageImpl() {}

    void convert(size_t start, size_t end) const
    {
      for (size_t i = start; i < end; i++)
      {
        m_FlatImageData[i] = int((m_ImageData[numComp * i] * convRFactor) + (m_ImageData[numComp * i + 1] * convGFactor) + (m_ImageData[numComp * i + 2] * convBFactor));
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      convert(r.begin(), r.end());
    }
#endif
  private:
    unsigned char* m_ImageData;
    unsigned char* m_FlatImageData;
    float  convRFactor;
    float  convGFactor;
    float  convBFactor;
    size_t numComp;

};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FlattenImage::FlattenImage() :
  AbstractFilter(),
  m_FlattenMethod(DREAM3D::FlattenImageMethod::Luminosity),
  m_ImageDataArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::ImageData),
  m_FlatImageDataArrayName(DREAM3D::CellData::FlatImageData),
  m_ImageData(NULL),
  m_FlatImageData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FlattenImage::~FlattenImage()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FlattenImage::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("ImageData", "ImageDataArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", false, ""));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Flattening Method");
    parameter->setPropertyName("FlattenMethod");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    parameter->setValueType("quint32");
    QVector<QString> choices;
    choices.push_back("Lightness");
    choices.push_back("Average");
    choices.push_back("Luminosity");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("FlatImageData", "FlatImageDataArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FlattenImage::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFlatImageDataArrayName(reader->readString("FlatImageDataArrayName", getFlatImageDataArrayName() ) );
  setImageDataArrayPath(reader->readDataArrayPath("ImageDataArrayPath", getImageDataArrayPath() ) );
  setFlattenMethod( reader->readValue("FlattenMethod", getFlattenMethod()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FlattenImage::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("FlatImageDataArrayName", getFlatImageDataArrayName() );
  writer->writeValue("ImageDataArrayPath", getImageDataArrayPath() );
  writer->writeValue("FlattenMethod", getFlattenMethod() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FlattenImage::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  int numImageComp = 1;

  VolumeDataContainer* m = m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, m_ImageDataArrayPath.getDataContainerName());
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer attrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, m_ImageDataArrayPath.getAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  IDataArray::Pointer iDataArray = attrMat->getAttributeArray(m_ImageDataArrayPath.getDataArrayName());
  if(getErrorCondition() < 0) { return; }
  if(NULL != iDataArray.get())
  {
    UInt8ArrayType* imageDataPtr = UInt8ArrayType::SafePointerDownCast(iDataArray.get());
    if (NULL != imageDataPtr)
    {
      numImageComp = imageDataPtr->getNumberOfComponents();
    }
  }

  QVector<size_t> dims(1, numImageComp);
  m_ImageDataPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned char>, AbstractFilter>(this, getImageDataArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ImageDataPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ImageData = m_ImageDataPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  tempPath.update(m_ImageDataArrayPath.getDataContainerName(), m_ImageDataArrayPath.getAttributeMatrixName(), getFlatImageDataArrayName() );
  m_FlatImageDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<unsigned char>, AbstractFilter, unsigned char>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FlatImageDataPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FlatImageData = m_FlatImageDataPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FlattenImage::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FlattenImage::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  int64_t totalPoints = m_ImageDataPtr.lock()->getNumberOfTuples();

  float Rfactor = 1.0;
  float Gfactor = 1.0;
  float Bfactor = 1.0;
  if (m_FlattenMethod == DREAM3D::FlattenImageMethod::Average)
  {
    Rfactor = 1.0 / 3.0;
    Gfactor = 1.0 / 3.0;
    Bfactor = 1.0 / 3.0;
  }
  else if (m_FlattenMethod == DREAM3D::FlattenImageMethod::Luminosity)
  {
    Rfactor = 0.21;
    Gfactor = 0.72;
    Bfactor = 0.07;
  }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  size_t comp = m_ImageDataPtr.lock()->getNumberOfComponents();

  //  qDebug() << "FlattenImage: " << m_ConversionFactor << "\n";
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                      FlattenImageImpl(m_ImageData, m_FlatImageData, Rfactor, Gfactor, Bfactor, comp), tbb::auto_partitioner());

  }
  else
#endif
  {
    FlattenImageImpl serial(m_ImageData, m_FlatImageData, Rfactor, Gfactor, Bfactor, comp);
    serial.convert(0, totalPoints);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FlattenImage::newFilterInstance(bool copyFilterParameters)
{
  FlattenImage::Pointer filter = FlattenImage::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}
