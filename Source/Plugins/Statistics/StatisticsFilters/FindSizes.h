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
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "Statistics/StatisticsDLLExport.h"

/**
 * @brief The FindSizes class. See [Filter documentation](@ref findsizes) for details.
 */
class Statistics_EXPORT FindSizes : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindSizes SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindSizes)
  PYB11_FILTER_NEW_MACRO(FindSizes)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureAttributeMatrixName)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  PYB11_FILTER_PARAMETER(QString, VolumesArrayName)
  PYB11_FILTER_PARAMETER(QString, EquivalentDiametersArrayName)
  PYB11_FILTER_PARAMETER(QString, NumElementsArrayName)
  PYB11_FILTER_PARAMETER(bool, SaveElementSizes)
  PYB11_PROPERTY(DataArrayPath FeatureAttributeMatrixName READ getFeatureAttributeMatrixName WRITE setFeatureAttributeMatrixName)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_PROPERTY(QString VolumesArrayName READ getVolumesArrayName WRITE setVolumesArrayName)
  PYB11_PROPERTY(QString EquivalentDiametersArrayName READ getEquivalentDiametersArrayName WRITE setEquivalentDiametersArrayName)
  PYB11_PROPERTY(QString NumElementsArrayName READ getNumElementsArrayName WRITE setNumElementsArrayName)
  PYB11_PROPERTY(bool SaveElementSizes READ getSaveElementSizes WRITE setSaveElementSizes)
#endif

public:
  using Self = FindSizes;
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
   * @brief Returns the name of the class for FindSizes
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindSizes
   */
  static QString ClassName();

  ~FindSizes() override;

  /**
   * @brief Setter property for FeatureAttributeMatrixName
   */
  void setFeatureAttributeMatrixName(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureAttributeMatrixName
   * @return Value of FeatureAttributeMatrixName
   */
  DataArrayPath getFeatureAttributeMatrixName() const;

  Q_PROPERTY(DataArrayPath FeatureAttributeMatrixName READ getFeatureAttributeMatrixName WRITE setFeatureAttributeMatrixName)

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
   * @brief Setter property for VolumesArrayName
   */
  void setVolumesArrayName(const QString& value);
  /**
   * @brief Getter property for VolumesArrayName
   * @return Value of VolumesArrayName
   */
  QString getVolumesArrayName() const;

  Q_PROPERTY(QString VolumesArrayName READ getVolumesArrayName WRITE setVolumesArrayName)

  /**
   * @brief Setter property for EquivalentDiametersArrayName
   */
  void setEquivalentDiametersArrayName(const QString& value);
  /**
   * @brief Getter property for EquivalentDiametersArrayName
   * @return Value of EquivalentDiametersArrayName
   */
  QString getEquivalentDiametersArrayName() const;

  Q_PROPERTY(QString EquivalentDiametersArrayName READ getEquivalentDiametersArrayName WRITE setEquivalentDiametersArrayName)

  /**
   * @brief Setter property for NumElementsArrayName
   */
  void setNumElementsArrayName(const QString& value);
  /**
   * @brief Getter property for NumElementsArrayName
   * @return Value of NumElementsArrayName
   */
  QString getNumElementsArrayName() const;

  Q_PROPERTY(QString NumElementsArrayName READ getNumElementsArrayName WRITE setNumElementsArrayName)

  /**
   * @brief Setter property for SaveElementSizes
   */
  void setSaveElementSizes(bool value);
  /**
   * @brief Getter property for SaveElementSizes
   * @return Value of SaveElementSizes
   */
  bool getSaveElementSizes() const;

  Q_PROPERTY(bool SaveElementSizes READ getSaveElementSizes WRITE setSaveElementSizes)

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
  FindSizes();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief findSizes Determines the size of each Feature independent of geometry
   * @param igeom
   */
  void findSizes(IGeometry::Pointer igeom);

  /**
   * @brief findSizesImage Specialization for determining Feature sizes on ImageGeometries
   * @param image
   */
  void findSizesImage(ImageGeom::Pointer image);

  /**
   * @brief findSizesUnstructured Determines the size of each Feature for non-ImageGeometries
   * @param igeom
   */
  void findSizesUnstructured(IGeometry::Pointer igeom);

private:
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;
  std::weak_ptr<DataArray<float>> m_VolumesPtr;
  float* m_Volumes = nullptr;
  std::weak_ptr<DataArray<float>> m_EquivalentDiametersPtr;
  float* m_EquivalentDiameters = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_NumElementsPtr;
  int32_t* m_NumElements = nullptr;

  DataArrayPath m_FeatureAttributeMatrixName = {};
  DataArrayPath m_FeatureIdsArrayPath = {};
  QString m_VolumesArrayName = {};
  QString m_EquivalentDiametersArrayName = {};
  QString m_NumElementsArrayName = {};
  bool m_SaveElementSizes = {};

public:
  FindSizes(const FindSizes&) = delete;            // Copy Constructor Not Implemented
  FindSizes(FindSizes&&) = delete;                 // Move Constructor Not Implemented
  FindSizes& operator=(const FindSizes&) = delete; // Copy Assignment Not Implemented
  FindSizes& operator=(FindSizes&&) = delete;      // Move Assignment Not Implemented
};
