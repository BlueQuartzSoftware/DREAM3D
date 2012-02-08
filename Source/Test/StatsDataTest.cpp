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

#include <stdlib.h>

#include <iostream>
#include <limits>
#include <string>

#include "MXA/Utilities/MXADir.h"

#include "H5Support/H5Utilities.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/StatsGen.h"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/Common/StatsDataContainer.h"
#include "DREAM3DLib/Common/StatsData.h"


#include "UnitTestSupport.hpp"

#include "TestFileLocations.h"

StatsDataContainer::Pointer g_StatsContainer;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void initializeOmega3( StatsData::Pointer statsData, int count, uint32_t distType = DREAM3D::DistributionType::Beta)
{
  float alpha, beta;
  DREAM3D_RANDOMNG_NEW()
  VectorOfFloatArray data;
  FloatArrayType::Pointer alphas = FloatArrayType::CreateArray(count);
  alphas->SetName(DREAM3D::HDF5::Alpha);
  FloatArrayType::Pointer betas = FloatArrayType::CreateArray(count);
  betas->SetName(DREAM3D::HDF5::Beta);

  for (qint32 i = 0; i < count; ++i)
   {
    alpha = (0*i) + 10.0 + rg.genrand_res53();
    beta = (0*i) + 1.5 + (0.5*rg.genrand_res53());
    alphas->SetValue(i, alpha);
    betas->SetValue(i, beta);
   }
  data.push_back(alphas);
  data.push_back(betas);
  statsData->setGrainSize_Omegas(data);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void initializeBOverA( StatsData::Pointer statsData, int count, uint32_t distType = DREAM3D::DistributionType::Beta)
{
  float alpha, beta;
  DREAM3D_RANDOMNG_NEW()
  VectorOfFloatArray data;
  FloatArrayType::Pointer alphas = FloatArrayType::CreateArray(count);
  alphas->SetName(DREAM3D::HDF5::Alpha);
  FloatArrayType::Pointer betas = FloatArrayType::CreateArray(count);
  betas->SetName(DREAM3D::HDF5::Beta);

  for (qint32 i = 0; i < count; ++i)
   {
    alpha = (0*i) + 15.0 + rg.genrand_res53();
    beta = (0*i) + 1.25 + (0.5*rg.genrand_res53());
    alphas->SetValue(i, alpha);
    betas->SetValue(i, beta);
   }
  data.push_back(alphas);
  data.push_back(betas);
  statsData->setGrainSize_BOverA(data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void initializeCOverA( StatsData::Pointer statsData, int count, uint32_t distType = DREAM3D::DistributionType::Beta)
{
  float alpha, beta;
  DREAM3D_RANDOMNG_NEW()
  VectorOfFloatArray data;
  FloatArrayType::Pointer alphas = FloatArrayType::CreateArray(count);
  alphas->SetName(DREAM3D::HDF5::Alpha);
  FloatArrayType::Pointer betas = FloatArrayType::CreateArray(count);
  betas->SetName(DREAM3D::HDF5::Beta);

  for (qint32 i = 0; i < count; ++i)
   {
    alpha = (0*i) + 14.0 + rg.genrand_res53();
    beta = (0*i) + 1.15 + (0.5*rg.genrand_res53());
    alphas->SetValue(i, alpha);
    betas->SetValue(i, beta);
   }
  data.push_back(alphas);
  data.push_back(betas);
  statsData->setGrainSize_COverA(data);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void initializeCOverB( StatsData::Pointer statsData, int count, uint32_t distType = DREAM3D::DistributionType::Beta)
{
  float alpha, beta;
  DREAM3D_RANDOMNG_NEW()
  VectorOfFloatArray data;
  FloatArrayType::Pointer alphas = FloatArrayType::CreateArray(count);
  alphas->SetName(DREAM3D::HDF5::Alpha);
  FloatArrayType::Pointer betas = FloatArrayType::CreateArray(count);
  betas->SetName(DREAM3D::HDF5::Beta);

  for (qint32 i = 0; i < count; ++i)
   {
    alpha = (0*i) + 13.0 + rg.genrand_res53();
    beta = (0*i) + 1.05 + (0.5*rg.genrand_res53());
    alphas->SetValue(i, alpha);
    betas->SetValue(i, beta);
   }
  data.push_back(alphas);
  data.push_back(betas);
  statsData->setGrainSize_COverB(data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void initializeNeighbors( StatsData::Pointer statsData, std::vector<float> binNumbers, uint32_t distType = DREAM3D::DistributionType::Power)
{
  float alpha, k, beta;
  int32_t count = binNumbers.size();

  DREAM3D_RANDOMNG_NEW()
  VectorOfFloatArray data;
  FloatArrayType::Pointer alphas = FloatArrayType::CreateArray(count);
  alphas->SetName(DREAM3D::HDF5::Alpha);
  FloatArrayType::Pointer betas = FloatArrayType::CreateArray(count);
  betas->SetName(DREAM3D::HDF5::Beta);
  FloatArrayType::Pointer ks = FloatArrayType::CreateArray(count);
  ks->SetName(DREAM3D::HDF5::Exp_k);


  int middlebin = count/2;

  for (qint32 i = 0; i < count; ++i)
   {
    alpha = (4*(binNumbers[i]/binNumbers[middlebin])) + rg.genrand_res53();
    k = 2 + (0.2*(binNumbers[i]/binNumbers[middlebin])) + (0.05*rg.genrand_res53());
    beta = (0*i) + 1;
    alphas->SetValue(i, alpha);
    ks->SetValue(i, k);
    betas->SetValue(i, beta);
   }
  data.push_back(alphas);
  data.push_back(ks);
  data.push_back(betas);
  statsData->setGrainSize_Neighbors(data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void initializeODF_MDF( StatsData::Pointer statsData)
{
  float totalWeight = 0.0;

  std::vector<float> e1s;
  std::vector<float> e2s;
  std::vector<float> e3s;
  std::vector<float> weights;
  std::vector<float> sigmas;
  std::vector<float> odf;
  std::vector<float> mdf;
  std::vector<float> angles;
  std::vector<float> axes;

  Texture::calculateCubicODFData(e1s, e2s, e3s, weights, sigmas, true, odf, totalWeight);

  // Stupid, but copy the data from the vector to the DataArray<float> instance
  FloatArrayType::Pointer odfData = FloatArrayType::CreateArray(odf.size() );
  odfData->SetName(DREAM3D::HDF5::ODF);
  for(size_t i = 0; i < odf.size(); ++i)
  {
    odfData->SetValue(i, odf[i]);
  }

  // Push it onto the StatsData instance
  statsData->setODF(odfData);

  {
    FloatArrayType::Pointer euler1 = FloatArrayType::CreateArray(1);
    euler1->SetName(DREAM3D::HDF5::Euler1);
    euler1->SetValue(0, 10.0f);
    FloatArrayType::Pointer euler2 = FloatArrayType::CreateArray(1);
    euler2->SetName(DREAM3D::HDF5::Euler2);
    euler2->SetValue(0, 10.0f);
    FloatArrayType::Pointer euler3 = FloatArrayType::CreateArray(1);
    euler3->SetName(DREAM3D::HDF5::Euler3);
    euler3->SetValue(0, 10.0f);
    FloatArrayType::Pointer sigma = FloatArrayType::CreateArray(1);
    sigma->SetName(DREAM3D::HDF5::Sigma);
    sigma->SetValue(0, 0.99f);
    FloatArrayType::Pointer weight = FloatArrayType::CreateArray(1);
    weight->SetName(DREAM3D::HDF5::Weight);
    weight->SetValue(0, 2.0f);

    VectorOfFloatArray odfWeights;
    odfWeights.push_back(euler1);
    odfWeights.push_back(euler2);
    odfWeights.push_back(euler3);
    odfWeights.push_back(sigma);
    odfWeights.push_back(weight);
    statsData->setODF_Weights(odfWeights);
    statsData->setAxisODF_Weights(odfWeights);
  }
  //unsigned long long int nElements = 18 * 18 * 18;
  Texture::calculateMDFData<std::vector<float>, CubicOps>(angles, axes, weights, odf, mdf);

  // Stupid, but copy the data from the vector to the DataArray<float> instance
  FloatArrayType::Pointer mdfData = FloatArrayType::CreateArray(mdf.size() );
  mdfData->SetName(DREAM3D::HDF5::MisorientationBins);
  for(size_t i = 0; i < mdf.size(); ++i)
  {
    mdfData->SetValue(i, mdf[i]);
  }
  {
    FloatArrayType::Pointer angles = FloatArrayType::CreateArray(1);
    angles->SetName(DREAM3D::HDF5::Angle);
    angles->SetValue(0, 45.0f);
    FloatArrayType::Pointer axis = FloatArrayType::CreateArray(3);
    axis->SetName(DREAM3D::HDF5::Axis);
    axis->SetNumberOfComponents(3);
    axis->SetComponent(0, 0, 1.0f);
    axis->SetComponent(0, 1, 2.0f);
    axis->SetComponent(0, 2, 3.0f);
    FloatArrayType::Pointer weight = FloatArrayType::CreateArray(1);
    weight->SetName(DREAM3D::HDF5::Weight);
    weight->SetValue(0, 2.1254f);
    VectorOfFloatArray mdfWeights;
    mdfWeights.push_back(angles);
    mdfWeights.push_back(axis);
    mdfWeights.push_back(weight);
    statsData->setMDF_Weights(mdfWeights);
  }

  // Push it onto the StatsData instance
  statsData->setMisorientationBins(mdfData);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void initializeAxisODF( StatsData::Pointer statsData)
{
  float totalWeight = 0.0;

  std::vector<float> e1s;
  std::vector<float> e2s;
  std::vector<float> e3s;
  std::vector<float> weights;
  std::vector<float> sigmas;
  std::vector<float> aodf;

  Texture::calculateOrthoRhombicODFData(e1s, e2s, e3s, weights, sigmas, true, aodf, totalWeight);

  // Stupid, but copy the data from the vector to the DataArray<float> instance
  FloatArrayType::Pointer aodfData = FloatArrayType::CreateArray(aodf.size() );
  aodfData->SetName(DREAM3D::HDF5::AxisOrientation);
  for(size_t i = 0; i < aodf.size(); ++i)
  {
    aodfData->SetValue(i, aodf[i]);
  }

  // Push it onto the StatsData instance
  statsData->setAxisOrientation(aodfData);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int computeBinsAndCutOffs( float mu, float sigma,
                                     float minCutOff, float maxCutOff,
                                     float binStepSize,
                                     std::vector<float> &binsizes,
                                     std::vector<float> &xCo,
                                     std::vector<float> &yCo,
                                     float &xMax, float &yMax,
                                     std::vector<float> &x,
                                     std::vector<float> &y)
{
  int err = 0;
  int size = 250;

  StatsGen sg;
  err = sg.GenLogNormalPlotData<std::vector<float> > (mu, sigma, x, y, size);
  if (err == 1)
  {
    //TODO: Present Error Message
    return -1;
  }

//  float xMax = std::numeric_limits<float >::min();
//  float yMax = std::numeric_limits<float >::min();
  for (int i = 0; i < size; ++i)
  {
    //   std::cout << x[i] << "  " << y[i] << std::endl;
    if (x[i] > xMax)
    {
      xMax = x[i];
    }
    if (y[i] > yMax)
    {
      yMax = y[i];
    }
  }

  xCo.clear();
  yCo.clear();
  int numsizebins = 1;
  binsizes.clear();
  // std::vector<int> numgrains;
  err = sg.GenCutOff<float, std::vector<float> > (mu, sigma, minCutOff, maxCutOff, binStepSize, xCo, yCo, yMax, numsizebins, binsizes);

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestStatsData()
{

  StatsDataContainer::Pointer statsContainer = StatsDataContainer::New();

  float m_PhaseFraction = 0.25;
  float m_TotalPhaseFraction = 1.0f;

  int err = 0;
  float mu = 1.0f;
  float sigma = 0.1f;
  float minCutOff = 5.0f;
  float maxCutOff = 5.0f;
  float binStep = 0.5f;

  // Set the first phase
  float calcPhaseFraction = m_PhaseFraction / m_TotalPhaseFraction;

  std::vector<float> xCo;
  std::vector<float> yCo;
  std::vector<float> binsizes;
  float xMax = std::numeric_limits<float>::min();
  float yMax = std::numeric_limits<float>::min();
  std::vector<float> x;
  std::vector<float> y;
  err = computeBinsAndCutOffs(mu, sigma, minCutOff, maxCutOff, binStep, binsizes, xCo, yCo, xMax, yMax, x, y);
  DREAM3D_REQUIRE(err >= 0)

  // We need to compute the Max and Min Diameter Bin Values
  float mindiameter = xCo[0];
  float maxdiameter = xCo[1];
  float avglogdiam = mu;
  float sdlogdiam = sigma;

  size_t nBins = binsizes.size();

  // Phase 1
  StatsData::Pointer data1 = StatsData::New();
  data1->setPhaseFraction(calcPhaseFraction);
  data1->setGrainDiameterInfo(binStep, maxdiameter, mindiameter);
  data1->setGrainSizeDistribution(avglogdiam, sdlogdiam);
  initializeOmega3(data1, nBins);
  initializeBOverA(data1, nBins);
  initializeCOverA(data1, nBins);
  initializeCOverB(data1, nBins);
  initializeNeighbors(data1, binsizes);
  initializeODF_MDF(data1);
  initializeAxisODF(data1);
  statsContainer->setStatsData(0, data1);
  DREAM3D_REQUIRE_EQUAL(1, statsContainer->GetNumberOfTuples());

  // Phase 2
  StatsData::Pointer data2 = StatsData::New();
  calcPhaseFraction = 0.75f;
  data2->setPhaseFraction(calcPhaseFraction);
  data2->setGrainDiameterInfo(binStep, maxdiameter, mindiameter);
  data2->setGrainSizeDistribution(avglogdiam, sdlogdiam);
  initializeOmega3(data2, nBins);
  initializeBOverA(data2, nBins);
  initializeCOverA(data2, nBins);
  initializeCOverB(data2, nBins);
  initializeNeighbors(data2, binsizes);
  initializeODF_MDF(data2);
  initializeAxisODF(data2);
  statsContainer->setStatsData(1, data2);
  DREAM3D_REQUIRE_EQUAL(2, statsContainer->GetNumberOfTuples());

  statsContainer->clearAll();
  DREAM3D_REQUIRE_EQUAL(0, statsContainer->GetNumberOfTuples());

  statsContainer->setStatsData(0, data1);
  statsContainer->setStatsData(1, data2);

  // Set the Global variable
  g_StatsContainer = statsContainer;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestWriteData()
{

  // Make sure the output directory is created
  MXADir::mkdir(UnitTest::StatsDataTest::TestDir, true);

  herr_t err = 0;
  hid_t fileId = H5Utilities::createFile(UnitTest::StatsDataTest::TestFile);
  DREAM3D_REQUIRE( fileId > 0);


  hid_t dcGid = H5Utilities::createGroup(fileId, DREAM3D::HDF5::DataContainerName);
  DREAM3D_REQUIRE( dcGid > 0);

  // Write the Ensemble data
  err = H5Utilities::createGroupsFromPath(H5_ENSEMBLE_DATA_GROUP_NAME, dcGid);
  DREAM3D_REQUIRE( err >= 0);

  err = H5Lite::writeStringAttribute(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, H5_NAME, H5_ENSEMBLE_DATA_DEFAULT);
  DREAM3D_REQUIRE( err >= 0);

  hid_t ensembleGid = H5Gopen(dcGid, H5_ENSEMBLE_DATA_GROUP_NAME, H5P_DEFAULT);
  DREAM3D_REQUIRE( ensembleGid > 0);


  g_StatsContainer->writeH5Data(ensembleGid);



  err = H5Utilities::closeHDF5Object(ensembleGid);
  err = H5Utilities::closeHDF5Object(dcGid);
  err = H5Utilities::closeFile(fileId);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestReadData()
{

}

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;
#if !REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() );
#endif

  DREAM3D_REGISTER_TEST( TestStatsData() );
  DREAM3D_REGISTER_TEST( TestWriteData() );
  DREAM3D_REGISTER_TEST( TestReadData() );


#if REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() );
#endif


  PRINT_TEST_SUMMARY();
  return err;
}

