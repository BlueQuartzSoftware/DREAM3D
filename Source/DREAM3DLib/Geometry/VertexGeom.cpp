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

#include "DREAM3DLib/Geometry/VertexGeom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexGeom::VertexGeom()
{
  m_GeometryTypeName = DREAM3D::Geometry::VertexGeometry;
  m_GeometryType = DREAM3D::GeometryType::VertexGeometry;
  m_XdmfGridType = DREAM3D::XdmfGridType::PolyData;
  m_UnitDimensionality = 0;
  m_SpatialDimensionality = 3;
  m_VertexList = VertexGeom::CreateSharedVertexList(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexGeom::~VertexGeom()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexGeom::Pointer VertexGeom::CreateGeometry(int64_t numVertices, const QString &name)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  SharedVertexList::Pointer vertices = VertexGeom::CreateSharedVertexList(numVertices);
  VertexGeom* d = new VertexGeom();
  d->setVertices(vertices);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexGeom::Pointer VertexGeom::CreateGeometry(SharedVertexList::Pointer vertices, const QString& name)
{
  if (name.isEmpty() == true)
  {
    return VertexGeom::NullPointer();
  }
  if (vertices.get() == NULL)
  {
    return VertexGeom::NullPointer();
  }
  VertexGeom* d = new VertexGeom();
  d->setVertices(vertices);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::initializeWithZeros()
{
  m_VertexList->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::addAttributeMatrix(const QString& name, AttributeMatrix::Pointer data)
{
  if (data->getType() != 0)
  {
    // VertexGeom can only accept vertex Attribute Matrices
    return;
  }
  if (data->getNumTuples() != getNumberOfTuples())
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
size_t VertexGeom::getNumberOfTuples()
{
  return m_VertexList->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexGeom::findElementsContainingVert()
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer VertexGeom::getElementsContainingVert()
{
  return ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::setElementsContainingVert(ElementDynamicList::Pointer elementsContainingVert)
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::deleteElementsContainingVert()
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexGeom::findElementNeighbors()
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer VertexGeom::getElementNeighbors()
{
  return ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::setElementNeighbors(ElementDynamicList::Pointer elementNeighbors)
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::deleteElementNeighbors()
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexGeom::findElementCentroids()
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer VertexGeom::getElementCentroids()
{
  return FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::setElementCentroids(FloatArrayType::Pointer elementCentroids)
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::deleteElementCentroids()
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::getParametricCenter(double pCoords[3])
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::getShapeFunctions(double pCoords[3], double shape[8])
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexGeom::findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives)
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexGeom::writeGeometryToHDF5(hid_t parentId, bool writeXdmf)
{
  herr_t err = 0;
  QVector<size_t> tDims(1, 0);

  if (m_VertexList.get() != NULL)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_VertexList);
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

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexGeom::writeXdmf(QTextStream& out, QString dcName, QString hdfFileName)
{
  herr_t err = 0;

  // Always start the grid
  out << "  <!-- *************** START OF " << dcName << " *************** -->" << "\n";
  out << "  <Grid Name=\"" << dcName << "\" GridType=\"Uniform\">" << "\n";

#if 0
  DataArrayPath dap = getTemporalDataPath();
  if(dap.isValid())
  {
    IDataArray::Pointer timeValues = getAttributeMatrix(dap.getAttributeMatrixName())->getAttributeArray(dap.getDataArrayName());
    Int32ArrayType::Pointer timeValuePtr = boost::dynamic_pointer_cast<Int32ArrayType>(timeValues);
    out << "    <Time TimeType=\"Single\" Value=\"" << timeValuePtr->getValue(0) << "\"/>\n";
  }
#endif

  out << "    <Topology TopologyType=\"Polyvertex\" NumberOfElements=\"" << getNumberOfVertices() << "\">" << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << getNumberOfVertices() << "\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << dcName << "/" << DREAM3D::Geometry::Geometry << "/" << "Verts" << "\n";
  out << "      </DataItem>" << "\n";
  out << "    </Topology>" << "\n";

  out << "    <Geometry Type=\"XYZ\">" << "\n";
  out << "      <DataItem Format=\"HDF\"  Dimensions=\"" << getNumberOfVertices() << " 3\" NumberType=\"Float\" Precision=\"4\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << dcName << "/" << DREAM3D::Geometry::Geometry << "/" << DREAM3D::Geometry::SharedVertexList << "\n";
  out << "      </DataItem>" << "\n";
  out << "    </Geometry>" << "\n";
  out << "" << "\n";

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  SharedVertexList::Pointer vertices = SharedVertexList::NullPointer();
  vertices = GeometryHelpers::GeomIO::ReadMeshFromHDF5<SharedVertexList>(DREAM3D::Geometry::SharedVertexList, parentId, preflight);
  if (vertices.get() == NULL)
  {
    return -1;
  }
  setVertices(vertices);

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer VertexGeom::deepCopy()
{
  VertexGeom::Pointer vertexCopy = VertexGeom::CreateGeometry(getVertices(), getName());

  vertexCopy->setSpatialDimensionality(getSpatialDimensionality());

  return vertexCopy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// Shared ops includes
#define GEOM_CLASS_NAME VertexGeom
#include "DREAM3DLib/Geometry/SharedGeometryOps.cpp"
#include "DREAM3DLib/Geometry/SharedVertexOps.cpp"

