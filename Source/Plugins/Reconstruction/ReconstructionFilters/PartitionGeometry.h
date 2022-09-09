/* ============================================================================
 * Copyright (c) 2022-2022 BlueQuartz Software, LLC
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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/FilterParameters/FloatVec2FilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Reconstruction/ReconstructionPlugin.h"
#include "SIMPLib/Geometry/RectGridGeom.h"

/**
 * @brief The PartitionGeometry class. See [Filter documentation](@ref partitiongeometry) for details.
 */
class Reconstruction_EXPORT PartitionGeometry : public AbstractFilter
{
  Q_OBJECT

  // clang-format off
  PYB11_BEGIN_BINDINGS(PartitionGeometry SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(PartitionGeometry)
  PYB11_FILTER_NEW_MACRO(PartitionGeometry)
  PYB11_PROPERTY(int PartitioningMode READ getPartitioningMode WRITE setPartitioningMode)
  PYB11_PROPERTY(DataArrayPath AttributeMatrixPath READ getAttributeMatrixPath WRITE setAttributeMatrixPath)
//  PYB11_PROPERTY(DataArrayPath BoundingBoxPath READ getBoundingBoxPath WRITE setBoundingBoxPath)
  PYB11_PROPERTY(FloatVec3Type LowerLeftCoord READ getLowerLeftCoord WRITE setLowerLeftCoord)
  PYB11_PROPERTY(FloatVec3Type UpperRightCoord READ getUpperRightCoord WRITE setUpperRightCoord)
  PYB11_PROPERTY(FloatVec3Type PartitioningSchemeOrigin READ getPartitioningSchemeOrigin WRITE setPartitioningSchemeOrigin)
  PYB11_PROPERTY(FloatVec3Type LengthPerPartition READ getLengthPerPartition WRITE setLengthPerPartition)
  PYB11_PROPERTY(IntVec3Type NumberOfPartitionsPerAxis READ getNumberOfPartitionsPerAxis WRITE setNumberOfPartitionsPerAxis)
  PYB11_PROPERTY(int OutOfBoundsValue READ getOutOfBoundsValue WRITE setOutOfBoundsValue)
  PYB11_PROPERTY(int StartingPartitionID READ getStartingPartitionID WRITE setStartingPartitionID)
  PYB11_PROPERTY(QString InputGeometryInformation READ getInputGeometryInformation)
  PYB11_PROPERTY(QString PartitioningSchemeInformation READ getPartitioningSchemeInformation)
  PYB11_PROPERTY(QString PartitionIdsArrayName READ getPartitionIdsArrayName WRITE setPartitionIdsArrayName)
  PYB11_PROPERTY(bool SavePartitioningScheme READ getSavePartitioningScheme WRITE setSavePartitioningScheme)
  PYB11_PROPERTY(DataArrayPath PSDataContainerPath READ getPSDataContainerPath WRITE setPSDataContainerPath)
  PYB11_PROPERTY(QString PSAttributeMatrixName READ getPSAttributeMatrixName WRITE setPSAttributeMatrixName)
  PYB11_END_BINDINGS()
  // clang-format on

public:
  using Self = PartitionGeometry;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for PartitionGeometry
   */
  QString getNameOfClass() const override;

  /**
   * @brief Returns the name of the class for PartitionGeometry
   */
  static QString ClassName();

  ~PartitionGeometry() override;

  /**
   * @brief Setter property for PartitioningMode
   */
  void setPartitioningMode(const int& value);
  /**
   * @brief Getter property for PartitioningMode
   * @return Value of PartitioningMode
   */
  int getPartitioningMode() const;
  Q_PROPERTY(int PartitioningMode READ getPartitioningMode WRITE setPartitioningMode)

  /**
   * @brief Setter property for AttributeMatrixPath
   */
  void setAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for AttributeMatrixPath
   * @return Value of AttributeMatrixPath
   */
  DataArrayPath getAttributeMatrixPath() const;
  Q_PROPERTY(DataArrayPath AttributeMatrixPath READ getAttributeMatrixPath WRITE setAttributeMatrixPath)

  //  /**
  //   * @brief Setter property for BoundingBoxPath
  //   */
  //  void setBoundingBoxPath(const DataArrayPath& value);
  //  /**
  //   * @brief Getter property for BoundingBoxPath
  //   * @return Value of BoundingBoxPath
  //   */
  //  DataArrayPath getBoundingBoxPath() const;
  //  Q_PROPERTY(DataArrayPath BoundingBoxPath READ getBoundingBoxPath WRITE setBoundingBoxPath)

  /**
   * @brief Setter property for LowerLeftCoord
   */
  void setLowerLeftCoord(const FloatVec3Type& value);
  /**
   * @brief Getter property for LowerLeftCoord
   * @return Value of LowerLeftCoord
   */
  FloatVec3Type getLowerLeftCoord() const;
  Q_PROPERTY(FloatVec3Type LowerLeftCoord READ getLowerLeftCoord WRITE setLowerLeftCoord)

  /**
   * @brief Setter property for UpperRightCoord
   */
  void setUpperRightCoord(const FloatVec3Type& value);
  /**
   * @brief Getter property for UpperRightCoord
   * @return Value of UpperRightCoord
   */
  FloatVec3Type getUpperRightCoord() const;
  Q_PROPERTY(FloatVec3Type UpperRightCoord READ getUpperRightCoord WRITE setUpperRightCoord)

  /**
   * @brief Setter property for PartitioningSchemeOrigin
   */
  void setPartitioningSchemeOrigin(const FloatVec3Type& value);
  /**
   * @brief Getter property for PartitioningSchemeOrigin
   * @return Value of PartitioningSchemeOrigin
   */
  FloatVec3Type getPartitioningSchemeOrigin() const;
  Q_PROPERTY(FloatVec3Type PartitioningSchemeOrigin READ getPartitioningSchemeOrigin WRITE setPartitioningSchemeOrigin)

  /**
   * @brief Setter property for LengthPerPartition
   */
  void setLengthPerPartition(const FloatVec3Type& value);
  /**
   * @brief Getter property for LengthPerPartition
   * @return Value of LengthPerPartition
   */
  FloatVec3Type getLengthPerPartition() const;
  Q_PROPERTY(FloatVec3Type LengthPerPartition READ getLengthPerPartition WRITE setLengthPerPartition)

  /**
   * @brief Setter property for NumberOfPartitionsPerAxis
   */
  void setNumberOfPartitionsPerAxis(const IntVec3Type& value);
  /**
   * @brief Getter property for NumberOfPartitionsPerAxis
   * @return Value of NumberOfPartitionsPerAxis
   */
  IntVec3Type getNumberOfPartitionsPerAxis() const;
  Q_PROPERTY(IntVec3Type NumberOfPartitionsPerAxis READ getNumberOfPartitionsPerAxis WRITE setNumberOfPartitionsPerAxis)

  /**
   * @brief Setter property for OutOfBoundsValue
   */
  void setOutOfBoundsValue(const int& value);
  /**
   * @brief Getter property for OutOfBoundsValue
   * @return Value of OutOfBoundsValue
   */
  int getOutOfBoundsValue() const;
  Q_PROPERTY(int OutOfBoundsValue READ getOutOfBoundsValue WRITE setOutOfBoundsValue)

  /**
   * @brief Setter property for StartingPartitionID
   */
  void setStartingPartitionID(const int& value);
  /**
   * @brief Getter property for StartingPartitionID
   * @return Value of StartingPartitionID
   */
  int getStartingPartitionID() const;
  Q_PROPERTY(int StartingPartitionID READ getStartingPartitionID WRITE setStartingPartitionID)

  /**
   * @brief Getter property for InputGeometryInformation
   * @return Value of InputGeometryInformation
   */
  QString getInputGeometryInformation() const;
  Q_PROPERTY(QString InputGeometryInformation READ getInputGeometryInformation)

  /**
   * @brief Getter property for PartitioningSchemeInformation
   * @return Value of PartitioningSchemeInformation
   */
  QString getPartitioningSchemeInformation() const;
  Q_PROPERTY(QString PartitioningSchemeInformation READ getPartitioningSchemeInformation)

  /**
   * @brief Setter property for PartitionIdsArrayName
   */
  void setPartitionIdsArrayName(const QString& value);
  /**
   * @brief Getter property for PartitionIdsArrayName
   * @return Value of PartitionIdsArrayName
   */
  QString getPartitionIdsArrayName() const;
  Q_PROPERTY(QString PartitionIdsArrayName READ getPartitionIdsArrayName WRITE setPartitionIdsArrayName)

  /**
   * @brief Setter property for SavePartitioningScheme
   */
  void setSavePartitioningScheme(const bool& value);

  /**
   * @brief Getter property for SavePartitioningScheme
   * @return Value of SavePartitioningScheme
   */
  bool getSavePartitioningScheme() const;
  Q_PROPERTY(bool SavePartitioningScheme READ getSavePartitioningScheme WRITE setSavePartitioningScheme)

  /**
   * @brief Setter property for PSDataContainerPath
   */
  void setPSDataContainerPath(const DataArrayPath& value);
  /**
   * @brief Getter property for PSDataContainerPath
   * @return Value of PSDataContainerPath
   */
  DataArrayPath getPSDataContainerPath() const;
  Q_PROPERTY(DataArrayPath PSDataContainerPath READ getPSDataContainerPath WRITE setPSDataContainerPath)

  /**
   * @brief Setter property for PSAttributeMatrixName
   */
  void setPSAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for PSAttributeMatrixName
   * @return Value of PSAttributeMatrixName
   */
  QString getPSAttributeMatrixName() const;
  Q_PROPERTY(QString PSAttributeMatrixName READ getPSAttributeMatrixName WRITE setPSAttributeMatrixName)

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
  PartitionGeometry();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  using PartitioningImageGeomResult = std::pair<ImageGeom::Pointer, std::optional<QString>>;

  int m_PartitioningMode = 0;
  IntVec3Type m_NumberOfPartitionsPerAxis = {};
  FloatVec3Type m_PartitioningSchemeOrigin = {0, 0, 0};
  FloatVec3Type m_LengthPerPartition = {1, 1, 1};
  FloatVec3Type m_LowerLeftCoord = {0, 0, 0};
  FloatVec3Type m_UpperRightCoord = {1, 1, 1};
  bool m_SavePartitioningScheme = {false};
  QString m_PartitionIdsArrayName = {"PartitioningSchemeIds"};
  DataArrayPath m_PSDataContainerPath = {"PartitioningSchemeDataContainer", "", ""};
  QString m_PSAttributeMatrixName = {"CellData"};
  QString m_PSDataArrayName = m_PartitionIdsArrayName;
  QString m_PSImageGeomName = {"PartitioningSchemeImageGeom"};
  int m_OutOfBoundsValue = 0;
  int m_StartingPartitionID = 1;
  DataArrayPath m_AttributeMatrixPath = {"", "", ""};
  //  DataArrayPath m_BoundingBoxPath = {"", "", ""};
  const QString m_VertexAttrMatrixName = "VertexData";

  std::weak_ptr<FloatArrayType> m_BoundingBoxPtr;
  float* m_BoundingBox = nullptr;
  std::weak_ptr<Int32ArrayType> m_PartitionIdsPtr;
  int32_t* m_PartitionIds = nullptr;
  std::weak_ptr<Int32ArrayType> m_PartitioningSchemeIdsPtr;
  int32_t* m_PartitioningSchemeIds = nullptr;
  PartitioningImageGeomResult m_PartitionImageGeometryResult;

  QString getInputImageGeometryInformation(const ImageGeom& geometry) const;
  QString getInputRectGridGeometryInformation() const;
  QString getInputVertexGeometryInformation() const;
  QString getInputEdgeGeometryInformation() const;
  QString getInputTriangleGeometryInformation() const;
  QString getInputQuadGeometryInformation() const;
  QString getInputTetrahedralGeometryInformation() const;
  QString getInputHexahedralGeometryInformation() const;
  QString getInputUnknownGeometryInformation() const;

  template <typename T>
  PartitioningImageGeomResult createPartitioningSchemeGeometry(const T& geometry);

  void partitionCellBasedGeometry(const IGeometryGrid& geometry, Int32ArrayType& partitionIds);
  void partitionNodeBasedGeometry(const SharedVertexList& vertexList, Int32ArrayType& partitionIds);

public:
  PartitionGeometry(const PartitionGeometry&) = delete;            // Copy Constructor Not Implemented
  PartitionGeometry& operator=(const PartitionGeometry&) = delete; // Copy Assignment Not Implemented
  PartitionGeometry(PartitionGeometry&&) = delete;                 // Move Constructor Not Implemented
  PartitionGeometry& operator=(PartitionGeometry&&) = delete;      // Move Assignment Not Implemented
};
