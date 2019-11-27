/* ============================================================================
 * This filter has been created by Krzysztof Glowinski (kglowinski at ymail.com).
 * It computes 'approximate distances' to the nearest characteristic GBs.
 * Besides the calculation of the distances, many parts of the code come from
 * the sources of other filters, mainly "Find GBCD".
 * Therefore, the below copyright notice applies.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

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

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The FindDistsToCharactGBs class. See [Filter documentation](@ref finddiststocharactgbs) for details.
 */
class OrientationAnalysis_EXPORT FindDistsToCharactGBs : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindDistsToCharactGBs SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindDistsToCharactGBs)
  PYB11_FILTER_NEW_MACRO(FindDistsToCharactGBs)
  PYB11_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureEulerAnglesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceNormalsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, DistToTiltArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, DistToTwistArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, DistToSymmetricArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, DistTo180TiltArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)
  PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)
  PYB11_PROPERTY(DataArrayPath DistToTiltArrayPath READ getDistToTiltArrayPath WRITE setDistToTiltArrayPath)
  PYB11_PROPERTY(DataArrayPath DistToTwistArrayPath READ getDistToTwistArrayPath WRITE setDistToTwistArrayPath)
  PYB11_PROPERTY(DataArrayPath DistToSymmetricArrayPath READ getDistToSymmetricArrayPath WRITE setDistToSymmetricArrayPath)
  PYB11_PROPERTY(DataArrayPath DistTo180TiltArrayPath READ getDistTo180TiltArrayPath WRITE setDistTo180TiltArrayPath)
#endif

public:
  static const double INF_DIST;

  using Self = FindDistsToCharactGBs;
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
   * @brief Returns the name of the class for FindDistsToCharactGBs
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindDistsToCharactGBs
   */
  static QString ClassName();

  ~FindDistsToCharactGBs() override;

  /**
   * @brief Setter property for CrystalStructuresArrayPath
   */
  void setCrystalStructuresArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CrystalStructuresArrayPath
   * @return Value of CrystalStructuresArrayPath
   */
  DataArrayPath getCrystalStructuresArrayPath() const;

  Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

  /**
   * @brief Setter property for FeatureEulerAnglesArrayPath
   */
  void setFeatureEulerAnglesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureEulerAnglesArrayPath
   * @return Value of FeatureEulerAnglesArrayPath
   */
  DataArrayPath getFeatureEulerAnglesArrayPath() const;

  Q_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)

  /**
   * @brief Setter property for FeaturePhasesArrayPath
   */
  void setFeaturePhasesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeaturePhasesArrayPath
   * @return Value of FeaturePhasesArrayPath
   */
  DataArrayPath getFeaturePhasesArrayPath() const;

  Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

  /**
   * @brief Setter property for SurfaceMeshFaceLabelsArrayPath
   */
  void setSurfaceMeshFaceLabelsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceMeshFaceLabelsArrayPath
   * @return Value of SurfaceMeshFaceLabelsArrayPath
   */
  DataArrayPath getSurfaceMeshFaceLabelsArrayPath() const;

  Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

  /**
   * @brief Setter property for SurfaceMeshFaceNormalsArrayPath
   */
  void setSurfaceMeshFaceNormalsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceMeshFaceNormalsArrayPath
   * @return Value of SurfaceMeshFaceNormalsArrayPath
   */
  DataArrayPath getSurfaceMeshFaceNormalsArrayPath() const;

  Q_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)

  /**
   * @brief Setter property for DistToTiltArrayPath
   */
  void setDistToTiltArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for DistToTiltArrayPath
   * @return Value of DistToTiltArrayPath
   */
  DataArrayPath getDistToTiltArrayPath() const;

  Q_PROPERTY(DataArrayPath DistToTiltArrayPath READ getDistToTiltArrayPath WRITE setDistToTiltArrayPath)

  /**
   * @brief Setter property for DistToTwistArrayPath
   */
  void setDistToTwistArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for DistToTwistArrayPath
   * @return Value of DistToTwistArrayPath
   */
  DataArrayPath getDistToTwistArrayPath() const;

  Q_PROPERTY(DataArrayPath DistToTwistArrayPath READ getDistToTwistArrayPath WRITE setDistToTwistArrayPath)

  /**
   * @brief Setter property for DistToSymmetricArrayPath
   */
  void setDistToSymmetricArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for DistToSymmetricArrayPath
   * @return Value of DistToSymmetricArrayPath
   */
  DataArrayPath getDistToSymmetricArrayPath() const;

  Q_PROPERTY(DataArrayPath DistToSymmetricArrayPath READ getDistToSymmetricArrayPath WRITE setDistToSymmetricArrayPath)

  /**
   * @brief Setter property for DistTo180TiltArrayPath
   */
  void setDistTo180TiltArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for DistTo180TiltArrayPath
   * @return Value of DistTo180TiltArrayPath
   */
  DataArrayPath getDistTo180TiltArrayPath() const;

  Q_PROPERTY(DataArrayPath DistTo180TiltArrayPath READ getDistTo180TiltArrayPath WRITE setDistTo180TiltArrayPath)

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
  FindDistsToCharactGBs();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<DataArray<unsigned int>> m_CrystalStructuresPtr;
  unsigned int* m_CrystalStructures = nullptr;
  std::weak_ptr<DataArray<float>> m_FeatureEulerAnglesPtr;
  float* m_FeatureEulerAngles = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_FeaturePhasesPtr;
  int32_t* m_FeaturePhases = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_SurfaceMeshFaceLabelsPtr;
  int32_t* m_SurfaceMeshFaceLabels = nullptr;
  std::weak_ptr<DataArray<double>> m_SurfaceMeshFaceNormalsPtr;
  double* m_SurfaceMeshFaceNormals = nullptr;
  std::weak_ptr<DataArray<double>> m_DistToTiltPtr;
  double* m_DistToTilt = nullptr;
  std::weak_ptr<DataArray<double>> m_DistToTwistPtr;
  double* m_DistToTwist = nullptr;
  std::weak_ptr<DataArray<double>> m_DistToSymmetricPtr;
  double* m_DistToSymmetric = nullptr;
  std::weak_ptr<DataArray<double>> m_DistTo180TiltPtr;
  double* m_DistTo180Tilt = nullptr;

  DataArrayPath m_CrystalStructuresArrayPath = {};
  DataArrayPath m_FeatureEulerAnglesArrayPath = {};
  DataArrayPath m_FeaturePhasesArrayPath = {};
  DataArrayPath m_SurfaceMeshFaceLabelsArrayPath = {};
  DataArrayPath m_SurfaceMeshFaceNormalsArrayPath = {};
  DataArrayPath m_DistToTiltArrayPath = {};
  DataArrayPath m_DistToTwistArrayPath = {};
  DataArrayPath m_DistToSymmetricArrayPath = {};
  DataArrayPath m_DistTo180TiltArrayPath = {};

public:
  FindDistsToCharactGBs(const FindDistsToCharactGBs&) = delete;            // Copy Constructor Not Implemented
  FindDistsToCharactGBs(FindDistsToCharactGBs&&) = delete;                 // Move Constructor Not Implemented
  FindDistsToCharactGBs& operator=(const FindDistsToCharactGBs&) = delete; // Copy Assignment Not Implemented
  FindDistsToCharactGBs& operator=(FindDistsToCharactGBs&&) = delete;      // Move Assignment Not Implemented
};

