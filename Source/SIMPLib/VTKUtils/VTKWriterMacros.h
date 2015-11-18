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

#ifndef _VTKWRITER_H_
#define _VTKWRITER_H_

#include <QtCore/QtEndian>

#include "SIMPLib/Utilities/SIMPLibEndian.h"

/**@file VTKWriterMacros.h
 * @brief This file contains various macros to write out consistent VTK legacy
 * style files.
 */
# if defined(__LP64__) && __LP64__

#define WRITE_RECTILINEAR_GRID_HEADER(FILE_TYPE, ptr, xpoints, ypoints, zpoints)\
  fprintf(f, "# vtk DataFile Version 2.0\n");\
  fprintf(f, "Data set from %s\n", SIMPLib::Version::PackageComplete().toLatin1().constData());\
  fprintf(f, FILE_TYPE);\
  fprintf(f, "\n");\
  fprintf(f, "DATASET RECTILINEAR_GRID\n");\
  fprintf(f, "DIMENSIONS %ld %ld %ld\n", xpoints, ypoints, zpoints);\

#define WRITE_STRUCTURED_POINTS_HEADER(FILE_TYPE, ptr)\
  fprintf(f, "# vtk DataFile Version 2.0\n");\
  fprintf(f, "Data set from %s\n", SIMPLib::Version::PackageComplete().toLatin1().constData());\
  fprintf(f, FILE_TYPE); fprintf(f, "\n");\
  fprintf(f, "DATASET STRUCTURED_POINTS\n");\
  fprintf(f, "DIMENSIONS %ld %ld %ld\n", ptr->getXPoints(), ptr->getYPoints(), ptr->getZPoints());\
  fprintf(f, "SPACING %f %f %f\n", ptr->getXRes(), ptr->getYRes(), ptr->getZRes());\
  fprintf(f, "ORIGIN 0.0 0.0 0.0\n");\
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



#define WRITE_VTK_FEATURE_IDS_ASCII(ptr, ScalarName, m_FeatureIds)\
  fprintf(f, "SCALARS %s int 1\n", ScalarName.toLatin1().data());\
  fprintf(f, "LOOKUP_TABLE default\n");\
  for (int64_t i = 0; i < totalPoints; i++) {\
    if(i%20 == 0 && i > 0) { fprintf(f, "\n"); }\
    fprintf(f, "%d ", m_FeatureIds[i]);\
  }\
  fprintf(f, "\n");\


#define WRITE_VTK_FEATURE_IDS_BINARY(ptr, ScalarName)  \
  fprintf(f, "SCALARS %s int 1\n", ScalarName.toLatin1().data());\
  fprintf(f, "LOOKUP_TABLE default\n"); \
  { \
    std::vector<int> gn(totalPoints, 0);\
    int t;\
    for (int64_t i = 0; i < totalPoints; i++) {\
      t = m_FeatureIds[i];\
      SIMPLib::Endian::FromSystemToBig::convert(t);\
      gn[i] = t; \
    }\
    int64_t totalWritten = fwrite( &(gn.front()), sizeof(int), totalPoints, f);\
    if (totalWritten != totalPoints)  {\
      qDebug() << "Error Writing Binary VTK Data into file " << file ;\
      fclose(f);\
      return -1;\
    }\
  }


#define WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(ptr, name, m_msgType, var, FORMAT)\
  fprintf(f, "SCALARS %s %s 1\n", name.toLatin1().data(), #m_msgType);\
  fprintf(f, "LOOKUP_TABLE default\n");\
  for (int64_t i = 0; i < totalPoints; i++) {\
    if(i%20 == 0 && i > 0) { fprintf(f, "\n");}\
    fprintf(f, FORMAT, var[i]);\
  }fprintf(f,"\n"); \

#define WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(ptr, name, m_msgType, var)\
  fprintf(f, "SCALARS %s %s 1\n", name.toLatin1().data(), #m_msgType);\
  fprintf(f, "LOOKUP_TABLE default\n");\
  { \
    m_msgType* gn = new m_msgType[totalPoints];\
    m_msgType t;\
    for (int64_t i = 0; i < totalPoints; i++) {\
      t = var[i];\
      SIMPLib::Endian::FromSystemToBig::convert(t);\
      gn[i] = t; \
    }\
    int64_t totalWritten = fwrite(gn, sizeof(m_msgType), totalPoints, f);\
    delete[] gn;\
    if (totalWritten != totalPoints)  {\
      qDebug() << "Error Writing Binary VTK Data into file " << file ;\
      fclose(f);\
      return -1;\
    }\
  }


#define WRITE_VTK_SCALARS_FROM_VOXEL_BINARY_NOSWAP(ptr, name, m_msgType, var)\
  fprintf(f, "SCALARS %s %s 1\n", name.toLatin1().data(), #m_msgType);\
  fprintf(f, "LOOKUP_TABLE default\n");\
  { \
    m_msgType* gn = new m_msgType[totalPoints];\
    for (int64_t i = 0; i < totalPoints; i++) {\
      gn[i] = var[i];\
    }\
    int64_t totalWritten = fwrite(gn, sizeof(m_msgType), totalPoints, f);\
    delete[] gn;\
    if (totalWritten != totalPoints)  {\
      qDebug() << "Error Writing Binary VTK Data into file " << file ;\
      fclose(f);\
      return -1;\
    }\
  }


#define WRITE_VTK_SCALARS_FROM_FIELD_ASCII(ptr, name, m_msgType, var, feature_indicies, FORMAT)\
  fprintf(f, "SCALARS %s %s 1\n", name.toLatin1().data(), #m_msgType);\
  fprintf(f, "LOOKUP_TABLE default\n");\
  for (int64_t i = 0; i < totalPoints; i++) {\
    if(i%20 == 0 && i > 0) { fprintf(f, "\n");}\
    fprintf(f, FORMAT, var[feature_indicies[i]]);\
  } fprintf(f,"\n");

#define WRITE_VTK_SCALARS_FROM_FIELD_BINARY(ptr, name, m_msgType, var, feature_indicies)\
  fprintf(f, "SCALARS %s %s 1\n", name.toLatin1().data(), #m_msgType);\
  fprintf(f, "LOOKUP_TABLE default\n");\
  { \
    m_msgType* gn = new m_msgType[totalPoints];\
    m_msgType t;\
    for (int64_t i = 0; i < totalPoints; i++) {\
      t = var[feature_indicies[i]];\
      SIMPLib::Endian::FromSystemToBig::convert(t); \
      gn[i] = t; \
    }\
    int64_t totalWritten = fwrite(gn, sizeof(m_msgType), totalPoints, f);\
    delete[] gn;\
    if (totalWritten != totalPoints)  {\
      qDebug() << "Error Writing Binary VTK Data into file " << file ;\
      fclose(f);\
      return -1;\
    }\
  }

  /**
    *@param
    * @param
    * @param
    * @param
    */
#define VTK_WRITE_RECTILINEAR_DATA(ArrayType, iDataPtr, VtkType, Type, Format)\
  {\
    ArrayType::Pointer array = std::dynamic_pointer_cast<ArrayType>(iDataPtr);\
    if(NULL != array.get()) {\
      size_t totalElements = array->getSize();\
      Type* val = array->getPointer(0);\
      int numComps = array->getNumberOfComponents();\
      QString dName = array->getName();\
      dName = dName.replace(" ", "_");\
      fprintf(f, "SCALARS %s %s %d\n", dName.toLatin1().data(), VtkType, numComps);\
      fprintf(f, "LOOKUP_TABLE default\n");\
      if(getWriteBinaryFile()) {\
        if(BIGENDIAN == 0) {array->byteSwapElements(); }\
        size_t totalWritten = fwrite(val, array->getTypeSize(), totalElements, f);\
        if(totalWritten != totalElements) {}\
        fprintf(f,"\n");\
        if(BIGENDIAN == 0) {array->byteSwapElements(); }\
      } else {\
        for (size_t i = 0; i < totalElements; i++) {\
          if(i%20 == 0 && i > 0) { fprintf(f, "\n");}\
          fprintf(f, Format, val[i]);\
        }\
        fprintf(f,"\n");\
      }\
    }\
  }



#endif /* VTKWRITER_H_ */

