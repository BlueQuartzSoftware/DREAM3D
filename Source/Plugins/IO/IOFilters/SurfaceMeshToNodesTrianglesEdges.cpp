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


#include "SurfaceMeshToNodesTrianglesEdges.h"


#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/FileSystemFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"

#include "IO/IOConstants.h"


#define WRITE_EDGES_FILE 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshToNodesTrianglesEdges::SurfaceMeshToNodesTrianglesEdges() :
  AbstractFilter(),
  m_OutputNodesFile(""),
  m_OutputEdgesFile(""),
  m_OutputTrianglesFile(""),
  m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshNodeTypeArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::VertexAttributeMatrixName, DREAM3D::VertexData::SurfaceMeshNodeType),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshNodeTypeArrayName(DREAM3D::VertexData::SurfaceMeshNodeType),
  m_SurfaceMeshNodeType(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshToNodesTrianglesEdges::~SurfaceMeshToNodesTrianglesEdges()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNodesTrianglesEdges::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FileSystemFilterParameter::New("Output Nodes File", "OutputNodesFile", FilterParameterWidgetType::OutputFileWidget, getOutputNodesFile(), FilterParameter::Uncategorized));
  parameters.push_back(FileSystemFilterParameter::New("Output Triangles File", "OutputTrianglesFile", FilterParameterWidgetType::OutputFileWidget, getOutputTrianglesFile(), FilterParameter::Uncategorized));
#if WRITE_EDGES_FILE
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Output Edges File");
    parameter->setPropertyName("OutputEdgesFile");
    parameter->setWidgetType(FilterParameterWidgetType::OutputFileWidget);
    //parameter->setValueType("QString");
    parameters.push_back(parameter);
  }
#endif

  parameters.push_back(SeparatorFilterParameter::New("Required Information", FilterParameter::Uncategorized));
  parameters.push_back(FilterParameter::New("SurfaceMeshFaceLabels", "SurfaceMeshFaceLabelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFaceLabelsArrayPath(), FilterParameter::Uncategorized, ""));
  parameters.push_back(FilterParameter::New("SurfaceMeshNodeType", "SurfaceMeshNodeTypeArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshNodeTypeArrayPath(), FilterParameter::Uncategorized, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNodesTrianglesEdges::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshNodeTypeArrayPath(reader->readDataArrayPath("SurfaceMeshNodeTypeArrayPath", getSurfaceMeshNodeTypeArrayPath() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  setOutputNodesFile( reader->readString( "OutputNodesFile", getOutputNodesFile() ) );
  setOutputTrianglesFile( reader->readString( "OutputTrianglesFile", getOutputTrianglesFile() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshToNodesTrianglesEdges::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshNodeTypeArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputNodesFile)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputEdgesFile)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputTrianglesFile)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNodesTrianglesEdges::dataCheck()
{
  setErrorCondition(0);

  if(true == m_OutputNodesFile.isEmpty())
  {
    setErrorCondition(-380);
    notifyErrorMessage(getHumanLabel(), "The output Nodes file needs to be set", getErrorCondition());
  }
#if WRITE_EDGES_FILE
  if(true == m_OutputEdgesFile.isEmpty())
  {
    setErrorCondition(-381);
    notifyErrorMessage(getHumanLabel(), "The output Edges file needs to be set", getErrorCondition());
  }
#endif
  if(true == m_OutputTrianglesFile.isEmpty())
  {
    setErrorCondition(-382);
    notifyErrorMessage(getHumanLabel(), "The output Triangles file needs to be set", getErrorCondition());
  }

  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

  TriangleGeom::Pointer triangles =  sm->getPrereqGeometry<TriangleGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0) { return; }

  // We MUST have Nodes
  if (NULL == triangles->getVertices().get())
  {
    setErrorCondition(-386);
    notifyErrorMessage(getHumanLabel(), "DataContainer Geometry missing Vertices", getErrorCondition());
  }
  // We MUST have Triangles defined also.
  if (NULL == triangles->getTriangles().get())
  {
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), "DataContainer Geometry missing Triangles", getErrorCondition());
  }

#if WRITE_EDGES_FILE
  IDataArray::Pointer edges = sm->getPointData(DREAM3D::CellData::SurfaceMeshEdges);
  if (edges.get() == NULL)
  {
    setErrorCondition(-385);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing DREAM3D::CellData::SurfaceMeshEdges Information", getErrorCondition());
  }
#endif
  QVector<size_t> dims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_SurfaceMeshNodeTypePtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, getSurfaceMeshNodeTypeArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshNodeTypePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshNodeType = m_SurfaceMeshNodeTypePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNodesTrianglesEdges::preflight()
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
void SurfaceMeshToNodesTrianglesEdges::execute()
{
  int err = 0;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName());

  TriangleGeom::Pointer triangleGeom = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName())->getGeometryAs<TriangleGeom>();
  float* nodes = triangleGeom->getVertexPointer(0);
  int64_t* triangles = triangleGeom->getTriPointer(0);

  qint64 numNodes = triangleGeom->getNumberOfVertices();
  int64_t numTriangles = triangleGeom->getNumberOfTris();

#if WRITE_EDGES_FILE
  IDataArray::Pointer edges = sm->getPointData(DREAM3D::CellData::SurfaceMeshEdges);
  IDataArray::Pointer iEdges = sm->getPointData(DREAM3D::CellData::SurfaceMeshInternalEdges);
#endif

  // ++++++++++++++ Write the Nodes File +++++++++++++++++++++++++++++++++++++++++++
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  notifyStatusMessage(getHumanLabel(), "Writing Nodes Text File");
  QFileInfo fi(getOutputNodesFile());
  QDir parentPath = fi.path();

  if(!parentPath.mkpath("."))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }
  FILE* nodesFile = NULL;
  nodesFile = fopen(getOutputNodesFile().toLatin1().data(), "wb");
  if (NULL == nodesFile)
  {
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), "Error opening Nodes file for writing", -100);
    return;
  }

  fprintf(nodesFile, "%lld\n", numNodes);
  for (int i = 0; i < numNodes; i++)
  {
    fprintf(nodesFile, "%10d    %3d    %8.4f %8.4f %8.4f\n", i, static_cast<int>(m_SurfaceMeshNodeType[i]), nodes[i*3], nodes[i*3+1], nodes[i*3+2]);
  }
  fclose(nodesFile);

#if WRITE_EDGES_FILE
  // ++++++++++++++ Write the Edges File +++++++++++++++++++++++++++++++++++++++++++
  notifyStatusMessage(getHumanLabel(), "Writing Edges Text File");
  parentPath = QFileInfo::parentPath(getOutputEdgesFile());
  QDir dir;
  if(!dir.mkpath(parentPath))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }
  FILE* eFile = fopen(getOutputEdgesFile().toLatin1().data(), "wb");
  if (NULL == eFile)
  {
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), "Error opening Edges file for writing", -100);
    return;
  }

  size_t nfe = edges->getNumberOfTuples();
  size_t nie = iEdges->getNumberOfTuples();
  int kind = 0;
  int newnid1 = 0, newnid2 = 0;
  StructArray<SurfaceMesh::M3C::Segment>* faceEdges = StructArray<SurfaceMesh::M3C::Segment>::SafePointerDownCast(edges.get());
  SurfaceMesh::M3C::Segment* fe = faceEdges->getPointer(0);
  fprintf(eFile, "%lu\n", nfe + nie);

  for (size_t k = 0; k < nfe; ++k)
  {

    kind = fe[k].edgeKind;
    newnid1 = fe[k].node_id[0];
    newnid2 = fe[k].node_id[1];

    fprintf(eFile, "%10lu    %8d %8d    %6d    %6d %6d %6d %6d\n", k, newnid1, newnid2, kind, fe[k].nSpin[0], fe[k].nSpin[1], 0, 0);
  }

  StructArray<SurfaceMesh::M3C::ISegment>* internalEdges = StructArray<SurfaceMesh::M3C::ISegment>::SafePointerDownCast(iEdges.get());
  SurfaceMesh::M3C::ISegment* ie = internalEdges->getPointer(0);
  for (size_t kk = 0; kk < nie; ++kk)
  {

    kind = ie[kk].edgeKind;
    newnid1 = ie[kk].node_id[0];
    newnid2 = ie[kk].node_id[1];
    fprintf(eFile, "%10lu    %8d %8d    %6d    %6d %6d %6d %6d\n", kk + nfe, newnid1, newnid2, kind, ie[kk].nSpin[0], ie[kk].nSpin[1], ie[kk].nSpin[2], ie[kk].nSpin[3]);
  }

  fclose(eFile);
#endif

  // ++++++++++++++ Write the Triangles File +++++++++++++++++++++++++++++++++++++++++++
  notifyStatusMessage(getHumanLabel(), "Writing Triangles Text File");
  QFileInfo triFI(getOutputTrianglesFile());
  parentPath = triFI.path();
  if(!parentPath.mkpath("."))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }
  FILE* triFile = fopen(getOutputTrianglesFile().toLatin1().data(), "wb");
  if (NULL == triFile)
  {
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), "Error opening Triangles file for writing", -100);
    return;
  }

  fprintf(triFile, "%lld\n", (long long int)(numTriangles));

  int n1, n2, n3, e1 = -1, e2 = -1, e3 = -1;
  for (int64_t j = 0; j < numTriangles; ++j)
  {
    n1 = triangles[j*3];
    n2 = triangles[j*3+1];
    n3 = triangles[j*3+2];

#if WRITE_EDGES_FILE
    e1 = t[j].e_id[0];
    e2 = t[j].e_id[1];
    e3 = t[j].e_id[2];
    <<< <<< <<< < #error FIX THIS CODE BELOW. WE PROBABLY NEED TO GENERATE THE EDGE INFORMATION IF NEEDED
#endif

    fprintf(triFile, "%10lld    %10d %10d %10d    %10d %10d %10d    %5d %5d\n", (long long int)(j), n1, n2, n3, e1, e2, e3, m_SurfaceMeshFaceLabels[j * 2], m_SurfaceMeshFaceLabels[j * 2 + 1]);
  }

  fclose(triFile);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SurfaceMeshToNodesTrianglesEdges::newFilterInstance(bool copyFilterParameters)
{
  SurfaceMeshToNodesTrianglesEdges::Pointer filter = SurfaceMeshToNodesTrianglesEdges::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SurfaceMeshToNodesTrianglesEdges::getCompiledLibraryName()
{ return IOConstants::IOBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SurfaceMeshToNodesTrianglesEdges::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SurfaceMeshToNodesTrianglesEdges::getSubGroupName()
{ return DREAM3D::FilterSubGroups::OutputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SurfaceMeshToNodesTrianglesEdges::getHumanLabel()
{ return "Write Nodes/Triangles from Surface Mesh"; }

