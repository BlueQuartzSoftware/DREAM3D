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

#include "DREAM3DLib/SurfaceMeshingFilters/GenerateUniqueEdges.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateSurfaceMeshConnectivity::GenerateSurfaceMeshConnectivity() :
  SurfaceMeshFilter(),
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
  std::vector<FilterParameter::Pointer> parameters;
  /* Place all your option initialization code here */

  /*   For a simple true/false boolean use this code*/
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Generate Per Vertex Triangle List");
    parameter->setPropertyName("GenerateVertexTriangleLists");
    parameter->setWidgetType(FilterParameter::BooleanWidget);
    parameter->setValueType("bool");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Generate Triangle Neighbors List");
    parameter->setPropertyName("GenerateTriangleNeighbors");
    parameter->setWidgetType(FilterParameter::BooleanWidget);
    parameter->setValueType("bool");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Generate Edge Id List");
    parameter->setPropertyName("GenerateEdgeIdList");
    parameter->setWidgetType(FilterParameter::BooleanWidget);
    parameter->setValueType("bool");
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateSurfaceMeshConnectivity::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateSurfaceMeshConnectivity::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  writer->writeValue("GenerateVertexTriangleLists", getGenerateVertexTriangleLists() );
  writer->writeValue("GenerateTriangleNeighbors", getGenerateTriangleNeighbors() );
  writer->writeValue("GenerateEdgeIdList", getGenerateEdgeIdList() );

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateSurfaceMeshConnectivity::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", getErrorCondition());
  }
  else
  {
    // We MUST have Nodes
    if(sm->getVertices().get() == NULL)
    {
      setErrorCondition(-384);
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
    }

    // We MUST have Triangles defined also.
    if(sm->getFaces().get() == NULL)
    {
      setErrorCondition(-385);
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateSurfaceMeshConnectivity::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateSurfaceMeshConnectivity::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Voxel DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  dataCheck(false, 1, 1, 1);
  if (getErrorCondition() < 0)
  {
    return;
  }
  // We need the vertex->Triangle Lists to build the Triangle Neighbor lists so if either
  // of those are true then build the vertex->triangle lists
  if (m_GenerateVertexTriangleLists == true || m_GenerateTriangleNeighbors == true)
  {
    notifyStatusMessage("Generating Vertex Triangle List");
    getSurfaceMeshDataContainer()->buildMeshVertLinks();
  }
  if (m_GenerateTriangleNeighbors == true)
  {
    notifyStatusMessage("Generating Face Neighbors List");
    getSurfaceMeshDataContainer()->buildMeshFaceNeighborLists();
  }

  if (m_GenerateEdgeIdList == true)
  {
    // There was no Edge connectivity before this filter so delete it when we are done with it
    GenerateUniqueEdges::Pointer conn = GenerateUniqueEdges::New();
    ss.str("");
    ss << getMessagePrefix() << " |->Generating Unique Edge Ids |->";
    conn->setMessagePrefix(ss.str());
    conn->setObservers(getObservers());
    conn->setVoxelDataContainer(getVoxelDataContainer());
    conn->setSurfaceMeshDataContainer(getSurfaceMeshDataContainer());
    conn->setSolidMeshDataContainer(getSolidMeshDataContainer());
    conn->execute();
    if(conn->getErrorCondition() < 0)
    {
      setErrorCondition(conn->getErrorCondition());
      return;
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
