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

#define RBR_FILE_NOT_OPEN -1000
#define RBR_FILE_TOO_SMALL -1010
#define RBR_FILE_TOO_BIG -1020
#define RBR_READ_EOF       -1030
#define RBR_NO_ERROR       0


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
int SanityCheckFileSizeVersusAllocatedSize(size_t allocatedBytes,size_t fileSize, int skipHeaderBytes)
{
  if (fileSize-skipHeaderBytes < allocatedBytes) { return -1; }
  else if (fileSize-skipHeaderBytes > allocatedBytes) { return 1; }
  // File Size and Allocated Size are equal so we  are good to go
  return 0;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
int ReadBinaryFile(typename DataArray<T>::Pointer p, const std::string &filename, int skipHeaderBytes)
{
  int err = 0;
  uint64_t fileSize = MXAFileInfo::fileSize(filename);
  size_t allocatedBytes = p->GetSize() * sizeof(T);
  err = SanityCheckFileSizeVersusAllocatedSize(allocatedBytes, fileSize, skipHeaderBytes);

  if (err < 0)
  {
    return RBR_FILE_TOO_SMALL;
  }


  FILE* f = fopen(filename.c_str(), "rb");
  if (NULL == f)
  {
    return RBR_FILE_NOT_OPEN;
  }

  ScopedFileMonitor monitor(f);
  size_t numElements = p->GetNumberOfTuples() * p->GetNumberOfComponents();
  size_t numRead = 0;

  T* ptr = p->GetPointer(0);

  //Skip some header bytes by just reading those bytes into the pointer knowing that the next
  // thing we are going to do it over write those bytes with the real data that we are after.
  if (skipHeaderBytes > 0)
  {
    numRead = fread(ptr, 1, skipHeaderBytes, f);
  }
  numRead = 0;
  // Now start reading the data in chunks if needed.
  while(1)
  {
    numRead += fread(ptr, sizeof(T), numElements, f);

    // If we try to read at or past EOF
    if ( feof(f) != 0 )
    {
      return RBR_READ_EOF;
    }
	// Don't read junk at the end of the file
    else if (numRead == numElements)
    {
      break;
    }
    // If we are here we did NOT read all the data, so increment the pointer and loop again.
    ptr = p->GetPointer(numRead);
  }
  return RBR_NO_ERROR;
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
  m_OverRideOriginResolution(true),
  m_SkipHeaderBytes(0),
  m_OutputArrayName(""),
  m_InputFile("")
{
  m_Dimensions.x = 0;
  m_Dimensions.y = 0;
  m_Dimensions.z = 0;

  m_Origin.x = 0.0;
  m_Origin.y = 0.0;
  m_Origin.z = 0.0;

  m_Resolution.x = 1.0;
  m_Resolution.y = 1.0;
  m_Resolution.z = 1.0;
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
    option->setValueType("IntVec3Widget_t");
    option->setUnits("XYZ");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Origin");
    option->setPropertyName("Origin");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("XYZ");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Resolution");
    option->setPropertyName("Resolution");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("XYZ");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Over Ride Origin & Resolution");
    option->setPropertyName("OverRideOriginResolution");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Skip Header Bytes");
    option->setPropertyName("SkipHeaderBytes");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output Array Name");
    option->setPropertyName("OutputArrayName");
    option->setWidgetType(FilterParameter::StringWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RawBinaryReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RawBinaryReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  writer->writeValue("ScalarType", getScalarType() );
  writer->writeValue("Dimensionality", getDimensionality() );
  writer->writeValue("NumberOfComponents", getNumberOfComponents() );
  writer->writeValue("Endian", getEndian() );
  writer->writeValue("Dimensions", getDimensions() );
  writer->writeValue("Origin", getOrigin() );
  writer->writeValue("Resolution", getResolution() );
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("OverRideOriginResolution", getOverRideOriginResolution() );
  writer->writeValue("SkipHeaderBytes", getSkipHeaderBytes() );
  writer->writeValue("OutputArrayName", getOutputArrayName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
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

  if(m_OutputArrayName.empty() == true)
  {
    ss.str("");
    ss << "The Output Array Name is blank (empty) and a value must be filled in for the pipeline to complete.";
    setErrorCondition(-398);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }

  if (m_NumberOfComponents < 1)
  {
    ss.str("");
    ss << "The number of components must be larger than Zero";
    setErrorCondition(-391);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }

  if (m_Dimensionality < 1)
  {
    ss.str("");
    ss << "The dimensionality must be larger than Zero";
    setErrorCondition(-389);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }

  if (  m_Dimensions.x == 0 || m_Dimensions.y == 0 || m_Dimensions.z == 0)
  {
    ss.str("");
    ss << "One of the dimensions has a size less than or Equal to Zero (0). The minimum size must be greater than One (1).";
    setErrorCondition(-390);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }

  if (true == preflight)
  {
    size_t allocatedBytes = 0;
    IDataArray::Pointer p = IDataArray::NullPointer();
    if (m_ScalarType == Detail::Int8)
    {
      p = Int8ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
      allocatedBytes = sizeof(int8_t) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::UInt8)
    {
      p = UInt8ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
      allocatedBytes = sizeof(uint8_t) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::Int16)
    {
      p = Int16ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
      allocatedBytes = sizeof(int16_t) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::UInt16)
    {
      p = UInt16ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
      allocatedBytes = sizeof(uint16_t) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::Int32)
    {
      p = Int32ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
      allocatedBytes = sizeof(int32_t) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::UInt32)
    {
      p = UInt32ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
      allocatedBytes = sizeof(uint32_t) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::Int64)
    {
      p = Int64ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
      allocatedBytes = sizeof(int64_t) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::UInt64)
    {
      p = UInt64ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
      allocatedBytes = sizeof(uint64_t) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::Float)
    {
      p = FloatArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
      allocatedBytes = sizeof(float) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::Double)
    {
      p = DoubleArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
      allocatedBytes = sizeof(double) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }

    // Sanity Check Allocated Bytes versus size of file
    uint64_t fileSize = MXAFileInfo::fileSize(m_InputFile);
    int check = SanityCheckFileSizeVersusAllocatedSize(allocatedBytes, fileSize, m_SkipHeaderBytes);
    if (check == -1)
    {
      ss.str("");
      ss << "The file size is " << fileSize << " but the number of bytes needed to fill the array is " << allocatedBytes << ". This condition would cause an error reading the input file.";
      ss << " Please adjust the input parameters to match the size of the file or select a different data file.";
      setErrorCondition(RBR_FILE_TOO_SMALL);
      addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
    }
    else if (check == 1)
    {
      ss.str("");
      ss << "The file size is " << fileSize << " but the number of bytes needed to fill the array is " << allocatedBytes << " which is less than the size of the file.";
      ss << " DREAM3D will read only the first part of the file into the array.";
      addWarningMessage(getHumanLabel(), ss.str(), RBR_FILE_TOO_BIG);
    }

    m->addCellData(p->GetName(), p);

    m->setDimensions(m_Dimensions.x, m_Dimensions.y, m_Dimensions.z);
    m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
    m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);
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
  if (m_OverRideOriginResolution == true)
  {
    m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);
    m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  }
  m->setDimensions(m_Dimensions.x, m_Dimensions.y, m_Dimensions.z);


  array = IDataArray::NullPointer();
  if (m_ScalarType == Detail::Int8)
  {
    Int8ArrayType::Pointer p = Int8ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    err = ReadBinaryFile<int8_t>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 ) { SWAP_ARRAY(p)
          array = p;}
  }
  else if (m_ScalarType == Detail::UInt8)
  {
    UInt8ArrayType::Pointer p = UInt8ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    err = ReadBinaryFile<uint8_t>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 ) { SWAP_ARRAY(p)
          array = p;}
  }
  else if (m_ScalarType == Detail::Int16)
  {
    Int16ArrayType::Pointer p = Int16ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    err = ReadBinaryFile<int16_t>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 ) { SWAP_ARRAY(p)
          array = p;}
  }
  else if (m_ScalarType == Detail::UInt16)
  {
    UInt16ArrayType::Pointer p = UInt16ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    err = ReadBinaryFile<uint16_t>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 ) { SWAP_ARRAY(p)
          array = p;}
  }
  else if (m_ScalarType == Detail::Int32)
  {
    Int32ArrayType::Pointer p = Int32ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    err = ReadBinaryFile<int32_t>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 ) { SWAP_ARRAY(p)
          array = p;}
  }
  else if (m_ScalarType == Detail::UInt32)
  {
    UInt32ArrayType::Pointer p = UInt32ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    err = ReadBinaryFile<uint32_t>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 ) { SWAP_ARRAY(p)
          array = p;}
  }
  else if (m_ScalarType == Detail::Int64)
  {
    Int64ArrayType::Pointer p = Int64ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    err = ReadBinaryFile<int64_t>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 ) { SWAP_ARRAY(p)
          array = p;}
  }
  else if (m_ScalarType == Detail::UInt64)
  {
    UInt64ArrayType::Pointer p = UInt64ArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    err = ReadBinaryFile<uint64_t>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 ) { SWAP_ARRAY(p)
          array = p;}
  }
  else if (m_ScalarType == Detail::Float)
  {
    FloatArrayType::Pointer p = FloatArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    err = ReadBinaryFile<float>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 ) { SWAP_ARRAY(p)
          array = p;}
  }
  else if (m_ScalarType == Detail::Double)
  {
    DoubleArrayType::Pointer p = DoubleArrayType::CreateArray(voxels, m_NumberOfComponents, m_OutputArrayName);
    err = ReadBinaryFile<double>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 ) { SWAP_ARRAY(p)
          array = p;}
  }

  if (NULL != array.get())
  {
    m->addCellData(array->GetName(), array);
  }
  else if(err == RBR_FILE_NOT_OPEN )
  {
    setErrorCondition(RBR_FILE_NOT_OPEN);
    notifyErrorMessage("RawBinaryReader was unable to open the specified file.", getErrorCondition());
  }
  else if (err == RBR_FILE_TOO_SMALL)
  {
    setErrorCondition(RBR_FILE_TOO_SMALL);
    notifyErrorMessage("The file size is smaller than the allocated size.", getErrorCondition());
  }
  else if (err == RBR_FILE_TOO_BIG)
  {
    notifyWarningMessage("The file size is larger than the allocated size.", RBR_FILE_TOO_BIG);
  }
  else if(err == RBR_READ_EOF)
  {
    setErrorCondition(RBR_READ_EOF);
    notifyErrorMessage("RawBinaryReader read past the end of the specified file.", getErrorCondition());
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
