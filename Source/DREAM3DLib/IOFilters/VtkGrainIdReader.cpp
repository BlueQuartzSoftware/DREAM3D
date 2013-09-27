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

#include "VtkGrainIdReader.h"

#include <QtCore/QMap>
#include <QtCore/QFileInfo>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Utilities/DREAM3DEndian.h"

#define kBufferSize 1024

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkGrainIdReader::VtkGrainIdReader() :
FileReader(),
m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
m_InputFile(""),
m_Comment("DREAM3D Generated File"),
m_DatasetType(""),
m_FileIsBinary(true),
m_GrainIdScalarName(DREAM3D::CellData::GrainIds),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_GrainIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkGrainIdReader::~VtkGrainIdReader()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkGrainIdReader::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
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
    option->setHumanLabel("Grain Id Scalar Name");
    option->setPropertyName("GrainIdScalarName");
    option->setWidgetType(FilterParameter::StringWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkGrainIdReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setInputFile( reader->readValue( "InputFile", getInputFile() ) );
  setGrainIdScalarName( reader->readValue( "GrainIdScalarName", getGrainIdScalarName() ) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkGrainIdReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkGrainIdReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{

  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  QFileInfo fi(getInputFile());
  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, int32_t, Int32ArrayType, 0, voxels, 1)

  // Errors will get propagated to the user through the normal mechanism
  // Reading the header will set the Dimensions, Resolution and Origin
  readHeader();


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkGrainIdReader::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t VtkGrainIdReader::parseByteSize(char text[256])
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
int VtkGrainIdReader::ignoreData(QFile &in, int byteSize, char* text, size_t xDim, size_t yDim, size_t zDim)
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
  if (strcmp(text, cunsigned_char) == 0 ) {
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
int VtkGrainIdReader::readHeader()
{

  int err = 0;

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {

    QString ss = QObject::tr("DataContainer Pointer was NULL and Must be valid.%1(%2)").arg(__FILE__).arg(__LINE__);
    setErrorCondition(-51000);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return getErrorCondition();
  }


  if (getInputFile().isEmpty() == true)
  {

    QString ss = QObject::tr("Input filename was empty%1(%2)").arg(__FILE__).arg(__LINE__);
    setErrorCondition(-51010);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return getErrorCondition();
  }


  QFile instream(getInputFile());

  if (!instream.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QObject::tr("Vtk file could not be opened: %1").arg(getInputFile());
    setErrorCondition(-100);
    notifyErrorMessage(msg, getErrorCondition());
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
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return getErrorCondition();
  }
  buf = instream.readLine(); // Read Line 4 - Type of Dataset
  {
    char text[256];
    int n = sscanf(buf, "%s %s", text, &(text[16]) );
    if (n < 2)
    {

    QString ss = QObject::tr("Error Reading the type of data set. Was expecting 2 fields but got %1").arg(n);
    setErrorCondition(-51040);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
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
  getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName())->setDimensions(dims);

#if 1
  buf = instream.readLine(); // Read Line 6 which is the Origin values
  float origin[3];
  origin[0] = tokens[1].toFloat(&ok);
  origin[1] = tokens[2].toFloat(&ok);
  origin[2] = tokens[3].toFloat(&ok);
  getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName())->setOrigin(origin);

  buf = instream.readLine();// Read Line 7 which is the Scaling values
  float resolution[3];
  resolution[0] = tokens[1].toFloat(&ok);
  resolution[1] = tokens[2].toFloat(&ok);
  resolution[2] = tokens[3].toFloat(&ok);
  getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName())->setResolution(resolution);

#endif

  instream.close();
  return err;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkGrainIdReader::parseCoordinateLine(const char* input, size_t &value)
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
int VtkGrainIdReader::readFile()
{
  int err = 0;

  QFile instream(getInputFile());

  if (!instream.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QObject::tr("Vtk file could not be opened: %1").arg(getInputFile());
    setErrorCondition(-100);
    notifyErrorMessage(msg, getErrorCondition());
    return -100;
  }
  QByteArray buf;
  for (int i = 0; i < 8; ++i)
  {
    buf = instream.readLine();
  }

  // These should have been set from reading the header
  size_t dims[3];
  getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName())->getDimensions(dims);

#if 0
  size_t dim = 0;
  // Now parse the X, coordinates.
 // ::memset(buf, 0, kBufferSize);
  err = readLine(instream, buf, kBufferSize);
  err = parseCoordinateLine(buf, dim);
  if (err < 0 || dim != dims[0])
  {

    ss << "x dimension does not match expected dimension: " << dim << " <--> " << dims[0];
    addErrorMessage(getHumanLabel(), ss, -1);
    return -1;
  }
  float xscale = 1.0f;
  err = skipVolume<float>(instream, 4, dim, 1, 1, xscale);

  // Now parse the Y coordinates.
 // ::memset(buf, 0, kBufferSize);
  err = readLine(instream, buf, kBufferSize);
  err = parseCoordinateLine(buf, dim);
  if (err < 0 || dim != dims[1])
  {

    ss << "y dimension does not match expected dimension: " << dim << " <--> " << dims[1];
    addErrorMessage(getHumanLabel(), ss, -1);
    return -1;
  }
  float yscale = 1.0f;
  err = skipVolume<float>(instream, 4, 1, dim, 1, yscale);

  // Now parse the Z coordinates.
//  ::memset(buf, 0, kBufferSize);
  err = readLine(instream, buf, kBufferSize);
  err = parseCoordinateLine(buf, dim);
  if (err < 0 || dim != dims[2])
  {

    ss << "z dimension does not match expected dimension: " << dim << " <--> " << dims[2];
    addErrorMessage(getHumanLabel(), ss, -1);
    return -1;
  }
  float zscale = 1.0f;
  err = skipVolume<float>(instream, 4, 1, 1, dim, zscale);
  if (err < 0)
  {

    ss << "Error skipping Volume section of VTK file.";
    return err;
  }
  // This makes a very bad assumption that the Rectilinear grid has even spacing
  // along each axis which it does NOT have to have. Since this class is specific
  // to the DREAM.3D package this is a safe assumption.
  getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName())->setResolution(xscale, yscale, zscale);
#endif
  bool ok = false;
  // Now we need to search for the 'GrainID' and
  bool needGrainIds = true;

  QString scalarName;
  int typeByteSize = 0;

  //Cell Data is one less in each direction
  size_t totalVoxels = dims[0] * dims[1] * dims[2];
  DataArray<int>::Pointer grainIds = DataArray<int>::CreateArray(totalVoxels, DREAM3D::CellData::GrainIds);

  buf = instream.readLine();
  QList<QByteArray> tokens;
 // int i = 0;
  while (needGrainIds == true)
  {
     buf = instream.readLine();
     tokens = buf.split(' ');

    //int n = sscanf(buf, "%s %s %s %s", text1, text2, text3, text4);
    if (tokens.size() != 4)
    {

      QString ss = QObject::tr("Error reading SCALARS header section of VTK file.");
      addErrorMessage(getHumanLabel(), ss, -1);
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

    if (m_GrainIdScalarName.compare(scalarName) == 0)
    {
    //  QMap<int, int> grainIdMap;
      if (getFileIsBinary() == true)
      {
        // Splat 0xAB across the entire array. that way if the read messes up we
        //  can more easily diagnose the problem.
        ::memset(grainIds->getPointer(0), 0xAB, sizeof(int) * totalVoxels);
        instream.read(reinterpret_cast<char*> (grainIds->getPointer(0)), sizeof(int) * totalVoxels);
        int t;
        // We need to Byte Swap (Possibly) from the Big Endian format stored by
        // the vtk binary file into what ever system we are running.
        for (size_t i = 0; i < totalVoxels; ++i)
        {
          t = grainIds->GetValue(i);
          DREAM3D::Endian::FromSystemToBig::convert(t);
          grainIds->SetValue(i, t);
        }
      }
      else // ASCII VTK File
      {
        int grain_index = -1;
        QTextStream in(&instream);
        for (size_t i = 0; i < totalVoxels; ++i)
        {
          in >> grain_index;
          grainIds->SetValue(i, grain_index);
       //   grainIdMap[grain_index]++;
        }
      }
      needGrainIds = false;
    }
    else
    {
        ignoreData(instream, typeByteSize, text3.toLatin1().data(), dims[0], dims[1], dims[2]);
    }

  }

  // push our grain id data into the DataContainer map
  getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName())->addCellData(DREAM3D::CellData::GrainIds, grainIds);

  instream.close();
  return err;
}
