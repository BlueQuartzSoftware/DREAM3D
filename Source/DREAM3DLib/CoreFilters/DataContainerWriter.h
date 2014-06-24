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

#ifndef _DataContainerWriter_H_
#define _DataContainerWriter_H_

#include <QtCore/QString>
#include <QtCore/QtDebug>

#include <hdf5.h>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/Common/Constants.h"



/**
 * @class DataContainerWriter DataContainerWriter.h /IOFiltersFilters/DataContainerWriter.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT DataContainerWriter : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(DataContainerWriter)
    DREAM3D_STATIC_NEW_MACRO(DataContainerWriter)
    DREAM3D_TYPE_MACRO_SUPER(DataContainerWriter, AbstractFilter)

    virtual ~DataContainerWriter();

    DREAM3D_FILTER_PARAMETER(QString, OutputFile)
    Q_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)
    DREAM3D_INSTANCE_PROPERTY(bool, WritePipeline)
    DREAM3D_FILTER_PARAMETER(bool, WriteXdmfFile)
    Q_PROPERTY(bool WriteXdmfFile READ getWriteXdmfFile WRITE setWriteXdmfFile)


    virtual const QString getCompiledLibraryName();
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
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
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight();

    /**
     * @brief newFilterInstance Returns a new instance of the filter optionally copying the filter parameters from the
     * current filter to the new instance.
     * @param copyFilterParameters
     * @return
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

  signals:
    /**
     * @brief updateFilterParameters This is emitted when the filter requests all the latest Filter Parameters need to be
     * pushed from a user facing control such as the FilterParameter Widget
     * @param filter The filter to push the values into
     */
    void updateFilterParameters(AbstractFilter* filter);

    /**
     * @brief parametersChanged This signal can be emitted when any of the filter parameters are changed internally.
     */
    void parametersChanged();

    /**
     * @brief preflightAboutToExecute Emitted just before the dataCheck() is called. This can change if needed.
     */
    void preflightAboutToExecute();

    /**
     * @brief preflightExecuted Emitted just after the dataCheck() is called. Typically. This can change if needed.
     */
    void preflightExecuted();

  protected:
    DataContainerWriter();

    void dataCheck();

    /**
     * @brief Opens or Creates an HDF5 file to write data into
     * @param append Should a new file be created or append data to a currenlty existing file
     * @return
     */
    int openFile(bool append = false);

    /**
     * @brief Closes the currently open file
     * @return
     */
    int closeFile();

    int writePipeline();

    void writeXdmfHeader(QTextStream& out);
    void writeXdmfFooter(QTextStream& out);

  private:
    hid_t m_FileId;


    DataContainerWriter(const DataContainerWriter&); // Copy Constructor Not Implemented
    void operator=(const DataContainerWriter&); // Operator '=' Not Implemented

};

#endif /* _DATACONTAINERWRITER_H_ */


