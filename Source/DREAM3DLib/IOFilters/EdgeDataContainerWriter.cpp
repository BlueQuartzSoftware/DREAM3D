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

#include "EdgeDataContainerWriter.h"



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
EdgeDataContainerWriter::EdgeDataContainerWriter() :
VertexDataContainerWriter()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeDataContainerWriter::~EdgeDataContainerWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainerWriter::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainerWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainerWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
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
void EdgeDataContainerWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  EdgeDataContainer* m = getEdgeDataContainer();

  if(NULL == m)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer is missing", getErrorCondition());
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
void EdgeDataContainerWriter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainerWriter::execute()
{
  int err = 0;

  setErrorCondition(err);
  EdgeDataContainer* sm = getEdgeDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Surfacemesh DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  // Create the HDF5 Group for the Data Container
  err = H5Utilities::createGroupsFromPath(DREAM3D::HDF5::EdgeDataContainerName.toLatin1().data(), getHdfFileId());
  if (err < 0)
  {
    ss.str("");
    ss << "Error creating HDF Group " << DREAM3D::HDF5::EdgeDataContainerName << "\n";
    setErrorCondition(-60);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    return;
  }
  hid_t dcGid = H5Gopen(getHdfFileId(), DREAM3D::HDF5::EdgeDataContainerName.toLatin1().data(), H5P_DEFAULT );
  if (dcGid < 0)
  {
    ss.str("");
    ss << "Error opening Group " << DREAM3D::HDF5::EdgeDataContainerName << "\n";
    setErrorCondition(-61);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    return;
  }

  // Add some VTK hints into the group
  err = createVtkObjectGroup(DREAM3D::HDF5::EdgeDataContainerName, H5_VTK_POLYDATA);
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


  err = writeEdgeFieldData(dcGid);
  if (err < 0)
  {
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }

  err = writeEdgeEnsembleData(dcGid);
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
void EdgeDataContainerWriter::writeXdmfGridHeader()
{
  //if (m_WriteXdmfFile == false || getXdmfOStream() == NULL)
  //{
  //  return;
  //}
  //FaceArray::Pointer faces = getEdgeDataContainer()->getFaces();
  //if (NULL == faces.get())
  //{
  //  return;
  //}
  //VertexArray::Pointer verts = getEdgeDataContainer()->getVertices();
  //if(NULL == verts.get())
  //{
  //  return;
  //}

  //std::ostream& out = *getXdmfOStream();
  //out << "  <Grid Name=\"SurfaceMesh DataContainer\">" << "\n";

  //out << "    <Topology TopologyType=\"Triangle\" NumberOfElements=\"" << faces->getNumberOfTuples() << "\">" << "\n";
  //out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << faces->getNumberOfTuples() << " 3\">" << "\n";
  //ssize_t nameSize = H5Fget_name(getHdfFileId(), NULL, 0) + 1;
  //QVector<char> nameBuffer(nameSize, 0);
  //nameSize = H5Fget_name(getHdfFileId(), &(nameBuffer.front()), nameSize);
  //QString hdfFileName(&(nameBuffer.front()), nameSize);
  //hdfFileName = QFileInfo::filename(hdfFileName);
  //out << "        " << hdfFileName << ":/EdgeDataContainer/Faces" << "\n";
  //out << "      </DataItem>" << "\n";
  //out << "    </Topology>" << "\n";

  //out << "    <Geometry Type=\"XYZ\">" << "\n";
  //out << "      <DataItem Format=\"HDF\"  Dimensions=\"" << verts->getNumberOfTuples() << " 3\" NumberType=\"Float\" Precision=\"4\">" << "\n";
  //out << "        " << hdfFileName << ":/EdgeDataContainer/Vertices" << "\n";
  //out << "      </DataItem>" << "\n";
  //out << "    </Geometry>" << "\n";
  //out << "" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainerWriter::writeXdmfGridFooter()
{
  //if (m_WriteXdmfFile == false || getXdmfOStream() == NULL)
  //{
  //  return;
  //}
  //FaceArray::Pointer faces = getEdgeDataContainer()->getFaces();
  //if (NULL == faces.get())
  //{
  //  return;
  //}
  //std::ostream& out = *getXdmfOStream();
  //out << "  </Grid>" << "\n";
  //out << "    <!-- *************** END OF SurfaceMesh DataContainer *************** -->" << "\n";
  //out << "\n";
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EdgeDataContainerWriter::writeXdmfAttributeDataHelper(int numComp, const QString &attrType,
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
    dimStr << array->getNumberOfTuples() << " " << array->GetNumberOfComponents();
    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem Format=\"HDF\" Dimensions=\"" << dimStr.str() <<  "\" ";
    out << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";

    ssize_t nameSize = H5Fget_name(getHdfFileId(), NULL, 0) + 1;
    QVector<char> nameBuffer(nameSize, 0);
    nameSize = H5Fget_name(getHdfFileId(), &(nameBuffer.front()), nameSize);

    QString hdfFileName(&(nameBuffer.front()), nameSize);
    hdfFileName = QFileInfo::filename(hdfFileName);

    out << "        " << hdfFileName << ":/EdgeDataContainer/" << groupName << "/" << array->GetName() << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n" << "\n";
  }
  else
  {
    //First Slab
    out << "    <Attribute Name=\"" << array->GetName() << " (Field 0)\" ";
    out << "AttributeType=\"" << attrType << "\" ";
    dimStr1 << array->getNumberOfTuples() << " " << array->GetNumberOfComponents();
    dimStr1half << array->getNumberOfTuples() << " " << (array->GetNumberOfComponents()/2);
    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"" << dimStr1half.str() <<  "\" ";
    out << "Type=\"HyperSlab\" " << "Name=\"" << array->GetName() << " (Field 0)\" >" << "\n";
    out << "        <DataItem Dimensions=\"3 2\" " << "Format=\"XML\" >" << "\n";
    out << "          0        0" << "\n";
    out << "          1        1" << "\n";
    out << "          " << dimStr1half.str() << " </DataItem>" << "\n";
    out << "\n";
    out << "        <DataItem Format=\"HDF\" Dimensions=\"" << dimStr1.str() << "\" " << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";
    ssize_t nameSize = H5Fget_name(getHdfFileId(), NULL, 0) + 1;
    QVector<char> nameBuffer(nameSize, 0);
    nameSize = H5Fget_name(getHdfFileId(), &(nameBuffer.front()), nameSize);
    QString hdfFileName(&(nameBuffer.front()), nameSize);
    hdfFileName = QFileInfo::filename(hdfFileName);
    out << "        " << hdfFileName << ":/EdgeDataContainer/" << groupName << "/" << array->GetName() << "\n";
    out << "        </DataItem>" << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n" << "\n";

    //Second Slab
    out << "    <Attribute Name=\"" << array->GetName() << " (Field 1)\" ";
    out << "AttributeType=\"" << attrType << "\" ";
    dimStr2 << array->getNumberOfTuples() << " " << array->GetNumberOfComponents();
    dimStr2half << array->getNumberOfTuples() << " " << (array->GetNumberOfComponents()/2);
    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"" << dimStr2half.str() <<  "\" ";
    out << "Type=\"HyperSlab\" " << "Name=\"" << array->GetName() << " (Field 1)\" >" << "\n";
    out << "        <DataItem Dimensions=\"3 2\" " << "Format=\"XML\" >" << "\n";
    out << "          0        " << (array->GetNumberOfComponents()/2) << "\n";
    out << "          1        1" << "\n";
    out << "          " << dimStr2half.str() << " </DataItem>" << "\n";
    out << "\n";
    out << "        <DataItem Format=\"HDF\" Dimensions=\"" << dimStr2.str() << "\" " << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";
    ssize_t nameSize2 = H5Fget_name(getHdfFileId(), NULL, 0) + 1;
    QVector<char> nameBuffer2(nameSize2, 0);
    nameSize2 = H5Fget_name(getHdfFileId(), &(nameBuffer2.front()), nameSize2);
    QString hdfFileName2(&(nameBuffer2.front()), nameSize2);
    hdfFileName2 = QFileInfo::filename(hdfFileName2);
    out << "        " << hdfFileName2 << ":/EdgeDataContainer/" << groupName << "/" << array->GetName() << "\n";
    out << "        </DataItem>" << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n" << "\n";
  }
  return out.str();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainerWriter::writeXdmfAttributeData(const QString &groupName, IDataArray::Pointer array, const QString &centering)
{
#if 0
      <Attribute Name="Node Type" Center="Node">
      <DataItem Format="HDF" DataType="char" Precision="1" Dimensions="43029 1">
        MC_IsoGG_50cubed_55grains_Bounded_Multi.dream3d:/EdgeDataContainer/POINT_DATA/SurfaceMeshNodeType
      </DataItem>
    </Attribute>
#endif
  if (m_WriteXdmfFile == false || getXdmfOStream() == NULL)
  { return; }



  QTextStream& out = *getXdmfOStream();
  int precision = 0;
  QString xdmfTypeName;
  array->GetXdmfTypeAndSize(xdmfTypeName, precision);
  if (0 == precision)
  {
    out << "<!-- " << array->GetName() << " has unkown type or unsupported type or precision for XDMF to understand" << " -->" << "\n";
    return;
  }
  int numComp = array->GetNumberOfComponents();
  QString attrType = "Scalar";
  if(numComp > 2) attrType = "Vector";

  QString block = writeXdmfAttributeDataHelper(numComp,attrType,groupName,array,centering,precision,xdmfTypeName);

  out << block << "\n";
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainerWriter::createVtkObjectGroup(const QString &hdfGroupPath, const char* vtkDataObjectType)
{
  // qDebug() << "   vtkH5DataWriter::WritePoints()" << "\n";
  herr_t err = H5Utilities::createGroupsFromPath(hdfGroupPath, getHdfFileId());
  if (err < 0)
  {
    qDebug() << "Error creating HDF Group " << hdfGroupPath << "\n";
  }
  err = H5Lite::writeStringAttribute(getHdfFileId(), hdfGroupPath, H5_VTK_DATA_OBJECT, vtkDataObjectType );
  if(err < 0)
  {
    qDebug() << "Error writing string attribute to HDF Group " << hdfGroupPath << "\n";
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainerWriter::writeMeshLinks(hid_t dcGid)
{
  MeshLinks::Pointer links = getEdgeDataContainer()->getMeshLinks();
  if (NULL == links.get())
  {
    return 0;
  }
  EdgeDataContainer* sm = getEdgeDataContainer();
  VertexArray::Pointer verticesPtr = sm->getVertices();
  if (NULL == verticesPtr.get())
  {
    return -1;
  }

  herr_t err = -1;
  size_t total = 0;
  size_t nVerts = verticesPtr->getNumberOfTuples();
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
int EdgeDataContainerWriter::writeEdges(hid_t dcGid)
{
  herr_t err = 0;
 // notifyWarningMessage("Edge Data is NOT currently implemented. If you need this functionality please contact the authors.", -10995);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EdgeDataContainerWriter::writeEdgeAttributeData(hid_t dcGid)
{

  int err = 0;
  EdgeDataContainer* sm = getEdgeDataContainer();

  // Write the Face Data
  err = H5Utilities::createGroupsFromPath(H5_EDGE_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {
    ss.str("");
    ss << "Error creating HDF Group " << H5_EDGE_DATA_GROUP_NAME << "\n";
    setErrorCondition(-63);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    return err;
  }
  hid_t cellGroupId = H5Gopen(dcGid, H5_EDGE_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {
    ss.str("");
    ss << "Error writing string attribute to HDF Group " << H5_EDGE_DATA_GROUP_NAME << "\n";
    setErrorCondition(-64);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    return err;
  }
  NameListType names = sm->getEdgeArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    ss.str("");
    ss << "Writing Edge Data '" << *iter << "' to HDF5 File" << "\n";
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
int EdgeDataContainerWriter::writeEdgeFieldData(hid_t dcGid)
{

  int err = 0;
  EdgeDataContainer* m = getEdgeDataContainer();

#if WRITE_FIELD_XDMF
// Get the name of the .dream3d file that we are writing to:
  ssize_t nameSize = H5Fget_name(getHdfFileId(), NULL, 0) + 1;
  QVector<char> nameBuffer(nameSize, 0);
  nameSize = H5Fget_name(getHdfFileId(), &(nameBuffer.front()), nameSize);

  QString hdfFileName(&(nameBuffer.front()), nameSize);
  hdfFileName = QFileInfo::filename(hdfFileName);
  QString xdmfGroupPath = QString(":/") + VolumeDataContainer::ClassName() + QString("/") + H5_FIELD_DATA_GROUP_NAME;
#endif

  int64_t volDims[3] = { 0,0,0 };


  // Write the Field Data
  err = H5Utilities::createGroupsFromPath(H5_FIELD_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {
    qDebug() << "Error creating HDF Group " << H5_FIELD_DATA_GROUP_NAME << "\n";
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
    ss << "Error opening field Group " << H5_FIELD_DATA_GROUP_NAME << "\n";
    setErrorCondition(-65);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }

  size_t total = 0;
  typedef QVector<IDataArray*> VectorOfIDataArrays_t;
  VectorOfIDataArrays_t neighborListArrays;

  NameListType names = m->getEdgeFieldArrayNameList();
  if (names.size() > 0)
  {
    IDataArray::Pointer array = m->getEdgeFieldData(names.front());
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
    IDataArray::Pointer array = m->getEdgeFieldData(*iter);
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
int EdgeDataContainerWriter::writeEdgeEnsembleData(hid_t dcGid)
{

  int err = 0;
  EdgeDataContainer* m = getEdgeDataContainer();

  // Write the Ensemble data
  err = H5Utilities::createGroupsFromPath(H5_ENSEMBLE_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {
    ss.str("");
    ss << "Error creating HDF Group " << H5_ENSEMBLE_DATA_GROUP_NAME << "\n";
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
    ss << "Error opening ensemble Group " << H5_ENSEMBLE_DATA_GROUP_NAME << "\n";
    setErrorCondition(-67);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  NameListType names = m->getEdgeEnsembleArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    IDataArray::Pointer array = m->getEdgeEnsembleData(*iter);
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

