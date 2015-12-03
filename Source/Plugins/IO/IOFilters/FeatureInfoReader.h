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


#ifndef _FeatureInfoReader_H_
#define _FeatureInfoReader_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/FileReader.h"

/**
 * @brief The FeatureInfoReader class. See [Filter documentation](@ref featureinforeader) for details.
 */
class  FeatureInfoReader : public FileReader
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    SIMPL_SHARED_POINTERS(FeatureInfoReader)
    SIMPL_STATIC_NEW_MACRO(FeatureInfoReader)
    SIMPL_TYPE_MACRO_SUPER(FeatureInfoReader, FileReader)

    virtual ~FeatureInfoReader();

    SIMPL_FILTER_PARAMETER(DataArrayPath, CellAttributeMatrixName)
    Q_PROPERTY(DataArrayPath CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(QString, CellFeatureAttributeMatrixName)
    Q_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)

    /* Input Parameters */
    SIMPL_FILTER_PARAMETER(QString, InputFile)
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

    SIMPL_FILTER_PARAMETER(bool, CreateCellLevelArrays)
    Q_PROPERTY(bool CreateCellLevelArrays READ getCreateCellLevelArrays WRITE setCreateCellLevelArrays)

    SIMPL_FILTER_PARAMETER(bool, RenumberFeatures)
    Q_PROPERTY(bool RenumberFeatures READ getRenumberFeatures WRITE setRenumberFeatures)

    SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    SIMPL_FILTER_PARAMETER(QString, CellPhasesArrayName)
    Q_PROPERTY(QString CellPhasesArrayName READ getCellPhasesArrayName WRITE setCellPhasesArrayName)

    SIMPL_FILTER_PARAMETER(QString, CellEulerAnglesArrayName)
    Q_PROPERTY(QString CellEulerAnglesArrayName READ getCellEulerAnglesArrayName WRITE setCellEulerAnglesArrayName)

    SIMPL_FILTER_PARAMETER(QString, FeaturePhasesArrayName)
    Q_PROPERTY(QString FeaturePhasesArrayName READ getFeaturePhasesArrayName WRITE setFeaturePhasesArrayName)

    SIMPL_FILTER_PARAMETER(QString, FeatureEulerAnglesArrayName)
    Q_PROPERTY(QString FeatureEulerAnglesArrayName READ getFeatureEulerAnglesArrayName WRITE setFeatureEulerAnglesArrayName)

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
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    virtual void preflight();

  protected:
    FeatureInfoReader();

    /**
     * @brief readHeader Reimplemented from @see FileReader class
     */
    virtual int32_t readHeader();

    /**
     * @brief readFile Reimplemented from @see FileReader class
     */
    virtual int32_t readFile();

    /**
     * @brief readFile Reimplemented from @see FileReader class
     */
    void dataCheck();

    /**
     * @brief updateFeatureInstancePointers Updates raw feature pointers
     */
    void updateFeatureInstancePointers();

  private:
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)

    DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_DATAARRAY_VARIABLE(float, CellEulerAngles)
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_DATAARRAY_VARIABLE(float, FeatureEulerAngles)

    FeatureInfoReader(const FeatureInfoReader&); //Not Implemented
    void operator=(const FeatureInfoReader&); //Not Implemented
};

#endif /* _FeatureInfoReader_h_ */
