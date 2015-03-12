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
 * Redisytibutions in binary form must reproduce the above copyright notice, this
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

/* ============================================================================
 * QuadGeom uses code adapated from the following vtk modules:
 *
 * * vtkQuad.cxx
 *   - adapted vtkQuad::GetParametricCenter to QuadGeom::getParametricCenter
 *   - adapted vtkQuad::InterpolationDerivs to QuadGeom::getShapeFunctions
 * * vtkGradientFilter.cxx
 *   - adapted vtkGradientFilter template function ComputeCellGradientsUG to
 *     QuadGeom::findDerivatives
 *
 * The vtk license is reproduced below.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ============================================================================
 * Copyright (c) 1993-2008 Ken Martin, Will Schroeder, Bill Lorensen
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * * Neither name of Ken Martin, Will Schroeder, or Bill Lorensen nor the names of
 * any contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
  m_QuadsContainingVert = ElementDynamicList::NullPointer();
  m_QuadNeighbors = ElementDynamicList::NullPointer();
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
  SharedQuadList::Pointer quads = QuadGeom::CreateSharedQuadList(numQuads);
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
SharedEdgeList::Pointer QuadGeom::findUniqueEdges()
{
  int64_t numQuads = getNumberOfQuads();
  int64_t verts[4];
  int64_t v0 = 0;
  int64_t v1 = 0;

  QMap<int64_t, int64_t> edgeMap;
  QMap<int64_t, int64_t>::Iterator it;

  for (size_t t = 0; t < numQuads; t++)
  {
    getVertsAtQuad(t, verts);

    // edge 0
    int i = 0;
    if (verts[i] > verts[i+1]) { v0 = verts[i+1]; v1 = verts[i]; }
    else { v0 = verts[i]; v1 = verts[i+1]; }
    it = edgeMap.find(v0);
    if (it == edgeMap.end() || it.value() != v1) { edgeMap.insert(v0,v1); }

    // edge 1
    i = 1;
    if (verts[i] > verts[i+1]) { v0 = verts[i+1]; v1 = verts[i]; }
    else { v0 = verts[i]; v1 = verts[i+1]; }
    it = edgeMap.find(v0);
    if (it == edgeMap.end() || it.value() != v1) { edgeMap.insert(v0,v1); }

    // edge 2
    i = 2;
    if (verts[i] > verts[i+1]) { v0 = verts[i+1]; v1 = verts[i]; }
    else { v0 = verts[i]; v1 = verts[i+1]; }
    it = edgeMap.find(v0);
    if (it == edgeMap.end() || it.value() != v1) { edgeMap.insert(v0,v1); }

    // edge 3
    i = 3;
    if (verts[i] > verts[0]) { v0 = verts[0]; v1 = verts[i]; }
    else { v0 = verts[i]; v1 = verts[0]; }
    it = edgeMap.find(v0);
    if (it == edgeMap.end() || it.value() != v1) { edgeMap.insert(v0,v1); }
  }

  SharedEdgeList::Pointer uniqueEdges = CreateSharedEdgeList(edgeMap.size());
  int64_t* uEdges = uniqueEdges->getPointer(0);

  int64_t index = 0;

  qDebug() << edgeMap.size();

  for (it = edgeMap.begin(); it != edgeMap.end(); it++)
  {
    qDebug() << index << " " << it.key() << " " << it.value();
    ++index;
  }

  return uniqueEdges;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::initializeWithZeros()
{
  m_VertexList->initializeWithZeros();
  m_QuadList->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::addAttributeMatrix(const QString& name, AttributeMatrix::Pointer data)
{
  if (data->getType() != 0 || data->getType() != 1 || data->getType() != 2)
  {
    // QuadGeom can only accept vertex, edge, or face Attribute Matrices
    return;
  }
  if (data->getType() == 0 && static_cast<int64_t>(data->getNumTuples()) != getNumberOfVertices())
  {
    return;
  }
  if (data->getType() == 1 && static_cast<int64_t>(data->getNumTuples()) != getNumberOfEdges())
  {
    return;
  }
  if (data->getType() == 2 && data->getNumTuples() != getNumberOfTuples())
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
size_t QuadGeom::getNumberOfTuples()
{
  return m_QuadList->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuadGeom::findElementsContainingVert()
{
  m_QuadsContainingVert = ElementDynamicList::New();
  GeometryHelpers::Connectivity::FindElementsContainingVert<uint16_t, int64_t>(m_QuadList, m_QuadsContainingVert, getNumberOfVertices());
  if (m_QuadsContainingVert.get() == NULL)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer QuadGeom::getElementsContainingVert()
{
  return m_QuadsContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert)
{
  m_QuadsContainingVert = elementsContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::deleteElementsContainingVert()
{
  m_QuadsContainingVert = ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuadGeom::findElementNeighbors()
{
  int err = 0;
  if (m_QuadsContainingVert.get() == NULL)
  {
    return -1;
  }
  m_QuadNeighbors = ElementDynamicList::New();
  err = GeometryHelpers::Connectivity::FindElementNeighbors<uint16_t, int64_t>(m_QuadList, m_QuadsContainingVert, m_QuadNeighbors);
  if (m_QuadNeighbors.get() == NULL)
  {
    return -1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer QuadGeom::getElementNeighbors()
{
  return m_QuadNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::setElementNeighbors(ElementDynamicList::Pointer elementNeighbors)
{
  m_QuadNeighbors = elementNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::deleteElementNeighbors()
{
  m_QuadNeighbors = ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuadGeom::findElementCentroids()
{
  QVector<size_t> cDims(1, 3);
  m_QuadCentroids = FloatArrayType::CreateArray(getNumberOfQuads(), cDims, DREAM3D::StringConstants::QuadCentroids);
  GeometryHelpers::Topology::FindElementCentroids<int64_t>(m_QuadList, m_VertexList, m_QuadCentroids);
  if (m_QuadCentroids.get() == NULL)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer QuadGeom::getElementCentroids()
{
  return m_QuadCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::setElementCentroids(FloatArrayType::Pointer elementCentroids)
{
  m_QuadCentroids = elementCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::deleteElementCentroids()
{
  m_QuadCentroids = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::getParametricCenter(double pCoords[3])
{
  pCoords[0] = 0.5;
  pCoords[1] = 0.5;
  pCoords[2] = 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::getShapeFunctions(double pCoords[3], double* shape)
{
  double rm, sm;

  rm = 1.0 - pCoords[0];
  sm = 1.0 - pCoords[1];

  shape[0] = -sm;
  shape[1] = sm;
  shape[2] = pCoords[1];
  shape[3] = -pCoords[1];
  shape[4] = -rm;
  shape[5] = -pCoords[0];
  shape[6] = pCoords[0];
  shape[7] = rm;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives)
{
  int64_t numQuads = getNumberOfQuads();
  int cDims = field->getNumberOfComponents();
  double* fieldPtr = field->getPointer(0);
  double* derivsPtr = derivatives->getPointer(0);
  double values[4];
  double derivs[3];
  int64_t verts[4];
  for (int64_t i = 0; i < numQuads; i++)
  {
    getVertsAtQuad(i, verts);
    for (int j = 0; j < cDims; j++)
    {
      for (size_t k = 0; k < 4; k++)
      {
        values[k] = fieldPtr[cDims*verts[k]+j];
      }
      DerivativeHelpers::QuadDeriv()(this, i, values, derivs);
      derivsPtr[i*3*cDims+j*3] = derivs[0];
      derivsPtr[i*3*cDims+j*3+1] = derivs[1];
      derivsPtr[i*3*cDims+j*3+2] = derivs[2];
    }
  }
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
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_QuadNeighbors, numQuads, DREAM3D::StringConstants::QuadNeighbors);
    if (err < 0)
    {
      return err;
    }
  }

  // Last write faces containing verts if they exist
  if (m_QuadsContainingVert.get() != NULL)
  {
    size_t numVerts = getNumberOfVertices();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_QuadsContainingVert, numVerts, DREAM3D::StringConstants::QuadsContainingVert);
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
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << getNumberOfQuads() << " 4\">" << "\n";
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
  if (preflight == true)
  {
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::QuadNeighbors, dims, type_class, type_size);
    if (err >= 0)
    {
      ElementDynamicList::Pointer quadNeighbors = ElementDynamicList::New();
      m_QuadNeighbors = quadNeighbors;
    }
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::QuadsContainingVert, dims, type_class, type_size);
    if (err >= 0)
    {
      ElementDynamicList::Pointer quadsContainingVert = ElementDynamicList::New();
      m_QuadsContainingVert = quadsContainingVert;
    }
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::QuadCentroids, dims, type_class, type_size);
    if (err >= 0)
    {
      m_QuadCentroids = quadCentroids;
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
      ElementDynamicList::Pointer quadNeighbors = ElementDynamicList::New();
      quadNeighbors->deserializeLinks(buffer, numQuads);
      m_QuadNeighbors = quadNeighbors;
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
      ElementDynamicList::Pointer quadsContainingVert = ElementDynamicList::New();
      quadsContainingVert->deserializeLinks(buffer, numQuads);
      m_QuadsContainingVert = quadsContainingVert;
    }
    err = quadCentroids->readH5Data(parentId);
    if (err >= 0)
    {
      m_QuadCentroids = quadCentroids;
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
IGeometry::Pointer QuadGeom::deepCopy()
{
  QuadGeom::Pointer quadCopy = QuadGeom::CreateGeometry(getQuads(), getVertices(), getName());

  quadCopy->setElementsContainingVert(getElementsContainingVert());
  quadCopy->setElementNeighbors(getElementNeighbors());
  quadCopy->setElementCentroids(getElementCentroids());
  quadCopy->setSpatialDimensionality(getSpatialDimensionality());

  return quadCopy;
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
