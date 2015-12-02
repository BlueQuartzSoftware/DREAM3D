/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "MultiThresholdObjects.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/ThresholdFilterHelper.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/ComparisonSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

#include "Processing/ProcessingConstants.h"

// Include the MOC generated file for this class
#include "moc_MultiThresholdObjects.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiThresholdObjects::MultiThresholdObjects() :
  AbstractFilter(),
  m_DestinationArrayName(DREAM3D::GeneralData::Mask),
  m_SelectedThresholds(),
  m_Destination(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiThresholdObjects::~MultiThresholdObjects()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    ComparisonSelectionFilterParameter::Pointer parameter = ComparisonSelectionFilterParameter::New();
    parameter->setHumanLabel("Select Arrays to Threshold");
    parameter->setPropertyName("SelectedThresholds");

    parameter->setShowOperators(true);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(StringFilterParameter::New("Output Attribute Array", "DestinationArrayName", getDestinationArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDestinationArrayName(reader->readString("DestinationArrayName", getDestinationArrayName() ) );
  setSelectedThresholds(reader->readComparisonInputs("SelectedThresholds", getSelectedThresholds()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MultiThresholdObjects::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(DestinationArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(SelectedThresholds)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects::dataCheck()
{
  setErrorCondition(0);

  if (m_SelectedThresholds.size() == 0)
  {
    setErrorCondition(-12000);
    notifyErrorMessage(getHumanLabel(), "You must add at least 1 threshold value.", getErrorCondition());
  }
  else
  {
    int32_t count = m_SelectedThresholds.size();
    QSet<QString> dcSet;
    QSet<QString> amSet;

    // Loop through each selected threshold item which will have the complete path and check that path
    for(int32_t i = 0; i < count; i++)
    {
      ComparisonInput_t comp = m_SelectedThresholds[i];
      dcSet.insert(comp.dataContainerName);
      amSet.insert(comp.attributeMatrixName);
    }

    // Enforce that right now all the arrays MUST come from the same data container and attribute matrix
    if(dcSet.size() != 1)
    {
      setErrorCondition(-13090);
      QString ss = QObject::tr("Threshold selections must come from the same DataContainer. %1 were selected").arg(dcSet.size());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    if(amSet.size() != 1)
    {
      setErrorCondition(-13091);
      QString ss = QObject::tr("Threshold selections must come from the same AttributeMatrix. %1 were selected").arg(amSet.size());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

    ComparisonInput_t comp = m_SelectedThresholds[0];
    QVector<size_t> cDims(1, 1);
    DataArrayPath tempPath(comp.dataContainerName, comp.attributeMatrixName, getDestinationArrayName());
    m_DestinationPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, true, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_DestinationPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_Destination = m_DestinationPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

    // Do not allow non-scalar arrays
    for (size_t i = 0; i < m_SelectedThresholds.size(); ++i)
    {
      ComparisonInput_t comp = m_SelectedThresholds[i];
      tempPath.update(comp.dataContainerName, comp.attributeMatrixName, comp.attributeArrayName);
      IDataArray::Pointer inputData = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, tempPath);
      if(getErrorCondition() >= 0)
      {
        cDims = inputData->getComponentDimensions();
        int32_t numComp = static_cast<int32_t>(cDims[0]);
        for (int32_t d = 1; d < cDims.size(); d++)
        {
          numComp *= cDims[d];
        }
        if (numComp > 1)
        {
          QString ss = QObject::tr("Selected array '%1' is not a scalar array").arg(m_SelectedThresholds[i].attributeArrayName);
          setErrorCondition(-11003);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects::preflight()
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
void MultiThresholdObjects::execute()
{
  int32_t err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  // Get the first comparison object
  ComparisonInput_t& comp_0 = m_SelectedThresholds[0];
  // Get the names of the Data Container and AttributeMatrix for later
  QString dcName = comp_0.dataContainerName;
  QString amName = comp_0.attributeMatrixName;

  DataContainerArray::Pointer dca = getDataContainerArray();
  DataContainer::Pointer m = dca->getDataContainer(dcName);

  // Prime our output array with the result of the first comparison
  {
    ThresholdFilterHelper filter(static_cast<DREAM3D::Comparison::Enumeration>(comp_0.compOperator), comp_0.compValue, m_DestinationPtr.lock().get());
    // Run the first threshold and store the results in our output array
    err = filter.execute(m->getAttributeMatrix(amName)->getAttributeArray(comp_0.attributeArrayName).get(), m_DestinationPtr.lock().get());
    if (err < 0)
    {
      DataArrayPath tempPath(comp_0.dataContainerName, comp_0.attributeMatrixName, comp_0.attributeArrayName);
      QString ss = QObject::tr("Error Executing threshold filter on first array. The path is %1").arg(tempPath.serialize());
      setErrorCondition(-13001);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }

  if (m_SelectedThresholds.size() > 1)
  {
    // Get the total number of tuples, create and initialize an array to use for these results
    int64_t totalTuples = static_cast<int64_t>(m->getAttributeMatrix(amName)->getNumTuples());
    BoolArrayType::Pointer currentArrayPtr = BoolArrayType::CreateArray(totalTuples, "_INTERNAL_USE_ONLY_TEMP");

    // Loop on the remaining Comparison objects updating our final result array as we go
    for (int32_t i = 1; i < m_SelectedThresholds.size(); ++i)
    {
      // Initialize the array to false
      currentArrayPtr->initializeWithZeros();
      // Get the pointer to the front of the array. Raw Pointers = fast access = NO Bounds Checking!!!
      bool* currentArray = currentArrayPtr->getPointer(0);

      ComparisonInput_t& compRef = m_SelectedThresholds[i];

      ThresholdFilterHelper filter(static_cast<DREAM3D::Comparison::Enumeration>(compRef.compOperator), compRef.compValue, currentArrayPtr.get());

      err = filter.execute(m->getAttributeMatrix(amName)->getAttributeArray(compRef.attributeArrayName).get(), currentArrayPtr.get());
      if (err < 0)
      {
        DataArrayPath tempPath(compRef.dataContainerName, compRef.attributeMatrixName, compRef.attributeArrayName);
        QString ss = QObject::tr("Error Executing threshold filter on array. The path is %1").arg(tempPath.serialize());
        setErrorCondition(-13002);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
      for (int64_t p = 0; p < totalTuples; ++p)
      {
        if (m_Destination[p] == false || currentArray[p] == false)
        {
          m_Destination[p] = false;
        }
      }
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MultiThresholdObjects::newFilterInstance(bool copyFilterParameters)
{
  MultiThresholdObjects::Pointer filter = MultiThresholdObjects::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiThresholdObjects::getCompiledLibraryName()
{
  return ProcessingConstants::ProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiThresholdObjects::getBrandingString()
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiThresholdObjects::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiThresholdObjects::getGroupName()
{ return DREAM3D::FilterGroups::ProcessingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiThresholdObjects::getSubGroupName()
{ return DREAM3D::FilterSubGroups::ThresholdFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiThresholdObjects::getHumanLabel()
{ return "Threshold Objects"; }
