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

#ifndef _DataContainerReader_H_
#define _DataContainerReader_H_

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/DataContainers/DataContainerArrayProxy.h"
#include "DREAM3DLib/Common/Constants.h"

/**
 * @class DataContainerReader DataContainerReader.h DREAM3DLib/IOFilters/DataContainerReader.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jul 17, 2012
 * @version 1.0
 */
class DREAM3DLib_EXPORT DataContainerReader : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(DataContainerReader)
    DREAM3D_STATIC_NEW_MACRO(DataContainerReader)
    DREAM3D_TYPE_MACRO_SUPER(DataContainerReader, AbstractFilter)

    virtual ~DataContainerReader();

    QString getInputFile();
    void setInputFile(QString filePath);
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

    DREAM3D_FILTER_PARAMETER(bool, OverwriteExistingDataContainers)
    Q_PROPERTY(bool OverwriteExistingDataContainers READ getOverwriteExistingDataContainers WRITE setOverwriteExistingDataContainers)

    DataContainerArrayProxy getInputFileDataContainerArrayProxy();
    void setInputFileDataContainerArrayProxy(DataContainerArrayProxy proxy);
    Q_PROPERTY(DataContainerArrayProxy InputFileDataContainerArrayProxy READ getInputFileDataContainerArrayProxy WRITE setInputFileDataContainerArrayProxy)

	bool getIsProxyValid();
	void setIsProxyValid(bool valid);
	Q_PROPERTY(bool IsProxyValid READ getIsProxyValid WRITE setIsProxyValid)


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
     * @brief readExistingPipelineFromFile This will read the existing pipeline that is stored in the file and store it
     * in the class instance for later writing to another dream3d data file
     * @param fileId
     * @return
     */
    int readExistingPipelineFromFile(hid_t fileId);

    /**
     * @brief writeExistingPipelineToFile
     * @param writer
     * @param index
     * @return
     */
    int writeExistingPipelineToFile(AbstractFilterParametersWriter* writer, int index);

    /**
     * @brief Reads the structure of the DataContainer Array from the hdf5 based .dream3d file. For this method to work
     * the member variable for the file path should have been set prior to calling this method.
     * @param path The file path to the DREAm3D file
     * @return
     */
    DataContainerArrayProxy readDataContainerArrayStructure(const QString& path);

    /**
     * @brief readDataContainerBundles Reads the data Container bundles from the HDF5 file
     * @return
     */
    int readDataContainerBundles(hid_t fileId, DataContainerArray::Pointer dca);


  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();


  protected:
    DataContainerReader();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param volumes The number of volumes
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck();

    void readData(bool preflight, DataContainerArrayProxy& proxy, DataContainerArray::Pointer dca);

  private:
    DataContainerArrayProxy         m_InputFileDataContainerArrayProxy;

    QString                         m_InputFile;

    FilterPipeline::Pointer         m_PipelineFromFile;

    /**
     * @brief readDataContainers
     * @param dcArrayGroupId
     * @param proxy
     * @param h5InternalPath
     */
    void readDataContainers(hid_t dcArrayGroupId, DataContainerArrayProxy& proxy, QString h5InternalPath);

    /**
     * @brief readAttributeMatrix
     * @param containerId
     * @param dataContainer
     * @param h5InternalPath
     */
    void readAttributeMatrix(hid_t containerId, DataContainerProxy& dataContainer, QString h5InternalPath);

    /**
     * @brief readDataArrays
     * @param attrMatGid
     * @param attrMatrix
     * @param h5InternalPath
     */
    void readDataArrays(hid_t attrMatGid, AttributeMatrixProxy& attrMatrix, QString h5InternalPath);



    DataContainerReader(const DataContainerReader&); // Copy Constructor Not Implemented
    void operator=(const DataContainerReader&); // Operator '=' Not Implemented


};

#endif /* _DATACONTAINER_READER_H_ */



