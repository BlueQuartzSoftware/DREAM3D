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

#pragma once

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "Processing/ProcessingDLLExport.h"

/**
 * @brief The RemoveFlaggedFeatures class. See [Filter documentation](@ref removeflaggedfeatures) for details.
 */
class Processing_EXPORT RemoveFlaggedFeatures : public AbstractFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(RemoveFlaggedFeatures SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(bool FillRemovedFeatures READ getFillRemovedFeatures WRITE setFillRemovedFeatures)
    PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
    PYB11_PROPERTY(DataArrayPath FlaggedFeaturesArrayPath READ getFlaggedFeaturesArrayPath WRITE setFlaggedFeaturesArrayPath)
public:
  SIMPL_SHARED_POINTERS(RemoveFlaggedFeatures)
  SIMPL_FILTER_NEW_MACRO(RemoveFlaggedFeatures)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(RemoveFlaggedFeatures, AbstractFilter)

  ~RemoveFlaggedFeatures() override;

  SIMPL_FILTER_PARAMETER(bool, FillRemovedFeatures)
  Q_PROPERTY(bool FillRemovedFeatures READ getFillRemovedFeatures WRITE setFillRemovedFeatures)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FlaggedFeaturesArrayPath)
  Q_PROPERTY(DataArrayPath FlaggedFeaturesArrayPath READ getFlaggedFeaturesArrayPath WRITE setFlaggedFeaturesArrayPath)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  const QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  const QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  const QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
  * @brief preflight Reimplemented from @see AbstractFilter class
  */
  void preflight() override;

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
  RemoveFlaggedFeatures();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief assign_badpoints Coarsens those Features remaining in the structure after removing any flagged Features.
   * The coarsening is intended to fill gaps left by the removed Features and proceeds via an isotropic growth process.
   */
  void assign_badpoints();

  /**
   * @brief remove_flaggedfeatures Assigns a boolean value to Features dependent upon whether they meet
   * the supplied criterion for the minimum size.
   * @return QVector<bool> A vector of boolean values whose length is the number of Features.
   */
  QVector<bool> remove_flaggedfeatures();

private:
  int32_t* m_Neighbors;

  DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
  DEFINE_DATAARRAY_VARIABLE(bool, FlaggedFeatures)

public:
  RemoveFlaggedFeatures(const RemoveFlaggedFeatures&) = delete; // Copy Constructor Not Implemented
  RemoveFlaggedFeatures(RemoveFlaggedFeatures&&) = delete;      // Move Constructor Not Implemented
  RemoveFlaggedFeatures& operator=(const RemoveFlaggedFeatures&) = delete; // Copy Assignment Not Implemented
  RemoveFlaggedFeatures& operator=(RemoveFlaggedFeatures&&) = delete;      // Move Assignment Not Implemented
};

