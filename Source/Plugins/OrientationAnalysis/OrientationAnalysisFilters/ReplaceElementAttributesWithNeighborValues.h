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

class IDataArray;
using IDataArrayWkPtrType = std::weak_ptr<IDataArray>;

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The ReplaceElementAttributesWithNeighborValues class. See [Filter documentation](@ref ReplaceElementAttributesWithNeighborValues) for details.
 */
class OrientationAnalysis_EXPORT ReplaceElementAttributesWithNeighborValues : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(ReplaceElementAttributesWithNeighborValues SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(ReplaceElementAttributesWithNeighborValues)
  PYB11_FILTER_NEW_MACRO(ReplaceElementAttributesWithNeighborValues)
  PYB11_FILTER_PARAMETER(float, MinConfidence)
  PYB11_FILTER_PARAMETER(bool, Loop)
  PYB11_FILTER_PARAMETER(DataArrayPath, ConfidenceIndexArrayPath)
  PYB11_FILTER_PARAMETER(int, SelectedComparison)
  PYB11_FILTER_PARAMETER(QVector<DataArrayPath>, IgnoredDataArrayPaths)
  PYB11_PROPERTY(float MinConfidence READ getMinConfidence WRITE setMinConfidence)
  PYB11_PROPERTY(bool Loop READ getLoop WRITE setLoop)
  PYB11_PROPERTY(DataArrayPath ConfidenceIndexArrayPath READ getConfidenceIndexArrayPath WRITE setConfidenceIndexArrayPath)
  PYB11_PROPERTY(int SelectedComparison READ getSelectedComparison WRITE setSelectedComparison)
#endif

public:
  using Self = ReplaceElementAttributesWithNeighborValues;
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
   * @brief Returns the name of the class for ReplaceElementAttributesWithNeighborValues
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ReplaceElementAttributesWithNeighborValues
   */
  static QString ClassName();

  ~ReplaceElementAttributesWithNeighborValues() override;

  /**
   * @brief Setter property for MinConfidence
   */
  void setMinConfidence(float value);
  /**
   * @brief Getter property for MinConfidence
   * @return Value of MinConfidence
   */
  float getMinConfidence() const;

  Q_PROPERTY(float MinConfidence READ getMinConfidence WRITE setMinConfidence)

  /**
   * @brief Setter property for Loop
   */
  void setLoop(bool value);
  /**
   * @brief Getter property for Loop
   * @return Value of Loop
   */
  bool getLoop() const;

  Q_PROPERTY(bool Loop READ getLoop WRITE setLoop)

  /**
   * @brief Setter property for ConfidenceIndexArrayPath
   */
  void setConfidenceIndexArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for ConfidenceIndexArrayPath
   * @return Value of ConfidenceIndexArrayPath
   */
  DataArrayPath getConfidenceIndexArrayPath() const;

  Q_PROPERTY(DataArrayPath ConfidenceIndexArrayPath READ getConfidenceIndexArrayPath WRITE setConfidenceIndexArrayPath)

  /**
   * @brief Setter property for SelectedComparison
   */
  void setSelectedComparison(int value);
  /**
   * @brief Getter property for SelectedComparison
   * @return Value of SelectedComparison
   */
  int getSelectedComparison() const;

  Q_PROPERTY(int SelectedComparison READ getSelectedComparison WRITE setSelectedComparison)

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
  ReplaceElementAttributesWithNeighborValues();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  IDataArrayWkPtrType m_InArrayPtr;
  void* m_InArray = nullptr;

  float m_MinConfidence = {};
  bool m_Loop = {};
  DataArrayPath m_ConfidenceIndexArrayPath = {};
  int m_SelectedComparison = {};
  QVector<DataArrayPath> m_IgnoredDataArrayPaths = {};

public:
  ReplaceElementAttributesWithNeighborValues(const ReplaceElementAttributesWithNeighborValues&) = delete; // Copy Constructor Not Implemented
  ReplaceElementAttributesWithNeighborValues(ReplaceElementAttributesWithNeighborValues&&) = delete;      // Move Constructor Not Implemented
  ReplaceElementAttributesWithNeighborValues& operator=(const ReplaceElementAttributesWithNeighborValues&) = delete; // Copy Assignment Not Implemented
  ReplaceElementAttributesWithNeighborValues& operator=(ReplaceElementAttributesWithNeighborValues&&) = delete;      // Move Assignment Not Implemented
};

