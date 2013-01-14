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

#include "GenerateNodeTriangleConectivity.h"

#include "DREAM3DLib/Common/ManagedPointerArray.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateNodeTriangleConectivity::GenerateNodeTriangleConectivity() :
  AbstractFilter(),
  m_SurfaceMeshUniqueEdgesArrayName(DREAM3D::CellData::SurfaceMeshUniqueEdges),
  m_SurfaceMeshTriangleEdgesArrayName(DREAM3D::CellData::SurfaceMeshTriangleEdges),
//  m_SurfaceMeshUniqueEdges(NULL),
  m_SurfaceMeshTriangleEdges(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateNodeTriangleConectivity::~GenerateNodeTriangleConectivity()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateNodeTriangleConectivity::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateNodeTriangleConectivity::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
/*      writer->writeValue("TriangleId", getTriangleId() );
      writer->writeValue("RegionId", getTriangleId() );*/
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateNodeTriangleConectivity::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
    else
    {
      // This depends on the triangles array already being created
      int size = sm->getTriangles()->GetNumberOfTuples();
      CREATE_NON_PREREQ_DATA(sm, DREAM3D, CellData, SurfaceMeshTriangleEdges, ss, int32_t, Int32ArrayType, 0, size, 3)
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
void GenerateNodeTriangleConectivity::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateNodeTriangleConectivity::execute()
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

  notifyStatusMessage("Starting");

  // Generate the connectivity data
  generateConnectivity();

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NodeTrianglesMap_t& GenerateNodeTriangleConectivity::getNode2TriangleMap()
{
  return m_Node2Triangle;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateNodeTriangleConectivity::generateConnectivity()
{

  // Get our Reference counted Array of Triangle Structures
  StructArray<Triangle>::Pointer trianglesPtr = getSurfaceMeshDataContainer()->getTriangles();
  if(NULL == trianglesPtr.get())
  {
    setErrorCondition(-556);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Triangles", -556);
    return;
  }
  int ntri = trianglesPtr->GetNumberOfTuples();
  NodeTrianglesMap_t m_Node2Triangle;

  // get the triangle definitions - use the pointer to the start of the Struct Array
  Triangle* triangles = trianglesPtr->GetPointer(0);
  // Generate the map of node_id -> Triangles that include that node_id value
  for(int i = 0; i < ntri; ++i)
  {
    Triangle& tri = triangles[i];
    m_Node2Triangle[tri.node_id[0]].insert(i);
    m_Node2Triangle[tri.node_id[1]].insert(i);
    m_Node2Triangle[tri.node_id[2]].insert(i);
  }

  ManagedPointerArray<int>::Pointer nodeTriangleArray = ManagedPointerArray<int>::CreateArray(m_Node2Triangle.size(), DREAM3D::CellData::SurfaceMeshNodeTriangles);
  // Loop over each entry in the map
  for(NodeTrianglesMap_t::iterator iter = m_Node2Triangle.begin(); iter != m_Node2Triangle.end(); ++iter)
  {
    int nodeId = (*iter).first;
    ManagedPointerArray<int>::Data_t& entry = *(nodeTriangleArray->GetPointer(nodeId));
    UniqueTriangleIds_t& triangles = (*iter).second;
    // Allocate enough memory to hold the list of triangles
    entry.count = triangles.size();
    if (entry.count > 0)
    {
      entry.data = (int*)(malloc(sizeof(int) * entry.count));
      int index = 0;
      for(UniqueTriangleIds_t::iterator tIter = triangles.begin(); tIter != triangles.end(); ++tIter)
      {
        entry.data[index++] = *tIter; // Copy the value from the triangle Ids set into the ManagedPointer
      }
   //   m_Node2Triangle.erase(iter); // Erase this data as we build up the actual array.
    }
  }

  getSurfaceMeshDataContainer()->addCellData(nodeTriangleArray->GetName(), nodeTriangleArray);
  return;
}

