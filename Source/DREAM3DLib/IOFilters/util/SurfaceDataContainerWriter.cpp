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



#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "DREAM3DLib/DataArrays/NeighborList.hpp"

#define WRITE_FIELD_XDMF 0

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceDataContainerWriter::SurfaceDataContainerWriter() :
EdgeDataContainerWriter()
{
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
void SurfaceDataContainerWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());
  if (NULL == dc)
  {
    QString ss = QObject::tr("DataContainer Pointer was NULL and Must be valid.%1(%2)").arg(__FILE__).arg(__LINE__);
    addErrorMessage(getHumanLabel(), ss, -2);
    setErrorCondition(-1);
    return;
  }
  if(getHdfGroupId() < 0)
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

  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());
  if (NULL == dc)
  {
    QString ss = QObject::tr("DataContainer Pointer was NULL and Must be valid.%1(%2)").arg(__FILE__).arg(__LINE__);
    addErrorMessage(getHumanLabel(), ss, -2);
    setErrorCondition(-1);
    return;
  }

  setErrorCondition(0);

  hid_t dcGid = H5Gopen(getHdfGroupId(), getDataContainer()->getName().toLatin1().data(), H5P_DEFAULT );

  // Add some VTK hints into the group
  err = createVtkObjectGroup(getDataContainer()->getName(), H5_VTK_POLYDATA);
  if (err < 0)  {
    return;
  }

  writeXdmfGridHeader();

  err = writeMeshData(dcGid);
  if (err < 0)
  {
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }

  err = writeFaceData(dcGid, H5_FACE_DATA_GROUP_NAME);
  if (err < 0)
  {
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }

  err = writeFaceFieldData(dcGid, H5_FACE_FIELD_DATA_GROUP_NAME);
  if (err < 0)
  {
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }

  err = writeFaceEnsembleData(dcGid, H5_FACE_ENSEMBLE_DATA_GROUP_NAME);
  if (err < 0)
  {
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }


  // Now finally close the group and the HDf5 File
  H5Gclose(dcGid); // Close the Data Container Group


  writeXdmfGridFooter(QString("Surface Data"));


  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainerWriter::writeXdmfGridHeader()
{

  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());

  if (getWriteXdmfFile() == false || getXdmfOStream() == NULL)
  {
    return;
  }
  FaceArray::Pointer faces = dc->getFaces();
  if (NULL == faces.get())
  {
    return;
  }
  VertexArray::Pointer verts = dc->getVertices();
  if(NULL == verts.get())
  {
    return;
  }

  QString hdfFileName = QH5Utilities::fileNameFromFileId(getHdfGroupId());

  QTextStream& out = *getXdmfOStream();
  out << "  <Grid Name=\"SurfaceMesh DataContainer\">" << "\n";

  out << "    <Topology TopologyType=\"Triangle\" NumberOfElements=\"" << faces->getNumberOfTuples() << "\">" << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << faces->getNumberOfTuples() << " 3\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/SurfaceDataContainer/Faces" << "\n";
  out << "      </DataItem>" << "\n";
  out << "    </Topology>" << "\n";

  out << "    <Geometry Type=\"XYZ\">" << "\n";
  out << "      <DataItem Format=\"HDF\"  Dimensions=\"" << verts->getNumberOfTuples() << " 3\" NumberType=\"Float\" Precision=\"4\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/SurfaceDataContainer/Vertices" << "\n";
  out << "      </DataItem>" << "\n";
  out << "    </Geometry>" << "\n";
  out << "" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainerWriter::writeXdmfGridFooter(const QString &label)
{
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());

  if (getWriteXdmfFile() == false || getXdmfOStream() == NULL)
  {
    return;
  }
  FaceArray::Pointer faces = dc->getFaces();
  if (NULL == faces.get())
  {
    return;
  }
  QTextStream& out = *getXdmfOStream();
  out << "  </Grid>" << "\n";
  out << "    <!-- *************** END OF SurfaceMesh DataContainer " << label << " *************** -->" << "\n";
  out << "\n";
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
  QString buf;
  QTextStream out(&buf);

  QString hdfFileName = QH5Utilities::fileNameFromFileId(getHdfGroupId());

  QString dimStr = QString::number(array->getNumberOfTuples()) + QString(" ") + QString::number(array->GetNumberOfComponents());
  QString dimStrHalf = QString::number(array->getNumberOfTuples()) + QString(" ") + QString::number(array->GetNumberOfComponents()/2);

  if((numComp%2) == 1)
  {
    out << "    <Attribute Name=\"" << array->GetName() << "\" ";
    out << "AttributeType=\"" << attrType << "\" ";
    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem Format=\"HDF\" Dimensions=\"" << dimStr <<  "\" ";
    out << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";
    out << "        " << hdfFileName << ":/DataContainers/SurfaceDataContainer/" << groupName << "/" << array->GetName() << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n" << "\n";
  }
  else
  {
    //First Slab
    out << "    <Attribute Name=\"" << array->GetName() << " (Field 0)\" ";
    out << "AttributeType=\"" << attrType << "\" ";

    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"" << dimStrHalf <<  "\" ";
    out << "Type=\"HyperSlab\" " << "Name=\"" << array->GetName() << " (Field 0)\" >" << "\n";
    out << "        <DataItem Dimensions=\"3 2\" " << "Format=\"XML\" >" << "\n";
    out << "          0        0" << "\n";
    out << "          1        1" << "\n";
    out << "          " << dimStrHalf << " </DataItem>" << "\n";
    out << "\n";
    out << "        <DataItem Format=\"HDF\" Dimensions=\"" << dimStr << "\" " << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";

    out << "        " << hdfFileName << ":/DataContainers/SurfaceDataContainer/" << groupName << "/" << array->GetName() << "\n";
    out << "        </DataItem>" << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n" << "\n";

    //Second Slab
    out << "    <Attribute Name=\"" << array->GetName() << " (Field 1)\" ";
    out << "AttributeType=\"" << attrType << "\" ";

    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"" << dimStrHalf <<  "\" ";
    out << "Type=\"HyperSlab\" " << "Name=\"" << array->GetName() << " (Field 1)\" >" << "\n";
    out << "        <DataItem Dimensions=\"3 2\" " << "Format=\"XML\" >" << "\n";
    out << "          0        " << (array->GetNumberOfComponents()/2) << "\n";
    out << "          1        1" << "\n";
    out << "          " << dimStrHalf << " </DataItem>" << "\n";
    out << "\n";
    out << "        <DataItem Format=\"HDF\" Dimensions=\"" << dimStr << "\" " << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";
    out << "        " << hdfFileName << ":/DataContainers/SurfaceDataContainer/" << groupName << "/" << array->GetName() << "\n";
    out << "        </DataItem>" << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n" << "\n";
  }
  return buf;
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
  if (getWriteXdmfFile() == false || getXdmfOStream() == NULL)
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
int SurfaceDataContainerWriter::writeMeshData(hid_t dcGid)
{
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());

  herr_t err = 0;
  //first write the faces if they exist
  FaceArray::Pointer facesPtr = dc->getFaces();
  if (facesPtr.get() != NULL)
  {
    int32_t rank = 2; // THIS NEEDS TO BE THE SAME AS THE NUMBER OF ELEMENTS IN THE Structure from SurfaceMesh::DataStruc
    hsize_t dims[2] = {facesPtr->getNumberOfTuples(), 3};

    int32_t* data = reinterpret_cast<int32_t*>(facesPtr->getPointer(0));

    err = QH5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::FacesName, rank, dims, data);
    if (err < 0) 
    {
      setErrorCondition(err);
      notifyErrorMessage("Error Writing Face List to DREAM3D file", getErrorCondition());
    }

    //next write face neighbors if they exist
    Int32DynamicListArray::Pointer faceNeighbors = facesPtr->getFaceNeighbors();
    if (NULL != faceNeighbors.get())
    {
      size_t total = 0;
      size_t nFaces = facesPtr->getNumberOfTuples();
      for(size_t v = 0; v < nFaces; ++v)
      {
        total += faceNeighbors->getNumberOfElements(v);
      }

      size_t totalBytes = nFaces * sizeof(uint16_t) + total * sizeof(int32_t);

      // Allocate a flat array to copy the data into
      QVector<uint8_t> buffer(totalBytes, 0);
      uint8_t* bufPtr = &(buffer.front());
      size_t offset = 0;

      for(size_t v = 0; v < nFaces; ++v)
      {
        uint16_t ncells = faceNeighbors->getNumberOfElements(v);
        int32_t* cells = faceNeighbors->getElementListPointer(v);
        ::memcpy(bufPtr + offset, &ncells, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        ::memcpy(bufPtr + offset, cells, ncells*sizeof(int32_t) );
        offset += ncells*sizeof(int32_t);
      }

      int32_t rank = 1;
      hsize_t dims[1] = {totalBytes};

      err = QH5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::FaceNeighbors, rank, dims, bufPtr);
      if (err < 0)
      {
        notifyErrorMessage("Error writing the Face Neighbors", -998);
        return err;
      }
    }

    //last write faces containing verts if they exist
    Int32DynamicListArray::Pointer facesContainingVert = facesPtr->getFacesContainingVert();
    if (NULL == facesContainingVert.get())
    {
      return 0;
    }
    VertexArray::Pointer verticesPtr = dc->getVertices();
    if (NULL == verticesPtr.get())
    {
      return -1;
    }

    herr_t err = -1;
    size_t total = 0;
    size_t nVerts = verticesPtr->getNumberOfTuples();
    for(size_t v = 0; v < nVerts; ++v)
    {
      total += facesContainingVert->getNumberOfElements(v);
    }

    size_t totalBytes = nVerts * sizeof(uint16_t) + total * sizeof(int32_t);

    // Allocate a flat array to copy the data into
    QVector<uint8_t> buffer(totalBytes, 0);
    uint8_t* bufPtr = &(buffer.front());
    size_t offset = 0;

    for(size_t v = 0; v < nVerts; ++v)
    {
      uint16_t ncells = facesContainingVert->getNumberOfElements(v);
      int32_t* cells = facesContainingVert->getElementListPointer(v);
      ::memcpy(bufPtr + offset, &ncells, sizeof(uint16_t));
      offset += sizeof(uint16_t);
      ::memcpy(bufPtr + offset, cells, ncells*sizeof(int32_t) );
      offset += ncells*sizeof(int32_t);
    }

    rank = 1;
    dims[0] = totalBytes;

    err = QH5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::FacesContainingVert, rank, dims, bufPtr);
    if (err < 0)
    {
      notifyErrorMessage("Error writing the Faces Containing Verts", -999);
      return err;
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerWriter::writeFaceData(hid_t dcGid, QString groupName)
{

  int err = 0;
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());

  //QString groupName(H5_FACE_DATA_GROUP_NAME)

  // Write the Face Data
 err = QH5Utilities::createGroupsFromPath(groupName, dcGid);
  if(err < 0)
  {
    QString ss = QObject::tr("Error creating HDF Group ").arg(groupName);
    setErrorCondition(-63);
    addErrorMessage(getHumanLabel(), ss, err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  hid_t cellGroupId = H5Gopen(dcGid, groupName.toLatin1().data(), H5P_DEFAULT);
  if(err < 0)
  {
    QString ss = QObject::tr("Error writing string attribute to HDF Group %1").arg(groupName);
    setErrorCondition(-64);
    addErrorMessage(getHumanLabel(), ss, err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  NameListType names = dc->getFaceArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    QString ss = QObject::tr("Writing Data '%1' to HDF5 File").arg(*iter);
    notifyStatusMessage(ss);
    IDataArray::Pointer array = dc->getFaceData(*iter);
    err = array->writeH5Data(cellGroupId);
    if(err < 0)
    {
      QString ss = QObject::tr("Error writing array '%1' to the HDF5 File").arg(*iter);
      addErrorMessage(getHumanLabel(), ss, err);
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
int SurfaceDataContainerWriter::writeFaceFieldData(hid_t dcGid, QString groupName)
{

  int err = 0;
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());

  //QString groupName(H5_FIELD_DATA_GROUP_NAME)
#if WRITE_FIELD_XDMF
// Get the name of the .dream3d file that we are writing to:
  ssize_t nameSize = H5Fget_name(getHdfGroupId(), NULL, 0) + 1;
  QVector<char> nameBuffer(nameSize, 0);
  nameSize = H5Fget_name(getHdfGroupId(), &(nameBuffer.front()), nameSize);

  QString hdfFileName(&(nameBuffer.front()), nameSize);
  hdfFileName = QFileInfo::filename(hdfFileName);
  QString xdmfGroupPath = QString(":/") + VolumeDataContainer::ClassName() + QString("/") + H5_FIELD_DATA_GROUP_NAME;
#endif

  int64_t volDims[3] = { 0,0,0 };

  // Write the Field Data
  // Write the Field Data
  err = QH5Utilities::createGroupsFromPath(groupName, dcGid);
  if(err < 0)
  {
    qDebug() << "Error creating HDF Group " << groupName << "\n";
    return err;
  }
  err = QH5Lite::writeStringAttribute(dcGid, groupName, H5_NAME, H5_FIELD_DATA_DEFAULT);
  if(err < 0)
  {
    return err;
  }
 hid_t fieldGroupId = H5Gopen(dcGid, groupName.toLatin1().data(), H5P_DEFAULT);
  if(err < 0)
  {
    QString ss = QObject::tr("Error opening field Group ").arg(groupName);
    setErrorCondition(-65);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }


  size_t total = 0;
  typedef QVector<IDataArray*> VectorOfIDataArrays_t;
  VectorOfIDataArrays_t neighborListArrays;

  NameListType names = dc->getFaceFieldArrayNameList();
  if (names.size() > 0)
  {
    IDataArray::Pointer array = dc->getFaceFieldData(names.front());
    total = array->GetSize();
    volDims[0] = total;
    volDims[1] = 1;
    volDims[2] = 1;
#if WRITE_FIELD_XDMF
    ss.str("");
    ss << "Field Data (" << total << ")";
    writeFieldXdmfGridHeader(total, ss);
#endif
  }
  // Now loop over all the field data and write it out, possibly wrapping it with XDMF code also.
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    IDataArray::Pointer array = dc->getFaceFieldData(*iter);
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
    ss.str("");
    ss << "Neighbor Data (" << total << ")";
    writeFieldXdmfGridHeader(total, ss);
    #endif
    for(VectorOfIDataArrays_t::iterator iter = arrays.begin(); iter < arrays.end(); ++iter)
    {
      err = (*iter)->writeH5Data(fieldGroupId);
      if(err < 0)
      {
        QString ss = QObject::tr("Error writing field array '%1' to the HDF5 File").arg( (*iter)->GetName());
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
    writeXdmfGridFooter(ss);
#endif

  }

  H5Gclose(fieldGroupId);
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerWriter::writeFaceEnsembleData(hid_t dcGid, QString groupName)
{

  int err = 0;
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());

  // QString groupName(H5_ENSEMBLE_DATA_GROUP_NAME)
  //H5_ENSEMBLE_DATA_DEFAULT

  // Write the Ensemble data
  err = QH5Utilities::createGroupsFromPath(groupName, dcGid);
  if(err < 0)
  {
    QString ss = QObject::tr("Error creating HDF Group ").arg(groupName);
    setErrorCondition(-63);
    addErrorMessage(getHumanLabel(), ss, err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  err = QH5Lite::writeStringAttribute(dcGid, groupName, H5_NAME, H5_ENSEMBLE_DATA_DEFAULT);

  hid_t ensembleGid = H5Gopen(dcGid, groupName.toLatin1().data(), H5P_DEFAULT);
  if(err < 0)
  {
    QString ss = QObject::tr("Error opening ensemble Group %1").arg(groupName);
    setErrorCondition(-67);
    addErrorMessage(getHumanLabel(), ss, err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  NameListType names = dc->getFaceEnsembleArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    IDataArray::Pointer array = dc->getFaceEnsembleData(*iter);
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

