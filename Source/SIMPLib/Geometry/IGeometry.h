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

#ifndef _IGeometry_H_
#define _IGeometry_H_

#include <QtCore/QString>
#include <QtCore/QMap>
#include <QMutex>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/Observable.h"
#include "SIMPLib/DataArrays/DynamicListArray.hpp"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"

class QTextStream;

// -----------------------------------------------------------------------------
// Typedefs
// -----------------------------------------------------------------------------
typedef FloatArrayType SharedVertexList;
typedef Int64ArrayType SharedEdgeList;
typedef Int64ArrayType SharedTriList;
typedef Int64ArrayType SharedQuadList;
typedef UInt16Int64DynamicListArray ElementDynamicList;

/**
 * @brief The IGeometry class
 */
class SIMPLib_EXPORT IGeometry : public Observable
{
  public:
    SIMPL_SHARED_POINTERS(IGeometry)
    SIMPL_TYPE_MACRO_SUPER(IGeometry, Observable)

    IGeometry();
    virtual ~IGeometry();

    /**
     * @brief AttributeMatrixMap_t
     */
    typedef QMap<QString, AttributeMatrix::Pointer> AttributeMatrixMap_t;

// -----------------------------------------------------------------------------
// Connectivity
// -----------------------------------------------------------------------------

    /**
     * @brief findElementsContainingVert
     * @return
     */
    virtual int findElementsContainingVert() = 0;

    /**
     * @brief getElementsContainingVert
     * @return
     */
    virtual ElementDynamicList::Pointer getElementsContainingVert() = 0;

    /**
     * @brief deleteElementsContainingVert
     */
    virtual void deleteElementsContainingVert() = 0;

    /**
     * @brief findElementNeighbors
     * @return
     */
    virtual int findElementNeighbors() = 0;

    /**
     * @brief getElementNeighbors
     * @return
     */
    virtual ElementDynamicList::Pointer getElementNeighbors() = 0;

    /**
     * @brief deleteElementNeighbors
     */
    virtual void deleteElementNeighbors() = 0;

// -----------------------------------------------------------------------------
// Topology
// -----------------------------------------------------------------------------

    /**
     * @brief getNumberOfElements
     * @return
     */
    virtual size_t getNumberOfElements() = 0;

    /**
     * @brief findElementCentroids
     * @return
     */
    virtual int findElementCentroids() = 0;

    /**
     * @brief getElementCentroids
     * @return
     */
    virtual FloatArrayType::Pointer getElementCentroids() = 0;

    /**
     * @brief deleteElementCentroids
     */
    virtual void deleteElementCentroids() = 0;

    /**
     * @brief getParametricCenter
     * @param pCoords
     */
    virtual void getParametricCenter(double pCoords[3]) = 0;

    /**
     * @brief getShapeFunctions
     * @param pCoords
     * @param derivs
     */
    virtual void getShapeFunctions(double pCoords[3], double* shape) = 0;

    /**
     * @brief findDerivatives
     * @param field
     * @param derivatives
     */
    virtual void findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives, Observable* observable) = 0;

// -----------------------------------------------------------------------------
// Generic
// -----------------------------------------------------------------------------

    /**
     * @brief setName
     * @param name
     */
    virtual void setName(const QString& name) = 0;

    /**
     * @brief getName
     * @return
     */
    virtual QString getName() = 0;

    /**
     * @brief getGeometryType
     * @return
     */
    virtual unsigned int getGeometryType() = 0;

    /**
     * @brief getGeometryTypeAsString
     * @return
     */
    virtual QString getGeometryTypeAsString() = 0;

    /**
     * @brief getInfoString
     * @return Returns a formatted string that contains general infomation about
     * the instance of the object.
     */
    virtual QString getInfoString(DREAM3D::InfoStringFormat) = 0;

    /**
     * @brief setMessagePrefix
     * @param prefix
     */
    virtual void setMessagePrefix(const QString& prefix) = 0;

    /**
     * @brief getMessagePrefix
     * @return
     */
    virtual QString getMessagePrefix() = 0;

    /**
     * @brief setMessageTitle
     * @param title
     */
    virtual void setMessageTitle(const QString& title) = 0;

    /**
     * @brief getMessageTitle
     * @return
     */
    virtual QString getMessageTitle() = 0;

    /**
     * @brief setMessageLabel
     * @param label
     */
    virtual void setMessageLabel(const QString& label) = 0;

    /**
     * @brief getMessageLabel
     * @return
     */
    virtual QString getMessageLabel() = 0;

    /**
     * @brief getXdmfGridType
     * @return
     */
    virtual unsigned int getXdmfGridType() = 0;

    /**
     * @brief getUnitDimensionality
     * @return
     */
    virtual unsigned int getUnitDimensionality() = 0;

    /**
     * @brief setSpatialDimensionality
     * @param spatialDims
     */
    virtual void setSpatialDimensionality(unsigned int spatialDims) = 0;

    /**
     * @brief getSpatialDimensionality
     * @return
     */
    virtual unsigned int getSpatialDimensionality() = 0;

    /**
     * @brief writeGeometryToHDF5
     * @param parentId
     * @param writeXdmf
     * @return
     */
    virtual int writeGeometryToHDF5(hid_t parentId, bool writeXdmf) = 0;

    /**
     * @brief writeXdmf
     * @param out
     * @param dcName
     * @param hdfFileName
     * @return
     */
    virtual int writeXdmf(QTextStream& out, QString dcName, QString hdfFileName) = 0;

    /**
     * @brief readGeometryFromHDF5
     * @param parentId
     * @param preflight
     * @return
     */
    virtual int readGeometryFromHDF5(hid_t parentId, bool preflight) = 0;

    /**
     * @brief deepCopy
     * @return
     */
    virtual Pointer deepCopy() = 0;

    /**
     * @brief initializeWithZeros
     */
    virtual void initializeWithZeros() = 0;

    /**
     * @brief addAttributeMatrix
     */
    virtual void addAttributeMatrix(const QString& name, AttributeMatrix::Pointer data) = 0;

    /**
     * @brief getAttributeMatrix
     * @param name
     * @return
     */
    virtual AttributeMatrix::Pointer getAttributeMatrix(const QString& name) = 0;

    /**
     * @brief removeAttributeMatrix
     * @param name
     * @return
     */
    virtual AttributeMatrix::Pointer removeAttributeMatrix(const QString& name) = 0;

  protected:

    /**
     * @brief sendThreadSafeProgressMessage
     * @param counter
     * @param max
     */
    virtual void sendThreadSafeProgressMessage(int64_t counter, int64_t max) = 0;

    /**
     * @brief setElementsContaingVert
     * @param elementsContaingVert
     */
    virtual void setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert) = 0;

    /**
     * @brief setElementNeighbors
     * @param elementNeighbors
     */
    virtual void setElementNeighbors(ElementDynamicList::Pointer elementsNeighbors) = 0;

    /**
     * @brief setElementCentroids
     * @param elementCentroids
     */
    virtual void setElementCentroids(FloatArrayType::Pointer elementCentroids) = 0;

  private:

    IGeometry(const IGeometry&); // Copy Constructor Not Implemented
    void operator=(const IGeometry&); // Operator '=' Not Implemented
};

#endif /* _IGeometry_H_ */

