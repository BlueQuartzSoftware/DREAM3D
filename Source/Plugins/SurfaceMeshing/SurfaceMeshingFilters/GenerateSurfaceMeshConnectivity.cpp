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


#include "GenerateSurfaceMeshConnectivity.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateSurfaceMeshConnectivity::GenerateSurfaceMeshConnectivity() :
  SurfaceMeshFilter(),
  m_SurfaceDataContainerName(DREAM3D::Defaults::SurfaceDataContainerName),
  m_GenerateVertexTriangleLists(true),
  m_GenerateTriangleNeighbors(true),
  m_GenerateEdgeIdList(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateSurfaceMeshConnectivity::~GenerateSurfaceMeshConnectivity()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateSurfaceMeshConnectivity::setupFilterParameters()
{
  FilterParameterVector parameters;
  /* Place all your option initialization code here */

  /*   For a simple true/false boolean use this code*/
  parameters.push_back(FilterParameter::New("Generate Per Vertex Triangle List", "GenerateVertexTriangleLists", FilterParameterWidgetType::BooleanWidget, getGenerateVertexTriangleLists(), false));
  parameters.push_back(FilterParameter::New("Generate Triangle Neighbors List", "GenerateTriangleNeighbors", FilterParameterWidgetType::BooleanWidget, getGenerateTriangleNeighbors(), false));
  parameters.push_back(FilterParameter::New("Generate Edge Id List", "GenerateEdgeIdList", FilterParameterWidgetType::BooleanWidget, getGenerateEdgeIdList(), false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Surface Data Container", "SurfaceDataContainerName", FilterParameterWidgetType::DataContainerSelectionWidget, getSurfaceDataContainerName(), true, ""));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateSurfaceMeshConnectivity::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceDataContainerName(reader->readString("SurfaceDataContainerName", getSurfaceDataContainerName() ) );
  setGenerateVertexTriangleLists( reader->readValue("GenerateVertexTriangleLists", false) );
  setGenerateTriangleNeighbors( reader->readValue("GenerateTriangleNeighbors", false) );
  setGenerateEdgeIdList( reader->readValue("GenerateEdgeIdList", false) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenerateSurfaceMeshConnectivity::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SurfaceDataContainerName", getSurfaceDataContainerName() );
  writer->writeValue("GenerateVertexTriangleLists", getGenerateVertexTriangleLists() );
  writer->writeValue("GenerateTriangleNeighbors", getGenerateTriangleNeighbors() );
  writer->writeValue("GenerateEdgeIdList", getGenerateEdgeIdList() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateSurfaceMeshConnectivity::dataCheck()
{
  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getSurfaceDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

  // We MUST have Nodes
  if(sm->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
  }

  // We MUST have Triangles defined also.
  if(sm->getFaces().get() == NULL)
  {
    setErrorCondition(-385);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateSurfaceMeshConnectivity::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateSurfaceMeshConnectivity::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  // We need the vertex->Triangle Lists to build the Triangle Neighbor lists so if either
  // of those are true then build the vertex->triangle lists
  if (m_GenerateVertexTriangleLists == true || m_GenerateTriangleNeighbors == true)
  {
    notifyStatusMessage(getHumanLabel(), "Generating Vertex Triangle List");
    getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->getFaces()->findFacesContainingVert();
  }
  if (m_GenerateTriangleNeighbors == true)
  {
    notifyStatusMessage(getHumanLabel(), "Generating Face Neighbors List");
    getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->getFaces()->findFaceNeighbors();
  }

  if (m_GenerateEdgeIdList == true)
  {
    notifyStatusMessage(getHumanLabel(), "Generating Unique Edges List");
    getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->getFaces()->generateUniqueEdgeIds();
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateSurfaceMeshConnectivity::newFilterInstance(bool copyFilterParameters)
{
  /*
  * GenerateVertexTriangleLists
  * GenerateTriangleNeighbors
  * GenerateEdgeIdList
  */
  GenerateSurfaceMeshConnectivity::Pointer filter = GenerateSurfaceMeshConnectivity::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}
