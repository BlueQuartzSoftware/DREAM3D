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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

class LaueOps;
using LaueOpsShPtrType = std::shared_ptr<LaueOps>;
using LaueOpsContainer = std::vector<LaueOpsShPtrType>;

/**
 * @brief The FindTwinBoundaries class. See [Filter documentation](@ref findtwinboundaries) for details.
 */
class OrientationAnalysis_EXPORT FindTwinBoundaries : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(FindTwinBoundaries SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(FindTwinBoundaries)
  PYB11_FILTER_NEW_MACRO(FindTwinBoundaries)
  PYB11_PROPERTY(float AxisTolerance READ getAxisTolerance WRITE setAxisTolerance)
  PYB11_PROPERTY(float AngleTolerance READ getAngleTolerance WRITE setAngleTolerance)
  PYB11_PROPERTY(bool FindCoherence READ getFindCoherence WRITE setFindCoherence)
  PYB11_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)
  PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)
  PYB11_PROPERTY(QString SurfaceMeshTwinBoundaryArrayName READ getSurfaceMeshTwinBoundaryArrayName WRITE setSurfaceMeshTwinBoundaryArrayName)
  PYB11_PROPERTY(QString SurfaceMeshTwinBoundaryIncoherenceArrayName READ getSurfaceMeshTwinBoundaryIncoherenceArrayName WRITE setSurfaceMeshTwinBoundaryIncoherenceArrayName)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = FindTwinBoundaries;
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
   * @brief Returns the name of the class for FindTwinBoundaries
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindTwinBoundaries
   */
  static QString ClassName();

  ~FindTwinBoundaries() override;

  /**
   * @brief Setter property for AxisTolerance
   */
  void setAxisTolerance(float value);
  /**
   * @brief Getter property for AxisTolerance
   * @return Value of AxisTolerance
   */
  float getAxisTolerance() const;
  Q_PROPERTY(float AxisTolerance READ getAxisTolerance WRITE setAxisTolerance)

  /**
   * @brief Setter property for AngleTolerance
   */
  void setAngleTolerance(float value);
  /**
   * @brief Getter property for AngleTolerance
   * @return Value of AngleTolerance
   */
  float getAngleTolerance() const;
  Q_PROPERTY(float AngleTolerance READ getAngleTolerance WRITE setAngleTolerance)

  /**
   * @brief Setter property for FindCoherence
   */
  void setFindCoherence(bool value);
  /**
   * @brief Getter property for FindCoherence
   * @return Value of FindCoherence
   */
  bool getFindCoherence() const;
  Q_PROPERTY(bool FindCoherence READ getFindCoherence WRITE setFindCoherence)

  /**
   * @brief Setter property for AvgQuatsArrayPath
   */
  void setAvgQuatsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for AvgQuatsArrayPath
   * @return Value of AvgQuatsArrayPath
   */
  DataArrayPath getAvgQuatsArrayPath() const;
  Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

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
   * @brief Setter property for SurfaceMeshTwinBoundaryArrayName
   */
  void setSurfaceMeshTwinBoundaryArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshTwinBoundaryArrayName
   * @return Value of SurfaceMeshTwinBoundaryArrayName
   */
  QString getSurfaceMeshTwinBoundaryArrayName() const;
  Q_PROPERTY(QString SurfaceMeshTwinBoundaryArrayName READ getSurfaceMeshTwinBoundaryArrayName WRITE setSurfaceMeshTwinBoundaryArrayName)

  /**
   * @brief Setter property for SurfaceMeshTwinBoundaryIncoherenceArrayName
   */
  void setSurfaceMeshTwinBoundaryIncoherenceArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshTwinBoundaryIncoherenceArrayName
   * @return Value of SurfaceMeshTwinBoundaryIncoherenceArrayName
   */
  QString getSurfaceMeshTwinBoundaryIncoherenceArrayName() const;
  Q_PROPERTY(QString SurfaceMeshTwinBoundaryIncoherenceArrayName READ getSurfaceMeshTwinBoundaryIncoherenceArrayName WRITE setSurfaceMeshTwinBoundaryIncoherenceArrayName)

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

protected:
  FindTwinBoundaries();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

private:
  std::weak_ptr<DataArray<float>> m_AvgQuatsPtr;
  float* m_AvgQuats = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_FeaturePhasesPtr;
  int32_t* m_FeaturePhases = nullptr;
  std::weak_ptr<DataArray<uint32_t>> m_CrystalStructuresPtr;
  uint32_t* m_CrystalStructures = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_SurfaceMeshFaceLabelsPtr;
  int32_t* m_SurfaceMeshFaceLabels = nullptr;
  std::weak_ptr<DataArray<double>> m_SurfaceMeshFaceNormalsPtr;
  double* m_SurfaceMeshFaceNormals = nullptr;
  std::weak_ptr<DataArray<bool>> m_SurfaceMeshTwinBoundaryPtr;
  bool* m_SurfaceMeshTwinBoundary = nullptr;
  std::weak_ptr<DataArray<float>> m_SurfaceMeshTwinBoundaryIncoherencePtr;
  float* m_SurfaceMeshTwinBoundaryIncoherence = nullptr;

  float m_AxisTolerance = {0.0f};
  float m_AngleTolerance = {0.0f};
  bool m_FindCoherence = {true};
  DataArrayPath m_AvgQuatsArrayPath = {SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::AvgQuats};
  DataArrayPath m_FeaturePhasesArrayPath = {SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases};
  DataArrayPath m_CrystalStructuresArrayPath = {SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures};
  DataArrayPath m_SurfaceMeshFaceLabelsArrayPath = {SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels};
  DataArrayPath m_SurfaceMeshFaceNormalsArrayPath = {SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceNormals};
  QString m_SurfaceMeshTwinBoundaryArrayName = {SIMPL::FaceData::SurfaceMeshTwinBoundary};
  QString m_SurfaceMeshTwinBoundaryIncoherenceArrayName = {SIMPL::FaceData::SurfaceMeshTwinBoundaryIncoherence};

  LaueOpsContainer m_OrientationOps;
  /**
   * @brief dataCheckVoxel Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheckVoxel();

  /**
   * @brief dataCheckSurfaceMesh Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheckSurfaceMesh();

public:
  FindTwinBoundaries(const FindTwinBoundaries&) = delete;            // Copy Constructor Not Implemented
  FindTwinBoundaries(FindTwinBoundaries&&) = delete;                 // Move Constructor Not Implemented
  FindTwinBoundaries& operator=(const FindTwinBoundaries&) = delete; // Copy Assignment Not Implemented
  FindTwinBoundaries& operator=(FindTwinBoundaries&&) = delete;      // Move Assignment Not Implemented
};
