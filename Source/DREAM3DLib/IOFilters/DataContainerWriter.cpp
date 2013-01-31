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

#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"

#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/Common/H5FilterParametersWriter.h"
#include "DREAM3DLib/IOFilters/VoxelDataContainerWriter.h"
#include "DREAM3DLib/IOFilters/SurfaceMeshDataContainerWriter.h"
//#include "DREAM3DLib/IOFilters/SolidMeshDataContainerWriter.h"

#define APPEND_DATA_TRUE 1
#define APPEND_DATA_FALSE 0


/**
 * @brief The HDF5FileSentinel class ensures the HDF5 file that is currently open
 * is closed when the variable goes out of Scope
 */
class HDF5ScopedFileSentinel
{
  public:
    HDF5ScopedFileSentinel(hid_t fileId) : m_FileId(fileId)
    {}
    virtual ~HDF5ScopedFileSentinel()
    {
      if (m_FileId > 0) {
        H5Utilities::closeFile(m_FileId);
      }
    }

    DREAM3D_INSTANCE_PROPERTY(hid_t, FileId)

};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerWriter::DataContainerWriter() :
  AbstractFilter(),
  m_WritePipeline(true),
  m_WriteVoxelData(true),
  m_WriteSurfaceMeshData(true),
  m_WriteSolidMeshData(true),
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
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("dream3d");
    option->setFileType("DREAM.3D Data");
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Voxel Data");
    option->setPropertyName("WriteVoxelData");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write SurfaceMesh Data");
    option->setPropertyName("WriteSurfaceMeshData");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Solid Mesh");
    option->setPropertyName("WriteSolidMeshData");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("OutputFile", getOutputFile() );
  writer->writeValue("WriteVoxelData", getWriteVoxelData() );
  writer->writeValue("WriteSurfaceMeshData", getWriteSurfaceMeshData() );
  writer->writeValue("WriteSolidMeshData", getWriteSolidMeshData() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;

  if (m_OutputFile.empty() == true)
  {
    ss <<  ": The output file must be set before executing this filter.";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    setErrorCondition(-1);
  }

  std::string parentPath = MXAFileInfo::parentPath(m_OutputFile);
  if (MXADir::exists(parentPath) == false)
  {
    ss.str("");
    ss <<  "The directory path for the output file does not exist.";
    addWarningMessage(getHumanLabel(), ss.str(), -1);
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


  VoxelDataContainer* m = getVoxelDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss <<  " DataContainer was NULL";
    notifyErrorMessage(ss.str(), -10);
    return;
  }
  setErrorCondition(0);
  dataCheck(false, 1, 1, 1);

  std::stringstream ss;
  int err = 0;

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  std::string parentPath = MXAFileInfo::parentPath(m_OutputFile);
  if(!MXADir::mkdir(parentPath, true))
  {
    std::stringstream ss;
    ss << "Error creating parent path '" << parentPath << "'";
    notifyErrorMessage(ss.str(), -1);
    setErrorCondition(-1);
    return;
  }

  err = openFile(false); // Do NOT append to any existing file
  if (err < 0)
  {
    ss.str("");
    ss <<  ": The hdf5 file could not be opened or created.\n The Given filename was:\n\t[" << m_OutputFile<< "]";
    setErrorCondition(-59);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    return;
  }

  // This will make sure if we return early from this method that the HDF5 File is properly closed.
  HDF5ScopedFileSentinel scopedFileSentinel(0);
  scopedFileSentinel.setFileId(m_FileId);

  // Write our File Version string to the Root "/" group
  H5Lite::writeStringAttribute(m_FileId, "/", DREAM3D::HDF5::FileVersionName, DREAM3D::HDF5::FileVersion);

  // Write the Pipeline to the File
  err = writePipeline();

  /* WRITE THE VOXEL DATA TO THE HDF5 FILE */
  if (getVoxelDataContainer() != NULL && m_WriteVoxelData == true)
  {
    VoxelDataContainerWriter::Pointer voxelWriter = VoxelDataContainerWriter::New();
    voxelWriter->setHdfFileId(m_FileId);
    voxelWriter->setVoxelDataContainer(getVoxelDataContainer());
    voxelWriter->setObservers(getObservers());
    ss.str("");
    ss << getMessagePrefix() << " |--> Writing Voxel Data ";
    voxelWriter->setMessagePrefix(ss.str());
    voxelWriter->execute();
    if (voxelWriter->getErrorCondition() < 0)
    {
      notifyErrorMessage("Error Writing the Voxel Data", -803);
      return;
    }
  }

  /* WRITE THE SurfaceMesh DATA TO THE HDF5 FILE */
  if (NULL != getSurfaceMeshDataContainer() && m_WriteSurfaceMeshData)
  {
    SurfaceMeshDataContainerWriter::Pointer smWriter = SurfaceMeshDataContainerWriter::New();
    smWriter->setHdfFileId(m_FileId);
    smWriter->setSurfaceMeshDataContainer(getSurfaceMeshDataContainer());
    smWriter->setObservers(getObservers());
    ss.str("");
    ss << getMessagePrefix() << " |--> Writing SurfaceMesh Data ";
    smWriter->setMessagePrefix(ss.str());
    smWriter->execute();
    if (smWriter->getErrorCondition() < 0)
    {
      notifyErrorMessage("Error Writing the SurfaceMesh Data", smWriter->getErrorCondition());
      return;
    }
  }


  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerWriter::writePipeline()
{

  // WRITE THE PIPELINE TO THE HDF5 FILE
  H5FilterParametersWriter::Pointer optionsWriter = H5FilterParametersWriter::New();
  hid_t pipelineGroupId = H5Utilities::createGroup(m_FileId, DREAM3D::HDF5::PipelineGroupName);
  optionsWriter->setGroupId(pipelineGroupId);

  AbstractFilter::Pointer preFilter = getPreviousFilter();
  while (preFilter.get() != NULL)
  {
    optionsWriter->openOptionsGroup(preFilter.get());
    preFilter->writeFilterParameters(optionsWriter.get());
    optionsWriter->closeOptionsGroup();
    preFilter = preFilter->getPreviousFilter();
  }
  optionsWriter->openOptionsGroup(this);
  writeFilterParameters(optionsWriter.get());
  optionsWriter->closeOptionsGroup();

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
    m_FileId = H5Utilities::openFile(m_OutputFile, false);
  }
  // No file was found or we are writing new data only to a clean file
  if (APPEND_DATA_FALSE == appendData || m_FileId < 0)
  {
    m_FileId = H5Utilities::createFile (m_OutputFile);
  }
  return m_FileId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerWriter::closeFile()
{
  // Close the file when we are finished with it
  return H5Utilities::closeFile(m_FileId);
}

