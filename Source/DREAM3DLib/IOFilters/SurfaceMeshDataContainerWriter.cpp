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

#include "SurfaceMeshDataContainerWriter.h"

#include "MXA/Utilities/MXAFileInfo.h"


#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"


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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshDataContainerWriter::SurfaceMeshDataContainerWriter() :
  AbstractFilter(),
  m_HdfFileId(-1),
  m_WriteXdmfFile(false),
  m_XdmfPtr(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshDataContainerWriter::~SurfaceMeshDataContainerWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainerWriter::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainerWriter::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainerWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainerWriter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainerWriter::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Surfacemesh DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  // Create the HDF5 Group for the Data Container
  err = H5Utilities::createGroupsFromPath(DREAM3D::HDF5::SurfaceMeshDataContainerName.c_str(), m_HdfFileId);
  if (err < 0)
  {
    ss.str("");
    ss << "Error creating HDF Group " << DREAM3D::HDF5::SurfaceMeshDataContainerName << std::endl;
    setErrorCondition(-60);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    return;
  }
  hid_t dcGid = H5Gopen(m_HdfFileId, DREAM3D::HDF5::SurfaceMeshDataContainerName.c_str(), H5P_DEFAULT );
  if (dcGid < 0)
  {
    ss.str("");
    ss << "Error opening Group " << DREAM3D::HDF5::SurfaceMeshDataContainerName << std::endl;
    setErrorCondition(-61);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    return;
  }

  // Add some VTK hints into the group
  err = createVtkObjectGroup(DREAM3D::HDF5::SurfaceMeshDataContainerName, H5_VTK_POLYDATA);
  if (err < 0)  {
    return;
  }

  writeXdmfGridHeader();


  H5GroupAutoCloser dcGidAutoCloser(&dcGid);

  err = writeVertices(dcGid);
  if (err < 0)
  {
    return;
  }

  err = writeMeshVertLinks(dcGid);
  if (err < 0)
  {
    return;
  }

  err = writeVertexAttributeData(dcGid);
  if (err < 0)
  {
    return;
  }


  err = writeTriangles(dcGid);
  if (err < 0)
  {
    return;
  }

  err = writeMeshTriangleNeighborLists(dcGid);
  if (err < 0)
  {
    return;
  }

  err = writeFaceAttributeData(dcGid);
  if (err < 0)
  {
    return;
  }

  err = writeEdges(dcGid);
  if (err < 0)
  {
    return;
  }

  err = writeEdgeAttributeData(dcGid);
  if (err < 0)
  {
    return;
  }


  // Now finally close the group and the HDf5 File
  H5Gclose(dcGid); // Close the Data Container Group
  dcGid = -1;

  writeXdmfGridFooter();


  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainerWriter::setXdmfOStream(std::ostream *xdmf)
{
  m_XdmfPtr = xdmf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainerWriter::writeXdmfGridHeader()
{
  if (m_WriteXdmfFile == false || m_XdmfPtr == NULL)
  {
    return;
  }
  SurfaceMesh::DataStructures::FaceListPointer_t faces = getSurfaceMeshDataContainer()->getFaces();
  if (NULL == faces.get())
  {
    return;
  }
  SurfaceMesh::DataStructures::VertListPointer_t verts = getSurfaceMeshDataContainer()->getVertices();
  if(NULL == verts.get())
  {
    return;
  }

  std::ostream& out = *m_XdmfPtr;
  out << "  <Grid Name=\"SurfaceMesh DataContainer\">" << std::endl;

  out << "    <Topology TopologyType=\"Triangle\" NumberOfElements=\"" << faces->GetNumberOfTuples() << "\">" << std::endl;
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << faces->GetNumberOfTuples() << " 3\">" << std::endl;
  ssize_t nameSize = H5Fget_name(m_HdfFileId, NULL, 0) + 1;
  std::vector<char> nameBuffer(nameSize, 0);
  nameSize = H5Fget_name(m_HdfFileId, &(nameBuffer.front()), nameSize);
  std::string hdfFileName(&(nameBuffer.front()), nameSize);
  hdfFileName = MXAFileInfo::filename(hdfFileName);
  out << "        " << hdfFileName << ":/SurfaceMeshDataContainer/Faces" << std::endl;
  out << "      </DataItem>" << std::endl;
  out << "    </Topology>" << std::endl;

  out << "    <Geometry Type=\"XYZ\">" << std::endl;
  out << "      <DataItem Format=\"HDF\"  Dimensions=\"" << verts->GetNumberOfTuples() << " 3\" NumberType=\"Float\" Precision=\"4\">" << std::endl;
  out << "        " << hdfFileName << ":/SurfaceMeshDataContainer/Vertices" << std::endl;
  out << "      </DataItem>" << std::endl;
  out << "    </Geometry>" << std::endl;
  out << "" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainerWriter::writeXdmfGridFooter()
{
  if (m_WriteXdmfFile == false || m_XdmfPtr == NULL)
  {
    return;
  }
  SurfaceMesh::DataStructures::FaceListPointer_t faces = getSurfaceMeshDataContainer()->getFaces();
  if (NULL == faces.get())
  {
    return;
  }
  std::ostream& out = *m_XdmfPtr;
  out << "  </Grid>" << std::endl;
  out << "    <!-- *************** END OF SurfaceMesh DataContainer *************** -->" << std::endl;
  out << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainerWriter::writeXdmfAttributeData(const std::string &groupName, IDataArray::Pointer array, const std::string &centering)
{
#if 0
      <Attribute Name="Node Type" Center="Node">
      <DataItem Format="HDF" DataType="char" Precision="1" Dimensions="43029 1">
        MC_IsoGG_50cubed_55grains_Bounded_Multi.dream3d:/SurfaceMeshDataContainer/POINT_DATA/SurfaceMeshNodeType
      </DataItem>
    </Attribute>
#endif
  if (m_WriteXdmfFile == false || m_XdmfPtr == NULL)
  { return; }



  std::ostream& out = *m_XdmfPtr;
  std::stringstream dimStr;
  int precision = 0;
  std::string xdmfTypeName;
  array->GetXdmfTypeAndSize(xdmfTypeName, precision);
  if (0 == precision)
  {
    out << "<!-- " << array->GetName() << " has unkown type or unsupported type or precision for XDMF to understand" << " -->" << std::endl;
    return;
  }

  int numComp = array->GetNumberOfComponents();
  out << "    <Attribute Name=\"" << array->GetName() << "\" ";
  if (numComp == 1)
  {
    out << "AttributeType=\"Scalar\" ";
    dimStr << array->GetNumberOfTuples() << " " << array->GetNumberOfComponents();
  }
  else
  {
    out << "AttributeType=\"Vector\" ";
    dimStr << array->GetNumberOfTuples() << " " << array->GetNumberOfComponents();
  }
  out << "Center=\"" << centering << "\">" << std::endl;
  // Open the <DataItem> Tag
  out << "      <DataItem Format=\"HDF\" Dimensions=\"" << dimStr.str() <<  "\" ";
  out << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << std::endl;

  ssize_t nameSize = H5Fget_name(m_HdfFileId, NULL, 0) + 1;
  std::vector<char> nameBuffer(nameSize, 0);
  nameSize = H5Fget_name(m_HdfFileId, &(nameBuffer.front()), nameSize);

  std::string hdfFileName(&(nameBuffer.front()), nameSize);
  hdfFileName = MXAFileInfo::filename(hdfFileName);

  out << "        " << hdfFileName << ":/SurfaceMeshDataContainer/" << groupName << "/" << array->GetName() << std::endl;
  out << "      </DataItem>" << std::endl;
  out << "    </Attribute>" << std::endl << std::endl;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerWriter::createVtkObjectGroup(const std::string &hdfGroupPath, const char* vtkDataObjectType)
{
  // std::cout << "   vtkH5DataWriter::WritePoints()" << std::endl;
  herr_t err = H5Utilities::createGroupsFromPath(hdfGroupPath, m_HdfFileId);
  if (err < 0)
  {
    std::cout << "Error creating HDF Group " << hdfGroupPath << std::endl;
  }
  err = H5Lite::writeStringAttribute(m_HdfFileId, hdfGroupPath, H5_VTK_DATA_OBJECT, vtkDataObjectType );
  if(err < 0)
  {
    std::cout << "Error writing string attribute to HDF Group " << hdfGroupPath << std::endl;
  }
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerWriter::writeVertices(hid_t dcGid)
{
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  SurfaceMesh::DataStructures::VertList_t::Pointer verticesPtr = sm->getVertices();
  if (NULL == verticesPtr.get())
  {
    return -1;
  }

  int32_t rank = 2;
  hsize_t dims[2] = {verticesPtr->GetNumberOfTuples(), SurfaceMesh::DataStructures::k_VertexNumElements};

  SurfaceMesh::DataStructures::Float_t* data = reinterpret_cast<SurfaceMesh::DataStructures::Float_t*>(verticesPtr->GetPointer(0));

  herr_t err = H5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::VerticesName, rank, dims, data);
  if (err < 0) {
    setErrorCondition(err);
    notifyErrorMessage("Error Writing Vertex List to DREAM3D file", getErrorCondition());
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerWriter::writeMeshVertLinks(hid_t dcGid)
{
  MeshVertLinks::Pointer links = getSurfaceMeshDataContainer()->getMeshVertLinks();
  if (NULL == links.get())
  {
    return 0;
  }
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  SurfaceMesh::DataStructures::VertList_t::Pointer verticesPtr = sm->getVertices();
  if (NULL == verticesPtr.get())
  {
    return -1;
  }

  herr_t err = -1;
  size_t total = 0;
  size_t nVerts = verticesPtr->GetNumberOfTuples();
  for(size_t v = 0; v < nVerts; ++v)
  {
    total += links->getNumberOfTriangles(v);
  }

  size_t totalBytes = nVerts * sizeof(uint16_t) + total * sizeof(int32_t);

  // Allocate a flat array to copy the data into
  std::vector<uint8_t> buffer(totalBytes, 0);
  uint8_t* bufPtr = &(buffer.front());
  size_t offset = 0;

  for(size_t v = 0; v < nVerts; ++v)
  {
    uint16_t ncells = links->getNumberOfTriangles(v);
    int32_t* cells = links->getTriangleListPointer(v);
    ::memcpy(bufPtr + offset, &ncells, sizeof(uint16_t));
    offset += sizeof(uint16_t);
    ::memcpy(bufPtr + offset, cells, ncells*sizeof(int32_t) );
    offset += ncells*sizeof(int32_t);
  }

  int32_t rank = 1;
  hsize_t dims[1] = {totalBytes};

  err = H5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::MeshVertLinksName, rank, dims, bufPtr);
  if (err < 0)
  {
    notifyErrorMessage("Error writing the Mesh Vert Links", -999);
    return err;
  }

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerWriter::writeVertexAttributeData(hid_t dcGid)
{
  std::stringstream ss;
  int err = 0;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();

  // Write the Vertex Data
  err = H5Utilities::createGroupsFromPath(H5_POINT_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {
    ss.str("");
    ss << "Error creating HDF Group " << H5_POINT_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-63);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  hid_t cellGroupId = H5Gopen(dcGid, H5_POINT_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {
    ss.str("");
    ss << "Error writing string attribute to HDF Group " << H5_POINT_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-64);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  NameListType names = sm->getPointArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    ss.str("");
    ss << "Writing Cell Data '" << *iter << "' to HDF5 File" << std::endl;
    notifyStatusMessage(ss.str());
    IDataArray::Pointer array = sm->getPointData(*iter);
    err = array->writeH5Data(cellGroupId);
    if(err < 0)
    {
      ss.str("");
      ss << "Error writing array '" << *iter << "' to the HDF5 File";
      addErrorMessage(getHumanLabel(), ss.str(), err);
      setErrorCondition(err);
      H5Gclose(cellGroupId); // Close the Cell Group
      H5Gclose(dcGid); // Close the Data Container Group
      return err;
    }
    writeXdmfAttributeData(H5_POINT_DATA_GROUP_NAME, array, "Node");
  }
  H5Gclose(cellGroupId); // Close the Cell Group
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerWriter::writeMeshTriangleNeighborLists(hid_t dcGid)
{
  MeshTriangleNeighbors::Pointer links = getSurfaceMeshDataContainer()->getMeshTriangleNeighborLists();
  if (NULL == links.get())
  {
    return 0;
  }
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  SurfaceMesh::DataStructures::FaceList_t::Pointer facesPtr = sm->getFaces();
  if (NULL == facesPtr.get())
  {
    return -1;
  }

  herr_t err = -1;
  size_t total = 0;
  size_t nFaces = facesPtr->GetNumberOfTuples();
  for(size_t v = 0; v < nFaces; ++v)
  {
    total += links->getNumberOfTriangles(v);
  }

  size_t totalBytes = nFaces * sizeof(uint16_t) + total * sizeof(int32_t);

  // Allocate a flat array to copy the data into
  std::vector<uint8_t> buffer(totalBytes, 0);
  uint8_t* bufPtr = &(buffer.front());
  size_t offset = 0;

  for(size_t v = 0; v < nFaces; ++v)
  {
    uint16_t ncells = links->getNumberOfTriangles(v);
    int32_t* cells = links->getNeighborListPointer(v);
    ::memcpy(bufPtr + offset, &ncells, sizeof(uint16_t));
    offset += sizeof(uint16_t);
    ::memcpy(bufPtr + offset, cells, ncells*sizeof(int32_t) );
    offset += ncells*sizeof(int32_t);
  }

  int32_t rank = 1;
  hsize_t dims[1] = {totalBytes};

  err = H5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::MeshTriangleNeighborLists, rank, dims, bufPtr);
  if (err < 0)
  {
    notifyErrorMessage("Error writing the Mesh Triangle Neighbor Lists", -998);
    return err;
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerWriter::writeTriangles(hid_t dcGid)
{
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  SurfaceMesh::DataStructures::FaceList_t::Pointer facesPtr = sm->getFaces();
  if (facesPtr.get() == NULL)
  {
    return -1;
  }

  int32_t rank = 2; // THIS NEEDS TO BE THE SAME AS THE NUMBER OF ELEMENTS IN THE Structure from SurfaceMesh::DataStruc
  hsize_t dims[2] = {facesPtr->GetNumberOfTuples(), SurfaceMesh::DataStructures::k_FaceNumElements};

  int32_t* data = reinterpret_cast<int32_t*>(facesPtr->GetPointer(0));

  herr_t err = H5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::FacesName, rank, dims, data);
  if (err < 0) {
    setErrorCondition(err);
    notifyErrorMessage("Error Writing Face List to DREAM3D file", getErrorCondition());
  }
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerWriter::writeFaceAttributeData(hid_t dcGid)
{
  std::stringstream ss;
  int err = 0;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();

  // Write the Face Data
  err = H5Utilities::createGroupsFromPath(H5_FACE_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {
    ss.str("");
    ss << "Error creating HDF Group " << H5_FACE_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-63);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  hid_t cellGroupId = H5Gopen(dcGid, H5_FACE_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {
    ss.str("");
    ss << "Error writing string attribute to HDF Group " << H5_FACE_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-64);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  NameListType names = sm->getFaceArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    ss.str("");
    ss << "Writing Face Data '" << *iter << "' to HDF5 File" << std::endl;
    notifyStatusMessage(ss.str());
    IDataArray::Pointer array = sm->getFaceData(*iter);
    err = array->writeH5Data(cellGroupId);
    if(err < 0)
    {
      ss.str("");
      ss << "Error writing array '" << *iter << "' to the HDF5 File";
      addErrorMessage(getHumanLabel(), ss.str(), err);
      setErrorCondition(err);
      H5Gclose(cellGroupId); // Close the Cell Group
      H5Gclose(dcGid); // Close the Data Container Group
      return err;
    }
    writeXdmfAttributeData(H5_FACE_DATA_GROUP_NAME, array, "Cell");
  }
  H5Gclose(cellGroupId); // Close the Cell Group
  return err;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerWriter::writeEdges(hid_t dcGid)
{
  herr_t err = 0;
  notifyWarningMessage("Edge Data is NOT currently implemented. If you need this functionality please contact the authors.", -10995);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshDataContainerWriter::writeEdgeAttributeData(hid_t dcGid)
{
  std::stringstream ss;
  int err = 0;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();

  // Write the Face Data
  err = H5Utilities::createGroupsFromPath(H5_EDGE_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {
    ss.str("");
    ss << "Error creating HDF Group " << H5_EDGE_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-63);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    return err;
  }
  hid_t cellGroupId = H5Gopen(dcGid, H5_EDGE_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {
    ss.str("");
    ss << "Error writing string attribute to HDF Group " << H5_EDGE_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-64);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    return err;
  }
  NameListType names = sm->getEdgeArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    ss.str("");
    ss << "Writing Edge Data '" << *iter << "' to HDF5 File" << std::endl;
    notifyStatusMessage(ss.str());
    IDataArray::Pointer array = sm->getEdgeData(*iter);
    err = array->writeH5Data(cellGroupId);
    if(err < 0)
    {
      ss.str("");
      ss << "Error writing Edge array '" << *iter << "' to the HDF5 File";
      addErrorMessage(getHumanLabel(), ss.str(), err);
      setErrorCondition(err);
      H5Gclose(cellGroupId); // Close the Cell Group
      return err;
    }
  }
  H5Gclose(cellGroupId); // Close the Cell Group
  return err;
}


