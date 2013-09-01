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
#include <QtCore/QDir>
#include <QtCore/QFile>
#include "MXA/Utilities/MXAFileInfo.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Observer.h"
#include "DREAM3DLib/DREAM3DFilters.h"
#include "DREAM3DLib/SyntheticBuildingFilters/PackPrimaryPhases.h"
#include "DREAM3DLib/ReconstructionFilters/SegmentGrains.h"

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

  typedef DataArray<unsigned int> ShapeTypeArrayType;
  ShapeTypeArrayType::Pointer m_ShapeTypes = ShapeTypeArrayType::CreateArray(3, DREAM3D::EnsembleData::ShapeTypes);


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

  m_ShapeTypes->SetValue(0, DREAM3D::ShapeType::UnknownShapeType);
  m_ShapeTypes->SetValue(1, DREAM3D::ShapeType::SuperEllipsoidShape);
  m_ShapeTypes->SetValue(2, DREAM3D::ShapeType::SuperEllipsoidShape);

  int err = 0;
  // Instantiate our DataContainer object
  VoxelDataContainer::Pointer m = VoxelDataContainer::New();
  // Create a Vector to hold all the filters. Later on we will execute all the filters
  std::vector<AbstractFilter::Pointer> pipeline;

  if(m_AlreadyFormed == false)
  {
    m->setDimensions(m_XPoints, m_YPoints, m_ZPoints);
    m->setResolution(m_XResolution, m_YResolution, m_ZResolution);
    m->addEnsembleData(DREAM3D::EnsembleData::ShapeTypes, m_ShapeTypes);

    PackPrimaryPhases::Pointer pack_grains = PackPrimaryPhases::New();
    pack_grains->setPeriodicBoundaries(m_PeriodicBoundary);
    pack_grains->setNeighborhoodErrorWeight(m_NeighborhoodErrorWeight);
    pipeline.push_back(pack_grains);

  }
  else if(m_AlreadyFormed == true)
  {
    BOOST_ASSERT(false);
  }

  if(m_AlreadyFormed == false)
  {
    InsertPrecipitatePhases::Pointer place_precipitates = InsertPrecipitatePhases::New();
    place_precipitates->setPeriodicBoundaries(m_PeriodicBoundary);
    pipeline.push_back(place_precipitates);
  }

  MatchCrystallography::Pointer match_crystallography = MatchCrystallography::New();
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
    (*filter)->setVoxelDataContainer(m.get());
    setCurrentFilter(*filter);
    (*filter)->preflight();
    err = (*filter)->getErrorCondition();
    if(err < 0)
    {
      preflightError |= err;
      preflightMessageStream << (*filter)->getNameOfClass() << " produced the following preflight errors:" << std::endl;
      std::vector<PipelineMessage> ems = (*filter)->getPipelineMessages();
      for (std::vector<PipelineMessage>::iterator iter = ems.begin(); iter != ems.end(); ++iter )
      {
        preflightMessageStream << (*iter).generateErrorString();
      }
    }
  }

  DREAM3D_REQUIRE_EQUAL(err , 0);

}

#define PASS 0
#define FAIL_IS_PASS 1

#define MAKE_FILTER_TEST(name, condition)\
void name##_PreFlightTest() {\
  int err = 0;\
  VoxelDataContainer::Pointer m = VoxelDataContainer::New();\
  std::vector<AbstractFilter::Pointer> pipeline;\
  name::Pointer filter = name::New();\
  pipeline.push_back(filter);\
  int preflightError = 0;\
  std::stringstream ss;\
  ss << "------------------------------------------------" << std::endl;\
  ss << "Starting Preflight test for " << #name << std::endl;\
  for (std::vector<AbstractFilter::Pointer>::iterator filter = pipeline.begin(); filter != pipeline.end(); ++filter) {\
    (*filter)->setVoxelDataContainer(m.get());\
    setCurrentFilter(*filter);\
    (*filter)->preflight();\
    err = (*filter)->getErrorCondition();\
    if(err < 0) {\
      preflightError |= err;\
      ss << (*filter)->getNameOfClass() << " produced the following preflight errors:" << std::endl;\
      std::vector<PipelineMessage> ems = (*filter)->getPipelineMessages();\
      for (std::vector<PipelineMessage>::iterator iter = ems.begin(); iter != ems.end(); ++iter ) {\
        ss << (*iter).generateErrorString();\
      }\
    }\
  }\
  std::cout << ss.str() << std::endl;\
  if (condition) { DREAM3D_REQUIRE_NE(preflightError, 0);}\
  else { DREAM3D_REQUIRE_EQUAL(preflightError, 0);  }\
}


MAKE_FILTER_TEST(  FindNeighbors, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindGrainPhases, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindSurfaceCells, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindCellQuats, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindGrainCentroids, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindSurfaceGrains, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindBoundingBoxGrains, FAIL_IS_PASS)
MAKE_FILTER_TEST(  ReadH5Ebsd, FAIL_IS_PASS)
MAKE_FILTER_TEST(  DataContainerReader, FAIL_IS_PASS)
MAKE_FILTER_TEST(  DataContainerWriter, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FieldDataCSVWriter, FAIL_IS_PASS)
MAKE_FILTER_TEST(  VtkRectilinearGridWriter, FAIL_IS_PASS)
MAKE_FILTER_TEST(  H5VoxelFileReader, FAIL_IS_PASS)
MAKE_FILTER_TEST(  DxWriter, FAIL_IS_PASS)
MAKE_FILTER_TEST(  DxReader, FAIL_IS_PASS)
MAKE_FILTER_TEST(  PhWriter, FAIL_IS_PASS)
MAKE_FILTER_TEST(  PhReader, FAIL_IS_PASS)
MAKE_FILTER_TEST(  YSChoiAbaqusReader, FAIL_IS_PASS)
MAKE_FILTER_TEST(  EbsdToH5Ebsd, FAIL_IS_PASS)
MAKE_FILTER_TEST(  AvizoRectilinearCoordinateWriter, FAIL_IS_PASS)
MAKE_FILTER_TEST(  AvizoUniformCoordinateWriter, FAIL_IS_PASS)
MAKE_FILTER_TEST(  MinNeighbors, FAIL_IS_PASS)
MAKE_FILTER_TEST(  MinSize, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FillBadData, FAIL_IS_PASS)
MAKE_FILTER_TEST(  OpenCloseBadData, FAIL_IS_PASS)
MAKE_FILTER_TEST(  ConvertEulerAngles, FAIL_IS_PASS)
MAKE_FILTER_TEST(  AlignSectionsMisorientation, FAIL_IS_PASS)
MAKE_FILTER_TEST(  AlignSectionsMutualInformation, FAIL_IS_PASS)
MAKE_FILTER_TEST(  AlignSectionsFeature, FAIL_IS_PASS)
MAKE_FILTER_TEST(  AlignSectionsList, FAIL_IS_PASS)
MAKE_FILTER_TEST(  EBSDSegmentGrains, FAIL_IS_PASS)
MAKE_FILTER_TEST(  CAxisSegmentGrains, FAIL_IS_PASS)
MAKE_FILTER_TEST(  MergeTwins, FAIL_IS_PASS)
MAKE_FILTER_TEST(  MergeColonies, FAIL_IS_PASS)
MAKE_FILTER_TEST(  GroupMicroTextureRegions, FAIL_IS_PASS)
MAKE_FILTER_TEST(  ChangeResolution, FAIL_IS_PASS)
MAKE_FILTER_TEST(  RegularizeZSpacing, FAIL_IS_PASS)
MAKE_FILTER_TEST(  CropVolume, FAIL_IS_PASS)
MAKE_FILTER_TEST(  Hex2SqrConverter, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindAvgOrientations, FAIL_IS_PASS)
//MAKE_FILTER_TEST(  FindAxisODF, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindEuclideanDistMap, FAIL_IS_PASS)
//MAKE_FILTER_TEST(  FindMDF, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindNeighborhoods, FAIL_IS_PASS)
//MAKE_FILTER_TEST(  FindODF, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindNumFields, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindDeformationStatistics, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindSchmids, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindShapes, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindSizes, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindGrainReferenceMisorientations, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindKernelAvgMisorientations, FAIL_IS_PASS)
MAKE_FILTER_TEST(  FindSlicetoSliceRotations, FAIL_IS_PASS)
MAKE_FILTER_TEST(  PackPrimaryPhases, FAIL_IS_PASS)
MAKE_FILTER_TEST(  MatchCrystallography, FAIL_IS_PASS)
MAKE_FILTER_TEST(  InsertPrecipitatePhases, FAIL_IS_PASS)
MAKE_FILTER_TEST(  EstablishMatrixPhase, FAIL_IS_PASS)
MAKE_FILTER_TEST(  InitializeSyntheticVolume, FAIL_IS_PASS)




// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( FindNeighbors_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindGrainPhases_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindSurfaceCells_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindCellQuats_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindGrainCentroids_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindSurfaceGrains_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindBoundingBoxGrains_PreFlightTest() )
  DREAM3D_REGISTER_TEST( ReadH5Ebsd_PreFlightTest() )
  DREAM3D_REGISTER_TEST( DataContainerReader_PreFlightTest() )
  DREAM3D_REGISTER_TEST( DataContainerWriter_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FieldDataCSVWriter_PreFlightTest() )
  DREAM3D_REGISTER_TEST( VtkRectilinearGridWriter_PreFlightTest() )
  DREAM3D_REGISTER_TEST( H5VoxelFileReader_PreFlightTest() )
  DREAM3D_REGISTER_TEST( DxWriter_PreFlightTest() )
  DREAM3D_REGISTER_TEST( DxReader_PreFlightTest() )
  DREAM3D_REGISTER_TEST( PhWriter_PreFlightTest() )
  DREAM3D_REGISTER_TEST( PhReader_PreFlightTest() )
  DREAM3D_REGISTER_TEST( YSChoiAbaqusReader_PreFlightTest() )
  DREAM3D_REGISTER_TEST( EbsdToH5Ebsd_PreFlightTest() )
  DREAM3D_REGISTER_TEST( AvizoRectilinearCoordinateWriter_PreFlightTest() )
  DREAM3D_REGISTER_TEST( AvizoUniformCoordinateWriter_PreFlightTest() )
  DREAM3D_REGISTER_TEST( MinNeighbors_PreFlightTest() )
  DREAM3D_REGISTER_TEST( MinSize_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FillBadData_PreFlightTest() )
  DREAM3D_REGISTER_TEST( OpenCloseBadData_PreFlightTest() )
  DREAM3D_REGISTER_TEST( ConvertEulerAngles_PreFlightTest() )
  DREAM3D_REGISTER_TEST( AlignSectionsMisorientation_PreFlightTest() )
  DREAM3D_REGISTER_TEST( AlignSectionsMutualInformation_PreFlightTest() )
  DREAM3D_REGISTER_TEST( AlignSectionsFeature_PreFlightTest() )
  DREAM3D_REGISTER_TEST( AlignSectionsList_PreFlightTest() )
  DREAM3D_REGISTER_TEST( EBSDSegmentGrains_PreFlightTest() )
  DREAM3D_REGISTER_TEST( CAxisSegmentGrains_PreFlightTest() )
  DREAM3D_REGISTER_TEST( MergeTwins_PreFlightTest() )
  DREAM3D_REGISTER_TEST( MergeColonies_PreFlightTest() )
  DREAM3D_REGISTER_TEST( GroupMicroTextureRegions_PreFlightTest() )
  DREAM3D_REGISTER_TEST( ChangeResolution_PreFlightTest() )
  DREAM3D_REGISTER_TEST( RegularizeZSpacing_PreFlightTest() )
  DREAM3D_REGISTER_TEST( CropVolume_PreFlightTest() )
  DREAM3D_REGISTER_TEST( Hex2SqrConverter_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindAvgOrientations_PreFlightTest() )
  //DREAM3D_REGISTER_TEST( FindAxisODF_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindEuclideanDistMap_PreFlightTest() )
  //DREAM3D_REGISTER_TEST( FindMDF_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindNeighborhoods_PreFlightTest() )
  //DREAM3D_REGISTER_TEST( FindODF_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindNumFields_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindDeformationStatistics_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindSchmids_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindShapes_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindSizes_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindGrainReferenceMisorientations_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindKernelAvgMisorientations_PreFlightTest() )
  DREAM3D_REGISTER_TEST( FindSlicetoSliceRotations_PreFlightTest() )
  DREAM3D_REGISTER_TEST( PackPrimaryPhases_PreFlightTest() )
  DREAM3D_REGISTER_TEST( MatchCrystallography_PreFlightTest() )
  DREAM3D_REGISTER_TEST( InsertPrecipitatePhases_PreFlightTest() )
  DREAM3D_REGISTER_TEST( EstablishMatrixPhase_PreFlightTest() )
  DREAM3D_REGISTER_TEST( InitializeSyntheticVolume_PreFlightTest() )




  PRINT_TEST_SUMMARY();
  return err;
}

