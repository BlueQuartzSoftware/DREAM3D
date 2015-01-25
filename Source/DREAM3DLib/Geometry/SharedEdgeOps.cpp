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
  if (edges->getName().compare(DREAM3D::Geometry::SharedEdgeList) != 0)
  {
    edges->setName(DREAM3D::Geometry::SharedEdgeList);
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
void GEOM_CLASS_NAME::setVertsAtEdge(int64_t edgeId, int64_t* verts)
{
  int64_t* Edge = m_EdgeList->getTuplePointer(edgeId);
  Edge[0] = verts[0];
  Edge[1] = verts[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::getVertsAtEdge(int64_t edgeId, int64_t* verts)
{
  int64_t* Edge = m_EdgeList->getTuplePointer(edgeId);
  verts[0] = Edge[0];
  verts[1] = Edge[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::getVertReferencesAtEdge(int64_t edgeId, float* vert1, float* vert2)
{
  int64_t* Edge = m_EdgeList->getTuplePointer(edgeId);
  vert1 = m_VertexList->getTuplePointer(Edge[0]);
  vert2 = m_VertexList->getTuplePointer(Edge[1]);
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
