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

#include <QtCore/QFileInfo>

#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/HDF5ScopedFileSentinel.h"

#include "DREAM3DLib/IOFilters/util/VolumeDataContainerReader.h"
#include "DREAM3DLib/IOFilters/util/SurfaceDataContainerReader.h"
#include "DREAM3DLib/IOFilters/util/VertexDataContainerReader.h"
#include "DREAM3DLib/IOFilters/util/EdgeDataContainerReader.h"
#include "DREAM3DLib/HDF5/H5FilterParametersReader.h"
#include "DREAM3DLib/Common/FilterManager.h"



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
  setSelectedVolumeCellFieldArrays( reader->readValue("SelectedVolumeCellFieldArrays", getSelectedVolumeCellFieldArrays() ) );
  setSelectedVolumeCellEnsembleArrays( reader->readValue("SelectedVolumeCellEnsembleArrays", getSelectedVolumeCellEnsembleArrays() ) );
  setSelectedSurfaceVertexArrays( reader->readValue("SelectedSurfaceVertexArrays", getSelectedSurfaceVertexArrays() ) );
  setSelectedSurfaceFaceArrays( reader->readValue("SelectedSurfaceFaceArrays", getSelectedSurfaceFaceArrays() ) );
  setSelectedSurfaceEdgeArrays( reader->readValue("SelectedSurfaceEdgeArrays", getSelectedSurfaceEdgeArrays() ) );
  setSelectedSurfaceFaceFieldArrays( reader->readValue("SelectedSurfaceFaceFieldArrays", getSelectedSurfaceFaceFieldArrays() ) );
  setSelectedSurfaceFaceEnsembleArrays( reader->readValue("SelectedSurfaceFaceEnsembleArrays", getSelectedSurfaceFaceEnsembleArrays() ) );
  setSelectedEdgeVertexArrays( reader->readValue("SelectedEdgeVertexArrays", getSelectedEdgeVertexArrays() ) );
  setSelectedEdgeEdgeArrays( reader->readValue("SelectedEdgeEdgeArrays", getSelectedEdgeEdgeArrays() ) );
  setSelectedEdgeEdgeFieldArrays( reader->readValue("SelectedEdgeEdgeFieldArrays", getSelectedEdgeEdgeFieldArrays() ) );
  setSelectedEdgeEdgeEnsembleArrays( reader->readValue("SelectedEdgeEdgeEnsembleArrays", getSelectedEdgeEdgeEnsembleArrays() ) );
  setSelectedVertexVertexArrays( reader->readValue("SelectedVertexVertexArrays", getSelectedVertexVertexArrays() ) );
  setSelectedVertexVertexFieldArrays( reader->readValue("SelectedVertexVertexFieldArrays", getSelectedVertexVertexFieldArrays() ) );
  setSelectedVertexVertexEnsembleArrays( reader->readValue("SelectedVertexVertexEnsembleArrays", getSelectedVertexVertexEnsembleArrays() ) );
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
  writer->writeValue("SelectedVolumeCellFieldArrays", getSelectedVolumeCellFieldArrays() );
  writer->writeValue("SelectedVolumeCellEnsembleArrays", getSelectedVolumeCellEnsembleArrays() );
  writer->writeValue("SelectedSurfaceVertexArrays", getSelectedSurfaceVertexArrays() );
  writer->writeValue("SelectedSurfaceFaceArrays", getSelectedSurfaceFaceArrays() );
  writer->writeValue("SelectedSurfaceEdgeArrays", getSelectedSurfaceEdgeArrays() );
  writer->writeValue("SelectedSurfaceFaceFieldArrays", getSelectedSurfaceFaceFieldArrays() );
  writer->writeValue("SelectedSurfaceFaceEnsembleArrays", getSelectedSurfaceFaceEnsembleArrays() );
  writer->writeValue("SelectedEdgeVertexArrays", getSelectedEdgeVertexArrays() );
  writer->writeValue("SelectedEdgeEdgeArrays", getSelectedEdgeEdgeArrays() );
  writer->writeValue("SelectedEdgeEdgeFieldArrays", getSelectedEdgeEdgeFieldArrays() );
  writer->writeValue("SelectedEdgeEdgeEnsembleArrays", getSelectedEdgeEdgeEnsembleArrays() );
  writer->writeValue("SelectedVertexVertexArrays", getSelectedVertexVertexArrays() );
  writer->writeValue("SelectedVertexVertexFieldArrays", getSelectedVertexVertexFieldArrays() );
  writer->writeValue("SelectedVertexVertexEnsembleArrays", getSelectedVertexVertexEnsembleArrays() );
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
  float fVersion = 0.0f;
  bool check = false;
  QFileInfo fi(getInputFile());
  if (getInputFile().isEmpty() == true)
  {
    ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (fi.exists() == false)
  {
    ss = QObject::tr("The input file does not exist.");
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {
    // Read the Meta Data and Array names from the file
    hid_t fileId = QH5Utilities::openFile(m_InputFile, true); // Open the file Read Only
    if(fileId < 0)
    {
      ss = QObject::tr(": Error opening input file '%1'").arg(ClassName());
      setErrorCondition(-150);
      addErrorMessage(getHumanLabel(), ss, err);
      return;
    }

    //Check to see if version of .dream3d file is prior to new data container names
    err = QH5Lite::readStringAttribute(fileId, "/", DREAM3D::HDF5::FileVersionName, m_FileVersion);
    m_FileVersion.toFloat(&check);
    if(fVersion < 5.0 || err < 0)
    {
      QH5Utilities::closeFile(fileId);
      fileId = QH5Utilities::openFile(m_InputFile, false); // Re-Open the file as Read/Write
      err = H5Lmove(fileId, "VoxelDataContainer", fileId, DREAM3D::HDF5::VolumeDataContainerName.toLatin1().data(), H5P_DEFAULT, H5P_DEFAULT);
      err = H5Lmove(fileId, "SurfaceMeshDataContainer", fileId, DREAM3D::HDF5::SurfaceDataContainerName.toLatin1().data(), H5P_DEFAULT, H5P_DEFAULT);
      err = QH5Lite::writeStringAttribute(fileId, "/", DREAM3D::HDF5::FileVersionName, DREAM3D::HDF5::FileVersion);
      QH5Utilities::closeFile(fileId);
      fileId = QH5Utilities::openFile(m_InputFile, true); // Re-Open the file as Read Only
    }

    // This will make sure if we return early from this method that the HDF5 File is properly closed.
    HDF5ScopedFileSentinel scopedFileSentinel(&fileId, true);

    /* READ THE VOXEL DATA TO THE HDF5 FILE */
    if (getVolumeDataContainer() != NULL && m_ReadVolumeData == true)
    {
      VolumeDataContainerReader::Pointer volumeReader = VolumeDataContainerReader::New();
      volumeReader->setHdfFileId(fileId);
      volumeReader->setDataContainer(getVolumeDataContainer());
      volumeReader->setObservers(getObservers());
      ss = getMessagePrefix() + " |--> Reading Volume Data ";
      volumeReader->setMessagePrefix(ss);
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
      smReader->setDataContainer(getSurfaceDataContainer());
      smReader->setObservers(getObservers());
      ss = getMessagePrefix() + " |--> Reading Surface Data ";
      smReader->setMessagePrefix(ss);
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
      eReader->setDataContainer(getEdgeDataContainer());
      eReader->setObservers(getObservers());
      ss = getMessagePrefix() + " |--> Reading Surface Data ";
      eReader->setMessagePrefix(ss);
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
      smReader->setDataContainer(getVertexDataContainer());
      smReader->setObservers(getObservers());
      ss = getMessagePrefix() + " |--> Reading Solid Mesh Data ";
      smReader->setMessagePrefix(ss);
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

  hid_t fileId = QH5Utilities::openFile(m_InputFile, true); // Open the file Read Only
  if(fileId < 0)
  {
    ss =QObject::tr(": Error opening input file '%1'").arg(m_InputFile);
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss, err);
    return;
  }

  // This will make sure if we return early from this method that the HDF5 File is properly closed.
  HDF5ScopedFileSentinel scopedFileSentinel(&fileId, true);

  // Read our File Version string to the Root "/" group
  QString fileVersion;

  err = QH5Lite::readStringAttribute(fileId, "/", DREAM3D::HDF5::FileVersionName, fileVersion);

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
    volumeReader->setFieldArraysToRead(m_SelectedVolumeCellFieldArrays);
    volumeReader->setEnsembleArraysToRead(m_SelectedVolumeCellEnsembleArrays);
    volumeReader->setReadAllArrays(m_ReadAllArrays);
    volumeReader->setDataContainer(getVolumeDataContainer());
    volumeReader->setObservers(getObservers());
    ss = getMessagePrefix() + " |--> Reading Volume Data ";
    volumeReader->setMessagePrefix(ss);
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
    smReader->setFieldArraysToRead(m_SelectedSurfaceFaceFieldArrays);
    smReader->setEnsembleArraysToRead(m_SelectedSurfaceFaceEnsembleArrays);
    smReader->setReadAllArrays(m_ReadAllArrays);
    smReader->setDataContainer(getSurfaceDataContainer());
    smReader->setObservers(getObservers());
    ss = getMessagePrefix() + " |--> Reading Surface Data ";
    smReader->setMessagePrefix(ss);
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
    eReader->setEdgeFieldArraysToRead(m_SelectedEdgeEdgeFieldArrays);
    eReader->setEdgeEnsembleArraysToRead(m_SelectedEdgeEdgeEnsembleArrays);
    eReader->setReadAllArrays(m_ReadAllArrays);
    eReader->setDataContainer(getEdgeDataContainer());
    eReader->setObservers(getObservers());
    ss = getMessagePrefix() + " |--> Reading Surface Data ";
    eReader->setMessagePrefix(ss);
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
    vReader->setVertexFieldArraysToRead(m_SelectedVertexVertexFieldArrays);
    vReader->setVertexEnsembleArraysToRead(m_SelectedVertexVertexEnsembleArrays);
    vReader->setReadAllArrays(m_ReadAllArrays);
    vReader->setDataContainer(getVertexDataContainer());
    vReader->setObservers(getObservers());
    ss = getMessagePrefix() + " |--> Reading Edge Data ";
    vReader->setMessagePrefix(ss);
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
  hid_t pipelineGroupId = H5Gopen(fileId, DREAM3D::HDF5::PipelineGroupName.toLatin1().data(), H5P_DEFAULT);
  reader->setGroupId(pipelineGroupId);

  // Use QH5Lite to ask how many "groups" are in the "Pipeline Group"
  QList<QString> groupList;
  err = QH5Utilities::getGroupObjects(pipelineGroupId, H5Utilities::H5Support_GROUP, groupList);

  // Loop over the items getting the "ClassName" attribute from each group
  QString classNameStr = "";
  for (int i=0; i<groupList.size(); i++)
  {
    QString ss = QString::number(i, 10);

    err = QH5Lite::readStringAttribute(pipelineGroupId, ss, "ClassName", classNameStr);
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
  m_SelectedVolumeCellFieldArrays = selectedFieldArrays;
  m_SelectedVolumeCellEnsembleArrays = selectedEnsembleArrays;
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
  m_SelectedSurfaceFaceFieldArrays = selectedFieldArrays;
  m_SelectedSurfaceFaceEnsembleArrays = selectedEnsembleArrays;
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
  m_SelectedEdgeEdgeFieldArrays = selectedFieldArrays;
  m_SelectedEdgeEdgeEnsembleArrays = selectedEnsembleArrays;
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
  m_SelectedVertexVertexFieldArrays = selectedFieldArrays;
  m_SelectedVertexVertexEnsembleArrays = selectedEnsembleArrays;
  m_ReadAllArrays = false;
}




