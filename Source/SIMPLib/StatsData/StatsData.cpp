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

#include "StatsData.h"

#include <QtCore/QString>
#include <vector>

#include "H5Support/H5Utilities.h"

#include "SIMPLib/HDF5/H5StatsDataDelegate.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsData::StatsData()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsData::~StatsData()
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsData::getStatsType()
{
  return DREAM3D::StringConstants::StatsData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int StatsData::getPhaseType()
{
  return DREAM3D::PhaseType::UnknownPhaseType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VectorOfFloatArray StatsData::CreateCorrelatedDistributionArrays(uint32_t distributionType, size_t numBins)
{
  VectorOfFloatArray v;
  if(distributionType == DREAM3D::DistributionType::Beta)
  {
    v.resize(DREAM3D::DistributionType::BetaColumnCount);
    v[0] = FloatArrayType::CreateArray(numBins, DREAM3D::StringConstants::Alpha);
    v[0]->initializeWithZeros();
    v[1] = FloatArrayType::CreateArray(numBins, DREAM3D::StringConstants::Beta);
    v[1]->initializeWithZeros();
  }
  else if(distributionType == DREAM3D::DistributionType::LogNormal)
  {
    v.resize(DREAM3D::DistributionType::LogNormalColumnCount);
    v[0] = FloatArrayType::CreateArray(numBins, DREAM3D::StringConstants::Average);
    v[0]->initializeWithZeros();
    v[1] = FloatArrayType::CreateArray(numBins, DREAM3D::StringConstants::StandardDeviation);
    v[1]->initializeWithZeros();
  }
  else if(distributionType == DREAM3D::DistributionType::Power)
  {
    v.resize(DREAM3D::DistributionType::LogNormalColumnCount);
    v[0] = FloatArrayType::CreateArray(numBins, DREAM3D::StringConstants::Alpha);
    v[0]->initializeWithZeros();
    v[1] = FloatArrayType::CreateArray(numBins, DREAM3D::StringConstants::MinimumValue);
    v[1]->initializeWithZeros();
  }
  else if(distributionType == DREAM3D::DistributionType::UnknownDistributionType)
  {
    v.resize(DREAM3D::DistributionType::LogNormalColumnCount);
    v[0] = FloatArrayType::CreateArray(numBins, DREAM3D::StringConstants::Alpha);
    v[0]->initializeWithZeros();
    v[1] = FloatArrayType::CreateArray(numBins, DREAM3D::StringConstants::Beta);
    v[1]->initializeWithZeros();
  }
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer StatsData::CreateDistributionArrays(uint32_t distributionType)
{
  FloatArrayType::Pointer v;
  if(distributionType == DREAM3D::DistributionType::Beta)
  {
    v = FloatArrayType::CreateArray(DREAM3D::DistributionType::BetaColumnCount, DREAM3D::StringConstants::Feature_Size_Distribution);
    v->initializeWithZeros();
  }
  else if(distributionType == DREAM3D::DistributionType::LogNormal)
  {
    v = FloatArrayType::CreateArray(DREAM3D::DistributionType::LogNormalColumnCount, DREAM3D::StringConstants::Feature_Size_Distribution);
    v->initializeWithZeros();
  }
  else if(distributionType == DREAM3D::DistributionType::Power)
  {
    v = FloatArrayType::CreateArray(DREAM3D::DistributionType::PowerLawColumnCount, DREAM3D::StringConstants::Feature_Size_Distribution);
    v->initializeWithZeros();
  }
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsData::initialize()
{
  //These should be implemented in the subclasses.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsData::writeHDF5Data(hid_t groupId)
{
  int err = 0;
  //These should be implemented in the subclasses.
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsData::readHDF5Data(hid_t groupId)
{
  int err = 0;
  //These should be implemented in the subclasses.
  return err;
}
