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
SharedTriList::Pointer GEOM_CLASS_NAME::CreateSharedTriList(int64_t numTris)
{
  QVector<size_t> triDims(1, 3);
  SharedTriList::Pointer triangles = SharedEdgeList::CreateArray(numTris, triDims, DREAM3D::Geometry::SharedTriList);
  triangles->initializeWithZeros();
  return triangles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::resizeTriList(int64_t newNumTris)
{
  m_TriList->resize(newNumTris);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setTriangles(SharedTriList::Pointer triangles)
{
  if (triangles->getName().compare(DREAM3D::Geometry::SharedTriList) != 0)
  {
    triangles->setName(DREAM3D::Geometry::SharedTriList);
  }
  m_TriList = triangles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedTriList::Pointer GEOM_CLASS_NAME::getTriangles()
{
  return m_TriList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setVertsAtTri(int64_t triId, int64_t verts[3])
{
  int64_t* Tri = m_TriList->getTuplePointer(triId);
  Tri[0] = verts[0];
  Tri[1] = verts[1];
  Tri[2] = verts[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::getVertsAtTri(int64_t triId, int64_t verts[3])
{
  int64_t* Tri = m_TriList->getTuplePointer(triId);
  verts[0] = Tri[0];
  verts[1] = Tri[1];
  verts[2] = Tri[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::getVertCoordsAtTri(int64_t triId, float vert1[3], float vert2[3], float vert3[3])
{
  int64_t* Tri = m_TriList->getTuplePointer(triId);
  vert1 = m_VertexList->getTuplePointer(Tri[0]);
  vert2 = m_VertexList->getTuplePointer(Tri[1]);
  vert3 = m_VertexList->getTuplePointer(Tri[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t* GEOM_CLASS_NAME::getTriPointer(int64_t i)
{
  return m_TriList->getTuplePointer(i);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t GEOM_CLASS_NAME::getNumberOfTris()
{
  return m_TriList->getNumberOfTuples();
}
