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

#include "DataContainerReader.h"

#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"

#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/StringUtils.h"

#include "DREAM3DLib/IOFilters/VolumeDataContainerReader.h"
#include "DREAM3DLib/IOFilters/SurfaceDataContainerReader.h"
#include "DREAM3DLib/IOFilters/VertexDataContainerReader.h"
#include "DREAM3DLib/IOFilters/EdgeDataContainerReader.h"
#include "DREAM3DLib/HDF5/H5FilterParametersReader.h"
#include "DREAM3DLib/Common/FilterManager.h"

#include "H5Support/HDF5ScopedFileSentinel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReader::DataContainerReader() :
  AbstractFilter(),
  m_InputFile(""),
  m_ReadVolumeData(true),
  m_ReadSurfaceData(false),
  m_ReadVertexData(false),
  m_ReadEdgeData(false),
  m_ReadAllArrays(true)
{
  m_PipelineFromFile = FilterPipeline::New();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReader::~DataContainerReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputFile( reader->readValue("InputFile", getInputFile() ) );
  setReadVolumeData( reader->readValue("ReadVolumeData", getReadVolumeData() ) );
  setReadSurfaceData( reader->readValue("ReadSurfaceData", getReadSurfaceData() ) );
  setReadEdgeData( reader->readValue("ReadEdgeData", getReadEdgeData() ) );
  setReadVertexData( reader->readValue("ReadVertexData", getReadVertexData() ) );

  setSelectedVolumeVertexArrays( reader->readValue("SelectedVolumeVertexArrays", getSelectedVolumeVertexArrays() ) );
  setSelectedVolumeFaceArrays( reader->readValue("SelectedVolumeFaceArrays", getSelectedVolumeFaceArrays() ) );
  setSelectedVolumeEdgeArrays( reader->readValue("SelectedVolumeEdgeArrays", getSelectedVolumeEdgeArrays() ) );
  setSelectedVolumeCellArrays( reader->readValue("SelectedVolumeCellArrays", getSelectedVolumeCellArrays() ) );
  setSelectedVolumeFieldArrays( reader->readValue("SelectedVolumeFieldArrays", getSelectedVolumeFieldArrays() ) );
  setSelectedVolumeEnsembleArrays( reader->readValue("SelectedVolumeEnsembleArrays", getSelectedVolumeEnsembleArrays() ) );
  setSelectedSurfaceVertexArrays( reader->readValue("SelectedSurfaceVertexArrays", getSelectedSurfaceVertexArrays() ) );
  setSelectedSurfaceFaceArrays( reader->readValue("SelectedSurfaceFaceArrays", getSelectedSurfaceFaceArrays() ) );
  setSelectedSurfaceEdgeArrays( reader->readValue("SelectedSurfaceEdgeArrays", getSelectedSurfaceEdgeArrays() ) );
  setSelectedSurfaceFieldArrays( reader->readValue("SelectedSurfaceFieldArrays", getSelectedSurfaceFieldArrays() ) );
  setSelectedSurfaceEnsembleArrays( reader->readValue("SelectedSurfaceEnsembleArrays", getSelectedSurfaceEnsembleArrays() ) );
  setSelectedEdgeVertexArrays( reader->readValue("SelectedEdgeVertexArrays", getSelectedEdgeVertexArrays() ) );
  setSelectedEdgeEdgeArrays( reader->readValue("SelectedEdgeEdgeArrays", getSelectedEdgeEdgeArrays() ) );
  setSelectedEdgeFieldArrays( reader->readValue("SelectedEdgeFieldArrays", getSelectedEdgeFieldArrays() ) );
  setSelectedEdgeEnsembleArrays( reader->readValue("SelectedEdgeEnsembleArrays", getSelectedEdgeEnsembleArrays() ) );
  setSelectedVertexVertexArrays( reader->readValue("SelectedVertexVertexArrays", getSelectedVertexVertexArrays() ) );
  setSelectedVertexFieldArrays( reader->readValue("SelectedVertexFieldArrays", getSelectedVertexFieldArrays() ) );
  setSelectedVertexEnsembleArrays( reader->readValue("SelectedVertexEnsembleArrays", getSelectedVertexEnsembleArrays() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  index = writeExistingPipelineToFile(writer, index);

  writer->openFilterGroup(this, index);
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("ReadVolumeData", getReadVolumeData() );
  writer->writeValue("ReadSurfaceData", getReadSurfaceData() );
  writer->writeValue("ReadEdgeData", getReadEdgeData() );
  writer->writeValue("ReadVertexData", getReadVertexData() );

  writer->writeValue("SelectedVolumeVertexArrays", getSelectedVolumeVertexArrays() );
  writer->writeValue("SelectedVolumeFaceArrays", getSelectedVolumeFaceArrays() );
  writer->writeValue("SelectedVolumeEdgeArrays", getSelectedVolumeEdgeArrays() );
  writer->writeValue("SelectedVolumeCellArrays", getSelectedVolumeCellArrays() );
  writer->writeValue("SelectedVolumeFieldArrays", getSelectedVolumeFieldArrays() );
  writer->writeValue("SelectedVolumeEnsembleArrays", getSelectedVolumeEnsembleArrays() );
  writer->writeValue("SelectedSurfaceVertexArrays", getSelectedSurfaceVertexArrays() );
  writer->writeValue("SelectedSurfaceFaceArrays", getSelectedSurfaceFaceArrays() );
  writer->writeValue("SelectedSurfaceEdgeArrays", getSelectedSurfaceEdgeArrays() );
  writer->writeValue("SelectedSurfaceFieldArrays", getSelectedSurfaceFieldArrays() );
  writer->writeValue("SelectedSurfaceEnsembleArrays", getSelectedSurfaceEnsembleArrays() );
  writer->writeValue("SelectedEdgeVertexArrays", getSelectedEdgeVertexArrays() );
  writer->writeValue("SelectedEdgeEdgeArrays", getSelectedEdgeEdgeArrays() );
  writer->writeValue("SelectedEdgeFieldArrays", getSelectedEdgeFieldArrays() );
  writer->writeValue("SelectedEdgeEnsembleArrays", getSelectedEdgeEnsembleArrays() );
  writer->writeValue("SelectedVertexVertexArrays", getSelectedVertexVertexArrays() );
  writer->writeValue("SelectedVertexFieldArrays", getSelectedVertexFieldArrays() );
  writer->writeValue("SelectedVertexEnsembleArrays", getSelectedVertexEnsembleArrays() );
  writer->closeFilterGroup();
  return ++index; // we want to return the index after the one we just wrote to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::dataCheck(bool preflight, size_t volumes, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  QString ss;
  int32_t err = 0;
  QString m_FileVersion;
  float fVersion;
  bool check;

  if (getInputFile().empty() == true)
  {
    ss << ClassName() << " needs the Input File Set and it was not.";
    setErrorCondition(-387);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else if (MXAFileInfo::exists(getInputFile()) == false)
  {
    ss << "The input file does not exist.";
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else
  {
    // Read the Meta Data and Array names from the file
    hid_t fileId = H5Utilities::openFile(m_InputFile, true); // Open the file as Read Only
    if(fileId < 0)
    {
      ss.str("");
      ss << ": Error opening input file '" << m_InputFile << "'";
      setErrorCondition(-150);
      addErrorMessage(getHumanLabel(), ss.str(), err);
      return;
    }

    //Check to see if version of .dream3d file is prior to new data container names
    err = H5Lite::readStringAttribute(fileId, "/", DREAM3D::HDF5::FileVersionName, m_FileVersion);
    check = StringUtils::stringToNum(fVersion, m_FileVersion);
    if(fVersion < 5.0 || err < 0)
    {
      H5Utilities::closeFile(fileId);
      fileId = H5Utilities::openFile(m_InputFile, false); // Re-Open the file as Read/Write
      err = H5Lmove(fileId, "VoxelDataContainer", fileId, DREAM3D::HDF5::VolumeDataContainerName.c_str(), H5P_DEFAULT, H5P_DEFAULT);
      err = H5Lmove(fileId, "SurfaceMeshDataContainer", fileId, DREAM3D::HDF5::SurfaceDataContainerName.c_str(), H5P_DEFAULT, H5P_DEFAULT); 
      err = H5Lite::writeStringAttribute(fileId, "/", DREAM3D::HDF5::FileVersionName, DREAM3D::HDF5::FileVersion);
      H5Utilities::closeFile(fileId);
      fileId = H5Utilities::openFile(m_InputFile, true); // Re-Open the file as Read Only
    }

    // This will make sure if we return early from this method that the HDF5 File is properly closed.
    HDF5ScopedFileSentinel scopedFileSentinel(&fileId, true);

    /* READ THE VOXEL DATA TO THE HDF5 FILE */
    if (getVolumeDataContainer() != NULL && m_ReadVolumeData == true)
    {
      VolumeDataContainerReader::Pointer volumeReader = VolumeDataContainerReader::New();
      volumeReader->setHdfFileId(fileId);
      volumeReader->setVolumeDataContainer(getVolumeDataContainer());
      volumeReader->setObservers(getObservers());
      ss.str("");
      ss << getMessagePrefix() << " |--> Reading Volume Data ";
      volumeReader->setMessagePrefix(ss.str());
      volumeReader->preflight();
      if (volumeReader->getErrorCondition() < 0)
      {
        setReadVolumeData(false);
        setErrorCondition(volumeReader->getErrorCondition());
        addErrorMessage(getHumanLabel(), "The volume data was not available in the data file.", getErrorCondition());
      }
    }

    /* READ THE Surface DATA TO THE HDF5 FILE */
    if (NULL != getSurfaceDataContainer() && m_ReadSurfaceData == true)
    {
      SurfaceDataContainerReader::Pointer smReader = SurfaceDataContainerReader::New();
      smReader->setHdfFileId(fileId);
      smReader->setSurfaceDataContainer(getSurfaceDataContainer());
      smReader->setObservers(getObservers());
      ss.str("");
      ss << getMessagePrefix() << " |--> Reading Surface Data ";
      smReader->setMessagePrefix(ss.str());
      smReader->preflight();
      if (smReader->getErrorCondition() < 0)
      {
        setReadSurfaceData(false);
        setErrorCondition(smReader->getErrorCondition());
        addErrorMessage(getHumanLabel(), "The surface mesh data was not available in the data file.", getErrorCondition());
      }
    }

    /* READ THE Edge DATA TO THE HDF5 FILE */
    if (NULL != getEdgeDataContainer() && m_ReadEdgeData == true)
    {
      EdgeDataContainerReader::Pointer eReader = EdgeDataContainerReader::New();
      eReader->setHdfFileId(fileId);
      eReader->setEdgeDataContainer(getEdgeDataContainer());
      eReader->setObservers(getObservers());
      ss.str("");
      ss << getMessagePrefix() << " |--> Reading Surface Data ";
      eReader->setMessagePrefix(ss.str());
      eReader->preflight();
      if (eReader->getErrorCondition() < 0)
      {
        setReadEdgeData(false);
        setErrorCondition(eReader->getErrorCondition());
        addErrorMessage(getHumanLabel(), "The surface mesh data was not available in the data file.", getErrorCondition());
      }
    }

    /* READ THE Vertex DATA TO THE HDF5 FILE */
    if (NULL != getVertexDataContainer() && m_ReadVertexData == true)
    {
      VertexDataContainerReader::Pointer smReader = VertexDataContainerReader::New();
      smReader->setHdfFileId(fileId);
      smReader->setVertexDataContainer(getVertexDataContainer());
      smReader->setObservers(getObservers());
      ss.str("");
      ss << getMessagePrefix() << " |--> Reading Solid Mesh Data ";
      smReader->setMessagePrefix(ss.str());
      smReader->preflight();
      if (smReader->getErrorCondition() < 0)
      {
        setReadVertexData(false);
        setErrorCondition(smReader->getErrorCondition());
        addErrorMessage(getHumanLabel(), "The solid mesh data was not available in the data file.", getErrorCondition());
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::execute()
{
  int32_t err = 0;
  QString ss;
  // dataCheck(false, 1, 1, 1);

  hid_t fileId = H5Utilities::openFile(m_InputFile, true); // Open the file Read Only
  if(fileId < 0)
  {
    ss.str("");
    ss << ": Error opening input file '" << m_InputFile << "'";
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    return;
  }

  // This will make sure if we return early from this method that the HDF5 File is properly closed.
  HDF5ScopedFileSentinel scopedFileSentinel(&fileId, true);

  // Read our File Version string to the Root "/" group
  QString fileVersion;
  float fVersion;

  err = H5Lite::readStringAttribute(fileId, "/", DREAM3D::HDF5::FileVersionName, fileVersion);
  bool check = StringUtils::stringToNum(fVersion, fileVersion);
  if(fVersion < 5.0 || err < 0)
  {
    H5Utilities::closeFile(fileId);
    fileId = H5Utilities::openFile(m_InputFile, false); // Re-Open the file as Read/Write
    err = H5Lmove(fileId, "VoxelDataContainer", fileId, DREAM3D::HDF5::VolumeDataContainerName.c_str(), H5P_DEFAULT, H5P_DEFAULT);
    err = H5Lmove(fileId, "SurfaceMeshDataContainer", fileId, DREAM3D::HDF5::SurfaceDataContainerName.c_str(), H5P_DEFAULT, H5P_DEFAULT); 
    err = H5Lite::writeStringAttribute(fileId, "/", DREAM3D::HDF5::FileVersionName, DREAM3D::HDF5::FileVersion);
    H5Utilities::closeFile(fileId);
    fileId = H5Utilities::openFile(m_InputFile, true); // Re-Open the file as Read Only
  }

  err = readExistingPipelineFromFile(fileId);

  /* READ THE VOXEL DATA TO THE HDF5 FILE */
  if (getVolumeDataContainer() != NULL && m_ReadVolumeData == true)
  {
    VolumeDataContainerReader::Pointer volumeReader = VolumeDataContainerReader::New();
    volumeReader->setHdfFileId(fileId);
    volumeReader->setVertexArraysToRead(m_SelectedVolumeVertexArrays);
    volumeReader->setFaceArraysToRead(m_SelectedVolumeFaceArrays);
    volumeReader->setEdgeArraysToRead(m_SelectedVolumeEdgeArrays);
    volumeReader->setCellArraysToRead(m_SelectedVolumeCellArrays);
    volumeReader->setFieldArraysToRead(m_SelectedVolumeFieldArrays);
    volumeReader->setEnsembleArraysToRead(m_SelectedVolumeEnsembleArrays);
    volumeReader->setReadAllArrays(m_ReadAllArrays);
    volumeReader->setVolumeDataContainer(getVolumeDataContainer());
    volumeReader->setObservers(getObservers());
    ss.str("");
    ss << getMessagePrefix() << " |--> Reading Volume Data ";
    volumeReader->setMessagePrefix(ss.str());
    volumeReader->execute();
    if (volumeReader->getErrorCondition() < 0)
    {
      notifyErrorMessage("Error Reading the Volume Data", -803);
      return;
    }
  }

  /* READ THE Surface DATA TO THE HDF5 FILE */
  if (NULL != getSurfaceDataContainer() && m_ReadSurfaceData == true)
  {
    SurfaceDataContainerReader::Pointer smReader = SurfaceDataContainerReader::New();
    smReader->setHdfFileId(fileId);
    smReader->setVertexArraysToRead(m_SelectedSurfaceVertexArrays);
    smReader->setFaceArraysToRead(m_SelectedSurfaceFaceArrays);
    smReader->setEdgeArraysToRead(m_SelectedSurfaceEdgeArrays);
    smReader->setFieldArraysToRead(m_SelectedSurfaceFieldArrays);
    smReader->setEnsembleArraysToRead(m_SelectedSurfaceEnsembleArrays);
    smReader->setReadAllArrays(m_ReadAllArrays);
    smReader->setSurfaceDataContainer(getSurfaceDataContainer());
    smReader->setObservers(getObservers());
    ss.str("");
    ss << getMessagePrefix() << " |--> Reading Surface Data ";
    smReader->setMessagePrefix(ss.str());
    smReader->execute();
    if (smReader->getErrorCondition() < 0)
    {
      notifyErrorMessage("Error Reading the Surface Data", smReader->getErrorCondition());
      return;
    }
  }


  /* READ THE Surface DATA TO THE HDF5 FILE */
  if (NULL != getEdgeDataContainer() && m_ReadEdgeData == true)
  {
    EdgeDataContainerReader::Pointer eReader = EdgeDataContainerReader::New();
    eReader->setHdfFileId(fileId);
    eReader->setVertexArraysToRead(m_SelectedEdgeVertexArrays);
    eReader->setEdgeArraysToRead(m_SelectedEdgeEdgeArrays);
    eReader->setEdgeFieldArraysToRead(m_SelectedEdgeFieldArrays);
    eReader->setEdgeEnsembleArraysToRead(m_SelectedEdgeEnsembleArrays);
    eReader->setReadAllArrays(m_ReadAllArrays);
    eReader->setEdgeDataContainer(getEdgeDataContainer());
    eReader->setObservers(getObservers());
    ss.str("");
    ss << getMessagePrefix() << " |--> Reading Surface Data ";
    eReader->setMessagePrefix(ss.str());
    eReader->preflight();
    if (eReader->getErrorCondition() < 0)
    {
      notifyErrorMessage("Error Reading the Edge Data", eReader->getErrorCondition());
      return;
    }
  }

  /* READ THE Vertex DATA TO THE HDF5 FILE */
  if (NULL != getVertexDataContainer() && m_ReadVertexData == true)
  {
    VertexDataContainerReader::Pointer vReader = VertexDataContainerReader::New();
    vReader->setHdfFileId(fileId);
    vReader->setVertexArraysToRead(m_SelectedVertexVertexArrays);
    vReader->setVertexFieldArraysToRead(m_SelectedVertexFieldArrays);
    vReader->setVertexEnsembleArraysToRead(m_SelectedVertexEnsembleArrays);
    vReader->setReadAllArrays(m_ReadAllArrays);
    vReader->setVertexDataContainer(getVertexDataContainer());
    vReader->setObservers(getObservers());
    ss.str("");
    ss << getMessagePrefix() << " |--> Reading Solid Mesh Data ";
    vReader->setMessagePrefix(ss.str());
    vReader->execute();
    if (vReader->getErrorCondition() < 0)
    {
      notifyErrorMessage("Error Reading the Solid Mesh Data", vReader->getErrorCondition());
      return;
    }
  }

  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerReader::readExistingPipelineFromFile(hid_t fileId)
{
  int err = 0;
  m_PipelineFromFile->clear();

  H5FilterParametersReader::Pointer reader = H5FilterParametersReader::New();

  // HDF5: Open the "Pipeline" Group
  hid_t pipelineGroupId = H5Gopen(fileId, DREAM3D::HDF5::PipelineGroupName.c_str(), H5P_DEFAULT);
  reader->setGroupId(pipelineGroupId);

  // Use H5Lite to ask how many "groups" are in the "Pipeline Group"
  QList<QString> groupList;
  err = H5Utilities::getGroupObjects(pipelineGroupId, H5Utilities::H5Support_GROUP, groupList);

  // Loop over the items getting the "ClassName" attribute from each group
  QString classNameStr = "";
  for (int i=0; i<groupList.size(); i++)
  {
    QString ss;
    ss << i;
    err = H5Lite::readStringAttribute(pipelineGroupId, ss.str(), "ClassName", classNameStr);
#if (__APPLE__)
#warning DOES THIS FILTER MANAGER GET THE CORRECT SINGLETON?
#endif
    // Instantiate a new filter using the FilterFactory based on the value of the className attribute
    FilterManager::Pointer fm = FilterManager::Instance();
    IFilterFactory::Pointer ff = fm->getFactoryForFilter(classNameStr);
    if (NULL != ff.get())
    {
      AbstractFilter::Pointer filter = ff->create();
      if(NULL != filter.get())
      {
        // Read the parameters
        filter->readFilterParameters( reader.get(), i);

        // Add filter to m_PipelineFromFile
        m_PipelineFromFile->pushBack(filter);
      }
    }
  }
  err = H5Gclose(pipelineGroupId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerReader::writeExistingPipelineToFile(AbstractFilterParametersWriter* writer, int index)
{
  FilterPipeline::FilterContainerType container = m_PipelineFromFile->getFilterContainer();

  for(FilterPipeline::FilterContainerType::iterator iter = container.begin(); iter != container.end(); ++iter)
  {
    index = (*iter)->writeFilterParameters(writer, index);
  }
  return index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::setVolumeSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                                           QSet<QString> selectedFaceArrays,
                                                           QSet<QString> selectedEdgeArrays,
                                                           QSet<QString> selectedCellArrays,
                                                           QSet<QString> selectedFieldArrays,
                                                           QSet<QString> selectedEnsembleArrays)
{
  m_SelectedVolumeVertexArrays = selectedVertexArrays;
  m_SelectedVolumeFaceArrays = selectedFaceArrays;
  m_SelectedVolumeEdgeArrays = selectedEdgeArrays;
  m_SelectedVolumeCellArrays = selectedCellArrays;
  m_SelectedVolumeFieldArrays = selectedFieldArrays;
  m_SelectedVolumeEnsembleArrays = selectedEnsembleArrays;
  m_ReadAllArrays = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::setSurfaceSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                                           QSet<QString> selectedEdgeArrays,
                                                           QSet<QString> selectedFaceArrays,
                                                           QSet<QString> selectedFieldArrays,
                                                           QSet<QString> selectedEnsembleArrays)
{
  m_SelectedSurfaceVertexArrays = selectedVertexArrays;
  m_SelectedSurfaceEdgeArrays = selectedEdgeArrays;
  m_SelectedSurfaceFaceArrays = selectedFaceArrays;
  m_SelectedSurfaceFieldArrays = selectedFieldArrays;
  m_SelectedSurfaceEnsembleArrays = selectedEnsembleArrays;
  m_ReadAllArrays = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::setEdgeSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                                           QSet<QString> selectedEdgeArrays,
                                                           QSet<QString> selectedFieldArrays,
                                                           QSet<QString> selectedEnsembleArrays)
{
  m_SelectedEdgeVertexArrays = selectedVertexArrays;
  m_SelectedEdgeEdgeArrays = selectedEdgeArrays;
  m_SelectedEdgeFieldArrays = selectedFieldArrays;
  m_SelectedEdgeEnsembleArrays = selectedEnsembleArrays;
  m_ReadAllArrays = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::setVertexSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                                         QSet<QString> selectedFieldArrays,
                                                         QSet<QString> selectedEnsembleArrays)
{
  m_SelectedVertexVertexArrays = selectedVertexArrays;
  m_SelectedVertexFieldArrays = selectedFieldArrays;
  m_SelectedVertexEnsembleArrays = selectedEnsembleArrays;
  m_ReadAllArrays = false;
}




