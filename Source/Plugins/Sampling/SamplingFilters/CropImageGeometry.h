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
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "Sampling/SamplingDLLExport.h"

/**
 * @brief The CropImageGeometry class. See [Filter documentation](@ref CropImageGeometry) for details.
 */
class Sampling_EXPORT CropImageGeometry : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(CropImageGeometry SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(CropImageGeometry)
  PYB11_FILTER_NEW_MACRO(CropImageGeometry)
  PYB11_FILTER_PARAMETER(DataArrayPath, NewDataContainerName)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellAttributeMatrixPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellFeatureAttributeMatrixPath)
  PYB11_FILTER_PARAMETER(int, XMin)
  PYB11_FILTER_PARAMETER(int, YMin)
  PYB11_FILTER_PARAMETER(int, ZMin)
  PYB11_FILTER_PARAMETER(int, XMax)
  PYB11_FILTER_PARAMETER(int, YMax)
  PYB11_FILTER_PARAMETER(int, ZMax)
  PYB11_FILTER_PARAMETER(bool, RenumberFeatures)
  PYB11_FILTER_PARAMETER(bool, SaveAsNewDataContainer)
  PYB11_FILTER_PARAMETER(bool, UpdateOrigin)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath NewDataContainerName READ getNewDataContainerName WRITE setNewDataContainerName)
  PYB11_PROPERTY(DataArrayPath CellAttributeMatrixPath READ getCellAttributeMatrixPath WRITE setCellAttributeMatrixPath)
  PYB11_PROPERTY(DataArrayPath CellFeatureAttributeMatrixPath READ getCellFeatureAttributeMatrixPath WRITE setCellFeatureAttributeMatrixPath)
  PYB11_PROPERTY(int XMin READ getXMin WRITE setXMin)
  PYB11_PROPERTY(int YMin READ getYMin WRITE setYMin)
  PYB11_PROPERTY(int ZMin READ getZMin WRITE setZMin)
  PYB11_PROPERTY(int XMax READ getXMax WRITE setXMax)
  PYB11_PROPERTY(int YMax READ getYMax WRITE setYMax)
  PYB11_PROPERTY(int ZMax READ getZMax WRITE setZMax)
  PYB11_PROPERTY(bool RenumberFeatures READ getRenumberFeatures WRITE setRenumberFeatures)
  PYB11_PROPERTY(bool SaveAsNewDataContainer READ getSaveAsNewDataContainer WRITE setSaveAsNewDataContainer)
  PYB11_PROPERTY(bool UpdateOrigin READ getUpdateOrigin WRITE setUpdateOrigin)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
#endif

public:
  using Self = CropImageGeometry;
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
   * @brief Returns the name of the class for CropImageGeometry
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for CropImageGeometry
   */
  static QString ClassName();

  ~CropImageGeometry() override;

  /**
   * @brief Setter property for NewDataContainerName
   */
  void setNewDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for NewDataContainerName
   * @return Value of NewDataContainerName
   */
  DataArrayPath getNewDataContainerName() const;

  Q_PROPERTY(DataArrayPath NewDataContainerName READ getNewDataContainerName WRITE setNewDataContainerName)

  /**
   * @brief Setter property for CellAttributeMatrixPath
   */
  void setCellAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CellAttributeMatrixPath
   * @return Value of CellAttributeMatrixPath
   */
  DataArrayPath getCellAttributeMatrixPath() const;

  Q_PROPERTY(DataArrayPath CellAttributeMatrixPath READ getCellAttributeMatrixPath WRITE setCellAttributeMatrixPath)

  /**
   * @brief Setter property for CellFeatureAttributeMatrixPath
   */
  void setCellFeatureAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CellFeatureAttributeMatrixPath
   * @return Value of CellFeatureAttributeMatrixPath
   */
  DataArrayPath getCellFeatureAttributeMatrixPath() const;

  Q_PROPERTY(DataArrayPath CellFeatureAttributeMatrixPath READ getCellFeatureAttributeMatrixPath WRITE setCellFeatureAttributeMatrixPath)

  IntVec3Type getCurrentVolumeDataContainerDimensions();
  Q_PROPERTY(IntVec3Type CurrentVolumeDataContainerDimensions READ getCurrentVolumeDataContainerDimensions)

  FloatVec3Type getCurrentVolumeDataContainerResolutions();
  Q_PROPERTY(FloatVec3Type CurrentVolumeDataContainerResolutions READ getCurrentVolumeDataContainerResolutions)

  /**
   * @brief Setter property for XMin
   */
  void setXMin(int value);
  /**
   * @brief Getter property for XMin
   * @return Value of XMin
   */
  int getXMin() const;

  Q_PROPERTY(int XMin READ getXMin WRITE setXMin)

  /**
   * @brief Setter property for YMin
   */
  void setYMin(int value);
  /**
   * @brief Getter property for YMin
   * @return Value of YMin
   */
  int getYMin() const;

  Q_PROPERTY(int YMin READ getYMin WRITE setYMin)

  /**
   * @brief Setter property for ZMin
   */
  void setZMin(int value);
  /**
   * @brief Getter property for ZMin
   * @return Value of ZMin
   */
  int getZMin() const;

  Q_PROPERTY(int ZMin READ getZMin WRITE setZMin)

  /**
   * @brief Setter property for XMax
   */
  void setXMax(int value);
  /**
   * @brief Getter property for XMax
   * @return Value of XMax
   */
  int getXMax() const;

  Q_PROPERTY(int XMax READ getXMax WRITE setXMax)

  /**
   * @brief Setter property for YMax
   */
  void setYMax(int value);
  /**
   * @brief Getter property for YMax
   * @return Value of YMax
   */
  int getYMax() const;

  Q_PROPERTY(int YMax READ getYMax WRITE setYMax)

  /**
   * @brief Setter property for ZMax
   */
  void setZMax(int value);
  /**
   * @brief Getter property for ZMax
   * @return Value of ZMax
   */
  int getZMax() const;

  Q_PROPERTY(int ZMax READ getZMax WRITE setZMax)

  /**
   * @brief Setter property for RenumberFeatures
   */
  void setRenumberFeatures(bool value);
  /**
   * @brief Getter property for RenumberFeatures
   * @return Value of RenumberFeatures
   */
  bool getRenumberFeatures() const;

  Q_PROPERTY(bool RenumberFeatures READ getRenumberFeatures WRITE setRenumberFeatures)

  /**
   * @brief Setter property for SaveAsNewDataContainer
   */
  void setSaveAsNewDataContainer(bool value);
  /**
   * @brief Getter property for SaveAsNewDataContainer
   * @return Value of SaveAsNewDataContainer
   */
  bool getSaveAsNewDataContainer() const;

  Q_PROPERTY(bool SaveAsNewDataContainer READ getSaveAsNewDataContainer WRITE setSaveAsNewDataContainer)

  /**
   * @brief Setter property for UpdateOrigin
   */
  void setUpdateOrigin(bool value);
  /**
   * @brief Getter property for UpdateOrigin
   * @return Value of UpdateOrigin
   */
  bool getUpdateOrigin() const;

  Q_PROPERTY(bool UpdateOrigin READ getUpdateOrigin WRITE setUpdateOrigin)

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

  QString getOldBoxDimensions();
  Q_PROPERTY(QString OldBoxDimensions READ getOldBoxDimensions)
  
  QString getNewBoxDimensions();
  Q_PROPERTY(QString NewBoxDimensions READ getNewBoxDimensions)
  
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
  CropImageGeometry();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;

  DataArrayPath m_NewDataContainerName = {};
  DataArrayPath m_CellAttributeMatrixPath = {};
  DataArrayPath m_CellFeatureAttributeMatrixPath = {};
  int m_XMin = {};
  int m_YMin = {};
  int m_ZMin = {};
  int m_XMax = {};
  int m_YMax = {};
  int m_ZMax = {};
  bool m_RenumberFeatures = {};
  bool m_SaveAsNewDataContainer = {};
  bool m_UpdateOrigin = {};
  DataArrayPath m_FeatureIdsArrayPath = {};

  IntVec3Type m_OldDimensions;
  FloatVec3Type m_OldResolution;
  FloatVec3Type m_OldOrigin;

  IntVec3Type m_NewDimensions;
  FloatVec3Type m_NewResolution;
  FloatVec3Type m_NewOrigin;

public:
  CropImageGeometry(const CropImageGeometry&) = delete; // Copy Constructor Not Implemented
  CropImageGeometry(CropImageGeometry&&) = delete;      // Move Constructor Not Implemented
  CropImageGeometry& operator=(const CropImageGeometry&) = delete; // Copy Assignment Not Implemented
  CropImageGeometry& operator=(CropImageGeometry&&) = delete;      // Move Assignment Not Implemented
};

