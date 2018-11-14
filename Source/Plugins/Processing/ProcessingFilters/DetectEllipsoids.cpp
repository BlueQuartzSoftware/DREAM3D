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

#include "DetectEllipsoids.h"

#define NOMINMAX

#include <QtCore/QDateTime>

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/atomic.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/FilterParameters/AttributeMatrixCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "Processing/ProcessingConstants.h"
#include "Processing/ProcessingVersion.h"
#include "ProcessingFilters/HelperClasses/DetectEllipsoidsImpl.h"

#include <cmath>
#include <limits>

#define STORE_PIXEL_VALUES(array, count)                                                                                                                                                               \
  array->setComponent(count, 0, xc + x);                                                                                                                                                               \
  array->setComponent(count, 1, yc + y);                                                                                                                                                               \
  count++;                                                                                                                                                                                             \
  array->setComponent(count, 0, xc - x);                                                                                                                                                               \
  array->setComponent(count, 1, yc - y);                                                                                                                                                               \
  count++;

double DetectEllipsoids::m_img_scale_length = 588.0;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DetectEllipsoids::DetectEllipsoids()
: m_EllipseFeatureAttributeMatrixPath("", "", "")
, m_CenterCoordinatesArrayName("EllipsoidsCenterCoords")
, m_MajorAxisLengthArrayName("EllipsoidsMajorAxisLength")
, m_MinorAxisLengthArrayName("EllipsoidsMinorAxisLength")
, m_RotationalAnglesArrayName("EllipsoidsRotationalAngles")
, m_MinFiberAxisLength(4)
, m_MaxFiberAxisLength(18)
, m_HoughTransformThreshold(0.5f)
, m_MinAspectRatio(0.4f)
, m_ImageScaleBarLength(100)
, m_Ellipse_Count(0)
, m_MaxFeatureId(0)
, m_NextExecutedFeatureId(1)
, m_TotalNumberOfFeatures(0)
, m_FeaturesCompleted(0)
, m_MaxFeatureIdSem(1)
, m_NextExecutedFeatureIdSem(1)
, m_FeaturesCompletedSem(1)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DetectEllipsoids::~DetectEllipsoids() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetectEllipsoids::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);

  m_TotalNumberOfFeatures = 0;
  m_NextExecutedFeatureId = 1;
  m_FeaturesCompleted = 0;
  m_MaxFeatureId = 0;

  // Initialize counter to track number of detected ellipses
  m_Ellipse_Count = 0;

  m_ThreadIndex = 0;
  m_ThreadWork.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetectEllipsoids::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(SIMPL_NEW_INTEGER_FP("Min Fiber Axis Length", MinFiberAxisLength, FilterParameter::Parameter, DetectEllipsoids));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Max Fiber Axis Length", MaxFiberAxisLength, FilterParameter::Parameter, DetectEllipsoids));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Threshold for Hough Transform", HoughTransformThreshold, FilterParameter::Parameter, DetectEllipsoids));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Minimum Aspect Ratio", MinAspectRatio, FilterParameter::Parameter, DetectEllipsoids));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Length of Image Scale Bar", ImageScaleBarLength, FilterParameter::Parameter, DetectEllipsoids));

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, DetectEllipsoids, req));
  }

  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Feature Attribute Matrix", FeatureAttributeMatrixPath, FilterParameter::RequiredArray, DetectEllipsoids, req));
  }

  {
    AttributeMatrixCreationFilterParameter::RequirementType req;
    req.dcGeometryTypes = IGeometry::Types(1, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_CREATION_FP("Ellipsoid Feature Attribute Matrix", EllipseFeatureAttributeMatrixPath, FilterParameter::CreatedArray, DetectEllipsoids, req));
  }

  {
    parameters.push_back(SIMPL_NEW_STRING_FP("Ellipsoid Center Coordinates", CenterCoordinatesArrayName, FilterParameter::CreatedArray, DetectEllipsoids));
  }

  {
    parameters.push_back(SIMPL_NEW_STRING_FP("Ellipsoid Major Axis Lengths", MajorAxisLengthArrayName, FilterParameter::CreatedArray, DetectEllipsoids));
  }

  {
    parameters.push_back(SIMPL_NEW_STRING_FP("Ellipsoid Minor Axis Lengths", MinorAxisLengthArrayName, FilterParameter::CreatedArray, DetectEllipsoids));
  }

  {
    parameters.push_back(SIMPL_NEW_STRING_FP("Ellipsoid Rotational Angles", RotationalAnglesArrayName, FilterParameter::CreatedArray, DetectEllipsoids));
  }

  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Detected Ellipsoids Feature Ids", DetectedEllipsoidsFeatureIdsArrayPath, FilterParameter::CreatedArray, DetectEllipsoids, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetectEllipsoids::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  getDataContainerArray()->getPrereqArrayFromPath<Int32ArrayType, AbstractFilter>(this, m_FeatureIdsArrayPath, QVector<size_t>(1, 1));

  m_DetectedEllipsoidsFeatureIdsPtr =
      getDataContainerArray()->createNonPrereqArrayFromPath<Int32ArrayType, AbstractFilter, int32_t>(this, m_DetectedEllipsoidsFeatureIdsArrayPath, 0, QVector<size_t>(1, 1));

  DataContainer::Pointer ellipseDC = getDataContainerArray()->getPrereqDataContainer(this, m_EllipseFeatureAttributeMatrixPath.getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  int err = 0;
  AttributeMatrix::Pointer featureAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, m_FeatureAttributeMatrixPath, err);
  if(getErrorCondition() < 0)
  {
    return;
  }

  m_TotalNumberOfFeatures = featureAM->getNumberOfTuples();

  m_EllipseFeatureAttributeMatrixPtr = ellipseDC->createNonPrereqAttributeMatrix(this, m_EllipseFeatureAttributeMatrixPath.getAttributeMatrixName(), QVector<size_t>(1, m_TotalNumberOfFeatures + 1),
                                                                                 AttributeMatrix::Type::CellFeature);

  DataArrayPath tmp = m_EllipseFeatureAttributeMatrixPath;
  tmp.setDataArrayName(m_CenterCoordinatesArrayName);
  m_CenterCoordinatesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DoubleArrayType, AbstractFilter, double>(this, tmp, std::numeric_limits<double>::quiet_NaN(), QVector<size_t>(1, 2));

  tmp.setDataArrayName(m_MajorAxisLengthArrayName);
  m_MajorAxisLengthArrayPtr =
      getDataContainerArray()->createNonPrereqArrayFromPath<DoubleArrayType, AbstractFilter, double>(this, tmp, std::numeric_limits<double>::quiet_NaN(), QVector<size_t>(1, 1));

  tmp.setDataArrayName(m_MinorAxisLengthArrayName);
  m_MinorAxisLengthArrayPtr =
      getDataContainerArray()->createNonPrereqArrayFromPath<DoubleArrayType, AbstractFilter, double>(this, tmp, std::numeric_limits<double>::quiet_NaN(), QVector<size_t>(1, 1));

  tmp.setDataArrayName(m_RotationalAnglesArrayName);
  m_RotationalAnglesArrayPtr =
      getDataContainerArray()->createNonPrereqArrayFromPath<DoubleArrayType, AbstractFilter, double>(this, tmp, std::numeric_limits<double>::quiet_NaN(), QVector<size_t>(1, 1));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetectEllipsoids::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetectEllipsoids::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  /* Finding the top-left and bottom-right corners of each featureId  */
  Int32ArrayType::Pointer cellFeatureIds = getDataContainerArray()->getPrereqArrayFromPath<Int32ArrayType, AbstractFilter>(this, m_FeatureIdsArrayPath, QVector<size_t>(1, 1));
  if(getErrorCondition() < 0)
  {
    return;
  }

  int* cellFeatureIdsPtr = cellFeatureIds->getPointer(0);
  if(cellFeatureIdsPtr != nullptr)
  {
    int featureId = 0;
    size_t numComps = 6;
    QVector<size_t> cDims(1, numComps);
    int err = 0;
    AttributeMatrix::Pointer featureAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, m_FeatureAttributeMatrixPath, err);

    // Create corners array, which stores pixel coordinates for the top-left and bottom-right coordinates of each feature object
    UInt32ArrayType::Pointer corners = UInt32ArrayType::CreateArray(featureAM->getTupleDimensions(), cDims, "Corners of Feature");
    for(int i = 0; i < corners->getNumberOfTuples(); i++)
    {
      corners->setComponent(i, 0, std::numeric_limits<uint32_t>::max());
      corners->setComponent(i, 1, std::numeric_limits<uint32_t>::max());
      corners->setComponent(i, 2, std::numeric_limits<uint32_t>::max());
      corners->setComponent(i, 3, std::numeric_limits<uint32_t>::min());
      corners->setComponent(i, 4, std::numeric_limits<uint32_t>::min());
      corners->setComponent(i, 5, std::numeric_limits<uint32_t>::min());
    }

    AttributeMatrix::Pointer featureIdsAM = getDataContainerArray()->getAttributeMatrix(m_FeatureIdsArrayPath);

    QVector<size_t> imageDims = featureIdsAM->getTupleDimensions();
    size_t xDim = imageDims[0], yDim = imageDims[1], zDim = imageDims[2];

    size_t index = 0;
    // Store the coordinates in the corners array
    for(size_t z = 0; z < zDim; z++)
    {
      for(size_t y = 0; y < yDim; y++)
      {
        for(size_t x = 0; x < xDim; x++)
        {
          index = sub2ind(imageDims, x, y, z); // Index into cellFeatureIds array

          featureId = cellFeatureIdsPtr[index];
          if(featureId == 0)
          {
            continue;
          }

          if(featureId >= corners->getNumberOfTuples())
          {
            setErrorCondition(-31000);
            QString ss = QObject::tr("The feature attribute matrix '%1' has a smaller tuple count than the maximum feature id in '%2'").arg(featureAM->getName()).arg(cellFeatureIds->getName());
            notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
            return;
          }

          uint32_t* featureCorner = corners->getPointer(featureId * numComps);

          uint32_t val = featureCorner[0];
          if(x < featureCorner[0])
          {
            featureCorner[0] = x;
          }
          val = featureCorner[1];
          if(y < featureCorner[1])
          {
            featureCorner[1] = y;
          }
          val = featureCorner[2];
          if(z < featureCorner[2])
          {
            featureCorner[2] = z;
          }

          val = featureCorner[3];
          if(x > featureCorner[3])
          {
            featureCorner[3] = x;
          }
          val = featureCorner[4];
          if(y > featureCorner[4])
          {
            featureCorner[4] = y;
          }
          val = featureCorner[5];
          if(z > featureCorner[5])
          {
            featureCorner[5] = z;
          }
        }
      }
    }

    double img_pix_length = m_ImageScaleBarLength / m_img_scale_length;
    double axis_min = std::round(m_MinFiberAxisLength / img_pix_length);
    double axis_max = std::round(m_MaxFiberAxisLength / img_pix_length);

    // Execute the Orientation Filter and Hough Circle Filter
    QVector<size_t> orient_tDims;
    DoubleArrayType::Pointer orientArray = orientationFilter(axis_min, axis_max, orient_tDims);
    DE_ComplexDoubleVector houghCircleVector = houghCircleFilter(axis_min, axis_max);

    if(orientArray->getNumberOfTuples() != houghCircleVector.size())
    {
      setErrorCondition(-31001);
      QString ss = QObject::tr("There was an internal error.  Please ask the DREAM.3D developers for more information.");
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

    // This convolution function fills the convCoords_X, convCoords_Y, and convCoords_Z arrays with values
    DE_ComplexDoubleVector convCoords_X;
    DE_ComplexDoubleVector convCoords_Y;
    DE_ComplexDoubleVector convCoords_Z;
    convolutionFilter(orientArray, houghCircleVector, convCoords_X, convCoords_Y, convCoords_Z);

    // Reverse these kernels now so that we don't have to reverse them during every single convolution run on each feature id
    std::reverse(std::begin(convCoords_X), std::end(convCoords_X));
    std::reverse(std::begin(convCoords_Y), std::end(convCoords_Y));
    std::reverse(std::begin(convCoords_Z), std::end(convCoords_Z));

    // Create offset array to use for convolutions
    Int32ArrayType::Pointer convOffsetArray = createOffsetArray(orient_tDims);

    // Execute the smoothing filter
    int n_size = 3;
    QVector<size_t> smooth_tDims;
    std::vector<double> smoothFil = smoothingFilter(n_size, smooth_tDims);

    // Reverse this kernel now so that we don't have to reverse it during every single convolution run on each feature id
    std::reverse(std::begin(smoothFil), std::end(smoothFil));

    // Create offset array to use for smoothing convolutions
    Int32ArrayType::Pointer smoothOffsetArray = createOffsetArray(smooth_tDims);

    QString ss = QObject::tr("0/%2").arg(m_TotalNumberOfFeatures);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

    m_MaxFeatureId = m_TotalNumberOfFeatures;

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    tbb::task_scheduler_init init;
    bool doParallel = true;

    if(doParallel)
    {
      std::shared_ptr<tbb::task_group> g(new tbb::task_group);
      int threads = tbb::task_scheduler_init::default_num_threads();

      for(int i = 0; i < threads; i++)
      {
        m_ThreadWork[i] = 0;
        g->run(DetectEllipsoidsImpl(i, this, cellFeatureIdsPtr, imageDims, corners, convCoords_X, convCoords_Y, convCoords_Z, orient_tDims, convOffsetArray, smoothFil, smoothOffsetArray, axis_min,
                                    axis_max, m_HoughTransformThreshold, m_MinAspectRatio, m_CenterCoordinatesPtr, m_MajorAxisLengthArrayPtr, m_MinorAxisLengthArrayPtr, m_RotationalAnglesArrayPtr,
                                    m_EllipseFeatureAttributeMatrixPtr));
      }

      g->wait();
    }
    else
#endif
    {
      DetectEllipsoidsImpl impl(0, this, cellFeatureIdsPtr, imageDims, corners, convCoords_X, convCoords_Y, convCoords_Z, orient_tDims, convOffsetArray, smoothFil, smoothOffsetArray, axis_min,
                                axis_max, m_HoughTransformThreshold, m_MinAspectRatio, m_CenterCoordinatesPtr, m_MajorAxisLengthArrayPtr, m_MinorAxisLengthArrayPtr, m_RotationalAnglesArrayPtr,
                                m_EllipseFeatureAttributeMatrixPtr);
      m_ThreadWork[0] = 0;
      impl();
    }

    if(getCancel())
    {
      return;
    }

    // Plot each detected ellipse in the new Ellipse Detection Feature Ids array
    for(int featureId = 1; featureId < m_CenterCoordinatesPtr->getNumberOfTuples(); featureId++)
    {
      double cenx_val = m_CenterCoordinatesPtr->getComponent(featureId, 0);
      double ceny_val = m_CenterCoordinatesPtr->getComponent(featureId, 1);
      double majaxis_val = m_MajorAxisLengthArrayPtr->getValue(featureId);
      double minaxis_val = m_MinorAxisLengthArrayPtr->getValue(featureId);
      double rotangle_val = m_RotationalAnglesArrayPtr->getValue(featureId);

      double nan = std::numeric_limits<double>::quiet_NaN();
      if(cenx_val == nan || ceny_val == nan || majaxis_val == nan || minaxis_val == nan || rotangle_val == nan)
      {
        continue;
      }

      size_t count = 1;
      DoubleArrayType::Pointer ellipseCoords = plotEllipsev2(cenx_val, ceny_val, majaxis_val, minaxis_val, rotangle_val, count);
      for(int i = 1; i <= count; i++)
      {
        int x = static_cast<int>(ellipseCoords->getComponent(i, 1));
        int y = static_cast<int>(ellipseCoords->getComponent(i, 0));
        int z = 0; // 3DIM: This can be changed later to handle 3-dimensions

        if(x >= 0 && y >= 0 && x < xDim && y < yDim)
        {
          index = sub2ind(imageDims, x, y, z);
          m_DetectedEllipsoidsFeatureIdsPtr->setValue(index, featureId);
        }
      }
    }
  }

#if 0
  QMapIterator<int, int> i(m_ThreadWork);
  while (i.hasNext()) {
      i.next();
      std::cout << i.key() << ": " << i.value() << std::endl;
  }
#endif
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DoubleArrayType::Pointer DetectEllipsoids::orientationFilter(int minAxisLength, int maxAxisLength, QVector<size_t>& tDims)
{
  double doubleMax = static_cast<double>(maxAxisLength);
  double doubleMin = static_cast<double>(minAxisLength);
  double doubleMin_squared = doubleMin * doubleMin;
  double doubleMax_squared = doubleMax * doubleMax;

  size_t xDim = 2 * maxAxisLength + 1;
  size_t yDim = 2 * maxAxisLength + 1;
  size_t zDim = 1; // 3DIM: This can be changed later to handle 3-dimensions
  QVector<size_t> cDims(1, 3);
  tDims.clear();
  tDims.push_back(xDim);
  tDims.push_back(yDim);
  tDims.push_back(zDim);
  DoubleArrayType::Pointer orientationCoords = DoubleArrayType::CreateArray(tDims, cDims, "Orientation Coordinates");

  for(int z = 1; z <= zDim; z++)
  {
    for(int y = 1; y <= yDim; y++)
    {
      for(int x = 1; x <= xDim; x++)
      {
        int xIdx = x - 1;
        int yIdx = y - 1;
        int zIdx = z - 1;
        size_t index = sub2ind(tDims, xIdx, yIdx, zIdx);

        double m = static_cast<double>(y) - 1.0 - doubleMax;
        double n = static_cast<double>(x) - 1.0 - doubleMax;
        double theta = std::atan2(n, m);

        if((m * m) + (n * n) >= doubleMin_squared && (m * m) + (n * n) <= doubleMax_squared)
        {
          orientationCoords->setComponent(index, 0, std::cos(theta));
          orientationCoords->setComponent(index, 1, std::sin(theta));
          orientationCoords->setComponent(index, 2, 0); // 3DIM: This can be changed later to handle 3-dimensions
        }
        else
        {
          orientationCoords->setComponent(index, 0, 0);
          orientationCoords->setComponent(index, 1, 0);
          orientationCoords->setComponent(index, 2, 0);
        }
      }
    }
  }

  return orientationCoords;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DE_ComplexDoubleVector DetectEllipsoids::houghCircleFilter(int minAxisLength, int maxAxisLength)
{
  size_t xDim = 2 * maxAxisLength + 1;
  size_t yDim = 2 * maxAxisLength + 1;
  size_t zDim = 1; // 3DIM: This can be changed later to handle 3-dimensions
  size_t totalElements = xDim * yDim * zDim;
  QVector<size_t> tDims;
  tDims.push_back(xDim);
  tDims.push_back(yDim);
  tDims.push_back(zDim);
  DE_ComplexDoubleVector houghCircleCoords(totalElements);
  int minAxisLength_squared = minAxisLength * minAxisLength;
  int maxAxisLength_squared = maxAxisLength * maxAxisLength;

  for(int z = 1; z <= zDim; z++)
  {
    for(int y = 1; y <= yDim; y++)
    {
      for(int x = 1; x <= xDim; x++)
      {
        int xIdx = x - 1;
        int yIdx = y - 1;
        int zIdx = z - 1;

        size_t index = sub2ind(tDims, xIdx, yIdx, zIdx);

        double m = y - 1 - maxAxisLength;
        double n = x - 1 - maxAxisLength;
        double phi = (std::sqrt((m * m) + (n * n)) - minAxisLength) / (maxAxisLength - minAxisLength);

        if((m * m) + (n * n) >= minAxisLength_squared && (m * m) + (n * n) <= maxAxisLength_squared)
        {
          std::complex<double> complexVal(std::cos(2 * M_PI * phi), std::sin(2 * M_PI * phi));
          std::complex<double> value = 1.0 / 2.0 / M_PI / std::sqrt((m * m) + (n * n)) * complexVal;
          houghCircleCoords[index] = value;
        }
        else
        {
          houghCircleCoords[index] = 0;
        }
      }
    }
  }

  return houghCircleCoords;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetectEllipsoids::convolutionFilter(DoubleArrayType::Pointer orientationFilter, DE_ComplexDoubleVector houghCircleFilter, DE_ComplexDoubleVector& convCoords_X,
                                         DE_ComplexDoubleVector& convCoords_Y, DE_ComplexDoubleVector& convCoords_Z)
{
  if(orientationFilter->getNumberOfTuples() != houghCircleFilter.size() || orientationFilter->getNumberOfComponents() != 3)
  {
    return;
  }

  for(int i = 0; i < orientationFilter->getNumberOfTuples(); i++)
  {
    std::complex<double> hcValue = houghCircleFilter[i];

    double orientValue_X = orientationFilter->getComponent(i, 0);
    std::complex<double> valueX = orientValue_X * hcValue;
    convCoords_X.push_back(valueX);

    double orientValue_Y = orientationFilter->getComponent(i, 1);
    std::complex<double> valueY = orientValue_Y * hcValue;
    convCoords_Y.push_back(valueY);

    double orientValue_Z = orientationFilter->getComponent(i, 2);
    std::complex<double> valueZ = orientValue_Z * hcValue;
    convCoords_Z.push_back(valueZ);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> DetectEllipsoids::smoothingFilter(int n_size, QVector<size_t>& tDims)
{
  int xDim = 2 * n_size + 1;
  int yDim = 2 * n_size + 1;
  int zDim = 1; // 3DIM: This can be changed later to handle 3-dimensions
  tDims.clear();
  tDims.push_back(xDim);
  tDims.push_back(yDim);
  tDims.push_back(zDim);

  std::vector<double> smooth(xDim * yDim * zDim);
  int n_size_squared = n_size * n_size;

  for(int z = 0; z < zDim; z++)
  {
    for(int y = 0; y < yDim; y++)
    {
      for(int x = 0; x < xDim; x++)
      {
        int m = y - n_size;
        int n = x - n_size;
        int index = sub2ind(tDims, x, y, z);

        if(((m * m) + (n * n)) <= n_size_squared)
        {
          smooth[index] = 1;
        }
        else
        {
          smooth[index] = 0;
        }
      }
    }
  }

  return smooth;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Int32ArrayType::Pointer DetectEllipsoids::createOffsetArray(QVector<size_t> kernel_tDims)
{
  QVector<size_t> cDims(1, 3);
  Int32ArrayType::Pointer offsetArray = Int32ArrayType::CreateArray(kernel_tDims, cDims, "Coordinate Array");
  size_t xDim = kernel_tDims[0], yDim = kernel_tDims[1], zDim = kernel_tDims[2];
  int index = 0;

  for(int z = 0; z < zDim; z++)
  {
    for(int y = 0; y < yDim; y++)
    {
      for(int x = 0; x < xDim; x++)
      {
        int xVal = x - xDim / 2;
        offsetArray->setComponent(index, 0, xVal);
        int yVal = y - yDim / 2;
        offsetArray->setComponent(index, 1, yVal);
        int zVal = z - zDim / 2;
        offsetArray->setComponent(index, 2, zVal);
        index++;
      }
    }
  }

  return offsetArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DoubleArrayType::Pointer DetectEllipsoids::plotEllipsev2(double xc, double yc, double p, double q, double theta, size_t& count)
{
  // xc, yc = center of ellipse
  // p, q = length of semi-major and semi-minor axes, respectively
  // theta = angle of counterclockwise rotation of major axis from x-axis in radians

  //    if(isreal(xc) == 0 || isreal(yc) == 0 || isreal(p) == 0 || isreal(q) == 0 || isreal(theta) == 0)
  //    {
  //      // Error: Input must be real valued!
  //      return DoubleArrayType::NullPointer();
  //    }

  // theta must statisfy: -M_PI/2 < theta <= M_PI/2 (can be rotated due to symmetry)
  while(theta > M_PI_2)
  {
    theta = theta - M_PI;
  }
  while(theta <= -M_PI_2)
  {
    theta = theta + M_PI;
  }

  // if(theta >= 0) %(xa,xb) is in 1st quadrant and (xb,yb) is in 2nd quadrant
  // else (xa,xb) is in 4th quadrant and (xb,yb) is in 1nd quadrant
  double xa = p * cos(theta);
  double ya = p * sin(theta);
  double xb = -q * sin(theta);
  double yb = q * cos(theta);

  double xa_sq = xa * xa;
  double ya_sq = ya * ya;
  double xb_sq = xb * xb;
  double yb_sq = yb * yb;

  double xbyb_sqsq = std::pow((xb_sq + yb_sq), 2);
  double xaya_sqsq = std::pow((xa_sq + ya_sq), 2);

  // (xa,ya) and (xb,yb) are the points on the ellipse where the major and minor
  // axis intersect with the ellipse boundary

  double a = (xa_sq * xbyb_sqsq) + (xb_sq * xaya_sqsq);
  double b = (xa * ya * xbyb_sqsq) + (xb * yb * xaya_sqsq);
  double c = (ya_sq * xbyb_sqsq) + (yb_sq * xaya_sqsq);
  double d = xaya_sqsq * xbyb_sqsq;

  // a,b,c,d are the parameters of an ellipse centered at the origin such that
  // the ellipse is described by the eqution:
  //   A*x^2 + 2*B*x*y + C*y^2 = D

  // Initialize Values
  double y = -ya;
  double x = -xa;
  double dy = -((a * x) + (b * y));
  double dx = (b * x) + (c * y);

  // Round values to nearest whole integer
  a = std::round(a);
  b = std::round(b);
  c = std::round(c);
  d = std::round(d);
  x = std::round(x);
  y = std::round(y);
  dx = std::round(dx);
  dy = std::round(dy);

  // estimate number of points on ellipse for array pre-allocation using arc length

  // Estimate perimeter using approximate formula
  // (Note this is a bad approximation if the eccentricity is high)
  size_t perim = static_cast<size_t>(std::ceil((M_PI * sqrt(2 * (p * p + q * q) - std::pow((p - q), 2) / 2))));
  // Preallocate array using estimated perimeter
  DoubleArrayType::Pointer ellipseCoords = DoubleArrayType::CreateArray(perim, QVector<size_t>(1, 2), "Ellipse Coordinates");
  for(int i = 0; i < ellipseCoords->getNumberOfTuples(); i++)
  {
    ellipseCoords->setComponent(i, 0, std::numeric_limits<double>::quiet_NaN());
    ellipseCoords->setComponent(i, 1, std::numeric_limits<double>::quiet_NaN());
  }

  if(x <= 0 && y <= 0) // (-xa,-ya) is in the third quadrant or on the x axis
  {
    if(dx == 0 || std::abs(dy / dx) > 1) // 1. Slope at (-xa,-ya) is larger than 1 in magnitude. Five sub-arcs are drawn.
    {
      /* (a) Arc from (-xa, -ya) to a point (x0, y0) whose slope is
            infinite. For all points between, the ellipse has slope larger
            than 1 in magnitude, so y is always incremented at each step. */

      while(dx < 0) // loop until point with infinite slope occurs
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        y++;
        dy = dy - b;
        dx = dx + c;
        double sigma = a * x * x + 2 * b * x * y + c * y * y - d;
        if(sigma < 0)
        {
          x--;
          dy = dy + a;
          dx = dx - b;
        }
      }

      /* (b) Arc from (x0, y0) to a point (x1, y1) whose slope is 1. For
            all points between, the ellipse has slope larger than 1 in
            magnitude, so y is always incremented at each step. */
      while(dy > dx)
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        y++;
        dy = dy - b;
        dx = dx + c;
        double sigma = a * (x + 1) * (x + 1) + 2 * b * (x + 1) * y + c * y * y - d;
        if(sigma >= 0)
        {
          x++;
          dy = dy - a;
          dx = dx + b;
        }
      }

      /* (c) Arc from (x1, y1) to a point (x2, y2) whose slope is 0. For
            all points between, the ellipse has slope less than 1 in
            magnitude, so x is always incremented at each step. */
      while(dy > 0)
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        x++;
        dy = dy - a;
        dx = dx + b;
        double sigma = a * x * x + 2 * b * x * y + c * y * y - d;
        if(sigma < 0)
        {
          y++;
          dy = dy - b;
          dx = dx + c;
        }
      }

      /* (d) Arc from (x2, y2) to a point (x3, y3) whose slope is -1. For
            all points between, the ellipse has slope less than 1 in
            magnitude, so x is always incremented at each step. */
      while(std::abs(dy) < dx)
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        x++;
        dy = dy - a;
        dx = dx + b;
        double sigma = a * x * x + 2 * b * x * (y - 1) + c * (y - 1) * (y - 1) - d;
        if(sigma >= 0)
        {
          y--;
          dy = dy + b;
          dx = dx - c;
        }
      }

      /* (e) Arc from (x3, y3) to (xa, ya). For all points between, the
            ellipse has slope larger than 1 in magnitude, so y is always
            decremented at each step. */
      while(y > ya)
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        y--;
        dy = dy + b;
        dx = dx - c;
        double sigma = a * x * x + 2 * b * x * y + c * y * y - d;
        if(sigma < 0)
        {
          x++;
          dy = dy - a;
          dx = dx + b;
        }
      }
    }
    else // 2. Slope at (-xa,-ya) is smaller than equal to 1 in magnitude. Five subarcs are drawn (i.e., abs(dy/dx) <= 1 ).
    {
      /* (a) Arc from (-xa, -ya) to a point (x0, y0) whose slope is -1. For
            all points between, the ellipse has slope less than 1 in
            magnitude, so x is always decremented at each step. */
      while(dy < std::abs(dx)) // loop until point with infinite slope occurs
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        x--;
        dy = dy + a;
        dx = dx - b;
        double sigma = a * x * x + 2 * b * x * (y + 1) + c * (y + 1) * (y + 1) - d;
        if(sigma >= 0)
        {
          y++;
          dy = dy - b;
          dx = dx + c;
        }
      }

      /* (b) Arc from (x0, y0) to a point (x1, y1) whose slope is infinite. For
            all points between, the ellipse has slope larger than 1 in
            magnitude, so y is always incremented at each step. */
      while(dx < 0)
      {

        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        y++;
        dy = dy - b;
        dx = dx + c;
        double sigma = a * x * x + 2 * b * x * y + c * y * y - d;
        if(sigma < 0)
        {
          x--;
          dy = dy + a;
          dx = dx - b;
        }
      }

      /* (c) Arc from (x1, y1) to a point (x2, y2) whose slope is 1. For
         all points between, the ellipse has slope larger than 1 in
         magnitude, so y is always incremented at each step. */
      while(dy > dx)
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        y++;
        dy = dy - b;
        dx = dx + c;
        double sigma = a * (x + 1) * (x + 1) + 2 * b * (x + 1) * y + c * y * y - d;
        if(sigma >= 0)
        {
          x++;
          dy = dy - a;
          dx = dx + b;
        }
      }

      /* (d) Arc from (x2, y2) to a point (x3, y3) whose slope is
            zero. For all points between, the ellipse has slope less
            than 1 in magnitude, so x is always incremented at each step. */

      while(dy > 0) // loop until point with infinite slope occurs
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        x++;
        dy = dy - a;
        dx = dx + b;
        double sigma = a * x * x + 2 * b * x * y + c * y * y - d;
        if(sigma < 0)
        {
          y++;
          dy = dy - b;
          dx = dx + c;
        }
      }

      /* (e) Arc from (x3, y3) to (xa, ya). For all points between, the
            ellipse has slope less than 1 in magnitude, so x is always
            incremented at each step. */

      while(x < xa)
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        x++;
        dy = dy - a;
        dx = dx + b;
        double sigma = a * x * x + 2 * b * x * (y - 1) + c * (y - 1) * (y - 1) - d;
        if(sigma >= 0)
        {
          y--;
          dy = dy + b;
          dx = dx - c;
        }
      }
    }
  }
  else // (-xa,-xb) is in the second quadrant
  {
    if(std::abs(dy / dx) >= 1) // 1. Slope at (-xa,-ya) is greater than or equal to 1 in magnitude. Five subarcs are drawn.
    {
      /* (a) Arc from (-xa,-ya) to a point (x0, y0) whose slope is 1.
            For all points between, the ellipse has slope larger than 1 in
            magnitude, so y is incremented at each step. */
      while(dy > dx)
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        y++;
        dy = dy - b;
        dx = dx + c;
        double sigma = a * (x + 1) * (x + 1) + 2 * b * (x + 1) * y + c * y * y - d;
        if(sigma >= 0)
        {
          x++;
          dy = dy - a;
          dx = dx + b;
        }
      }

      /* (b) Arc from (x0, y0) to a point (x1, y1) whose slope is
            zero. For all points between, the ellipse has slope less
            than 1 in magnitude, so x is always incremented. */
      while(dy > 0)
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        x++;
        dy = dy - a;
        dx = dx + b;
        double sigma = a * x * x + 2 * b * x * y + c * y * y - d;
        if(sigma < 0)
        {
          y++;
          dy = dy - b;
          dx = dx + c;
        }
      }

      /* (c) Arc from (x1, y1) to a point (x2, y2) whose slope is -1.
            For all points between, the ellipse has slope less than 1 in
            magnitude, so x is always incremented at each step. */
      while(std::abs(dy) < dx)
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        x++;
        dy = dy - a;
        dx = dx + b;
        double sigma = a * x * x + 2 * b * x * (y - 1) + c * (y - 1) * (y - 1) - d;
        if(sigma >= 0)
        {
          y--;
          dy = dy + b;
          dx = dx - c;
        }
      }

      /* (d) Arc from (x2, y2) to a point (x3, y3) whose slope is infinity.
         For all points between, the ellipse has slope greater than 1 in
         magnitude, so y is always decremented at each step. */
      while(dx > 0)
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        y--;
        dy = dy + b;
        dx = dx - c;
        double sigma = a * x * x + 2 * b * x * y + c * y * y - d;
        if(sigma < 0)
        {
          x++;
          dy = dy - a;
          dx = dx + b;
        }
      }

      /* (e) Arc from (x3, y3) to (xa, ya). For all points between, the
          ellipse has slope greater than 1 in magnitude, so y is always
          decremented at each step. */
      while(y > ya)
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        y--;
        dy = dy + b;
        dx = dx - c;
        double sigma = a * (x - 1) * (x - 1) + 2 * b * (x - 1) * y + c * y * y - d;
        if(sigma >= 0)
        {
          x--;
          dy = dy + a;
          dx = dx - b;
        }
      }
    }

    else // 2. Slop at (-xa,-ya) is smaller than 1 in magnitude (i.e., dy/dx < 0)
    {
      /* (a) Arc from (-xa, -ya) to a point (x0, y0) whose slope is 0.
            For all points between, the ellipse has slope less than 1 in
            magnitude, so x is always incremented at each step. */
      while(dy > 0)
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        x++;
        dy = dy - a;
        dx = dx + b;
        double sigma = a * x * x + 2 * b * x * y + c * y * y - d;
        if(sigma < 0)
        {
          y++;
          dy = dy - b;
          dx = dx + c;
        }
      }

      /* (b) Arc from (x0,y0) to a point (x1, y1) whose slope is -1.
            For all points between, the ellipse has slope less than 1 in
            magnitude, so x is always decremented. */
      while(std::abs(dy) < dx)
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        x++;
        dy = dy - a;
        dx = dx + b;
        double sigma = a * x * x + 2 * b * x * (y - 1) + c * (y - 1) * (y - 1) - d;
        if(sigma >= 0)
        {
          y--;
          dy = dy + b;
          dx = dx - c;
        }
      }

      /* (c) Arc from (x1, y1) to a point (x2, y2) whose slope is
            infinite. For all points between, the ellipse has slope larger
            than 1, so y is always incremented. */
      while(dx > 0)
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        y--;
        dy = dy + b;
        dx = dx - c;
        double sigma = a * x * x + 2 * b * x * y + c * y * y - d;
        if(sigma < 0)
        {
          x++;
          dy = dy - a;
          dx = dx + b;
        }
      }

      /* (d) Arc from (x2, y2) to a point (x3, y3) whose slope is 1.
            For all points between, the ellipse has slope larger than 1 in
            magnitude, so y is always decremented at each step. */
      while(dy < dx)
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        y--;
        dy = dy + b;
        dx = dx - c;
        double sigma = a * (x - 1) * (x - 1) + 2 * b * (x - 1) * y + c * y * y - d;
        if(sigma >= 0)
        {
          x--;
          dy = dy + a;
          dx = dx - b;
        }
      }

      /* (e) Arc from (x3, y3) to (xa, ya). For all points between, the
          ellipse has slope less than 1 in magnitude, so x is always
          incremented at each step. */
      while(x > xa)
      {
        // Store pixel values
        STORE_PIXEL_VALUES(ellipseCoords, count);

        x--;
        dy = dy + a;
        dx = dx - b;
        double sigma = a * x * x + 2 * b * x * y + c * y * y - d;
        if(sigma < 0)
        {
          y--;
          dy = dy + b;
          dx = dx - c;
        }
      }
    }
  }

  count--;

  return ellipseCoords;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Int32ArrayType::Pointer DetectEllipsoids::fillEllipse(Int32ArrayType::Pointer I, QVector<size_t> I_tDims, double xc, double yc, double p, double q, double theta, double val)
{
  /* if(theta >= 0) %(xa,xb) is in 1st quadrant and (xb,yb) is in 2nd quadrant
   else (xa,xb) is in 4th quadrant and (xb,yb) is in 1nd quadrant */
  double xa = p * std::cos(theta);
  double ya = p * std::sin(theta);
  double xb = -q * std::sin(theta);
  double yb = q * std::cos(theta);

  double xa_sq = xa * xa;
  double ya_sq = ya * ya;
  double xb_sq = xb * xb;
  double yb_sq = yb * yb;

  double xbyb_sqsq = std::pow((xb_sq + yb_sq), 2);
  double xaya_sqsq = std::pow((xa_sq + ya_sq), 2);

  /* (xa,ya) and (xb,yb) are the points on the ellipse where the major and
   minor axis intersect with the ellipse boundary */

  double a = (xa_sq * xbyb_sqsq) + (xb_sq * xaya_sqsq);
  double b = (xa * ya * xbyb_sqsq) + (xb * yb * xaya_sqsq);
  double c = (ya_sq * xbyb_sqsq) + (yb_sq * xaya_sqsq);
  double d = xaya_sqsq * xbyb_sqsq;

  /* a,b,c,d are the parameters of an ellipse centered at the origin such that
   the ellipse is described by the equation:
   A*x^2 + 2*B*x*y + C*y^2 = D */

  int maxStack = 1000; // Initial stack size

  // Artifical stack
  DoubleArrayType::Pointer stackX = DoubleArrayType::CreateArray(maxStack, QVector<size_t>(1, 1), "stackX");
  stackX->initializeWithZeros();
  DoubleArrayType::Pointer stackY = DoubleArrayType::CreateArray(maxStack, QVector<size_t>(1, 1), "stackY");
  stackY->initializeWithZeros();

  // push back current point to begin search
  stackX->setValue(0, xc);
  stackY->setValue(0, yc);
  int stackSize = 0;

  size_t sizeX = I_tDims[1];
  size_t sizeY = I_tDims[0];

  if(xc <= 0 || xc > sizeX || yc <= 0 || yc > sizeY)
  {
    // Error: Fill ellipse initiated outside of image boundary!
    return Int32ArrayType::NullPointer();
  }

  Int32ArrayType::Pointer I_tmp = Int32ArrayType::CreateArray(I_tDims, QVector<size_t>(1, 1), I->getName());
  I_tmp->initializeWithZeros();

  bool copy = I->copyIntoArray(I_tmp);
  if(!copy)
  {
    // Error: Could not copy array contents into new array!
    return Int32ArrayType::NullPointer();
  }

  while(stackSize >= 0)
  {
    double x = stackX->getValue(stackSize);
    double y = stackY->getValue(stackSize);
    double z = 0; // 3DIM: This can be changed later to handle 3-dimensions
    stackSize--;

    double xt = x - xc;
    double yt = y - yc;

    double sigma = a * xt * xt + 2 * b * xt * yt + c * yt * yt - d;

    size_t index = sub2ind(I_tDims, y, x, z);
    if(sigma <= 0 && I_tmp->getValue(index) != val) // fill in pixel
    {
      I_tmp->setValue(index, val);

      if(stackSize > maxStack - 5)
      {
        // Increase stack size
        stackX->resize(maxStack + 1000);
        stackX->initializeWithValue(0, maxStack);

        stackY->resize(maxStack + 1000);
        stackY->initializeWithValue(0, maxStack);
        maxStack = maxStack + 1000;
      }

      // x + 1
      if(x + 1 < sizeX)
      {
        stackSize++;
        stackX->setValue(stackSize, x + 1);
        stackY->setValue(stackSize, y);
      }

      // x - 1
      if(x - 1 >= 0)
      {
        stackSize++;
        stackX->setValue(stackSize, x - 1);
        stackY->setValue(stackSize, y);
      }

      // y + 1
      if(y + 1 < sizeY)
      {
        stackSize++;
        stackX->setValue(stackSize, x);
        stackY->setValue(stackSize, y + 1);
      }

      // y - 1
      if(y - 1 >= 0)
      {
        stackSize++;
        stackX->setValue(stackSize, x);
        stackY->setValue(stackSize, y - 1);
      }
    }
  }

  return I_tmp;
}

// -----------------------------------------------------------------------------
// Helper Method - Grabs Index From Matrix Coordinates
// -----------------------------------------------------------------------------
size_t DetectEllipsoids::sub2ind(QVector<size_t> tDims, size_t x, size_t y, size_t z) const
{
  return (tDims[1] * tDims[0] * z) + (tDims[0] * y) + x;
}

// -----------------------------------------------------------------------------
// Helper Method - Grabs Matrix Coordinates From Array Index
// -----------------------------------------------------------------------------
void DetectEllipsoids::ind2sub(QVector<size_t> tDims, size_t index, size_t& x, size_t& y, size_t& z) const
{
  x = (index % tDims[0]);
  y = (index / tDims[0]) % tDims[1];

  // 3DIM:
  if(tDims.size() > 2)
  {
    z = ((index / tDims[0]) / tDims[1]) % tDims[2];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t DetectEllipsoids::getUniqueFeatureId()
{
  m_MaxFeatureIdSem.acquire();
  m_MaxFeatureId++;
  int32_t id = m_MaxFeatureId;
  m_MaxFeatureIdSem.release();
  return id;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t DetectEllipsoids::getNextFeatureId()
{
  m_NextExecutedFeatureIdSem.acquire();
  int32_t featureId = m_NextExecutedFeatureId;
  if(m_NextExecutedFeatureId >= m_TotalNumberOfFeatures)
  {
    featureId = -1;
  }
  else
  {
    m_NextExecutedFeatureId++;
  }
  m_NextExecutedFeatureIdSem.release();
  return featureId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetectEllipsoids::notifyFeatureCompleted(int featureId, int threadIndex)
{

  m_FeaturesCompletedSem.acquire();
  m_ThreadWork[threadIndex]++;
  m_FeaturesCompleted++;
  QString ss = QObject::tr("[%1/%2] Completed:").arg(m_FeaturesCompleted).arg(m_TotalNumberOfFeatures);
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  m_FeaturesCompletedSem.release();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DetectEllipsoids::getThreadIndex()
{
  return m_ThreadIndex++;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer DetectEllipsoids::newFilterInstance(bool copyFilterParameters) const
{
  DetectEllipsoids::Pointer filter = DetectEllipsoids::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DetectEllipsoids::getCompiledLibraryName() const
{
  return ProcessingConstants::ProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DetectEllipsoids::getBrandingString() const
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DetectEllipsoids::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Processing::Version::Major() << "." << Processing::Version::Minor() << "." << Processing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DetectEllipsoids::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid DetectEllipsoids::getUuid()
{
  return QUuid("{14a39c35-8558-58c1-9d00-952396e6632b}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DetectEllipsoids::getSubGroupName() const
{
  return "FiberToolbox";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DetectEllipsoids::getHumanLabel() const
{
  return "Detect 2D Ellipses";
}
