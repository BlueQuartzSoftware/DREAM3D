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

#include "SurfaceMeshDataContainerReader.h"


#include <vector>

#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"

#include "DREAM3DLib/HDF5/VTKH5Constants.h"
#include "DREAM3DLib/HDF5/H5DataArrayReader.h"
#include "DREAM3DLib/Common/StatsDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshDataContainerReader::SurfaceMeshDataContainerReader() :
  AbstractFilter(),
  m_HdfFileId(-1)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshDataContainerReader::~SurfaceMeshDataContainerReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainerReader::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainerReader::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainerReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* m = getSurfaceMeshDataContainer();

  if(NULL == m)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer is missing", getErrorCondition());
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
void SurfaceMeshDataContainerReader::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainerReader::execute()
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
void SurfaceMeshDataContainerReader::addCreatedPointData(const std::string &name)
{
  addCreatedCellData(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainerReader::addCreatedFaceData(const std::string &name)
{
  addCreatedFieldData(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainerReader::addCreatedEdgeData(const std::string &name)
{
  addCreatedEnsembleData(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerReader::gatherData(bool preflight)
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

  hid_t dcGid = H5Gopen(m_HdfFileId, DREAM3D::HDF5::SurfaceMeshDataContainerName.c_str(), H5P_DEFAULT );
  if (dcGid < 0)
  {
    ss.str("");
    ss << "Error opening Group " << DREAM3D::HDF5::SurfaceMeshDataContainerName << std::endl;
    setErrorCondition(-61);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
    return getErrorCondition();
  }

  HDF_ERROR_HANDLER_OFF
  int err = 0;

  err = gatherVertexData(dcGid, preflight);


  err = gatherFaceData(dcGid, preflight);


  err = gatherEdgeData(dcGid, preflight);

  // Now finally close the group
  H5Gclose(dcGid); // Close the Data Container Group

  HDF_ERROR_HANDLER_ON

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerReader::gatherVertexData(hid_t dcGid, bool preflight)
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
      SurfaceMesh::DataStructures::VertList_t::Pointer vertices = SurfaceMesh::DataStructures::VertList_t::CreateArray(1, DREAM3D::PointData::SurfaceMeshNodes);
      getSurfaceMeshDataContainer()->setVertices(vertices);
    }
  }
  else
  {
    err = readVertices(dcGid);
    if (err < 0)
    {
    }
  }
  // This will conditionally read all the MeshVertLinks data if preflight is true
  err = readMeshVertLinks(dcGid, preflight);
  if (err < 0)
  {
  }

  // Read all the Vertex Attribute data
  std::vector<std::string> readNames;
  err = readGroupsData(dcGid, H5_POINT_DATA_GROUP_NAME, preflight, readNames);
  if(err == -154) // The group was not in the file so just ignore that error
  {
    err = 0;
  }
  for(size_t i = 0; i < readNames.size(); ++i)
  {
    addCreatedPointData(readNames[i]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerReader::gatherFaceData(hid_t dcGid, bool preflight)
{
  int err = 0;
  std::vector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  if (true == preflight)
  {
    err = H5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::FacesName, dims, type_class, type_size);
    if (err >= 0)
    {
      StructArray<SurfaceMesh::DataStructures::Face_t>::Pointer triangles = StructArray<SurfaceMesh::DataStructures::Face_t>::CreateArray(1, DREAM3D::FaceData::SurfaceMeshTriangles);
      getSurfaceMeshDataContainer()->setFaces(triangles);
    }
  }
  else
  {
    err = readFaces(dcGid);
  }

  // This will conditionally read all the MeshTriangleNeighborLists data if preflight is true
  err = readMeshTriangleNeighborLists(dcGid, preflight);
  if (err < 0)
  {
  }

  // Read all the Face Attribute data
  std::vector<std::string> readNames;
  err = readGroupsData(dcGid, H5_FACE_DATA_GROUP_NAME, preflight, readNames);
  if(err == -154) // The group was not in the file so just ignore that error
  {
    err = 0;
  }
  for(size_t i = 0; i < readNames.size(); ++i)
  {
    addCreatedFaceData(readNames[i]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerReader::gatherEdgeData(hid_t dcGid, bool preflight)
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
//      StructArray<SurfaceMesh::DataStructures::Edge_t>::Pointer edges = StructArray<SurfaceMesh::DataStructures::Edge_t>::CreateArray(1, DREAM3D::EdgeData::SurfaceMeshEdges);
//      getSurfaceMeshDataContainer()->setEdges(edges);
//    }
//  }
//  else
//  {
//    err = readEdges(dcGid);
//  }

  // Read all the Edge Attribute data
  std::vector<std::string> readNames;
  err = readGroupsData(dcGid, H5_EDGE_DATA_GROUP_NAME, preflight, readNames);
  if(err == -154) // The group was not in the file so just ignore that error
  {
    err = 0;
  }
  for(size_t i = 0; i < readNames.size(); ++i)
  {
    addCreatedEdgeData(readNames[i]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerReader::readVertices(hid_t dcGid)
{
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
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
  SurfaceMesh::DataStructures::VertList_t::Pointer verticesPtr = SurfaceMesh::DataStructures::VertList_t::CreateArray(dims[0],  DREAM3D::PointData::SurfaceMeshNodes);
  // Read the data
  SurfaceMesh::DataStructures::Float_t* data = reinterpret_cast<SurfaceMesh::DataStructures::Float_t*>(verticesPtr->GetPointer(0));
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
int SurfaceMeshDataContainerReader::readMeshVertLinks(hid_t dcGid, bool preflight)
{
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  SurfaceMesh::DataStructures::VertList_t::Pointer verticesPtr = sm->getVertices();
  if (NULL == verticesPtr.get())
  {
    return -1;
  }

  MeshVertLinks::Pointer meshVertLinks = MeshVertLinks::New();

  size_t nVerts = verticesPtr->GetNumberOfTuples();
  herr_t err = 0;
  std::vector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size = 0;
  err = H5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::MeshVertLinksName, dims, type_class, type_size);
  if (err < 0)
  {
    return err;
  }
  else
  {
    sm->setMeshVertLinks(meshVertLinks);
  }

  if (false == preflight && type_size > 0)
  {
    //Read the array into the buffer
    std::vector<uint8_t> buffer;
    err = H5Lite::readVectorDataset(dcGid, DREAM3D::HDF5::MeshVertLinksName, buffer);
    if (err < 0)
    {
      setErrorCondition(err);
      notifyErrorMessage("Error Reading Vertex Links from Data file", getErrorCondition());
      return err;
    }
    meshVertLinks->deserializeLinks(buffer, nVerts);
    sm->setMeshVertLinks(meshVertLinks);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerReader::readVertexAttributeData(hid_t dcGid)
{
  int err = -1;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerReader::readFaces(hid_t dcGid)
{
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();

  herr_t err = 0;
  std::vector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  // Get the size of the data set so we know what to allocate
  err = H5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::FacesName, dims, type_class, type_size);
  if (err < 0)
  {
    setErrorCondition(err);
    notifyErrorMessage("No Vertex Links in Data file", getErrorCondition());
    return err;
  }
  // Allocate the Face_t structures
  SurfaceMesh::DataStructures::FaceList_t::Pointer facesPtr = SurfaceMesh::DataStructures::FaceList_t::CreateArray(dims[0], DREAM3D::FaceData::SurfaceMeshTriangles);
  // We need this to properly use H5Lite because the data is stored as int32_t in 5 columns
  int32_t* data = reinterpret_cast<int32_t*>(facesPtr->GetPointer(0));
  // Read the data from the file
  err = H5Lite::readPointerDataset(dcGid, DREAM3D::HDF5::FacesName, data);
  if (err < 0) {
    setErrorCondition(err);
    notifyErrorMessage("Error Writing Face List to DREAM3D file", getErrorCondition());
    return err;
  }
  sm->setFaces(facesPtr);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerReader::readMeshTriangleNeighborLists(hid_t dcGid, bool preflight)
{
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  SurfaceMesh::DataStructures::FaceList_t::Pointer facesPtr = sm->getFaces();
  if (NULL == facesPtr.get())
  {
    return -1;
  }

  MeshTriangleNeighbors::Pointer meshTriangleNeighbors = MeshTriangleNeighbors::New();

  size_t nFaces= facesPtr->GetNumberOfTuples();
  herr_t err = 0;
  std::vector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size = 0;
  err = H5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::MeshTriangleNeighborLists, dims, type_class, type_size);
  if (err < 0)
  {
    return err;
  }
  else
  {
    sm->setMeshTriangleNeighborLists(meshTriangleNeighbors);
  }

  if(false == preflight && type_size > 0)
  {
    //Read the array into the buffer
    std::vector<uint8_t> buffer;
    err = H5Lite::readVectorDataset(dcGid, DREAM3D::HDF5::MeshTriangleNeighborLists, buffer);
    if (err < 0)
    {
      setErrorCondition(err);
      notifyErrorMessage("Error Reading Face Neighbor Links from Data file", getErrorCondition());
    }
    else
    {
      meshTriangleNeighbors->deserializeLinks(buffer, nFaces);
      sm->setMeshTriangleNeighborLists(meshTriangleNeighbors);
    }
  }

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerReader::readFaceAttributeData(hid_t dcGid)
{
  herr_t err = -1;
  // H5_FACE_DATA_GROUP_NAME
  return err;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerReader::readEdges(hid_t dcGid)
{
  herr_t err = -1;

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerReader::readGroupsData(hid_t dcGid, const std::string &groupName, bool preflight, std::vector<std::string> &namesRead)
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
      if(groupName.compare(H5_POINT_DATA_GROUP_NAME) == 0)
      {
        getSurfaceMeshDataContainer()->addPointData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_FACE_DATA_GROUP_NAME) == 0)
      {
        getSurfaceMeshDataContainer()->addFaceData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_EDGE_DATA_GROUP_NAME) == 0)
      {
        getSurfaceMeshDataContainer()->addEdgeData(dPtr->GetName(), dPtr);
      }
    }

  }
  H5Gclose(gid); // Close the Cell Group
  return err;
}
