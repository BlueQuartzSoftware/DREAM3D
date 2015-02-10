/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * RedisQuadbution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * RedisQuadbutions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * RedisQuadbutions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the disQuadbution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its conQuadbutors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONQuadBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONQuadBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, SQuadCT LIABILITY,
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
SharedQuadList::Pointer GEOM_CLASS_NAME::CreateSharedQuadList(int64_t numQuads)
{
  QVector<size_t> quadDims(1, 4);
  SharedQuadList::Pointer quads = SharedEdgeList::CreateArray(numQuads, quadDims, DREAM3D::Geometry::SharedQuadList);
  quads->initializeWithZeros();
  return quads;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::resizeQuadList(int64_t newNumQuads)
{
  m_QuadList->resize(newNumQuads);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setQuads(SharedQuadList::Pointer quads)
{
  if (quads.get() != NULL)
  {
    if (quads->getName().compare(DREAM3D::Geometry::SharedQuadList) != 0)
    {
      quads->setName(DREAM3D::Geometry::SharedQuadList);
    }
  }
  m_QuadList = quads;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedQuadList::Pointer GEOM_CLASS_NAME::getQuads()
{
  return m_QuadList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setVertsAtQuad(int64_t quadId, int64_t verts[4])
{
  int64_t* Quad = m_QuadList->getTuplePointer(quadId);
  Quad[0] = verts[0];
  Quad[1] = verts[1];
  Quad[2] = verts[2];
  Quad[3] = verts[3];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::getVertsAtQuad(int64_t quadId, int64_t verts[4])
{
  int64_t* Quad = m_QuadList->getTuplePointer(quadId);
  verts[0] = Quad[0];
  verts[1] = Quad[1];
  verts[2] = Quad[2];
  verts[3] = Quad[3];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::getVertCoordsAtQuad(int64_t quadId, float vert1[3], float vert2[3], float vert3[3], float vert4[3])
{
  int64_t* Quad = m_QuadList->getTuplePointer(quadId);
  vert1 = m_VertexList->getTuplePointer(Quad[0]);
  vert2 = m_VertexList->getTuplePointer(Quad[1]);
  vert3 = m_VertexList->getTuplePointer(Quad[2]);
  vert4 = m_VertexList->getTuplePointer(Quad[3]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t* GEOM_CLASS_NAME::getQuadPointer(int64_t i)
{
  return m_QuadList->getTuplePointer(i);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t GEOM_CLASS_NAME::getNumberOfQuads()
{
  return m_QuadList->getNumberOfTuples();
}
