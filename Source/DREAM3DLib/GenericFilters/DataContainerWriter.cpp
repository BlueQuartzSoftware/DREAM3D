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

#include "EbsdLib/EbsdConstants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerWriter::DataContainerWriter() :
AbstractFilter()
{
  setupFilterOptions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerWriter::~DataContainerWriter()
{
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
void DataContainerWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;

  if (m_OutputFile.empty() == true)
  {
    ss << getNameOfClass() << ": The output file must be set before executing this filter.";
    setErrorCondition(-1);
  }

  setErrorMessage(ss.str());
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
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }
  setErrorCondition(0);
  dataCheck(false, 1, 1, 1);

  typedef std::list<std::string> NameListType;
  std::stringstream ss;
  int err = 0;
  hid_t dcGid = -1;

  H5DataWriter::Pointer writer = H5DataWriter::New();
  writer->setFileName(m_OutputFile);

  int64_t volDims[3] =
  { m->getXPoints(), m->getYPoints(), m->getZPoints() };
  float spacing[3] =
  { m->getXRes(), m->getYRes(), m->getZRes() };
  float origin[3] =
  { 0.0f, 0.0f, 0.0f };

  err = writer->openFile(false); // Do NOT append to any existing file
  if (err < 0)
  {
    ss.str("");
    ss << getNameOfClass() << ": Error creating output file;";
    setErrorCondition(-59);
    setErrorMessage(ss.str());
    return;
  }

  // Create the HDF5 Group for the Data Container
  err = H5Utilities::createGroupsFromPath(DREAM3D::HDF5::DataContainerName.c_str(), writer->getFileId());
  if (err < 0)
  {
    ss.str("");
    ss << "Error creating HDF Group " << DREAM3D::HDF5::DataContainerName << std::endl;
    setErrorCondition(-60);
    setErrorMessage(ss.str());
    return;
  }
  dcGid = H5Gopen(writer->getFileId(), DREAM3D::HDF5::DataContainerName.c_str(), H5P_DEFAULT );
  if (dcGid < 0)
  {
    ss.str("");
    ss << "Error opening Group " << DREAM3D::HDF5::DataContainerName << std::endl;
    setErrorCondition(-61);
    setErrorMessage(ss.str());
    return;
  }

  // This just writes the header information
  err = writer->writeStructuredPoints(DREAM3D::HDF5::DataContainerName, volDims, spacing, origin);
  if (err < 0)
  {
    ss.str("");
    ss << getNameOfClass() << ":Error Writing header information to output file" << std::endl;
    setErrorCondition(-62);
    setErrorMessage(ss.str());
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }



  // Write the Voxel Data
  err = H5Utilities::createGroupsFromPath(H5_CELL_DATA_GROUP_NAME, dcGid);
  if (err < 0)
  {
    ss.str("");
    ss << "Error creating HDF Group " << H5_CELL_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-63);
    setErrorMessage(ss.str());
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }
  hid_t cellGroupId = H5Gopen(dcGid, H5_CELL_DATA_GROUP_NAME, H5P_DEFAULT );
  if(err < 0)
  {
    ss.str("");
    ss << "Error writing string attribute to HDF Group " << H5_CELL_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-64);
    setErrorMessage(ss.str());
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }
  NameListType names = m->getCellArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter )
  {
    IDataArray::Pointer array = m->getCellData(*iter);
    err = array->writeH5Data(cellGroupId);
    if (err < 0)
    {
      ss.str("");
      ss << "Error writing array '" << *iter << "' to the HDF5 File";
      setErrorMessage(ss.str());
      setErrorCondition(err);
      H5Gclose(cellGroupId); // Close the Cell Group
      H5Gclose(dcGid); // Close the Data Container Group
      return;
    }
  }
  H5Gclose(cellGroupId); // Close the Cell Group


  // Write the Field Data
  err = H5Utilities::createGroupsFromPath(H5_FIELD_DATA_GROUP_NAME, dcGid);
  if (err < 0)
  {
    std::cout << "Error creating HDF Group " << H5_FIELD_DATA_GROUP_NAME << std::endl;
    return ;
  }
  err = H5Lite::writeStringAttribute(dcGid, H5_FIELD_DATA_GROUP_NAME, H5_NAME, H5_FIELD_DATA_DEFAULT);
  if (err < 0) {
    //FIXME: Check for errors
  }

  hid_t fieldGroupId = H5Gopen(dcGid, H5_FIELD_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {
    ss.str("");
    ss << "Error opening field Group " << H5_FIELD_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-65);
    setErrorMessage(ss.str());
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }
  names = m->getFieldArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter )
  {
    IDataArray::Pointer array = m->getFieldData(*iter);
    err = array->writeH5Data(fieldGroupId);
    if (err < 0)
    {
      ss.str("");
      ss << "Error writing field array '" << *iter << "' to the HDF5 File";
      setErrorMessage(ss.str());
      setErrorCondition(err);
      H5Gclose(fieldGroupId); // Close the Cell Group
      H5Gclose(dcGid); // Close the Data Container Group
      return;
    }
  }
  H5Gclose(fieldGroupId);

  // Write the Ensemble data
  err = H5Utilities::createGroupsFromPath(H5_ENSEMBLE_DATA_GROUP_NAME, dcGid);
  if (err < 0)
  {
    ss.str("");
    ss <<  "Error creating HDF Group " << H5_ENSEMBLE_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-66);
    setErrorMessage(ss.str());
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }
  err = H5Lite::writeStringAttribute(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, H5_NAME, H5_ENSEMBLE_DATA_DEFAULT);


  hid_t ensembleGid = H5Gopen(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, H5P_DEFAULT);
  if(err < 0)
  {
    ss.str("");
    ss << "Error opening ensemble Group " << H5_ENSEMBLE_DATA_GROUP_NAME << std::endl;
    setErrorCondition(-67);
    setErrorMessage(ss.str());
    H5Gclose(dcGid); // Close the Data Container Group
    return;
  }
  names = m->getEnsembleArrayNameList();
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter )
  {
    IDataArray::Pointer array = m->getEnsembleData(*iter);
    err = array->writeH5Data(ensembleGid);
    if (err < 0)
    {
      ss.str("");
      ss << "Error writing Ensemble array '" << *iter << "' to the HDF5 File";
      setErrorMessage(ss.str());
      setErrorCondition(err);
      H5Gclose(ensembleGid); // Close the Cell Group
      H5Gclose(dcGid); // Close the Data Container Group
      return;
    }
  }


  // These should eventually go away when these "Ensemble Data" items get wrapped into the map structure of the
  // data container class.
  err = writeEnsembleDataArray<DREAM3D::Reconstruction::PhaseType, int>(writer, m->phaseType, DREAM3D::EnsembleData::PhaseType);
  err = H5Lite::writeStringAttribute(ensembleGid, DREAM3D::EnsembleData::PhaseType, DREAM3D::HDF5::ObjectType, "vector");
  if(err < 0)
  {
    //FIXME: Add Error Handling Code
  }
  err = writeEnsembleDataArray<Ebsd::CrystalStructure, int>(writer, m->crystruct, DREAM3D::EnsembleData::CrystalStructure);
  err = H5Lite::writeStringAttribute(ensembleGid, DREAM3D::EnsembleData::CrystalStructure, DREAM3D::HDF5::ObjectType, "vector");
  if(err < 0)
  {
    //FIXME: Add Error Handling Code
  }

  H5Gclose(ensembleGid);


  H5Gclose(dcGid); // Close the Data Container Group
  writer->closeFile();
  writer = H5DataWriter::NullPointer();
  setErrorCondition(0);
  notify("DataContainerWriter Complete", 0, Observable::UpdateProgressMessage);
}

