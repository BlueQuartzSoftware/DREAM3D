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
#ifndef VTKWRITER_H_
#define VTKWRITER_H_

/**@file VTKWriterMacros.h
 * @brief This file contains various macros to write out consistent VTK legacy
 * style files.
 */
# if defined(__LP64__) && __LP64__

#define WRITE_RECTILINEAR_GRID_HEADER(FILE_TYPE, ptr, xpoints, ypoints, zpoints)\
  fprintf(f, "# vtk DataFile Version 2.0\n");\
  fprintf(f, "data set from DREAM3D\n");\
  fprintf(f, FILE_TYPE); fprintf(f, "\n");\
  fprintf(f, "DATASET RECTILINEAR_GRID\n");\
  fprintf(f, "DIMENSIONS %ld %ld %ld\n", xpoints, ypoints, zpoints);\

#define WRITE_STRUCTURED_POINTS_HEADER(FILE_TYPE, ptr)\
  fprintf(f, "# vtk DataFile Version 2.0\n");\
  fprintf(f, "data set from DREAM3D\n");\
  fprintf(f, FILE_TYPE); fprintf(f, "\n");\
  fprintf(f, "DATASET STRUCTURED_POINTS\n");\
  fprintf(f, "DIMENSIONS %ld %ld %ld\n", ptr->getXPoints(), ptr->getYPoints(), ptr->getZPoints());\
  fprintf(f, "ORIGIN 0.0 0.0 0.0\n");\
  fprintf(f, "SPACING %f %f %f\n", ptr->getXRes(), ptr->getYRes(), ptr->getZRes());\
  fprintf(f, "POINT_DATA %ld\n\n", ptr->getXPoints() * ptr->getYPoints() * ptr->getZPoints() );\


#else

#define WRITE_RECTILINEAR_GRID_HEADER(FILE_TYPE, ptr, xpoints, ypoints, zpoints)\
  fprintf(f, "# vtk DataFile Version 2.0\n");\
  fprintf(f, "data set from DREAM3D\n");\
  fprintf(f, FILE_TYPE); fprintf(f, "\n");\
  fprintf(f, "DATASET RECTILINEAR_GRID\n");\
  fprintf(f, "DIMENSIONS %ld %ld %ld\n", xpoints, ypoints, zpoints);\

#define WRITE_STRUCTURED_POINTS_HEADER(FILE_TYPE, ptr)\
  fprintf(f, "# vtk DataFile Version 2.0\n");\
  fprintf(f, "data set from DREAM3D\n");\
  fprintf(f, FILE_TYPE); fprintf(f, "\n");\
  fprintf(f, "DATASET STRUCTURED_POINTS\n");\
  fprintf(f, "DIMENSIONS %ld %ld %ld\n", ptr->getXPoints(), ptr->getYPoints(), ptr->getZPoints());\
  fprintf(f, "ORIGIN 0.0 0.0 0.0\n");\
  fprintf(f, "SPACING %f %f %f\n", ptr->getXRes(), ptr->getYRes(), ptr->getZRes());\
  fprintf(f, "POINT_DATA %lld\n\n", ptr->getXPoints() * ptr->getYPoints() * ptr->getZPoints() );\


#endif



#define WRITE_VTK_GRAIN_IDS_ASCII(ptr, ScalarName, grain_indicies)\
  fprintf(f, "SCALARS %s int 1\n", ScalarName.c_str());\
  fprintf(f, "LOOKUP_TABLE default\n");\
  for (int64_t i = 0; i < totalPoints; i++) {\
    if(i%20 == 0 && i > 0) { fprintf(f, "\n"); }\
    fprintf(f, "%d ", grain_indicies[i]);\
  }\
  fprintf(f, "\n");\


#define WRITE_VTK_GRAIN_IDS_BINARY(ptr, ScalarName)  \
  fprintf(f, "SCALARS %s int 1\n", ScalarName.c_str());\
  fprintf(f, "LOOKUP_TABLE default\n"); \
  { \
  int* gn = new int[totalPoints];\
  int t;\
  for (int64_t i = 0; i < totalPoints; i++) {\
    t = grain_indicies[i];\
    MXA::Endian::FromSystemToBig::convert<int>(t); \
    gn[i] = t; \
  }\
  int64_t totalWritten = fwrite(gn, sizeof(int), totalPoints, f);\
  delete[] gn;\
  if (totalWritten != totalPoints)  {\
    std::cout << "Error Writing Binary VTK Data into file " << file << std::endl;\
    fclose(f);\
    return -1;\
  }\
  }


#define WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(ptr, name, m_msgType, var, FORMAT)\
  fprintf(f, "SCALARS %s %s 1\n", name.c_str(), #m_msgType);\
  fprintf(f, "LOOKUP_TABLE default\n");\
  for (int64_t i = 0; i < totalPoints; i++) {\
    if(i%20 == 0 && i > 0) { fprintf(f, "\n");}\
    fprintf(f, FORMAT, var[i]);\
  }fprintf(f,"\n"); \

#define WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(ptr, name, m_msgType, var)\
  fprintf(f, "SCALARS %s %s 1\n", name.c_str(), #m_msgType);\
  fprintf(f, "LOOKUP_TABLE default\n");\
  { \
  m_msgType* gn = new m_msgType[totalPoints];\
  m_msgType t;\
  for (int64_t i = 0; i < totalPoints; i++) {\
    t = var[i];\
    MXA::Endian::FromSystemToBig::convert<m_msgType>(t); \
    gn[i] = t; \
  }\
  int64_t totalWritten = fwrite(gn, sizeof(m_msgType), totalPoints, f);\
  delete[] gn;\
  if (totalWritten != totalPoints)  {\
    std::cout << "Error Writing Binary VTK Data into file " << file << std::endl;\
    fclose(f);\
    return -1;\
  }\
  }


#define WRITE_VTK_SCALARS_FROM_VOXEL_BINARY_NOSWAP(ptr, name, m_msgType, var)\
  fprintf(f, "SCALARS %s %s 1\n", name.c_str(), #m_msgType);\
  fprintf(f, "LOOKUP_TABLE default\n");\
  { \
  m_msgType* gn = new m_msgType[totalPoints];\
  for (int64_t i = 0; i < totalPoints; i++) {\
    gn[i] = var[i];\
  }\
  int64_t totalWritten = fwrite(gn, sizeof(m_msgType), totalPoints, f);\
  delete[] gn;\
  if (totalWritten != totalPoints)  {\
    std::cout << "Error Writing Binary VTK Data into file " << file << std::endl;\
    fclose(f);\
    return -1;\
  }\
 }


#define WRITE_VTK_SCALARS_FROM_FIELD_ASCII(ptr, name, m_msgType, var, grain_indicies, FORMAT)\
  fprintf(f, "SCALARS %s %s 1\n", name.c_str(), #m_msgType);\
  fprintf(f, "LOOKUP_TABLE default\n");\
  for (int64_t i = 0; i < totalPoints; i++) {\
    if(i%20 == 0 && i > 0) { fprintf(f, "\n");}\
    fprintf(f, FORMAT, var[grain_indicies[i]]);\
  } fprintf(f,"\n");

#define WRITE_VTK_SCALARS_FROM_FIELD_BINARY(ptr, name, m_msgType, var, grain_indicies)\
  fprintf(f, "SCALARS %s %s 1\n", name.c_str(), #m_msgType);\
  fprintf(f, "LOOKUP_TABLE default\n");\
  { \
  m_msgType* gn = new m_msgType[totalPoints];\
  m_msgType t;\
  for (int64_t i = 0; i < totalPoints; i++) {\
    t = var[grain_indicies[i]];\
    MXA::Endian::FromSystemToBig::convert<m_msgType>(t); \
    gn[i] = t; \
  }\
  int64_t totalWritten = fwrite(gn, sizeof(m_msgType), totalPoints, f);\
  delete[] gn;\
  if (totalWritten != totalPoints)  {\
    std::cout << "Error Writing Binary VTK Data into file " << file << std::endl;\
    fclose(f);\
    return -1;\
  }\
  }

#define VTK_IPF_COLOR_REFDIRECTION(var)\
    float var[3] = {0.0f, 0.0f, 1.0f};

#define GGVTKW_IPFCOLOR_BIANRY(var, quat)\
if (r->crystruct[phase] == Ebsd::CrystalStructure::Cubic) {\
  OIMColoring::GenerateCubicIPFColor(var->euler1,\
                              var->euler2,\
                              var->euler3,\
                              RefDirection[0], RefDirection[1], RefDirection[2],\
                              &rgba[i * 4], hkl);\
} else if (r->crystruct[phase] == Ebsd::CrystalStructure::Hexagonal)   { \
  q1[1] = var->quat[1];\
  q1[2] = var->quat[2];\
  q1[3] = var->quat[3];\
  q1[4] = var->quat[4];\
  OIMColoring::CalculateHexIPFColor(q1, RefDirection[0], RefDirection[1], RefDirection[2], &rgba[i * 4]); \
}


#endif /* VTKWRITER_H_ */
