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
  m_OutputNodesFile(""),
  m_OutputEdgesFile(""),
  m_OutputTrianglesFile("")
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
  QVector<FilterParameter::Pointer> parameters;
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
  setOutputNodesFile( reader->readValue( "OutputNodesFile", getOutputNodesFile() ) );
  setOutputTrianglesFile( reader->readValue( "OutputTrianglesFile", getOutputTrianglesFile() ) );
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
void SurfaceMeshToNodesTrianglesEdges::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  if(true == m_OutputNodesFile.isEmpty())
  {
    addErrorMessage(getHumanLabel(), "The output Nodes file needs to be set", -380);
    setErrorCondition(-380);
  }
  #if WRITE_EDGES_FILE
  if(true == m_OutputEdgesFile.isEmpty())
  {
    addErrorMessage(getHumanLabel(), "The output Edges file needs to be set", -381);
    setErrorCondition(-381);
  }
  #endif
  if(true == m_OutputTrianglesFile.isEmpty())
  {
    addErrorMessage(getHumanLabel(), "The output Triangles file needs to be set", -382);
    setErrorCondition(-382);
  }

  SurfaceDataContainer* sm = getSurfaceDataContainer();
  if (NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceDataContainer is missing", -383);
    setErrorCondition(-384);
  }
  else {
    if (sm->getFaces().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -384);
      setErrorCondition(-384);
    }
    if (sm->getVertices().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Vertices", -385);
      setErrorCondition(-384);
    }
  #if WRITE_EDGES_FILE
    IDataArray::Pointer edges = sm->getPointData(DREAM3D::CellData::SurfaceMeshEdges);
    if (edges.get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing DREAM3D::CellData::SurfaceMeshEdges Information", -386);
      setErrorCondition(-385);
    }
#endif
    IDataArray::Pointer nodeKinds = sm->getVertexData(DREAM3D::VertexData::SurfaceMeshNodeType);
    if (nodeKinds.get() == NULL)
    {
      setErrorCondition(-559);
      notifyErrorMessage("SurfaceMesh DataContainer missing DREAM3D::VertexData::SurfaceMeshNodeType Array", -387);
      return;
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNodesTrianglesEdges::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNodesTrianglesEdges::execute()
{
  int err = 0;

  setErrorCondition(err);


  dataCheck(false, 1,1,1);
  if (getErrorCondition() < 0)
  {
    return;
  }
  SurfaceDataContainer* sm = getSurfaceDataContainer();


  DREAM3D::Mesh::VertListPointer_t nodes = sm->getVertices();
  DREAM3D::Mesh::FaceListPointer_t triangles = sm->getFaces();
  IDataArray::Pointer nodeKinds = sm->getVertexData(DREAM3D::VertexData::SurfaceMeshNodeType);

#if WRITE_EDGES_FILE
  IDataArray::Pointer edges = sm->getPointData(DREAM3D::CellData::SurfaceMeshEdges);
  IDataArray::Pointer iEdges = sm->getPointData(DREAM3D::CellData::SurfaceMeshInternalEdges);
#endif

  setErrorCondition(0);


  // ++++++++++++++ Write the Nodes File +++++++++++++++++++++++++++++++++++++++++++
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  notifyStatusMessage("Writing Nodes Text File");
  QFileInfo fi(getOutputNodesFile());
  QDir parentPath = fi.path();

  if(!parentPath.mkpath("."))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    notifyErrorMessage(ss, -1);
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

  int numNodes = nodes->GetNumberOfTuples();
  fprintf(nodesFile, "%d\n", numNodes);
  DREAM3D::Mesh::Vert_t* v = nodes->GetPointer(0);
  int8_t* nodeKind = reinterpret_cast<int8_t*>(nodeKinds->GetVoidPointer(0));
  for (int i = 0; i < numNodes; i++)
  {
    fprintf(nodesFile, "%10d    %3d    %8.4f %8.4f %8.4f\n", i, static_cast<int>(nodeKind[i]), v[i].pos[0], v[i].pos[1], v[i].pos[2]);
  }
  fclose(nodesFile);

#if WRITE_EDGES_FILE
  // ++++++++++++++ Write the Edges File +++++++++++++++++++++++++++++++++++++++++++
  notifyStatusMessage("Writing Edges Text File");
  parentPath = QFileInfo::parentPath(getOutputEdgesFile());
    QDir dir;
  if(!dir.mkpath(parentPath))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    notifyErrorMessage(ss, -1);
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

  size_t nfe = edges->GetNumberOfTuples();
  size_t nie = iEdges->GetNumberOfTuples();
  int kind = 0;
  int newnid1 = 0, newnid2 = 0;
  StructArray<SurfaceMesh::M3C::Segment>* faceEdges = StructArray<SurfaceMesh::M3C::Segment>::SafePointerDownCast(edges.get());
  SurfaceMesh::M3C::Segment* fe = faceEdges->GetPointer(0);
  fprintf(eFile, "%lu\n", nfe + nie);

  for (size_t k = 0; k < nfe; ++k)
  {

    kind = fe[k].edgeKind;
    newnid1 = fe[k].node_id[0];
    newnid2 = fe[k].node_id[1];

    fprintf(eFile, "%10lu    %8d %8d    %6d    %6d %6d %6d %6d\n", k, newnid1, newnid2, kind, fe[k].nSpin[0], fe[k].nSpin[1], 0, 0);
  }

  StructArray<SurfaceMesh::M3C::ISegment>* internalEdges = StructArray<SurfaceMesh::M3C::ISegment>::SafePointerDownCast(iEdges.get());
  SurfaceMesh::M3C::ISegment* ie = internalEdges->GetPointer(0);
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
  notifyStatusMessage("Writing Triangles Text File");
  QFileInfo triFI(getOutputTrianglesFile());
  parentPath = triFI.path();
  if(!parentPath.mkpath("."))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    notifyErrorMessage(ss, -1);
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

  size_t numTriangles = triangles->GetNumberOfTuples();
  fprintf(triFile, "%lu\n", numTriangles);
  StructArray<DREAM3D::Mesh::Face_t>* ts = DREAM3D::Mesh::FaceList_t::SafePointerDownCast(triangles.get());
  DREAM3D::Mesh::Face_t* t = ts->GetPointer(0);


  IDataArray::Pointer flPtr = getSurfaceDataContainer()->getFaceData(DREAM3D::FaceData::SurfaceMeshFaceLabels);
  DataArray<int32_t>* faceLabelsPtr = DataArray<int32_t>::SafePointerDownCast(flPtr.get());
  int32_t* faceLabels = faceLabelsPtr->GetPointer(0);


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
<<<<<<<<<< #error FIX THIS CODE BELOW. WE PROBABLY NEED TO GENERATE THE EDGE INFORMATION IF NEEDED
#endif

    fprintf(triFile, "%10lu    %10d %10d %10d    %10d %10d %10d    %5d %5d\n", j, n1, n2, n3, e1, e2, e3, faceLabels[j*2], faceLabels[j*2+1]);
  }

  fclose(triFile);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
