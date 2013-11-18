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

#include <QtCore/QString>
#include <QtCore/QtEndian>


#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/HKL/CtfConstants.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Utilities/DREAM3DEndian.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/VTKUtils/VTKWriterMacros.h"

#define GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(dataContainer, feature, name, typeClass, m_msgType, size, valuePtr) \
m_msgType* valuePtr = NULL;\
{\
  IDataArray::Pointer iDataArray = dataContainer->get##feature##Data(name);\
  if (iDataArray.get() == NULL) { \
    QString s = QObject::tr(": Array %1 from the DataContainer class was not in the DataContainer").arg(name);\
    setErrorCondition(-10);\
    addErrorMessage(getNameOfClass(), s, -10);\
    return -10;\
  } \
  if (static_cast<size_t>(size) != iDataArray->GetSize()) {\
    QString s = QObject::tr(": Array %1 from the DataContainer class did not have the correct number of elements.").arg(name);\
    setErrorCondition(-20);\
    addErrorMessage(getNameOfClass(), s, -20);\
    return -20;\
  }\
  valuePtr =\
  IDataArray::SafeReinterpretCast<IDataArray*, typeClass*, m_msgType* >(dataContainer->get##feature##Data(name).get());\
  if (NULL == valuePtr) {\
    QString s = QObject::tr(": Array %1 from the DataContainer class could not be cast to type ").arg(#m_msgType);\
    setErrorCondition(-30);\
    addErrorMessage(getNameOfClass(), s, -30);\
    return -30;\
  }\
}


/**
 * @brief This is the SuperClass to implement if you want to write scalars to
 * a VTK Legacy File. The only method you need to implement is the writeScalars(FILE* f).
 */
class VtkScalarWriter : public AbstractFilter
{
  public:
    VtkScalarWriter() : AbstractFilter(), m_WriteBinaryFiles(true), m_ErrorCondition(0) {}
    DREAM3D_TYPE_MACRO(VtkScalarWriter)

    virtual ~VtkScalarWriter() {}

    bool m_WriteBinaryFiles;
    DREAM3D_INSTANCE_PROPERTY(int, ErrorCondition)

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
class VoxelFeatureIdScalarWriter : public VtkScalarWriter
{
  public:
    VoxelFeatureIdScalarWriter(T* rPtr) : VtkScalarWriter(), r(rPtr) {}
    DREAM3D_TYPE_MACRO_SUPER(VoxelFeatureIdScalarWriter<T>, VtkScalarWriter)
    virtual ~VoxelFeatureIdScalarWriter() {}


    int writeScalars(FILE* f)
    {
      int err = 0;
      QString file;
      int64_t totalPoints = r->getTotalPoints();
      GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(r, Cell, DREAM3D::CellData::FeatureIds, Int32ArrayType, int32_t, (totalPoints), feature_indicies);

      if (m_WriteBinaryFiles == true)
      {
        WRITE_VTK_GRAIN_IDS_BINARY(r, DREAM3D::CellData::FeatureIds);
      }
      else
      {
        WRITE_VTK_GRAIN_IDS_ASCII(r, DREAM3D::CellData::FeatureIds, feature_indicies)
      }
      return err;
    }

  private:
    T* r;
    VoxelFeatureIdScalarWriter(const VoxelFeatureIdScalarWriter&); // Copy Constructor Not Implemented
    void operator=(const VoxelFeatureIdScalarWriter&); // Operator '=' Not Implemented
};

/**
 * @brief This macro can help you define the class needed to write a "Scalar" array
 * to the VTK file. This class is specifically setup for writing voxel based
 * properties to the VTK file
 */
#define VtkSCALARWRITER_CLASS_DEF(name, r, feature, arrayName, scalarName, arrayType, m_msgType, format)\
  template<typename T>\
  class name : public VtkScalarWriter\
  {\
    public:\
      name(T* r) : VtkScalarWriter(), r(r) {}\
      DREAM3D_TYPE_MACRO_SUPER(name<T>, VtkScalarWriter)\
      virtual ~name(){}\
      int writeScalars(FILE* f)  {\
        int err = 0;\
        QString file;\
        int64_t totalPoints = r->getTotalPoints();\
        GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(r, feature, arrayName, arrayType, m_msgType, totalPoints, var);\
        if (m_WriteBinaryFiles == true) {\
          WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(r, scalarName, m_msgType, var)\
        }    else    {\
          WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, scalarName, m_msgType, var, format)\
        }\
        return err;\
      }\
    private:\
      T* r;\
      name(const name&); \
      void operator=(const name&);\
  };

#define VtkSCALARWRITER_CLASS_DEF_FIELD(name, r, feature, arrayName, scalarName, arrayType, m_msgType, format)\
  template<typename T>\
  class name : public VtkScalarWriter\
  {\
    public:\
      name(T* r) : VtkScalarWriter(), r(r) {}\
      DREAM3D_TYPE_MACRO_SUPER(name<T>, VtkScalarWriter)\
      virtual ~name(){}\
      int writeScalars(FILE* f)  {\
        int err = 0;\
        QString file;\
        int64_t totalFeatures = r->getNumCellFeatureTuples();\
        GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(r, Cell##feature, arrayName, arrayType, m_msgType, totalFeatures, var);\
        int64_t totalPoints = r->getTotalPoints();\
        GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(r, Cell, DREAM3D::CellData::FeatureIds, Int32ArrayType, int32_t, (totalPoints), feature_indicies);\
        if (m_WriteBinaryFiles == true) {\
          WRITE_VTK_SCALARS_FROM_FIELD_BINARY(r, scalarName, m_msgType, var, feature_indicies)\
        }    else    {\
          WRITE_VTK_SCALARS_FROM_FIELD_ASCII(r, scalarName, m_msgType, var, feature_indicies, format)\
        }\
        return err;\
      }\
    private:\
      T* r;\
      name(const name&); \
      void operator=(const name&);\
  };

#define VtkSCALARWRITER_CLASS_DEF_CHAR(name, r, feature, arrayName, scalarName, arrayType, m_msgType, format)\
  template<typename T>\
  class name : public VtkScalarWriter\
  {\
    public:\
      name(T* rPtr) : VtkScalarWriter(), r(rPtr) {}\
      DREAM3D_TYPE_MACRO_SUPER(name<T>, VtkScalarWriter)\
      virtual ~name(){}\
      int writeScalars(FILE* f)  {\
        int err = 0;\
        QString file;\
        int64_t totalPoints = r->getTotalPoints();\
        GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(r, feature, arrayName, arrayType, m_msgType, totalPoints, var);\
        if (m_WriteBinaryFiles == true) {\
          WRITE_VTK_SCALARS_FROM_VOXEL_BINARY_NOSWAP(r, scalarName, m_msgType, var)\
        }    else    {\
          WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(r, scalarName, m_msgType, var, format)\
        }\
        return err;\
      }\
    private:\
      T* r;\
      name(const name&); \
      void operator=(const name&);\
  };


VtkSCALARWRITER_CLASS_DEF(VoxelParentIdScalarWriter, r, Cell, DREAM3D::CellData::ParentIds, DREAM3D::CellData::ParentIds, Int32ArrayType, int, "%d ")
VtkSCALARWRITER_CLASS_DEF(VoxelPhaseIdScalarWriter, r, Cell, DREAM3D::CellData::Phases, DREAM3D::CellData::Phases, Int32ArrayType, int, "%d ")
VtkSCALARWRITER_CLASS_DEF(VoxelBCScalarWriter, r, Cell, Ebsd::Ctf::BC, Ebsd::Ctf::BC, Int32ArrayType, int, "%d ")
VtkSCALARWRITER_CLASS_DEF(VoxelIQScalarWriter, r, Cell, DREAM3D::CellData::ImageQuality, DREAM3D::CellData::ImageQualityNoSpace, FloatArrayType, float, "%f ")
VtkSCALARWRITER_CLASS_DEF(VoxelCIScalarWriter, r, Cell, DREAM3D::CellData::ConfidenceIndex, DREAM3D::CellData::ConfidenceIndexNoSpace, FloatArrayType, float, "%f ")
VtkSCALARWRITER_CLASS_DEF_CHAR(VoxelGoodVoxelScalarWriter, r, Cell, DREAM3D::CellData::GoodVoxels, DREAM3D::CellData::GoodVoxels, BoolArrayType, char, "%d ")
VtkSCALARWRITER_CLASS_DEF(VoxelGlobAlphaScalarWriter, r, Cell, DREAM3D::CellData::GlobAlpha, DREAM3D::CellData::GlobAlpha, Int32ArrayType, int, "%d ")
VtkSCALARWRITER_CLASS_DEF(VoxelKernelAverageMisorientationScalarWriter, r, Cell, DREAM3D::CellData::KernelAverageMisorientations, DREAM3D::CellData::KernelAverageMisorientations, FloatArrayType, float, "%f ")
VtkSCALARWRITER_CLASS_DEF(VoxelFeatureReferenceMisorientationScalarWriter, r, Cell, DREAM3D::CellData::FeatureReferenceMisorientations, DREAM3D::CellData::FeatureReferenceMisorientations, FloatArrayType, float, "%f ")
VtkSCALARWRITER_CLASS_DEF(VoxelFeatureReferenceCAxisMisorientationScalarWriter, r, Cell, DREAM3D::CellData::FeatureReferenceCAxisMisorientations, DREAM3D::CellData::FeatureReferenceCAxisMisorientations, FloatArrayType, float, "%f ")
VtkSCALARWRITER_CLASS_DEF(VoxelGBEDMScalarWriter, r, Cell, DREAM3D::CellData::GBEuclideanDistances, DREAM3D::CellData::GBEuclideanDistances, FloatArrayType, float, "%f ")
VtkSCALARWRITER_CLASS_DEF(VoxelTJEDMScalarWriter, r, Cell, DREAM3D::CellData::TJEuclideanDistances, DREAM3D::CellData::TJEuclideanDistances, FloatArrayType, float, "%f ")
VtkSCALARWRITER_CLASS_DEF(VoxelQPEDMScalarWriter, r, Cell, DREAM3D::CellData::QPEuclideanDistances, DREAM3D::CellData::QPEuclideanDistances, FloatArrayType, float, "%f ")
VtkSCALARWRITER_CLASS_DEF_CHAR(VoxelSurfaceVoxelScalarWriter, r, Cell, DREAM3D::CellData::SurfaceVoxels, DREAM3D::CellData::SurfaceVoxels, Int8ArrayType, char, "%d ")
VtkSCALARWRITER_CLASS_DEF_FIELD(FeatureSizeScalarWriter, r, Feature, DREAM3D::FeatureData::EquivalentDiameters, DREAM3D::FeatureData::EquivalentDiameters, FloatArrayType, float, "%f ")
VtkSCALARWRITER_CLASS_DEF_FIELD(SchmidFactorScalarWriter, r, Feature, DREAM3D::FeatureData::Schmids, DREAM3D::FeatureData::Schmids, FloatArrayType, float, "%f ")

template<typename T>
class VoxelEulerAngleScalarWriter : public VtkScalarWriter
{
  public:
    VoxelEulerAngleScalarWriter(T* rPtr) : VtkScalarWriter(), r(rPtr) {}
    DREAM3D_TYPE_MACRO_SUPER(VoxelEulerAngleScalarWriter<T>, VtkScalarWriter)\

    virtual ~VoxelEulerAngleScalarWriter() {}

    int writeScalars(FILE* f)
    {
      int err = 0;
      int64_t totalPoints = r->getTotalPoints();
      size_t dims[3];
      r->getDimensions(dims);

      QVector<QString> names(3);
      names[0] = "Phi1";
      names[1] = "Phi";
      names[2] = "Phi2";

      GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(r, Cell, DREAM3D::CellData::EulerAngles, FloatArrayType, float, (3 * totalPoints), eulerangles);

      boost::shared_array<float> buffer(new float[dims[0]]);

      //std::vector<float> buffer(dims[0]);
      // Loop over each component of the Euler Angles
      for (int eIndex = 0; eIndex < 3; ++eIndex)
      {
        QString name = names[eIndex];
        fprintf(f, "SCALARS %s %s 1\n", name.toLatin1().data(), "float");
        fprintf(f, "LOOKUP_TABLE default\n");
        size_t index = 0;
        for(size_t z = 0; z < dims[2]; ++z)
        {
          for(size_t y = 0; y < dims[1]; ++y)
          {
            for(size_t x = 0; x < dims[0]; ++x)
            {
              index = (z * dims[0] * dims[1]) + (y * dims[0]) + x;
              buffer[x] = eulerangles[3 * index + eIndex];
              if (false == m_WriteBinaryFiles)
              {
                fprintf(f, "%f ", buffer[x]);
              }
            }
            // We just buffered an dim[0] worth of data, now write it out.
            if (true == m_WriteBinaryFiles)
            {
              float tmp = 0.0f;
              // VTK Binary files are written with Big Endian Byte Ordering
              for(size_t ii = 0; ii < dims[0]; ++ii)
              {
                tmp = buffer[ii];
                DREAM3D::Endian::FromSystemToBig::convert(tmp);
                buffer[ii] = tmp;
              }
              size_t totalWritten = fwrite( buffer.get(), sizeof(char), dims[0] * sizeof(float), f);
              if (totalWritten != dims[0] * 4)
              {
                qDebug() << "Error Writing Binary Data for Euler Angles to file " ;
                fclose(f);
                return -1;
              }
            }
            else
            {
              fprintf(f, "\n"); // Add a new line after each row.
            }
          }
        }
      }
      return err;
    }

  private:
    T* r;
    VoxelEulerAngleScalarWriter(const VoxelEulerAngleScalarWriter&); // Copy Constructor Not Implemented
    void operator=(const VoxelEulerAngleScalarWriter&); // Operator '=' Not Implemented


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
class VTKRectilinearGridFileWriter : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(VTKRectilinearGridFileWriter)
    DREAM3D_STATIC_NEW_MACRO(VTKRectilinearGridFileWriter)
    DREAM3D_TYPE_MACRO(VTKRectilinearGridFileWriter)

    VTKRectilinearGridFileWriter() : AbstractFilter(), m_WriteBinaryFiles(false), m_ErrorCondition(0) {}
    virtual ~VTKRectilinearGridFileWriter() {}

    DREAM3D_INSTANCE_PROPERTY(bool, WriteBinaryFiles)
    DREAM3D_INSTANCE_PROPERTY(int, ErrorCondition)

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
          DREAM3D::Endian::FromSystemToBig::convert(d);
          data[idx] = d;
        }
        size_t totalWritten = fwrite(static_cast<void*>(data), sizeof(T), static_cast<size_t>(npoints), f);
        delete[] data;
        if (totalWritten != static_cast<size_t>(npoints) )
        {
          qDebug() << "Error Writing Binary VTK Data into file " ;
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
    int write(const QString& file, T* r, std::vector<VtkScalarWriter*> scalars)
    {
      int err = 0;
      FILE* f = NULL;
      f = fopen(file.toLatin1().data(), "wb");
      if (NULL == f)
      {
        return -1;
      }
      // Write the correct header
      if (m_WriteBinaryFiles == true)
      {
        WRITE_RECTILINEAR_GRID_HEADER("BINARY", r, r->getXPoints() + 1, r->getYPoints() + 1, r->getZPoints() + 1)
      }
      else
      {
        WRITE_RECTILINEAR_GRID_HEADER("ASCII", r, r->getXPoints() + 1, r->getYPoints() + 1, r->getZPoints() + 1)
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
          addErrorMessages((*iter)->getPipelineMessages());
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
    DREAM3D_SHARED_POINTERS(VTKStructuredPointsFileWriter)
    DREAM3D_STATIC_NEW_MACRO(VTKStructuredPointsFileWriter)
    DREAM3D_TYPE_MACRO(VTKStructuredPointsFileWriter)

    virtual ~VTKStructuredPointsFileWriter() {}

    DREAM3D_INSTANCE_PROPERTY(bool, WriteBinaryFiles)

    template<typename T>
    int write(const QString& file, T* r, std::vector<VtkScalarWriter*> scalars)
    {
      int err = 0;
      FILE* f = NULL;
      f = fopen(file.toLatin1().data(), "wb");
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

      //size_t total = r->getXPoints() * r->getYPoints() * r->getZPoints();
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
class VtkMiscFileWriter : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(VtkMiscFileWriter)
    DREAM3D_STATIC_NEW_MACRO(VtkMiscFileWriter)
    DREAM3D_TYPE_MACRO(VtkMiscFileWriter)

    virtual ~VtkMiscFileWriter() {}

    DREAM3D_INSTANCE_PROPERTY(bool, WriteBinaryFiles)
    DREAM3D_INSTANCE_PROPERTY(int, ErrorCondition)

    /**
     * @brief Writes a VTK visualization file with vector arrays for the disorientation colors and feature ID.
     * @param Output file name
     * @return 0 on Success
     */
    template <typename T>
    int writeDisorientationFile(T* m, const QString& file)
    {

      FILE* f = NULL;
      f = fopen(file.toLatin1().data(), "wb");
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
      GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(m, Voxel, DREAM3D::CellData::FeatureIds, Int32ArrayType, int32_t, (totalPoints), feature_indicies);
      GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(m, Voxel, DREAM3D::CellData::KernelAverageMisorientations, FloatArrayType, float, (totalPoints), kernelmisorientation);
      GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(m, Voxel, DREAM3D::CellData::FeatureReferenceMisorientations, FloatArrayType, float, (totalPoints), featuremisorientation);
//      GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(m, Voxel, DREAM3D::CellData::MisorientationGradients, FloatArrayType, float, (totalPoints), misorientationgradient);

      //size_t total = m->getXPoints() * m->getYPoints() * m->getZPoints();
      if (true == m_WriteBinaryFiles)
      {
        WRITE_VTK_GRAIN_IDS_BINARY(m, DREAM3D::CellData::FeatureIds);
        WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(m, DREAM3D::FeatureData::KernelAvgMisorientations, float, kernelmisorientation)
        WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(m, DREAM3D::FeatureData::FeatureAvgMisorientations, float, featuremisorientation)
        //     WRITE_VTK_SCALARS_FROM_VOXEL_BINARY(m, DREAM3D::FeatureData::LMG, float, misorientationgradient)
      }
      else
      {
        WRITE_VTK_GRAIN_IDS_ASCII(m, DREAM3D::CellData::FeatureIds, feature_indicies)
        WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(m, DREAM3D::FeatureData::KernelAvgMisorientations, float, kernelmisorientation, "%f ")
        WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(m, DREAM3D::FeatureData::FeatureAvgMisorientations, float, featuremisorientation, "%f ")
//       WRITE_VTK_SCALARS_FROM_VOXEL_ASCII(m, DREAM3D::FeatureData::LMG, float, misorientationgradient, "%f ")
      }
      fclose(f);
      return 0;

    }

  protected:
    VtkMiscFileWriter() : AbstractFilter() { }

  private:
    VtkMiscFileWriter(const VtkMiscFileWriter&); // Copy Constructor Not Implemented
    void operator=(const VtkMiscFileWriter&); // Operator '=' Not Implemented
};


#endif /* _VTKFILEWRITERS_HPP_ */
