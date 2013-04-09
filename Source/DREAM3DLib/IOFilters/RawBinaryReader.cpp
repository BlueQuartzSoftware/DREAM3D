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
#include "RawBinaryReader.h"

#include <stdio.h>


#include "MXA/MXA.h"
#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Common/IO/MXAFileReader64.h"


#include "DREAM3DLib/Common/ScopedFileMonitor.hpp"
namespace Detail
{
  enum NumType {
    Int8 = 0,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Int64,
    UInt64,
    Float,
    Double,
    UnknownNumType
  };
}

#ifdef CMP_WORDS_BIGENDIAN
#define SWAP_ARRAY(array)\
  if (m_Endian == 0) { array->byteSwapElements(); }

#else
#define SWAP_ARRAY(array)\
  if (m_Endian == 1) { array->byteSwapElements(); }

#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
int ReadBinaryFile(typename DataArray<T>::Pointer p, const std::string &filename)
{
  int err = 0;
  FILE* f = fopen(filename.c_str(), "rb");
  if (NULL == f)
  {
    return -1;
  }

  ScopedFileMonitor monitor(f);
  size_t numElements = p->GetNumberOfTuples() * p->GetNumberOfComponents();
  size_t numRead = 0;

  T* ptr = p->GetPointer(0);
  while(1)
  {
    numRead += fread(ptr, sizeof(T), numElements, f);
    if (numRead == numElements)
    {
      break;
    }
    ptr = p->GetPointer(numRead);
  }
  return err;
}





// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RawBinaryReader::RawBinaryReader() :
  AbstractFilter(),
  m_ScalarType(0),
  m_Endian(0),
  m_Dimensionality(0),
  m_NumberOfComponents(0),
  m_OutputArrayName(""),
  m_InputFile("")
{
  m_Dimensions.x = 0;
  m_Dimensions.y = 0;
  m_Dimensions.z = 0;

  m_Origin.x = 0.0;
  m_Origin.y = 0.0;
  m_Origin.z = 0.0;

  m_Spacing.x = 1.0;
  m_Spacing.y = 1.0;
  m_Spacing.z = 1.0;
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RawBinaryReader::~RawBinaryReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RawBinaryReader::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  /* Place all your option initialization code here */

  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Input File");
    parameter->setPropertyName("InputFile");
    parameter->setWidgetType(FilterParameter::InputFileWidget);
    parameter->setValueType("string");
    parameter->setFileExtension("*.raw *.bin");
    parameters.push_back(parameter);
  }
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Scalar Type");
    parameter->setPropertyName("ScalarType");
    parameter->setWidgetType(FilterParameter::ChoiceWidget);
    parameter->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("signed   int 8  bit");
    choices.push_back("unsigned int 8  bit");
    choices.push_back("signed   int 16 bit");
    choices.push_back("unsigned int 16 bit");
    choices.push_back("signed   int 32 bit");
    choices.push_back("unsigned int 32 bit");
    choices.push_back("signed   int 64 bit");
    choices.push_back("unsigned int 64 bit");
    choices.push_back("       Float 32 bit");
    choices.push_back("      Double 64 bit");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Dimensionality");
    option->setPropertyName("Dimensionality");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Number Of Components");
    option->setPropertyName("NumberOfComponents");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Endian");
    parameter->setPropertyName("Endian");
    parameter->setWidgetType(FilterParameter::ChoiceWidget);
    parameter->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("Little");
    choices.push_back("Big");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Dimensions");
    option->setPropertyName("Dimensions");
    option->setWidgetType(FilterParameter::IntVec3Widget);
    option->setValueType("int");
    option->setUnits("XYZ");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Origin");
    option->setPropertyName("Origin");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("float");
    option->setUnits("XYZ");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Spacing");
    option->setPropertyName("Spacing");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("float");
    option->setUnits("XYZ");
    parameters.push_back(option);
  }

  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Output Array Name");
    parameter->setPropertyName("OutputArrayName");
    parameter->setWidgetType(FilterParameter::StringWidget);
    parameter->setValueType("string");
    parameters.push_back(parameter);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RawBinaryReader::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  writer->writeValue("ScalarType", getScalarType() );
  writer->writeValue("Dimensionality", getDimensionality() );
  writer->writeValue("NumberOfComponents", getNumberOfComponents() );
  writer->writeValue("Endian", getEndian() );
  writer->writeValue("Dimensions", getDimensions() );
  writer->writeValue("Origin", getOrigin() );
  writer->writeValue("Spacing", getSpacing() );
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("OutputArrayName", getOutputArrayName() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RawBinaryReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if (getInputFile().empty() == true)
  {
    ss << ClassName() << " needs the Input File Set and it was not.";
    setErrorCondition(-387);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else if (MXAFileInfo::exists(getInputFile()) == false)
  {
    ss << "The input file does not exist.";
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }

  if (m_NumberOfComponents < 1)
  {
    ss.str("");
    ss << "The number of components must be larger than Zero";
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }

  if (m_Dimensionality < 1)
  {
    ss.str("");
    ss << "The dimensionality must be larger than Zero";
    setErrorCondition(-389);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }

  if (  m_Dimensions.x == 0 ||   m_Dimensions.y == 0 || m_Dimensions.z == 0)
  {
    ss.str("");
    ss << "One of the dimensions has a size less than Zero (0). The minimum size must be at least One (1).";
    setErrorCondition(-390);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }

  if (true == preflight)
  {
    IDataArray::Pointer p = IDataArray::NullPointer();
    if (m_ScalarType == Detail::Int8)
    {
      p = Int8ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::UInt8)
    {
      p = UInt8ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::Int16)
    {
      p = Int16ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::UInt16)
    {
      p = UInt16ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::Int32)
    {
      p = Int32ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::UInt32)
    {
      p = UInt32ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::Int64)
    {
      p = Int64ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::UInt64)
    {
      p = UInt64ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::Float)
    {
      p = FloatArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    }
    else if (m_ScalarType == Detail::Double)
    {
      p = DoubleArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    }

    m->addCellData(p->GetName(), p);
    addCreatedCellData(p->GetName());
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RawBinaryReader::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RawBinaryReader::execute()
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

  // Get the total size of the array from the options
  size_t voxels = m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
  m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);
  m->setResolution(m_Spacing.x, m_Spacing.y, m_Spacing.z);
  m->setDimensions(m_Dimensions.x, m_Dimensions.y, m_Dimensions.z);


  IDataArray::Pointer array = IDataArray::NullPointer();
  if (m_ScalarType == Detail::Int8)
  {
    Int8ArrayType::Pointer p = Int8ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    ReadBinaryFile<int8_t>(p, m_InputFile);
    SWAP_ARRAY(p)
        array = p;
  }
  else if (m_ScalarType == Detail::UInt8)
  {
    UInt8ArrayType::Pointer p = UInt8ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    ReadBinaryFile<uint8_t>(p, m_InputFile);
    SWAP_ARRAY(p)
        array = p;
  }
  else if (m_ScalarType == Detail::Int16)
  {
    Int16ArrayType::Pointer p = Int16ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    ReadBinaryFile<int16_t>(p, m_InputFile);
    SWAP_ARRAY(p)
    array = p;
  }
  else if (m_ScalarType == Detail::UInt16)
  {
    UInt16ArrayType::Pointer p = UInt16ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    ReadBinaryFile<uint16_t>(p, m_InputFile);
    SWAP_ARRAY(p)
        array = p;
  }
  else if (m_ScalarType == Detail::Int32)
  {
    Int32ArrayType::Pointer p = Int32ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    ReadBinaryFile<int32_t>(p, m_InputFile);
    SWAP_ARRAY(p)
        array = p;
  }
  else if (m_ScalarType == Detail::UInt32)
  {
    UInt32ArrayType::Pointer p = UInt32ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    ReadBinaryFile<uint32_t>(p, m_InputFile);
    SWAP_ARRAY(p)
        array = p;
  }
  else if (m_ScalarType == Detail::Int64)
  {
    Int64ArrayType::Pointer p = Int64ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    ReadBinaryFile<int64_t>(p, m_InputFile);
    SWAP_ARRAY(p)
        array = p;
  }
  else if (m_ScalarType == Detail::UInt64)
  {
    UInt64ArrayType::Pointer p = UInt64ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    ReadBinaryFile<uint64_t>(p, m_InputFile);
    SWAP_ARRAY(p)
        array = p;
  }
  else if (m_ScalarType == Detail::Float)
  {
    FloatArrayType::Pointer p = FloatArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    ReadBinaryFile<float>(p, m_InputFile);
    SWAP_ARRAY(p)
        array = p;
  }
  else if (m_ScalarType == Detail::Double)
  {
    DoubleArrayType::Pointer p = DoubleArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    ReadBinaryFile<double>(p, m_InputFile);
    SWAP_ARRAY(p)
        array = p;
  }


  m->addCellData(array->GetName(), array);
  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
