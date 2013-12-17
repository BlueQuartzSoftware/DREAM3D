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
  setName(DREAM3D::Defaults::SurfaceDataContainerName);
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
  writeVerticesToHDF5(dcGid);
  writeEdgesToHDF5(dcGid);
  writeFacesToHDF5(dcGid);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainer::writeFacesToHDF5(hid_t dcGid)
{
  herr_t err = 0;

  FaceArray::Pointer facesPtr = getFaces();
  if (facesPtr.get() != NULL)
  {
    int32_t rank = 2; // THIS NEEDS TO BE THE SAME AS THE NUMBER OF ELEMENTS IN THE Structure from SurfaceMesh::DataStruc
    hsize_t dims[2] = {facesPtr->getNumberOfTuples(), 3};

    int32_t* data = reinterpret_cast<int32_t*>(facesPtr->getPointer(0));

    err = QH5Lite::writePointerDataset(dcGid, DREAM3D::StringConstants::FacesName, rank, dims, data);
    if (err < 0)
    {
//      setErrorCondition(err);
//      notifyErrorMessage(getHumanLabel(), "Error Writing Face List to DREAM3D file", getErrorCondition());
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

      err = QH5Lite::writePointerDataset(dcGid, DREAM3D::StringConstants::FaceNeighbors, rank, dims, bufPtr);
      if (err < 0)
      {
        notifyErrorMessage(getNameOfClass(), "Error writing the Face Neighbors", -998);
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

    err = QH5Lite::writePointerDataset(dcGid, DREAM3D::StringConstants::FacesContainingVert, rank, dims, bufPtr);
    if (err < 0)
    {
      notifyErrorMessage(getNameOfClass(), "Error writing the Faces Containing Verts", -999);
      return err;
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainer::writeXdmf(QTextStream& out, QString hdfFileName)
{
  herr_t err = 0;

  // Write the Mesh Structure to the XDMF file
  writeXdmfMeshStructureHeader(out, hdfFileName);
  // Get all of our AttributeMatrices
  AttributeMatrixMap_t amMap = getAttributeMatrices();
  // Loop over each AttributeMatrix and write the meta data to the Xdmf file
  for(QMap<QString, AttributeMatrix::Pointer>::iterator iter = amMap.begin(); iter != amMap.end(); ++iter)
  {
    AttributeMatrix::Pointer attrMat = iter.value();
    uint32_t amType = attrMat->getType();
    if(amType == DREAM3D::AttributeMatrixType::Face)
    {
      QString xdmfText = attrMat->generateXdmfText("Cell", getName(), hdfFileName);
      out << xdmfText;
    }
  }

  // Write the Grid Footer to the Xdmf file
  writeXdmfMeshStructureFooter(out);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainer::writeXdmfMeshStructureHeader(QTextStream& out, QString hdfFileName)
{
  FaceArray::Pointer faces = getFaces();
  if (NULL == faces.get())
  {
    return;
  }
  VertexArray::Pointer verts = getVertices();
  if(NULL == verts.get())
  {
    return;
  }

  out << "  <Grid Name=\"" << getName() << "\">" << "\n";
  out << "    <Topology TopologyType=\"Triangle\" NumberOfElements=\"" << faces->getNumberOfTuples() << "\">" << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << faces->getNumberOfTuples() << " 3\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << getName() << "/Faces" << "\n";
  out << "      </DataItem>" << "\n";
  out << "    </Topology>" << "\n";

  out << "    <Geometry Type=\"XYZ\">" << "\n";
  out << "      <DataItem Format=\"HDF\"  Dimensions=\"" << verts->getNumberOfTuples() << " 3\" NumberType=\"Float\" Precision=\"4\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << getName() << "/Vertices" << "\n";
  out << "      </DataItem>" << "\n";
  out << "    </Geometry>" << "\n";
  out << "" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainer::readMeshDataFromHDF5(hid_t dcGid, bool preflight)
{
  int err = 0;
  QVector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  if (true == preflight)
  {
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::StringConstants::FacesName, dims, type_class, type_size);
    if (err >= 0)
    {
      FaceArray::Pointer triangles = FaceArray::CreateArray(1, DREAM3D::FaceData::SurfaceMeshFaces, NULL);
      setFaces(triangles);
    }
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::StringConstants::FaceNeighbors, dims, type_class, type_size);
    if(err >= 0)
    {
      Int32DynamicListArray::Pointer faceNeighbors = Int32DynamicListArray::New();
      getFaces()->setFaceNeighbors(faceNeighbors);
    }
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::StringConstants::FacesContainingVert, dims, type_class, type_size);
    if(err >= 0)
    {
      Int32DynamicListArray::Pointer facesContainingVert = Int32DynamicListArray::New();
      getFaces()->setFacesContainingVert(facesContainingVert);
    }
  }
  else
  {
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::StringConstants::FacesName, dims, type_class, type_size);
    if (err >= 0)
    {
      // Allocate the Face_t structures
      FaceArray::Pointer facesPtr = FaceArray::CreateArray(dims[0], DREAM3D::FaceData::SurfaceMeshFaces, getVertices().get());
      // We need this to properly use QH5Lite because the data is stored as int32_t in 5 columns
      int32_t* data = reinterpret_cast<int32_t*>(facesPtr->getPointer(0));
      // Read the data from the file
      err = QH5Lite::readPointerDataset(dcGid, DREAM3D::StringConstants::FacesName, data);
      if (err < 0)
      {
//        setErrorCondition(err);
//        notifyErrorMessage(getHumanLabel(), "Error Reading Face List from DREAM3D file", getErrorCondition());
        return err;
      }
      setFaces(facesPtr);
      size_t nFaces = facesPtr->getNumberOfTuples();
      err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::StringConstants::FaceNeighbors, dims, type_class, type_size);
      if (err >= 0)
      {
        //Read the faceNeighbors array into the buffer

        std::vector<uint8_t> buffer;
        err = QH5Lite::readVectorDataset(dcGid, DREAM3D::StringConstants::FaceNeighbors, buffer);
        if(err < 0)
        {
//          setErrorCondition(err);
//          notifyErrorMessage(getHumanLabel(), "Error Reading Face List from DREAM3D file", getErrorCondition());
          return err;
        }
        Int32DynamicListArray::Pointer faceNeighbors = Int32DynamicListArray::New();
        faceNeighbors->deserializeLinks(buffer, nFaces);
        getFaces()->setFaceNeighbors(faceNeighbors);
      }
      err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::StringConstants::FacesContainingVert, dims, type_class, type_size);
      if (err >= 0)
      {
        //Read the faceNeighbors array into the buffer

        std::vector<uint8_t> buffer;
        err = QH5Lite::readVectorDataset(dcGid, DREAM3D::StringConstants::FacesContainingVert, buffer);
        if(err < 0)
        {
//          setErrorCondition(err);
//          notifyErrorMessage(getHumanLabel(), "Error Reading Face List from DREAM3D file", getErrorCondition());
          return err;
        }
        Int32DynamicListArray::Pointer facesContainingVert = Int32DynamicListArray::New();
        facesContainingVert->deserializeLinks(buffer, nFaces);
        getFaces()->setFacesContainingVert(facesContainingVert);
      }
    }
  }
  return 1;
}
