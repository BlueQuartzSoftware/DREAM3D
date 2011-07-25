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



/**
 * @breif This is the SuperClass to implement if you want to write scalars to
 * a VTK Legacy File. The only method you need to implement is the writeScalars(FILE* f).
 */
class VtkScalarWriter
{
  public:
    VtkScalarWriter() : m_WriteBinaryFiles(true){}
    virtual ~VtkScalarWriter(){}

    bool m_WriteBinaryFiles;

    virtual int writeScalars(FILE* f)
    {
      int err = -1;
      return err;
    }

  protected:

  private:
    VtkScalarWriter(const VtkScalarWriter&); // Copy Constructor Not Implemented
    void operator=(const VtkScalarWriter&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
class VoxelGrainIdScalarWriter : public VtkScalarWriter
{
  public:
  VoxelGrainIdScalarWriter(T* r) : r(r) {}
  virtual ~VoxelGrainIdScalarWriter(){}

  int writeScalars(FILE* f)
  {
    int err = 0;
    std::string file;
    size_t total = r->xpoints * r->ypoints * r->zpoints;
    if (m_WriteBinaryFiles == true) {
      WRITE_VTK_GRAIN_IDS_BINARY(r, AIM::VTK::GrainIdScalarName);
    }
    else
    {
      WRITE_VTK_GRAIN_IDS_ASCII(r, AIM::VTK::GrainIdScalarName)
    }
    return err;
  }

  private:
    T* r;
    VoxelGrainIdScalarWriter(const VoxelGrainIdScalarWriter&); // Copy Constructor Not Implemented
    void operator=(const VoxelGrainIdScalarWriter&); // Operator '=' Not Implemented
};

/**
 * @brief This macro can help you define the class needed to write a "Scalar" array
 * to the VTK file. This class is specifically setup for writing voxel based
 * properties to the VTK file
 */
#define VtkSCALARWRITER_CLASS_DEF(name, r, const_name, type, scalar, format)\
template<typename T>\
class name : public VtkScalarWriter\
{\
  public:\
    name(T* r) : r(r) {}\
    virtual ~name(){}\
  int writeScalars(FILE* f)  {\
    int err = 0;\
    std::string file;\
    size_t total = r->xpoints * r->ypoints * r->zpoints;\
    if (m_WriteBinaryFiles == true) {\
      WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, const_name, type, scalar)\
    }    else    {\
      WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, const_name, type, scalar, format)\
    }\
    return err;\
  }\
  private:\
    T* r;\
    name(const name&); \
    void operator=(const name&);\
};\


VtkSCALARWRITER_CLASS_DEF(VoxelPhaseIdScalarWriter, r, AIM::VTK::PhaseIdScalarName, int, phases, "%d ")
// VtkSCALARWRITER_CLASS_DEF(VoxelEuclideanScalarWriter, r, AIM::VTK::EuclideanScalarName, float, nearestneighbordistances[0], "%f ")
VtkSCALARWRITER_CLASS_DEF(VoxelImageQualityScalarWriter, r, AIM::VTK::ImageQualityScalarName, float, imagequalities, "%f ")
VtkSCALARWRITER_CLASS_DEF(VoxelSurfaceVoxelScalarWriter, r, AIM::VTK::SurfaceVoxelScalarName, int, surfacevoxels, "%d ")
VtkSCALARWRITER_CLASS_DEF(VoxelKAMScalarWriter, r, AIM::VTK::KAMScalarName, float, kernelmisorientations, "%f ")

/**
 * @brief This class will write the IPF colors to a Scalar array in the VTK file
 */
template<typename T>
class VoxelIPFColorScalarWriter : public VtkScalarWriter
{
  public:
  VoxelIPFColorScalarWriter(T* r) : r(r) {}
  virtual ~VoxelIPFColorScalarWriter(){}

  int writeScalars(FILE* f)
  {
      int err = 0;
      size_t total = r->xpoints * r->ypoints * r->zpoints;
      unsigned char hkl[3] =
      { 0, 0, 0 };
      VTK_IPF_COLOR_REFDIRECTION(RefDirection)
      int phase;
      unsigned char* rgba = NULL;
      float red, green, blue;
      size_t index = 0;
      if (m_WriteBinaryFiles == true)
      {
        fprintf(f, "COLOR_SCALARS IPF_Colors 4\n");
        rgba = new unsigned char[total * 4]; // We need the whole array because we build it and write it all at the end
      }
      else
      {
        fprintf(f, "COLOR_SCALARS IPF_Colors 3\n");
        rgba = new unsigned char[4]; // We just need 4 bytes for ASCII writing
      }

      // Write the IPF Coloring Cell Data
	  float voxquat[5];
      for (size_t i = 0; i < total; i++)
      {
        phase = r->phases[i];
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
          OIMColoring::GenerateIPFColor(r->euler1s[i], r->euler2s[i], r->euler3s[i], RefDirection[0], RefDirection[1], RefDirection[2], &rgba[index], hkl);
        }
        else if (r->crystruct[phase] == AIM::Reconstruction::Hexagonal)
        {
		  voxquat[0] = r->quats[i][0];
		  voxquat[1] = r->quats[i][1];
		  voxquat[2] = r->quats[i][2];
		  voxquat[3] = r->quats[i][3];
		  voxquat[4] = r->quats[i][4];
          OIMColoring::CalculateHexIPFColor(voxquat, RefDirection, &rgba[index]);
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
          std::cout << "Error Writing Binary Data for IPF Colors to file " << std::endl;
          fclose( f);
          return -1;
        }
      }
      // Clean up the allocated memory
      delete[] rgba;

      return err;
    }

  private:
    T* r;
    VoxelIPFColorScalarWriter(const VoxelIPFColorScalarWriter&); // Copy Constructor Not Implemented
    void operator=(const VoxelIPFColorScalarWriter&); // Operator '=' Not Implemented
};



/**
 * @class VTKRectilinearGridFileWriter VTKRectilinearGridFileWriter.h DREAM3D/Common/VTKUtils/VTKRectilinearGridFileWriter.h
 * @brief This is the main class to call when you want to write voxel based data
 * to a Rectilinear Grid based VTK data set. In order to write Scalar attributes
 * one needs to also include a vector of VtkScalarWriter objects. Each VtkScalarWriter
 * subclass knows how to write a specific scalar array to the file. When implementing
 * those subclasses keep in mind that you should be able to write both the ASCII
 * and Binary versions of your data to the VTK File.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jun 13, 2011
 * @version 1.0
 */
class VTKRectilinearGridFileWriter
{
  public:
    MXA_SHARED_POINTERS(VTKRectilinearGridFileWriter);
    MXA_STATIC_NEW_MACRO(VTKRectilinearGridFileWriter);
    MXA_TYPE_MACRO(VTKRectilinearGridFileWriter);

    VTKRectilinearGridFileWriter() : m_WriteBinaryFiles(false) {}
    virtual ~VTKRectilinearGridFileWriter() {}

    MXA_INSTANCE_PROPERTY(bool, WriteBinaryFiles)

    /**
     * @brief This function writes a set of Axis coordinates to that are needed
     * for a Rectilinear Grid based data set.
     * @param f The "C" FILE* pointer to the file being written to.
     * @param axis The name of the Axis that is being written
     * @param type The type of primitive being written (float, int, ...)
     * @param npoints The total number of points in the array
     * @param min The minimum value of the axis
     * @param max The maximum value of the axis
     * @param step The step value between each point on the axis.
     */
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
        int totalWritten = fwrite(data, sizeof(T), npoints, f);
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
        T d;
        for (int idx = 0; idx < npoints; ++idx)
        {
          d = idx * step + min;
          fprintf(f, "%f ", d);
          if (idx % 20 == 0 && idx != 0) { fprintf(f, "\n"); }
        }
        fprintf(f, "\n");
      }
      return err;
    }

    /**
     * @brief This is the method to call to write the Rectilinear Grid Data to the
     * legacy VTK data file
     * @param file The name of the file to write
     * @param r The Template Parameter for a one of the Plugin Func classes.
     * @param scalars The Vector of Scalars that need to also be written
     * @return Negative Value on error
     */
    template<typename T>
    int write(const std::string &file, T* r, std::vector<VtkScalarWriter*> scalars)
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
      fprintf(f, "CELL_DATA %d\n", (int)total);

      // Now loop on all of our Scalars and write those arrays as CELL_DATA
      for (typename std::vector<VtkScalarWriter*>::iterator iter = scalars.begin(); iter != scalars.end(); ++iter )
      {
        err = (*iter)->writeScalars(f);
        if (err < 0)
        {
          break;
        }
      }
      // Close the file
      fclose(f);
      return err;
    }


  private:
    VTKRectilinearGridFileWriter(const VTKRectilinearGridFileWriter&); // Copy Constructor Not Implemented
    void operator=(const VTKRectilinearGridFileWriter&); // Operator '=' Not Implemented
};


/**
 *
 */
class VTKStructuredPointsFileWriter
{
  public:
    MXA_SHARED_POINTERS(VTKStructuredPointsFileWriter);
    MXA_STATIC_NEW_MACRO(VTKStructuredPointsFileWriter);
    MXA_TYPE_MACRO(VTKStructuredPointsFileWriter);

    virtual ~VTKStructuredPointsFileWriter(){}

    MXA_INSTANCE_PROPERTY(bool, WriteBinaryFiles)

    template<typename T>
    int write(const std::string &file, T* r, std::vector<VtkScalarWriter*> scalars)
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
        WRITE_STRUCTURED_POINTS_HEADER("BINARY", r)
      }
      else
      {
        WRITE_STRUCTURED_POINTS_HEADER("ASCII", r)
      }

      size_t total = r->xpoints * r->ypoints * r->zpoints;
      // Now loop on all of our Scalars and write those arrays as CELL_DATA
      for (typename std::vector<VtkScalarWriter*>::iterator iter = scalars.begin(); iter != scalars.end(); ++iter )
      {
        err = (*iter)->writeScalars(f);
        if (err < 0)
        {
          break;
        }
      }

      fclose(f);
      return err;
    }
  protected:
    VTKStructuredPointsFileWriter() {}

  private:
    VTKStructuredPointsFileWriter(const VTKStructuredPointsFileWriter&); // Copy Constructor Not Implemented
    void operator=(const VTKStructuredPointsFileWriter&); // Operator '=' Not Implemented
};


/**
 * @class VTKFileWriters VTKFileWriters.h DREAM3D/Common/VTKUtils/VTKFileWriters.h
 * @brief This class holds the functions to write the various VTK output files.
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Michael A. Groeber, PhD, US Air Force Research Laboratory
 * @date Feb 19, 2011
 * @version 1.0
 */
class VtkMiscFileWriter
{
  public:
    MXA_SHARED_POINTERS(VtkMiscFileWriter);
    MXA_STATIC_NEW_MACRO(VtkMiscFileWriter);
    MXA_TYPE_MACRO(VtkMiscFileWriter);

    virtual ~VtkMiscFileWriter(){}

    MXA_INSTANCE_PROPERTY(bool, WriteBinaryFiles)

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
        WRITE_STRUCTURED_POINTS_HEADER("BINARY", r)
      }
      else
      {
        WRITE_STRUCTURED_POINTS_HEADER("ASCII", r)
      }
      size_t total = r->xpoints * r->ypoints * r->zpoints;
      if (true == m_WriteBinaryFiles)
      {
        WRITE_VTK_GRAIN_IDS_BINARY(r, AIM::VTK::GrainIdScalarName);
        WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::KAMScalarName, float, kernelmisorientation)
        WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::GAMScalarName, float, grainmisorientation)
        WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, AIM::VTK::LMGScalarName, float, misorientationgradient)
      }
      else
      {
        WRITE_VTK_GRAIN_IDS_ASCII(r, AIM::VTK::GrainIdScalarName)
        WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::KAMScalarName, float, kernelmisorientation, "%f ")
        WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::GAMScalarName, float, grainmisorientation, "%f ")
        WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, AIM::VTK::LMGScalarName, float, misorientationgradient, "%f ")
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
        WRITE_STRUCTURED_POINTS_HEADER("BINARY", r)
      }
      else
      {
        WRITE_STRUCTURED_POINTS_HEADER("ASCII", r)
      }
      size_t total = r->xpoints * r->ypoints * r->zpoints;
      if (true == m_WriteBinaryFiles)
      {
        WRITE_VTK_GRAIN_IDS_BINARY(r, AIM::VTK::GrainIdScalarName);
        WRITE_VTK_GRAIN_WITH_GRAIN_SCALAR_VALUE_BINARY(r, AIM::VTK::SchmidFactorScalarName, float, schmidfactor)
      }
      else
      {
        WRITE_VTK_GRAIN_IDS_ASCII(r, AIM::VTK::GrainIdScalarName)
          WRITE_VTK_GRAIN_WITH_GRAIN_SCALAR_VALUE_ASCII(r, AIM::VTK::SchmidFactorScalarName, float, schmidfactor, "%f ")
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
            fprintf(f, "%d ", r->grain_indicies[index]);
            gnames[r->grain_indicies[index]]++;
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
    VtkMiscFileWriter(){}

  private:
    VtkMiscFileWriter(const VtkMiscFileWriter&); // Copy Constructor Not Implemented
    void operator=(const VtkMiscFileWriter&); // Operator '=' Not Implemented
};



#endif /* _VTKFILEWRITERS_HPP_ */
