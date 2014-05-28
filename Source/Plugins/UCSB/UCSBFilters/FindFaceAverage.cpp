/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "FindFaceAverage.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFaceAverage::FindFaceAverage() :
  SurfaceMeshFilter(),
  m_SurfaceMeshTriangleAreasArrayName(DREAM3D::FaceData::SurfaceMeshFaceAreas),
  m_SurfaceMeshGrainFaceIdArrayName(DREAM3D::FaceData::SurfaceMeshGrainFaceId),
  m_SurfaceMeshTriangleAreas(NULL),
  m_SurfaceMeshGrainFaceId(NULL),
  m_SelectedFaceArrayName(""),
  m_AverageMethod(1)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFaceAverage::~FindFaceAverage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFaceAverage::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Face Array Name");
    option->setPropertyName("SelectedFaceArrayName");
    option->setWidgetType(FilterParameter::SurfaceMeshFaceArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    parameters.push_back(option);
  }
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Average Type");
    parameter->setPropertyName("AverageMethod");
    parameter->setWidgetType(FilterParameter::ChoiceWidget);
    parameter->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("Number Average");
    choices.push_back("Area Weighted Average");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFaceAverage::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedFaceArrayName( reader->readValue( "SelectedFaceArrayName", getSelectedFaceArrayName() ) );
  setAverageMethod( reader->readValue( "AverageMethod", getAverageMethod() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindFaceAverage::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedFaceArrayName", getSelectedFaceArrayName() );
  writer->writeValue("AverageMethod", getAverageMethod() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFaceAverage::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-383);
  }
  else
  {
    // We MUST have Triangles defined.
    if(sm->getFaces().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -385);
      setErrorCondition(-385);
    }
    else
    {
      GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshGrainFaceId, ss, -387, int32_t, Int32ArrayType, fields, 1)
      if(1==m_AverageMethod)
      {
        GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshTriangleAreas, ss, -387, double, DoubleArrayType, fields, 1)
      }

      if(m_SelectedFaceArrayName.empty() == true)
      {
        setErrorCondition(-11000);
        addErrorMessage(getHumanLabel(), "An array from the Face Data Container must be selected.", getErrorCondition());
      }
      else if(preflight)
      {
        IDataArray::Pointer inputData = sm->getFaceData(m_SelectedFaceArrayName);
        if (NULL == inputData.get())
        {
          ss.str("");
          ss << "Selected array '" << m_SelectedFaceArrayName << "' does not exist in the Surface Mesh Data Container. Was it spelled correctly?";
          setErrorCondition(-11001);
          notifyErrorMessage(ss.str(), getErrorCondition());
          return;
        }

        int numberOfComponents = inputData->GetNumberOfComponents();
        std::string dType = inputData->getTypeAsString();
        IDataArray::Pointer p = IDataArray::NullPointer();
        if (dType.compare("int8_t") == 0)
        {
          p = Int8ArrayType::CreateArray(1, numberOfComponents, m_SelectedFaceArrayName);
        }
        else if (dType.compare("uint8_t") == 0)
        {
          p = UInt8ArrayType::CreateArray(1, numberOfComponents, m_SelectedFaceArrayName);
        }
        else if (dType.compare("int16_t") == 0)
        {
          p = Int16ArrayType::CreateArray(1, numberOfComponents, m_SelectedFaceArrayName);
        }
        else if (dType.compare("uint16_t") == 0)
        {
          p = UInt16ArrayType::CreateArray(1, numberOfComponents, m_SelectedFaceArrayName);
        }
        else if (dType.compare("int32_t") == 0)
        {
          p = Int32ArrayType::CreateArray(1, numberOfComponents, m_SelectedFaceArrayName);
        }
        else if (dType.compare("uint32_t") == 0)
        {
          p = UInt32ArrayType::CreateArray(1, numberOfComponents, m_SelectedFaceArrayName);
        }
        else if (dType.compare("int64_t") == 0)
        {
          p = Int64ArrayType::CreateArray(1, numberOfComponents, m_SelectedFaceArrayName);
        }
        else if (dType.compare("uint64_t") == 0)
        {
          p = UInt64ArrayType::CreateArray(1, numberOfComponents, m_SelectedFaceArrayName);
        }
        else if (dType.compare("float") == 0)
        {
          p = FloatArrayType::CreateArray(1, numberOfComponents, m_SelectedFaceArrayName);
        }
        else if (dType.compare("double") == 0)
        {
          p = DoubleArrayType::CreateArray(1, numberOfComponents, m_SelectedFaceArrayName);
        }
        else if (dType.compare("bool") == 0)
        {
          p = BoolArrayType::CreateArray(1, numberOfComponents, m_SelectedFaceArrayName);
        }
        sm->addFieldData(p->GetName(), p);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFaceAverage::preflight()
{
  dataCheckSurfaceMesh(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
IDataArray::Pointer averageData(IDataArray::Pointer inputData, int64_t triangles, int64_t faces, int32_t* faceIDs, unsigned int method, double* areas)
{
  std::string fieldArrayName = inputData->GetName();

  DataArray<T>* cell = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == cell)
  {
    return IDataArray::NullPointer();
  }
  typename DataArray<T>::Pointer field = DataArray<T>::CreateArray(faces, inputData->GetNumberOfComponents(), fieldArrayName);

  T* fPtr = field->GetPointer(0);
  T* cPtr = cell->GetPointer(0);

  int32_t numComp = field->GetNumberOfComponents();
  int32_t grainIdx = 0;

  double* triangleCount = new double[faces];
  double* tempContainer = new double[faces*numComp];
  for(int i=0; i<faces; i++){triangleCount[i]=0;}
  for(int i=0; i<faces*numComp; i++){tempContainer[i]=0;}

  for(int64_t i = 0; i < triangles; ++i)
  {
    //get grain id of voxel
    int fId = faceIDs[i];

    //gffsets for grain / voxel arrays
    int vIndex=i*numComp;
    int fInxed=fId*numComp;

    //add componenets
    if(0==method)//number weighted average
    {
      for(int j=0; j<numComp; j++)
      {
        tempContainer[fInxed+j]=tempContainer[fInxed+j]+cell->GetValue(vIndex+j);
      }
      //add to face's triangle count
      triangleCount[fId]=triangleCount[fId]+1;
    }
    else if(1==method)//area weighted average
    {
      for(int j=0; j<numComp; j++)
      {
        tempContainer[fInxed+j]=tempContainer[fInxed+j]+(cell->GetValue(vIndex+j))*areas[i];
      }
      //add to face's triangle area
      triangleCount[fId]=triangleCount[fId]+areas[i];
    }


  }

  //loop over field, normalizing and copying
  for(int i=0; i<faces; i++)
  {
    int index=i*numComp;
    for(int j=0; j<numComp; j++)
    {
      tempContainer[index+j]=tempContainer[index+j]/triangleCount[i];
      field->SetValue(index+j, static_cast<T>(tempContainer[index+j]));
    }
  }

  //cleanup
  delete triangleCount;
  delete tempContainer;

  return field;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFaceAverage::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMesh DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  int64_t numberTriangles = sm->getNumFaceTuples();
  int64_t fields = sm->getNumFieldTuples();
  dataCheckSurfaceMesh(false, 0, numberTriangles, 0);

  IDataArray::Pointer inputData = sm->getFaceData(m_SelectedFaceArrayName);
  if (NULL == inputData.get())
  {
    ss.str("");
    ss << "Selected array '" << m_SelectedFaceArrayName << "' does not exist in the Voxel Data Container. Was it spelled correctly?";
    setErrorCondition(-11001);
    notifyErrorMessage(ss.str(), getErrorCondition());
    return;
  }

  notifyStatusMessage("Starting");

  fields=0;
  for(int i=0; i<numberTriangles; i++)
  {
    if(m_SurfaceMeshGrainFaceId[i]>fields)fields=m_SurfaceMeshGrainFaceId[i];
  }
  fields++;

  //BOOST_ASSERT(false);

  std::string dType = inputData->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();

  if (dType.compare("int8_t") == 0)
  {
    p = averageData<int8_t>(inputData, numberTriangles, fields, m_SurfaceMeshGrainFaceId, m_AverageMethod, m_SurfaceMeshTriangleAreas);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    p = averageData<uint8_t>(inputData, numberTriangles, fields, m_SurfaceMeshGrainFaceId, m_AverageMethod, m_SurfaceMeshTriangleAreas);
  }
  else if (dType.compare("int16_t") == 0)
  {
    p = averageData<int16_t>(inputData, numberTriangles, fields, m_SurfaceMeshGrainFaceId, m_AverageMethod, m_SurfaceMeshTriangleAreas);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    p = averageData<uint16_t>(inputData, numberTriangles, fields, m_SurfaceMeshGrainFaceId, m_AverageMethod, m_SurfaceMeshTriangleAreas);
  }
  else if (dType.compare("int32_t") == 0)
  {
    p = averageData<int32_t>(inputData, numberTriangles, fields, m_SurfaceMeshGrainFaceId, m_AverageMethod, m_SurfaceMeshTriangleAreas);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    p = averageData<uint32_t>(inputData, numberTriangles, fields, m_SurfaceMeshGrainFaceId, m_AverageMethod, m_SurfaceMeshTriangleAreas);
  }
  else if (dType.compare("int64_t") == 0)
  {
    p = averageData<int64_t>(inputData, numberTriangles, fields, m_SurfaceMeshGrainFaceId, m_AverageMethod, m_SurfaceMeshTriangleAreas);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    p = averageData<uint64_t>(inputData, numberTriangles, fields, m_SurfaceMeshGrainFaceId, m_AverageMethod, m_SurfaceMeshTriangleAreas);
  }
  else if (dType.compare("float") == 0)
  {
    p = averageData<float>(inputData, numberTriangles, fields, m_SurfaceMeshGrainFaceId, m_AverageMethod, m_SurfaceMeshTriangleAreas);
  }
  else if (dType.compare("double") == 0)
  {
    p = averageData<double>(inputData, numberTriangles, fields, m_SurfaceMeshGrainFaceId, m_AverageMethod, m_SurfaceMeshTriangleAreas);
  }
  else if (dType.compare("bool") == 0)
  {
    p = averageData<bool>(inputData, numberTriangles, fields, m_SurfaceMeshGrainFaceId, m_AverageMethod, m_SurfaceMeshTriangleAreas);
  }

  if (p.get() != NULL)
  {
    sm->addFieldData(p->GetName(), p);

  }
  else
  {
    setErrorCondition(-14000);
    notifyErrorMessage("There was an issue creating a cell data array from a field data array. Either the types did not match or we could not cast the input array to the expected output type", getErrorCondition());
  }





  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
