/* ============================================================================
 * Software developed by US federal government employees (including military personnel)
 * as part of their official duties is not subject to copyright protection and is
 * considered "public domain" (see 17 USC Section 105). Public domain software can be used
 * by anyone for any purpose, and cannot be released under a copyright license
 * (including typical open source software licenses).
 *
 * This source code file was originally written by United States DoD employees. The
 * original source code files are released into the Public Domain.
 *
 * Subsequent changes to the codes by others may elect to add a copyright and license
 * for those changes.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _LabelTriangleGeometry_h_
#define _LabelTriangleGeometry_h_

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SurfaceMeshing/SurfaceMeshingDLLExport.h"

/**
 * @brief The LabelTriangleGeometry class. See [Filter documentation](@ref LabelTriangleGeometry) for details.
 */
class SurfaceMeshing_EXPORT LabelTriangleGeometry : public AbstractFilter
{
  Q_OBJECT

public:
  using Self = LabelTriangleGeometry;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<LabelTriangleGeometry> New();

  /**
   * @brief Returns the name of the class for LabelTriangleGeometry
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for LabelTriangleGeometry
   */
  static QString ClassName();

  ~LabelTriangleGeometry() override;

  /**
   * @brief Setter property for CADDataContainerName
   */
  void setCADDataContainerPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CADDataContainerName
   * @return Value of CADDataContainerName
   */
  DataArrayPath getCADDataContainerPath() const;
  Q_PROPERTY(DataArrayPath CADDataContainerPath READ getCADDataContainerPath WRITE setCADDataContainerPath)

  /**
   * @brief Setter property for TriangleAttributeMatrixName
   */
  void setTriangleAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for TriangleAttributeMatrixName
   * @return Value of TriangleAttributeMatrixName
   */
  QString getTriangleAttributeMatrixName() const;
  Q_PROPERTY(QString TriangleAttributeMatrixName READ getTriangleAttributeMatrixName WRITE setTriangleAttributeMatrixName)

  /**
   * @brief Setter property for RegionIdArrayName
   */
  void setRegionIdArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for RegionIdArrayName
   * @return Value of RegionIdArrayName
   */
  DataArrayPath getRegionIdArrayPath() const;
  Q_PROPERTY(DataArrayPath RegionIdArrayName READ getRegionIdArrayPath WRITE setRegionIdArrayPath)

  /**
   * @brief Setter property for RegionIdArrayName
   */
  void setNumTrianglesArrayName(const QString& value);
  /**
   * @brief Getter property for RegionIdArrayName
   * @return Value of RegionIdArrayName
   */
  QString getNumTrianglesArrayName() const;
  Q_PROPERTY(QString NumTrianglesArrayName READ getNumTrianglesArrayName WRITE setNumTrianglesArrayName)

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

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

protected:
  LabelTriangleGeometry();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief updateEdgeInstancePointers
   */
  void updateTriangleInstancePointers();

private:
  std::weak_ptr<DataArray<uint64_t>> m_NumTrianglesPtr;
  uint64_t* m_NumTriangles = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_RegionIdPtr;
  int32_t* m_RegionId = nullptr;

  DataArrayPath m_CADDataContainerPath = {"", "", ""};
  QString m_TriangleAttributeMatrixName = {"Triangle Features"};
  DataArrayPath m_RegionIdArrayPath = {"", "", "FeatureIds"};
  QString m_NumTrianglesArrayName = {"NuMTriangles"};

public:
  LabelTriangleGeometry(const LabelTriangleGeometry&) = delete;            // Copy Constructor Not Implemented
  LabelTriangleGeometry(LabelTriangleGeometry&&) = delete;                 // Move Constructor Not Implemented
  LabelTriangleGeometry& operator=(const LabelTriangleGeometry&) = delete; // Copy Assignment Not Implemented
  LabelTriangleGeometry& operator=(LabelTriangleGeometry&&) = delete;      // Move Assignment Not Implemented
};

#endif /* _LabelTriangleGeometry_H_ */
