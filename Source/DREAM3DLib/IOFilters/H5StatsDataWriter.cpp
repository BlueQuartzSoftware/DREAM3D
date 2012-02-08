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

#include "H5StatsDataWriter.h"

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5StatsDataWriter::H5StatsDataWriter() :
AbstractFilter()
{
  setupFilterOptions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5StatsDataWriter::~H5StatsDataWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5StatsDataWriter::setupFilterOptions()
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
void H5StatsDataWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
void H5StatsDataWriter::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5StatsDataWriter::execute()
{
  int err = 0;
  setErrorCondition(err);
  DataContainer* m = getDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }

//  int64_t totalPoints = m->totalPoints();
//  int totalFields = m->getTotalFields();
#if 0
  std::string filename = getOutputFile();

  IDataArray::Pointer statsContainer = m->getEnsembleData(DREAM3D::EnsembleData::Statistics);
  if(NULL != statsContainer.get())
  {
    herr_t err = 0;
    hid_t fileId = H5Utilities::createFile(UnitTest::StatsDataTest::TestFile);
    if(fileId < 0)

    hid_t dcGid = H5Utilities::createGroup(fileId, DREAM3D::HDF5::DataContainerName);
    if(dcGid < 0)
    {
    }

    // Write the Ensemble data
    err = H5Utilities::createGroupsFromPath(H5_ENSEMBLE_DATA_GROUP_NAME, dcGid);
    if(err < 0)
    {
    }

    err = H5Lite::writeStringAttribute(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, H5_NAME, H5_ENSEMBLE_DATA_DEFAULT);
    if(err < 0)
    {
    }

    hid_t ensembleGid = H5Gopen(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, H5P_DEFAULT);
    if(ensembleGid < 0)
    {
    }

    err = statsContainer->writeH5Data(ensembleGid);
    if(err < 0)
    {

    }

    err = H5Utilities::closeHDF5Object(ensembleGid);
    err = H5Utilities::closeHDF5Object(dcGid);
    err = H5Utilities::closeFile(fileId);
  }
#endif
  // If there is an error set this to something negative and also set a message
  notify("H5StatsDataWriter Completed", 0, Observable::UpdateProgressMessage);

}
