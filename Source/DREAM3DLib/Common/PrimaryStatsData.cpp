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
#include "PrimaryStatsData.h"

#include <string>
#include <vector>

#include "H5Support/H5Utilities.h"

#include "DREAM3DLib/HDF5/H5PrimaryStatsDataDelegate.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrimaryStatsData::PrimaryStatsData()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrimaryStatsData::~PrimaryStatsData()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryStatsData::initialize()
{
  m_GrainSize_DistType = DREAM3D::DistributionType::LogNormal;
  m_BOverA_DistType = DREAM3D::DistributionType::Beta;
  m_COverA_DistType = DREAM3D::DistributionType::Beta;
  m_Neighbors_DistType = DREAM3D::DistributionType::LogNormal;
  m_Omegas_DistType = DREAM3D::DistributionType::Beta;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer PrimaryStatsData::generateBinNumbers()
{
  float grainDiameterInfo[3];
  getGrainDiameterInfo(grainDiameterInfo);
  std::vector<float> bins;
  float d = grainDiameterInfo[2];
  while (d <= grainDiameterInfo[1])
  {
  //  std::cout << d << std::endl;
    bins.push_back(d);
    d = d + grainDiameterInfo[0];
  }
  // Copy this into the DataArray<float>
  m_BinNumbers = FloatArrayType::CreateArray(bins.size(),DREAM3D::HDF5::BinNumber );
  ::memcpy(m_BinNumbers->GetVoidPointer(0), &(bins.front()), bins.size() * sizeof(float));
  return m_BinNumbers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PrimaryStatsData::writeHDF5Data(hid_t groupId)
{
  int err = 0;
  H5PrimaryStatsDataDelegate::Pointer writer = H5PrimaryStatsDataDelegate::New();
  err = writer->writePrimaryStatsData(this, groupId);
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PrimaryStatsData::readHDF5Data(hid_t groupId)
{
  int err = 0;
  H5PrimaryStatsDataDelegate::Pointer reader = H5PrimaryStatsDataDelegate::New();
  err = reader->readPrimaryStatsData(this, groupId);
  return err;
}
