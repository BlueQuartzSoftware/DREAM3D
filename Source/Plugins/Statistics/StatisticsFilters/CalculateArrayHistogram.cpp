/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2013 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
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
 *  This code was written under United States Air Force Contract number
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "CalculateArrayHistogram.h"


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
  m_NewDataArrayName(""),
  m_NewAttributeMatrixName(""),
  m_NewDataContainer(false),
  m_NewDataContainerName(""),
  m_NewDataArray(NULL)
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
  parameters.push_back(FilterParameter::New("Selected Array", "SelectedArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedArrayPath(), false));
  parameters.push_back(FilterParameter::New("Number of Bins", "NumberOfBins", FilterParameterWidgetType::IntWidget, getNumberOfBins(), false));
  //  parameters.push_back(FilterParameter::New("Normalize", "Normalize", FilterParameterWidgetType::BooleanWidget, getNormalize(), false));
  QStringList linkedProps;
  linkedProps << "MinRange" << "MaxRange";
  parameters.push_back(LinkedBooleanFilterParameter::New("Use Min & Max Range", "UserDefinedRange", getNewDataContainer(), linkedProps, false));

  parameters.push_back(FilterParameter::New("Min Value", "MinRange", FilterParameterWidgetType::DoubleWidget, getMinRange(), false));
  parameters.push_back(FilterParameter::New("Max Value", "MaxRange", FilterParameterWidgetType::DoubleWidget, getMaxRange(), false));


  linkedProps.clear();
  linkedProps << "NewDataContainerName";
  parameters.push_back(LinkedBooleanFilterParameter::New("New Data Container", "NewDataContainer", getNewDataContainer(), linkedProps, false));
  parameters.push_back(FilterParameter::New("New Data Container Name", "NewDataContainerName", FilterParameterWidgetType::StringWidget, getNewDataContainerName(), false, ""));

  parameters.push_back(FilterParameter::New("New Attribute Matrix Name", "NewAttributeMatrixName", FilterParameterWidgetType::StringWidget, getNewAttributeMatrixName(), false, ""));
  parameters.push_back(FilterParameter::New("New Histogram Array Name", "NewDataArrayName", FilterParameterWidgetType::StringWidget, getNewDataArrayName(), false, ""));

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
int CalculateArrayHistogram::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
      DREAM3D_FILTER_WRITE_PARAMETER(SelectedArrayPath)
      DREAM3D_FILTER_WRITE_PARAMETER(NumberOfBins)
      DREAM3D_FILTER_WRITE_PARAMETER(Normalize)
      DREAM3D_FILTER_WRITE_PARAMETER(NewDataArrayName)
      DREAM3D_FILTER_WRITE_PARAMETER(NewAttributeMatrixName)
      DREAM3D_FILTER_WRITE_PARAMETER(NewDataContainer)
      DREAM3D_FILTER_WRITE_PARAMETER(NewDataContainerName)
      writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CalculateArrayHistogram::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  if (m_SelectedArrayPath.isValid() == false)
  {
    setErrorCondition(-11010);
    QString ss = QObject::tr("The DataArray path for variable '%1' is not valid. There may be missing parts in the path. Please set a valid path that has a DataContainer, AttributeMatrix and DataArray name.").arg("Selected Array");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_NewAttributeMatrixName.isEmpty() == true)
  {
    setErrorCondition(-11009);
    QString ss = QObject::tr("The New Histogram Attribute Matrix name can not be empty. Please set a value.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_NewDataArrayName.isEmpty() == true)
  {
    setErrorCondition(-11008);
    QString ss = QObject::tr("The New Histogram Array name can not be empty. Please set a value.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_NewDataContainer && m_NewDataContainerName.isEmpty() == true)
  {
    setErrorCondition(-11014);
    QString ss = QObject::tr("The New Data Container name can not be empty. Please set a value.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_NumberOfBins <= 0)
  {
    setErrorCondition(-11011);
    QString ss = QObject::tr("The Number of Bins can not be zero or less. The current value is '%1'. Please set a value greater than zero.").arg(m_NumberOfBins);
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

  if (m_NewDataContainer) // create a new data container
  {
    DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getNewDataContainerName());
    if (getErrorCondition() < 0) { return; }
    AttributeMatrix::Pointer attrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getNewAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Generic);
    if (getErrorCondition() < 0 || NULL == attrMat.get()) { return; }
    tempPath.update(getNewDataContainerName(), getNewAttributeMatrixName(), newArrayName);
  }
  else // use existing data container
  {
    DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName());
    if (getErrorCondition() < 0) { return; }
    AttributeMatrix::Pointer attrMat = dc->createNonPrereqAttributeMatrix<AbstractFilter>(this, getNewAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Generic);
    if (getErrorCondition() < 0 || NULL == attrMat.get())  { return; }
    tempPath.update(dc->getName(), getNewAttributeMatrixName(), newArrayName);
  }

  // histogram array
  m_NewDataArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<>(this, tempPath, 0, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_NewDataArrayPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
    m_NewDataArray = m_NewDataArrayPtr.lock()->getPointer(0); /* Now assign the raw pointer to data from the DataArray<T> object */
  }
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
void findHistogram(IDataArray::Pointer inputData, int numberOfBins, bool userRange, double minRange, double maxRange, DataArray<double>::Pointer newDataArray)
{
  DataArray<T>* inputArray = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == inputArray)
  {
    return;
  }

  newDataArray->initializeWithZeros(); // we must initialize the histogram array to prepare for incrementing the array elements
  double* newDataArrayPtr = newDataArray->getPointer(0);

  T* inputArrayPtr = inputArray->getPointer(0);
  size_t numPoints = inputArray->getNumberOfTuples();
  int bin = 0;
  float min = std::numeric_limits<float>::max();
  float max = -1.0 * std::numeric_limits<float>::max();
  if(userRange)
  {
    min = minRange;
    max = maxRange;
  }
  else
  {
    for (size_t i = 0; i < numPoints; i++) // min and max in the input array
    {
      if(static_cast<float>(inputArrayPtr[i]) > max) { max = static_cast<float>(inputArrayPtr[i]); }
      if(static_cast<float>(inputArrayPtr[i]) < min) { min = static_cast<float>(inputArrayPtr[i]); }
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
        newDataArrayPtr[bin*2+1]++; // increment histogram element corresponding to this input array value
      }
    }
  }


  for(int i = 0; i < numberOfBins; i++)
  {
    newDataArrayPtr[i*2] = min + increment *(i+1);
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
  if(getErrorCondition() < 0) { return; }

  QString ss;
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName());

  IDataArray::Pointer inputData = m->getAttributeMatrix(m_SelectedArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedArrayPath.getDataArrayName());
  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedArrayPath.getDataArrayName());
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<size_t> dims = inputData->getComponentDimensions();
  int numComp = dims[0];
  for (int i = 1; i < dims.size(); i++)
  {
    numComp *= dims[i];
  }
  if (numComp > 1)
  {
    ss = QObject::tr("Selected array '%1' is not a scalar array").arg(m_SelectedArrayPath.getDataArrayName());
    setErrorCondition(-11003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QString dType = inputData->getTypeAsString();
  if (dType.compare("int8_t") == 0)
  {
    findHistogram<int8_t>(inputData, m_NumberOfBins, m_UserDefinedRange, m_MinRange, m_MaxRange, m_NewDataArrayPtr.lock());
  }
  else if (dType.compare("uint8_t") == 0)
  {
    findHistogram<uint8_t>(inputData, m_NumberOfBins, m_UserDefinedRange, m_MinRange, m_MaxRange, m_NewDataArrayPtr.lock());
  }
  else if (dType.compare("int16_t") == 0)
  {
    findHistogram<int16_t>(inputData, m_NumberOfBins, m_UserDefinedRange, m_MinRange, m_MaxRange, m_NewDataArrayPtr.lock());
  }
  else if (dType.compare("uint16_t") == 0)
  {
    findHistogram<uint16_t>(inputData, m_NumberOfBins, m_UserDefinedRange, m_MinRange, m_MaxRange, m_NewDataArrayPtr.lock());
  }
  else if (dType.compare("int32_t") == 0)
  {
    findHistogram<int32_t>(inputData, m_NumberOfBins, m_UserDefinedRange, m_MinRange, m_MaxRange, m_NewDataArrayPtr.lock());
  }
  else if (dType.compare("uint32_t") == 0)
  {
    findHistogram<uint32_t>(inputData, m_NumberOfBins, m_UserDefinedRange, m_MinRange, m_MaxRange, m_NewDataArrayPtr.lock());
  }
  else if (dType.compare("int64_t") == 0)
  {
    findHistogram<int64_t>(inputData, m_NumberOfBins, m_UserDefinedRange, m_MinRange, m_MaxRange, m_NewDataArrayPtr.lock());
  }
  else if (dType.compare("uint64_t") == 0)
  {
    findHistogram<uint64_t>(inputData, m_NumberOfBins, m_UserDefinedRange, m_MinRange, m_MaxRange, m_NewDataArrayPtr.lock());
  }
  else if (dType.compare("float") == 0)
  {
    findHistogram<float>(inputData, m_NumberOfBins, m_UserDefinedRange, m_MinRange, m_MaxRange, m_NewDataArrayPtr.lock());
  }
  else if (dType.compare("double") == 0)
  {
    findHistogram<double>(inputData, m_NumberOfBins, m_UserDefinedRange, m_MinRange, m_MaxRange, m_NewDataArrayPtr.lock());
  }
  else if (dType.compare("bool") == 0)
  {
    findHistogram<bool>(inputData, m_NumberOfBins, m_UserDefinedRange, m_MinRange, m_MaxRange, m_NewDataArrayPtr.lock());
  }
  else
  {
    ss = QObject::tr("Incorrect type for selected array '%1' for histogram ").arg(m_SelectedArrayPath.getDataArrayName());
    setErrorCondition(-11003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
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
{ return Statistics::StatisticsBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CalculateArrayHistogram::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CalculateArrayHistogram::getSubGroupName()
{ return DREAM3D::FilterSubGroups::EnsembleStatsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CalculateArrayHistogram::getHumanLabel()
{ return "Calculate Frequency Histogram"; }

