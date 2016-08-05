/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "AvizoUniformCoordinateWriter.h"


#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QDateTime>

#include "IO/IOConstants.h"
#include "IO/IOVersion.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

// Include the MOC generated file for this class
#include "moc_AvizoUniformCoordinateWriter.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AvizoUniformCoordinateWriter::AvizoUniformCoordinateWriter() :
  AbstractFilter(),
  m_OutputFile(""),
  m_WriteBinaryFile(false),
  m_WriteFeatureIds(true),
  m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds),
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

  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output File", OutputFile, FilterParameter::Parameter, AvizoUniformCoordinateWriter, "*.am", "Amira Mesh"));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Write Binary File", WriteBinaryFile, FilterParameter::Parameter, AvizoUniformCoordinateWriter));
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("FeatureIds", FeatureIdsArrayPath, FilterParameter::RequiredArray, AvizoUniformCoordinateWriter, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void AvizoUniformCoordinateWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setOutputFile( reader->readString( "OutputFile", getOutputFile() ) );
  setWriteBinaryFile( reader->readValue("WriteBinaryFile", getWriteBinaryFile()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoUniformCoordinateWriter::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoUniformCoordinateWriter::dataCheck()
{
  setErrorCondition(0);

  DataContainer::Pointer dc = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName(), false);
  if (getErrorCondition() < 0 || NULL == dc.get()) { return; }

  ImageGeom::Pointer image = dc->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if (getErrorCondition() < 0 || NULL == image.get()) { return; }

  if(m_OutputFile.isEmpty() == true)
  {
    QString ss = QObject::tr("The output file must be set before executing this filter.");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if(m_WriteFeatureIds == true)
  {
    QVector<size_t> dims(1, 1);
    m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoUniformCoordinateWriter::preflight()
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
void AvizoUniformCoordinateWriter::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(m_OutputFile);
  QString parentPath = fi.path();
  QDir dir;
  if(!dir.mkpath(parentPath))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QFile writer(getOutputFile());
  if (!writer.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Avizo Output file could not be opened: %1").arg(getOutputFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QDataStream out(&writer);
  generateHeader(out);

  err = writeData(out);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
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
  getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName())->getGeometryAs<ImageGeom>()->getDimensions(x, y, z);
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
  getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName())->getGeometryAs<ImageGeom>()->getOrigin(origin);
  float res[3];
  getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName())->getGeometryAs<ImageGeom>()->getResolution(res);
  ss << "     # Bounding Box is xmin xmax ymin ymax zmin zmax\n";
  ss << "     BoundingBox " << origin[0] << " " << origin[0] + (res[0] * x);
  ss << " " << origin[1] << " " << origin[1] + (res[1] * y);
  ss << " " << origin[2] << " " << origin[2] + (res[2] * z);
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
    out.writeRawData(reinterpret_cast<char*>(m_FeatureIds), m_FeatureIdsPtr.lock()->getNumberOfTuples() * sizeof(int32_t));
  }
  else
  {
    // The "20 Items" is purely arbitrary and is put in to try and save some space in the ASCII file
    int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AvizoUniformCoordinateWriter::newFilterInstance(bool copyFilterParameters)
{
  AvizoUniformCoordinateWriter::Pointer filter = AvizoUniformCoordinateWriter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AvizoUniformCoordinateWriter::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AvizoUniformCoordinateWriter::getBrandingString()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AvizoUniformCoordinateWriter::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  IO::Version::Major() << "." << IO::Version::Minor() << "." << IO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AvizoUniformCoordinateWriter::getGroupName()
{ return SIMPL::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AvizoUniformCoordinateWriter::getSubGroupName()
{ return SIMPL::FilterSubGroups::OutputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AvizoUniformCoordinateWriter::getHumanLabel()
{ return "Avizo Uniform Coordinate Writer"; }

