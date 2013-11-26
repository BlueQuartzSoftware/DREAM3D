/* ============================================================================
 * Copyright (c) 2009, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2009, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"

// C Includes

// C++ Includes
#include <iostream>
#include <fstream>

// EbsdLib Includes
#include "EbsdLib/EbsdConstants.h"

// DREAM3D Includes
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeDataContainer::VolumeDataContainer() :
SurfaceDataContainer()
{
  m_Dimensions[0] = 0; m_Dimensions[1] = 0; m_Dimensions[2] = 0;
  m_Resolution[0] = 1.0f; m_Resolution[1] = 1.0f; m_Resolution[2] = 1.0f;
  m_Origin[0] = 0.0f; m_Origin[1] = 0.0f; m_Origin[2] = 0.0f;

  setName(DREAM3D::HDF5::VolumeDataContainerName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeDataContainer::~VolumeDataContainer()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainer::writeMeshToHDF5(hid_t dcGid)
{
  herr_t err = 0;
  //first write the faces if they exist
  VertexArray::Pointer verticesPtr = getVertices();
  if (verticesPtr.get() != NULL)
  {
    int32_t rank = 2; // THIS NEEDS TO BE THE SAME AS THE NUMBER OF ELEMENTS IN THE Structure from SurfaceMesh::DataStruc
    hsize_t dims[2] = {verticesPtr->getNumberOfTuples(), 3};

    float* data = reinterpret_cast<float*>(verticesPtr->getPointer(0));

    err = QH5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::VerticesName, rank, dims, data);
    if (err < 0)
    {
//      setErrorCondition(err);
//      notifyErrorMessage("Error Writing Face List to DREAM3D file", getErrorCondition());
    }
  }
  //next write the edges if they exist
  EdgeArray::Pointer edgesPtr = getEdges();
  if (edgesPtr.get() != NULL)
  {
    int32_t rank = 2; // THIS NEEDS TO BE THE SAME AS THE NUMBER OF ELEMENTS IN THE Structure from SuredgeMesh::DataStruc
    hsize_t dims[2] = {edgesPtr->getNumberOfTuples(), 2};

    int32_t* data = reinterpret_cast<int32_t*>(edgesPtr->getPointer(0));

    err = QH5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::EdgesName, rank, dims, data);
    if (err < 0)
    {
//      setErrorCondition(err);
//      notifyErrorMessage("Error Writing Edge List to DREAM3D file", getErrorCondition());
    }

    //next write edge neighbors if they exist
    Int32DynamicListArray::Pointer edgeNeighbors = edgesPtr->getEdgeNeighbors();
    if (NULL != edgeNeighbors.get())
    {
      size_t total = 0;
      size_t nEdges = edgesPtr->getNumberOfTuples();
      for(size_t v = 0; v < nEdges; ++v)
      {
        total += edgeNeighbors->getNumberOfElements(v);
      }

      size_t totalBytes = nEdges * sizeof(uint16_t) + total * sizeof(int32_t);

      // Allocate a flat array to copy the data into
      QVector<uint8_t> buffer(totalBytes, 0);
      uint8_t* bufPtr = &(buffer.front());
      size_t offset = 0;

      for(size_t v = 0; v < nEdges; ++v)
      {
        uint16_t ncells = edgeNeighbors->getNumberOfElements(v);
        int32_t* cells = edgeNeighbors->getElementListPointer(v);
        ::memcpy(bufPtr + offset, &ncells, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        ::memcpy(bufPtr + offset, cells, ncells * sizeof(int32_t) );
        offset += ncells * sizeof(int32_t);
      }

      rank = 1;
      dims[0] = totalBytes;

      err = QH5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::EdgeNeighbors, rank, dims, bufPtr);
      if (err < 0)
      {
        notifyErrorMessage("Error writing the Edge Neighbors", -998);
        return err;
      }
    }

    //last write edges containing verts if they exist
    Int32DynamicListArray::Pointer edgesContainingVert = edgesPtr->getEdgesContainingVert();
    if (NULL == edgesContainingVert.get())
    {
      return 0;
    }
    VertexArray::Pointer verticesPtr = getVertices();
    if (NULL == verticesPtr.get())
    {
      return -1;
    }

    err = -1;
    size_t total = 0;
    size_t nVerts = verticesPtr->getNumberOfTuples();
    for(size_t v = 0; v < nVerts; ++v)
    {
      total += edgesContainingVert->getNumberOfElements(v);
    }

    size_t totalBytes = nVerts * sizeof(uint16_t) + total * sizeof(int32_t);

    // Allocate a flat array to copy the data into
    QVector<uint8_t> buffer(totalBytes, 0);
    uint8_t* bufPtr = &(buffer.front());
    size_t offset = 0;

    for(size_t v = 0; v < nVerts; ++v)
    {
      uint16_t ncells = edgesContainingVert->getNumberOfElements(v);
      int32_t* cells = edgesContainingVert->getElementListPointer(v);
      ::memcpy(bufPtr + offset, &ncells, sizeof(uint16_t));
      offset += sizeof(uint16_t);
      ::memcpy(bufPtr + offset, cells, ncells * sizeof(int32_t) );
      offset += ncells * sizeof(int32_t);
    }

    rank = 1;
    dims[0] = totalBytes;

    err = QH5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::EdgesContainingVert, rank, dims, bufPtr);
    if (err < 0)
    {
      notifyErrorMessage("Error writing the Edges Containing Verts", -999);
      return err;
    }
  }
  //next write the faces if they exist
  FaceArray::Pointer facesPtr = getFaces();
  if (facesPtr.get() != NULL)
  {
    int32_t rank = 2; // THIS NEEDS TO BE THE SAME AS THE NUMBER OF ELEMENTS IN THE Structure from SurfaceMesh::DataStruc
    hsize_t dims[2] = {facesPtr->getNumberOfTuples(), 3};

    int32_t* data = reinterpret_cast<int32_t*>(facesPtr->getPointer(0));

    err = QH5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::FacesName, rank, dims, data);
    if (err < 0)
    {
//      setErrorCondition(err);
//      notifyErrorMessage("Error Writing Face List to DREAM3D file", getErrorCondition());
    }

    //next write face neighbors if they exist
    Int32DynamicListArray::Pointer faceNeighbors = facesPtr->getFaceNeighbors();
    if (NULL != faceNeighbors.get())
    {
      size_t total = 0;
      size_t nFaces = facesPtr->getNumberOfTuples();
      for(size_t v = 0; v < nFaces; ++v)
      {
        total += faceNeighbors->getNumberOfElements(v);
      }

      size_t totalBytes = nFaces * sizeof(uint16_t) + total * sizeof(int32_t);

      // Allocate a flat array to copy the data into
      QVector<uint8_t> buffer(totalBytes, 0);
      uint8_t* bufPtr = &(buffer.front());
      size_t offset = 0;

      for(size_t v = 0; v < nFaces; ++v)
      {
        uint16_t ncells = faceNeighbors->getNumberOfElements(v);
        int32_t* cells = faceNeighbors->getElementListPointer(v);
        ::memcpy(bufPtr + offset, &ncells, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        ::memcpy(bufPtr + offset, cells, ncells * sizeof(int32_t) );
        offset += ncells * sizeof(int32_t);
      }

      rank = 1;
      dims[0] = totalBytes;

      err = QH5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::FaceNeighbors, rank, dims, bufPtr);
      if (err < 0)
      {
        notifyErrorMessage("Error writing the Face Neighbors", -998);
        return err;
      }
    }

    //last write faces containing verts if they exist
    Int32DynamicListArray::Pointer facesContainingVert = facesPtr->getFacesContainingVert();
    if (NULL == facesContainingVert.get())
    {
      return 0;
    }
    VertexArray::Pointer verticesPtr = getVertices();
    if (NULL == verticesPtr.get())
    {
      return -1;
    }

    err = -1;
    size_t total = 0;
    size_t nVerts = verticesPtr->getNumberOfTuples();
    for(size_t v = 0; v < nVerts; ++v)
    {
      total += facesContainingVert->getNumberOfElements(v);
    }

    size_t totalBytes = nVerts * sizeof(uint16_t) + total * sizeof(int32_t);

    // Allocate a flat array to copy the data into
    QVector<uint8_t> buffer(totalBytes, 0);
    uint8_t* bufPtr = &(buffer.front());
    size_t offset = 0;

    for(size_t v = 0; v < nVerts; ++v)
    {
      uint16_t ncells = facesContainingVert->getNumberOfElements(v);
      int32_t* cells = facesContainingVert->getElementListPointer(v);
      ::memcpy(bufPtr + offset, &ncells, sizeof(uint16_t));
      offset += sizeof(uint16_t);
      ::memcpy(bufPtr + offset, cells, ncells * sizeof(int32_t) );
      offset += ncells * sizeof(int32_t);
    }

    rank = 1;
    dims[0] = totalBytes;

    err = QH5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::FacesContainingVert, rank, dims, bufPtr);
    if (err < 0)
    {
      notifyErrorMessage("Error writing the Faces Containing Verts", -999);
      return err;
    }
  }
  //last write the cells if they exist
  CellArray::Pointer cellsPtr = getCells();
  if (cellsPtr.get() != NULL)
  {
    int32_t rank = 2; // THIS NEEDS TO BE THE SAME AS THE NUMBER OF ELEMENTS IN THE Structure from SurcellMesh::DataStruc
    hsize_t dims[2] = {cellsPtr->getNumberOfTuples(), 3};

    int32_t* data = reinterpret_cast<int32_t*>(cellsPtr->getPointer(0));

    err = QH5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::CellsName, rank, dims, data);
    if (err < 0)
    {
//      setErrorCondition(err);
//      notifyErrorMessage("Error Writing Cell List to DREAM3D file", getErrorCondition());
    }

    //next write cell neighbors if they exist
    Int32DynamicListArray::Pointer cellNeighbors = cellsPtr->getCellNeighbors();
    if (NULL != cellNeighbors.get())
    {
      size_t total = 0;
      size_t nCells = cellsPtr->getNumberOfTuples();
      for(size_t v = 0; v < nCells; ++v)
      {
        total += cellNeighbors->getNumberOfElements(v);
      }

      size_t totalBytes = nCells * sizeof(uint16_t) + total * sizeof(int32_t);

      // Allocate a flat array to copy the data into
      QVector<uint8_t> buffer(totalBytes, 0);
      uint8_t* bufPtr = &(buffer.front());
      size_t offset = 0;

      for(size_t v = 0; v < nCells; ++v)
      {
        uint16_t ncells = cellNeighbors->getNumberOfElements(v);
        int32_t* cells = cellNeighbors->getElementListPointer(v);
        ::memcpy(bufPtr + offset, &ncells, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        ::memcpy(bufPtr + offset, cells, ncells * sizeof(int32_t) );
        offset += ncells * sizeof(int32_t);
      }

      rank = 1;
      dims[0] = totalBytes;

      err = QH5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::CellNeighbors, rank, dims, bufPtr);
      if (err < 0)
      {
        notifyErrorMessage("Error writing the Cell Neighbors", -998);
        return err;
      }
    }

    //last write cells containing verts if they exist
    Int32DynamicListArray::Pointer cellsContainingVert = cellsPtr->getCellsContainingVert();
    if (NULL == cellsContainingVert.get())
    {
      return 0;
    }
    VertexArray::Pointer verticesPtr = getVertices();
    if (NULL == verticesPtr.get())
    {
      return -1;
    }

    err = -1;
    size_t total = 0;
    size_t nVerts = verticesPtr->getNumberOfTuples();
    for(size_t v = 0; v < nVerts; ++v)
    {
      total += cellsContainingVert->getNumberOfElements(v);
    }

    size_t totalBytes = nVerts * sizeof(uint16_t) + total * sizeof(int32_t);

    // Allocate a flat array to copy the data into
    QVector<uint8_t> buffer(totalBytes, 0);
    uint8_t* bufPtr = &(buffer.front());
    size_t offset = 0;

    for(size_t v = 0; v < nVerts; ++v)
    {
      uint16_t ncells = cellsContainingVert->getNumberOfElements(v);
      int32_t* cells = cellsContainingVert->getElementListPointer(v);
      ::memcpy(bufPtr + offset, &ncells, sizeof(uint16_t));
      offset += sizeof(uint16_t);
      ::memcpy(bufPtr + offset, cells, ncells * sizeof(int32_t) );
      offset += ncells * sizeof(int32_t);
    }

    rank = 1;
    dims[0] = totalBytes;

    err = QH5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::CellsContainingVert, rank, dims, bufPtr);
    if (err < 0)
    {
      notifyErrorMessage("Error writing the Cells Containing Verts", -999);
      return err;
    }
  }
  return err;
}
