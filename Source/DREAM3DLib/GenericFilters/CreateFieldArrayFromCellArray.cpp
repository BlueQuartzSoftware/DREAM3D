#include "CreateFieldArrayFromCellArray.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateFieldArrayFromCellArray::CreateFieldArrayFromCellArray() :
  AbstractFilter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_SelectedCellArrayName(""),
  m_GrainIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateFieldArrayFromCellArray::~CreateFieldArrayFromCellArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFieldArrayFromCellArray::setupFilterParameters()
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
void CreateFieldArrayFromCellArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setSelectedCellArrayName( reader->readValue( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CreateFieldArrayFromCellArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFieldArrayFromCellArray::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  GET_PREREQ_DATA( m, DREAM3D, CellData, GrainIds, -301, int32_t, Int32ArrayType, voxels, 1)

  if(m_SelectedCellArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    addErrorMessage(getHumanLabel(), "An array from the Voxel Data Container must be selected.", getErrorCondition());
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFieldArrayFromCellArray::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
IDataArray::Pointer copyCellData(IDataArray::Pointer inputData, int64_t fields, int32_t* grainIds)
{


  QString fieldArrayName = inputData->GetName();

  DataArray<T>* cell = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == cell)
  {
    return IDataArray::NullPointer();
  }
  typename DataArray<T>::Pointer field = DataArray<T>::CreateArray(fields, inputData->GetNumberOfComponents(), fieldArrayName);

  T* fPtr = field->getPointer(0);
  T* cPtr = cell->getPointer(0);

  int32_t numComp = cell->GetNumberOfComponents();
  int32_t grainIdx = 0;

  int64_t cells = inputData->getNumberOfTuples();
  for(int64_t i = 0; i < cells; ++i)
  {
    // Get the grain id (or what ever the user has selected as their "Grain" identifier
    grainIdx = grainIds[i];
    // Now get the pointer to the start of the tuple for the Cell Array at the given Grain Id Index value
    T* cSourcePtr = cPtr + (numComp * i);
    // Now get the pointer to the start of the tuple for the Field Array at the proper index
    T* fDestPtr = fPtr + (numComp * grainIdx);

    // Now just raw copy the bytes from the source to the destination
    ::memcpy(fDestPtr, cSourcePtr, sizeof(T) * numComp);
  }
  return field;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFieldArrayFromCellArray::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
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

  IDataArray::Pointer inputData = m->getCellData(m_SelectedCellArrayName);
  if (NULL == inputData.get())
  {

    QString ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedCellArrayName);
    setErrorCondition(-11001);
    notifyErrorMessage(ss, getErrorCondition());
    return;
  }

  QString dType = inputData->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();
  if (dType.compare("int8_t") == 0)
  {
    p = copyCellData<int8_t>(inputData, fields, m_GrainIds);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    p = copyCellData<uint8_t>(inputData, fields, m_GrainIds);
  }
  else if (dType.compare("int16_t") == 0)
  {
    p = copyCellData<int16_t>(inputData, fields, m_GrainIds);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    p = copyCellData<uint16_t>(inputData, fields, m_GrainIds);
  }
  else if (dType.compare("int32_t") == 0)
  {
    p = copyCellData<int32_t>(inputData, fields, m_GrainIds);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    p = copyCellData<uint32_t>(inputData, fields, m_GrainIds);
  }
  else if (dType.compare("int64_t") == 0)
  {
    p = copyCellData<int64_t>(inputData, fields, m_GrainIds);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    p = copyCellData<uint64_t>(inputData, fields, m_GrainIds);
  }
  else if (dType.compare("float") == 0)
  {
    p = copyCellData<float>(inputData, fields, m_GrainIds);
  }
  else if (dType.compare("double") == 0)
  {
    p = copyCellData<double>(inputData, fields, m_GrainIds);
  }
  else if (dType.compare("bool") == 0)
  {
    p = copyCellData<bool>(inputData, fields, m_GrainIds);
  }


  if (p.get() != NULL)
  {
    m->addCellFieldData(p->GetName(), p);

  }
  else
  {
    setErrorCondition(-14000);
    notifyErrorMessage("There was an issue creating a field data array from a cell data array. Either the types did not match or we could not cast the input array to the expected output type", getErrorCondition());
  }
  notifyStatusMessage("Complete");
}

