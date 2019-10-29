/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#pragma once

#include <memory>

#include <QtCore/QString>

#include "SIMPLib/DataArrays/StructArray.hpp"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/MeshStructs.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

#include "SurfaceMeshing/SurfaceMeshingDLLExport.h"

/**
 * @class M3CSliceBySlice M3CSliceBySlice.h DREAM3DLic/SurfaceMeshingFilters/M3CSliceBySlice.h
 * @brief This filter was contributed by Dr. Sukbin Lee of Carnegi-Mellon University and uses a "MultiMaterial Marching
 * Cubes" algorithm originally proposed by Wu & Sullivan. @n
 * This version of the code only considers 2 slices of the volume at any give instant
 * in time during the algorithm. The 2 slices are meshed and the resulting triangles
 * and nodes are written out to disk. At the conclusion of all slices the entire
 * generated triangle array and node array are read into memory. This version trades
 * off mush lower memory footprint during execution of the filter for some speed.
 * The increase in time to mesh a volume is due to the File I/O of the algorithm. File
 * writes are done in pure binary so to make them as quick as possible. An adaptive
 * memory allocation routine is also employeed to be able to scale the speed of the
 * algorithm from small voxel volumes to very large voxel volumes.
 *
 * Multiple material marching cubes algorithm, Ziji Wu1, John M. Sullivan Jr2, International Journal for Numerical Methods in Engineering
 * Special Issue: Trends in Unstructured Mesh Generation, Volume 58, Issue 2, pages 189
 * @author
 * @date
 * @version 1.0
 */
class SurfaceMeshing_EXPORT M3CSliceBySlice : public AbstractFilter
{
  Q_OBJECT
  //  PYB11_CREATE_BINDINGS(M3CSliceBySlice SUPERCLASS AbstractFilter)
  // PYB11_PROPERTY(DataArrayPath SurfaceDataContainerName READ getSurfaceDataContainerName WRITE setSurfaceDataContainerName)
  // PYB11_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)
  // PYB11_PROPERTY(QString FaceAttributeMatrixName READ getFaceAttributeMatrixName WRITE setFaceAttributeMatrixName)
  // PYB11_PROPERTY(QString FaceLabelsArrayName READ getFaceLabelsArrayName WRITE setFaceLabelsArrayName)
  // PYB11_PROPERTY(QString SurfaceMeshNodeTypesArrayName READ getSurfaceMeshNodeTypesArrayName WRITE setSurfaceMeshNodeTypesArrayName)
  // PYB11_PROPERTY(bool DeleteTempFiles READ getDeleteTempFiles WRITE setDeleteTempFiles)
  // PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
public:
  using Self = M3CSliceBySlice;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  
  /**
   * @brief Returns a NullPointer wrapped by a shared_ptr<>
   * @return
   */
  static Pointer NullPointer();

  /**
   * @brief Creates a new object wrapped in a shared_ptr<>
   * @return
   */
  static Pointer New();

  /**
   * @brief Returns the name of the class for M3CSliceBySlice
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for M3CSliceBySlice
   */
  static QString ClassName();

  ~M3CSliceBySlice() override;
  /**
   * @brief Setter property for SurfaceDataContainerName
   */
  void setSurfaceDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceDataContainerName
   * @return Value of SurfaceDataContainerName
   */
  DataArrayPath getSurfaceDataContainerName() const;

  Q_PROPERTY(DataArrayPath SurfaceDataContainerName READ getSurfaceDataContainerName WRITE setSurfaceDataContainerName)
  /**
   * @brief Setter property for VertexAttributeMatrixName
   */
  void setVertexAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for VertexAttributeMatrixName
   * @return Value of VertexAttributeMatrixName
   */
  QString getVertexAttributeMatrixName() const;

  Q_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)
  /**
   * @brief Setter property for FaceAttributeMatrixName
   */
  void setFaceAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for FaceAttributeMatrixName
   * @return Value of FaceAttributeMatrixName
   */
  QString getFaceAttributeMatrixName() const;

  Q_PROPERTY(QString FaceAttributeMatrixName READ getFaceAttributeMatrixName WRITE setFaceAttributeMatrixName)

  /**
   * @brief Setter property for FaceLabelsArrayName
   */
  void setFaceLabelsArrayName(const QString& value);
  /**
   * @brief Getter property for FaceLabelsArrayName
   * @return Value of FaceLabelsArrayName
   */
  QString getFaceLabelsArrayName() const;

  Q_PROPERTY(QString FaceLabelsArrayName READ getFaceLabelsArrayName WRITE setFaceLabelsArrayName)
  /**
   * @brief Setter property for SurfaceMeshNodeTypesArrayName
   */
  void setSurfaceMeshNodeTypesArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshNodeTypesArrayName
   * @return Value of SurfaceMeshNodeTypesArrayName
   */
  QString getSurfaceMeshNodeTypesArrayName() const;

  Q_PROPERTY(QString SurfaceMeshNodeTypesArrayName READ getSurfaceMeshNodeTypesArrayName WRITE setSurfaceMeshNodeTypesArrayName)

  /**
   * @brief Setter property for DeleteTempFiles
   */
  void setDeleteTempFiles(bool value);
  /**
   * @brief Getter property for DeleteTempFiles
   * @return Value of DeleteTempFiles
   */
  bool getDeleteTempFiles() const;

  Q_PROPERTY(bool DeleteTempFiles READ getDeleteTempFiles WRITE setDeleteTempFiles)

  void preflight() override;

  /**
   * @brief Setter property for FeatureIdsArrayPath
   */
  void setFeatureIdsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureIdsArrayPath
   * @return Value of FeatureIdsArrayPath
   */
  DataArrayPath getFeatureIdsArrayPath() const;

  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  QString getCompiledLibraryName() const override;
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;
  QString getGroupName() const override;
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;
  QString getHumanLabel() const override;

  void setupFilterParameters() override; /**
                                          *@brief This method will read the options from a file
                                          *@param reader The reader that is used to read the options from a file
                                          */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  void execute();

signals:
  void updateFilterParameters(AbstractFilter* filter);
  void parametersChanged();
  void preflightAboutToExecute();
  void preflightExecuted();

protected:
  M3CSliceBySlice();

  /**
   * @brief get_neighbor_list
   * @param NSP
   * @param NS
   * @param wrappedDims
   * @param neighborsPtr
   * @param neighborCSiteIdPtr
   */
  void get_neighbor_list(int NSP, int NS, int wrappedDims[], StructArray<SurfaceMesh::M3C::Neighbor>::Pointer neighborsPtr, DataArray<int32_t>::Pointer neighborCSiteIdPtr);

  /**
   * @brief initialize_nodes
   * @param NSP
   * @param zID
   * @param wrappedDims
   * @param res
   * @param cVertexPtr
   * @param voxelsPtr
   * @param cVertexNodeIdPtr
   * @param cVertexNodeTypePtr
   */
  void initialize_nodes(int NSP, int zID, int* wrappedDims, float* res, VertexArray::Pointer cVertexPtr, DataArray<int32_t>::Pointer voxelsPtr, DataArray<int32_t>::Pointer cVertexNodeIdPtr,
                        DataArray<int8_t>::Pointer cVertexNodeTypePtr);

  /**
   * @brief initialize_squares
   * @param zID
   * @param NSP
   * @param cSquarePtr
   * @param neighborsPtr
   */
  void initialize_squares(int zID, int NSP, StructArray<SurfaceMesh::M3C::Face>::Pointer cSquarePtr, StructArray<SurfaceMesh::M3C::Neighbor>::Pointer neighborsPtr);

  /**
   * @brief get_nodes_Edges
   * @param NSP
   * @param zID
   * @param wrappedDims
   * @param cSquarePtr
   * @param voxelsPtr
   * @param cEdgePtr
   * @param cVertexNodeTypePtr
   * @param neighborsPtr
   * @return
   */
  size_t get_nodes_Edges(int NSP, int zID, int* wrappedDims, StructArray<SurfaceMesh::M3C::Face>::Pointer cSquarePtr, DataArray<int32_t>::Pointer voxelsPtr,
                         StructArray<SurfaceMesh::M3C::Segment>::Pointer cEdgePtr, DataArray<int8_t>::Pointer cVertexNodeTypePtr, StructArray<SurfaceMesh::M3C::Neighbor>::Pointer neighborsPtr);
  /**
   * @brief get_triangles
   * @param NSP
   * @param wrappedDims
   * @param cSquarePtr
   * @param voxelsPtr
   * @param cVertexNodeTypePtr
   * @param cEdgePtr
   * @param cTrianglePtr
   * @return
   */
  int get_triangles(int NSP, int* wrappedDims, StructArray<SurfaceMesh::M3C::Face>::Pointer cSquarePtr, DataArray<int32_t>::Pointer voxelsPtr, DataArray<int8_t>::Pointer cVertexNodeTypePtr,
                    StructArray<SurfaceMesh::M3C::Segment>::Pointer cEdgePtr, StructArray<SurfaceMesh::M3C::Patch>::Pointer cTrianglePtr);

  /**
   * @brief arrange_featurenames
   * @param numT
   * @param zID
   * @param NSP
   * @param wrappedDims
   * @param cTrianglePtr
   * @param cVertexPtr
   * @param voxelsPtr
   * @param cVertexNodeIdPtr
   * @param neighborsPtr
   */
  void arrange_featurenames(int numT, int zID, int NSP, int* wrappedDims, float* res, StructArray<SurfaceMesh::M3C::Patch>::Pointer cTrianglePtr, VertexArray::Pointer cVertexPtr,
                            DataArray<int32_t>::Pointer voxelsPtr, StructArray<SurfaceMesh::M3C::Neighbor>::Pointer neighborsPtr);

  /**
   * @brief find_xcoord
   * @param xDim
   * @param xRes
   * @return
   */
  float find_xcoord(int index, int xDim, float xRes);

  /**
   * @brief find_ycoord
   * @param index
   * @param xDim
   * @param yDim
   * @param yRes
   * @return
   */
  float find_ycoord(int index, int xDim, int yDim, float yRes);

  /**
   * @brief find_zcoord
   * @param xDim
   * @param yDim
   * @param zRes
   * @return
   */
  float find_zcoord(int index, int xDim, int yDim, float zRes);

  /**
   * @brief treat_anomaly
   * @param tNSt
   * @param zID1
   * @param voxelsPtr
   * @param neighborsPtr
   * @return
   */
  int treat_anomaly(int tNSt[4], int zID1, DataArray<int32_t>::Pointer voxelsPtr, StructArray<SurfaceMesh::M3C::Neighbor>::Pointer neighborsPtr);

  /**
   * @brief get_nodes
   * @param cst
   * @param ord
   * @param nidx
   * @param nid
   * @param NSP
   * @param wrappedDims
   */
  void get_nodes(int cst, int ord, int nidx[2], int* nid, int NSP, int* wrappedDims);

  /**
   * @brief get_featurenames
   * @param cst
   * @param ord
   * @param pID
   * @param pfeaturename
   * @param NSP
   * @param wrappedDims
   * @param voxelsPtr
   */
  void get_featurenames(int cst, int ord, int pID[2], int* pfeaturename, int NSP, int* wrappedDims, DataArray<int32_t>::Pointer voxelsPtr);

  void get_case0_triangles(int site, int* ae, int nedge, int tin, int* tout, StructArray<SurfaceMesh::M3C::Segment>::Pointer cEdgePtr, StructArray<SurfaceMesh::M3C::Patch>::Pointer cTrianglePtr);
  void get_case2_triangles(int site, int* ae, int nedge, int* afc, int nfctr, int tin, int* tout, StructArray<SurfaceMesh::M3C::Segment>::Pointer cEdgePtr,
                           StructArray<SurfaceMesh::M3C::Patch>::Pointer cTrianglePtr);
  void get_caseM_triangles(int site, int* afe, int nfedge, int* afc, int nfctr, int tin, int* tout, int ccn, StructArray<SurfaceMesh::M3C::Segment>::Pointer cEdgePtr,
                           StructArray<SurfaceMesh::M3C::Patch>::Pointer cTrianglePtr);

  void get_case_triangles_helper(int nedge, int* burnt, int* burnt_list, int& loopID, int* ae, StructArray<SurfaceMesh::M3C::Segment>::Pointer cEdgePtr, QVector<int>& countVec);

  void get_case_triangles_helper_2(int* burnt_loop, int* burnt_list, int from, int to, int numN, int& ctid, StructArray<SurfaceMesh::M3C::Segment>::Pointer cEdgePtr,
                                   StructArray<SurfaceMesh::M3C::Patch>::Pointer cTrianglePtr);

  /**
   * @brief assign_nodeID
   * @param nN
   * @param NSP
   * @param cVertexNodeIdPtr
   * @param cVertexNodeTypePtr
   * @return
   */
  int assign_nodeID(int nN, int NSP, DataArray<int32_t>::Pointer cVertexNodeIdPtr, DataArray<int8_t>::Pointer cVertexNodeTypePtr);
  /**
   * @brief get_square_index
   * @param tns
   * @return
   */
  int get_square_index(int tns[4]);

  /**
   * @brief update_current_triangles
   * @param nT
   */
  void update_current_triangles(int nT);

  /**
   * @brief writeNodesFile
   * @param zID
   * @param cNodeID
   * @param NSP
   * @param nodesFile
   * @param cVertexPtr
   * @param cVertexNodeIdPtr
   * @param cVertexNodeTypePtr
   * @return
   */
  int writeNodesFile(int zID, int cNodeID, int NSP, const QString& nodesFile, VertexArray::Pointer cVertexPtr, DataArray<int32_t>::Pointer cVertexNodeIdPtr,
                     DataArray<int8_t>::Pointer cVertexNodeTypePtr);

  /**
   * @brief writeTrianglesFile
   * @param zID
   * @param ctid
   * @param trianglesFile
   * @param nt
   * @param cTrianglePtr
   * @param cVertexNodeIdPtr
   * @return
   */
  int writeTrianglesFile(int zID, int ctid, const QString& trianglesFile, int nt, StructArray<SurfaceMesh::M3C::Patch>::Pointer cTrianglePtr, DataArray<int32_t>::Pointer cVertexNodeIdPtr,
                         int32_t featureIdZeroMappingValue);

  /**
   * @brief volumeHasGhostLayer
   * @return
   */
  bool volumeHasGhostLayer();
  void copyBulkSliceIntoWorkingArray(int i, int* wrappedDims, size_t* dims, int32_t* voxels);
  void update_node_edge_kind(int nT, StructArray<SurfaceMesh::M3C::Patch>::Pointer cTrianglePtr, DataArray<int8_t>::Pointer cVertexNodeTypePtr,
                             StructArray<SurfaceMesh::M3C::Segment>::Pointer cEdgePtr);

  int32_t volumeHasFeatureValuesOfZero();
  void renumberVoxelFeatureIds(int32_t gid);

  // The next two methods are for analyzing the winding
  // void analyzeWinding();
  // QVector<int> findAdjacentTriangles(Triangle *triangle, int label);

private:
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;

  DataArrayPath m_SurfaceDataContainerName = {};
  QString m_VertexAttributeMatrixName = {};
  QString m_FaceAttributeMatrixName = {};
  QString m_FaceLabelsArrayName = {};
  QString m_SurfaceMeshNodeTypesArrayName = {};
  bool m_DeleteTempFiles = {};
  DataArrayPath m_FeatureIdsArrayPath = {};

  int numfeatures;

  float m_OriginX, m_OriginY, m_OriginZ;

  void dataCheck();

public:
  M3CSliceBySlice(const M3CSliceBySlice&) = delete; // Copy Constructor Not Implemented
  M3CSliceBySlice(M3CSliceBySlice&&) = delete;      // Move Constructor Not Implemented
  M3CSliceBySlice& operator=(const M3CSliceBySlice&) = delete; // Copy Assignment Not Implemented
  M3CSliceBySlice& operator=(M3CSliceBySlice&&) = delete;
};

