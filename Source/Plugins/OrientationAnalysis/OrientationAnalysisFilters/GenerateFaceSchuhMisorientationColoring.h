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

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @class GenerateFaceMisorientationColors GenerateFaceMisorientationColors.h DREAM3DLib/ProcessingFilters/GenerateFaceMisorientationColors.h
 * @brief This filter calculates the normal of each triangle in the surface mesh. The calculated normals have been
 * normalized themselves. This filter is parallelized using the Threading Building Blocks library and will attempt to
 * use the optimal number of processors to do the computation.
 * @author Will Lenthe
 * @date March 12, 2014
 * @version 1.0
 */
class OrientationAnalysis_EXPORT GenerateFaceSchuhMisorientationColoring : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(GenerateFaceSchuhMisorientationColoring SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(GenerateFaceSchuhMisorientationColoring)
  PYB11_FILTER_NEW_MACRO(GenerateFaceSchuhMisorientationColoring)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  PYB11_FILTER_PARAMETER(QString, SurfaceMeshFaceSchuhMisorientationColorsArrayName)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
  PYB11_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)
  PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(QString SurfaceMeshFaceSchuhMisorientationColorsArrayName READ getSurfaceMeshFaceSchuhMisorientationColorsArrayName WRITE setSurfaceMeshFaceSchuhMisorientationColorsArrayName)
#endif

public:
  using Self = GenerateFaceSchuhMisorientationColoring;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  
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
   * @brief Returns the name of the class for GenerateFaceSchuhMisorientationColoring
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for GenerateFaceSchuhMisorientationColoring
   */
  static QString ClassName();

  ~GenerateFaceSchuhMisorientationColoring() override;

  /**
   * @brief This returns the group that the filter belonds to. You can select
   * a different group if you want. The string returned here will be displayed
   * in the GUI for the filter
   */
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
   * @brief Setter property for SurfaceMeshFaceSchuhMisorientationColorsArrayName
   */
  void setSurfaceMeshFaceSchuhMisorientationColorsArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshFaceSchuhMisorientationColorsArrayName
   * @return Value of SurfaceMeshFaceSchuhMisorientationColorsArrayName
   */
  QString getSurfaceMeshFaceSchuhMisorientationColorsArrayName() const;

  Q_PROPERTY(QString SurfaceMeshFaceSchuhMisorientationColorsArrayName READ getSurfaceMeshFaceSchuhMisorientationColorsArrayName WRITE setSurfaceMeshFaceSchuhMisorientationColorsArrayName)

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
  GenerateFaceSchuhMisorientationColoring();

  /**
   * @brief dataCheckSurfaceMesh Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheckSurfaceMesh();

  /**
   * @brief dataCheckVoxel Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheckVoxel();

private:
  std::weak_ptr<DataArray<int32_t>> m_SurfaceMeshFaceLabelsPtr;
  int32_t* m_SurfaceMeshFaceLabels = nullptr;
  std::weak_ptr<DataArray<uint8_t>> m_SurfaceMeshFaceSchuhMisorientationColorsPtr;
  uint8_t* m_SurfaceMeshFaceSchuhMisorientationColors = nullptr;
  std::weak_ptr<DataArray<float>> m_AvgQuatsPtr;
  float* m_AvgQuats = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_FeaturePhasesPtr;
  int32_t* m_FeaturePhases = nullptr;
  std::weak_ptr<DataArray<unsigned int>> m_CrystalStructuresPtr;
  unsigned int* m_CrystalStructures = nullptr;

  DataArrayPath m_SurfaceMeshFaceLabelsArrayPath = {};
  DataArrayPath m_AvgQuatsArrayPath = {};
  DataArrayPath m_FeaturePhasesArrayPath = {};
  DataArrayPath m_CrystalStructuresArrayPath = {};
  QString m_SurfaceMeshFaceSchuhMisorientationColorsArrayName = {};

public:
  GenerateFaceSchuhMisorientationColoring(const GenerateFaceSchuhMisorientationColoring&) = delete;            // Copy Constructor Not Implemented
  GenerateFaceSchuhMisorientationColoring(GenerateFaceSchuhMisorientationColoring&&) = delete;                 // Move Constructor Not Implemented
  GenerateFaceSchuhMisorientationColoring& operator=(const GenerateFaceSchuhMisorientationColoring&) = delete; // Copy Assignment Not Implemented
  GenerateFaceSchuhMisorientationColoring& operator=(GenerateFaceSchuhMisorientationColoring&&) = delete;      // Move Assignment Not Implemented
};

