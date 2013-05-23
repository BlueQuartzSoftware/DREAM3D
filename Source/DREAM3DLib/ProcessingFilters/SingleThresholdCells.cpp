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
#include "SingleThresholdCells.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/ThresholdFilterHelper.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SingleThresholdCells::SingleThresholdCells():
  AbstractFilter(),
  m_SelectedCellArrayName(""),
  m_OutputArrayName(DREAM3D::CellData::GoodVoxels),
  m_Output(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SingleThresholdCells::~SingleThresholdCells()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleThresholdCells::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input Cell Array Name");
    option->setPropertyName("SelectedCellArrayName");
    option->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    parameters.push_back(option);
  }
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Comparison Operator");
    option->setPropertyName("ComparisonOperator");
    option->setWidgetType(FilterParameter::ChoiceWidget);
    option->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back(DREAM3D::Comparison::Strings::LessThan);
    choices.push_back(DREAM3D::Comparison::Strings::GreaterThan);
    choices.push_back(DREAM3D::Comparison::Strings::Equal);
    option->setChoices(choices);
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Value");
    option->setPropertyName("ComparisonValue");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("double");
    parameters.push_back(option);
  }
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Output Array Name");
    parameter->setPropertyName("OutputArrayName");
    parameter->setWidgetType(FilterParameter::ChoiceWidget);
    parameter->setValueType("string");
    parameter->setEditable(true);
    std::vector<std::string> choices;
    choices.push_back(DREAM3D::CellData::GoodVoxels);
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleThresholdCells::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->writeValue("ComparisonOperator", getComparisonOperator() );
  writer->writeValue("ComparisonValue", getComparisonValue() );
  writer->writeValue("OutputArrayName", getOutputArrayName() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleThresholdCells::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Output, ss, bool, BoolArrayType, true, voxels, 1)

  if(m_SelectedCellArrayName.empty() == true)
  {
    setErrorCondition(-11000);
    notifyErrorMessage("An array from the Voxel Data Container must be selected.", getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleThresholdCells::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleThresholdCells::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }
  //int err = 0;
  std::stringstream ss;

  IDataArray::Pointer inputData = m->getCellData(m_SelectedCellArrayName);
  if (NULL == inputData.get())
  {
    ss.str("");
    ss << "Selected array '" << m_SelectedCellArrayName << "' does not exist in the Voxel Data Container. Was it spelled correctly?";
    setErrorCondition(-11001);
    notifyErrorMessage(ss.str(), getErrorCondition());
    return;
  }

  IDataArray::Pointer goodVoxelsPtr = m->getCellData(m_OutputArrayName);
  BoolArrayType* goodVoxels = BoolArrayType::SafeObjectDownCast<IDataArray*, BoolArrayType*>(goodVoxelsPtr.get());
  if (NULL == goodVoxels)
  {
    setErrorCondition(-11002);
    notifyErrorMessage("Could not properly cast the output array to a BoolArrayType", getErrorCondition());
    return;
  }


  ThresholdFilterHelper filter(static_cast<DREAM3D::Comparison::Enumeration>(m_ComparisonOperator), m_ComparisonValue, goodVoxels);

  filter.execute(inputData.get(), goodVoxelsPtr.get());


  m->addCellData(goodVoxelsPtr->GetName(), goodVoxelsPtr);
  notifyStatusMessage("Complete");
}

