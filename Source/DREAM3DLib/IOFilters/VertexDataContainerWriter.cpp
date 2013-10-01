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

#include <QtCore/QFileInfo>


#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"

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
VertexDataContainerWriter::~VertexDataContainerWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainerWriter::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainerWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexDataContainerWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
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
void VertexDataContainerWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  
  VertexDataContainer* m = getVertexDataContainer();

  if(NULL == m)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "Vertex DataContainer is missing", getErrorCondition());
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
void VertexDataContainerWriter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainerWriter::execute()
{
  int err = 0;
  setErrorCondition(err);
  VertexDataContainer* m = getVertexDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SolidMesh DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  // Create the HDF5 Group for the Data Container
  err = QH5Utilities::createGroupsFromPath(DREAM3D::HDF5::VertexDataContainerName.toLatin1().data(), m_HdfFileId);
  if (err < 0)
  {

    QString ss = QObject::tr("Error creating HDF Group %1").arg(DREAM3D::HDF5::VertexDataContainerName);
    setErrorCondition(-60);
    addErrorMessage(getHumanLabel(), ss, err);
    return;
  }
  hid_t dcGid = H5Gopen(m_HdfFileId, DREAM3D::HDF5::VertexDataContainerName.toLatin1().data(), H5P_DEFAULT );
  if (dcGid < 0)
  {

    QString ss = QObject::tr("Error opening Group %1").arg(DREAM3D::HDF5::VertexDataContainerName);
    setErrorCondition(-61);
    addErrorMessage(getHumanLabel(), ss, err);
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

  err = writeVertexAttributeData(dcGid);
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

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainerWriter::setXdmfOStream(QTextStream* xdmf)
{
  m_XdmfPtr = xdmf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainerWriter::writeXdmfGridHeader()
{
  if (m_WriteXdmfFile == false || m_XdmfPtr == NULL)
  {
    return;
  }
  DREAM3D::Mesh::VertListPointer_t verts = getVertexDataContainer()->getVertices();
  if(NULL == verts.get())
  {
    return;
  }

  ssize_t nameSize = H5Fget_name(m_HdfFileId, NULL, 0) + 1;
  QByteArray nameBuffer(nameSize, 0);
  nameSize = H5Fget_name(m_HdfFileId, nameBuffer.data(), nameSize);
  QString hdfFileName(nameBuffer);
  QFileInfo fi(hdfFileName);
  hdfFileName = fi.fileName();

  QTextStream& out = *m_XdmfPtr;
  out << "  <Grid Name=\"Vertex DataContainer\">";

  out << "    <Topology TopologyType=\"Polyvertex\" NumberOfElements=\"" << verts->GetNumberOfTuples() << "\">";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << verts->GetNumberOfTuples() << " 1\">";
  out << "        " << hdfFileName << ":/VertexDataContainer/Vertices";
  out << "      </DataItem>";
  out << "    </Topology>";

  out << "    <Geometry Type=\"XYZ\">";
  out << "      <DataItem Format=\"HDF\"  Dimensions=\"" << verts->GetNumberOfTuples() << " 3\" NumberType=\"Float\" Precision=\"4\">";
  out << "        " << hdfFileName << ":/VertexDataContainer/Vertices";
  out << "      </DataItem>";
  out << "    </Geometry>";
  out << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainerWriter::writeXdmfGridFooter()
{
  if (m_WriteXdmfFile == false || m_XdmfPtr == NULL)
  {
    return;
  }
  QTextStream& out = *m_XdmfPtr;
  out << "  </Grid>";
  out << "    <!-- *************** END OF Vertex DataContainer *************** -->";
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
  QTextStream out;
  QTextStream dimStr;
  QTextStream dimStr1;
  QTextStream dimStr1half;
  QTextStream dimStr2;
  QTextStream dimStr2half;

    ssize_t nameSize = H5Fget_name(m_HdfFileId, NULL, 0) + 1;
  QByteArray nameBuffer(nameSize, 0);
  nameSize = H5Fget_name(m_HdfFileId, nameBuffer.data(), nameSize);
  QString hdfFileName(nameBuffer);
  QFileInfo fi(hdfFileName);
  hdfFileName = fi.fileName();

  out << "    <Attribute Name=\"" << array->GetName() << "\" ";
  out << "AttributeType=\"" << attrType << "\" ";
  dimStr << array->GetNumberOfTuples() << " " << array->GetNumberOfComponents();
  out << "Center=\"" << centering << "\">";
  // Open the <DataItem> Tag
  out << "      <DataItem Format=\"HDF\" Dimensions=\"" << *(dimStr.string()) <<  "\" ";
  out << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >";


  out << "        " << hdfFileName << ":/VertexDataContainer/" << groupName << "/" << array->GetName();
  out << "      </DataItem>";
  out << "    </Attribute>";

  return QString( *(out.string()));
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
  if (m_WriteXdmfFile == false || m_XdmfPtr == NULL)
  { return; }



  QTextStream& out = *m_XdmfPtr;
  QTextStream dimStr;
  int precision = 0;
  QString xdmfTypeName;
  array->GetXdmfTypeAndSize(xdmfTypeName, precision);
  if (0 == precision)
  {
    out << "<!-- " << array->GetName() << " has unkown type or unsupported type or precision for XDMF to understand" << " -->";
    return;
  }
  int numComp = array->GetNumberOfComponents();
  QString attrType = "Scalar";
  if(numComp > 2) attrType = "Vector";

  QString block = writeXdmfAttributeDataHelper(numComp,attrType,groupName,array,centering,precision,xdmfTypeName);

  out << block;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexDataContainerWriter::createVtkObjectGroup(const QString &hdfGroupPath, const char* vtkDataObjectType)
{
  // qDebug() << "   vtkH5DataWriter::WritePoints()";
  herr_t err = QH5Utilities::createGroupsFromPath(hdfGroupPath, m_HdfFileId);
  if (err < 0)
  {
    qDebug() << "Error creating HDF Group " << hdfGroupPath;
  }
  err = QH5Lite::writeStringAttribute(m_HdfFileId, hdfGroupPath, H5_VTK_DATA_OBJECT, vtkDataObjectType );
  if(err < 0)
  {
    qDebug() << "Error writing string attribute to HDF Group " << hdfGroupPath;
  }
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexDataContainerWriter::writeVertices(hid_t dcGid)
{
  VertexDataContainer* sm = getVertexDataContainer();
  DREAM3D::Mesh::VertList_t::Pointer verticesPtr = sm->getVertices();
  if (NULL == verticesPtr.get())
  {
    return -1;
  }

  int32_t rank = 2;
  hsize_t dims[2] = {verticesPtr->GetNumberOfTuples(), DREAM3D::Mesh::k_VertexNumElements};

  DREAM3D::Mesh::Float_t* data = reinterpret_cast<DREAM3D::Mesh::Float_t*>(verticesPtr->GetPointer(0));

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
int VertexDataContainerWriter::writeVertexAttributeData(hid_t dcGid)
{
  
  int err = 0;
  VertexDataContainer* sm = getVertexDataContainer();

  // Write the Vertex Data
  err = QH5Utilities::createGroupsFromPath(H5_VERTEX_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {

    QString ss = QObject::tr("Error creating HDF Group %1").arg(H5_VERTEX_DATA_GROUP_NAME);
    setErrorCondition(-63);
    addErrorMessage(getHumanLabel(), ss, err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  hid_t cellGroupId = H5Gopen(dcGid, H5_VERTEX_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {

    QString ss = QObject::tr("Error writing string attribute to HDF Group %1").arg(H5_VERTEX_DATA_GROUP_NAME);
    setErrorCondition(-64);
    addErrorMessage(getHumanLabel(), ss, err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  NameListType names = sm->getVertexArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {

    QString ss = QObject::tr("Writing Cell Data '%1' to HDF5 File").arg(*iter);
    notifyStatusMessage(ss);
    IDataArray::Pointer array = sm->getVertexData(*iter);
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
    writeXdmfAttributeData(H5_VERTEX_DATA_GROUP_NAME, array, "Node");
  }
  H5Gclose(cellGroupId); // Close the Cell Group
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexDataContainerWriter::writeFieldData(hid_t dcGid)
{
  
  int err = 0;
  VertexDataContainer* m = getVertexDataContainer();

#if WRITE_FIELD_XDMF
// Get the name of the .dream3d file that we are writing to:
  ssize_t nameSize = H5Fget_name(m_HdfFileId, NULL, 0) + 1;
  std::vector<char> nameBuffer(nameSize, 0);
  nameSize = H5Fget_name(m_HdfFileId, &(nameBuffer.front()), nameSize);

  QString hdfFileName(&(nameBuffer.front()), nameSize);
  hdfFileName = MXAFileInfo::filename(hdfFileName);
  QString xdmfGroupPath = QString(":/") + VolumeDataContainer::ClassName() + QString("/") + H5_FIELD_DATA_GROUP_NAME;
#endif

  int64_t volDims[3] = { 0,0,0 };


  // Write the Field Data
  err = QH5Utilities::createGroupsFromPath(H5_FIELD_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {
    qDebug() << "Error creating HDF Group " << H5_FIELD_DATA_GROUP_NAME;
    return err;
  }
  err = QH5Lite::writeStringAttribute(dcGid, H5_FIELD_DATA_GROUP_NAME, H5_NAME, H5_FIELD_DATA_DEFAULT);
  if(err < 0)
  {
    return err;
  }

  hid_t fieldGroupId = H5Gopen(dcGid, H5_FIELD_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {

    QString ss = QObject::tr("Error opening field Group %1").arg(H5_FIELD_DATA_GROUP_NAME);
    setErrorCondition(-65);
    addErrorMessage(getHumanLabel(), ss, err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }

  size_t total = 0;
  typedef std::vector<IDataArray*> VectorOfIDataArrays_t;
  VectorOfIDataArrays_t neighborListArrays;

  NameListType names = m->getFieldArrayNameList();
  if (names.size() > 0)
  {
    IDataArray::Pointer array = m->getFieldData(names.front());
    total = array->GetSize();
    volDims[0] = total;
    volDims[1] = 1;
    volDims[2] = 1;
#if WRITE_FIELD_XDMF

    QString ss = QObject::tr("Field Data (%1)").arg(total);
    writeFieldXdmfGridHeader(total, ss.str());
#endif
  }
  // Now loop over all the field data and write it out, possibly wrapping it with XDMF code also.
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    IDataArray::Pointer array = m->getFieldData(*iter);
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

    QString ss = QObject::tr("Neighbor Data (%1)").arg(total);
    writeFieldXdmfGridHeader(total, ss.str());
    #endif
    for(VectorOfIDataArrays_t::iterator iter = arrays.begin(); iter < arrays.end(); ++iter)
    {
      err = (*iter)->writeH5Data(fieldGroupId);
      if(err < 0)
      {
        IDataArray* p = *iter;
        QString ss = QObject::tr("Error writing field array '%1' to the HDF5 File").arg(p->GetName());
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
    writeXdmfGridFooter(ss.str());
#endif

  }

  H5Gclose(fieldGroupId);
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexDataContainerWriter::writeEnsembleData(hid_t dcGid)
{
  
  int err = 0;
  VertexDataContainer* m = getVertexDataContainer();

  // Write the Ensemble data
  err = QH5Utilities::createGroupsFromPath(H5_ENSEMBLE_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {

    QString ss = QObject::tr("Error creating HDF Group %1").arg(H5_ENSEMBLE_DATA_GROUP_NAME);
    setErrorCondition(-66);
    addErrorMessage(getHumanLabel(), ss, err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  err = QH5Lite::writeStringAttribute(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, H5_NAME, H5_ENSEMBLE_DATA_DEFAULT);

  hid_t ensembleGid = H5Gopen(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {

    QString ss = QObject::tr("Error opening ensemble Group %1").arg(H5_ENSEMBLE_DATA_GROUP_NAME);
    setErrorCondition(-67);
    addErrorMessage(getHumanLabel(), ss, err);
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

