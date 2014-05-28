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

#include "CopyFieldArrayToFaceArray.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CopyFieldArrayToFaceArray::CopyFieldArrayToFaceArray() :
  AbstractFilter(),
  m_SurfaceMeshGrainFaceIdArrayName(DREAM3D::FaceData::SurfaceMeshGrainFaceId),
  m_SelectedFieldArrayName(""),
  m_SurfaceMeshGrainFaceId(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CopyFieldArrayToFaceArray::~CopyFieldArrayToFaceArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFieldArrayToFaceArray::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Field Array Name");
    option->setPropertyName("SelectedFieldArrayName");
    option->setWidgetType(FilterParameter::SurfaceMeshFieldArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFieldArrayToFaceArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int CopyFieldArrayToFaceArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedFieldArrayName", getSelectedFieldArrayName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFieldArrayToFaceArray::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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


      if(m_SelectedFieldArrayName.empty() == true)
      {
        setErrorCondition(-11000);
        addErrorMessage(getHumanLabel(), "An array from the Surface Mesh Field Data Container must be selected.", getErrorCondition());
      }
      else if(preflight)
      {
        IDataArray::Pointer inputData = sm->getFieldData(m_SelectedFieldArrayName);
        if (NULL == inputData.get())
        {
          ss.str("");
          ss << "Selected array '" << m_SelectedFieldArrayName << "' does not exist in the Surface Mesh Data Container. Was it spelled correctly?";
          setErrorCondition(-11001);
          notifyErrorMessage(ss.str(), getErrorCondition());
          return;
        }

        int numberOfComponents = inputData->GetNumberOfComponents();
        std::string dType = inputData->getTypeAsString();
        IDataArray::Pointer p = IDataArray::NullPointer();
        if (dType.compare("int8_t") == 0)
        {
          p = Int8ArrayType::CreateArray(1, numberOfComponents, m_SelectedFieldArrayName);
        }
        else if (dType.compare("uint8_t") == 0)
        {
          p = UInt8ArrayType::CreateArray(1, numberOfComponents, m_SelectedFieldArrayName);
        }
        else if (dType.compare("int16_t") == 0)
        {
          p = Int16ArrayType::CreateArray(1, numberOfComponents, m_SelectedFieldArrayName);
        }
        else if (dType.compare("uint16_t") == 0)
        {
          p = UInt16ArrayType::CreateArray(1, numberOfComponents, m_SelectedFieldArrayName);
        }
        else if (dType.compare("int32_t") == 0)
        {
          p = Int32ArrayType::CreateArray(1, numberOfComponents, m_SelectedFieldArrayName);
        }
        else if (dType.compare("uint32_t") == 0)
        {
          p = UInt32ArrayType::CreateArray(1, numberOfComponents, m_SelectedFieldArrayName);
        }
        else if (dType.compare("int64_t") == 0)
        {
          p = Int64ArrayType::CreateArray(1, numberOfComponents, m_SelectedFieldArrayName);
        }
        else if (dType.compare("uint64_t") == 0)
        {
          p = UInt64ArrayType::CreateArray(1, numberOfComponents, m_SelectedFieldArrayName);
        }
        else if (dType.compare("float") == 0)
        {
          p = FloatArrayType::CreateArray(1, numberOfComponents, m_SelectedFieldArrayName);
        }
        else if (dType.compare("double") == 0)
        {
          p = DoubleArrayType::CreateArray(1, numberOfComponents, m_SelectedFieldArrayName);
        }
        else if (dType.compare("bool") == 0)
        {
          p = BoolArrayType::CreateArray(1, numberOfComponents, m_SelectedFieldArrayName);
        }
        sm->addFaceData(p->GetName(), p);
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyFieldArrayToFaceArray::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
IDataArray::Pointer copyData(IDataArray::Pointer inputData, int64_t faces, int32_t* faceIds)
{


  std::string cellArrayName = inputData->GetName();

  DataArray<T>* field = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == field)
  {
    return IDataArray::NullPointer();
  }
  typename DataArray<T>::Pointer cell = DataArray<T>::CreateArray(faces, inputData->GetNumberOfComponents(), cellArrayName);

  T* fPtr = field->GetPointer(0);
  T* cPtr = cell->GetPointer(0);

  int32_t numComp = field->GetNumberOfComponents();
  int32_t faceIdx = 0;

  for(int64_t i = 0; i < faces; ++i)
  {
    // Get the grain id (or what ever the user has selected as their "Grain" identifier
    faceIdx = faceIds[i];
    // Now get the pointer to the start of the tuple for the Field Array at the give Grain Id Index value
    T* fSourcePtr = fPtr + (numComp * faceIdx);
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
void CopyFieldArrayToFaceArray::execute()
{
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMeshDataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  int64_t numberTriangles = sm->getNumFaceTuples();
  int64_t fields = sm->getNumFieldTuples();
  dataCheck(false, 0, numberTriangles, 0);
  if (getErrorCondition() < 0)
  {
    return;
  }
  //int err = 0;
  std::stringstream ss;






  IDataArray::Pointer inputData = sm->getFieldData(m_SelectedFieldArrayName);
  if (NULL == inputData.get())
  {
    ss.str("");
    ss << "Selected array '" << m_SelectedFieldArrayName << "' does not exist in the Surface Mesh Data Container. Was it spelled correctly?";
    setErrorCondition(-11001);
    notifyErrorMessage(ss.str(), getErrorCondition());
    return;
  }

  std::string dType = inputData->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();
  if (dType.compare("int8_t") == 0)
  {
    p = copyData<int8_t>(inputData, numberTriangles, m_SurfaceMeshGrainFaceId);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    p = copyData<uint8_t>(inputData, numberTriangles, m_SurfaceMeshGrainFaceId);
  }
  else if (dType.compare("int16_t") == 0)
  {
    p = copyData<int16_t>(inputData, numberTriangles, m_SurfaceMeshGrainFaceId);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    p = copyData<uint16_t>(inputData, numberTriangles, m_SurfaceMeshGrainFaceId);
  }
  else if (dType.compare("int32_t") == 0)
  {
    p = copyData<int32_t>(inputData, numberTriangles, m_SurfaceMeshGrainFaceId);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    p = copyData<uint32_t>(inputData, numberTriangles, m_SurfaceMeshGrainFaceId);
  }
  else if (dType.compare("int64_t") == 0)
  {
    p = copyData<int64_t>(inputData, numberTriangles, m_SurfaceMeshGrainFaceId);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    p = copyData<uint64_t>(inputData, numberTriangles, m_SurfaceMeshGrainFaceId);
  }
  else if (dType.compare("float") == 0)
  {
    p = copyData<float>(inputData, numberTriangles, m_SurfaceMeshGrainFaceId);
  }
  else if (dType.compare("double") == 0)
  {
    p = copyData<double>(inputData, numberTriangles, m_SurfaceMeshGrainFaceId);
  }
  else if (dType.compare("bool") == 0)
  {
    p = copyData<bool>(inputData, numberTriangles, m_SurfaceMeshGrainFaceId);
  }


  if (p.get() != NULL)
  {
    sm->addFaceData(p->GetName(), p);

  }
  else
  {
    setErrorCondition(-14000);
    notifyErrorMessage("There was an issue creating a cell data array from a field data array. Either the types did not match or we could not cast the input array to the expected output type", getErrorCondition());
  }

  notifyStatusMessage("Complete");
}

