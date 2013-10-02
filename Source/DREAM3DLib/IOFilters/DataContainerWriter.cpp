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

#include "DataContainerWriter.h"


#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>


#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/HDF5ScopedFileSentinel.h"

#include "EbsdLib/EbsdConstants.h"


#include "DREAM3DLib/HDF5/H5FilterParametersWriter.h"
#include "DREAM3DLib/IOFilters/util/VolumeDataContainerWriter.h"
#include "DREAM3DLib/IOFilters/util/SurfaceDataContainerWriter.h"
#include "DREAM3DLib/IOFilters/util/VertexDataContainerWriter.h"
#include "DREAM3DLib/IOFilters/util/EdgeDataContainerWriter.h"


#define APPEND_DATA_TRUE 1
#define APPEND_DATA_FALSE 0

namespace Detail
{
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerWriter::DataContainerWriter() :
  AbstractFilter(),
  m_WritePipeline(true),
  m_WriteVolumeData(true),
  m_WriteSurfaceData(true),
  m_WriteEdgeData(false),
  m_WriteVertexData(false),
  m_WriteXdmfFile(true),
  m_FileId(-1)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerWriter::~DataContainerWriter()
{
  closeFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.dream3d");
    option->setFileType("DREAM.3D Data");
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Volume DataContainer");
    option->setPropertyName("WriteVolumeData");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Surface DataContainer");
    option->setPropertyName("WriteSurfaceData");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Edge DataContainer");
    option->setPropertyName("WriteEdgeData");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Vertex DataContainer");
    option->setPropertyName("WriteVertexData");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Xdmf File");
    option->setPropertyName("WriteXdmfFile");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    option->setUnits("ParaView Compatible File");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setOutputFile( reader->readValue( "OutputFile", getOutputFile() ) );
  setWriteVolumeData( reader->readValue("WriteVolumeData", getWriteVolumeData()) );
  setWriteEdgeData( reader->readValue("WriteEdgeData", getWriteEdgeData() ) );
  setWriteSurfaceData( reader->readValue("WriteSurfaceData", getWriteSurfaceData() ) );
  setWriteVertexData( reader->readValue("WriteVertexData", getWriteVertexData() ) );
  setWriteXdmfFile( reader->readValue("WriteXdmfFile", getWriteXdmfFile()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputFile", getOutputFile() );
  writer->writeValue("WriteVolumeData", getWriteVolumeData() );
  writer->writeValue("WriteEdgeData", getWriteEdgeData() );
  writer->writeValue("WriteSurfaceData", getWriteSurfaceData() );
  writer->writeValue("WritevertexData", getWriteVertexData() );
  writer->writeValue("WriteXdmfFile", getWriteXdmfFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  QString ss;

  if (m_OutputFile.isEmpty() == true)
  {
    ss = QObject::tr(": The output file must be set before executing this filter.");
    addErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
  }

  QFileInfo fi(m_OutputFile);
  QDir parentPath(fi.path());
  if (parentPath.exists() == false)
  {
    ss = QObject::tr("The directory path for the output file does not exist.");
    addWarningMessage(getHumanLabel(), ss, -1);
  }
  if (fi.suffix().compare("") == 0)
  {
    m_OutputFile.append(".dream3d");
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::execute()
{

  setErrorCondition(0);
  dataCheck(false, 1, 1, 1);

  QString ss;
  int err = 0;

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(m_OutputFile);
  QString parentPath = fi.path();
  QDir dir;
  if(!dir.mkpath(parentPath))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    notifyErrorMessage(ss, -1);
    setErrorCondition(-1);
    return;
  }

  err = openFile(false); // Do NOT append to any existing file
  if (err < 0)
  {
    QString ss = QObject::tr(": The hdf5 file could not be opened or created.\n The Given filename was:\n\t[%1]").arg(m_OutputFile);
    setErrorCondition(-59);
    addErrorMessage(getHumanLabel(), ss, err);
    return;
  }

  // This will make sure if we return early from this method that the HDF5 File is properly closed.
  HDF5ScopedFileSentinel scopedFileSentinel(&m_FileId, true);

  // Write our File Version string to the Root "/" group
  QH5Lite::writeStringAttribute(m_FileId, "/", DREAM3D::HDF5::FileVersionName, DREAM3D::HDF5::FileVersion);

  QFile xdmfFile;
  QTextStream out(&xdmfFile);
  if (m_WriteXdmfFile == true)
  {
    QFileInfo fi(m_OutputFile);
    QString name = fi.baseName();
    if(parentPath.isEmpty() == true)
    {
      name = name + ".xdmf";
    }
    else
    {
      name =  parentPath + "/" + name + ".xdmf";
    }
    xdmfFile.setFileName(name);
    if (xdmfFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      writeXdmfHeader(out);
    }
  }

  // Write the Pipeline to the File
  err = writePipeline();

  err = H5Utilities::createGroupsFromPath(DREAM3D::HDF5::DataContainerName.toLatin1().data(), m_FileId);
  if (err < 0)
  {
    QString ss = QObject::tr("Error creating HDF Group %1").arg(DREAM3D::HDF5::DataContainerName);
    setErrorCondition(-60);
    addErrorMessage(getHumanLabel(), ss, err);
    return;
  }
  hid_t dcaGid = H5Gopen(m_FileId, DREAM3D::HDF5::DataContainerName.toLatin1().data(), H5P_DEFAULT );

  int32_t dcType = DREAM3D::DataContainerType::UnknownDataContainer;
  QList<QString> dcNames = getDataContainerArray()->getDataContainerNames();
  for(size_t iter = 0; iter < getDataContainerArray()->size(); iter++)
  {
    dcType = DREAM3D::DataContainerType::UnknownDataContainer;
    VolumeDataContainer* vl = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(dcNames[iter]);
    SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(dcNames[iter]);
    EdgeDataContainer* e = getDataContainerArray()->getDataContainerAs<EdgeDataContainer>(dcNames[iter]);
    VertexDataContainer* v = getDataContainerArray()->getDataContainerAs<VertexDataContainer>(dcNames[iter]);
    if(vl != NULL) dcType = DREAM3D::DataContainerType::VolumeDataContainer;
    else if(sm != NULL) dcType = DREAM3D::DataContainerType::SurfaceDataContainer;
    else if(e != NULL) dcType = DREAM3D::DataContainerType::EdgeDataContainer;
    else if(v != NULL) dcType = DREAM3D::DataContainerType::VertexDataContainer;
    if((dcType == DREAM3D::DataContainerType::VolumeDataContainer && m_WriteVolumeData == true) ||
      (dcType == DREAM3D::DataContainerType::SurfaceDataContainer && m_WriteSurfaceData == true) ||
      (dcType == DREAM3D::DataContainerType::EdgeDataContainer && m_WriteEdgeData == true) ||
      (dcType == DREAM3D::DataContainerType::VertexDataContainer && m_WriteVertexData == true))
    {

      err = H5Utilities::createGroupsFromPath(dcNames[iter].toLatin1().data(), dcaGid);
      if (err < 0)
      {
        QString ss = QObject::tr("Error creating HDF Group %1").arg(dcNames[iter]);
        setErrorCondition(-60);
        addErrorMessage(getHumanLabel(), ss, err);
        return;
      }

      err = QH5Lite::writeScalarAttribute(dcaGid, dcNames[iter], DREAM3D::HDF5::DataContainerType, dcType);
      if (err < 0)
      {
        qDebug() << "Error Writing H5_NUMBER_OF_POINTS attribute for " << dcNames[iter] << "\n";
      }

      if(dcType == 0)
      {
        VolumeDataContainerWriter::Pointer volWriter = VolumeDataContainerWriter::New();
        volWriter->setHdfFileId(m_FileId);
        volWriter->setHdfGroupId(dcaGid);
        volWriter->setDataContainer(getDataContainerArray()->getDataContainer(dcNames[iter]).get());
        volWriter->setdcType(dcType);
        volWriter->setObservers(getObservers());
        volWriter->setWriteXdmfFile(getWriteXdmfFile());
        volWriter->setXdmfOStream(&out);

        ss = QObject::tr("%1 |--> Writing Volume Data ").arg(getMessagePrefix());
        volWriter->setMessagePrefix(ss);
        volWriter->execute();
        if (volWriter->getErrorCondition() < 0)
        {
          notifyErrorMessage("Error Writing the Volume Data", -803);
          return;
        }
      }
      if(dcType <= 1)
      {
        SurfaceDataContainerWriter::Pointer surfWriter = SurfaceDataContainerWriter::New();
        surfWriter->setHdfFileId(m_FileId);
        surfWriter->setHdfGroupId(dcaGid);
        surfWriter->setDataContainer(getDataContainerArray()->getDataContainer(dcNames[iter]).get());
        surfWriter->setdcType(dcType);
        surfWriter->setObservers(getObservers());
        surfWriter->setWriteXdmfFile(getWriteXdmfFile());
        surfWriter->setXdmfOStream(&out);

        ss = QObject::tr("%1 |--> Writing Surface Data ").arg(getMessagePrefix());
        surfWriter->setMessagePrefix(ss);
        surfWriter->execute();
        if (surfWriter->getErrorCondition() < 0)
        {
          notifyErrorMessage("Error Writing the Surface Data", surfWriter->getErrorCondition());
          return;
        }
      }
      if(dcType <= 2)
      {
        EdgeDataContainerWriter::Pointer edgeWriter = EdgeDataContainerWriter::New();
        edgeWriter->setHdfFileId(m_FileId);
        edgeWriter->setHdfGroupId(dcaGid);
        edgeWriter->setDataContainer(getDataContainerArray()->getDataContainer(dcNames[iter]).get());
        edgeWriter->setdcType(dcType);
        edgeWriter->setObservers(getObservers());
        edgeWriter->setWriteXdmfFile(getWriteXdmfFile());
        edgeWriter->setXdmfOStream(&out);

        ss = QObject::tr("%1 |--> Writing Edge Data ").arg(getMessagePrefix());
        edgeWriter->setMessagePrefix(ss);
        edgeWriter->execute();
        if (edgeWriter->getErrorCondition() < 0)
        {
          notifyErrorMessage("Error Writing the Edge Data", edgeWriter->getErrorCondition());
          return;
        }
      }
      if(dcType <= 3)
      {
        VertexDataContainerWriter::Pointer vertWriter = VertexDataContainerWriter::New();
        vertWriter->setHdfFileId(m_FileId);
        vertWriter->setHdfGroupId(dcaGid);
        vertWriter->setDataContainer(getDataContainerArray()->getDataContainer(dcNames[iter]).get());
        vertWriter->setdcType(dcType);
        vertWriter->setObservers(getObservers());
        vertWriter->setWriteXdmfFile(getWriteXdmfFile());
        vertWriter->setXdmfOStream(&out);

        ss = QObject::tr("%1 |--> Writing Vertex Data ").arg(getMessagePrefix());
        vertWriter->setMessagePrefix(ss);
        vertWriter->execute();
        if (vertWriter->getErrorCondition() < 0)
        {
          notifyErrorMessage("Error Writing the Vertex Data", vertWriter->getErrorCondition());
          return;
        }
      }

      if (m_WriteXdmfFile == true)
      {      
        writeXdmfGridFooter(out, dcNames[iter]);
      }

    }
  }

  if (m_WriteXdmfFile == true)
  {
      writeXdmfFooter(out);
  }


  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::writeXdmfHeader(QTextStream &xdmf)
{
  xdmf << "<?xml version=\"1.0\"?>" << "\n";
  xdmf << "<!DOCTYPE Xdmf SYSTEM \"Xdmf.dtd\"[]>" << "\n";
  xdmf << "<Xdmf xmlns:xi=\"http://www.w3.org/2003/XInclude\" Version=\"2.2\">" << "\n";
  xdmf << " <Domain>" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::writeXdmfFooter(QTextStream &xdmf)
{
  xdmf << " </Domain>" << "\n";
  xdmf << "</Xdmf>" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::writeXdmfGridFooter(QTextStream &xdmf, const QString label)
{
  xdmf << "  </Grid>" << "\n";
  xdmf << "    <!-- *************** END OF " << label << " *************** -->" << "\n";
  xdmf << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerWriter::writePipeline()
{

  // WRITE THE PIPELINE TO THE HDF5 FILE
  H5FilterParametersWriter::Pointer parametersWriter = H5FilterParametersWriter::New();
  hid_t pipelineGroupId = QH5Utilities::createGroup(m_FileId, DREAM3D::HDF5::PipelineGroupName);
  parametersWriter->setGroupId(pipelineGroupId);


  // Now start walking BACKWARDS through the pipeline to find the first filter.
  AbstractFilter::Pointer previousFilter = getPreviousFilter();
  while (previousFilter.get() != NULL)
  {
    if (NULL == previousFilter->getPreviousFilter())
    {
      break;
    }
    else
    {
      previousFilter = previousFilter->getPreviousFilter();
    }
  }


  // Now starting with the first filter in the pipeline, start the actual writing
  AbstractFilter::Pointer currentFilter = previousFilter;
  int index = 0;
  while(NULL != currentFilter.get())
  {
    index = currentFilter->writeFilterParameters(parametersWriter.get(), index);
    currentFilter = currentFilter->getNextFilter();
  }



  H5Gclose(pipelineGroupId);
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerWriter::openFile(bool appendData)
{
  // Try to open a file to append data into
  if (APPEND_DATA_TRUE == appendData)
  {
    m_FileId = QH5Utilities::openFile(m_OutputFile, false);
  }
  // No file was found or we are writing new data only to a clean file
  if (APPEND_DATA_FALSE == appendData || m_FileId < 0)
  {
    m_FileId = QH5Utilities::createFile (m_OutputFile);
  }
  return m_FileId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerWriter::closeFile()
{
  // Close the file when we are finished with it
  return QH5Utilities::closeFile(m_FileId);
}