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

#include <stdio.h>

#include <iostream>
#include <vector>
#include <string>


#include "DREAM3D/Common/VTKUtils/VTKFileWriters.hpp"
#include "DREAM3D/Reconstruction/ReconstructionFunc.h"
#include "DREAM3D/MicrostructureStatistics/MicrostructureStatisticsFunc.h"
#include "DREAM3D/GrainGenerator/GrainGeneratorFunc.h"

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class ScalarWrapper
{
  public:
  ScalarWrapper()
  {}
  ~ScalarWrapper()
  {}

  virtual int writeScalars(FILE* f)
  {
    int err = -1;

    return err;
  }

  protected:

  private:

  ScalarWrapper(const ScalarWrapper&); // Copy Constructor Not Implemented
  void operator=(const ScalarWrapper&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
class GrainIdWriter : public ScalarWrapper
{
  public:
  GrainIdWriter(T* r) : r(r)
  {}
  ~GrainIdWriter()
  {}

  int writeScalars(FILE* f)
  {
    int err = -1;
    std::cout << "GrainIdWriter..." << std::endl;
    std::string file;
    size_t total = r->xpoints * r->ypoints * r->zpoints;
    WRITE_VTK_GRAIN_IDS_BINARY(r, AIM::VTK::GrainIdScalarName, voxels);
    return err;
  }

  private:
  T* r;
  GrainIdWriter(const GrainIdWriter&); // Copy Constructor Not Implemented
  void operator=(const GrainIdWriter&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
class PhaseIdWriter : public ScalarWrapper
{
  public:
  PhaseIdWriter(T* f) : r(f)
  {}
  ~PhaseIdWriter()
  {}

  int writeScalars(FILE* f)
  {
    int err = -1;
    std::cout << "PhaseIdWriter..." << std::endl;
    return err;
  }

  private:
  T* r;
  PhaseIdWriter(const PhaseIdWriter&); // Copy Constructor Not Implemented
  void operator=(const PhaseIdWriter&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class VTKFileWriter
{
  public:
  VTKFileWriter() : m_WriteBinaryFiles(false)
  {}
  ~VTKFileWriter()
  {}

  template<typename T>
  int write(const std::string &file, T* r, std::vector<ScalarWrapper*> scalars)
  {
    int err = 0;
    FILE* f = NULL;
    f = fopen(file.c_str(), "wb");
    if (NULL == f)
    {
      return 1;
    }
    // Write the correct header
    if (m_WriteBinaryFiles == true)
    {
      WRITE_RECTILINEAR_GRID_HEADER("BINARY", r, r->xpoints + 1, r->ypoints+1, r->zpoints+1)
    }
    else
    {
      WRITE_RECTILINEAR_GRID_HEADER("ASCII", r, r->xpoints + 1, r->ypoints+1, r->zpoints+1)
    }

    for (typename std::vector<ScalarWrapper*>::iterator iter = scalars.begin(); iter != scalars.end(); ++iter )
    {
      (*iter)->writeScalars(f);
    }

    fclose(f);
    return err;
  }

  bool m_WriteBinaryFiles;

  private:
  VTKFileWriter(const VTKFileWriter&); // Copy Constructor Not Implemented
  void operator=(const VTKFileWriter&); // Operator '=' Not Implemented
};

#endif



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

//  ScalarWrapper* gidWriter
//      = static_cast<ScalarWrapper*>(new GrainIdWriter<ReconstructionFunc>(r.get()) );
//
//  ScalarWrapper* pidWriter
//      = static_cast<ScalarWrapper*>(new PhaseIdWriter<ReconstructionFunc>(r.get()) );

  VtkScalarWriter* w0 =
      static_cast<VtkScalarWriter*>(new VoxelGrainIdScalarWriter<ReconstructionFunc>(r.get()));
  VtkScalarWriter* w1 =
      static_cast<VtkScalarWriter*>(new VoxelPhaseIdScalarWriter<ReconstructionFunc>(r.get()));
  VtkScalarWriter* w2 =
      static_cast<VtkScalarWriter*>(new VoxelEuclideanScalarWriter<ReconstructionFunc>(r.get()));
  VtkScalarWriter* w3 =
      static_cast<VtkScalarWriter*>(new VoxelImageQualityScalarWriter<ReconstructionFunc>(r.get()));
  VtkScalarWriter* w4 =
      static_cast<VtkScalarWriter*>(new VoxelSurfaceVoxelScalarWriter<ReconstructionFunc>(r.get()));
//  VtkScalarWrapper* w5 =
//      static_cast<VtkScalarWrapper*>(new VoxelKAMScalarWriter<ReconstructionFunc>(r.get()));

  std::vector<VtkScalarWriter*> scalarsToWrite;
  scalarsToWrite.push_back(w0);
  scalarsToWrite.push_back(w1);
  scalarsToWrite.push_back(w2);
  scalarsToWrite.push_back(w3);
  scalarsToWrite.push_back(w4);
//  scalarsToWrite.push_back(w5);

  VTKRectilinearGridFileWriter writer;
  writer.write<ReconstructionFunc>("/tmp/vtkfilewriterstest.vtk", r.get(), scalarsToWrite);

  return 0;
}

