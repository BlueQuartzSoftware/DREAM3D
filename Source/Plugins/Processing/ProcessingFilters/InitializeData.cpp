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

#include "InitializeData.h"

#include <QtCore/QDateTime>
#include <QtCore/QCoreApplication>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Processing/ProcessingConstants.h"
#include "Processing/ProcessingVersion.h"

// Include the MOC generated file for this class
#include "moc_InitializeData.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void delay(int seconds)
{
  QTime dieTime = QTime::currentTime().addSecs(seconds);
  while (QTime::currentTime() < dieTime)
  {
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InitializeData::InitializeData() :
  AbstractFilter(),
  m_XMin(0),
  m_YMin(0),
  m_ZMin(0),
  m_XMax(0),
  m_YMax(0),
  m_ZMax(0),
  m_InitType(Manual),
  m_InitValue(0)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InitializeData::~InitializeData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeData::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    MultiDataArraySelectionFilterParameter::RequirementType req = MultiDataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(MultiDataArraySelectionFilterParameter::New("Cell Arrays", "CellAttributeMatrixPaths", getCellAttributeMatrixPaths(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(IntFilterParameter::New("X Min (Column)", "XMin", getXMin(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Y Min (Row)", "YMin", getYMin(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Z Min (Plane)", "ZMin", getZMin(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("X Max (Column)", "XMax", getXMax(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Y Max (Row)", "YMax", getYMax(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Z Max (Plane)", "ZMax", getZMax(), FilterParameter::Parameter));

  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Initialization Type");
    parameter->setPropertyName("InitType");

    parameter->setDefaultValue(Manual);

    QVector<QString> choices;
    choices.push_back("Manual");
    choices.push_back("Random");
    choices.push_back("Random With Range");
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "InitValue" << "InitRange";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(DoubleFilterParameter::New("Initialization Value", "InitValue", getInitValue(), FilterParameter::Parameter, Manual));
  parameters.push_back(RangeFilterParameter::New("Initialization Range", "InitRange", getInitRange(), FilterParameter::Parameter, RandomWithRange));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellAttributeMatrixPaths( reader->readDataArrayPathVector("CellAttributeMatrixPaths", getCellAttributeMatrixPaths() ) );
  setXMin( reader->readValue("XMin", getXMin()) );
  setYMin( reader->readValue("YMin", getYMin()) );
  setZMin( reader->readValue("ZMin", getZMin()) );
  setXMax( reader->readValue("XMax", getXMax()) );
  setYMax( reader->readValue("YMax", getYMax()) );
  setZMax( reader->readValue("ZMax", getZMax()) );
  setInitType(reader->readValue("InitType", getInitType()));
  setInitValue(reader->readValue("InitValue", getInitValue()));
  setInitRange(reader->readPairOfDoubles("InitRange", getInitRange()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int InitializeData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(CellAttributeMatrixPaths)
  SIMPL_FILTER_WRITE_PARAMETER(XMin)
  SIMPL_FILTER_WRITE_PARAMETER(YMin)
  SIMPL_FILTER_WRITE_PARAMETER(ZMin)
  SIMPL_FILTER_WRITE_PARAMETER(XMax)
  SIMPL_FILTER_WRITE_PARAMETER(YMax)
  SIMPL_FILTER_WRITE_PARAMETER(ZMax)
  SIMPL_FILTER_WRITE_PARAMETER(InitType)
  SIMPL_FILTER_WRITE_PARAMETER(InitValue)
  SIMPL_FILTER_WRITE_PARAMETER(InitRange)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeData::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeData::dataCheck()
{
  setErrorCondition(0);

  if (m_CellAttributeMatrixPaths.size() <= 0)
  {
    QString ss = "At least one data array must be selected.";
    setErrorCondition(-5550);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataArrayPath attributeMatrixPath(m_CellAttributeMatrixPaths[0].getDataContainerName(), m_CellAttributeMatrixPaths[0].getAttributeMatrixName(), "");
  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, attributeMatrixPath, -301);

  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, attributeMatrixPath.getDataContainerName());
  if(NULL == image.get()) { return; }

  if (getXMax() < getXMin())
  {
    QString ss = QObject::tr("X Max (%1) less than X Min (%2)").arg(getXMax()).arg(getXMin());
    setErrorCondition(-5551);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getYMax() < getYMin())
  {
    QString ss = QObject::tr("Y Max (%1) less than Y Min (%2)").arg(getYMax()).arg(getYMin());
    setErrorCondition(-5552);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getZMax() < getZMin())
  {
    QString ss = QObject::tr("Z Max (%1) less than Z Min (%2)").arg(getZMax()).arg(getZMin());
    setErrorCondition(-5553);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getXMin() < 0)
  {
    QString ss = QObject::tr("X Min (%1) less than 0").arg(getXMin());
    setErrorCondition(-5554);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getYMin() < 0)
  {
    QString ss = QObject::tr("Y Min (%1) less than 0").arg(getYMin());
    setErrorCondition(-5555);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getZMin() < 0)
  {
    QString ss = QObject::tr("Z Min (%1) less than 0").arg(getZMin());
    setErrorCondition(-5556);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getXMax() > (static_cast<int64_t>(image->getXPoints()) - 1))
  {
    QString ss = QObject::tr("The X Max you entered of %1 is greater than your Max X Point of %2").arg(getXMax()).arg(static_cast<int64_t>(image->getXPoints()) - 1);
    setErrorCondition(-5557);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getYMax() > (static_cast<int64_t>(image->getYPoints()) - 1))
  {
    QString ss = QObject::tr("The Y Max you entered of %1 is greater than your Max Y Point of %2").arg(getYMax()).arg(static_cast<int64_t>(image->getYPoints()) - 1);
    setErrorCondition(-5558);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getZMax() > (static_cast<int64_t>(image->getZPoints()) - 1))
  {
    QString ss = QObject::tr("The Z Max you entered of %1) greater than your Max Z Point of %2").arg(getZMax()).arg(static_cast<int64_t>(image->getZPoints()) - 1);
    setErrorCondition(-5559);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(attributeMatrixPath.getDataContainerName());

  size_t udims[3] =
  { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  QString attrMatName = attributeMatrixPath.getAttributeMatrixName();
  QList<QString> voxelArrayNames = DataArrayPath::GetDataArrayNames(m_CellAttributeMatrixPaths);

  for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(*iter);

    QString type = p->getTypeAsString();
    if (type == "int8_t")
    {
      checkInitialization<int8_t>(p);
    }
    else if (type == "int16_t")
    {
      checkInitialization<int16_t>(p);
    }
    else if (type == "int32_t")
    {
      checkInitialization<int32_t>(p);
    }
    else if (type == "int64_t")
    {
      checkInitialization<int64_t>(p);
    }
    else if (type == "uint8_t")
    {
      checkInitialization<uint8_t>(p);
    }
    else if (type == "uint16_t")
    {
      checkInitialization<uint16_t>(p);
    }
    else if (type == "uint32_t")
    {
      checkInitialization<uint32_t>(p);
    }
    else if (type == "uint64_t")
    {
      checkInitialization<uint64_t>(p);
    }
    else if (type == "float")
    {
      checkInitialization<float>(p);
    }
    else if (type == "double")
    {
      checkInitialization<double>(p);
    }

    if (getErrorCondition() < 0)
    {
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void InitializeData::checkInitialization(IDataArray::Pointer p)
{
  QString arrayName = p->getName();

  if (m_InitType == Manual)
  {
    double input = m_InitValue;
    if (input < static_cast<double>(std::numeric_limits<T>().lowest()) || input > static_cast<double>(std::numeric_limits<T>().max()))
    {
      setErrorCondition(-4000);
      QString ss = QObject::tr("%1: The initialization value could not be converted. The valid range is %2 to %3").arg(arrayName).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }
  else if (m_InitType == RandomWithRange)
  {
    double min = m_InitRange.first;
    double max = m_InitRange.second;
    if (min > max)
    {
      QString ss = arrayName + ": Invalid initialization range.  Minimum value is larger than maximum value.";
      setErrorCondition(-5550);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    else if (min < static_cast<double>(std::numeric_limits<T>().lowest()) || max > static_cast<double>(std::numeric_limits<T>().max()))
    {
      setErrorCondition(-4001);
      QString ss = QObject::tr("%1: The initialization range can only be from %2 to %3").arg(arrayName).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    else if (min == max)
    {
      setErrorCondition(-4002);
      QString ss = arrayName + ": The initialization range must have differing values";
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeData::preflight()
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
void InitializeData::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataArrayPath attributeMatrixPath(m_CellAttributeMatrixPaths[0].getDataContainerName(), m_CellAttributeMatrixPaths[0].getAttributeMatrixName(), "");
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(attributeMatrixPath.getDataContainerName());

  size_t udims[3] =
  { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  int64_t dims[3] =
  { static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]), };

  QString attrMatName = attributeMatrixPath.getAttributeMatrixName();
  QList<QString> voxelArrayNames = DataArrayPath::GetDataArrayNames(m_CellAttributeMatrixPaths);

  for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(*iter);

    QString type = p->getTypeAsString();
    if (type == "int8_t")
    {
      initializeArrayWithInts<int8_t>(p, dims);
    }
    else if (type == "int16_t")
    {
      initializeArrayWithInts<int16_t>(p, dims);
    }
    else if (type == "int32_t")
    {
      initializeArrayWithInts<int32_t>(p, dims);
    }
    else if (type == "int64_t")
    {
      initializeArrayWithInts<int64_t>(p, dims);
    }
    else if (type == "uint8_t")
    {
      initializeArrayWithInts<uint8_t>(p, dims);
    }
    else if (type == "uint16_t")
    {
      initializeArrayWithInts<uint16_t>(p, dims);
    }
    else if (type == "uint32_t")
    {
      initializeArrayWithInts<uint32_t>(p, dims);
    }
    else if (type == "uint64_t")
    {
      initializeArrayWithInts<uint64_t>(p, dims);
    }
    else if (type == "float")
    {
      initializeArrayWithReals<float>(p, dims);
    }
    else if (type == "double")
    {
      initializeArrayWithReals<double>(p, dims);
    }

    delay(1); // Delay the execution by 1 second to avoid the exact same seedings for each array
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void InitializeData::initializeArrayWithInts(IDataArray::Pointer p, int64_t dims[3])
{
  T rangeMin;
  T rangeMax;
  if (m_InitType == RandomWithRange)
  {
    rangeMin = m_InitRange.first;
    rangeMax = m_InitRange.second;
  }
  else
  {
    rangeMin = std::numeric_limits<T>().min();
    rangeMax = std::numeric_limits<T>().max();
  }

  typedef boost::mt19937 RandomNumberGenerator;
  typedef boost::uniform_int<T> IntDistribution;
  typedef boost::variate_generator<RandomNumberGenerator&, IntDistribution> IntGenerator;

  std::shared_ptr<IntDistribution> distribution = std::shared_ptr<IntDistribution>(new IntDistribution(rangeMin, rangeMax));
  std::shared_ptr<RandomNumberGenerator> randomNumberGenerator = std::shared_ptr<RandomNumberGenerator>(new RandomNumberGenerator);
  randomNumberGenerator->seed(static_cast<size_t>(QDateTime::currentMSecsSinceEpoch())); // seed with the current time
  std::shared_ptr<IntGenerator> intGeneratorPtr = std::shared_ptr<IntGenerator>(new IntGenerator(*randomNumberGenerator, *distribution));
  IntGenerator& intGenerator = *intGeneratorPtr;

  for (int32_t k = m_ZMin; k < m_ZMax + 1; k++)
  {
    for (int32_t j = m_YMin; j < m_YMax + 1; j++)
    {
      for (int32_t i = m_XMin; i < m_XMax + 1; i++)
      {
        size_t index = (k * dims[0] * dims[1]) + (j * dims[0]) + i;

        if (m_InitType == Manual)
        {
          T num = static_cast<T>(m_InitValue);
          p->initializeTuple(index, &num);
        }
        else
        {
          T temp = intGenerator();
          p->initializeTuple(index, &temp);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void InitializeData::initializeArrayWithReals(IDataArray::Pointer p, int64_t dims[3])
{
  T rangeMin;
  T rangeMax;
  if (m_InitType == RandomWithRange)
  {
    rangeMin = static_cast<T>(m_InitRange.first);
    rangeMax = static_cast<T>(m_InitRange.second);
  }
  else
  {
    rangeMin = std::numeric_limits<T>().min();
    rangeMax = std::numeric_limits<T>().max();
  }

  typedef boost::mt19937 RandomNumberGenerator;
  typedef boost::uniform_real<T> RealDistribution;
  typedef boost::variate_generator<RandomNumberGenerator&, RealDistribution> RealGenerator;

  std::shared_ptr<RealDistribution> distribution = std::shared_ptr<RealDistribution>(new RealDistribution(rangeMin, rangeMax));
  std::shared_ptr<RandomNumberGenerator> randomNumberGenerator = std::shared_ptr<RandomNumberGenerator>(new RandomNumberGenerator);
  randomNumberGenerator->seed(static_cast<size_t>(QDateTime::currentMSecsSinceEpoch())); // seed with the current time
  std::shared_ptr<RealGenerator> realGeneratorPtr = std::shared_ptr<RealGenerator>(new RealGenerator(*randomNumberGenerator, *distribution));
  RealGenerator& realGenerator = *realGeneratorPtr;

  for (int32_t k = m_ZMin; k < m_ZMax + 1; k++)
  {
    for (int32_t j = m_YMin; j < m_YMax + 1; j++)
    {
      for (int32_t i = m_XMin; i < m_XMax + 1; i++)
      {
        size_t index = (k * dims[0] * dims[1]) + (j * dims[0]) + i;

        if (m_InitType == Manual)
        {
          T num = static_cast<T>(m_InitValue);
          p->initializeTuple(index, &num);
        }
        else
        {
          T temp = realGenerator();
          p->initializeTuple(index, &temp);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer InitializeData::newFilterInstance(bool copyFilterParameters)
{
  InitializeData::Pointer filter = InitializeData::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeData::getCompiledLibraryName()
{
  return ProcessingConstants::ProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeData::getBrandingString()
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeData::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  Processing::Version::Major() << "." << Processing::Version::Minor() << "." << Processing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeData::getGroupName()
{ return SIMPL::FilterGroups::ProcessingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeData::getSubGroupName()
{ return SIMPL::FilterSubGroups::CleanupFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeData::getHumanLabel()
{ return "Initialize Data"; }
