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

#include "DREAM3DLib/IOFilters/VoxelDataContainerReader.h"
#include "DREAM3DLib/IOFilters/SurfaceMeshDataContainerReader.h"
#include "DREAM3DLib/IOFilters/SolidMeshDataContainerReader.h"
#include "DREAM3DLib/HDF5/H5FilterParametersReader.h"
#include "DREAM3DLib/Common/FilterManager.h"

#include "H5Support/HDF5ScopedFileSentinel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReader::DataContainerReader() :
  AbstractFilter(),
  m_InputFile(""),
  m_ReadVoxelData(true),
  m_ReadSurfaceMeshData(false),
  m_ReadSolidMeshData(false),
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
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Read Voxel Data");
    option->setPropertyName("ReadVoxelData");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Read SurfaceMesh Data");
    option->setPropertyName("ReadSurfaceMeshData");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Read SolidMesh Data");
    option->setPropertyName("ReadSolidMeshData");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }

  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Arrays to Read");
    parameter->setPropertyName("ArraysToRead");
    parameter->setWidgetType(FilterParameter::ArraySelectionWidget);
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
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
  writer->writeValue("ReadVoxelData", getReadVoxelData() );
  writer->writeValue("ReadSurfaceMeshData", getReadSurfaceMeshData() );
  writer->writeValue("ReadSolidMeshData", getReadSolidMeshData() );
  writer->closeFilterGroup();
  return ++index; // we want to return the index after the one we just wrote to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  int32_t err = 0;

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

    /* READ THE VOXEL DATA TO THE HDF5 FILE */
    if (getVoxelDataContainer() != NULL && m_ReadVoxelData == true)
    {
      VoxelDataContainerReader::Pointer voxelReader = VoxelDataContainerReader::New();
      voxelReader->setHdfFileId(fileId);
      voxelReader->setVoxelDataContainer(getVoxelDataContainer());
      voxelReader->setObservers(getObservers());
      ss.str("");
      ss << getMessagePrefix() << " |--> Reading Voxel Data ";
      voxelReader->setMessagePrefix(ss.str());
      voxelReader->preflight();
      if (voxelReader->getErrorCondition() < 0)
      {
        setReadVoxelData(false);
        setErrorCondition(voxelReader->getErrorCondition());
        addErrorMessage(getHumanLabel(), "The voxel data was not available in the data file.", getErrorCondition());
      }
    }

    /* READ THE SurfaceMesh DATA TO THE HDF5 FILE */
    if (NULL != getSurfaceMeshDataContainer() && m_ReadSurfaceMeshData == true)
    {
      SurfaceMeshDataContainerReader::Pointer smReader = SurfaceMeshDataContainerReader::New();
      smReader->setHdfFileId(fileId);
      smReader->setSurfaceMeshDataContainer(getSurfaceMeshDataContainer());
      smReader->setObservers(getObservers());
      ss.str("");
      ss << getMessagePrefix() << " |--> Reading SurfaceMesh Data ";
      smReader->setMessagePrefix(ss.str());
      smReader->preflight();
      if (smReader->getErrorCondition() < 0)
      {
        setReadSurfaceMeshData(false);
        setErrorCondition(smReader->getErrorCondition());
        addErrorMessage(getHumanLabel(), "The surface mesh data was not available in the data file.", getErrorCondition());
      }
    }

    /* READ THE SolidMesh DATA TO THE HDF5 FILE */
    if (NULL != getSolidMeshDataContainer() && m_ReadSolidMeshData == true)
    {
      SolidMeshDataContainerReader::Pointer smReader = SolidMeshDataContainerReader::New();
      smReader->setHdfFileId(fileId);
      smReader->setSolidMeshDataContainer(getSolidMeshDataContainer());
      smReader->setObservers(getObservers());
      ss.str("");
      ss << getMessagePrefix() << " |--> Reading Solid Mesh Data ";
      smReader->setMessagePrefix(ss.str());
      smReader->preflight();
      if (smReader->getErrorCondition() < 0)
      {
        setReadSolidMeshData(false);
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
  std::stringstream ss;
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
  std::string fileVersion;

  err = H5Lite::readStringAttribute(fileId, "/", DREAM3D::HDF5::FileVersionName, fileVersion);

  err = readExistingPipelineFromFile(fileId);

  /* READ THE VOXEL DATA TO THE HDF5 FILE */
  if (getVoxelDataContainer() != NULL && m_ReadVoxelData == true)
  {
    VoxelDataContainerReader::Pointer voxelReader = VoxelDataContainerReader::New();
    voxelReader->setHdfFileId(fileId);
    voxelReader->setCellArraysToRead(m_SelectedVoxelCellArrays);
    voxelReader->setFieldArraysToRead(m_SelectedVoxelFieldArrays);
    voxelReader->setEnsembleArraysToRead(m_SelectedVoxelEnsembleArrays);
    voxelReader->setReadAllArrays(m_ReadAllArrays);
    voxelReader->setVoxelDataContainer(getVoxelDataContainer());
    voxelReader->setObservers(getObservers());
    ss.str("");
    ss << getMessagePrefix() << " |--> Reading Voxel Data ";
    voxelReader->setMessagePrefix(ss.str());
    voxelReader->execute();
    if (voxelReader->getErrorCondition() < 0)
    {
      notifyErrorMessage("Error Reading the Voxel Data", -803);
      return;
    }
  }

  /* READ THE SurfaceMesh DATA TO THE HDF5 FILE */
  if (NULL != getSurfaceMeshDataContainer() && m_ReadSurfaceMeshData == true)
  {
    SurfaceMeshDataContainerReader::Pointer smReader = SurfaceMeshDataContainerReader::New();
    smReader->setHdfFileId(fileId);
    smReader->setVertexArraysToRead(m_SelectedSurfaceMeshVertexArrays);
    smReader->setFaceArraysToRead(m_SelectedSurfaceMeshFaceArrays);
    smReader->setEdgeArraysToRead(m_SelectedSurfaceMeshEdgeArrays);
    smReader->setFieldArraysToRead(m_SelectedSurfaceMeshFieldArrays);
    smReader->setEnsembleArraysToRead(m_SelectedSurfaceMeshEnsembleArrays);
    smReader->setReadAllArrays(m_ReadAllArrays);
    smReader->setSurfaceMeshDataContainer(getSurfaceMeshDataContainer());
    smReader->setObservers(getObservers());
    ss.str("");
    ss << getMessagePrefix() << " |--> Reading SurfaceMesh Data ";
    smReader->setMessagePrefix(ss.str());
    smReader->execute();
    if (smReader->getErrorCondition() < 0)
    {
      notifyErrorMessage("Error Reading the SurfaceMesh Data", smReader->getErrorCondition());
      return;
    }
  }

  /* READ THE SolidMesh DATA TO THE HDF5 FILE */
  if (NULL != getSolidMeshDataContainer() && m_ReadSolidMeshData == true)
  {
    SolidMeshDataContainerReader::Pointer smReader = SolidMeshDataContainerReader::New();
    smReader->setHdfFileId(fileId);
    smReader->setCellArraysToRead(m_SelectedSolidMeshVertexArrays);
    smReader->setFieldArraysToRead(m_SelectedSolidMeshFaceArrays);
    smReader->setEnsembleArraysToRead(m_SelectedSolidMeshEdgeArrays);
    smReader->setReadAllArrays(m_ReadAllArrays);
    smReader->setSolidMeshDataContainer(getSolidMeshDataContainer());
    smReader->setObservers(getObservers());
    ss.str("");
    ss << getMessagePrefix() << " |--> Reading Solid Mesh Data ";
    smReader->setMessagePrefix(ss.str());
    smReader->execute();
    if (smReader->getErrorCondition() < 0)
    {
      notifyErrorMessage("Error Reading the Solid Mesh Data", smReader->getErrorCondition());
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
  std::list<std::string> groupList;
  err = H5Utilities::getGroupObjects(pipelineGroupId, H5Utilities::H5Support_GROUP, groupList);

  // Loop over the items getting the "ClassName" attribute from each group
  std::string classNameStr = "";
  for (int i=0; i<groupList.size(); i++)
  {
    std::stringstream ss;
    ss << i;
    err = H5Lite::readStringAttribute(pipelineGroupId, ss.str(), "ClassName", classNameStr);

    // Instantiate a new filter using the FilterFactory based on the value of the className attribute
    FilterManager::Pointer fm = FilterManager::Instance();
    IFilterFactory::Pointer ff = fm->getFactoryForFilter(classNameStr);
    AbstractFilter::Pointer filter = ff->create();

    // Read the parameters
    filter->readFilterParameters( reader.get(), i);

    // Add filter to m_PipelineFromFile
    m_PipelineFromFile->pushBack(filter);
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
void DataContainerReader::setVoxelSelectedArrayNames(std::set<std::string> selectedCellArrays,
                                                     std::set<std::string> selectedFieldArrays,
                                                     std::set<std::string> selectedEnsembleArrays)
{
  m_SelectedVoxelCellArrays = selectedCellArrays;
  m_SelectedVoxelFieldArrays = selectedFieldArrays;
  m_SelectedVoxelEnsembleArrays = selectedEnsembleArrays;
  m_ReadAllArrays = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::setSurfaceMeshSelectedArrayNames(std::set<std::string> selectedVertexArrays,
                                                           std::set<std::string> selectedFaceArrays,
                                                           std::set<std::string> selectedEdgeArrays,
                                                           std::set<std::string> selectedFieldArrays,
                                                           std::set<std::string> selectedEnsembleArrays)
{
  m_SelectedSurfaceMeshVertexArrays = selectedVertexArrays;
  m_SelectedSurfaceMeshFaceArrays = selectedFaceArrays;
  m_SelectedSurfaceMeshEdgeArrays = selectedEdgeArrays;
  m_SelectedSurfaceMeshFieldArrays = selectedFieldArrays;
  m_SelectedSurfaceMeshEnsembleArrays = selectedEnsembleArrays;
  m_ReadAllArrays = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::setSolidMeshSelectedArrayNames(std::set<std::string> selectedVertexArrays,
                                                         std::set<std::string> selectedFaceArrays,
                                                         std::set<std::string> selectedEdgeArrays)
{
  m_SelectedSolidMeshVertexArrays = selectedVertexArrays;
  m_SelectedSolidMeshFaceArrays = selectedFaceArrays;
  m_SelectedSolidMeshEdgeArrays = selectedEdgeArrays;
  m_ReadAllArrays = false;
}




