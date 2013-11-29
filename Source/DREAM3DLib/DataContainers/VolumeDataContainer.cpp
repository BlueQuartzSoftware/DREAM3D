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
#include "DREAM3DLib/HDF5/VTKH5Constants.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeDataContainer::VolumeDataContainer() :
  SurfaceDataContainer()
{
  m_Dimensions[0] = 0;
  m_Dimensions[1] = 0;
  m_Dimensions[2] = 0;
  m_Resolution[0] = 1.0f;
  m_Resolution[1] = 1.0f;
  m_Resolution[2] = 1.0f;
  m_Origin[0] = 0.0f;
  m_Origin[1] = 0.0f;
  m_Origin[2] = 0.0f;

  setName(DREAM3D::Defaults::VolumeDataContainerName);
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
  writeVerticesToHDF5(dcGid);
  writeEdgesToHDF5(dcGid);
  writeFacesToHDF5(dcGid);
  writeCellsToHDF5(dcGid);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainer::writeCellsToHDF5(hid_t dcGid)
{
  herr_t err = 0;

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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainer::writeXdmf(QTextStream* out, QString hdfFileName)
{
  herr_t err = 0;
  if (out == NULL)
  {
    return -1;
  }

  writeXdmfMeshStructure(*out, hdfFileName);
  for(QMap<QString, AttributeMatrix::Pointer>::iterator iter = getAttributeMatrices().begin(); iter != getAttributeMatrices().end(); ++iter)
  {
    if((*iter)->getType() == DREAM3D::AttributeMatrixType::Cell)
    {
      (*iter)->generateXdmfText("Cell", getName(), hdfFileName);
    }
  }
  writeXdmfGridFooter(*out);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::writeXdmfMeshStructure(QTextStream& out, QString hdfFileName)
{
  EdgeArray::Pointer edges = getEdges();
  if (NULL == edges.get())
  {
    return;
  }
  VertexArray::Pointer verts = getVertices();
  if(NULL == verts.get())
  {
    return;
  }

  out << "  <Grid Name=\"" << getName() << "\">" << "\n";
  out << "    <Topology TopologyType=\"Polyline\" NodesPerElement=\"2\" NumberOfElements=\"" << edges->getNumberOfTuples() << "\">" << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << edges->getNumberOfTuples() << " 2\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << getName() << "/Edges" << "\n";
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
void VolumeDataContainer::writeXdmfGridFooter(QTextStream& xdmf)
{
  xdmf << "  </Grid>" << "\n";
  xdmf << "    <!-- *************** END OF " << getName() << " *************** -->" << "\n";
  xdmf << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainer::readMeshDataFromHDF5(hid_t dcGid, bool preflight)
{
  int err = 0;
  QVector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  if (true == preflight)
  {
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::CellsName, dims, type_class, type_size);
    if (err >= 0)
    {
      CellArray::Pointer triangles = CellArray::CreateArray(1, DREAM3D::CellData::SurfaceMeshCells, NULL);
      setCells(triangles);
    }
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::CellNeighbors, dims, type_class, type_size);
    if(err >= 0)
    {
      Int32DynamicListArray::Pointer cellNeighbors = Int32DynamicListArray::New();
      getCells()->setCellNeighbors(cellNeighbors);
    }
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::CellsContainingVert, dims, type_class, type_size);
    if(err >= 0)
    {
      Int32DynamicListArray::Pointer cellsContainingVert = Int32DynamicListArray::New();
      getCells()->setCellsContainingVert(cellsContainingVert);
    }
  }
  else
  {
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::CellsName, dims, type_class, type_size);
    if (err >= 0)
    {
      // Allocate the Cell_t structures
      CellArray::Pointer cellsPtr = CellArray::CreateArray(dims[0], DREAM3D::CellData::SurfaceMeshCells, getVertices().get());
      // We need this to properly use QH5Lite because the data is stored as int32_t in 5 columns
      int32_t* data = reinterpret_cast<int32_t*>(cellsPtr->getPointer(0));
      // Read the data from the file
      err = QH5Lite::readPointerDataset(dcGid, DREAM3D::HDF5::CellsName, data);
      if (err < 0)
      {
//        setErrorCondition(err);
//        notifyErrorMessage("Error Reading Cell List from DREAM3D file", getErrorCondition());
        return err;
      }
      setCells(cellsPtr);
      size_t nCells = cellsPtr->getNumberOfTuples();
      err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::CellNeighbors, dims, type_class, type_size);
      if (err >= 0)
      {
        //Read the cellNeighbors array into the buffer

        std::vector<uint8_t> buffer;
        err = QH5Lite::readVectorDataset(dcGid, DREAM3D::HDF5::CellNeighbors, buffer);
        if(err < 0)
        {
//          setErrorCondition(err);
//          notifyErrorMessage("Error Reading Cell List from DREAM3D file", getErrorCondition());
          return err;
        }
        Int32DynamicListArray::Pointer cellNeighbors = Int32DynamicListArray::New();
        cellNeighbors->deserializeLinks(buffer, nCells);
        getCells()->setCellNeighbors(cellNeighbors);
      }
      err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::CellsContainingVert, dims, type_class, type_size);
      if (err >= 0)
      {
        //Read the cellNeighbors array into the buffer

        std::vector<uint8_t> buffer;
        err = QH5Lite::readVectorDataset(dcGid, DREAM3D::HDF5::CellsContainingVert, buffer);
        if(err < 0)
        {
//          setErrorCondition(err);
//          notifyErrorMessage("Error Reading Cell List from DREAM3D file", getErrorCondition());
          return err;
        }
        Int32DynamicListArray::Pointer cellsContainingVert = Int32DynamicListArray::New();
        cellsContainingVert->deserializeLinks(buffer, nCells);
        getCells()->setCellsContainingVert(cellsContainingVert);
      }
    }
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainer::gatherMetaData(hid_t dcGid, int64_t volDims[3], float spacing[3], float origin[3])
{
  int err = QH5Lite::readPointerDataset(dcGid, H5_DIMENSIONS, volDims);
  if(err < 0)
  {
//    PipelineMessage em (getHumanLabel(), "DataContainerReader Error Reading the Dimensions", err);
//    addErrorMessage(em);
//    setErrorCondition(-151);
    return -1;
  }

  err = QH5Lite::readPointerDataset(dcGid, H5_SPACING, spacing);
  if(err < 0)
  {
//    PipelineMessage em (getHumanLabel(), "DataContainerReader Error Reading the Spacing (Resolution)", err);
//    addErrorMessage(em);
//    setErrorCondition(-152);
    return -1;
  }

  err = QH5Lite::readPointerDataset(dcGid, H5_ORIGIN, origin);
  if(err < 0)
  {
//    PipelineMessage em (getHumanLabel(), "DataContainerReader Error Reading the Origin", err);
//    addErrorMessage(em);
//    setErrorCondition(-153);
    return -1;
  }
  return err;
}
