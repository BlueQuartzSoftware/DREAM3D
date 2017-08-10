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

#include "MultiThresholdObjects2.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/ThresholdFilterHelper.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ComparisonSelectionAdvancedFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "Processing/ProcessingConstants.h"
#include "Processing/ProcessingVersion.h"

// Include the MOC generated file for this class
#include "moc_MultiThresholdObjects2.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiThresholdObjects2::MultiThresholdObjects2()
: AbstractFilter()
, m_DestinationArrayName(SIMPL::GeneralData::Mask)
, m_SelectedThresholds()
, m_Destination(nullptr)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiThresholdObjects2::~MultiThresholdObjects2()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects2::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    ComparisonSelectionAdvancedFilterParameter::Pointer parameter = ComparisonSelectionAdvancedFilterParameter::New();
    parameter->setHumanLabel("Select Arrays to Threshold");
    parameter->setPropertyName("SelectedThresholds");

    parameter->setShowOperators(true);
    parameter->setCategory(FilterParameter::Parameter);
    parameter->setSetterCallback(SIMPL_BIND_SETTER(MultiThresholdObjects2, this, SelectedThresholds));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(MultiThresholdObjects2, this, SelectedThresholds));
    parameters.push_back(parameter);
  }
  parameters.push_back(SIMPL_NEW_STRING_FP("Output Attribute Array", DestinationArrayName, FilterParameter::CreatedArray, MultiThresholdObjects2));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects2::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDestinationArrayName(reader->readString("DestinationArrayName", getDestinationArrayName()));
  setSelectedThresholds(reader->readComparisonInputsAdvanced("SelectedThresholds", getSelectedThresholds()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects2::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects2::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  QVector<AbstractComparison::Pointer> comparisonValues = m_SelectedThresholds.getComparisonValues();

  if(comparisonValues.size() == 0)
  {
    setErrorCondition(-12000);
    notifyErrorMessage(getHumanLabel(), "You must add at least 1 threshold value.", getErrorCondition());
  }
  else
  {
    //int32_t count = comparisonValues.size();
    QString dcName = m_SelectedThresholds.getDataContainerName();
    QString amName = m_SelectedThresholds.getAttributeMatrixName();

    // Enforce that right now all the arrays MUST come from the same data container and attribute matrix
    if(dcName.isEmpty())
    {
      setErrorCondition(-13090);
      QString ss = QObject::tr("Threshold must have a DataContainer. None were selected");
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    if(amName.isEmpty())
    {
      setErrorCondition(-13091);
      QString ss = QObject::tr("Threshold must have an AttributeMatrix. None were selected");
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

    //AbstractComparison::Pointer comp = m_SelectedThresholds[0];
    QVector<size_t> cDims(1, 1);
    DataArrayPath tempPath(dcName, amName, getDestinationArrayName());
    m_DestinationPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, true,
                                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_DestinationPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_Destination = m_DestinationPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */

    // Do not allow non-scalar arrays
    for(size_t i = 0; i < comparisonValues.size(); ++i)
    {
      ComparisonValue::Pointer comp = std::dynamic_pointer_cast<ComparisonValue>(comparisonValues[i]);

      if (nullptr != comp)
      {
        tempPath.update(dcName, amName, comp->getAttributeArrayName());
        IDataArray::Pointer inputData = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, tempPath);
        if (getErrorCondition() >= 0)
        {
          cDims = inputData->getComponentDimensions();
          int32_t numComp = static_cast<int32_t>(cDims[0]);
          for (int32_t d = 1; d < cDims.size(); d++)
          {
            numComp *= cDims[d];
          }
          if (numComp > 1)
          {
            QString ss = QObject::tr("Selected array '%1' is not a scalar array").arg(comp->getAttributeArrayName());
            setErrorCondition(-11003);
            notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects2::preflight()
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
void MultiThresholdObjects2::execute()
{
  int32_t err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  // Get the first comparison object
  AbstractComparison::Pointer comp_0 = m_SelectedThresholds[0];
  // Get the names of the Data Container and AttributeMatrix for later
  QString dcName = m_SelectedThresholds.getDataContainerName();
  QString amName = m_SelectedThresholds.getAttributeMatrixName();

  DataContainerArray::Pointer dca = getDataContainerArray();
  DataContainer::Pointer m = dca->getDataContainer(dcName);

  // At least one threshold value is required
  if (!m_SelectedThresholds.hasComparisonValue())
  {
    QString ss = QObject::tr("Error Executing threshold filter. There are no specified values to threshold against");
    setErrorCondition(-13001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  else
  {
    bool invert = m_SelectedThresholds.shouldInvert();

    int64_t thresholdSize;
    BoolArrayType::Pointer thresholdArray;

    createBoolArray(thresholdSize, thresholdArray);
    bool firstValueFound = false;

    // Loop on the remaining Comparison objects updating our final result array as we go
    for(int32_t i = 0; i < m_SelectedThresholds.size() && err >= 0; ++i)
    {
      if (std::dynamic_pointer_cast<ComparisonSet>(m_SelectedThresholds[i]))
      {
        ComparisonSet::Pointer comparisonSet = std::dynamic_pointer_cast<ComparisonSet>(m_SelectedThresholds[i]);
        thresholdSet(comparisonSet, thresholdArray, err, !firstValueFound, false);
        firstValueFound = true;
      }
      else if(std::dynamic_pointer_cast<ComparisonValue>(m_SelectedThresholds[i]))
      {
        ComparisonValue::Pointer comparisonValue = std::dynamic_pointer_cast<ComparisonValue>(m_SelectedThresholds[i]);
        thresholdValue(comparisonValue, thresholdArray, err, !firstValueFound, false);
        firstValueFound = true;
      }
    }

    if (invert)
    {
      invertThreshold(thresholdSize, thresholdArray);
    }

    bool* threshold = thresholdArray->getPointer(0);
    for (int64_t p = 0; p < thresholdSize; p++)
    {
      m_Destination[p] = threshold[p];
    }
    
  }
  
  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects2::createBoolArray(int64_t& totalTuples, BoolArrayType::Pointer& thresholdArrayPtr)
{
  // Get the names of the Data Container and AttributeMatrix for later
  QString dcName = m_SelectedThresholds.getDataContainerName();
  QString amName = m_SelectedThresholds.getAttributeMatrixName();

  DataContainerArray::Pointer dca = getDataContainerArray();
  DataContainer::Pointer m = dca->getDataContainer(dcName);

  // Get the total number of tuples, create and initialize an array to use for these results
  totalTuples = static_cast<int64_t>(m->getAttributeMatrix(amName)->getNumberOfTuples());
  thresholdArrayPtr = BoolArrayType::CreateArray(totalTuples, "_INTERNAL_USE_ONLY_TEMP");

  // Initialize the array to false
  thresholdArrayPtr->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects2::insertThreshold(int64_t numItems, BoolArrayType::Pointer currentArrayPtr, int unionOperator, const BoolArrayType::Pointer newArrayPtr, bool inverse)
{
  bool* newArray = newArrayPtr->getPointer(0);
  bool* currentArray = currentArrayPtr->getPointer(0);

  for (int64_t i = 0; i < numItems; i++)
  {
    // invert the current comparison if necessary
    if (inverse)
    {
      newArray[i] = !newArray[i];
    }

    if (SIMPL::Union::Operator_Or == unionOperator)
    {
      currentArray[i] = currentArray[i] || newArray[i];
    }
    else if (currentArray[i] == false || newArray[i] == false)
    {
      currentArray[i] = false;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects2::invertThreshold(int64_t numItems, BoolArrayType::Pointer thresholdArray)
{
  bool* threshold = thresholdArray->getPointer(0);

  for (int64_t i = 0; i < numItems; i++)
  {
    threshold[i] = !threshold[i];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects2::thresholdSet(ComparisonSet::Pointer comparisonSet, BoolArrayType::Pointer& currentThreshold, int32_t &err, bool replaceInput, bool inverse)
{
  if (nullptr == comparisonSet)
  {
    return;
  }

  if (inverse)
  {
    inverse = !comparisonSet->getInvertComparison();
  }
  else
  {
    inverse = comparisonSet->getInvertComparison();
  }

  int64_t setArraySize;
  BoolArrayType::Pointer setThresholdArray;

  createBoolArray(setArraySize, setThresholdArray);
  bool firstValueFound = false;

  QVector<AbstractComparison::Pointer> comparisons = comparisonSet->getComparisons();
  for (int i = 0; i < comparisons.size(); i++)
  {
    // Check all contents of child Comparison Sets
    if (std::dynamic_pointer_cast<ComparisonSet>(comparisons.at(i)))
    { 
      ComparisonSet::Pointer childSet = std::dynamic_pointer_cast<ComparisonSet>(comparisons.at(i));
      thresholdSet(childSet, setThresholdArray, err, !firstValueFound, false);
      firstValueFound = true;
    }
    // Check Comparison Values
    if (std::dynamic_pointer_cast<ComparisonValue>(comparisons.at(i)))
    {
      ComparisonValue::Pointer childValue = std::dynamic_pointer_cast<ComparisonValue>(comparisons.at(i));
      thresholdValue(childValue, setThresholdArray, err, !firstValueFound, false);
      firstValueFound = true;
    }

    if (err < 0)
    {
      return;
    }
  }

  if (replaceInput)
  {
    if (inverse)
    {
      invertThreshold(setArraySize, setThresholdArray);
    }
    currentThreshold.swap(setThresholdArray);
  }
  else
  {
    // insert into current threshold
    insertThreshold(setArraySize, currentThreshold, comparisonSet->getUnionOperator(), setThresholdArray, inverse);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects2::thresholdValue(ComparisonValue::Pointer comparisonValue, BoolArrayType::Pointer& inputThreshold, int32_t &err, bool replaceInput, bool inverse)
{
  if (nullptr == comparisonValue)
  {
    return;
  }

  // Get the names of the Data Container and AttributeMatrix for later
  QString dcName = m_SelectedThresholds.getDataContainerName();
  QString amName = m_SelectedThresholds.getAttributeMatrixName();

  DataContainerArray::Pointer dca = getDataContainerArray();
  DataContainer::Pointer m = dca->getDataContainer(dcName);

  // Get the total number of tuples, create and initialize an array to use for these results
  int64_t totalTuples = static_cast<int64_t>(m->getAttributeMatrix(amName)->getNumberOfTuples());
  BoolArrayType::Pointer currentArrayPtr = BoolArrayType::CreateArray(totalTuples, "_INTERNAL_USE_ONLY_TEMP");

  // Initialize the array to false
  currentArrayPtr->initializeWithZeros();

  //bool* currentArray = currentArrayPtr->getPointer(0);
  int compOperator = comparisonValue->getCompOperator();
  double compValue = comparisonValue->getCompValue();

  ThresholdFilterHelper filter(static_cast<SIMPL::Comparison::Enumeration>(compOperator), compValue, currentArrayPtr.get());

  err = filter.execute(m->getAttributeMatrix(amName)->getAttributeArray(comparisonValue->getAttributeArrayName()).get(), currentArrayPtr.get());
  if (err < 0)
  {
    DataArrayPath tempPath(m_SelectedThresholds.getDataContainerName(), m_SelectedThresholds.getAttributeMatrixName(), comparisonValue->getAttributeArrayName());
    QString ss = QObject::tr("Error Executing threshold filter on array. The path is %1").arg(tempPath.serialize());
    setErrorCondition(-13002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  
  if (replaceInput)
  {
    if (inverse)
    {
      invertThreshold(totalTuples, currentArrayPtr);
    }
    inputThreshold.swap(currentArrayPtr);
  }
  else 
  {
    // insert into current threshold
    insertThreshold(totalTuples, inputThreshold, comparisonValue->getUnionOperator(), currentArrayPtr, inverse);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MultiThresholdObjects2::newFilterInstance(bool copyFilterParameters)
{
  MultiThresholdObjects2::Pointer filter = MultiThresholdObjects2::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiThresholdObjects2::getCompiledLibraryName()
{
  return ProcessingConstants::ProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiThresholdObjects2::getBrandingString()
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiThresholdObjects2::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << Processing::Version::Major() << "." << Processing::Version::Minor() << "." << Processing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiThresholdObjects2::getGroupName()
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiThresholdObjects2::getSubGroupName()
{
  return SIMPL::FilterSubGroups::ThresholdFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiThresholdObjects2::getHumanLabel()
{
  return "Threshold Objects 2";
}
