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
#include "MultiThresholdCells.h"

#include <vector>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/Common/ThresholdFilterHelper.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiThresholdCells::MultiThresholdCells() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::HDF5::CellAttributeMatrixName),
  m_OutputArrayName(DREAM3D::CellData::GoodVoxels)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiThresholdCells::~MultiThresholdCells()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdCells::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Output Array Name");
    parameter->setPropertyName("OutputArrayName");
    parameter->setWidgetType(FilterParameter::ChoiceWidget);
    parameter->setValueType("string");
    parameter->setEditable(true);
    QVector<QString> choices;
    choices.push_back(DREAM3D::CellData::GoodVoxels);
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }

  {
    ComparisonFilterParameter::Pointer parameter = ComparisonFilterParameter::New();
    parameter->setHumanLabel("Select Arrays to Threshold");
    parameter->setPropertyName("ComparisonInputs");
    parameter->setWidgetType(FilterParameter::CellArrayComparisonSelectionWidget);
    parameter->setValueType("QVector<ComparisonInput_t>");
    parameter->setShowOperators(true);
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdCells::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int MultiThresholdCells::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
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
void MultiThresholdCells::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  if (m_ComparisonInputs.size() == 0)
  {
    setErrorCondition(-12000);
    notifyErrorMessage("You must add at least 1 comparison array.", getErrorCondition());
  }

  QVector<int> dims(1, 1);
  m_OutputPtr = m->createNonPrereqArray<bool, AbstractFilter>(this, m_CellAttributeMatrixName,  m_OutputArrayName, true, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_OutputPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_Output = m_OutputPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdCells::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    addErrorMessage(getHumanLabel(), "The VolumeDataContainer Object with the specific name " + getDataContainerName() + " was not available.", getErrorCondition());
    return;
  }

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdCells::execute()
{
  int err = 0;

  setErrorCondition(err);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Voxel DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  dataCheck(false, totalPoints, 0, 0);
  if (getErrorCondition() < 0)
  {
    return;
  }
  /* Place all your code to execute your filter here. */

  IDataArray::Pointer outputArrayPtr = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(m_OutputArrayName);
  BoolArrayType* outputArray = BoolArrayType::SafeObjectDownCast<IDataArray*, BoolArrayType*>(outputArrayPtr.get());
  if (NULL == outputArray)
  {
    setErrorCondition(-11002);
    notifyErrorMessage("Could not properly cast the output array to a BoolArrayType", getErrorCondition());
    return;
  }
  m_Output = outputArray->getPointer(0);

  // Prime our output array with the result of the first comparison
  {
    ComparisonInput_t& comp_0 = m_ComparisonInputs[0];

    ThresholdFilterHelper filter(static_cast<DREAM3D::Comparison::Enumeration>(comp_0.compOperator),
                                 comp_0.compValue,
                                 outputArray);

    err = filter.execute(m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(comp_0.arrayName).get(), outputArrayPtr.get());

    if (err < 0)
    {
      setErrorCondition(-13001);
      notifyErrorMessage("Error Executing threshold filter on first array", getErrorCondition());
      return;
    }
  }
  for(size_t i = 1; i < m_ComparisonInputs.size(); ++i)
  {

    BoolArrayType::Pointer currentArrayPtr = BoolArrayType::CreateArray(m->getTotalPoints(), "TEMP");
    currentArrayPtr->initializeWithZeros();
    bool* currentArray = currentArrayPtr->getPointer(0);

    ComparisonInput_t& compRef = m_ComparisonInputs[i];

    ThresholdFilterHelper filter(static_cast<DREAM3D::Comparison::Enumeration>(compRef.compOperator),
                                 compRef.compValue,
                                 currentArrayPtr.get());

    err = filter.execute(m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(compRef.arrayName).get(), currentArrayPtr.get());
    if (err < 0)
    {
      setErrorCondition(-13002);
      notifyErrorMessage("Error Executing threshold filter on array", getErrorCondition());
      return;
    }
    for (int64_t p = 0; p < totalPoints; ++p)
    {
      if(m_Output[p] == false || currentArray[p] == false)
      {
        m_Output[p] = false;
      }
    }

  }



  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
