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

#include "EdgeDataContainerReader.h"


#include <vector>

#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"

#include "DREAM3DLib/HDF5/VTKH5Constants.h"
#include "DREAM3DLib/HDF5/H5DataArrayReader.h"
#include "DREAM3DLib/Common/StatsDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeDataContainerReader::EdgeDataContainerReader() :
  AbstractFilter(),
  m_HdfFileId(-1),
  m_ReadVertexData(true),
  m_ReadEdgeData(true),
  m_ReadFieldData(true),
  m_ReadEnsembleData(true),
  m_ReadAllArrays(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeDataContainerReader::~EdgeDataContainerReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainerReader::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainerReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainerReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainerReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  EdgeDataContainer* m = getEdgeDataContainer();

  if(NULL == m)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "Edge DataContainer is missing", getErrorCondition());
  }

  if(m_HdfFileId < 0)
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
void EdgeDataContainerReader::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainerReader::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);

  dataCheck(false, 1, 1, 1);

  err = gatherData(false);
  setErrorCondition(err);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainerReader::gatherData(bool preflight)
{
  std::stringstream ss;

  if(m_HdfFileId < 0)
  {
    ss.str("");
    ss << ": Error opening input file";
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
    return -1;
  }

  hid_t dcGid = H5Gopen(m_HdfFileId, DREAM3D::HDF5::EdgeDataContainerName.c_str(), H5P_DEFAULT );
  if (dcGid < 0)
  {
    ss.str("");
    ss << "Error opening Group " << DREAM3D::HDF5::EdgeDataContainerName << std::endl;
    setErrorCondition(-61);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
    return getErrorCondition();
  }

  HDF_ERROR_HANDLER_OFF
  int err = 0;

  err = gatherVertexData(dcGid, preflight);


  err = gatherEdgeData(dcGid, preflight);


  err = gatherFieldData(dcGid, preflight);


  err = gatherEnsembleData(dcGid, preflight);

  // Now finally close the group
  H5Gclose(dcGid); // Close the Data Container Group

  HDF_ERROR_HANDLER_ON

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainerReader::gatherFieldData(hid_t dcGid, bool preflight)
{
    std::vector<std::string> readNames;
    herr_t err = readGroupsData(dcGid, H5_FIELD_DATA_GROUP_NAME, preflight, readNames, m_FieldArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
    return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainerReader::gatherEnsembleData(hid_t dcGid, bool preflight)
{
    std::vector<std::string> readNames;
    herr_t err = readGroupsData(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, preflight, readNames, m_EnsembleArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
    return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainerReader::gatherVertexData(hid_t dcGid, bool preflight)
{
  int err = 0;
  std::vector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;

  if (true == preflight)
  {
    err = H5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::VerticesName, dims, type_class, type_size);
    if (err >= 0) // The Vertices Data set existed so add a dummy to the Data Container
    {
      DREAM3D::Mesh::VertList_t::Pointer vertices = DREAM3D::Mesh::VertList_t::CreateArray(1, DREAM3D::VertexData::SurfaceMeshNodes);
      getEdgeDataContainer()->setVertices(vertices);
    }
  }
  else
  {
    err = readVertices(dcGid);
    if (err < 0)
    {
    }
  }
  // This will conditionally read all the MeshLinks data if preflight is true
  err = readMeshLinks(dcGid, preflight);
  if (err < 0)
  {
  }

  // Read all the Vertex Attribute data
  std::vector<std::string> readNames;
  err = readGroupsData(dcGid, H5_VERTEX_DATA_GROUP_NAME, preflight, readNames, m_VertexArraysToRead);
  if(err == -154) // The group was not in the file so just ignore that error
  {
    err = 0;
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainerReader::gatherEdgeData(hid_t dcGid, bool preflight)
{
  int err = 0;
//  std::vector<hsize_t> dims;
//  H5T_class_t type_class;
//  size_t type_size;

//  if (true == preflight)
//  {
//    err = H5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::EdgesName, dims, type_class, type_size);
//    if (err >= 0)
//    {
//      StructArray<DREAM3D::Mesh::Edge_t>::Pointer edges = StructArray<DREAM3D::Mesh::Edge_t>::CreateArray(1, DREAM3D::EdgeData::SurfaceMeshEdges);
//      getEdgeDataContainer()->setEdges(edges);
//    }
//  }
//  else
//  {
//    err = readEdges(dcGid);
//  }

  // Read all the Edge Attribute data
  std::vector<std::string> readNames;
  err = readGroupsData(dcGid, H5_EDGE_DATA_GROUP_NAME, preflight, readNames, m_EdgeArraysToRead);
  if(err == -154) // The group was not in the file so just ignore that error
  {
    err = 0;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainerReader::readVertices(hid_t dcGid)
{
  EdgeDataContainer* sm = getEdgeDataContainer();
  herr_t err = 0;
  std::vector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  err = H5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::VerticesName, dims, type_class, type_size);
  if (err < 0)
  {
    setErrorCondition(err);
    notifyErrorMessage("No Vertices Data in Data file", getErrorCondition());
    return err;
  }
  // Allocate the data
  DREAM3D::Mesh::VertList_t::Pointer verticesPtr = DREAM3D::Mesh::VertList_t::CreateArray(dims[0],  DREAM3D::VertexData::SurfaceMeshNodes);
  // Read the data
  DREAM3D::Mesh::Float_t* data = reinterpret_cast<DREAM3D::Mesh::Float_t*>(verticesPtr->GetPointer(0));
  err = H5Lite::readPointerDataset(dcGid, DREAM3D::HDF5::VerticesName, data);
  if (err < 0) {
    setErrorCondition(err);
    notifyErrorMessage("Error Reading Vertex List to DREAM3D file", getErrorCondition());
  }
  sm->setVertices(verticesPtr);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainerReader::readMeshLinks(hid_t dcGid, bool preflight)
{
  EdgeDataContainer* sm = getEdgeDataContainer();
  DREAM3D::Mesh::VertList_t::Pointer verticesPtr = sm->getVertices();
  if (NULL == verticesPtr.get())
  {
    return -1;
  }

  MeshLinks::Pointer MeshLinks = MeshLinks::New();

  size_t nVerts = verticesPtr->GetNumberOfTuples();
  herr_t err = 0;
  std::vector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size = 0;
  err = H5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::MeshLinksName, dims, type_class, type_size);
  if (err < 0)
  {
    return err;
  }
  else
  {
    sm->setMeshLinks(MeshLinks);
  }

  if (false == preflight && type_size > 0)
  {
    //Read the array into the buffer
    std::vector<uint8_t> buffer;
    err = H5Lite::readVectorDataset(dcGid, DREAM3D::HDF5::MeshLinksName, buffer);
    if (err < 0)
    {
      setErrorCondition(err);
      notifyErrorMessage("Error Reading Vertex Links from Data file", getErrorCondition());
      return err;
    }
    MeshLinks->deserializeLinks(buffer, nVerts);
    sm->setMeshLinks(MeshLinks);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainerReader::readVertexAttributeData(hid_t dcGid)
{
  int err = -1;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainerReader::readEdges(hid_t dcGid)
{
  herr_t err = -1;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainerReader::readGroupsData(hid_t dcGid, const std::string &groupName, bool preflight,
                                                std::vector<std::string> &namesRead,
                                                std::set<std::string> &namesToRead)
{
  std::stringstream ss;
  int err = 0;
  //Read the Cell Data
  hid_t gid = H5Gopen(dcGid, groupName.c_str(), H5P_DEFAULT);
  if(gid < 0)
  {
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
      if(groupName.compare(H5_VERTEX_DATA_GROUP_NAME) == 0)
      {
        getEdgeDataContainer()->addVertexData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_EDGE_DATA_GROUP_NAME) == 0)
      {
        getEdgeDataContainer()->addEdgeData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_FIELD_DATA_GROUP_NAME) == 0)
      {
        getEdgeDataContainer()->addFieldData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_ENSEMBLE_DATA_GROUP_NAME) == 0)
      {
        getEdgeDataContainer()->addEnsembleData(dPtr->GetName(), dPtr);
      }
    }

  }
  H5Gclose(gid); // Close the Cell Group
  return err;
}
