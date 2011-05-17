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
