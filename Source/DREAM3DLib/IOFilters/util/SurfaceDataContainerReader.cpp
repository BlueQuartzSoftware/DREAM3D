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

#include "SurfaceDataContainerReader.h"


#include <vector>

#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "DREAM3DLib/HDF5/VTKH5Constants.h"
#include "DREAM3DLib/HDF5/H5DataArrayReader.h"
#include "DREAM3DLib/DataArrays/StatsDataArray.h"

namespace Detail {
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
SurfaceDataContainerReader::SurfaceDataContainerReader() :
EdgeDataContainerReader(),
  m_ReadFaceData(true),
  m_ReadFaceFieldData(true),
  m_ReadFaceEnsembleData(true),
  m_ReadAllFaceArrays(false),
  m_ReadAllFaceFieldArrays(false),
  m_ReadAllFaceEnsembleArrays(false),
  m_ReadAllArrays(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceDataContainerReader::~SurfaceDataContainerReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainerReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());
  if(NULL == dc)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
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
void SurfaceDataContainerReader::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainerReader::execute()
{
  int err = 0;

  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());
  if(NULL == dc)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  if(m_FaceArraysToRead.size() == 0 && m_ReadAllFaceArrays != true && m_ReadAllArrays != true) m_ReadFaceData = false;
  if(m_FaceFieldArraysToRead.size() == 0 && m_ReadAllFaceFieldArrays != true && m_ReadAllArrays != true) m_ReadFaceFieldData = false;
  if(m_FaceEnsembleArraysToRead.size() == 0 && m_ReadAllFaceEnsembleArrays != true && m_ReadAllArrays != true) m_ReadFaceEnsembleData = false;

  if(m_ReadFaceData == true) dc->clearFaceData();
  if(m_ReadFaceFieldData == true) dc->clearFaceFieldData();
  if(m_ReadFaceEnsembleData == true) dc->clearFaceEnsembleData();

  err = gatherData(false);
  setErrorCondition(err);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerReader::gatherData(bool preflight)
{

  if(getHdfGroupId() < 0)
  {
    QString ss = QObject::tr(": Error opening input file");
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }

  hid_t dcGid = H5Gopen(getHdfGroupId(), getDataContainer()->getName().toLatin1().data(), H5P_DEFAULT );
  if(dcGid < 0)
  {
    QString ss = QObject::tr(": Error opening group '%1'. Is the .dream3d file a version 4 data file?").arg(DREAM3D::HDF5::EdgeDataContainerName);
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }

  int err = 0;

  readMeshData(dcGid, preflight);

  if(m_ReadFaceData == true)
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

  if(m_ReadFaceFieldData == true)
  {
    QVector<QString> readNames;
    QSet<QString> faceFieldArraysToRead = getFaceFieldArraysToRead();
    err |= readGroupsData(dcGid, H5_CELL_FIELD_DATA_GROUP_NAME, preflight, readNames, faceFieldArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
  }

  if(m_ReadFaceEnsembleData == true)
  {
    QVector<QString> readNames;
    QSet<QString> faceEnsembleArraysToRead = getFaceEnsembleArraysToRead();
    err |= readGroupsData(dcGid, H5_CELL_ENSEMBLE_DATA_GROUP_NAME, preflight, readNames, faceEnsembleArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
  }

  // Now finally close the group
  err |= H5Gclose(dcGid); // Close the Data Container Group

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerReader::readMeshData(hid_t dcGid, bool preflight)
{
  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());

  int err = 0;
  QVector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  if (true == preflight)
  {
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::FacesName, dims, type_class, type_size);
    if (err >= 0)
    {
      FaceArray::Pointer triangles = FaceArray::CreateArray(1, DREAM3D::FaceData::SurfaceMeshFaces, NULL);
      dc->setFaces(triangles);
    }
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::FaceNeighbors, dims, type_class, type_size);
    if(err >= 0)
    {
      Int32DynamicListArray::Pointer faceNeighbors = Int32DynamicListArray::New();
      dc->getFaces()->setFaceNeighbors(faceNeighbors);
    }
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::FacesContainingVert, dims, type_class, type_size);
    if(err >= 0)
    {
      Int32DynamicListArray::Pointer facesContainingVert = Int32DynamicListArray::New();
      dc->getFaces()->setFacesContainingVert(facesContainingVert);
    }
  }
  else
  {
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::FacesName, dims, type_class, type_size);
    if (err >= 0)
    {
      // Allocate the Face_t structures
      FaceArray::Pointer facesPtr = FaceArray::CreateArray(dims[0], DREAM3D::FaceData::SurfaceMeshFaces, NULL);
      // We need this to properly use QH5Lite because the data is stored as int32_t in 5 columns
      int32_t* data = reinterpret_cast<int32_t*>(facesPtr->getPointer(0));
      // Read the data from the file
      err = QH5Lite::readPointerDataset(dcGid, DREAM3D::HDF5::FacesName, data);
      if (err < 0)
      {
        setErrorCondition(err);
        notifyErrorMessage("Error Reading Face List from DREAM3D file", getErrorCondition());
        return err;
      }
      dc->setFaces(facesPtr);
      size_t nFaces= facesPtr->getNumberOfTuples();
      err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::FaceNeighbors, dims, type_class, type_size);
      if (err >= 0)
      {
        //Read the faceNeighbors array into the buffer

        std::vector<uint8_t> buffer;
        err = QH5Lite::readVectorDataset(dcGid, DREAM3D::HDF5::FaceNeighbors, buffer);
        if(err < 0)
        {
          setErrorCondition(err);
          notifyErrorMessage("Error Reading Face List from DREAM3D file", getErrorCondition());
          return err;
        }
        Int32DynamicListArray::Pointer faceNeighbors = Int32DynamicListArray::New();
        faceNeighbors->deserializeLinks(buffer, nFaces);
        dc->getFaces()->setFaceNeighbors(faceNeighbors);
      }
      err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::FacesContainingVert, dims, type_class, type_size);
      if (err >= 0)
      {
        //Read the faceNeighbors array into the buffer

        std::vector<uint8_t> buffer;
        err = QH5Lite::readVectorDataset(dcGid, DREAM3D::HDF5::FacesContainingVert, buffer);
        if(err < 0)
        {
          setErrorCondition(err);
          notifyErrorMessage("Error Reading Face List from DREAM3D file", getErrorCondition());
          return err;
        }
        Int32DynamicListArray::Pointer facesContainingVert = Int32DynamicListArray::New();
        facesContainingVert->deserializeLinks(buffer, nFaces);
        dc->getFaces()->setFacesContainingVert(facesContainingVert);
      }
    }
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerReader::readGroupsData(hid_t dcGid, const QString &groupName, bool preflight,
                                                QVector<QString> &namesRead,
                                                QSet<QString> &namesToRead)
{

  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());

  int err = 0;
  //Read the Cell Data
  hid_t gid = H5Gopen(dcGid, groupName.toLatin1().data(), H5P_DEFAULT);
  if(gid < 0)
  {
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
      if(groupName.compare(H5_VERTEX_DATA_GROUP_NAME) == 0)
      {
        dc->addVertexData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_FACE_DATA_GROUP_NAME) == 0)
      {
       dc->addFaceData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_EDGE_DATA_GROUP_NAME) == 0)
      {
        dc->addEdgeData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_FIELD_DATA_GROUP_NAME) == 0)
      {
        dc->addFaceFieldData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_ENSEMBLE_DATA_GROUP_NAME) == 0)
      {
        dc->addFaceEnsembleData(dPtr->GetName(), dPtr);
      }
    }

  }
  H5Gclose(gid); // Close the Cell Group
  return err;
}
