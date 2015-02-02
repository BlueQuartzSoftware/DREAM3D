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
SharedVertexList::Pointer GEOM_CLASS_NAME::CreateSharedVertexList(int64_t numVertices)
{
  QVector<size_t> vertDims(1, 3);
  SharedVertexList::Pointer vertices = SharedVertexList::CreateArray(numVertices, vertDims, DREAM3D::Geometry::SharedVertexList);
  vertices->initializeWithZeros();
  return vertices;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::resizeVertexList(int64_t newNumVertices)
{
  m_VertexList->resize(newNumVertices);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setVertices(SharedVertexList::Pointer vertices)
{
  if (vertices->getName().compare(DREAM3D::Geometry::SharedVertexList) != 0)
  {
    vertices->setName(DREAM3D::Geometry::SharedVertexList);
  }
  m_VertexList = vertices;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedVertexList::Pointer GEOM_CLASS_NAME::getVertices()
{
  return m_VertexList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setCoords(int64_t vertId, float coords[3])
{
  float* Vert = m_VertexList->getTuplePointer(vertId);
  Vert[0] = coords[0];
  Vert[1] = coords[1];
  Vert[2] = coords[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::getCoords(int64_t vertId, float coords[3])
{
  float* Vert = m_VertexList->getTuplePointer(vertId);
  coords[0] = Vert[0];
  coords[1] = Vert[1];
  coords[2] = Vert[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float* GEOM_CLASS_NAME::getVertexPointer(int64_t i)
{
  return m_VertexList->getTuplePointer(i);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t GEOM_CLASS_NAME::getNumberOfVertices()
{
  return m_VertexList->getNumberOfTuples();
}
