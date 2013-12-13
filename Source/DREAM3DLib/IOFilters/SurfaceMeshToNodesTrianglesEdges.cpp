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

#include "SurfaceMeshToNodesTrianglesEdges.h"


#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>


#define WRITE_EDGES_FILE 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshToNodesTrianglesEdges::SurfaceMeshToNodesTrianglesEdges() :
  AbstractFilter(),
  m_SurfaceDataContainerName(DREAM3D::Defaults::SurfaceDataContainerName),
  m_FaceAttributeMatrixName(DREAM3D::Defaults::FaceAttributeMatrixName),
  m_VertexAttributeMatrixName(DREAM3D::Defaults::VertexAttributeMatrixName),
  m_OutputNodesFile(""),
  m_OutputEdgesFile(""),
  m_OutputTrianglesFile(""),
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
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output Nodes File");
    option->setPropertyName("OutputNodesFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output Triangles File");
    option->setPropertyName("OutputTrianglesFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
#if WRITE_EDGES_FILE
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output Edges File");
    option->setPropertyName("OutputEdgesFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
#endif

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNodesTrianglesEdges::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setOutputNodesFile( reader->readString( "OutputNodesFile", getOutputNodesFile() ) );
  setOutputTrianglesFile( reader->readString( "OutputTrianglesFile", getOutputTrianglesFile() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshToNodesTrianglesEdges::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputNodesFile", getOutputNodesFile() );
  writer->writeValue("OutputEdgesFile", getOutputEdgesFile() );
  writer->writeValue("OutputTrianglesFile", getOutputTrianglesFile() );
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
    PipelineMessage em (getHumanLabel(), "The output Nodes file needs to be set", getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
  }
#if WRITE_EDGES_FILE
  if(true == m_OutputEdgesFile.isEmpty())
  {
    setErrorCondition(-381);
    PipelineMessage em (getHumanLabel(), "The output Edges file needs to be set", getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
  }
#endif
  if(true == m_OutputTrianglesFile.isEmpty())
  {
    setErrorCondition(-382);
    PipelineMessage em (getHumanLabel(), "The output Triangles file needs to be set", getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
  }

  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getSurfaceDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* amV = sm->getPrereqAttributeMatrix<AbstractFilter>(this, getVertexAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* amF = sm->getPrereqAttributeMatrix<AbstractFilter>(this, getFaceAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  if (sm->getFaces().get() == NULL)
  {
    setErrorCondition(-384);
    PipelineMessage em (getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
  }
  if (sm->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    PipelineMessage em (getHumanLabel(), "SurfaceMesh DataContainer missing Vertices", getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
  }
#if WRITE_EDGES_FILE
  IDataArray::Pointer edges = sm->getPointData(DREAM3D::CellData::SurfaceMeshEdges);
  if (edges.get() == NULL)
  {
    setErrorCondition(-385);
    PipelineMessage em (getHumanLabel(), "SurfaceMesh DataContainer missing DREAM3D::CellData::SurfaceMeshEdges Information", getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
  }
#endif
  QVector<int> dims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = amF->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayName, -386, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_SurfaceMeshNodeTypePtr = amV->getPrereqArray<DataArray<int8_t>, AbstractFilter>(this, m_SurfaceMeshNodeTypeArrayName, -386, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshNodeTypePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshNodeType = m_SurfaceMeshNodeTypePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNodesTrianglesEdges::preflight()
{
  dataCheck();
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

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());

  VertexArray::Pointer nodes = sm->getVertices();
  FaceArray::Pointer triangles = sm->getFaces();

  int64_t numNodes = nodes->getNumberOfTuples();
  int64_t numTriangles = triangles->getNumberOfTuples();

#if WRITE_EDGES_FILE
  IDataArray::Pointer edges = sm->getPointData(DREAM3D::CellData::SurfaceMeshEdges);
  IDataArray::Pointer iEdges = sm->getPointData(DREAM3D::CellData::SurfaceMeshInternalEdges);
#endif

  // ++++++++++++++ Write the Nodes File +++++++++++++++++++++++++++++++++++++++++++
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  emit filterGeneratedMessage(PipelineMessage::CreateStatusMessage(getHumanLabel(), "Writing Nodes Text File") );
  QFileInfo fi(getOutputNodesFile());
  QDir parentPath = fi.path();

  if(!parentPath.mkpath("."))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    PipelineMessage em(getHumanLabel(), ss, -1, PipelineMessage::Error);
    emit filterGeneratedMessage(em);
    setErrorCondition(-1);
    return;
  }
  FILE* nodesFile = NULL;
  nodesFile = fopen(getOutputNodesFile().toLatin1().data(), "wb");
  if (NULL == nodesFile)
  {
    setErrorCondition(-100);
    notifyErrorMessage("Error opening Nodes file for writing", -100);
    return;
  }

  fprintf(nodesFile, "%d\n", numNodes);
  VertexArray::Vert_t* v = nodes->getPointer(0);
  for (int i = 0; i < numNodes; i++)
  {
    fprintf(nodesFile, "%10d    %3d    %8.4f %8.4f %8.4f\n", i, static_cast<int>(m_SurfaceMeshNodeType[i]), v[i].pos[0], v[i].pos[1], v[i].pos[2]);
  }
  fclose(nodesFile);

#if WRITE_EDGES_FILE
  // ++++++++++++++ Write the Edges File +++++++++++++++++++++++++++++++++++++++++++
  emit filterGeneratedMessage(PipelineMessage::CreateStatusMessage(getHumanLabel(), "Writing Edges Text File") );
  parentPath = QFileInfo::parentPath(getOutputEdgesFile());
  QDir dir;
  if(!dir.mkpath(parentPath))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    PipelineMessage em(getHumanLabel(), ss, -1, PipelineMessage::Error);
    emit filterGeneratedMessage(em);
    setErrorCondition(-1);
    return;
  }
  FILE* eFile = fopen(getOutputEdgesFile().toLatin1().data(), "wb");
  if (NULL == eFile)
  {
    setErrorCondition(-100);
    notifyErrorMessage("Error opening Edges file for writing", -100);
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
  emit filterGeneratedMessage(PipelineMessage::CreateStatusMessage(getHumanLabel(), "Writing Triangles Text File") );
  QFileInfo triFI(getOutputTrianglesFile());
  parentPath = triFI.path();
  if(!parentPath.mkpath("."))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    PipelineMessage em(getHumanLabel(), ss, -1, PipelineMessage::Error);
    emit filterGeneratedMessage(em);
    setErrorCondition(-1);
    return;
  }
  FILE* triFile = fopen(getOutputTrianglesFile().toLatin1().data(), "wb");
  if (NULL == triFile)
  {
    setErrorCondition(-100);
    notifyErrorMessage("Error opening Triangles file for writing", -100);
    return;
  }

  fprintf(triFile, "%lu\n", numTriangles);
  FaceArray::Face_t* t = triangles->getPointer(0);

  int n1, n2, n3, e1 = -1, e2 = -1, e3 = -1;
  for (size_t j = 0; j < numTriangles; ++j)
  {
    n1 = t[j].verts[0];
    n2 = t[j].verts[1];
    n3 = t[j].verts[2];

#if WRITE_EDGES_FILE
    e1 = t[j].e_id[0];
    e2 = t[j].e_id[1];
    e3 = t[j].e_id[2];
    <<< <<< <<< < #error FIX THIS CODE BELOW. WE PROBABLY NEED TO GENERATE THE EDGE INFORMATION IF NEEDED
#endif

    fprintf(triFile, "%10lu    %10d %10d %10d    %10d %10d %10d    %5d %5d\n", j, n1, n2, n3, e1, e2, e3, m_SurfaceMeshFaceLabels[j * 2], m_SurfaceMeshFaceLabels[j * 2 + 1]);
  }

  fclose(triFile);

  /* Let the GUI know we are done with this filter */
  emit filterGeneratedMessage(PipelineMessage::CreateStatusMessage(getHumanLabel(), "Complete") );
}

