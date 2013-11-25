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

#ifndef SURFACEMESHDATACONTAINER_H_
#define SURFACEMESHDATACONTAINER_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif

//-- C++ includes
#include <vector>
#include <map>
#include <sstream>
#include <list>

//-- Boost includes
#include <boost/shared_array.hpp>

//-- DREAM3D Includes
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataContainers/DataContainerMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Common/Observable.h"
#include "DREAM3DLib/Common/SurfaceMeshStructs.h"
#include "DREAM3DLib/DataArrays/StructArray.hpp"
#include "DREAM3DLib/SurfaceMeshingFilters/MeshVertLinks.hpp"
#include "DREAM3DLib/SurfaceMeshingFilters/MeshFaceNeighbors.hpp"


/**
 * @class SurfaceMeshDataContainer SurfaceMeshDataContainer.h DREAM3DLib/DataContainers/SurfaceMeshDataContainer.h
 * @brief This data container holds data the represents a SurfaceMesh
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Sep 28, 2012
 * @version 1.0
 */
class DREAM3DLib_EXPORT SurfaceMeshDataContainer : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS (SurfaceMeshDataContainer)
    DREAM3D_STATIC_NEW_MACRO (SurfaceMeshDataContainer)
    DREAM3D_TYPE_MACRO_SUPER(SurfaceMeshDataContainer, Observable)

    virtual ~SurfaceMeshDataContainer();

    METHOD_DEF_TEMPLATE_INITIALIZEARRAYDATA (Vertex)
    METHOD_DEF_TEMPLATE_INITIALIZEARRAYDATA (Face)
    METHOD_DEF_TEMPLATE_INITIALIZEARRAYDATA (Edge)
    METHOD_DEF_TEMPLATE_INITIALIZEARRAYDATA (Field)
    METHOD_DEF_TEMPLATE_INITIALIZEARRAYDATA (Ensemble)

    METHOD_DEF_TEMPLATE_GETARRAYDATA (getVertexData)
    METHOD_DEF_TEMPLATE_GETARRAYDATA (getFaceData)
    METHOD_DEF_TEMPLATE_GETARRAYDATA (getEdgeData)
    METHOD_DEF_TEMPLATE_GETARRAYDATA (getFieldData)
    METHOD_DEF_TEMPLATE_GETARRAYDATA (getEnsembleData)

    DREAM3D_INSTANCE_PROPERTY(DREAM3D::SurfaceMesh::VertListPointer_t, Vertices)
    DREAM3D_INSTANCE_PROPERTY(DREAM3D::SurfaceMesh::FaceListPointer_t, Faces)


    DOES_DATASET_EXIST_DECL(VertexData)
    DOES_DATASET_EXIST_DECL(FaceData)
    DOES_DATASET_EXIST_DECL(EdgeData)
    DOES_DATASET_EXIST_DECL(FieldData)
    DOES_DATASET_EXIST_DECL(EnsembleData)

    /**
     * @brief buildMeshVertLinks Creates the list of Faces for each vertex that the vertex is a part of
     */
    void buildMeshVertLinks();

    /**
     * @brief removeMeshVertLinks Removes the VertLinks data structures to reclaim memory
     */
    void removeMeshVertLinks();

    /**
     * @brief getMeshVertLinks Returns the vert Links object
     * @return
     */
    MeshVertLinks::Pointer getMeshVertLinks();

    /**
     * @brief setMeshVertLinks
     * @param vertLinks
     */
    void setMeshVertLinks(MeshVertLinks::Pointer vertLinks);

    /**
     * @brief buildMeshFaceNeighborLists Creates the list of Faces that share a common edge with a Face. Since
     * we create non-manifold meshes we can have more than 3 neighbors.
     */
    void buildMeshFaceNeighborLists();

    /**
     * @brief removeMeshFaceNeighborLists Remove the Face neighbor lists to reclaim memory.
     */
    void removeMeshFaceNeighborLists();

    /**
     * @brief getMeshFaceNeighborLists Returns the Face Neighbor lists object
     * @return
     */
    MeshFaceNeighbors::Pointer getMeshFaceNeighborLists();

    /**
     * @brief setMeshFaceNeighborLists
     * @param neighbors
     */
    void setMeshFaceNeighborLists(MeshFaceNeighbors::Pointer neighbors);


    /**
     * @brief Adds/overwrites the data for a named array
     * @param name The name that the array will be known by
     * @param data The IDataArray::Pointer that will hold the data
     */
    void addVertexData(const std::string &name, IDataArray::Pointer data);

    /**
     * @brief Returns the array for a given named array or the equivelant to a
     * null pointer if the name does not exist.
     * @param name The name of the data array
     */
    IDataArray::Pointer getVertexData(const std::string &name);

    /**
     * @brief Removes the named data array from the Data Container and returns it to the calling
     * method.
     * @param name The name of the array
     * @return
     */
    IDataArray::Pointer removeVertexData(const std::string &name);

    /**
     * @brief Removes all the Point Arrays
     */
    void clearVertexData();

    /**
     * @brief Returns a list that contains the names of all the arrays currently stored in the
     * Point group
     * @return
     */
    std::list<std::string> getPointArrayNameList();

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
    void addFaceData(const std::string &name, IDataArray::Pointer data);

    /**
     * @brief Returns the array for a given named array or the equivelant to a
     * null pointer if the name does not exist.
     * @param name The name of the data array
     */
    IDataArray::Pointer getFaceData(const std::string &name);

    /**
     * @brief Removes the named data array from the Data Container and returns it to the calling
     * method
     * @param name The name of the array
     * @return
     */
    IDataArray::Pointer removeFaceData(const std::string &name);

    /**
     * @brief Removes all the Face Arrays
     */
    void clearFaceData();

    /**
     * @brief Returns a list that contains the names of all the arrays currently stored in the
     * Face (Formerly Grain) group
     * @return
     */
    std::list<std::string> getFaceArrayNameList();

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
    void addEdgeData(const std::string &name, IDataArray::Pointer data);

    /**
     * @brief Returns the array for a given named array or the equivelant to a
     * null pointer if the name does not exist.
     * @param name The name of the data array
     */
    IDataArray::Pointer getEdgeData(const std::string &name);

    /**
     * @brief Removes the named data array from the Data Container and returns it to the calling
     * method.
     * @param name The name of the array
     * @return
     */
    IDataArray::Pointer removeEdgeData(const std::string &name);

    /**
     * @brief Removes all the ensemble data
     */
    void clearEdgeData();

    std::list<std::string> getEdgeArrayNameList();

    int getNumEdgeArrays();

    DREAM3D_INSTANCE_PROPERTY(size_t, NumEdgeTuples)

    /**
     * @brief Adds/overwrites the data for a named array
     * @param name The name that the array will be known by
     * @param data The IDataArray::Pointer that will hold the data
     */
    void addFieldData(const std::string &name, IDataArray::Pointer data);

    /**
     * @brief Returns the array for a given named array or the equivelant to a
     * null pointer if the name does not exist.
     * @param name The name of the data array
     */
    IDataArray::Pointer getFieldData(const std::string &name);

    /**
     * @brief Removes the named data array from the Data Container and returns it to the calling
     * method
     * @param name The name of the array
     * @return
     */
    IDataArray::Pointer removeFieldData(const std::string &name);

    /**
     * @brief Removes all the Field Arrays
     */
    void clearFieldData();

    /**
     * @brief Returns a list that contains the names of all the arrays currently stored in the
     * Field (Formerly Grain) group
     * @return
     */
    std::list<std::string> getFieldArrayNameList();

    /**
     * @brief Returns the total number of arrays that are stored in the Field group
     * @return
     */
    int getNumFieldArrays();

    /**
     * @brief Returns the number of Tuples that the field data has. For example if there are 32 grains
     * in during a set of filtering operations then the a value of '32' would be returned.
     * @return
     */
    DREAM3D_INSTANCE_PROPERTY(size_t, NumFieldTuples)

    /**
     * @brief Resizes all of the Field Arrays to have 'size' tuples
     * @param size The number of tuples that each DataArray should contain.
     */
    void resizeFieldDataArrays(size_t size);

    /**
     * @brief Adds/overwrites the data for a named array
     * @param name The name that the array will be known by
     * @param data The IDataArray::Pointer that will hold the data
     */
    void addEnsembleData(const std::string &name, IDataArray::Pointer data);

    /**
     * @brief Returns the array for a given named array or the equivelant to a
     * null pointer if the name does not exist.
     * @param name The name of the data array
     */
    IDataArray::Pointer getEnsembleData(const std::string &name);

    /**
     * @brief Removes the named data array from the Data Container and returns it to the calling
     * method
     * @param name The name of the array
     * @return
     */
    IDataArray::Pointer removeEnsembleData(const std::string &name);

    /**
     * @brief Removes all the Ensemble Arrays
     */
    void clearEnsembleData();

    /**
     * @brief Returns a list that contains the names of all the arrays currently stored in the
     * Ensemble (Formerly Grain) group
     * @return
     */
    std::list<std::string> getEnsembleArrayNameList();

    /**
     * @brief Returns the total number of arrays that are stored in the Ensemble group
     * @return
     */
    int getNumEnsembleArrays();

    /**
     * @brief Returns the number of Tuples that the field data has. For example if there are 32 grains
     * in during a set of filtering operations then the a value of '32' would be returned.
     * @return
     */
    DREAM3D_INSTANCE_PROPERTY(size_t, NumEnsembleTuples)

    /**
     * @brief Resizes all of the Ensemble Arrays to have 'size' tuples
     * @param size The number of tuples that each DataArray should contain.
     */
    void resizeEnsembleDataArrays(size_t size);



  protected:
     SurfaceMeshDataContainer();

   private:

     std::map<std::string, IDataArray::Pointer> m_VertexData;
     std::map<std::string, IDataArray::Pointer> m_FaceData;
     std::map<std::string, IDataArray::Pointer> m_EdgeData;
     std::map<std::string, IDataArray::Pointer> m_FieldData;
     std::map<std::string, IDataArray::Pointer> m_EnsembleData;

     MeshVertLinks::Pointer m_MeshVertLinks;
     MeshFaceNeighbors::Pointer m_FaceNeighbors;

     SurfaceMeshDataContainer(const SurfaceMeshDataContainer&);
     void operator =(const SurfaceMeshDataContainer&);

};

#endif /* SURFACEMESHDATACONTAINER_H_ */
