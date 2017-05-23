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

#include "AvizoRectilinearCoordinateWriter.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "IO/IOConstants.h"
#include "IO/IOVersion.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/SIMPLibVersion.h"

// Include the MOC generated file for this class
#include "moc_AvizoRectilinearCoordinateWriter.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AvizoRectilinearCoordinateWriter::AvizoRectilinearCoordinateWriter()
: AbstractFilter()
, m_OutputFile("")
, m_WriteBinaryFile(false)
, m_Units("microns")
, m_WriteFeatureIds(true)
, m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_FeatureIds(nullptr)
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

  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output File", OutputFile, FilterParameter::Parameter, AvizoRectilinearCoordinateWriter, "*.am", "Amira Mesh"));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Write Binary File", WriteBinaryFile, FilterParameter::Parameter, AvizoRectilinearCoordinateWriter));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, AvizoRectilinearCoordinateWriter, req));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("Units", Units, FilterParameter::Parameter, AvizoRectilinearCoordinateWriter, 0));

  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void AvizoRectilinearCoordinateWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setOutputFile(reader->readString("OutputFile", getOutputFile()));
  setWriteBinaryFile(reader->readValue("WriteBinaryFile", getWriteBinaryFile()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoRectilinearCoordinateWriter::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoRectilinearCoordinateWriter::dataCheck()
{
  setErrorCondition(0);

  DataContainer::Pointer dc = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName(), false);
  if(getErrorCondition() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = dc->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0 || nullptr == image.get())
  {
    return;
  }

  if(m_OutputFile.isEmpty() == true)
  {
    QString ss = QObject::tr("The output file must be set before executing this filter.");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if(m_WriteFeatureIds == true)
  {
    QVector<size_t> dims(1, 1);
    m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                          dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_FeatureIdsPtr.lock().get())                                                                  /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
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
  if(getErrorCondition() < 0)
  {
    return;
  }

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(m_OutputFile);
  QString parentPath = fi.path();
  QDir dir;
  if(!dir.mkpath(parentPath))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    setErrorCondition(-93000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  FILE* avizoFile = fopen(getOutputFile().toLatin1().data(), "wb");
  if(nullptr == avizoFile)
  {
    setErrorCondition(-93001);
    QString ss = QObject::tr("Error creating file '%1'").arg(getOutputFile());
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  generateHeader(avizoFile);

  err = writeData(avizoFile);

  fclose(avizoFile);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoRectilinearCoordinateWriter::generateHeader(FILE* f)
{
  if(m_WriteBinaryFile == true)
  {
#ifdef CMP_WORDS_BIGENDIAN
    fprintf(f, "# AmiraMesh BINARY 2.1\n");
#else
    fprintf(f, "# AmiraMesh BINARY-LITTLE-ENDIAN 2.1\n");
#endif
  }
  else
  {
    fprintf(f, "# AmiraMesh 3D ASCII 2.0\n");
  }
  fprintf(f, "\n");
  fprintf(f, "# Dimensions in x-, y-, and z-direction\n");
  size_t x = 0, y = 0, z = 0;
  getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName())->getGeometryAs<ImageGeom>()->getDimensions(x, y, z);

  fprintf(f, "define Lattice %llu %llu %llu\n", static_cast<unsigned long long>(x), static_cast<unsigned long long>(y), static_cast<unsigned long long>(z));
  fprintf(f, "define Coordinates %llu\n\n", static_cast<unsigned long long>(x + y + z));

  fprintf(f, "Parameters {\n");
  fprintf(f, "     DREAM3DParams {\n");
  fprintf(f, "         Author \"DREAM.3D %s\",\n", IO::Version::PackageComplete().toLatin1().data());
  fprintf(f, "         DateTime \"%s\"\n", QDateTime::currentDateTime().toString().toLatin1().data());
  fprintf(f, "         FeatureIds Path \"%s\"\n", getFeatureIdsArrayPath().serialize("/").toLatin1().data());
  fprintf(f, "     }\n");

  fprintf(f, "     Units {\n");
  fprintf(f, "         Coordinates \"%s\"\n", getUnits().toLatin1().data());
  fprintf(f, "     }\n");
  float origin[3];
  getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName())->getGeometryAs<ImageGeom>()->getOrigin(origin);
  float res[3];
  getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName())->getGeometryAs<ImageGeom>()->getResolution(res);

  fprintf(f, "     CoordType \"rectilinear\"\n");
  fprintf(f, "}\n\n");

  fprintf(f, "Lattice { int FeatureIds } = @1\n");
  fprintf(f, "Coordinates { float xyz } = @2\n\n");

  fprintf(f, "# Data section follows\n");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AvizoRectilinearCoordinateWriter::writeData(FILE* f)
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  size_t dims[3];
  m->getGeometryAs<ImageGeom>()->getDimensions(dims);
  float origin[3];
  m->getGeometryAs<ImageGeom>()->getOrigin(origin);
  float res[3];
  m->getGeometryAs<ImageGeom>()->getResolution(res);

  QString start("@1 # FeatureIds in z, y, x with X moving fastest, then Y, then Z\n");
  fprintf(f, "%s", start.toLatin1().data());
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  if(true == m_WriteBinaryFile)
  {
    fwrite(m_FeatureIds, sizeof(int32_t), totalPoints, f);
  }
  else
  {
    // The "20 Items" is purely arbitrary and is put in to try and save some space in the ASCII file
    int count = 0;
    for(size_t i = 0; i < totalPoints; ++i)
    {
      fprintf(f, "%d", m_FeatureIds[i]);
      if(count < 20)
      {
        fprintf(f, " ");
        count++;
      }
      else
      {
        fprintf(f, "\n");
        count = 0;
      }
    }
  }
  fprintf(f, "\n");

  start = "@2 # x coordinates, then y, then z\n";
  fprintf(f, "%s", start.toLatin1().data());

  if(true == m_WriteBinaryFile)
  {
    for(int d = 0; d < 3; ++d)
    {
      std::vector<float> coords(dims[d]);
      for(size_t i = 0; i < dims[d]; ++i)
      {
        coords[i] = origin[d] + (res[d] * i);
      }
      fwrite(reinterpret_cast<char*>(coords.data()), sizeof(char), sizeof(char) * sizeof(float) * dims[d], f);
      fprintf(f, "\n");
    }
  }
  else
  {
    for(int d = 0; d < 3; ++d)
    {
      for(size_t i = 0; i < dims[d]; ++i)
      {
        fprintf(f, "%f ", origin[d] + (res[d] * i));
      }
      fprintf(f, "\n");
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
  vStream << IO::Version::Major() << "." << IO::Version::Minor() << "." << IO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AvizoRectilinearCoordinateWriter::getGroupName()
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AvizoRectilinearCoordinateWriter::getSubGroupName()
{
  return SIMPL::FilterSubGroups::OutputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AvizoRectilinearCoordinateWriter::getHumanLabel()
{
  return "Avizo Rectilinear Coordinate Exporter";
}
