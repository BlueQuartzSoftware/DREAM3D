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

namespace Detail
{
class H5GroupAutoCloser
{
public:
  H5GroupAutoCloser(hid_t* groupId) :
  gid(groupId)
  {}

  virtual ~H5GroupAutoCloser()
  {
    if (*gid > 0)
    {
      H5Gclose(*gid);
    }
  }
  private:
   hid_t* gid;
};
}


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
void VolumeDataContainerWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* dc = VolumeDataContainer::SafePointerDownCast(getDataContainer());
  if(NULL == dc)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "Voxel DataContainer is missing", getErrorCondition());
  }
  if(getHdfFileId() < 0)
  {
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), "The HDF5 file id was < 0. This means this value was not set correctly from the calling object.", getErrorCondition());
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainerWriter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
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
    addErrorMessage(getHumanLabel(), ss, -2);
    setErrorCondition(-1);
    return;
  }

  setErrorCondition(0);

  // Create the HDF5 Group for the Data Container
  err = H5Utilities::createGroupsFromPath(DREAM3D::HDF5::VolumeDataContainerName.toLatin1().data(), getHdfFileId());
  if (err < 0)
  {

    QString ss = QObject::tr("Error creating HDF Group %1").arg(DREAM3D::HDF5::VolumeDataContainerName);
    setErrorCondition(-60);
    addErrorMessage(getHumanLabel(), ss, err);
    return;
  }
  hid_t dcGid = H5Gopen(getHdfFileId(), DREAM3D::HDF5::VolumeDataContainerName.toLatin1().data(), H5P_DEFAULT );
  if (dcGid < 0)
  {
    QString ss = QObject::tr("Error opening Group %1").arg(DREAM3D::HDF5::VolumeDataContainerName);
    setErrorCondition(-61);
    addErrorMessage(getHumanLabel(), ss, err);
    return;
  }
  Detail::H5GroupAutoCloser dcGidAutoCloser(&dcGid);

  // This just writes the header information
  int64_t volDims[3] =
  { dc->getXPoints(), dc->getYPoints(), dc->getZPoints() };
  float spacing[3] =
  { dc->getXRes(), dc->getYRes(), dc->getZRes() };
  float origin[3] =
  { 0.0f, 0.0f, 0.0f };
  dc->getOrigin(origin);
  err = writeMetaInfo(DREAM3D::HDF5::VolumeDataContainerName, volDims, spacing, origin);
  if (err < 0)
  {
    QString ss = QObject::tr(":Error Writing header information to output file");
    setErrorCondition(-62);
    addErrorMessage(getHumanLabel(), ss, err);
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }

  err = writeCellData(dcGid);
  if (err < 0)
  {
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }

  err = writeCellFieldData(dcGid);
  if (err < 0)
  {
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }

  err = writeCellEnsembleData(dcGid);
  if (err < 0)
  {
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }

  // Now finally close the group and the HDf5 File
  H5Gclose(dcGid); // Close the Data Container Group



  notifyStatusMessage("Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainerWriter::writeCellXdmfGridHeader(float* origin, float* spacing, int64_t* volDims)
{

  if (false == getWriteXdmfFile() || NULL == getXdmfOStream() || NULL == getDataContainer())
  {
    return;
  }
  QTextStream& out = *(getXdmfOStream());
  out << "\n  <Grid Name=\"Cell Data\" GridType=\"Uniform\">" << "\n";
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
void VolumeDataContainerWriter::writeFieldXdmfGridHeader(size_t numElements, const QString &label)
{

  if (false == getWriteXdmfFile() || NULL == getXdmfOStream() || NULL == getDataContainer())
  {
    return;
  }

  QTextStream& out = *getXdmfOStream();
  out << "\n  <Grid Name=\"" << label << "\" GridType=\"Uniform\">" << "\n";
  out << "      <Topology TopologyType=\"3DCoRectMesh\" Dimensions=\"" << numElements << " 1 1\"></Topology>" << "\n";
  out << "      <Geometry Type=\"ORIGIN_DXDYDZ\">" << "\n";
  out << "        <!-- Origin -->" << "\n";
  out << "        <DataItem Format=\"XML\" Dimensions=\"3\">0 0 0</DataItem>" << "\n";
  out << "        <!-- DxDyDz (Spacing/Resolution)-->" << "\n";
  out << "        <DataItem Format=\"XML\" Dimensions=\"3\">1 1 1</DataItem>" << "\n";
  out << "      </Geometry>" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainerWriter::writeXdmfGridFooter(const QString &label)
{
  if (false == getWriteXdmfFile() || NULL == getXdmfOStream() || NULL == getDataContainer())
  {
    return;
  }
  QTextStream& out = *getXdmfOStream();
  out << "  </Grid>" << "\n";
  out << "    <!-- *************** END OF " << label << " *************** -->" << "\n";
  out << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainerWriter::writeMetaInfo(const QString &hdfPath, int64_t volDims[3], float spacing[3], float origin[3])
{
  herr_t err = 0;
  err = createVtkObjectGroup(hdfPath, H5_VTK_STRUCTURED_POINTS);
  if (err < 0)  {
    return err;
  }
  hid_t gid = H5Gopen(getHdfFileId(), hdfPath.toLatin1().data(), H5P_DEFAULT );

  int32_t rank =1;
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
  err = QH5Lite::writeScalarAttribute(getHdfFileId(), hdfPath, H5_NUMBER_OF_POINTS, nPoints);
  if (err < 0)
  {
    qDebug() << "Error Writing H5_NUMBER_OF_POINTS attribute for " << hdfPath << "\n";
  }

  err |= H5Gclose(gid);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainerWriter::writeCellData(hid_t dcGid)
{
  QString ss;
  int err = 0;
  VolumeDataContainer* dc = VolumeDataContainer::SafePointerDownCast(getDataContainer());
  int64_t volDims[3] =
  { dc->getXPoints(), dc->getYPoints(), dc->getZPoints() };
  float spacing[3] =
  { dc->getXRes(), dc->getYRes(), dc->getZRes() };
  float origin[3] =
  { 0.0f, 0.0f, 0.0f };
  dc->getOrigin(origin);

  writeCellXdmfGridHeader(origin, spacing, volDims);

  QString hdfFileName = QH5Utilities::fileNameFromFileId(getHdfFileId());
  QString xdmfGroupPath = QString(":/") + VolumeDataContainer::ClassName() + QString("/") + H5_CELL_DATA_GROUP_NAME;

  // Write the Voxel Data
  err = QH5Utilities::createGroupsFromPath(H5_CELL_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {

    QString ss = QObject::tr("Error creating HDF Group %1").arg(H5_CELL_DATA_GROUP_NAME);
    setErrorCondition(-63);
    notifyErrorMessage( ss, err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  hid_t cellGroupId = H5Gopen(dcGid, H5_CELL_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {

    QString ss = QObject::tr("Error writing string attribute to HDF Group %1").arg(H5_CELL_DATA_GROUP_NAME);
    setErrorCondition(-64);
    notifyErrorMessage( ss, err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  NameListType names = dc->getCellArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {

    QString ss = QObject::tr("Writing Cell Data '%1' to HDF5 File").arg(*iter);
    notifyStatusMessage(ss);
    IDataArray::Pointer array = dc->getCellData(*iter);
    err = array->writeH5Data(cellGroupId);
    if(err < 0)
    {

      QString ss = QObject::tr("Error writing array '%1' to the HDF5 File").arg(*iter);
      notifyErrorMessage(ss, err);
      setErrorCondition(err);
      H5Gclose(cellGroupId); // Close the Cell Group
      H5Gclose(dcGid); // Close the Data Container Group
      return err;
    }
    array->writeXdmfAttribute( *getXdmfOStream(), volDims, hdfFileName, xdmfGroupPath, " (Cell)");
  }
  H5Gclose(cellGroupId); // Close the Cell Group
  writeXdmfGridFooter("Cell Data");
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainerWriter::writeCellFieldData(hid_t dcGid)
{
  QString ss;
  int err = 0;
  VolumeDataContainer* dc = VolumeDataContainer::SafePointerDownCast(getDataContainer());

#if WRITE_FIELD_XDMF
// Get the name of the .dream3d file that we are writing to:
  ssize_t nameSize = H5Fget_name(getHdfFileId(), NULL, 0) + 1;
  std::vector<char> nameBuffer(nameSize, 0);
  nameSize = H5Fget_name(getHdfFileId(), &(nameBuffer.front()), nameSize);

  QString hdfFileName(&(nameBuffer.front()), nameSize);
  hdfFileName = QFileInfo::filename(hdfFileName);
  QString xdmfGroupPath = QString(":/") + VoxelDataContainer::ClassName() + QString("/") + H5_FIELD_DATA_GROUP_NAME;
#endif

  int64_t volDims[3] = { 0,0,0 };


  // Write the Field Data
  err = QH5Utilities::createGroupsFromPath(H5_FIELD_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {
    qDebug() << "Error creating HDF Group " << H5_FIELD_DATA_GROUP_NAME << "\n";
    return err;
  }
  err = QH5Lite::writeStringAttribute(dcGid, H5_FIELD_DATA_GROUP_NAME, H5_NAME, H5_FIELD_DATA_DEFAULT);
  if(err < 0)
  {
    return err;
  }

  hid_t fieldGroupId = H5Gopen(dcGid, H5_FIELD_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {

    QString ss = QObject::tr("Error opening field Group %1").arg(H5_FIELD_DATA_GROUP_NAME);
    setErrorCondition(-65);
    addErrorMessage(getHumanLabel(), ss, err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }

  size_t total = 0;
  typedef std::vector<IDataArray*> VectorOfIDataArrays_t;
  VectorOfIDataArrays_t neighborListArrays;

  NameListType names = dc->getCellFieldArrayNameList();
  if (names.size() > 0)
  {
    IDataArray::Pointer array = dc->getCellFieldData(names.front());
    total = array->GetSize();
    volDims[0] = total;
    volDims[1] = 1;
    volDims[2] = 1;
#if WRITE_FIELD_XDMF

    QString ss = QObject::tr("Field Data (%1)").arg(total);
    writeFieldXdmfGridHeader(total, ss.str());
#endif
  }
  // Now loop over all the field data and write it out, possibly wrapping it with XDMF code also.
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    IDataArray::Pointer array = dc->getCellFieldData(*iter);
    if (array->getTypeAsString().compare(NeighborList<int>::ClassName()) == 0)
    {
      neighborListArrays.push_back(array.get());
    }
    else if (NULL != array.get())
    {
      err = array->writeH5Data(fieldGroupId);
      if(err < 0)
      {

        QString ss = QObject::tr("Error writing field array '%1' to the HDF5 File").arg(*iter);
        addErrorMessage(getHumanLabel(), ss, err);
        setErrorCondition(err);
        H5Gclose(fieldGroupId); // Close the Cell Group
        H5Gclose(dcGid); // Close the Data Container Group
        return err;
      }
#if WRITE_FIELD_XDMF
      array->writeXdmfAttribute( *getXdmfOStream(), volDims, hdfFileName, xdmfGroupPath, " (Field)");
#endif
    }
  }


#if WRITE_FIELD_XDMF
  if (names.size() > 0)
  {
    writeXdmfGridFooter("Field Data");
  }
#endif

  // Write the NeighborLists onto their own grid
  // We need to determine how many total elements we are going to end up with and group the arrays by
  // those totals so we can minimize the number of grids
  typedef QMap<size_t, VectorOfIDataArrays_t> SizeToIDataArrays_t;
  SizeToIDataArrays_t sizeToDataArrays;

  for(VectorOfIDataArrays_t::iterator iter = neighborListArrays.begin(); iter < neighborListArrays.end(); ++iter)
  {
    IDataArray* array = (*iter);
    sizeToDataArrays[array->GetSize()].push_back(array);
  }

  // Now loop over each pair in the map creating a section in the XDMF and also writing the data to the HDF5 file
  for(SizeToIDataArrays_t::iterator pair = sizeToDataArrays.begin(); pair != sizeToDataArrays.end(); ++pair)
  {
    total = pair.key();
    VectorOfIDataArrays_t& arrays = pair.value();
    volDims[0] = total;
    volDims[1] = 1;
    volDims[2] = 1;
    #if WRITE_FIELD_XDMF

    QString ss = QObject::tr("Neighbor Data (%1)").arg(total);
    writeFieldXdmfGridHeader(total, ss.str());
    #endif
    for(VectorOfIDataArrays_t::iterator iter = arrays.begin(); iter < arrays.end(); ++iter)
    {
      err = (*iter)->writeH5Data(fieldGroupId);
      if(err < 0)
      {

        QString ss = QObject::tr("Error writing field array '%1' to the HDF5 File").arg((*iter)->GetName());
        addErrorMessage(getHumanLabel(), ss, err);
        setErrorCondition(err);
        H5Gclose(fieldGroupId); // Close the Cell Group
        H5Gclose(dcGid); // Close the Data Container Group
        return err;
      }
#if WRITE_FIELD_XDMF
      (*iter)->writeXdmfAttribute( *getXdmfOStream(), volDims, hdfFileName, xdmfGroupPath, " (Neighbor Data)");
#endif
    }
#if WRITE_FIELD_XDMF
    writeXdmfGridFooter(ss.str());
#endif

  }

  H5Gclose(fieldGroupId);
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainerWriter::writeCellEnsembleData(hid_t dcGid)
{
  QString ss;
  int err = 0;
  VolumeDataContainer* dc = VolumeDataContainer::SafePointerDownCast(getDataContainer());

  // Write the Ensemble data
  err = QH5Utilities::createGroupsFromPath(H5_ENSEMBLE_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {

    QString ss = QObject::tr("Error creating HDF Group %1").arg(H5_ENSEMBLE_DATA_GROUP_NAME);
    setErrorCondition(-66);
    addErrorMessage(getHumanLabel(), ss, err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  err = QH5Lite::writeStringAttribute(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, H5_NAME, H5_ENSEMBLE_DATA_DEFAULT);

  hid_t ensembleGid = H5Gopen(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {

    QString ss = QObject::tr("Error opening ensemble Group %1").arg(H5_ENSEMBLE_DATA_GROUP_NAME);
    setErrorCondition(-67);
    addErrorMessage(getHumanLabel(), ss, err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  NameListType names = dc->getCellEnsembleArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    IDataArray::Pointer array = dc->getCellEnsembleData(*iter);
    err = array->writeH5Data(ensembleGid);
    if(err < 0)
    {

      QString ss = QObject::tr("Error writing Ensemble array '%1' to the HDF5 File").arg(*iter);
      addErrorMessage(getHumanLabel(), ss, err);
      setErrorCondition(err);
      H5Gclose(ensembleGid); // Close the Cell Group
      H5Gclose(dcGid); // Close the Data Container Group
      return err;
    }
  }

  H5Gclose(ensembleGid);

  return err;
}

