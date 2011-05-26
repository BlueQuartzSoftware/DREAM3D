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
#include "GrainGeneratorVoxelWriter.h"

#include<string.h>

#include <vector>
#include <string>
#include<iomanip>

#include "AIM/Common/OIMColoring.hpp"
#include "AIM/Common/VTKWriterMacros.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGeneratorVoxelWriter::GrainGeneratorVoxelWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGeneratorVoxelWriter::~GrainGeneratorVoxelWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GrainGeneratorVoxelWriter::writeVisualizationFile(GrainGeneratorFunc* r, const std::string &file)
{
  FILE* f = NULL;
  f = fopen(file.c_str(), "wb");
  if (NULL == f)
  {
    return 1;
  }
  // Write the correct header
  if (true == m_WriteBinaryFiles)
  {
    WRITE_VTK_GRAIN_HEADER("BINARY", r)
  }
  else
  {
    WRITE_VTK_GRAIN_HEADER("ASCII", r)
  }
  size_t total = r->xpoints * r->ypoints * r->zpoints;

  if (true == m_WriteBinaryFiles)
  {
    WRITE_VTK_GRAIN_IDS_BINARY(r, AIM::Reconstruction::GrainIdScalarName, voxels);
    WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::Reconstruction::SurfaceVoxelScalarName, int, voxels, surfacevoxel)
    WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::Reconstruction::PhaseIdScalarName, int, voxels, phase)
  }
  else
  {
    WRITE_VTK_GRAIN_IDS_ASCII(r, AIM::Reconstruction::GrainIdScalarName, voxels)
    WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::Reconstruction::SurfaceVoxelScalarName, int, voxels, surfacevoxel, "%d ")
    WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::Reconstruction::PhaseIdScalarName, int, voxels, unassigned, "%d ")
  }
  fclose(f);
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GrainGeneratorVoxelWriter::writeIPFVizFile(GrainGeneratorFunc* r, const std::string &file)
{
  FILE* f = NULL;
  f = fopen(file.c_str(), "wb");
  if (NULL == f)
  {
    return 1;
  }

  size_t total = r->xpoints * r->ypoints * r->zpoints;
  unsigned char hkl[3] =
  { 0, 0, 0 };
  VTK_IPF_COLOR_REFDIRECTION(RefDirection);
  int phase;
  unsigned char* rgba = NULL;
  float red, green, blue;
  size_t index = 0;
  // Write the correct header
  if (true == m_WriteBinaryFiles)
  {
    WRITE_VTK_GRAIN_HEADER("BINARY", r)
    WRITE_VTK_GRAIN_IDS_BINARY(r, AIM::Reconstruction::GrainIdScalarName, voxels);
    WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::Reconstruction::PhaseIdScalarName, int, voxels, phase)
    // Write the COLOR_SCALARS
    fprintf(f, "COLOR_SCALARS IPF_Colors 4\n");
    rgba = new unsigned char[total * 4]; // We need the whole array because we build it and write it all at the end
  }
  else
  {
    WRITE_VTK_GRAIN_HEADER("ASCII", r)
    WRITE_VTK_GRAIN_IDS_ASCII(r, AIM::Reconstruction::GrainIdScalarName, voxels);
    WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::Reconstruction::PhaseIdScalarName, int, voxels, phase, "%d ")
    // Write the COLOR_SCALARS
    fprintf(f, "COLOR_SCALARS IPF_Colors 3\n");
    rgba = new unsigned char[4]; // We just need 4 bytes for ASCII writing
  }

  for (size_t i = 0; i < total; i++)
  {
    phase = r->voxels[i].phase;
    if (true == m_WriteBinaryFiles)
    {
      index = i * 4;
    }
    else
    {
      index = 0;
    }
    if (r->crystruct[phase] == AIM::Reconstruction::Cubic)
    {
      OIMColoring::GenerateIPFColor(r->m_Grains[r->voxels[i].grain_index]->euler1, r->m_Grains[r->voxels[i].grain_index]->euler2, r->m_Grains[r->voxels[i].grain_index]->euler3, RefDirection[0], RefDirection[1], RefDirection[2], &rgba[index], hkl);
    }
    else if (r->crystruct[phase] == AIM::Reconstruction::Hexagonal)
    {
      OIMColoring::CalculateHexIPFColor(r->m_Grains[r->voxels[i].grain_index]->avg_quat, RefDirection, &rgba[index]);
    }
    if (true == m_WriteBinaryFiles)
    {
      rgba[index + 3] = 255;
    }
    else
    {
      red = static_cast<float> (float(rgba[index]) / 255.0);
      green = static_cast<float> (float(rgba[index + 1]) / 255.0);
      blue = static_cast<float> (float(rgba[index + 2]) / 255.0);
      fprintf(f, "%f %f %f\n", red, green, blue);
    }
  }

  if (true == m_WriteBinaryFiles)
  {
    size_t totalWritten = fwrite(rgba, sizeof(char), total * 4, f);
    if (totalWritten != total * 4)
    {
      std::cout << "Error Writing Binary Data for IPF Colors to file " << file << std::endl;
      fclose(f);
      return -1;
    }
  }
  // Clean up the allocated memory
  delete[] rgba;

  fclose(f);
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GrainGeneratorVoxelWriter::writePhFile(GrainGeneratorFunc* r, const std::string &file)
{
  std::string OutputName;

  // Change the name of the input filename for outout
  // std::vector<std::string> tokens;
 // std::string delimeters = "."; // Only a period
  //  std::tokenize(filename, tokens, delimeters);

  //OutputName = tokens[0] + ".ph";
  std::ofstream outfile;
  outfile.open(file.c_str());
  if (!outfile)
  {
    std::cout << "Failed to open: " << file << std::endl;
    return -1;
  }

  // Find the unique number of grains
  std::map<int, bool> used;

  for (int i = 0; i < r->totalpoints; ++i)
  {
    used[r->voxels[i].grain_index] = true;
  }

  int grains = 0;
  typedef std::map<int, bool>::iterator iterator;
  for (iterator i = used.begin(); i != used.end(); i++)
    if ((*i).second == true) grains++;

  //std::cout<<grains<< " " << used.size() << std::endl;


  outfile << "     " << r->xpoints << "     " << r->ypoints << "     " << r->zpoints << std::endl;
  outfile << "\'DREAM3\'              52.00  1.000  1.0       " << grains << std::endl;
  outfile << " 3.000 0.000 0.000          0        \n"; // << grains << endl;

  int count = 0;
  for (int k = 0; k < r->totalpoints; k++)
  {
    outfile << std::setw(6) << r->voxels[k].grain_index;
    count++;
    if (count == 20)
    {
      outfile << std::endl;
      count = 0;
    }
    //                    outfile << grid[i][j][k] << endl;
  }
  outfile << std::endl;
  outfile.close();
  return 0;
}
