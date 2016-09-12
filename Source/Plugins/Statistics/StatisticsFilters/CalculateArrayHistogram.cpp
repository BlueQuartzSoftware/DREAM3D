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

#include "CalculateArrayHistogram.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"

// Include the MOC generated file for this class
#include "moc_CalculateArrayHistogram.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculateArrayHistogram::CalculateArrayHistogram() :
  AbstractFilter(),
  m_SelectedArrayPath("", "", ""),
  m_NumberOfBins(-1),
  m_MinRange(0.0f),
  m_MaxRange(1.0f),
  m_UserDefinedRange(false),
  m_Normalize(false),
  m_NewAttributeMatrixName(SIMPL::Defaults::NewAttributeMatrixName),
  m_NewDataArrayName(SIMPL::CellData::Histogram),
  m_NewDataContainer(false),
  m_NewDataContainerName(SIMPL::Defaults::NewDataContainerName),
  m_InDataArray(nullptr),
  m_NewDataArray(nullptr)
{
  setupFilterParameters();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CalculateArrayHistogram::~CalculateArrayHistogram()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculateArrayHistogram::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Bins", NumberOfBins, FilterParameter::Parameter, CalculateArrayHistogram));
  QStringList linkedProps;
  linkedProps << "MinRange" << "MaxRange";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Min & Max Range", UserDefinedRange, FilterParameter::Parameter, CalculateArrayHistogram, linkedProps));
  parameters.push_back(SIMPL_NEW_DOUBLE_FP("Min Value", MinRange, FilterParameter::Parameter, CalculateArrayHistogram));
  parameters.push_back(SIMPL_NEW_DOUBLE_FP("Max Value", MaxRange, FilterParameter::Parameter, CalculateArrayHistogram));
  linkedProps.clear();
  linkedProps << "NewDataContainerName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("New Data Container", NewDataContainer, FilterParameter::Parameter, CalculateArrayHistogram, linkedProps));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::Defaults::AnyPrimitive, 1, SIMPL::AttributeMatrixObjectType::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Attribute Array to Histogram", SelectedArrayPath, FilterParameter::RequiredArray, CalculateArrayHistogram, req));
  }
  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container ", NewDataContainerName, FilterParameter::CreatedArray, CalculateArrayHistogram));
  parameters.push_back(SIMPL_NEW_STRING_FP("Attribute Matrix", NewAttributeMatrixName, FilterParameter::CreatedArray, CalculateArrayHistogram));
  parameters.push_back(SIMPL_NEW_STRING_FP("Histogram", NewDataArrayName, FilterParameter::CreatedArray, CalculateArrayHistogram));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculateArrayHistogram::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedArrayPath(reader->readDataArrayPath("SelectedArrayPath", getSelectedArrayPath()));
  setNumberOfBins(reader->readValue("NumberOfBins", getNumberOfBins()));
  setNormalize(reader->readValue("Normalize", false));
  setNewAttributeMatrixName(reader->readString("NewAttributeMatrixName", getNewAttributeMatrixName()));
  setNewDataArrayName(reader->readString("NewDataArrayName", getNewDataArrayName()));
  setNewDataContainer(reader->readValue("NewDataContainer", false));
  setNewDataContainerName(reader->readString("NewDataContainerName", getNewDataContainerName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculateArrayHistogram::readFilterParameters(QJsonObject &obj)
{
  AbstractFilter::readFilterParameters(obj);
  setNormalize(static_cast<bool>(obj["Normalize"].toInt()));
}

// FP: Check why these values are not connected to a filter parameter!

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculateArrayHistogram::writeFilterParameters(QJsonObject &obj)
{
  AbstractFilter::writeFilterParameters(obj);
  obj["Normalize"] = static_cast<int>(getNormalize());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculateArrayHistogram::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculateArrayHistogram::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  if (m_NumberOfBins <= 0)
  {
    setErrorCondition(-11011);
    QString ss = QObject::tr("The number of bins (%1) must be positive").arg(m_NumberOfBins);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<size_t> tDims(1, m_NumberOfBins);
  QVector<size_t> cDims(1, 2);

  QString newArrayName;
  if (m_Normalize == true)
  {
    newArrayName = getNewDataArrayName() + QString("_Normalized");
  }
  else
  {
    newArrayName = getNewDataArrayName();
  }

  m_InDataArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSelectedArrayPath());
  if (getErrorCondition() < 0) { return; }
  if (nullptr != m_InDataArrayPtr.lock().get())
  {
    int32_t cDims = m_InDataArrayPtr.lock()->getNumberOfComponents();
    if(cDims != 1)
    {
      QString ss = QObject::tr("Selected array has number of components %1 and is not a scalar array. The path is %2").arg(cDims).arg(getSelectedArrayPath().serialize());
      setErrorCondition(-11003);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }

  if (m_NewDataContainer) // create a new data container
  {
    DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getNewDataContainerName());
    if (getErrorCondition() < 0) { return; }
    AttributeMatrix::Pointer attrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getNewAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::Generic);
    if (getErrorCondition() < 0 || nullptr == attrMat.get()) { return; }
    tempPath.update(getNewDataContainerName(), getNewAttributeMatrixName(), newArrayName);
  }
  else // use existing data container
  {
    DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName());
    AttributeMatrix::Pointer attrMat = dc->createNonPrereqAttributeMatrix<AbstractFilter>(this, getNewAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::Generic);
    if (getErrorCondition() < 0 || nullptr == attrMat.get())  { return; }
    tempPath.update(dc->getName(), getNewAttributeMatrixName(), newArrayName);
  }

  // histogram array
  m_NewDataArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<>(this, tempPath, 0, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (nullptr != m_NewDataArrayPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  { m_NewDataArray = m_NewDataArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculateArrayHistogram::preflight()
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
template<typename T>
void findHistogram(IDataArray::Pointer inDataPtr, int32_t numberOfBins, bool userRange, double minRange, double maxRange, DoubleArrayType::Pointer newDataArray)
{
  typename DataArray<T>::Pointer inputDataPtr = std::dynamic_pointer_cast<DataArray<T> >(inDataPtr);

  newDataArray->initializeWithZeros(); // we must initialize the histogram array to prepare for incrementing the array elements
  double* newDataArrayPtr = newDataArray->getPointer(0);

  T* inputArrayPtr = inputDataPtr->getPointer(0);
  size_t numPoints = inputDataPtr->getNumberOfTuples();
  int32_t bin = 0;
  float min = std::numeric_limits<float>::max();
  float max = -1.0 * std::numeric_limits<float>::max();
  if (userRange)
  {
    min = minRange;
    max = maxRange;
  }
  else
  {
    for (size_t i = 0; i < numPoints; i++) // min and max in the input array
    {
      if (static_cast<float>(inputArrayPtr[i]) > max) { max = static_cast<float>(inputArrayPtr[i]); }
      if (static_cast<float>(inputArrayPtr[i]) < min) { min = static_cast<float>(inputArrayPtr[i]); }
    }
  }

  float increment = (max - min) / (numberOfBins);
  if (numberOfBins == 1) // if one bin, just set the first element to total number of points
  {
    newDataArrayPtr[0] = max;
    newDataArrayPtr[1] = numPoints;
  }
  else
  {
    for (size_t i = 0; i < numPoints; i++) // sort into bins to create the histogram
    {
      bin = size_t((inputArrayPtr[i] - min) / increment); // find bin for this input array value
      if ((bin >= 0) && (bin < numberOfBins)) // make certain bin is in range
      {
        newDataArrayPtr[bin * 2 + 1]++; // increment histogram element corresponding to this input array value
      }
    }
  }

  for(int32_t i = 0; i < numberOfBins; i++)
  {
    newDataArrayPtr[i * 2] = min + increment * (i + 1);
  }

  //  if (normalize) // if normalize is checked, divide each element in the histogram by total number of points
  //  {
  //    for (size_t i = 0; i < numberOfBins; i++)
  //    {
  //      newDataArrayPtr[i] /= numPoints;
  //    }
  //  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculateArrayHistogram::execute()
{
  setErrorCondition(0);
  dataCheck();
  if (getErrorCondition() < 0) { return; }

  EXECUTE_FUNCTION_TEMPLATE(this, findHistogram, m_InDataArrayPtr.lock(), m_InDataArrayPtr.lock(), m_NumberOfBins, m_UserDefinedRange, m_MinRange, m_MaxRange, m_NewDataArrayPtr.lock())

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CalculateArrayHistogram::newFilterInstance(bool copyFilterParameters)
{
  CalculateArrayHistogram::Pointer filter = CalculateArrayHistogram::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CalculateArrayHistogram::getCompiledLibraryName()
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CalculateArrayHistogram::getBrandingString()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CalculateArrayHistogram::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CalculateArrayHistogram::getGroupName()
{ return SIMPL::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CalculateArrayHistogram::getSubGroupName()
{ return SIMPL::FilterSubGroups::EnsembleStatsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CalculateArrayHistogram::getHumanLabel()
{ return "Calculate Frequency Histogram"; }
