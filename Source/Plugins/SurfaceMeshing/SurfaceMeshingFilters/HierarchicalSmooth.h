/* ============================================================================
 * Copyright (c) 2020-2020 BlueQuartz Software, LLC
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "SurfaceMeshing/SurfaceMeshingDLLExport.h"

/**
 * @brief The SurfaceMeshFilter class. This class serves as a superclass for other classes
 * in the SurfaceMeshing plugin.
 */
class SurfaceMeshing_EXPORT HierarchicalSmooth : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(HierarchicalSmooth SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(HierarchicalSmooth)
  PYB11_STATIC_NEW_MACRO(HierarchicalSmooth)
  PYB11_PROPERTY(DataArrayPath TriListPath READ getTriListPath WRITE setTriListPath)
  PYB11_PROPERTY(DataArrayPath VertexListPath READ getVertexListPath WRITE setVertexListPath)
  PYB11_PROPERTY(DataArrayPath FaceLabelsPath READ getFaceLabelsPath WRITE setFaceLabelsPath)
  PYB11_PROPERTY(DataArrayPath NodeTypePath READ getNodeTypePath WRITE setNodeTypePath)
  PYB11_PROPERTY(DataArrayPath SmoothedVertexListPath READ getSmoothedVertexListPath WRITE setSmoothedVertexListPath)
#endif

public:
  using Self = HierarchicalSmooth;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;

  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for CreateDataArray
   */
  QString getNameOfClass() const override;

  /**
   * @brief Returns the name of the class for CreateDataArray
   */
  static QString ClassName();

  ~HierarchicalSmooth() override;

  /**
   * @brief Getter property for TriListPath
   * @return
   */
  DataArrayPath getTriListPath() const;

  /**
   * @brief Setter property for TriListPath
   * @param value
   */
  void setTriListPath(const DataArrayPath& value);

  Q_PROPERTY(DataArrayPath TriListPath READ getTriListPath WRITE setTriListPath)

  /**
   * @brief Getter property for VertexListPath
   * @return
   */
  DataArrayPath getVertexListPath() const;

  /**
   * @brief Setter property for VertexListPath
   * @param value
   */
  void setVertexListPath(const DataArrayPath& value);

  Q_PROPERTY(DataArrayPath VertexListPath READ getVertexListPath WRITE setVertexListPath)

  /**
   * @brief Getter property for FaceLabelsPath
   * @return
   */
  DataArrayPath getFaceLabelsPath() const;

  /**
   * @brief Setter property for FaceLabelsPath
   * @param value
   */
  void setFaceLabelsPath(const DataArrayPath& value);

  Q_PROPERTY(DataArrayPath FaceLabelsPath READ getFaceLabelsPath WRITE setFaceLabelsPath)

  /**
   * @brief Getter property for NodeTypesPath
   * @return
   */
  DataArrayPath getNodeTypesPath() const;

  /**
   * @brief Setter property for NodeTypesPath
   * @param value
   */
  void setNodeTypesPath(const DataArrayPath& value);

  Q_PROPERTY(DataArrayPath NodeTypesPath READ getNodeTypesPath WRITE setNodeTypesPath)

  /**
   * @brief Getter property for SmoothedVertexListPath
   * @return
   */
  DataArrayPath getSmoothedVertexListPath() const;

  /**
   * @brief Setter property for SmoothedVertexListPath
   * @param value
   */
  void setSmoothedVertexListPath(const DataArrayPath& value);

  Q_PROPERTY(DataArrayPath SmoothedVertexListPath READ getSmoothedVertexListPath WRITE setSmoothedVertexListPath)

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
   * @brief This method will instantiate all the end user settable options/parameters
   * for this filter
   */
  void setupFilterParameters() override;

  /**
   * @brief This method will read the options from a file
   * @param reader The reader that is used to read the options from a file
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
   * @brief This function runs some sanity checks on the DataContainer and inputs
   * in an attempt to ensure the filter can process the inputs.
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
  HierarchicalSmooth();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  struct Impl;
  std::unique_ptr<Impl> p_Impl;

  DataArrayPath m_TriListPath;
  DataArrayPath m_VertexListPath;
  DataArrayPath m_FaceLabelsPath;
  DataArrayPath m_NodeTypesPath;
  DataArrayPath m_SmoothedVertexListPath;

public:
  HierarchicalSmooth(const HierarchicalSmooth&) = delete;            // Copy Constructor Not Implemented
  HierarchicalSmooth(HierarchicalSmooth&&) = delete;                 // Move Constructor Not Implemented
  HierarchicalSmooth& operator=(const HierarchicalSmooth&) = delete; // Copy Assignment Not Implemented
  HierarchicalSmooth& operator=(HierarchicalSmooth&&) = delete;      // Move Assignment Not Implemented
};
