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

#ifndef _SOLIDMESHDATACONTAINER_H_
#define _SOLIDMESHDATACONTAINER_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif

//-- C++ includes
#include <vector>
#include <QMap>
#include <sstream>
#include <QtCore/QList>

//-- DREAM3D Includes
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/DataContainerMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/Common/Observable.h"
#include "DREAM3DLib/Common/SurfaceMeshStructs.h"
#include "DREAM3DLib/Common/SolidMeshStructs.h"
#include "DREAM3DLib/Common/StructArray.hpp"
#include "DREAM3DLib/SurfaceMeshingFilters/MeshVertLinks.hpp"
#include "DREAM3DLib/SurfaceMeshingFilters/MeshFaceNeighbors.hpp"

/**
 * @class SolidMeshDataContainer SolidMeshDataContainer.h DREAM3DLib/Common/SolidMeshDataContainer.h
 * @brief This data container holds data the represents a SurfaceMesh
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Sep 28, 2012
 * @version 1.0
 */
class DREAM3DLib_EXPORT SolidMeshDataContainer : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS (SolidMeshDataContainer)
    DREAM3D_STATIC_NEW_MACRO (SolidMeshDataContainer)
    DREAM3D_TYPE_MACRO_SUPER(SolidMeshDataContainer, Observable)

    virtual ~SolidMeshDataContainer();

    METHOD_DEF_TEMPLATE_INITIALIZEARRAYDATA (Vertex)
    METHOD_DEF_TEMPLATE_INITIALIZEARRAYDATA (Face)
    METHOD_DEF_TEMPLATE_INITIALIZEARRAYDATA (Edge)

    METHOD_DEF_TEMPLATE_GETARRAYDATA (getVertexData)
    METHOD_DEF_TEMPLATE_GETARRAYDATA (getFaceData)
    METHOD_DEF_TEMPLATE_GETARRAYDATA (getEdgeData)

    DREAM3D_INSTANCE_PROPERTY(DREAM3D::SurfaceMesh::VertListPointer_t, Vertices)
    DREAM3D_INSTANCE_PROPERTY(StructArray<Tetrahedron>::Pointer, Tetrahedrons)


    DOES_DATASET_EXIST_DECL(VertexData)
    DOES_DATASET_EXIST_DECL(FaceData)
    DOES_DATASET_EXIST_DECL(EdgeData)


  /**
     * @brief Adds/overwrites the data for a named array
     * @param name The name that the array will be known by
     * @param data The IDataArray::Pointer that will hold the data
     */
    void addVertexData(const QString &name, IDataArray::Pointer data);

    /**
     * @brief Returns the array for a given named array or the equivelant to a
     * null pointer if the name does not exist.
     * @param name The name of the data array
     */
    IDataArray::Pointer getVertexData(const QString &name);

    /**
     * @brief Removes the named data array from the Data Container and returns it to the calling
     * method.
     * @param name The name of the array
     * @return
     */
    IDataArray::Pointer removeVertexData(const QString &name);

    /**
     * @brief Removes all the Point Arrays
     */
    void clearVertexData();

    /**
     * @brief Returns a list that contains the names of all the arrays currently stored in the
     * Point group
     * @return
     */
    QList<QString> getPointArrayNameList();

    /**
     * @brief Returns the total number of arrays that are stored in the Point group
     * @return
     */
    int getNumPointArrays();

    /**
     * @brief Returns the number of Tuples that the field data has. For example if there are 32 grains
     * in during a set of filtering operations then the a value of '32' would be returned.
     * @return
     */
    DREAM3D_INSTANCE_PROPERTY(size_t, NumVertexTuples)

    /**
     * @brief Adds/overwrites the data for a named array
     * @param name The name that the array will be known by
     * @param data The IDataArray::Pointer that will hold the data
     */
    void addFaceData(const QString &name, IDataArray::Pointer data);

    /**
     * @brief Returns the array for a given named array or the equivelant to a
     * null pointer if the name does not exist.
     * @param name The name of the data array
     */
    IDataArray::Pointer getFaceData(const QString &name);

    /**
     * @brief Removes the named data array from the Data Container and returns it to the calling
     * method
     * @param name The name of the array
     * @return
     */
    IDataArray::Pointer removeFaceData(const QString &name);

    /**
     * @brief Removes all the Face Arrays
     */
    void clearFaceData();

    /**
     * @brief Returns a list that contains the names of all the arrays currently stored in the
     * Face (Formerly Grain) group
     * @return
     */
    QList<QString> getFaceArrayNameList();

    /**
     * @brief Returns the total number of arrays that are stored in the Face group
     * @return
     */
    int getNumFaceArrays();

    /**
     * @brief Returns the number of Tuples that the field data has. For example if there are 32 grains
     * in during a set of filtering operations then the a value of '32' would be returned.
     * @return
     */
    DREAM3D_INSTANCE_PROPERTY(size_t, NumFaceTuples)

    /**
     * @brief Resizes all of the Face Arrays to have 'size' tuples
     * @param size The number of tuples that each DataArray should contain.
     */
    void resizeFaceDataArrays(size_t size);

    /**
     * @brief Adds/overwrites the data for a named array
     * @param name The name that the array will be known by
     * @param data The IDataArray::Pointer that will hold the data
     */
    void addEdgeData(const QString &name, IDataArray::Pointer data);

    /**
     * @brief Returns the array for a given named array or the equivelant to a
     * null pointer if the name does not exist.
     * @param name The name of the data array
     */
    IDataArray::Pointer getEdgeData(const QString &name);

    /**
     * @brief Removes the named data array from the Data Container and returns it to the calling
     * method.
     * @param name The name of the array
     * @return
     */
    IDataArray::Pointer removeEdgeData(const QString &name);

    /**
     * @brief Removes all the ensemble data
     */
    void clearEdgeData();

    QList<QString> getEdgeArrayNameList();

    int getNumEdgeArrays();

    DREAM3D_INSTANCE_PROPERTY(size_t, NumEdgeTuples)

  protected:
     SolidMeshDataContainer();

   private:

     QMap<QString, IDataArray::Pointer> m_VertexData;
     QMap<QString, IDataArray::Pointer> m_FaceData;
     QMap<QString, IDataArray::Pointer> m_EdgeData;

     MeshVertLinks::Pointer m_MeshVertLinks;
     MeshFaceNeighbors::Pointer m_FaceNeighbors;

     SolidMeshDataContainer(const SolidMeshDataContainer&);
     void operator =(const SolidMeshDataContainer&);

};

#endif /* _SOLIDMESHDATACONTAINER_H_ */
