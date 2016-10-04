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
#ifndef _establishmatrixphase_h_
#define _establishmatrixphase_h_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/DataArrays/StringDataArray.hpp"


/**
 * @brief The EstablishMatrixPhase class. See [Filter documentation](@ref establishmatrixphase) for details.
 */
class EstablishMatrixPhase : public AbstractFilter
{
    Q_OBJECT
  public:
    SIMPL_SHARED_POINTERS(EstablishMatrixPhase)
    SIMPL_STATIC_NEW_MACRO(EstablishMatrixPhase)
    SIMPL_TYPE_MACRO_SUPER(EstablishMatrixPhase, AbstractFilter)

    virtual ~EstablishMatrixPhase();

    SIMPL_FILTER_PARAMETER(DataArrayPath, OutputCellAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath OutputCellAttributeMatrixPath READ getOutputCellAttributeMatrixPath WRITE setOutputCellAttributeMatrixPath)

    SIMPL_INSTANCE_STRING_PROPERTY(OutputCellFeatureAttributeMatrixName)
    Q_PROPERTY(QString OutputCellFeatureAttributeMatrixName READ getOutputCellFeatureAttributeMatrixName WRITE setOutputCellFeatureAttributeMatrixName)

    SIMPL_INSTANCE_STRING_PROPERTY(OutputCellEnsembleAttributeMatrixName)
    Q_PROPERTY(QString OutputCellEnsembleAttributeMatrixName READ getOutputCellEnsembleAttributeMatrixName WRITE setOutputCellEnsembleAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(QString, FeatureIdsArrayName)
    Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

    SIMPL_FILTER_PARAMETER(QString, CellPhasesArrayName)
    Q_PROPERTY(QString CellPhasesArrayName READ getCellPhasesArrayName WRITE setCellPhasesArrayName)

    SIMPL_FILTER_PARAMETER(QString, FeaturePhasesArrayName)
    Q_PROPERTY(QString FeaturePhasesArrayName READ getFeaturePhasesArrayName WRITE setFeaturePhasesArrayName)

    SIMPL_FILTER_PARAMETER(QString, NumFeaturesArrayName)
    Q_PROPERTY(QString NumFeaturesArrayName READ getNumFeaturesArrayName WRITE setNumFeaturesArrayName)

    SIMPL_FILTER_PARAMETER(DataArrayPath, MaskArrayPath)
    Q_PROPERTY(DataArrayPath MaskArrayPath READ getMaskArrayPath WRITE setMaskArrayPath)

    SIMPL_FILTER_PARAMETER(bool, UseMask)
    Q_PROPERTY(bool UseMask READ getUseMask WRITE setUseMask)

    SIMPL_FILTER_PARAMETER(DataArrayPath, InputStatsArrayPath)
    Q_PROPERTY(DataArrayPath InputStatsArrayPath READ getInputStatsArrayPath WRITE setInputStatsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, InputPhaseTypesArrayPath)
    Q_PROPERTY(DataArrayPath InputPhaseTypesArrayPath READ getInputPhaseTypesArrayPath WRITE setInputPhaseTypesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, InputPhaseNamesArrayPath)
    Q_PROPERTY(DataArrayPath InputPhaseNamesArrayPath READ getInputPhaseNamesArrayPath WRITE setInputPhaseNamesArrayPath)

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
    EstablishMatrixPhase();
    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();


    /**
     * @brief establish_matrix Fills a synthetic volume with the correct volume fraction
     * of matrix phase(s) based on the input statistics
     */
    void establish_matrix();

    std::vector<int32_t> matrixphases;
    std::vector<float> matrixphasefractions;

  private:
    size_t firstMatrixFeature;
    float sizex;
    float sizey;
    float sizez;
    float totalvol;

    // Cell Data - make sure these are all initialized to nullptr in the constructor
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_DATAARRAY_VARIABLE(bool, Mask)

    // Feature Data - make sure these are all initialized to nullptr in the constructor
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)

    // Ensemble Data - make sure these are all initialized to nullptr in the constructor
    DEFINE_DATAARRAY_VARIABLE(int32_t, NumFeatures)
    DEFINE_DATAARRAY_VARIABLE(uint32_t, PhaseTypes)
    DEFINE_STRINGARRAY_VARIABLE(PhaseNames)
    StatsDataArray::WeakPointer m_StatsDataArray;

    /**
     * @brief updateFeatureInstancePointers Resets the raw pointers that belong to a
     * Feature Attribute Matrix
     */
    void updateFeatureInstancePointers();

    EstablishMatrixPhase(const EstablishMatrixPhase&); // Copy Constructor Not Implemented
    void operator=(const EstablishMatrixPhase&); // Operator '=' Not Implemented
};

#endif /* EstablishMatrixPhase_H_ */



