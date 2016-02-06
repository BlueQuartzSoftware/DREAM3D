/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "VtkStructuredPointsReader.h"
#include <fstream>

#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "IO/IOConstants.h"

#define vtkErrorMacro(msg)\
  std::cout  msg

#define kBufferSize 1024

// Include the MOC generated file for this class
#include "moc_VtkStructuredPointsReader.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkStructuredPointsReader::VtkStructuredPointsReader() :
  AbstractFilter(),
  m_ReadCellData(true),
  m_VolumeDataContainerName("ImageDataContainer_CellData"),
  m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName),
  m_ReadPointData(true),
  m_VertexDataContainerName("ImageDataContainer_PointData"),
  m_VertexAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName),
  m_InputFile(""),
  m_Comment(""),
  m_DatasetType(""),
  m_FileIsBinary(true)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkStructuredPointsReader::~VtkStructuredPointsReader()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkStructuredPointsReader::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(InputFileFilterParameter::New("Input VTK File", "InputFile", getInputFile(), FilterParameter::Parameter));
  QStringList linkedProps;
  linkedProps << "VertexDataContainerName" << "VertexAttributeMatrixName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Read Point Data", "ReadPointData", getReadPointData(), linkedProps, FilterParameter::Parameter));
  linkedProps.clear();
  linkedProps << "VolumeDataContainerName" << "CellAttributeMatrixName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Read Cell Data", "ReadCellData", getReadCellData(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(StringFilterParameter::New("Point Data Data Container", "VertexDataContainerName", getVertexDataContainerName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Data Data Container", "VolumeDataContainerName", getVolumeDataContainerName(), FilterParameter::CreatedArray));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Point Data Attribute Matrix", "VertexAttributeMatrixName", getVertexAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Data Attribute Matrix", "CellAttributeMatrixName", getCellAttributeMatrixName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkStructuredPointsReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setVertexDataContainerName(reader->readString("VertexDataContainerName", getVertexDataContainerName()));
  setVolumeDataContainerName(reader->readString("VolumeDataContainerName", getVolumeDataContainerName() ) );
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setVertexAttributeMatrixName(reader->readString("VertexAttributeMatrixName", getVertexAttributeMatrixName() ) );
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  setReadPointData(reader->readValue("ReadPointData", getReadPointData()));
  setReadCellData(reader->readValue("ReadCellData", getReadCellData()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkStructuredPointsReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(InputFile)
  SIMPL_FILTER_WRITE_PARAMETER(VertexDataContainerName)
  SIMPL_FILTER_WRITE_PARAMETER(VolumeDataContainerName)
  SIMPL_FILTER_WRITE_PARAMETER(CellAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(VertexAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(ReadPointData)
  SIMPL_FILTER_WRITE_PARAMETER(ReadCellData)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkStructuredPointsReader::dataCheck()
{
  setErrorCondition(0);

  QFileInfo fi(getInputFile());
  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-61000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-61001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  // First shot Sanity Checks.
  if(!getReadCellData() && !getReadPointData())
  {
    QString ss = QObject::tr("At least one of Read Point Data or Read Cell Data must be checked");
    setErrorCondition(-61002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  // Last chance sanity check
  if(getErrorCondition() < 0) { return; }

  // Create a Vertex Data Container even though we may remove it later. We need it later
  // on in order to set the proper AttributeMatrix
  DataContainer::Pointer pointData_DataContainer = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getVertexDataContainerName());
  if(getErrorCondition() < 0 && NULL == pointData_DataContainer) { return; }

  ImageGeom::Pointer pointDataGeom = ImageGeom::CreateGeometry(getVertexDataContainerName());
  pointData_DataContainer->setGeometry(pointDataGeom);

  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer pointAttrMat = pointData_DataContainer->createNonPrereqAttributeMatrix<AbstractFilter>(this, getVertexAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  // Create a Volume Data Container even though we may remove it later. We need it later
  // on in order to set the proper AttributeMatrix
  DataContainer::Pointer cellData_DataContainer = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getVolumeDataContainerName());
  if(getErrorCondition() < 0 && NULL == cellData_DataContainer) { return; }

  ImageGeom::Pointer cellDataGeom = ImageGeom::CreateGeometry(getVolumeDataContainerName());
  cellData_DataContainer->setGeometry(cellDataGeom);

  tDims.resize(3);
  tDims[0] = 0;
  tDims[1] = 0;
  tDims[2] = 0;
  AttributeMatrix::Pointer cellAttrMat = cellData_DataContainer->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  // Scan through the file
  readFile();

  // now check to see what the user wanted
  if (!getReadPointData())
  {
    getDataContainerArray()->removeDataContainer(getVertexDataContainerName());
  }
  if (!getReadCellData())
  {
    getDataContainerArray()->removeDataContainer(getVolumeDataContainerName());
  }

  // If there was no Cell Data, remove that dataContainer
  if (cellAttrMat->getNumAttributeArrays() == 0)
  {
    getDataContainerArray()->removeDataContainer(getVolumeDataContainerName());
  }

  // If there were no Point Arrays then remove that dataContainer
  if (pointAttrMat->getNumAttributeArrays() == 0)
  {
    getDataContainerArray()->removeDataContainer(getVertexDataContainerName());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkStructuredPointsReader::preflight()
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
void VtkStructuredPointsReader::execute()
{
  setErrorCondition(0);

  // The dataCheck function will do everything based on boolean "InPreflight". If
  // the filter is preflighting then the filter will just gather all the scalar
  // and vector data set names and number of tuples from the file. If the filter
  // is executing then the file data will be read into the data arrays
  dataCheck();

  notifyStatusMessage(getHumanLabel(), "Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t VtkStructuredPointsReader::parseByteSize(QString text)
{
  if (text.compare("unsigned_char") == 0) {return 1; }
  if (text.compare("char") == 0) {return 1; }
  if (text.compare("unsigned_short") == 0) {return 2; }
  if (text.compare("short") == 0) {return 2; }
  if (text.compare("unsigned_int") == 0) {return 4; }
  if (text.compare("int") == 0) {return 4; }
  if (text.compare("unsigned_long") == 0) {return 8; }
  if (text.compare("long") == 0) {return 8; }
  if (text.compare("float") == 0) {return 4; }
  if (text.compare("double") == 0) {return 8; }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
int32_t skipVolume(std::istream& in, bool binary, size_t totalSize)
{
  int32_t err = 0;
  if (binary == true)
  {
    std::istream::pos_type pos = in.tellg();
    qint64 newPos = totalSize * sizeof(T) + 1;
    in.seekg(newPos, std::ios_base::cur); // Move relative to the current position
    pos = in.tellg();
    if (in.fail())
    {
      // check if the position to jump to is past the end of the file
      return -1;
    }
  }
  else
  {
    T tmp;
    for (size_t z = 0; z < totalSize; ++z)
    {
      in >> tmp;
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -------------------------------------------------------------------------
template<typename T>
int32_t vtkReadBinaryData(std::istream& in, T* data, int32_t numTuples, int32_t numComp)
{
  if (numTuples == 0 || numComp == 0)
  {
    // nothing to read here.
    return 1;
  }

  size_t numBytesToRead = static_cast<size_t>(numTuples) * static_cast<size_t>(numComp) * sizeof(T);
  size_t numRead = 0;
  // Cast our pointer to a pointer that std::istream will take
  char* chunkptr = reinterpret_cast<char*>(data);

  numRead = 0;
  // Now start reading the data in chunks if needed.
  size_t chunkSize = DEFAULT_BLOCKSIZE;
  // Sanity check the chunk size to make sure it is not any larger than the chunk of data we are about to read
  if (numBytesToRead < DEFAULT_BLOCKSIZE)
  {
    chunkSize = numBytesToRead;
  }

  size_t master_counter = 0;
  size_t bytes_read = 0;

  // Now chunk through the file reading up chunks of data that can actually be
  // read in a single read. DEFAULT_BLOCKSIZE will control this.
  while(1)
  {
    in.read(chunkptr, chunkSize);
    bytes_read = in.gcount();

    chunkptr = chunkptr + bytes_read;
    master_counter += bytes_read;

    if (numBytesToRead - master_counter < chunkSize)
    {
      chunkSize = numBytesToRead - master_counter;
    }
    if (master_counter >= numBytesToRead)
    {
      break;
    }
    if (in.good())
    {
      //std::cout << "all data read successfully." << in.gcount() << std::endl;
    }

    if ((in.rdstate() & std::ifstream::failbit) != 0)
    {
      std::cout << "FAIL " << in.gcount() << " could be read. Needed " << chunkSize << " total bytes read = " << master_counter << std::endl;
      return -12020;
    }
    if ((in.rdstate() & std::ifstream::eofbit) != 0)
    {
      std::cout << "EOF " << in.gcount() << " could be read. Needed " << chunkSize << " total bytes read = " << master_counter << std::endl;
      return -12021;
    }
    if ((in.rdstate() & std::ifstream::badbit) != 0)
    {
      std::cout << "BAD " << in.gcount() << " could be read. Needed " << chunkSize << " total bytes read = " << master_counter << std::endl;
      return -12021;
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
int32_t readDataChunk(AttributeMatrix::Pointer attrMat, std::istream& in, bool inPreflight, bool binary,
                      const QString& scalarName, int32_t scalarNumComp)
{
  size_t numTuples = attrMat->getNumTuples();

  QVector<size_t> tDims = attrMat->getTupleDimensions();
  QVector<size_t> cDims(1, scalarNumComp);

  typename DataArray<T>::Pointer data = DataArray<T>::CreateArray(tDims, cDims, scalarName, !inPreflight);
  data->initializeWithZeros();
  attrMat->addAttributeArray(data->getName(), data);
  if (inPreflight == true)
  {
    return skipVolume<T>(in, binary, numTuples * scalarNumComp);
  }
  else
  {
    if (binary)
    {
      int32_t err = vtkReadBinaryData<T>(in, data->getPointer(0), numTuples, scalarNumComp);
      if( err < 0 )
      {
        std::cout << "Error Reading Binary Data '" << scalarName.toStdString() << "' " << attrMat->getName().toStdString()
                  << " numTuples = " << numTuples << std::endl;
        return err;
      }
      if(BIGENDIAN == 0) {data->byteSwapElements(); }
    }
    else
    {
      T value = static_cast<T>(0.0);
      size_t totalSize = numTuples * scalarNumComp;
      for (size_t i = 0; i < totalSize; ++i)
      {
        in >> value;
        data->setValue(i, value);
      }
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t VtkStructuredPointsReader::readHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t VtkStructuredPointsReader::readLine(std::istream& in, char* result, size_t length)
{
  in.getline(result, length);
  if (in.fail())
  {
    if (in.eof())
    {
      return 0;
    }
    if (in.gcount() == length)
    {
      // Read kBufferSize chars; ignoring the rest of the line.
      in.clear();
      in.ignore(std::numeric_limits<int>::max(), '\n');
    }
  }
  return 1;
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
int32_t VtkStructuredPointsReader::readString(std::istream& in, char* result, size_t length)
{
  in.width(length);
  in >> result;
  if (in.fail())
  {
    return 0;
  }
  return 1;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char* VtkStructuredPointsReader::lowerCase(char* str, const size_t len)
{
  size_t i;
  char* s;

  for ( i = 0, s = str; *s != '\0' && i < len; s++, i++)
  {
    *s = tolower(*s);
  }
  return str;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t VtkStructuredPointsReader:: readFile()
{
  int32_t err = 0;

  DataContainer::Pointer volDc = getDataContainerArray()->getDataContainer(getVolumeDataContainerName());
  AttributeMatrix::Pointer volAm = volDc->getAttributeMatrix(getCellAttributeMatrixName());

  DataContainer::Pointer vertDc = getDataContainerArray()->getDataContainer(getVertexDataContainerName());
  AttributeMatrix::Pointer vertAm = vertDc->getAttributeMatrix(getVertexAttributeMatrixName());

  std::ifstream in(getInputFile().toLatin1().constData(), std::ios_base::in | std::ios_base::binary);

  if (!in.is_open())
  {
    QString msg = QObject::tr("Error opening output file '%1'").arg(getInputFile());
    setErrorCondition(-61003);
    notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
    return -100;
  }

  QByteArray buf(kBufferSize, '\0');
  char* buffer = buf.data();

  err = readLine(in, buffer, kBufferSize); // Read Line 1 - VTK Version Info
  err = readLine(in, buffer, kBufferSize); // Read Line 2 - User Comment
  setComment(QString(buf));
  err = readLine(in, buffer, kBufferSize); // Read Line 3 - BINARY or ASCII
  QString fileType(buf);
  if (fileType.startsWith("BINARY") == true)
  {
    setFileIsBinary(true);
  }
  else if (fileType.startsWith("ASCII") == true)
  {
    setFileIsBinary(false);
  }
  else
  {
    QString ss = QObject::tr("The file type of the VTK legacy file could not be determined. It should be 'ASCII' or 'BINARY' and should appear on line 3 of the file");
    setErrorCondition(-61004);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return getErrorCondition();
  }

  // Read Line 4 - Type of Dataset
  err = readLine(in, buffer, kBufferSize);
  QList<QByteArray> words = buf.split(' ');
  if (words.size() != 2)
  {
    QString ss = QObject::tr("Error reading the type of data set. Was expecting 2 words but got %1").arg(QString(buf));
    setErrorCondition(-61005);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return getErrorCondition();
  }
  QString dataset(words.at(1));
  dataset = dataset.trimmed();
  setDatasetType(dataset); // Should be STRUCTURED_POINTS

  bool ok = false;
  err = readLine(in, buffer, kBufferSize);  // Read Line 5 which is the Dimension values
  // But we need the 'extents' which is one less in all directions (unless dim=1)
  QVector<size_t> dims(3, 0);
  QList<QByteArray> tokens = buf.split(' ');
  dims[0] = tokens[1].toInt(&ok, 10);
  dims[1] = tokens[2].toInt(&ok, 10);
  dims[2] = tokens[3].toInt(&ok, 10);
  QVector<size_t> tDims(3, 0);
  tDims[0] = dims[0];
  tDims[1] = dims[1];
  tDims[2] = dims[2];
  vertAm->setTupleDimensions(tDims);
  vertDc->getGeometryAs<ImageGeom>()->setDimensions(dims.data());

  tDims[0] = dims[0] - 1;
  tDims[1] = dims[1] - 1;
  tDims[2] = dims[2] - 1;
  volAm->setTupleDimensions(tDims);
  volDc->getGeometryAs<ImageGeom>()->setDimensions(tDims.data());

  err = readLine(in, buffer, kBufferSize); // Read Line 7 which is the Scaling values
  tokens = buf.split(' ');
  float resolution[3];
  resolution[0] = tokens[1].toFloat(&ok);
  resolution[1] = tokens[2].toFloat(&ok);
  resolution[2] = tokens[3].toFloat(&ok);

  volDc->getGeometryAs<ImageGeom>()->setResolution(resolution);
  vertDc->getGeometryAs<ImageGeom>()->setResolution(resolution);

  err = readLine(in, buffer, kBufferSize); // Read Line 6 which is the Origin values
  tokens = buf.split(' ');
  float origin[3];
  origin[0] = tokens[1].toFloat(&ok);
  origin[1] = tokens[2].toFloat(&ok);
  origin[2] = tokens[3].toFloat(&ok);

  volDc->getGeometryAs<ImageGeom>()->setOrigin(origin);
  vertDc->getGeometryAs<ImageGeom>()->setOrigin(origin);

  // Read the first key word which should be POINT_DATA or CELL_DATA
  err = readLine(in, buffer, kBufferSize); // Read Line 6 which is the first type of data we are going to read

  tokens = buf.split(' ');
  QString word = QString(tokens[0]);
  int32_t npts = 0, ncells = 0;
  int32_t numPts = 0;

  if ( word.startsWith("CELL_DATA") )
  {
    DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVolumeDataContainerName());
    m_CurrentAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());
    ncells = tokens[1].toInt(&ok);
    if (m_CurrentAttrMat->getNumTuples() != ncells)
    {
      setErrorCondition(-61006);
      notifyErrorMessage(getHumanLabel(), QString("Number of cells does not match number of tuples in the Attribute Matrix"), getErrorCondition());
      return getErrorCondition();
    }
    this->readDataTypeSection(in, ncells, "point_data");
  }
  else if ( word.startsWith("POINT_DATA") )
  {
    DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVertexDataContainerName());
    m_CurrentAttrMat = m->getAttributeMatrix(getVertexAttributeMatrixName());
    npts = tokens[1].toInt(&ok);
    if (m_CurrentAttrMat->getNumTuples() != npts)
    {
      setErrorCondition(-61007);
      notifyErrorMessage(getHumanLabel(), QString("Number of points does not match number of tuples in the Attribute Matrix"), getErrorCondition());
      return getErrorCondition();
    }
    this->readDataTypeSection(in, numPts, "cell_data");
  }

  // Close the file since we are done with it.
  in.close();

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t VtkStructuredPointsReader::readDataTypeSection(std::istream& in, int32_t numValues, const std::string& nextKeyWord)
{
  QByteArray buf(kBufferSize, '\0');
  char* line = buf.data();

  // Read keywords until end-of-file
  while (this->readString(in, line, kBufferSize))
  {
    // read scalar data
    if ( ! strncmp(lowerCase(line, kBufferSize), "scalars", 7) )
    {
      if ( this->readScalarData(in, numValues) <= 0 )
      {
        return 0;
      }
    }
    // read vector data
    else if ( ! strncmp(line, "vectors", 7) )
    {
      if ( this->readVectorData(in, numValues) <= 0 )
      {
        return 0;
      }
    }
#if 0
    //
    // read 3x3 tensor data
    //
    else if ( ! strncmp(line, "tensors", 7) )
    {
      if ( ! this->ReadTensorData(a, numPts) )
      {
        return 0;
      }
    }
    //
    // read normals data
    //
    else if ( ! strncmp(line, "normals", 7) )
    {

      if ( ! this->ReadNormalData(a, numPts) )
      {
        return 0;
      }
    }
    //
    // read texture coordinates data
    //
    else if ( ! strncmp(line, "texture_coordinates", 19) )
    {
      if ( ! this->ReadTCoordsData(a, numPts) )
      {
        return 0;
      }
    }
    //
    // read the global id data
    //
    else if ( ! strncmp(line, "global_ids", 10) )
    {
      if ( ! this->ReadGlobalIds(a, numPts) )
      {
        return 0;
      }
    }
    //
    // read the pedigree id data
    //
    else if ( ! strncmp(line, "pedigree_ids", 10) )
    {
      if ( ! this->ReadPedigreeIds(a, numPts) )
      {
        return 0;
      }
    }
    //
    // read color scalars data
    //
    else if ( ! strncmp(line, "color_scalars", 13) )
    {
      if ( ! this->ReadCoScalarData(a, numPts) )
      {
        return 0;
      }
    }
    //
    // read lookup table. Associate with scalar data.
    //
    else if ( ! strncmp(line, "lookup_table", 12) )
    {
      if ( ! this->ReadLutData(a) )
      {
        return 0;
      }
    }
    //
    // read field of data
    //
    else if ( ! strncmp(line, "field", 5) )
    {
      vtkFieldData* f;
      if ( ! (f = this->ReadFieldData()) )
      {
        return 0;
      }
      for(int i = 0; i < f->GetNumberOfArrays(); i++)
      {
        a->AddArray(f->GetAbstractArray(i));
      }
      f->Delete();
    }
#endif

    // maybe bumped into cell data
    else if ( ! strncmp(line, nextKeyWord.c_str(), 9) )
    {
      bool ok = false;
      if (readString(in, line, 256))
      {
        if (nextKeyWord.compare("cell_data") == 0)
        {
          DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVolumeDataContainerName());
          m_CurrentAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());
          int32_t ncells = QString(line).toInt(&ok);
          this->readDataTypeSection(in, ncells, "point_data");
        }
        else if (nextKeyWord.compare("point_data") == 0)
        {
          DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVertexDataContainerName());
          m_CurrentAttrMat = m->getAttributeMatrix(getVertexAttributeMatrixName());
          int32_t npts = QString(line).toInt(&ok);
          this->readDataTypeSection(in, npts, "cell_data");
        }
      }
    }
    else
    {
      //vtkErrorMacro(<< "Unsupported point attribute type: " << line
      //<< " for file: " << (this->FileName?this->FileName:"(Null FileName)"));
      return 0;
    }
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// ------------------------------------------------------------------------
int32_t VtkStructuredPointsReader::DecodeString(char* resname, const char* name)
{
  if ( !resname || !name )
  {
    return 0;
  }
  std::ostringstream str;
  size_t cc = 0;
  unsigned int ch;
  size_t len = strlen(name);
  size_t reslen = 0;
  char buffer[10] = "0x";
  while (name[cc])
  {
    if ( name[cc] == '%' )
    {
      if ( cc <= (len - 3) )
      {
        buffer[2] = name[cc + 1];
        buffer[3] = name[cc + 2];
        buffer[4] = 0;
        sscanf(buffer, "%x", &ch);
        str << static_cast<char>(ch);
        cc += 2;
        reslen++;
      }
    }
    else
    {
      str << name[cc];
      reslen++;
    }
    cc ++;
  }
  strncpy(resname, str.str().c_str(), reslen + 1);
  resname[reslen] = 0;
  return static_cast<int32_t>(reslen);
}


// ------------------------------------------------------------------------
//
// ------------------------------------------------------------------------
int32_t VtkStructuredPointsReader::readScalarData(std::istream& in, int32_t numPts)
{
  char line[256], name[256], key[256], tableName[256];
  int32_t numComp = 1;
  char buffer[1024];

  if (!(this->readString(in, buffer, 1024) && this->readString(in, line, 1024)))
  {
    vtkErrorMacro( << "Cannot read scalar header!" << " for file: "
                   << (getInputFile().toStdString()));
    return 0;
  }

  this->DecodeString(name, buffer);

  if (!this->readString(in, key, 1024))
  {
    vtkErrorMacro( << "Cannot read scalar header!" << " for file: "
                   << getInputFile().toStdString());
    return 0;
  }

  QString scalarType(line);

  // the next string could be an integer number of components or a lookup table
  if (strcmp(this->lowerCase(key, 256), "lookup_table"))
  {
    numComp = atoi(key);
    if (numComp < 1 || !this->readString(in, key, 1024))
    {
      vtkErrorMacro( << "Cannot read scalar header!" << " for file: "
                     << getInputFile().toStdString());
      return 0;
    }
  }

  if (strcmp(this->lowerCase(key, 256), "lookup_table"))
  {
    vtkErrorMacro( << "Lookup table must be specified with scalar.\n" <<
                   "Use \"LOOKUP_TABLE default\" to use default table.");
    return 0;
  }

  if (!this->readString(in, tableName, 1024))
  {
    vtkErrorMacro( << "Cannot read scalar header!" << " for file: "
                   << getInputFile().toStdString());
    return 0;
  }

  // Suck up the newline at the end of the current line
  this->readLine(in, line, 1024);

  int32_t err = 1;
  // Read the data
  if (scalarType.compare("unsigned_char") == 0)
  {
    err = readDataChunk<uint8_t>(m_CurrentAttrMat, in, getInPreflight(), getFileIsBinary(), name, numComp);
  }
  else if (scalarType.compare("char") == 0)
  {
    err = readDataChunk<int8_t>(m_CurrentAttrMat, in, getInPreflight(), getFileIsBinary(), name, numComp);
  }
  else if (scalarType.compare("unsigned_short") == 0)
  {
    err = readDataChunk<uint16_t>(m_CurrentAttrMat, in, getInPreflight(), getFileIsBinary(), name, numComp);
  }
  else if (scalarType.compare("short") == 0)
  {
    err = readDataChunk<int16_t>(m_CurrentAttrMat, in, getInPreflight(), getFileIsBinary(), name, numComp);
  }
  else if (scalarType.compare("unsigned_int") == 0)
  {
    err = readDataChunk<uint32_t>(m_CurrentAttrMat, in, getInPreflight(), getFileIsBinary(), name, numComp);
  }
  else if (scalarType.compare("int") == 0)
  {
    err = readDataChunk<int32_t>(m_CurrentAttrMat, in, getInPreflight(), getFileIsBinary(), name, numComp);
  }
  else if (scalarType.compare("unsigned_long") == 0)
  {
    err = readDataChunk<qint64>(m_CurrentAttrMat, in, getInPreflight(), getFileIsBinary(), name, numComp);
  }
  else if (scalarType.compare("long") == 0)
  {
    err = readDataChunk<quint64>(m_CurrentAttrMat, in, getInPreflight(), getFileIsBinary(), name, numComp);
  }
  else if (scalarType.compare("float") == 0)
  {
    err = readDataChunk<float>(m_CurrentAttrMat, in, getInPreflight(), getFileIsBinary(), name, numComp);
  }
  else if (scalarType.compare("double") == 0)
  {
    err = readDataChunk<double>(m_CurrentAttrMat, in, getInPreflight(), getFileIsBinary(), name, numComp);
  }

  return err;
}


// -----------------------------------------------------------------------------
//
// ------------------------------------------------------------------------
int32_t VtkStructuredPointsReader::readVectorData(std::istream& in, int32_t numPts)
{
#if 0
  int skipVector = 0;
  char line[256], name[256];
  vtkDataArray* data;
  char buffer[1024];

  if (!(this->readString(buffer) && this->readString(line)))
  {
    vtkErrorMacro( << "Cannot read vector data!" << " for file: " << (this->FileName ? this->FileName : "(Null FileName)"));
    return 0;
  }
  this->DecodeString(name, buffer);

  //
  // See whether vector has been already read or vector name (if specified)
  // matches name in file.
  //
  if ( a->GetVectors() != NULL || (this->VectorsName && strcmp(name, this->VectorsName)) )
  {
    skipVector = 1;
  }

  data = vtkDataArray::SafeDownCast(
           this->ReadArray(line, numPts, 3));
  if ( data != NULL )
  {
    data->SetName(name);
    if ( ! skipVector )
    {
      a->SetVectors(data);
    }
    else if ( this->ReadAllVectors )
    {
      a->AddArray(data);
    }
    data->Delete();
  }
  else
  {
    return 0;
  }

  float progress = this->GetProgress();
  this->UpdateProgress(progress + 0.5 * (1.0 - progress));
#endif
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t VtkStructuredPointsReader::parseCoordinateLine(const char* input, size_t& value)
{
  char text[256];
  char text1[256];
  int32_t i = 0;
  int32_t n = sscanf(input, "%s %d %s", text, &i, text1);
  if (n != 3)
  {
    value = -1;
    return -1;
  }
  value = i;
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkStructuredPointsReader::readData(std::istream& instream)
{
#if 0
  QByteArray buf(kBufferSize, '\0');
  char* buffer = buf.data();

  QList<QByteArray> tokens;
  int err = 0;

  bool hasPointData = false;
  bool hasCellData = false;
  bool skipChunk = false;

  AttributeMatrix::Pointer attrMat;


  while(instream.atEnd() == false)
  {
    buf = instream.readLine().trimmed();
  }
  // Check to make sure we didn't read to the end of the file
  if(instream.atEnd() == true)
  {
    return;
  }
  tokens = buf.split(' ');

  bool readDataSections;
  while(instream.atEnd() == false)
  {
    skipChunk = false;
    readDataSections = false;
    QString dataStr(tokens.at(0));
    dataStr = "CELL_DATA";
    if (dataStr.compare("POINT_DATA") == 0)
    {
      attrMat = getDataContainerArray()->getDataContainer(getVolumeDataContainerName())->getAttributeMatrix(getVertexAttributeMatrixName());
      readDataSections = true;
      if(m_ReadPointData == true) { hasPointData = true; }
      else { skipChunk = true; }
    }
    else if (dataStr.compare("CELL_DATA") == 0)
    {
      attrMat = getDataContainerArray()->getDataContainer(getVolumeDataContainerName())->getAttributeMatrix(getCellAttributeMatrixName());
      readDataSections = true;
      if(m_ReadCellData == true) { hasCellData = true; }
      else { skipChunk = true; }
    }

    while(readDataSections == true)
    {
      // Read the SCALARS/VECTORS line which should be 3 or 4 words
      buf = instream.readLine().trimmed();
      // If we read an empty line, then we should drop into this while loop and start reading lines until
      // we find a line with something on it.
      while(buf.isEmpty() == true && instream.atEnd() == false)
      {
        buf = instream.readLine().trimmed();
      }
      // Check to make sure we didn't read to the end of the file
      if(instream.atEnd() == true)
      {
        readDataSections = false;
        continue;
      }
      tokens = buf.split(' ');

      QString scalarNumComps;
      QString scalarKeyWord = tokens[0];

      //Check to see if the line read is actually a POINT_DATA or CELL_DATA line
      //This would happen on the second or later block of data and means we have switched data types and need to jump out of this while loop
      if(scalarKeyWord.compare("POINT_DATA") == 0 || scalarKeyWord.compare("CELL_DATA") == 0)
      {
        readDataSections = false;
        continue;
      }

      //if we didn't exit from the POINT_DATA/CELL_DATA check, then make sure the scalars line has the correct info on it
      if (tokens.size() < 3 || tokens.size() > 4)
      {
        QString ss = QObject::tr("Error reading SCALARS header section of VTK file. 3 or 4 words are needed. Found %1. Read Line was\n  %2").arg(tokens.size()).arg(QString(buf));
        setErrorCondition(-61009);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

      if(scalarKeyWord.compare("SCALARS") == 0)
      {
        scalarNumComps = QString("1");
      }
      else if (scalarKeyWord.compare("VECTORS") == 0)
      {
        scalarNumComps = QString("3");
      }
      else
      {
        QString ss = QObject::tr("Error reading Dataset section. Unknown Keyword found. %1").arg(scalarKeyWord);
        setErrorCondition(-61010);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
      QString scalarName = tokens[1];
      scalarName = scalarName.replace("%20", " "); // Replace URL style encoding of string names. %20 is a Space.
      QString scalarType = tokens[2];

      if(tokens.size() == 4)
      {
        scalarNumComps = tokens[3];
      }

      // Read the LOOKUP_TABLE line which should be 2 words
      buf = instream.readLine().trimmed();
      tokens = buf.split(' ');
      QString lookupKeyWord = tokens[0];
      if (lookupKeyWord.compare("LOOKUP_TABLE") != 0 || tokens.size() != 2)
      {
        QString ss = QObject::tr("Error reading LOOKUP_TABLE header section of VTK file. 2 words are needed. Found %1. Read Line was\n  %2").arg(tokens.size()).arg(QString(buf));
        setErrorCondition(-61011);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

      if (scalarType.compare("unsigned_char") == 0)
      {
        err = readDataChunk<uint8_t>(attrMat, instream, getInPreflight(), getFileIsBinary(), scalarName, scalarType, scalarNumComps, skipChunk);
      }
      else if (scalarType.compare("char") == 0)
      {
        err = readDataChunk<int8_t>(attrMat, instream, getInPreflight(), getFileIsBinary(), scalarName, scalarType, scalarNumComps, skipChunk);
      }
      else if (scalarType.compare("unsigned_short") == 0)
      {
        err = readDataChunk<uint16_t>(attrMat, instream, getInPreflight(), getFileIsBinary(), scalarName, scalarType, scalarNumComps, skipChunk);
      }
      else if (scalarType.compare("short") == 0)
      {
        err = readDataChunk<int16_t>(attrMat, instream, getInPreflight(), getFileIsBinary(), scalarName, scalarType, scalarNumComps, skipChunk);
      }
      else if (scalarType.compare("unsigned_int") == 0)
      {
        err = readDataChunk<uint32_t>(attrMat, instream, getInPreflight(), getFileIsBinary(), scalarName, scalarType, scalarNumComps, skipChunk);
      }
      else if (scalarType.compare("int") == 0)
      {
        err = readDataChunk<int32_t>(attrMat, instream, getInPreflight(), getFileIsBinary(), scalarName, scalarType, scalarNumComps, skipChunk);
      }
      else if (scalarType.compare("unsigned_long") == 0)
      {
        err = readDataChunk<qint64>(attrMat, instream, getInPreflight(), getFileIsBinary(), scalarName, scalarType, scalarNumComps, skipChunk);
      }
      else if (scalarType.compare("long") == 0)
      {
        err = readDataChunk<quint64>(attrMat, instream, getInPreflight(), getFileIsBinary(), scalarName, scalarType, scalarNumComps, skipChunk);
      }
      else if (scalarType.compare("float") == 0)
      {
        err = readDataChunk<float>(attrMat, instream, getInPreflight(), getFileIsBinary(), scalarName, scalarType, scalarNumComps, skipChunk);
      }
      else if (scalarType.compare("double") == 0)
      {
        err = readDataChunk<double>(attrMat, instream, getInPreflight(), getFileIsBinary(), scalarName, scalarType, scalarNumComps, skipChunk);
      }

      if(err < 0)
      {
        QString ss = QObject::tr("Error Reading Dataset from VTK File. Dataset Type %1\n  DataSet Name %2\n  Numerical Type: %3\n  File Pos").arg(scalarKeyWord).arg(scalarKeyWord).arg(scalarType).arg(filePos);
        setErrorCondition(err);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

    }

  }

#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VtkStructuredPointsReader::newFilterInstance(bool copyFilterParameters)
{
  VtkStructuredPointsReader::Pointer filter = VtkStructuredPointsReader::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VtkStructuredPointsReader::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VtkStructuredPointsReader::getBrandingString()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VtkStructuredPointsReader::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VtkStructuredPointsReader::getGroupName()
{ return SIMPL::FilterGroups::IOFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VtkStructuredPointsReader::getSubGroupName()
{ return SIMPL::FilterSubGroups::InputFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VtkStructuredPointsReader::getHumanLabel()
{ return "VTK STRUCTURED_POINTS Reader"; }
