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

#include "VolumeDataContainerReader.h"



#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "DREAM3DLib/HDF5/VTKH5Constants.h"
#include "DREAM3DLib/HDF5/H5DataArrayReader.h"
#include "DREAM3DLib/DataArrays/StatsDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeDataContainerReader::VolumeDataContainerReader() :
  SurfaceDataContainerReader(),
  m_ReadCellData(true),
  m_ReadCellFieldData(true),
  m_ReadCellEnsembleData(true),
  m_ReadAllArrays(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeDataContainerReader::~VolumeDataContainerReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainerReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* dc = VolumeDataContainer::SafePointerDownCast(getDataContainer());
  if(NULL == dc)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "Voxel DataContainer is missing", getErrorCondition());
  }
  if(getHdfGroupId() < 0)
  {
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), "The HDF5 file id was < 0. This means this value was not set correctly from the calling object.", getErrorCondition());
  }
  else if (preflight == true)
  {
    int err = gatherData(preflight);
    if (err < 0)
    {

    }
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainerReader::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainerReader::execute()
{

  VolumeDataContainer* dc = VolumeDataContainer::SafePointerDownCast(getDataContainer());
  if (NULL == dc)
  {
    QString ss = QObject::tr("DataContainer Pointer was NULL and Must be valid.%1(%2)").arg(__FILE__).arg(__LINE__);
    addErrorMessage(getHumanLabel(), ss, -2);
    setErrorCondition(-1);
    return;
  }
  setErrorCondition(0);
  //dataCheck(false, 1, 1, 1);
  int err = 0;

  // Clear out everything from the data container before we start.
  int64_t volDims[3] =
  { 0, 0, 0 };
  float spacing[3] =
  { 1.0f, 1.0f, 1.0f };
  float origin[3] =
  { 0.0f, 0.0f, 0.0f };
  dc->setDimensions(volDims[0], volDims[1], volDims[2]);
  dc->setResolution(spacing);
  dc->setOrigin(origin);

  if(getVertexArraysToRead().size() == 0 && m_ReadAllArrays != true) setReadVertexData(false);
  if(getEdgeArraysToRead().size() == 0 && m_ReadAllArrays != true) setReadEdgeData(false);
  if(getFaceArraysToRead().size() == 0 && m_ReadAllArrays != true) setReadFaceData(false);
  if(m_CellArraysToRead.size() == 0 && m_ReadAllArrays != true) m_ReadCellData = false;
  if(m_CellFieldArraysToRead.size() == 0 && m_ReadAllArrays != true) m_ReadCellFieldData = false;
  if(m_CellEnsembleArraysToRead.size() == 0 && m_ReadAllArrays != true) m_ReadCellEnsembleData = false;

  if(getReadVertexData() == true) dc->clearVertexData();
  if(getReadEdgeData() == true) dc->clearEdgeData();
  if(getReadFaceData() == true) dc->clearFaceData();
  if(m_ReadCellData == true) dc->clearCellData();
  if(m_ReadCellFieldData == true) dc->clearCellFieldData();
  if(m_ReadCellEnsembleData == true) dc->clearCellEnsembleData();

  // We are actually wanting to read the file so set preflight to false
  err = gatherData(false);

  setErrorCondition(err);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainerReader::getSizeResolutionOrigin(hid_t fileId, int64_t volDims[3], float spacing[3], float origin[3])
{
  int err = 0;


  hid_t dcGid = H5Gopen(fileId, DREAM3D::HDF5::VolumeDataContainerName.toLatin1().data(), 0);
  if (dcGid < 0) // Check to see if this was a Version 3 or earlier file
  {
    dcGid = H5Gopen(fileId, DREAM3D::HDF5::DataContainerName.toLatin1().data(), 0);
  }
  if(dcGid < 0)
  {
    err = QH5Utilities::closeFile(fileId);
    QString ss = QObject::tr(": Error opening group '%1'").arg(DREAM3D::HDF5::VolumeDataContainerName);
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss, -150);
    return -1;
  }

  err |= gatherMetaData(dcGid, volDims, spacing, origin);

  err = H5Gclose(dcGid);
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainerReader::gatherMetaData(hid_t dcGid, int64_t volDims[3], float spacing[3], float origin[3])
{
  int err = QH5Lite::readPointerDataset(dcGid, H5_DIMENSIONS, volDims);
  if(err < 0)
  {
    PipelineMessage em (getHumanLabel(), "DataContainerReader Error Reading the Dimensions", err);
    addErrorMessage(em);
    setErrorCondition(-151);
    return -1;
  }

  err = QH5Lite::readPointerDataset(dcGid, H5_SPACING, spacing);
  if(err < 0)
  {
    PipelineMessage em (getHumanLabel(), "DataContainerReader Error Reading the Spacing (Resolution)", err);
    addErrorMessage(em);
    setErrorCondition(-152);
    return -1;
  }

  err = QH5Lite::readPointerDataset(dcGid, H5_ORIGIN, origin);
  if(err < 0)
  {
    PipelineMessage em (getHumanLabel(), "DataContainerReader Error Reading the Origin", err);
    addErrorMessage(em);
    setErrorCondition(-153);
    return -1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainerReader::gatherData(bool preflight)
{
  int err = 0;

  int64_t volDims[3] =
  { 0, 0, 0 };
  float spacing[3] =
  { 1.0f, 1.0f, 1.0f };
  float origin[3] =
  { 0.0f, 0.0f, 0.0f };
  VolumeDataContainer* dc = VolumeDataContainer::SafePointerDownCast(getDataContainer());

  hid_t dcGid = H5Gopen(getHdfGroupId(), getDataContainer()->getName().toLatin1().data(), 0);
  if(dcGid < 0)
  {
    QString ss = QObject::tr(": Error opening data container folder");
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }

  //if(false == preflight)
  {
    err = gatherMetaData(dcGid, volDims, spacing, origin);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
    dc->setDimensions(volDims[0], volDims[1], volDims[2]); // We use this signature so the compiler will cast the value to the proper int type
    dc->setResolution(spacing);
    dc->setOrigin(origin);
  }

  if(getReadVertexData() == true)
  {
    QVector<QString> readNames;
    QSet<QString> vertexArraysToRead = getVertexArraysToRead();
    err |= readGroupsData(dcGid, H5_VERTEX_DATA_GROUP_NAME, preflight, readNames, vertexArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
  }

  if(getReadEdgeData() == true)
  {
    QVector<QString> readNames;
    QSet<QString> edgeArraysToRead = getEdgeArraysToRead();
    err |= readGroupsData(dcGid, H5_EDGE_DATA_GROUP_NAME, preflight, readNames, edgeArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
  }

  if(getReadFaceData() == true)
  {
    QVector<QString> readNames;
    QSet<QString> faceArraysToRead = getFaceArraysToRead();
    err |= readGroupsData(dcGid, H5_FACE_DATA_GROUP_NAME, preflight, readNames, faceArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
  }

  if(m_ReadCellData == true)
  {
    QVector<QString> readNames;
    QSet<QString> cellArraysToRead = getCellArraysToRead();
    err |= readGroupsData(dcGid, H5_CELL_DATA_GROUP_NAME, preflight, readNames, cellArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
  }

  if(m_ReadCellFieldData == true)
  {
    QVector<QString> readNames;
    QSet<QString> cellFeildArraysToRead = getCellFieldArraysToRead();
    err |= readGroupsData(dcGid, H5_CELL_FIELD_DATA_GROUP_NAME, preflight, readNames, cellFeildArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
  }

  if(m_ReadCellEnsembleData == true)
  {
    QVector<QString> readNames;
    QSet<QString> cellEnsembleArraysToRead = getCellEnsembleArraysToRead();
    err |= readGroupsData(dcGid, H5_CELL_ENSEMBLE_DATA_GROUP_NAME, preflight, readNames, cellEnsembleArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
  }

  err |= H5Gclose(dcGid);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainerReader::readGroupsData(hid_t dcGid, const QString &groupName, bool preflight,
                                                QVector<QString> &namesRead,
                                                QSet<QString> &namesToRead)
{

  VolumeDataContainer* dc = VolumeDataContainer::SafePointerDownCast(getDataContainer());

  int err = 0;
  //Read the Cell Data
  hid_t gid = H5Gopen(dcGid, groupName.toLatin1().data(), H5P_DEFAULT);
  if(err < 0)
  {
    QString ss = QObject::tr("Error opening HDF5 Group %1").arg(groupName);
    setErrorCondition(-154);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -154;
  }

  NameListType names;
  QH5Utilities::getGroupObjects(gid, H5Utilities::H5Support_DATASET | H5Utilities::H5Support_ANY, names);
  //  qDebug() << "Number of Items in " << groupName << " Group: " << names.size() << "\n";
  QString classType;
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    QSet<QString>::iterator contains = namesToRead.find(*iter);
    if (contains == namesToRead.end() && false == preflight && m_ReadAllArrays == false) { continue; } // Do not read this item if it is NOT in the set of arrays to read
    namesRead.push_back(*iter);
    classType.clear();
    QH5Lite::readStringAttribute(gid, *iter, DREAM3D::HDF5::ObjectType, classType);
    //   qDebug() << groupName << " Array: " << *iter << " with C++ ClassType of " << classType << "\n";
    IDataArray::Pointer dPtr = IDataArray::NullPointer();

    if(classType.startsWith("DataArray") == true)
    {
      dPtr = H5DataArrayReader::readIDataArray(gid, *iter, preflight);
    }
    else if(classType.compare("StringDataArray") == 0)
    {
      dPtr = H5DataArrayReader::readStringDataArray(gid, *iter, preflight);
    }
    else if(classType.compare("vector") == 0)
    {

    }
    else if(classType.compare("NeighborList<T>") == 0)
    {
      dPtr = H5DataArrayReader::readNeighborListData(gid, *iter, preflight);
    }
    else if ( (*iter).compare(DREAM3D::EnsembleData::Statistics) == 0)
    {
      StatsDataArray::Pointer statsData = StatsDataArray::New();
      statsData->SetName(DREAM3D::EnsembleData::Statistics);
      statsData->readH5Data(gid);
      dPtr = statsData;
    }

    if (NULL != dPtr.get())
    {
      if(groupName.compare(H5_CELL_DATA_GROUP_NAME) == 0)
      {
        dc->addCellData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_CELL_FIELD_DATA_GROUP_NAME) == 0)
      {
        dc->addCellFieldData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_CELL_ENSEMBLE_DATA_GROUP_NAME) == 0)
      {
        dc->addCellEnsembleData(dPtr->GetName(), dPtr);
      }
    }

  }
  H5Gclose(gid); // Close the Cell Group
  return err;
}
