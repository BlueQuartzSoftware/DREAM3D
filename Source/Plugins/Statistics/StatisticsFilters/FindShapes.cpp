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

#include "FindShapes.h"

#include <array>
#include <cmath>
#include <utility>

#include <Eigen/Core>

#include <QtCore/QTextStream>

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "OrientationLib/OrientationMath/OrientationArray.hpp"
#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"


using FloatVec3Type = std::array<float, 3>;

namespace
{
/**
 * @brief Sorts the 3 values
 * @param a First Value
 * @param b Second Value
 * @param c Third Value
 * @return The indices in their sorted order
 */
template <typename T>
std::array<size_t, 3> TripletSort(T a, T b, T c, bool lowToHigh)
{
  constexpr size_t A = 0;
  constexpr size_t B = 1;
  constexpr size_t C = 2;
  std::array<size_t, 3> idx = {0, 1, 2};
  if(a > b && a > c)
  {
    // sorted[2] = a;
    if(b > c)
    {
      // sorted[1] = b;
      // sorted[0] = c;
      idx = {C, B, A};
    }
    else
    {
      // sorted[1] = c;
      // sorted[0] = b;
      idx = {B, C, A};
    }
  }
  else if(b > a && b > c)
  {
    // sorted[2] = b;
    if(a > c)
    {
      // sorted[1] = a;
      // sorted[0] = c;
      idx = {C, A, B};
    }
    else
    {
      // sorted[1] = c;
      // sorted[0] = a;
      idx = {A, C, B};
    }
  }
  else if(a > b)
  {
    // sorted[1] = a;
    // sorted[0] = b;
    // sorted[2] = c;
    idx = {B, A, C};
  }
  else if(a >= c && b >= c)
  {
    // sorted[0] = c;
    // sorted[1] = a;
    // sorted[2] = b;
    idx = {C, A, B};
  }
  else
  {
    // sorted[0] = a;
    // sorted[1] = b;
    // sorted[2] = c;
    idx = {A, B, C};
  }

  if(!lowToHigh)
  {
    std::swap(idx[0], idx[2]);
  }
  return idx;
}

} // namespace


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindShapes::FindShapes() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindShapes::~FindShapes() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, FindShapes, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req =
        AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Feature Attribute Matrix", CellFeatureAttributeMatrixName, FilterParameter::RequiredArray, FindShapes, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Centroids", CentroidsArrayPath, FilterParameter::RequiredArray, FindShapes, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Omega3s", Omega3sArrayName, FilterParameter::CreatedArray, FindShapes));
  parameters.push_back(SIMPL_NEW_STRING_FP("Axis Lengths", AxisLengthsArrayName, FilterParameter::CreatedArray, FindShapes));
  parameters.push_back(SIMPL_NEW_STRING_FP("Axis Euler Angles", AxisEulerAnglesArrayName, FilterParameter::CreatedArray, FindShapes));
  parameters.push_back(SIMPL_NEW_STRING_FP("Aspect Ratios", AspectRatiosArrayName, FilterParameter::CreatedArray, FindShapes));
  parameters.push_back(SIMPL_NEW_STRING_FP("Volumes", VolumesArrayName, FilterParameter::CreatedArray, FindShapes));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setAspectRatiosArrayName(reader->readString("AspectRatiosArrayName", getAspectRatiosArrayName()));
  setAxisEulerAnglesArrayName(reader->readString("AxisEulerAnglesArrayName", getAxisEulerAnglesArrayName()));
  setAxisLengthsArrayName(reader->readString("AxisLengthsArrayName", getAxisLengthsArrayName()));
  setVolumesArrayName(reader->readString("VolumesArrayName", getVolumesArrayName()));
  setOmega3sArrayName(reader->readString("Omega3sArrayName", getOmega3sArrayName()));
  setCentroidsArrayPath(reader->readDataArrayPath("CentroidsArrayPath", getCentroidsArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::initialize()
{
  m_ScaleFactor = 1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::dataCheck()
{
  initialize();
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  m_FeatureMomentsPtr = DataArray<double>::CreateArray(0, QString("m_FeatureMomentsPtr"), true);
  m_FeatureEigenValsPtr = DataArray<double>::CreateArray(0, QString("m_FeatureEigenValsPtr"), true);

  QVector<size_t> cDims = {1};
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<Int32ArrayType, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims);

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getOmega3sArrayName());
  m_Omega3sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType, AbstractFilter, float>(this, tempPath, 0, cDims, "");

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getVolumesArrayName());
  m_VolumesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType, AbstractFilter>(this, tempPath, 0, cDims, "");

  cDims[0] = 3;
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getAxisLengthsArrayName());
  m_AxisLengthsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType, AbstractFilter, float>(this, tempPath, 0, cDims, "");

  m_CentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<FloatArrayType, AbstractFilter>(this, getCentroidsArrayPath(), cDims);

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getAxisEulerAnglesArrayName());
  m_AxisEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType, AbstractFilter>(this, tempPath, 0, cDims);

  cDims[0] = 2;
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getAspectRatiosArrayName());
  m_AspectRatiosPtr = getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType, AbstractFilter>(this, tempPath, 0, cDims, "");
}

#define FS_DECLARE_REF(TYPE, NAME, VAR)                                                                                                                                                                \
  TYPE::Pointer ptr_##NAME = m_##NAME##Ptr.lock();                                                                                                                                                     \
  TYPE& VAR = *ptr_##NAME;
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_moments()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();

  DoubleArrayType& featureMoments = *m_FeatureMomentsPtr; // Get a local reference to the Data Array
  DoubleArrayType& featureEigenVals = *m_FeatureEigenValsPtr;

  FS_DECLARE_REF(Int32ArrayType, FeatureIds, featureIds)
  FS_DECLARE_REF(FloatArrayType, Centroids, centroids)
  FS_DECLARE_REF(FloatArrayType, Volumes, volumes)
  FS_DECLARE_REF(FloatArrayType, Omega3s, omega3s)

  float u200 = 0.0f;
  float u020 = 0.0f;
  float u002 = 0.0f;
  float u110 = 0.0f;
  float u011 = 0.0f;
  float u101 = 0.0f;
  float xx = 0.0f, yy = 0.0f, zz = 0.0f, xy = 0.0f, xz = 0.0f, yz = 0.0f;

  size_t xPoints = imageGeom->getXPoints();
  size_t yPoints = imageGeom->getYPoints();
  size_t zPoints = imageGeom->getZPoints();
  FloatVec3Type spacing=imageGeom->getSpacing();
  FloatVec3Type origin;
  imageGeom->getOrigin(origin.data());

  // using a modified resolution to keep the moment calculations "small" and prevent exceeding numerical bounds.
  // scaleFactor is applied later to rescale the calculated axis lengths
  float modXRes = spacing[0] * static_cast<float>(m_ScaleFactor);
  float modYRes = spacing[1] * static_cast<float>(m_ScaleFactor);
  float modZRes = spacing[2] * static_cast<float>(m_ScaleFactor);

  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();

  for(size_t i = 0; i < numfeatures; i++)
  {
    featureMoments[6 * i + 0] = 0.0;
    featureMoments[6 * i + 1] = 0.0;
    featureMoments[6 * i + 2] = 0.0;
    featureMoments[6 * i + 3] = 0.0;
    featureMoments[6 * i + 4] = 0.0;
    featureMoments[6 * i + 5] = 0.0;
  }

  float x = 0.0f, y = 0.0f, z = 0.0f, x1 = 0.0f, x2 = 0.0f, y1 = 0.0f, y2 = 0.0f, z1 = 0.0f, z2 = 0.0f;
  float xdist1 = 0.0f, xdist2 = 0.0f, xdist3 = 0.0f, xdist4 = 0.0f, xdist5 = 0.0f, xdist6 = 0.0f, xdist7 = 0.0f, xdist8 = 0.0f;
  float ydist1 = 0.0f, ydist2 = 0.0f, ydist3 = 0.0f, ydist4 = 0.0f, ydist5 = 0.0f, ydist6 = 0.0f, ydist7 = 0.0f, ydist8 = 0.0f;
  float zdist1 = 0.0f, zdist2 = 0.0f, zdist3 = 0.0f, zdist4 = 0.0f, zdist5 = 0.0f, zdist6 = 0.0f, zdist7 = 0.0f, zdist8 = 0.0f;
  size_t zStride = 0, yStride = 0;
  for(size_t i = 0; i < zPoints; i++)
  {
    zStride = i * xPoints * yPoints;
    for(size_t j = 0; j < yPoints; j++)
    {
      yStride = j * xPoints;
      for(size_t k = 0; k < xPoints; k++)
      {
        int32_t gnum = featureIds[zStride + yStride + k];
        x = float(k * modXRes) + (origin[0] * static_cast<float>(m_ScaleFactor));
        y = float(j * modYRes) + (origin[1] * static_cast<float>(m_ScaleFactor));
        z = float(i * modZRes) + (origin[2] * static_cast<float>(m_ScaleFactor));
        x1 = x + (modXRes / 4.0f);
        x2 = x - (modXRes / 4.0f);
        y1 = y + (modYRes / 4.0f);
        y2 = y - (modYRes / 4.0f);
        z1 = z + (modZRes / 4.0f);
        z2 = z - (modZRes / 4.0f);
        xdist1 = (x1 - (centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
        ydist1 = (y1 - (centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
        zdist1 = (z1 - (centroids[gnum * 3 + 2] * static_cast<float>(m_ScaleFactor)));
        xdist2 = (x1 - (centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
        ydist2 = (y1 - (centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
        zdist2 = (z2 - (centroids[gnum * 3 + 2] * static_cast<float>(m_ScaleFactor)));
        xdist3 = (x1 - (centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
        ydist3 = (y2 - (centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
        zdist3 = (z1 - (centroids[gnum * 3 + 2] * static_cast<float>(m_ScaleFactor)));
        xdist4 = (x1 - (centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
        ydist4 = (y2 - (centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
        zdist4 = (z2 - (centroids[gnum * 3 + 2] * static_cast<float>(m_ScaleFactor)));
        xdist5 = (x2 - (centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
        ydist5 = (y1 - (centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
        zdist5 = (z1 - (centroids[gnum * 3 + 2] * static_cast<float>(m_ScaleFactor)));
        xdist6 = (x2 - (centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
        ydist6 = (y1 - (centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
        zdist6 = (z2 - (centroids[gnum * 3 + 2] * static_cast<float>(m_ScaleFactor)));
        xdist7 = (x2 - (centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
        ydist7 = (y2 - (centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
        zdist7 = (z1 - (centroids[gnum * 3 + 2] * static_cast<float>(m_ScaleFactor)));
        xdist8 = (x2 - (centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
        ydist8 = (y2 - (centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
        zdist8 = (z2 - (centroids[gnum * 3 + 2] * static_cast<float>(m_ScaleFactor)));

        xx = ((ydist1) * (ydist1)) + ((zdist1) * (zdist1)) + ((ydist2) * (ydist2)) + ((zdist2) * (zdist2)) + ((ydist3) * (ydist3)) + ((zdist3) * (zdist3)) + ((ydist4) * (ydist4)) +
             ((zdist4) * (zdist4)) + ((ydist5) * (ydist5)) + ((zdist5) * (zdist5)) + ((ydist6) * (ydist6)) + ((zdist6) * (zdist6)) + ((ydist7) * (ydist7)) + ((zdist7) * (zdist7)) +
             ((ydist8) * (ydist8)) + ((zdist8) * (zdist8));
        yy = ((xdist1) * (xdist1)) + ((zdist1) * (zdist1)) + ((xdist2) * (xdist2)) + ((zdist2) * (zdist2)) + ((xdist3) * (xdist3)) + ((zdist3) * (zdist3)) + ((xdist4) * (xdist4)) +
             ((zdist4) * (zdist4)) + ((xdist5) * (xdist5)) + ((zdist5) * (zdist5)) + ((xdist6) * (xdist6)) + ((zdist6) * (zdist6)) + ((xdist7) * (xdist7)) + ((zdist7) * (zdist7)) +
             ((xdist8) * (xdist8)) + ((zdist8) * (zdist8));
        zz = ((xdist1) * (xdist1)) + ((ydist1) * (ydist1)) + ((xdist2) * (xdist2)) + ((ydist2) * (ydist2)) + ((xdist3) * (xdist3)) + ((ydist3) * (ydist3)) + ((xdist4) * (xdist4)) +
             ((ydist4) * (ydist4)) + ((xdist5) * (xdist5)) + ((ydist5) * (ydist5)) + ((xdist6) * (xdist6)) + ((ydist6) * (ydist6)) + ((xdist7) * (xdist7)) + ((ydist7) * (ydist7)) +
             ((xdist8) * (xdist8)) + ((ydist8) * (ydist8));
        xy = ((xdist1) * (ydist1)) + ((xdist2) * (ydist2)) + ((xdist3) * (ydist3)) + ((xdist4) * (ydist4)) + ((xdist5) * (ydist5)) + ((xdist6) * (ydist6)) + ((xdist7) * (ydist7)) +
             ((xdist8) * (ydist8));
        yz = ((ydist1) * (zdist1)) + ((ydist2) * (zdist2)) + ((ydist3) * (zdist3)) + ((ydist4) * (zdist4)) + ((ydist5) * (zdist5)) + ((ydist6) * (zdist6)) + ((ydist7) * (zdist7)) +
             ((ydist8) * (zdist8));
        xz = ((xdist1) * (zdist1)) + ((xdist2) * (zdist2)) + ((xdist3) * (zdist3)) + ((xdist4) * (zdist4)) + ((xdist5) * (zdist5)) + ((xdist6) * (zdist6)) + ((xdist7) * (zdist7)) +
             ((xdist8) * (zdist8));

        featureMoments[gnum * 6 + 0] = featureMoments[gnum * 6 + 0] + static_cast<double>(xx);
        featureMoments[gnum * 6 + 1] = featureMoments[gnum * 6 + 1] + static_cast<double>(yy);
        featureMoments[gnum * 6 + 2] = featureMoments[gnum * 6 + 2] + static_cast<double>(zz);
        featureMoments[gnum * 6 + 3] = featureMoments[gnum * 6 + 3] + static_cast<double>(xy);
        featureMoments[gnum * 6 + 4] = featureMoments[gnum * 6 + 4] + static_cast<double>(yz);
        featureMoments[gnum * 6 + 5] = featureMoments[gnum * 6 + 5] + static_cast<double>(xz);
        volumes[gnum] = volumes[gnum] + 1.0;
      }
    }
  }
  double sphere = (2000.0 * M_PI * M_PI) / 9.0;
  // constant for moments because voxels are broken into smaller voxels
  double konst1 = static_cast<double>((modXRes / 2.0) * (modYRes / 2.0) * (modZRes / 2.0));
  // constant for volumes because voxels are counted as one
  double konst2 = static_cast<double>((spacing[0]) * (spacing[1]) * (spacing[2]));
  double konst3 = static_cast<double>((modXRes) * (modYRes) * (modZRes));
  double o3 = 0.0, vol5 = 0.0, omega3 = 0.0;
  for(size_t featureId = 1; featureId < numfeatures; featureId++)
  {
    // calculating the modified volume for the omega3 value
    vol5 = volumes[featureId] * konst3;
    volumes[featureId] = volumes[featureId] * konst2;
    featureMoments[featureId * 6 + 0] = featureMoments[featureId * 6 + 0] * konst1;
    featureMoments[featureId * 6 + 1] = featureMoments[featureId * 6 + 1] * konst1;
    featureMoments[featureId * 6 + 2] = featureMoments[featureId * 6 + 2] * konst1;
    featureMoments[featureId * 6 + 3] = -featureMoments[featureId * 6 + 3] * konst1;
    featureMoments[featureId * 6 + 4] = -featureMoments[featureId * 6 + 4] * konst1;
    featureMoments[featureId * 6 + 5] = -featureMoments[featureId * 6 + 5] * konst1;

    // Now store the 3x3 Matrix for the Eigen Value/Vectors
    Eigen::Matrix3f moment;
    // clang-format off
    moment <<
      featureMoments[featureId * 6 + 0], featureMoments[featureId * 6 + 3], featureMoments[featureId * 6 + 5],
      featureMoments[featureId * 6 + 3], featureMoments[featureId * 6 + 1], featureMoments[featureId * 6 + 4],
      featureMoments[featureId * 6 + 5], featureMoments[featureId * 6 + 4], featureMoments[featureId * 6 + 2];
    // clang-format on
    Eigen::EigenSolver<Eigen::Matrix3f> es(moment);
    Eigen::EigenSolver<Eigen::Matrix3f>::EigenvalueType eigenValues = es.eigenvalues();
    Eigen::EigenSolver<Eigen::Matrix3f>::EigenvectorsType eigenVectors = es.eigenvectors();

    // Returns the argument order sorted high to low
    std::array<size_t, 3> idxs = ::TripletSort(eigenValues[0].real(), eigenValues[1].real(), eigenValues[2].real(), false);
    featureEigenVals[featureId * 3 + 0] = eigenValues[idxs[0]].real();
    featureEigenVals[featureId * 3 + 1] = eigenValues[idxs[1]].real();
    featureEigenVals[featureId * 3 + 2] = eigenValues[idxs[2]].real();

    // EigenVector associated with the largest EigenValue goes in the 3rd column
    auto col = eigenVectors.col(idxs[0]);
    m_EFVec->setComponent(featureId, 2, col(0).real());
    m_EFVec->setComponent(featureId, 5, col(1).real());
    m_EFVec->setComponent(featureId, 8, col(2).real());

    // Then the next largest into the 2nd column
    col = eigenVectors.col(idxs[1]);
    m_EFVec->setComponent(featureId, 1, col(0).real());
    m_EFVec->setComponent(featureId, 4, col(1).real());
    m_EFVec->setComponent(featureId, 7, col(2).real());

    // The the smallest into the 1rst column
    col = eigenVectors.col(idxs[2]);
    m_EFVec->setComponent(featureId, 0, col(0).real());
    m_EFVec->setComponent(featureId, 3, col(1).real());
    m_EFVec->setComponent(featureId, 6, col(2).real());

    // Only for Omega3 below
    u200 = static_cast<float>((featureMoments[featureId * 6 + 1] + featureMoments[featureId * 6 + 2] - featureMoments[featureId * 6 + 0]) / 2.0f);
    u020 = static_cast<float>((featureMoments[featureId * 6 + 0] + featureMoments[featureId * 6 + 2] - featureMoments[featureId * 6 + 1]) / 2.0f);
    u002 = static_cast<float>((featureMoments[featureId * 6 + 0] + featureMoments[featureId * 6 + 1] - featureMoments[featureId * 6 + 2]) / 2.0f);
    u110 = static_cast<float>(-featureMoments[featureId * 6 + 3]);
    u011 = static_cast<float>(-featureMoments[featureId * 6 + 4]);
    u101 = static_cast<float>(-featureMoments[featureId * 6 + 5]);
    o3 = static_cast<double>((u200 * u020 * u002) + (2.0f * u110 * u101 * u011) - (u200 * u011 * u011) - (u020 * u101 * u101) - (u002 * u110 * u110));
    vol5 = pow(vol5, 5.0);
    omega3 = vol5 / o3;
    omega3 = omega3 / sphere;
    if(omega3 > 1)
    {
      omega3 = 1.0;
    }
    if(vol5 == 0.0)
    {
      omega3 = 0.0;
    }
    omega3s[featureId] = static_cast<float>(omega3);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_moments2D()
{
  DoubleArrayType& featureMoments = *m_FeatureMomentsPtr; // Get a local reference to the Data Array

  FS_DECLARE_REF(Int32ArrayType, FeatureIds, featureIds)
  FS_DECLARE_REF(FloatArrayType, Centroids, centroids)
  FS_DECLARE_REF(FloatArrayType, Volumes, volumes)

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();

  float xx = 0.0f, yy = 0.0f, xy = 0.0f;
  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();

  size_t xPoints = 0, yPoints = 0;
  FloatVec3Type spacing = m->getGeometryAs<ImageGeom>()->getSpacing();

  if(imageGeom->getXPoints() == 1)
  {
    xPoints = imageGeom->getYPoints();
    yPoints = imageGeom->getZPoints();
    spacing = imageGeom->getSpacing();
  }
  if(imageGeom->getYPoints() == 1)
  {
    xPoints = imageGeom->getXPoints();
    yPoints = imageGeom->getZPoints();
    spacing = imageGeom->getSpacing();
  }
  if(imageGeom->getZPoints() == 1)
  {
    xPoints = imageGeom->getXPoints();
    yPoints = imageGeom->getYPoints();
    spacing = imageGeom->getSpacing();
  }

  float modXRes = spacing[0] * m_ScaleFactor;
  float modYRes = spacing[1] * m_ScaleFactor;

  FloatVec3Type origin;
  imageGeom->getOrigin(origin.data());

  for(size_t featureId = 0; featureId < 6 * numfeatures; featureId++)
  {
    featureMoments[featureId] = 0.0;
  }

  size_t yStride = 0;
  for(size_t yPoint = 0; yPoint < yPoints; yPoint++)
  {
    yStride = yPoint * xPoints;
    for(size_t xPoint = 0; xPoint < xPoints; xPoint++)
    {
      int32_t gnum = featureIds[yStride + xPoint];
      float x = static_cast<float>(xPoint * modXRes) + (origin[0] * static_cast<float>(m_ScaleFactor));
      float y = static_cast<float>(yPoint * modYRes) + (origin[1] * static_cast<float>(m_ScaleFactor));
      float x1 = x + (modXRes / 4.0f);
      float x2 = x - (modXRes / 4.0f);
      float y1 = y + (modYRes / 4.0f);
      float y2 = y - (modYRes / 4.0f);
      float xdist1 = (x1 - (centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
      float ydist1 = (y1 - (centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
      float xdist2 = (x1 - (centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
      float ydist2 = (y2 - (centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
      float xdist3 = (x2 - (centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
      float ydist3 = (y1 - (centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
      float xdist4 = (x2 - (centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
      float ydist4 = (y2 - (centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
      xx = ((ydist1) * (ydist1)) + ((ydist2) * (ydist2)) + ((ydist3) * (ydist3)) + ((ydist4) * (ydist4));
      yy = ((xdist1) * (xdist1)) + ((xdist2) * (xdist2)) + ((xdist3) * (xdist3)) + ((xdist4) * (xdist4));
      xy = ((xdist1) * (ydist1)) + ((xdist2) * (ydist2)) + ((xdist3) * (ydist3)) + ((xdist4) * (ydist4));
      featureMoments[gnum * 6 + 0] = featureMoments[gnum * 6 + 0] + xx;
      featureMoments[gnum * 6 + 1] = featureMoments[gnum * 6 + 1] + yy;
      featureMoments[gnum * 6 + 2] = featureMoments[gnum * 6 + 2] + xy;
      volumes[gnum] = volumes[gnum] + 1.0;
    }
  }
  double konst1 = static_cast<double>((modXRes / 2.0f) * (modYRes / 2.0f));
  double konst2 = static_cast<double>(spacing[0] * spacing[1]);
  for(size_t featureId = 1; featureId < numfeatures; featureId++)
  {
    // Eq. 12 Moment matrix. Omega 2
    // Eq. 11 Omega 2
    // E1. 13 Omega 1
    // xx = u20 =
    volumes[featureId] = volumes[featureId] * konst2;                                // Area
    featureMoments[featureId * 6 + 0] = featureMoments[featureId * 6 + 0] * konst1;  // u20
    featureMoments[featureId * 6 + 1] = featureMoments[featureId * 6 + 1] * konst1;  // u02
    featureMoments[featureId * 6 + 2] = -featureMoments[featureId * 6 + 2] * konst1; // u11
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_axes()
{
  DoubleArrayType& featureEigenVals = *m_FeatureEigenValsPtr;

  FS_DECLARE_REF(FloatArrayType, AxisLengths, axisLengths)
  FS_DECLARE_REF(FloatArrayType, AspectRatios, aspectRatios)

  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();
  constexpr double multiplier = 1.0 / (4.0 * M_PI);
  for(size_t featureId = 1; featureId < numfeatures; featureId++)
  {
    double r1 = featureEigenVals[3 * featureId];
    double r2 = featureEigenVals[3 * featureId + 1];
    double r3 = featureEigenVals[3 * featureId + 2];

    // Adjust to ABC of ellipsoid volume
    double I1 = (15.0 * r1) * multiplier;
    double I2 = (15.0 * r2) * multiplier;
    double I3 = (15.0 * r3) * multiplier;
    double A = (I1 + I2 - I3) * 0.5;
    double B = (I1 + I3 - I2) * 0.5;
    double C = (I2 + I3 - I1) * 0.5;
    double a = (A * A * A * A) / (B * C);
    a = std::pow(a, 0.1);
    double b = B / A;
    b = std::sqrt(b) * a;
    double c = A / (a * a * a * b);

    axisLengths[3 * featureId] = static_cast<float>(a / m_ScaleFactor);
    axisLengths[3 * featureId + 1] = static_cast<float>(b / m_ScaleFactor);
    axisLengths[3 * featureId + 2] = static_cast<float>(c / m_ScaleFactor);
    double bovera = b / a;
    double covera = c / a;
    if(A == 0.0 || B == 0.0 || C == 0.0)
    {
      bovera = 0.0f;
      covera = 0.0f;
    }
    aspectRatios[2 * featureId] = bovera;
    aspectRatios[2 * featureId + 1] = covera;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_axes2D()
{
  DoubleArrayType& featureMoments = *m_FeatureMomentsPtr; // Get a local reference to the Data Array
  FS_DECLARE_REF(FloatArrayType, Volumes, volumes)
  FS_DECLARE_REF(FloatArrayType, AxisLengths, axisLengths)
  FS_DECLARE_REF(FloatArrayType, AspectRatios, aspectRatios)

  double Ixx = 0.0, Iyy = 0.0, Ixy = 0.0;

  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();

  FloatVec3Type spacing;

  if(imageGeom->getXPoints() == 1)
  {
    spacing = imageGeom->getSpacing();
  }
  if(imageGeom->getYPoints() == 1)
  {
    spacing = imageGeom->getSpacing();
  }
  if(imageGeom->getZPoints() == 1)
  {
    spacing = imageGeom->getSpacing();
  }

  double preterm = 4.0 / M_PI;
  preterm = std::pow(preterm, 0.25);

  for(size_t i = 1; i < numfeatures; i++)
  {
    Ixx = featureMoments[i * 6 + 0];
    Iyy = featureMoments[i * 6 + 1];
    Ixy = featureMoments[i * 6 + 2];
    double r1 = (Ixx + Iyy) / 2.0 + std::sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 - (Ixx * Iyy - Ixy * Ixy));
    double r2 = (Ixx + Iyy) / 2.0 - std::sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 - (Ixx * Iyy - Ixy * Ixy));
    if(r2 <= 0)
    {
      float tempScale1 = 1.0f;
      float tempScale2 = 1.0f;
      if(Ixx >= Iyy)
      {
        tempScale1 = spacing[0];
        tempScale2 = spacing[1];
      }
      if(Ixx < Iyy)
      {
        tempScale1 = spacing[1];
        tempScale2 = spacing[0];
      }
      axisLengths[3 * i] = volumes[i] / tempScale1;
      axisLengths[3 * i + 1] = volumes[i] / tempScale2;
      aspectRatios[2 * i] = tempScale2 / tempScale1;
      aspectRatios[2 * i + 1] = 0.0f;
      continue;
    }

    double postterm1 = r1 * r1 * r1 / r2;
    double postterm2 = r2 * r2 * r2 / r1;
    postterm1 = std::pow(postterm1, 0.125f);
    postterm2 = std::pow(postterm2, 0.125f);
    r1 = preterm * postterm1;
    r2 = preterm * postterm2;
    axisLengths[3 * i] = static_cast<float>(r1 / m_ScaleFactor);
    axisLengths[3 * i + 1] = static_cast<float>(r2 / m_ScaleFactor);
    aspectRatios[2 * i] = static_cast<float>(r2 / r1);
    aspectRatios[2 * i + 1] = 0.0f;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_axiseulers()
{
  FS_DECLARE_REF(FloatArrayType, Centroids, centroids)
  FS_DECLARE_REF(FloatArrayType, AxisEulerAngles, axisEulerAngles)

  size_t numfeatures = centroids.getNumberOfTuples();
  for(size_t featureId = 1; featureId < numfeatures; featureId++)
  {
    // insert principal unit vectors into rotation matrix representing Feature reference frame within the sample reference frame
    //(Note that the 3 direction is actually the long axis and the 1 direction is actually the short axis)
    // clang-format off
    size_t idx = featureId*9;
    float g[3][3] = {{(*m_EFVec)[idx + 0], (*m_EFVec)[idx + 3], (*m_EFVec)[idx + 6]},
                     {(*m_EFVec)[idx + 1], (*m_EFVec)[idx + 4], (*m_EFVec)[idx + 7]},
                     {(*m_EFVec)[idx + 2], (*m_EFVec)[idx + 5], (*m_EFVec)[idx + 8]}};
    // clang-format on

    // check for right-handedness
    typedef OrientationTransforms<FOrientArrayType, float> OrientationTransformType;
    OrientationTransformType::ResultType result = FOrientTransformsType::om_check(FOrientArrayType(g));

    if(result.result == 0)
    {
      g[2][0] *= -1.0f;
      g[2][1] *= -1.0f;
      g[2][2] *= -1.0f;
    }

    FOrientArrayType eu(3, 0.0f);
    FOrientTransformsType::om2eu(FOrientArrayType(g), eu);


    axisEulerAngles[3 * featureId] = eu[0];
    axisEulerAngles[3 * featureId + 1] = eu[1];
    axisEulerAngles[3 * featureId + 2] = eu[2];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_axiseulers2D()
{
  DoubleArrayType& featureMoments = *m_FeatureMomentsPtr;
  FS_DECLARE_REF(FloatArrayType, Centroids, centroids)
  FS_DECLARE_REF(FloatArrayType, AxisEulerAngles, axisEulerAngles)

  size_t numfeatures = centroids.getNumberOfTuples();

  for(size_t featureId = 1; featureId < numfeatures; featureId++)
  {
    double Ixx = featureMoments[featureId * 6 + 0];
    double Iyy = featureMoments[featureId * 6 + 1];
    double Ixy = featureMoments[featureId * 6 + 2];
    if(Ixy == 0)
    {
      if(Ixx > Iyy)
      {
        axisEulerAngles[3 * featureId] = SIMPLib::Constants::k_PiOver2;
        axisEulerAngles[3 * featureId + 1] = 0.0f;
        axisEulerAngles[3 * featureId + 2] = 0.0f;
        continue;
      }
      if(Iyy >= Ixx)
      {
        axisEulerAngles[3 * featureId] = 0.0f;
        axisEulerAngles[3 * featureId + 1] = 0.0f;
        axisEulerAngles[3 * featureId + 2] = 0.0f;
        continue;
      }
    }
    double I1 = (Ixx + Iyy) / 2.0 + std::sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 + (Ixy * Ixy - Ixx * Iyy));
    double I2 = (Ixx + Iyy) / 2.0 - std::sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 + (Ixy * Ixy - Ixx * Iyy));
    double n1x = (Ixx - I1) / Ixy;
    double n1y = 1.0;
    double n2x = (Ixx - I2) / Ixy;
    double n2y = 1.0;
    double norm1 = sqrt((n1x * n1x + n1y * n1y));
    double norm2 = sqrt((n2x * n2x + n2y * n2y));
    n1x = n1x / norm1;
    n1y = n1y / norm1;
    n2x = n2x / norm2;
    n2y = n2y / norm2;
    double cosine1 = n1x;
    double ea1 = acos(cosine1);
    if(ea1 > M_PI)
    {
      ea1 = ea1 - M_PI;
    }
    axisEulerAngles[3 * featureId] = static_cast<float>(ea1);
    axisEulerAngles[3 * featureId + 1] = 0.0f;
    axisEulerAngles[3 * featureId + 2] = 0.0f;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::preflight()
{
  setInPreflight(true);
  Q_EMIT preflightAboutToExecute();
  Q_EMIT updateFilterParameters(this);
  dataCheck();
  Q_EMIT preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::execute()
{
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();

  FloatVec3Type spacing = imageGeom->getSpacing();

  m_ScaleFactor = static_cast<double>(1.0f / spacing[0]);
  if(spacing[1] > spacing[0] && spacing[1] > spacing[2])
  {
    m_ScaleFactor = static_cast<double>(1.0f / spacing[1]);
  }
  if(spacing[2] > spacing[0] && spacing[2] > spacing[1])
  {
    m_ScaleFactor = static_cast<double>(1.0f / spacing[2]);
  }

  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();
  m_FeatureMomentsPtr->resize(numfeatures * 6);

  m_FeatureEigenValsPtr->resize(numfeatures * 3);

  // this is a temp array that is used during the calculations
  m_EFVec = FloatArrayType::CreateArray(numfeatures, std::vector<size_t>{9}, QString("Eigen Vectors"), true);

  if(imageGeom->getXPoints() > 1 && imageGeom->getYPoints() > 1 && imageGeom->getZPoints() > 1)
  {
    find_moments();
    find_axes();
    find_axiseulers();
  }
  if(imageGeom->getXPoints() == 1 || imageGeom->getYPoints() == 1 || imageGeom->getZPoints() == 1)
  {
    find_moments2D();
    find_axes2D();
    find_axiseulers2D();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindShapes::newFilterInstance(bool copyFilterParameters) const
{
  FindShapes::Pointer filter = FindShapes::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindShapes::getCompiledLibraryName() const
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindShapes::getBrandingString() const
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindShapes::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindShapes::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid FindShapes::getUuid()
{
  return QUuid("{3b0ababf-9c8d-538d-96af-e40775c4f0ab}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindShapes::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindShapes::getHumanLabel() const
{
  return "Find Feature Shapes";
}

// -----------------------------------------------------------------------------
FindShapes::Pointer FindShapes::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindShapes> FindShapes::New()
{
  struct make_shared_enabler : public FindShapes
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
const QString FindShapes::getNameOfClass() const
{
  return QString("FindShapes");
}

// -----------------------------------------------------------------------------
QString FindShapes::ClassName()
{
  return QString("FindShapes");
}

// -----------------------------------------------------------------------------
void FindShapes::setCellFeatureAttributeMatrixName(const DataArrayPath& value)
{
  m_CellFeatureAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindShapes::getCellFeatureAttributeMatrixName() const
{
  return m_CellFeatureAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void FindShapes::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindShapes::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void FindShapes::setCentroidsArrayPath(const DataArrayPath& value)
{
  m_CentroidsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindShapes::getCentroidsArrayPath() const
{
  return m_CentroidsArrayPath;
}

// -----------------------------------------------------------------------------
void FindShapes::setOmega3sArrayName(const QString& value)
{
  m_Omega3sArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindShapes::getOmega3sArrayName() const
{
  return m_Omega3sArrayName;
}

// -----------------------------------------------------------------------------
void FindShapes::setVolumesArrayName(const QString& value)
{
  m_VolumesArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindShapes::getVolumesArrayName() const
{
  return m_VolumesArrayName;
}

// -----------------------------------------------------------------------------
void FindShapes::setAxisLengthsArrayName(const QString& value)
{
  m_AxisLengthsArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindShapes::getAxisLengthsArrayName() const
{
  return m_AxisLengthsArrayName;
}

// -----------------------------------------------------------------------------
void FindShapes::setAxisEulerAnglesArrayName(const QString& value)
{
  m_AxisEulerAnglesArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindShapes::getAxisEulerAnglesArrayName() const
{
  return m_AxisEulerAnglesArrayName;
}

// -----------------------------------------------------------------------------
void FindShapes::setAspectRatiosArrayName(const QString& value)
{
  m_AspectRatiosArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindShapes::getAspectRatiosArrayName() const
{
  return m_AspectRatiosArrayName;
}
