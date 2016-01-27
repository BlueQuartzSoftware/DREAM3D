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


#ifndef _DataContainerReader_H_
#define _DataContainerReader_H_

#include <QtCore/QDateTime>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"

/**
 * @brief The DataContainerReader class. See [Filter documentation](@ref datacontainerreader) for details.
 */
class SIMPLib_EXPORT DataContainerReader : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    SIMPL_SHARED_POINTERS(DataContainerReader)
    SIMPL_STATIC_NEW_MACRO(DataContainerReader)
    SIMPL_TYPE_MACRO_SUPER(DataContainerReader, AbstractFilter)

    virtual ~DataContainerReader();

    SIMPL_FILTER_PARAMETER(QString, InputFile)
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

    SIMPL_FILTER_PARAMETER(bool, OverwriteExistingDataContainers)
    Q_PROPERTY(bool OverwriteExistingDataContainers READ getOverwriteExistingDataContainers WRITE setOverwriteExistingDataContainers)

    SIMPL_FILTER_PARAMETER(QString, LastFileRead)
    Q_PROPERTY(QString LastFileRead READ getLastFileRead WRITE setLastFileRead)

    SIMPL_FILTER_PARAMETER(QDateTime, LastRead)
    Q_PROPERTY(QDateTime LastRead READ getLastRead WRITE setLastRead)

    SIMPL_FILTER_PARAMETER(DataContainerArrayProxy, InputFileDataContainerArrayProxy)
    Q_PROPERTY(DataContainerArrayProxy InputFileDataContainerArrayProxy READ getInputFileDataContainerArrayProxy WRITE setInputFileDataContainerArrayProxy)

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

    /**
     * @brief readExistingPipelineFromFile Reads the existing pipeline that is stored in the file and store it
     * in the class instance for later writing to another DREAM.3D data file
     * @param fileId HDF5 group Id for pipeline
     * @return Integer error value
     */
    int readExistingPipelineFromFile(hid_t fileId);

    /**
     * @brief writeExistingPipelineToFile Writes the filter parameters of the existing pipline to a
     * DREAM.3D file
     * @param writer AbstractFilterParametersWriter instance pointer
     * @param index Group index
     * @return Integer error value
     */
    int writeExistingPipelineToFile(AbstractFilterParametersWriter* writer, int index);

    /**
     * @brief readDataContainerArrayStructure Reads the structure of the DataContainerArray from the HDF5 based .dream3d file. For this method to work
     * the member variable for the file path should have been set prior to calling this method.
     * @param path The file path to the DREAM.3D file
     * @return DataContainerArrayProxy instance
     */
    DataContainerArrayProxy readDataContainerArrayStructure(const QString& path);

    /**
     * @brief readDataContainerBundles Reads the DataContainerBundles from the HDF5 based .dream3d file
     * @return Integer error value
     */
    int readDataContainerBundles(hid_t fileId, DataContainerArray::Pointer dca);

    /**
    * @brief syncProxies Combines the file and cached proxies if they are out-of-sync
    * @return
    */
    void syncProxies();

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
    DataContainerReader();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief readData Reads the actual data from the HDF5 based .dream3d file
     * @param preflight Boolean check for preflight status
     * @param proxy DataContainerArrayProxy reference
     * @param dca DataContainerArray instance pointer
     */
    void readData(bool preflight, DataContainerArrayProxy& proxy, DataContainerArray::Pointer dca);

  private:
    FilterPipeline::Pointer m_PipelineFromFile;

    DataContainerReader(const DataContainerReader&); // Copy Constructor Not Implemented
    void operator=(const DataContainerReader&); // Operator '=' Not Implemented
};

#endif /* _DataContainerReader_H_ */
