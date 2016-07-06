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


#ifndef _readangdata_h_
#define _readangdata_h_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/StringDataArray.hpp"

#include "EbsdLib/TSL/AngPhase.h"
#include "EbsdLib/TSL/AngReader.h"

// our PIMPL private class
class ReadAngDataPrivate;

/**
 * @brief The ReadAngData class. See [Filter documentation](@ref readangdata) for details.
 */
class ReadAngData : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
    Q_DECLARE_PRIVATE(ReadAngData)
  public:
    SIMPL_SHARED_POINTERS(ReadAngData)
    SIMPL_STATIC_NEW_MACRO(ReadAngData)
    SIMPL_TYPE_MACRO_SUPER(ReadAngData, AbstractFilter)

    virtual ~ReadAngData();

    SIMPL_FILTER_PARAMETER(QString, DataContainerName)

    Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)

    SIMPL_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
    Q_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(QString, CellAttributeMatrixName)
    Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(bool, FileWasRead)
    Q_PROPERTY(bool FileWasRead READ getFileWasRead)

    SIMPL_INSTANCE_STRING_PROPERTY(MaterialNameArrayName)

    SIMPL_FILTER_PARAMETER(QString, InputFile)
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

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

    /* These are non-exposed to the user through the GUI. Manual Pipelines are OK to set them */
    SIMPL_INSTANCE_PROPERTY(uint32_t, RefFrameZDir)

    SIMPL_INSTANCE_PROPERTY(int, Manufacturer)

    SIMPL_PIMPL_PROPERTY_DECL(QString, InputFile_Cache)

    SIMPL_PIMPL_PROPERTY_DECL(QDateTime, TimeStamp_Cache)

    SIMPL_PIMPL_PROPERTY_DECL(Ang_Private_Data, Data)
    Q_PROPERTY(Ang_Private_Data Data READ getData WRITE setData)

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

  public slots:
    /**
     * @brief flushCache Resets the cache file state
     */
    void flushCache();

  protected:
    ReadAngData();
    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();


    /**
    * @brief copyRawEbsdData Reads the Ang file and puts the data into the data container
     * @param reader AngReader instance pointer
     * @param tDims Tuple dimensions
     * @param cDims Component dimensions
     */
    void copyRawEbsdData(AngReader* reader, QVector<size_t>& tDims, QVector<size_t>& cDims);

    /**
    * @brief loadMaterialInfo Reads the values for the phase type, crystal structure
    * and precipitate fractions from the EBSD file
    * @param reader AngReader instance pointer
    * @return Integer error value
    */
    int32_t loadMaterialInfo(AngReader* reader);

    /**
     * @brief readDataFile Reads the Ang file
     * @param reader AngReader instance pointer
     * @param m DataContainer instance pointer
     * @param tDims Tuple dimensions
     */
    void readDataFile(AngReader* reader, DataContainer::Pointer m, QVector<size_t>& tDims, ANG_READ_FLAG = ANG_FULL_FILE);

  private:
    QScopedPointer<ReadAngDataPrivate> const d_ptr;

    DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_DATAARRAY_VARIABLE(float, CellEulerAngles)
    DEFINE_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)
    DEFINE_DATAARRAY_VARIABLE(float, LatticeConstants)

    ReadAngData(const ReadAngData&); // Copy Constructor Not Implemented
    void operator=(const ReadAngData&); // Operator '=' Not Implemented
};

#endif /* _ReadAngData_H_ */
