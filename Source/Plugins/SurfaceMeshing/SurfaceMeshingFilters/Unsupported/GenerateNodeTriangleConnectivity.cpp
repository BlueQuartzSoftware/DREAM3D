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

#include "GenerateNodeTriangleConnectivity.h"

#include "SIMPLib/Common/ManagedPointerArray.hpp"

// Include the MOC generated file for this class
#include "moc_GenerateNodeTriangleConnectivity.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateNodeTriangleConnectivity::GenerateNodeTriangleConnectivity()
: AbstractFilter()
, m_SurfaceMeshUniqueEdgesArrayName(SIMPL::CellData::SurfaceMeshUniqueEdges)
, m_SurfaceMeshTriangleEdgesArrayName(SIMPL::CellData::SurfaceMeshTriangleEdges)
, m_SurfaceMeshTriangleEdges(nullptr)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateNodeTriangleConnectivity::~GenerateNodeTriangleConnectivity()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateNodeTriangleConnectivity::setupFilterParameters()
{
  FilterParameterVector parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateNodeTriangleConnectivity::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  ////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateNodeTriangleConnectivity::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateNodeTriangleConnectivity::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(nullptr == sm)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", getErrorCondition());
  }
  else
  {
    // We MUST have Nodes
    if(sm->getVertices().get() == nullptr)
    {
      setErrorCondition(-384);
      notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
    }

    // We MUST have Triangles defined also.
    if(sm->getFaces().get() == nullptr)
    {
      setErrorCondition(-384);
      notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
    }
    else
    {
      // This depends on the triangles array already being created
      int size = sm->getFaces()->GetNumberOfTuples();
      m_SurfaceMeshTriangleEdgesPtr = sattrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_CellAttributeMatrixName, m_SurfaceMeshTriangleEdgesArrayName, 0, size,
                                                                                                                  3); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
      if(nullptr != m_SurfaceMeshTriangleEdgesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
      {
        m_SurfaceMeshTriangleEdges = m_SurfaceMeshTriangleEdgesPtr.lock()->getPointer(0);
      } /* Now assign the raw pointer to data from the DataArray<T> object */
    }

    // We do not know the size of the array so we can not use the macro so we just manually call
    // the needed methods that will propagate these array additions to the pipeline
    DataArray<int>::Pointer uniqueEdgesArray = DataArray<int>::CreateArray(1, 2, SIMPL::CellData::SurfaceMeshUniqueEdges);
    sm->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(SIMPL::CellData::SurfaceMeshUniqueEdges, uniqueEdgesArray);

    // This is just for tracking what Arrays are being created by this filter. Normally the macro
    // would do this for us.
    addCreatedCellData(SIMPL::CellData::SurfaceMeshUniqueEdges);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateNodeTriangleConnectivity::preflight()
{
  setInPreflight(true);
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateNodeTriangleConnectivity::execute()
{
  int err = 0;

  // Just to double check we have everything.
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  setErrorCondition(err);
  SurfaceMeshDataContainer* m = getSurfaceMeshDataContainer();
  if(nullptr == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage(getHumanLabel(), "The SurfaceMesh DataContainer Object was nullptr", -999);
    return;
  }
  setErrorCondition(0);
  setWarningCondition(0);

  notifyStatusMessage(getHumanLabel(), "Starting");

  // Generate the connectivity data
  generateConnectivity();

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NodeTrianglesMap_t& GenerateNodeTriangleConnectivity::getNode2TriangleMap()
{
  return m_Node2Triangle;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateNodeTriangleConnectivity::generateConnectivity()
{

  // Get our Reference counted Array of Triangle Structures
  StructArray<SurfaceMesh::DataStructures::Face_t>::Pointer trianglesPtr = getSurfaceMeshDataContainer()->getTriangles();
  if(nullptr == trianglesPtr.get())
  {
    setErrorCondition(-556);
    notifyErrorMessage(getHumanLabel(), "The SurfaceMesh DataContainer Does NOT contain Triangles", -556);
    return;
  }
  int ntri = trianglesPtr->GetNumberOfTuples();
  NodeTrianglesMap_t m_Node2Triangle;
  notifyStatusMessage(getHumanLabel(), "Creating the Mapping of Triangles to Node");
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
  if(getCancel() == true)
  {
    return;
  }

  ManagedPointerArray<int>::Pointer nodeTriangleArray = ManagedPointerArray<int>::CreateArray(m_Node2Triangle.size(), SIMPL::CellData::SurfaceMeshNodeTriangles);

  float progIndex = 0.0;
  float curPercent = 0.0;
  float total = static_cast<float>(m_Node2Triangle.size());

  // Loop over each entry in the map
  for(NodeTrianglesMap_t::iterator iter = m_Node2Triangle.begin(); iter != m_Node2Triangle.end(); ++iter)
  {
    if(progIndex / total * 100.0f > (curPercent))
    {
      ss.str("");
      ss << (progIndex / total * 100.0f) << "% Complete";
      notifyStatusMessage(getHumanLabel(), ss.str());
      curPercent += 5.0f;
    }
    progIndex++;
    if(getCancel() == true)
    {
      return;
    }

    int nodeId = iter.key();
    ManagedPointerArray<int>::Data_t& entry = *(nodeTriangleArray->GetPointer(nodeId));
    UniqueTriangleIds_t& triangles = iter.value();
    // Allocate enough memory to hold the list of triangles
    entry.count = triangles.size();
    if(entry.count > 0)
    {
      entry.data = (int*)(malloc(sizeof(int) * entry.count));
      int index = 0;
      for(UniqueTriangleIds_t::iterator tIter = triangles.begin(); tIter != triangles.end(); ++tIter)
      {
        entry.data[index++] = *tIter; // Copy the value from the triangle Ids set into the ManagedPointer
      }
    }
  }

  getSurfaceMeshDataContainer()->addCellData(nodeTriangleArray->getName(), nodeTriangleArray);
  return;
}
