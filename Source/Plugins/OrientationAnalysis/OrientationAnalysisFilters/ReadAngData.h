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
#ifndef _ReadAngData_H_
#define _ReadAngData_H_

#include <QtCore/QString>
#include <QtCore/QScopedPointer>
#include <QtCore/QDateTime>

#include "EbsdLib/TSL/AngReader.h"
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataArrays/StringDataArray.hpp"
#include "DREAM3DLib/DataContainers/DataContainer.h"


#include "EbsdLib/TSL/AngPhase.h"


// our PIMPL private class
class ReadAngDataPrivate;

/**
* @class ReadOrientationData ReadOrientationData.h /FilterCategoryFilters/ReadOrientationData.h
* @brief
* @author
* @date
* @version 1.0
*/
class ReadAngData : public AbstractFilter
{

  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
    Q_DECLARE_PRIVATE(ReadAngData)

public:
  DREAM3D_SHARED_POINTERS(ReadAngData)
    DREAM3D_STATIC_NEW_MACRO(ReadAngData)
    DREAM3D_TYPE_MACRO_SUPER(ReadAngData, AbstractFilter)

    virtual ~ReadAngData();


  DREAM3D_FILTER_PARAMETER(QString, DataContainerName)

    Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)

    DREAM3D_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
    Q_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(QString, CellAttributeMatrixName)
    Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(bool, FileWasRead)
    Q_PROPERTY(bool FileWasRead READ getFileWasRead)

    DREAM3D_INSTANCE_STRING_PROPERTY(PhaseNameArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(MaterialNameArrayName)

    DREAM3D_FILTER_PARAMETER(QString, InputFile)
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getCompiledLibraryName();

    /**
     * @brief newFilterInstance
     * @param copyFilterParameters
     * @return
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

    /**
     * @brief getGroupName
     * @return
     */
    virtual const QString getGroupName();

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getSubGroupName();

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
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

    /* These are non-exposed to the user through the GUI. Manual Pipelines are OK to set them */
    DREAM3D_INSTANCE_PROPERTY(uint32_t, RefFrameZDir)

    void populateAngData(AngReader* reader, DataContainer::Pointer m, QVector<size_t> dims, ANG_READ_FLAG = ANG_FULL_FILE);

    DREAM3D_INSTANCE_PROPERTY(int, Manufacturer)

    DREAM3D_PIMPL_PROPERTY_DECL(QString, InputFile_Cache)
    DREAM3D_PIMPL_PROPERTY_DECL(QDateTime, TimeStamp_Cache)
    DREAM3D_PIMPL_PROPERTY_DECL(Ang_Private_Data, Data)
    Q_PROPERTY(Ang_Private_Data Data READ getData WRITE setData)


  signals:

    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  public slots:
    void flushCache();

  protected:
    ReadAngData();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck();

    /**
    * @brief copyRawEbsdData This reads the Ang file and puts the data into the Voxel Data container
     * @param reader
     * @param tDims
     * @param cDims
     */
    void copyRawEbsdData(AngReader* reader, QVector<size_t> &tDims, QVector<size_t> &cDims);

    /**
    * @brief This method reads the values for the phase type, crystal structure
    * and precipitate fractions from the EBSD file.
    * @param reader The EbsdReader instance
    * @return Zero/Positive on Success - Negative on error.
    */
    int loadMaterialInfo(AngReader* reader);

    /**
     * @brief readDataFile
     * @param reader
     * @param m
     * @param dims
     */
    void readDataFile(AngReader* reader, DataContainer::Pointer m, QVector<size_t>& tDims, ANG_READ_FLAG = ANG_FULL_FILE);

  private:
    QScopedPointer<ReadAngDataPrivate> const d_ptr;

    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, CellPhases)

    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, CellEulerAngles)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, LatticeConstants)

    ReadAngData(const ReadAngData&); // Copy Constructor Not Implemented

    void operator=(const ReadAngData&); // Operator '=' Not Implemented

};


#endif /* _ReadAngData_H_ */





