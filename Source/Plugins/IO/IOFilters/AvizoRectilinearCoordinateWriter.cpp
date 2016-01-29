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

#include "AvizoRectilinearCoordinateWriter.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QDateTime>

#include "IO/IOConstants.h"

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"


// Include the MOC generated file for this class
#include "moc_AvizoRectilinearCoordinateWriter.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AvizoRectilinearCoordinateWriter::AvizoRectilinearCoordinateWriter() :
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
AvizoRectilinearCoordinateWriter::~AvizoRectilinearCoordinateWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoRectilinearCoordinateWriter::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(OutputFileFilterParameter::New("Output File", "OutputFile", getOutputFile(), FilterParameter::Parameter, "*.am", "Amira Mesh"));
  parameters.push_back(BooleanFilterParameter::New("Write Binary File", "WriteBinaryFile", getWriteBinaryFile(), FilterParameter::Parameter));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Ids", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));
  }

  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void AvizoRectilinearCoordinateWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int AvizoRectilinearCoordinateWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(OutputFile)
  SIMPL_FILTER_WRITE_PARAMETER(WriteBinaryFile)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoRectilinearCoordinateWriter::dataCheck()
{
  setErrorCondition(0);

  DataContainer::Pointer dc = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName(), false);
  if (getErrorCondition() < 0) { return; }

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
void AvizoRectilinearCoordinateWriter::preflight()
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
void AvizoRectilinearCoordinateWriter::execute()
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
void AvizoRectilinearCoordinateWriter::generateHeader(QDataStream& ss)
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
  ss << "define Lattice " << (qint64)x << " " << (qint64)y << (qint64)z << "\n";
  ss << "define Coordinates " << (qint64)(x + y + z) << "\n\n";

  ss << "Parameters {\n";
  ss << "     DREAM3DParams {\n";
  ss << "         Author \"DREAM3D\",\n";
  ss << "         DateTime \"" << QDateTime::currentDateTime().toString() << "\"\n";
  ss << "     }\n";

  ss << "     Units {\n";
  ss << "         Coordinates \"microns\"\n";
  ss << "     }\n";
  float origin[3];
  getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName())->getGeometryAs<ImageGeom>()->getOrigin(origin);
  float res[3];
  getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName())->getGeometryAs<ImageGeom>()->getResolution(res);

  ss << "     CoordType \"rectilinear\"\n";
  ss << "}\n\n";

  ss << "Lattice { int FeatureIds } = @1\n";
  ss << "Coordinates { float xyz } = @2\n\n";

  ss << "# Data section follows\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AvizoRectilinearCoordinateWriter::writeData(QDataStream& out)
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  size_t dims[3];
  m->getGeometryAs<ImageGeom>()->getDimensions(dims);
  float origin[3];
  m->getGeometryAs<ImageGeom>()->getOrigin(origin);
  float res[3];
  m->getGeometryAs<ImageGeom>()->getResolution(res);

  QString start("@1 # FeatureIds in z, y, x with X moving fastest, then Y, then Z\n");
  out << start;
  if (true == m_WriteBinaryFile)
  {
    out.writeRawData(reinterpret_cast<char*>(m_FeatureIds), m_FeatureIdsPtr.lock()->getNumberOfTuples() * sizeof(int32_t));
    //writer.writeArray(m_FeatureIds, getDataContainerArray()->getDataContainer(getDataContainerName())->getTotalPoints());
    out << "\n";
  }
  else
  {
    // The "20 Items" is purely arbitrary and is put in to try and save some space in the ASCII file
    int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
    int count = 0;
    QString ss;
    for (int64_t i = 0; i < totalPoints; ++i)
    {
      out << m_FeatureIds[i];
      if(count < 20)
      {
        ss = ss.append(" ");
        count++;
      }
      else
      {
        out << "\n";
        out << ss;
        ss.clear();
        count = 0;
      }
    }
    ss = ss.append("\n"); // Make sure there is a new line at the end of the data block
    // Pick up any remaining data that was not written because we did not have 20 items on a line.
    out << ss;
  }

  start = "@2 # x coordinates, then y, then z\n";
  out << start;
  if (true == m_WriteBinaryFile)
  {
    for (int d = 0; d < 3; ++d)
    {
      std::vector<float> coords(dims[d]);
      for (size_t i = 0; i < dims[d]; ++i)
      {
        coords[i] = origin[d] + (res[d] * i);
      }
      out.writeRawData(reinterpret_cast<char*>(&(coords.front())), dims[d] * sizeof(float));
      out << "\n"; // This puts a new line character
    }
  }
  else
  {
    for (int d = 0; d < 3; ++d)
    {
      for (size_t i = 0; i < dims[d]; ++i)
      {
        out << (origin[d] + (res[d] * i)) << " ";
      }
      out << "\n";
    }
  }

  return 1;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AvizoRectilinearCoordinateWriter::newFilterInstance(bool copyFilterParameters)
{
  AvizoRectilinearCoordinateWriter::Pointer filter = AvizoRectilinearCoordinateWriter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AvizoRectilinearCoordinateWriter::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AvizoRectilinearCoordinateWriter::getBrandingString()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AvizoRectilinearCoordinateWriter::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AvizoRectilinearCoordinateWriter::getGroupName()
{ return SIMPL::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AvizoRectilinearCoordinateWriter::getSubGroupName()
{ return SIMPL::FilterSubGroups::OutputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AvizoRectilinearCoordinateWriter::getHumanLabel()
{ return "Avizo Rectilinear Coordinate Writer"; }

