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

#include "BinaryNodesTrianglesReader.h"

#include <QtCore/QtDebug>
#include <QtCore/QString>
#include <sstream>


#include "SIMPLib/Common/ScopedFileMonitor.hpp"
#include "SIMPLib/Geometry/MeshStructs.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "BinaryNodesTrianglesReader.h"

// Include the MOC generated file for this class
#include "moc_BinaryNodesTrianglesReader.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BinaryNodesTrianglesReader::BinaryNodesTrianglesReader() :
  SurfaceMeshFilter(),
  m_SurfaceDataContainerName(SIMPL::Defaults::TriangleDataContainerName),
  m_VertexAttributeMatrixName(SIMPL::Defaults::VertexAttributeMatrixName),
  m_FaceAttributeMatrixName(SIMPL::Defaults::FaceAttributeMatrixName),
  m_FaceLabelsArrayName(SIMPL::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshNodeTypesArrayName(SIMPL::VertexData::SurfaceMeshNodeType),
  m_BinaryNodesFile(""),
  m_BinaryTrianglesFile(""),
  m_SurfaceMeshNodeTypes(NULL),
  m_FaceLabels(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BinaryNodesTrianglesReader::~BinaryNodesTrianglesReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BinaryNodesTrianglesReader::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(InputFileFilterParameter::New("Binary Nodes Input File", "BinaryNodesFile", getBinaryNodesFile(), FilterParameter::Parameter, SIMPL_BIND_SETTER(BinaryNodesTrianglesReader, this, BinaryNodesFile), SIMPL_BIND_GETTER(BinaryNodesTrianglesReader, this, BinaryNodesFile), "*.raw", "Raw Files"));
  parameters.push_back(InputFileFilterParameter::New("Binary Triangles Input File", "BinaryTrianglesFile", getBinaryTrianglesFile(), FilterParameter::Parameter, SIMPL_BIND_SETTER(BinaryNodesTrianglesReader, this, BinaryTrianglesFile), SIMPL_BIND_GETTER(BinaryNodesTrianglesReader, this, BinaryTrianglesFile), "*.raw", "Raw Files"));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BinaryNodesTrianglesReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BinaryNodesTrianglesReader::updateVertexInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_SurfaceMeshNodeTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshNodeTypes = m_SurfaceMeshNodeTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BinaryNodesTrianglesReader::updateFaceInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_FaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FaceLabels = m_FaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BinaryNodesTrianglesReader::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BinaryNodesTrianglesReader::dataCheck()
{
  if (getBinaryNodesFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Binary Nodes File path set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (getBinaryNodesFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Binary Nodes File path set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  DataArrayPath tempPath;

  QVector<size_t> dims(1, 1);

  DataContainer::Pointer sm = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getSurfaceDataContainerName());
  if(getErrorCondition() < 0) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer vertexAttrMat = sm->createNonPrereqAttributeMatrix<AbstractFilter>(this, getVertexAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::Vertex);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer faceAttrMat = sm->createNonPrereqAttributeMatrix<AbstractFilter>(this, getFaceAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::Face);
  if(getErrorCondition() < 0) { return; }

  SharedVertexList::Pointer vertices = TriangleGeom::CreateSharedVertexList(0);
  TriangleGeom::Pointer triangleGeom = TriangleGeom::CreateGeometry(0, vertices, SIMPL::Geometry::TriangleGeometry, !getInPreflight());
  sm->setGeometry(triangleGeom);

  dims[0] = 2;
  tempPath.update(getSurfaceDataContainerName(), getFaceAttributeMatrixName(), getFaceLabelsArrayName() );
  m_FaceLabelsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FaceLabels = m_FaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  tempPath.update(getSurfaceDataContainerName(), getVertexAttributeMatrixName(), getSurfaceMeshNodeTypesArrayName() );
  m_SurfaceMeshNodeTypesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshNodeTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshNodeTypes = m_SurfaceMeshNodeTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BinaryNodesTrianglesReader::preflight()
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
void BinaryNodesTrianglesReader::execute()
{
  int err = 0;
  QString ss;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  /* Place all your code to execute your filter here. */
  err = read();
  setErrorCondition(err);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BinaryNodesTrianglesReader::read()
{
  int err = 0;
  setErrorCondition(err);

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceDataContainerName());
  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();
  AttributeMatrix::Pointer vertAttrMat = sm->getAttributeMatrix(getVertexAttributeMatrixName());
  AttributeMatrix::Pointer faceAttrMat = sm->getAttributeMatrix(getFaceAttributeMatrixName());

  // Open the Nodes file for reading
  FILE* nodesFile = fopen(m_BinaryNodesFile.toLatin1().data(), "rb+");
  if(nodesFile == NULL)
  {
    QString ss = QObject::tr("Error opening nodes file '%1'").arg(m_BinaryNodesFile);
    setErrorCondition(786);
    return getErrorCondition();
  }
  ScopedFileMonitor nodesMonitor(nodesFile);

  // Calculate how many nodes are in the file based on the file size
  fseek(nodesFile, 0, SEEK_END);
  size_t fLength = ftell(nodesFile);
  size_t nNodes = fLength / SurfaceMesh::NodesFile::ByteCount;
  fseek(nodesFile, 0, SEEK_SET);
  fLength = ftell(nodesFile);
  if(0 != fLength)
  {
    QString ss = QObject::tr("%1: Error Could not rewind to beginning of file after nodes count.'%2'").arg(getNameOfClass()).arg(m_BinaryNodesFile);
    setErrorCondition(787);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return getErrorCondition();
  }
  {
    QString ss = QObject::tr("Calc Node Count from Nodes.bin File: ").arg(nNodes);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  }
  // Open the triangles file for reading
  FILE* triFile = fopen(m_BinaryTrianglesFile.toLatin1().data(), "rb+");
  if(triFile == NULL)
  {
    QString ss = QObject::tr("%1: Error opening Triangles file '%2'").arg(getNameOfClass()).arg(m_BinaryTrianglesFile);
    setErrorCondition(788);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return getErrorCondition();
  }

  ScopedFileMonitor trianglesMonitor(triFile);
  // Calculate how many Triangles are in the file based in the file size
  fseek(triFile, 0, SEEK_END);
  fLength = ftell(triFile);
  size_t nTriangles = fLength / SurfaceMesh::TrianglesFile::ByteCount;
  fseek(triFile, 0, SEEK_SET);
  fLength = ftell(triFile);
  if(0 != fLength)
  {
    QString ss = QObject::tr("%1: Error Could not rewind to beginning of file after triangles count.'%2'").arg(getNameOfClass()).arg(m_BinaryTrianglesFile);
    setErrorCondition(789);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return getErrorCondition();
  }

  {
    QString ss = QObject::tr("Calc Triangle Count from Triangles.bin File: %1").arg(nTriangles);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  }

  // Allocate all the nodes
  triangleGeom->resizeVertexList(nNodes);
  float* m_NodeList = triangleGeom->getVertexPointer(0);

  QVector<size_t> tDims(1, nNodes);
  vertAttrMat->resizeAttributeArrays(tDims);
  updateVertexInstancePointers();
  {
    QString ss  = QObject::tr("Reading Nodes file into Memory");
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  }
  size_t nread = 0;
  SurfaceMesh::NodesFile::NodesFileRecord_t nRecord;

  for (size_t i = 0; i < nNodes; i++)
  {
    nread = fread(&nRecord, SurfaceMesh::NodesFile::ByteCount, 1, nodesFile); // Read one set of positions from the nodes file
    if(nread != 1)
    {
      break;
    }
    m_NodeList[i * 3] = nRecord.x;
    m_NodeList[i * 3 + 1] = nRecord.y;
    m_NodeList[i * 3 + 2] = nRecord.z;
    m_SurfaceMeshNodeTypes[nRecord.nodeId] = nRecord.nodeKind;
  }

  {
    QString ss = QObject::tr("Reading Triangles file into Memory");
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  }

  // Allocate all the Triangle Objects
  triangleGeom->resizeTriList(nTriangles);
  triangleGeom->getTriangles()->initializeWithValue(0xABABABABABABABAB);
  int64_t* m_TriangleList = triangleGeom->getTriPointer(0);

  tDims[0] = nTriangles;
  faceAttrMat->resizeAttributeArrays(tDims);
  updateFaceInstancePointers();

  SurfaceMesh::TrianglesFile::TrianglesFileRecord_t tRecord;
  for (size_t i = 0; i < nTriangles; i++)
  {
    // Read from the Input Triangles Temp File
    nread = fread(&tRecord, SurfaceMesh::TrianglesFile::ByteCount, 1, triFile);
    if(nread != 1)
    {
      break;
    }
    m_TriangleList[i * 3] = tRecord.nodeId_0;
    m_TriangleList[i * 3 + 1] = tRecord.nodeId_1;
    m_TriangleList[i * 3 + 2] = tRecord.nodeId_2;
    m_FaceLabels[tRecord.triId * 2] = tRecord.label_0;
    m_FaceLabels[tRecord.triId * 2 + 1] = tRecord.label_1;
  }

  // The ScopedFileMonitor classes will take care of closing the files

  return getErrorCondition();
}
