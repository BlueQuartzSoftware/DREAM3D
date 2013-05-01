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

#include "LinkFieldMapToCellArray.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkFieldMapToCellArray::LinkFieldMapToCellArray() :
  AbstractFilter(),
  m_SelectedCellDataArrayName("")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkFieldMapToCellArray::~LinkFieldMapToCellArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkFieldMapToCellArray::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Cell Array Name");
    option->setPropertyName("SelectedCellDataArrayName");
    option->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkFieldMapToCellArray::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("SelectedCellDataArrayName", getSelectedCellDataArrayName() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkFieldMapToCellArray::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  IDataArray::Pointer data = m->getCellData(m_SelectedCellDataArrayName);
  if (NULL == data.get())
  {
    ss.str("");
    ss << "Selected array '" << m_SelectedCellDataArrayName << "' does not exist in the Voxel Data Container. Was it spelled correctly?";
    setErrorCondition(-11001);
    addErrorMessage(getHumanLabel(),ss.str(),getErrorCondition());
    notifyErrorMessage(ss.str(), getErrorCondition());
    return;
  }

  std::string dType = data->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();
  if (dType.compare("int32_t") == 0)
  {
    DataArray<int32_t>* field = DataArray<int32_t>::SafePointerDownCast(data.get());
    m_SelectedCellData = field->GetPointer(0);
  }
  else
  {
    ss.str("");
    ss << "Selected array '" << m_SelectedCellDataArrayName << "' is not an Integer array. Is this the array you want to use?";
    setErrorCondition(-11001);
    addErrorMessage(getHumanLabel(),ss.str(),getErrorCondition());
    notifyErrorMessage(ss.str(), getErrorCondition());
    return;
  }

  m->clearFieldData();
  BoolArrayType::Pointer active = BoolArrayType::CreateArray(fields, 1, DREAM3D::FieldData::Active);
  // bool* mActive = m_Active->GetPointer(0);
  m->addFieldData(DREAM3D::FieldData::Active, active);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkFieldMapToCellArray::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkFieldMapToCellArray::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  int64_t voxels = m->getTotalPoints();
  int64_t fields = m->getNumFieldTuples();
  dataCheck(false, voxels, fields, m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }
  //int err = 0;
  std::stringstream ss;

  m->clearFieldData();

  int maxIndex = 0;
  std::vector<bool> active;
  for(int64_t i=0;i<voxels;i++)
  {
    int index = m_SelectedCellData[i];
    if((index+1) > maxIndex)
    {
      active.resize(index+1);
      active[index] = true;
      maxIndex = index+1;
    }
  }

  BoolArrayType::Pointer m_Active = BoolArrayType::CreateArray(maxIndex, 1, DREAM3D::FieldData::Active);
  bool* mActive = m_Active->GetPointer(0);
  for(int i=0;i<maxIndex;i++)
  {
    mActive[i] = active[i];
  }
  m->addFieldData(DREAM3D::FieldData::Active, m_Active);

  notifyStatusMessage("Complete");
}

