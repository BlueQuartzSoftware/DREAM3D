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
  float* tmp1 = m_VertexList->getTuplePointer(Quad[0]);
  float* tmp2 = m_VertexList->getTuplePointer(Quad[1]);
  float* tmp3 = m_VertexList->getTuplePointer(Quad[2]);
  float* tmp4 = m_VertexList->getTuplePointer(Quad[3]);
  vert1[0] = tmp1[0];
  vert1[1] = tmp1[1];
  vert1[2] = tmp1[2];
  vert2[0] = tmp2[0];
  vert2[1] = tmp2[1];
  vert2[2] = tmp2[2];
  vert3[0] = tmp3[0];
  vert3[1] = tmp3[1];
  vert3[2] = tmp3[2];
  vert4[0] = tmp4[0];
  vert4[1] = tmp4[1];
  vert4[2] = tmp4[2];
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
