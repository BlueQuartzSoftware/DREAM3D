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
#include "DREAM3DLib/DataContainers/VertexArray.h"
#include "DREAM3DLib/HDF5/VTKH5Constants.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"

#define WRITE_FIELD_XDMF 0


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
void VertexDataContainerWriter::dataCheck()
{
  setErrorCondition(0);

  VertexDataContainer* dc = VertexDataContainer::SafePointerDownCast(getDataContainer());
  if(NULL == dc)
  {
    setErrorCondition(-999);
    notifyErrorMessage(getHumanLabel(), "The DataContainer Object was NULL", getErrorCondition());
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
void VertexDataContainerWriter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck();
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainerWriter::execute()
{
  int err = 0;

  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  VertexDataContainer* dc = VertexDataContainer::SafePointerDownCast(getDataContainer());
  if(NULL == dc)
  {
    setErrorCondition(-999);
    notifyErrorMessage(getHumanLabel(), "The DataContainer Object was NULL", getErrorCondition());
    return;
  }
  setErrorCondition(0);

  hid_t dcGid = H5Gopen(getHdfGroupId(), getDataContainer()->getName().toLatin1().data(), H5P_DEFAULT );

  if(getdcType() == 3) { writeXdmfMeshStructure(); }

  // Now finally close the group and the HDf5 File
  H5Gclose(dcGid); // Close the Data Container Group

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainerWriter::writeXdmfMeshStructure()
{

  VertexDataContainer* dc = VertexDataContainer::SafePointerDownCast(getDataContainer());

  if (getWriteXdmfFile() == false || getXdmfOStream() == NULL)
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
  out << "    <Topology TopologyType=\"Polyvertex\" NumberOfElements=\"" << verts->getNumberOfTuples() << "\">" << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << verts->getNumberOfTuples() << " 3\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << getDataContainer()->getName() << "/Verts" << "\n";
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
QString VertexDataContainerWriter::writeXdmfAttributeDataHelper(int numComp, const QString& attrType,
    const QString& groupName,
    IDataArray::Pointer array,
    const QString& centering,
    int precision, const QString& xdmfTypeName)
{
  QString buf;
  QTextStream out(&buf);

  QString hdfFileName = QH5Utilities::fileNameFromFileId(getHdfGroupId());
  QString dimStr = QString::number(array->getNumberOfTuples()) + QString(" ") + QString::number(array->GetNumberOfComponents());

  out << "    <Attribute Name=\"" << array->GetName() << "\" ";
  out << "AttributeType=\"" << attrType << "\" ";
  out << "Center=\"" << centering << "\">" << "\n";
  // Open the <DataItem> Tag
  out << "      <DataItem Format=\"HDF\" Dimensions=\"" << dimStr <<  "\" ";
  out << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << getDataContainer()->getName() << "/" << groupName << "/" << array->GetName() << "\n";
  out << "      </DataItem>" << "\n";
  out << "    </Attribute>" << "\n" << "\n";

  return buf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainerWriter::writeXdmfAttributeData(const QString& groupName, IDataArray::Pointer array, const QString& centering)
{
#if 0
  < Attribute Name = "Node Type" Center = "Node" >
                                          < DataItem Format = "HDF" DataType = "char" Precision = "1" Dimensions = "43029 1" >
                                                            MC_IsoGG_50cubed_55features_Bounded_Multi.dream3d:
                                                              / VertexDataContainer / POINT_DATA / VertexMeshNodeType
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

