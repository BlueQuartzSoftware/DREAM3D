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
  setName(DREAM3D::HDF5::EdgeDataContainerName);
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
int EdgeDataContainer::writeMeshToHDF5(hid_t dcGid)
{
  herr_t err = 0;
  writeVerticesToHDF5(dcGid);
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
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainer::writeXdmf(QTextStream& out)
{
  herr_t err = 0;



  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainer::writeXdmfMeshStructure()
{

  EdgeDataContainer* dc = EdgeDataContainer::SafePointerDownCast(getDataContainer());

  if (getWriteXdmfFile() == false || getXdmfOStream() == NULL)
  {
    return;
  }
  EdgeArray::Pointer edges = dc->getEdges();
  if (NULL == edges.get())
  {
    return;
  }
  VertexArray::Pointer verts = dc->getVertices();
  if(NULL == verts.get())
  {
    return;
  }

  QString hdfFileName = QH5Utilities::fileNameFromFileId(getHdfFileId());

  QTextStream& out = *getXdmfOStream();

  out << "  <Grid Name=\"" << getDataContainer()->getName() << "\">" << "\n";
  out << "    <Topology TopologyType=\"Polyline\" NodesPerElement=\"2\" NumberOfElements=\"" << edges->getNumberOfTuples() << "\">" << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << edges->getNumberOfTuples() << " 2\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << getDataContainer()->getName() << "/Edges" << "\n";
  out << "      </DataItem>" << "\n";
  out << "    </Topology>" << "\n";

  out << "    <Geometry Type=\"XYZ\">" << "\n";
  out << "      <DataItem Format=\"HDF\"  Dimensions=\"" << verts->getNumberOfTuples() << " 3\" NumberType=\"Float\" Precision=\"4\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << getDataContainer()->getName() << "/Vertices" << "\n";
  out << "      </DataItem>" << "\n";
  out << "    </Geometry>" << "\n";
  out << "" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EdgeDataContainer::writeXdmfAttributeDataHelper(int numComp, const QString& attrType,
    const QString& groupName,
    IDataArray::Pointer array,
    const QString& centering,
    int precision, const QString& xdmfTypeName)
{
  QString buf;
  QTextStream out(&buf);

  QString hdfFileName = QH5Utilities::fileNameFromFileId(getHdfGroupId());

  QString dimStr = QString::number(array->getNumberOfTuples()) + QString(" ") + QString::number(array->GetNumberOfComponents());
  QString dimStrHalf = QString::number(array->getNumberOfTuples()) + QString(" ") + QString::number(array->GetNumberOfComponents() / 2);

  if((numComp % 2) == 1)
  {
    out << "    <Attribute Name=\"" << array->GetName() << "\" ";
    out << "AttributeType=\"" << attrType << "\" ";
    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem Format=\"HDF\" Dimensions=\"" << array->getNumberOfTuples() << " " << array->GetNumberOfComponents() <<  "\" ";
    out << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";
    out << "        " << hdfFileName << ":/DataContainers/" << getDataContainer()->getName() << "/" << groupName << "/" << array->GetName() << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n" << "\n";
  }
  else
  {
    //First Slab
    out << "    <Attribute Name=\"" << array->GetName() << " (Feature 0)\" ";
    out << "AttributeType=\"" << attrType << "\" ";
    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"" << array->getNumberOfTuples() << " " << (array->GetNumberOfComponents() / 2) <<  "\" ";
    out << "Type=\"HyperSlab\" " << "Name=\"" << array->GetName() << " (Feature 0)\" >" << "\n";
    out << "        <DataItem Dimensions=\"3 2\" " << "Format=\"XML\" >" << "\n";
    out << "          0        0" << "\n";
    out << "          1        1" << "\n";
    out << "          " << array->getNumberOfTuples() << " " << (array->GetNumberOfComponents() / 2) << " </DataItem>" << "\n";
    out << "\n";
    out << "        <DataItem Format=\"HDF\" Dimensions=\"" << array->getNumberOfTuples() << " " << array->GetNumberOfComponents() << "\" " << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";
    out << "        " << hdfFileName << ":/DataContainers/" << getDataContainer()->getName() << "/" << groupName << "/" << array->GetName() << "\n";
    out << "        </DataItem>" << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n" << "\n";

    //Second Slab
    out << "    <Attribute Name=\"" << array->GetName() << " (Feature 1)\" ";
    out << "AttributeType=\"" << attrType << "\" ";
    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"" << array->getNumberOfTuples() << " " << (array->GetNumberOfComponents() / 2) <<  "\" ";
    out << "Type=\"HyperSlab\" " << "Name=\"" << array->GetName() << " (Feature 1)\" >" << "\n";
    out << "        <DataItem Dimensions=\"3 2\" " << "Format=\"XML\" >" << "\n";
    out << "          0        " << (array->GetNumberOfComponents() / 2) << "\n";
    out << "          1        1" << "\n";
    out << "          " << array->getNumberOfTuples() << " " << (array->GetNumberOfComponents() / 2) << " </DataItem>" << "\n";
    out << "\n";
    out << "        <DataItem Format=\"HDF\" Dimensions=\"" << array->getNumberOfTuples() << " " << array->GetNumberOfComponents() << "\" " << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";

    out << "        " << hdfFileName << ":/DataContainers/" << getDataContainer()->getName() << "/" << groupName << "/" << array->GetName() << "\n";
    out << "        </DataItem>" << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n" << "\n";
  }
  return buf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainer::writeXdmfAttributeData(const QString& groupName, IDataArray::Pointer array, const QString& centering)
{
#if 0
  < Attribute Name = "Node Type" Center = "Node" >
                                          < DataItem Format = "HDF" DataType = "char" Precision = "1" Dimensions = "43029 1" >
                                                            MC_IsoGG_50cubed_55features_Bounded_Multi.dream3d:
                                                              / EdgeDataContainer / POINT_DATA / SurfaceMeshNodeType
                                                              < / DataItem >
                                                              < / Attribute >
#endif
                                                              if (getWriteXdmfFile() == false || getXdmfOStream() == NULL)
  { return; }



  QTextStream& out = *getXdmfOStream();
  int precision = 0;
  QString xdmfTypeName;
  array->GetXdmfTypeAndSize(xdmfTypeName, precision);
  if (0 == precision)
  {
    out << "<!-- " << array->GetName() << " has unkown type or unsupported type or precision for XDMF to understand" << " -->" << "\n";
    return;
  }
  int numComp = array->GetNumberOfComponents();
  QString attrType = "Scalar";
  if(numComp > 2) { attrType = "Vector"; }

  QString block = writeXdmfAttributeDataHelper(numComp, attrType, groupName, array, centering, precision, xdmfTypeName);

  out << block << "\n";
}