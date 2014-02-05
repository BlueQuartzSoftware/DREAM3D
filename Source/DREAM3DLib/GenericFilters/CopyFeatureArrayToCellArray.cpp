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

#include "CopyFeatureArrayToCellArray.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CopyFeatureArrayToCellArray::CopyFeatureArrayToCellArray() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_SelectedFeatureArrayName(""),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CopyFeatureArrayToCellArray::~CopyFeatureArrayToCellArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFeatureArrayToCellArray::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Feature Array Name");
    parameter->setPropertyName("SelectedFeatureArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFeatureArrayToCellArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setSelectedFeatureArrayName( reader->readString( "SelectedFeatureArrayName", getSelectedFeatureArrayName() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CopyFeatureArrayToCellArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedFeatureArrayName", getSelectedFeatureArrayName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFeatureArrayToCellArray::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureIdsArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_SelectedFeatureArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition());
  }
  if(cellFeatureAttrMat->doesAttributeArrayExist(m_SelectedFeatureArrayName) == false)
  {
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), "The array selected does not exist in the DataContainer selected.", getErrorCondition());
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFeatureArrayToCellArray::preflight()
{
  dataCheck();
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
IDataArray::Pointer copyData(IDataArray::Pointer inputData, int64_t voxels, int32_t* featureIds)
{
  QString cellArrayName = inputData->getName();

  DataArray<T>* feature = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == feature)
  {
    return IDataArray::NullPointer();
  }
  QVector<size_t> dims = inputData->getComponentDimensions();
  typename DataArray<T>::Pointer cell = DataArray<T>::CreateArray(voxels, dims, cellArrayName);

  T* fPtr = feature->getPointer(0);
  T* cPtr = cell->getPointer(0);

  int32_t numComp = feature->getNumberOfComponents();
  int32_t featureIdx = 0;

  for(int64_t i = 0; i < voxels; ++i)
  {
    // Get the feature id (or what ever the user has selected as their "Feature" identifier
    featureIdx = featureIds[i];
    // Now get the pointer to the start of the tuple for the Feature Array at the given Feature Id Index value
    T* fSourcePtr = fPtr + (numComp * featureIdx);
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
void CopyFeatureArrayToCellArray::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();

  QString ss;

  IDataArray::Pointer inputData = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(m_SelectedFeatureArrayName);

  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedFeatureArrayName);
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QString dType = inputData->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();
  if (dType.compare("int8_t") == 0)
  {
    p = copyData<int8_t>(inputData, totalPoints, m_FeatureIds);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    p = copyData<uint8_t>(inputData, totalPoints, m_FeatureIds);
  }
  else if (dType.compare("int16_t") == 0)
  {
    p = copyData<int16_t>(inputData, totalPoints, m_FeatureIds);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    p = copyData<uint16_t>(inputData, totalPoints, m_FeatureIds);
  }
  else if (dType.compare("int32_t") == 0)
  {
    p = copyData<int32_t>(inputData, totalPoints, m_FeatureIds);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    p = copyData<uint32_t>(inputData, totalPoints, m_FeatureIds);
  }
  else if (dType.compare("int64_t") == 0)
  {
    p = copyData<int64_t>(inputData, totalPoints, m_FeatureIds);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    p = copyData<uint64_t>(inputData, totalPoints, m_FeatureIds);
  }
  else if (dType.compare("float") == 0)
  {
    p = copyData<float>(inputData, totalPoints, m_FeatureIds);
  }
  else if (dType.compare("double") == 0)
  {
    p = copyData<double>(inputData, totalPoints, m_FeatureIds);
  }
  else if (dType.compare("bool") == 0)
  {
    p = copyData<bool>(inputData, totalPoints, m_FeatureIds);
  }

  if (p.get() != NULL)
  {
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(p->getName(), p);
  }
  else
  {
    setErrorCondition(-14000);
    notifyErrorMessage(getHumanLabel(), "There was an issue creating a cell data array from a feature data array. Either the types did not match or we could not cast the input array to the expected output type", getErrorCondition());
  }
  notifyStatusMessage(getHumanLabel(), "Complete");
}

