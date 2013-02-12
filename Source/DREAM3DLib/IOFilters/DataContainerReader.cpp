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


/**
 * @brief The HDF5FileSentinel class ensures the HDF5 file that is currently open
 * is closed when the variable goes out of Scope
 */
class HDF5ScopedFileSentinel
{
  public:
    HDF5ScopedFileSentinel(hid_t fileId, bool turnOffErrors) : m_FileId(fileId), m_TurnOffErrors(turnOffErrors)
    {
      if (m_TurnOffErrors == true)
      {
        H5Eget_auto(H5E_DEFAULT, &_oldHDF_error_func, &_oldHDF_error_client_data);\
        H5Eset_auto(H5E_DEFAULT, NULL, NULL);
      }

    }
    virtual ~HDF5ScopedFileSentinel()
    {
      if (m_TurnOffErrors == true)
      {
        H5Eset_auto(H5E_DEFAULT, _oldHDF_error_func, _oldHDF_error_client_data);
      }
      if (m_FileId > 0) {
        H5Utilities::closeFile(m_FileId);
      }

    }

    DREAM3D_INSTANCE_PROPERTY(hid_t, FileId)
    DREAM3D_INSTANCE_PROPERTY(bool, TurnOffErrors)

  private:
    herr_t (*_oldHDF_error_func)(hid_t, void *);
    void* _oldHDF_error_client_data;
};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReader::DataContainerReader() :
  AbstractFilter(),
  m_InputFile(""),
  m_ReadVoxelData(true),
  m_ReadSurfaceMeshData(true),
  m_ReadSolidMeshData(false)
{
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
  //  {
  //    FilterParameter::Pointer option = FilterParameter::New();
  //    option->setHumanLabel("Read SolidMesh Data");
  //    option->setPropertyName("ReadSolidMeshData");
  //    option->setWidgetType(FilterParameter::BooleanWidget);
  //    option->setValueType("bool");
  //    parameters.push_back(option);
  //  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("ReadVoxelData", getReadVoxelData() );
  writer->writeValue("ReadSurfaceMeshData", getReadSurfaceMeshData() );
  writer->writeValue("ReadSolidMeshData", getReadSolidMeshData() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  int32_t err = 0;

  if(m_InputFile.empty() == true)
  {
    ss << "The input file must be set before executing this filter.";
    addErrorMessage(getHumanLabel(), ss.str(), 1);
    setErrorCondition(-1);
  }
  else if (MXAFileInfo::exists(m_InputFile) == false)
  {
    ss << "The input file does not exist.";
    PipelineMessage em (getHumanLabel(), ss.str(), -1);
    addErrorMessage(em);
    setErrorCondition(-1);
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
    HDF5ScopedFileSentinel scopedFileSentinel(fileId, true);

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
        notifyErrorMessage("Error Reading the Voxel Data", -803);
        return;
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
        notifyErrorMessage("Error Reading the SurfaceMesh Data", smReader->getErrorCondition());
        return;
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
        notifyErrorMessage("Error Reading the Solid Mesh Data", smReader->getErrorCondition());
        return;
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
  HDF5ScopedFileSentinel scopedFileSentinel(fileId, true);

  // Read our File Version string to the Root "/" group
  std::string fileVersion;

  H5Lite::readStringAttribute(fileId, "/", DREAM3D::HDF5::FileVersionName, fileVersion);


  /* READ THE VOXEL DATA TO THE HDF5 FILE */
  if (getVoxelDataContainer() != NULL && m_ReadVoxelData == true)
  {
    VoxelDataContainerReader::Pointer voxelReader = VoxelDataContainerReader::New();
    voxelReader->setHdfFileId(fileId);
    voxelReader->setCellArraysToRead(m_SelectedVoxelCellArrays);
    voxelReader->setFieldArraysToRead(m_SelectedVoxelFieldArrays);
    voxelReader->setEnsembleArraysToRead(m_SelectedVoxelEnsembleArrays);

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
void DataContainerReader::setVoxelSelectedArrayNames(std::set<std::string> selectedCellArrays,
                                                     std::set<std::string> selectedFieldArrays,
                                                     std::set<std::string> selectedEnsembleArrays)
{
  m_SelectedVoxelCellArrays = selectedCellArrays;
  m_SelectedVoxelFieldArrays = selectedFieldArrays;
  m_SelectedVoxelEnsembleArrays = selectedEnsembleArrays;
}
