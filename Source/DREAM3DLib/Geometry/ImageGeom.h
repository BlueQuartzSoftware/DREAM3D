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
#ifndef _ImageGeom_H_
#define _ImageGeom_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Geometry/IGeometry.h"
#include "DREAM3DLib/Geometry/VertexGeom.h"

/**
 * @brief The ImageGeom class represents a structured rectlinear grid
 */
class DREAM3DLib_EXPORT ImageGeom : public IGeometry
{
  public:
    DREAM3D_SHARED_POINTERS(ImageGeom)
    DREAM3D_STATIC_NEW_MACRO(ImageGeom)
    DREAM3D_TYPE_MACRO_SUPER(ImageGeom, Observable)

    virtual ~ImageGeom();

    /**
     * @brief CreateGeometry
     * @param name
     * @return
     */
    static Pointer CreateGeometry(const QString& name);

    DREAM3D_INSTANCE_VEC3_PROPERTY(size_t, Dimensions)

    size_t getXPoints() {return m_Dimensions[0];}
    size_t getYPoints() {return m_Dimensions[1];}
    size_t getZPoints() {return m_Dimensions[2];}

    DREAM3D_INSTANCE_VEC3_PROPERTY(float, Resolution)

    float getXRes() {return m_Resolution[0];}
    float getYRes() {return m_Resolution[1];}
    float getZRes() {return m_Resolution[2];}

    DREAM3D_INSTANCE_VEC3_PROPERTY(float, Origin)

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
     * @brief findCellsContainingVert
     * @return
     */
    virtual int findCellsContainingVert();

    /**
     * @brief getCellsContainingVert
     * @return
     */
    virtual CellDynamicList::Pointer getCellsContainingVert();

    /**
     * @brief deleteCellsContainingVert
     */
    virtual void deleteCellsContainingVert();

    /**
     * @brief findCellNeighbors
     * @return
     */
    virtual int findCellNeighbors();

    /**
     * @brief getCellNeighbors
     * @return
     */
    virtual CellDynamicList::Pointer getCellNeighbors();

    /**
     * @brief deleteCellNeighbors
     */
    virtual void deleteCellNeighbors();

    /**
     * @brief findCellCentroids
     * @return
     */
    virtual int findCellCentroids();

    /**
     * @brief getCellCentroids
     * @return
     */
    virtual FloatArrayType::Pointer getCellCentroids();

    /**
     * @brief deleteCellCentroids
     */
    virtual void deleteCellCentroids();

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

    /**
     * @brief deepCopy
     * @return
     */
    virtual IGeometry::Pointer deepCopy();

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
     * @brief setCellsContaingVert
     * @param cellsContaingVert
     */
    virtual void setCellsContaingVert(CellDynamicList::Pointer cellsContaingVert);

    /**
     * @brief setCellNeighbors
     * @param cellNeighbors
     */
    virtual void setCellNeighbors(CellDynamicList::Pointer cellNeighbors);

    /**
     * @brief setCellCentroids
     * @param cellCentroids
     */
    virtual void setCellCentroids(FloatArrayType::Pointer cellCentroids);

  private:

    QString m_Name;
    QString m_GeometryTypeName;
    unsigned int m_GeometryType;
    unsigned int m_XdmfGridType;
    unsigned int m_UnitDimensionality;
    unsigned int m_SpatialDimensionality;

    ImageGeom(const ImageGeom&); // Copy Constructor Not Implemented
    void operator=(const ImageGeom&); // Operator '=' Not Implemented
};


#endif /* _ImageGeom_H_ */

