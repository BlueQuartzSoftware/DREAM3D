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
#include "MultiThresholdObjects.h"

#include <vector>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/Common/ThresholdFilterHelper.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiThresholdObjects::MultiThresholdObjects() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_AttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_OutputArrayName(DREAM3D::CellData::GoodVoxels),
  m_DestinationArrayName(""),
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
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Output Array Name");
    parameter->setPropertyName("OutputArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::StringWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }

  {
    ComparisonFilterParameter::Pointer parameter = ComparisonFilterParameter::New();
    parameter->setHumanLabel("Select Arrays to Threshold");
    parameter->setPropertyName("ComparisonInputs");
    parameter->setWidgetType(FilterParameterWidgetType::ComparisonSelectionWidget);
    parameter->setValueType("QVector<ComparisonInput_t>");
    parameter->setShowOperators(true);
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setOutputArrayName(reader->readString( "OutputArrayName", getOutputArrayName()));
  setComparisonInputs(reader->readComparisonInputs("ComparisonInputs", getComparisonInputs()));
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MultiThresholdObjects::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputArrayName", getOutputArrayName());
  writer->writeValue("ComparisonInputs", getComparisonInputs());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer attrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == attrMat.get() ) { return; }

  if (m_ComparisonInputs.size() == 0)
  {
    setErrorCondition(-12000);
    notifyErrorMessage(getHumanLabel(), "You must add at least 1 comparison array.", getErrorCondition());
  }

  QVector<size_t> dims(1, 1);
  m_DestinationPtr = attrMat->createNonPrereqArray<DataArray<bool>, AbstractFilter, bool>(this, m_OutputArrayName, true, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_DestinationPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Destination = m_DestinationPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdObjects::execute()
{
  int err = 0;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  int64_t totalTuples = m->getAttributeMatrix(getAttributeMatrixName())->getNumTuples();

  // Prime our output array with the result of the first comparison
  {
    ComparisonInput_t& comp_0 = m_ComparisonInputs[0];

    ThresholdFilterHelper filter(static_cast<DREAM3D::Comparison::Enumeration>(comp_0.compOperator), comp_0.compValue, m_DestinationPtr.lock().get());

    err = filter.execute(m->getAttributeMatrix(getAttributeMatrixName())->getAttributeArray(comp_0.arrayName).get(), m_DestinationPtr.lock().get());
    if (err < 0)
    {
      setErrorCondition(-13001);
      notifyErrorMessage(getHumanLabel(), "Error Executing threshold filter on first array", getErrorCondition());
      return;
    }
  }
  for(size_t i = 1; i < m_ComparisonInputs.size(); ++i)
  {
    BoolArrayType::Pointer currentArrayPtr = BoolArrayType::CreateArray(totalTuples, "TEMP");
    currentArrayPtr->initializeWithZeros();
    bool* currentArray = currentArrayPtr->getPointer(0);

    ComparisonInput_t& compRef = m_ComparisonInputs[i];

    ThresholdFilterHelper filter(static_cast<DREAM3D::Comparison::Enumeration>(compRef.compOperator), compRef.compValue, currentArrayPtr.get());

    err = filter.execute(m->getAttributeMatrix(getAttributeMatrixName())->getAttributeArray(compRef.arrayName).get(), currentArrayPtr.get());
    if (err < 0)
    {
      setErrorCondition(-13002);
      notifyErrorMessage(getHumanLabel(), "Error Executing threshold filter on array", getErrorCondition());
      return;
    }
    for (int64_t p = 0; p < totalTuples; ++p)
    {
      if(m_Destination[p] == false || currentArray[p] == false)
      {
        m_Destination[p] = false;
      }
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

