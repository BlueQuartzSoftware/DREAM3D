/* ============================================================================
*
*
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "FindFieldAverage.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFieldAverage::FindFieldAverage() :
  AbstractFilter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_SelectedVoxelArrayName(""),
  m_GrainIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFieldAverage::~FindFieldAverage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFieldAverage::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Voxel Array Name");
    option->setPropertyName("SelectedVoxelArrayName");
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
void FindFieldAverage::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedVoxelArrayName( reader->readValue( "SelectedVoxelArrayName", getSelectedVoxelArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindFieldAverage::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedVoxelArrayName", getSelectedVoxelArrayName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFieldAverage::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA( m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1)

  if(m_SelectedVoxelArrayName.empty() == true)
  {
    setErrorCondition(-11000);
    addErrorMessage(getHumanLabel(), "An array from the Voxel Data Container must be selected.", getErrorCondition());
  } else if(preflight)
  {
    IDataArray::Pointer inputData = m->getCellData(m_SelectedVoxelArrayName);
    if (NULL == inputData.get())
    {
      ss.str("");
      ss << "Selected array '" << m_SelectedVoxelArrayName << "' does not exist in the Surface Mesh Data Container. Was it spelled correctly?";
      setErrorCondition(-11001);
      notifyErrorMessage(ss.str(), getErrorCondition());
      return;
    }

    int numberOfComponents = inputData->GetNumberOfComponents();
    std::string dType = inputData->getTypeAsString();
    IDataArray::Pointer p = IDataArray::NullPointer();
    if (dType.compare("int8_t") == 0)
    {
      p = Int8ArrayType::CreateArray(1, numberOfComponents, m_SelectedVoxelArrayName);
    }
    else if (dType.compare("uint8_t") == 0)
    {
      p = UInt8ArrayType::CreateArray(1, numberOfComponents, m_SelectedVoxelArrayName);
    }
    else if (dType.compare("int16_t") == 0)
    {
      p = Int16ArrayType::CreateArray(1, numberOfComponents, m_SelectedVoxelArrayName);
    }
    else if (dType.compare("uint16_t") == 0)
    {
      p = UInt16ArrayType::CreateArray(1, numberOfComponents, m_SelectedVoxelArrayName);
    }
    else if (dType.compare("int32_t") == 0)
    {
      p = Int32ArrayType::CreateArray(1, numberOfComponents, m_SelectedVoxelArrayName);
    }
    else if (dType.compare("uint32_t") == 0)
    {
      p = UInt32ArrayType::CreateArray(1, numberOfComponents, m_SelectedVoxelArrayName);
    }
    else if (dType.compare("int64_t") == 0)
    {
      p = Int64ArrayType::CreateArray(1, numberOfComponents, m_SelectedVoxelArrayName);
    }
    else if (dType.compare("uint64_t") == 0)
    {
      p = UInt64ArrayType::CreateArray(1, numberOfComponents, m_SelectedVoxelArrayName);
    }
    else if (dType.compare("float") == 0)
    {
      p = FloatArrayType::CreateArray(1, numberOfComponents, m_SelectedVoxelArrayName);
    }
    else if (dType.compare("double") == 0)
    {
      p = DoubleArrayType::CreateArray(1, numberOfComponents, m_SelectedVoxelArrayName);
    }
    else if (dType.compare("bool") == 0)
    {
      p = BoolArrayType::CreateArray(1, numberOfComponents, m_SelectedVoxelArrayName);
    }
    m->addFieldData(p->GetName(), p);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFieldAverage::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
IDataArray::Pointer copyData(IDataArray::Pointer inputData, int64_t voxels, int64_t grains, int32_t* grainIds)
{

  std::string fieldArrayName = inputData->GetName();

  DataArray<T>* cell = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == cell)
  {
    return IDataArray::NullPointer();
  }
  typename DataArray<T>::Pointer field = DataArray<T>::CreateArray(grains, inputData->GetNumberOfComponents(), fieldArrayName);

  T* fPtr = field->GetPointer(0);
  T* cPtr = cell->GetPointer(0);

  int32_t numComp = field->GetNumberOfComponents();
  int32_t grainIdx = 0;

  int* voxelCount = new int[grains];
  double* tempContainer = new double[grains*numComp];
  for(int i=0; i<grains; i++){voxelCount[i]=0;}
  for(int i=0; i<grains*numComp; i++){tempContainer[i]=0;}

  for(int64_t i = 0; i < voxels; ++i)
  {
    //get grain id of voxel
    int gId = grainIds[i];

    //gffsets for grain / voxel arrays
    int vIndex=i*numComp;
    int fInxed=gId*numComp;

    //add componenets
    for(int j=0; j<numComp; j++)
    {
      tempContainer[fInxed+j]=tempContainer[fInxed+j]+cell->GetValue(vIndex+j);
    }

    //add to grain's voxel count
    voxelCount[gId]=voxelCount[gId]+1;
  }

  //loop over field, normalizing and copying
  for(int i=0; i<grains; i++)
  {
    int index=i*numComp;
    for(int j=0; j<numComp; j++)
    {
      tempContainer[index+j]=tempContainer[index+j]/voxelCount[i];
      field->SetValue(index+j, static_cast<T>(tempContainer[index+j]));
    }
  }

  //cleanup
  delete voxelCount;
  delete tempContainer;

  return field;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFieldAverage::execute()
{
  std::stringstream ss;

  int32_t err =0;
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  int64_t totalPoints = m->getTotalPoints();
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, numgrains, numensembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  IDataArray::Pointer inputData = m->getCellData(m_SelectedVoxelArrayName);
  if (NULL == inputData.get())
  {
    ss.str("");
    ss << "Selected array '" << m_SelectedVoxelArrayName << "' does not exist in the Voxel Data Container. Was it spelled correctly?";
    setErrorCondition(-11001);
    notifyErrorMessage(ss.str(), getErrorCondition());
    return;
  }

  int64_t voxels = m->getTotalPoints();
  int64_t fields = m->getNumFieldTuples();

  std::string dType = inputData->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();

  if (dType.compare("int8_t") == 0)
  {
    p = copyData<int8_t>(inputData, voxels, fields, m_GrainIds);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    p = copyData<uint8_t>(inputData, voxels, fields, m_GrainIds);
  }
  else if (dType.compare("int16_t") == 0)
  {
    p = copyData<int16_t>(inputData, voxels, fields, m_GrainIds);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    p = copyData<uint16_t>(inputData, voxels, fields, m_GrainIds);
  }
  else if (dType.compare("int32_t") == 0)
  {
    p = copyData<int32_t>(inputData, voxels, fields, m_GrainIds);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    p = copyData<uint32_t>(inputData, voxels, fields, m_GrainIds);
  }
  else if (dType.compare("int64_t") == 0)
  {
    p = copyData<int64_t>(inputData, voxels, fields, m_GrainIds);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    p = copyData<uint64_t>(inputData, voxels, fields, m_GrainIds);
  }
  else if (dType.compare("float") == 0)
  {
    p = copyData<float>(inputData, voxels, fields, m_GrainIds);
  }
  else if (dType.compare("double") == 0)
  {
    p = copyData<double>(inputData, voxels, fields, m_GrainIds);
  }
  else if (dType.compare("bool") == 0)
  {
    p = copyData<bool>(inputData, voxels, fields, m_GrainIds);
  }

  if (p.get() != NULL)
  {
    m->addFieldData(p->GetName(), p);

  }
  else
  {
    setErrorCondition(-14000);
    notifyErrorMessage("There was an issue creating a cell data array from a field data array. Either the types did not match or we could not cast the input array to the expected output type", getErrorCondition());
  }

  notifyStatusMessage("Complete");
}
