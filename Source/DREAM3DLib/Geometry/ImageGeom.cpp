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

#include "DREAM3DLib/Geometry/ImageGeom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageGeom::ImageGeom()
{
  m_GeometryTypeName = DREAM3D::Geometry::ImageGeometry;
  m_GeometryType = DREAM3D::GeometryType::ImageGeometry;
  m_XdmfGridType = DREAM3D::XdmfGridType::RectilinearGrid;
  m_UnitDimensionality = 3;
  m_SpatialDimensionality = 3;
  m_Dimensions[0] = 0;
  m_Dimensions[1] = 0;
  m_Dimensions[2] = 0;
  m_Resolution[0] = 1.0f;
  m_Resolution[1] = 1.0f;
  m_Resolution[2] = 1.0f;
  m_Origin[0] = 0.0f;
  m_Origin[1] = 0.0f;
  m_Origin[2] = 0.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageGeom::~ImageGeom()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageGeom::Pointer ImageGeom::CreateGeometry(const QString& name)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  ImageGeom* d = new ImageGeom();
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageGeom::writeGeometryToHDF5(hid_t parentId, bool writeXdmf)
{
  herr_t err = 0;
  int64_t volDims[3] =
  { static_cast<int64_t>(getXPoints()), static_cast<int64_t>(getYPoints()), static_cast<int64_t>(getZPoints()) };
  float spacing[3] =
  { getXRes(), getYRes(), getZRes() };
  float origin[3] =
  { 0.0f, 0.0f, 0.0f };
  getOrigin(origin);

  int32_t rank = 1;
  hsize_t dims[1] = {3};

  err = H5Lite::writePointerDataset(parentId, H5_DIMENSIONS, rank, dims, volDims);
  if (err < 0)
  {
    return err;
  }
  err = H5Lite::writePointerDataset(parentId, H5_ORIGIN, rank, dims, origin);
  if (err < 0)
  {
    return err;
  }
  err = H5Lite::writePointerDataset(parentId, H5_SPACING, rank, dims, spacing);
  if (err < 0)
  {
    return err;
  }

  err |= H5Gclose(parentId);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageGeom::writeXdmf(QTextStream &out, QString dcName, QString hdfFileName)
{
  herr_t err = 0;

  int64_t volDims[3] =
  { static_cast<int64_t>(getXPoints()), static_cast<int64_t>(getYPoints()), static_cast<int64_t>(getZPoints()) };
  float spacing[3] =
  { getXRes(), getYRes(), getZRes() };
  float origin[3] =
  { 0.0f, 0.0f, 0.0f };
  getOrigin(origin);

  out << "  <!-- *************** START OF " << dcName << " *************** -->" << "\n";
  out << "  <Grid Name=\"" << dcName << "\" GridType=\"Uniform\">" << "\n";
  out << "    <Topology TopologyType=\"3DCoRectMesh\" Dimensions=\"" << volDims[2] + 1 << " " << volDims[1] + 1 << " " << volDims[0] + 1 << " \"></Topology>" << "\n";
  out << "    <Geometry Type=\"ORIGIN_DXDYDZ\">" << "\n";
  out << "      <!-- Origin  Z, Y, X -->" << "\n";
  out << "      <DataItem Format=\"XML\" Dimensions=\"3\">" << origin[2] << " " << origin[1] << " " << origin[0] <<  "</DataItem>" << "\n";
  out << "      <!-- DxDyDz (Spacing/Resolution) Z, Y, X -->" << "\n";
  out << "      <DataItem Format=\"XML\" Dimensions=\"3\">" << spacing[2] << " " << spacing[1] << " " << spacing[0] <<  "</DataItem>" << "\n";
  out << "    </Geometry>" << "\n";

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  int err = 0;
  int64_t volDims[3] =
  { 0, 0, 0 };
  float spacing[3] =
  { 1.0f, 1.0f, 1.0f };
  float origin[3] =
  { 0.0f, 0.0f, 0.0f };
  unsigned int spatialDims = 0;
  QString geomName = "";
  err = gatherMetaData(parentId, volDims, spacing, origin, spatialDims, geomName);
  if (err < 0)
  {
    return err;
  }

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageGeom::gatherMetaData(hid_t parentId, int64_t volDims[3], float spacing[3], float origin[3], unsigned int spatialDims, QString geomName)
{
  int err = QH5Lite::readPointerDataset(parentId, H5_DIMENSIONS, volDims);
  if(err < 0)
  {
    return err;
  }
  err = QH5Lite::readPointerDataset(parentId, H5_SPACING, spacing);
  if(err < 0)
  {
    return err;
  }
  err = QH5Lite::readPointerDataset(parentId, H5_ORIGIN, origin);
  if(err < 0)
  {
    return err;
  }
  setDimensions(volDims[0], volDims[1], volDims[2]);
  setResolution(spacing);
  setOrigin(origin);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// Shared ops includes
#define GEOM_CLASS_NAME ImageGeom
#include "DREAM3DLib/Geometry/SharedGeometryOps.cpp"
