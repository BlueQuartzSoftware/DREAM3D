/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
#ifndef _vtkrectilineargridwriter_h_
#define _vtkrectilineargridwriter_h_

#include <QtCore/QString>
#include <QtCore/QVector>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/VTKUtils/VTKWriterMacros.h"

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
class VtkRectilinearGridWriter : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    SIMPL_SHARED_POINTERS(VtkRectilinearGridWriter)
    SIMPL_STATIC_NEW_MACRO(VtkRectilinearGridWriter)
    SIMPL_TYPE_MACRO_SUPER(VtkRectilinearGridWriter, AbstractFilter)

    virtual ~VtkRectilinearGridWriter();

    SIMPL_FILTER_PARAMETER(QString, OutputFile)
    Q_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)

    SIMPL_FILTER_PARAMETER(bool, WriteBinaryFile)
    Q_PROPERTY(bool WriteBinaryFile READ getWriteBinaryFile WRITE setWriteBinaryFile)

    SIMPL_FILTER_PARAMETER(QVector<DataArrayPath>, SelectedDataArrayPaths)
    Q_PROPERTY(QVector<DataArrayPath> SelectedDataArrayPaths READ getSelectedDataArrayPaths WRITE setSelectedDataArrayPaths)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName();

    /**
     * @brief getBrandingString Returns the branding string for the filter, which is a tag
     * used to denote the filter's association with specific plugins
     * @return Branding string
    */
    virtual const QString getBrandingString();

    /**
     * @brief getFilterVersion Returns a version string for this filter. Default
     * value is an empty string.
     * @return
     */
    virtual const QString getFilterVersion();

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getGroupName();

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName();

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel();

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters();

    /**
     * @brief writeFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    virtual void preflight();

#if 0
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
#endif

  signals:
    /**
     * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
     * be pushed from a user-facing control (such as a widget)
     * @param filter Filter instance pointer
     */
    void updateFilterParameters(AbstractFilter* filter);

    /**
     * @brief parametersChanged Emitted when any Filter parameter is changed internally
     */
    void parametersChanged();

    /**
     * @brief preflightAboutToExecute Emitted just before calling dataCheck()
     */
    void preflightAboutToExecute();

    /**
     * @brief preflightExecuted Emitted just after calling dataCheck()
     */
    void preflightExecuted();

  protected:
    VtkRectilinearGridWriter();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

  private:
    QVector<IDataArray::WeakPointer> m_SelectedWeakPtrVector;

    VtkRectilinearGridWriter(const VtkRectilinearGridWriter&); // Copy Constructor Not Implemented
    void operator=(const VtkRectilinearGridWriter&); // Operator '=' Not Implemented

    void write(const QString& file);
};

#endif /* VtkRectilinearGridWriter_H_ */
