/*
 * Your License or Copyright can go here
 */

#pragma once

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/FilterParameters/FloatVec2FilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Reconstruction/ReconstructionPlugin.h"

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
  PYB11_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(FloatVec2Type XBounds READ getXBounds WRITE setXBounds)
  PYB11_PROPERTY(FloatVec2Type YBounds READ getYBounds WRITE setYBounds)
  PYB11_PROPERTY(FloatVec2Type ZBounds READ getZBounds WRITE setZBounds)
  PYB11_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)
  PYB11_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)
  PYB11_PROPERTY(DataArrayPath PartitionIdsArrayPath READ getPartitionIdsArrayPath WRITE setPartitionIdsArrayPath)
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
   * @brief Setter property for AttributeMatrixPath
   */
  void setAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for AttributeMatrixPath
   * @return Value of AttributeMatrixPath
   */
  DataArrayPath getAttributeMatrixPath() const;
  Q_PROPERTY(DataArrayPath AttributeMatrixPath READ getAttributeMatrixPath WRITE setAttributeMatrixPath)

  /**
   * @brief Setter property for PartitionOrigin
   */
  void setPartitionOrigin(const FloatVec3Type& value);
  /**
   * @brief Getter property for PartitionOrigin
   * @return Value of PartitionOrigin
   */
  FloatVec3Type getPartitionOrigin() const;
  Q_PROPERTY(FloatVec3Type PartitionOrigin READ getPartitionOrigin WRITE setPartitionOrigin)

  /**
   * @brief Setter property for PartitionSpacing
   */
  void setPartitionSpacing(const FloatVec3Type& value);
  /**
   * @brief Getter property for PartitionSpacing
   * @return Value of PartitionSpacing
   */
  FloatVec3Type getPartitionSpacing() const;
  Q_PROPERTY(FloatVec3Type PartitionSpacing READ getPartitionSpacing WRITE setPartitionSpacing)

  /**
   * @brief Setter property for PartitionDimensions
   */
  void setPartitionDimensions(const IntVec3Type& value);
  /**
   * @brief Getter property for PartitionDimensions
   * @return Value of PartitionDimensions
   */
  IntVec3Type getPartitionDimensions() const;
  Q_PROPERTY(IntVec3Type PartitionDimensions READ getPartitionDimensions WRITE setPartitionDimensions)

  /**
   * @brief Setter property for DefaultValue
   */
  void setDefaultValue(const int& value);
  /**
   * @brief Getter property for DefaultValue
   * @return Value of DefaultValue
   */
  int getDefaultValue() const;
  Q_PROPERTY(int DefaultValue READ getDefaultValue WRITE setDefaultValue)

  /**
   * @brief Getter property for GeometryString
   * @return Value of GeometryString
   */
  QString getGeometryString() const;
  Q_PROPERTY(QString GeometryString READ getGeometryString)

  /**
   * @brief Getter property for PartitionSpaceString
   * @return Value of PartitionSpaceString
   */
  QString getPartitionSpaceString() const;
  Q_PROPERTY(QString PartitionSpaceString READ getPartitionSpaceString)

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
  IntVec3Type m_PartitionDimensions = {};
  FloatVec3Type m_PartitionOrigin = {0, 0, 0};
  FloatVec3Type m_PartitionSpacing = {1, 1, 1};
  int m_DefaultValue = 0;
  DataArrayPath m_AttributeMatrixPath = {"", "", ""};
  QString m_PartitionIdsArrayName = {"PartitionIds"};

  std::weak_ptr<Int32ArrayType> m_PartitionIdsPtr;
  int32_t* m_PartitionIds = nullptr;

  QString getImageGeometrySpaceDescription(const ImageGeom& geometry) const;
  QString getRectGridGeometrySpaceDescription() const;
  QString getVertexGeometrySpaceDescription() const;
  QString getEdgeGeometrySpaceDescription() const;
  QString getTriangleGeometrySpaceDescription() const;
  QString getQuadGeometrySpaceDescription() const;
  QString getTetrahedralGeometrySpaceDescription() const;
  QString getHexahedralGeometrySpaceDescription() const;
  QString getUnknownGeometrySpaceDescription() const;

  void partitionCellBasedGeometry(const IGeometryGrid& geometry, const ImageGeom& partitionImageGeom, Int32ArrayType& partitionIds);
  void partitionNodeBasedGeometry(const SharedVertexList& vertexList, const ImageGeom& partitionImageGeom, Int32ArrayType& partitionIds);

public:
  PartitionGeometry(const PartitionGeometry&) = delete;            // Copy Constructor Not Implemented
  PartitionGeometry& operator=(const PartitionGeometry&) = delete; // Copy Assignment Not Implemented
  PartitionGeometry(PartitionGeometry&&) = delete;                 // Move Constructor Not Implemented
  PartitionGeometry& operator=(PartitionGeometry&&) = delete;      // Move Assignment Not Implemented
};
