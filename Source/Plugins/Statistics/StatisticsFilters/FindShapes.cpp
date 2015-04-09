/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FindShapes.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

FindShapes::FindShapes()  :
  AbstractFilter(),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_CentroidsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Centroids),
  m_Omega3sArrayName(DREAM3D::FeatureData::Omega3s),
  m_VolumesArrayName(DREAM3D::FeatureData::Volumes),
  m_AxisLengthsArrayName(DREAM3D::FeatureData::AxisLengths),
  m_AxisEulerAnglesArrayName(DREAM3D::FeatureData::AxisEulerAngles),
  m_AspectRatiosArrayName(DREAM3D::FeatureData::AspectRatios),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_AxisEulerAngles(NULL),
  m_CentroidsArrayName(DREAM3D::FeatureData::Centroids),
  m_Centroids(NULL),
  m_AxisLengths(NULL),
  m_Omega3s(NULL),
  m_Volumes(NULL),
  m_AspectRatios(NULL)
{
  featuremoments = NULL;
  featureeigenvals = NULL;

  scaleFactor = 1.0;

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
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Centroids", "CentroidsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCentroidsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix Name", "CellFeatureAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Omega3s", "Omega3sArrayName", FilterParameterWidgetType::StringWidget, getOmega3sArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("Volumes", "VolumesArrayName", FilterParameterWidgetType::StringWidget, getVolumesArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("AxisLengths", "AxisLengthsArrayName", FilterParameterWidgetType::StringWidget, getAxisLengthsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("AxisEulerAngles", "AxisEulerAnglesArrayName", FilterParameterWidgetType::StringWidget, getAxisEulerAnglesArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("AspectRatios", "AspectRatiosArrayName", FilterParameterWidgetType::StringWidget, getAspectRatiosArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setAspectRatiosArrayName(reader->readString("AspectRatiosArrayName", getAspectRatiosArrayName() ) );
  setAxisEulerAnglesArrayName(reader->readString("AxisEulerAnglesArrayName", getAxisEulerAnglesArrayName() ) );
  setAxisLengthsArrayName(reader->readString("AxisLengthsArrayName", getAxisLengthsArrayName() ) );
  setVolumesArrayName(reader->readString("VolumesArrayName", getVolumesArrayName() ) );
  setOmega3sArrayName(reader->readString("Omega3sArrayName", getOmega3sArrayName() ) );
  setCentroidsArrayPath(reader->readDataArrayPath("CentroidsArrayPath", getCentroidsArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindShapes::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(AspectRatiosArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(AxisEulerAnglesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(AxisLengthsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(VolumesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(Omega3sArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CentroidsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  INIT_DataArray(m_FeatureMoments, double);
  INIT_DataArray(m_FeatureEigenVals, double);

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName())->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0 || NULL == image.get()) { return; }

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getOmega3sArrayName() );
  m_Omega3sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_Omega3sPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Omega3s = m_Omega3sPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getVolumesArrayName() );
  m_VolumesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_VolumesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Volumes = m_VolumesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getAxisLengthsArrayName() );
  m_AxisLengthsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AxisLengthsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AxisLengths = m_AxisLengthsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCentroidsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getAxisEulerAnglesArrayName() );
  m_AxisEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AxisEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AxisEulerAngles = m_AxisEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 2;
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getAspectRatiosArrayName() );
  m_AspectRatiosPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AspectRatiosPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AspectRatios = m_AspectRatiosPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
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
void FindShapes::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  float xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  float yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  float zRes = m->getGeometryAs<ImageGeom>()->getZRes();

  scaleFactor = 1.0 / xRes;
  if(yRes > xRes && yRes > zRes) { scaleFactor = 1.0 / yRes; }
  if(zRes > xRes && zRes > yRes) { scaleFactor = 1.0 / zRes; }

  if(m->getGeometryAs<ImageGeom>()->getXPoints() > 1 && m->getGeometryAs<ImageGeom>()->getYPoints() > 1 && m->getGeometryAs<ImageGeom>()->getZPoints() > 1) { find_moments(); }
  if(m->getGeometryAs<ImageGeom>()->getXPoints() == 1 || m->getGeometryAs<ImageGeom>()->getYPoints() == 1 || m->getGeometryAs<ImageGeom>()->getZPoints() == 1) { find_moments2D(); }

  if(m->getGeometryAs<ImageGeom>()->getXPoints() > 1 && m->getGeometryAs<ImageGeom>()->getYPoints() > 1 && m->getGeometryAs<ImageGeom>()->getZPoints() > 1) { find_axes(); }
  if(m->getGeometryAs<ImageGeom>()->getXPoints() == 1 || m->getGeometryAs<ImageGeom>()->getYPoints() == 1 || m->getGeometryAs<ImageGeom>()->getZPoints() == 1) { find_axes2D(); }

  if(m->getGeometryAs<ImageGeom>()->getXPoints() > 1 && m->getGeometryAs<ImageGeom>()->getYPoints() > 1 && m->getGeometryAs<ImageGeom>()->getZPoints() > 1) { find_axiseulers(); }
  if(m->getGeometryAs<ImageGeom>()->getXPoints() == 1 || m->getGeometryAs<ImageGeom>()->getYPoints() == 1 || m->getGeometryAs<ImageGeom>()->getZPoints() == 1) { find_axiseulers2D(); }

  notifyStatusMessage(getHumanLabel(), "FindShapes Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_moments()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  float u200 = 0;
  float u020 = 0;
  float u002 = 0;
  float u110 = 0;
  float u011 = 0;
  float u101 = 0;
  float xx, yy, zz, xy, xz, yz;
  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();
  m_FeatureMoments->resize(numfeatures * 6);
  featuremoments = m_FeatureMoments->getPointer(0);

  size_t xPoints = m->getGeometryAs<ImageGeom>()->getXPoints();
  size_t yPoints = m->getGeometryAs<ImageGeom>()->getYPoints();
  size_t zPoints = m->getGeometryAs<ImageGeom>()->getZPoints();
  float xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  float yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  float zRes = m->getGeometryAs<ImageGeom>()->getZRes();

  //using a modified resolution to keept he moment calculations "small" and prevent exceeding numerical bounds.
  //scaleFactor is applied later to rescale the calculated axis lengths
  float modXRes = xRes * scaleFactor;
  float modYRes = yRes * scaleFactor;
  float modZRes = zRes * scaleFactor;

  for (size_t i = 0; i < numfeatures; i++)
  {
    featuremoments[6 * i + 0] = 0.0f;
    featuremoments[6 * i + 1] = 0.0f;
    featuremoments[6 * i + 2] = 0.0f;
    featuremoments[6 * i + 3] = 0.0f;
    featuremoments[6 * i + 4] = 0.0f;
    featuremoments[6 * i + 5] = 0.0f;
    m_Volumes[i] = 0.0f;
  }
  float x, y, z, x1, x2, y1, y2, z1, z2;
  float xdist1, xdist2, xdist3, xdist4, xdist5, xdist6, xdist7, xdist8;
  float ydist1, ydist2, ydist3, ydist4, ydist5, ydist6, ydist7, ydist8;
  float zdist1, zdist2, zdist3, zdist4, zdist5, zdist6, zdist7, zdist8;
  size_t zStride, yStride;
  for(size_t i = 0; i < zPoints; i++)
  {
    zStride = i * xPoints * yPoints;
    for (size_t j = 0; j < yPoints; j++)
    {
      yStride = j * xPoints;
      for(size_t k = 0; k < xPoints; k++)
      {

        int gnum = m_FeatureIds[zStride + yStride + k];
        x = float(k * modXRes);
        y = float(j * modYRes);
        z = float(i * modZRes);
        x1 = x + (modXRes / 4);
        x2 = x - (modXRes / 4);
        y1 = y + (modYRes / 4);
        y2 = y - (modYRes / 4);
        z1 = z + (modZRes / 4);
        z2 = z - (modZRes / 4);
        xdist1 = (x1 - (m_Centroids[gnum * 3 + 0] * scaleFactor));
        ydist1 = (y1 - (m_Centroids[gnum * 3 + 1] * scaleFactor));
        zdist1 = (z1 - (m_Centroids[gnum * 3 + 2] * scaleFactor));
        xdist2 = (x1 - (m_Centroids[gnum * 3 + 0] * scaleFactor));
        ydist2 = (y1 - (m_Centroids[gnum * 3 + 1] * scaleFactor));
        zdist2 = (z2 - (m_Centroids[gnum * 3 + 2] * scaleFactor));
        xdist3 = (x1 - (m_Centroids[gnum * 3 + 0] * scaleFactor));
        ydist3 = (y2 - (m_Centroids[gnum * 3 + 1] * scaleFactor));
        zdist3 = (z1 - (m_Centroids[gnum * 3 + 2] * scaleFactor));
        xdist4 = (x1 - (m_Centroids[gnum * 3 + 0] * scaleFactor));
        ydist4 = (y2 - (m_Centroids[gnum * 3 + 1] * scaleFactor));
        zdist4 = (z2 - (m_Centroids[gnum * 3 + 2] * scaleFactor));
        xdist5 = (x2 - (m_Centroids[gnum * 3 + 0] * scaleFactor));
        ydist5 = (y1 - (m_Centroids[gnum * 3 + 1] * scaleFactor));
        zdist5 = (z1 - (m_Centroids[gnum * 3 + 2] * scaleFactor));
        xdist6 = (x2 - (m_Centroids[gnum * 3 + 0] * scaleFactor));
        ydist6 = (y1 - (m_Centroids[gnum * 3 + 1] * scaleFactor));
        zdist6 = (z2 - (m_Centroids[gnum * 3 + 2] * scaleFactor));
        xdist7 = (x2 - (m_Centroids[gnum * 3 + 0] * scaleFactor));
        ydist7 = (y2 - (m_Centroids[gnum * 3 + 1] * scaleFactor));
        zdist7 = (z1 - (m_Centroids[gnum * 3 + 2] * scaleFactor));
        xdist8 = (x2 - (m_Centroids[gnum * 3 + 0] * scaleFactor));
        ydist8 = (y2 - (m_Centroids[gnum * 3 + 1] * scaleFactor));
        zdist8 = (z2 - (m_Centroids[gnum * 3 + 2] * scaleFactor));

        xx = ((ydist1) * (ydist1)) + ((zdist1) * (zdist1)) + ((ydist2) * (ydist2)) + ((zdist2) * (zdist2)) + ((ydist3) * (ydist3)) + ((zdist3) * (zdist3))
             + ((ydist4) * (ydist4)) + ((zdist4) * (zdist4)) + ((ydist5) * (ydist5)) + ((zdist5) * (zdist5)) + ((ydist6) * (ydist6)) + ((zdist6) * (zdist6))
             + ((ydist7) * (ydist7)) + ((zdist7) * (zdist7)) + ((ydist8) * (ydist8)) + ((zdist8) * (zdist8));
        yy = ((xdist1) * (xdist1)) + ((zdist1) * (zdist1)) + ((xdist2) * (xdist2)) + ((zdist2) * (zdist2)) + ((xdist3) * (xdist3)) + ((zdist3) * (zdist3))
             + ((xdist4) * (xdist4)) + ((zdist4) * (zdist4)) + ((xdist5) * (xdist5)) + ((zdist5) * (zdist5)) + ((xdist6) * (xdist6)) + ((zdist6) * (zdist6))
             + ((xdist7) * (xdist7)) + ((zdist7) * (zdist7)) + ((xdist8) * (xdist8)) + ((zdist8) * (zdist8));
        zz = ((xdist1) * (xdist1)) + ((ydist1) * (ydist1)) + ((xdist2) * (xdist2)) + ((ydist2) * (ydist2)) + ((xdist3) * (xdist3)) + ((ydist3) * (ydist3))
             + ((xdist4) * (xdist4)) + ((ydist4) * (ydist4)) + ((xdist5) * (xdist5)) + ((ydist5) * (ydist5)) + ((xdist6) * (xdist6)) + ((ydist6) * (ydist6))
             + ((xdist7) * (xdist7)) + ((ydist7) * (ydist7)) + ((xdist8) * (xdist8)) + ((ydist8) * (ydist8));
        xy = ((xdist1) * (ydist1)) + ((xdist2) * (ydist2)) + ((xdist3) * (ydist3)) + ((xdist4) * (ydist4)) + ((xdist5) * (ydist5)) + ((xdist6) * (ydist6))
             + ((xdist7) * (ydist7)) + ((xdist8) * (ydist8));
        yz = ((ydist1) * (zdist1)) + ((ydist2) * (zdist2)) + ((ydist3) * (zdist3)) + ((ydist4) * (zdist4)) + ((ydist5) * (zdist5)) + ((ydist6) * (zdist6))
             + ((ydist7) * (zdist7)) + ((ydist8) * (zdist8));
        xz = ((xdist1) * (zdist1)) + ((xdist2) * (zdist2)) + ((xdist3) * (zdist3)) + ((xdist4) * (zdist4)) + ((xdist5) * (zdist5)) + ((xdist6) * (zdist6))
             + ((xdist7) * (zdist7)) + ((xdist8) * (zdist8));

        featuremoments[gnum * 6 + 0] = featuremoments[gnum * 6 + 0] + xx;
        featuremoments[gnum * 6 + 1] = featuremoments[gnum * 6 + 1] + yy;
        featuremoments[gnum * 6 + 2] = featuremoments[gnum * 6 + 2] + zz;
        featuremoments[gnum * 6 + 3] = featuremoments[gnum * 6 + 3] + xy;
        featuremoments[gnum * 6 + 4] = featuremoments[gnum * 6 + 4] + yz;
        featuremoments[gnum * 6 + 5] = featuremoments[gnum * 6 + 5] + xz;
        m_Volumes[gnum] = m_Volumes[gnum] + 1.0;
      }
    }
  }
  double sphere = (2000.0f * DREAM3D::Constants::k_Pi * DREAM3D::Constants::k_Pi) / 9.0f;
  //constant for moments because voxels are broken into smaller voxels
  double konst1 =  (modXRes / 2.0f) * (modYRes / 2.0f) * (modZRes / 2.0f);
  //constant for volumes because voxels are counted as one
  double konst2 =  (xRes) * (yRes) * (zRes);
  double konst3 =  (modXRes) * (modYRes) * (modZRes);
  double o3, vol5, omega3;
  for (size_t i = 1; i < numfeatures; i++)
  {
    //calculating the modified volume for the omega3 value
    vol5 = m_Volumes[i] * konst3;
    m_Volumes[i] = m_Volumes[i] * konst2;
    featuremoments[i * 6 + 0] = featuremoments[i * 6 + 0] * konst1;
    featuremoments[i * 6 + 1] = featuremoments[i * 6 + 1] * konst1;
    featuremoments[i * 6 + 2] = featuremoments[i * 6 + 2] * konst1;
    featuremoments[i * 6 + 3] = -featuremoments[i * 6 + 3] * konst1;
    featuremoments[i * 6 + 4] = -featuremoments[i * 6 + 4] * konst1;
    featuremoments[i * 6 + 5] = -featuremoments[i * 6 + 5] * konst1;
    u200 = (featuremoments[i * 6 + 1] + featuremoments[i * 6 + 2] - featuremoments[i * 6 + 0]) / 2.0f;
    u020 = (featuremoments[i * 6 + 0] + featuremoments[i * 6 + 2] - featuremoments[i * 6 + 1]) / 2.0f;
    u002 = (featuremoments[i * 6 + 0] + featuremoments[i * 6 + 1] - featuremoments[i * 6 + 2]) / 2.0f;
    u110 = -featuremoments[i * 6 + 3];
    u011 = -featuremoments[i * 6 + 4];
    u101 = -featuremoments[i * 6 + 5];
    o3 = (u200 * u020 * u002) + (2.0f * u110 * u101 * u011) - (u200 * u011 * u011) - (u020 * u101 * u101) - (u002 * u110 * u110);
    vol5 = powf(vol5, 5);
    omega3 = vol5 / o3;
    omega3 = omega3 / sphere;
    if (omega3 > 1) { omega3 = 1; }
    if(vol5 == 0) { omega3 = 0; }
    m_Omega3s[i] = omega3;
  }
}
void FindShapes::find_moments2D()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  float xx, yy, xy;
  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();
  m_FeatureMoments->resize(numfeatures * 6);
  featuremoments = m_FeatureMoments->getPointer(0);

  int xPoints = 0, yPoints = 0;
  float xRes = 0.0f, yRes = 0.0f;

  if(m->getGeometryAs<ImageGeom>()->getXPoints() == 1)
  {
    xPoints = m->getGeometryAs<ImageGeom>()->getYPoints();
    xRes = m->getGeometryAs<ImageGeom>()->getYRes();
    yPoints = m->getGeometryAs<ImageGeom>()->getZPoints();
    yRes = m->getGeometryAs<ImageGeom>()->getZRes();
  }
  if(m->getGeometryAs<ImageGeom>()->getYPoints() == 1)
  {
    xPoints = m->getGeometryAs<ImageGeom>()->getXPoints();
    xRes = m->getGeometryAs<ImageGeom>()->getXRes();
    yPoints = m->getGeometryAs<ImageGeom>()->getZPoints();
    yRes = m->getGeometryAs<ImageGeom>()->getZRes();
  }
  if(m->getGeometryAs<ImageGeom>()->getZPoints() == 1)
  {
    xPoints = m->getGeometryAs<ImageGeom>()->getXPoints();
    xRes = m->getGeometryAs<ImageGeom>()->getXRes();
    yPoints = m->getGeometryAs<ImageGeom>()->getYPoints();
    yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  }

  double modXRes = xRes * scaleFactor;
  double modYRes = yRes * scaleFactor;

  for (size_t i = 0; i < 6 * numfeatures; i++)
  {
    featuremoments[i] = 0.0;
  }
  float x, y, x1, x2, y1, y2;
  float xdist1, xdist2, xdist3, xdist4;
  float ydist1, ydist2, ydist3, ydist4;
  int yStride;
  for (int j = 0; j < yPoints; j++)
  {
    yStride = j * xPoints;
    for(int k = 0; k < xPoints; k++)
    {
      int gnum = m_FeatureIds[yStride + k];
      x = float(k) * modXRes;
      y = float(j) * modYRes;
      x1 = x + (modXRes / 2);
      x2 = x - (modXRes / 2);
      y1 = y + (modYRes / 2);
      y2 = y - (modYRes / 2);
      xdist1 = (x1 - m_Centroids[gnum * 3 + 0]);
      ydist1 = (y1 - m_Centroids[gnum * 3 + 1]);
      xdist2 = (x1 - m_Centroids[gnum * 3 + 0]);
      ydist2 = (y2 - m_Centroids[gnum * 3 + 1]);
      xdist3 = (x2 - m_Centroids[gnum * 3 + 0]);
      ydist3 = (y1 - m_Centroids[gnum * 3 + 1]);
      xdist4 = (x2 - m_Centroids[gnum * 3 + 0]);
      ydist4 = (y2 - m_Centroids[gnum * 3 + 1]);
      xx = ((ydist1) * (ydist1)) + ((ydist2) * (ydist2)) + ((ydist3) * (ydist3)) + ((ydist4) * (ydist4));
      yy = ((xdist1) * (xdist1)) + ((xdist2) * (xdist2)) + ((xdist3) * (xdist3)) + ((xdist4) * (xdist4));
      xy = ((xdist1) * (ydist1)) + ((xdist2) * (ydist2)) + ((xdist3) * (ydist3)) + ((xdist4) * (ydist4));
      featuremoments[gnum * 6 + 0] = featuremoments[gnum * 6 + 0] + xx;
      featuremoments[gnum * 6 + 1] = featuremoments[gnum * 6 + 1] + yy;
      featuremoments[gnum * 6 + 2] = featuremoments[gnum * 6 + 2] + xy;
    }
  }
  float konst1 = (modXRes / 2.0f) * (modYRes / 2.0f);
  for (size_t i = 1; i < numfeatures; i++)
  {
    featuremoments[i * 6 + 0] = featuremoments[i * 6 + 0] * konst1;
    featuremoments[i * 6 + 1] = featuremoments[i * 6 + 1] * konst1;
    featuremoments[i * 6 + 2] = -featuremoments[i * 6 + 2] * konst1;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_axes()
{
  double I1, I2, I3;
  double Ixx, Iyy, Izz, Ixy, Ixz, Iyz;
  double a, b, c, d, f, g, h;
  double rsquare, r, theta;
  double A, B, C;
  double r1, r2, r3;
  float bovera, covera;
  double value;

  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();

  m_FeatureMoments->resize(numfeatures * 6);
  featuremoments = m_FeatureMoments->getPointer(0);

  m_FeatureEigenVals->resize(numfeatures * 3);
  featureeigenvals = m_FeatureEigenVals->getPointer(0);

  for (size_t i = 1; i < numfeatures; i++)
  {
    Ixx = featuremoments[i * 6 + 0];
    Iyy = featuremoments[i * 6 + 1];
    Izz = featuremoments[i * 6 + 2];

    Ixy = featuremoments[i * 6 + 3];
    Iyz = featuremoments[i * 6 + 4];
    Ixz = featuremoments[i * 6 + 5];

    a = 1;
    b = (-Ixx - Iyy - Izz);
    c = ((Ixx * Izz) + (Ixx * Iyy) + (Iyy * Izz) - (Ixz * Ixz) - (Ixy * Ixy) - (Iyz * Iyz));
    d = 0.0;
    d = ((Ixz * Iyy * Ixz) + (Ixy * Izz * Ixy) + (Iyz * Ixx * Iyz) - (Ixx * Iyy * Izz) - (Ixy * Iyz * Ixz) - (Ixy * Iyz * Ixz));
    // f and g are the p and q values when reducing the cubic equation to t^3 + pt + q = 0
    f = ((3.0f * c / a) - ((b / a) * (b / a))) / 3.0f;
    g = ((2.0f * (b / a) * (b / a) * (b / a)) - (9.0f * b * c / (a * a)) + (27.0f * (d / a))) / 27.0f;
    h = (g * g / 4.0f) + (f * f * f / 27.0f);
    rsquare = (g * g / 4) - h;
    r = sqrt(rsquare);
    if(rsquare < 0) { r = 0; }
    theta = 0;
    if (r == 0)
    {
      theta = 0;
    }
    if (r != 0)
    {
      value = -g / (2.0f * r);
      if(value > 1) { value = 1.0f; }
      if(value < -1) { value = -1.0f; }
      theta = acos(value);
    }
    float const1 = powf(r, 0.33333333333f);
    float const2 = cosf(theta / 3.0f);
    float const3 = b / (3.0f * a);
    float const4 = 1.7320508f * sinf(theta / 3.0f);

    r1 = 2 * const1 * const2 - (const3);
    r2 = -const1 * (const2 - (const4)) - const3;
    r3 = -const1 * (const2 + (const4)) - const3;
    featureeigenvals[3 * i] = r1;
    featureeigenvals[3 * i + 1] = r2;
    featureeigenvals[3 * i + 2] = r3;

    I1 = (15.0f * r1) / (4.0f * DREAM3D::Constants::k_Pi);
    I2 = (15.0f * r2) / (4.0f * DREAM3D::Constants::k_Pi);
    I3 = (15.0f * r3) / (4.0f * DREAM3D::Constants::k_Pi);
    A = (I1 + I2 - I3) / 2.0f;
    B = (I1 + I3 - I2) / 2.0f;
    C = (I2 + I3 - I1) / 2.0f;
    a = (A * A * A * A) / (B * C);
    a = powf(a, 0.1f);
    b = B / A;
    b = sqrt(b) * a;
    c = A / (a * a * a * b);

    m_AxisLengths[3 * i] = a / scaleFactor;
    m_AxisLengths[3 * i + 1] = b / scaleFactor;
    m_AxisLengths[3 * i + 2] = c / scaleFactor;
    bovera = b / a;
    covera = c / a;
    if(A == 0 || B == 0 || C == 0) { bovera = 0, covera = 0; }
    m_AspectRatios[2 * i] = bovera;
    m_AspectRatios[2 * i + 1] = covera;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_axes2D()
{
  float Ixx = 0.0f, Iyy = 0.0f, Ixy= 0.0f;

  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();

  m_FeatureMoments->resize(numfeatures * 6);
  featuremoments = m_FeatureMoments->getPointer(0);

  for (size_t i = 1; i < numfeatures; i++)
  {
    Ixx = featuremoments[i * 6 + 0];
    Iyy = featuremoments[i * 6 + 1];
    Ixy = featuremoments[i * 6 + 2];
    float r1 = (Ixx + Iyy) / 2.0f + sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0f - (Ixx * Iyy - Ixy * Ixy));
    float r2 = (Ixx + Iyy) / 2.0f - sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0f - (Ixx * Iyy - Ixy * Ixy));
    float preterm = 4.0f / DREAM3D::Constants::k_Pi;
    preterm = powf(preterm, 0.25f);
    float postterm1 = r1 * r1 * r1 / r2;
    float postterm2 = r2 * r2 * r2 / r1;
    postterm1 = powf(postterm1, 0.125f);
    postterm2 = powf(postterm2, 0.125f);
    r1 = preterm * postterm1;
    r2 = preterm * postterm2;
    m_AxisLengths[3 * i] = r1 / scaleFactor;
    m_AxisLengths[3 * i + 1] = r2 / scaleFactor;
    m_AspectRatios[2 * i] = r2 / r1;
    m_AspectRatios[2 * i + 1] = 0;
  }
}

void FindShapes::find_axiseulers()
{
  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();
  float ea1 = 0, ea2 = 0, ea3 = 0;
  for (size_t i = 1; i < numfeatures; i++)
  {
    float Ixx = featuremoments[i * 6 + 0];
    float Iyy = featuremoments[i * 6 + 1];
    float Izz = featuremoments[i * 6 + 2];
    float Ixy = -featuremoments[i * 6 + 3];
    float Iyz = -featuremoments[i * 6 + 4];
    float Ixz = -featuremoments[i * 6 + 5];
    float radius1 = featureeigenvals[3 * i];
    float radius2 = featureeigenvals[3 * i + 1];
    float radius3 = featureeigenvals[3 * i + 2];


    float e[3][1];
    float vect[3][3];
    e[0][0] = radius1;
    e[1][0] = radius2;
    e[2][0] = radius3;
    float uber[3][3];
    float bmat[3][1];
    bmat[0][0] = 0.0000001f;
    bmat[1][0] = 0.0000001f;
    bmat[2][0] = 0.0000001f;

    for (int j = 0; j < 3; j++)
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
      float** uberelim;
      float** uberbelim;
      uberelim = new float *[3];
      uberbelim = new float *[3];
      for (int d = 0; d < 3; d++)
      {
        uberelim[d] = new float[3];
        uberbelim[d] = new float[1];
      }
      int elimcount = 0;
      int elimcount1 = 0;
      float q = 0;
      float sum = 0;
      float c = 0;
      for (int a = 0; a < 3; a++)
      {
        elimcount1 = 0;
        for (int b = 0; b < 3; b++)
        {
          uberelim[elimcount][elimcount1] = uber[a][b];
          elimcount1++;
        }
        uberbelim[elimcount][0] = bmat[a][0];
        elimcount++;
      }
      for (int k = 0; k < elimcount - 1; k++)
      {
        for (int l = k + 1; l < elimcount; l++)
        {
          c = uberelim[l][k] / uberelim[k][k];
          for (int r = k + 1; r < elimcount; r++)
          {
            uberelim[l][r] = uberelim[l][r] - c * uberelim[k][r];
          }
          uberbelim[l][0] = uberbelim[l][0] - c * uberbelim[k][0];
        }
      }
      uberbelim[elimcount - 1][0] = uberbelim[elimcount - 1][0] / uberelim[elimcount - 1][elimcount - 1];
      for (int l = 1; l < elimcount; l++)
      {
        int r = (elimcount - 1) - l;
        sum = 0;
        for (int n = r + 1; n < elimcount; n++)
        {
          sum = sum + (uberelim[r][n] * uberbelim[n][0]);
        }
        uberbelim[r][0] = (uberbelim[r][0] - sum) / uberelim[r][r];
      }
      for (int p = 0; p < elimcount; p++)
      {
        q = uberbelim[p][0];
        vect[j][p] = q;
      }
      for (int d = 0; d < 3; d++)
      {
        delete uberelim[d];
        delete uberbelim[d];
      }
      delete uberelim;
      delete uberbelim;
    }

    float n1x = vect[0][0];
    float n1y = vect[0][1];
    float n1z = vect[0][2];
    float n2x = vect[1][0];
    float n2y = vect[1][1];
    float n2z = vect[1][2];
    float n3x = vect[2][0];
    float n3y = vect[2][1];
    float n3z = vect[2][2];
    float norm1 = sqrt(((n1x * n1x) + (n1y * n1y) + (n1z * n1z)));
    float norm2 = sqrt(((n2x * n2x) + (n2y * n2y) + (n2z * n2z)));
    float norm3 = sqrt(((n3x * n3x) + (n3y * n3y) + (n3z * n3z)));
    n1x = n1x / norm1;
    n1y = n1y / norm1;
    n1z = n1z / norm1;
    n2x = n2x / norm2;
    n2y = n2y / norm2;
    n2z = n2z / norm2;
    n3x = n3x / norm3;
    n3y = n3y / norm3;
    n3z = n3z / norm3;

    if(n1z > 1.0) { n1z = 1.0; }
    if(n1z < -1.0) { n1z = -1.0; }
    ea2 = acos(n1z);
    if (ea2 == 0.0)
    {
      ea1 = 0.0;
      ea3 = acos(n2y);
    }
    else
    {
      float cosine3 = (n2z / sinf(ea2));
      float sine3 = (n3z / sinf(ea2));
      float cosine1 = (-n1y / sinf(ea2));
      float sine1 = (n1x / sinf(ea2));
      if (cosine3 > 1.0) { cosine3 = 1.0; }
      if (cosine3 < -1.0) { cosine3 = -1.0; }
      if (cosine1 > 1.0) { cosine1 = 1.0; }
      if (cosine1 < -1.0) { cosine1 = -1.0; }
      ea3 = acos(cosine3);
      ea1 = acos(cosine1);
      if (sine3 < 0) { ea3 = (2 * DREAM3D::Constants::k_Pi) - ea3; }
      if (sine1 < 0) { ea1 = (2 * DREAM3D::Constants::k_Pi) - ea1; }

    }
    m_AxisEulerAngles[3 * i] = ea1;
    m_AxisEulerAngles[3 * i + 1] = ea2;
    m_AxisEulerAngles[3 * i + 2] = ea3;
  }
}

void FindShapes::find_axiseulers2D()
{
  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();

  for (size_t i = 1; i < numfeatures; i++)
  {
    float Ixx = featuremoments[i * 6 + 0];
    float Iyy = featuremoments[i * 6 + 1];
    float Ixy = featuremoments[i * 6 + 2];
    float I1 = (Ixx + Iyy) / 2.0f + sqrtf(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0f + (Ixy * Ixy - Ixx * Iyy));
    float I2 = (Ixx + Iyy) / 2.0f - sqrtf(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0f + (Ixy * Ixy - Ixx * Iyy));
    float n1x = (Ixx - I1) / Ixy;
    float n1y = 1;
    float n2x = (Ixx - I2) / Ixy;
    float n2y = 1;
    float norm1 = sqrtf((n1x * n1x + n1y * n1y));
    float norm2 = sqrtf((n2x * n2x + n2y * n2y));
    n1x = n1x / norm1;
    n1y = n1y / norm1;
    n2x = n2x / norm2;
    n2y = n2y / norm2;
    float cosine1 = n1x;
    float ea1 = acosf(cosine1);
    if (ea1 > DREAM3D::Constants::k_Pi) { ea1 = ea1 - DREAM3D::Constants::k_Pi; }
    m_AxisEulerAngles[3 * i] = ea1;
    m_AxisEulerAngles[3 * i + 1] = 0.0f;
    m_AxisEulerAngles[3 * i + 2] = 0.0f;
  }
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
{ return Statistics::StatisticsBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindShapes::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindShapes::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MorphologicalFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindShapes::getHumanLabel()
{ return "Find Feature Shapes"; }

