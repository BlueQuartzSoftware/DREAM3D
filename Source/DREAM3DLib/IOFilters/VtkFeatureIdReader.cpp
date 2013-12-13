/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "VtkFeatureIdReader.h"

#include <QtCore/QMap>
#include <QtCore/QFileInfo>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Utilities/DREAM3DEndian.h"

#define kBufferSize 1024

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkFeatureIdReader::VtkFeatureIdReader() :
  FileReader(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_InputFile(""),
  m_Comment("DREAM3D Generated File"),
  m_DatasetType(""),
  m_FileIsBinary(true),
  m_FeatureIdScalarName(DREAM3D::CellData::FeatureIds),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkFeatureIdReader::~VtkFeatureIdReader()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkFeatureIdReader::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input Vtk File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Feature Id Scalar Name");
    option->setPropertyName("FeatureIdScalarName");
    option->setWidgetType(FilterParameter::StringWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkFeatureIdReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  setFeatureIdScalarName( reader->readString( "FeatureIdScalarName", getFeatureIdScalarName() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkFeatureIdReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkFeatureIdReader::dataCheck()
{

  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, VtkFeatureIdReader>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* attrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  QFileInfo fi(getInputFile());
  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    PipelineMessage em (getHumanLabel(), ss, getErrorCondition(), PipelineMessage::Error);
emit filterGeneratedMessage(em);
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388);
    PipelineMessage em (getHumanLabel(), ss, getErrorCondition(), PipelineMessage::Error);
emit filterGeneratedMessage(em);
  }

  QVector<int> dims(1, 1);
  m_FeatureIdsPtr = attrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this,  m_FeatureIdsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Errors will get propagated to the user through the normal mechanism
  // Reading the header will set the Dimensions, Resolution and Origin
  readHeader();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkFeatureIdReader::preflight()
{
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t VtkFeatureIdReader::parseByteSize(char text[256])
{

  char cunsigned_char [64] = "unsigned_char";
  char cchar [64] = "char";
  char cunsigned_short [64] = "unsigned_short";
  char cshort [64] = "short";
  char cunsigned_int [64] = "unsigned_int";
  char cint [64] = "int";
  char cunsigned_long [64] = " unsigned_long";
  char clong [64] = "long";
  char cfloat [64] = "float";
  char cdouble [64] = " double";

  if (strcmp(text, cunsigned_char) == 0 ) { return 1;}
  if (strcmp(text, cchar) == 0 ) { return 1;}
  if (strcmp(text, cunsigned_short) == 0 ) { return 2;}
  if (strcmp(text, cshort) == 0 ) { return 2;}
  if (strcmp(text, cunsigned_int) == 0 ) { return 4;}
  if (strcmp(text, cint) == 0 ) { return 4;}
  if (strcmp(text, cunsigned_long) == 0 ) { return 8;}
  if (strcmp(text, clong) == 0 ) { return 8;}
  if (strcmp(text, cfloat) == 0 ) { return 4;}
  if (strcmp(text, cdouble) == 0 ) { return  8;}
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkFeatureIdReader::ignoreData(QFile& in, int byteSize, char* text, size_t xDim, size_t yDim, size_t zDim)
{
  char cunsigned_char [64] = "unsigned_char";
  char cchar [64] = "char";
  char cunsigned_short [64] = "unsigned_short";
  char cshort [64] = "short";
  char cunsigned_int [64] = "unsigned_int";
  char cint [64] = "int";
  char cunsigned_long [64] = " unsigned_long";
  char clong [64] = "long";
  char cfloat [64] = "float";
  char cdouble [64] = " double";
  int err = 0;
  if (strcmp(text, cunsigned_char) == 0 )
  {
    err |= skipVolume<char>(in, byteSize, xDim, yDim, zDim);
  }
  if (strcmp(text, cchar) == 0 ) { err |= skipVolume<char>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cunsigned_short) == 0 ) { err |= skipVolume<unsigned short>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cshort) == 0 ) {err |= skipVolume<short>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cunsigned_int) == 0 ) { err |= skipVolume<unsigned int>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cint) == 0 ) { err |= skipVolume<int>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cunsigned_long) == 0 ) { err |= skipVolume<unsigned long long int>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, clong) == 0 ) { err |= skipVolume<long long int>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cfloat) == 0 ) { err |= skipVolume<float>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cdouble) == 0 ) { err |= skipVolume<double>(in, byteSize, xDim, yDim, zDim);}
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkFeatureIdReader::readHeader()
{
  int err = 0;

  dataCheck();

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  if (getInputFile().isEmpty() == true)
  {

    QString ss = QObject::tr("Input filename was empty%1(%2)").arg(__FILE__).arg(__LINE__);
    setErrorCondition(-51010);
    PipelineMessage em (getHumanLabel(), ss, getErrorCondition(), PipelineMessage::Error);
emit filterGeneratedMessage(em);
    return getErrorCondition();
  }

  QFile instream(getInputFile());

  if (!instream.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QObject::tr("Vtk file could not be opened: %1").arg(getInputFile());
    setErrorCondition(-100);
    PipelineMessage em(getHumanLabel(), msg, getErrorCondition(), PipelineMessage::Error);
emit filterGeneratedMessage(em);
    return -100;
  }
  QByteArray buf = instream.readLine(); // Read Line 1 - VTK Version Info
  buf = instream.readLine(); // Read Line 2 - User Comment
  setComment(QString(buf));
  buf = instream.readLine(); // Read Line 3 - BINARY or ASCII
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

    QString ss = QObject::tr("The file type of the VTK legacy file could not be determined. It should be ASCII' or 'BINARY' and should appear on line 3 of the file.");
    setErrorCondition(-51030);
    PipelineMessage em (getHumanLabel(), ss, getErrorCondition(), PipelineMessage::Error);
emit filterGeneratedMessage(em);
    return getErrorCondition();
  }
  buf = instream.readLine(); // Read Line 4 - Type of Dataset
  {
    char text[256];
    int n = sscanf(buf, "%s %s", text, &(text[16]) );
    if (n < 2)
    {

      QString ss = QObject::tr("Error Reading the type of data set. Was expecting 2 features but got %1").arg(n);
      setErrorCondition(-51040);
      PipelineMessage em (getHumanLabel(), ss, getErrorCondition(), PipelineMessage::Error);
emit filterGeneratedMessage(em);
      return getErrorCondition();
    }
    QString dataset(&(text[16]));
    setDatasetType(dataset);
  }
  bool ok = false;
  buf = instream.readLine(); // Read Line 5 which is the Dimension values
  size_t dims[3];
  QList<QByteArray> tokens = buf.split(' ');
  dims[0] = tokens[1].toInt(&ok, 10);
  dims[1] = tokens[2].toInt(&ok, 10);
  dims[2] = tokens[3].toInt(&ok, 10);
  m->setDimensions(dims);

#if 1
  buf = instream.readLine(); // Read Line 6 which is the Origin values
  float origin[3];
  origin[0] = tokens[1].toFloat(&ok);
  origin[1] = tokens[2].toFloat(&ok);
  origin[2] = tokens[3].toFloat(&ok);
  m->setOrigin(origin);

  buf = instream.readLine();// Read Line 7 which is the Scaling values
  float resolution[3];
  resolution[0] = tokens[1].toFloat(&ok);
  resolution[1] = tokens[2].toFloat(&ok);
  resolution[2] = tokens[3].toFloat(&ok);
  m->setResolution(resolution);

#endif

  instream.close();
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkFeatureIdReader::parseCoordinateLine(const char* input, size_t& value)
{
  char text[256];
  char text1[256];
  int i = 0;
  int n = sscanf(input, "%s %d %s", text, &i, text1);
  if (n != 3)
  {
    value = -1;
    return -1;
  }
  value = i;
  return 0;
}

// -----------------------------------------------------------------------------
// Called from the 'execute' method in the superclass
// -----------------------------------------------------------------------------
int VtkFeatureIdReader::readFile()
{
  int err = 0;

  dataCheck();
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QFile instream(getInputFile());

  if (!instream.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QObject::tr("Vtk file could not be opened: %1").arg(getInputFile());
    setErrorCondition(-100);
    PipelineMessage em(getHumanLabel(), msg, getErrorCondition(), PipelineMessage::Error);
emit filterGeneratedMessage(em);
    return -100;
  }
  QByteArray buf;
  for (int i = 0; i < 8; ++i)
  {
    buf = instream.readLine();
  }

  // These should have been set from reading the header
  size_t dims[3];
  m->getDimensions(dims);

  bool ok = false;
  // Now we need to search for the 'FeatureID' and
  bool needFeatureIds = true;

  QString scalarName;
  int typeByteSize = 0;

  //Cell Data is one less in each direction
  size_t totalVoxels = dims[0] * dims[1] * dims[2];
  m->getAttributeMatrix(getCellAttributeMatrixName())->resizeAttributeArrays(totalVoxels);
  dataCheck();

  buf = instream.readLine();
  QList<QByteArray> tokens;
// int i = 0;
  while (needFeatureIds == true)
  {
    buf = instream.readLine();
    tokens = buf.split(' ');

    //int n = sscanf(buf, "%s %s %s %s", text1, text2, text3, text4);
    if (tokens.size() != 4)
    {

      QString ss = QObject::tr("Error reading SCALARS header section of VTK file.");
      PipelineMessage em (getHumanLabel(), ss, -1, PipelineMessage::Error);
      emit filterGeneratedMessage(em);
      return -1;
    }
    scalarName = QString(tokens[0]);
    typeByteSize = tokens[3].toInt(&ok, 10);
    if (typeByteSize == 0)
    {
      return -1;
    }

    QString text3 = tokens.at(2);

    // Check to make sure we are reading the correct set of scalars and if we are
    // NOT then read all this particular Scalar Data and try again

    if (m_FeatureIdScalarName.compare(scalarName) == 0)
    {
      //  QMap<int, int> featureIdMap;
      if (getFileIsBinary() == true)
      {
        // Splat 0xAB across the entire array. that way if the read messes up we
        //  can more easily diagnose the problem.
        ::memset(m_FeatureIdsPtr.lock()->getPointer(0), 0xAB, sizeof(int) * totalVoxels);
        instream.read(reinterpret_cast<char*> (m_FeatureIdsPtr.lock()->getPointer(0)), sizeof(int) * totalVoxels);
        int t;
        // We need to Byte Swap (Possibly) from the Big Endian format stored by
        // the vtk binary file into what ever system we are running.
        for (size_t i = 0; i < totalVoxels; ++i)
        {
          t = m_FeatureIdsPtr.lock()->GetValue(i);
          DREAM3D::Endian::FromSystemToBig::convert(t);
          m_FeatureIds[i] = t;
        }
      }
      else // ASCII VTK File
      {
        int feature_index = -1;
        QTextStream in(&instream);
        for (size_t i = 0; i < totalVoxels; ++i)
        {
          in >> feature_index;
          m_FeatureIds[i] = feature_index;
        }
      }
      needFeatureIds = false;
    }
    else
    {
      ignoreData(instream, typeByteSize, text3.toLatin1().data(), dims[0], dims[1], dims[2]);
    }

  }

  instream.close();
  return err;
}
