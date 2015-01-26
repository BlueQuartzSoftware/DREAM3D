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
void EdgeGeom::findEdgesContainingVert()
{
  size_t numVerts = m_VertexList->getNumberOfTuples();
  size_t numEdges = m_EdgeList->getNumberOfTuples();

  m_EdgesContainingVert = UInt16Int64DynamicListArray::New();

  // Allocate the basic structures
  QVector<size_t> linkCount(numEdges, 0);
  size_t edgeId = 0;
  int64_t* linkLoc;

  // Fill out lists with number of references to cells
  typedef boost::shared_array<int64_t> SharedInt64Array_t;
  SharedInt64Array_t linkLocPtr(new int64_t[numVerts]);
  linkLoc = linkLocPtr.get();

  ::memset(linkLoc, 0, numVerts * sizeof(int64_t));

  int64_t verts[2];

  //vtkPolyData *pdata = static_cast<vtkPolyData *>(data);
  // Traverse data to determine number of uses of each point
  for (edgeId = 0; edgeId < numEdges; edgeId++)
  {
    getVertsAtEdge(edgeId, verts);
    for (size_t j = 0; j < 2; j++)
    {
      linkCount[verts[j]]++;
    }
  }

  // Now allocate storage for the links
  m_EdgesContainingVert->allocateLists(linkCount);

  for (edgeId = 0; edgeId < numEdges; edgeId++)
  {
    getVertsAtEdge(edgeId, verts);
    for (size_t j = 0; j < 2; j++)
    {
      m_EdgesContainingVert->insertCellReference(verts[j], (linkLoc[verts[j]])++, edgeId);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::deleteEdgesContainingVert()
{
  m_EdgesContainingVert = UInt16Int64DynamicListArray::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::findEdgeNeighbors()
{
  int64_t numEdges = m_EdgeList->getNumberOfTuples();

  m_EdgeNeighbors = UInt16Int64DynamicListArray::New();

  QVector<uint16_t> linkCount(numEdges, 0);

  // Allocate an array of bools that we use each iteration of edge so that we don't put duplicates into the array
  boost::shared_array<bool> visitedPtr(new bool[numEdges]);
  bool* visited = visitedPtr.get();
  ::memset(visitedPtr.get(), 0, numEdges);

  // Reuse this vector for each loop. Avoids re-allocating the memory each time through the loop
  QVector<int64_t> loop_neighbors(32, 0);

  // Build up the edge adjacency list now that we have the cell links
  for(int64_t t = 0; t < numEdges; ++t)
  {
    //   qDebug() << "Analyzing Edge " << t << "\n";
    int64_t* seedEdge = getEdgePointer(t);
    for(int64_t v = 0; v < 2; ++v)
    {
      //   qDebug() << " vert " << v << "\n";
      int nEs = m_EdgesContainingVert->getNumberOfElements(seedEdge[v]);
      int64_t* vertIdxs = m_EdgesContainingVert->getElementListPointer(seedEdge[v]);

      for(int vt = 0; vt < nEs; ++vt)
      {
        if (vertIdxs[vt] == static_cast<int64_t>(t) ) { continue; } // This is the same edge as our "source" edge
        if (visited[vertIdxs[vt]] == true) { continue; } // We already added this edge so loop again
        //      qDebug() << "   Comparing Edge " << vertIdxs[vt] << "\n";
        int64_t* vertEdge = getEdgePointer(vertIdxs[vt]);
        int64_t vCount = 0;
        // Loop over all the vertex indices of this edge and try to match 2 of them to the current loop edge
        // If there are 2 matches then that edge is a neighbor of this edge. if there are more than 2 matches
        // then there is a real problem with the mesh and the program is going to assert.
        // Unrolled the loop to shave about 25% of time off the outer loop.
        int64_t seedEdgeVert0 = seedEdge[0];
        int64_t seedEdgeVert1 = seedEdge[1];
        int64_t trgtEdgeVert0 = vertEdge[0];
        int64_t trgtEdgeVert1 = vertEdge[1];

        if (seedEdgeVert0 == trgtEdgeVert0 || seedEdgeVert0 == trgtEdgeVert1)
        {
          vCount++;
        }
        if (seedEdgeVert1 == trgtEdgeVert0 || seedEdgeVert1 == trgtEdgeVert1)
        {
          vCount++;
        }

        BOOST_ASSERT(vCount < 2); // No way 2 edges can share both vertices. Something is VERY wrong at this point

        // So if our vertex match count is 2 and we have not visited the edge in question then add this edge index
        // into the list of vertex indices as neighbors for the source edge.
        if (vCount == 1)
        {
          //qDebug() << "       Neighbor: " << vertIdxs[vt] << "\n";
          // Use the current count of neighbors as the index
          // into the loop_neighbors vector and place the value of the vertex edge at that index
          loop_neighbors[linkCount[t]] = vertIdxs[vt];
          linkCount[t]++; // Increment the count for the next time through
          if (linkCount[t] >= loop_neighbors.size())
          {
            loop_neighbors.resize(loop_neighbors.size() + 10);
          }
          visited[vertIdxs[vt]] = true; // Set this edge as visited so we do NOT add it again
        }
      }
    }
    BOOST_ASSERT(linkCount[t] > 1);
    // Reset all the visited edge indexs back to false (zero)
    for(int64_t k = 0; k < linkCount[t]; ++k)
    {
      visited[loop_neighbors[k]] = false;
    }
    // Allocate the array storage for the current edge to hold its vertex list
    m_EdgeNeighbors->setElementList(t, linkCount[t], &(loop_neighbors[0]));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::deleteEdgeNeighbors()
{
  m_EdgeNeighbors = UInt16Int64DynamicListArray::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::writeGeometryToHDF5(hid_t parentId, bool writeXdmf)
{
  herr_t err = 0;
  int32_t rank = 0;
  hsize_t dims[2] = {0, 2ULL};
  QVector<size_t> tDims(1, 0);

  if (m_VertexList.get() != NULL)
  {
    tDims[0] = getNumberOfVertices();
    err = m_VertexList->writeH5Data(parentId, tDims);
    if (err < 0)
    {
      return err;
    }
    if(writeXdmf == true)
    {
      QVector<size_t> cDims(1, 1);
      DataArray<int64_t>::Pointer vertsPtr = DataArray<int64_t>::CreateArray(getNumberOfVertices(), cDims, DREAM3D::StringConstants::VertsName);
      int64_t* verts = vertsPtr->getPointer(0);
      for(size_t i = 0; i < vertsPtr->getNumberOfTuples(); i++)
      {
        verts[i] = i;
      }
      tDims[0] = vertsPtr->getNumberOfTuples();
      err = vertsPtr->writeH5Data(parentId, tDims);
    }
  }

  if (m_EdgeList.get() != NULL)
  {
    tDims[0] = getNumberOfEdges();
    err = m_EdgeList->writeH5Data(parentId, tDims);
    if (err < 0)
    {
      return err;
    }
  }

  // Next write edge neighbors if they exist
  UInt16Int64DynamicListArray::Pointer edgeNeighbors = getEdgeNeighbors();
  if (NULL != edgeNeighbors.get())
  {
    size_t total = 0;
    size_t numEdges = getNumberOfEdges();
    for(size_t v = 0; v < numEdges; ++v)
    {
      total += edgeNeighbors->getNumberOfElements(v);
    }

    size_t totalBytes = numEdges * sizeof(uint16_t) + total * sizeof(int32_t);

    // Allocate a flat array to copy the data into
    QVector<uint8_t> buffer(totalBytes, 0);
    uint8_t* bufPtr = &(buffer.front());
    size_t offset = 0;

    for(size_t v = 0; v < numEdges; ++v)
    {
      uint16_t ncells = edgeNeighbors->getNumberOfElements(v);
      int64_t* cells = edgeNeighbors->getElementListPointer(v);
      ::memcpy(bufPtr + offset, &ncells, sizeof(uint16_t));
      offset += sizeof(uint16_t);
      ::memcpy(bufPtr + offset, cells, ncells * sizeof(int64_t) );
      offset += ncells * sizeof(int64_t);
    }

    rank = 1;
    dims[0] = totalBytes;

    err = QH5Lite::writePointerDataset(parentId, DREAM3D::StringConstants::EdgeNeighbors, rank, dims, bufPtr);
    if (err < 0)
    {
      return err;
    }
  }

  // last write edges containing verts if they exist
  UInt16Int64DynamicListArray::Pointer edgesContainingVert = getEdgesContainingVert();
  if (NULL != edgesContainingVert.get())
  {
    size_t total = 0;
    size_t numVerts = getNumberOfVertices();
    for(size_t v = 0; v < numVerts; ++v)
    {
      total += edgesContainingVert->getNumberOfElements(v);
    }

    size_t totalBytes = numVerts * sizeof(uint16_t) + total * sizeof(int32_t);

    // Allocate a flat array to copy the data into
    QVector<uint8_t> buffer(totalBytes, 0);
    uint8_t* bufPtr = &(buffer.front());
    size_t offset = 0;

    for(size_t v = 0; v < numVerts; ++v)
    {
      uint16_t ncells = edgesContainingVert->getNumberOfElements(v);
      int64_t* cells = edgesContainingVert->getElementListPointer(v);
      ::memcpy(bufPtr + offset, &ncells, sizeof(uint16_t));
      offset += sizeof(uint16_t);
      ::memcpy(bufPtr + offset, cells, ncells * sizeof(int64_t) );
      offset += ncells * sizeof(int64_t);
    }

    rank = 1;
    dims[0] = totalBytes;

    err = QH5Lite::writePointerDataset(parentId, DREAM3D::StringConstants::EdgesContainingVert, rank, dims, bufPtr);
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
  vertices = IGeometry::ReadMeshFromHDF5<SharedVertexList>(DREAM3D::Geometry::SharedVertexList, parentId, preflight);
  edges = IGeometry::ReadMeshFromHDF5<SharedEdgeList>(DREAM3D::Geometry::SharedEdgeList, parentId, preflight);
  if (true == preflight)
  {
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::EdgeNeighbors, dims, type_class, type_size);
    if(err >= 0)
    {
      UInt16Int64DynamicListArray::Pointer edgeNeighbors = UInt16Int64DynamicListArray::New();
      setEdgeNeighbors(edgeNeighbors);
    }
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::EdgesContainingVert, dims, type_class, type_size);
    if(err >= 0)
    {
      UInt16Int64DynamicListArray::Pointer edgesContainingVert = UInt16Int64DynamicListArray::New();
      setEdgesContainingVert(edgesContainingVert);
    }
    setVertices(vertices);
    setEdges(edges);
  }
  else
  {
    int64_t numEdges = edges->getNumberOfTuples();
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::EdgeNeighbors, dims, type_class, type_size);
    if (err >= 0)
    {
      // Read the edgeNeighbors array into the buffer
      std::vector<uint8_t> buffer;
      err = QH5Lite::readVectorDataset(parentId, DREAM3D::StringConstants::EdgeNeighbors, buffer);
      if(err < 0)
      {
        return err;
      }
      UInt16Int64DynamicListArray::Pointer edgeNeighbors = UInt16Int64DynamicListArray::New();
      edgeNeighbors->deserializeLinks(buffer, numEdges);
      setEdgeNeighbors(edgeNeighbors);
    }
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::EdgesContainingVert, dims, type_class, type_size);
    if (err >= 0)
    {
      // Read the edgesContainingVert array into the buffer
      std::vector<uint8_t> buffer;
      err = QH5Lite::readVectorDataset(parentId, DREAM3D::StringConstants::EdgesContainingVert, buffer);
      if(err < 0)
      {
        return err;
      }
      UInt16Int64DynamicListArray::Pointer edgesContainingVert = UInt16Int64DynamicListArray::New();
      edgesContainingVert->deserializeLinks(buffer, numEdges);
      setEdgesContainingVert(edgesContainingVert);
    }
    setVertices(vertices);
    setEdges(edges);
  }

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// Shared ops includes
#define GEOM_CLASS_NAME EdgeGeom
#include "DREAM3DLib/Geometry/SharedGeometryOps.cpp"
#include "DREAM3DLib/Geometry/SharedVertexOps.cpp"
#include "DREAM3DLib/Geometry/SharedEdgeOps.cpp"
