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

#include <complex>

#include <QtCore/QMutex>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

using DE_ComplexDoubleVector = std::vector<std::complex<double>>;

class DetectEllipsoidsImpl;

#include "Processing/ProcessingDLLExport.h"

/**
 * @brief The DetectEllipsoids class. See [Filter documentation](@ref fibertoolboxfilter) for details.
 */
class Processing_EXPORT DetectEllipsoids : public AbstractFilter
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(DetectEllipsoids SUPERCLASS AbstractFilter)
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

public:
  SIMPL_SHARED_POINTERS(DetectEllipsoids)
  SIMPL_FILTER_NEW_MACRO(DetectEllipsoids)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(DetectEllipsoids, AbstractFilter)

  ~DetectEllipsoids() override;

  friend class DetectEllipsoidsImpl;

  enum ScaleBarUnits
  {
    MillimeterUnits = 0,
    MicronUnits = 1
  };

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, DetectedEllipsoidsFeatureIdsArrayPath)
  Q_PROPERTY(DataArrayPath DetectedEllipsoidsFeatureIdsArrayPath READ getDetectedEllipsoidsFeatureIdsArrayPath WRITE setDetectedEllipsoidsFeatureIdsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureAttributeMatrixPath)
  Q_PROPERTY(DataArrayPath FeatureAttributeMatrixPath READ getFeatureAttributeMatrixPath WRITE setFeatureAttributeMatrixPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, EllipseFeatureAttributeMatrixPath)
  Q_PROPERTY(DataArrayPath EllipseFeatureAttributeMatrixPath READ getEllipseFeatureAttributeMatrixPath WRITE setEllipseFeatureAttributeMatrixPath)

  SIMPL_FILTER_PARAMETER(QString, CenterCoordinatesArrayName)
  Q_PROPERTY(QString CenterCoordinatesArrayName READ getCenterCoordinatesArrayName WRITE setCenterCoordinatesArrayName)

  SIMPL_FILTER_PARAMETER(QString, MajorAxisLengthArrayName)
  Q_PROPERTY(QString MajorAxisLengthArrayName READ getMajorAxisLengthArrayName WRITE setMajorAxisLengthArrayName)

  SIMPL_FILTER_PARAMETER(QString, MinorAxisLengthArrayName)
  Q_PROPERTY(QString MinorAxisLengthArrayName READ getMinorAxisLengthArrayName WRITE setMinorAxisLengthArrayName)

  SIMPL_FILTER_PARAMETER(QString, RotationalAnglesArrayName)
  Q_PROPERTY(QString RotationalAnglesArrayName READ getRotationalAnglesArrayName WRITE setRotationalAnglesArrayName)

  SIMPL_FILTER_PARAMETER(int, MinFiberAxisLength)
  Q_PROPERTY(int MinFiberAxisLength READ getMinFiberAxisLength WRITE setMinFiberAxisLength)

  SIMPL_FILTER_PARAMETER(int, MaxFiberAxisLength)
  Q_PROPERTY(int MaxFiberAxisLength READ getMaxFiberAxisLength WRITE setMaxFiberAxisLength)

  SIMPL_FILTER_PARAMETER(float, HoughTransformThreshold)
  Q_PROPERTY(float HoughTransformThreshold READ getHoughTransformThreshold WRITE setHoughTransformThreshold)

  SIMPL_FILTER_PARAMETER(float, MinAspectRatio)
  Q_PROPERTY(float MinAspectRatio READ getMinAspectRatio WRITE setMinAspectRatio)

  SIMPL_FILTER_PARAMETER(int, ImageScaleBarLength)
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
  const QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  const QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  const QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

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

