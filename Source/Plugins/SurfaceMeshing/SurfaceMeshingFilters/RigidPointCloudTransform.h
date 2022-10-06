/*
 * Your License or Copyright can go here
 */

#pragma once

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SurfaceMeshing/SurfaceMeshingPlugin.h"

#include <Eigen/Dense>
#include <QtCore/QDateTime>

typedef Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> MatrixNx3f;
typedef Eigen::Matrix<std::complex<float>, Eigen::Dynamic, 3, Eigen::RowMajor> MatrixNx3cf;
/**
 * @brief The RigidPointCloudTransform class. See [Filter documentation](@ref rigidpointcloudtransform) for details.
 */
class SurfaceMeshing_EXPORT RigidPointCloudTransform : public AbstractFilter
{
  Q_OBJECT

  // clang-format off
  PYB11_BEGIN_BINDINGS(RigidPointCloudTransform SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(RigidPointCloudTransform)
  PYB11_FILTER_NEW_MACRO(RigidPointCloudTransform)
  PYB11_PROPERTY(DataArrayPath MovingGeometry READ getMovingGeometry WRITE setMovingGeometry)
  PYB11_PROPERTY(DynamicTableData MovingKeyPoints READ getMovingKeyPoints WRITE setMovingKeyPoints)
  PYB11_PROPERTY(DynamicTableData FixedKeyPoints READ getFixedKeyPoints WRITE setFixedKeyPoints)

  PYB11_END_BINDINGS()
  // clang-format on

public:
  using Self = RigidPointCloudTransform;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for RigidPointCloudTransform
   */
  QString getNameOfClass() const override;

  /**
   * @brief Returns the name of the class for RigidPointCloudTransform
   */
  static QString ClassName();

  ~RigidPointCloudTransform() override;

  /**
   * @brief Sets the value for Filter Parameter for MovingGeometry
   * @param value The new value to use.
   */
  void setMovingGeometry(const DataArrayPath& value);
  /**
   * @brief Gets the Filter Parameter value for MovingGeometry
   * @return The value for MovingGeometry
   */
  DataArrayPath getMovingGeometry() const;
  Q_PROPERTY(DataArrayPath MovingGeometry READ getMovingGeometry WRITE setMovingGeometry)

  /**
   * @brief Sets the value for Filter Parameter for MovingKeyPoints
   * @param value The new value to use.
   */
  void setMovingKeyPoints(const DynamicTableData& value);
  /**
   * @brief Gets the Filter Parameter value for MovingKeyPoints
   * @return The value for MovingKeyPoints
   */
  DynamicTableData getMovingKeyPoints() const;
  Q_PROPERTY(DynamicTableData MovingKeyPoints READ getMovingKeyPoints WRITE setMovingKeyPoints)

  /**
   * @brief Sets the value for Filter Parameter for FixedKeyPoints
   * @param value The new value to use.
   */
  void setFixedKeyPoints(const DynamicTableData& value);
  /**
   * @brief Gets the Filter Parameter value for FixedKeyPoints
   * @return The value for FixedKeyPoints
   */
  DynamicTableData getFixedKeyPoints() const;
  Q_PROPERTY(DynamicTableData FixedKeyPoints READ getFixedKeyPoints WRITE setFixedKeyPoints)

  /**
   * @brief Sets the value for Filter Parameter for Millis
   * @param value The new value to use.
   */
  void RigidPointCloudTransform::setMillis(const qint64 value);
  /**
   * @brief Gets the Filter Parameter value for Millis
   * @return The value for GenerationCount
   */
  qint64 RigidPointCloudTransform::getMillis() const;
  /**
   * @brief Sets the value for Filter Parameter for GenerationCount
   * @param value The new value to use.
   */
  void RigidPointCloudTransform::setGenerationCount(const int value);
  /**
   * @brief Gets the Filter Parameter value for GenerationCount
   * @return The value for GenerationCount
   */
  int RigidPointCloudTransform::getGenerationCount() const;

  /**
   * @brief Sets the value for Filter Parameter for MovingKeyPointsMatrix
   * @param value The new value to use.
   */
  void setMovingKeyPointsMatrix(const MatrixNx3f& value);
  /**
   * @brief Gets the Filter Parameter value for MovingKeyPointsMatrix
   * @return The value for MovingKeyPoints
   */
  MatrixNx3f getMovingKeyPointsMatrix() const;

  /**
   * @brief Sets the value for Filter Parameter for FixedKeyPointsMatrix
   * @param value The new value to use.
   */
  void setFixedKeyPointsMatrix(const MatrixNx3f& value);
  /**
   * @brief Gets the Filter Parameter value for FixedKeyPointsMatrix
   * @return The value for FixedKeyPoints
   */
  MatrixNx3f getFixedKeyPointsMatrix() const;

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
   * @brief sendThreadSafeProgressMessage sends a message to GUI in a threadsafe manner
   */
  void RigidPointCloudTransform::sendThreadSafeProgressMessage(size_t numCompleted, size_t totalFeatures);

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
  RigidPointCloudTransform();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  DataArrayPath m_MovingGeometry;
  DynamicTableData m_MovingKeyPoints;
  DynamicTableData m_FixedKeyPoints;
  Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> m_MovingKeyPointsMatrix;
  Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> m_FixedKeyPointsMatrix;
  int m_GenerationCount = 10;
  qint64 m_Millis = QDateTime::currentMSecsSinceEpoch();

public:
  RigidPointCloudTransform(const RigidPointCloudTransform&) = delete;            // Copy Constructor Not Implemented
  RigidPointCloudTransform& operator=(const RigidPointCloudTransform&) = delete; // Copy Assignment Not Implemented
  RigidPointCloudTransform(RigidPointCloudTransform&&) = delete;                 // Move Constructor Not Implemented
  RigidPointCloudTransform& operator=(RigidPointCloudTransform&&) = delete;      // Move Assignment Not Implemented
};
