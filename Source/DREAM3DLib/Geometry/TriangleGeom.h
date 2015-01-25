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
#ifndef _TriangleGeom_H_
#define _TriangleGeom_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/DynamicListArray.hpp"
#include "DREAM3DLib/Geometry/IGeometry.h"
#include "DREAM3DLib/Geometry/VertexGeom.h"
#include "DREAM3DLib/Geometry/EdgeGeom.h"

/**
 * @brief The TriangleGeom class represents a collection of triangles
 */
class DREAM3DLib_EXPORT TriangleGeom : public IGeometry
{
  public:

    DREAM3D_SHARED_POINTERS(TriangleGeom)
    DREAM3D_STATIC_NEW_MACRO(TriangleGeom)
    DREAM3D_TYPE_MACRO_SUPER(TriangleGeom, Observable)

    virtual ~TriangleGeom();

    DREAM3D_INSTANCE_PROPERTY(UInt16Int64DynamicListArray::Pointer, TrianglesContainingVert)
    DREAM3D_INSTANCE_PROPERTY(UInt16Int64DynamicListArray::Pointer, TriangleNeighbors)

    /**
     * @brief CreateGeometry
     * @param numTriangles
     * @param vertices
     * @param name
     * @return
     */
    static Pointer CreateGeometry(int64_t numTriangles, SharedVertexList::Pointer vertices, const QString& name);

    /**
     * @brief CreateGeometry
     * @param triangles
     * @param vertices
     * @param name
     * @return
     */
    static Pointer CreateGeometry(SharedTriList::Pointer triangles, SharedVertexList::Pointer vertices, const QString& name);

    /**
     * @brief CreateGeometry
     * @param triangles
     * @param edges
     * @param vertices
     * @param name
     * @return
     */
    static Pointer CreateGeometry(SharedTriList::Pointer trianglesByEdge, SharedEdgeList::Pointer edges, SharedVertexList::Pointer vertices, const QString& name);

    /**
     * @brief initializeWithZeros
     */
    void initializeWithZeros();

    /**
     * @brief findTrianglesContainingVert
     */
    void findTrianglesContainingVert();

    /**
     * @brief deleteTrianglesContainingVert
     */
    void deleteTrianglesContainingVert();

    /**
     * @brief findTriangleNeighbors
     */
    void findTriangleNeighbors();

    /**
     * @brief deleteTriangleNeighbors
     */
    void deleteTriangleNeighbors();

// -----------------------------------------------------------------------------
// Inherited from SharedVertexOps
// -----------------------------------------------------------------------------

    /**
     * @brief CreateSharedVertexList
     * @param numVertices
     * @return
     */
    static SharedVertexList::Pointer CreateSharedVertexList(int64_t numVertices);

    /**
     * @brief resizeVertexList
     * @param newNumVertices
     */
    void resizeVertexList(int64_t newNumVertices);

    /**
     * @brief setVertices
     * @param vertices
     */
    void setVertices(SharedVertexList::Pointer vertices);

    /**
     * @brief getVertices
     * @return
     */
    SharedVertexList::Pointer getVertices();

    /**
     * @brief setCoords
     * @param vertId
     * @param coords
     */
    void setCoords(int64_t vertId, float* coords);

    /**
     * @brief getCoords
     * @param vertId
     * @param coords
     */
    void getCoords(int64_t vertId, float* coords);

    /**
     * @brief getVertexPointer
     * @param i
     * @return
     */
    float* getVertexPointer(int64_t i);

    /**
     * @brief getNumberOfVertices
     * @return
     */
    int64_t getNumberOfVertices();

// -----------------------------------------------------------------------------
// Inherited from SharedEdgeOps
// -----------------------------------------------------------------------------

    /**
     * @brief CreateSharedEdgeList
     * @param numEdges
     * @return
     */
    static SharedEdgeList::Pointer CreateSharedEdgeList(int64_t numEdges);

    /**
     * @brief resizeEdgeList
     * @param newNumEdges
     */
    void resizeEdgeList(int64_t newNumEdges);

    /**
     * @brief setEdges
     * @param edges
     */
    void setEdges(SharedEdgeList::Pointer edges);

    /**
     * @brief getEdges
     * @return
     */
    SharedEdgeList::Pointer getEdges();

    /**
     * @brief setVerts
     * @param edgeId
     * @param verts
     */
    void setVertsAtEdge(int64_t edgeId, int64_t* verts);

    /**
     * @brief getVerts
     * @param edgeId
     * @param verts
     */
    void getVertsAtEdge(int64_t edgeId, int64_t* verts);

    /**
     * @brief getVertReferences
     * @param edgeId
     * @param vert1
     * @param vert2
     */
    void getVertReferencesAtEdge(int64_t edgeId, float* vert1, float* vert2);

    /**
     * @brief getEdgePointer
     * @param i
     * @return
     */
    int64_t* getEdgePointer(int64_t i);

    /**
     * @brief getNumberOfEdges
     * @return
     */
    int64_t getNumberOfEdges();

// -----------------------------------------------------------------------------
// Inherited from SharedTriOps
// -----------------------------------------------------------------------------

    /**
     * @brief CreateSharedTriLost
     * @param numTris
     * @return
     */
    static SharedTriList::Pointer CreateSharedTriList(int64_t numTris);

    /**
     * @brief resizeTriList
     * @param newNumTris
     */
    void resizeTriList(int64_t newNumTris);

    /**
     * @brief setTriangles
     * @param triangles
     */
    void setTriangles(SharedTriList::Pointer triangles);

    /**
     * @brief getTriangles
     * @return
     */
    SharedTriList::Pointer getTriangles();

    /**
     * @brief setVertsAtTri
     * @param triId
     * @param verts
     */
    void setVertsAtTri(int64_t triId, int64_t* verts);

    /**
     * @brief getVertsAtTri
     * @param triId
     * @param verts
     */
    void getVertsAtTri(int64_t triId, int64_t* verts);

    /**
     * @brief getVertReferencesAtTri
     * @param triId
     * @param vert1
     * @param vert2
     * @param vert3
     */
    void getVertReferencesAtTri(int64_t triId, float* vert1, float* vert2, float* vert3);

    /**
     * @brief getPointer
     * @param i
     * @return
     */
    int64_t* getTriPointer(int64_t i);

    /**
     * @brief getNumberOfTris
     * @return
     */
    int64_t getNumberOfTris();

// -----------------------------------------------------------------------------
// Inherited from IGeometry
// -----------------------------------------------------------------------------

    /**
     * @brief setName
     * @param name
     */
    virtual void setName(const QString& name);

    /**
     * @brief getName
     * @return
     */
    virtual QString getName();

    /**
     * @brief getGeometryType
     * @return
     */
    virtual unsigned int getGeometryType();

    /**
     * @brief getGeometryTypeAsString
     * @return
     */
    virtual QString getGeometryTypeAsString();

    /**
     * @brief getXdmfGridType
     * @return
     */
    virtual unsigned int getXdmfGridType();

    /**
     * @brief getUnitDimensionality
     * @return
     */
    virtual unsigned int getUnitDimensionality();

    /**
     * @brief setSpatialDimensionality
     * @param spatialDims
     */
    virtual void setSpatialDimensionality(unsigned int spatialDims);

    /**
     * @brief getSpatialDimensionality
     * @return
     */
    virtual unsigned int getSpatialDimensionality();

    /**
     * @brief writeGeometryToHDF5
     * @param parentId
     * @param writeXdmf
     * @return
     */
    virtual int writeGeometryToHDF5(hid_t parentId, bool writeXdmf);

    /**
     * @brief writeXdmf
     * @param out
     * @param dcName
     * @param hdfFileName
     * @return
     */
    virtual int writeXdmf(QTextStream& out, QString dcName, QString hdfFileName);

    /**
     * @brief readGeometryFromHDF5
     * @param parentId
     * @param preflight
     * @return
     */
    virtual int readGeometryFromHDF5(hid_t parentId, bool preflight);

  protected:

    TriangleGeom();

  private:

    QString m_Name;
    QString m_GeometryTypeName;
    unsigned int m_GeometryType;
    unsigned int m_XdmfGridType;
    unsigned int m_UnitDimensionality;
    unsigned int m_SpatialDimensionality;
    SharedVertexList::Pointer m_VertexList;
    SharedEdgeList::Pointer m_EdgeList;
    SharedTriList::Pointer m_TriList;

    TriangleGeom(const TriangleGeom&); // Copy Constructor Not Implemented
    void operator=(const TriangleGeom&); // Operator '=' Not Implemented
};


#endif /* _TriangleGeom_H_ */

