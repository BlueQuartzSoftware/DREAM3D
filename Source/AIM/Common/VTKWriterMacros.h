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
#ifndef VTKWRITER_H_
#define VTKWRITER_H_

/**@file VTKWriterMacros.h
 * @brief This file contains various macros to write out consistent VTK legacy
 * style files.
 */


#define WRITE_VTK_GRAIN_HEADER(FILE_TYPE, ptr)\
  fprintf(f, "# vtk DataFile Version 2.0\n");\
  fprintf(f, "data set from AIMReconstruction\n");\
  fprintf(f, FILE_TYPE); fprintf(f, "\n");\
  fprintf(f, "DATASET STRUCTURED_POINTS\n");\
  fprintf(f, "DIMENSIONS %d %d %d\n", ptr->xpoints, ptr->ypoints, ptr->zpoints);\
  fprintf(f, "ORIGIN 0.0 0.0 0.0\n");\
  fprintf(f, "SPACING %f %f %f\n", ptr->resx, ptr->resy, ptr->resz);\
  fprintf(f, "POINT_DATA %d\n\n", ptr->xpoints * ptr->ypoints * ptr->zpoints );\


#define WRITE_VTK_GRAIN_IDS_ASCII(ptr, ScalarName, voxelVar)\
  fprintf(f, "SCALARS %s int 1\n", ScalarName.c_str());\
  fprintf(f, "LOOKUP_TABLE default\n");\
  for (size_t i = 0; i < total; i++) {\
    if(i%20 == 0 && i > 0) { fprintf(f, "\n"); }\
    fprintf(f, "%d ", ptr->voxelVar[i].grainname);\
  }\
  fprintf(f, "\n");\


#define WRITE_VTK_GRAIN_IDS_BINARY(ptr, ScalarName, voxelVar)  \
  fprintf(f, "SCALARS %s int 1\n", ScalarName.c_str());\
  fprintf(f, "LOOKUP_TABLE default\n"); \
  { \
  int* gn = new int[total];\
  int t;\
  for (size_t i = 0; i < total; i++) {\
    t = ptr->voxelVar[i].grainname;\
    MXA::Endian::FromSystemToBig::convert<int>(t); \
    gn[i] = t; \
  }\
  size_t totalWritten = fwrite(gn, sizeof(int), total, f);\
  delete[] gn;\
  if (totalWritten != total)  {\
    std::cout << "Error Writing Binary VTK Data into file " << file << std::endl;\
    fclose(f);\
    return -1;\
  }\
  }


#define WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(ptr, name, type, voxelsVar, var, FORMAT)\
  fprintf(f, "SCALARS %s %s 1\n", name.c_str(), #type);\
  fprintf(f, "LOOKUP_TABLE default\n");\
  for (size_t i = 0; i < total; i++) {\
    if(i%20 == 0 && i > 0) { fprintf(f, "\n");}\
    fprintf(f, FORMAT, ptr->voxelsVar[i].var);\
  }\

#define WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(ptr, name, type, voxelsVar, var)\
  fprintf(f, "SCALARS %s %s 1\n", name.c_str(), #type);\
  fprintf(f, "LOOKUP_TABLE default\n");\
  { \
  type* gn = new type[total];\
  type t;\
  for (size_t i = 0; i < total; i++) {\
    t = ptr->voxelsVar[i].var;\
    MXA::Endian::FromSystemToBig::convert<type>(t); \
    gn[i] = t; \
  }\
  size_t totalWritten = fwrite(gn, sizeof(type), total, f);\
  delete[] gn;\
  if (totalWritten != total)  {\
    std::cout << "Error Writing Binary VTK Data into file " << file << std::endl;\
    fclose(f);\
    return -1;\
  }\
  }

#define WRITE_VTK_GRAIN_WITH_GRAIN_SCALAR_VALUE_ASCII(ptr, name, type, var, FORMAT)\
  fprintf(f, "SCALARS %s float 1\n", name.c_str());\
  fprintf(f, "LOOKUP_TABLE default\n");\
  for (size_t i = 0; i < total; i++) {\
    if(i%20 == 0 && i > 0) { fprintf(f, "\n");}\
    fprintf(f, FORMAT, ptr->m_Grains[ptr->voxels[i].grainname]->var);\
  }

#define WRITE_VTK_GRAIN_WITH_GRAIN_SCALAR_VALUE_BINARY(ptr, name, type, var)\
  fprintf(f, "SCALARS %s %s 1\n", name.c_str(), #type);\
  fprintf(f, "LOOKUP_TABLE default\n");\
  { \
  type* gn = new type[total];\
  type t;\
  for (size_t i = 0; i < total; i++) {\
    t = ptr->m_Grains[ptr->voxels[i].grainname]->var;\
    MXA::Endian::FromSystemToBig::convert<type>(t); \
    gn[i] = t; \
  }\
  size_t totalWritten = fwrite(gn, sizeof(type), total, f);\
  delete[] gn;\
  if (totalWritten != total)  {\
    std::cout << "Error Writing Binary VTK Data into file " << file << std::endl;\
    fclose(f);\
    return -1;\
  }\
  }

#define VTK_IPF_COLOR_REFDIRECTION(var)\
    float var[3] = {0.0f, 0.0f, 1.0f};

#define GGVTKW_IPFCOLOR_BIANRY(var, quat)\
if (r->crystruct[phase] == AIM::Reconstruction::Cubic) {\
  OIMColoring::GenerateIPFColor(var->euler1,\
                              var->euler2,\
                              var->euler3,\
                              RefDirection[0], RefDirection[1], RefDirection[2],\
                              &rgba[i * 4], hkl);\
} else if (r->crystruct[phase] == AIM::Reconstruction::Hexagonal)   { \
  q1[1] = var->quat[1];\
  q1[2] = var->quat[2];\
  q1[3] = var->quat[3];\
  q1[4] = var->quat[4];\
  OIMColoring::CalculateHexIPFColor(q1, RefDirection[0], RefDirection[1], RefDirection[2], &rgba[i * 4]); \
}


#endif /* VTKWRITER_H_ */
