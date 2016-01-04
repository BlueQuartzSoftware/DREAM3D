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
 * QuadGeom re-implements code from the following vtk modules:
 *
 * * vtkQuad.cxx
 *   - re-implemented vtkQuad::GetParametricCenter to QuadGeom::getParametricCenter
 *   - re-implemented vtkQuad::InterpolationDerivs to QuadGeom::getShapeFunctions
 * * vtkGradientFilter.cxx
 *   - re-implemented vtkGradientFilter template function ComputeCellGradientsUG to
 *     QuadGeom::findDerivatives
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SIMPLib/Geometry/QuadGeom.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Geometry/GeometryHelpers.hpp"
#include "SIMPLib/Geometry/DerivativeHelpers.h"

/**
 * @brief The FindQuadDerivativesImpl class implements a threaded algorithm that computes the
 * derivative of an arbitrary dimensional field on the underlying quadrilaterals
 */
class FindQuadDerivativesImpl
{
  public:
    FindQuadDerivativesImpl(QuadGeom* quads, DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivs) :
      m_Quads(quads),
      m_Field(field),
      m_Derivatives(derivs)
    {}
    virtual ~FindQuadDerivativesImpl() {}

    void compute(int64_t start, int64_t end) const
    {
      int32_t cDims = m_Field->getNumberOfComponents();
      double* fieldPtr = m_Field->getPointer(0);
      double* derivsPtr = m_Derivatives->getPointer(0);
      double values[4] = { 0.0, 0.0, 0.0, 0.0 };
      double derivs[3] = { 0.0, 0.0, 0.0 };
      int64_t verts[4] = { 0, 0, 0, 0 };

      int64_t counter = 0;
      int64_t totalElements = m_Quads->getNumberOfQuads();
      int64_t progIncrement = static_cast<int64_t>(totalElements / 100);

      for (int64_t i = start; i < end; i++)
      {
        m_Quads->getVertsAtQuad(i, verts);
        for (int32_t j = 0; j < cDims; j++)
        {
          for (size_t k = 0; k < 4; k++)
          {
            values[k] = fieldPtr[cDims * verts[k] + j];
          }
          DerivativeHelpers::QuadDeriv()(m_Quads, i, values, derivs);
          derivsPtr[i * 3 * cDims + j * 3] = derivs[0];
          derivsPtr[i * 3 * cDims + j * 3 + 1] = derivs[1];
          derivsPtr[i * 3 * cDims + j * 3 + 2] = derivs[2];
        }

        if (counter > progIncrement)
        {
          m_Quads->sendThreadSafeProgressMessage(counter, totalElements);
          counter = 0;
        }
        counter++;
      }
    }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<int64_t>& r) const
    {
      compute(r.begin(), r.end());
    }
#endif
  private:
    QuadGeom* m_Quads;
    DoubleArrayType::Pointer m_Field;
    DoubleArrayType::Pointer m_Derivatives;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuadGeom::QuadGeom()
{
  m_GeometryTypeName = DREAM3D::Geometry::QuadGeometry;
  m_GeometryType = DREAM3D::GeometryType::QuadGeometry;
  m_XdmfGridType = DREAM3D::XdmfGridType::PolyData;
  m_MessagePrefix = "";
  m_MessageTitle = "";
  m_MessageLabel = "";
  m_UnitDimensionality = 2;
  m_SpatialDimensionality = 3;
  m_VertexList = QuadGeom::CreateSharedVertexList(0);
  m_QuadList = QuadGeom::CreateSharedQuadList(0);
  m_EdgeList = SharedEdgeList::NullPointer();
  m_UnsharedEdgeList = SharedEdgeList::NullPointer();
  m_QuadsContainingVert = ElementDynamicList::NullPointer();
  m_QuadNeighbors = ElementDynamicList::NullPointer();
  m_QuadCentroids = FloatArrayType::NullPointer();
  m_ProgressCounter = 0;
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
size_t QuadGeom::getNumberOfElements()
{
  return m_QuadList->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuadGeom::findEdges()
{
  m_EdgeList = CreateSharedEdgeList(0);
  GeometryHelpers::Connectivity::Find2DElementEdges<int64_t>(m_QuadList, m_EdgeList);
  if (m_EdgeList.get() == NULL)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::deleteEdges()
{
  m_EdgeList = SharedEdgeList::NullPointer();
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
int QuadGeom::findUnsharedEdges()
{
  QVector<size_t> cDims(1, 2);
  m_UnsharedEdgeList = SharedEdgeList::CreateArray(0, cDims, DREAM3D::Geometry::UnsharedEdgeList);
  GeometryHelpers::Connectivity::Find2DUnsharedEdges<int64_t>(m_QuadList, m_UnsharedEdgeList);
  if (m_UnsharedEdgeList.get() == NULL)
  {
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedEdgeList::Pointer QuadGeom::getUnsharedEdges()
{
  return m_UnsharedEdgeList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::setUnsharedEdges(SharedEdgeList::Pointer bEdgeList)
{
  m_UnsharedEdgeList = bEdgeList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuadGeom::deleteUnsharedEdges()
{
  m_UnsharedEdgeList = SharedEdgeList::NullPointer();
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
  double rm = 0.0;
  double sm = 0.0;

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
void QuadGeom::findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives, Observable* observable)
{
  m_ProgressCounter = 0;
  int64_t numQuads = getNumberOfQuads();

  if (observable)
  {
    connect(this, SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
            observable, SLOT(broadcastPipelineMessage(const PipelineMessage&)));
  }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<int64_t>(0, numQuads),
                      FindQuadDerivativesImpl(this, field, derivatives), tbb::auto_partitioner());
  }
  else
#endif
  {
    FindQuadDerivativesImpl serial(this, field, derivatives);
    serial.compute(0, numQuads);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuadGeom::writeGeometryToHDF5(hid_t parentId, bool SIMPL_NOT_USED(writeXdmf))
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

  if (m_UnsharedEdgeList.get() != NULL)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_UnsharedEdgeList);
    if (err < 0)
    {
      return err;
    }
  }

  if (m_QuadCentroids.get() != NULL)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_QuadCentroids);
    if (err < 0)
    {
      return err;
    }
  }

  if (m_QuadNeighbors.get() != NULL)
  {
    size_t numQuads = getNumberOfQuads();
    err = GeometryHelpers::GeomIO::WriteDynamicListToHDF5<uint16_t, int64_t>(parentId, m_QuadNeighbors, numQuads, DREAM3D::StringConstants::QuadNeighbors);
    if (err < 0)
    {
      return err;
    }
  }

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
QString QuadGeom::getInfoString(DREAM3D::InfoStringFormat format)
{
  QString info;
  QTextStream ss (&info);

  if (format == DREAM3D::HtmlFormat)
  {
    ss << "<tr bgcolor=\"#D3D8E0\"><th colspan=2>Quad Geometry Info</th></tr>";
    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Number of Quads</th><td>" << getNumberOfQuads() << "</td></tr>";
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
int QuadGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  herr_t err = 0;
  SharedVertexList::Pointer vertices = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedVertexList>(DREAM3D::Geometry::SharedVertexList, parentId, preflight, err);
  SharedQuadList::Pointer quads = GeometryHelpers::GeomIO::ReadListFromHDF5<SharedQuadList>(DREAM3D::Geometry::SharedQuadList, parentId, preflight, err);
  if (quads.get() == NULL || vertices.get() == NULL)
  {
    return -1;
  }
  size_t numQuads = quads->getNumberOfTuples();
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
  FloatArrayType::Pointer quadCentroids = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(DREAM3D::StringConstants::QuadCentroids, parentId, preflight, err);
  if (err < 0 && err != -2)
  {
    return -1;
  }
  ElementDynamicList::Pointer quadNeighbors = GeometryHelpers::GeomIO::ReadDynamicListFromHDF5<uint16_t, int64_t>(DREAM3D::StringConstants::QuadNeighbors, parentId, numQuads, preflight, err);
  if (err < 0 && err != -2)
  {
    return -1;
  }
  ElementDynamicList::Pointer quadsContainingVert = GeometryHelpers::GeomIO::ReadDynamicListFromHDF5<uint16_t, int64_t>(DREAM3D::StringConstants::QuadsContainingVert, parentId, numVerts, preflight, err);
  if (err < 0 && err != -2)
  {
    return -1;
  }

  setVertices(vertices);
  setEdges(edges);
  setUnsharedEdges(bEdges);
  setQuads(quads);
  setElementCentroids(quadCentroids);
  setElementNeighbors(quadNeighbors);
  setElementsContainingVert(quadsContainingVert);

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer QuadGeom::deepCopy()
{
  QuadGeom::Pointer quadCopy = QuadGeom::CreateGeometry(getQuads(), getVertices(), getName());

  quadCopy->setEdges(getEdges());
  quadCopy->setUnsharedEdges(getUnsharedEdges());
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
#include "SIMPLib/Geometry/SharedGeometryOps.cpp"
#include "SIMPLib/Geometry/SharedVertexOps.cpp"
#include "SIMPLib/Geometry/SharedEdgeOps.cpp"
#include "SIMPLib/Geometry/SharedQuadOps.cpp"
