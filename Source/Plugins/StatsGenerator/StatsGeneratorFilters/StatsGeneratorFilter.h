/* ============================================================================
 * Copyright (c) 2015 BlueQuartz Softwae, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
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
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _statsgeneratorfilter_h_
#define _statsgeneratorfilter_h_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"

/**
 * @brief The StatsGeneratorFilter class. See [Filter documentation](@ref statsgeneratorfilter) for details.
 */
class StatsGeneratorFilter : public AbstractFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    SIMPL_SHARED_POINTERS(StatsGeneratorFilter)
    SIMPL_STATIC_NEW_MACRO(StatsGeneratorFilter)
    SIMPL_TYPE_MACRO_SUPER(StatsGeneratorFilter, AbstractFilter)

    virtual ~StatsGeneratorFilter();

    SIMPL_FILTER_PARAMETER(QString, StatsGeneratorDataContainerName)
    Q_PROPERTY(QString StatsGeneratorDataContainerName READ getStatsGeneratorDataContainerName WRITE setStatsGeneratorDataContainerName)

    SIMPL_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
    Q_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(QString, StatsDataArrayName)
    Q_PROPERTY(QString StatsDataArrayName READ getStatsDataArrayName WRITE setStatsDataArrayName)

    SIMPL_FILTER_PARAMETER(QString, CrystalStructuresArrayName)
    Q_PROPERTY(QString CrystalStructuresArrayName READ getCrystalStructuresArrayName WRITE setCrystalStructuresArrayName)

    SIMPL_FILTER_PARAMETER(QString, PhaseTypesArrayName)
    Q_PROPERTY(QString PhaseTypesArrayName READ getPhaseTypesArrayName WRITE setPhaseTypesArrayName)

    SIMPL_FILTER_PARAMETER(StatsDataArray::Pointer, StatsDataArray)
    Q_PROPERTY(StatsDataArray::Pointer StatsDataArray READ getStatsDataArray WRITE setStatsDataArray)

    SIMPL_FILTER_PARAMETER(UInt32ArrayType::Pointer, CrystalStructures)
    Q_PROPERTY(UInt32ArrayType::Pointer CrystalStructures READ getCrystalStructures WRITE setCrystalStructures)

    SIMPL_FILTER_PARAMETER(UInt32ArrayType::Pointer, PhaseTypes)
    Q_PROPERTY(UInt32ArrayType::Pointer PhaseTypes READ getPhaseTypes WRITE setPhaseTypes)

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
    StatsGeneratorFilter();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief readArray
     * @param arrayName
     * @param jsonKey
     * @return
     */
    void readArray(const QJsonObject& jsonRoot, size_t numTuples);

  private:
    StatsGeneratorFilter(const StatsGeneratorFilter&); // Copy Constructor Not Implemented
    void operator=(const StatsGeneratorFilter&); // Operator '=' Not Implemented
};

#endif /* _statsGeneratorFilter_H_ */
