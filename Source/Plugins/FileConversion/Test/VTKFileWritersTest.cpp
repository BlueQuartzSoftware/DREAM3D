/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#include <stdio.h>

#include <iostream>
#include <vector>
#include <string>

#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "FileConversion/GrainIdWriter.h"
#include "FileConversion/VtkGrainIdReader.h"

#include "Test/UnitTestSupport.hpp"

#define REMOVE_TEST_FILES 0

namespace Detail
{
  static const std::string TestFile("VtkIOTest.vtk");
  static const int XSize = 2;
  static const int YSize = 10;
  static const int ZSize = 5;
  static const int Offset = 66;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  MXADir::remove(Detail::TestFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestVtkGrainIdWriter()
{
  int size = Detail::XSize * Detail::YSize * Detail::ZSize;
  AIMArray<int>::Pointer grainIds = AIMArray<int>::CreateArray(size);
  for (int i = 0; i < size; ++i)
  {
    grainIds->SetValue(i, i + Detail::Offset);
  }
  int nx = Detail::XSize;
  int ny = Detail::YSize;
  int nz = Detail::ZSize;


  VtkGrainIdWriter::Pointer writer = VtkGrainIdWriter::New();
  writer->setWriteBinaryFiles(true);
  writer->setFileName(Detail::TestFile);
  writer->setDimensions(nx, ny, nz);
  writer->setGrainIds(grainIds);
  int err = writer->writeGrainIds();
  DREAM3D_REQUIRE_EQUAL(err, 0);
  return EXIT_SUCCESS;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestVtkGrainIdReader()
{

  VtkGrainIdReader::Pointer reader = VtkGrainIdReader::New();
  int nx = 0;
  int ny = 0;
  int nz = 0;

  reader->setFileName(Detail::TestFile);
  int err = reader->readGrainIds();

  DREAM3D_REQUIRE_EQUAL(err, 0);

  reader->getDimensions(nx, ny, nz);
  DREAM3D_REQUIRE_EQUAL(nx, Detail::XSize);
  DREAM3D_REQUIRE_EQUAL(ny, Detail::YSize);
  DREAM3D_REQUIRE_EQUAL(nz, Detail::ZSize);

  AIMArray<int>::Pointer grainIds = reader->getGrainIds();
  DREAM3D_REQUIRE(NULL != grainIds.get());

  int size = Detail::XSize * Detail::YSize * Detail::ZSize;

  for (int i = 0; i < size; ++i)
  {
    DREAM3D_REQUIRE_EQUAL( (i+Detail::Offset), grainIds->GetValue(i) );
  }


  return EXIT_SUCCESS;
}


int TestVtkWriters()
{
#if 0
  VtkMiscFileWriter::Pointer vtkWriter = VtkMiscFileWriter::New();

  MicrostructureStatisticsFunc::Pointer m = MicrostructureStatisticsFunc::New();
  vtkWriter->writeDisorientationFile(m.get(), "out");
  vtkWriter->writeSchmidFactorVizFile(m.get(), "out");

  ReconstructionFunc::Pointer r = ReconstructionFunc::New();
  r->xpoints = 5;
  r->ypoints = 4;
  r->zpoints = 3;
  r->resx = 0.35;
  r->resy = 0.50;
  r->resz = 0.75;

  vtkWriter->setWriteBinaryFiles(true);
  vtkWriter->writeVisualizationFile(r.get(), "out");
  vtkWriter->writeIPFVizFile(r.get(), "out");
  vtkWriter->writeImageQualityVizFile(r.get(), "out");
  vtkWriter->writeDownSampledVizFile(r.get(), "out");


  ReconstructionFunc::Pointer r = ReconstructionFunc::New();
  r->xpoints = 5;
  r->ypoints = 4;
  r->zpoints = 3;
  r->resx = 0.35;
  r->resy = 0.50;
  r->resz = 0.75;

  std::vector<Ebsd::CrystalStructure> crystalStructures(2);
  std::vector<DREAM3D::Reconstruction::PhaseType> phaseTypes(2);
  std::vector<float> precipFractions(2);

  r->initialize(5,5,5, 0.35, 0.35, 0.55, false, false, 25, 1.0, 5.0, crystalStructures , phaseTypes ,precipFractions ,0);

  for (size_t i = 0; i < 5*5*5; ++i)
  {
    r->surfacevoxels[i] = 0;
  }


//  ScalarWrapper* gidWriter
//      = static_cast<ScalarWrapper*>(new GrainIdWriter<ReconstructionFunc>(r.get()) );
//
//  ScalarWrapper* pidWriter
//      = static_cast<ScalarWrapper*>(new PhaseIdWriter<ReconstructionFunc>(r.get()) );

  VtkScalarWriter* w0 =
      static_cast<VtkScalarWriter*>(new VoxelGrainIdScalarWriter<ReconstructionFunc>(r.get()));
  VtkScalarWriter* w1 =
      static_cast<VtkScalarWriter*>(new VoxelPhaseIdScalarWriter<ReconstructionFunc>(r.get()));
//  VtkScalarWriter* w2 =
//      static_cast<VtkScalarWriter*>(new VoxelEuclideanScalarWriter<ReconstructionFunc>(r.get()));
//  VtkScalarWriter* w3 =
//      static_cast<VtkScalarWriter*>(new VoxelImageQualityScalarWriter<ReconstructionFunc>(r.get()));
  VtkScalarWriter* w4 =
      static_cast<VtkScalarWriter*>(new VoxelSurfaceVoxelScalarWriter<ReconstructionFunc>(r.get()));
//  VtkScalarWrapper* w5 =
//      static_cast<VtkScalarWrapper*>(new VoxelKAMScalarWriter<ReconstructionFunc>(r.get()));

  std::vector<VtkScalarWriter*> scalarsToWrite;
  scalarsToWrite.push_back(w0);
  scalarsToWrite.push_back(w1);
//  scalarsToWrite.push_back(w2);
//  scalarsToWrite.push_back(w3);
  scalarsToWrite.push_back(w4);
//  scalarsToWrite.push_back(w5);

  VTKRectilinearGridFileWriter writer;
  writer.write<ReconstructionFunc>("vtkfilewriterstest.vtk", r.get(), scalarsToWrite);
#endif
  return 0;
}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( TestVtkGrainIdWriter() );
  DREAM3D_REGISTER_TEST( TestVtkGrainIdReader() );
  DREAM3D_REGISTER_TEST( TestVtkWriters() );
  DREAM3D_REGISTER_TEST( RemoveTestFiles() );
  PRINT_TEST_SUMMARY();
  return err;
}
