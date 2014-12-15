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

#ifndef _VtkStructuredPointsReader_H_
#define _VtkStructuredPointsReader_H_

#include <string.h> // needed for the ::memcpy function below

#include <QtCore/QString>
#include <QtCore/QtEndian>
#include <QtCore/QFile>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Common/Constants.h"

/**
 * @brief The VtkStructuredPointsReader class
 * @author
 * @version
 */
class  VtkStructuredPointsReader : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(VtkStructuredPointsReader)
    DREAM3D_STATIC_NEW_MACRO(VtkStructuredPointsReader)
    DREAM3D_TYPE_MACRO_SUPER(VtkStructuredPointsReader, AbstractFilter)


    virtual ~VtkStructuredPointsReader();


    DREAM3D_FILTER_PARAMETER(bool, ReadCellData)
    Q_PROPERTY(bool ReadCellData READ getReadCellData WRITE setReadCellData)

    DREAM3D_FILTER_PARAMETER(QString, VolumeDataContainerName)
    Q_PROPERTY(QString VolumeDataContainerName READ getVolumeDataContainerName WRITE setVolumeDataContainerName)

    DREAM3D_FILTER_PARAMETER(QString, CellAttributeMatrixName)
    Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(bool, ReadPointData)
    Q_PROPERTY(bool ReadPointData READ getReadPointData WRITE setReadPointData)

    DREAM3D_FILTER_PARAMETER(QString, VertexDataContainerName)
    Q_PROPERTY(QString VertexDataContainerName READ getVertexDataContainerName WRITE setVertexDataContainerName)

    DREAM3D_FILTER_PARAMETER(QString, VertexAttributeMatrixName)
    Q_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(QString, InputFile)
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)


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
     * @brief preflight
     */
    virtual void preflight();


    // Normal Instance Variables that are not Filter Parameters
    DREAM3D_INSTANCE_STRING_PROPERTY(Comment)
    DREAM3D_INSTANCE_STRING_PROPERTY(DatasetType)
    DREAM3D_INSTANCE_PROPERTY(bool, FileIsBinary)


  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    VtkStructuredPointsReader();

    /**
     * @brief execute
     */
    void execute();

    /**
     * @brief dataCheck
     */
    void dataCheck();

    /**
     * @brief Reads the VTK header and sets the values that are described in the header
     * @return Error Condition. Negative is Error.
     */
    int readHeader();

    /**
     * @brief
     * @return Error Condition. Negative is Error.
     */
    virtual int readFile();

    /**
     * @brief readData
     * @param instream
     */
    virtual void readData(QFile &instream);

    /**
     *
     * @param input
     * @param value
     * @return Error Condition. Negative is Error.
     */
    int parseCoordinateLine(const char* input, size_t& value);

    /**
      * @brief Parses the byte size from a data set declaration line
      * @param text
      * @return
      */
    size_t parseByteSize(QString text);

  private:

    VtkStructuredPointsReader(const VtkStructuredPointsReader&); // Copy Constructor Not Implemented
    void operator=(const VtkStructuredPointsReader&); // Operator '=' Not Implemented
};

#endif /* VTKRECTILINEARGRIDREADER_H_ */


