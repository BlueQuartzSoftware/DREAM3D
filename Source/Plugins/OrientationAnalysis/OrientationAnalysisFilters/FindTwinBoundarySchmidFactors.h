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
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "OrientationLib/LaueOps/LaueOps.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The FindTwinBoundarySchmidFactors class. See [Filter documentation](@ref findtwinboundaryschmidfactors) for details.
 */
class OrientationAnalysis_EXPORT FindTwinBoundarySchmidFactors : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindTwinBoundarySchmidFactors SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindTwinBoundarySchmidFactors)
  PYB11_FILTER_NEW_MACRO(FindTwinBoundarySchmidFactors)
  PYB11_FILTER_PARAMETER(bool, WriteFile)
  PYB11_FILTER_PARAMETER(QString, TwinBoundarySchmidFactorsFile)
  PYB11_FILTER_PARAMETER(FloatVec3Type, LoadingDir)
  PYB11_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceNormalsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceMeshTwinBoundaryArrayPath)
  PYB11_FILTER_PARAMETER(QString, SurfaceMeshTwinBoundarySchmidFactorsArrayName)
  PYB11_PROPERTY(bool WriteFile READ getWriteFile WRITE setWriteFile)
  PYB11_PROPERTY(QString TwinBoundarySchmidFactorsFile READ getTwinBoundarySchmidFactorsFile WRITE setTwinBoundarySchmidFactorsFile)
  PYB11_PROPERTY(FloatVec3Type LoadingDir READ getLoadingDir WRITE setLoadingDir)
  PYB11_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)
  PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshTwinBoundaryArrayPath READ getSurfaceMeshTwinBoundaryArrayPath WRITE setSurfaceMeshTwinBoundaryArrayPath)
  PYB11_PROPERTY(QString SurfaceMeshTwinBoundarySchmidFactorsArrayName READ getSurfaceMeshTwinBoundarySchmidFactorsArrayName WRITE setSurfaceMeshTwinBoundarySchmidFactorsArrayName)
#endif

public:
  using Self = FindTwinBoundarySchmidFactors;
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
   * @brief Returns the name of the class for FindTwinBoundarySchmidFactors
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindTwinBoundarySchmidFactors
   */
  static QString ClassName();

  ~FindTwinBoundarySchmidFactors() override;

  /**
   * @brief Setter property for WriteFile
   */
  void setWriteFile(bool value);
  /**
   * @brief Getter property for WriteFile
   * @return Value of WriteFile
   */
  bool getWriteFile() const;

  Q_PROPERTY(bool WriteFile READ getWriteFile WRITE setWriteFile)

  /**
   * @brief Setter property for TwinBoundarySchmidFactorsFile
   */
  void setTwinBoundarySchmidFactorsFile(const QString& value);
  /**
   * @brief Getter property for TwinBoundarySchmidFactorsFile
   * @return Value of TwinBoundarySchmidFactorsFile
   */
  QString getTwinBoundarySchmidFactorsFile() const;

  Q_PROPERTY(QString TwinBoundarySchmidFactorsFile READ getTwinBoundarySchmidFactorsFile WRITE setTwinBoundarySchmidFactorsFile)

  /**
   * @brief Setter property for LoadingDir
   */
  void setLoadingDir(const FloatVec3Type& value);
  /**
   * @brief Getter property for LoadingDir
   * @return Value of LoadingDir
   */
  FloatVec3Type getLoadingDir() const;

  Q_PROPERTY(FloatVec3Type LoadingDir READ getLoadingDir WRITE setLoadingDir)

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
   * @brief Setter property for SurfaceMeshTwinBoundaryArrayPath
   */
  void setSurfaceMeshTwinBoundaryArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceMeshTwinBoundaryArrayPath
   * @return Value of SurfaceMeshTwinBoundaryArrayPath
   */
  DataArrayPath getSurfaceMeshTwinBoundaryArrayPath() const;

  Q_PROPERTY(DataArrayPath SurfaceMeshTwinBoundaryArrayPath READ getSurfaceMeshTwinBoundaryArrayPath WRITE setSurfaceMeshTwinBoundaryArrayPath)

  /**
   * @brief Setter property for SurfaceMeshTwinBoundarySchmidFactorsArrayName
   */
  void setSurfaceMeshTwinBoundarySchmidFactorsArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshTwinBoundarySchmidFactorsArrayName
   * @return Value of SurfaceMeshTwinBoundarySchmidFactorsArrayName
   */
  QString getSurfaceMeshTwinBoundarySchmidFactorsArrayName() const;

  Q_PROPERTY(QString SurfaceMeshTwinBoundarySchmidFactorsArrayName READ getSurfaceMeshTwinBoundarySchmidFactorsArrayName WRITE setSurfaceMeshTwinBoundarySchmidFactorsArrayName)

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
  FindTwinBoundarySchmidFactors();

private:
  std::weak_ptr<DataArray<float>> m_AvgQuatsPtr;
  float* m_AvgQuats = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_FeaturePhasesPtr;
  int32_t* m_FeaturePhases = nullptr;
  std::weak_ptr<DataArray<unsigned int>> m_CrystalStructuresPtr;
  unsigned int* m_CrystalStructures = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_SurfaceMeshFaceLabelsPtr;
  int32_t* m_SurfaceMeshFaceLabels = nullptr;
  std::weak_ptr<DataArray<double>> m_SurfaceMeshFaceNormalsPtr;
  double* m_SurfaceMeshFaceNormals = nullptr;
  std::weak_ptr<DataArray<bool>> m_SurfaceMeshTwinBoundaryPtr;
  bool* m_SurfaceMeshTwinBoundary = nullptr;
  std::weak_ptr<DataArray<float>> m_SurfaceMeshTwinBoundarySchmidFactorsPtr;
  float* m_SurfaceMeshTwinBoundarySchmidFactors = nullptr;

  bool m_WriteFile = {};
  QString m_TwinBoundarySchmidFactorsFile = {};
  FloatVec3Type m_LoadingDir = {};
  DataArrayPath m_AvgQuatsArrayPath = {};
  DataArrayPath m_FeaturePhasesArrayPath = {};
  DataArrayPath m_CrystalStructuresArrayPath = {};
  DataArrayPath m_SurfaceMeshFaceLabelsArrayPath = {};
  DataArrayPath m_SurfaceMeshFaceNormalsArrayPath = {};
  DataArrayPath m_SurfaceMeshTwinBoundaryArrayPath = {};
  QString m_SurfaceMeshTwinBoundarySchmidFactorsArrayName = {};

  QVector<LaueOps::Pointer> m_OrientationOps;

  /**
   * @brief dataCheckVoxel Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheckVoxel();

  /**
   * @brief dataCheckSurfaceMesh Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheckSurfaceMesh();

public:
  FindTwinBoundarySchmidFactors(const FindTwinBoundarySchmidFactors&) = delete; // Copy Constructor Not Implemented
  FindTwinBoundarySchmidFactors(FindTwinBoundarySchmidFactors&&) = delete;      // Move Constructor Not Implemented
  FindTwinBoundarySchmidFactors& operator=(const FindTwinBoundarySchmidFactors&) = delete; // Copy Assignment Not Implemented
  FindTwinBoundarySchmidFactors& operator=(FindTwinBoundarySchmidFactors&&) = delete;      // Move Assignment Not Implemented
};

