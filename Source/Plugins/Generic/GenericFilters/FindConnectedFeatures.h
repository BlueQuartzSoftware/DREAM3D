/* ============================================================================
* Copyright (c) 2018 BlueQuartz Software, LLC
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
*    United States Air Force Prime Contract FA8650-15-D-5231
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _findconnectedfeatures_h_
#define _findconnectedfeatures_h_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"

/**
 * @brief The FindConnectedFeatures class. See [Filter documentation](@ref findconnectedfeatures) for details.
 */
class FindConnectedFeatures : public AbstractFilter
{
  Q_OBJECT

  public:
    SIMPL_SHARED_POINTERS(FindConnectedFeatures)
    SIMPL_STATIC_NEW_MACRO(FindConnectedFeatures)
    SIMPL_TYPE_MACRO_SUPER_OVERRIDE(FindConnectedFeatures, AbstractFilter)

    virtual ~FindConnectedFeatures();

    enum class AlgorithmTypeString : unsigned int
    {
      Feature = 0,
      Phase = 1
    };

    SIMPL_FILTER_PARAMETER(DataArrayPath, NeighborListArrayPath)
    Q_PROPERTY(DataArrayPath NeighborListArrayPath READ getNeighborListArrayPath WRITE setNeighborListArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, PhasesArrayPath)
    Q_PROPERTY(DataArrayPath PhasesArrayPath READ getPhasesArrayPath WRITE setPhasesArrayPath)

    SIMPL_FILTER_PARAMETER(QString, ConnectedFeaturesArrayName)
    Q_PROPERTY(QString ConnectedFeaturesArrayName READ getConnectedFeaturesArrayName WRITE setConnectedFeaturesArrayName)

    SIMPL_FILTER_PARAMETER(int, SelectedFeatureId)
    Q_PROPERTY(int SelectedFeatureId READ getSelectedFeatureId WRITE setSelectedFeatureId)

    SIMPL_FILTER_PARAMETER(int, SelectedPhaseId)
    Q_PROPERTY(int SelectedPhaseId READ getSelectedPhaseId WRITE setSelectedPhaseId)

    SIMPL_FILTER_PARAMETER(int, AlgorithmType)
    Q_PROPERTY(int AlgorithmType READ getAlgorithmType WRITE setAlgorithmType)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName() override;

    /**
     * @brief getBrandingString Returns the branding string for the filter, which is a tag
     * used to denote the filter's association with specific plugins
     * @return Branding string
    */
    virtual const QString getBrandingString() override;

    /**
     * @brief getFilterVersion Returns a version string for this filter. Default
     * value is an empty string.
     * @return
     */
    virtual const QString getFilterVersion() override;

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) override;

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getGroupName() override;

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName() override;

    /**
     * @brief getUuid Return the unique identifier for this filter.
     * @return A QUuid object.
     */
    virtual const QUuid getUuid() override;

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel() override;

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters() override;

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    virtual void execute() override;

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    virtual void preflight() override;

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
    FindConnectedFeatures();

    /**
    * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
    */
    void dataCheck();

    /**
    * @brief Initializes all the private instance variables.
    */
    void initialize();

  private:
    NeighborList<int>::WeakPointer  m_NeighborList;
    DEFINE_DATAARRAY_VARIABLE(int32_t, PhasesArray)
    DEFINE_DATAARRAY_VARIABLE(bool, ConnectedFeaturesArray)

    /**
     * @brief findConnectedFeaturesOfFeature
     */
    void findConnectedFeaturesOfFeature(int featureId);

    /**
     * @brief findConnectedFeaturesOfPhase
     */
    void findConnectedFeaturesOfPhase();

    FindConnectedFeatures(const FindConnectedFeatures&); // Copy Constructor Not Implemented
    void operator=(const FindConnectedFeatures&); // Operator '=' Not Implemented
};

#endif /* _FindConnectedFeatures_H_ */
