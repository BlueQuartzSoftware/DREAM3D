/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FlattenImage.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "Processing/ProcessingConstants.h"

/**
 * @brief The FlattenImageImpl class implements a threaded algorithm that flattens an RGB array into a grayscale array
 */
class FlattenImageImpl
{
  public:
    FlattenImageImpl(uint8_t* data, uint8_t* newdata, float Rfactor, float Gfactor, float Bfactor, size_t comp) :
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
        m_FlatImageData[i] = int32_t((m_ImageData[numComp * i] * convRFactor) + (m_ImageData[numComp * i + 1] * convGFactor) + (m_ImageData[numComp * i + 2] * convBFactor));
      }
    }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      convert(r.begin(), r.end());
    }
#endif
  private:
    uint8_t* m_ImageData;
    uint8_t* m_FlatImageData;
    float  convRFactor;
    float  convGFactor;
    float  convBFactor;
    size_t numComp;

};

// Include the MOC generated file for this class
#include "moc_FlattenImage.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FlattenImage::FlattenImage() :
  AbstractFilter(),
  m_FlattenMethod(SIMPL::FlattenImageMethod::Luminosity),
  m_ImageDataArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::ImageData),
  m_FlatImageDataArrayName(SIMPL::CellData::FlatImageData),
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
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Flattening Method");
    parameter->setPropertyName("FlattenMethod");

    QVector<QString> choices;
    choices.push_back("Average");
    choices.push_back("Luminosity");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt8, SIMPL::Defaults::AnyComponentSize, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    QVector< QVector<size_t> > cDims;
    cDims.push_back(QVector<size_t>(1, 3));
    cDims.push_back(QVector<size_t>(1, 4));
    req.componentDimensions = cDims;
    parameters.push_back(DataArraySelectionFilterParameter::New("Image Data", "ImageDataArrayPath", getImageDataArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Flat Image Data", "FlatImageDataArrayName", getFlatImageDataArrayName(), FilterParameter::CreatedArray));
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
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(FlatImageDataArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(ImageDataArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(FlattenMethod)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FlattenImage::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FlattenImage::dataCheck()
{
  setErrorCondition(0);

  DataArrayPath tempPath;
  int32_t numImageComp = 1;

  IDataArray::Pointer iDataArray = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getImageDataArrayPath());
  if (getErrorCondition() < 0) { return; }
  if (NULL != iDataArray.get())
  {
    numImageComp = iDataArray->getNumberOfComponents();
  }

  QVector<size_t> cDims(1, numImageComp);
  m_ImageDataPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, getImageDataArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ImageDataPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ImageData = m_ImageDataPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  tempPath.update(m_ImageDataArrayPath.getDataContainerName(), m_ImageDataArrayPath.getAttributeMatrixName(), getFlatImageDataArrayName() );
  m_FlatImageDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FlatImageDataPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FlatImageData = m_FlatImageDataPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FlattenImage::preflight()
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
void FlattenImage::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  size_t totalPoints = m_ImageDataPtr.lock()->getNumberOfTuples();

  float Rfactor = 1.0f;
  float Gfactor = 1.0f;
  float Bfactor = 1.0f;
  if (m_FlattenMethod == SIMPL::FlattenImageMethod::Average)
  {
    Rfactor = 1.0f / 3.0f;
    Gfactor = 1.0f / 3.0f;
    Bfactor = 1.0f / 3.0f;
  }
  else if (m_FlattenMethod == SIMPL::FlattenImageMethod::Luminosity)
  {
    Rfactor = 0.21f;
    Gfactor = 0.72f;
    Bfactor = 0.07f;
  }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  int32_t comp = m_ImageDataPtr.lock()->getNumberOfComponents();

  //  qDebug() << "FlattenImage: " << m_ConversionFactor << "\n";
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FlattenImage::getCompiledLibraryName()
{
  return ProcessingConstants::ProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FlattenImage::getBrandingString()
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FlattenImage::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FlattenImage::getGroupName()
{ return SIMPL::FilterGroups::ProcessingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FlattenImage::getSubGroupName()
{ return SIMPL::FilterSubGroups::ImageFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FlattenImage::getHumanLabel()
{ return "Flatten Image"; }

