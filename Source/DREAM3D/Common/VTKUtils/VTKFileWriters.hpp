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

#ifndef _VTKFILEWRITERS_HPP_
#define _VTKFILEWRITERS_HPP_

#include <string>

#include "MXA/Common/MXASetGetMacros.h"
#include "MXA/Common/MXAEndian.h"

#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/OIMColoring.hpp"
#include "DREAM3D/Common/VTKUtils/VTKWriterMacros.h"


class DREAM3DLib_EXPORT GrainGeneratorFunc;

/**
 * @class VTKFileWriters VTKFileWriters.h DREAM3D/Common/VTKUtils/VTKFileWriters.h
 * @brief This class holds the functions to write the various VTK output files.
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Michael A. Groeber, PhD, US Air Force Research Laboratory
 * @date Feb 19, 2011
 * @version 1.0
 */
class VTKFileWriters
{
  public:
    MXA_SHARED_POINTERS(VTKFileWriters);
    MXA_STATIC_NEW_MACRO(VTKFileWriters);
    MXA_TYPE_MACRO(VTKFileWriters);

    virtual ~VTKFileWriters();

    MXA_INSTANCE_PROPERTY(bool, WriteBinaryFiles)

    template<typename T>
    int writeCoords(FILE* f, const char* axis, const char* type, int npoints, T min, T max, T step)
    {
      int err = 0;
      fprintf(f, "%s %d %s\n", axis, npoints, type);
      if (m_WriteBinaryFiles == true)
      {
        T* data = new T[npoints];
        T d;
        for (int idx = 0; idx < npoints; ++idx)
        {
          d = idx * step + min;
          MXA::Endian::FromSystemToBig::convert<T>(d);
          data[idx] = d;
        }
        size_t totalWritten = fwrite(data, sizeof(T), npoints, f);
        delete[] data;
        if (totalWritten != npoints)
        {
          std::cout << "Error Writing Binary VTK Data into file " << std::endl;
          fclose(f);
          return -1;
        }

      }
      else
      {

      }
      return err;
    }


    template<typename T>
    int writeReconRectilinearGrid(T* r, const std::string &file, bool writeEuclidean, bool writePhaseId, bool writeImageQual, bool writeIPFColor)
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

      // Write the XCoords
      writeCoords(f, "X_COORDINATES", "float", r->xpoints + 1, 0.0f - r->resx * 0.5f, (float)(r->xpoints + 1 * r->resx), r->resx);
      writeCoords(f, "Y_COORDINATES", "float", r->ypoints + 1, 0.0f - r->resy * 0.5f, (float)(r->ypoints + 1 * r->resy), r->resy);
      writeCoords(f, "Z_COORDINATES", "float", r->zpoints + 1, 0.0f - r->resz * 0.5f, (float)(r->zpoints + 1 * r->resz), r->resz);

      size_t total = r->xpoints * r->ypoints * r->zpoints;
      unsigned char hkl[3] =
      { 0, 0, 0 };
      VTK_IPF_COLOR_REFDIRECTION(RefDirection)
      int phase;
      unsigned char* rgba = NULL;
      float red, green, blue;
      size_t index = 0;
      fprintf(f, "CELL_DATA %d\n", (int)total);
      if (true == m_WriteBinaryFiles)
      {
        WRITE_VTK_GRAIN_IDS_BINARY(r, AIM::VTK::GrainIdScalarName, voxels);
        if (writeEuclidean)
        {
          WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::EuclideanScalarName, float, voxels, nearestneighbordistance[0])
        }
        if (writePhaseId)
        {
          WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::PhaseIdScalarName, int, voxels, phase)
        }
        if (writeImageQual)
        {
          WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::ImageQualityScalarName, float, voxels, imagequality)
        }

        // Write the IPF Colors
        if (writeIPFColor)
        {
          fprintf(f, "COLOR_SCALARS IPF_Colors 4\n");
          rgba = new unsigned char[total * 4]; // We need the whole array because we build it and write it all at the end
        }
      }
      else
      {
        WRITE_VTK_GRAIN_IDS_ASCII(r, AIM::VTK::GrainIdScalarName, voxels)
        if (writeEuclidean)
        { WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::EuclideanScalarName, float, voxels, nearestneighbordistance[0], "%f ")}
        if (writePhaseId)
        { WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::PhaseIdScalarName, int, voxels, phase, "%d ")}
        if (writeImageQual)
        { WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::ImageQualityScalarName, float, voxels, imagequality, "%f ")}

        if (writeIPFColor)
        {
          fprintf(f, "COLOR_SCALARS IPF_Colors 3\n");
          rgba = new unsigned char[4]; // We just need 4 bytes for ASCII writing
        }
      }

      if (writeIPFColor)
      {
        // Write the IPF Coloring Cell Data
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
            OIMColoring::GenerateIPFColor(r->voxels[i].euler1, r->voxels[i].euler2, r->voxels[i].euler3, RefDirection[0], RefDirection[1], RefDirection[2], &rgba[index], hkl);
          }
          else if (r->crystruct[phase] == AIM::Reconstruction::Hexagonal)
          {
            OIMColoring::CalculateHexIPFColor(r->voxels[i].quat, RefDirection, &rgba[index]);
          }
          if (true == m_WriteBinaryFiles)
          {
            rgba[index + 3] = 255;
          }
          else
          {
            red = static_cast<float>(float(rgba[index]) / 255.0);green = static_cast<float> (float(rgba[index + 1]) / 255.0);
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
            fclose( f);
            return -1;
          }
        }
        // Clean up the allocated memory
        delete[] rgba;
      }

      fclose(f);
      return err;
    }




    template<typename T>
    int writeGrainGenRectilinearGrid(T* r, const std::string &file, bool writeSurfaceVoxel, bool writePhaseId, bool writeIPFColor)
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

      // Write the XCoords
      writeCoords(f, "X_COORDINATES", "float", r->xpoints + 1, 0.0f - r->resx * 0.5f, (float)(r->xpoints + 1 * r->resx), r->resx);
      writeCoords(f, "Y_COORDINATES", "float", r->ypoints + 1, 0.0f - r->resy * 0.5f, (float)(r->ypoints + 1 * r->resy), r->resy);
      writeCoords(f, "Z_COORDINATES", "float", r->zpoints + 1, 0.0f - r->resz * 0.5f, (float)(r->zpoints + 1 * r->resz), r->resz);

      size_t total = r->xpoints * r->ypoints * r->zpoints;
      unsigned char hkl[3] =
      { 0, 0, 0 };
      VTK_IPF_COLOR_REFDIRECTION(RefDirection)
      int phase;
      unsigned char* rgba = NULL;
      float red, green, blue;
      size_t index = 0;
      fprintf(f, "CELL_DATA %d\n", (int)total);
      if (true == m_WriteBinaryFiles)
      {
        WRITE_VTK_GRAIN_IDS_BINARY(r, AIM::VTK::GrainIdScalarName, voxels);
        if (writeSurfaceVoxel)
        {
			WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::SurfaceVoxelScalarName, float, voxels, surfacevoxel)
        }
        if (writePhaseId)
        {
          WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::PhaseIdScalarName, int, voxels, phase)
        }

        // Write the IPF Colors
        if (writeIPFColor)
        {
          fprintf(f, "COLOR_SCALARS IPF_Colors 4\n");
          rgba = new unsigned char[total * 4]; // We need the whole array because we build it and write it all at the end
        }
      }
      else
      {
        WRITE_VTK_GRAIN_IDS_ASCII(r, AIM::VTK::GrainIdScalarName, voxels)
        if (writeSurfaceVoxel)
		{ WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::SurfaceVoxelScalarName, float, voxels, surfacevoxel, "%f ")}
        if (writePhaseId)
        { WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::PhaseIdScalarName, int, voxels, phase, "%d ")}

        if (writeIPFColor)
        {
          fprintf(f, "COLOR_SCALARS IPF_Colors 3\n");
          rgba = new unsigned char[4]; // We just need 4 bytes for ASCII writing
        }
      }

      if (writeIPFColor)
      {
        // Write the IPF Coloring Cell Data
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
            OIMColoring::GenerateIPFColor(r->voxels[i].euler1, r->voxels[i].euler2, r->voxels[i].euler3, RefDirection[0], RefDirection[1], RefDirection[2], &rgba[index], hkl);
          }
          else if (r->crystruct[phase] == AIM::Reconstruction::Hexagonal)
          {
            OIMColoring::CalculateHexIPFColor(r->voxels[i].quat, RefDirection, &rgba[index]);
          }
          if (true == m_WriteBinaryFiles)
          {
            rgba[index + 3] = 255;
          }
          else
          {
            red = static_cast<float>(float(rgba[index]) / 255.0);green = static_cast<float> (float(rgba[index + 1]) / 255.0);
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
            fclose( f);
            return -1;
          }
        }
        // Clean up the allocated memory
        delete[] rgba;
      }

      fclose(f);
      return err;
    }




    template<typename T>
    int writeMicroStatsRectilinearGrid(T* r, const std::string &file, bool writeSurfaceVoxel, bool writePhaseId, bool writeIPFColor, bool writeKernelMis)
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

      // Write the XCoords
      writeCoords(f, "X_COORDINATES", "float", r->xpoints + 1, 0.0f - r->resx * 0.5f, (float)(r->xpoints + 1 * r->resx), r->resx);
      writeCoords(f, "Y_COORDINATES", "float", r->ypoints + 1, 0.0f - r->resy * 0.5f, (float)(r->ypoints + 1 * r->resy), r->resy);
      writeCoords(f, "Z_COORDINATES", "float", r->zpoints + 1, 0.0f - r->resz * 0.5f, (float)(r->zpoints + 1 * r->resz), r->resz);

      size_t total = r->xpoints * r->ypoints * r->zpoints;
      unsigned char hkl[3] =
      { 0, 0, 0 };
      VTK_IPF_COLOR_REFDIRECTION(RefDirection)
      int phase;
      unsigned char* rgba = NULL;
      float red, green, blue;
      size_t index = 0;
      fprintf(f, "CELL_DATA %d\n", (int)total);
      if (true == m_WriteBinaryFiles)
      {
        WRITE_VTK_GRAIN_IDS_BINARY(r, AIM::VTK::GrainIdScalarName, voxels);
        if (writeSurfaceVoxel)
        {
			WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::SurfaceVoxelScalarName, float, voxels, surfacevoxel)
        }
        if (writePhaseId)
        {
          WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::PhaseIdScalarName, int, voxels, phase)
        }
        if (writeKernelMis)
        {
			WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::KAMScalarName, float, voxels, kernelmisorientation)
        }

        // Write the IPF Colors
        if (writeIPFColor)
        {
          fprintf(f, "COLOR_SCALARS IPF_Colors 4\n");
          rgba = new unsigned char[total * 4]; // We need the whole array because we build it and write it all at the end
        }
      }
      else
      {
        WRITE_VTK_GRAIN_IDS_ASCII(r, AIM::VTK::GrainIdScalarName, voxels)
        if (writeSurfaceVoxel)
		{ WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::SurfaceVoxelScalarName, float, voxels, surfacevoxel, "%f ")}
        if (writePhaseId)
        { WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::PhaseIdScalarName, int, voxels, phase, "%d ")}
        if (writeKernelMis)
		{ WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::KAMScalarName, float, voxels, kernelmisorientation, "%f ")}

        if (writeIPFColor)
        {
          fprintf(f, "COLOR_SCALARS IPF_Colors 3\n");
          rgba = new unsigned char[4]; // We just need 4 bytes for ASCII writing
        }
      }

      if (writeIPFColor)
      {
        // Write the IPF Coloring Cell Data
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
            OIMColoring::GenerateIPFColor(r->voxels[i].euler1, r->voxels[i].euler2, r->voxels[i].euler3, RefDirection[0], RefDirection[1], RefDirection[2], &rgba[index], hkl);
          }
          else if (r->crystruct[phase] == AIM::Reconstruction::Hexagonal)
          {
            OIMColoring::CalculateHexIPFColor(r->voxels[i].quat, RefDirection, &rgba[index]);
          }
          if (true == m_WriteBinaryFiles)
          {
            rgba[index + 3] = 255;
          }
          else
          {
            red = static_cast<float>(float(rgba[index]) / 255.0);green = static_cast<float> (float(rgba[index + 1]) / 255.0);
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
            fclose( f);
            return -1;
          }
        }
        // Clean up the allocated memory
        delete[] rgba;
      }

      fclose(f);
      return err;
    }




    /**
     * @brief Writes a VTK visualization file
     * @param Output file name
     * @return 0 on Success
     */
    template <typename T>
    int writeVisualizationFile(T* r, const std::string &file)
    {

      FILE* f = NULL;
      f = fopen(file.c_str(), "wb");
      if (NULL == f)
      {
        return 1;
      }
      // Write the correct header
      if (m_WriteBinaryFiles == true)
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
        WRITE_VTK_GRAIN_IDS_BINARY(r, AIM::VTK::GrainIdScalarName, voxels);
        WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::EuclideanScalarName, float, voxels, nearestneighbordistance[0])
        WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::PhaseIdScalarName, int, voxels, phase)
      }
      else
      {
        WRITE_VTK_GRAIN_IDS_ASCII(r, AIM::VTK::GrainIdScalarName, voxels)
        WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::EuclideanScalarName, float, voxels, nearestneighbordistance[0], "%f ")
        WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::PhaseIdScalarName, int, voxels, phase, "%d ")
      }

      fclose(f);
      return 0;
    }

    /**
     * @brief Writes a VTK visualization file with vector arrays for the Inverse
     * pole figure colors and grain ID.
     * @param Output file name
     * @return 0 on Success
     */
    template <typename T>
    int writeIPFVizFile(T* r, const std::string &file)
    {

      FILE* f = NULL;
      f = fopen(file.c_str(), "wb");
      if (NULL == f)
      {
        return 1;
      }

      size_t total = r->xpoints * r->ypoints * r->zpoints;
      unsigned char hkl[3] = { 0, 0, 0 };
      VTK_IPF_COLOR_REFDIRECTION(RefDirection)
      int phase;
      unsigned char* rgba = NULL;
      float red, green, blue;
      size_t index = 0;
      // Write the correct header
      if (true == m_WriteBinaryFiles)
      {
        WRITE_VTK_GRAIN_HEADER("BINARY", r)
        WRITE_VTK_GRAIN_IDS_BINARY(r, AIM::VTK::GrainIdScalarName, voxels);
        WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::PhaseIdScalarName, int, voxels, phase)
        // Write the COLOR_SCALARS
        fprintf(f, "COLOR_SCALARS IPF_Colors 4\n");
        rgba = new unsigned char[total * 4]; // We need the whole array because we build it and write it all at the end
      }
      else
      {
        WRITE_VTK_GRAIN_HEADER("ASCII", r)
        WRITE_VTK_GRAIN_IDS_ASCII(r, AIM::VTK::GrainIdScalarName, voxels);
        WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::PhaseIdScalarName, int, voxels, phase, "%d ")
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
          OIMColoring::GenerateIPFColor(r->voxels[i].euler1, r->voxels[i].euler2, r->voxels[i].euler3, RefDirection[0], RefDirection[1], RefDirection[2], &rgba[index], hkl);
        }
        else if (r->crystruct[phase] == AIM::Reconstruction::Hexagonal)
        {
          OIMColoring::CalculateHexIPFColor(r->voxels[i].quat, RefDirection, &rgba[index]);
        }
        if (true == m_WriteBinaryFiles)
        {
          rgba[index + 3] = 255;
        }
        else
        {
          red = static_cast<float>(float(rgba[index]) / 255.0);green = static_cast<float> (float(rgba[index + 1]) / 255.0);
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
          fclose( f);
          return -1;
        }
      }
      // Clean up the allocated memory
      delete[] rgba;

      fclose( f);
      return 0;

    }

    /**
     * @brief Writes a VTK visualization file with vector arrays for the disorientation colors and grain ID.
     * @param Output file name
     * @return 0 on Success
     */
    template <typename T>
    int writeDisorientationFile(T* r, const std::string &file)
    {

      FILE* f = NULL;
      f = fopen(file.c_str(), "wb");
      if (NULL == f)
      {
        return 1;
      }
      // Write the correct header
      if (m_WriteBinaryFiles == true)
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
        WRITE_VTK_GRAIN_IDS_BINARY(r, AIM::VTK::GrainIdScalarName, voxels);
        WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::KAMScalarName, float, voxels, kernelmisorientation)
        WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::GAMScalarName, float, voxels, grainmisorientation)
        WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::LMGScalarName, float, voxels, misorientationgradient)
      }
      else
      {
        WRITE_VTK_GRAIN_IDS_ASCII(r, AIM::VTK::GrainIdScalarName, voxels)
        WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::KAMScalarName, float, voxels, kernelmisorientation, "%f ")
        WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::GAMScalarName, float, voxels, grainmisorientation, "%f ")
        WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::LMGScalarName, float, voxels, misorientationgradient, "%f ")
      }
      fclose(f);
      return 0;

    }

    /**
     * @brief Writes a VTK visualization file with vector arrays for the Schmid Factor and grain ID.
     * @param Output file name
     * @return 0 on Success
     */
    template <typename T>
    int writeSchmidFactorVizFile(T* r, const std::string &file)
    {
      FILE* f = NULL;
      f = fopen(file.c_str(), "wb");
      if (NULL == f)
      {
        return 1;
      }
      // Write the correct header
      if (m_WriteBinaryFiles == true)
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
        WRITE_VTK_GRAIN_IDS_BINARY(r, AIM::VTK::GrainIdScalarName, voxels);
        WRITE_VTK_GRAIN_WITH_GRAIN_SCALAR_VALUE_BINARY(r, AIM::VTK::SchmidFactorScalarName, float, schmidfactor)
      }
      else
      {
        WRITE_VTK_GRAIN_IDS_ASCII(r, AIM::VTK::GrainIdScalarName, voxels)
          WRITE_VTK_GRAIN_WITH_GRAIN_SCALAR_VALUE_ASCII(r, AIM::VTK::SchmidFactorScalarName, float, schmidfactor, "%f ")
      }
      fclose(f);
      return 0;
    }

    /**
     * @brief Writes a VTK visualization file with vector arrays for the Image Quality and grain ID.
     * @param Output file name
     * @return 0 on Success
     */
    template <typename T>
    int writeImageQualityVizFile(T* r, const std::string &file)
    {

      FILE* f = NULL;
      f = fopen(file.c_str(), "wb");
      if (NULL == f)
      {
        return 1;
      }
      // Write the correct header
      if (m_WriteBinaryFiles == true)
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
        WRITE_VTK_GRAIN_IDS_BINARY(r, AIM::VTK::GrainIdScalarName, voxels);
        WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::ImageQualityScalarName, float, voxels, imagequality)
      }
      else
      {
        WRITE_VTK_GRAIN_IDS_ASCII(r, AIM::VTK::GrainIdScalarName, voxels)
        WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::ImageQualityScalarName, float, voxels, imagequality, "%f ")
      }
      fclose(f);
      return 0;

    }

    /**
     * @brief Writes a low resolution VTK visualization file.
     * @param Output file name
     * @return 0 on Success
     */
    template <typename T>
    int writeDownSampledVizFile(T* r, const std::string &file)
    {

      FILE* f = NULL;
      f = fopen(file.c_str(), "wb");
      if (NULL == f)
      {
        return 1;
      }
      int counter = 0;
      float x, y, z;
      float dsresx, dsresy, dsresz;
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
            fprintf(f, "%d ", r->voxels[index].grain_index);
            gnames[r->voxels[index].grain_index]++;
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



  protected:
    VTKFileWriters();

  private:
    VTKFileWriters(const VTKFileWriters&); // Copy Constructor Not Implemented
    void operator=(const VTKFileWriters&); // Operator '=' Not Implemented
};



#endif /* _VTKFILEWRITERS_HPP_ */
