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

#include <iostream>
#include <string>

#include "MXA/Common/LogTime.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include "MXA/Utilities/MXAFileInfo.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractPipeline.h"
#include "SIMPLib/Common/Observer.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/VTKUtils/VTKFileWriters.hpp"
//#include "SIMPLib/HDF5/H5VoxelReader.h"
#include "SIMPLib/IOFilters/DataContainerWriter.h"
#include "SIMPLib/IOFilters/VtkRectilinearGridWriter.h"
#include "SIMPLib/SyntheticBuilderFilters/InitializeSyntheticVolume.h"
#include "SIMPLib/SyntheticBuilderFilters/MatchCrystallography.h"
#include "SIMPLib/SyntheticBuilderFilters/InsertPrecipitatePhases.h"
#include "SIMPLib/SyntheticBuilderFilters/InitializeSyntheticVolume.h"
#include "SIMPLib/SyntheticBuilderFilters/PackPrimaryPhases.h"
#include "SIMPLib/SyntheticBuilderFilters/AdjustVolume.h"
#include "SIMPLib/IOFilters/FeatureDataCSVWriter.h"

#include "UnitTestSupport.hpp"

#include "TestFileLocations.h"

#define PACK_GRAINS_ERROR_TXT_OUT 1
#define PACK_GRAINS_VTK_FILE_OUT 1



QString m_H5StatsFile("");
QString m_OutputDirectory = UnitTest::SyntheticBuilderTest::TestDir;
QString m_OutputFilePrefix("");
size_t m_XPoints = 150;
size_t m_YPoints = 150;
size_t m_ZPoints = 150;

typedef DataArray<unsigned int> ShapeTypeArrayType;


float m_XResolution = 0.25f;
float m_YResolution = 0.25f;
float m_ZResolution = 0.25f;
double m_NeighborhoodErrorWeight = 1.0f;
double m_FractionPrecipitates = 0.0f;

bool m_PeriodicBoundary = false;
QString m_StructureFile("");
bool m_AlreadyFormed = false;

int m_Precipitates = 0;

bool m_WriteBinaryVTKFiles = true;
bool m_WriteVtkFile = true;
bool m_WriteSurfaceVoxel = true;
bool m_WritePhaseId = true;
bool m_WriteIPFColor = true;

bool m_WriteHDF5GrainFile = false;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString getH5StatsFile()
{
  QString s = UnitTest::DataDir + MXADir::Separator + "3Phase.h5";
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
  QFile::remove(UnitTest::SyntheticBuilderTest::CsvFile);
  QFile::remove(UnitTest::SyntheticBuilderTest::OutputFile);
  QFile::remove(UnitTest::SyntheticBuilderTest::VtkOutputFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestSyntheticBuilder()
{

  QDir dir(m_OutputDirectory);
  dir.mkpath(".");

  Observer* observer = new Observer;


  int err = 0;

  // Create our Pipeline object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  VoxelDataContainer::Pointer m = VoxelDataContainer::New();
  pipeline->setVoxelDataContainer(m);

  ShapeTypeArrayType::Pointer m_ShapeTypes = ShapeTypeArrayType::CreateArray(4, DREAM3D::EnsembleData::ShapeTypes);
  m_ShapeTypes->SetValue(0, DREAM3D::ShapeType::UnknownShapeType);
  m_ShapeTypes->SetValue(1, DREAM3D::ShapeType::EllipsoidShape);
  m_ShapeTypes->SetValue(2, DREAM3D::ShapeType::EllipsoidShape);
  m_ShapeTypes->SetValue(3, DREAM3D::ShapeType::EllipsoidShape);

  InitializeSyntheticVolume::Pointer init_volume = InitializeSyntheticVolume::New();
  init_volume->setShapeTypes(m_ShapeTypes);
  init_volume->setInputFile(getH5StatsFile());
  init_volume->setXVoxels(m_XPoints);
  init_volume->setYVoxels(m_YPoints);
  init_volume->setZVoxels(m_ZPoints);
  init_volume->setXRes(m_XResolution);
  init_volume->setYRes(m_YResolution);
  init_volume->setZRes(m_ZResolution);
  pipeline->pushBack(init_volume);

  PackPrimaryPhases::Pointer pack_grains = PackPrimaryPhases::New();
  pack_grains->setPeriodicBoundaries(m_PeriodicBoundary);
  pack_grains->setNeighborhoodErrorWeight(m_NeighborhoodErrorWeight);
#if PACK_GRAINS_ERROR_TXT_OUT
  MAKE_OUTPUT_FILE_PATH( errorFile, DREAM3D::SyntheticBuilder::ErrorFile)
  pack_grains->setErrorOutputFile(errorFile);
#endif
#if PACK_GRAINS_VTK_FILE_OUT
  MAKE_OUTPUT_FILE_PATH( vtkFile, DREAM3D::SyntheticBuilder::VtkFile)
  pack_grains->setVtkOutputFile(vtkFile);
#endif
  pipeline->pushBack(pack_grains);

  AdjustVolume::Pointer adjust_grains = AdjustVolume::New();
  pipeline->pushBack(adjust_grains);

  InsertPrecipitatePhases::Pointer place_precipitates = InsertPrecipitatePhases::New();
  place_precipitates->setPeriodicBoundaries(m_PeriodicBoundary);
  pipeline->pushBack(place_precipitates);

  MatchCrystallography::Pointer match_crystallography = MatchCrystallography::New();
//  pipeline->pushBack(match_crystallography);

  FeatureDataCSVWriter::Pointer write_featuredata = FeatureDataCSVWriter::New();
  write_featuredata->setFeatureDataFile(UnitTest::SyntheticBuilderTest::CsvFile);
  pipeline->pushBack(write_featuredata);

  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile(UnitTest::SyntheticBuilderTest::OutputFile);
  pipeline->pushBack(writer);

  bool m_WriteVtkFile(true);
  bool m_WriteBinaryVTKFiles(true);
  bool m_WritePhaseId(true);
  bool m_WriteIPFColor(false);

  if(m_WriteVtkFile)
  {
    VtkRectilinearGridWriter::Pointer vtkWriter = VtkRectilinearGridWriter::New();
    vtkWriter->setOutputFile(UnitTest::SyntheticBuilderTest::VtkOutputFile);
    vtkWriter->setWriteGrainIds(true);
    vtkWriter->setWritePhaseIds(m_WritePhaseId);
    // vtkWriter->setWriteGoodVoxels(m_WriteGoodVoxels);
    vtkWriter->setWriteIPFColors(m_WriteIPFColor);
    vtkWriter->setWriteBinaryFile(m_WriteBinaryVTKFiles);
    pipeline->pushBack(vtkWriter);
  }

  std::cout << "********* RUNNING PIPELINE **********************" << std::endl;

  m = VoxelDataContainer::New();

  pipeline->setVoxelDataContainer(m);
  pipeline->run();
  err = pipeline->getErrorCondition();
  DREAM3D_REQUIRE_EQUAL(err, 0);

  DREAM3D_REQUIRE_EQUAL(false, pipeline->empty());

  pipeline->clear();
  DREAM3D_REQUIRE_EQUAL(0, pipeline->size());
  DREAM3D_REQUIRE_EQUAL(true, pipeline->empty());

  delete observer;
}



// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;

#if !REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif

  DREAM3D_REGISTER_TEST( TestSyntheticBuilder() )

#if REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif


  PRINT_TEST_SUMMARY();
  return err;
}




