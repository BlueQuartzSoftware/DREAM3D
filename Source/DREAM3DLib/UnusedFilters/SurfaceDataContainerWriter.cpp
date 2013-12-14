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
void SurfaceDataContainerWriter::dataCheck()
{
  setErrorCondition(0);

  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());
  if (NULL == dc)
  {
    QString ss = QObject::tr("DataContainer Pointer was NULL and Must be valid.%1(%2)").arg(__FILE__).arg(__LINE__);
    notifyErrorMessage(getHumanLabel(), ss, -2);
    setErrorCondition(-1);
    return;
  }
  if(getHdfGroupId() < 0)
  {
    setErrorCondition(-150);
    notifyErrorMessage(getHumanLabel(), "The HDF5 file id was < 0. This means this value was not set correctly from the calling object.", getErrorCondition());
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainerWriter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck();
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
    notifyErrorMessage(getHumanLabel(), ss, -2);
    setErrorCondition(-1);
    return;
  }

  setErrorCondition(0);

  hid_t dcGid = H5Gopen(getHdfGroupId(), getDataContainer()->getName().toLatin1().data(), H5P_DEFAULT );

  if(getdcType() == 1) { writeXdmfMeshStructure(); }

  // Now finally close the group and the HDf5 File
  H5Gclose(dcGid); // Close the Data Container Group

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainerWriter::writeXdmfMeshStructure()
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

  QString hdfFileName = QH5Utilities::fileNameFromFileId(getHdfFileId());

  QTextStream& out = *getXdmfOStream();

  out << "  <Grid Name=\"" << getDataContainer()->getName() << "\">" << "\n";
  out << "    <Topology TopologyType=\"Triangle\" NumberOfElements=\"" << faces->getNumberOfTuples() << "\">" << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << faces->getNumberOfTuples() << " 3\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << getDataContainer()->getName() << "/Faces" << "\n";
  out << "      </DataItem>" << "\n";
  out << "    </Topology>" << "\n";

  out << "    <Geometry Type=\"XYZ\">" << "\n";
  out << "      <DataItem Format=\"HDF\"  Dimensions=\"" << verts->getNumberOfTuples() << " 3\" NumberType=\"Float\" Precision=\"4\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << getDataContainer()->getName() << "/Vertices" << "\n";
  out << "      </DataItem>" << "\n";
  out << "    </Geometry>" << "\n";
  out << "" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SurfaceDataContainerWriter::writeXdmfAttributeDataHelper(int numComp, const QString& attrType,
    const QString& groupName,
    IDataArray::Pointer array,
    const QString& centering,
    int precision, const QString& xdmfTypeName)
{
  QString buf;
  QTextStream out(&buf);

  QString hdfFileName = QH5Utilities::fileNameFromFileId(getHdfGroupId());

  QString dimStr = QString::number(array->getNumberOfTuples()) + QString(" ") + QString::number(array->GetNumberOfComponents());
  QString dimStrHalf = QString::number(array->getNumberOfTuples()) + QString(" ") + QString::number(array->GetNumberOfComponents() / 2);

  if((numComp % 2) == 1)
  {
    out << "    <Attribute Name=\"" << array->GetName() << "\" ";
    out << "AttributeType=\"" << attrType << "\" ";
    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem Format=\"HDF\" Dimensions=\"" << dimStr <<  "\" ";
    out << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";
    out << "        " << hdfFileName << ":/DataContainers/" << getDataContainer()->getName() << "/" << groupName << "/" << array->GetName() << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n" << "\n";
  }
  else
  {
    //First Slab
    out << "    <Attribute Name=\"" << array->GetName() << " (Feature 0)\" ";
    out << "AttributeType=\"" << attrType << "\" ";

    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"" << dimStrHalf <<  "\" ";
    out << "Type=\"HyperSlab\" " << "Name=\"" << array->GetName() << " (Feature 0)\" >" << "\n";
    out << "        <DataItem Dimensions=\"3 2\" " << "Format=\"XML\" >" << "\n";
    out << "          0        0" << "\n";
    out << "          1        1" << "\n";
    out << "          " << dimStrHalf << " </DataItem>" << "\n";
    out << "\n";
    out << "        <DataItem Format=\"HDF\" Dimensions=\"" << dimStr << "\" " << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";

    out << "        " << hdfFileName << ":/DataContainers/" << getDataContainer()->getName() << "/" << groupName << "/" << array->GetName() << "\n";
    out << "        </DataItem>" << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n" << "\n";

    //Second Slab
    out << "    <Attribute Name=\"" << array->GetName() << " (Feature 1)\" ";
    out << "AttributeType=\"" << attrType << "\" ";

    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"" << dimStrHalf <<  "\" ";
    out << "Type=\"HyperSlab\" " << "Name=\"" << array->GetName() << " (Feature 1)\" >" << "\n";
    out << "        <DataItem Dimensions=\"3 2\" " << "Format=\"XML\" >" << "\n";
    out << "          0        " << (array->GetNumberOfComponents() / 2) << "\n";
    out << "          1        1" << "\n";
    out << "          " << dimStrHalf << " </DataItem>" << "\n";
    out << "\n";
    out << "        <DataItem Format=\"HDF\" Dimensions=\"" << dimStr << "\" " << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";
    out << "        " << hdfFileName << ":/DataContainers/" << getDataContainer()->getName() << "/" << groupName << "/" << array->GetName() << "\n";
    out << "        </DataItem>" << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n" << "\n";
  }
  return buf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainerWriter::writeXdmfAttributeData(const QString& groupName, IDataArray::Pointer array, const QString& centering)
{
#if 0
  < Attribute Name = "Node Type" Center = "Node" >
                                          < DataItem Format = "HDF" DataType = "char" Precision = "1" Dimensions = "43029 1" >
                                                            MC_IsoGG_50cubed_55features_Bounded_Multi.dream3d:
                                                              / SurfaceDataContainer / POINT_DATA / SurfaceMeshNodeType
                                                              < / DataItem >
                                                              < / Attribute >
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
  if(numComp > 2) { attrType = "Vector"; }

  QString block = writeXdmfAttributeDataHelper(numComp, attrType, groupName, array, centering, precision, xdmfTypeName);

  out << block << "\n";
}
