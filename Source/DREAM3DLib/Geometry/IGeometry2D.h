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

#ifndef _IGeometry2D_H_
#define _IGeometry2D_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Geometry/IGeometry.h"

/**
 * @brief The IGeometry2D class extends IGeometry for elements with a topology of 2D
 */
class DREAM3DLib_EXPORT IGeometry2D : public IGeometry
{
  public:
    DREAM3D_SHARED_POINTERS(IGeometry2D)
    DREAM3D_TYPE_MACRO_SUPER(IGeometry2D, Observable)

    IGeometry2D();
    virtual ~IGeometry2D();

// -----------------------------------------------------------------------------
// Inherited from SharedVertexOps
// -----------------------------------------------------------------------------

    /**
     * @brief resizeVertexList
     * @param newNumVertices
     */
    virtual void resizeVertexList(int64_t newNumVertices) = 0;

    /**
     * @brief setVertices
     * @param vertices
     */
    virtual void setVertices(SharedVertexList::Pointer vertices) = 0;

    /**
     * @brief getVertices
     * @return
     */
    virtual SharedVertexList::Pointer getVertices() = 0;

    /**
     * @brief setCoords
     * @param vertId
     * @param coords
     */
    virtual void setCoords(int64_t vertId, float coords[3]) = 0;

    /**
     * @brief getCoords
     * @param vertId
     * @param coords
     */
    virtual void getCoords(int64_t vertId, float coords[3]) = 0;

    /**
     * @brief getVertexPointer
     * @param i
     * @return
     */
    virtual float* getVertexPointer(int64_t i) = 0;

    /**
     * @brief getNumberOfVertices
     * @return
     */
    virtual int64_t getNumberOfVertices() = 0;

// -----------------------------------------------------------------------------
// Inherited from SharedEdgeOps
// -----------------------------------------------------------------------------

    /**
     * @brief resizeEdgeList
     * @param newNumEdges
     */
    virtual void resizeEdgeList(int64_t newNumEdges) = 0;

    /**
     * @brief getEdges
     * @return
     */
    virtual SharedEdgeList::Pointer getEdges() = 0;

    /**
     * @brief setVerts
     * @param edgeId
     * @param verts
     */
    virtual void setVertsAtEdge(int64_t edgeId, int64_t verts[2]) = 0;

    /**
     * @brief getVerts
     * @param edgeId
     * @param verts
     */
    virtual void getVertsAtEdge(int64_t edgeId, int64_t verts[2]) = 0;

    /**
     * @brief getVertCoordsAtEdge
     * @param edgeId
     * @param vert1
     * @param vert2
     */
    virtual void getVertCoordsAtEdge(int64_t edgeId, float vert1[3], float vert2[3]) = 0;

    /**
     * @brief getEdgePointer
     * @param i
     * @return
     */
    virtual int64_t* getEdgePointer(int64_t i) = 0;

    /**
     * @brief getNumberOfEdges
     * @return
     */
    virtual int64_t getNumberOfEdges() = 0;


// -----------------------------------------------------------------------------
// Connectivity
// -----------------------------------------------------------------------------

    /**
     * @brief findElementEdges
     * @return
     */
    virtual int findEdges() = 0;

    /**
     * @brief deleteElementEdges
     */
    virtual void deleteEdges() = 0;

// -----------------------------------------------------------------------------
// Topology
// -----------------------------------------------------------------------------

    /**
     * @brief findUnsharedEdges
     */
    virtual int findUnsharedEdges() = 0;

    /**
     * @brief getUnsharedEdges
     * @return
     */
    virtual SharedEdgeList::Pointer getUnsharedEdges() = 0;

    /**
     * @brief deleteUnsharedEdges
     */
    virtual void deleteUnsharedEdges() = 0;

  protected:

    /**
     * @brief setEdges
     * @param edges
     */
    virtual void setEdges(SharedEdgeList::Pointer edges) = 0;

    /**
     * @brief setUnsharedEdges
     * @param bEdgeList
     */
    virtual void setUnsharedEdges(SharedEdgeList::Pointer bEdgeList) = 0;


  private:

    IGeometry2D(const IGeometry2D&); // Copy Constructor Not Implemented
    void operator=(const IGeometry2D&); // Operator '=' Not Implemented
};

#endif /* _IGeometry2D_H_ */

