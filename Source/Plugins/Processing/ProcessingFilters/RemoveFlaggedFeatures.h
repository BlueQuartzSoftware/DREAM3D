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

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "Processing/ProcessingDLLExport.h"

/**
 * @brief The RemoveFlaggedFeatures class. See [Filter documentation](@ref removeflaggedfeatures) for details.
 */
class Processing_EXPORT RemoveFlaggedFeatures : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(RemoveFlaggedFeatures SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(RemoveFlaggedFeatures)
  PYB11_FILTER_NEW_MACRO(RemoveFlaggedFeatures)
  PYB11_FILTER_PARAMETER(bool, FillRemovedFeatures)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FlaggedFeaturesArrayPath)
  PYB11_FILTER_PARAMETER(QVector<DataArrayPath>, IgnoredDataArrayPaths)
  PYB11_PROPERTY(bool FillRemovedFeatures READ getFillRemovedFeatures WRITE setFillRemovedFeatures)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath FlaggedFeaturesArrayPath READ getFlaggedFeaturesArrayPath WRITE setFlaggedFeaturesArrayPath)
#endif

public:
  using Self = RemoveFlaggedFeatures;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  
  /**
   * @brief Returns a NullPointer wrapped by a shared_ptr<>
   * @return
   */
  static Pointer NullPointer();

  /**
   * @brief Creates a new object wrapped in a shared_ptr<>
   * @return
   */
  static Pointer New();

  /**
   * @brief Returns the name of the class for RemoveFlaggedFeatures
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for RemoveFlaggedFeatures
   */
  static QString ClassName();

  ~RemoveFlaggedFeatures() override;

  /**
   * @brief Setter property for FillRemovedFeatures
   */
  void setFillRemovedFeatures(bool value);
  /**
   * @brief Getter property for FillRemovedFeatures
   * @return Value of FillRemovedFeatures
   */
  bool getFillRemovedFeatures() const;

  Q_PROPERTY(bool FillRemovedFeatures READ getFillRemovedFeatures WRITE setFillRemovedFeatures)

  /**
   * @brief Setter property for FeatureIdsArrayPath
   */
  void setFeatureIdsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureIdsArrayPath
   * @return Value of FeatureIdsArrayPath
   */
  DataArrayPath getFeatureIdsArrayPath() const;

  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  /**
   * @brief Setter property for FlaggedFeaturesArrayPath
   */
  void setFlaggedFeaturesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FlaggedFeaturesArrayPath
   * @return Value of FlaggedFeaturesArrayPath
   */
  DataArrayPath getFlaggedFeaturesArrayPath() const;

  Q_PROPERTY(DataArrayPath FlaggedFeaturesArrayPath READ getFlaggedFeaturesArrayPath WRITE setFlaggedFeaturesArrayPath)

  /**
   * @brief Setter property for IgnoredDataArrayPaths
   */
  void setIgnoredDataArrayPaths(const QVector<DataArrayPath>& value);
  /**
   * @brief Getter property for IgnoredDataArrayPaths
   * @return Value of IgnoredDataArrayPaths
   */
  QVector<DataArrayPath> getIgnoredDataArrayPaths() const;

  Q_PROPERTY(QVector<DataArrayPath> IgnoredDataArrayPaths READ getIgnoredDataArrayPaths WRITE setIgnoredDataArrayPaths)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  QString getHumanLabel() const override;

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
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;
  std::weak_ptr<DataArray<bool>> m_FlaggedFeaturesPtr;
  bool* m_FlaggedFeatures = nullptr;

  bool m_FillRemovedFeatures = {};
  DataArrayPath m_FeatureIdsArrayPath = {};
  DataArrayPath m_FlaggedFeaturesArrayPath = {};
  QVector<DataArrayPath> m_IgnoredDataArrayPaths = {};

  int32_t* m_Neighbors;

public:
  RemoveFlaggedFeatures(const RemoveFlaggedFeatures&) = delete; // Copy Constructor Not Implemented
  RemoveFlaggedFeatures(RemoveFlaggedFeatures&&) = delete;      // Move Constructor Not Implemented
  RemoveFlaggedFeatures& operator=(const RemoveFlaggedFeatures&) = delete; // Copy Assignment Not Implemented
  RemoveFlaggedFeatures& operator=(RemoveFlaggedFeatures&&) = delete;      // Move Assignment Not Implemented
};

