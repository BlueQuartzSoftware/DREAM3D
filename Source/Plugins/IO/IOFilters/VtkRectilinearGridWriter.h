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


#ifndef _VtkRectilinearGridWriter_H_
#define _VtkRectilinearGridWriter_H_

#include <QtCore/QString>

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/HKL/CtfConstants.h"
#include "EbsdLib/TSL/AngConstants.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "DREAM3DLib/DataContainers/DataContainer.h"
#include "DREAM3DLib/VTKUtils/VTKWriterMacros.h"

/**
 * @class VTKRectilinearGridFileWriter VTKRectilinearGridFileWriter.h DREAM3D/Common/VTKUtils/VTKRectilinearGridFileWriter.h
 * @brief This is the main class to call when you want to write voxel based data
 * to a Rectilinear Grid based VTK data set. In order to write Scalar attributes
 * one needs to also include a vector of VtkScalarWriter objects. Each VtkScalarWriter
 * subclass knows how to write a specific scalar array to the file. When implementing
 * those subclasses keep in mind that you should be able to write both the ASCII
 * and Binary versions of your data to the VTK File.
 *
 * @date Jun 13, 2011
 * @version 1.0
 */
class  VtkRectilinearGridWriter : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(VtkRectilinearGridWriter)
    DREAM3D_STATIC_NEW_MACRO(VtkRectilinearGridWriter)
    DREAM3D_TYPE_MACRO_SUPER(VtkRectilinearGridWriter, AbstractFilter)

    virtual ~VtkRectilinearGridWriter();

    DREAM3D_FILTER_PARAMETER(QString, OutputFile)
    Q_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)

    DREAM3D_FILTER_PARAMETER(bool, WriteBinaryFile)
    Q_PROPERTY(bool WriteBinaryFile READ getWriteBinaryFile WRITE setWriteBinaryFile)


    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath SelectedAttributeMatrixPath READ getSelectedAttributeMatrixPath WRITE setSelectedAttributeMatrixPath)

    virtual void preflight();

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();

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
#if CMP_SIZEOF_LONG == 8 && !defined (__APPLE__)
      fprintf(f, "%s %ld %s\n", axis, npoints, type);
#else
      fprintf(f, "%s %lld %s\n", axis, npoints, type);
#endif
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
        fprintf(f, "\n"); // Write a newline character at the end of the coordinates
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
        qDebug() << "  Dataset Name: " << data->getName() ;
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
      int numComp = data->getNumberOfComponents();
      fprintf(f, "CELL_DATA %d\n", (int)total);

      fprintf(f, "SCALARS %s %s %d\n", data->getName().toLatin1().data(), dataType.toLatin1().data(), numComp);
      fprintf(f, "LOOKUP_TABLE default\n");
#ifdef DREAM3D_LITTLE_ENDIAN
      data->byteSwapElements();
#endif
      int64_t totalWritten = fwrite(data->getPointer(0), sizeof(T), (total * numComp), f);
      if (totalWritten != (total))
      {
        qDebug() << "Error Writing Binary VTK Data:" ;
        qDebug() << "  FileName: " << filename ;
        qDebug() << "  Dataset Name: " << data->getName() ;
      }
#ifdef DREAM3D_LITTLE_ENDIAN
      data->byteSwapElements();
#endif
      // Close the file
      fclose(f);
      return err;
    }


  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    VtkRectilinearGridWriter();

    void dataCheck();


  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeatureIds)

    VtkRectilinearGridWriter(const VtkRectilinearGridWriter&); // Copy Constructor Not Implemented
    void operator=(const VtkRectilinearGridWriter&); // Operator '=' Not Implemented

    int write(const QString& file);
};

#endif /* VTKRECTILINEARGRIDWRITER_H_ */


