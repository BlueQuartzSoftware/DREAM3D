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


#include <QSet>

#include "DREAM3DLib/DataArrays/ManagedArrayOfArrays.hpp"


typedef QSet<int64_t>  EdgeSet_t;
typedef EdgeSet_t::iterator EdgesIdSetIterator_t;




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateUniqueEdges::GenerateUniqueEdges() :
  SurfaceMeshFilter(),
  m_SurfaceDataContainerName(DREAM3D::Defaults::SurfaceDataContainerName),
  m_EdgeAttributeMatrixName(DREAM3D::Defaults::EdgeAttributeMatrixName),
  m_VertexAttributeMatrixName(DREAM3D::Defaults::VertexAttributeMatrixName),
  m_SurfaceMeshUniqueEdgesArrayName(DREAM3D::EdgeData::SurfaceMeshUniqueEdges)
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
  FilterParameterVector parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateUniqueEdges::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenerateUniqueEdges::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateUniqueEdges::dataCheck()
{
  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getSurfaceDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

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

  // We do not know the size of the array so we can not use the macro so we just manually call
  // the needed methods that will propagate these array additions to the pipeline
  QVector<int> dims(1, 2);
  DataArray<int>::Pointer uniqueEdgesArray = DataArray<int>::CreateArray(1, dims, m_SurfaceMeshUniqueEdgesArrayName);
  sm->getAttributeMatrix(getEdgeAttributeMatrixName())->addAttributeArray(m_SurfaceMeshUniqueEdgesArrayName, uniqueEdgesArray);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateUniqueEdges::preflight()
{
  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  if(NULL == sm)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "SurfaceDataContainer is missing", getErrorCondition());
  }

  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateUniqueEdges::execute()
{
  int err = 0;

  setErrorCondition(err);
  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMesh DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  // Just to double check we have everything.
  dataCheck(false, 0, 0, 0);
  if (getErrorCondition() < 0)
  {
    return;
  }

  /* Place all your code to execute your filter here. */
  generateUniqueEdgeIds();


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateUniqueEdges::generateUniqueEdgeIds()
{

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());

  FaceArray::Pointer trianglesPtr = sm->getFaces();
  size_t totalPoints = trianglesPtr->getNumberOfTuples();
  FaceArray::Face_t* faces = trianglesPtr->getPointer(0);

  notifyStatusMessage("Stage 1 of 2");
  struct  { int32_t v0; int32_t v1; } edge;
  int64_t* u64Edge = reinterpret_cast<int64_t*>(&edge); // This pointer is a 64 bit integer interpretation of the above struct variable


  EdgeSet_t uedges_id_set;
  for(size_t t = 0; t < totalPoints; ++t)
  {
    //Get the Triangle
    FaceArray::Face_t& tri = faces[t];

    //Edge 0
    int i = 0;
    edge.v0 = tri.verts[i];
    edge.v1 = tri.verts[i + 1];
    if (edge.v0 > edge.v1) { edge.v0 = tri.verts[i + 1]; edge.v1 = tri.verts[i]; }
    uedges_id_set.insert(*u64Edge);

    //Edge 1
    i = 1;
    edge.v0 = tri.verts[i];
    edge.v1 = tri.verts[i + 1];
    if (edge.v0 > edge.v1) { edge.v0 = tri.verts[i + 1]; edge.v1 = tri.verts[i]; }
    uedges_id_set.insert(*u64Edge);

    // Edge 2
    i = 2;
    edge.v0 = tri.verts[i];
    edge.v1 = tri.verts[0];
    if (edge.v0 > edge.v1) { edge.v0 = tri.verts[0]; edge.v1 = tri.verts[i]; }
    uedges_id_set.insert(*u64Edge);

  }

  notifyStatusMessage("Stage 1 of 2");
// qDebug() << "uedges_id_set size: " << uedges_id_set.size() << "\n";
  QVector<int> dims(1, 2);
  DataArray<int>::Pointer uniqueEdgesArrayPtr = DataArray<int>::CreateArray(uedges_id_set.size(), dims, m_SurfaceMeshUniqueEdgesArrayName);
  int32_t* surfaceMeshUniqueEdges = uniqueEdgesArrayPtr->getPointer(0);
  int index = 0;
  for(EdgeSet_t::iterator iter = uedges_id_set.begin(); iter != uedges_id_set.end(); ++iter)
  {
    *u64Edge = *iter;
    surfaceMeshUniqueEdges[index * 2] = edge.v0;
    surfaceMeshUniqueEdges[index * 2 + 1] = edge.v1;
    ++index;
  }
  sm->getAttributeMatrix(getEdgeAttributeMatrixName())->addAttributeArray(uniqueEdgesArrayPtr->GetName(), uniqueEdgesArrayPtr);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateUniqueEdges::generateEdgeTriangleConnectivity()
{

// this function returns a list of unique edges in the polygon list.  A 2 x M array is returned that lists
// the unique pairs of vertex points that are paired along an edge.  This is the way that this function
// opertates in this program.  The time to cacluate the vertex connectivity arrays is long, and not needed
// for mesh smoothing.

  notifyStatusMessage("Generating edge list for mesh. Stage 1 of 2");
  // Get our Reference counted Array of Triangle Structures
  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  FaceArray::Pointer trianglesPtr = sm->getFaces();
  if(NULL == trianglesPtr.get())
  {
    setErrorCondition(-556);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Triangles", -556);
    return;
  }
  int ntri = trianglesPtr->getNumberOfTuples();

  // get the triangle definitions - use the pointer to the start of the Struct Array
  FaceArray::Face_t* triangles = trianglesPtr->getPointer(0);

  // need to make a list of triangle edges
  // each triangle has three edges, made up of two pairs of vertices
  QMap<int64_t, int> uedges_id_map;

  QMap<int64_t, FaceArray::UniqueFaceIds_t > edgeTriangleSet;

  int edge_id = 0;
  int cur_edge_id = 0;

  struct  { int32_t v0; int32_t v1; } edge;
  int64_t* u64Edge = reinterpret_cast<int64_t*>(&edge); // This pointer is a 64 bit integer interpretation of the above struct variable

  typedef QMap<int64_t, int>::iterator EdgesIdMapIterator_t;

  float curPercent = 0.0;


  for(int i = 0; i < ntri; ++i)
  {
    if ( static_cast<float>(i) / static_cast<float>(ntri) * 100.0f > (curPercent) )
    {
      QString ss = QObject::tr("Stage 1/2: %1% Complete").arg(static_cast<float>(i) / static_cast<float>(ntri) * 100.0f);
      notifyStatusMessage(ss);
      curPercent += 5.0f;
    }
    if (getCancel() == true) { return; }

    FaceArray::Face_t& tri = triangles[i];
    // Edge 0
    edge.v0 = tri.verts[0];
    edge.v1 = tri.verts[1];
    if (edge.v0 > edge.v1) { edge.v0 = tri.verts[1]; edge.v1 = tri.verts[0]; }
    EdgesIdMapIterator_t iter = uedges_id_map.find(*u64Edge);
    if ( iter == uedges_id_map.end())
    {
      uedges_id_map[*u64Edge] = edge_id;
      cur_edge_id = edge_id;
//       tri.e_id[0] = cur_edge_id;
      ++edge_id;
    }
    else
    {
//      tri.e_id[0] = iter.value();
    }
    edgeTriangleSet[*u64Edge].insert(i);

    // Edge 1
    edge.v0 = tri.verts[1];
    edge.v1 = tri.verts[2];
    if (edge.v0 > edge.v1) { edge.v0 = tri.verts[2]; edge.v1 = tri.verts[1]; }
    iter = uedges_id_map.find(*u64Edge);
    if ( iter == uedges_id_map.end())
    {
      uedges_id_map[*u64Edge] = edge_id;
      cur_edge_id = edge_id;
//       tri.e_id[1] = cur_edge_id;
      ++edge_id;
    }
    else
    {
//      tri.e_id[1] = iter.value();
    }
    edgeTriangleSet[*u64Edge].insert(i);

    // Edge 2
    edge.v0 = tri.verts[2];
    edge.v1 = tri.verts[0];
    if (edge.v0 > edge.v1) { edge.v0 = tri.verts[0]; edge.v1 = tri.verts[2]; }
    iter = uedges_id_map.find(*u64Edge);
    if ( iter == uedges_id_map.end())
    {
      uedges_id_map[*u64Edge] = edge_id;
      cur_edge_id = edge_id;
//       tri.e_id[2] = cur_edge_id;
      ++edge_id;
    }
    else
    {
//      tri.e_id[2] = iter.value();
    }
    edgeTriangleSet[*u64Edge].insert(i);
  }


  notifyStatusMessage("Generating edge list for mesh. Stage 2 of 2");
  // Now copy the unique Edges out of the map and into an array at the proper index (which is the "value" that goes with the "key" to the map.
  int index = 0;
  QVector<int> dims(1, 2);
  Int32ArrayType::Pointer uniqueEdgesArrayPtr = Int32ArrayType::CreateArray(uedges_id_map.size(), dims, DREAM3D::EdgeData::SurfaceMeshUniqueEdges);
  m_SurfaceMeshUniqueEdges = uniqueEdgesArrayPtr->getPointer(0);

  ManagedArrayOfArrays<int>::Pointer edgeTriangleArray = ManagedArrayOfArrays<int>::CreateArray(edgeTriangleSet.size(), DREAM3D::EdgeData::SurfaceMeshEdgeFaces);

  float progIndex = 0.0;
  curPercent = 0.0;
  float total = static_cast<float>(uedges_id_map.size());


  for(QMap<int64_t, int>::iterator iter = uedges_id_map.begin(); iter != uedges_id_map.end(); ++iter)
  {

    if ( progIndex / total * 100.0f > (curPercent) )
    {
      QString ss = QObject::tr("Stage 2/2: %1% Complete").arg(progIndex / total * 100.0f);
      notifyStatusMessage(ss);
      curPercent += 5.0f;
    }
    progIndex++;
    if (getCancel() == true) { return; }

    *u64Edge = iter.key();
    index = iter.value();
    m_SurfaceMeshUniqueEdges[index * 2] = edge.v0;
    m_SurfaceMeshUniqueEdges[index * 2 + 1] = edge.v1;

    ManagedArrayOfArrays<int>::Data_t& entry = *(edgeTriangleArray->getPointer(index));
    FaceArray::UniqueFaceIds_t& tris = edgeTriangleSet[*u64Edge];
    // Allocate enough memory to hold the list of triangles
    entry.count = tris.size();
    if (entry.count > 0)
    {
      entry.data = (int*)(malloc(sizeof(int) * entry.count));
      index = 0;
      for(FaceArray::UniqueFaceIds_t::iterator tIter = tris.begin(); tIter != tris.end(); ++tIter)
      {
        entry.data[index++] = *tIter; // Copy the value from the triangle Ids set into the ManagedPointer
      }
    }
  }

  // Finally push both the arrays into the Data Container for the pipeline
  sm->getAttributeMatrix(getVertexAttributeMatrixName())->addAttributeArray(uniqueEdgesArrayPtr->GetName(), uniqueEdgesArrayPtr);
  sm->getAttributeMatrix(getVertexAttributeMatrixName())->addAttributeArray(edgeTriangleArray->GetName(), edgeTriangleArray);

  notifyStatusMessage("Complete");
  return;
}


