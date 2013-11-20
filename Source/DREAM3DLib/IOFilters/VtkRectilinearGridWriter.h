/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef VTKRECTILINEARGRIDWRITER_H_
#define VTKRECTILINEARGRIDWRITER_H_

#include <QtCore/QString>

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/HKL/CtfConstants.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/VTKUtils/VTKFileWriters.hpp"


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
class DREAM3DLib_EXPORT VtkRectilinearGridWriter : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(VtkRectilinearGridWriter)
    DREAM3D_STATIC_NEW_MACRO(VtkRectilinearGridWriter)
    DREAM3D_TYPE_MACRO_SUPER(VtkRectilinearGridWriter, AbstractFilter)

    virtual ~VtkRectilinearGridWriter();
    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellFeatureAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellAttributeMatrixName)
    
   //------ Required Cell Data
                                    DREAM3D_INSTANCE_STRING_PROPERTY(FeatureReferenceRotationsArrayName)
                                    
    DREAM3D_INSTANCE_STRING_PROPERTY(OutputFile)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteFeatureIds)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteParentIds)
    DREAM3D_INSTANCE_PROPERTY(bool, WritePhaseIds)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteBandContrasts)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteConfidenceIndicies)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteImageQualities)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteGoodVoxels)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteGlobAlpha)
    //DREAM3D_INSTANCE_PROPERTY(bool, WriteRodriguesGAMColors)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteFeatureReferenceMisorientations)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteFeatureReferenceCAxisMisorientations)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteKernelAverageMisorientations)
    //DREAM3D_INSTANCE_PROPERTY(bool, WriteIPFColors)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteGBEuclideanDistanceMap)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteTJEuclideanDistanceMap)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteQPEuclideanDistanceMap)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteSchmidFactors)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteFeatureSizes)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteEulerAngles)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteBinaryFile)


    virtual void preflight();

    virtual const QString getGroupName() { return DREAM3D::FilterGroups::IOFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::OutputFilters; }
    virtual const QString getHumanLabel() { return "Write Vtk File (Rectilinear Grid)"; }

    virtual void setupFilterParameters();
    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void execute();

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
    static int WriteCoords(FILE* f, const char* axis, const char* type, int64_t npoints, T min, T max, T step, bool binary)
    {
      int err = 0;
      fprintf(f, "%s %lld %s\n", axis, npoints, type);
      if (binary == true)
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
     * @brief WriteDataArrayToFile
     * @param filename
     * @param data
     * @param dims
     * @param res
     * @param dataType
     * @param writeBinary
     * @return
     */
    template<typename T>
    static int WriteDataArrayToFile(const QString& filename, DataArray<T>* data, size_t* dims, float* res,
                                    const QString& dataType, bool writeBinary )
    {
      int err = 0;
      FILE* f = NULL;
      f = fopen(filename.toLatin1().data(), "wb");
      if(NULL == f)
      {
        qDebug() << "Could not open file for writing" ;
        qDebug() << "  FileName: " << filename ;
        qDebug() << "  Dataset Name: " << data->GetName() ;
        return -1;
      }
      // Write the correct header
      if(writeBinary == true)
      {
        WRITE_RECTILINEAR_GRID_HEADER("BINARY", NULL, (dims[0] + 1), (dims[1] + 1), (dims[2] + 1) )
      }
      else
      {
        WRITE_RECTILINEAR_GRID_HEADER("ASCII", NULL, (dims[0] + 1), (dims[1] + 1), (dims[2] + 1) )
      }

      // Write the XCoords
      VtkRectilinearGridWriter::WriteCoords(f, "X_COORDINATES", "float", dims[0] + 1, 0.0f - res[0] * 0.5f, (float)(dims[0] + 1 * res[0]), res[0], writeBinary);
      VtkRectilinearGridWriter::WriteCoords(f, "Y_COORDINATES", "float", dims[1] + 1, 0.0f - res[1] * 0.5f, (float)(dims[1] + 1 * res[1] ), res[1], writeBinary);
      VtkRectilinearGridWriter::WriteCoords(f, "Z_COORDINATES", "float", dims[2] + 1, 0.0f - res[2] * 0.5f, (float)(dims[2] + 1 * res[2]), res[2], writeBinary);

      size_t total = dims[0] * dims[1] * dims[2];
      int numComp = data->GetNumberOfComponents();
      fprintf(f, "CELL_DATA %d\n", (int)total);

      fprintf(f, "SCALARS %s %s %d\n", data->GetName().toLatin1().data(), dataType.toLatin1().data(), numComp);
      fprintf(f, "LOOKUP_TABLE default\n");
#ifdef DREAM3D_LITTLE_ENDIAN
      data->byteSwapElements();
#endif
      int64_t totalWritten = fwrite(data->getPointer(0), sizeof(T), (total * numComp), f);
      if (totalWritten != (total))
      {
        qDebug() << "Error Writing Binary VTK Data:" ;
        qDebug() << "  FileName: " << filename ;
        qDebug() << "  Dataset Name: " << data->GetName() ;
      }
#ifdef DREAM3D_LITTLE_ENDIAN
      data->byteSwapElements();
#endif
      // Close the file
      fclose(f);
      return err;
    }


  protected:
    VtkRectilinearGridWriter();

    void dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles);


  private:
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeatureIds)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, ParentIds)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, CellPhases)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, BC)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, GlobAlpha)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, ImageQuality)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, ConfidenceIndex)
    bool*    m_GoodVoxels;
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, CellEulerAngles)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, GBEuclideanDistances)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, TJEuclideanDistances)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, QPEuclideanDistances)
    float*   m_FeatureReferenceRotations;
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, FeatureReferenceMisorientations)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, FeatureReferenceCAxisMisorientations)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, KernelAverageMisorientations)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, EquivalentDiameters)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, Schmids)


    VtkRectilinearGridWriter(const VtkRectilinearGridWriter&); // Copy Constructor Not Implemented
    void operator=(const VtkRectilinearGridWriter&); // Operator '=' Not Implemented


    int write(const QString& file, VolumeDataContainer* r, std::vector<VtkScalarWriter*>& scalars);

};

#endif /* VTKRECTILINEARGRIDWRITER_H_ */
