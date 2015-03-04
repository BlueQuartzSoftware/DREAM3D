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
#include "DREAM3DLib/Geometry/GeometryHelpers.hpp"

// placeholder includes until issues #286/#284 are fixed
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Geometry/DerivativeHelpers.h"

/**
 * @brief The QuadGeom class represents a collection of quadrilaterals
 */
class DREAM3DLib_EXPORT QuadGeom : public IGeometry
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

    // the following functions are placeholders until issues #286/#284 are fixed
    static void ComputeNormal(float n0[3], float n1[3], float n2[3], float normal[3])
    {
      float vert0[3];
      float vert1[3];
      float vert2[3];
      float u[3];
      float w[3];

      vert0[0] = static_cast<float>(n0[0]);
      vert0[1] = static_cast<float>(n0[1]);
      vert0[2] = static_cast<float>(n0[2]);

      vert1[0] = static_cast<float>(n1[0]);
      vert1[1] = static_cast<float>(n1[1]);
      vert1[2] = static_cast<float>(n1[2]);

      vert2[0] = static_cast<float>(n2[0]);
      vert2[1] = static_cast<float>(n2[1]);
      vert2[2] = static_cast<float>(n2[2]);

      // Compute the normal
      u[0] = vert1[0] - vert0[0];
      u[1] = vert1[1] - vert0[1];
      u[2] = vert1[2] - vert0[2];

      w[0] = vert2[0] - vert0[0];
      w[1] = vert2[1] - vert0[1];
      w[2] = vert2[2] - vert0[2];

      MatrixMath::CrossProduct(u, w, normal);
      MatrixMath::Normalize3x1(normal);
    }

   static void InterpolationDerivatives(float pCoords[3], float derivs[8])
   {
     float rm, sm;

     rm = 1.0f - pCoords[0];
     sm = 1.0f - pCoords[1];

     derivs[0] = -sm;
     derivs[1] = sm;
     derivs[2] = pCoords[1];
     derivs[3] = -pCoords[1];
     derivs[4] = -rm;
     derivs[5] = -pCoords[0];
     derivs[6] = pCoords[0];
     derivs[7] = rm;
   }

   static void GetParametricCenter(float pCoords[3])
   {
     pCoords[0] = 0.5f;
     pCoords[1] = 0.5f;
     pCoords[2] = 0.0f;
   }

   void findDerivatives(FloatArrayType::Pointer field, FloatArrayType::Pointer derivatives)
   {
     int64_t numQuads = getNumberOfQuads();
     std::vector<float> values(4);
     float derivs[3];
     int cDimsIn = field->getNumberOfComponents();
     float* fieldPtr = field->getPointer(0);
     float* derivPtr = derivatives->getPointer(0);
     int64_t verts[4];
     for (int64_t i = 0; i < numQuads; i++)
     {
       getVertsAtQuad(i, verts);
       for (int j = 0; j < cDimsIn; j++)
       {
         for (int k = 0; k < 4; k++)
         {
           values[k] = static_cast<float>(fieldPtr[cDimsIn*verts[k]+j]);
         }
         DerivativeHelpers::QuadDeriv()(this, i, &values[0], derivs, 1);
         derivPtr[i*3*cDimsIn+j*3] = static_cast<float>(derivs[0]);
         derivPtr[i*3*cDimsIn+j*3+1] = static_cast<float>(derivs[1]);
         derivPtr[i*3*cDimsIn+j*3+2] = static_cast<float>(derivs[2]);
       }
     }
   }

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
    SharedQuadList::Pointer m_QuadList;
    ElementDynamicList::Pointer m_QuadsContainingVert;
    ElementDynamicList::Pointer m_QuadNeighbors;
    FloatArrayType::Pointer m_QuadCentroids;

    QuadGeom(const QuadGeom&); // Copy Constructor Not Implemented
    void operator=(const QuadGeom&); // Operator '=' Not Implemented
};


#endif /* _QuadGeom_H_ */

