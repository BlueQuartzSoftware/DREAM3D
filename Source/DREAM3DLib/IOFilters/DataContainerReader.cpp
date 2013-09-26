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
#include "H5Support/H5Utilities.h"
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
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_SurfaceDataContainerName(DREAM3D::HDF5::SurfaceDataContainerName),
  m_EdgeDataContainerName(DREAM3D::HDF5::EdgeDataContainerName),
  m_VertexDataContainerName(DREAM3D::HDF5::VertexDataContainerName),
  m_InputFile(""),
  m_ReadVolumeData(false),
  m_ReadSurfaceData(false),
  m_ReadVertexData(false),
  m_ReadEdgeData(false),
  m_ReadAllVertexArrays(false),
  m_ReadAllVertexFieldArrays(false),
  m_ReadAllVertexEnsembleArrays(false),
  m_ReadAllEdgeArrays(false),
  m_ReadAllEdgeFieldArrays(false),
  m_ReadAllEdgeEnsembleArrays(false),
  m_ReadAllFaceArrays(false),
  m_ReadAllFaceFieldArrays(false),
  m_ReadAllFaceEnsembleArrays(false),
  m_ReadAllCellArrays(false),
  m_ReadAllCellFieldArrays(false),
  m_ReadAllCellEnsembleArrays(false),
  m_ReadAllArrays(false)
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

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::preflight()
{
  dataCheck(true, 1, 1, 1);

  readData(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::execute()
{
  int32_t err = 0;
  QString ss;

  readData(false);

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
void DataContainerReader::readData(bool preflight)
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
    fVersion = m_FileVersion.toFloat(&check);
    if(fVersion < 5.0 || err < 0)
    {
      QH5Utilities::closeFile(fileId);
      fileId = QH5Utilities::openFile(m_InputFile, false); // Re-Open the file as Read/Write
      err = H5Lmove(fileId, "VoxelDataContainer", fileId, DREAM3D::HDF5::VolumeDataContainerName.toLatin1().data(), H5P_DEFAULT, H5P_DEFAULT);
      err = H5Lmove(fileId, "SurfaceMeshDataContainer", fileId, DREAM3D::HDF5::SurfaceDataContainerName.toLatin1().data(), H5P_DEFAULT, H5P_DEFAULT);
      err = QH5Lite::writeStringAttribute(fileId, "/", DREAM3D::HDF5::FileVersionName, "5.0");
      QH5Utilities::closeFile(fileId);
      fileId = QH5Utilities::openFile(m_InputFile, true); // Re-Open the file as Read Only
    }
    if(fVersion < 6.0)
    {
      ss = QObject::tr(": File unable to be read - file structure older than 6.0 '%1'").arg(ClassName());
      setErrorCondition(-250);
      addErrorMessage(getHumanLabel(), ss, err);
      return;
    }
    hid_t dcGid = H5Gopen(fileId, DREAM3D::HDF5::DataContainerName.toLatin1().data(), 0);

    QList<QString> dcNames;
    err = QH5Utilities::getGroupObjects(dcGid, H5Utilities::CustomHDFDataTypes::H5Support_GROUP, dcNames);

    // This will make sure if we return early from this method that the HDF5 File is properly closed.
    HDF5ScopedFileSentinel scopedFileSentinel(&fileId, true);

    int32_t dcType = DREAM3D::DataContainerType::UnknownDataContainer;
    for(int iter = 0; iter < dcNames.size(); iter++)
    {
      err = QH5Lite::readScalarAttribute(dcGid, dcNames[iter], DREAM3D::HDF5::DataContainerType, dcType);
      if((dcType == DREAM3D::DataContainerType::VolumeDataContainer && m_ReadVolumeData == true) ||
      (dcType == DREAM3D::DataContainerType::SurfaceDataContainer && m_ReadSurfaceData == true) ||
      (dcType == DREAM3D::DataContainerType::EdgeDataContainer && m_ReadEdgeData == true) ||
      (dcType == DREAM3D::DataContainerType::VertexDataContainer && m_ReadVertexData == true))
      {
        if(dcType == DREAM3D::DataContainerType::VolumeDataContainer) 
        {
          VolumeDataContainer::Pointer dc = VolumeDataContainer::New();
          dc->setName(dcNames[iter]);
          getDataContainerArray()->pushBack(dc);
        }
        if(dcType == DREAM3D::DataContainerType::SurfaceDataContainer) 
        {
          SurfaceDataContainer::Pointer dc = SurfaceDataContainer::New();
          dc->setName(dcNames[iter]);
          getDataContainerArray()->pushBack(dc);
        }
        if(dcType == DREAM3D::DataContainerType::EdgeDataContainer) 
        {
          EdgeDataContainer::Pointer dc = EdgeDataContainer::New();
          dc->setName(dcNames[iter]);
          getDataContainerArray()->pushBack(dc);
        }
        if(dcType == DREAM3D::DataContainerType::VertexDataContainer) 
        {
          VertexDataContainer::Pointer dc = VertexDataContainer::New();
          dc->setName(dcNames[iter]);
          getDataContainerArray()->pushBack(dc);
        }

        VolumeDataContainerReader::Pointer volumeReader = VolumeDataContainerReader::New();
        volumeReader->setCellArraysToRead(m_SelectedVolumeCellArrays);
        volumeReader->setCellFieldArraysToRead(m_SelectedVolumeCellFieldArrays);
        volumeReader->setCellEnsembleArraysToRead(m_SelectedVolumeCellEnsembleArrays);
        volumeReader->setReadAllCellArrays(m_ReadAllCellArrays);
        volumeReader->setReadAllCellFieldArrays(m_ReadAllCellFieldArrays);
        volumeReader->setReadAllCellEnsembleArrays(m_ReadAllCellEnsembleArrays);
        volumeReader->setReadAllArrays(m_ReadAllArrays);
        volumeReader->setHdfFileId(fileId);
        volumeReader->setHdfGroupId(dcGid);
        volumeReader->setDataContainer(getDataContainerArray()->getDataContainer(dcNames[iter]).get());
        volumeReader->setObservers(getObservers());
        ss = getMessagePrefix() + " |--> Reading Volume Data ";
        volumeReader->setMessagePrefix(ss);
        if(preflight == true) volumeReader->preflight();
        else volumeReader->execute();
        if (volumeReader->getErrorCondition() < 0)
        {
          if(preflight == true)
          {
            setReadVolumeData(false);
            setErrorCondition(volumeReader->getErrorCondition());
            addErrorMessage(getHumanLabel(), "The volume data was not available in the data file.", getErrorCondition());
          }
          else
          {
            notifyErrorMessage("Error Reading the Volume Data", volumeReader->getErrorCondition());
            return;
          }
        }

        SurfaceDataContainerReader::Pointer smReader = SurfaceDataContainerReader::New();
        smReader->setFaceArraysToRead(m_SelectedSurfaceFaceArrays);
        smReader->setFaceFieldArraysToRead(m_SelectedSurfaceFaceFieldArrays);
        smReader->setFaceEnsembleArraysToRead(m_SelectedSurfaceFaceEnsembleArrays);
        smReader->setReadAllFaceArrays(m_ReadAllFaceArrays);
        smReader->setReadAllFaceFieldArrays(m_ReadAllFaceFieldArrays);
        smReader->setReadAllFaceEnsembleArrays(m_ReadAllFaceEnsembleArrays);
        smReader->setReadAllArrays(m_ReadAllArrays);
        smReader->setHdfFileId(fileId);
        smReader->setHdfGroupId(dcGid);
        smReader->setDataContainer(getDataContainerArray()->getDataContainer(dcNames[iter]).get());
        smReader->setObservers(getObservers());
        ss = getMessagePrefix() + " |--> Reading Surface Data ";
        smReader->setMessagePrefix(ss);
        if(preflight == true) smReader->preflight();
        else smReader->execute();
        if (smReader->getErrorCondition() < 0)
        {
          if(preflight == true)
          {
            setReadSurfaceData(false);
            setErrorCondition(smReader->getErrorCondition());
            addErrorMessage(getHumanLabel(), "The surface mesh data was not available in the data file.", getErrorCondition());
          }
          else
          {
            notifyErrorMessage("Error Reading the Surface Data", smReader->getErrorCondition());
            return;
          }
        }

        EdgeDataContainerReader::Pointer eReader = EdgeDataContainerReader::New();
        eReader->setEdgeArraysToRead(m_SelectedEdgeEdgeArrays);
        eReader->setEdgeFieldArraysToRead(m_SelectedEdgeEdgeFieldArrays);
        eReader->setEdgeEnsembleArraysToRead(m_SelectedEdgeEdgeEnsembleArrays);
        eReader->setReadAllEdgeArrays(m_ReadAllEdgeArrays);
        eReader->setReadAllEdgeFieldArrays(m_ReadAllEdgeFieldArrays);
        eReader->setReadAllEdgeEnsembleArrays(m_ReadAllEdgeEnsembleArrays);
        eReader->setReadAllArrays(m_ReadAllArrays);
        eReader->setHdfFileId(fileId);
        eReader->setHdfGroupId(dcGid);
        eReader->setDataContainer(getDataContainerArray()->getDataContainer(dcNames[iter]).get());
        eReader->setObservers(getObservers());
        ss = getMessagePrefix() + " |--> Reading Surface Data ";
        eReader->setMessagePrefix(ss);
        if(preflight == true) eReader->preflight();
        else eReader->execute();
        if (eReader->getErrorCondition() < 0)
        {
          if(preflight == true)
          {
            setReadEdgeData(false);
            setErrorCondition(eReader->getErrorCondition());
            addErrorMessage(getHumanLabel(), "The edge mesh data was not available in the data file.", getErrorCondition());
          }
          else
          {
            notifyErrorMessage("Error Reading the Edge Data", eReader->getErrorCondition());
            return;
          }
        }

        VertexDataContainerReader::Pointer vReader = VertexDataContainerReader::New();
        vReader->setVertexArraysToRead(m_SelectedVertexVertexArrays);
        vReader->setVertexFieldArraysToRead(m_SelectedVertexVertexFieldArrays);
        vReader->setVertexEnsembleArraysToRead(m_SelectedVertexVertexEnsembleArrays);
        vReader->setReadAllVertexArrays(m_ReadAllVertexArrays);
        vReader->setReadAllVertexFieldArrays(m_ReadAllVertexFieldArrays);
        vReader->setReadAllVertexEnsembleArrays(m_ReadAllVertexEnsembleArrays);
        vReader->setReadAllArrays(m_ReadAllArrays);
        vReader->setHdfFileId(fileId);
        vReader->setHdfGroupId(dcGid);
        vReader->setDataContainer(getDataContainerArray()->getDataContainer(dcNames[iter]).get());
        vReader->setObservers(getObservers());
        ss = getMessagePrefix() + " |--> Reading Solid Mesh Data ";
        vReader->setMessagePrefix(ss);
        if(preflight == true) vReader->preflight();
        else vReader->execute();
        if (vReader->getErrorCondition() < 0)
        {
          if(preflight == true)
          {
            setReadVertexData(false);
            setErrorCondition(vReader->getErrorCondition());
            addErrorMessage(getHumanLabel(), "The solid mesh data was not available in the data file.", getErrorCondition());
          }
          else
          {
            notifyErrorMessage("Error Reading the Vertex Data", vReader->getErrorCondition());
            return;
          }
        }
      }
    }
  }

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




