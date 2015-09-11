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


#include <stdlib.h>
#include <string.h>


#include <limits>
#include <iostream>
#include <limits>
#include <string>

#include <QtCore/QDir>
#include <QtCore/QFile>

#include "H5Support/H5Utilities.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Common/StatsGen.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/StatsData/StatsData.h"
#include "SIMPLib/IOFilters/DataContainerWriter.h"
#include "SIMPLib/IOFilters/DataContainerReader.h"

#include "UnitTestSupport.hpp"

#include "TestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
  QFile::remove(UnitTest::StatsDataTest::TestFile);
  QFile::remove(UnitTest::StatsDataTest::TestFile2);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void initializeOmega3(StatsData::Pointer statsData, int count, uint32_t distType = DREAM3D::DistributionType::Beta)
{
  float alpha, beta;
  SIMPL_RANDOMNG_NEW()
  VectorOfFloatArray data;
  FloatArrayType::Pointer alphas = FloatArrayType::CreateArray(count, DREAM3D::StringConstants::Alpha);
  FloatArrayType::Pointer betas = FloatArrayType::CreateArray(count, DREAM3D::StringConstants::Beta);

  for (int32_t i = 0; i < count; ++i)
  {
    alpha = (0 * i) + 10.0 + rg.genrand_res53();
    beta = (0 * i) + 1.5 + (0.5 * rg.genrand_res53());
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
void initializeBOverA(StatsData::Pointer statsData, int count, uint32_t distType = DREAM3D::DistributionType::Beta)
{
  float alpha, beta;
  SIMPL_RANDOMNG_NEW()
  VectorOfFloatArray data;
  FloatArrayType::Pointer alphas = FloatArrayType::CreateArray(count, DREAM3D::StringConstants::Alpha);
  FloatArrayType::Pointer betas = FloatArrayType::CreateArray(count, DREAM3D::StringConstants::Beta);

  for (int32_t i = 0; i < count; ++i)
  {
    alpha = (0 * i) + 15.0 + rg.genrand_res53();
    beta = (0 * i) + 1.25 + (0.5 * rg.genrand_res53());
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
void initializeCOverA(StatsData::Pointer statsData, int count, uint32_t distType = DREAM3D::DistributionType::Beta)
{
  float alpha, beta;
  SIMPL_RANDOMNG_NEW()
  VectorOfFloatArray data;
  FloatArrayType::Pointer alphas = FloatArrayType::CreateArray(count, DREAM3D::StringConstants::Alpha);
  FloatArrayType::Pointer betas = FloatArrayType::CreateArray(count, DREAM3D::StringConstants::Beta);

  for (int32_t i = 0; i < count; ++i)
  {
    alpha = (0 * i) + 14.0 + rg.genrand_res53();
    beta = (0 * i) + 1.15 + (0.5 * rg.genrand_res53());
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
void initializeNeighbors(StatsData::Pointer statsData, std::vector<float> binNumbers, uint32_t distType = DREAM3D::DistributionType::Power)
{
  float alpha, k, beta;
  int32_t count = binNumbers.size();

  SIMPL_RANDOMNG_NEW()
  VectorOfFloatArray data;
  FloatArrayType::Pointer alphas = FloatArrayType::CreateArray(count, DREAM3D::StringConstants::Alpha);
  FloatArrayType::Pointer betas = FloatArrayType::CreateArray(count, DREAM3D::StringConstants::Beta);
  FloatArrayType::Pointer ks = FloatArrayType::CreateArray(count, DREAM3D::StringConstants::Exp_k);

  int middlebin = count / 2;

  for (int32_t i = 0; i < count; ++i)
  {
    alpha = (4 * (binNumbers[i] / binNumbers[middlebin])) + rg.genrand_res53();
    k = 2 + (0.2 * (binNumbers[i] / binNumbers[middlebin])) + (0.05 * rg.genrand_res53());
    beta = (0 * i) + 1;
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
void initializeODF_MDF(StatsData::Pointer statsData)
{
  std::vector<float> e1s;
  std::vector<float> e2s;
  std::vector<float> e3s;
  std::vector<float> weights;
  std::vector<float> sigmas;
  std::vector<float> odf;
  std::vector<float> mdf;
  std::vector<float> angles;
  std::vector<float> axes;

  Texture::calculateCubicODFData(e1s, e2s, e3s, weights, sigmas, true, odf);

  // Stupid, but copy the data from the vector to the DataArray<float> instance
  FloatArrayType::Pointer odfData = FloatArrayType::CreateArray(odf.size(), DREAM3D::StringConstants::ODF);
  for (size_t i = 0; i < odf.size(); ++i)
  {
    odfData->SetValue(i, odf[i]);
  }

  // Push it onto the StatsData instance
  statsData->setODF(odfData);

  {
    FloatArrayType::Pointer euler1 = FloatArrayType::CreateArray(1, DREAM3D::StringConstants::Euler1);
    euler1->SetValue(0, 10.0f);
    FloatArrayType::Pointer euler2 = FloatArrayType::CreateArray(1, DREAM3D::StringConstants::Euler2);
    euler2->SetValue(0, 10.0f);
    FloatArrayType::Pointer euler3 = FloatArrayType::CreateArray(1, DREAM3D::StringConstants::Euler3);
    euler3->SetValue(0, 10.0f);
    FloatArrayType::Pointer sigma = FloatArrayType::CreateArray(1, DREAM3D::StringConstants::Sigma);
    sigma->SetValue(0, 0.99f);
    FloatArrayType::Pointer weight = FloatArrayType::CreateArray(1, DREAM3D::StringConstants::Weight);
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
  FloatArrayType::Pointer mdfData = FloatArrayType::CreateArray(mdf.size(), DREAM3D::StringConstants::MisorientationBins);
  for (size_t i = 0; i < mdf.size(); ++i)
  {
    mdfData->SetValue(i, mdf[i]);
  }
  {
    FloatArrayType::Pointer angles = FloatArrayType::CreateArray(1, DREAM3D::StringConstants::Angle);
    angles->SetValue(0, 45.0f);
    FloatArrayType::Pointer axis = FloatArrayType::CreateArray(3, DREAM3D::StringConstants::Axis);
    axis->SetNumberOfComponents(3);
    axis->SetComponent(0, 0, 1.0f);
    axis->SetComponent(0, 1, 2.0f);
    axis->SetComponent(0, 2, 3.0f);
    FloatArrayType::Pointer weight = FloatArrayType::CreateArray(1, DREAM3D::StringConstants::Weight);
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
void initializeAxisODF(StatsData::Pointer statsData)
{
  std::vector<float> e1s;
  std::vector<float> e2s;
  std::vector<float> e3s;
  std::vector<float> weights;
  std::vector<float> sigmas;
  std::vector<float> aodf;

  Texture::calculateOrthoRhombicODFData(e1s, e2s, e3s, weights, sigmas, true, aodf);

  // Stupid, but copy the data from the vector to the DataArray<float> instance
  FloatArrayType::Pointer aodfData = FloatArrayType::CreateArray(aodf.size(), DREAM3D::StringConstants::AxisOrientation);
  for (size_t i = 0; i < aodf.size(); ++i)
  {
    aodfData->SetValue(i, aodf[i]);
  }

  // Push it onto the StatsData instance
  statsData->setAxisOrientation(aodfData);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int computeBinsAndCutOffs(float mu,
                          float sigma,
                          float minCutOff,
                          float maxCutOff,
                          float binStepSize,
                          std::vector<float>& binsizes,
                          std::vector<float>& xCo,
                          std::vector<float>& yCo,
                          float& xMax,
                          float& yMax,
                          std::vector<float>& x,
                          std::vector<float>& y)
{
  int err = 0;
  int size = 250;

  StatsGen sg;
  err = sg.GenLogNormalPlotData<std::vector<float> >(mu, sigma, x, y, size);
  if(err == 1)
  {
    //TODO: Present Error Message
    return -1;
  }

//  float xMax = std::numeric_limits<float >::min();
//  float yMax = std::numeric_limits<float >::min();
  for (int i = 0; i < size; ++i)
  {
    //   std::cout << x[i] << "  " << y[i] << std::endl;
    if(x[i] > xMax)
    {
      xMax = x[i];
    }
    if(y[i] > yMax)
    {
      yMax = y[i];
    }
  }

  xCo.clear();
  yCo.clear();
  int numsizebins = 1;
  binsizes.clear();
  // std::vector<int> numgrains;
  err = sg.GenCutOff<float, std::vector<float> >(mu, sigma, minCutOff, maxCutOff, binStepSize, xCo, yCo, yMax, numsizebins, binsizes);

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsDataArray::Pointer createStatsDataArray()
{

  StatsDataArray::Pointer statsArray = StatsDataArray::New();
  StatsData::Pointer s0; // Create a NULL instance of StatsData;
  statsArray->setStatsData(0, s0); // The underlying storage will resize as needed.

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
//  float avglogdiam = mu;
//  float sdlogdiam = sigma;

  size_t nBins = binsizes.size();
  // Copy this into the DataArray<float>
  FloatArrayType::Pointer binNumbers = FloatArrayType::CreateArray(nBins, DREAM3D::StringConstants::BinNumber);
  ::memcpy(binNumbers->getVoidPointer(0), &(binsizes.front()), binsizes.size() * sizeof(float));


  // Phase 1
  StatsData::Pointer data1 = StatsData::New();
  data1->setPhaseFraction(calcPhaseFraction);
  data1->setGrainDiameterInfo(binStep, maxdiameter, mindiameter);
//  data1->setGrainSizeDistribution(avglogdiam, sdlogdiam);
  data1->setBinNumbers(binNumbers);
  initializeOmega3(data1, nBins);
  initializeBOverA(data1, nBins);
  initializeCOverA(data1, nBins);
  initializeNeighbors(data1, binsizes);
  initializeODF_MDF(data1);
  initializeAxisODF(data1);
  statsArray->setStatsData(1, data1);

  // Phase 2
  StatsData::Pointer data2 = StatsData::New();
  calcPhaseFraction = 0.75f;
  data2->setPhaseFraction(calcPhaseFraction);
  data2->setGrainDiameterInfo(binStep, maxdiameter, mindiameter);
//  data2->setGrainSizeDistribution(avglogdiam, sdlogdiam);
  data2->setBinNumbers(binNumbers);
  initializeOmega3(data2, nBins);
  initializeBOverA(data2, nBins);
  initializeCOverA(data2, nBins);
  initializeNeighbors(data2, binsizes);
  initializeODF_MDF(data2);
  initializeAxisODF(data2);
  statsArray->setStatsData(2, data2);

  return statsArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestStatsData()
{
  StatsDataArray::Pointer statsArray = createStatsDataArray();
  DREAM3D_REQUIRE_EQUAL(3, statsArray->GetNumberOfTuples());

  StatsData::Pointer s0 = statsArray->getStatsData(0);

  StatsData::Pointer s1 = statsArray->getStatsData(1);
  StatsData::Pointer s2 = statsArray->getStatsData(2);

  std::vector<size_t> idx(1, 1);
  statsArray->EraseTuples(idx);
  DREAM3D_REQUIRE_EQUAL(2, statsArray->GetNumberOfTuples());
  idx[0] = 0;
  statsArray->EraseTuples(idx);
  DREAM3D_REQUIRE_EQUAL(1, statsArray->GetNumberOfTuples());

  statsArray->clearAll();
  DREAM3D_REQUIRE_EQUAL(0, statsArray->GetNumberOfTuples());

  statsArray->setStatsData(0, s0);
  statsArray->setStatsData(1, s1);
  statsArray->setStatsData(2, s2);
  DREAM3D_REQUIRE_EQUAL(3, statsArray->GetNumberOfTuples());


  StatsDataArray& arrayRef = *statsArray;
  StatsData::Pointer t0 = arrayRef[0];
  DREAM3D_REQUIRE_EQUAL(NULL, t0.get());

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestWriteData()
{
  // Make sure the output directory is created
  QDir dir(UnitTest::StatsDataTest::TestDir);
  dir.mkpath(".");

  // Create some test data for a 2 phase material
  StatsDataArray::Pointer statsArray = createStatsDataArray();

  // Create a data container and set the StatsArray
  VoxelDataContainer::Pointer m = VoxelDataContainer::New();
  m->addEnsembleData(DREAM3D::EnsembleData::Statistics, statsArray);

  DataArray<unsigned int>::Pointer crystalStructures = DataArray<unsigned int>::CreateArray(3, DREAM3D::EnsembleData::CrystalStructures);
  crystalStructures->SetValue(0, Ebsd::CrystalStructure::UnknownCrystalStructure);
  crystalStructures->SetValue(1, Ebsd::CrystalStructure::Cubic);
  crystalStructures->SetValue(2, Ebsd::CrystalStructure::Cubic);
  m->addEnsembleData(DREAM3D::EnsembleData::CrystalStructures, crystalStructures);

  DataArray<unsigned int>::Pointer phaseTypes = DataArray<unsigned int>::CreateArray(3, DREAM3D::EnsembleData::PhaseTypes);
  phaseTypes->SetValue(0, DREAM3D::PhaseType::UnknownPhaseType);
  phaseTypes->SetValue(1, DREAM3D::PhaseType::PrimaryPhase);
  phaseTypes->SetValue(2, DREAM3D::PhaseType::PrimaryPhase);
  m->addEnsembleData(DREAM3D::EnsembleData::PhaseTypes, phaseTypes);

  DataArray<unsigned int>::Pointer shapeTypes = DataArray<unsigned int>::CreateArray(3, DREAM3D::EnsembleData::ShapeTypes);
  shapeTypes->SetValue(0, DREAM3D::ShapeType::UnknownShapeType);
  shapeTypes->SetValue(1, DREAM3D::ShapeType::EllipsoidShape);
  shapeTypes->SetValue(2, DREAM3D::ShapeType::EllipsoidShape);
  m->addEnsembleData(DREAM3D::EnsembleData::ShapeTypes, shapeTypes);


  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  // H5StatsDataWriter::Pointer writer = H5StatsDataWriter::New();
  writer->setVoxelDataContainer(m.get());
  writer->setOutputFile(UnitTest::StatsDataTest::TestFile);
  writer->execute();
  DREAM3D_REQUIRE( writer->getErrorCondition() >= 0)


  // This is an example of getting the StatsDataArray back from the DataContiner
  // and being able to do something meaningful with it.
  // Since StatsDataArray subclasses the IDataArray there are some simple things you
  // can do like ask how many StatsData instances are being stored:
  int numStatsData = m->getEnsembleData(DREAM3D::EnsembleData::Statistics)->GetSize(); // This works because the
  // size of each Tuple is 1
  numStatsData = m->getEnsembleData(DREAM3D::EnsembleData::Statistics)->GetNumberOfTuples();
  // This also works. Either way the value at this point should be '3'
  DREAM3D_REQUIRE_EQUAL(3, numStatsData);

  // This will get us a Pointer to the instance of StatsDataArray class that is stored
  // in the DataContainer.
  StatsDataArray* statsArrayPtr = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getEnsembleData(DREAM3D::EnsembleData::Statistics).get());

  // Unfortunately we can NOT use the operator [] with a pointer so sometimes creating
  // a reference variable can be help. We do this by dereferencing the pointer and
  // assigning that to a "reference variable"
  StatsDataArray& statsArrayRef = *statsArrayPtr;

  // So now we can get to the actual "StatsData" instances stored in the StatsDataArray object.
  StatsData::Pointer s1 = statsArrayRef[1];
  // We could have also done it using the pointer from above in this manner:
  StatsData::Pointer s1_Alt = statsArrayPtr->getStatsData(1);

  // Both shared pointer objects should wrapp the SAME StatsData Pointer. This line
  // will verify that. You would NOT use this next line in normal production code. It
  // is for Unit Testing ONLY.
  DREAM3D_REQUIRE_EQUAL( (s1.get()), (s1_Alt.get()))

  size_t numBins = s1->getNumberOfBins();

  // So Now if you want to get the B Over A values you can do this:
  VectorOfFloatArray bovera = s1->getGrainSize_BOverA();
  // This holds 2 arrays. Alphas and Betas which are of type FloatArrayType, or a
  // DataArray<float>::Pointer

  FloatArrayType::Pointer alphas = bovera[0];
  FloatArrayType::Pointer betas = bovera[1];

  DREAM3D_REQUIRE_EQUAL(numBins, alphas->GetSize());
  // Since these are all shared_pointers, well at least the alphas and betas are,
  // anything you do to this array (adding, erasing, changing values) you are doing
  // to the arrays stored in the Data Container.
  alphas->SetValue(0, 100.0f); // This sets the value at index 0 to 100.0
  betas->SetValue(0, 6.0f); // This sets the value at index 0 to 6.0
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestReadData()
{
  VoxelDataContainer::Pointer m = VoxelDataContainer::New();

  DataContainerReader::Pointer reader = DataContainerReader::New();
  reader->setInputFile(UnitTest::StatsDataTest::TestFile);
  reader->setVoxelDataContainer(m.get());
  reader->execute();
  int err = reader->getErrorCondition();
  if (err < 0)
  {
    std::cout << reader->getErrorMessage() << std::endl;
  }
  DREAM3D_REQUIRE( err >= 0);

  IDataArray::Pointer p = m->getEnsembleData(DREAM3D::EnsembleData::Statistics);
  DREAM3D_REQUIRE_EQUAL(p->GetNumberOfTuples(), 3);

  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile(UnitTest::StatsDataTest::TestFile2);
  writer->setVoxelDataContainer(m.get());
  writer->execute();
  err = writer->getErrorCondition();
  if (err < 0)
  {
    std::cout << "Rewriting the data failed" << std::endl;
  }
  DREAM3D_REQUIRE( err >= 0);
}

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;
#if !REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles())
#endif

  DREAM3D_REGISTER_TEST( TestStatsData())
  DREAM3D_REGISTER_TEST( TestWriteData())
  DREAM3D_REGISTER_TEST( TestReadData())

#if REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif

  PRINT_TEST_SUMMARY();
  return err;
}

