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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "AbaqusSurfaceMeshWriter.h"

#include <QtCore/QDir>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/ScopedFileMonitor.hpp"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"

#include "ImportExport/ImportExportConstants.h"
#include "ImportExport/ImportExportVersion.h"

#define TRI_ELEMENT_TYPE "SFM3D3"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbaqusSurfaceMeshWriter::AbaqusSurfaceMeshWriter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbaqusSurfaceMeshWriter::~AbaqusSurfaceMeshWriter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output File", OutputFile, FilterParameter::Category::Parameter, AbaqusSurfaceMeshWriter, "*.inp"));
  parameters.push_back(SeparatorFilterParameter::Create("Face Data", FilterParameter::Category::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", SurfaceMeshFaceLabelsArrayPath, FilterParameter::Category::RequiredArray, AbaqusSurfaceMeshWriter, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath()));
  setOutputFile(reader->readString("OutputFile", getOutputFile()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  FileSystemPathHelper::CheckOutputFile(this, "Output File Path", getOutputFile(), true);

  QVector<IDataArray::Pointer> dataArrays;

  TriangleGeom::Pointer triangles = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());
  if(getErrorCode() >= 0)
  {
    dataArrays.push_back(triangles->getTriangles());
  }

  std::vector<size_t> cDims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getSurfaceMeshFaceLabelsArrayPath(), cDims);
  if(nullptr != m_SurfaceMeshFaceLabelsPtr.lock())
  {
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrays.push_back(m_SurfaceMeshFaceLabelsPtr.lock());
  }

  getDataContainerArray()->validateNumberOfTuples(this, dataArrays);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getOutputFile());
  QDir parentPath = fi.path();
  if(!parentPath.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    setErrorCondition(-8005, ss);
    return;
  }

  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();

  // Store all the unique Spins
  std::set<int32_t> uniqueSpins;
  for(MeshIndexType i = 0; i < triangleGeom->getNumberOfTris(); i++)
  {
    uniqueSpins.insert(m_SurfaceMeshFaceLabels[i * 2]);
    uniqueSpins.insert(m_SurfaceMeshFaceLabels[i * 2 + 1]);
  }

  FILE* f = fopen(m_OutputFile.toLatin1().data(), "wb");
  ScopedFileMonitor fileMonitor(f);

  int32_t err = writeHeader(f, triangleGeom->getNumberOfVertices(), triangleGeom->getNumberOfTris(), uniqueSpins.size() - 1);
  if(err < 0)
  {
    QString ss = QObject::tr("Error writing header for file '%1'").arg(m_OutputFile);
    setErrorCondition(-8001, ss);
    return;
  }
  err = writeNodes(f);
  if(err < 0)
  {
    QString ss = QObject::tr("Error writing nodes for file '%1'").arg(m_OutputFile);
    setErrorCondition(-8002, ss);
    return;
  }
  err = writeTriangles(f);
  if(err < 0)
  {
    QString ss = QObject::tr("Error writing triangles for file '%1'").arg(m_OutputFile);
    setErrorCondition(-8003, ss);
    return;
  }
  err = writeFeatures(f);
  if(err < 0)
  {
    QString ss = QObject::tr("Error writing Features for file '%1'").arg(m_OutputFile);
    setErrorCondition(-8004, ss);
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t AbaqusSurfaceMeshWriter::writeHeader(FILE* f, int64_t nodeCount, int64_t triCount, int featureCount)
{
  if(nullptr == f)
  {
    return -1;
  }
  fprintf(f, "*HEADING\n");
  fprintf(f, "** File Created with DREAM3D Version %s\n", ImportExport::Version::Complete().toStdString().c_str());
  fprintf(f, "** There are 3 Sections to this INP File: Nodes, Elements and Sets of Elements for each grain\n");
  fprintf(f, "** This file represents a trianguglar based mesh. The element type selected is %s for the triangles\n", TRI_ELEMENT_TYPE);
  fprintf(f, "** This file is an experimental output from DREAM3D. The user is responsible for verifying all elements in Abaqus\n");
  fprintf(f, "** We have selected to use a 'shell' element type currently. No boundary elements are written\n");
  fprintf(f, "**Number of Nodes: %lld     Number of Triangles: %lld   Number of Grains: %d\n", (long long int)nodeCount, (long long int)triCount, featureCount);
  fprintf(f, "*PREPRINT,ECHO=NO,HISTORY=NO,MODEL=NO\n");
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t AbaqusSurfaceMeshWriter::writeNodes(FILE* f)
{
  TriangleGeom::Pointer triangleGeom = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName())->getGeometryAs<TriangleGeom>();
  float* nodes = triangleGeom->getVertexPointer(0);

  int64_t numNodes = triangleGeom->getNumberOfVertices();
  int32_t err = 0;
  fprintf(f, "*Node,NSET=NALL\n");
  // 1, 72.520433763730, 70.306420652241, 100.000000000000
  // Abaqus Starts number at 1 NOT 0(Zero).
  for(int64_t i = 1; i <= numNodes; ++i)
  {
    fprintf(f, "%lld, %0.6f, %0.6f, %0.6f\n", (long long int)i, nodes[(i - 1) * 3], nodes[(i - 1) * 3 + 1], nodes[(i - 1) * 3 + 2]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t AbaqusSurfaceMeshWriter::writeTriangles(FILE* f)
{
  int32_t err = 0;
  TriangleGeom::Pointer triangleGeom = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName())->getGeometryAs<TriangleGeom>();
  MeshIndexType numTri = triangleGeom->getNumberOfTris();
  MeshIndexType* triangles = triangleGeom->getTriPointer(0);

  fprintf(f, "*ELEMENT, TYPE=%s\n", TRI_ELEMENT_TYPE);
  for(MeshIndexType i = 1; i <= numTri; ++i)
  {
    // When we get the node index, add 1 to it because Abaqus number is 1 based.
    int64_t nId0 = triangles[(i - 1) * 3] + 1;
    int64_t nId1 = triangles[(i - 1) * 3 + 1] + 1;
    int64_t nId2 = triangles[(i - 1) * 3 + 2] + 1;
    fprintf(f, "%lld, %lld, %lld, %lld\n", (long long int)i, (long long int)nId0, (long long int)nId1, (long long int)nId2);
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t AbaqusSurfaceMeshWriter::writeFeatures(FILE* f)
{
  //*Elset, elset=Feature1
  // 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
  int32_t err = 0;

  TriangleGeom::Pointer triangleGeo = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName())->getGeometryAs<TriangleGeom>();

  int64_t nTriangles = triangleGeo->getNumberOfTris();

  // Store all the unique Spins
  std::set<int32_t> uniqueSpins;
  for(int64_t i = 0; i < nTriangles; i++)
  {
    uniqueSpins.insert(m_SurfaceMeshFaceLabels[i * 2]);
    uniqueSpins.insert(m_SurfaceMeshFaceLabels[i * 2 + 1]);
  }

  int32_t spin = 0;

  // Loop over the unique Spins
  for(std::set<int32_t>::iterator spinIter = uniqueSpins.begin(); spinIter != uniqueSpins.end(); ++spinIter)
  {
    spin = *spinIter;
    if(spin < 0)
    {
      continue;
    }

    fprintf(f, "*ELSET, ELSET=Feature%d\n", spin);

    {
      QString ss = QObject::tr("Writing ELSET for Feature Id %1").arg(spin);
      notifyStatusMessage(ss);
    }

    // Loop over all the triangles for this spin
    int64_t lineCount = 0;
    for(int64_t t = 0; t < nTriangles; ++t)
    {
      if(m_SurfaceMeshFaceLabels[t * 2] != spin && m_SurfaceMeshFaceLabels[t * 2 + 1] != spin)
      {
        continue; // We do not match either spin so move to the next triangle
      }

      // Only print 15 Triangles per line
      if(lineCount == 15)
      {
        fprintf(f, ", %lld\n", (long long int)(t));
        lineCount = 0;
      }
      else if(lineCount == 0) // First value on the line
      {
        fprintf(f, "%lld", (long long int)(t));
        lineCount++;
      }
      else
      {
        fprintf(f, ", %lld", (long long int)(t));
        lineCount++;
      }
    }
    // Make sure we have a new line at the end of the section
    if(lineCount != 0)
    {
      fprintf(f, "\n");
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AbaqusSurfaceMeshWriter::newFilterInstance(bool copyFilterParameters) const
{
  AbaqusSurfaceMeshWriter::Pointer filter = AbaqusSurfaceMeshWriter::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AbaqusSurfaceMeshWriter::getCompiledLibraryName() const
{
  return ImportExportConstants::ImportExportBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AbaqusSurfaceMeshWriter::getBrandingString() const
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AbaqusSurfaceMeshWriter::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << ImportExport::Version::Major() << "." << ImportExport::Version::Minor() << "." << ImportExport::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AbaqusSurfaceMeshWriter::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid AbaqusSurfaceMeshWriter::getUuid() const
{
  return QUuid("{abbe2e1e-6fb2-5511-91f3-0637252f0705}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AbaqusSurfaceMeshWriter::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::OutputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AbaqusSurfaceMeshWriter::getHumanLabel() const
{
  return "Export Abaqus Surface Mesh";
}

// -----------------------------------------------------------------------------
AbaqusSurfaceMeshWriter::Pointer AbaqusSurfaceMeshWriter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<AbaqusSurfaceMeshWriter> AbaqusSurfaceMeshWriter::New()
{
  struct make_shared_enabler : public AbaqusSurfaceMeshWriter
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString AbaqusSurfaceMeshWriter::getNameOfClass() const
{
  return QString("AbaqusSurfaceMeshWriter");
}

// -----------------------------------------------------------------------------
QString AbaqusSurfaceMeshWriter::ClassName()
{
  return QString("AbaqusSurfaceMeshWriter");
}

// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::setOutputFile(const QString& value)
{
  m_OutputFile = value;
}

// -----------------------------------------------------------------------------
QString AbaqusSurfaceMeshWriter::getOutputFile() const
{
  return m_OutputFile;
}

// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::setSurfaceMeshFaceLabelsArrayPath(const DataArrayPath& value)
{
  m_SurfaceMeshFaceLabelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath AbaqusSurfaceMeshWriter::getSurfaceMeshFaceLabelsArrayPath() const
{
  return m_SurfaceMeshFaceLabelsArrayPath;
}
