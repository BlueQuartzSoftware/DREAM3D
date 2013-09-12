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
#ifndef _VertexArray_H_
#define _VertexArray_H_


#include <string.h>

#include <boost/shared_array.hpp>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/MeshStructs.h"

/**
 * @brief The MeshLinks class contains arrays of Faces for each Node in the mesh. This allows quick query to the node
 * to determine what Cells the node is a part of.
 */
class VertexArray
{
  public:

    typedef struct
    {
        float pos[3];
    } Vert_t;

    DREAM3D_SHARED_POINTERS(VertexArray)
    DREAM3D_STATIC_NEW_MACRO(VertexArray)
    DREAM3D_TYPE_MACRO(VertexArray)

      /**
     * @brief Static constructor
     * @param numElements The number of elements in the internal array.
     * @param name The name of the array
     * @return Boost::Shared_Ptr wrapping an instance of DataArrayTemplate<T>
     */
    static Pointer CreateArray(size_t numElements, const QString &name)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      VertexArray* d = new VertexArray(numElements, true);
      if (d->Allocate() < 0)
      { // Could not allocate enough memory, reset the pointer to null and return
        delete d;
        return DataArray<T>::NullPointer();
      }
      d->SetName(name);
      Pointer ptr(d);
      return ptr;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    virtual ~VertexArray(){ }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void resizeArray(size_t newSize) { m_Array->Resize(newSize); }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    int64_t getNumberOfTuples() { return m_Array->getNumberOfTuples(); }
    size_t count() { return m_Array->getNumberOfTuples(); }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void getCoords(StructArray<Vert_t>::Pointer Verts, size_t vertId, float* coords)
    {
      Vert_t& Vert = *(Verts->GetPointer(vertId));
      coords[0] = Vert.pos[0];
      coords[1] = Vert.pos[1];
      coords[2] = Vert.pos[2];
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void setCoords(StructArray<Vert_t>::Pointer Verts, size_t vertId, float* coords)
    {
      Vert_t& Vert = *(Verts->GetPointer(vertId));
      Vert.pos[0] = coords[0];
      Vert.pos[1] = coords[1];
      Vert.pos[2] = coords[2];
    }

     /**
     * @brief Returns reference to the Face_t at the index i
     * @param i
     * @return
     */
    inline Vert_t& operator[](size_t i)
    {
      return (*m_Array)[i];
    }
     /**
     * @brief Returns reference to the Face_t at the index i
     * @param i
     * @return
     */
    inline Vert_t& getVert(size_t i)
    {
      return (*m_Array)[i];
    }


  protected:
    VertexArray(size_t);

  private:
    StructArray<Vert_t>::Pointer  m_Array;

    VertexArray(const VertexArray&); // Copy Constructor Not Implemented
    void operator=(const VertexArray&); // Operator '=' Not Implemented

};



#endif /* _VertexArray_H_ */
