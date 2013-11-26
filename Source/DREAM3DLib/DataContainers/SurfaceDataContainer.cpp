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

#include "SurfaceDataContainer.h"

// C Includes

// C++ Includes
#include <iostream>
#include <fstream>


// DREAM3D Includes
#include "DREAM3DLib/Math/DREAM3DMath.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceDataContainer::SurfaceDataContainer() :
EdgeDataContainer()
{
  setName(DREAM3D::HDF5::SurfaceDataContainerName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceDataContainer::~SurfaceDataContainer()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainer::writeMeshToHDF5(hid_t dcGid)
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
  return err;
}
