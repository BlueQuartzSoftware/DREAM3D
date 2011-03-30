/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "H5ReconStatsReader.h"

#define CHECK_STATS_READ_ERROR(err, name)\
if (err < 0) {\
  std::cout << "H5ReconStatsReader Error: Could not read the " << name << " Group Data" << std::endl;\
    std::cout << "  File: " << __FILE__ << std::endl;\
    std::cout << "  Line: " << __LINE__ << std::endl;\
return err; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5ReconStatsReader::H5ReconStatsReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5ReconStatsReader::~H5ReconStatsReader()
{
}

H5ReconStatsReader::Pointer H5ReconStatsReader::New(const std::string &filename)
{
  Pointer sharedPtr(new H5ReconStatsReader);
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

std::string H5ReconStatsReader::getDistributionType(int phase, const std::string &group,
                                                    AIM::Reconstruction::DistributionType &dt)
{
  herr_t err = 0;

  std::string index = StringUtils::numToString(phase);

  std::string path = AIM::HDF5::Reconstruction + "/" + index + "/" + group;
  hid_t fileId = H5Utilities::openFile(m_FileName, false);\

  if (fileId < 0)
  {
    return std::string();
  }
  std::string data;
  err = H5Lite::readStringAttribute(fileId, path, AIM::HDF5::DistributionType, data);

  err = H5Utilities::closeFile(fileId);
  if (err < 0)
  {
    data.clear();
  }

  if (data.compare(AIM::HDF5::BetaDistribution) == 0)
  {
    dt = AIM::Reconstruction::Beta;
  }
  else if (data.compare(AIM::HDF5::LogNormalDistribution) == 0)
  {
    dt = AIM::Reconstruction::LogNormal;
  }
  else if (data.compare(AIM::HDF5::PowerLawDistribution) == 0)
  {
    dt = AIM::Reconstruction::Power;
  }
  else
  {
    dt = AIM::Reconstruction::UnknownDistributionType;
  }
  return data;
}
