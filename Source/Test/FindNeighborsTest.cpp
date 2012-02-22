/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include <string>
#include <vector>

#include "MXA/MXA.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/HKL/CtfConstants.h"


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Observer.h"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/Common/QualityMetricFilter.h"
#include "DREAM3DLib/IOFilters/DataContainerWriter.h"
#include "DREAM3DLib/IOFilters/DataContainerReader.h"
#include "DREAM3DLib/IOFilters/VtkRectilinearGridWriter.h"
#include "DREAM3DLib/ReconstructionFilters/LoadSlices.h"
#include "DREAM3DLib/ReconstructionFilters/AlignSections.h"
#include "DREAM3DLib/ReconstructionFilters/SegmentGrains.h"
#include "DREAM3DLib/ReconstructionFilters/CleanupGrains.h"
#include "DREAM3DLib/PrivateFilters/FindNeighbors.h"
#include "DREAM3DLib/StatisticsFilters/FindSizes.h"
#include "DREAM3DLib/StatisticsFilters/FindShapes.h"
#include "DREAM3DLib/StatisticsFilters/FindAvgOrientations.h"
#include "DREAM3DLib/StatisticsFilters/FindODF.h"
#include "DREAM3DLib/StatisticsFilters/FindMDF.h"

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
  MXADir::remove(UnitTest::FindNeighborTest::OutputFile);
  MXADir::remove(UnitTest::FindNeighborTest::VtkOutputFile);
}


void updateProgressAndMessage(const std::string &msg, int prog)
{
  std::cout << prog << "% - " << msg << std::endl;
}

std::string getH5EbsdFile()
{
  std::string s = UnitTest::DataDir + MXADir::Separator + "Small_IN100.h5ebsd";
  return s;
}

int getZStartIndex() { return 1; }
int getZEndIndex() { return 30; }
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
    filter->setFieldName(Ebsd::Ang::ImageQuality);
    filter->setFieldValue(120);
    filter->setFieldOperator(">");
    filters.push_back(filter);
  }
  {
    QualityMetricFilter::Pointer filter = QualityMetricFilter::New();
    filter->setFieldName(Ebsd::Ang::ConfidenceIndex);
    filter->setFieldValue(0.1f);
    filter->setFieldOperator(">");
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

void pipelineProgressMessage(const std::string &msg)
{
  std::cout << msg << std::endl;
}

void pipelineErrorMessage(const char* message)
{
  std::cout << "Error Message: " << message << std::endl;
}

void setErrorCondition(int err) {
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

  std::string m_OutputDirectory = MXADir::toNativeSeparators(UnitTest::FindNeighborTest::TestDir);
  MXADir::mkdir(m_OutputDirectory, true);

 // updateProgressAndMessage(("Loading Slices"), 10);
  LoadSlices::Pointer load_slices = LoadSlices::New();
  load_slices->setH5EbsdFile(getH5EbsdFile());
  load_slices->setRefFrameZDir(Ebsd::LowtoHigh);
  load_slices->setZStartIndex(getZStartIndex());
  load_slices->setZEndIndex(getZEndIndex());
  load_slices->setPhaseTypes(getPhaseTypes());
  load_slices->setQualityMetricFilters(getQualityMetricFilters());
  load_slices->setMisorientationTolerance(m_MisorientationTolerance);
  pipeline->pushBack(load_slices);

  AlignSections::Pointer align_sections = AlignSections::New();
  align_sections->setMisorientationTolerance(m_MisorientationTolerance);
  align_sections->setAlignmentMethod(DREAM3D::AlignmentMethod::OuterBoundary);
  pipeline->pushBack(align_sections);

  SegmentGrains::Pointer segment_grains = SegmentGrains::New();
  segment_grains->setMisorientationTolerance(m_MisorientationTolerance);
  pipeline->pushBack(segment_grains);

  DREAM3D_REQUIRE_EQUAL(true, segment_grains->doesPipelineContainFilterBeforeThis(LoadSlices::ClassName()));
  DREAM3D_REQUIRE_EQUAL(true, segment_grains->doesPipelineContainFilterBeforeThis(AlignSections::ClassName()));
  DREAM3D_REQUIRE_EQUAL(false, segment_grains->doesPipelineContainFilterBeforeThis(CleanupGrains::ClassName()));

  CleanupGrains::Pointer cleanup_grains = CleanupGrains::New();
  cleanup_grains->setMinAllowedGrainSize(m_MinAllowedGrainSize);
  cleanup_grains->setMinNumNeighbors(m_MinNumNeighbors);
  cleanup_grains->setMisorientationTolerance(m_MisorientationTolerance);
  pipeline->pushBack(cleanup_grains);

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
  iDataPtr = m->getFieldData(DREAM3D::EnsembleData::CrystalStructures).get();
  UInt32ArrayType* data = UInt32ArrayType::SafeObjectDownCast<IDataArray*, UInt32ArrayType*>(iDataPtr);
  m_CrystalStructure = data->GetValue(index);
#endif

  FindSizes::Pointer find_sizes = FindSizes::New();
  find_sizes->setDistributionType(DREAM3D::DistributionType::LogNormal);
  pipeline->pushBack(find_sizes);

  FindShapes::Pointer find_shapes = FindShapes::New();
  find_shapes->setDistributionType(DREAM3D::DistributionType::Beta);
  pipeline->pushBack(find_shapes);

  FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
  pipeline->pushBack(find_neighbors);

  FindAvgOrientations::Pointer find_avgorients = FindAvgOrientations::New();
  pipeline->pushBack(find_avgorients);

  FindODF::Pointer find_odf = FindODF::New();
  pipeline->pushBack(find_odf);

  FindMDF::Pointer find_mdf = FindMDF::New();
  pipeline->pushBack(find_mdf);

  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile(UnitTest::FindNeighborTest::OutputFile2);
  pipeline->pushBack(writer);

#if 0
  DataContainerReader::Pointer reader = DataContainerReader::New();
  reader->setInputFile(UnitTest::FindNeighborTest::OutputFile2);
  pipeline->pushBack(reader);
#endif

//  std::cout << "********* RUNNING PREFLIGHT **********************" << std::endl;
//  int err = pipeline->preflightPipeline();
//  DREAM3D_REQUIRE_EQUAL(err, 0);


  std::cout << "********* RUNNING PIPELINE **********************" << std::endl;
  DataContainer::Pointer m = DataContainer::New();
  pipeline->run();
  err = pipeline->getErrorCondition();
  DREAM3D_REQUIRE_EQUAL(err, 0);


}


void OtherTest()
{

  int err = 0;

  // Create a Vector to hold all the filters. Later on we will execute all the filter
  DataContainer::Pointer m = DataContainer::New();

  DataContainerReader::Pointer reader = DataContainerReader::New();
  reader->setInputFile("C:\\Users\\mjackson\\Desktop\\FindNeighborTest_Rewrite.h5");
  reader->setInputFile(UnitTest::FindNeighborTest::OutputFile);
  reader->setDataContainer(m.get());
//  reader->setReadCellData(false);
//  reader->setReadFieldData(false);
//  reader->setReadEnsembleData(true);

  DataArray<uint32_t>::Pointer grainIds = DataArray<uint32_t>::CreateArray(2, DREAM3D::EnsembleData::PhaseTypes);
  std::cout << "********* RUNNING PIPELINE **********************" << std::endl;
  reader->execute();
  err = reader->getErrorCondition();

  IDataArray::Pointer iPtr = m->getEnsembleData(DREAM3D::EnsembleData::CrystalStructures);
  DREAM3D_REQUIRE_NE(NULL, iPtr.get());


  DREAM3D_REQUIRE_EQUAL(err, 0);

}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
  int err = EXIT_SUCCESS;
#if !REMOVE_TEST_FILES
 // DREAM3D_REGISTER_TEST( RemoveTestFiles() );
#endif
//  DREAM3D_REGISTER_TEST( TestFindNeighbors() );
//  DREAM3D_REGISTER_TEST( TestDataContainerReader() );
  DREAM3D_REGISTER_TEST( OtherTest() );
#if REMOVE_TEST_FILES
//  DREAM3D_REGISTER_TEST( RemoveTestFiles() );
#endif
  PRINT_TEST_SUMMARY();
  return err;
}
