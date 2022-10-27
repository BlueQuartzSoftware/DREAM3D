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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SurfaceMeshing/SurfaceMeshingDLLExport.h"

/**
 * @brief The RemoveFlaggedTriangles class. See [Filter documentation](@ref removeflaggedtriangles) for details.
 */
class SurfaceMeshing_EXPORT RemoveFlaggedTriangles : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(RemoveFlaggedTriangles SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(RemoveFlaggedTriangles)
  PYB11_FILTER_NEW_MACRO(RemoveFlaggedTriangles)
  PYB11_PROPERTY(DataArrayPath TriangleGeometry READ getTriangleGeometry WRITE setTriangleGeometry)
  PYB11_PROPERTY(DataArrayPath MaskArrayPath READ getMaskArrayPath WRITE setMaskArrayPath)
  PYB11_PROPERTY(DataArrayPath RegionIDsArrayPath READ getRegionIDsArrayPath WRITE setRegionIDsArrayPath)
  PYB11_PROPERTY(QString ReducedTriangleGeometry READ getReducedTriangleGeometry WRITE setReducedTriangleGeometry)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = RemoveFlaggedTriangles;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<RemoveFlaggedTriangles> New();

  /**
   * @brief Returns the name of the class for RemoveFlaggedTriangles
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for RemoveFlaggedTriangles
   */
  static QString ClassName();

  ~RemoveFlaggedTriangles() override;

  /**
   * @brief Setter property for TriangleGeometry
   */
  void setTriangleGeometry(const DataArrayPath& value);
  /**
   * @brief Getter property for TriangleGeometry
   * @return Value of TriangleGeometry
   */
  DataArrayPath getTriangleGeometry() const;
  Q_PROPERTY(DataArrayPath TriangleGeometry READ getTriangleGeometry WRITE setTriangleGeometry)

  /**
   * @brief Setter property for MaskArrayPath
   */
  void setMaskArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for MaskArrayPath
   * @return Value of MaskArrayPath
   */
  DataArrayPath getMaskArrayPath() const;
  Q_PROPERTY(DataArrayPath MaskArrayPath READ getMaskArrayPath WRITE setMaskArrayPath)

  /**
   * @brief Setter property for RegionIDArrayPath
   */
  void setRegionIDsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for RegionIDArrayPath
   * @return Value of RegionIDArrayPath
   */
  DataArrayPath getRegionIDsArrayPath() const;
  Q_PROPERTY(DataArrayPath RegionIDsArrayPath READ getRegionIDsArrayPath WRITE setRegionIDsArrayPath)

  /**
   * @brief Setter property for ReducedTriangleGeometry
   */
  void setReducedTriangleGeometry(const QString& value);
  /**
   * @brief Getter property for ReducedTriangleGeometry
   * @return Value of ReducedTriangleGeometry
   */
  QString getReducedTriangleGeometry() const;
  Q_PROPERTY(QString ReducedTriangleGeometry READ getReducedTriangleGeometry WRITE setReducedTriangleGeometry)

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
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

protected:
  RemoveFlaggedTriangles();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

private:
  std::weak_ptr<DataArray<bool>> m_MaskPtr;
  bool* m_Mask = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_RegionIDsPtr;
  int32_t* m_RegionIDs = nullptr;

  DataArrayPath m_TriangleGeometry = {SIMPL::Defaults::TriangleDataContainerName, "", ""};
  DataArrayPath m_MaskArrayPath = {SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceFeatureAttributeMatrixName, SIMPL::CellData::Mask};
  DataArrayPath m_RegionIDsArrayPath = {SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::CellData::FeatureIds};
  QString m_ReducedTriangleGeometry = {"ReducedTriangleDataContainer"};

  QList<QString> m_AttrMatList;

public:
  RemoveFlaggedTriangles(const RemoveFlaggedTriangles&) = delete;            // Copy Constructor Not Implemented
  RemoveFlaggedTriangles(RemoveFlaggedTriangles&&) = delete;                 // Move Constructor Not Implemented
  RemoveFlaggedTriangles& operator=(const RemoveFlaggedTriangles&) = delete; // Copy Assignment Not Implemented
  RemoveFlaggedTriangles& operator=(RemoveFlaggedTriangles&&) = delete;      // Move Assignment Not Implemented
};
