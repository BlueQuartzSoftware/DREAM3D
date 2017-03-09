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

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"

// Include the MOC generated file for this class
#include "moc_FindShapes.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindShapes::FindShapes()
: AbstractFilter()
, m_CellFeatureAttributeMatrixName(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, "")
, m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_CentroidsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Centroids)
, m_Omega3sArrayName(SIMPL::FeatureData::Omega3s)
, m_VolumesArrayName(SIMPL::FeatureData::Volumes)
, m_AxisLengthsArrayName(SIMPL::FeatureData::AxisLengths)
, m_AxisEulerAnglesArrayName(SIMPL::FeatureData::AxisEulerAngles)
, m_AspectRatiosArrayName(SIMPL::FeatureData::AspectRatios)
, m_FeatureIds(nullptr)
, m_Centroids(nullptr)
, m_AxisEulerAngles(nullptr)
, m_AxisLengths(nullptr)
, m_Omega3s(nullptr)
, m_Volumes(nullptr)
, m_AspectRatios(nullptr)
, m_ScaleFactor(1.0f)
{
  m_FeatureMoments = nullptr;
  m_FeatureEigenVals = nullptr;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindShapes::~FindShapes()
{
}

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
  m_ScaleFactor = 1.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::dataCheck()
{
  setErrorCondition(0);
  initialize();
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  INIT_DataArray(m_FeatureMomentsPtr, double);
  INIT_DataArray(m_FeatureEigenValsPtr, double);

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getOmega3sArrayName());
  m_Omega3sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0,
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_Omega3sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Omega3s = m_Omega3sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getVolumesArrayName());
  m_VolumesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0,
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_VolumesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Volumes = m_VolumesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getAxisLengthsArrayName());
  m_AxisLengthsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0,
                                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AxisLengthsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AxisLengths = m_AxisLengthsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_CentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCentroidsArrayPath(),
                                                                                                     cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CentroidsPtr.lock().get())                                                                 /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Centroids = m_CentroidsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getAxisEulerAnglesArrayName());
  m_AxisEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims);                   /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AxisEulerAnglesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AxisEulerAngles = m_AxisEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 2;
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getAspectRatiosArrayName());
  m_AspectRatiosPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0,
                                                                                                                     cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AspectRatiosPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AspectRatios = m_AspectRatiosPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_moments()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();

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
  float xRes = imageGeom->getXRes();
  float yRes = imageGeom->getYRes();
  float zRes = imageGeom->getZRes();

  float xOrigin = 0.0f;
  float yOrigin = 0.0f;
  float zOrigin = 0.0f;
  imageGeom->getOrigin(xOrigin, yOrigin, zOrigin);

  // using a modified resolution to keep the moment calculations "small" and prevent exceeding numerical bounds.
  // scaleFactor is applied later to rescale the calculated axis lengths
  float modXRes = xRes * static_cast<float>(m_ScaleFactor);
  float modYRes = yRes * static_cast<float>(m_ScaleFactor);
  float modZRes = zRes * static_cast<float>(m_ScaleFactor);

  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();

  for(size_t i = 0; i < numfeatures; i++)
  {
    m_FeatureMoments[6 * i + 0] = 0.0;
    m_FeatureMoments[6 * i + 1] = 0.0;
    m_FeatureMoments[6 * i + 2] = 0.0;
    m_FeatureMoments[6 * i + 3] = 0.0;
    m_FeatureMoments[6 * i + 4] = 0.0;
    m_FeatureMoments[6 * i + 5] = 0.0;
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
        int32_t gnum = m_FeatureIds[zStride + yStride + k];
        x = float(k * modXRes) + xOrigin;
        y = float(j * modYRes) + yOrigin;
        z = float(i * modZRes) + zOrigin;
        x1 = x + (modXRes / 4.0f);
        x2 = x - (modXRes / 4.0f);
        y1 = y + (modYRes / 4.0f);
        y2 = y - (modYRes / 4.0f);
        z1 = z + (modZRes / 4.0f);
        z2 = z - (modZRes / 4.0f);
        xdist1 = (x1 - (m_Centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
        ydist1 = (y1 - (m_Centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
        zdist1 = (z1 - (m_Centroids[gnum * 3 + 2] * static_cast<float>(m_ScaleFactor)));
        xdist2 = (x1 - (m_Centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
        ydist2 = (y1 - (m_Centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
        zdist2 = (z2 - (m_Centroids[gnum * 3 + 2] * static_cast<float>(m_ScaleFactor)));
        xdist3 = (x1 - (m_Centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
        ydist3 = (y2 - (m_Centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
        zdist3 = (z1 - (m_Centroids[gnum * 3 + 2] * static_cast<float>(m_ScaleFactor)));
        xdist4 = (x1 - (m_Centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
        ydist4 = (y2 - (m_Centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
        zdist4 = (z2 - (m_Centroids[gnum * 3 + 2] * static_cast<float>(m_ScaleFactor)));
        xdist5 = (x2 - (m_Centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
        ydist5 = (y1 - (m_Centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
        zdist5 = (z1 - (m_Centroids[gnum * 3 + 2] * static_cast<float>(m_ScaleFactor)));
        xdist6 = (x2 - (m_Centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
        ydist6 = (y1 - (m_Centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
        zdist6 = (z2 - (m_Centroids[gnum * 3 + 2] * static_cast<float>(m_ScaleFactor)));
        xdist7 = (x2 - (m_Centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
        ydist7 = (y2 - (m_Centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
        zdist7 = (z1 - (m_Centroids[gnum * 3 + 2] * static_cast<float>(m_ScaleFactor)));
        xdist8 = (x2 - (m_Centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
        ydist8 = (y2 - (m_Centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
        zdist8 = (z2 - (m_Centroids[gnum * 3 + 2] * static_cast<float>(m_ScaleFactor)));

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

        m_FeatureMoments[gnum * 6 + 0] = m_FeatureMoments[gnum * 6 + 0] + static_cast<double>(xx);
        m_FeatureMoments[gnum * 6 + 1] = m_FeatureMoments[gnum * 6 + 1] + static_cast<double>(yy);
        m_FeatureMoments[gnum * 6 + 2] = m_FeatureMoments[gnum * 6 + 2] + static_cast<double>(zz);
        m_FeatureMoments[gnum * 6 + 3] = m_FeatureMoments[gnum * 6 + 3] + static_cast<double>(xy);
        m_FeatureMoments[gnum * 6 + 4] = m_FeatureMoments[gnum * 6 + 4] + static_cast<double>(yz);
        m_FeatureMoments[gnum * 6 + 5] = m_FeatureMoments[gnum * 6 + 5] + static_cast<double>(xz);
        m_Volumes[gnum] = m_Volumes[gnum] + 1.0;
      }
    }
  }
  double sphere = (2000.0 * M_PI * M_PI) / 9.0;
  // constant for moments because voxels are broken into smaller voxels
  double konst1 = static_cast<double>((modXRes / 2.0) * (modYRes / 2.0) * (modZRes / 2.0));
  // constant for volumes because voxels are counted as one
  double konst2 = static_cast<double>((xRes) * (yRes) * (zRes));
  double konst3 = static_cast<double>((modXRes) * (modYRes) * (modZRes));
  double o3 = 0.0, vol5 = 0.0, omega3 = 0.0;
  for(size_t i = 1; i < numfeatures; i++)
  {
    // calculating the modified volume for the omega3 value
    vol5 = m_Volumes[i] * konst3;
    m_Volumes[i] = m_Volumes[i] * konst2;
    m_FeatureMoments[i * 6 + 0] = m_FeatureMoments[i * 6 + 0] * konst1;
    m_FeatureMoments[i * 6 + 1] = m_FeatureMoments[i * 6 + 1] * konst1;
    m_FeatureMoments[i * 6 + 2] = m_FeatureMoments[i * 6 + 2] * konst1;
    m_FeatureMoments[i * 6 + 3] = -m_FeatureMoments[i * 6 + 3] * konst1;
    m_FeatureMoments[i * 6 + 4] = -m_FeatureMoments[i * 6 + 4] * konst1;
    m_FeatureMoments[i * 6 + 5] = -m_FeatureMoments[i * 6 + 5] * konst1;
    u200 = static_cast<float>((m_FeatureMoments[i * 6 + 1] + m_FeatureMoments[i * 6 + 2] - m_FeatureMoments[i * 6 + 0]) / 2.0f);
    u020 = static_cast<float>((m_FeatureMoments[i * 6 + 0] + m_FeatureMoments[i * 6 + 2] - m_FeatureMoments[i * 6 + 1]) / 2.0f);
    u002 = static_cast<float>((m_FeatureMoments[i * 6 + 0] + m_FeatureMoments[i * 6 + 1] - m_FeatureMoments[i * 6 + 2]) / 2.0f);
    u110 = static_cast<float>(-m_FeatureMoments[i * 6 + 3]);
    u011 = static_cast<float>(-m_FeatureMoments[i * 6 + 4]);
    u101 = static_cast<float>(-m_FeatureMoments[i * 6 + 5]);
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
    m_Omega3s[i] = static_cast<float>(omega3);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_moments2D()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();

  float xx = 0.0f, yy = 0.0f, xy = 0.0f;
  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();

  size_t xPoints = 0, yPoints = 0;
  float xRes = 0.0f, yRes = 0.0f;

  if(imageGeom->getXPoints() == 1)
  {
    xPoints = imageGeom->getYPoints();
    xRes = imageGeom->getYRes();
    yPoints = imageGeom->getZPoints();
    yRes = imageGeom->getZRes();
  }
  if(imageGeom->getYPoints() == 1)
  {
    xPoints = imageGeom->getXPoints();
    xRes = imageGeom->getXRes();
    yPoints = imageGeom->getZPoints();
    yRes = imageGeom->getZRes();
  }
  if(imageGeom->getZPoints() == 1)
  {
    xPoints = imageGeom->getXPoints();
    xRes = imageGeom->getXRes();
    yPoints = imageGeom->getYPoints();
    yRes = imageGeom->getYRes();
  }

  float modXRes = xRes * m_ScaleFactor;
  float modYRes = yRes * m_ScaleFactor;

  float xOrigin = 0.0f;
  float yOrigin = 0.0f;
  float zOrigin = 0.0f;
  imageGeom->getOrigin(xOrigin, yOrigin, zOrigin);

  for(size_t i = 0; i < 6 * numfeatures; i++)
  {
    m_FeatureMoments[i] = 0.0;
  }

  float x = 0.0f, y = 0.0f, x1 = 0.0f, x2 = 0.0f, y1 = 0.0f, y2 = 0.0f;
  float xdist1 = 0.0f, xdist2 = 0.0f, xdist3 = 0.0f, xdist4 = 0.0f;
  float ydist1 = 0.0f, ydist2 = 0.0f, ydist3 = 0.0f, ydist4 = 0.0f;
  size_t yStride = 0;
  for(size_t j = 0; j < yPoints; j++)
  {
    yStride = j * xPoints;
    for(size_t k = 0; k < xPoints; k++)
    {
      int32_t gnum = m_FeatureIds[yStride + k];
      x = float(k) * modXRes;
      y = float(j) * modYRes;
      x1 = x + (modXRes / 4.0f);
      x2 = x - (modXRes / 4.0f);
      y1 = y + (modYRes / 4.0f);
      y2 = y - (modYRes / 4.0f);
      xdist1 = (x1 - (m_Centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
      ydist1 = (y1 - (m_Centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
      xdist2 = (x1 - (m_Centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
      ydist2 = (y2 - (m_Centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
      xdist3 = (x2 - (m_Centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
      ydist3 = (y1 - (m_Centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
      xdist4 = (x2 - (m_Centroids[gnum * 3 + 0] * static_cast<float>(m_ScaleFactor)));
      ydist4 = (y2 - (m_Centroids[gnum * 3 + 1] * static_cast<float>(m_ScaleFactor)));
      xx = ((ydist1) * (ydist1)) + ((ydist2) * (ydist2)) + ((ydist3) * (ydist3)) + ((ydist4) * (ydist4));
      yy = ((xdist1) * (xdist1)) + ((xdist2) * (xdist2)) + ((xdist3) * (xdist3)) + ((xdist4) * (xdist4));
      xy = ((xdist1) * (ydist1)) + ((xdist2) * (ydist2)) + ((xdist3) * (ydist3)) + ((xdist4) * (ydist4));
      m_FeatureMoments[gnum * 6 + 0] = m_FeatureMoments[gnum * 6 + 0] + xx;
      m_FeatureMoments[gnum * 6 + 1] = m_FeatureMoments[gnum * 6 + 1] + yy;
      m_FeatureMoments[gnum * 6 + 2] = m_FeatureMoments[gnum * 6 + 2] + xy;
      m_Volumes[gnum] = m_Volumes[gnum] + 1.0;
    }
  }
  double konst1 = static_cast<double>( (modXRes / 2.0f) * (modYRes / 2.0f));
  double konst2 = static_cast<double>(xRes * yRes);
  for(size_t i = 1; i < numfeatures; i++)
  {
   // Eq. 12 Moment matrix. Omega 2
   // Eq. 11 Omega 2
   // E1. 13 Omega 1
   // xx = u20 =
    m_Volumes[i] = m_Volumes[i] * konst2; // Area
    m_FeatureMoments[i * 6 + 0] = m_FeatureMoments[i * 6 + 0] * konst1; // u20
    m_FeatureMoments[i * 6 + 1] = m_FeatureMoments[i * 6 + 1] * konst1; // u02
    m_FeatureMoments[i * 6 + 2] = -m_FeatureMoments[i * 6 + 2] * konst1; // u11
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_axes()
{
  double I1 = 0.0, I2 = 0.0, I3 = 0.0;
  double Ixx = 0.0, Iyy = 0.0, Izz = 0.0, Ixy = 0.0, Ixz = 0.0, Iyz = 0.0;
  double a = 0.0, b = 0.0, c = 0.0, d = 0.0, f = 0.0, g = 0.0, h = 0.0;
  double rsquare = 0.0, r = 0.0, theta = 0.0;
  double A = 0.0, B = 0.0, C = 0.0;
  double r1 = 0.0, r2 = 0.0, r3 = 0.0;
  float bovera = 0.0f, covera = 0.0f;
  double value = 0.0;

  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();


  for(size_t i = 1; i < numfeatures; i++)
  {
    Ixx = m_FeatureMoments[i * 6 + 0];
    Iyy = m_FeatureMoments[i * 6 + 1];
    Izz = m_FeatureMoments[i * 6 + 2];

    Ixy = m_FeatureMoments[i * 6 + 3];
    Iyz = m_FeatureMoments[i * 6 + 4];
    Ixz = m_FeatureMoments[i * 6 + 5];

    a = 1.0;
    b = (-Ixx - Iyy - Izz);
    c = ((Ixx * Izz) + (Ixx * Iyy) + (Iyy * Izz) - (Ixz * Ixz) - (Ixy * Ixy) - (Iyz * Iyz));
    d = 0.0;
    d = ((Ixz * Iyy * Ixz) + (Ixy * Izz * Ixy) + (Iyz * Ixx * Iyz) - (Ixx * Iyy * Izz) - (Ixy * Iyz * Ixz) - (Ixy * Iyz * Ixz));
    // f and g are the p and q values when reducing the cubic equation to t^3 + pt + q = 0
    f = ((3.0 * c / a) - ((b / a) * (b / a))) / 3.0;
    g = ((2.0 * (b / a) * (b / a) * (b / a)) - (9.0 * b * c / (a * a)) + (27.0 * (d / a))) / 27.0;
    h = (g * g / 4.0) + (f * f * f / 27.0);
    rsquare = (g * g / 4.0) - h;
    r = sqrt(rsquare);
    if(rsquare < 0.0)
    {
      r = 0.0;
    }
    theta = 0;
    if(r == 0)
    {
      theta = 0;
    }
    if(r != 0)
    {
      value = -g / (2.0 * r);
      if(value > 1)
      {
        value = 1.0;
      }
      if(value < -1)
      {
        value = -1.0;
      }
      theta = acos(value);
    }
    double const1 = pow(r, 0.33333333333);
    double const2 = cos(theta / 3.0);
    double const3 = b / (3.0 * a);
    double const4 = 1.7320508 * sin(theta / 3.0);

    r1 = 2 * const1 * const2 - (const3);
    r2 = -const1 * (const2 - (const4)) - const3;
    r3 = -const1 * (const2 + (const4)) - const3;
    m_FeatureEigenVals[3 * i] = r1;
    m_FeatureEigenVals[3 * i + 1] = r2;
    m_FeatureEigenVals[3 * i + 2] = r3;

    I1 = (15.0 * r1) / (4.0 * M_PI);
    I2 = (15.0 * r2) / (4.0 * M_PI);
    I3 = (15.0 * r3) / (4.0 * M_PI);
    A = (I1 + I2 - I3) / 2.0;
    B = (I1 + I3 - I2) / 2.0;
    C = (I2 + I3 - I1) / 2.0;
    a = (A * A * A * A) / (B * C);
    a = pow(a, 0.1);
    b = B / A;
    b = sqrt(b) * a;
    c = A / (a * a * a * b);

    m_AxisLengths[3 * i] = static_cast<float>(a / m_ScaleFactor);
    m_AxisLengths[3 * i + 1] = static_cast<float>(b / m_ScaleFactor);
    m_AxisLengths[3 * i + 2] = static_cast<float>(c / m_ScaleFactor);
    bovera = static_cast<float>(b / a);
    covera = static_cast<float>(c / a);
    if(A == 0.0 || B == 0.0 || C == 0.0)
    {
      bovera = 0.0f;
      covera = 0.0f;
    }
    m_AspectRatios[2 * i] = bovera;
    m_AspectRatios[2 * i + 1] = covera;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_axes2D()
{
  double Ixx = 0.0, Iyy = 0.0, Ixy = 0.0;

  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();


  size_t xPoints = 0;
  size_t yPoints = 0;
  float xRes = 0.0f;
  float yRes = 0.0f;

  if(imageGeom->getXPoints() == 1)
  {
    xPoints = imageGeom->getYPoints();
    xRes = imageGeom->getYRes();
    yPoints = imageGeom->getZPoints();
    yRes = imageGeom->getZRes();
  }
  if(imageGeom->getYPoints() == 1)
  {
    xPoints = imageGeom->getXPoints();
    xRes = imageGeom->getXRes();
    yPoints = imageGeom->getZPoints();
    yRes = imageGeom->getZRes();
  }
  if(imageGeom->getZPoints() == 1)
  {
    xPoints = imageGeom->getXPoints();
    xRes = imageGeom->getXRes();
    yPoints = imageGeom->getYPoints();
    yRes = imageGeom->getYRes();
  }


  for(size_t i = 1; i < numfeatures; i++)
  {
    Ixx = m_FeatureMoments[i * 6 + 0];
    Iyy = m_FeatureMoments[i * 6 + 1];
    Ixy = m_FeatureMoments[i * 6 + 2];
    double r1 = (Ixx + Iyy) / 2.0 + sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 - (Ixx * Iyy - Ixy * Ixy));
    double r2 = (Ixx + Iyy) / 2.0 - sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 - (Ixx * Iyy - Ixy * Ixy));
    if(r2 <= 0)
    {
      float tempScale1 = 1.0f;
      float tempScale2 = 1.0f;
      if(Ixx >= Iyy)
      {
        tempScale1 = xRes;
        tempScale2 = yRes;
      }
      if(Ixx < Iyy)
      {
        tempScale1 = yRes;
        tempScale2 = xRes;
      }
      m_AxisLengths[3 * i] = m_Volumes[i] / tempScale1;
      m_AxisLengths[3 * i + 1] = m_Volumes[i] / tempScale2;
      m_AspectRatios[2 * i] = tempScale2 / tempScale1;
      m_AspectRatios[2 * i + 1] = 0.0f;
      continue;
    }
    double preterm = 4.0 / M_PI;
    preterm = pow(preterm, 0.25);
    double postterm1 = r1 * r1 * r1 / r2;
    double postterm2 = r2 * r2 * r2 / r1;
    postterm1 = pow(postterm1, 0.125f);
    postterm2 = pow(postterm2, 0.125f);
    r1 = preterm * postterm1;
    r2 = preterm * postterm2;
    m_AxisLengths[3 * i] = static_cast<float>(r1 / m_ScaleFactor);
    m_AxisLengths[3 * i + 1] = static_cast<float>(r2 / m_ScaleFactor);
    m_AspectRatios[2 * i] = static_cast<float>(r2 / r1);
    m_AspectRatios[2 * i + 1] = 0.0f;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_axiseulers()
{
  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();
  for(size_t i = 1; i < numfeatures; i++)
  {
    double Ixx = m_FeatureMoments[i * 6 + 0];
    double Iyy = m_FeatureMoments[i * 6 + 1];
    double Izz = m_FeatureMoments[i * 6 + 2];
    double Ixy = m_FeatureMoments[i * 6 + 3];
    double Iyz = m_FeatureMoments[i * 6 + 4];
    double Ixz = m_FeatureMoments[i * 6 + 5];
    double radius1 = m_FeatureEigenVals[3 * i];
    double radius2 = m_FeatureEigenVals[3 * i + 1];
    double radius3 = m_FeatureEigenVals[3 * i + 2];

    double e[3][1];
    double vect[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    e[0][0] = radius1;
    e[1][0] = radius2;
    e[2][0] = radius3;
    double uber[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    double bmat[3][1];
    bmat[0][0] = 0.0000001;
    bmat[1][0] = 0.0000001;
    bmat[2][0] = 0.0000001;

    for(int32_t j = 0; j < 3; j++)
    {
      uber[0][0] = Ixx - e[j][0];
      uber[0][1] = Ixy;
      uber[0][2] = Ixz;
      uber[1][0] = Ixy;
      uber[1][1] = Iyy - e[j][0];
      uber[1][2] = Iyz;
      uber[2][0] = Ixz;
      uber[2][1] = Iyz;
      uber[2][2] = Izz - e[j][0];
      double** uberelim;
      double** uberbelim;
      uberelim = new double*[3];
      uberbelim = new double*[3];
      for(int32_t d = 0; d < 3; d++)
      {
        uberelim[d] = new double[3];
        uberbelim[d] = new double[1];
      }
      int32_t elimcount = 0;
      int32_t elimcount1 = 0;
      double q = 0.0;
      double sum = 0.0;
      double c = 0.0;
      for(int32_t a = 0; a < 3; a++)
      {
        elimcount1 = 0;
        for(int32_t b = 0; b < 3; b++)
        {
          uberelim[elimcount][elimcount1] = uber[a][b];
          elimcount1++;
        }
        uberbelim[elimcount][0] = bmat[a][0];
        elimcount++;
      }
      for(int32_t k = 0; k < elimcount - 1; k++)
      {
        for(int32_t l = k + 1; l < elimcount; l++)
        {
          c = uberelim[l][k] / uberelim[k][k];
          for(int32_t r = k + 1; r < elimcount; r++)
          {
            uberelim[l][r] = uberelim[l][r] - c * uberelim[k][r];
          }
          uberbelim[l][0] = uberbelim[l][0] - c * uberbelim[k][0];
        }
      }
      uberbelim[elimcount - 1][0] = uberbelim[elimcount - 1][0] / uberelim[elimcount - 1][elimcount - 1];
      for(int32_t l = 1; l < elimcount; l++)
      {
        int32_t r = (elimcount - 1) - l;
        sum = 0.0;
        for(int32_t n = r + 1; n < elimcount; n++)
        {
          sum = sum + (uberelim[r][n] * uberbelim[n][0]);
        }
        uberbelim[r][0] = (uberbelim[r][0] - sum) / uberelim[r][r];
      }
      for(int32_t p = 0; p < elimcount; p++)
      {
        q = uberbelim[p][0];
        vect[j][p] = q;
      }
      for(int32_t d = 0; d < 3; d++)
      {
        delete uberelim[d];
        delete uberbelim[d];
      }
      delete uberelim;
      delete uberbelim;
    }

    double n1x = vect[0][0];
    double n1y = vect[0][1];
    double n1z = vect[0][2];
    double n2x = vect[1][0];
    double n2y = vect[1][1];
    double n2z = vect[1][2];
    double n3x = vect[2][0];
    double n3y = vect[2][1];
    double n3z = vect[2][2];
    double norm1 = sqrt(((n1x * n1x) + (n1y * n1y) + (n1z * n1z)));
    double norm2 = sqrt(((n2x * n2x) + (n2y * n2y) + (n2z * n2z)));
    double norm3 = sqrt(((n3x * n3x) + (n3y * n3y) + (n3z * n3z)));
    n1x = n1x / norm1;
    n1y = n1y / norm1;
    n1z = n1z / norm1;
    n2x = n2x / norm2;
    n2y = n2y / norm2;
    n2z = n2z / norm2;
    n3x = n3x / norm3;
    n3y = n3y / norm3;
    n3z = n3z / norm3;

    // insert principal unit vectors into rotation matrix representing Feature reference frame within the sample reference frame
    //(Note that the 3 direction is actually the long axis and the 1 direction is actually the short axis)
    float g[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    g[0][0] = n3x;
    g[0][1] = n3y;
    g[0][2] = n3z;
    g[1][0] = n2x;
    g[1][1] = n2y;
    g[1][2] = n2z;
    g[2][0] = n1x;
    g[2][1] = n1y;
    g[2][2] = n1z;

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

    m_AxisEulerAngles[3 * i] = eu[0];
    m_AxisEulerAngles[3 * i + 1] = eu[1];
    m_AxisEulerAngles[3 * i + 2] = eu[2];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_axiseulers2D()
{
  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();

  for(size_t i = 1; i < numfeatures; i++)
  {
    double Ixx = m_FeatureMoments[i * 6 + 0];
    double Iyy = m_FeatureMoments[i * 6 + 1];
    double Ixy = m_FeatureMoments[i * 6 + 2];
    if(Ixy == 0)
    {
      if(Ixx > Iyy)
      {
        m_AxisEulerAngles[3 * i] = static_cast<float>(SIMPLib::Constants::k_PiOver2);
        m_AxisEulerAngles[3 * i + 1] = 0.0f;
        m_AxisEulerAngles[3 * i + 2] = 0.0f;
        continue;
      }
      if(Iyy >= Ixx)
      {
        m_AxisEulerAngles[3 * i] = 0.0f;
        m_AxisEulerAngles[3 * i + 1] = 0.0f;
        m_AxisEulerAngles[3 * i + 2] = 0.0f;
        continue;
      }
    }
    double I1 = (Ixx + Iyy) / 2.0 + sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 + (Ixy * Ixy - Ixx * Iyy));
    double I2 = (Ixx + Iyy) / 2.0 - sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 + (Ixy * Ixy - Ixx * Iyy));
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
    m_AxisEulerAngles[3 * i] = static_cast<float>(ea1);
    m_AxisEulerAngles[3 * i + 1] = 0.0f;
    m_AxisEulerAngles[3 * i + 2] = 0.0f;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();


  float xRes = imageGeom->getXRes();
  float yRes = imageGeom->getYRes();
  float zRes = imageGeom->getZRes();

  m_ScaleFactor = static_cast<double>(1.0f / xRes);
  if(yRes > xRes && yRes > zRes)
  {
    m_ScaleFactor = static_cast<double>(1.0f / yRes);
  }
  if(zRes > xRes && zRes > yRes)
  {
    m_ScaleFactor = static_cast<double>(1.0f / zRes);
  }

  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();
  m_FeatureMomentsPtr->resize(numfeatures * 6);
  m_FeatureMoments = m_FeatureMomentsPtr->getPointer(0);

  m_FeatureEigenValsPtr->resize(numfeatures * 3);
  m_FeatureEigenVals = m_FeatureEigenValsPtr->getPointer(0);


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


  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindShapes::newFilterInstance(bool copyFilterParameters)
{
  FindShapes::Pointer filter = FindShapes::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindShapes::getCompiledLibraryName()
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindShapes::getBrandingString()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindShapes::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindShapes::getGroupName()
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindShapes::getSubGroupName()
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindShapes::getHumanLabel()
{
  return "Find Feature Shapes";
}
