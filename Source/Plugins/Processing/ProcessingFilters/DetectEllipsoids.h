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
 * SERVICES; LOSS OF USE, Data, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
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

#include <complex>

#include <QtCore/QMutex>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/DataArray.hpp"

using DE_ComplexDoubleVector = std::vector<std::complex<double>>;

class DetectEllipsoidsImpl;

#include "Processing/ProcessingDLLExport.h"

/**
 * @brief The DetectEllipsoids class. See [Filter documentation](@ref fibertoolboxfilter) for details.
 */
class Processing_EXPORT DetectEllipsoids : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(DetectEllipsoids SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(DetectEllipsoids)
  PYB11_FILTER_NEW_MACRO(DetectEllipsoids)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, DetectedEllipsoidsFeatureIdsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureAttributeMatrixPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, EllipseFeatureAttributeMatrixPath)
  PYB11_FILTER_PARAMETER(QString, CenterCoordinatesArrayName)
  PYB11_FILTER_PARAMETER(QString, MajorAxisLengthArrayName)
  PYB11_FILTER_PARAMETER(QString, MinorAxisLengthArrayName)
  PYB11_FILTER_PARAMETER(QString, RotationalAnglesArrayName)
  PYB11_FILTER_PARAMETER(int, MinFiberAxisLength)
  PYB11_FILTER_PARAMETER(int, MaxFiberAxisLength)
  PYB11_FILTER_PARAMETER(float, HoughTransformThreshold)
  PYB11_FILTER_PARAMETER(float, MinAspectRatio)
  PYB11_FILTER_PARAMETER(int, ImageScaleBarLength)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath DetectedEllipsoidsFeatureIdsArrayPath READ getDetectedEllipsoidsFeatureIdsArrayPath WRITE setDetectedEllipsoidsFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath FeatureAttributeMatrixPath READ getFeatureAttributeMatrixPath WRITE setFeatureAttributeMatrixPath)
  PYB11_PROPERTY(DataArrayPath EllipseFeatureAttributeMatrixPath READ getEllipseFeatureAttributeMatrixPath WRITE setEllipseFeatureAttributeMatrixPath)
  PYB11_PROPERTY(QString CenterCoordinatesArrayName READ getCenterCoordinatesArrayName WRITE setCenterCoordinatesArrayName)
  PYB11_PROPERTY(QString MajorAxisLengthArrayName READ getMajorAxisLengthArrayName WRITE setMajorAxisLengthArrayName)
  PYB11_PROPERTY(QString MinorAxisLengthArrayName READ getMinorAxisLengthArrayName WRITE setMinorAxisLengthArrayName)
  PYB11_PROPERTY(QString RotationalAnglesArrayName READ getRotationalAnglesArrayName WRITE setRotationalAnglesArrayName)
  PYB11_PROPERTY(int MinFiberAxisLength READ getMinFiberAxisLength WRITE setMinFiberAxisLength)
  PYB11_PROPERTY(int MaxFiberAxisLength READ getMaxFiberAxisLength WRITE setMaxFiberAxisLength)
  PYB11_PROPERTY(float HoughTransformThreshold READ getHoughTransformThreshold WRITE setHoughTransformThreshold)
  PYB11_PROPERTY(float MinAspectRatio READ getMinAspectRatio WRITE setMinAspectRatio)
  PYB11_PROPERTY(int ImageScaleBarLength READ getImageScaleBarLength WRITE setImageScaleBarLength)
#endif

public:
  using Self = DetectEllipsoids;
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
   * @brief Returns the name of the class for DetectEllipsoids
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for DetectEllipsoids
   */
  static QString ClassName();

  ~DetectEllipsoids() override;

  friend class DetectEllipsoidsImpl;

  enum ScaleBarUnits
  {
    MillimeterUnits = 0,
    MicronUnits = 1
  };

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
   * @brief Setter property for DetectedEllipsoidsFeatureIdsArrayPath
   */
  void setDetectedEllipsoidsFeatureIdsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for DetectedEllipsoidsFeatureIdsArrayPath
   * @return Value of DetectedEllipsoidsFeatureIdsArrayPath
   */
  DataArrayPath getDetectedEllipsoidsFeatureIdsArrayPath() const;

  Q_PROPERTY(DataArrayPath DetectedEllipsoidsFeatureIdsArrayPath READ getDetectedEllipsoidsFeatureIdsArrayPath WRITE setDetectedEllipsoidsFeatureIdsArrayPath)

  /**
   * @brief Setter property for FeatureAttributeMatrixPath
   */
  void setFeatureAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureAttributeMatrixPath
   * @return Value of FeatureAttributeMatrixPath
   */
  DataArrayPath getFeatureAttributeMatrixPath() const;

  Q_PROPERTY(DataArrayPath FeatureAttributeMatrixPath READ getFeatureAttributeMatrixPath WRITE setFeatureAttributeMatrixPath)

  /**
   * @brief Setter property for EllipseFeatureAttributeMatrixPath
   */
  void setEllipseFeatureAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for EllipseFeatureAttributeMatrixPath
   * @return Value of EllipseFeatureAttributeMatrixPath
   */
  DataArrayPath getEllipseFeatureAttributeMatrixPath() const;

  Q_PROPERTY(DataArrayPath EllipseFeatureAttributeMatrixPath READ getEllipseFeatureAttributeMatrixPath WRITE setEllipseFeatureAttributeMatrixPath)

  /**
   * @brief Setter property for CenterCoordinatesArrayName
   */
  void setCenterCoordinatesArrayName(const QString& value);
  /**
   * @brief Getter property for CenterCoordinatesArrayName
   * @return Value of CenterCoordinatesArrayName
   */
  QString getCenterCoordinatesArrayName() const;

  Q_PROPERTY(QString CenterCoordinatesArrayName READ getCenterCoordinatesArrayName WRITE setCenterCoordinatesArrayName)

  /**
   * @brief Setter property for MajorAxisLengthArrayName
   */
  void setMajorAxisLengthArrayName(const QString& value);
  /**
   * @brief Getter property for MajorAxisLengthArrayName
   * @return Value of MajorAxisLengthArrayName
   */
  QString getMajorAxisLengthArrayName() const;

  Q_PROPERTY(QString MajorAxisLengthArrayName READ getMajorAxisLengthArrayName WRITE setMajorAxisLengthArrayName)

  /**
   * @brief Setter property for MinorAxisLengthArrayName
   */
  void setMinorAxisLengthArrayName(const QString& value);
  /**
   * @brief Getter property for MinorAxisLengthArrayName
   * @return Value of MinorAxisLengthArrayName
   */
  QString getMinorAxisLengthArrayName() const;

  Q_PROPERTY(QString MinorAxisLengthArrayName READ getMinorAxisLengthArrayName WRITE setMinorAxisLengthArrayName)

  /**
   * @brief Setter property for RotationalAnglesArrayName
   */
  void setRotationalAnglesArrayName(const QString& value);
  /**
   * @brief Getter property for RotationalAnglesArrayName
   * @return Value of RotationalAnglesArrayName
   */
  QString getRotationalAnglesArrayName() const;

  Q_PROPERTY(QString RotationalAnglesArrayName READ getRotationalAnglesArrayName WRITE setRotationalAnglesArrayName)

  /**
   * @brief Setter property for MinFiberAxisLength
   */
  void setMinFiberAxisLength(int value);
  /**
   * @brief Getter property for MinFiberAxisLength
   * @return Value of MinFiberAxisLength
   */
  int getMinFiberAxisLength() const;

  Q_PROPERTY(int MinFiberAxisLength READ getMinFiberAxisLength WRITE setMinFiberAxisLength)

  /**
   * @brief Setter property for MaxFiberAxisLength
   */
  void setMaxFiberAxisLength(int value);
  /**
   * @brief Getter property for MaxFiberAxisLength
   * @return Value of MaxFiberAxisLength
   */
  int getMaxFiberAxisLength() const;

  Q_PROPERTY(int MaxFiberAxisLength READ getMaxFiberAxisLength WRITE setMaxFiberAxisLength)

  /**
   * @brief Setter property for HoughTransformThreshold
   */
  void setHoughTransformThreshold(float value);
  /**
   * @brief Getter property for HoughTransformThreshold
   * @return Value of HoughTransformThreshold
   */
  float getHoughTransformThreshold() const;

  Q_PROPERTY(float HoughTransformThreshold READ getHoughTransformThreshold WRITE setHoughTransformThreshold)

  /**
   * @brief Setter property for MinAspectRatio
   */
  void setMinAspectRatio(float value);
  /**
   * @brief Getter property for MinAspectRatio
   * @return Value of MinAspectRatio
   */
  float getMinAspectRatio() const;

  Q_PROPERTY(float MinAspectRatio READ getMinAspectRatio WRITE setMinAspectRatio)

  /**
   * @brief Setter property for ImageScaleBarLength
   */
  void setImageScaleBarLength(int value);
  /**
   * @brief Getter property for ImageScaleBarLength
   * @return Value of ImageScaleBarLength
   */
  int getImageScaleBarLength() const;

  Q_PROPERTY(int ImageScaleBarLength READ getImageScaleBarLength WRITE setImageScaleBarLength)

  void incrementEllipseCount();

  /**
   * @brief getUniqueFeatureId
   * @return
   */
  int32_t getUniqueFeatureId();

  /**
   * @brief getNextFeatureId
   * @return
   */
  size_t getNextFeatureId();

  /**
   * @brief notifyFeatureCompleted
   * @return
   */
  void notifyFeatureCompleted(int featureId, int threadIndex);

  /**
   * @brief getThreadIndex Gets a new incremented thread Index for the private implemenetation to use.
   * @return
   */
  int getThreadIndex();

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
  DetectEllipsoids();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  DataArrayPath m_FeatureIdsArrayPath = {};
  DataArrayPath m_DetectedEllipsoidsFeatureIdsArrayPath = {};
  DataArrayPath m_FeatureAttributeMatrixPath = {};
  DataArrayPath m_EllipseFeatureAttributeMatrixPath = {};
  QString m_CenterCoordinatesArrayName = {};
  QString m_MajorAxisLengthArrayName = {};
  QString m_MinorAxisLengthArrayName = {};
  QString m_RotationalAnglesArrayName = {};
  int m_MinFiberAxisLength = {};
  int m_MaxFiberAxisLength = {};
  float m_HoughTransformThreshold = {};
  float m_MinAspectRatio = {};
  int m_ImageScaleBarLength = {};

  static double m_img_scale_length;
  int32_t m_MaxFeatureId = 0;
  int32_t m_NextExecutedFeatureId = 1;
  int32_t m_TotalNumberOfFeatures = 0;
  int32_t m_FeaturesCompleted = 0;
  size_t m_Ellipse_Count = 0;

  QMutex m_MaxFeatureIdMutex;
  QMutex m_NextExecutedFeatureIdMutex;
  QMutex m_FeaturesCompletedMutex;
  QMutex m_IncrementCountMutex;

  int m_ThreadIndex = 0;
  QMap<int, int> m_ThreadWork;

  AttributeMatrix::Pointer m_EllipseFeatureAttributeMatrixPtr;
  Int32ArrayType::Pointer m_DetectedEllipsoidsFeatureIdsPtr;
  DoubleArrayType::Pointer m_CenterCoordinatesPtr;
  DoubleArrayType::Pointer m_MajorAxisLengthArrayPtr;
  DoubleArrayType::Pointer m_MinorAxisLengthArrayPtr;
  DoubleArrayType::Pointer m_RotationalAnglesArrayPtr;

  /**
   * @brief orientationFilter
   * @return
   */
  DoubleArrayType::Pointer orientationFilter(int minAxisLength, int maxAxisLength, std::vector<size_t>& tDims);

  /**
   * @brief houghCircleFilter
   * @return
   */
  DE_ComplexDoubleVector houghCircleFilter(int minAxisLength, int maxAxisLength);

  /**
   * @brief convolutionFilter Multiplies the orientationFilter and houghCircleFilter arrays element-by-element.
   * @param orientationFilter OrientationFilter array input
   * @param houghCircleFilter HoughCircleFilter array input
   * @param convCoords_X
   * @param convCoords_Y
   * @param convCoords_Z
   * @return
   */
  void convolutionFilter(DoubleArrayType::Pointer orientationFilter, DE_ComplexDoubleVector houghCircleFilter, DE_ComplexDoubleVector& convCoords_X, DE_ComplexDoubleVector& convCoords_Y,
                         DE_ComplexDoubleVector& convCoords_Z);

  /**
   * @brief createConvOffsetArray
   * @param kernel_tDims
   * @return
   */
  Int32ArrayType::Pointer createOffsetArray(std::vector<size_t> kernel_tDims);

  /**
   * @brief smoothingFilter
   * @param n_size
   * @return
   */
  std::vector<double> smoothingFilter(int n_size, std::vector<size_t>& tDims);

  /**
   * @brief plotEllipsev2
   * @param xc
   * @param yc
   * @param p
   * @param q
   * @param theta
   * @param count
   * @return
   */
  DoubleArrayType::Pointer plotEllipsev2(double xc, double yc, double p, double q, double theta, size_t& count);

  /**
   * @brief fillEllipse
   * @param I
   * @param I_tDims
   * @param xc
   * @param yc
   * @param p
   * @param q
   * @param theta
   * @param val
   * @return
   */
  Int32ArrayType::Pointer fillEllipse(Int32ArrayType::Pointer I, std::vector<size_t> I_tDims, double xc, double yc, double p, double q, double theta, double val);

  /**
   * @brief sub2ind Helper Method.  Computes index from matrix coordinates
   * @param tDims
   * @param x
   * @param y
   * @param z
   * @return
   */
  size_t sub2ind(std::vector<size_t> tDims, size_t x, size_t y, size_t z) const;

  /**
   * @brief ind2sub Helper Method.  Computes matrix coordinates from index
   * @param tDims
   * @param index
   * @param x
   * @param y
   * @param z
   */
  void ind2sub(std::vector<size_t> tDims, size_t index, size_t& x, size_t& y, size_t& z) const;

public:
  DetectEllipsoids(const DetectEllipsoids&) = delete;            // Copy Constructor Not Implemented
  DetectEllipsoids(DetectEllipsoids&&) = delete;                 // Move Constructor Not Implemented
  DetectEllipsoids& operator=(const DetectEllipsoids&) = delete; // Copy Assignment Not Implemented
  DetectEllipsoids& operator=(DetectEllipsoids&&) = delete;      // Move Assignment Not Implemented
};

