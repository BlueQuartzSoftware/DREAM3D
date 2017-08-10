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

#include "GoldfeatherReader.h"

#include <QtCore/QFileInfo>

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "IO/IOConstants.h"
#include "IO/IOVersion.h"

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
  void operator=(const ScopedFileMonitor&);    // Operator '=' Not Implemented
};

// Include the MOC generated file for this class
#include "moc_GoldfeatherReader.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GoldfeatherReader::GoldfeatherReader()
: AbstractFilter()
, m_SurfaceDataContainerName(SIMPL::Defaults::TriangleDataContainerName)
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
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GoldfeatherReader::~GoldfeatherReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GoldfeatherReader::setupFilterParameters()
{
  FilterParameterVector parameters;
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

  parameters.push_back(SIMPL_NEW_STRING_FP("Surface DataContainer", SurfaceDataContainerName, FilterParameter::CreatedArray, GoldfeatherReader));
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
  setSurfaceDataContainerName(reader->readString("SurfaceDataContainerName", getSurfaceDataContainerName()));
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
  setErrorCondition(0);
  setWarningCondition(0);

  if(nullptr != m_SurfaceMeshNodeNormalsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshNodeNormals = m_SurfaceMeshNodeNormalsPtr.lock()->getPointer(0);
  }                                                                /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_SurfaceMeshPrincipalCurvature1sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalCurvature1s = m_SurfaceMeshPrincipalCurvature1sPtr.lock()->getPointer(0);
  }                                                                /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_SurfaceMeshPrincipalCurvature2sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalCurvature2s = m_SurfaceMeshPrincipalCurvature2sPtr.lock()->getPointer(0);
  }                                                                /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_SurfaceMeshPrincipalDirection1sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalDirection1s = m_SurfaceMeshPrincipalDirection1sPtr.lock()->getPointer(0);
  }                                                                /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_SurfaceMeshPrincipalDirection2sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalDirection2s = m_SurfaceMeshPrincipalDirection2sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GoldfeatherReader::updateFaceInstancePointers()
{
  setErrorCondition(0);
  setWarningCondition(0);

  if(nullptr != m_SurfaceMeshFaceLabelsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  }                                                       /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_SurfaceMeshFaceNormalsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
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

  DataContainer::Pointer sm = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getSurfaceDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer vertAttrMat = sm->createNonPrereqAttributeMatrix<AbstractFilter>(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex);
  if(getErrorCondition() < 0 || nullptr == vertAttrMat.get())
  {
    return;
  }
  AttributeMatrix::Pointer faceAttrMat = sm->createNonPrereqAttributeMatrix<AbstractFilter>(this, getFaceAttributeMatrixName(), tDims, AttributeMatrix::Type::Face);
  if(getErrorCondition() < 0 || nullptr == faceAttrMat.get())
  {
    return;
  }

  QFileInfo fi(getInputFile());
  if(getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if(fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  SharedVertexList::Pointer sharedVertList = TriangleGeom::CreateSharedVertexList(0);
  TriangleGeom::Pointer triangleGeom = TriangleGeom::CreateGeometry(0, sharedVertList, SIMPL::Geometry::TriangleGeometry, !getInPreflight());

  sm->setGeometry(triangleGeom);

  QVector<size_t> dims(1, 3);
  tempPath.update(getSurfaceDataContainerName(), getVertexAttributeMatrixName(), getSurfaceMeshNodeNormalsArrayName());
  m_SurfaceMeshNodeNormalsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, tempPath, 0, dims);                           /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshNodeNormalsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshNodeNormals = m_SurfaceMeshNodeNormalsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 1;
  tempPath.update(getSurfaceDataContainerName(), getVertexAttributeMatrixName(), getSurfaceMeshPrincipalCurvature1sArrayName());
  m_SurfaceMeshPrincipalCurvature1sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, tempPath, 0, dims);                                    /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshPrincipalCurvature1sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalCurvature1s = m_SurfaceMeshPrincipalCurvature1sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getSurfaceDataContainerName(), getVertexAttributeMatrixName(), getSurfaceMeshPrincipalCurvature2sArrayName());
  m_SurfaceMeshPrincipalCurvature2sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, tempPath, 0, dims);                                    /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshPrincipalCurvature2sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalCurvature2s = m_SurfaceMeshPrincipalCurvature2sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 3;
  tempPath.update(getSurfaceDataContainerName(), getVertexAttributeMatrixName(), getSurfaceMeshPrincipalDirection1sArrayName());
  m_SurfaceMeshPrincipalDirection1sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, tempPath, 0, dims);                                    /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshPrincipalDirection1sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalDirection1s = m_SurfaceMeshPrincipalDirection1sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getSurfaceDataContainerName(), getVertexAttributeMatrixName(), getSurfaceMeshPrincipalDirection2sArrayName());
  m_SurfaceMeshPrincipalDirection2sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, tempPath, 0, dims);                                    /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshPrincipalDirection2sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalDirection2s = m_SurfaceMeshPrincipalDirection2sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 2;
  tempPath.update(getSurfaceDataContainerName(), getFaceAttributeMatrixName(), getSurfaceMeshFaceLabelsArrayName());
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, dims);                          /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceLabelsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 3;
  tempPath.update(getSurfaceDataContainerName(), getFaceAttributeMatrixName(), getSurfaceMeshFaceNormalsArrayName());
  m_SurfaceMeshFaceNormalsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, tempPath, 0, dims);                           /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceNormalsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
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
  int err = 0;
  QString ss;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceDataContainerName());

  FILE* f = fopen(m_InputFile.toLatin1().data(), "r");
  if(nullptr == f)
  {
    setErrorCondition(-999);
    notifyErrorMessage(getHumanLabel(), "Error opening Input file", getErrorCondition());
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

  QVector<size_t> tDims(1, nNodes);
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
  err = fscanf(f, "%d\n", &nTriangles);
  if(err < 0)
  {
    setErrorCondition(-876);
    notifyErrorMessage(getHumanLabel(), "Error Reading the number of Triangles from the file.", getErrorCondition());
    return;
  }

  triangleGeom->resizeTriList(nTriangles);
  int64_t* triangles = triangleGeom->getTriPointer(0);

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

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GoldfeatherReader::newFilterInstance(bool copyFilterParameters)
{
  GoldfeatherReader::Pointer filter = GoldfeatherReader::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GoldfeatherReader::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GoldfeatherReader::getBrandingString()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GoldfeatherReader::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << IO::Version::Major() << "." << IO::Version::Minor() << "." << IO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GoldfeatherReader::getGroupName()
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GoldfeatherReader::getSubGroupName()
{
  return SIMPL::FilterSubGroups::MiscFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GoldfeatherReader::getHumanLabel()
{
  return "Goldfeather Mesh Importer";
}
