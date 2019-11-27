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

#include "Statistics/StatisticsDLLExport.h"

/**
 * @brief The FindShapes class. See [Filter documentation](@ref findshapes) for details.
 */
class Statistics_EXPORT FindShapes : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindShapes SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindShapes)
  PYB11_FILTER_NEW_MACRO(FindShapes)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellFeatureAttributeMatrixName)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CentroidsArrayPath)
  PYB11_FILTER_PARAMETER(QString, Omega3sArrayName)
  PYB11_FILTER_PARAMETER(QString, VolumesArrayName)
  PYB11_FILTER_PARAMETER(QString, AxisLengthsArrayName)
  PYB11_FILTER_PARAMETER(QString, AxisEulerAnglesArrayName)
  PYB11_FILTER_PARAMETER(QString, AspectRatiosArrayName)
  PYB11_PROPERTY(DataArrayPath CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)
  PYB11_PROPERTY(QString Omega3sArrayName READ getOmega3sArrayName WRITE setOmega3sArrayName)
  PYB11_PROPERTY(QString VolumesArrayName READ getVolumesArrayName WRITE setVolumesArrayName)
  PYB11_PROPERTY(QString AxisLengthsArrayName READ getAxisLengthsArrayName WRITE setAxisLengthsArrayName)
  PYB11_PROPERTY(QString AxisEulerAnglesArrayName READ getAxisEulerAnglesArrayName WRITE setAxisEulerAnglesArrayName)
  PYB11_PROPERTY(QString AspectRatiosArrayName READ getAspectRatiosArrayName WRITE setAspectRatiosArrayName)
#endif

public:
  using Self = FindShapes;
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
   * @brief Returns the name of the class for FindShapes
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindShapes
   */
  static QString ClassName();

  ~FindShapes() override;
  /**
   * @brief Setter property for CellFeatureAttributeMatrixName
   */
  void setCellFeatureAttributeMatrixName(const DataArrayPath& value);
  /**
   * @brief Getter property for CellFeatureAttributeMatrixName
   * @return Value of CellFeatureAttributeMatrixName
   */
  DataArrayPath getCellFeatureAttributeMatrixName() const;

  Q_PROPERTY(DataArrayPath CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)
  // N x 6 Array
  /**
   * @brief Setter property for FeatureMomentsPtr
   */
  void setFeatureMomentsPtr(const std::shared_ptr<DataArray<double>>& value);
  /**
   * @brief Getter property for FeatureMomentsPtr
   * @return Value of FeatureMomentsPtr
   */
  std::shared_ptr<DataArray<double>> getFeatureMomentsPtr() const;

  // N x 3 Array
  /**
   * @brief Setter property for FeatureEigenValsPtr
   */
  void setFeatureEigenValsPtr(const std::shared_ptr<DataArray<double>>& value);
  /**
   * @brief Getter property for FeatureEigenValsPtr
   * @return Value of FeatureEigenValsPtr
   */
  std::shared_ptr<DataArray<double>> getFeatureEigenValsPtr() const;

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
   * @brief Setter property for CentroidsArrayPath
   */
  void setCentroidsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CentroidsArrayPath
   * @return Value of CentroidsArrayPath
   */
  DataArrayPath getCentroidsArrayPath() const;

  Q_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)

  /**
   * @brief Setter property for Omega3sArrayName
   */
  void setOmega3sArrayName(const QString& value);
  /**
   * @brief Getter property for Omega3sArrayName
   * @return Value of Omega3sArrayName
   */
  QString getOmega3sArrayName() const;

  Q_PROPERTY(QString Omega3sArrayName READ getOmega3sArrayName WRITE setOmega3sArrayName)

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
   * @brief Setter property for AxisLengthsArrayName
   */
  void setAxisLengthsArrayName(const QString& value);
  /**
   * @brief Getter property for AxisLengthsArrayName
   * @return Value of AxisLengthsArrayName
   */
  QString getAxisLengthsArrayName() const;

  Q_PROPERTY(QString AxisLengthsArrayName READ getAxisLengthsArrayName WRITE setAxisLengthsArrayName)

  /**
   * @brief Setter property for AxisEulerAnglesArrayName
   */
  void setAxisEulerAnglesArrayName(const QString& value);
  /**
   * @brief Getter property for AxisEulerAnglesArrayName
   * @return Value of AxisEulerAnglesArrayName
   */
  QString getAxisEulerAnglesArrayName() const;

  Q_PROPERTY(QString AxisEulerAnglesArrayName READ getAxisEulerAnglesArrayName WRITE setAxisEulerAnglesArrayName)

  /**
   * @brief Setter property for AspectRatiosArrayName
   */
  void setAspectRatiosArrayName(const QString& value);
  /**
   * @brief Getter property for AspectRatiosArrayName
   * @return Value of AspectRatiosArrayName
   */
  QString getAspectRatiosArrayName() const;

  Q_PROPERTY(QString AspectRatiosArrayName READ getAspectRatiosArrayName WRITE setAspectRatiosArrayName)

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
  FindShapes();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief find_moments Determines the second order moments for each Feature
   */
  void find_moments();

  /**
   * @brief find_moments2D Determines the second order moments for each Feature (2D version)
   */
  void find_moments2D();

  /**
   * @brief find_axes Determine principal axis lengths for each Feature
   */
  void find_axes();

  /**
   * @brief find_axes2D Determine principal axis lengths for each Feature (2D version)
   */
  void find_axes2D();

  /**
   * @brief find_axiseulers Determine principal axis directions for each Feature
   */
  void find_axiseulers();

  /**
   * @brief find_axiseulers2D Determine principal axis directions for each Feature (2D version)
   */
  void find_axiseulers2D();

private:
  DataArrayPath m_CellFeatureAttributeMatrixName = {};
  std::shared_ptr<DataArray<double>> m_FeatureMomentsPtr;
  double* m_FeatureMoments = nullptr;
  std::shared_ptr<DataArray<double>> m_FeatureEigenValsPtr;
  double* m_FeatureEigenVals = nullptr;
  DataArrayPath m_FeatureIdsArrayPath = {};
  DataArrayPath m_CentroidsArrayPath = {};
  QString m_Omega3sArrayName = {};
  QString m_VolumesArrayName = {};
  QString m_AxisLengthsArrayName = {};
  QString m_AxisEulerAnglesArrayName = {};
  QString m_AspectRatiosArrayName = {};
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;
  std::weak_ptr<DataArray<float>> m_CentroidsPtr;
  float* m_Centroids = nullptr;
  std::weak_ptr<DataArray<float>> m_AxisEulerAnglesPtr;
  float* m_AxisEulerAngles = nullptr;
  std::weak_ptr<DataArray<float>> m_AxisLengthsPtr;
  float* m_AxisLengths = nullptr;
  std::weak_ptr<DataArray<float>> m_Omega3sPtr;
  float* m_Omega3s = nullptr;
  std::weak_ptr<DataArray<float>> m_VolumesPtr;
  float* m_Volumes = nullptr;
  std::weak_ptr<DataArray<float>> m_AspectRatiosPtr;
  float* m_AspectRatios = nullptr;

  double m_ScaleFactor;

public:
  FindShapes(const FindShapes&) = delete;            // Copy Constructor Not Implemented
  FindShapes(FindShapes&&) = delete;                 // Move Constructor Not Implemented
  FindShapes& operator=(const FindShapes&) = delete; // Copy Assignment Not Implemented
  FindShapes& operator=(FindShapes&&) = delete;      // Move Assignment Not Implemented
};

