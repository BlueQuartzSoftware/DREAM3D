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

#ifndef _VTKFILEWRITERS_HPP_
#define _VTKFILEWRITERS_HPP_

#include <string>

#include "MXA/Common/MXAEndian.h"

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DataContainer.h"
#include "DREAM3DLib/Common/EbsdColoring.hpp"
#include "DREAM3DLib/VTKUtils/VTKWriterMacros.h"



/**
 * @brief This is the SuperClass to implement if you want to write scalars to
 * a VTK Legacy File. The only method you need to implement is the writeScalars(FILE* f).
 */
class VtkScalarWriter
{
  public:
    VtkScalarWriter() : m_WriteBinaryFiles(true), m_ErrorMessage(""),m_ErrorCondition(0){}
    DREAM3D_TYPE_MACRO(VtkScalarWriter);

    virtual ~VtkScalarWriter(){}

    bool m_WriteBinaryFiles;
    DREAM3D_INSTANCE_STRING_PROPERTY(ErrorMessage);
    DREAM3D_INSTANCE_PROPERTY(int, ErrorCondition);

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
  VoxelGrainIdScalarWriter(T* r) : VtkScalarWriter(), r(r) {}
  DREAM3D_TYPE_MACRO_SUPER(VoxelGrainIdScalarWriter<T>, VtkScalarWriter)
  virtual ~VoxelGrainIdScalarWriter(){}


  int writeScalars(FILE* f)
  {
    int err = 0;
    std::string file;
    int64_t totalPoints = r->totalPoints();
    GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(r, Voxel, DREAM3D::VoxelData::GrainIds, Int32ArrayType, int32_t, (totalPoints), grain_indicies);

    if (m_WriteBinaryFiles == true) {
      WRITE_VTK_GRAIN_IDS_BINARY(r, DREAM3D::VTK::GrainIdScalarName);
    }
    else
    {
      WRITE_VTK_GRAIN_IDS_ASCII(r, DREAM3D::VTK::GrainIdScalarName)
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
#define VtkSCALARWRITER_CLASS_DEF(name, r, field, arrayName, scalarName, arrayType, type, format)\
template<typename T>\
class name : public VtkScalarWriter\
{\
  public:\
    name(T* r) : VtkScalarWriter(), r(r) {}\
    DREAM3D_TYPE_MACRO_SUPER(name<T>, VtkScalarWriter)\
    virtual ~name(){}\
    int writeScalars(FILE* f)  {\
      int err = 0;\
      std::string file;\
      int64_t totalPoints = r->totalPoints();\
      GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(r, field, arrayName, arrayType, type, (r->totalPoints()), var);\
      if (m_WriteBinaryFiles == true) {\
        WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, scalarName, type, var)\
      }    else    {\
        WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, scalarName, type, var, format)\
      }\
      return err;\
  }\
  private:\
    T* r;\
    name(const name&); \
    void operator=(const name&);\
};

#define VtkSCALARWRITER_CLASS_DEF_CHAR(name, r, field, arrayName, scalarName, arrayType, type, format)\
template<typename T>\
class name : public VtkScalarWriter\
{\
  public:\
    name(T* r) : VtkScalarWriter(), r(r) {}\
    DREAM3D_TYPE_MACRO_SUPER(name<T>, VtkScalarWriter)\
    virtual ~name(){}\
    int writeScalars(FILE* f)  {\
    int err = 0;\
    std::string file;\
    int64_t totalPoints = r->totalPoints();\
    GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(r, field, arrayName, arrayType, type, (r->totalPoints()), var);\
    if (m_WriteBinaryFiles == true) {\
      WRITE_VTK_SCALARS_FROM_VOXEL_BINARY_NOSWAP(r, scalarName, type, var)\
    }    else    {\
      WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, scalarName, type, var, format)\
    }\
    return err;\
  }\
  private:\
    T* r;\
    name(const name&); \
    void operator=(const name&);\
};


VtkSCALARWRITER_CLASS_DEF(VoxelPhaseIdScalarWriter, r, Voxel, DREAM3D::VoxelData::Phases, DREAM3D::VTK::PhaseIdScalarName, Int32ArrayType, int, "%d ")
VtkSCALARWRITER_CLASS_DEF_CHAR(VoxelGoodVoxelScalarWriter, r, Voxel, DREAM3D::VoxelData::GoodVoxels, DREAM3D::VTK::GoodVoxelScalarName, BoolArrayType, bool, "%d ")
VtkSCALARWRITER_CLASS_DEF(VoxelKAMScalarWriter, r, Voxel, DREAM3D::VoxelData::KernelAverageMisorientations, DREAM3D::VTK::KAMScalarName, FloatArrayType, float, "%f ")
VtkSCALARWRITER_CLASS_DEF_CHAR(VoxelSurfaceVoxelScalarWriter, r, Voxel, DREAM3D::VoxelData::SurfaceVoxels, DREAM3D::VTK::SurfaceVoxelScalarName, Int8ArrayType, char, "%d ")


/**
 * @brief This class will write the IPF colors to a Scalar array in the VTK file
 */
template<typename T>
class VoxelIPFColorScalarWriter : public VtkScalarWriter
{
  public:
  VoxelIPFColorScalarWriter(T* r) : VtkScalarWriter(), r(r) {}
  DREAM3D_TYPE_MACRO_SUPER(VoxelIPFColorScalarWriter<T>, VtkScalarWriter)\

  virtual ~VoxelIPFColorScalarWriter(){}

  int writeScalars(FILE* f)
  {
      int err = 0;
      size_t total = r->getXPoints() * r->getYPoints() * r->getZPoints();
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


      GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(r, Voxel, DREAM3D::VoxelData::Phases, Int32ArrayType, int32_t, (r->totalPoints()), phases);
      GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(r, Voxel, DREAM3D::VoxelData::Euler1, FloatArrayType, float, (r->totalPoints()), euler1s);
      GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(r, Voxel, DREAM3D::VoxelData::Euler2, FloatArrayType, float, (r->totalPoints()), euler2s);
      GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(r, Voxel, DREAM3D::VoxelData::Euler3, FloatArrayType, float, (r->totalPoints()), euler3s);

      // Write the IPF Coloring Cell Data
      for (size_t i = 0; i < total; i++)
      {
        phase = phases[i];
        if(true == m_WriteBinaryFiles)
        {
          index = i * 4;
        }
        else
        {
          index = 0;
        }
        if(phase > 0)
        {
          if(r->crystruct[phase] == Ebsd::Cubic)
          {
            EbsdColoring::GenerateIPFColor(euler1s[i], euler2s[i], euler3s[i], RefDirection[0], RefDirection[1], RefDirection[2], &rgba[index], hkl);
          }
          else if(r->crystruct[phase] == Ebsd::Hexagonal)
          {
            EbsdColoring::CalculateHexIPFColor(euler1s[i], euler2s[i], euler3s[i], RefDirection[0], RefDirection[1], RefDirection[2], &rgba[index]);
          }
        }
        else if(phase <= 0)
        {
          rgba[index] = 0.0;
          rgba[index + 1] = 0.0;
          rgba[index + 2] = 0.0;
        }
        if(true == m_WriteBinaryFiles)
        {
          rgba[index + 3] = 255;
        }
        else
        {
          red = static_cast<float>(float(rgba[index]) / 255.0);
          green = static_cast<float>(float(rgba[index + 1]) / 255.0);
          blue = static_cast<float>(float(rgba[index + 2]) / 255.0);
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
    DREAM3D_SHARED_POINTERS(VTKRectilinearGridFileWriter);
    DREAM3D_STATIC_NEW_MACRO(VTKRectilinearGridFileWriter);
    DREAM3D_TYPE_MACRO(VTKRectilinearGridFileWriter);

    VTKRectilinearGridFileWriter() : m_WriteBinaryFiles(false), m_ErrorMessage(""),m_ErrorCondition(0) {}
    virtual ~VTKRectilinearGridFileWriter() {}

    DREAM3D_INSTANCE_PROPERTY(bool, WriteBinaryFiles)
    DREAM3D_INSTANCE_STRING_PROPERTY(ErrorMessage);
    DREAM3D_INSTANCE_PROPERTY(int, ErrorCondition);

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
    int writeCoords(FILE* f, const char* axis, const char* type, int64_t npoints, T min, T max, T step)
    {
      int err = 0;
      fprintf(f, "%s %lld %s\n", axis, npoints, type);
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
        size_t totalWritten = fwrite(static_cast<void*>(data), sizeof(T), static_cast<size_t>(npoints), f);
        delete[] data;
        if (totalWritten != static_cast<size_t>(npoints) )
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
        return -1;
      }
      // Write the correct header
      if (m_WriteBinaryFiles == true)
      {
        WRITE_RECTILINEAR_GRID_HEADER("BINARY", r, r->getXPoints() + 1, r->getYPoints()+1, r->getZPoints()+1)
      }
      else
      {
        WRITE_RECTILINEAR_GRID_HEADER("ASCII", r, r->getXPoints() + 1, r->getYPoints()+1, r->getZPoints()+1)
      }

      // Write the XCoords
      writeCoords(f, "X_COORDINATES", "float", r->getXPoints() + 1, 0.0f - r->getXRes() * 0.5f, (float)(r->getXPoints() + 1 * r->getXRes()), r->getXRes());
      writeCoords(f, "Y_COORDINATES", "float", r->getYPoints() + 1, 0.0f - r->getYRes() * 0.5f, (float)(r->getYPoints() + 1 * r->getYRes()), r->getYRes());
      writeCoords(f, "Z_COORDINATES", "float", r->getZPoints() + 1, 0.0f - r->getZRes() * 0.5f, (float)(r->getZPoints() + 1 * r->getZRes()), r->getZRes());

      size_t total = r->getXPoints() * r->getYPoints() * r->getZPoints();
      fprintf(f, "CELL_DATA %d\n", (int)total);

      // Now loop on all of our Scalars and write those arrays as CELL_DATA
      for (typename std::vector<VtkScalarWriter*>::iterator iter = scalars.begin(); iter != scalars.end(); ++iter )
      {
        err = (*iter)->writeScalars(f);
        if (err < 0)
        {
          setErrorCondition((*iter)->getErrorCondition());
          setErrorMessage((*iter)->getErrorMessage());
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
    DREAM3D_SHARED_POINTERS(VTKStructuredPointsFileWriter);
    DREAM3D_STATIC_NEW_MACRO(VTKStructuredPointsFileWriter);
    DREAM3D_TYPE_MACRO(VTKStructuredPointsFileWriter);

    virtual ~VTKStructuredPointsFileWriter(){}

    DREAM3D_INSTANCE_PROPERTY(bool, WriteBinaryFiles)

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

      size_t total = r->getXPoints() * r->getYPoints() * r->getZPoints();
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
    DREAM3D_SHARED_POINTERS(VtkMiscFileWriter);
    DREAM3D_STATIC_NEW_MACRO(VtkMiscFileWriter);
    DREAM3D_TYPE_MACRO(VtkMiscFileWriter);

    virtual ~VtkMiscFileWriter(){}

    DREAM3D_INSTANCE_PROPERTY(bool, WriteBinaryFiles)
    DREAM3D_INSTANCE_STRING_PROPERTY(ErrorMessage);
    DREAM3D_INSTANCE_PROPERTY(int, ErrorCondition);

    /**
     * @brief Writes a VTK visualization file with vector arrays for the disorientation colors and grain ID.
     * @param Output file name
     * @return 0 on Success
     */
    template <typename T>
    int writeDisorientationFile(T* m, const std::string &file)
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
        WRITE_STRUCTURED_POINTS_HEADER("BINARY", m)
      }
      else
      {
        WRITE_STRUCTURED_POINTS_HEADER("ASCII", m)
      }
      int64_t totalPoints = m->totalPoints();
      GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(m, Voxel, DREAM3D::VoxelData::GrainIds, Int32ArrayType, int32_t, (totalPoints), grain_indicies);
      GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(m, Voxel, DREAM3D::VoxelData::KernelAverageMisorientations, FloatArrayType, float, (totalPoints), kernelmisorientation);
      GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(m, Voxel, DREAM3D::VoxelData::GrainMisorientations, FloatArrayType, float, (totalPoints), grainmisorientation);
      GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(m, Voxel, DREAM3D::VoxelData::MisorientationGradients, FloatArrayType, float, (totalPoints), misorientationgradient);

      size_t total = m->getXPoints() * m->getYPoints() * m->getZPoints();
      if (true == m_WriteBinaryFiles)
      {
        WRITE_VTK_GRAIN_IDS_BINARY(m, DREAM3D::VTK::GrainIdScalarName);
        WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(m, DREAM3D::VTK::KAMScalarName, float, kernelmisorientation)
        WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(m, DREAM3D::VTK::GAMScalarName, float, grainmisorientation)
        WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(m, DREAM3D::VTK::LMGScalarName, float, misorientationgradient)
      }
      else
      {
        WRITE_VTK_GRAIN_IDS_ASCII(m, DREAM3D::VTK::GrainIdScalarName)
        WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(m, DREAM3D::VTK::KAMScalarName, float, kernelmisorientation, "%f ")
        WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(m, DREAM3D::VTK::GAMScalarName, float, grainmisorientation, "%f ")
        WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(m, DREAM3D::VTK::LMGScalarName, float, misorientationgradient, "%f ")
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
    int writeSchmidFactorVizFile(T* m, const std::string &file)
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
        WRITE_STRUCTURED_POINTS_HEADER("BINARY", m)
      }
      else
      {
        WRITE_STRUCTURED_POINTS_HEADER("ASCII", m)
      }
      size_t totalPoints = m->totalPoints();
      GET_NAMED_ARRAY_SIZE_CHK_NOMSG_RET(m, Voxel, DREAM3D::VoxelData::GrainIds, Int32ArrayType, int32_t, (m->totalPoints()), grain_indicies);

      if (true == m_WriteBinaryFiles)
      {
        WRITE_VTK_GRAIN_IDS_BINARY(m, DREAM3D::VTK::GrainIdScalarName);
        WRITE_VTK_GRAIN_WITH_GRAIN_SCALAR_VALUE_BINARY(m, DREAM3D::VTK::SchmidFactorScalarName, float, schmidfactor)
      }
      else
      {
        WRITE_VTK_GRAIN_IDS_ASCII(m, DREAM3D::VTK::GrainIdScalarName)
        WRITE_VTK_GRAIN_WITH_GRAIN_SCALAR_VALUE_ASCII(m, DREAM3D::VTK::SchmidFactorScalarName, float, schmidfactor, "%f ")
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
