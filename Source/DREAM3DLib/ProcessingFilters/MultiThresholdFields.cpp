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
#include "MultiThresholdFields.h"

#include <vector>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/Common/ThresholdFilterHelper.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiThresholdFields::MultiThresholdFields() :
  AbstractFilter(),
  m_OutputArrayName(DREAM3D::FieldData::GoodFields)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiThresholdFields::~MultiThresholdFields()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdFields::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Output Array Name");
    parameter->setPropertyName("OutputArrayName");
    parameter->setWidgetType(FilterParameter::StringWidget);
    parameter->setValueType("string");
    parameters.push_back(parameter);
  }

  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Select Arrays to Threshold");
    parameter->setPropertyName("ComparisonInputs");
    parameter->setWidgetType(FilterParameter::FieldArrayComparisonSelectionWidget);
    parameter->setValueType("std::vector<ComparisonInput_t>");
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdFields::writeFilterParameters(AbstractFilterParametersWriter* writer)
{

  int numQFilters = static_cast<int>( m_ComparisonInputs.size() );
  writer->writeValue("NumComparisons",  numQFilters);
  std::stringstream ss;
  for(int i = 0; i < numQFilters; i++)
  {
    ss << "Comparison-" << i;
    writer->writeValue(ss.str(), m_ComparisonInputs[i]);
    ss.str("");
  }
  writer->writeValue("OutputArrayName", getOutputArrayName() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdFields::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  //  for(int i = 0; i < m_ComparisonInputs.size(); ++i)
  //  {
  //    ComparisonInput_t& input = m_ComparisonInputs[i];
  //    std::cout << input.arrayName << "  " << input.compOperator << "  " << input.compValue << std::endl;
  //  }
  if (m_ComparisonInputs.size() == 0)
  {
    setErrorCondition(-12000);
    notifyErrorMessage("You must add at least 1 comparison array.", getErrorCondition());
  }

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Output, ss, bool, BoolArrayType, true, fields, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdFields::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiThresholdFields::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Voxel DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  int64_t nFields = m->getNumFieldTuples();
  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }
  /* Place all your code to execute your filter here. */

  IDataArray::Pointer outputArrayPtr = m->getFieldData(m_OutputArrayName);
  BoolArrayType* outputArray = BoolArrayType::SafeObjectDownCast<IDataArray*, BoolArrayType*>(outputArrayPtr.get());
  if (NULL == outputArray)
  {
    setErrorCondition(-11002);
    notifyErrorMessage("Could not properly cast the output array to a BoolArrayType", getErrorCondition());
    return;
  }
  m_Output = outputArray->GetPointer(0);

  // Prime our output array with the result of the first comparison
  {
    ComparisonInput_t& comp_0 = m_ComparisonInputs[0];

    ThresholdFilterHelper filter(static_cast<DREAM3D::Comparison::Enumeration>(comp_0.compOperator),
                                 comp_0.compValue,
                                 outputArray);

    err = filter.execute(m->getFieldData(comp_0.arrayName).get(), outputArrayPtr.get());

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
    bool* currentArray = currentArrayPtr->GetPointer(0);

    ComparisonInput_t& compRef = m_ComparisonInputs[i];

    ThresholdFilterHelper filter(static_cast<DREAM3D::Comparison::Enumeration>(compRef.compOperator),
                                 compRef.compValue,
                                 currentArrayPtr.get());

    err = filter.execute(m->getFieldData(compRef.arrayName).get(), currentArrayPtr.get());
    if (err < 0)
    {
      setErrorCondition(-13002);
      notifyErrorMessage("Error Executing threshold filter on array", getErrorCondition());
      return;
    }
    for (int64_t p = 0; p < nFields; ++p)
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
