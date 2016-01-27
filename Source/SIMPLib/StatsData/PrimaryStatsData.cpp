/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "PrimaryStatsData.h"

#include <QtCore/QString>
#include <vector>

#include "H5Support/H5Utilities.h"

#include "SIMPLib/HDF5/H5PrimaryStatsDataDelegate.h"


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
QString PrimaryStatsData::getStatsType()
{
  return DREAM3D::StringConstants::PrimaryStatsData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int PrimaryStatsData::getPhaseType()
{
  return DREAM3D::PhaseType::PrimaryPhase;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryStatsData::initialize()
{
  m_FeatureSize_DistType = DREAM3D::DistributionType::LogNormal;
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
  float featureDiameterInfo[3];
  getFeatureDiameterInfo(featureDiameterInfo);
  QVector<float> bins;
  float d = featureDiameterInfo[2];
  while (d <= featureDiameterInfo[1])
  {
    //  qDebug() << d << "\n";
    bins.push_back(d);
    d = d + featureDiameterInfo[0];
  }
  // Copy this into the DataArray<float>
  m_BinNumbers = FloatArrayType::CreateArray(bins.size(), DREAM3D::StringConstants::BinNumber );
  ::memcpy(m_BinNumbers->getVoidPointer(0), &(bins.front()), bins.size() * sizeof(float));
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
