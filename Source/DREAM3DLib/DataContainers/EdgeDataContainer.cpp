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

#include "DREAM3DLib/DataContainers/EdgeDataContainer.h"

// C Includes

// C++ Includes
#include <iostream>
#include <fstream>


// DREAM3D Includes
#include "DREAM3DLib/Math/DREAM3DMath.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeDataContainer::EdgeDataContainer() :
  VertexDataContainer()
{
  setName(DREAM3D::Defaults::EdgeDataContainerName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeDataContainer::~EdgeDataContainer()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainer::writeMeshToHDF5(hid_t dcGid, bool writeXdmf)
{
  herr_t err = 0;
  writeVerticesToHDF5(dcGid, writeXdmf);
  writeEdgesToHDF5(dcGid);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainer::writeEdgesToHDF5(hid_t dcGid)
{
  herr_t err = 0;

  EdgeArray::Pointer edgesPtr = getEdges();
  if (edgesPtr.get() != NULL)
  {
    int32_t rank = 2; // THIS NEEDS TO BE THE SAME AS THE NUMBER OF ELEMENTS IN THE Structure from SuredgeMesh::DataStruc
    hsize_t dims[2] = {edgesPtr->getNumberOfTuples(), 2};

    int32_t* data = reinterpret_cast<int32_t*>(edgesPtr->getPointer(0));

    err = QH5Lite::writePointerDataset(dcGid, DREAM3D::StringConstants::EdgesName, rank, dims, data);
    if (err < 0)
    {
//      setErrorCondition(err);
//      notifyErrorMessage(getHumanLabel(), "Error Writing Edge List to DREAM3D file", getErrorCondition());
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

      err = QH5Lite::writePointerDataset(dcGid, DREAM3D::StringConstants::EdgeNeighbors, rank, dims, bufPtr);
      if (err < 0)
      {
        notifyErrorMessage(getNameOfClass(), "Error writing the Edge Neighbors", -998);
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

    err = QH5Lite::writePointerDataset(dcGid, DREAM3D::StringConstants::EdgesContainingVert, rank, dims, bufPtr);
    if (err < 0)
    {
      notifyErrorMessage(getNameOfClass(), "Error writing the Edges Containing Verts", -999);
      return err;
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer EdgeDataContainer::deepCopy()
{
  EdgeDataContainer::Pointer dcCopy = EdgeDataContainer::New(getName());

  dcCopy->setEdges(getEdges());
  dcCopy->setMeshLinks(getMeshLinks());
  dcCopy->setName(getName());
  dcCopy->setVertices(getVertices());

  for(QMap<QString, AttributeMatrix::Pointer>::iterator iter = getAttributeMatrices().begin(); iter != getAttributeMatrices().end(); ++iter)
  {
    AttributeMatrix::Pointer attrMat = (*iter)->deepCopy();
    dcCopy->addAttributeMatrix(attrMat->getName(), attrMat);
  }

  return dcCopy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainer::writeXdmf(QTextStream& out, QString hdfFileName)
{
  herr_t err = 0;
  uint8_t gridType = DREAM3D::XdmfGridType::PolyData;

  // Write the Mesh Structure to the XDMF file
  writeXdmfMeshStructureHeader(out, hdfFileName);
  // Get all of our AttributeMatrices
  AttributeMatrixMap_t amMap = getAttributeMatrices();
  // Loop over each AttributeMatrix and write the meta data to the Xdmf file
  QString xdmfCenter = "";
  for(QMap<QString, AttributeMatrix::Pointer>::iterator iter = amMap.begin(); iter != amMap.end(); ++iter)
  {
    xdmfCenter = "";
    AttributeMatrix::Pointer attrMat = iter.value();
    uint32_t amType = attrMat->getType();
    switch(amType)
    {
    //FIXME: There are more AttributeMatrix Types that should be implemented
      case DREAM3D::AttributeMatrixType::Vertex:
        xdmfCenter = DREAM3D::XdmfCenterType::Node; break;
      case DREAM3D::AttributeMatrixType::Edge:
        xdmfCenter = DREAM3D::XdmfCenterType::Cell; break;
      default:
        break;
    }
    if(xdmfCenter.isEmpty() == false)
    {
      QString xdmfText = attrMat->generateXdmfText(xdmfCenter, getName(), hdfFileName, gridType);
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
void EdgeDataContainer::writeXdmfMeshStructureHeader(QTextStream& out, QString hdfFileName)
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
int EdgeDataContainer::readMeshDataFromHDF5(hid_t dcGid, bool preflight)
{
  int err = 0;

  err = readVertices(dcGid, preflight);
  err = readEdges(dcGid, preflight);

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainer::readEdges(hid_t dcGid, bool preflight)
{
  int err = 0;
  QVector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  if (true == preflight)
  {
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::StringConstants::EdgesName, dims, type_class, type_size);
    if (err >= 0)
    {
      EdgeArray::Pointer edges = EdgeArray::CreateArray(1, DREAM3D::EdgeData::SurfaceMeshEdges, NULL);
      setEdges(edges);
    }
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::StringConstants::EdgeNeighbors, dims, type_class, type_size);
    if(err >= 0)
    {
      Int32DynamicListArray::Pointer edgeNeighbors = Int32DynamicListArray::New();
      getEdges()->setEdgeNeighbors(edgeNeighbors);
    }
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::StringConstants::EdgesContainingVert, dims, type_class, type_size);
    if(err >= 0)
    {
      Int32DynamicListArray::Pointer edgesContainingVert = Int32DynamicListArray::New();
      getEdges()->setEdgesContainingVert(edgesContainingVert);
    }
  }
  else
  {
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::StringConstants::EdgesName, dims, type_class, type_size);
    if (err >= 0)
    {
      // Allocate the Edge_t structures
      EdgeArray::Pointer edgesPtr = EdgeArray::CreateArray(dims[0], DREAM3D::EdgeData::SurfaceMeshEdges, getVertices().get());
      // We need this to properly use QH5Lite because the data is stored as int32_t in 5 columns
      int32_t* data = reinterpret_cast<int32_t*>(edgesPtr->getPointer(0));
      // Read the data from the file
      err = QH5Lite::readPointerDataset(dcGid, DREAM3D::StringConstants::EdgesName, data);
      if (err < 0)
      {
//        setErrorCondition(err);
//        notifyErrorMessage(getHumanLabel(), "Error Reading Edge List from DREAM3D file", getErrorCondition());
        return err;
      }
      setEdges(edgesPtr);
      size_t nEdges = edgesPtr->getNumberOfTuples();
      err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::StringConstants::EdgeNeighbors, dims, type_class, type_size);
      if (err >= 0)
      {
        //Read the edgeNeighbors array into the buffer

        std::vector<uint8_t> buffer;
        err = QH5Lite::readVectorDataset(dcGid, DREAM3D::StringConstants::EdgeNeighbors, buffer);
        if(err < 0)
        {
//          setErrorCondition(err);
//          notifyErrorMessage(getHumanLabel(), "Error Reading Edge List from DREAM3D file", getErrorCondition());
          return err;
        }
        Int32DynamicListArray::Pointer edgeNeighbors = Int32DynamicListArray::New();
        edgeNeighbors->deserializeLinks(buffer, nEdges);
        getEdges()->setEdgeNeighbors(edgeNeighbors);
      }
      err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::StringConstants::EdgesContainingVert, dims, type_class, type_size);
      if (err >= 0)
      {
        //Read the edgeNeighbors array into the buffer

        std::vector<uint8_t> buffer;
        err = QH5Lite::readVectorDataset(dcGid, DREAM3D::StringConstants::EdgesContainingVert, buffer);
        if(err < 0)
        {
//          setErrorCondition(err);
//          notifyErrorMessage(getHumanLabel(), "Error Reading Edge List from DREAM3D file", getErrorCondition());
          return err;
        }
        Int32DynamicListArray::Pointer edgesContainingVert = Int32DynamicListArray::New();
        edgesContainingVert->deserializeLinks(buffer, nEdges);
        getEdges()->setEdgesContainingVert(edgesContainingVert);
      }
    }
  }
  return 1;

}
