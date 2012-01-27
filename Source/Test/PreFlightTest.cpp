/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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


#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/MXAFileInfo.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractPipeline.h"
#include "DREAM3DLib/Common/Observer.h"
#include "DREAM3DLib/DREAM3DFilters.h"

#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string getH5StatsFile()
{
  std::string s = UnitTest::DataDir + MXADir::Separator + "Equiaxed_Precip.h5stats";
  return s;
}

void setCurrentFilter(AbstractFilter::Pointer f)
{

}

void pipelineFinished()
{

}

void setErrorCondition(int err)
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES

#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SyntheticBuilder_PreFlight()
{

  std::string m_H5StatsFile("");
  std::string m_OutputDirectory = UnitTest::SyntheticBuilderTest::TestDir;
  std::string m_OutputFilePrefix("");
  size_t m_XPoints = 128;
  size_t m_YPoints = 128;
  size_t m_ZPoints = 128;

  typedef DataArray<DREAM3D::SyntheticBuilder::ShapeType> ShapeTypeArrayType;
  ShapeTypeArrayType::Pointer m_ShapeTypes = ShapeTypeArrayType::CreateArray(3);


  float m_XResolution = 0.1f;
  float m_YResolution = 0.1f;
  float m_ZResolution = 0.1f;
  double m_NeighborhoodErrorWeight = 1.0f;
//  double m_FractionPrecipitates = 0.0f;

  bool m_PeriodicBoundary = false;
  std::string m_StructureFile("");
  bool m_AlreadyFormed = false;

//  int m_Precipitates = 0;
//
//  bool m_WriteBinaryVTKFiles = true;
//  bool m_WriteVtkFile = true;
//  bool m_WriteSurfaceVoxel = true;
//  bool m_WritePhaseId = true;
//  bool m_WriteIPFColor = true;
//
//  bool m_WriteHDF5GrainFile = false;

  m_ShapeTypes->SetValue(0, DREAM3D::SyntheticBuilder::UnknownShapeType);
  m_ShapeTypes->SetValue(1, DREAM3D::SyntheticBuilder::SuperEllipsoidShape);
  m_ShapeTypes->SetValue(2, DREAM3D::SyntheticBuilder::SuperEllipsoidShape);

  int err = 0;
  // Instantiate our DataContainer object
  DataContainer::Pointer m = DataContainer::New();
  // Create a Vector to hold all the filters. Later on we will execute all the filters
  std::vector<AbstractFilter::Pointer> pipeline;

  if(m_AlreadyFormed == false)
  {
    m->setDimensions(m_XPoints, m_YPoints, m_ZPoints);
    m->setResolution(m_XResolution, m_YResolution, m_ZResolution);
    m->addEnsembleData(DREAM3D::EnsembleData::ShapeTypes, m_ShapeTypes);

    PackGrainsGen2::Pointer pack_grains = PackGrainsGen2::New();
    pack_grains->setH5StatsInputFile(getH5StatsFile());
    pack_grains->setPeriodicBoundaries(m_PeriodicBoundary);
    pack_grains->setNeighborhoodErrorWeight(m_NeighborhoodErrorWeight);
    pipeline.push_back(pack_grains);

    AdjustVolume::Pointer adjust_grains = AdjustVolume::New();
    pipeline.push_back(adjust_grains);
  }
  else if(m_AlreadyFormed == true)
  {
    assert(false);
  }

  if(m_AlreadyFormed == false)
  {
    PlacePrecipitates::Pointer place_precipitates = PlacePrecipitates::New();
    place_precipitates->setH5StatsInputFile(getH5StatsFile());
    place_precipitates->setPeriodicBoundaries(m_PeriodicBoundary);
    pipeline.push_back(place_precipitates);
  }

  MatchCrystallography::Pointer match_crystallography = MatchCrystallography::New();
  match_crystallography->setH5StatsInputFile(getH5StatsFile());
  pipeline.push_back(match_crystallography);

  MAKE_OUTPUT_FILE_PATH( FieldDataFile, DREAM3D::SyntheticBuilder::GrainDataFile)
  FieldDataCSVWriter::Pointer write_fielddata = FieldDataCSVWriter::New();
  write_fielddata->setFieldDataFile(FieldDataFile);
  pipeline.push_back(write_fielddata);

  int preflightError = 0;
  std::stringstream preflightMessageStream;
  // Start looping through the Pipeline and preflight everything

  for (std::vector<AbstractFilter::Pointer>::iterator filter = pipeline.begin(); filter != pipeline.end(); ++filter)
  {
    (*filter)->setDataContainer(m.get());
    setCurrentFilter(*filter);
    (*filter)->preflight();
    err = (*filter)->getErrorCondition();
    if(err < 0)
    {
      preflightError |= err;
      preflightMessageStream << (*filter)->getNameOfClass() << " produced the following preflight errors:" << std::endl;
      preflightMessageStream << (*filter)->getErrorMessage() << std::endl;
    }
  }

  DREAM3D_REQUIRE_EQUAL(err , 0);

}

#define PASS 0
#define FAIL_IS_PASS 1

#define MAKE_FILTER_TEST(name, condition)\
void name##_PreFlightTest() {\
  int err = 0;\
  DataContainer::Pointer m = DataContainer::New();\
  std::vector<AbstractFilter::Pointer> pipeline;\
  name::Pointer filter = name::New();\
  pipeline.push_back(filter);\
  int preflightError = 0;\
  std::stringstream ss;\
  ss << "------------------------------------------------" << std::endl;\
  ss << "Starting Preflight test for " << #name << std::endl;\
  for (std::vector<AbstractFilter::Pointer>::iterator filter = pipeline.begin(); filter != pipeline.end(); ++filter) {\
    (*filter)->setDataContainer(m.get());\
    setCurrentFilter(*filter);\
    (*filter)->preflight();\
    err = (*filter)->getErrorCondition();\
    if(err < 0) {\
      preflightError |= err;\
      ss << (*filter)->getNameOfClass() << " produced the following preflight errors:" << std::endl;\
      ss << (*filter)->getErrorMessage();\
    }\
  }\
  std::cout << ss.str() << std::endl;\
  if (condition) { DREAM3D_REQUIRE_NE(preflightError, 0);}\
  else { DREAM3D_REQUIRE_EQUAL(preflightError, 0);  }\
}


MAKE_FILTER_TEST(  LoadSlices, PASS)
MAKE_FILTER_TEST(  AlignSections, FAIL_IS_PASS)
MAKE_FILTER_TEST(  SegmentGrains, FAIL_IS_PASS)
MAKE_FILTER_TEST(  CleanupGrains, FAIL_IS_PASS)
MAKE_FILTER_TEST(  MergeTwins, FAIL_IS_PASS)
MAKE_FILTER_TEST(  MergeColonies, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindNeighbors, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FieldDataCSVWriter, PASS)
MAKE_FILTER_TEST(  ChangeResolution, FAIL_IS_PASS)
MAKE_FILTER_TEST(  CropVolume, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindAvgOrientations, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindAxisODF, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindBoundingBoxGrains, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindEuclideanDistMap, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindMDF, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindNeighborhoods, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindODF, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindDeformationStatistics, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindSchmids, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindShapes, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindSizes, FAIL_IS_PASS)
MAKE_FILTER_TEST(  LoadVolume, PASS)
MAKE_FILTER_TEST(  FindLocalMisorientationGradients, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindSurfaceGrains, FAIL_IS_PASS)
MAKE_FILTER_TEST(  WriteH5StatsFile, FAIL_IS_PASS)
MAKE_FILTER_TEST(  AdjustVolume, FAIL_IS_PASS)
MAKE_FILTER_TEST(  PackGrainsGen2, PASS)
MAKE_FILTER_TEST(  MatchCrystallography, FAIL_IS_PASS)
MAKE_FILTER_TEST(  PlacePrecipitates, FAIL_IS_PASS)
MAKE_FILTER_TEST(  SurfaceMeshFilter, FAIL_IS_PASS)



// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( SyntheticBuilder_PreFlight() );
  DREAM3D_REGISTER_TEST( LoadSlices_PreFlightTest() );
  DREAM3D_REGISTER_TEST( AlignSections_PreFlightTest() );
  DREAM3D_REGISTER_TEST( SegmentGrains_PreFlightTest() );
  DREAM3D_REGISTER_TEST( CleanupGrains_PreFlightTest() );
  DREAM3D_REGISTER_TEST( MergeTwins_PreFlightTest() );
  DREAM3D_REGISTER_TEST( MergeColonies_PreFlightTest() );
  DREAM3D_REGISTER_TEST( FindNeighbors_PreFlightTest() );
  DREAM3D_REGISTER_TEST( FieldDataCSVWriter_PreFlightTest() );
  DREAM3D_REGISTER_TEST( ChangeResolution_PreFlightTest() );
  DREAM3D_REGISTER_TEST( CropVolume_PreFlightTest() );
  DREAM3D_REGISTER_TEST( FindAvgOrientations_PreFlightTest() );
  DREAM3D_REGISTER_TEST( FindAxisODF_PreFlightTest() );
  DREAM3D_REGISTER_TEST( FindBoundingBoxGrains_PreFlightTest() );
  DREAM3D_REGISTER_TEST( FindEuclideanDistMap_PreFlightTest() );
  DREAM3D_REGISTER_TEST( FindMDF_PreFlightTest() );
  DREAM3D_REGISTER_TEST( FindNeighborhoods_PreFlightTest() );
  DREAM3D_REGISTER_TEST( FindODF_PreFlightTest() );
  DREAM3D_REGISTER_TEST( FindDeformationStatistics_PreFlightTest() );
  DREAM3D_REGISTER_TEST( FindSchmids_PreFlightTest() );
  DREAM3D_REGISTER_TEST( FindShapes_PreFlightTest() );
  DREAM3D_REGISTER_TEST( FindSizes_PreFlightTest() );
  DREAM3D_REGISTER_TEST( LoadVolume_PreFlightTest() );
  DREAM3D_REGISTER_TEST( FindLocalMisorientationGradients_PreFlightTest() );
  DREAM3D_REGISTER_TEST( FindSurfaceGrains_PreFlightTest() );
  DREAM3D_REGISTER_TEST( WriteH5StatsFile_PreFlightTest() );
  DREAM3D_REGISTER_TEST( AdjustVolume_PreFlightTest() );
  DREAM3D_REGISTER_TEST( PackGrainsGen2_PreFlightTest() );
  DREAM3D_REGISTER_TEST( MatchCrystallography_PreFlightTest() );
  DREAM3D_REGISTER_TEST( PlacePrecipitates_PreFlightTest() );
  DREAM3D_REGISTER_TEST( SurfaceMeshFilter_PreFlightTest() );


  PRINT_TEST_SUMMARY();
  return err;
}

