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
#include "GrainGeneratorVTKWriter.h"

#include "AIM/Common/OIMColoring.hpp"
#include "AIM/Common/VTKWriterMacros.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGeneratorVTKWriter::GrainGeneratorVTKWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGeneratorVTKWriter::~GrainGeneratorVTKWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GrainGeneratorVTKWriter::writeVisualizationFile(GrainGeneratorFunc* r, const std::string &file)
{
  bool bin = false;
#if AIM_WRITE_BINARY_VTK_FILE
  bin = true;
#endif

  FILE* f = NULL;
  f = fopen(file.c_str(), "wb");
  if (NULL == f)
  {
    return 1;
  }
  // Write the correct header
  if (bin == true)
  {
    WRITE_VTK_GRAIN_HEADER("BINARY", r)
  }
  else
  {
    WRITE_VTK_GRAIN_HEADER("ASCII", r)
  }
  size_t total = r->xpoints * r->ypoints * r->zpoints;

  if (true == bin)
  {
    WRITE_VTK_GRAIN_IDS_BINARY(r, AIM::Reconstruction::GrainIdScalarName, voxels);
    WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, SurfaceVoxel, int, voxels, surfacevoxel)
    WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, PhaseID, int, voxels, phase)
  }
  else
  {
    WRITE_VTK_GRAIN_IDS_ASCII(r, AIM::Reconstruction::GrainIdScalarName, voxels)
    WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, SurfaceVoxel, int, voxels, surfacevoxel, "%d ")
    WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, PhaseID, int, voxels, unassigned, "%d ")
  }
  fclose(f);
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GrainGeneratorVTKWriter::writeIPFVizFile(GrainGeneratorFunc* r, const std::string &file)
{
  bool bin = false;
#if AIM_WRITE_BINARY_VTK_FILE
  bin = true;
#endif
  FILE* f = NULL;
  f = fopen(file.c_str(), "wb");
  if (NULL == f)
  {
    return 1;
  }
  // Write the correct header
  if (bin == true)
  {
    WRITE_VTK_GRAIN_HEADER("BINARY", r)
  }
  else
  {
    WRITE_VTK_GRAIN_HEADER("ASCII", r)
  }
  size_t total = r->xpoints * r->ypoints * r->zpoints;
  float q1[5];
  unsigned char hkl[3] = { 0, 0, 0 };
  float RefDirection[3] = { 1.0, 0.0, 0.0 };
  int phase;

  if (true == bin)
  {
    WRITE_VTK_GRAIN_IDS_BINARY(r, AIM::Reconstruction::GrainIdScalarName, voxels);
    WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, Phase, int, voxels, phase)
    // Write the COLOR_SCALARS
    fprintf(f, "COLOR_SCALARS IPF_Colors 4\n");
    // Allocate our RGBA array
    unsigned char* rgba = new unsigned char[total * 4];
    for (size_t i = 0; i < total; i++)
    {
      phase = r->voxels[i].phase;
      if (r->crystruct[phase] == AIM::Reconstruction::Cubic)
      {
        OIMColoring::GenerateIPFColor(r->voxels[i].euler1, r->voxels[i].euler2, r->voxels[i].euler3, RefDirection[0], RefDirection[1], RefDirection[2], &rgba[i * 4], hkl);
      }
      else if (r->crystruct[phase] == AIM::Reconstruction::Hexagonal)
      {
        q1[1] = r->voxels[i].quat[1];
        q1[2] = r->voxels[i].quat[2];
        q1[3] = r->voxels[i].quat[3];
        q1[4] = r->voxels[i].quat[4];
        OIMColoring::CalculateHexIPFColor(q1, RefDirection[0], RefDirection[1], RefDirection[2], &rgba[i * 4]);
      }
      rgba[i * 4 + 3] = 255;
    }
    size_t totalWritten = fwrite(rgba, sizeof(char), total * 4, f);
    delete[] rgba;
    if (totalWritten != total * 4)
    {
      std::cout << "Error Writing Binary Data for IPF Colors to file " << file << std::endl;
      fclose(f);
      return -1;
    }
  }
  else
  {
    WRITE_VTK_GRAIN_IDS_ASCII(r, AIM::Reconstruction::GrainIdScalarName, voxels);
    WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, Phase, int, voxels, phase, "%d ")
    // Write the COLOR_SCALARS
    fprintf(f, "COLOR_SCALARS IPF_Colors 3\n");
    float red, green, blue;
    unsigned char rgb[3] = { 0, 0, 0 };
    for (size_t i = 0; i < total; i++)
    {
      phase = r->voxels[i].phase;
      if (r->crystruct[phase] == AIM::Reconstruction::Cubic)
      {
        OIMColoring::GenerateIPFColor(r->voxels[i].euler1, r->voxels[i].euler2, r->voxels[i].euler3, RefDirection[0], RefDirection[1], RefDirection[2], rgb, hkl);
      }
      else if (r->crystruct[phase] == AIM::Reconstruction::Hexagonal)
      {
        q1[1] = r->voxels[i].quat[1];
        q1[2] = r->voxels[i].quat[2];
        q1[3] = r->voxels[i].quat[3];
        q1[4] = r->voxels[i].quat[4];
        OIMColoring::CalculateHexIPFColor(q1, RefDirection[0], RefDirection[1], RefDirection[2], rgb);
      }
      red = static_cast<float> (float(rgb[0]) / 255.0);
      green = static_cast<float> (float(rgb[1]) / 255.0);
      blue = static_cast<float> (float(rgb[2]) / 255.0);
      fprintf(f, "%f %f %f\n", red, green, blue);
    }
  }

  fclose(f);
  return 0;
}
