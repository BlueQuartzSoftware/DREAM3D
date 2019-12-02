/* ============================================================================
 * This filter has been created by Krzysztof Glowinski (kglowinski at ymail.com).
 * It adapts the algorithm described in K.Glowinski, A.Morawiec, "Analysis of
 * experimental grain boundary distributions based on boundary-space metrics",
 * Metall. Mater. Trans. A 45, 3189-3194 (2014).
 * Besides the algorithm itself, many parts of the code come from
 * the sources of other filters, mainly "Find GBCD" and "Write GBCD Pole Figure (GMT5)".
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
 * @brief The FindGBPDMetricBased class. See [Filter documentation](@ref findgbpdmetricbased) for details.
 */
class OrientationAnalysis_EXPORT FindGBPDMetricBased : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindGBPDMetricBased SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindGBPDMetricBased)
  PYB11_FILTER_NEW_MACRO(FindGBPDMetricBased)
  PYB11_FILTER_PARAMETER(int, PhaseOfInterest)
  PYB11_FILTER_PARAMETER(float, LimitDist)
  PYB11_FILTER_PARAMETER(int, NumSamplPts)
  PYB11_FILTER_PARAMETER(bool, ExcludeTripleLines)
  PYB11_FILTER_PARAMETER(QString, DistOutputFile)
  PYB11_FILTER_PARAMETER(QString, ErrOutputFile)
  PYB11_FILTER_PARAMETER(bool, SaveRelativeErr)
  PYB11_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureEulerAnglesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceNormalsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceAreasArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFeatureFaceLabelsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, NodeTypesArrayPath)
  PYB11_PROPERTY(int PhaseOfInterest READ getPhaseOfInterest WRITE setPhaseOfInterest)
  PYB11_PROPERTY(float LimitDist READ getLimitDist WRITE setLimitDist)
  PYB11_PROPERTY(int NumSamplPts READ getNumSamplPts WRITE setNumSamplPts)
  PYB11_PROPERTY(bool ExcludeTripleLines READ getExcludeTripleLines WRITE setExcludeTripleLines)
  PYB11_PROPERTY(QString DistOutputFile READ getDistOutputFile WRITE setDistOutputFile)
  PYB11_PROPERTY(QString ErrOutputFile READ getErrOutputFile WRITE setErrOutputFile)
  PYB11_PROPERTY(bool SaveRelativeErr READ getSaveRelativeErr WRITE setSaveRelativeErr)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)
  PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceAreasArrayPath READ getSurfaceMeshFaceAreasArrayPath WRITE setSurfaceMeshFaceAreasArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFeatureFaceLabelsArrayPath READ getSurfaceMeshFeatureFaceLabelsArrayPath WRITE setSurfaceMeshFeatureFaceLabelsArrayPath)
  PYB11_PROPERTY(DataArrayPath NodeTypesArrayPath READ getNodeTypesArrayPath WRITE setNodeTypesArrayPath)
#endif

public:
  using Self = FindGBPDMetricBased;
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
   * @brief Returns the name of the class for FindGBPDMetricBased
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindGBPDMetricBased
   */
  static QString ClassName();

  ~FindGBPDMetricBased() override;

  /**
   * @brief Setter property for PhaseOfInterest
   */
  void setPhaseOfInterest(int value);
  /**
   * @brief Getter property for PhaseOfInterest
   * @return Value of PhaseOfInterest
   */
  int getPhaseOfInterest() const;

  Q_PROPERTY(int PhaseOfInterest READ getPhaseOfInterest WRITE setPhaseOfInterest)

  /**
   * @brief Setter property for LimitDist
   */
  void setLimitDist(float value);
  /**
   * @brief Getter property for LimitDist
   * @return Value of LimitDist
   */
  float getLimitDist() const;

  Q_PROPERTY(float LimitDist READ getLimitDist WRITE setLimitDist)

  /**
   * @brief Setter property for NumSamplPts
   */
  void setNumSamplPts(int value);
  /**
   * @brief Getter property for NumSamplPts
   * @return Value of NumSamplPts
   */
  int getNumSamplPts() const;

  Q_PROPERTY(int NumSamplPts READ getNumSamplPts WRITE setNumSamplPts)

  /**
   * @brief Setter property for ExcludeTripleLines
   */
  void setExcludeTripleLines(bool value);
  /**
   * @brief Getter property for ExcludeTripleLines
   * @return Value of ExcludeTripleLines
   */
  bool getExcludeTripleLines() const;

  Q_PROPERTY(bool ExcludeTripleLines READ getExcludeTripleLines WRITE setExcludeTripleLines)

  /**
   * @brief Setter property for DistOutputFile
   */
  void setDistOutputFile(const QString& value);
  /**
   * @brief Getter property for DistOutputFile
   * @return Value of DistOutputFile
   */
  QString getDistOutputFile() const;

  Q_PROPERTY(QString DistOutputFile READ getDistOutputFile WRITE setDistOutputFile)

  /**
   * @brief Setter property for ErrOutputFile
   */
  void setErrOutputFile(const QString& value);
  /**
   * @brief Getter property for ErrOutputFile
   * @return Value of ErrOutputFile
   */
  QString getErrOutputFile() const;

  Q_PROPERTY(QString ErrOutputFile READ getErrOutputFile WRITE setErrOutputFile)

  /**
   * @brief Setter property for SaveRelativeErr
   */
  void setSaveRelativeErr(bool value);
  /**
   * @brief Getter property for SaveRelativeErr
   * @return Value of SaveRelativeErr
   */
  bool getSaveRelativeErr() const;

  Q_PROPERTY(bool SaveRelativeErr READ getSaveRelativeErr WRITE setSaveRelativeErr)

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
   * @brief Setter property for SurfaceMeshFaceAreasArrayPath
   */
  void setSurfaceMeshFaceAreasArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceMeshFaceAreasArrayPath
   * @return Value of SurfaceMeshFaceAreasArrayPath
   */
  DataArrayPath getSurfaceMeshFaceAreasArrayPath() const;

  Q_PROPERTY(DataArrayPath SurfaceMeshFaceAreasArrayPath READ getSurfaceMeshFaceAreasArrayPath WRITE setSurfaceMeshFaceAreasArrayPath)

  /**
   * @brief Setter property for SurfaceMeshFeatureFaceLabelsArrayPath
   */
  void setSurfaceMeshFeatureFaceLabelsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceMeshFeatureFaceLabelsArrayPath
   * @return Value of SurfaceMeshFeatureFaceLabelsArrayPath
   */
  DataArrayPath getSurfaceMeshFeatureFaceLabelsArrayPath() const;

  Q_PROPERTY(DataArrayPath SurfaceMeshFeatureFaceLabelsArrayPath READ getSurfaceMeshFeatureFaceLabelsArrayPath WRITE setSurfaceMeshFeatureFaceLabelsArrayPath)

  /**
   * @brief Setter property for NodeTypesArrayPath
   */
  void setNodeTypesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for NodeTypesArrayPath
   * @return Value of NodeTypesArrayPath
   */
  DataArrayPath getNodeTypesArrayPath() const;

  Q_PROPERTY(DataArrayPath NodeTypesArrayPath READ getNodeTypesArrayPath WRITE setNodeTypesArrayPath)

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
  FindGBPDMetricBased();
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
  std::weak_ptr<DataArray<double>> m_SurfaceMeshFaceAreasPtr;
  double* m_SurfaceMeshFaceAreas = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_SurfaceMeshFaceLabelsPtr;
  int32_t* m_SurfaceMeshFaceLabels = nullptr;
  std::weak_ptr<DataArray<double>> m_SurfaceMeshFaceNormalsPtr;
  double* m_SurfaceMeshFaceNormals = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_SurfaceMeshFeatureFaceLabelsPtr;
  int32_t* m_SurfaceMeshFeatureFaceLabels = nullptr;
  std::weak_ptr<DataArray<int8_t>> m_NodeTypesPtr;
  int8_t* m_NodeTypes = nullptr;

  int m_PhaseOfInterest = {};
  float m_LimitDist = {};
  int m_NumSamplPts = {};
  bool m_ExcludeTripleLines = {};
  QString m_DistOutputFile = {};
  QString m_ErrOutputFile = {};
  bool m_SaveRelativeErr = {};
  DataArrayPath m_CrystalStructuresArrayPath = {};
  DataArrayPath m_FeatureEulerAnglesArrayPath = {};
  DataArrayPath m_FeaturePhasesArrayPath = {};
  DataArrayPath m_SurfaceMeshFaceLabelsArrayPath = {};
  DataArrayPath m_SurfaceMeshFaceNormalsArrayPath = {};
  DataArrayPath m_SurfaceMeshFaceAreasArrayPath = {};
  DataArrayPath m_SurfaceMeshFeatureFaceLabelsArrayPath = {};
  DataArrayPath m_NodeTypesArrayPath = {};

  void appendSamplPtsFixedZenith(QVector<float>* xVec, QVector<float>* yVec, QVector<float>* zVec, double theta, double minPhi, double maxPhi, double step);

  void appendSamplPtsFixedAzimuth(QVector<float>* xVec, QVector<float>* yVec, QVector<float>* zVec, double phi, double minTheta, double maxTheta, double step);

public:
  FindGBPDMetricBased(const FindGBPDMetricBased&) = delete;            // Copy Constructor Not Implemented
  FindGBPDMetricBased(FindGBPDMetricBased&&) = delete;                 // Move Constructor Not Implemented
  FindGBPDMetricBased& operator=(const FindGBPDMetricBased&) = delete; // Copy Assignment Not Implemented
  FindGBPDMetricBased& operator=(FindGBPDMetricBased&&) = delete;      // Move Assignment Not Implemented
};

