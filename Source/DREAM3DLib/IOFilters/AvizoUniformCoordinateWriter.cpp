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
#include "AvizoUniformCoordinateWriter.h"


#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QDateTime>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AvizoUniformCoordinateWriter::AvizoUniformCoordinateWriter() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_WriteFeatureIds(true),
  m_WriteBinaryFile(false),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AvizoUniformCoordinateWriter::~AvizoUniformCoordinateWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoUniformCoordinateWriter::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.am");
    option->setFileType("Amira Mesh");
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Binary File");
    option->setPropertyName("WriteBinaryFile");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void AvizoUniformCoordinateWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setOutputFile( reader->readString( "OutputFile", getOutputFile() ) );
  setWriteBinaryFile( reader->readValue("WriteBinaryFile", getWriteBinaryFile()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AvizoUniformCoordinateWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputFile", getOutputFile());
  writer->writeValue("WriteBinaryFile", getWriteBinaryFile());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoUniformCoordinateWriter::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  if(m_OutputFile.isEmpty() == true)
  {
    QString ss = QObject::tr("The output file must be set before executing this filter.");
    addErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
  }
  if(m_WriteFeatureIds == true)
  {
    QVector<int> dims(1 , 1);
    m_FeatureIdsPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -301, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoUniformCoordinateWriter::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    addErrorMessage(getHumanLabel(), "The VolumeDataContainer Object with the specific name " + getDataContainerName() + " was not available.", getErrorCondition());
    return;
  }

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoUniformCoordinateWriter::execute()
{
  int err = 0;
  setErrorCondition(err);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(m_OutputFile);
  QString parentPath = fi.path();
  QDir dir;
  if(!dir.mkpath(parentPath))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    notifyErrorMessage(ss, -1);
    setErrorCondition(-1);
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  size_t totalFeatures = 0; //m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  size_t totalEnsembleTuples = 0; //m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  dataCheck(false, totalPoints, totalFeatures, totalEnsembleTuples);


  QFile writer(getOutputFile());
  if (!writer.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Avizo Output file could not be opened: %1").arg(getOutputFile());
    setErrorCondition(-100);
    notifyErrorMessage(ss, getErrorCondition());
    return;
  }

  QDataStream out(&writer);
  generateHeader(out);

  err = writeData(out);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoUniformCoordinateWriter::generateHeader(QDataStream& ss)
{
  if(m_WriteBinaryFile == true)
  {
#ifdef CMP_WORDS_BIGENDIAN
    ss << "# AmiraMesh BINARY 2.1\n";
#else
    ss << "# AmiraMesh BINARY-LITTLE-ENDIAN 2.1\n";
#endif
  }
  else
  {
    ss << "# AmiraMesh 3D ASCII 2.0\n";
  }
  ss << "\n";
  ss << "# Dimensions in x-, y-, and z-direction\n";
  size_t x = 0, y = 0, z = 0;
  getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName())->getDimensions(x, y, z);
  ss << "define Lattice " << (qint32)x << " " << (qint32)y << " " << (qint32)z << "\n\n";

  ss << "Parameters {\n";
  ss << "     DREAM3DParams {\n";
  ss << "         Author \"DREAM3D\",\n";
  ss << "         DateTime \"" << QDateTime::currentDateTime().toString() << "\"\n";
  ss << "     }\n";

  ss << "     Units {\n";
  ss << "         Coordinates \"microns\"\n";
  ss << "     }\n";
  ss << "     Content \"" << (qint32)x << "x" << (qint32)y << "x" << (qint32)z << " int, uniform coordinates\",\n";
  float origin[3];
  getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName())->getOrigin(origin);
  float res[3];
  getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName())->getResolution(res);
  ss << "     # Bounding Box is xmin xmax ymin ymax zmin zmax\n";
  ss << "     BoundingBox " << origin[0] << " " << origin[0] + (res[0] * x);
  ss << " " << origin[1] << " " << origin[1] + (res[1] * x);
  ss << " " << origin[2] << " " << origin[2] + (res[2] * x);
  ss << ",\n";
  ss << "     CoordType \"uniform\"\n";
  ss << "}\n\n";

  ss << "Lattice { int FeatureIds } = @1\n\n";

  ss << "# Data section follows\n";

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AvizoUniformCoordinateWriter::writeData(QDataStream& out)
{
  QString start("@1\n");
  out << start;
  if(true == m_WriteBinaryFile)
  {
    out.writeRawData(reinterpret_cast<char*>(m_FeatureIds), getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName())->getTotalPoints() * sizeof(int32_t));
  }
  else
  {
    // The "20 Items" is purely arbitrary and is put in to try and save some space in the ASCII file
    int64_t totalPoints = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName())->getTotalPoints();
    int count = 0;
    for (int64_t i = 0; i < totalPoints; ++i)
    {
      out << m_FeatureIds[i];
      if(count < 20)
      {
        out << " ";
        count++;
      }
      else
      {
        out << "\n";
        count = 0;
      }
    }
    // Pick up any remaining data that was not written because we did not have 20 items on a line.
    out << "\n";
  }
  return 1;
}

