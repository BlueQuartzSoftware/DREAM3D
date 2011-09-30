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


#include "DREAM3DLib/VTKUtils/VTKFileWriters.hpp"
#include "Reconstruction/ReconstructionFunc.h"
#include "MicrostructureStatistics/MicrostructureStatisticsFunc.h"
#include "GrainGenerator/GrainGeneratorFunc.h"


int main(int argc, char **argv)
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
#endif

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

  return 0;
}

