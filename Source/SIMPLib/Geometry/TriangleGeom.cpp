/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* ============================================================================
 * TriangleGeom uses code adapated from the following vtk modules:
 *
 * * vtkTriangle.cxx
 *   - adapted vtkTriangle::GetParametricCenter to TriangleGeom::getParametricCenter
 *   - adapted vtkTriangle::InterpolationDerivs to TriangleGeom::getShapeFunctions
 * * vtkGradientFilter.cxx
 *   - adapted vtkGradientFilter template function ComputeCellGradientsUG to
 *     TriangleGeom::findDerivatives
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
#include "SIMPLib/Geometry/TriangleGeom.h"

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
  m_UnsharedEdgeList = SharedEdgeList::NullPointer();
  m_TrianglesContainingVert = ElementDynamicList::NullPointer();
  m_TriangleNeighbors = ElementDynamicList::NullPointer();
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
  SharedTriList::Pointer triangles = TriangleGeom::CreateSharedTriList(numTriangles);
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
  m_TriList->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::addAttributeMatrix(const QString& name, AttributeMatrix::Pointer data)
{
  if (data->getType() != 0 || data->getType() != 1 || data->getType() != 2)
  {
    // TriangleGeom can only accept vertex, edge, or face Attribute Matrices
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
  if (data->getType() == 2 && data->getNumTuples() != getNumberOfElements())
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
size_t TriangleGeom::getNumberOfElements()
{
  return m_TriList->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::findEdges()
{
  m_EdgeList = CreateSharedEdgeList(0);
  GeometryHelpers::Connectivity::Find2DElementEdges<int64_t>(m_TriList, m_EdgeList);
  if (m_EdgeList.get() == NULL)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::deleteEdges()
{
  m_EdgeList = SharedEdgeList::NullPointer();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::findElementsContainingVert()
{
  m_TrianglesContainingVert = ElementDynamicList::New();
  GeometryHelpers::Connectivity::FindElementsContainingVert<uint16_t, int64_t>(m_TriList, m_TrianglesContainingVert, getNumberOfVertices());
  if (m_TrianglesContainingVert.get() == NULL)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer TriangleGeom::getElementsContainingVert()
{
  return m_TrianglesContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert)
{
  m_TrianglesContainingVert = elementsContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::deleteElementsContainingVert()
{
  m_TrianglesContainingVert = ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::findElementNeighbors()
{
  int err = 0;
  if (m_TrianglesContainingVert.get() == NULL)
  {
    return -1;
  }
  m_TriangleNeighbors = ElementDynamicList::New();
  err = GeometryHelpers::Connectivity::FindElementNeighbors<uint16_t, int64_t>(m_TriList, m_TrianglesContainingVert, m_TriangleNeighbors);
  if (m_TriangleNeighbors.get() == NULL)
  {
    return -1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer TriangleGeom::getElementNeighbors()
{
  return m_TriangleNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::setElementNeighbors(ElementDynamicList::Pointer elementNeighbors)
{
  m_TriangleNeighbors = elementNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::deleteElementNeighbors()
{
  m_TriangleNeighbors = ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::findElementCentroids()
{
  QVector<size_t> cDims(1, 3);
  m_TriangleCentroids = FloatArrayType::CreateArray(getNumberOfTris(), cDims, DREAM3D::StringConstants::TriangleCentroids);
  GeometryHelpers::Topology::FindElementCentroids<int64_t>(m_TriList, m_VertexList, m_TriangleCentroids);
  if (m_TriangleCentroids.get() == NULL)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer TriangleGeom::getElementCentroids()
{
  return m_TriangleCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::setElementCentroids(FloatArrayType::Pointer elementCentroids)
{
  m_TriangleCentroids = elementCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::deleteElementCentroids()
{
  m_TriangleCentroids = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::findUnsharedEdges()
{
  QVector<size_t> cDims(1, 2);
  m_UnsharedEdgeList = SharedEdgeList::CreateArray(0, cDims, DREAM3D::Geometry::UnsharedEdgeList);
  GeometryHelpers::Connectivity::Find2DUnsharedEdges<int64_t>(m_TriList, m_UnsharedEdgeList);
  if (m_UnsharedEdgeList.get() == NULL)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedEdgeList::Pointer TriangleGeom::getUnsharedEdges()
{
  return m_UnsharedEdgeList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::setUnsharedEdges(SharedEdgeList::Pointer bEdgeList)
{
  m_UnsharedEdgeList = bEdgeList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::deleteUnsharedEdges()
{
  m_UnsharedEdgeList = SharedEdgeList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::getParametricCenter(double pCoords[3])
{
  pCoords[0] = 1. / 3;
  pCoords[1] = 1. / 3;
  pCoords[2] = 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::getShapeFunctions(double pCoords[3], double* shape)
{
  (void)pCoords;

  // r derivatives
  shape[0] = -1.0;
  shape[1] = 1.0;
  shape[2] = 0.0;

  // s derivatives
  shape[3] = -1.0;
  shape[4] = 0.0;
  shape[5] = 1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives)
{
  int64_t numTris = getNumberOfTris();
  int cDims = field->getNumberOfComponents();
  double* fieldPtr = field->getPointer(0);
  double* derivsPtr = derivatives->getPointer(0);
  double values[3];
  double derivs[3];
  int64_t verts[3];
  for (int64_t i = 0; i < numTris; i++)
  {
    getVertsAtTri(i, verts);
    for (int j = 0; j < cDims; j++)
    {
      for (size_t k = 0; k < 3; k++)
      {
        values[k] = fieldPtr[cDims * verts[k] + j];
      }
      DerivativeHelpers::TriangleDeriv()(this, i, values, derivs);
      derivsPtr[i * 3 * cDims + j * 3] = derivs[0];
      derivsPtr[i * 3 * cDims + j * 3 + 1] = derivs[1];
      derivsPtr[i * 3 * cDims + j * 3 + 2] = derivs[2];
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::writeGeometryToHDF5(hid_t parentId, bool SIMPL_NOT_USED(writeXdmf))
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

  if (m_UnsharedEdgeList.get() != NULL)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_UnsharedEdgeList);
    if (err < 0)
    {
      return err;
    }
  }

  if (m_TriangleCentroids.get() != NULL)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_TriangleCentroids);
    if (err < 0)
    {
      return err;
    }
  }

  if (m_TriangleNeighbors.get() != NULL)
  {
    size_t numTris = getNumberOfTris();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_TriangleNeighbors, numTris, DREAM3D::StringConstants::TriangleNeighbors);
    if (err < 0)
    {
      return err;
    }
  }

  if (m_TrianglesContainingVert.get() != NULL)
  {
    size_t numVerts = getNumberOfVertices();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_TrianglesContainingVert, numVerts, DREAM3D::StringConstants::TrianglesContainingVert);
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
QString TriangleGeom::getInfoString(DREAM3D::InfoStringFormat format)
{
  QString info;
  QTextStream ss (&info);

  if(format == DREAM3D::HtmlFormat)
  {
    ss << "<tr bgcolor=\"#D3D8E0\"><th colspan=2>Triangle Geometry Info</th></tr>";
    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Number of Triangles</th><td>" << getNumberOfTris() << "</td></tr>";
    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Number of Vertices</th><td>" << getNumberOfVertices() << "</td></tr>";
  }
  else
  {

  }
  return info;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  herr_t err = 0;
  SharedVertexList::Pointer vertices = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedVertexList>(DREAM3D::Geometry::SharedVertexList, parentId, preflight, err);
  SharedTriList::Pointer tris = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedQuadList>(DREAM3D::Geometry::SharedTriList, parentId, preflight, err);
  if (tris.get() == NULL || vertices.get() == NULL)
  {
    return -1;
  }
  size_t numTris = tris->getNumberOfTuples();
  size_t numVerts = vertices->getNumberOfTuples();
  SharedEdgeList::Pointer edges = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedEdgeList>(DREAM3D::Geometry::SharedEdgeList, parentId, preflight, err);
  if (err < 0 && err != -2)
  {
    return -1;
  }
  SharedEdgeList::Pointer bEdges = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedEdgeList>(DREAM3D::Geometry::UnsharedEdgeList, parentId, preflight, err);
  if (err < 0 && err != -2)
  {
    return -1;
  }
  FloatArrayType::Pointer triCentroids = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(DREAM3D::StringConstants::TriangleCentroids, parentId, preflight, err);
  if (err < 0 && err != -2)
  {
    return -1;
  }
  ElementDynamicList::Pointer triNeighbors = GeometryHelpers::GeomIO::ReadDynamicListFromHDF5<uint16_t, int64_t>(DREAM3D::StringConstants::TriangleNeighbors, parentId, numTris, preflight, err);
  if (err < 0 && err != -2)
  {
    return -1;
  }
  ElementDynamicList::Pointer trisContainingVert = GeometryHelpers::GeomIO::ReadDynamicListFromHDF5<uint16_t, int64_t>(DREAM3D::StringConstants::TrianglesContainingVert, parentId, numVerts, preflight, err);
  if (err < 0 && err != -2)
  {
    return -1;
  }

  setVertices(vertices);
  setEdges(edges);
  setUnsharedEdges(bEdges);
  setTriangles(tris);
  setElementCentroids(triCentroids);
  setElementNeighbors(triNeighbors);
  setElementsContainingVert(trisContainingVert);

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer TriangleGeom::deepCopy()
{
  TriangleGeom::Pointer triCopy = TriangleGeom::CreateGeometry(getTriangles(), getVertices(), getName());

  triCopy->setEdges(getEdges());
  triCopy->setUnsharedEdges(getUnsharedEdges());
  triCopy->setElementsContainingVert(getElementsContainingVert());
  triCopy->setElementNeighbors(getElementNeighbors());
  triCopy->setElementCentroids(getElementCentroids());
  triCopy->setSpatialDimensionality(getSpatialDimensionality());

  return triCopy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// Shared ops includes
#define GEOM_CLASS_NAME TriangleGeom
#include "SIMPLib/Geometry/SharedGeometryOps.cpp"
#include "SIMPLib/Geometry/SharedVertexOps.cpp"
#include "SIMPLib/Geometry/SharedEdgeOps.cpp"
#include "SIMPLib/Geometry/SharedTriOps.cpp"
