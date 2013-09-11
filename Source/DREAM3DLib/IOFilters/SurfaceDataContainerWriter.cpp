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

#include "SurfaceDataContainerWriter.h"



#include "MXA/Utilities/MXAFileInfo.h"


#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"

#include "DREAM3DLib/Common/NeighborList.hpp"

#define WRITE_FIELD_XDMF 0

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
SurfaceDataContainerWriter::SurfaceDataContainerWriter() :
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
SurfaceDataContainerWriter::~SurfaceDataContainerWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainerWriter::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainerWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
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
void SurfaceDataContainerWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  
  SurfaceDataContainer* m = getSurfaceDataContainer();

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
void SurfaceDataContainerWriter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainerWriter::execute()
{
  int err = 0;
  
  setErrorCondition(err);
  SurfaceDataContainer* sm = getSurfaceDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Surfacemesh DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  // Create the HDF5 Group for the Data Container
  err = H5Utilities::createGroupsFromPath(DREAM3D::HDF5::SurfaceDataContainerName.c_str(), m_HdfFileId);
  if (err < 0)
  {
    ss.str("");
    ss << "Error creating HDF Group " << DREAM3D::HDF5::SurfaceDataContainerName << std::endl;
    setErrorCondition(-60);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    return;
  }
  hid_t dcGid = H5Gopen(m_HdfFileId, DREAM3D::HDF5::SurfaceDataContainerName.c_str(), H5P_DEFAULT );
  if (dcGid < 0)
  {
    ss.str("");
    ss << "Error opening Group " << DREAM3D::HDF5::SurfaceDataContainerName << std::endl;
    setErrorCondition(-61);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    return;
  }

  // Add some VTK hints into the group
  err = createVtkObjectGroup(DREAM3D::HDF5::SurfaceDataContainerName, H5_VTK_POLYDATA);
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

  err = writeMeshLinks(dcGid);
  if (err < 0)
  {
    return;
  }

  err = writeVertexAttributeData(dcGid);
  if (err < 0)
  {
    return;
  }


  err = writeFaces(dcGid);
  if (err < 0)
  {
    return;
  }

  err = writeMeshFaceNeighborLists(dcGid);
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


  err = writeFieldData(dcGid);
  if (err < 0)
  {
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }

  err = writeEnsembleData(dcGid);
  if (err < 0)
  {
    H5Gclose(dcGid); // Close the Data Container Group
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
void SurfaceDataContainerWriter::setXdmfOStream(std::ostream *xdmf)
{
  m_XdmfPtr = xdmf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainerWriter::writeXdmfGridHeader()
{
  if (m_WriteXdmfFile == false || m_XdmfPtr == NULL)
  {
    return;
  }
  DREAM3D::Mesh::FaceListPointer_t faces = getSurfaceDataContainer()->getFaces();
  if (NULL == faces.get())
  {
    return;
  }
  DREAM3D::Mesh::VertListPointer_t verts = getSurfaceDataContainer()->getVertices();
  if(NULL == verts.get())
  {
    return;
  }

  std::ostream& out = *m_XdmfPtr;
  out << "  <Grid Name=\"SurfaceMesh DataContainer\">" << std::endl;

  out << "    <Topology TopologyType=\"Triangle\" NumberOfElements=\"" << faces->GetNumberOfTuples() << "\">" << std::endl;
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << faces->GetNumberOfTuples() << " 3\">" << std::endl;
  ssize_t nameSize = H5Fget_name(m_HdfFileId, NULL, 0) + 1;
  QVector<char> nameBuffer(nameSize, 0);
  nameSize = H5Fget_name(m_HdfFileId, &(nameBuffer.front()), nameSize);
  QString hdfFileName(&(nameBuffer.front()), nameSize);
  hdfFileName = MXAFileInfo::filename(hdfFileName);
  out << "        " << hdfFileName << ":/SurfaceDataContainer/Faces" << std::endl;
  out << "      </DataItem>" << std::endl;
  out << "    </Topology>" << std::endl;

  out << "    <Geometry Type=\"XYZ\">" << std::endl;
  out << "      <DataItem Format=\"HDF\"  Dimensions=\"" << verts->GetNumberOfTuples() << " 3\" NumberType=\"Float\" Precision=\"4\">" << std::endl;
  out << "        " << hdfFileName << ":/SurfaceDataContainer/Vertices" << std::endl;
  out << "      </DataItem>" << std::endl;
  out << "    </Geometry>" << std::endl;
  out << "" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainerWriter::writeXdmfGridFooter()
{
  if (m_WriteXdmfFile == false || m_XdmfPtr == NULL)
  {
    return;
  }
  DREAM3D::Mesh::FaceListPointer_t faces = getSurfaceDataContainer()->getFaces();
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
QString SurfaceDataContainerWriter::writeXdmfAttributeDataHelper(int numComp, const QString &attrType,
                                                                              const QString &groupName,
                                                                              IDataArray::Pointer array,
                                                                              const QString &centering,
                                                                              int precision, const QString &xdmfTypeName)
{
  QTextStream out;
  QTextStream dimStr;
  QTextStream dimStr1;
  QTextStream dimStr1half;
  QTextStream dimStr2;
  QTextStream dimStr2half;

  if((numComp%2) == 1)
  {
    out << "    <Attribute Name=\"" << array->GetName() << "\" ";
    out << "AttributeType=\"" << attrType << "\" ";
    dimStr << array->GetNumberOfTuples() << " " << array->GetNumberOfComponents();
    out << "Center=\"" << centering << "\">" << std::endl;
    // Open the <DataItem> Tag
    out << "      <DataItem Format=\"HDF\" Dimensions=\"" << dimStr.str() <<  "\" ";
    out << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << std::endl;

    ssize_t nameSize = H5Fget_name(m_HdfFileId, NULL, 0) + 1;
    QVector<char> nameBuffer(nameSize, 0);
    nameSize = H5Fget_name(m_HdfFileId, &(nameBuffer.front()), nameSize);

    QString hdfFileName(&(nameBuffer.front()), nameSize);
    hdfFileName = MXAFileInfo::filename(hdfFileName);

    out << "        " << hdfFileName << ":/SurfaceDataContainer/" << groupName << "/" << array->GetName() << std::endl;
    out << "      </DataItem>" << std::endl;
    out << "    </Attribute>" << std::endl << std::endl;
  }
  else
  {
    //First Slab
    out << "    <Attribute Name=\"" << array->GetName() << " (Field 0)\" ";
    out << "AttributeType=\"" << attrType << "\" ";
    dimStr1 << array->GetNumberOfTuples() << " " << array->GetNumberOfComponents();
    dimStr1half << array->GetNumberOfTuples() << " " << (array->GetNumberOfComponents()/2);
    out << "Center=\"" << centering << "\">" << std::endl;
    // Open the <DataItem> Tag
    out << "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"" << dimStr1half.str() <<  "\" ";
    out << "Type=\"HyperSlab\" " << "Name=\"" << array->GetName() << " (Field 0)\" >" << std::endl;
    out << "        <DataItem Dimensions=\"3 2\" " << "Format=\"XML\" >" << std::endl;
    out << "          0        0" << std::endl;
    out << "          1        1" << std::endl;
    out << "          " << dimStr1half.str() << " </DataItem>" << std::endl;
    out << std::endl;
    out << "        <DataItem Format=\"HDF\" Dimensions=\"" << dimStr1.str() << "\" " << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << std::endl;
    ssize_t nameSize = H5Fget_name(m_HdfFileId, NULL, 0) + 1;
    QVector<char> nameBuffer(nameSize, 0);
    nameSize = H5Fget_name(m_HdfFileId, &(nameBuffer.front()), nameSize);
    QString hdfFileName(&(nameBuffer.front()), nameSize);
    hdfFileName = MXAFileInfo::filename(hdfFileName);
    out << "        " << hdfFileName << ":/SurfaceDataContainer/" << groupName << "/" << array->GetName() << std::endl;
    out << "        </DataItem>" << std::endl;
    out << "      </DataItem>" << std::endl;
    out << "    </Attribute>" << std::endl << std::endl;

    //Second Slab
    out << "    <Attribute Name=\"" << array->GetName() << " (Field 1)\" ";
    out << "AttributeType=\"" << attrType << "\" ";
    dimStr2 << array->GetNumberOfTuples() << " " << array->GetNumberOfComponents();
    dimStr2half << array->GetNumberOfTuples() << " " << (array->GetNumberOfComponents()/2);
    out << "Center=\"" << centering << "\">" << std::endl;
    // Open the <DataItem> Tag
    out << "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"" << dimStr2half.str() <<  "\" ";
    out << "Type=\"HyperSlab\" " << "Name=\"" << array->GetName() << " (Field 1)\" >" << std::endl;
    out << "        <DataItem Dimensions=\"3 2\" " << "Format=\"XML\" >" << std::endl;
    out << "          0        " << (array->GetNumberOfComponents()/2) << std::endl;
    out << "          1        1" << std::endl;
    out << "          " << dimStr2half.str() << " </DataItem>" << std::endl;
    out << std::endl;
    out << "        <DataItem Format=\"HDF\" Dimensions=\"" << dimStr2.str() << "\" " << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << std::endl;
    ssize_t nameSize2 = H5Fget_name(m_HdfFileId, NULL, 0) + 1;
    QVector<char> nameBuffer2(nameSize2, 0);
    nameSize2 = H5Fget_name(m_HdfFileId, &(nameBuffer2.front()), nameSize2);
    QString hdfFileName2(&(nameBuffer2.front()), nameSize2);
    hdfFileName2 = MXAFileInfo::filename(hdfFileName2);
    out << "        " << hdfFileName2 << ":/SurfaceDataContainer/" << groupName << "/" << array->GetName() << std::endl;
    out << "        </DataItem>" << std::endl;
    out << "      </DataItem>" << std::endl;
    out << "    </Attribute>" << std::endl << std::endl;
  }
  return out.str();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainerWriter::writeXdmfAttributeData(const QString &groupName, IDataArray::Pointer array, const QString &centering)
{
#if 0
      <Attribute Name="Node Type" Center="Node">
      <DataItem Format="HDF" DataType="char" Precision="1" Dimensions="43029 1">
        MC_IsoGG_50cubed_55grains_Bounded_Multi.dream3d:/SurfaceDataContainer/POINT_DATA/SurfaceMeshNodeType
      </DataItem>
    </Attribute>
#endif
  if (m_WriteXdmfFile == false || m_XdmfPtr == NULL)
  { return; }



  std::ostream& out = *m_XdmfPtr;
  QTextStream dimStr;
  int precision = 0;
  QString xdmfTypeName;
  array->GetXdmfTypeAndSize(xdmfTypeName, precision);
  if (0 == precision)
  {
    out << "<!-- " << array->GetName() << " has unkown type or unsupported type or precision for XDMF to understand" << " -->" << std::endl;
    return;
  }
  int numComp = array->GetNumberOfComponents();
  QString attrType = "Scalar";
  if(numComp > 2) attrType = "Vector";

  QString block = writeXdmfAttributeDataHelper(numComp,attrType,groupName,array,centering,precision,xdmfTypeName);

  out << block << std::endl;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerWriter::createVtkObjectGroup(const QString &hdfGroupPath, const char* vtkDataObjectType)
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
int SurfaceDataContainerWriter::writeVertices(hid_t dcGid)
{
  SurfaceDataContainer* sm = getSurfaceDataContainer();
  DREAM3D::Mesh::VertList_t::Pointer verticesPtr = sm->getVertices();
  if (NULL == verticesPtr.get())
  {
    return -1;
  }

  int32_t rank = 2;
  hsize_t dims[2] = {verticesPtr->GetNumberOfTuples(), DREAM3D::Mesh::k_VertexNumElements};

  float* data = reinterpret_cast<float*>(verticesPtr->GetPointer(0));

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
int SurfaceDataContainerWriter::writeMeshLinks(hid_t dcGid)
{
  MeshLinks::Pointer links = getSurfaceDataContainer()->getMeshLinks();
  if (NULL == links.get())
  {
    return 0;
  }
  SurfaceDataContainer* sm = getSurfaceDataContainer();
  DREAM3D::Mesh::VertList_t::Pointer verticesPtr = sm->getVertices();
  if (NULL == verticesPtr.get())
  {
    return -1;
  }

  herr_t err = -1;
  size_t total = 0;
  size_t nVerts = verticesPtr->GetNumberOfTuples();
  for(size_t v = 0; v < nVerts; ++v)
  {
    total += links->getNumberOfFaces(v);
  }

  size_t totalBytes = nVerts * sizeof(uint16_t) + total * sizeof(int32_t);

  // Allocate a flat array to copy the data into
  QVector<uint8_t> buffer(totalBytes, 0);
  uint8_t* bufPtr = &(buffer.front());
  size_t offset = 0;

  for(size_t v = 0; v < nVerts; ++v)
  {
    uint16_t ncells = links->getNumberOfFaces(v);
    int32_t* cells = links->getFaceListPointer(v);
    ::memcpy(bufPtr + offset, &ncells, sizeof(uint16_t));
    offset += sizeof(uint16_t);
    ::memcpy(bufPtr + offset, cells, ncells*sizeof(int32_t) );
    offset += ncells*sizeof(int32_t);
  }

  int32_t rank = 1;
  hsize_t dims[1] = {totalBytes};

  err = H5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::MeshLinksName, rank, dims, bufPtr);
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
int SurfaceDataContainerWriter::writeVertexAttributeData(hid_t dcGid)
{
  
  int err = 0;
  SurfaceDataContainer* sm = getSurfaceDataContainer();

  // Write the Vertex Data
  err = H5Utilities::createGroupsFromPath(H5_VERTEX_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {
    ss.str("");
    ss << "Error creating HDF Group " << H5_VERTEX_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-63);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  hid_t cellGroupId = H5Gopen(dcGid, H5_VERTEX_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {
    ss.str("");
    ss << "Error writing string attribute to HDF Group " << H5_VERTEX_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-64);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  NameListType names = sm->getVertexArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    ss.str("");
    ss << "Writing Cell Data '" << *iter << "' to HDF5 File" << std::endl;
    notifyStatusMessage(ss.str());
    IDataArray::Pointer array = sm->getVertexData(*iter);
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
    writeXdmfAttributeData(H5_VERTEX_DATA_GROUP_NAME, array, "Node");
  }
  H5Gclose(cellGroupId); // Close the Cell Group
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerWriter::writeMeshFaceNeighborLists(hid_t dcGid)
{
  MeshFaceNeighbors::Pointer links = getSurfaceDataContainer()->getMeshFaceNeighborLists();
  if (NULL == links.get())
  {
    return 0;
  }
  SurfaceDataContainer* sm = getSurfaceDataContainer();
  DREAM3D::Mesh::FaceList_t::Pointer facesPtr = sm->getFaces();
  if (NULL == facesPtr.get())
  {
    return -1;
  }

  herr_t err = -1;
  size_t total = 0;
  size_t nFaces = facesPtr->GetNumberOfTuples();
  for(size_t v = 0; v < nFaces; ++v)
  {
    total += links->getNumberOfFaces(v);
  }

  size_t totalBytes = nFaces * sizeof(uint16_t) + total * sizeof(int32_t);

  // Allocate a flat array to copy the data into
  QVector<uint8_t> buffer(totalBytes, 0);
  uint8_t* bufPtr = &(buffer.front());
  size_t offset = 0;

  for(size_t v = 0; v < nFaces; ++v)
  {
    uint16_t ncells = links->getNumberOfFaces(v);
    int32_t* cells = links->getNeighborListPointer(v);
    ::memcpy(bufPtr + offset, &ncells, sizeof(uint16_t));
    offset += sizeof(uint16_t);
    ::memcpy(bufPtr + offset, cells, ncells*sizeof(int32_t) );
    offset += ncells*sizeof(int32_t);
  }

  int32_t rank = 1;
  hsize_t dims[1] = {totalBytes};

  err = H5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::MeshFaceNeighborLists, rank, dims, bufPtr);
  if (err < 0)
  {
    notifyErrorMessage("Error writing the Mesh Face Neighbor Lists", -998);
    return err;
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerWriter::writeFaces(hid_t dcGid)
{
  SurfaceDataContainer* sm = getSurfaceDataContainer();
  DREAM3D::Mesh::FaceList_t::Pointer facesPtr = sm->getFaces();
  if (facesPtr.get() == NULL)
  {
    return -1;
  }

  int32_t rank = 2; // THIS NEEDS TO BE THE SAME AS THE NUMBER OF ELEMENTS IN THE Structure from SurfaceMesh::DataStruc
  hsize_t dims[2] = {facesPtr->GetNumberOfTuples(), DREAM3D::Mesh::k_FaceNumElements};

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
int SurfaceDataContainerWriter::writeFaceAttributeData(hid_t dcGid)
{
  
  int err = 0;
  SurfaceDataContainer* sm = getSurfaceDataContainer();

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
int SurfaceDataContainerWriter::writeEdges(hid_t dcGid)
{
  herr_t err = 0;
 // notifyWarningMessage("Edge Data is NOT currently implemented. If you need this functionality please contact the authors.", -10995);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerWriter::writeEdgeAttributeData(hid_t dcGid)
{
  
  int err = 0;
  SurfaceDataContainer* sm = getSurfaceDataContainer();

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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerWriter::writeFieldData(hid_t dcGid)
{
  
  int err = 0;
  SurfaceDataContainer* m = getSurfaceDataContainer();

#if WRITE_FIELD_XDMF
// Get the name of the .dream3d file that we are writing to:
  ssize_t nameSize = H5Fget_name(m_HdfFileId, NULL, 0) + 1;
  QVector<char> nameBuffer(nameSize, 0);
  nameSize = H5Fget_name(m_HdfFileId, &(nameBuffer.front()), nameSize);

  QString hdfFileName(&(nameBuffer.front()), nameSize);
  hdfFileName = MXAFileInfo::filename(hdfFileName);
  QString xdmfGroupPath = QString(":/") + VolumeDataContainer::ClassName() + QString("/") + H5_FIELD_DATA_GROUP_NAME;
#endif

  int64_t volDims[3] = { 0,0,0 };


  // Write the Field Data
  err = H5Utilities::createGroupsFromPath(H5_FIELD_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {
    std::cout << "Error creating HDF Group " << H5_FIELD_DATA_GROUP_NAME << std::endl;
    return err;
  }
  err = H5Lite::writeStringAttribute(dcGid, H5_FIELD_DATA_GROUP_NAME, H5_NAME, H5_FIELD_DATA_DEFAULT);
  if(err < 0)
  {
    return err;
  }

  hid_t fieldGroupId = H5Gopen(dcGid, H5_FIELD_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {
    ss.str("");
    ss << "Error opening field Group " << H5_FIELD_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-65);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }

  size_t total = 0;
  typedef QVector<IDataArray*> VectorOfIDataArrays_t;
  VectorOfIDataArrays_t neighborListArrays;

  NameListType names = m->getFieldArrayNameList();
  if (names.size() > 0)
  {
    IDataArray::Pointer array = m->getCellFieldData(names.front());
    total = array->GetSize();
    volDims[0] = total;
    volDims[1] = 1;
    volDims[2] = 1;
#if WRITE_FIELD_XDMF
    ss.str("");
    ss << "Field Data (" << total << ")";
    writeFieldXdmfGridHeader(total, ss.str());
#endif
  }
  // Now loop over all the field data and write it out, possibly wrapping it with XDMF code also.
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    IDataArray::Pointer array = m->getCellFieldData(*iter);
    if (array->getTypeAsString().compare(NeighborList<int>::ClassName()) == 0)
    {
      neighborListArrays.push_back(array.get());
    }
    else if (NULL != array.get())
    {
      err = array->writeH5Data(fieldGroupId);
      if(err < 0)
      {
        ss.str("");
        ss << "Error writing field array '" << *iter << "' to the HDF5 File";
        addErrorMessage(getHumanLabel(), ss.str(), err);
        setErrorCondition(err);
        H5Gclose(fieldGroupId); // Close the Cell Group
        H5Gclose(dcGid); // Close the Data Container Group
        return err;
      }
#if WRITE_FIELD_XDMF
      array->writeXdmfAttribute( *m_XdmfPtr, volDims, hdfFileName, xdmfGroupPath, " (Field)");
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
    total = (*pair).first;
    VectorOfIDataArrays_t& arrays = (*pair).second;
    volDims[0] = total;
    volDims[1] = 1;
    volDims[2] = 1;
    #if WRITE_FIELD_XDMF
    ss.str("");
    ss << "Neighbor Data (" << total << ")";
    writeFieldXdmfGridHeader(total, ss.str());
    #endif
    for(VectorOfIDataArrays_t::iterator iter = arrays.begin(); iter < arrays.end(); ++iter)
    {
      err = (*iter)->writeH5Data(fieldGroupId);
      if(err < 0)
      {
        ss.str("");
        ss << "Error writing field array '" << *iter << "' to the HDF5 File";
        addErrorMessage(getHumanLabel(), ss.str(), err);
        setErrorCondition(err);
        H5Gclose(fieldGroupId); // Close the Cell Group
        H5Gclose(dcGid); // Close the Data Container Group
        return err;
      }
#if WRITE_FIELD_XDMF
      (*iter)->writeXdmfAttribute( *m_XdmfPtr, volDims, hdfFileName, xdmfGroupPath, " (Neighbor Data)");
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
int SurfaceDataContainerWriter::writeEnsembleData(hid_t dcGid)
{
  
  int err = 0;
  SurfaceDataContainer* m = getSurfaceDataContainer();

  // Write the Ensemble data
  err = H5Utilities::createGroupsFromPath(H5_ENSEMBLE_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {
    ss.str("");
    ss << "Error creating HDF Group " << H5_ENSEMBLE_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-66);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  err = H5Lite::writeStringAttribute(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, H5_NAME, H5_ENSEMBLE_DATA_DEFAULT);

  hid_t ensembleGid = H5Gopen(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {
    ss.str("");
    ss << "Error opening ensemble Group " << H5_ENSEMBLE_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-67);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  NameListType names = m->getEnsembleArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    IDataArray::Pointer array = m->getEnsembleData(*iter);
    err = array->writeH5Data(ensembleGid);
    if(err < 0)
    {
      ss.str("");
      ss << "Error writing Ensemble array '" << *iter << "' to the HDF5 File";
      addErrorMessage(getHumanLabel(), ss.str(), err);
      setErrorCondition(err);
      H5Gclose(ensembleGid); // Close the Cell Group
      H5Gclose(dcGid); // Close the Data Container Group
      return err;
    }
  }

  H5Gclose(ensembleGid);

  return err;
}

