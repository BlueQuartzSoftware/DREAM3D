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
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "Sampling/SamplingDLLExport.h"

class DataContainer;

/**
 * @brief The ResampleImageGeom class. See [Filter documentation](@ref changeresolution) for details.
 */
class Sampling_EXPORT ResampleImageGeom : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(ResampleImageGeom SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(ResampleImageGeom)
  PYB11_FILTER_NEW_MACRO(ResampleImageGeom)
  PYB11_PROPERTY(DataArrayPath NewDataContainerPath READ getNewDataContainerPath WRITE setNewDataContainerPath)
  PYB11_PROPERTY(DataArrayPath CellAttributeMatrixPath READ getCellAttributeMatrixPath WRITE setCellAttributeMatrixPath)
  PYB11_PROPERTY(DataArrayPath CellFeatureAttributeMatrixPath READ getCellFeatureAttributeMatrixPath WRITE setCellFeatureAttributeMatrixPath)
  PYB11_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)
  PYB11_PROPERTY(bool RenumberFeatures READ getRenumberFeatures WRITE setRenumberFeatures)
  PYB11_PROPERTY(bool SaveAsNewDataContainer READ getSaveAsNewDataContainer WRITE setSaveAsNewDataContainer)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = ResampleImageGeom;
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
   * @brief Returns the name of the class for ResampleImageGeom
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ResampleImageGeom
   */
  static QString ClassName();

  ~ResampleImageGeom() override;

  /**
   * @brief Setter property for NewDataContainerPath
   */
  void setNewDataContainerPath(const DataArrayPath& value);
  /**
   * @brief Getter property for NewDataContainerPath
   * @return Value of NewDataContainerPath
   */
  DataArrayPath getNewDataContainerPath() const;
  Q_PROPERTY(DataArrayPath NewDataContainerPath READ getNewDataContainerPath WRITE setNewDataContainerPath)

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

  /**
   * @brief Setter property for Spacing
   */
  void setSpacing(const FloatVec3Type& value);
  /**
   * @brief Getter property for Spacing
   * @return Value of Spacing
   */
  FloatVec3Type getSpacing() const;
  Q_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)

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
   * @brief Setter property for FeatureIdsArrayPath
   */
  void setFeatureIdsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureIdsArrayPath
   * @return Value of FeatureIdsArrayPath
   */
  DataArrayPath getFeatureIdsArrayPath() const;
  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  QString getCurrentGeomtryInfo() const;
  Q_PROPERTY(QString CurrentGeomtryInfo READ getCurrentGeomtryInfo)

  QString getNewGeomtryInfo() const;
  Q_PROPERTY(QString NewGeomtryInfo READ getNewGeomtryInfo)
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
  ResampleImageGeom();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<Int32ArrayType> m_FeatureIdsPtr;

  DataArrayPath m_NewDataContainerPath = {".DATA_CONTAINER_COPY", "", ""};
  DataArrayPath m_CellAttributeMatrixPath = {"", "", ""};
  DataArrayPath m_CellFeatureAttributeMatrixPath = {SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, ""};
  FloatVec3Type m_Spacing = {0.0F, 0.0F, 0.0F};
  bool m_RenumberFeatures = {false};
  bool m_SaveAsNewDataContainer = {false};
  DataArrayPath m_FeatureIdsArrayPath = {SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds};

  std::shared_ptr<DataContainer> m_PreviousDataContainer;

  QString m_CurrentGeomInfo = {};
  QString m_NewGeomInfo = {};

  QString generateCurrentGeomInfo() const;
  QString generateNewGeomInfo() const;

public:
  ResampleImageGeom(const ResampleImageGeom&) = delete;            // Copy Constructor Not Implemented
  ResampleImageGeom(ResampleImageGeom&&) = delete;                 // Move Constructor Not Implemented
  ResampleImageGeom& operator=(const ResampleImageGeom&) = delete; // Copy Assignment Not Implemented
  ResampleImageGeom& operator=(ResampleImageGeom&&) = delete;      // Move Assignment Not Implemented
};
