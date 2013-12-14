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


#include "DREAM3DLib/FilterParameters/H5FilterParametersWriter.h"


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
  m_OutputFile(""),
  m_WritePipeline(true),
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
  setOutputFile( reader->readString( "OutputFile", getOutputFile() ) );
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
  writer->writeValue("WriteXdmfFile", getWriteXdmfFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::dataCheck()
{
  setErrorCondition(0);
  QString ss;

  if (m_OutputFile.isEmpty() == true)
  {
    ss = QObject::tr(": The output file must be set before executing this filter.");
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
  }

  QFileInfo fi(m_OutputFile);
  QDir parentPath(fi.path());
  if (parentPath.exists() == false)
  {
    ss = QObject::tr("The directory path for the output file does not exist.");
    notifyWarningMessage(getHumanLabel(), ss, -1);
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
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  int err = 0;

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(m_OutputFile);
  QString parentPath = fi.path();
  QDir dir;
  if(!dir.mkpath(parentPath))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }

  err = openFile(false); // Do NOT append to any existing file
  if (err < 0)
  {
    QString ss = QObject::tr(": The hdf5 file could not be opened or created.\n The Given filename was:\n\t[%1]").arg(m_OutputFile);
    setErrorCondition(-59);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
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
    QFileInfo ofFi(m_OutputFile);
    QString name = ofFi.baseName();
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

  err = H5Utilities::createGroupsFromPath(DREAM3D::StringConstants::DataContainerGroupName.toLatin1().data(), m_FileId);
  if (err < 0)
  {
    QString ss = QObject::tr("Error creating HDF Group %1").arg(DREAM3D::StringConstants::DataContainerGroupName);
    setErrorCondition(-60);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  hid_t dcaGid = H5Gopen(m_FileId, DREAM3D::StringConstants::DataContainerGroupName.toLatin1().data(), H5P_DEFAULT );
  scopedFileSentinel.addGroupId(&dcaGid);

  unsigned int dcType = DREAM3D::DataContainerType::UnknownDataContainer;
  QList<QString> dcNames = getDataContainerArray()->getDataContainerNames();
  for(size_t iter = 0; iter < getDataContainerArray()->size(); iter++)
  {
    DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(dcNames[iter]);
    err = H5Utilities::createGroupsFromPath(dcNames[iter].toLatin1().data(), dcaGid);
    if (err < 0)
    {
      QString ss = QObject::tr("Error creating HDF Group %1").arg(dcNames[iter]);
      setErrorCondition(-60);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    dcType = dc->getDCType();
    err = QH5Lite::writeScalarAttribute(dcaGid, dcNames[iter], DREAM3D::StringConstants::DataContainerType, dcType);
    if (err < 0)
    {
      qDebug() << "Error Writing DataContainerType attribute for " << dcNames[iter] << "\n";
    }

    hid_t dcGid = H5Gopen(dcaGid, dcNames[iter].toLatin1().data(), H5P_DEFAULT );
    QString ss = QObject::tr("%1 |--> Writing %2 DataContainer ").arg(getMessagePrefix().arg(dcNames[iter]));

    // Have the DataContainer write all of its Attribute Matrices and its Mesh
    err = dc->writeAttributeMatricesToHDF5(dcGid);
    if (err < 0)
    {
      notifyErrorMessage(getHumanLabel(), "Error Writing DataContainer Attribute Matrices", -803);
      return;
    }
    err = dc->writeMeshToHDF5(dcGid);
    if (err < 0)
    {
      notifyErrorMessage(getHumanLabel(), "Error Writing DataContainer Mesh", -804);
      return;
    }
    if (m_WriteXdmfFile == true)
    {
      err = dc->writeXdmf(&out, m_OutputFile);
      if (err < 0)
      {
        notifyErrorMessage(getHumanLabel(), "Error Writing Xdmf File", -805);
        return;
      }
    }
  }

  if (m_WriteXdmfFile == true)
  {
    writeXdmfFooter(out);
  }

  H5Gclose(dcaGid);
  dcaGid = -1;
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::writeXdmfHeader(QTextStream& xdmf)
{
  xdmf << "<?xml version=\"1.0\"?>" << "\n";
  xdmf << "<!DOCTYPE Xdmf SYSTEM \"Xdmf.dtd\"[]>" << "\n";
  xdmf << "<Xdmf xmlns:xi=\"http://www.w3.org/2003/XInclude\" Version=\"2.2\">" << "\n";
  xdmf << " <Domain>" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::writeXdmfFooter(QTextStream& xdmf)
{
  xdmf << " </Domain>" << "\n";
  xdmf << "</Xdmf>" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerWriter::writePipeline()
{

  // WRITE THE PIPELINE TO THE HDF5 FILE
  H5FilterParametersWriter::Pointer parametersWriter = H5FilterParametersWriter::New();
  hid_t pipelineGroupId = QH5Utilities::createGroup(m_FileId, DREAM3D::StringConstants::PipelineGroupName);
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

  int err = QH5Lite::writeScalarAttribute(m_FileId, DREAM3D::StringConstants::PipelineGroupName, DREAM3D::Settings::NumFilters, index);
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
  if(m_FileId > 0)
  {
    return QH5Utilities::closeFile(m_FileId);
  }
  return true;
}

