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

#include "GenerateUniqueEdges.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateUniqueEdges::GenerateUniqueEdges() :
AbstractFilter(),
m_SurfaceMeshUniqueEdgesArrayName(DREAM3D::CellData::SurfaceMeshUniqueEdges),
m_SurfaceMeshUniqueEdges(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateUniqueEdges::~GenerateUniqueEdges()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateUniqueEdges::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateUniqueEdges::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateUniqueEdges::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-384);
  }
  else
  {
      // We MUST have Nodes
    if(sm->getNodes().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
      setErrorCondition(-384);
    }

    // We MUST have Triangles defined also.
    if(sm->getTriangles().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -383);
      setErrorCondition(-384);
    }

    // We do not know the size of the array so we can not use the macro so we just manually call
    // the needed methods that will propagate these array additions to the pipeline
    DataArray<int>::Pointer uniqueEdgesArray = DataArray<int>::CreateArray(1, 2, DREAM3D::CellData::SurfaceMeshUniqueEdges);
    sm->addCellData(DREAM3D::CellData::SurfaceMeshUniqueEdges, uniqueEdgesArray);

    // This is just for tracking what Arrays are being created by this filter. Normally the macro
    // would do this for us.
    addCreatedCellData(DREAM3D::CellData::SurfaceMeshUniqueEdges);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateUniqueEdges::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateUniqueEdges::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceMeshDataContainer* m = getSurfaceMeshDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMesh DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  // Just to double check we have everything.
  dataCheck(false, 0,0,0);
  if (getErrorCondition() < 0)
  {
    return;
  }

  /* Place all your code to execute your filter here. */
  generateConnectivity();


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateUniqueEdges::generateConnectivity()
{

// this function returns a list of unique edges in the polygon list.  A 2 x M array is returned that lists
// the unique pairs of vertex points that are paired along an edge.  This is the way that this function
// opertates in this program.  The time to cacluate the vertex connectivity arrays is long, and not needed
// for mesh smoothing.


  // Get our Reference counted Array of Triangle Structures
  StructArray<Triangle>::Pointer trianglesPtr = getSurfaceMeshDataContainer()->getTriangles();
  if(NULL == trianglesPtr.get())
  {
    setErrorCondition(-556);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Triangles", -556);
    return;
  }
  int ntri = trianglesPtr->GetNumberOfTuples();

  // get the triangle definitions - use the pointer to the start of the Struct Array
  Triangle* triangles = trianglesPtr->GetPointer(0);

  // need to make a list of triangle edges
  // each triangle has three edges, made up of two pairs of vertices
  std::map<int64_t, int> uedges_id_map;

  int edge_id = 0;
  int cur_edge_id = 0;

  struct  { int32_t v0; int32_t v1; } edge;
  int64_t* u64Edge = reinterpret_cast<int64_t*>(&edge); // This pointer is a 64 bit integer interpretation of the above struct variable

  typedef std::map<int64_t, int>::iterator EdgesIdMapIterator_t;

  for(int i = 0; i < ntri; ++i)
  {
    Triangle& tri = triangles[i];
    // Edge 0
    edge.v0 = tri.node_id[0];
    edge.v1 = tri.node_id[1];
    EdgesIdMapIterator_t iter = uedges_id_map.find(*u64Edge);
    if ( iter == uedges_id_map.end())
    {
       uedges_id_map[*u64Edge] = edge_id;
       cur_edge_id = edge_id;
       ++edge_id;
    }

    // Edge 1
    edge.v0 = tri.node_id[1];
    edge.v1 = tri.node_id[2];
    iter = uedges_id_map.find(*u64Edge);
    if ( iter == uedges_id_map.end())
    {
       uedges_id_map[*u64Edge] = edge_id;
       cur_edge_id = edge_id;
       ++edge_id;
    }

    // Edge 2
    edge.v0 = tri.node_id[2];
    edge.v1 = tri.node_id[0];
    iter = uedges_id_map.find(*u64Edge);
    if ( iter == uedges_id_map.end())
    {
       uedges_id_map[*u64Edge] = edge_id;
       cur_edge_id = edge_id;
       ++edge_id;
    }
  }

  // Now copy the unique Edges out of the map and into an array at the proper index (which is the "value" that goes with the "key" to the map.
  int index = 0;
  DataArray<int>::Pointer uniqueEdgesArrayPtr = DataArray<int>::CreateArray(uedges_id_map.size(), 2, DREAM3D::CellData::SurfaceMeshUniqueEdges);
  m_SurfaceMeshUniqueEdges = uniqueEdgesArrayPtr->GetPointer(0);
  for(std::map<int64_t, int>::iterator iter = uedges_id_map.begin(); iter != uedges_id_map.end(); ++iter)
  {
    *u64Edge = iter->first;
    index = iter->second;
    m_SurfaceMeshUniqueEdges[index*2] = edge.v0;
    m_SurfaceMeshUniqueEdges[index*2 + 1] = edge.v1;
  }

  // Finally push both the arrays into the Data Container for the pipeline
  getSurfaceMeshDataContainer()->addCellData(DREAM3D::CellData::SurfaceMeshUniqueEdges, uniqueEdgesArrayPtr);


  return;
}

