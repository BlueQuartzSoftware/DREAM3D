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

#include "DREAM3DLib/Geometry/EdgeGeom.h"
#define GEOM_CLASS_NAME EdgeGeom

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeGeom::EdgeGeom()
{
  m_GeometryTypeName = DREAM3D::Geometry::EdgeGeometry;
  m_GeometryType = DREAM3D::GeometryType::EdgeGeometry;
  m_XdmfGridType = DREAM3D::XdmfGridType::PolyData;
  m_UnitDimensionality = 1;
  m_SpatialDimensionality = 3;
  m_VertexList = EdgeGeom::CreateSharedVertexList(0);
  m_EdgeList = EdgeGeom::CreateSharedEdgeList(0);
  m_EdgesContainingVert = CellDynamicList::NullPointer();
  m_EdgeNeighbors = CellDynamicList::NullPointer();
  m_EdgeCentroids = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeGeom::~EdgeGeom()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeGeom::Pointer EdgeGeom::CreateGeometry(int64_t numEdges, SharedVertexList::Pointer vertices, const QString& name)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  SharedEdgeList::Pointer edges = EdgeGeom::CreateSharedEdgeList(numEdges);
  EdgeGeom* d = new EdgeGeom();
  d->setVertices(vertices);
  d->setEdges(edges);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeGeom::Pointer EdgeGeom::CreateGeometry(SharedEdgeList::Pointer edges, SharedVertexList::Pointer vertices, const QString& name)
{
  if (name.isEmpty() == true)
  {
    return EdgeGeom::NullPointer();
  }
  if (vertices.get() == NULL)
  {
    return EdgeGeom::NullPointer();
  }
  if (edges.get() == NULL)
  {
    return EdgeGeom::NullPointer();
  }
  EdgeGeom* d = new EdgeGeom();
  d->setVertices(vertices);
  d->setEdges(edges);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::initializeWithZeros()
{
  m_VertexList->initializeWithZeros();
  m_EdgeList->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::addAttributeMatrix(const QString& name, AttributeMatrix::Pointer data)
{
  if (data->getType() != 0 || data->getType() != 1)
  {
    // EdgeGeom can only accept vertex or edge Attribute Matrices
    return;
  }
  if (data->getType() == 0 && static_cast<int64_t>(data->getNumTuples()) != getNumberOfVertices())
  {
    return;
  }
  if (data->getType() == 1 && data->getNumTuples() != getNumberOfTuples())
  {
    return;
  }
  if (data->getName().compare(name) != 0)
  {
    data->setName(name);
  }
  m_AttributeMatrices[name] = data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t EdgeGeom::getNumberOfTuples()
{
  return m_EdgeList->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::findCellsContainingVert()
{
  m_EdgesContainingVert = CellDynamicList::New();
  GeometryHelpers::Connectivity::FindCellsContainingVert<uint16_t, int64_t>(m_EdgeList, m_EdgesContainingVert, getNumberOfVertices());
  if (m_EdgesContainingVert.get() == NULL)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CellDynamicList::Pointer EdgeGeom::getCellsContainingVert()
{
  return m_EdgesContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::setCellsContaingVert(CellDynamicList::Pointer cellsContaingVert)
{
  m_EdgesContainingVert = cellsContaingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::deleteCellsContainingVert()
{
  m_EdgesContainingVert = CellDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::findCellNeighbors()
{
  int err = 0;
  if (m_EdgesContainingVert.get() == NULL)
  {
    return -1;
  }
  m_EdgeNeighbors = CellDynamicList::New();
  err = GeometryHelpers::Connectivity::FindCellNeighbors<uint16_t, int64_t>(m_EdgeList, m_EdgesContainingVert, m_EdgeNeighbors);
  if (m_EdgeNeighbors.get() == NULL)
  {
    err = -1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CellDynamicList::Pointer EdgeGeom::getCellNeighbors()
{
  return m_EdgeNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::setCellNeighbors(CellDynamicList::Pointer cellNeighbors)
{
  m_EdgeNeighbors = cellNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::deleteCellNeighbors()
{
  m_EdgeNeighbors = CellDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::findCellCentroids()
{
  QVector<size_t> cDims(1, 3);
  m_EdgeCentroids = FloatArrayType::CreateArray(getNumberOfEdges(), cDims, DREAM3D::StringConstants::EdgeCentroids);
  GeometryHelpers::Topology::FindCellCentroids<int64_t>(m_EdgeList, m_VertexList, m_EdgeCentroids);
  if (m_EdgeCentroids.get() == NULL)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer EdgeGeom::getCellCentroids()
{
  return m_EdgeCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::setCellCentroids(FloatArrayType::Pointer cellCentroids)
{
  m_EdgeCentroids = cellCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::deleteCellCentroids()
{
  m_EdgeCentroids = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::writeGeometryToHDF5(hid_t parentId, bool writeXdmf)
{
  herr_t err = 0;

  if (m_VertexList.get() != NULL)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_VertexList);
    if (err < 0)
    {
      return err;
    }
  }

  if (m_EdgeList.get() != NULL)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_EdgeList);
    if (err < 0)
    {
      return err;
    }
  }

  // Next write the edge centroids if the exist
  if (m_EdgeCentroids.get() != NULL)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_EdgeCentroids);
    if (err < 0)
    {
      return err;
    }
  }

  // Next write edge neighbors if they exist
  if (m_EdgeNeighbors.get() != NULL)
  {
    size_t numEdges = getNumberOfEdges();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_EdgeNeighbors, numEdges);
    if (err < 0)
    {
      return err;
    }
  }

  // last write edges containing verts if they exist
  if (m_EdgesContainingVert.get() != NULL)
  {
    size_t numVerts = getNumberOfVertices();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_EdgesContainingVert, numVerts);
    if (err < 0)
    {
      return err;
    }
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::writeXdmf(QTextStream& out, QString dcName, QString hdfFileName)
{
  herr_t err = 0;

  // Always start the grid
  out << "  <!-- *************** START OF " << dcName << " *************** -->" << "\n";
  out << "  <Grid Name=\"" << dcName << "\" GridType=\"Uniform\">" << "\n";

  out << "    <Topology TopologyType=\"Polyline\" NodesPerElement=\"2\" NumberOfElements=\"" << getNumberOfEdges() << "\">" << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << getNumberOfEdges() << " 2\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << dcName << "/" << DREAM3D::Geometry::Geometry << "/" << DREAM3D::Geometry::SharedEdgeList << "\n";
  out << "      </DataItem>" << "\n";
  out << "    </Topology>" << "\n";

  if (m_VertexList.get() == NULL)
  {
    out << "<!-- ********************* GEOMETRY ERROR ****************************************\n";
    out << "The Geometry with name '" << getName() << "' in DataContainer '" << dcName <<  "' \n";
    out << "did not have any vertices assigned.\n";
    out << "The Geometry types will be missing from the Xdmf which will cause issues when\n";
    out << "trying to load the file\n";
    out << " ********************************************************************************** -->\n";
  }
  else
  {
    out << "    <Geometry Type=\"XYZ\">" << "\n";
    out << "      <DataItem Format=\"HDF\"  Dimensions=\"" << getNumberOfVertices() << " 3\" NumberType=\"Float\" Precision=\"4\">" << "\n";
    out << "        " << hdfFileName << ":/DataContainers/" << dcName << "/" << DREAM3D::Geometry::Geometry << "/" << DREAM3D::Geometry::SharedVertexList << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Geometry>" << "\n";
    out << "" << "\n";
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  herr_t err = 0;
  QVector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  SharedVertexList::Pointer vertices = SharedVertexList::NullPointer();
  SharedEdgeList::Pointer edges = SharedEdgeList::NullPointer();
  vertices = GeometryHelpers::GeomIO::ReadMeshFromHDF5<SharedVertexList>(DREAM3D::Geometry::SharedVertexList, parentId, preflight);
  edges = GeometryHelpers::GeomIO::ReadMeshFromHDF5<SharedEdgeList>(DREAM3D::Geometry::SharedEdgeList, parentId, preflight);
  QVector<size_t> cDims(1, 0);
  FloatArrayType::Pointer edgeCentroids = FloatArrayType::CreateArray(cDims, cDims, DREAM3D::StringConstants::EdgeCentroids);
  if (preflight == true)
  {
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::EdgeNeighbors, dims, type_class, type_size);
    if (err >= 0)
    {
      CellDynamicList::Pointer edgeNeighbors = CellDynamicList::New();
      m_EdgeNeighbors = edgeNeighbors;
    }
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::EdgesContainingVert, dims, type_class, type_size);
    if (err >= 0)
    {
      CellDynamicList::Pointer edgesContainingVert = CellDynamicList::New();
      m_EdgesContainingVert = edgesContainingVert;
    }
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::EdgeCentroids, dims, type_class, type_size);
    if (err >= 0)
    {
      m_EdgeCentroids = edgeCentroids;
    }
    setVertices(vertices);
    setEdges(edges);
  }
  else
  {
    if (edges.get() == NULL)
    {
      return -1;
    }
    int64_t numEdges = edges->getNumberOfTuples();
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::EdgeNeighbors, dims, type_class, type_size);
    if (err >= 0)
    {
      // Read the edgeNeighbors array into the buffer
      std::vector<uint8_t> buffer;
      err = QH5Lite::readVectorDataset(parentId, DREAM3D::StringConstants::EdgeNeighbors, buffer);
      if (err < 0)
      {
        return err;
      }
      CellDynamicList::Pointer edgeNeighbors = CellDynamicList::New();
      edgeNeighbors->deserializeLinks(buffer, numEdges);
      m_EdgeNeighbors = edgeNeighbors;
    }
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::EdgesContainingVert, dims, type_class, type_size);
    if (err >= 0)
    {
      // Read the edgesContainingVert array into the buffer
      std::vector<uint8_t> buffer;
      err = QH5Lite::readVectorDataset(parentId, DREAM3D::StringConstants::EdgesContainingVert, buffer);
      if (err < 0)
      {
        return err;
      }
      CellDynamicList::Pointer edgesContainingVert = CellDynamicList::New();
      edgesContainingVert->deserializeLinks(buffer, numEdges);
      m_EdgesContainingVert = edgesContainingVert;
    }
    err = edgeCentroids->readH5Data(parentId);
    if (err >= 0)
    {
      m_EdgeCentroids = edgeCentroids;
    }
    setVertices(vertices);
    setEdges(edges);
  }

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer EdgeGeom::deepCopy()
{
  EdgeGeom::Pointer edgeCopy = EdgeGeom::CreateGeometry(getEdges(), getVertices(), getName());

  edgeCopy->setCellsContaingVert(getCellsContainingVert());
  edgeCopy->setCellNeighbors(getCellNeighbors());
  edgeCopy->setCellCentroids(getCellCentroids());
  edgeCopy->setSpatialDimensionality(getSpatialDimensionality());

  return edgeCopy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// Shared ops includes
#define GEOM_CLASS_NAME EdgeGeom
#include "DREAM3DLib/Geometry/SharedGeometryOps.cpp"
#include "DREAM3DLib/Geometry/SharedVertexOps.cpp"
#include "DREAM3DLib/Geometry/SharedEdgeOps.cpp"
