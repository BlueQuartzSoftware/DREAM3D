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

#ifndef _ImageGeom_H_
#define _ImageGeom_H_

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Geometry/IGeometry.h"

/**
 * @brief The ImageGeom class represents a structured rectlinear grid
 */
class SIMPLib_EXPORT ImageGeom : public IGeometry
{
  public:
    SIMPL_SHARED_POINTERS(ImageGeom)
    SIMPL_STATIC_NEW_MACRO(ImageGeom)
    SIMPL_TYPE_MACRO_SUPER(ImageGeom, Observable)

    virtual ~ImageGeom();

    /**
     * @brief CreateGeometry
     * @param name
     * @return
     */
    static Pointer CreateGeometry(const QString& name);

    SIMPL_INSTANCE_VEC3_PROPERTY(size_t, Dimensions)

    inline size_t getXPoints() { return m_Dimensions[0]; }
    inline size_t getYPoints() { return m_Dimensions[1]; }
    inline size_t getZPoints() { return m_Dimensions[2]; }

    SIMPL_INSTANCE_VEC3_PROPERTY(float, Resolution)

    inline float getXRes() { return m_Resolution[0]; }
    inline float getYRes() { return m_Resolution[1]; }
    inline float getZRes() { return m_Resolution[2]; }

    SIMPL_INSTANCE_VEC3_PROPERTY(float, Origin)

    void getCoords(size_t idx[3], float coords[3]);

    void getCoords(size_t x, size_t y, size_t z, float coords[3]);

    void getCoords(size_t idx, float coords[3]);

    void getCoords(size_t idx[3], double coords[3]);

    void getCoords(size_t x, size_t y, size_t z, double coords[3]);

    void getCoords(size_t idx, double coords[3]);

// -----------------------------------------------------------------------------
// Inherited from IGeometry
// -----------------------------------------------------------------------------

    /**
     * @brief initializeWithZeros
     */
    virtual void initializeWithZeros();

    /**
     * @brief getNumberOfElements
     * @return
     */
    virtual size_t getNumberOfElements();

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
    virtual void findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives, Observable* observable = NULL);

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
     * @brief getInfoString
     * @return Returns a formatted string that contains general infomation about
     * the instance of the object.
     */
    virtual QString getInfoString(DREAM3D::InfoStringFormat format);

    /**
     * @brief setMessagePrefix
     * @param prefix
     */
    virtual void setMessagePrefix(const QString& prefix);

    /**
     * @brief getMessagePrefix
     * @return
     */
    virtual QString getMessagePrefix();

    /**
     * @brief setMessageTitle
     * @param title
     */
    virtual void setMessageTitle(const QString& title);

    /**
     * @brief getMessageTitle
     * @return
     */
    virtual QString getMessageTitle();

    /**
     * @brief setMessageLabel
     * @param label
     */
    virtual void setMessageLabel(const QString& label);

    /**
     * @brief getMessageLabel
     * @return
     */
    virtual QString getMessageLabel();

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

    ImageGeom();

    /**
     * @brief Gathers the image geometry from an HDF5 file
     * @param parentid
     * @param volDims
     * @param spacing
     * @param origin
     * @return
     */
    virtual int gatherMetaData(hid_t parentid, size_t volDims[3], float spacing[3], float origin[3], unsigned int spatialDims, QString geomName);

    /**
     * @brief sendThreadSafeProgressMessage
     * @param counter
     * @param max
     */
    virtual void sendThreadSafeProgressMessage(int64_t counter, int64_t max);

    /**
     * @brief setElementsContaingVert
     * @param elementsContaingVert
     */
    virtual void setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert);

    /**
     * @brief setElementNeighbors
     * @param elementNeighbors
     */
    virtual void setElementNeighbors(ElementDynamicList::Pointer elementsNeighbors);

    /**
     * @brief setElementCentroids
     * @param elementCentroids
     */
    virtual void setElementCentroids(FloatArrayType::Pointer elementCentroids);

  private:

    QString m_Name;
    QString m_GeometryTypeName;
    QString m_MessagePrefix;
    QString m_MessageTitle;
    QString m_MessageLabel;
    unsigned int m_GeometryType;
    unsigned int m_XdmfGridType;
    unsigned int m_UnitDimensionality;
    unsigned int m_SpatialDimensionality;
    AttributeMatrixMap_t m_AttributeMatrices;
    QMutex m_Mutex;
    int64_t m_ProgressCounter;

    friend class FindImageDerivativesImpl;

    ImageGeom(const ImageGeom&); // Copy Constructor Not Implemented
    void operator=(const ImageGeom&); // Operator '=' Not Implemented
};


#endif /* _ImageGeom_H_ */

