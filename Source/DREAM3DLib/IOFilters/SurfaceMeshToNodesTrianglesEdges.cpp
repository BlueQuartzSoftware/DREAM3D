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


#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/MXAFileInfo.h"



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
  std::vector<FilterParameter::Pointer> parameters;
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
    option->setHumanLabel("Output Edges File");
    option->setPropertyName("OutputEdgesFile");
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
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNodesTrianglesEdges::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("OutputNodesFile", getOutputNodesFile() );
  writer->writeValue("OutputEdgesFile", getOutputEdgesFile() );
  writer->writeValue("OutputTrianglesFile", getOutputTrianglesFile() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNodesTrianglesEdges::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  if(true == m_OutputNodesFile.empty())
  {
    addErrorMessage(getHumanLabel(), "The output Nodes file needs to be set", -380);
    setErrorCondition(-380);
  }
  if(true == m_OutputEdgesFile.empty())
  {
    addErrorMessage(getHumanLabel(), "The output Edges file needs to be set", -381);
    setErrorCondition(-381);
  }
  if(true == m_OutputTrianglesFile.empty())
  {
    addErrorMessage(getHumanLabel(), "The output Triangles file needs to be set", -382);
    setErrorCondition(-382);
  }

  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if (NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-384);
  }
  else {
    if (sm->getTriangles().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -383);
      setErrorCondition(-384);
    }
    if (sm->getNodes().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -384);
      setErrorCondition(-384);
    }
    IDataArray::Pointer edges = sm->getCellData(DREAM3D::CellData::SurfaceMesh::Edges);
    if (edges.get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -385);
      setErrorCondition(-385);
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
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMesh DataContainer Object was NULL", -999);
    return;
  }

  StructArray<Node>::Pointer nodes = sm->getNodes();
  if (NULL == nodes.get())
  {
    setErrorCondition(-555);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Nodes", -555);
    return;
  }

  StructArray<Triangle>::Pointer triangles = sm->getTriangles();
  if (NULL == triangles.get())
  {
    setErrorCondition(-556);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Triangles", -556);
    return;
  }
  IDataArray::Pointer edges = sm->getCellData(DREAM3D::CellData::SurfaceMesh::Edges);
  if (edges.get() == NULL)
  {
    setErrorCondition(-557);
    notifyErrorMessage("SurfaceMesh DataContainer missing Edges", -557);
    return;
  }

  IDataArray::Pointer iEdges = sm->getCellData(DREAM3D::CellData::SurfaceMesh::InternalEdges);
  if (iEdges.get() == NULL)
  {
    setErrorCondition(-558);
    notifyErrorMessage("SurfaceMesh DataContainer missing Internal Edges", -558);
    return;
  }

  setErrorCondition(0);


  // ++++++++++++++ Write the Nodes File +++++++++++++++++++++++++++++++++++++++++++
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  std::string parentPath = MXAFileInfo::parentPath(getOutputNodesFile());
  if(!MXADir::mkdir(parentPath, true))
  {
    std::stringstream ss;
    ss << "Error creating parent path '" << parentPath << "'";
    notifyErrorMessage(ss.str(), -1);
    setErrorCondition(-1);
    return;
  }
  FILE* nodesFile = NULL;
  nodesFile = fopen(getOutputNodesFile().c_str(), "wb");
  if (NULL == nodesFile)
  {
    setErrorCondition(-100);
    notifyErrorMessage("Error opening Nodes file for writing", -100);
    return;
  }

  int numNodes = nodes->GetNumberOfTuples();
  fprintf(nodesFile, "%d\n", numNodes);
  Node* v = nodes->GetPointer(0);
  for (int i = 0; i < numNodes; i++)
  {
    fprintf(nodesFile, "%10d    %3d    %8.4f %8.4f %8.4f\n", i, v[i].nodeKind, v[i].coord[0], v[i].coord[1], v[i].coord[2]);
  }
  fclose(nodesFile);

  // ++++++++++++++ Write the Edges File +++++++++++++++++++++++++++++++++++++++++++
  parentPath = MXAFileInfo::parentPath(getOutputEdgesFile());
  if(!MXADir::mkdir(parentPath, true))
  {
    std::stringstream ss;
    ss << "Error creating parent path '" << parentPath << "'";
    notifyErrorMessage(ss.str(), -1);
    setErrorCondition(-1);
    return;
  }
  FILE* eFile = fopen(getOutputEdgesFile().c_str(), "wb");
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
  StructArray<Segment>* faceEdges = StructArray<Segment>::SafePointerDownCast(edges.get());
  Segment* fe = faceEdges->GetPointer(0);
  fprintf(eFile, "%lu\n", nfe + nie);

  for (size_t k = 0; k < nfe; ++k)
  {

    kind = fe[k].edgeKind;
    newnid1 = fe[k].node_id[0];
    newnid2 = fe[k].node_id[1];

    fprintf(eFile, "%10lu    %8d %8d    %6d    %6d %6d %6d %6d\n", k, newnid1, newnid2, kind, fe[k].neigh_spin[0], fe[k].neigh_spin[1], 0, 0);
  }

  StructArray<ISegment>* internalEdges = StructArray<ISegment>::SafePointerDownCast(iEdges.get());
  ISegment* ie = internalEdges->GetPointer(0);
  for (size_t kk = 0; kk < nie; ++kk)
  {

    kind = ie[kk].edgeKind;
    newnid1 = ie[kk].node_id[0];
    newnid2 = ie[kk].node_id[1];
    fprintf(eFile, "%10lu    %8d %8d    %6d    %6d %6d %6d %6d\n", kk + nfe, newnid1, newnid2, kind, ie[kk].nSpin[0], ie[kk].nSpin[1], ie[kk].nSpin[2], ie[kk].nSpin[3]);
  }

  fclose(eFile);

  // ++++++++++++++ Write the Triangles File +++++++++++++++++++++++++++++++++++++++++++
  parentPath = MXAFileInfo::parentPath(getOutputTrianglesFile());
  if(!MXADir::mkdir(parentPath, true))
  {
    std::stringstream ss;
    ss << "Error creating parent path '" << parentPath << "'";
    notifyErrorMessage(ss.str(), -1);
    setErrorCondition(-1);
    return;
  }
  FILE* triFile = fopen(getOutputTrianglesFile().c_str(), "wb");
  if (NULL == triFile)
  {
    setErrorCondition(-100);
    notifyErrorMessage("Error opening Triangles file for writing", -100);
    return;
  }

  size_t numTriangles = triangles->GetNumberOfTuples();
  fprintf(triFile, "%lu\n", numTriangles);
  StructArray<Triangle>* ts = StructArray<Triangle>::SafePointerDownCast(triangles.get());
  Triangle* t = ts->GetPointer(0);
  int n1, n2, n3, e1, e2, e3;
  for (size_t j = 0; j < numTriangles; ++j)
  {
    n1 = t[j].node_id[0];
    n2 = t[j].node_id[1];
    n3 = t[j].node_id[2];

    e1 = t[j].e_id[0];
    e2 = t[j].e_id[1];
    e3 = t[j].e_id[2];

    fprintf(triFile, "%10lu    %10d %10d %10d    %10d %10d %10d    %5d %5d\n", j, n1, n2, n3, e1, e2, e3, t[j].nSpin[0], t[j].nSpin[1]);
  }

  fclose(triFile);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
