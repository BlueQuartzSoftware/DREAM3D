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

#include <memory>

#include "GoldfeatherReader.h"

#include <QtCore/QFileInfo>

#include <QtCore/QTextStream>

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "ImportExport/ImportExportConstants.h"
#include "ImportExport/ImportExportVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,
  AttributeMatrixID22 = 22,

  DataContainerID = 1,
  DataArrayID30 = 30,
  DataArrayID31 = 31,
  DataArrayID32 = 32,
  DataArrayID33 = 33,
  DataArrayID34 = 34,
  DataArrayID35 = 35,
  DataArrayID36 = 36
};

/**
 * @brief The ScopedFileMonitor class will automatically close an open FILE pointer
 * when the object goes out of scope.
 */
class ScopedFileMonitor
{
public:
  ScopedFileMonitor(FILE* f)
  : m_File(f)
  {
  }
  virtual ~ScopedFileMonitor()
  {
    fclose(m_File);
  }

private:
  FILE* m_File;
  ScopedFileMonitor(const ScopedFileMonitor&); // Copy Constructor Not Implemented
  void operator=(const ScopedFileMonitor&);    // Move assignment Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GoldfeatherReader::GoldfeatherReader()
: m_SurfaceDataContainerName(SIMPL::Defaults::TriangleDataContainerName)
, m_VertexAttributeMatrixName(SIMPL::Defaults::VertexAttributeMatrixName)
, m_FaceAttributeMatrixName(SIMPL::Defaults::FaceAttributeMatrixName)
, m_SurfaceMeshPrincipalCurvature1sArrayName(SIMPL::FaceData::SurfaceMeshPrincipalCurvature1)
, m_SurfaceMeshPrincipalCurvature2sArrayName(SIMPL::FaceData::SurfaceMeshPrincipalCurvature2)
, m_SurfaceMeshPrincipalDirection1sArrayName(SIMPL::FaceData::SurfaceMeshPrincipalDirection1)
, m_SurfaceMeshPrincipalDirection2sArrayName(SIMPL::FaceData::SurfaceMeshPrincipalDirection2)
, m_SurfaceMeshNodeNormalsArrayName(SIMPL::VertexData::SurfaceMeshNodeNormals)
, m_SurfaceMeshFaceLabelsArrayName(SIMPL::FaceData::SurfaceMeshFaceLabels)
, m_SurfaceMeshFaceNormalsArrayName(SIMPL::FaceData::SurfaceMeshFaceNormals)
, m_InputFile("")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GoldfeatherReader::~GoldfeatherReader() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GoldfeatherReader::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  /*   For an input file use this code*/
  //  {
  //    FilterParameter::Pointer parameter = FilterParameter::New();
  //    parameter->setHumanLabel("Input File");
  //    parameter->setPropertyName("InputFile");
  //
  //    ////parameter->setValueType("QString");
  //    parameters.push_back(parameter);
  //  }
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Parameter, GoldfeatherReader, "*.jg"));

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Surface DataContainer", SurfaceDataContainerName, FilterParameter::CreatedArray, GoldfeatherReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("Vertex AttributeMatrix", VertexAttributeMatrixName, FilterParameter::CreatedArray, GoldfeatherReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("Face AttributeMatrix", FaceAttributeMatrixName, FilterParameter::CreatedArray, GoldfeatherReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("SurfaceMesh PrincipalCurvature 1 Array Name", SurfaceMeshPrincipalCurvature1sArrayName, FilterParameter::CreatedArray, GoldfeatherReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("SurfaceMesh PrincipalCurvature 2 Array Name", SurfaceMeshPrincipalCurvature2sArrayName, FilterParameter::CreatedArray, GoldfeatherReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("SurfaceMesh PrincipalDirection 1 Array Name", SurfaceMeshPrincipalDirection1sArrayName, FilterParameter::CreatedArray, GoldfeatherReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("SurfaceMesh PrincipalDirection 2 Array Name", SurfaceMeshPrincipalDirection2sArrayName, FilterParameter::CreatedArray, GoldfeatherReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("SurfaceMesh NodeNormals", SurfaceMeshNodeNormalsArrayName, FilterParameter::CreatedArray, GoldfeatherReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("SurfaceMesh FaceNormals", SurfaceMeshFaceNormalsArrayName, FilterParameter::CreatedArray, GoldfeatherReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("SurfaceMesh FaceLabels", SurfaceMeshFaceLabelsArrayName, FilterParameter::CreatedArray, GoldfeatherReader));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GoldfeatherReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputFile(reader->readString("InputFile", getInputFile()));
  setSurfaceDataContainerName(reader->readDataArrayPath("SurfaceDataContainerName", getSurfaceDataContainerName()));
  setVertexAttributeMatrixName(reader->readString("VertexAttributeMatrixName", getVertexAttributeMatrixName()));
  setFaceAttributeMatrixName(reader->readString("FaceAttributeMatrixName", getFaceAttributeMatrixName()));
  setSurfaceMeshPrincipalCurvature1sArrayName(reader->readString("SurfaceMeshPrincipalCurvature1sArrayName", getSurfaceMeshPrincipalCurvature1sArrayName()));
  setSurfaceMeshPrincipalCurvature2sArrayName(reader->readString("SurfaceMeshPrincipalCurvature2sArrayName", getSurfaceMeshPrincipalCurvature2sArrayName()));
  setSurfaceMeshPrincipalDirection1sArrayName(reader->readString("SurfaceMeshPrincipalDirection1sArrayName", getSurfaceMeshPrincipalDirection1sArrayName()));
  setSurfaceMeshPrincipalDirection2sArrayName(reader->readString("SurfaceMeshPrincipalDirection2sArrayName", getSurfaceMeshPrincipalDirection2sArrayName()));
  setSurfaceMeshNodeNormalsArrayName(reader->readString("SurfaceMeshNodeNormalsArrayName", getSurfaceMeshNodeNormalsArrayName()));
  setSurfaceMeshFaceNormalsArrayName(reader->readString("SurfaceMeshFaceNormalsArrayName", getSurfaceMeshFaceNormalsArrayName()));
  setSurfaceMeshFaceLabelsArrayName(reader->readString("SurfaceMeshFaceLabelsArrayName", getSurfaceMeshFaceLabelsArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GoldfeatherReader::updateVertexInstancePointers()
{
  clearErrorCode();
  clearWarningCode();

  if(nullptr != m_SurfaceMeshNodeNormalsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshNodeNormals = m_SurfaceMeshNodeNormalsPtr.lock()->getPointer(0);
  }                                                          /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_SurfaceMeshPrincipalCurvature1sPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalCurvature1s = m_SurfaceMeshPrincipalCurvature1sPtr.lock()->getPointer(0);
  }                                                          /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_SurfaceMeshPrincipalCurvature2sPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalCurvature2s = m_SurfaceMeshPrincipalCurvature2sPtr.lock()->getPointer(0);
  }                                                          /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_SurfaceMeshPrincipalDirection1sPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalDirection1s = m_SurfaceMeshPrincipalDirection1sPtr.lock()->getPointer(0);
  }                                                          /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_SurfaceMeshPrincipalDirection2sPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalDirection2s = m_SurfaceMeshPrincipalDirection2sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GoldfeatherReader::updateFaceInstancePointers()
{
  clearErrorCode();
  clearWarningCode();

  if(nullptr != m_SurfaceMeshFaceLabelsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  }                                                 /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_SurfaceMeshFaceNormalsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GoldfeatherReader::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GoldfeatherReader::dataCheck()
{
  DataArrayPath tempPath;

  DataContainer::Pointer sm = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getSurfaceDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }
  std::vector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer vertAttrMat = sm->createNonPrereqAttributeMatrix(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex, AttributeMatrixID21);
  if(getErrorCode() < 0 || nullptr == vertAttrMat.get())
  {
    return;
  }
  AttributeMatrix::Pointer faceAttrMat = sm->createNonPrereqAttributeMatrix(this, getFaceAttributeMatrixName(), tDims, AttributeMatrix::Type::Face, AttributeMatrixID22);
  if(getErrorCode() < 0 || nullptr == faceAttrMat.get())
  {
    return;
  }

  QFileInfo fi(getInputFile());
  if(getInputFile().isEmpty())
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387, ss);
  }
  else if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388, ss);
  }

  SharedVertexList::Pointer sharedVertList = TriangleGeom::CreateSharedVertexList(0);
  TriangleGeom::Pointer triangleGeom = TriangleGeom::CreateGeometry(0, sharedVertList, SIMPL::Geometry::TriangleGeometry, !getInPreflight());

  sm->setGeometry(triangleGeom);

  const QString surfaceDCName = getSurfaceDataContainerName().getDataContainerName();

  std::vector<size_t> dims(1, 3);
  tempPath.update(surfaceDCName, getVertexAttributeMatrixName(), getSurfaceMeshNodeNormalsArrayName());
  m_SurfaceMeshNodeNormalsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, tempPath, 0, dims, "", DataArrayID30);  /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshNodeNormalsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshNodeNormals = m_SurfaceMeshNodeNormalsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 1;
  tempPath.update(surfaceDCName, getVertexAttributeMatrixName(), getSurfaceMeshPrincipalCurvature1sArrayName());
  m_SurfaceMeshPrincipalCurvature1sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, tempPath, 0, dims, "", DataArrayID31);           /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshPrincipalCurvature1sPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalCurvature1s = m_SurfaceMeshPrincipalCurvature1sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(surfaceDCName, getVertexAttributeMatrixName(), getSurfaceMeshPrincipalCurvature2sArrayName());
  m_SurfaceMeshPrincipalCurvature2sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, tempPath, 0, dims, "", DataArrayID32);           /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshPrincipalCurvature2sPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalCurvature2s = m_SurfaceMeshPrincipalCurvature2sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 3;
  tempPath.update(surfaceDCName, getVertexAttributeMatrixName(), getSurfaceMeshPrincipalDirection1sArrayName());
  m_SurfaceMeshPrincipalDirection1sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, tempPath, 0, dims, "", DataArrayID33);           /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshPrincipalDirection1sPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalDirection1s = m_SurfaceMeshPrincipalDirection1sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(surfaceDCName, getVertexAttributeMatrixName(), getSurfaceMeshPrincipalDirection2sArrayName());
  m_SurfaceMeshPrincipalDirection2sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, tempPath, 0, dims, "", DataArrayID34);           /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshPrincipalDirection2sPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalDirection2s = m_SurfaceMeshPrincipalDirection2sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 2;
  tempPath.update(surfaceDCName, getFaceAttributeMatrixName(), getSurfaceMeshFaceLabelsArrayName());
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, dims, "", DataArrayID35); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceLabelsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 3;
  tempPath.update(surfaceDCName, getFaceAttributeMatrixName(), getSurfaceMeshFaceNormalsArrayName());
  m_SurfaceMeshFaceNormalsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, tempPath, 0, dims, "", DataArrayID36);  /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceNormalsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GoldfeatherReader::preflight()
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
void GoldfeatherReader::execute()
{
  clearErrorCode();
  clearWarningCode();

  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceDataContainerName());

  FILE* f = fopen(m_InputFile.toLatin1().data(), "r");
  if(nullptr == f)
  {
    setErrorCondition(-999, "Error opening Input file");
    return;
  }
  ScopedFileMonitor fileMonitor(f);

  int nNodes = 0;
  fscanf(f, "%d\n", &nNodes);

  // Allocate the Nodes, Normals, curvatures and principal direction vectors
  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();
  triangleGeom->resizeVertexList(nNodes);
  triangleGeom->initializeWithZeros();
  float* nodes = triangleGeom->getVertexPointer(0);

  std::vector<size_t> tDims(1, nNodes);
  sm->getAttributeMatrix(getVertexAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateVertexInstancePointers();

  float x, y, z, n0, n1, n2, p1, p2;
  for(int n = 0; n < nNodes; ++n)
  {
    fscanf(f, "%f %f %f %f %f %f %f %f\n", &x, &y, &z, &n0, &n1, &n2, &p1, &p2);
    nodes[n * 3] = x;
    nodes[n * 3 + 1] = y;
    nodes[n * 3 + 2] = z;
    m_SurfaceMeshNodeNormals[n * 3 + 0] = n0;
    m_SurfaceMeshNodeNormals[n * 3 + 1] = n1;
    m_SurfaceMeshNodeNormals[n * 3 + 2] = n2;
    m_SurfaceMeshPrincipalCurvature1s[n] = p1;
    m_SurfaceMeshPrincipalCurvature2s[n] = p2;

    // Read the next line of the data which is the principal direction vectors
    fscanf(f, "%f %f %f %f %f %f\n", &x, &y, &z, &n0, &n1, &n2);
    m_SurfaceMeshPrincipalDirection1s[n * 3 + 0] = x;
    m_SurfaceMeshPrincipalDirection1s[n * 3 + 1] = y;
    m_SurfaceMeshPrincipalDirection1s[n * 3 + 2] = z;

    m_SurfaceMeshPrincipalDirection2s[n * 3 + 0] = n0;
    m_SurfaceMeshPrincipalDirection2s[n * 3 + 1] = n1;
    m_SurfaceMeshPrincipalDirection2s[n * 3 + 2] = n2;
  }

  int nTriangles = 0;
  int err = fscanf(f, "%d\n", &nTriangles);
  if(err < 0)
  {
    setErrorCondition(-876, "Error Reading the number of Triangles from the file.");
    return;
  }

  triangleGeom->resizeTriList(nTriangles);
  MeshIndexType* triangles = triangleGeom->getTriPointer(0);

  tDims[0] = nTriangles;
  sm->getAttributeMatrix(getFaceAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateFaceInstancePointers();

  for(int t = 0; t < nTriangles; ++t)
  {
    fscanf(f, "%f %f %f %f %f %f", &x, &y, &z, &n0, &n1, &n2);
    triangles[t * 3] = x;
    triangles[t * 3 + 1] = y;
    triangles[t * 3 + 2] = z;
    //  triangles[t].tIndex = t;
    m_SurfaceMeshFaceLabels[t * 2] = 0;
    m_SurfaceMeshFaceLabels[t * 2 + 1] = 1;
    m_SurfaceMeshFaceNormals[t * 3 + 0] = n0;
    m_SurfaceMeshFaceNormals[t * 3 + 1] = n1;
    m_SurfaceMeshFaceNormals[t * 3 + 2] = n2;
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GoldfeatherReader::newFilterInstance(bool copyFilterParameters) const
{
  GoldfeatherReader::Pointer filter = GoldfeatherReader::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GoldfeatherReader::getCompiledLibraryName() const
{
  return ImportExportConstants::ImportExportBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GoldfeatherReader::getBrandingString() const
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GoldfeatherReader::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << ImportExport::Version::Major() << "." << ImportExport::Version::Minor() << "." << ImportExport::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GoldfeatherReader::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid GoldfeatherReader::getUuid() const
{
  return QUuid("{0b285734-f268-5c81-aeb0-8bfe7ad73f1f}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GoldfeatherReader::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MiscFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GoldfeatherReader::getHumanLabel() const
{
  return "Goldfeather Mesh Importer";
}

// -----------------------------------------------------------------------------
GoldfeatherReader::Pointer GoldfeatherReader::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<GoldfeatherReader> GoldfeatherReader::New()
{
  struct make_shared_enabler : public GoldfeatherReader
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString GoldfeatherReader::getNameOfClass() const
{
  return QString("GoldfeatherReader");
}

// -----------------------------------------------------------------------------
QString GoldfeatherReader::ClassName()
{
  return QString("GoldfeatherReader");
}

// -----------------------------------------------------------------------------
void GoldfeatherReader::setSurfaceDataContainerName(const DataArrayPath& value)
{
  m_SurfaceDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GoldfeatherReader::getSurfaceDataContainerName() const
{
  return m_SurfaceDataContainerName;
}

// -----------------------------------------------------------------------------
void GoldfeatherReader::setVertexAttributeMatrixName(const QString& value)
{
  m_VertexAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString GoldfeatherReader::getVertexAttributeMatrixName() const
{
  return m_VertexAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void GoldfeatherReader::setFaceAttributeMatrixName(const QString& value)
{
  m_FaceAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString GoldfeatherReader::getFaceAttributeMatrixName() const
{
  return m_FaceAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void GoldfeatherReader::setSurfaceMeshPrincipalCurvature1sArrayName(const QString& value)
{
  m_SurfaceMeshPrincipalCurvature1sArrayName = value;
}

// -----------------------------------------------------------------------------
QString GoldfeatherReader::getSurfaceMeshPrincipalCurvature1sArrayName() const
{
  return m_SurfaceMeshPrincipalCurvature1sArrayName;
}

// -----------------------------------------------------------------------------
void GoldfeatherReader::setSurfaceMeshPrincipalCurvature2sArrayName(const QString& value)
{
  m_SurfaceMeshPrincipalCurvature2sArrayName = value;
}

// -----------------------------------------------------------------------------
QString GoldfeatherReader::getSurfaceMeshPrincipalCurvature2sArrayName() const
{
  return m_SurfaceMeshPrincipalCurvature2sArrayName;
}

// -----------------------------------------------------------------------------
void GoldfeatherReader::setSurfaceMeshPrincipalDirection1sArrayName(const QString& value)
{
  m_SurfaceMeshPrincipalDirection1sArrayName = value;
}

// -----------------------------------------------------------------------------
QString GoldfeatherReader::getSurfaceMeshPrincipalDirection1sArrayName() const
{
  return m_SurfaceMeshPrincipalDirection1sArrayName;
}

// -----------------------------------------------------------------------------
void GoldfeatherReader::setSurfaceMeshPrincipalDirection2sArrayName(const QString& value)
{
  m_SurfaceMeshPrincipalDirection2sArrayName = value;
}

// -----------------------------------------------------------------------------
QString GoldfeatherReader::getSurfaceMeshPrincipalDirection2sArrayName() const
{
  return m_SurfaceMeshPrincipalDirection2sArrayName;
}

// -----------------------------------------------------------------------------
void GoldfeatherReader::setSurfaceMeshNodeNormalsArrayName(const QString& value)
{
  m_SurfaceMeshNodeNormalsArrayName = value;
}

// -----------------------------------------------------------------------------
QString GoldfeatherReader::getSurfaceMeshNodeNormalsArrayName() const
{
  return m_SurfaceMeshNodeNormalsArrayName;
}

// -----------------------------------------------------------------------------
void GoldfeatherReader::setSurfaceMeshFaceLabelsArrayName(const QString& value)
{
  m_SurfaceMeshFaceLabelsArrayName = value;
}

// -----------------------------------------------------------------------------
QString GoldfeatherReader::getSurfaceMeshFaceLabelsArrayName() const
{
  return m_SurfaceMeshFaceLabelsArrayName;
}

// -----------------------------------------------------------------------------
void GoldfeatherReader::setSurfaceMeshFaceNormalsArrayName(const QString& value)
{
  m_SurfaceMeshFaceNormalsArrayName = value;
}

// -----------------------------------------------------------------------------
QString GoldfeatherReader::getSurfaceMeshFaceNormalsArrayName() const
{
  return m_SurfaceMeshFaceNormalsArrayName;
}

// -----------------------------------------------------------------------------
void GoldfeatherReader::setInputFile(const QString& value)
{
  m_InputFile = value;
}

// -----------------------------------------------------------------------------
QString GoldfeatherReader::getInputFile() const
{
  return m_InputFile;
}
