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

#include <string>
#include <vector>

#include "MXA/MXA.h"
#include "MXA/Common/LogTime.h"
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/HKL/CtfConstants.h"


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Observer.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/QualityMetricFilter.h"
#include "SIMPLib/IOFilters/DataContainerWriter.h"
#include "SIMPLib/IOFilters/DataContainerReader.h"
#include "SIMPLib/IOFilters/VtkRectilinearGridWriter.h"
#include "SIMPLib/IOFilters/ReadH5Ebsd.h"
#include "SIMPLib/ReconstructionFilters/AlignSectionsFeature.h"
#include "SIMPLib/ReconstructionFilters/EBSDSegmentGrains.h"
#include "SIMPLib/ProcessingFilters/MinSize.h"
#include "SIMPLib/ProcessingFilters/MinNeighbors.h"
#include "SIMPLib/GenericFilters/FindNeighbors.h"
#include "SIMPLib/StatisticsFilters/FindSizes.h"
#include "SIMPLib/StatisticsFilters/FindShapes.h"
#include "SIMPLib/StatisticsFilters/FindAvgOrientations.h"
#include "SIMPLib/StatisticsFilters/FindNeighborhoods.h"
#include "SIMPLib/StatisticsFilters/FindODF.h"
#include "SIMPLib/StatisticsFilters/FindMDF.h"

#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"


#define DREAM3D_BENCHMARKS 1

#if DREAM3D_BENCHMARKS
#define START_CLOCK()\
  unsigned long long int millis;\
  millis = MXA::getMilliSeconds();
#else
#define START_CLOCK() unsigned long long int millis = 0;\
  millis = 0;
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
  QFile::remove(UnitTest::FindNeighborTest::OutputFile);
  QFile::remove(UnitTest::FindNeighborTest::VtkOutputFile);
}


void updateProgressAndMessage(const QString& msg, int prog)
{
  std::cout << prog << "% - " << msg << std::endl;
}

QString getH5EbsdFile()
{
  QString s = UnitTest::DataDir + MXADir::Separator + "AdjTi6246MicroTensile.h5ebsd";
  return s;
}

int getZStartIndex() { return 50; }
int getZEndIndex() { return 250; }
DataArray<unsigned int>::Pointer getPhaseTypes()
{
  DataArray<unsigned int>::Pointer phaseTypes
    = DataArray<unsigned int>::CreateArray(2, DREAM3D::EnsembleData::PhaseTypes);
  phaseTypes->SetValue(0, DREAM3D::PhaseType::UnknownPhaseType);
  phaseTypes->SetValue(1, DREAM3D::PhaseType::PrimaryPhase);
  return phaseTypes;
}

std::vector<QualityMetricFilter::Pointer> getQualityMetricFilters()
{
  std::vector<QualityMetricFilter::Pointer> filters;
  {
    QualityMetricFilter::Pointer filter = QualityMetricFilter::New();
    filter->setFeatureName(Ebsd::Ang::ImageQuality);
    filter->setFeatureValue(450);
    filter->setFeatureOperator(">");
    filter->setFeaturePhaseNumber(1);
    filters.push_back(filter);
  }
  {
    QualityMetricFilter::Pointer filter = QualityMetricFilter::New();
    filter->setFeatureName(Ebsd::Ang::ConfidenceIndex);
    filter->setFeatureValue(0.1f);
    filter->setFeatureOperator(">");
    filter->setFeaturePhaseNumber(1);
    filters.push_back(filter);
  }
  {
    QualityMetricFilter::Pointer filter = QualityMetricFilter::New();
    filter->setFeatureName(Ebsd::Ang::ImageQuality);
    filter->setFeatureValue(350);
    filter->setFeatureOperator(">");
    filter->setFeaturePhaseNumber(2);
    filters.push_back(filter);
  }
  {
    QualityMetricFilter::Pointer filter = QualityMetricFilter::New();
    filter->setFeatureName(Ebsd::Ang::ConfidenceIndex);
    filter->setFeatureValue(0.025f);
    filter->setFeatureOperator(">");
    filter->setFeaturePhaseNumber(2);
    filters.push_back(filter);
  }

  return filters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void pipelineProgress(int value)
{
  std::cout << value << "%" << std::endl;
}

void pipelineProgressMessage(const QString& msg)
{
  std::cout << msg << std::endl;
}

void pipelineErrorMessage(const char* message)
{
  std::cout << "Error Message: " << message << std::endl;
}

void setErrorCondition(int err)
{
  std::cout << "Error Condition: " << err << std::endl;
}

void pipelineFinished()
{
  std::cout << "Pipeline Complete." << std::endl;
}

typedef std::vector<AbstractFilter::Pointer>  FilterContainerType;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestFindNeighbors()
{
  float m_MisorientationTolerance = 5.0f;
  int m_MinAllowedGrainSize = 10;
  int m_MinNumNeighbors = 1;

  // Create our Pipeline object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  typedef DataArray<unsigned int> XTalStructArrayType;
  XTalStructArrayType::Pointer xtal = XTalStructArrayType::CreateArray(2, DREAM3D::EnsembleData::CrystalStructures);
  xtal->SetValue(0, Ebsd::CrystalStructure::UnknownCrystalStructure);
  xtal->SetValue(1, Ebsd::CrystalStructure::Cubic);

  QString m_OutputDirectory = MXADir::toNativeSeparators(UnitTest::FindNeighborTest::TestDir);
  QDir dir(m_OutputDirectory);
  dir.mkpath(".");

// updateProgressAndMessage(("Loading Slices"), 10);
  ReadH5Ebsd::Pointer read_h5ebsd = ReadH5Ebsd::New();
  read_h5ebsd->setH5EbsdFile(getH5EbsdFile());
  read_h5ebsd->setRefFrameZDir(Ebsd::LowtoHigh);
  read_h5ebsd->setZStartIndex(getZStartIndex());
  read_h5ebsd->setZEndIndex(getZEndIndex());
  read_h5ebsd->setPTypes(getPhaseTypes());
  read_h5ebsd->setQualityMetricFilters(getQualityMetricFilters());
  pipeline->pushBack(read_h5ebsd);

  AlignSectionsFeature::Pointer align_sections = AlignSectionsFeature::New();
// align_sections->setCrystalStructures(xtal);
  pipeline->pushBack(align_sections);

  EBSDSegmentGrains::Pointer ebsdsegment_grains = EBSDSegmentGrains::New();
  ebsdsegment_grains->setMisorientationTolerance(m_MisorientationTolerance);
  pipeline->pushBack(ebsdsegment_grains);

  MinSize::Pointer min_size = MinSize::New();
  min_size->setMinAllowedGrainSize(m_MinAllowedGrainSize);
  pipeline->pushBack(min_size);

  MinNeighbors::Pointer min_neighbors = MinNeighbors::New();
  min_neighbors->setMinNumNeighbors(m_MinNumNeighbors);
//  pipeline->pushBack(min_neighbors);


  bool m_WriteVtkFile(true);
  bool m_WriteBinaryVTKFiles(true);
  bool m_WritePhaseId(true);
  bool m_WriteIPFColor(true);
  bool m_WriteGoodVoxels(true);

  VtkRectilinearGridWriter::Pointer vtkWriter = VtkRectilinearGridWriter::New();
  if(m_WriteVtkFile)
  {
    vtkWriter->setOutputFile(UnitTest::FindNeighborTest::VtkOutputFile);
    vtkWriter->setWriteGrainIds(true);
    vtkWriter->setWritePhaseIds(m_WritePhaseId);
    vtkWriter->setWriteGoodVoxels(m_WriteGoodVoxels);
    vtkWriter->setWriteIPFColors(m_WriteIPFColor);
    vtkWriter->setWriteBinaryFile(m_WriteBinaryVTKFiles);
    pipeline->pushBack(vtkWriter);
  }

  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile(UnitTest::FindNeighborTest::OutputFile);
  pipeline->pushBack(writer);

  std::cout << "********* RUNNING PREFLIGHT **********************" << std::endl;
  int err = pipeline->preflightPipeline();
  DREAM3D_REQUIRE_EQUAL(err, 0);


  std::cout << "********* RUNNING PIPELINE **********************" << std::endl;
  pipeline->run();
  err = pipeline->getErrorCondition();
  DREAM3D_REQUIRE_EQUAL(err, 0);

  DREAM3D_REQUIRE_EQUAL(false, pipeline->empty());

  pipeline->clear();
  DREAM3D_REQUIRE_EQUAL(0, pipeline->size());
  DREAM3D_REQUIRE_EQUAL(true, pipeline->empty());

  updateProgressAndMessage("FindNeighborsTest Complete", 100);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestDataContainerReader()
{
  int err = 0;

  // Create a Vector to hold all the filters. Later on we will execute all the filters
  FilterPipeline::Pointer pipeline = FilterPipeline::New();


  DataContainerReader::Pointer h5Reader = DataContainerReader::New();
  h5Reader->setInputFile(UnitTest::FindNeighborTest::OutputFile);
  pipeline->pushBack(h5Reader);

#if 0
  IDataArray* iDataPtr = NULL;
  iDataPtr = m->getFeatureData(DREAM3D::EnsembleData::CrystalStructures).get();
  UInt32ArrayType* data = UInt32ArrayType::SafeObjectDownCast<IDataArray*, UInt32ArrayType*>(iDataPtr);
  m_CrystalStructure = data->GetValue(index);
#endif

  FindSizes::Pointer find_sizes = FindSizes::New();
  find_sizes->setDistributionType(DREAM3D::DistributionType::LogNormal);
  pipeline->pushBack(find_sizes);

  FindShapes::Pointer find_shapes = FindShapes::New();
  find_shapes->setDistributionType(DREAM3D::DistributionType::Beta);
  pipeline->pushBack(find_shapes);

  FindNeighborhoods::Pointer find_neighborhoods = FindNeighborhoods::New();
  find_neighborhoods->setDistributionType(DREAM3D::DistributionType::LogNormal);
  pipeline->pushBack(find_neighborhoods);

  FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
  pipeline->pushBack(find_neighbors);

  FindAvgOrientations::Pointer find_avgorients = FindAvgOrientations::New();
  pipeline->pushBack(find_avgorients);

  FindODF::Pointer find_odf = FindODF::New();
  pipeline->pushBack(find_odf);

  FindMDF::Pointer find_mdf = FindMDF::New();
  pipeline->pushBack(find_mdf);

  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile(UnitTest::FindNeighborTest::StatsFile);
  pipeline->pushBack(writer);


//  std::cout << "********* RUNNING PREFLIGHT **********************" << std::endl;
//  int err = pipeline->preflightPipeline();
//  DREAM3D_REQUIRE_EQUAL(err, 0);


  std::cout << "********* RUNNING PIPELINE **********************" << std::endl;
  VoxelDataContainer::Pointer m = VoxelDataContainer::New();
  pipeline->run();
  err = pipeline->getErrorCondition();
  DREAM3D_REQUIRE_EQUAL(err, 0);


}


void OtherTest()
{

  int err = 0;

  // Create a Vector to hold all the filters. Later on we will execute all the filter
  VoxelDataContainer::Pointer m = VoxelDataContainer::New();

  DataContainerReader::Pointer reader = DataContainerReader::New();
  reader->setInputFile("C:\\Users\\mjackson\\Desktop\\FindNeighborTest_Rewrite.h5");
  reader->setInputFile(UnitTest::FindNeighborTest::OutputFile);
  reader->setVoxelDataContainer(m.get());
  reader->setReadCellData(true);
  reader->setReadFeatureData(true);
  reader->setReadEnsembleData(true);

  reader->execute();
  err = reader->getErrorCondition();

  IDataArray::Pointer iPtr = m->getEnsembleData(DREAM3D::EnsembleData::CrystalStructures);
  DREAM3D_REQUIRE_NE(NULL, iPtr.get());


  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile(UnitTest::FindNeighborTest::OutputFile2);
  writer->setVoxelDataContainer(m.get());
  writer->execute();
  err = writer->getErrorCondition();

  DREAM3D_REQUIRE_EQUAL(err, 0);

}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;
#if !REMOVE_TEST_FILES
//  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif
  DREAM3D_REGISTER_TEST( TestFindNeighbors() )
//  DREAM3D_REGISTER_TEST( TestDataContainerReader() )
//  DREAM3D_REGISTER_TEST( OtherTest() )
#if REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif
  PRINT_TEST_SUMMARY();
  return err;
}
