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

#include "CreateFeatureArrayFromCellArray.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateFeatureArrayFromCellArray::CreateFeatureArrayFromCellArray() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_SelectedCellArrayName(""),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateFeatureArrayFromCellArray::~CreateFeatureArrayFromCellArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFeatureArrayFromCellArray::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Cell Array Name");
    option->setPropertyName("SelectedCellArrayName");
    option->setWidgetType(FilterParameter::VolumeCellArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFeatureArrayFromCellArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setSelectedCellArrayName( reader->readString( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CreateFeatureArrayFromCellArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFeatureArrayFromCellArray::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureIdsArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_SelectedCellArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition());
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFeatureArrayFromCellArray::preflight()
{
  dataCheck();
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
IDataArray::Pointer copyCellData(IDataArray::Pointer inputData, int64_t features, int32_t* featureIds)
{
  QString featureArrayName = inputData->getName();

  DataArray<T>* cell = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == cell)
  {
    return IDataArray::NullPointer();
  }
  QVector<size_t> dims(1, inputData->getNumberOfComponents());
  typename DataArray<T>::Pointer feature = DataArray<T>::CreateArray(features, dims, featureArrayName);

  T* fPtr = feature->getPointer(0);
  T* cPtr = cell->getPointer(0);

  int32_t numComp = cell->getNumberOfComponents();
  int32_t featureIdx = 0;

  int64_t cells = inputData->getNumberOfTuples();
  for(int64_t i = 0; i < cells; ++i)
  {
    // Get the feature id (or what ever the user has selected as their "Feature" identifier
    featureIdx = featureIds[i];
    // Now get the pointer to the start of the tuple for the Cell Array at the given Feature Id Index value
    T* cSourcePtr = cPtr + (numComp * i);
    // Now get the pointer to the start of the tuple for the Feature Array at the proper index
    T* fDestPtr = fPtr + (numComp * featureIdx);

    // Now just raw copy the bytes from the source to the destination
    ::memcpy(fDestPtr, cSourcePtr, sizeof(T) * numComp);
  }
  return feature;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFeatureArrayFromCellArray::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  int64_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  IDataArray::Pointer inputData = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(m_SelectedCellArrayName);
  if (NULL == inputData.get())
  {
    QString ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedCellArrayName);
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QString dType = inputData->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();
  if (dType.compare("int8_t") == 0)
  {
    p = copyCellData<int8_t>(inputData, totalFeatures, m_FeatureIds);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    p = copyCellData<uint8_t>(inputData, totalFeatures, m_FeatureIds);
  }
  else if (dType.compare("int16_t") == 0)
  {
    p = copyCellData<int16_t>(inputData, totalFeatures, m_FeatureIds);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    p = copyCellData<uint16_t>(inputData, totalFeatures, m_FeatureIds);
  }
  else if (dType.compare("int32_t") == 0)
  {
    p = copyCellData<int32_t>(inputData, totalFeatures, m_FeatureIds);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    p = copyCellData<uint32_t>(inputData, totalFeatures, m_FeatureIds);
  }
  else if (dType.compare("int64_t") == 0)
  {
    p = copyCellData<int64_t>(inputData, totalFeatures, m_FeatureIds);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    p = copyCellData<uint64_t>(inputData, totalFeatures, m_FeatureIds);
  }
  else if (dType.compare("float") == 0)
  {
    p = copyCellData<float>(inputData, totalFeatures, m_FeatureIds);
  }
  else if (dType.compare("double") == 0)
  {
    p = copyCellData<double>(inputData, totalFeatures, m_FeatureIds);
  }
  else if (dType.compare("bool") == 0)
  {
    p = copyCellData<bool>(inputData, totalFeatures, m_FeatureIds);
  }


  if (p.get() != NULL)
  {
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(p->getName(), p);
  }
  else
  {
    setErrorCondition(-14000);
    PipelineMessage em(getHumanLabel(), "There was an issue creating a feature data array from a cell data array. Either the types did not match or we could not cast the input array to the expected output type", getErrorCondition(), PipelineMessage::Error);
  }
  notifyStatusMessage(getHumanLabel(), "Complete");
}

