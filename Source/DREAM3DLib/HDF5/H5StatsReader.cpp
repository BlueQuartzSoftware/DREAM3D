/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "H5StatsReader.h"


#define CHECK_STATS_READ_ERROR(err, name)\
if (err < 0) {\
  std::cout << "H5StatsReader Error: Could not read the " << name << " Group Data" << std::endl;\
    std::cout << "  File: " << __FILE__ << std::endl;\
    std::cout << "  Line: " << __LINE__ << std::endl;\
return err; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5StatsReader::H5StatsReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5StatsReader::~H5StatsReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5StatsReader::Pointer H5StatsReader::New(const std::string &filename)
{
  Pointer sharedPtr(new H5StatsReader);
  sharedPtr->setFileName(filename);
  hid_t fileId = H5Utilities::openFile(filename, true);
  if (fileId < 0)
  {
    sharedPtr = NullPointer();
  }
  else
  {
    H5Utilities::closeFile(fileId);
  }
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string H5StatsReader::getDistributionType(int phase,
                                                    const std::string &group,
                                                    DREAM3D::Reconstruction::DistributionType &dt)
{
  herr_t err = 0;

  std::string index = StringUtils::numToString(phase);

  std::string path = DREAM3D::HDF5::Statistics + "/" + index + "/" + group;
  hid_t fileId = H5Utilities::openFile(m_FileName, false);\

  if (fileId < 0)
  {
    return std::string();
  }
  std::string data;
  err = H5Lite::readStringAttribute(fileId, path, DREAM3D::HDF5::DistributionType, data);

  err = H5Utilities::closeFile(fileId);
  if (err < 0)
  {
    data.clear();
  }

  if (data.compare(DREAM3D::HDF5::BetaDistribution) == 0)
  {
    dt = DREAM3D::Reconstruction::Beta;
  }
  else if (data.compare(DREAM3D::HDF5::LogNormalDistribution) == 0)
  {
    dt = DREAM3D::Reconstruction::LogNormal;
  }
  else if (data.compare(DREAM3D::HDF5::PowerLawDistribution) == 0)
  {
    dt = DREAM3D::Reconstruction::Power;
  }
  else
  {
    dt = DREAM3D::Reconstruction::UnknownDistributionType;
  }
  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5StatsReader::getPhaseAndCrystalStructures(std::vector<int> &phases,
                                  std::vector<Ebsd::CrystalStructure> &xtals)
{
  phases.clear();
  xtals.clear();
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(fileId, m_FileName)
  OPEN_RECONSTRUCTION_GROUP(reconGid, DREAM3D::HDF5::Statistics.c_str(), fileId)

  // Get a list of all the groups
  std::list<std::string> names;
  err = H5Utilities::getGroupObjects(reconGid, H5Utilities::H5Support_GROUP, names);
  if (err < 0)
  {
    retErr = err;
  }
  // Convert the string names to integers
  int i;
  for (std::list<std::string>::iterator pString = names.begin(); pString != names.end(); ++pString)
  {
    StringUtils::stringToNum(i, *pString);
    phases.push_back(i);
    hid_t gid = H5Gopen(reconGid,(*pString).c_str(), H5P_DEFAULT);
    unsigned int xtal = static_cast<unsigned int>(Ebsd::UnknownCrystalStructure);
    err = H5Lite::readScalarDataset(gid, DREAM3D::HDF5::CrystalStructure, xtal);
    if (err < 0)
    {
      xtal = static_cast<unsigned int>(Ebsd::UnknownCrystalStructure);
    }
    xtals.push_back(static_cast<Ebsd::CrystalStructure>(xtal) );
    err = H5Gclose(gid);
  }

  err = H5Gclose(reconGid);
  if (err < 0)
  {
    retErr = err;
  }
  err = H5Utilities::closeFile(fileId);
  if (err < 0)
  {
    retErr = err;
  }
  return retErr;
}
