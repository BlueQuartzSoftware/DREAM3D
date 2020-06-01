/* ============================================================================
 * Copyright (c) 2020 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"

#include "Sampling/SamplingDLLExport.h"
#include "Sampling/SamplingPlugin.h"

/**
 * @brief The RegularGridSampleRectilinearGrid class. See [Filter documentation](@ref regulargridsamplerectilineargrid) for details.
 */
class Sampling_EXPORT RegularGridSampleRectilinearGrid : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(RegularGridSampleRectilinearGrid SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(RegularGridSampleRectilinearGrid)
  PYB11_FILTER_NEW_MACRO(RegularGridSampleRectilinearGrid)
  PYB11_PROPERTY(DataArrayPath RectilinearGridPath READ getRectilinearGridPath WRITE setRectilinearGridPath)
  // PYB11_PROPERTY(DataArrayPath RectGridCellAttributeMatrix READ getRectGridCellAttributeMatrix WRITE setRectGridCellAttributeMatrix)
  PYB11_PROPERTY(QString RectGridGeometryDesc READ getRectGridGeometryDesc)
  PYB11_PROPERTY(QVector<DataArrayPath> SelectedDataArrayPaths READ getSelectedDataArrayPaths WRITE setSelectedDataArrayPaths)
  PYB11_PROPERTY(DataArrayPath ImageGeometryPath READ getImageGeometryPath WRITE setImageGeometryPath)
  PYB11_PROPERTY(QString ImageGeomCellAttributeMatrix READ getImageGeomCellAttributeMatrix WRITE setImageGeomCellAttributeMatrix)
  PYB11_PROPERTY(int32_t LengthUnit READ getLengthUnit WRITE setLengthUnit)
  PYB11_PROPERTY(IntVec3Type Dimensions READ getDimensions WRITE setDimensions)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:

  using Self = RegularGridSampleRectilinearGrid;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for RegularGridSampleRectilinearGrid
   */
  QString getNameOfClass() const override;

  /**
   * @brief Returns the name of the class for RegularGridSampleRectilinearGrid
   */
  static QString ClassName();

  ~RegularGridSampleRectilinearGrid() override;

  /**
   * @brief Setter property for RectilinearGridPath
   */
  void setRectilinearGridPath(const DataArrayPath& value);
  /**
   * @brief Getter property for RectilinearGridPath
   * @return Value of RectilinearGridPath
   */
  DataArrayPath getRectilinearGridPath() const;
  Q_PROPERTY(DataArrayPath RectilinearGridPath READ getRectilinearGridPath WRITE setRectilinearGridPath)

  /**
   * @brief Setter property for SelectedDataArrayPaths
   */
  void setSelectedDataArrayPaths(const QVector<DataArrayPath>& value);
  /**
   * @brief Getter property for SelectedDataArrayPaths
   * @return Value of SelectedDataArrayPaths
   */
  QVector<DataArrayPath> getSelectedDataArrayPaths() const;
  Q_PROPERTY(QVector<DataArrayPath> SelectedDataArrayPaths READ getSelectedDataArrayPaths WRITE setSelectedDataArrayPaths)

  /**
   * @brief Setter property for ImageGeometryPath
   */
  void setImageGeometryPath(const DataArrayPath& value);
  /**
   * @brief Getter property for ImageGeometryPath
   * @return Value of ImageGeometryPath
   */
  DataArrayPath getImageGeometryPath() const;
  Q_PROPERTY(DataArrayPath ImageGeometryPath READ getImageGeometryPath WRITE setImageGeometryPath)

  /**
   * @brief Setter property for ImageGeomCellAttributeMatrix
   */
  void setImageGeomCellAttributeMatrix(const QString& value);
  /**
   * @brief Getter property for ImageGeomCellAttributeMatrix
   * @return Value of ImageGeomCellAttributeMatrix
   */
  QString getImageGeomCellAttributeMatrix() const;
  Q_PROPERTY(QString ImageGeomCellAttributeMatrix READ getImageGeomCellAttributeMatrix WRITE setImageGeomCellAttributeMatrix)

  /**
   * @brief Setter property for LengthUnit
   */
  void setLengthUnit(int32_t value);
  /**
   * @brief Getter property for LengthUnit
   * @return Value of LengthUnit
   */
  int32_t getLengthUnit() const;
  Q_PROPERTY(int32_t LengthUnit READ getLengthUnit WRITE setLengthUnit)

  /**
   * @brief Setter property for Dimensions
   */
  void setDimensions(const IntVec3Type& value);
  /**
   * @brief Getter property for Dimensions
   * @return Value of Dimensions
   */
  IntVec3Type getDimensions() const;
  Q_PROPERTY(IntVec3Type Dimensions READ getDimensions WRITE setDimensions)

  /**
   * @brief getNewBoxDimensions
   * @return
   */
  QString getRectGridGeometryDesc();
  Q_PROPERTY(QString RectGridGeometryDesc READ getRectGridGeometryDesc)

  /**
   * @brief getBoxDimensions Returns a string describing the box dimensions and size/volume
   * @return
   */
  QString getCreatedGeometryDescription();
  Q_PROPERTY(QString CreatedGeometryDescription READ getCreatedGeometryDescription)

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
  RegularGridSampleRectilinearGrid();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  DataArrayPath m_RectilinearGridPath = {"Resampled Geometry", "", ""};
  // DataArrayPath m_RectGridCellAttributeMatrix = {};
  DataArrayPath m_ImageGeometryPath = {SIMPL::Defaults::ImageDataContainerName, "", ""};
  QString m_ImageGeomCellAttributeMatrix = {};
  int32_t m_LengthUnit = {6}; //  Micrometers default
  IntVec3Type m_Dimensions = {128, 128, 128};
  QVector<DataArrayPath> m_SelectedDataArrayPaths = {};
  QVector<IDataArray::WeakPointer> m_SelectedWeakPtrVector;

  RectGridGeom::Pointer m_InputRectGridGeometry;
  ImageGeom::Pointer m_GeneratedVolume;

public:
  RegularGridSampleRectilinearGrid(const RegularGridSampleRectilinearGrid&) = delete;            // Copy Constructor Not Implemented
  RegularGridSampleRectilinearGrid& operator=(const RegularGridSampleRectilinearGrid&) = delete; // Copy Assignment Not Implemented
  RegularGridSampleRectilinearGrid(RegularGridSampleRectilinearGrid &&) = delete;                // Move Constructor Not Implemented
  RegularGridSampleRectilinearGrid& operator=(RegularGridSampleRectilinearGrid&&) = delete;      // Move Assignment Not Implemented
};

