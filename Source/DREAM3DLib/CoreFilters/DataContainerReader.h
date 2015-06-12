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

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/DataContainers/DataContainerArrayProxy.h"

/**
 * @brief The DataContainerReader class. See [Filter documentation](@ref datacontainerreader) for details.
 */
class DREAM3DLib_EXPORT DataContainerReader : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(DataContainerReader)
    DREAM3D_STATIC_NEW_MACRO(DataContainerReader)
    DREAM3D_TYPE_MACRO_SUPER(DataContainerReader, AbstractFilter)

    virtual ~DataContainerReader();

    DREAM3D_FILTER_PARAMETER(QString, InputFile)
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

    DREAM3D_FILTER_PARAMETER(bool, OverwriteExistingDataContainers)
    Q_PROPERTY(bool OverwriteExistingDataContainers READ getOverwriteExistingDataContainers WRITE setOverwriteExistingDataContainers)

    DREAM3D_FILTER_PARAMETER(QString, LastFileRead)
    Q_PROPERTY(QString LastFileRead READ getLastFileRead WRITE setLastFileRead)

    DREAM3D_FILTER_PARAMETER(QDateTime, LastRead)
    Q_PROPERTY(QDateTime LastRead READ getLastRead WRITE setLastRead)

    DREAM3D_FILTER_PARAMETER(DataContainerArrayProxy, InputFileDataContainerArrayProxy)
    Q_PROPERTY(DataContainerArrayProxy InputFileDataContainerArrayProxy READ getInputFileDataContainerArrayProxy WRITE setInputFileDataContainerArrayProxy)


    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();

    FilterPipeline::Pointer getPipelinePointer() { return m_PipelineFromFile; }

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
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    DataContainerReader();

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