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

#include "DREAM3DLib/FilterParameters/H5FilterParametersReader.h"
#include "DREAM3DLib/Common/FilterManager.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReader::DataContainerReader() :
  AbstractFilter(),
  m_InputFile(""),
  m_OverwriteExistingDataContainers(false)
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
  FilterParameterVector parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputFile(reader->readString("InputFile", getInputFile() ) );

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


  writer->closeFilterGroup();
  return ++index; // we want to return the index after the one we just wrote to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::dataCheck(bool preflight, size_t volumes, size_t features, size_t ensembles)
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
  hid_t pipelineGroupId = H5Gopen(fileId, DREAM3D::StringConstants::PipelineGroupName.toLatin1().data(), H5P_DEFAULT);
  reader->setPipelineGroupId(pipelineGroupId);

  // Use QH5Lite to ask how many "groups" are in the "Pipeline Group"
  QList<QString> groupList;
  err = QH5Utilities::getGroupObjects(pipelineGroupId, H5Utilities::H5Support_GROUP, groupList);

  // Loop over the items getting the "ClassName" attribute from each group
  QString classNameStr = "";
  for (int i = 0; i < groupList.size(); i++)
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
      err = H5Lmove(fileId, "VoxelDataContainer", fileId, DREAM3D::Defaults::VolumeDataContainerName.toLatin1().data(), H5P_DEFAULT, H5P_DEFAULT);
      err = H5Lmove(fileId, "SurfaceMeshDataContainer", fileId, DREAM3D::Defaults::SurfaceDataContainerName.toLatin1().data(), H5P_DEFAULT, H5P_DEFAULT);
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
    hid_t dcaGid = H5Gopen(fileId, DREAM3D::StringConstants::DataContainerGroupName.toLatin1().data(), 0);

    // This will make sure if we return early from this method that the HDF5 File is properly closed.
    HDF5ScopedFileSentinel scopedFileSentinel(&fileId, true);
    scopedFileSentinel.addGroupId(&dcaGid);


    QList<QString> dcNames;
    err = QH5Utilities::getGroupObjects(dcaGid, H5Utilities::H5Support_GROUP, dcNames);
    DataContainerArray::Pointer dca = getDataContainerArray();

    uint32_t dcType = DREAM3D::DataContainerType::UnknownDataContainer;
    for(int iter = 0; iter < dcNames.size(); iter++)
    {
      if (dca->contains(dcNames[iter]) == true )
      {
        setErrorCondition(-10987);
        QString ss = QObject::tr("A Data Container with name %1 already exists in Memory. Reading a Data Container with the same name would over write the one in memory. Currently this is not allowed.").arg(dcNames[iter]);
        notifyErrorMessage(ss, getErrorCondition());
        return;
      }
      err = QH5Lite::readScalarAttribute(dcaGid, dcNames[iter], DREAM3D::StringConstants::DataContainerType, dcType);
      if (err < 0)
      {
        setErrorCondition(-109283);
        QString ss = QObject::tr("The DataContainer is missing the 'DataContainerType' attribute on the '%1' Data Container").arg(dcNames[iter]);
        notifyErrorMessage(ss, getErrorCondition());
        return;
      }
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
      hid_t dcGid = H5Gopen(dcaGid, dcNames[iter].toLatin1().data(), H5P_DEFAULT );
      if (dcGid < 0)
      {
        QString ss = QObject::tr("Error opening Group %1").arg(dcNames[iter]);
        setErrorCondition(-61);
        addErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
      MapOfAttributeMatrices_t::Iterator it = m_DataToRead.find(dcNames[iter]);
      MapOfAttributeArrays_t arraysToRead = it.value();
      getDataContainerArray()->getDataContainer(dcNames[iter])->readMeshDataFromHDF5(dcGid, preflight);
      err = getDataContainerArray()->getDataContainer(dcNames[iter])->readAttributeMatricesFromHDF5(preflight, dcGid, arraysToRead);
      if(err < 0)
      {
        if(preflight == true)
        {
          addErrorMessage(getHumanLabel(), "The data was not available in the data file.", getErrorCondition());
        }
        else
        {
          notifyErrorMessage("Error Reading Data", -100);
          return;
        }
      }
    }

    err = H5Gclose(dcaGid);
    dcaGid = -1;
  }
}
