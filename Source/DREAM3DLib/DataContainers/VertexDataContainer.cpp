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

#include "DREAM3DLib/DataContainers/VertexDataContainer.h"

// C Includes

// C++ Includes
#include <iostream>
#include <fstream>


// DREAM3D Includes
#include "DREAM3DLib/Math/DREAM3DMath.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexDataContainer::VertexDataContainer() :
  DataContainer()
{
  setName(DREAM3D::Defaults::VertexDataContainerName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexDataContainer::~VertexDataContainer()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexDataContainer::writeMeshToHDF5(hid_t dcGid, bool writeXdmf)
{
  herr_t err = 0;
  writeVerticesToHDF5(dcGid, writeXdmf);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexDataContainer::writeVerticesToHDF5(hid_t dcGid, bool writeXdmf)
{
  herr_t err = 0;
  //first write the faces if they exist
  VertexArray::Pointer verticesPtr = getVertices();
  if (verticesPtr.get() != NULL)
  {
    int32_t rank = 2; // THIS NEEDS TO BE THE SAME AS THE NUMBER OF ELEMENTS IN THE Structure from SurfaceMesh::DataStruc
    hsize_t dims[2] = {verticesPtr->getNumberOfTuples(), 3};

    float* data = reinterpret_cast<float*>(verticesPtr->getPointer(0));

    err = QH5Lite::writePointerDataset(dcGid, DREAM3D::StringConstants::VerticesName, rank, dims, data);
    if (err < 0)
    {
//      setErrorCondition(err);
//      notifyErrorMessage(getHumanLabel(), "Error Writing Face List to DREAM3D file", getErrorCondition());
    }

    if(writeXdmf == true)
    {
      QVector<size_t> cDims(1, 3);
      DataArray<int32_t>::Pointer vertsPtr = DataArray<int32_t>::CreateArray(verticesPtr->getNumberOfTuples(), cDims, DREAM3D::StringConstants::VertsName);
      int32_t* verts = vertsPtr->getPointer(0);
      for(int i=0;i<vertsPtr->getNumberOfTuples();i++)
      {
        verts[3*i] = 1;
        verts[3*i+1] = 1;
        verts[3*i+2] = i;
      }
      int32_t rank = 2;
      hsize_t dims[2] = {vertsPtr->getNumberOfTuples(), 3};
      err = QH5Lite::writePointerDataset(dcGid, DREAM3D::StringConstants::VertsName, rank, dims, verts);
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer VertexDataContainer::deepCopy()
{
  VertexDataContainer::Pointer dcCopy = VertexDataContainer::New(getName());

  size_t dims[3];
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
int VertexDataContainer::writeXdmf(QTextStream& out, QString hdfFileName)
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
    if(amType == DREAM3D::AttributeMatrixType::Vertex)
    {
      QString xdmfText = attrMat->generateXdmfText("Node", getName(), hdfFileName);
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
void VertexDataContainer::writeXdmfMeshStructureHeader(QTextStream& out, QString hdfFileName)
{
  VertexArray::Pointer verts = getVertices();
  if(NULL == verts.get())
  {
    return;
  }

  out << "  <Grid Name=\"" << getName() << "\">" << "\n";
  out << "    <Topology TopologyType=\"Polyvertex\" NumberOfElements=\"" << verts->getNumberOfTuples() << "\">" << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << verts->getNumberOfTuples() << " 3\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << getName() << "/Verts" << "\n";
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
void VertexDataContainer::writeXdmfMeshStructureFooter(QTextStream& xdmf)
{
  xdmf << "  </Grid>" << "\n";
  xdmf << "    <!-- *************** END OF " << getName() << " *************** -->" << "\n";
  xdmf << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexDataContainer::readMeshDataFromHDF5(hid_t dcGid, bool preflight)
{
  int err = 0;

  readVertices(dcGid, preflight);

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexDataContainer::readVertices(hid_t dcGid, bool preflight)
{
  int err = 0;
  QVector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  if (true == preflight)
  {
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::StringConstants::VerticesName, dims, type_class, type_size);
    if (err >= 0)
    {
      VertexArray::Pointer vertices = VertexArray::CreateArray(1, DREAM3D::VertexData::SurfaceMeshNodes);
      setVertices(vertices);
    }
  }
  else
  {
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::StringConstants::VerticesName, dims, type_class, type_size);
    if (err >= 0)
    {
      // Allocate the Vertex_t structures
      VertexArray::Pointer verticesPtr = VertexArray::CreateArray(dims[0], DREAM3D::VertexData::SurfaceMeshNodes);
      // We need this to properly use QH5Lite because the data is stored as int32_t in 5 columns
      float* data = reinterpret_cast<float*>(verticesPtr->getPointer(0));
      // Read the data from the file
      err = QH5Lite::readPointerDataset(dcGid, DREAM3D::StringConstants::VerticesName, data);
      if (err < 0)
      {
//        setErrorCondition(err);
//        notifyErrorMessage(getHumanLabel(), "Error Reading Vertex List from DREAM3D file", getErrorCondition());
        return err;
      }
      setVertices(verticesPtr);
    }
  }
  return 1;
}
