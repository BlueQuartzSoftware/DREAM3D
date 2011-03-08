/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Michael A. Groeber (US Air Force Research Laboratory)
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

#include "ReconstructionVTKWriter.h"

#include "AIM/Common/OIMColoring.hpp"


#define WRITE_VTK_GRAIN_HEADER(FILE_TYPE, ptr)\
  fprintf(f, "# vtk DataFile Version 2.0\n");\
  fprintf(f, "data set from AIMReconstruction\n");\
  fprintf(f, FILE_TYPE); fprintf(f, "\n");\
  fprintf(f, "DATASET STRUCTURED_POINTS\n");\
  fprintf(f, "DIMENSIONS %d %d %d\n", ptr->xpoints, ptr->ypoints, ptr->zpoints);\
  fprintf(f, "ORIGIN 0.0 0.0 0.0\n");\
  fprintf(f, "SPACING %f %f %f\n", ptr->resx, ptr->resy, ptr->resz);\
  fprintf(f, "POINT_DATA %d\n\n", ptr->xpoints * ptr->ypoints * ptr->zpoints );\


#define WRITE_VTK_GRAIN_IDS(ptr)\
  fprintf(f, "SCALARS GrainID int  1\n");\
  fprintf(f, "LOOKUP_TABLE default\n");\
  for (size_t i = 0; i < total; i++) {\
    if(i%20 == 0 && i > 0) { fprintf(f, "\n"); }\
    fprintf(f, "%d ", ptr->voxels[i].grainname);\
  }\
  fprintf(f, "\n");\


#define WRITE_VTK_SCALARS_FROM_VOXEL(ptr, name, type, var)\
  fprintf(f, "SCALARS %s %s\n", #name, #type);\
  fprintf(f, "LOOKUP_TABLE default\n");\
  for (size_t i = 0; i < total; i++) {\
    if(i%20 == 0 && i > 0) { fprintf(f, "\n");}\
    fprintf(f, "%f ", ptr->voxels[i].var);\
  }\


#define WRITE_VTK_GRAIN_WITH_VOXEL_SCALAR_VALUE(name, var)\
int ReconstructionVTKWriter::write##name##VizFile(ReconstructionFunc* r, const std::string &file)\
{\
  FILE* f = NULL;\
  f = fopen(file.c_str(), "w");\
  if (NULL == f) {return 1;}\
  WRITE_VTK_GRAIN_HEADER("ASCII", r)\
  size_t total = r->xpoints * r->ypoints * r->zpoints;\
  WRITE_VTK_GRAIN_IDS(r)\
  WRITE_VTK_SCALARS_FROM_VOXEL(r, name, float, var)\
  fclose(f);\
  return 0;\
}

#define WRITE_VTK_GRAIN_WITH_GRAIN_SCALAR_VALUE(name, var)\
int ReconstructionVTKWriter::write##name##VizFile(ReconstructionFunc* r, const std::string &file)\
{\
  FILE* f = NULL;\
  f = fopen(file.c_str(), "w");\
  if (NULL == f) {return 1;}\
  WRITE_VTK_GRAIN_HEADER("ASCII", r)\
  size_t total = r->xpoints * r->ypoints * r->zpoints;\
  WRITE_VTK_GRAIN_IDS(r)\
  fprintf(f, "SCALARS %s float\n", #name);\
  fprintf(f, "LOOKUP_TABLE default\n");\
  for (size_t i = 0; i < total; i++) {\
    if(i%20 == 0 && i > 0) { fprintf(f, "\n");}\
    fprintf(f, "%f ", r->m_Grains[r->voxels[i].grainname]->var);\
  }\
  fclose(f);\
  return 0;\
}


WRITE_VTK_GRAIN_WITH_VOXEL_SCALAR_VALUE(ImageQuality, imagequality)

WRITE_VTK_GRAIN_WITH_GRAIN_SCALAR_VALUE(SchmidFactor, schmidfactor)


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionVTKWriter::ReconstructionVTKWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionVTKWriter::~ReconstructionVTKWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ReconstructionVTKWriter::writeVisualizationFile(ReconstructionFunc* r, const std::string &file)
{
  FILE* f = NULL;
  f = fopen(file.c_str(), "w");
  if (NULL == f)
  {
    return 1;
  }
  WRITE_VTK_GRAIN_HEADER("ASCII", r)
  size_t total = r->xpoints * r->ypoints * r->zpoints;
  WRITE_VTK_GRAIN_IDS(r)

  WRITE_VTK_SCALARS_FROM_VOXEL(r, Euclidean, float, nearestneighbordistance[0])

  fclose(f);
  return 0;
}

int ReconstructionVTKWriter::writeDisorientationFile(ReconstructionFunc* r, const std::string &file)
{
  FILE* f = NULL;
  f = fopen(file.c_str(), "w");
  if (NULL == f)
  {
    return 1;
  }
  WRITE_VTK_GRAIN_HEADER("ASCII", r)
  size_t total = r->xpoints * r->ypoints * r->zpoints;
  WRITE_VTK_GRAIN_IDS(r)

  WRITE_VTK_SCALARS_FROM_VOXEL(r, KAM, float, kernelmisorientation)
  WRITE_VTK_SCALARS_FROM_VOXEL(r, GAM, float, grainmisorientation)
  WRITE_VTK_SCALARS_FROM_VOXEL(r, LMG, float, misorientationgradient)

  fclose(f);
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ReconstructionVTKWriter::writeIPFVizFile(ReconstructionFunc* r, const std::string &file)
{
  FILE* f = NULL;
  f = fopen(file.c_str(), "w");
  if (NULL == f)
  {
    return 1;
  }
#if AIM_WRITE_BINARY_VTK_FILE
  WRITE_VTK_GRAIN_HEADER("BINARY", r)
#else
  WRITE_VTK_GRAIN_HEADER("ASCII", r)
#endif

  size_t total = r->xpoints * r->ypoints * r->zpoints;
#if AIM_WRITE_BINARY_VTK_FILE
  fprintf(f, "SCALARS GrainID int  1\n");
  fprintf(f, "LOOKUP_TABLE default\n");
  int* gn = new int[total];
  int t;
  for (size_t i = 0; i < total; i++)
  {
    t = r->voxels[i].grainname;
#ifdef MXA_LITTLE_ENDIAN
    MXA::Endian::reverseBytes<int >(t);
#endif
    gn[i] = t;
  }
  size_t totalWritten = fwrite(gn, sizeof(int), total, f);

  delete[] gn;
  if (totalWritten != total)
  {
    std::cout << "Error Writing Binary VTK Data into file " << file << std::endl;
    fclose(f);
    return -1;
  }
#else
  WRITE_VTK_GRAIN_IDS(r)
#endif

#if AIM_WRITE_BINARY_VTK_FILE
  fprintf(f, "COLOR_SCALARS IPF_Colors 4\n");
  // Allocate our RGBA array
  unsigned char* rgba = new unsigned char[total * 4];
  //double red,green,blue;
  double q1[4];
  unsigned char hkl[3] = {0, 0, 0};

  double RefDirection[3] =
  { 0.0, 0.0, 1.0 };
  for (size_t i = 0; i < total; i++)
  {
    if (r->crystruct == AIM::Reconstruction::Cubic)
    {
      OIMColoring::GenerateIPFColor(r->voxels[i].euler1, r->voxels[i].euler2, r->voxels[i].euler3, RefDirection[0], RefDirection[1], RefDirection[2], &rgba[i * 4], hkl);
    }
    else if (r->crystruct == AIM::Reconstruction::Hexagonal)
    {
      q1[0] = r->voxels[i].quat[1];
      q1[1] = r->voxels[i].quat[2];
      q1[2] = r->voxels[i].quat[3];
      q1[3] = r->voxels[i].quat[4];
      OIMColoring::CalculateHexIPFColor(q1, &rgba[i]);
    }
    rgba[i * 4 + 3] = 255;
  }
  totalWritten = fwrite(rgba, sizeof(char), total * 4, f);
  delete[] rgba;
  if (totalWritten != total * 4)
  {
    std::cout << "Error Writing Binary Data for IPF Colors to file " << file << std::endl;
    fclose(f);
    return -1;
  }
#else

  fprintf(f, "COLOR_SCALARS IPF_Colors 3\n");
  double red,green,blue;
  double q1[4];
  unsigned char rgb[3] =
  { 0, 0, 0};
  unsigned char hkl[3] =
  { 0, 0, 0};
  double RefDirection[3] =
  { 0.0, 0.0, 1.0};
  for (size_t i = 0; i < total; i++)
  {
    if(r->crystruct == AIM::Reconstruction::Cubic)
    {
      OIMColoring::GenerateIPFColor(r->voxels[i].euler1, r->voxels[i].euler2, r->voxels[i].euler3, RefDirection[0], RefDirection[1], RefDirection[2], rgb, hkl);
//      OIMColoring::GenerateIPFColor(r->m_Grains[r->voxels[i].grainname]->euler1, r->m_Grains[r->voxels[i].grainname]->euler2, r->m_Grains[r->voxels[i].grainname]->euler3, RefDirection[0], RefDirection[1], RefDirection[2], rgb);
      red = static_cast<double>(double(rgb[0])/255.0);
      green = static_cast<double>(double(rgb[1])/255.0);
      blue = static_cast<double>(double(rgb[2])/255.0);
    }
    if(r->crystruct == AIM::Reconstruction::Hexagonal)
    {
      q1[0]=r->voxels[i].quat[1];
      q1[1]=r->voxels[i].quat[2];
      q1[2]=r->voxels[i].quat[3];
      q1[3]=r->voxels[i].quat[4];
      OIMColoring::CalculateHexIPFColor(q1, rgb);
      red = static_cast<double>(double(rgb[0])/255.0);
      green = static_cast<double>(double(rgb[1])/255.0);
      blue = static_cast<double>(double(rgb[2])/255.0);
    }
    fprintf(f, "%f %f %f\n",red, green, blue);
  }
#endif

  fclose(f);
  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ReconstructionVTKWriter::writeDownSampledVizFile(ReconstructionFunc* r, const std::string &file)
{
  FILE* f = NULL;
  f = fopen(file.c_str(), "w");
  if (NULL == f)
  {
    return 1;
  }
  int counter = 0;
  double x, y, z;
  double dsresx, dsresy, dsresz;
  int col, row, plane;
  int index;
  int *gnames;
  int numgrains = r->m_Grains.size();
  gnames = new int[numgrains];
  for (int i = 0; i < numgrains; i++)
  {
    gnames[i] = 0;
  }
  int dsxpoints = int(r->sizex / (r->resx * r->downsamplefactor));
  int dsypoints = int(r->sizey / (r->resy * r->downsamplefactor));
  int dszpoints = int(r->sizez / (r->resz * r->downsamplefactor));
  dsresx = r->resx * r->downsamplefactor;
  dsresy = r->resy * r->downsamplefactor;
  dsresz = r->resz * r->downsamplefactor;
  fprintf(f, "# vtk DataFile Version 2.0\n");
  fprintf(f, "Down Sampled from AIMReconstruction\n");
  fprintf(f, "ASCII\n");
  fprintf(f, "DATASET STRUCTURED_POINTS\n");
  fprintf(f, "DIMENSIONS %d %d %d\n", dsxpoints, dsypoints, dszpoints);
  fprintf(f, "ORIGIN 0.0 0.0 0.0\n");
  fprintf(f, "SPACING %f %f %f\n", dsresx, dsresy, dsresz);
  fprintf(f, "POINT_DATA %d\n\n", dsxpoints * dsypoints * dszpoints);
  fprintf(f, "SCALARS GrainID int  1\n");
  fprintf(f, "LOOKUP_TABLE default\n");
  for (int i = 0; i < dszpoints; i++)
  {
    for (int j = 0; j < dsypoints; j++)
    {
      for (int k = 0; k < dsxpoints; k++)
      {
        x = (k * dsresx) + (dsresx / 2.0);
        y = (j * dsresy) + (dsresy / 2.0);
        z = (i * dsresz) + (dsresz / 2.0);
        col = int(x / r->resx);
        row = int(y / r->resy);
        plane = int(z / r->resz);
        index = (plane * r->xpoints * r->ypoints) + (row * r->xpoints) + col;
        if (counter % 20 == 0 && counter > 0)
        {
          fprintf(f, "\n");
        }
        fprintf(f, "%d ", r->voxels[index].grainname);
        gnames[r->voxels[index].grainname]++;
        counter++;
      }
    }
  }
  for (int i = 0; i < numgrains; i++)
  {
    fprintf(f, "%d ", i);
    fprintf(f, "%d ", gnames[i]);
    fprintf(f, "\n");
  }
  fclose(f);
  return 0;
}
