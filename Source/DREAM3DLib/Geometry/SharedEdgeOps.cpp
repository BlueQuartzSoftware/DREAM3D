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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedEdgeList::Pointer GEOM_CLASS_NAME::CreateSharedEdgeList(int64_t numEdges)
{
  QVector<size_t> edgeDims(1, 2);
  SharedEdgeList::Pointer edges = SharedEdgeList::CreateArray(numEdges, edgeDims, DREAM3D::Geometry::SharedEdgeList);
  edges->initializeWithZeros();
  return edges;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::resizeEdgeList(int64_t newNumEdges)
{
  m_EdgeList->resize(newNumEdges);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setEdges(SharedEdgeList::Pointer edges)
{
  if (edges.get() != NULL)
  {
    if (edges->getName().compare(DREAM3D::Geometry::SharedEdgeList) != 0)
    {
      edges->setName(DREAM3D::Geometry::SharedEdgeList);
    }
  }
  m_EdgeList = edges;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedEdgeList::Pointer GEOM_CLASS_NAME::getEdges()
{
  return m_EdgeList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setVertsAtEdge(int64_t edgeId, int64_t verts[2])
{
  int64_t* Edge = m_EdgeList->getTuplePointer(edgeId);
  Edge[0] = verts[0];
  Edge[1] = verts[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::getVertsAtEdge(int64_t edgeId, int64_t verts[2])
{
  int64_t* Edge = m_EdgeList->getTuplePointer(edgeId);
  verts[0] = Edge[0];
  verts[1] = Edge[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::getVertCoordsAtEdge(int64_t edgeId, float vert1[3], float vert2[3])
{
  int64_t* Edge = m_EdgeList->getTuplePointer(edgeId);
  float* tmp1 = m_VertexList->getTuplePointer(Edge[0]);
  float* tmp2 = m_VertexList->getTuplePointer(Edge[1]);
  vert1[0] = tmp1[0];
  vert1[1] = tmp1[1];
  vert1[2] = tmp1[2];
  vert2[0] = tmp2[0];
  vert2[1] = tmp2[1];
  vert2[2] = tmp2[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t* GEOM_CLASS_NAME::getEdgePointer(int64_t i)
{
  return m_EdgeList->getTuplePointer(i);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t GEOM_CLASS_NAME::getNumberOfEdges()
{
  return m_EdgeList->getNumberOfTuples();
}
