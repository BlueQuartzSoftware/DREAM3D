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



#include <QtCore/QFileInfo>
#include <QtCore/QFile>

#include "DREAM3DLib/Common/ScopedFileMonitor.hpp"
#include "DREAM3DLib/Utilities/DREAM3DEndian.h"


#define RBR_FILE_NOT_OPEN -1000
#define RBR_FILE_TOO_SMALL -1010
#define RBR_FILE_TOO_BIG -1020
#define RBR_READ_EOF       -1030
#define RBR_NO_ERROR       0


namespace Detail
{
  enum NumType
  {
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
int SanityCheckFileSizeVersusAllocatedSize(size_t allocatedBytes, size_t fileSize, int skipHeaderBytes)
{
  if (fileSize - skipHeaderBytes < allocatedBytes)
  {
    return -1;
  }
  else if (fileSize - skipHeaderBytes > allocatedBytes)
  {
    return 1;
  }
  // File Size and Allocated Size are equal so we  are good to go
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
int readBinaryFile(typename DataArray<T>::Pointer p, const QString& filename, int skipHeaderBytes)
{
  int err = 0;
  QFileInfo fi(filename);
  uint64_t fileSize = fi.size();
  size_t allocatedBytes = p->getSize() * sizeof(T);
  err = SanityCheckFileSizeVersusAllocatedSize(allocatedBytes, fileSize, skipHeaderBytes);

  if (err < 0)
  {
    return RBR_FILE_TOO_SMALL;
  }

  FILE* f = fopen(filename.toLatin1().data(), "rb");
  if (NULL == f)
  {
    return RBR_FILE_NOT_OPEN;
  }

  ScopedFileMonitor monitor(f);
  size_t numBytesToRead = p->getNumberOfTuples() * static_cast<size_t>(p->getNumberOfComponents()) * sizeof(T);
  size_t numRead = 0;

  unsigned char* chunkptr = reinterpret_cast<unsigned char*>(p->getPointer(0));

  //Skip some header bytes by just reading those bytes into the pointer knowing that the next
  // thing we are going to do it over write those bytes with the real data that we are after.
  if (skipHeaderBytes > 0)
  {
    numRead = fread(chunkptr, 1, skipHeaderBytes, f);
  }
  numRead = 0;
  // Now start reading the data in chunks if needed.
  size_t chunkSize = DEFAULT_BLOCKSIZE;

  if(numBytesToRead < DEFAULT_BLOCKSIZE)
  {
    chunkSize = numBytesToRead;
  }

  size_t master_counter = 0;
  size_t bytes_read = 0;
  while(1)
  {
    bytes_read = fread(chunkptr, sizeof(unsigned char), chunkSize, f);
    chunkptr = chunkptr + bytes_read;
    master_counter += bytes_read;

    if( numBytesToRead - master_counter < chunkSize)
    {
      chunkSize = numBytesToRead - master_counter;
    }
    if(master_counter >= numBytesToRead)
    {
      break;
    }

  }

  return RBR_NO_ERROR;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RawBinaryReader::RawBinaryReader() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::DataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_ScalarType(0),
  m_Endian(0),
  m_Dimensionality(0),
  m_NumberOfComponents(0),
  m_OverRideOriginResolution(true),
  m_SkipHeaderBytes(0),
  m_OutputArrayName(""),
  m_InputFile(""),
  m_AddToExistingAttributeMatrix(false)
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
  FilterParameterVector parameters;
  /* Place all your option initialization code here */

  parameters.push_back(FileSystemFilterParameter::New("Input File", "InputFile", FilterParameterWidgetType::InputFileWidget, getInputFile(), false, "", "*.raw *.bin"));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Scalar Type");
    parameter->setPropertyName("ScalarType");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    QVector<QString> choices;
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
  parameters.push_back(FilterParameter::New("Dimensionality", "Dimensionality", FilterParameterWidgetType::IntWidget, getDimensionality(), false));
  parameters.push_back(FilterParameter::New("Number Of Components", "NumberOfComponents", FilterParameterWidgetType::IntWidget, getNumberOfComponents(), false));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Endian");
    parameter->setPropertyName("Endian");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    QVector<QString> choices;
    choices.push_back("Little");
    choices.push_back("Big");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  parameters.push_back(FilterParameter::New("Dimensions", "Dimensions", FilterParameterWidgetType::IntVec3Widget, getDimensions(), false, "XYZ"));
  parameters.push_back(FilterParameter::New("Origin", "Origin", FilterParameterWidgetType::FloatVec3Widget, getOrigin(), false, "XYZ"));
  parameters.push_back(FilterParameter::New("Resolution", "Resolution", FilterParameterWidgetType::FloatVec3Widget, getResolution(), false, "XYZ"));
  parameters.push_back(FilterParameter::New("Over Ride Origin & Resolution", "OverRideOriginResolution", FilterParameterWidgetType::BooleanWidget, getOverRideOriginResolution(), false));
  parameters.push_back(FilterParameter::New("Skip Header Bytes", "SkipHeaderBytes", FilterParameterWidgetType::IntWidget, getSkipHeaderBytes(), false));
  parameters.push_back(FilterParameter::New("Output Array Name", "OutputArrayName", FilterParameterWidgetType::StringWidget, getOutputArrayName(), false));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Add to Existing DataContainer & AttributeMatrix", "AddToExistingAttributeMatrix", FilterParameterWidgetType::BooleanWidget, getDataContainerName(), true));
  parameters.push_back(FilterParameter::New("Data Container Name", "DataContainerName", FilterParameterWidgetType::StringWidget, getDataContainerName(), true));
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix Name", "CellAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellAttributeMatrixName(), true));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RawBinaryReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName() ) );
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  setScalarType( reader->readValue("ScalarType", getScalarType()) );
  setDimensionality( reader->readValue("Dimensionality", getDimensionality()) );
  setNumberOfComponents( reader->readValue("NumberOfComponents", getNumberOfComponents()) );
  setEndian( reader->readValue("Endian", getEndian()) );
  setDimensions( reader->readIntVec3("Dimensions", getDimensions() ) );
  setOrigin( reader->readFloatVec3("Origin", getOrigin() ) );
  setResolution( reader->readFloatVec3("Resolution", getResolution() ) );
  setOverRideOriginResolution( reader->readValue("OverRideOriginResolution", getOverRideOriginResolution()) );
  setSkipHeaderBytes( reader->readValue("SkipHeaderBytes", getSkipHeaderBytes()) );
  setOutputArrayName( reader->readString( "OutputArrayName", getOutputArrayName() ) );
  setAddToExistingAttributeMatrix(reader->readValue("AddToExistingAttributeMatrix", getAddToExistingAttributeMatrix() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RawBinaryReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(DataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(ScalarType)
  DREAM3D_FILTER_WRITE_PARAMETER(Dimensionality)
  DREAM3D_FILTER_WRITE_PARAMETER(NumberOfComponents)
  DREAM3D_FILTER_WRITE_PARAMETER(Endian)
  DREAM3D_FILTER_WRITE_PARAMETER(Dimensions)
  DREAM3D_FILTER_WRITE_PARAMETER(Origin)
  DREAM3D_FILTER_WRITE_PARAMETER(Resolution)
  DREAM3D_FILTER_WRITE_PARAMETER(InputFile)
  DREAM3D_FILTER_WRITE_PARAMETER(OverRideOriginResolution)
  DREAM3D_FILTER_WRITE_PARAMETER(SkipHeaderBytes)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(AddToExistingAttributeMatrix)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RawBinaryReader::dataCheck()
{
  setErrorCondition(0);

  QFileInfo fi(getInputFile());
  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(m_OutputArrayName.isEmpty() == true)
  {
    QString ss = QObject::tr("The Output Array Name is blank (empty) and a value must be filled in for the pipeline to complete.");
    setErrorCondition(-398);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (m_NumberOfComponents < 1)
  {
    QString ss = QObject::tr("The number of components must be larger than Zero");
    setErrorCondition(-391);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (m_Dimensionality < 1)
  {
    QString ss = QObject::tr("The dimensionality must be larger than Zero");
    setErrorCondition(-389);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (  m_Dimensions.x == 0 || m_Dimensions.y == 0 || m_Dimensions.z == 0)
  {
    QString ss = QObject::tr("One of the dimensions has a size less than or Equal to Zero (0). The minimum size must be greater than One (1).");
    setErrorCondition(-390);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  DataContainer::Pointer m = DataContainer::NullPointer();
  AttributeMatrix::Pointer attrMat;

  if (getAddToExistingAttributeMatrix() )
  {
    m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
    if(getErrorCondition() < 0)
    {
      return;
    }

    attrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -10000);
    if(getErrorCondition() < 0)
    {
      return;
    }
  }
  else
  {
    m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
    if(getErrorCondition() < 0)
    {
      return;
    }

    QVector<size_t> tDims(3, 0);
    tDims[0] = m_Dimensions.x;
    tDims[1] = m_Dimensions.y;
    tDims[2] =  m_Dimensions.z;
    attrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
    if(getErrorCondition() < 0)
    {
      return;
    }
  }

  if (getInPreflight())
  {
    size_t allocatedBytes = 0;
    IDataArray::Pointer p = IDataArray::NullPointer();
    QVector<size_t> dims(1, m_NumberOfComponents);
    if (m_ScalarType == Detail::Int8)
    {
      attrMat->createAndAddAttributeArray<DataArray<int8_t>, AbstractFilter, int8_t>(this, m_OutputArrayName, 0, dims);
      allocatedBytes = sizeof(int8_t) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::UInt8)
    {
      attrMat->createAndAddAttributeArray<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, m_OutputArrayName, 0, dims);
      allocatedBytes = sizeof(uint8_t) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::Int16)
    {
      attrMat->createAndAddAttributeArray<DataArray<int16_t>, AbstractFilter, int16_t>(this, m_OutputArrayName, 0, dims);
      allocatedBytes = sizeof(int16_t) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::UInt16)
    {
      attrMat->createAndAddAttributeArray<DataArray<uint16_t>, AbstractFilter, uint16_t>(this, m_OutputArrayName, 0, dims);
      allocatedBytes = sizeof(uint16_t) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::Int32)
    {
      attrMat->createAndAddAttributeArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_OutputArrayName, 0, dims);
      allocatedBytes = sizeof(int32_t) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::UInt32)
    {
      attrMat->createAndAddAttributeArray<DataArray<uint32_t>, AbstractFilter, uint32_t>(this, m_OutputArrayName, 0, dims);
      allocatedBytes = sizeof(uint32_t) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::Int64)
    {
      attrMat->createAndAddAttributeArray<DataArray<int64_t>, AbstractFilter, int64_t>(this, m_OutputArrayName, 0, dims);
      allocatedBytes = sizeof(int64_t) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::UInt64)
    {
      attrMat->createAndAddAttributeArray<DataArray<uint64_t>, AbstractFilter, uint64_t>(this, m_OutputArrayName, 0, dims);
      allocatedBytes = sizeof(uint64_t) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::Float)
    {
      attrMat->createAndAddAttributeArray<DataArray<float>, AbstractFilter, float>(this, m_OutputArrayName, 0, dims);
      allocatedBytes = sizeof(float) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }
    else if (m_ScalarType == Detail::Double)
    {
      attrMat->createAndAddAttributeArray<DataArray<double>, AbstractFilter, double>(this, m_OutputArrayName, 0, dims);
      allocatedBytes = sizeof(double) * m_NumberOfComponents * m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    }

    // Sanity Check Allocated Bytes versus size of file
    uint64_t fileSize = fi.size();
    int check = SanityCheckFileSizeVersusAllocatedSize(allocatedBytes, fileSize, m_SkipHeaderBytes);
    if (check == -1)
    {

      QString ss = QObject::tr("The file size is %1 but the number of bytes needed to fill the array is %2. This condition would cause an error reading the input file."
                               " Please adjust the input parameters to match the size of the file or select a different data file.").arg(fileSize).arg(allocatedBytes);
      setErrorCondition(RBR_FILE_TOO_SMALL);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    else if (check == 1)
    {

      QString ss = QObject::tr("The file size is %1 but the number of bytes needed to fill the array is %2 which is less than the size of the file."
                               " DREAM3D will read only the first part of the file into the array.").arg(fileSize).arg(allocatedBytes);
      notifyWarningMessage(getHumanLabel(), ss, RBR_FILE_TOO_BIG);
    }

    ImageGeom::Pointer image = ImageGeom::CreateGeometry("BinaryImage");
    image->setDimensions(m_Dimensions.x, m_Dimensions.y, m_Dimensions.z);
    image->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
    image->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);
    m->setGeometry(image);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RawBinaryReader::preflight()
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
void RawBinaryReader::execute()
{
  int err = 0;
  QString ss;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  ImageGeom::Pointer image = ImageGeom::CreateGeometry("BinaryImage");
  m->setGeometry(image);

  setErrorCondition(0);

  // Get the total size of the array from the options
  size_t voxels = m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
  if (m_OverRideOriginResolution == true)
  {
    image->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);
    image->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  }
  image->setDimensions(m_Dimensions.x, m_Dimensions.y, m_Dimensions.z);

  if(getAddToExistingAttributeMatrix() == false)
  {
    QVector<size_t> tDims(3, 0);
    tDims[0] = m_Dimensions.x;
    tDims[1] = m_Dimensions.y;
    tDims[2] =  m_Dimensions.z;
    m->getAttributeMatrix(getCellAttributeMatrixName())->resizeAttributeArrays(tDims);
  }

  array = IDataArray::NullPointer();
  if (m_ScalarType == Detail::Int8)
  {
    QVector<size_t> dims(1, m_NumberOfComponents);
    Int8ArrayType::Pointer p = Int8ArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    err = readBinaryFile<int8_t>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 )
    {
      SWAP_ARRAY(p)
      array = p;
    }
  }
  else if (m_ScalarType == Detail::UInt8)
  {
    QVector<size_t> dims(1, m_NumberOfComponents);
    UInt8ArrayType::Pointer p = UInt8ArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    err = readBinaryFile<uint8_t>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 )
    {
      SWAP_ARRAY(p)
      array = p;
    }
  }
  else if (m_ScalarType == Detail::Int16)
  {
    QVector<size_t> dims(1, m_NumberOfComponents);
    Int16ArrayType::Pointer p = Int16ArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    err = readBinaryFile<int16_t>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 )
    {
      SWAP_ARRAY(p)
      array = p;
    }
  }
  else if (m_ScalarType == Detail::UInt16)
  {
    QVector<size_t> dims(1, m_NumberOfComponents);
    UInt16ArrayType::Pointer p = UInt16ArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    err = readBinaryFile<uint16_t>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 )
    {
      SWAP_ARRAY(p)
      array = p;
    }
  }
  else if (m_ScalarType == Detail::Int32)
  {
    QVector<size_t> dims(1, m_NumberOfComponents);
    Int32ArrayType::Pointer p = Int32ArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    err = readBinaryFile<int32_t>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 )
    {
      SWAP_ARRAY(p)
      array = p;
    }
  }
  else if (m_ScalarType == Detail::UInt32)
  {
    QVector<size_t> dims(1, m_NumberOfComponents);
    UInt32ArrayType::Pointer p = UInt32ArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    err = readBinaryFile<uint32_t>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 )
    {
      SWAP_ARRAY(p)
      array = p;
    }
  }
  else if (m_ScalarType == Detail::Int64)
  {
    QVector<size_t> dims(1, m_NumberOfComponents);
    Int64ArrayType::Pointer p = Int64ArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    err = readBinaryFile<int64_t>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 )
    {
      SWAP_ARRAY(p)
      array = p;
    }
  }
  else if (m_ScalarType == Detail::UInt64)
  {
    QVector<size_t> dims(1, m_NumberOfComponents);
    UInt64ArrayType::Pointer p = UInt64ArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    err = readBinaryFile<uint64_t>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 )
    {
      SWAP_ARRAY(p)
      array = p;
    }
  }
  else if (m_ScalarType == Detail::Float)
  {
    QVector<size_t> dims(1, m_NumberOfComponents);
    FloatArrayType::Pointer p = FloatArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    p->initializeWithValue(666.6666f);
    err = readBinaryFile<float>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 )
    {
      SWAP_ARRAY(p)
      array = p;
    }
  }
  else if (m_ScalarType == Detail::Double)
  {
    QVector<size_t> dims(1, m_NumberOfComponents);
    DoubleArrayType::Pointer p = DoubleArrayType::CreateArray(voxels, dims, m_OutputArrayName);
    err = readBinaryFile<double>(p, m_InputFile, m_SkipHeaderBytes);
    if (err >= 0 )
    {
      SWAP_ARRAY(p)
      array = p;
    }
  }

  if (NULL != array.get())
  {
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(array->getName(), array);
  }
  else if(err == RBR_FILE_NOT_OPEN )
  {
    setErrorCondition(RBR_FILE_NOT_OPEN);
    notifyErrorMessage(getHumanLabel(), "RawBinaryReader was unable to open the specified file.", getErrorCondition());
  }
  else if (err == RBR_FILE_TOO_SMALL)
  {
    setErrorCondition(RBR_FILE_TOO_SMALL);
    notifyErrorMessage(getHumanLabel(), "The file size is smaller than the allocated size.", getErrorCondition());
  }
  else if (err == RBR_FILE_TOO_BIG)
  {
    notifyWarningMessage(getHumanLabel(), "The file size is larger than the allocated size.", RBR_FILE_TOO_BIG);
  }
  else if(err == RBR_READ_EOF)
  {
    setErrorCondition(RBR_READ_EOF);
    notifyErrorMessage(getHumanLabel(), "RawBinaryReader read past the end of the specified file.", getErrorCondition());
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RawBinaryReader::newFilterInstance(bool copyFilterParameters)
{
  /*
  * ScalarType
  * Endian
  * Dimensionality
  * NumberOfComponents
  * Dimensions
  * Origin
  * Resolution
  * OverRideOriginResolution
  * SkipHeaderBytes
  * OutputArrayName
  * InputFile
  */
  RawBinaryReader::Pointer filter = RawBinaryReader::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RawBinaryReader::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RawBinaryReader::getGroupName()
{
  return DREAM3D::FilterGroups::IOFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RawBinaryReader::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::InputFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RawBinaryReader::getHumanLabel()
{
  return "Raw Binary Reader";
}

