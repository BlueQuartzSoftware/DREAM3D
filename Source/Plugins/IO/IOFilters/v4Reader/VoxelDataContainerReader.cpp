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


#include "VoxelDataContainerReader.h"



#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"

#include "DREAM3DLib/HDF5/VTKH5Constants.h"
#include "DREAM3DLib/HDF5/H5DataArrayReader.h"
#include "DREAM3DLib/DataArrays/StatsDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VoxelDataContainerReader::VoxelDataContainerReader() :
  AbstractFilter(),
  m_HdfFileId(-1),
  m_ReadVertexData(true),
  m_ReadEdgeData(true),
  m_ReadFaceData(true),
  m_ReadCellData(true),
  m_ReadFieldData(true),
  m_ReadEnsembleData(true),
  m_ReadAllArrays(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VoxelDataContainerReader::~VoxelDataContainerReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VoxelDataContainerReader::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VoxelDataContainerReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VoxelDataContainerReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VoxelDataContainerReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if(NULL == m)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "Voxel DataContainer is missing", getErrorCondition());
  }

  if(m_HdfFileId < 0)
  {
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), "The HDF5 file id was < 0. This means this value was not set correctly from the calling object.", getErrorCondition());
  }
  else if (getInPreflight())
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
void VoxelDataContainerReader::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VoxelDataContainerReader::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << " DataContainer was NULL";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    return;
  }
  setErrorCondition(0);
  //dataCheck(false, 1, 1, 1);
  int err = 0;
  std::stringstream ss;

  // Clear out everything from the data container before we start.
  int64_t volDims[3] =
  { 0, 0, 0 };
  float spacing[3] =
  { 1.0f, 1.0f, 1.0f };
  float origin[3] =
  { 0.0f, 0.0f, 0.0f };
  m->getGeometryAs<ImageGeom>()->setDimensions(volDims[0], volDims[1], volDims[2]);
  m->getGeometryAs<ImageGeom>()->setResolution(spacing);
  m->getGeometryAs<ImageGeom>()->setOrigin(origin);

  if(m_VertexArraysToRead.size() == 0 && m_ReadAllArrays != true) { m_ReadVertexData = false; }
  if(m_EdgeArraysToRead.size() == 0 && m_ReadAllArrays != true) { m_ReadEdgeData = false; }
  if(m_FaceArraysToRead.size() == 0 && m_ReadAllArrays != true) { m_ReadFaceData = false; }
  if(m_CellArraysToRead.size() == 0 && m_ReadAllArrays != true) { m_ReadCellData = false; }
  if(m_FieldArraysToRead.size() == 0 && m_ReadAllArrays != true) { m_ReadFieldData = false; }
  if(m_EnsembleArraysToRead.size() == 0 && m_ReadAllArrays != true) { m_ReadEnsembleData = false; }

  if(m_ReadVertexData == true) { m->clearVertexData(); }
  if(m_ReadEdgeData == true) { m->clearEdgeData(); }
  if(m_ReadFaceData == true) { m->clearFaceData(); }
  if(m_ReadCellData == true) { m->clearCellData(); }
  if(m_ReadFieldData == true) { m->clearFieldData(); }
  if(m_ReadEnsembleData == true) { m->clearEnsembleData(); }

  // We are actually wanting to read the file so set preflight to false
  err = gatherData(false);

  setErrorCondition(err);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VoxelDataContainerReader::getSizeResolutionOrigin(hid_t fileId, int64_t volDims[3], float spacing[3], float origin[3])
{
  int err = 0;
  std::stringstream ss;

  hid_t dcGid = H5Gopen(fileId, DREAM3D::HDF5::VoxelDataContainerName.c_str(), 0);
  if (dcGid < 0) // Check to see if this was a Version 3 or earlier file
  {
    dcGid = H5Gopen(fileId, DREAM3D::HDF5::DataContainerName.c_str(), 0);
  }
  if(dcGid < 0)
  {
    err = H5Utilities::closeFile(fileId);
    ss.str("");
    ss << ": Error opening group '" << DREAM3D::HDF5::VoxelDataContainerName << "'";
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss.str(), -150);
    return -1;
  }

  err |= gatherMetaData(dcGid, volDims, spacing, origin);

  err = H5Gclose(dcGid);
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VoxelDataContainerReader::gatherMetaData(hid_t dcGid, int64_t volDims[3], float spacing[3], float origin[3])
{
  int err = H5Lite::readPointerDataset(dcGid, H5_DIMENSIONS, volDims);
  if(err < 0)
  {
    PipelineMessage em (getHumanLabel(), "DataContainerReader Error Reading the Dimensions", err);
    addErrorMessage(em);
    setErrorCondition(-151);
    return -1;
  }

  err = H5Lite::readPointerDataset(dcGid, H5_SPACING, spacing);
  if(err < 0)
  {
    PipelineMessage em (getHumanLabel(), "DataContainerReader Error Reading the Spacing (Resolution)", err);
    addErrorMessage(em);
    setErrorCondition(-152);
    return -1;
  }

  err = H5Lite::readPointerDataset(dcGid, H5_ORIGIN, origin);
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
int VoxelDataContainerReader::gatherData(bool preflight)
{
  int err = 0;
  std::stringstream ss;
  int64_t volDims[3] =
  { 0, 0, 0 };
  float spacing[3] =
  { 1.0f, 1.0f, 1.0f };
  float origin[3] =
  { 0.0f, 0.0f, 0.0f };
  VoxelDataContainer* m = getVoxelDataContainer();

  if(m_HdfFileId < 0)
  {
    ss.str("");
    ss << ": Error opening input file";
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
    return -1;
  }
  hid_t dcGid = H5Gopen(m_HdfFileId, DREAM3D::HDF5::VoxelDataContainerName.c_str(), 0);
  if (dcGid < 0) // Check to see if this was a Version 3 or earlier file
  {
    dcGid = H5Gopen(m_HdfFileId, DREAM3D::HDF5::DataContainerName.c_str(), 0);
  }
  if(dcGid < 0)
  {
    ss.str("");
    ss << ": Error opening group '" << DREAM3D::HDF5::VoxelDataContainerName << "'. Is the .dream3d file a version 4 data file?";
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss.str(), err);
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
    m->getGeometryAs<ImageGeom>()->setDimensions(volDims[0], volDims[1], volDims[2]); // We use this signature so the compiler will cast the value to the proper int type
    m->getGeometryAs<ImageGeom>()->setResolution(spacing);
    m->getGeometryAs<ImageGeom>()->setOrigin(origin);
  }

  if(m_ReadVertexData == true)
  {
    std::vector<std::string> readNames;
    err |= readGroupsData(dcGid, H5_VERTEX_DATA_GROUP_NAME, preflight, readNames, m_VertexArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
  }

  if(m_ReadEdgeData == true)
  {
    std::vector<std::string> readNames;
    err |= readGroupsData(dcGid, H5_EDGE_DATA_GROUP_NAME, preflight, readNames, m_EdgeArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
  }

  if(m_ReadFaceData == true)
  {
    std::vector<std::string> readNames;
    err |= readGroupsData(dcGid, H5_FACE_DATA_GROUP_NAME, preflight, readNames, m_FaceArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
  }

  if(m_ReadCellData == true)
  {
    std::vector<std::string> readNames;
    err |= readGroupsData(dcGid, H5_CELL_DATA_GROUP_NAME, preflight, readNames, m_CellArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
  }

  if(m_ReadFieldData == true)
  {
    std::vector<std::string> readNames;
    err |= readGroupsData(dcGid, H5_FIELD_DATA_GROUP_NAME, preflight, readNames, m_FieldArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
  }

  if(m_ReadEnsembleData == true)
  {
    std::vector<std::string> readNames;
    err |= readGroupsData(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, preflight, readNames, m_EnsembleArraysToRead);
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
int VoxelDataContainerReader::readGroupsData(hid_t dcGid, const std::string& groupName, bool preflight,
                                             std::vector<std::string>& namesRead,
                                             std::set<std::string>& namesToRead)
{
  std::stringstream ss;
  int err = 0;
  //Read the Cell Data
  hid_t gid = H5Gopen(dcGid, groupName.c_str(), H5P_DEFAULT);
  if(err < 0)
  {
    ss.str("");
    ss << "Error opening HDF5 Group " << groupName << std::endl;
    setErrorCondition(-154);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    return -154;
  }

  NameListType names;
  H5Utilities::getGroupObjects(gid, H5Utilities::H5Support_DATASET | H5Utilities::H5Support_ANY, names);
  //  std::cout << "Number of Items in " << groupName << " Group: " << names.size() << std::endl;
  std::string classType;
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    std::set<std::string>::iterator contains = namesToRead.find(*iter);
    if (contains == namesToRead.end() && false == preflight && m_ReadAllArrays == false) { continue; } // Do not read this item if it is NOT in the set of arrays to read
    namesRead.push_back(*iter);
    classType.clear();
    H5Lite::readStringAttribute(gid, *iter, DREAM3D::HDF5::ObjectType, classType);
    //   std::cout << groupName << " Array: " << *iter << " with C++ ClassType of " << classType << std::endl;
    IDataArray::Pointer dPtr = IDataArray::NullPointer();

    if(classType.find("DataArray") == 0)
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
        getVoxelDataContainer()->addCellData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_FIELD_DATA_GROUP_NAME) == 0)
      {
        getVoxelDataContainer()->addFieldData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_ENSEMBLE_DATA_GROUP_NAME) == 0)
      {
        getVoxelDataContainer()->addEnsembleData(dPtr->GetName(), dPtr);
      }
    }

  }
  H5Gclose(gid); // Close the Cell Group
  return err;
}
