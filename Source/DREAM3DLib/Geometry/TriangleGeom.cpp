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

#include "DREAM3DLib/Geometry/TriangleGeom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleGeom::TriangleGeom()
{
  m_GeometryTypeName = DREAM3D::Geometry::TriangleGeometry;
  m_GeometryType = DREAM3D::GeometryType::TriangleGeometry;
  m_XdmfGridType = DREAM3D::XdmfGridType::PolyData;
  m_UnitDimensionality = 2;
  m_SpatialDimensionality = 3;
  m_VertexList = TriangleGeom::CreateSharedVertexList(0);
  m_TriList = TriangleGeom::CreateSharedTriList(0);
  m_EdgeList = SharedEdgeList::NullPointer();
  m_TrianglesContainingVert = CellDynamicList::NullPointer();
  m_TriangleNeighbors = CellDynamicList::NullPointer();
  m_TriangleCentroids = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleGeom::~TriangleGeom()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleGeom::Pointer TriangleGeom::CreateGeometry(int64_t numTriangles, SharedVertexList::Pointer vertices, const QString& name)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  SharedTriList::Pointer triangles = TriangleGeom::CreateSharedTriList(0);
  TriangleGeom* d = new TriangleGeom();
  d->setVertices(vertices);
  d->setTriangles(triangles);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleGeom::Pointer TriangleGeom::CreateGeometry(SharedTriList::Pointer triangles, SharedVertexList::Pointer vertices, const QString& name)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  if (vertices.get() == NULL)
  {
    return TriangleGeom::NullPointer();
  }
  if (triangles.get() == NULL)
  {
    return TriangleGeom::NullPointer();
  }
  TriangleGeom* d = new TriangleGeom();
  d->setVertices(vertices);
  d->setTriangles(triangles);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::initializeWithZeros()
{
  m_VertexList->initializeWithZeros();
  m_EdgeList->initializeWithZeros();
  m_TriList->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::findTrianglesContainingVert()
{
  m_TrianglesContainingVert = CellDynamicList::New();
  GeometryHelpers::Connectivity::FindCellsContainingVert<uint16_t, int64_t>(m_TriList, m_TrianglesContainingVert, getNumberOfVertices());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::deleteTrianglesContainingVert()
{
  m_TrianglesContainingVert = CellDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::findTriangleNeighbors()
{
  if (m_TrianglesContainingVert.get() == NULL)
  {
    return;
  }
  m_TriangleNeighbors = CellDynamicList::New();
  GeometryHelpers::Connectivity::FindCellNeighbors<uint16_t, int64_t>(m_TriList, m_TrianglesContainingVert, m_TriangleNeighbors);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::deleteTriangleNeighbors()
{
  m_TriangleNeighbors = CellDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::findTriangleCentroids()
{
  QVector<size_t> cDims(1, 3);
  m_TriangleCentroids = FloatArrayType::CreateArray(getNumberOfTris(), cDims, DREAM3D::StringConstants::TriangleCentroids);
  GeometryHelpers::Topology::FindCellCentroids<int64_t>(m_TriList, m_VertexList, m_TriangleCentroids);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::deleteTriangleCentroids()
{
  m_TriangleCentroids = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::writeGeometryToHDF5(hid_t parentId, bool writeXdmf)
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

  if (m_TriList.get() != NULL)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_TriList);
    if (err < 0)
    {
      return err;
    }
  }

  // Next write the triangle centroids if the exist
  if (m_TriangleCentroids.get() != NULL)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_TriangleCentroids);
    if (err < 0)
    {
      return err;
    }
  }

  // Next write triangle neighbors if they exist
  if (m_TriangleNeighbors.get() != NULL)
  {
    size_t numTris = getNumberOfTris();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_TriangleNeighbors, numTris);
    if (err < 0)
    {
      return err;
    }
  }

  // Last write triangles containing verts if they exist
  if (m_TrianglesContainingVert.get() != NULL)
  {
    size_t numVerts = getNumberOfVertices();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_TrianglesContainingVert, numVerts);
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
int TriangleGeom::writeXdmf(QTextStream& out, QString dcName, QString hdfFileName)
{
  herr_t err = 0;

  // Always start the grid
  out << "  <!-- *************** START OF " << dcName << " *************** -->" << "\n";
  out << "  <Grid Name=\"" << dcName << "\" GridType=\"Uniform\">" << "\n";

  out << "    <Topology TopologyType=\"Triangle\" NumberOfElements=\"" << getNumberOfTris() << "\">" << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << getNumberOfTris() << " 3\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << dcName << "/" << DREAM3D::Geometry::Geometry << "/" << DREAM3D::Geometry::SharedTriList << "\n";
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
int TriangleGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  herr_t err = 0;
  QVector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  SharedVertexList::Pointer vertices = SharedVertexList::NullPointer();
  SharedEdgeList::Pointer edges = SharedEdgeList::NullPointer();
  SharedTriList::Pointer triangles = SharedTriList::NullPointer();
  vertices = GeometryHelpers::GeomIO::ReadMeshFromHDF5<SharedVertexList>(DREAM3D::Geometry::SharedVertexList, parentId, preflight);
  edges = GeometryHelpers::GeomIO::ReadMeshFromHDF5<SharedEdgeList>(DREAM3D::Geometry::SharedEdgeList, parentId, preflight);
  triangles = GeometryHelpers::GeomIO::ReadMeshFromHDF5<SharedTriList>(DREAM3D::Geometry::SharedTriList, parentId, preflight);
  QVector<size_t> cDims(1, 0);
  FloatArrayType::Pointer triCentroids = FloatArrayType::CreateArray(cDims, cDims, DREAM3D::StringConstants::TriangleCentroids);
  if (true == preflight)
  {
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::TriangleNeighbors, dims, type_class, type_size);
    if (err >= 0)
    {
      CellDynamicList::Pointer triNeighbors = CellDynamicList::New();
      setTriangleNeighbors(triNeighbors);
    }
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::TrianglesContainingVert, dims, type_class, type_size);
    if (err >= 0)
    {
      CellDynamicList::Pointer trisContainingVert = CellDynamicList::New();
      setTrianglesContainingVert(trisContainingVert);
    }
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::TriangleCentroids, dims, type_class, type_size);
    if (err >= 0)
    {
      setTriangleCentroids(triCentroids);
    }
    setVertices(vertices);
    setEdges(edges);
    setTriangles(triangles);
  }
  else
  {
    if (triangles.get() == NULL)
    {
      return -1;
    }
    size_t numTris = triangles->getNumberOfTuples();
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::TriangleNeighbors, dims, type_class, type_size);
    if (err >= 0)
    {
      // Read the triNeighbors array into the buffer
      std::vector<uint8_t> buffer;
      err = QH5Lite::readVectorDataset(parentId, DREAM3D::StringConstants::TriangleNeighbors, buffer);
      if (err < 0)
      {
        return err;
      }
      CellDynamicList::Pointer triNeighbors = CellDynamicList::New();
      triNeighbors->deserializeLinks(buffer, numTris);
      setTriangleNeighbors(triNeighbors);
    }
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::TrianglesContainingVert, dims, type_class, type_size);
    if (err >= 0)
    {
      // Read the trisContainingVert array into the buffer
      std::vector<uint8_t> buffer;
      err = QH5Lite::readVectorDataset(parentId, DREAM3D::StringConstants::TrianglesContainingVert, buffer);
      if (err < 0)
      {
        return err;
      }
      CellDynamicList::Pointer trisContainingVert = CellDynamicList::New();
      trisContainingVert->deserializeLinks(buffer, numTris);
      setTrianglesContainingVert(trisContainingVert);
    }
    err = triCentroids->readH5Data(parentId);
    if (err >= 0)
    {
      setTriangleCentroids(triCentroids);
    }
    setVertices(vertices);
    setEdges(edges);
    setTriangles(triangles);
  }

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// Shared ops includes
#define GEOM_CLASS_NAME TriangleGeom
#include "DREAM3DLib/Geometry/SharedGeometryOps.cpp"
#include "DREAM3DLib/Geometry/SharedVertexOps.cpp"
#include "DREAM3DLib/Geometry/SharedEdgeOps.cpp"
#include "DREAM3DLib/Geometry/SharedTriOps.cpp"
