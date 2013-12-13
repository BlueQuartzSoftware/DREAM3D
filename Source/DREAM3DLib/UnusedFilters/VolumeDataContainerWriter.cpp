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
#include "VolumeDataContainerWriter.h"


#include <QtCore/QFileInfo>


#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "DREAM3DLib/DataArrays/NeighborList.hpp"

#define WRITE_FIELD_XDMF 0


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeDataContainerWriter::VolumeDataContainerWriter() :
  SurfaceDataContainerWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeDataContainerWriter::~VolumeDataContainerWriter()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainerWriter::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* dc = VolumeDataContainer::SafePointerDownCast(getDataContainer());
  if(NULL == dc)
  {
    setErrorCondition(-383);
    PipelineMessage em (getHumanLabel(), "Voxel DataContainer is missing", getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
  }
  if(getHdfGroupId() < 0)
  {
    setErrorCondition(-150);
    PipelineMessage em (getHumanLabel(), "The HDF5 file id was < 0. This means this value was not set correctly from the calling object.", getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainerWriter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainerWriter::execute()
{
  int err = 0;

  VolumeDataContainer* dc = VolumeDataContainer::SafePointerDownCast(getDataContainer());
  if (NULL == dc)
  {
    QString ss = QObject::tr("DataContainer Pointer was NULL and Must be valid.%1(%2)").arg(__FILE__).arg(__LINE__);
    PipelineMessage em (getHumanLabel(), ss, -2, PipelineMessage::Error);
    emit filterGeneratedMessage(em);
    setErrorCondition(-1);
    return;
  }

  setErrorCondition(0);

  hid_t dcGid = H5Gopen(getHdfGroupId(), getDataContainer()->getName().toLatin1().data(), H5P_DEFAULT );

  // This just writes the header information
  int64_t volDims[3] =
  { dc->getXPoints(), dc->getYPoints(), dc->getZPoints() };
  float spacing[3] =
  { dc->getXRes(), dc->getYRes(), dc->getZRes() };
  float origin[3] =
  { 0.0f, 0.0f, 0.0f };
  dc->getOrigin(origin);
  err = writeMetaInfo(getDataContainer()->getName(), volDims, spacing, origin);
  if (err < 0)
  {
    QString ss = QObject::tr(":Error Writing header information to output file");
    setErrorCondition(-62);
    PipelineMessage em (getHumanLabel(), ss, getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }

  if(getdcType() == 0) { writeXdmfMeshStructure(); }

  // Now finally close the group and the HDf5 File
  H5Gclose(dcGid); // Close the Data Container Group



  emit filterGeneratedMessage(PipelineMessage::CreateStatusMessage(getHumanLabel(), "Complete") );
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainerWriter::writeXdmfMeshStructure()
{
  VolumeDataContainer* dc = VolumeDataContainer::SafePointerDownCast(getDataContainer());
  int64_t volDims[3] =
  { dc->getXPoints(), dc->getYPoints(), dc->getZPoints() };
  float spacing[3] =
  { dc->getXRes(), dc->getYRes(), dc->getZRes() };
  float origin[3] =
  { 0.0f, 0.0f, 0.0f };
  dc->getOrigin(origin);

  if (false == getWriteXdmfFile() || NULL == getXdmfOStream() || NULL == getDataContainer())
  {
    return;
  }
  QTextStream& out = *(getXdmfOStream());
  out << "\n  <Grid Name=\"" << getDataContainer()->getName() << "\" GridType=\"Uniform\">" << "\n";
  out << "    <Topology TopologyType=\"3DCoRectMesh\" Dimensions=\"" << volDims[2] + 1 << " " << volDims[1] + 1 << " " << volDims[0] + 1 << " \"></Topology>" << "\n";
  out << "    <Geometry Type=\"ORIGIN_DXDYDZ\">" << "\n";
  out << "      <!-- Origin -->" << "\n";
  out << "      <DataItem Format=\"XML\" Dimensions=\"3\">" << origin[2] << " " << origin[1] << " " << origin[0] <<  "</DataItem>" << "\n";
  out << "      <!-- DxDyDz (Spacing/Resolution)-->" << "\n";
  out << "      <DataItem Format=\"XML\" Dimensions=\"3\">" << spacing[2] << " " << spacing[1] << " " << spacing[0] <<  "</DataItem>" << "\n";
  out << "    </Geometry>" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainerWriter::writeMetaInfo(const QString& hdfPath, int64_t volDims[3], float spacing[3], float origin[3])
{
  herr_t err = 0;
  err = createVtkObjectGroup(hdfPath, H5_VTK_STRUCTURED_POINTS);
  if (err < 0)
  {
    return err;
  }
  hid_t gid = H5Gopen(getHdfGroupId(), hdfPath.toLatin1().data(), H5P_DEFAULT );

  int32_t rank = 1;
  hsize_t dims[1] = {3};
  err = QH5Lite::writePointerDataset(gid, H5_DIMENSIONS, rank, dims, volDims);
  if (err < 0)
  {
    qDebug() << "Error Writing H5_DIMENSIONS array for " << hdfPath << "\n";
  }
  err = QH5Lite::writePointerDataset(gid, H5_ORIGIN, rank, dims, origin);
  if (err < 0)
  {
    qDebug() << "Error Writing H5_ORIGIN array for " << hdfPath << "\n";
  }
  err = QH5Lite::writePointerDataset(gid, H5_SPACING, rank, dims, spacing);
  if (err < 0)
  {
    qDebug() << "Error Writing H5_SPACING array for " << hdfPath << "\n";
  }
  int64_t nPoints = volDims[0] * volDims[1] * volDims[2];
  err = QH5Lite::writeScalarAttribute(getHdfGroupId(), hdfPath, H5_NUMBER_OF_POINTS, nPoints);
  if (err < 0)
  {
    qDebug() << "Error Writing H5_NUMBER_OF_POINTS attribute for " << hdfPath << "\n";
  }

  err |= H5Gclose(gid);
  return err;
}
