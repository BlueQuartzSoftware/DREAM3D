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

#include "DREAM3DLib/Common/H5FilterOptionsWriter.h"

#define APPEND_DATA_TRUE 1
#define APPEND_DATA_FALSE 0

typedef std::list<std::string> NameListType;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerWriter::DataContainerWriter() :
AbstractFilter(),
m_FileId(-1)
{
  setupFilterOptions();
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
void DataContainerWriter::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterOption::OutputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  setFilterOptions(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerWriter::writeFilterOptions(AbstractFilterOptionsWriter* writer)
{
  writer->writeValue("OutputFile", getOutputFile() );
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
    addErrorMessage(getNameOfClass(), ss.str(), -1);
    setErrorCondition(-1);
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


  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss <<  " DataContainer was NULL";
    addErrorMessage(getNameOfClass(), ss.str(), -10);
    return;
  }
  setErrorCondition(0);
  dataCheck(false, 1, 1, 1);

  std::stringstream ss;
  int err = 0;
  hid_t dcGid = -1;


  int64_t volDims[3] =
  { m->getXPoints(), m->getYPoints(), m->getZPoints() };
  float spacing[3] =
  { m->getXRes(), m->getYRes(), m->getZRes() };
  float origin[3] =
  { 0.0f, 0.0f, 0.0f };

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  std::string parentPath = MXAFileInfo::parentPath(m_OutputFile);
  if(!MXADir::mkdir(parentPath, true))
  {
      std::stringstream ss;
      ErrorMessage::Pointer em = ErrorMessage::New(getNameOfClass(), ss.str(), -1);
      addErrorMessage(em);
      setErrorCondition(-1);
      return;
  }



  err = openFile(false); // Do NOT append to any existing file
  if (err < 0)
  {
    ss.str("");
    ss <<  ": The hdf5 file could not be opened or created.\n The Given filename was:\n\t[" << m_OutputFile<< "]";
    setErrorCondition(-59);
    addErrorMessage(getNameOfClass(), ss.str(), err);
    return;
  }

  // Create the HDF5 Group for the Data Container
  err = H5Utilities::createGroupsFromPath(DREAM3D::HDF5::DataContainerName.c_str(), m_FileId);
  if (err < 0)
  {
    ss.str("");
    ss << "Error creating HDF Group " << DREAM3D::HDF5::DataContainerName << std::endl;
    setErrorCondition(-60);
    addErrorMessage(getNameOfClass(), ss.str(), err);
    return;
  }
  dcGid = H5Gopen(m_FileId, DREAM3D::HDF5::DataContainerName.c_str(), H5P_DEFAULT );
  if (dcGid < 0)
  {
    ss.str("");
    ss << "Error opening Group " << DREAM3D::HDF5::DataContainerName << std::endl;
    setErrorCondition(-61);
    addErrorMessage(getNameOfClass(), ss.str(), err);
    return;
  }

  // This just writes the header information
  err = writeMetaInfo(DREAM3D::HDF5::DataContainerName, volDims, spacing, origin);
  if (err < 0)
  {
    ss.str("");
    ss <<  ":Error Writing header information to output file" << std::endl;
    setErrorCondition(-62);
    addErrorMessage(getNameOfClass(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }

  err = writeCellData(dcGid);
  if (err < 0)
  {
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }

  err = writeFieldData(dcGid);
  if (err < 0)
  {
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }

  err = writeEnsembleData(dcGid);
  if (err < 0)
  {
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }

  H5FilterOptionsWriter::Pointer optionsWriter = H5FilterOptionsWriter::New();
  hid_t pipelineGroupId = H5Utilities::createGroup(dcGid, DREAM3D::HDF5::PipelineGroupName);
  optionsWriter->setGroupId(pipelineGroupId);

  AbstractFilter::Pointer preFilter = getPreviousFilter();
  while (preFilter.get() != NULL)
  {
	  optionsWriter->openOptionsGroup(preFilter.get());
    preFilter->writeFilterOptions(optionsWriter.get());
    optionsWriter->closeOptionsGroup();
    preFilter = preFilter->getPreviousFilter();
  }
  optionsWriter->openOptionsGroup(this);
  writeFilterOptions(optionsWriter.get());
  optionsWriter->closeOptionsGroup();

  H5Gclose(pipelineGroupId);
  // Now finally close the group and the HDf5 File
  H5Gclose(dcGid); // Close the Data Container Group
  closeFile();

  notifyProgress("Complete", 0, Observable::UpdateProgressMessage);
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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerWriter::writeMetaInfo(const std::string &hdfPath, int64_t volDims[3],
                          float spacing[3], float origin[3])
{
  herr_t err = 0;
  err = createVtkObjectGroup(hdfPath, H5_VTK_STRUCTURED_POINTS);
  if (err < 0)  {
    return err;
  }
  hid_t gid = H5Gopen(m_FileId, hdfPath.c_str(), H5P_DEFAULT );

  int32_t rank =1;
  hsize_t dims[1] = {3};
  err = H5Lite::writePointerDataset(gid, H5_DIMENSIONS, rank, dims, volDims);
  if (err < 0)
  {
     std::cout << "Error Writing H5_DIMENSIONS array for " << hdfPath << std::endl;
  }
  err = H5Lite::writePointerDataset(gid, H5_ORIGIN, rank, dims, origin);
  if (err < 0)
  {
     std::cout << "Error Writing H5_ORIGIN array for " << hdfPath << std::endl;
  }
  err = H5Lite::writePointerDataset(gid, H5_SPACING, rank, dims, spacing);
  if (err < 0)
  {
     std::cout << "Error Writing H5_SPACING array for " << hdfPath << std::endl;
  }
  int64_t nPoints = volDims[0] * volDims[1] * volDims[2];
  err = H5Lite::writeScalarAttribute(m_FileId, hdfPath, H5_NUMBER_OF_POINTS, nPoints);
  if (err < 0)
  {
    std::cout << "Error Writing H5_NUMBER_OF_POINTS attribute for " << hdfPath << std::endl;
  }

  err |= H5Gclose(gid);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerWriter::createVtkObjectGroup(const std::string &hdfGroupPath, const char* vtkDataObjectType)
{
  // std::cout << "   vtkH5DataWriter::WritePoints()" << std::endl;
  herr_t err = H5Utilities::createGroupsFromPath(hdfGroupPath, m_FileId);
  if (err < 0)
  {
    std::cout << "Error creating HDF Group " << hdfGroupPath << std::endl;
  }
  err = H5Lite::writeStringAttribute(m_FileId, hdfGroupPath, H5_VTK_DATA_OBJECT, vtkDataObjectType );
  if(err < 0)
  {
    std::cout << "Error writing string attribute to HDF Group " << hdfGroupPath << std::endl;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerWriter::writeCellData(hid_t dcGid)
{
  std::stringstream ss;
  int err = 0;
  DataContainer* m = getDataContainer();

  // Write the Voxel Data
  err = H5Utilities::createGroupsFromPath(H5_CELL_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {
    ss.str("");
    ss << "Error creating HDF Group " << H5_CELL_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-63);
    addErrorMessage(getNameOfClass(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  hid_t cellGroupId = H5Gopen(dcGid, H5_CELL_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {
    ss.str("");
    ss << "Error writing string attribute to HDF Group " << H5_CELL_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-64);
    addErrorMessage(getNameOfClass(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  NameListType names = m->getCellArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    IDataArray::Pointer array = m->getCellData(*iter);
    err = array->writeH5Data(cellGroupId);
    if(err < 0)
    {
      ss.str("");
      ss << "Error writing array '" << *iter << "' to the HDF5 File";
      addErrorMessage(getNameOfClass(), ss.str(), err);
      setErrorCondition(err);
      H5Gclose(cellGroupId); // Close the Cell Group
      H5Gclose(dcGid); // Close the Data Container Group
      return err;
    }
  }
  H5Gclose(cellGroupId); // Close the Cell Group
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerWriter::writeFieldData(hid_t dcGid)
{
  std::stringstream ss;
  int err = 0;
  DataContainer* m = getDataContainer();

  // Write the Field Data
  err = H5Utilities::createGroupsFromPath(H5_FIELD_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {
    std::cout << "Error creating HDF Group " << H5_FIELD_DATA_GROUP_NAME << std::endl;
    return err;
  }
  err = H5Lite::writeStringAttribute(dcGid, H5_FIELD_DATA_GROUP_NAME, H5_NAME, H5_FIELD_DATA_DEFAULT);
  if(err < 0)
  {
    return err;
  }

  hid_t fieldGroupId = H5Gopen(dcGid, H5_FIELD_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {
    ss.str("");
    ss << "Error opening field Group " << H5_FIELD_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-65);
    addErrorMessage(getNameOfClass(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  NameListType names = m->getFieldArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    IDataArray::Pointer array = m->getFieldData(*iter);
    err = array->writeH5Data(fieldGroupId);
    if(err < 0)
    {
      ss.str("");
      ss << "Error writing field array '" << *iter << "' to the HDF5 File";
      addErrorMessage(getNameOfClass(), ss.str(), err);
      setErrorCondition(err);
      H5Gclose(fieldGroupId); // Close the Cell Group
      H5Gclose(dcGid); // Close the Data Container Group
      return err;
    }
  }
  H5Gclose(fieldGroupId);
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerWriter::writeEnsembleData(hid_t dcGid)
{
  std::stringstream ss;
  int err = 0;
  DataContainer* m = getDataContainer();

  // Write the Ensemble data
  err = H5Utilities::createGroupsFromPath(H5_ENSEMBLE_DATA_GROUP_NAME, dcGid);
  if(err < 0)
  {
    ss.str("");
    ss << "Error creating HDF Group " << H5_ENSEMBLE_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-66);
    addErrorMessage(getNameOfClass(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  err = H5Lite::writeStringAttribute(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, H5_NAME, H5_ENSEMBLE_DATA_DEFAULT);

  hid_t ensembleGid = H5Gopen(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {
    ss.str("");
    ss << "Error opening ensemble Group " << H5_ENSEMBLE_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-67);
    addErrorMessage(getNameOfClass(), ss.str(), err);
    H5Gclose(dcGid); // Close the Data Container Group
    return err;
  }
  NameListType names = m->getEnsembleArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    IDataArray::Pointer array = m->getEnsembleData(*iter);
    err = array->writeH5Data(ensembleGid);
    if(err < 0)
    {
      ss.str("");
      ss << "Error writing Ensemble array '" << *iter << "' to the HDF5 File";
      addErrorMessage(getNameOfClass(), ss.str(), err);
      setErrorCondition(err);
      H5Gclose(ensembleGid); // Close the Cell Group
      H5Gclose(dcGid); // Close the Data Container Group
      return err;
    }
  }

  // These should eventually go away when these "Ensemble Data" items get wrapped into the map structure of the
  // data container class.
//  err = writeEnsembleDataArray<unsigned int, int>(ensembleGid, m->phaseType, DREAM3D::EnsembleData::PhaseType);
//  if(getErrorCondition() < 0)
//  {
//    return err;
//  }
//  err = writeEnsembleDataArray<unsigned int, int>(ensembleGid, m->crystruct, DREAM3D::EnsembleData::CrystalStructure);
//  if(getErrorCondition() < 0)
//  {
//    return err;
//  }

  H5Gclose(ensembleGid);

  return err;
}

