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

#include <hdf5.h>
#include "MXA/Utilities/MXAFileInfo.h"
#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"
#include "DREAM3DLib/HDF5/VTKH5Constants.h"
#include "DREAM3DLib/HDF5/H5DataArrayReader.h"
#include "DREAM3DLib/Common/StatsDataArray.h"

typedef std::list<std::string> NameListType;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReader::DataContainerReader() :
AbstractFilter(),
m_InputFile(""),
m_ReadCellData(true),
m_ReadFieldData(true),
m_ReadEnsembleData(true)
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
    option->setHumanLabel("Read Cell Data");
    option->setPropertyName("ReadCellData");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Read Field Data");
    option->setPropertyName("ReadFieldData");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Read Ensemble Data");
    option->setPropertyName("ReadEnsembleData");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("ReadCellData", getReadCellData() );
  writer->writeValue("ReadFieldData", getReadFieldData() );
  writer->writeValue("ReadEnsembleData", getReadEnsembleData() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;

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
    gatherData(preflight);
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
int DataContainerReader::getSizeResolutionOrigin(int64_t volDims[3], float spacing[3], float origin[3])
{
  int err = 0;

  if(m_InputFile.empty() == true)
  {
    addErrorMessage(getHumanLabel(), "DataContainerReader Error; Filename was empty", -1);
    return -1;
  }
  std::stringstream ss;

  hid_t fileId = H5Utilities::openFile(m_InputFile, false);
  if(fileId < 0)
  {
    ss.str("");
    ss <<": Error opening input file '" << m_InputFile << "'";
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    return -1;
  }
  hid_t dcGid = H5Gopen(fileId, DREAM3D::HDF5::DataContainerName.c_str(), 0);
  if(dcGid < 0)
  {
    err = H5Utilities::closeFile(fileId);
    ss.str("");
    ss <<": Error opening group '" << DREAM3D::HDF5::DataContainerName << "'";
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss.str(), -150);
    return -1;
  }

  err |= gatherMetaData(dcGid, volDims, spacing, origin);

  err = H5Gclose(dcGid);
  err = H5Fclose(fileId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReader::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss <<" DataContainer was NULL";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    return;
  }
  setErrorCondition(0);
  //dataCheck(false, 1, 1, 1);
  int err = 0;
  std::stringstream ss;

  // Clear out everything from the data container before we start.
  int64_t volDims[3] =
  { 0, 0, 0 };
  float spacing[3] =
  { 1.0f, 1.0f, 1.0f };
  float origin[3] =
  { 0.0f, 0.0f, 0.0f };
  m->setDimensions(volDims[0], volDims[1], volDims[2]);
  m->setResolution(spacing);
  m->setOrigin(origin);

  if(m_ReadCellData == true) m->clearCellData();
  if(m_ReadFieldData == true) m->clearFieldData();
  if(m_ReadEnsembleData == true) m->clearEnsembleData();

  // We are actually wanting to read the file so set preflight to false
  err = gatherData(false);

  setErrorCondition(err);
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerReader::gatherMetaData(hid_t dcGid, int64_t volDims[3], float spacing[3], float origin[3])
{
  int err = H5Lite::readPointerDataset(dcGid, H5_DIMENSIONS, volDims);
   if(err < 0)
   {
     PipelineMessage em (getHumanLabel(), "DataContainerReader Error Reading the Dimensions", err);
  addErrorMessage(em);
     setErrorCondition(-151);
     return -1;
   }

   err = H5Lite::readPointerDataset(dcGid, H5_SPACING, spacing);
   if(err < 0)
   {
     PipelineMessage em (getHumanLabel(), "DataContainerReader Error Reading the Spacing (Resolution)", err);
  addErrorMessage(em);
     setErrorCondition(-152);
     return -1;
   }

   err = H5Lite::readPointerDataset(dcGid, H5_ORIGIN, origin);
   if(err < 0)
   {
     PipelineMessage em (getHumanLabel(), "DataContainerReader Error Reading the Origin", err);
  addErrorMessage(em);
     setErrorCondition(-153);
     return -1;
   }
   return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerReader::gatherData(bool preflight)
{
  int err = 0;
  std::stringstream ss;
  int64_t volDims[3] =
  { 0, 0, 0 };
  float spacing[3] =
  { 1.0f, 1.0f, 1.0f };
  float origin[3] =
  { 0.0f, 0.0f, 0.0f };
  VoxelDataContainer* m = getVoxelDataContainer();

  hid_t fileId = H5Utilities::openFile(m_InputFile, true); // Open the file Read Only
  if(fileId < 0)
  {
    ss.str("");
    ss << ": Error opening input file '" << m_InputFile << "'";
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    return -1;
  }
  hid_t dcGid = H5Gopen(fileId, DREAM3D::HDF5::DataContainerName.c_str(), 0);
  if(dcGid < 0)
  {
    err = H5Utilities::closeFile(fileId);
    ss.str("");
    ss << ": Error opening group '" << DREAM3D::HDF5::DataContainerName << "'";
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    return -1;
  }

  if(false == preflight)
  {
    err = gatherMetaData(dcGid, volDims, spacing, origin);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      err |= H5Fclose(fileId);
      setErrorCondition(err);
      return -1;
    }
    m->setDimensions(volDims[0], volDims[1], volDims[2]); // We use this signature so the compiler will cast the value to the proper int type
    m->setResolution(spacing);
    m->setOrigin(origin);
  }

  if(m_ReadCellData == true)
  {
    err |= readGroupsData(dcGid, H5_CELL_DATA_GROUP_NAME, preflight);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      err |= H5Fclose(fileId);
      setErrorCondition(err);
      return -1;
    }
  }

  if(m_ReadFieldData == true)
  {
    err |= readGroupsData(dcGid, H5_FIELD_DATA_GROUP_NAME, preflight);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      err |= H5Fclose(fileId);
      setErrorCondition(err);
      return -1;
    }
  }

  if(m_ReadEnsembleData == true)
  {
    err |= readGroupsData(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, preflight);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      err |= H5Fclose(fileId);
      setErrorCondition(err);
      return -1;
    }
  }

  err |= H5Gclose(dcGid);
  err |= H5Fclose(fileId);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerReader::readGroupsData(hid_t dcGid, const std::string &groupName, bool preflight)
{
  std::stringstream ss;
  int err = 0;
  //Read the Cell Data
  hid_t gid = H5Gopen(dcGid, groupName.c_str(), H5P_DEFAULT);
  if(err < 0)
  {
    ss.str("");
    ss << "Error opening HDF5 Group " << groupName << std::endl;
    setErrorCondition(-154);
    addErrorMessage(getHumanLabel(), ss.str(), err);
    return -154;
  }

  NameListType names;
  H5Utilities::getGroupObjects(gid, H5Utilities::H5Support_DATASET | H5Utilities::H5Support_ANY, names);
//  std::cout << "Number of Items in " << groupName << " Group: " << names.size() << std::endl;
  std::string classType;
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    classType.clear();
    H5Lite::readStringAttribute(gid, *iter, DREAM3D::HDF5::ObjectType, classType);
 //   std::cout << groupName << " Array: " << *iter << " with C++ ClassType of " << classType << std::endl;
    IDataArray::Pointer dPtr = IDataArray::NullPointer();

    if(classType.find("DataArray") == 0)
    {
      dPtr = H5DataArrayReader::readIDataArray(gid, *iter, preflight);
    }
    else if(classType.compare("vector") == 0)
    {

    }
    else if(classType.compare("NeighborList<T>") == 0)
    {
      dPtr = H5DataArrayReader::readNeighborListData(gid, *iter, preflight);
    }
    else if ( (*iter).compare(DREAM3D::EnsembleData::Statistics) == 0)
    {
      StatsDataArray::Pointer statsData = StatsDataArray::New();
      statsData->SetName(DREAM3D::EnsembleData::Statistics);
      statsData->readH5Data(gid);
      dPtr = statsData;
    }

    if (NULL != dPtr.get())
    {
      if(groupName.compare(H5_CELL_DATA_GROUP_NAME) == 0)
      {
        getVoxelDataContainer()->addCellData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_FIELD_DATA_GROUP_NAME) == 0)
      {
        getVoxelDataContainer()->addFieldData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_ENSEMBLE_DATA_GROUP_NAME) == 0)
      {
        getVoxelDataContainer()->addEnsembleData(dPtr->GetName(), dPtr);
      }
    }

  }
  H5Gclose(gid); // Close the Cell Group
  return err;
}
