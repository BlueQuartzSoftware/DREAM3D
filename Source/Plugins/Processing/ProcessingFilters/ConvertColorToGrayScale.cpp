/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without
* modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of
* its
* contributors may be used to endorse or promote products derived from this
* software
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

#include "ConvertColorToGrayScale.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "Processing/ProcessingConstants.h"
#include "Processing/ProcessingVersion.h"

/**
 * @brief The ConvertColorToGrayScaleImpl class implements a threaded algorithm
 * that flattens an RGB array into a grayscale array
 */
class LuminosityImpl
{
public:
  LuminosityImpl(uint8_t* data, uint8_t* newdata, FloatVec3_t colorWeights, size_t comp)
  : m_ImageData(data)
  , m_FlatImageData(newdata)
  , m_ColorWeights(colorWeights)
  , m_NumComp(comp)
  {
  }
  virtual ~LuminosityImpl()
  {
  }

  void convert(size_t start, size_t end) const
  {
    for(size_t i = start; i < end; i++)
    {
      m_FlatImageData[i] =
          static_cast<uint8_t>((m_ImageData[m_NumComp * i] * m_ColorWeights.x) + (m_ImageData[m_NumComp * i + 1] * m_ColorWeights.y) + (m_ImageData[m_NumComp * i + 2] * m_ColorWeights.z));
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
  FloatVec3_t m_ColorWeights;
  size_t m_NumComp;
};

/**
 * @brief The ConvertColorToGrayScaleImpl class implements a threaded algorithm
 * that flattens an RGB array into a grayscale array
 */
class AverageImpl
{
public:
  AverageImpl(uint8_t* data, uint8_t* newdata, size_t comp)
  : m_ImageData(data)
  , m_FlatImageData(newdata)
  , m_NumComp(comp)
  {
  }
  virtual ~AverageImpl()
  {
  }

  void convert(size_t start, size_t end) const
  {
    for(size_t i = start; i < end; i++)
    {
      uint16_t sum = 0;
      for(size_t j = 0; j < 3; j++)
      {
        sum += static_cast<uint16_t>(m_ImageData[m_NumComp * i + j]);
      }

      sum = sum / 3;
      m_FlatImageData[i] = static_cast<uint8_t>(sum);
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
  size_t m_NumComp;
};

/**
 * @brief The ConvertColorToGrayScaleImpl class implements a threaded algorithm
 * that flattens an RGB array into a grayscale array
 */
class LightnessImpl
{
public:
  LightnessImpl(uint8_t* data, uint8_t* newdata, size_t comp)
  : m_ImageData(data)
  , m_FlatImageData(newdata)
  , m_NumComp(comp)
  {
  }
  virtual ~LightnessImpl()
  {
  }

  void convert(size_t start, size_t end) const
  {
    for(size_t i = start; i < end; i++)
    {
      uint8_t maxRGB = 0;
      uint8_t minRGB = 255;

      for(size_t j = 0; j < 3; j++)
      {
        if(m_ImageData[m_NumComp * i + j] > maxRGB)
        {
          maxRGB = m_ImageData[m_NumComp * i + j];
        }
        if(m_ImageData[m_NumComp * i + j] < minRGB)
        {
          minRGB = m_ImageData[m_NumComp * i + j];
        }
      }
      m_FlatImageData[i] = static_cast<uint8_t>((maxRGB + minRGB) / 2.0);
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
  size_t m_NumComp;
};

/**
 * @brief The ConvertColorToGrayScaleImpl class implements a threaded algorithm
 * that flattens an RGB array into a grayscale array
 */
class SingleChannelImpl
{
public:
  SingleChannelImpl(uint8_t* data, uint8_t* newdata, size_t comp, int32_t channel)
  : m_ImageData(data)
  , m_FlatImageData(newdata)
  , numComp(comp)
  , m_Channel(channel)
  {
  }
  virtual ~SingleChannelImpl()
  {
  }

  void convert(size_t start, size_t end) const
  {
    for(size_t i = start; i < end; i++)
    {
      m_FlatImageData[i] = static_cast<uint8_t>((m_ImageData[numComp * i + static_cast<size_t>(m_Channel)]));
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
  size_t numComp;
  int32_t m_Channel;
};

// Include the MOC generated file for this class
#include "moc_ConvertColorToGrayScale.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertColorToGrayScale::ConvertColorToGrayScale()
: AbstractFilter()
, m_ConversionAlgorithm(0)
, m_ColorChannel(0)
, m_CreateNewAttributeMatrix(false)
, m_OutputAttributeMatrixName("GrayScaleData")
, m_OutputArrayPrefix("")

{
  m_ColorWeights.x = 0.2125f;
  m_ColorWeights.y = 0.7154f;
  m_ColorWeights.z = 0.0721f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertColorToGrayScale::~ConvertColorToGrayScale()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertColorToGrayScale::setupFilterParameters()
{

  FilterParameterVector parameters;
  {
    QVector<QString> choices;
    choices.push_back("Luminosity");
    choices.push_back("Average");
    choices.push_back("Lightness");
    choices.push_back("SingleChannel");

    QStringList linkedProps;
    linkedProps << "ColorWeights"
                << "ColorChannel";

    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Conversion Algorithm");
    parameter->setPropertyName("ConversionAlgorithm");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(ConvertColorToGrayScale, this, ConversionAlgorithm));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(ConvertColorToGrayScale, this, ConversionAlgorithm));
    parameter->setChoices(choices);
    parameter->setLinkedProperties(linkedProps);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Color Weighting", ColorWeights, FilterParameter::Parameter, ConvertColorToGrayScale, 0));

  parameters.push_back(SIMPL_NEW_INTEGER_FP("Color Channel", ColorChannel, FilterParameter::Parameter, ConvertColorToGrayScale, 3));

  MultiDataArraySelectionFilterParameter::RequirementType req;
  req.dcGeometryTypes = IGeometry::Types(1, IGeometry::Type::Image);
  req.amTypes = AttributeMatrix::Types(1, AttributeMatrix::Type::Cell);
  parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Input Attribute Arrays", InputDataArrayVector, FilterParameter::RequiredArray, ConvertColorToGrayScale, req));

  QStringList linkedProps("OutputAttributeMatrixName");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Create Attribute Matrix", CreateNewAttributeMatrix, FilterParameter::Parameter, ConvertColorToGrayScale, linkedProps));
  parameters.push_back(SIMPL_NEW_STRING_FP("Output Cell Attribute Matrix", OutputAttributeMatrixName, FilterParameter::CreatedArray, ConvertColorToGrayScale));

  parameters.push_back(SIMPL_NEW_STRING_FP("Output Array Prefix", OutputArrayPrefix, FilterParameter::CreatedArray, ConvertColorToGrayScale));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertColorToGrayScale::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertColorToGrayScale::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  if(DataArrayPath::ValidateVector(getInputDataArrayVector()) == false)
  {
    setErrorCondition(-62100);
    QString ss = QObject::tr("All Attribute Arrays must belong to the same Data Container and Attribute Matrix");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getOutputArrayPrefix().isEmpty())
  {
    setErrorCondition(-62102);
    QString message = QObject::tr("Using a prefix (even a single alphanumeric value) is required so that the output Xdmf files can be written correctly");
    notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
  }

  if(getInputDataArrayVector().isEmpty())
  {
    setErrorCondition(-62103);
    QString message = QObject::tr("At least one Attribute Array must be selected");
    notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
    return;
  }

  DataArrayPath inputAMPath = DataArrayPath::GetAttributeMatrixPath(getInputDataArrayVector());

  AttributeMatrix::Pointer inAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, inputAMPath, -301);
  if(getErrorCondition() < 0 || nullptr == inAM.get())
  {
    return;
  }

  DataContainerArray::Pointer dca = getDataContainerArray();
  DataContainer::Pointer dc = dca->getDataContainer(inputAMPath.getDataContainerName());

  // Now create our output attributeMatrix which will contain all of our segmented images
  AttributeMatrix::Pointer outAM;
  if(m_CreateNewAttributeMatrix)
  {
    QVector<size_t> tDims = inAM->getTupleDimensions();

    outAM = dc->createNonPrereqAttributeMatrix<AbstractFilter>(this, getOutputAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
    if(getErrorCondition() < 0 || nullptr == outAM.get())
    {
      return;
    }
  }

  // Get the list of checked array names from the input m_Data arrays list
  QList<QString> arrayNames = DataArrayPath::GetDataArrayNames(getInputDataArrayVector());
  m_OutputArrayPaths.clear();
  m_OutputArrayPaths.resize(arrayNames.size());
  for(int32_t i = 0; i < arrayNames.size(); i++)
  {
    QString daName = arrayNames.at(i);
    QString newName = getOutputArrayPrefix() + arrayNames.at(i);
    inputAMPath.setDataArrayName(daName);

    // getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, inputAMPath, cDims);
    IDataArray::Pointer iDatArray = dca->getPrereqIDataArrayFromPath<IDataArray, ConvertColorToGrayScale>(this, inputAMPath);
    if(getErrorCondition() < 0)
    {
      return;
    }
    QVector<size_t> outCDims(1, 1);
    if(outAM.get() == nullptr)
    {
      outAM = dc->getPrereqAttributeMatrix<ConvertColorToGrayScale>(this, inputAMPath.getAttributeMatrixName(), -62105);
    }
    outAM->createAndAddAttributeArray<UInt8ArrayType, AbstractFilter, uint8_t>(this, newName, 0, outCDims);
    DataArrayPath newPath(dc->getName(), outAM->getName(), newName);
    m_OutputArrayPaths[i] = newPath;
  }

  if(static_cast<ConversionType>(m_ConversionAlgorithm) == ConversionType::SingleChannel)
  {
    if(m_ColorChannel > 2)
    {
      setErrorCondition(-62104);
      QString message = QObject::tr("The color channel should be 0, 1 or 2");
      notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertColorToGrayScale::preflight()
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
void ConvertColorToGrayScale::execute()
{
  initialize();
  setErrorCondition(0);
  setWarningCondition(0);
  QString ss;
  dataCheck();
  if(getErrorCondition() < 0)
  {
    setErrorCondition(-62106);
    ss = QObject::tr("DataCheck did not pass during execute");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<DataArrayPath> inputArrayPaths = getInputDataArrayVector();
  qint32 size = inputArrayPaths.size();

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  for(qint32 i = 0; i < size; i++)
  {
    DataArrayPath arrayPath = inputArrayPaths[i];

    // get volume container
    DataContainer::Pointer m = getDataContainerArray()->getDataContainer(arrayPath.getDataContainerName());
    QString attrMatName = arrayPath.getAttributeMatrixName();
    AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(arrayPath);

    // get input and output data
    IDataArray::Pointer inputData = attrMat->getAttributeArray(arrayPath.getDataArrayName());
    UInt8ArrayType::Pointer inputColorData = std::dynamic_pointer_cast<UInt8ArrayType>(inputData);

    if(nullptr == inputColorData.get())
    {
      setErrorCondition(-62107);
      ss = QObject::tr("Input Color Data at ArrayPath '%1' was not available. This array will be skipped.").arg(arrayPath.serialize("/"));
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      continue;
    }

    DataArrayPath newPath = m_OutputArrayPaths[i];

    AttributeMatrix::Pointer outAttrMat = getDataContainerArray()->getAttributeMatrix(newPath);

    IDataArray::Pointer outputData = outAttrMat->getAttributeArray(newPath.getDataArrayName());
    UInt8ArrayType::Pointer outputGrayData = std::dynamic_pointer_cast<UInt8ArrayType>(outputData);

    if(nullptr == outputGrayData.get())
    {
      setErrorCondition(-62108);
      ss = QObject::tr("Output Data at ArrayPath '%1' was not available. This array will be skipped.").arg(newPath.serialize("/"));
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      continue;
    }

    ConversionType convType = static_cast<ConversionType>(getConversionAlgorithm());

    size_t comp = inputColorData->getNumberOfComponents();
    size_t totalPoints = inputColorData->getNumberOfTuples();

    if(ConversionType::Luminosity == convType)
    {
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
      if(doParallel == true)
      {
        tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), LuminosityImpl(inputColorData->getPointer(0), outputGrayData->getPointer(0), m_ColorWeights, comp), tbb::auto_partitioner());
      }
      else
#endif
      {
        LuminosityImpl serial(inputColorData->getPointer(0), outputGrayData->getPointer(0), m_ColorWeights, comp);
        serial.convert(0, totalPoints);
      }
    }
    else if(ConversionType::Average == convType)
    {
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
      if(doParallel == true)
      {
        tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), AverageImpl(inputColorData->getPointer(0), outputGrayData->getPointer(0), comp), tbb::auto_partitioner());
      }
      else
#endif
      {
        AverageImpl serial(inputColorData->getPointer(0), outputGrayData->getPointer(0), comp);
        serial.convert(0, totalPoints);
      }
    }
    else if(ConversionType::Lightness == convType)
    {
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
      if(doParallel == true)
      {
        tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), LightnessImpl(inputColorData->getPointer(0), outputGrayData->getPointer(0), comp), tbb::auto_partitioner());
      }
      else
#endif
      {
        LightnessImpl serial(inputColorData->getPointer(0), outputGrayData->getPointer(0), comp);
        serial.convert(0, totalPoints);
      }
    }
    else if(ConversionType::SingleChannel == convType)
    {
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
      if(doParallel == true)
      {
        tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), SingleChannelImpl(inputColorData->getPointer(0), outputGrayData->getPointer(0), comp, getColorChannel()),
                          tbb::auto_partitioner());
      }
      else
#endif
      {
        SingleChannelImpl serial(inputColorData->getPointer(0), outputGrayData->getPointer(0), comp, getColorChannel());
        serial.convert(0, totalPoints);
      }
    }
    else
    {

      setErrorCondition(-62107);
      ss = QObject::tr("Conversion Algorithm did not match any available type. The type given was %1.").arg(getConversionAlgorithm());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }

#if 0
    size_t totalPoints = m_ImageDataPtr.lock()->getNumberOfTuples();

    float Rfactor = 1.0f;
    float Gfactor = 1.0f;
    float Bfactor = 1.0f;
    if(m_FlattenMethod == SIMPL::ConvertColorToGrayScaleMethod::Average)
    {
        Rfactor = 1.0f / 3.0f;
        Gfactor = 1.0f / 3.0f;
        Bfactor = 1.0f / 3.0f;
    }
    else if(m_FlattenMethod == SIMPL::ConvertColorToGrayScaleMethod::Luminosity)
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

    //  qDebug() << "ConvertColorToGrayScale: " << m_ConversionFactor << "\n";
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if(doParallel == true)
    {
        tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), ConvertColorToGrayScaleImpl(m_ImageData, m_FlatImageData, Rfactor, Gfactor, Bfactor, comp), tbb::auto_partitioner());
    }
    else
#endif
    {
        ConvertColorToGrayScaleImpl serial(m_ImageData, m_FlatImageData, Rfactor, Gfactor, Bfactor, comp);
        serial.convert(0, totalPoints);
    }

#endif
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ConvertColorToGrayScale::newFilterInstance(bool copyFilterParameters)
{
  ConvertColorToGrayScale::Pointer filter = ConvertColorToGrayScale::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertColorToGrayScale::getCompiledLibraryName()
{
  return ProcessingConstants::ProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertColorToGrayScale::getBrandingString()
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertColorToGrayScale::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << Processing::Version::Major() << "." << Processing::Version::Minor() << "." << Processing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertColorToGrayScale::getGroupName()
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertColorToGrayScale::getSubGroupName()
{
  return SIMPL::FilterSubGroups::ImageFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertColorToGrayScale::getHumanLabel()
{
  return "Color to GrayScale";
}
