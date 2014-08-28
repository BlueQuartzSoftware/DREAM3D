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
  m_SelectedFeatureArrayPath("", "", ""),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
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
  parameters.push_back(FilterParameter::New("Array to Copy to Cell Data", "SelectedFeatureArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedFeatureArrayPath(), false));
  parameters.push_back(FilterParameter::New("Cell Feature Ids", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Created Data", "", FilterParameterWidgetType::SeparatorWidget, "", false));
  parameters.push_back(FilterParameter::New("Created Cell Array Name", "CreatedArrayName", FilterParameterWidgetType::StringWidget, getCreatedArrayName(), false, ""));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFeatureArrayToCellArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setSelectedFeatureArrayPath( reader->readDataArrayPath( "SelectedFeatureArrayPath", getSelectedFeatureArrayPath() ) );
  setCreatedArrayName(reader->readString("CreatedArrayName", getCreatedArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CopyFeatureArrayToCellArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
      DREAM3D_FILTER_WRITE_PARAMETER(SelectedFeatureArrayPath)
      DREAM3D_FILTER_WRITE_PARAMETER(CreatedArrayName)
      writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFeatureArrayToCellArray::dataCheck()
{
  setErrorCondition(0);

  if(getSelectedFeatureArrayPath().isValid() == false)
  {
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), "Please select a Feature Attribute Array to copy into a Cell Attribute Matrix.", getErrorCondition());
    return;
  }

  if(getCreatedArrayName().isEmpty() == true)
  {
    setErrorCondition(-11002);
    notifyErrorMessage(getHumanLabel(), "The created array name was empty. You must set a name for the new Attribute Array.", getErrorCondition());
    return;
  }

  if(getFeatureIdsArrayPath().isValid() == false)
  {
    setErrorCondition(-11003);
    notifyErrorMessage(getHumanLabel(), "Please select the cell level array that is the Feature IDs.", getErrorCondition());
    return;
  }

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getSelectedFeatureArrayPath().getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }

  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getFeatureIdsArrayPath().getAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }

  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getSelectedFeatureArrayPath().getAttributeMatrixName(), -302);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  else
  {
    return;
  }

  IDataArray::Pointer featureArrayPtr = cellFeatureAttrMat->getAttributeArray(getSelectedFeatureArrayPath().getDataArrayName());

  if(featureArrayPtr.get() != NULL)
  {
    DataArray<int32_t>::Pointer features = m_FeatureIdsPtr.lock(); // Purely for convenience


    if(cellAttrMat->doesAttributeArrayExist(getCreatedArrayName()) == true)
    {
      setErrorCondition(-11007);
      notifyErrorMessage(getHumanLabel(), "The cell level array already exists with the given name. Please set a different name.", getErrorCondition());
      return;
    }
    IDataArray::Pointer createdArray = featureArrayPtr->createNewArray(features->getNumberOfTuples(), featureArrayPtr->getComponentDimensions(), getCreatedArrayName(), false);
    cellAttrMat->addAttributeArray(createdArray->getName(), createdArray);

  }
  else
  {

    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), "The cell level array does not exist in the DataContainer and AttributeMatrix selected.", getErrorCondition());
    return;
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFeatureArrayToCellArray::preflight()
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

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_SelectedFeatureArrayPath.getDataContainerName());
  int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  QString ss;

  IDataArray::Pointer inputData = m->getAttributeMatrix(m_SelectedFeatureArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedFeatureArrayPath.getDataArrayName());

  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedFeatureArrayPath.getDataArrayName());
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
    m->getAttributeMatrix(getFeatureIdsArrayPath().getAttributeMatrixName())->addAttributeArray(p->getName(), p);
  }
  else
  {
    setErrorCondition(-14000);
    notifyErrorMessage(getHumanLabel(), "There was an issue creating a cell data array from a feature data array. Either the types did not match or we could not cast the input array to the expected output type", getErrorCondition());
  }
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CopyFeatureArrayToCellArray::newFilterInstance(bool copyFilterParameters)
{
  CopyFeatureArrayToCellArray::Pointer filter = CopyFeatureArrayToCellArray::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyFeatureArrayToCellArray::getCompiledLibraryName()
{ return Core::CoreBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyFeatureArrayToCellArray::getGroupName()
{ return DREAM3D::FilterGroups::CoreFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyFeatureArrayToCellArray::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MemoryManagementFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyFeatureArrayToCellArray::getHumanLabel()
{ return "Create Cell Array From Feature Array"; }

