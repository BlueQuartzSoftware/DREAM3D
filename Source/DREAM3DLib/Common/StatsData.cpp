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
#include "StatsData.h"

#include <string>
#include <vector>

#include "H5Support/H5Utilities.h"

#include "DREAM3DLib/HDF5/H5StatsDataDelegate.h"


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
VectorOfFloatArray StatsData::CreateCorrelatedDistributionArrays(uint32_t distributionType, size_t numBins)
{
  VectorOfFloatArray v;
  if(distributionType == DREAM3D::DistributionType::Beta)
  {
    v.resize(DREAM3D::HDF5::BetaColumnCount);
    v[0] = FloatArrayType::CreateArray(numBins, DREAM3D::HDF5::Alpha);
    v[0]->initializeWithZeros();
    v[1] = FloatArrayType::CreateArray(numBins, DREAM3D::HDF5::Beta);
    v[1]->initializeWithZeros();
  }
  else if(distributionType == DREAM3D::DistributionType::LogNormal)
  {
	v.resize(DREAM3D::HDF5::LogNormalColumnCount);
    v[0] = FloatArrayType::CreateArray(numBins, DREAM3D::HDF5::Average);
    v[0]->initializeWithZeros();
    v[1] = FloatArrayType::CreateArray(numBins, DREAM3D::HDF5::StandardDeviation);
    v[1]->initializeWithZeros();
  }
  else if(distributionType == DREAM3D::DistributionType::Power)
  {
    v.resize(DREAM3D::HDF5::LogNormalColumnCount);
    v[0] = FloatArrayType::CreateArray(numBins, DREAM3D::HDF5::Alpha);
    v[0]->initializeWithZeros();
    v[1] = FloatArrayType::CreateArray(numBins, DREAM3D::HDF5::Exp_k);
    v[1]->initializeWithZeros();
    v[2] = FloatArrayType::CreateArray(numBins, DREAM3D::HDF5::Beta);
    v[2]->initializeWithZeros();
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
	v = FloatArrayType::CreateArray(DREAM3D::HDF5::BetaColumnCount, DREAM3D::HDF5::Grain_Size_Distribution);
    v->initializeWithZeros();
  }
  else if(distributionType == DREAM3D::DistributionType::LogNormal)
  {
	v = FloatArrayType::CreateArray(DREAM3D::HDF5::LogNormalColumnCount, DREAM3D::HDF5::Grain_Size_Distribution);
    v->initializeWithZeros();
  }
  else if(distributionType == DREAM3D::DistributionType::Power)
  {
	v = FloatArrayType::CreateArray(DREAM3D::HDF5::PowerLawColumnCount, DREAM3D::HDF5::Grain_Size_Distribution);
    v->initializeWithZeros();
  }
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsData::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsData::writeHDF5Data(hid_t groupId)
{
  int err = 0;
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsData::readHDF5Data(hid_t groupId)
{
  int err = 0;
  return err;
}
