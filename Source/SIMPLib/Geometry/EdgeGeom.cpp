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
 * EdgeGeom uses code adapated from the following vtk modules:
 *
 * * vtkLine.cxx
 *   - adapted vtkLine::GetParametricCenter to EdgeGeom::getParametricCenter
 *   - adapted vtkLine::InterpolationDerivs to EdgeGeom::getShapeFunctions
 * * vtkGradientFilter.cxx
 *   - adapted vtkGradientFilter template function ComputeCellGradientsUG to
 *     EdgeGeom::findDerivatives
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
#include "SIMPLib/Geometry/EdgeGeom.h"

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
  m_EdgesContainingVert = ElementDynamicList::NullPointer();
  m_EdgeNeighbors = ElementDynamicList::NullPointer();
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
  if (data->getType() == 1 && data->getNumTuples() != getNumberOfElements())
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
size_t EdgeGeom::getNumberOfElements()
{
  return m_EdgeList->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::findElementsContainingVert()
{
  m_EdgesContainingVert = ElementDynamicList::New();
  GeometryHelpers::Connectivity::FindElementsContainingVert<uint16_t, int64_t>(m_EdgeList, m_EdgesContainingVert, getNumberOfVertices());
  if (m_EdgesContainingVert.get() == NULL)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer EdgeGeom::getElementsContainingVert()
{
  return m_EdgesContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert)
{
  m_EdgesContainingVert = elementsContainingVert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::deleteElementsContainingVert()
{
  m_EdgesContainingVert = ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::findElementNeighbors()
{
  int err = 0;
  if (m_EdgesContainingVert.get() == NULL)
  {
    return -1;
  }
  m_EdgeNeighbors = ElementDynamicList::New();
  err = GeometryHelpers::Connectivity::FindElementNeighbors<uint16_t, int64_t>(m_EdgeList, m_EdgesContainingVert, m_EdgeNeighbors);
  if (m_EdgeNeighbors.get() == NULL)
  {
    err = -1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer EdgeGeom::getElementNeighbors()
{
  return m_EdgeNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::setElementNeighbors(ElementDynamicList::Pointer elementNeighbors)
{
  m_EdgeNeighbors = elementNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::deleteElementNeighbors()
{
  m_EdgeNeighbors = ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::findElementCentroids()
{
  QVector<size_t> cDims(1, 3);
  m_EdgeCentroids = FloatArrayType::CreateArray(getNumberOfEdges(), cDims, DREAM3D::StringConstants::EdgeCentroids);
  GeometryHelpers::Topology::FindElementCentroids<int64_t>(m_EdgeList, m_VertexList, m_EdgeCentroids);
  if (m_EdgeCentroids.get() == NULL)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer EdgeGeom::getElementCentroids()
{
  return m_EdgeCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::setElementCentroids(FloatArrayType::Pointer elementCentroids)
{
  m_EdgeCentroids = elementCentroids;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::deleteElementCentroids()
{
  m_EdgeCentroids = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::getParametricCenter(double pCoords[3])
{
  pCoords[0] = 0.5;
  pCoords[1] = 0.0;
  pCoords[2] = 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::getShapeFunctions(double pCoords[3], double* shape)
{
  (void)pCoords;
  shape[0] = -1.0;
  shape[1] = 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeGeom::findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives)
{
  int64_t numEdges = getNumberOfEdges();
  int cDims = field->getNumberOfComponents();
  double* fieldPtr = field->getPointer(0);
  double* derivsPtr = derivatives->getPointer(0);
  double values[2];
  double derivs[3];
  int64_t verts[2];
  for (int64_t i = 0; i < numEdges; i++)
  {
    getVertsAtEdge(i, verts);
    for (int j = 0; j < cDims; j++)
    {
      for (size_t k = 0; k < 2; k++)
      {
        values[k] = fieldPtr[cDims * verts[k] + j];
      }
      DerivativeHelpers::EdgeDeriv()(this, i, values, derivs);
      derivsPtr[i * 3 * cDims + j * 3] = derivs[0];
      derivsPtr[i * 3 * cDims + j * 3 + 1] = derivs[1];
      derivsPtr[i * 3 * cDims + j * 3 + 2] = derivs[2];
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::writeGeometryToHDF5(hid_t parentId, bool SIMPL_NOT_USED(writeXdmf))
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

  if (m_EdgeCentroids.get() != NULL)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_EdgeCentroids);
    if (err < 0)
    {
      return err;
    }
  }

  if (m_EdgeNeighbors.get() != NULL)
  {
    size_t numEdges = getNumberOfEdges();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_EdgeNeighbors, numEdges, DREAM3D::StringConstants::EdgeNeighbors);
    if (err < 0)
    {
      return err;
    }
  }

  if (m_EdgesContainingVert.get() != NULL)
  {
    size_t numVerts = getNumberOfVertices();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_EdgesContainingVert, numVerts, DREAM3D::StringConstants::EdgesContainingVert);
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
QString EdgeGeom::getInfoString(DREAM3D::InfoStringFormat format)
{
  QString info;
  QTextStream ss (&info);

  if(format == DREAM3D::HtmlFormat)
  {
    ss << "<tr bgcolor=\"#D3D8E0\"><th colspan=2>Edge Geometry Info</th></tr>";
    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Number of Edges</th><td>" << getNumberOfEdges() << "</td></tr>";
    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Number of Vertices</th><td>" << getNumberOfVertices() << "</td></tr>";
    ss << "</tbody></table>";
  }
  else
  {

  }
  return info;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  herr_t err = 0;
  SharedVertexList::Pointer vertices = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedVertexList>(DREAM3D::Geometry::SharedVertexList, parentId, preflight, err);
  SharedEdgeList::Pointer edges = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedEdgeList>(DREAM3D::Geometry::SharedEdgeList, parentId, preflight, err);
  if (edges.get() == NULL || vertices.get() == NULL)
  {
    return -1;
  }
  size_t numEdges = edges->getNumberOfTuples();
  size_t numVerts = vertices->getNumberOfTuples();
  FloatArrayType::Pointer edgeCentroids = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(DREAM3D::StringConstants::EdgeCentroids, parentId, preflight, err);
  if (err < 0 && err != -2)
  {
    return -1;
  }
  ElementDynamicList::Pointer edgeNeighbors = GeometryHelpers::GeomIO::ReadDynamicListFromHDF5<uint16_t, int64_t>(DREAM3D::StringConstants::EdgeNeighbors, parentId, numEdges, preflight, err);
  if (err < 0 && err != -2)
  {
    return -1;
  }
  ElementDynamicList::Pointer edgesContainingVert = GeometryHelpers::GeomIO::ReadDynamicListFromHDF5<uint16_t, int64_t>(DREAM3D::StringConstants::EdgesContainingVert, parentId, numVerts, preflight, err);
  if (err < 0 && err != -2)
  {
    return -1;
  }

  setVertices(vertices);
  setEdges(edges);
  setElementCentroids(edgeCentroids);
  setElementNeighbors(edgeNeighbors);
  setElementsContainingVert(edgesContainingVert);

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer EdgeGeom::deepCopy()
{
  EdgeGeom::Pointer edgeCopy = EdgeGeom::CreateGeometry(getEdges(), getVertices(), getName());

  edgeCopy->setElementsContainingVert(getElementsContainingVert());
  edgeCopy->setElementNeighbors(getElementNeighbors());
  edgeCopy->setElementCentroids(getElementCentroids());
  edgeCopy->setSpatialDimensionality(getSpatialDimensionality());

  return edgeCopy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// Shared ops includes
#define GEOM_CLASS_NAME EdgeGeom
#include "SIMPLib/Geometry/SharedGeometryOps.cpp"
#include "SIMPLib/Geometry/SharedVertexOps.cpp"
#include "SIMPLib/Geometry/SharedEdgeOps.cpp"
