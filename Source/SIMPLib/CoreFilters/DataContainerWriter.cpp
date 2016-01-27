/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "DataContainerWriter.h"

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/H5FilterParametersWriter.h"

#ifdef _WIN32
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
#endif

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

// Include the MOC generated file for this class
#include "moc_DataContainerWriter.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerWriter::DataContainerWriter() :
  AbstractFilter(),
  m_OutputFile(""),
  m_WritePipeline(true),
  m_WriteXdmfFile(true),
  m_AppendToExisting(false),
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

  parameters.push_back(OutputFileFilterParameter::New("Output File", "OutputFile", getOutputFile(), FilterParameter::Parameter, "*.dream3d", ""));
  parameters.push_back(BooleanFilterParameter::New("Write Xdmf File", "WriteXdmfFile", getWriteXdmfFile(), FilterParameter::Parameter, "ParaView Compatible File"));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputFile( reader->readString( "OutputFile", getOutputFile() ) );
  setWriteXdmfFile( reader->readValue("WriteXdmfFile", getWriteXdmfFile()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(OutputFile)
  SIMPL_FILTER_WRITE_PARAMETER(WriteXdmfFile)
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
    setErrorCondition(-10000);
    ss = QObject::tr("The output file must be set");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QFileInfo fi(m_OutputFile);
  QDir parentPath(fi.path());
  if (parentPath.exists() == false)
  {
    ss = QObject::tr("The directory path for the output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
    notifyWarningMessage(getHumanLabel(), ss, -1);
  }
  if (fi.suffix().compare("") == 0)
  {
    m_OutputFile.append(".dream3d");
  }
  if(fi.baseName().compare("") == 0)
  {
    setErrorCondition(-10001);
    ss = QObject::tr("The output file must have its actual filename set");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

#ifdef _WIN32
  // Turn file permission checking on
  qt_ntfs_permission_lookup++;
#endif

  QFileInfo dirInfo(fi.path());

  if (dirInfo.isWritable() == false && parentPath.exists() == true)
  {
    setErrorCondition(-10002);
    ss = QObject::tr("The user does not have the proper permissions to write to the output file");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
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
  if (!dir.mkpath(parentPath))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    setErrorCondition(-11110);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  err = openFile(m_AppendToExisting); // Do NOT append to any existing file
  if (err < 0)
  {
    QString ss = QObject::tr("The HDF5 file could not be opened or created.\n The given filename was:\n\t[%1]").arg(m_OutputFile);
    setErrorCondition(-11112);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  //qDebug() << "DREAM3D File: " << m_OutputFile;

  // This will make sure if we return early from this method that the HDF5 File is properly closed.
  HDF5ScopedFileSentinel scopedFileSentinel(&m_FileId, true);

  // Write our File Version string to the Root "/" group
  QH5Lite::writeStringAttribute(m_FileId, "/", DREAM3D::HDF5::FileVersionName, DREAM3D::HDF5::FileVersion);
  QH5Lite::writeStringAttribute(m_FileId, "/", DREAM3D::HDF5::DREAM3DVersion, SIMPLib::Version::Complete() );
  QFile xdmfFile;
  QTextStream xdmfOut(&xdmfFile);
  if (m_WriteXdmfFile == true)
  {
    QFileInfo ofFi(m_OutputFile);
    QString name = ofFi.baseName();
    if (parentPath.isEmpty() == true)
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
      writeXdmfHeader(xdmfOut);
    }
  }

  // Write the Pipeline to the File
  err = writePipeline();

  err = H5Utilities::createGroupsFromPath(DREAM3D::StringConstants::DataContainerGroupName.toLatin1().data(), m_FileId);
  if (err < 0)
  {
    QString ss = QObject::tr("Error creating HDF5 Group '%1'").arg(DREAM3D::StringConstants::DataContainerGroupName);
    setErrorCondition(-60);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  hid_t dcaGid = H5Gopen(m_FileId, DREAM3D::StringConstants::DataContainerGroupName.toLatin1().data(), H5P_DEFAULT );
  scopedFileSentinel.addGroupId(&dcaGid);

  QList<QString> dcNames = getDataContainerArray()->getDataContainerNames();
  for (size_t iter = 0; iter < getDataContainerArray()->getNumDataContainers(); iter++)
  {
    DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(dcNames[iter]);
    IGeometry::Pointer geometry = dc->getGeometry();
    err = H5Utilities::createGroupsFromPath(dcNames[iter].toLatin1().data(), dcaGid);
    if (err < 0)
    {
      QString ss = QObject::tr("Error creating HDF5 Group '%1'").arg(dcNames[iter]);
      setErrorCondition(-60);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    hid_t dcGid = H5Gopen(dcaGid, dcNames[iter].toLatin1().data(), H5P_DEFAULT );
    HDF5ScopedGroupSentinel groupSentinel(&dcGid, false);
    //QString ss = QObject::tr("%1 |--> Writing %2 DataContainer ").arg(getMessagePrefix()).arg(dcNames[iter]);

    // Have the DataContainer write all of its Attribute Matrices and its Mesh
    err = dc->writeAttributeMatricesToHDF5(dcGid);
    if (err < 0)
    {
      notifyErrorMessage(getHumanLabel(), "Error writing DataContainer AttributeMatrices", -803);
      return;
    }
    err = dc->writeMeshToHDF5(dcGid, m_WriteXdmfFile);
    if (err < 0)
    {
      notifyErrorMessage(getHumanLabel(), "Error writing DataContainer Geometry", -804);
      return;
    }
    if (m_WriteXdmfFile == true && geometry.get() != NULL)
    {
      QString hdfFileName = QH5Utilities::fileNameFromFileId(m_FileId);

      err = dc->writeXdmf(xdmfOut, hdfFileName);
      if (err < 0)
      {
        notifyErrorMessage(getHumanLabel(), "Error writing Xdmf File", -805);
        return;
      }
    }
  }

  // Write the Data ContainerBundles
  err = writeDataContainerBundles(m_FileId);
  if (err < 0)
  {
    QString ss = QObject::tr("Error writing DataContainerBundles");
    setErrorCondition(-11113);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // Write the XDMF File
  if (m_WriteXdmfFile == true)
  {
    writeXdmfFooter(xdmfOut);
  }

  H5Gclose(dcaGid);

  dcaGid = -1;

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerWriter::writeDataContainerBundles(hid_t fileId)
{
  int err = QH5Utilities::createGroupsFromPath(DREAM3D::StringConstants::DataContainerBundleGroupName, m_FileId);
  if (err < 0)
  {
    QString ss = QObject::tr("Error creating HDF5 Group '%1'").arg(DREAM3D::StringConstants::DataContainerBundleGroupName);
    setErrorCondition(-61);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }
  hid_t dcbGid = H5Gopen(m_FileId, DREAM3D::StringConstants::DataContainerBundleGroupName.toLatin1().data(), H5P_DEFAULT );

  Detail::H5GroupAutoCloser groupCloser(&dcbGid);

  QMap<QString, IDataContainerBundle::Pointer>& bundles = getDataContainerArray()->getDataContainerBundles();
  QMapIterator<QString, IDataContainerBundle::Pointer> iter(bundles);
  while (iter.hasNext())
  {
    iter.next();
    IDataContainerBundle::Pointer bundle = iter.value();
    err = bundle->writeH5Data(dcbGid);
    if (err < 0)
    {
      return err;
    }
  }
  H5Gclose(dcbGid);
  dcbGid = -1;
  return 0;
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
  AbstractFilter::Pointer previousFilter = getPreviousFilter().lock();
  while (previousFilter.get() != NULL)
  {
    if (NULL == previousFilter->getPreviousFilter().lock().get())
    {
      break;
    }
    else
    {
      previousFilter = previousFilter->getPreviousFilter().lock();
    }
  }


  // Now starting with the first filter in the pipeline, start the actual writing
  AbstractFilter::Pointer currentFilter = previousFilter;
  int index = 0;
  while (NULL != currentFilter.get())
  {
    index = currentFilter->writeFilterParameters(parametersWriter.get(), index);
    currentFilter = currentFilter->getNextFilter().lock();
  }

  int err = QH5Lite::writeScalarAttribute(m_FileId, DREAM3D::StringConstants::PipelineGroupName, DREAM3D::Settings::NumFilters, index);
  if (err < 0)
  {
    QString ss = QObject::tr("Error writing HDF5 scalar attribute '%1' on HDF5 Group '%2'").arg(DREAM3D::Settings::NumFilters).arg(DREAM3D::StringConstants::PipelineGroupName);
    setErrorCondition(-12324323);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition() );
  }
  H5Gclose(pipelineGroupId);
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
hid_t DataContainerWriter::openFile(bool appendData)
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
herr_t DataContainerWriter::closeFile()
{
  // Close the file when we are finished with it
  if (m_FileId > 0)
  {
    return QH5Utilities::closeFile(m_FileId);
  }
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer DataContainerWriter::newFilterInstance(bool copyFilterParameters)
{
  DataContainerWriter::Pointer filter = DataContainerWriter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DataContainerWriter::getCompiledLibraryName()
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DataContainerWriter::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DataContainerWriter::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DataContainerWriter::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DataContainerWriter::getSubGroupName()
{ return DREAM3D::FilterSubGroups::OutputFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DataContainerWriter::getHumanLabel()
{ return "Write DREAM.3D Data File"; }
