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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "VertexArray.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexArray::VertexArray()
{
  m_Array = VertexContainerType::CreateArray(0, "VertexArray_Internal_Use_Only");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexArray::~VertexArray()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexArray::resizeArray(size_t newSize)
{
  m_Array->Resize(newSize);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t VertexArray::getNumberOfTuples()
{
  return m_Array->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t VertexArray::count()
{
  return m_Array->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexArray::Pointer VertexArray::CreateArray(size_t numElements, const QString& name)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  VertexArray* d = new VertexArray();
  d->resizeArray(numElements);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexArray::initializeWithZeros()
{
  m_Array->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexArray::getCoords(size_t vertId, float* coords)
{
  Vert_t& Vert = *(m_Array->getPointer(vertId));
  coords[0] = Vert.pos[0];
  coords[1] = Vert.pos[1];
  coords[2] = Vert.pos[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexArray::setCoords(size_t vertId, float* coords)
{
  Vert_t& Vert = *(m_Array->getPointer(vertId));
  Vert.pos[0] = coords[0];
  Vert.pos[1] = coords[1];
  Vert.pos[2] = coords[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexArray::Vert_t* VertexArray::getPointer(size_t i)
{
  return m_Array->getPointer(i);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexArray::Vert_t& VertexArray::operator[](size_t i)
{
  return (*m_Array)[i];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexArray::Vert_t& VertexArray::getVert(size_t i)
{
  return (*m_Array)[i];
}
