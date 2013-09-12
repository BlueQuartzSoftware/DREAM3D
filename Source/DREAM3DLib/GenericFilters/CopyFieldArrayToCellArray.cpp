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

#include "CopyFieldArrayToCellArray.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CopyFieldArrayToCellArray::CopyFieldArrayToCellArray() :
  AbstractFilter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_SelectedFieldArrayName(""),
  m_GrainIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CopyFieldArrayToCellArray::~CopyFieldArrayToCellArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFieldArrayToCellArray::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Field Array Name");
    option->setPropertyName("SelectedFieldArrayName");
    option->setWidgetType(FilterParameter::VolumeFieldArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFieldArrayToCellArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setSelectedFieldArrayName( reader->readValue( "SelectedFieldArrayName", getSelectedFieldArrayName() ) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CopyFieldArrayToCellArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedFieldArrayName", getSelectedFieldArrayName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFieldArrayToCellArray::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  QString ss;
  VolumeDataContainer* m = getVolumeDataContainer();

  GET_PREREQ_DATA( m, DREAM3D, CellData, GrainIds, -301, int32_t, Int32ArrayType, voxels, 1)

  if(m_SelectedFieldArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    addErrorMessage(getHumanLabel(), "An array from the Voxel Data Container must be selected.", getErrorCondition());
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFieldArrayToCellArray::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
IDataArray::Pointer copyData(IDataArray::Pointer inputData, int64_t voxels, int32_t* grainIds)
{


  QString cellArrayName = inputData->GetName();

  DataArray<T>* field = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == field)
  {
    return IDataArray::NullPointer();
  }
  typename DataArray<T>::Pointer cell = DataArray<T>::CreateArray(voxels, inputData->GetNumberOfComponents(), cellArrayName);

  T* fPtr = field->GetPointer(0);
  T* cPtr = cell->GetPointer(0);

  int32_t numComp = field->GetNumberOfComponents();
  int32_t grainIdx = 0;

  for(int64_t i = 0; i < voxels; ++i)
  {
    // Get the grain id (or what ever the user has selected as their "Grain" identifier
    grainIdx = grainIds[i];
    // Now get the pointer to the start of the tuple for the Field Array at the give Grain Id Index value
    T* fSourcePtr = fPtr + (numComp * grainIdx);
    // Now get the pointer to the start of the tuple for the Cell Array at the proper index
    T* cDestPtr = cPtr + (numComp * i);

    // Now just raw copy the bytes from the source to the destination
    ::memcpy(cDestPtr, fSourcePtr, sizeof(T) * numComp);
  }
  return cell;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFieldArrayToCellArray::execute()
{
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  int64_t voxels = m->getTotalPoints();
  int64_t fields = m->getNumCellFieldTuples();
  dataCheck(false, voxels, fields, m->getNumCellEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }
  //int err = 0;
  QString ss;

  IDataArray::Pointer inputData = m->getCellFieldData(m_SelectedFieldArrayName);
  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedFieldArrayName);
    setErrorCondition(-11001);
    notifyErrorMessage(ss, getErrorCondition());
    return;
  }

  QString dType = inputData->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();
  if (dType.compare("int8_t") == 0)
  {
    p = copyData<int8_t>(inputData, voxels, m_GrainIds);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    p = copyData<uint8_t>(inputData, voxels, m_GrainIds);
  }
  else if (dType.compare("int16_t") == 0)
  {
    p = copyData<int16_t>(inputData, voxels, m_GrainIds);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    p = copyData<uint16_t>(inputData, voxels, m_GrainIds);
  }
  else if (dType.compare("int32_t") == 0)
  {
    p = copyData<int32_t>(inputData, voxels, m_GrainIds);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    p = copyData<uint32_t>(inputData, voxels, m_GrainIds);
  }
  else if (dType.compare("int64_t") == 0)
  {
    p = copyData<int64_t>(inputData, voxels, m_GrainIds);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    p = copyData<uint64_t>(inputData, voxels, m_GrainIds);
  }
  else if (dType.compare("float") == 0)
  {
    p = copyData<float>(inputData, voxels, m_GrainIds);
  }
  else if (dType.compare("double") == 0)
  {
    p = copyData<double>(inputData, voxels, m_GrainIds);
  }
  else if (dType.compare("bool") == 0)
  {
    p = copyData<bool>(inputData, voxels, m_GrainIds);
  }


  if (p.get() != NULL)
  {
    m->addCellData(p->GetName(), p);

  }
  else
  {
    setErrorCondition(-14000);
    notifyErrorMessage("There was an issue creating a cell data array from a field data array. Either the types did not match or we could not cast the input array to the expected output type", getErrorCondition());
  }
  notifyStatusMessage("Complete");
}

