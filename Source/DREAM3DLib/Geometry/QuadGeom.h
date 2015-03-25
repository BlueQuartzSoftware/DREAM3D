/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistribution of source code must retain the above copyright notice, this
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
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, SQTRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _QuadGeom_H_
#define _QuadGeom_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Geometry/IGeometry.h"
#include "DREAM3DLib/Geometry/IGeometry2D.h"
#include "DREAM3DLib/Geometry/GeometryHelpers.hpp"
#include "DREAM3DLib/Geometry/DerivativeHelpers.h"

/**
 * @brief The QuadGeom class represents a collection of quadrilaterals
 */
class DREAM3DLib_EXPORT QuadGeom : public IGeometry2D
{
  public:

    DREAM3D_SHARED_POINTERS(QuadGeom)
    DREAM3D_STATIC_NEW_MACRO(QuadGeom)
    DREAM3D_TYPE_MACRO_SUPER(QuadGeom, Observable)

    virtual ~QuadGeom();

    /**
     * @brief CreateGeometry
     * @param numQuads
     * @param vertices
     * @param name
     * @return
     */
    static Pointer CreateGeometry(int64_t numQuads, SharedVertexList::Pointer vertices, const QString& name);

    /**
     * @brief CreateGeometry
     * @param quads
     * @param vertices
     * @param name
     * @return
     */
    static Pointer CreateGeometry(SharedQuadList::Pointer quads, SharedVertexList::Pointer vertices, const QString& name);

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
    void setCoords(int64_t vertId, float coords[3]);

    /**
     * @brief getCoords
     * @param vertId
     * @param coords
     */
    void getCoords(int64_t vertId, float coords[3]);

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
    void setVertsAtEdge(int64_t edgeId, int64_t verts[2]);

    /**
     * @brief getVerts
     * @param edgeId
     * @param verts
     */
    void getVertsAtEdge(int64_t edgeId, int64_t verts[2]);

    /**
     * @brief getVertCoordsAtEdge
     * @param edgeId
     * @param vert1
     * @param vert2
     */
    void getVertCoordsAtEdge(int64_t edgeId, float vert1[3], float vert2[3]);

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
// Inherited from SharedQuadOps
// -----------------------------------------------------------------------------

    /**
     * @brief CreateSharedQuadLost
     * @param numQuads
     * @return
     */
    static SharedQuadList::Pointer CreateSharedQuadList(int64_t numQuads);

    /**
     * @brief resizeQuadList
     * @param newNumQuads
     */
    void resizeQuadList(int64_t newNumQuads);

    /**
     * @brief setQuads
     * @param quads
     */
    void setQuads(SharedQuadList::Pointer quads);

    /**
     * @brief getQuads
     * @return
     */
    SharedQuadList::Pointer getQuads();

    /**
     * @brief setVertsAtQuad
     * @param quadId
     * @param verts
     */
    void setVertsAtQuad(int64_t quadId, int64_t verts[4]);

    /**
     * @brief getVertsAtQuad
     * @param quadId
     * @param verts
     */
    void getVertsAtQuad(int64_t quadId, int64_t verts[4]);

    /**
     * @brief getVertCoordsAtQuad
     * @param quadId
     * @param vert1
     * @param vert2
     * @param vert3
     * @param vert4
     */
    void getVertCoordsAtQuad(int64_t quadId, float vert1[3], float vert2[3], float vert3[3], float vert4[3]);

    /**
     * @brief getPointer
     * @param i
     * @return
     */
    int64_t* getQuadPointer(int64_t i);

    /**
     * @brief getNumberOfQuads
     * @return
     */
    int64_t getNumberOfQuads();

// -----------------------------------------------------------------------------
// Inherited from IGeometry
// -----------------------------------------------------------------------------

    /**
     * @brief initializeWithZeros
     */
    virtual void initializeWithZeros();

    /**
     * @brief getNumberOfTuples
     * @return
     */
    virtual size_t getNumberOfTuples();

    /**
     * @brief findElementsContainingVert
     * @return
     */
    virtual int findElementsContainingVert();

    /**
     * @brief getElementsContainingVert
     * @return
     */
    virtual ElementDynamicList::Pointer getElementsContainingVert();

    /**
     * @brief deleteElementsContainingVert
     */
    virtual void deleteElementsContainingVert();

    /**
     * @brief findElementNeighbors
     * @return
     */
    virtual int findElementNeighbors();

    /**
     * @brief getElementNeighbors
     * @return
     */
    virtual ElementDynamicList::Pointer getElementNeighbors();

    /**
     * @brief deleteElementNeighbors
     */
    virtual void deleteElementNeighbors();

    /**
     * @brief findElementCentroids
     * @return
     */
    virtual int findElementCentroids();

    /**
     * @brief getElementCentroids
     * @return
     */
    virtual FloatArrayType::Pointer getElementCentroids();

    /**
     * @brief deleteElementCentroids
     */
    virtual void deleteElementCentroids();

    /**
     * @brief getParametricCenter
     * @param pCoords
     */
    virtual void getParametricCenter(double pCoords[3]);

    /**
     * @brief getShapeFunctions
     * @param pCoords
     * @param shape
     */
    virtual void getShapeFunctions(double pCoords[3], double* shape);

    /**
     * @brief findDerivatives
     * @param field
     * @param derivatives
     */
    virtual void findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives);

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
     * @brief getGeometryTypeAsSQuadng
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

    /**
     * @brief deepCopy
     * @return
     */
    virtual IGeometry::Pointer deepCopy();

    /**
     * @brief addAttributeMatrix
     */
    virtual void addAttributeMatrix(const QString& name, AttributeMatrix::Pointer data);

    /**
     * @brief getAttributeMatrix
     * @param name
     * @return
     */
    virtual AttributeMatrix::Pointer getAttributeMatrix(const QString& name);

    /**
     * @brief removeAttributeMatrix
     * @param name
     * @return
     */
    virtual AttributeMatrix::Pointer removeAttributeMatrix(const QString& name);

// -----------------------------------------------------------------------------
// Inherited from IGeometry2D
// -----------------------------------------------------------------------------

    /**
     * @brief findElementEdges
     * @return
     */
    virtual int findElementEdges();

    /**
     * @brief getElementEdges
     * @return
     */
    virtual SharedEdgeList::Pointer getElementEdges();

    /**
     * @brief deleteElementEdges
     */
    virtual void deleteElementEdges();

    /**
     * @brief findUnsharedEdges
     */
    virtual int findUnsharedEdges();

    /**
     * @brief getUnsharedEdges
     * @return
     */
    virtual SharedEdgeList::Pointer getUnsharedEdges();

    /**
     * @brief deleteUnsharedEdges
     */
    virtual void deleteUnsharedEdges();

  protected:

    QuadGeom();

    /**
     * @brief setElementsContainingVert
     * @param elementsContainingVert
     */
    virtual void setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert);

    /**
     * @brief setElementNeighbors
     * @param elementNeighbors
     */
    virtual void setElementNeighbors(ElementDynamicList::Pointer elementNeighbors);

    /**
     * @brief setElementCentroids
     * @param elementCentroids
     */
    virtual void setElementCentroids(FloatArrayType::Pointer elementCentroids);

    /**
     * @brief setUnsharedEdges
     * @param bEdgeList
     */
    virtual void setUnsharedEdges(SharedEdgeList::Pointer bEdgeList);

  private:

    QString m_Name;
    QString m_GeometryTypeName;
    unsigned int m_GeometryType;
    unsigned int m_XdmfGridType;
    unsigned int m_UnitDimensionality;
    unsigned int m_SpatialDimensionality;
    AttributeMatrixMap_t m_AttributeMatrices;
    SharedVertexList::Pointer m_VertexList;
    SharedEdgeList::Pointer m_EdgeList;
    SharedEdgeList::Pointer m_UnsharedEdgeList;
    SharedQuadList::Pointer m_QuadList;
    ElementDynamicList::Pointer m_QuadsContainingVert;
    ElementDynamicList::Pointer m_QuadNeighbors;
    FloatArrayType::Pointer m_QuadCentroids;

    QuadGeom(const QuadGeom&); // Copy Constructor Not Implemented
    void operator=(const QuadGeom&); // Operator '=' Not Implemented
};


#endif /* _QuadGeom_H_ */

