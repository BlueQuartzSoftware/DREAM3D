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
#include "VertexDataContainerWriter.h"


#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataContainers/VertexDataContainer.h"
#include "DREAM3DLib/DataContainers/VertexArray.hpp"
#include "DREAM3DLib/HDF5/VTKH5Constants.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"

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
VertexDataContainerWriter::VertexDataContainerWriter() :
  IOSupport(),
  m_WriteXdmfFile(false),
  m_XdmfOStream(NULL)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexDataContainerWriter::~VertexDataContainerWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainerWriter::execute()
{
  int err = 0;

  setErrorCondition(err);

  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  VertexDataContainer* dc = VertexDataContainer::SafePointerDownCast(getDataContainer());
  if(NULL == dc)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  // Create the HDF5 Group for the Data Container
  err = QH5Utilities::createGroupsFromPath(DREAM3D::HDF5::VertexDataContainerName.toLatin1().data(), getHdfFileId());
  if (err < 0)
  {
    QString ss = QObject::tr("Error creating HDF Group %1").arg(DREAM3D::HDF5::VertexDataContainerName);
    setErrorCondition(-60);
    addErrorMessage(getHumanLabel(), ss, err);
    return;
  }
  hid_t dcGid = H5Gopen(getHdfFileId(), DREAM3D::HDF5::VertexDataContainerName.toLatin1().data(), H5P_DEFAULT );
  if (dcGid < 0)
  {
    QString ss = QObject::tr("Error opening Group %1").arg(DREAM3D::HDF5::VertexDataContainerName);
    setErrorCondition(-61);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // Add some VTK hints into the group
  err = createVtkObjectGroup(DREAM3D::HDF5::VertexDataContainerName, H5_VTK_POLYDATA);
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

  err = writeVertexData(dcGid, H5_VERTEX_DATA_GROUP_NAME);
  if (err < 0)
  {
    return;
  }

  err = writeVertexFieldData(dcGid, H5_VERTEX_FIELD_DATA_GROUP_NAME);
  if (err < 0)
  {
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }

  err = writeVertexEnsembleData(dcGid, H5_VERTEX_ENSEMBLE_DATA_GROUP_NAME);
  if (err < 0)
  {
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }


  // Now finally close the group and the HDf5 File
  H5Gclose(dcGid); // Close the Data Container Group
  dcGid = -1;

  writeXdmfGridFooter();

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainerWriter::writeXdmfGridHeader()
{
  if (getWriteXdmfFile() == false || getXdmfOStream() == NULL)
  {
    return;
  }

  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  VertexDataContainer* dc = VertexDataContainer::SafePointerDownCast(getDataContainer());
  VertexArray::Pointer verts = dc->getVertices();
  if(NULL == verts.get())
  {
    return;
  }

  QString hdfFileName = QH5Utilities::fileNameFromFileId(getHdfFileId());

  QTextStream& out = *getXdmfOStream();
  out << "  <Grid Name=\"Vertex DataContainer\">" << "\n";

  out << "    <Topology TopologyType=\"Polyvertex\" NumberOfElements=\"" << verts->getNumberOfTuples() << "\">" << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << verts->getNumberOfTuples() << " 1\">" << "\n";

  out << "        " << hdfFileName << ":/VertexDataContainer/Vertices" << "\n";
  out << "      </DataItem>" << "\n";
  out << "    </Topology>" << "\n";

  out << "    <Geometry Type=\"XYZ\">" << "\n";
  out << "      <DataItem Format=\"HDF\"  Dimensions=\"" << verts->getNumberOfTuples() << " 3\" NumberType=\"Float\" Precision=\"4\">" << "\n";
  out << "        " << hdfFileName << ":/VertexDataContainer/Vertices" << "\n";
  out << "      </DataItem>" << "\n";
  out << "    </Geometry>" << "\n";
  out << "" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainerWriter::writeXdmfGridFooter()
{
  if (getWriteXdmfFile() == false || getXdmfOStream() == NULL)
  {
    return;
  }

  QTextStream& out = *getXdmfOStream();
  out << "  </Grid>" << "\n";
  out << "    <!-- *************** END OF Vertex DataContainer *************** -->" << "\n";
  out << "\n";
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VertexDataContainerWriter::writeXdmfAttributeDataHelper(int numComp, const QString &attrType,
                                                                              const QString &groupName,
                                                                              IDataArray::Pointer array,
                                                                              const QString &centering,
                                                                              int precision, const QString &xdmfTypeName)
{
  QString buf;
  QTextStream out(&buf);

  QString hdfFileName = QH5Utilities::fileNameFromFileId(getHdfFileId());
  QString dimStr = QString::number(array->getNumberOfTuples()) + QString(" ") + QString::number(array->GetNumberOfComponents());

  out << "    <Attribute Name=\"" << array->GetName() << "\" ";
  out << "AttributeType=\"" << attrType << "\" ";
  out << "Center=\"" << centering << "\">" << "\n";
  // Open the <DataItem> Tag
  out << "      <DataItem Format=\"HDF\" Dimensions=\"" << dimStr <<  "\" ";
  out << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";
  out << "        " << hdfFileName << ":/VertexDataContainer/" << groupName << "/" << array->GetName() << "\n";
  out << "      </DataItem>" << "\n";
  out << "    </Attribute>" << "\n" << "\n";

  return buf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainerWriter::writeXdmfAttributeData(const QString &groupName, IDataArray::Pointer array, const QString &centering)
{
#if 0
      <Attribute Name="Node Type" Center="Node">
      <DataItem Format="HDF" DataType="char" Precision="1" Dimensions="43029 1">
        MC_IsoGG_50cubed_55grains_Bounded_Multi.dream3d:/VertexDataContainer/POINT_DATA/VertexMeshNodeType
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
int VertexDataContainerWriter::writeVertices(hid_t dcGid)
{

  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  VertexDataContainer* dc = VertexDataContainer::SafePointerDownCast(getDataContainer());

  VertexArray::Pointer verts = dc->getVertices();
  if(NULL == verts.get())
  {
    return -1;
  }

  int32_t rank = 2;
  hsize_t dims[2] = {verts->getNumberOfTuples(), 3};

  float* data = reinterpret_cast<float*>(verts->getPointer(0));

  herr_t err = QH5Lite::writePointerDataset(dcGid, DREAM3D::HDF5::VerticesName, rank, dims, data);
  if (err < 0) {
    setErrorCondition(err);
    notifyErrorMessage("Error Writing Vertex List to DREAM3D file", getErrorCondition());
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexDataContainerWriter::writeVertexData(hid_t dcGid, QString groupName)
{

  int err = 0;
  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  VertexDataContainer* dc = VertexDataContainer::SafePointerDownCast(getDataContainer());

//  QString groupName(H5_VERTEX_DATA_GROUP_NAME);

  // Write the Vertex Data
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
  NameListType names = dc->getVertexArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    QString ss = QObject::tr("Writing Data '%1' to HDF5 File").arg(*iter);
    notifyStatusMessage(ss);
    IDataArray::Pointer array = dc->getVertexData(*iter);
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
    writeXdmfAttributeData(groupName, array, "Node");
  }
  H5Gclose(cellGroupId); // Close the Cell Group
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexDataContainerWriter::writeVertexFieldData(hid_t dcGid, QString groupName)
{

  int err = 0;
  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  VertexDataContainer* dc = VertexDataContainer::SafePointerDownCast(getDataContainer());

  // QString groupName(H5_VERTEX_FIELD_DATA_GROUP_NAME);
#if WRITE_FIELD_XDMF
// Get the name of the .dream3d file that we are writing to:
  ssize_t nameSize = H5Fget_name(m_HdfFileId, NULL, 0) + 1;
  QVector<char> nameBuffer(nameSize, 0);
  nameSize = H5Fget_name(m_HdfFileId, &(nameBuffer.front()), nameSize);

  QString hdfFileName(&(nameBuffer.front()), nameSize);
  hdfFileName = QFileInfo::filename(hdfFileName);
  QString xdmfGroupPath = QString(":/") + VolumeDataContainer::ClassName() + QString("/") + H5_FIELD_DATA_GROUP_NAME;
#endif

  int64_t volDims[3] = { 0,0,0 };


  // Write the Field Data
  err = QH5Utilities::createGroupsFromPath(groupName, dcGid);
  if(err < 0)
  {
    qDebug() << "Error creating HDF Group " << groupName << "\n";
    return err;
  }
  err = QH5Lite::writeStringAttribute(dcGid, groupName, H5_NAME, H5_VERTEX_FIELD_DATA_DEFAULT);
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

  NameListType names = dc->getVertexFieldArrayNameList();
  if (names.size() > 0)
  {
    IDataArray::Pointer array = dc->getVertexFieldData(names.front());
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
    IDataArray::Pointer array = dc->getVertexFieldData(*iter);
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
      (*iter)->writeXdmfAttribute( *m_XdmfPtr, volDims, hdfFileName, xdmfGroupPath, " (Neighbor Data)");
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
int VertexDataContainerWriter::writeVertexEnsembleData(hid_t dcGid, QString groupName)
{
  int err = 0;
  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  VertexDataContainer* dc = VertexDataContainer::SafePointerDownCast(getDataContainer());
  //QString groupName(H5_VERTEX_ENSEMBLE_DATA_GROUP_NAME);
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
  err = QH5Lite::writeStringAttribute(dcGid, groupName, H5_NAME, H5_VERTEX_ENSEMBLE_DATA_DEFAULT);

  hid_t ensembleGid = H5Gopen(dcGid, groupName.toLatin1().data(), H5P_DEFAULT);
  if(err < 0)
  {
    QString ss = QObject::tr("Error opening ensemble Group %1").arg(groupName);
    setErrorCondition(-67);
    addErrorMessage(getHumanLabel(), ss, err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  NameListType names = dc->getVertexEnsembleArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    IDataArray::Pointer array = dc->getVertexEnsembleData(*iter);
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

