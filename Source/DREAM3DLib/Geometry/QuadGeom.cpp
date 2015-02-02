/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * RedisQuadbution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * RedisQuadbutions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * RedisQuadbutions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the disQuadbution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its conQuadbutors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONQuadBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONQuadBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, SQuadCT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DREAM3DLib/Geometry/QuadGeom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuadGeom::QuadGeom()
{
  m_GeometryTypeName = DREAM3D::Geometry::QuadGeometry;
  m_GeometryType = DREAM3D::GeometryType::QuadGeometry;
  m_XdmfGridType = DREAM3D::XdmfGridType::PolyData;
  m_UnitDimensionality = 2;
  m_SpatialDimensionality = 3;
  m_VertexList = QuadGeom::CreateSharedVertexList(0);
  m_QuadList = QuadGeom::CreateSharedQuadList(0);
  m_EdgeList = SharedEdgeList::NullPointer();
  m_QuadsContainingVert = CellDynamicList::NullPointer();
  m_QuadNeighbors = CellDynamicList::NullPointer();
  m_QuadCentroids = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuadGeom::~QuadGeom()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuadGeom::Pointer QuadGeom::CreateGeometry(int64_t numQuads, SharedVertexList::Pointer vertices, const QString& name)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  SharedQuadList::Pointer quads = QuadGeom::CreateSharedQuadList(0);
  QuadGeom* d = new QuadGeom();
  d->setVertices(vertices);
  d->setQuads(quads);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuadGeom::Pointer QuadGeom::CreateGeometry(SharedQuadList::Pointer quads, SharedVertexList::Pointer vertices, const QString& name)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  if (vertices.get() == NULL)
  {
    return QuadGeom::NullPointer();
  }
  if (quads.get() == NULL)
  {
    return QuadGeom::NullPointer();
  }
  QuadGeom* d = new QuadGeom();
  d->setVertices(vertices);
  d->setQuads(quads);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::initializeWithZeros()
{
  m_VertexList->initializeWithZeros();
  m_EdgeList->initializeWithZeros();
  m_QuadList->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::findQuadsContainingVert()
{
  m_QuadsContainingVert = CellDynamicList::New();
  GeometryHelpers::Connectivity::FindCellsContainingVert<uint16_t, int64_t>(m_QuadList, m_QuadsContainingVert, getNumberOfVertices());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::deleteQuadsContainingVert()
{
  m_QuadsContainingVert = CellDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::findQuadNeighbors()
{
  if (m_QuadsContainingVert.get() == NULL)
  {
    return;
  }
  m_QuadNeighbors = CellDynamicList::New();
  GeometryHelpers::Connectivity::FindCellNeighbors<uint16_t, int64_t>(m_QuadList, m_QuadsContainingVert, m_QuadNeighbors);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::deleteQuadNeighbors()
{
  m_QuadNeighbors = CellDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::findQuadCentroids()
{
  QVector<size_t> cDims(1, 3);
  m_QuadCentroids = FloatArrayType::CreateArray(getNumberOfEdges(), cDims, DREAM3D::StringConstants::QuadCentroids);
  GeometryHelpers::Topology::FindCellCentroids<int64_t>(m_QuadList, m_VertexList, m_QuadCentroids);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::deleteQuadCentroids()
{
  m_QuadCentroids = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuadGeom::writeGeometryToHDF5(hid_t parentId, bool writeXdmf)
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

  if (m_QuadList.get() != NULL)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_QuadList);
    if (err < 0)
    {
      return err;
    }
  }

  // Next write the quad centroids if the exist
  if (m_QuadCentroids.get() != NULL)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_QuadCentroids);
    if (err < 0)
    {
      return err;
    }
  }

  // Next write quad neighbors if they exist
  if (m_QuadNeighbors.get() != NULL)
  {
    size_t numQuads = getNumberOfQuads();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_QuadNeighbors, numQuads);
    if (err < 0)
    {
      return err;
    }
  }

  // Last write faces containing verts if they exist
  if (m_QuadsContainingVert.get() != NULL)
  {
    size_t numVerts = getNumberOfVertices();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_QuadsContainingVert, numVerts);
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
int QuadGeom::writeXdmf(QTextStream& out, QString dcName, QString hdfFileName)
{
  herr_t err = 0;

  // Always start the grid
  out << "  <!-- *************** START OF " << dcName << " *************** -->" << "\n";
  out << "  <Grid Name=\"" << dcName << "\" GridType=\"Uniform\">" << "\n";

  out << "    <Topology TopologyType=\"Quadrilateral\" NumberOfElements=\"" << getNumberOfQuads() << "\">" << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << getNumberOfQuads() << " 3\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << dcName << "/" << DREAM3D::Geometry::Geometry << "/" << DREAM3D::Geometry::SharedQuadList << "\n";
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
int QuadGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  herr_t err = 0;
  QVector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  SharedVertexList::Pointer vertices = SharedVertexList::NullPointer();
  SharedEdgeList::Pointer edges = SharedEdgeList::NullPointer();
  SharedQuadList::Pointer quads = SharedQuadList::NullPointer();
  vertices = GeometryHelpers::GeomIO::ReadMeshFromHDF5<SharedVertexList>(DREAM3D::Geometry::SharedVertexList, parentId, preflight);
  edges = GeometryHelpers::GeomIO::ReadMeshFromHDF5<SharedEdgeList>(DREAM3D::Geometry::SharedEdgeList, parentId, preflight);
  quads = GeometryHelpers::GeomIO::ReadMeshFromHDF5<SharedQuadList>(DREAM3D::Geometry::SharedQuadList, parentId, preflight);
  QVector<size_t> cDims(1, 0);
  FloatArrayType::Pointer quadCentroids = FloatArrayType::CreateArray(cDims, cDims, DREAM3D::StringConstants::QuadCentroids);
  if (true == preflight)
  {
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::QuadNeighbors, dims, type_class, type_size);
    if (err >= 0)
    {
      CellDynamicList::Pointer quadNeighbors = CellDynamicList::New();
      setQuadNeighbors(quadNeighbors);
    }
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::QuadsContainingVert, dims, type_class, type_size);
    if (err >= 0)
    {
      CellDynamicList::Pointer quadsContainingVert = CellDynamicList::New();
      setQuadsContainingVert(quadsContainingVert);
    }
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::QuadCentroids, dims, type_class, type_size);
    if (err >= 0)
    {
      setQuadCentroids(quadCentroids);
    }
    setVertices(vertices);
    setEdges(edges);
    setQuads(quads);
  }
  else
  {
    if (quads.get() == NULL)
    {
      return -1;
    }
    size_t numQuads = quads->getNumberOfTuples();
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::QuadNeighbors, dims, type_class, type_size);
    if (err >= 0)
    {
      // Read the quadNeighbors array into the buffer
      std::vector<uint8_t> buffer;
      err = QH5Lite::readVectorDataset(parentId, DREAM3D::StringConstants::QuadNeighbors, buffer);
      if (err < 0)
      {
        return err;
      }
      CellDynamicList::Pointer quadNeighbors = CellDynamicList::New();
      quadNeighbors->deserializeLinks(buffer, numQuads);
      setQuadNeighbors(quadNeighbors);
    }
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::QuadsContainingVert, dims, type_class, type_size);
    if (err >= 0)
    {
      // Read the quadsContainingVert array into the buffer
      std::vector<uint8_t> buffer;
      err = QH5Lite::readVectorDataset(parentId, DREAM3D::StringConstants::QuadsContainingVert, buffer);
      if (err < 0)
      {
        return err;
      }
      CellDynamicList::Pointer quadsContainingVert = CellDynamicList::New();
      quadsContainingVert->deserializeLinks(buffer, numQuads);
      setQuadsContainingVert(quadsContainingVert);
    }
    err = quadCentroids->readH5Data(parentId);
    if (err >= 0)
    {
      setQuadCentroids(quadCentroids);
    }
    setVertices(vertices);
    setEdges(edges);
    setQuads(quads);
  }

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// Shared ops includes
#define GEOM_CLASS_NAME QuadGeom
#include "DREAM3DLib/Geometry/SharedGeometryOps.cpp"
#include "DREAM3DLib/Geometry/SharedVertexOps.cpp"
#include "DREAM3DLib/Geometry/SharedEdgeOps.cpp"
#include "DREAM3DLib/Geometry/SharedQuadOps.cpp"
