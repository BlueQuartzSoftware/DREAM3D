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

#include "FindTriangleGeomShapes.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

// Include the MOC generated file for this class
#include "moc_FindTriangleGeomShapes.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTriangleGeomShapes::FindTriangleGeomShapes()
: AbstractFilter()
, m_FeatureAttributeMatrixName(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceFeatureAttributeMatrixName, "")
, m_FaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels)
, m_CentroidsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceFeatureAttributeMatrixName, SIMPL::FeatureData::Centroids)
, m_VolumesArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceFeatureAttributeMatrixName, SIMPL::FeatureData::Volumes)
, m_Omega3sArrayName(SIMPL::FeatureData::Omega3s)
, m_AxisLengthsArrayName(SIMPL::FeatureData::AxisLengths)
, m_AxisEulerAnglesArrayName(SIMPL::FeatureData::AxisEulerAngles)
, m_AspectRatiosArrayName(SIMPL::FeatureData::AspectRatios)
, m_FaceLabels(nullptr)
, m_Centroids(nullptr)
, m_AxisEulerAngles(nullptr)
, m_AxisLengths(nullptr)
, m_Omega3s(nullptr)
, m_AspectRatios(nullptr)
, m_ScaleFactor(1.0f)
{
  featuremoments = nullptr;
  featureeigenvals = nullptr;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTriangleGeomShapes::~FindTriangleGeomShapes()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomShapes::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
	  DataArraySelectionFilterParameter::RequirementType req =
      DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
	  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", FaceLabelsArrayPath, FilterParameter::RequiredArray, FindTriangleGeomShapes, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Feature Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req =
    AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::FaceFeature, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Face Feature Attribute Matrix", FeatureAttributeMatrixName, FilterParameter::RequiredArray, FindTriangleGeomShapes, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
    DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Type::FaceFeature, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Centroids", CentroidsArrayPath, FilterParameter::RequiredArray, FindTriangleGeomShapes, req));
  }
  {
	  DataArraySelectionFilterParameter::RequirementType req =
      DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 1, AttributeMatrix::Type::FaceFeature, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Volumes", VolumesArrayPath, FilterParameter::RequiredArray, FindTriangleGeomShapes, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Omega3s", Omega3sArrayName, FilterParameter::CreatedArray, FindTriangleGeomShapes));
  parameters.push_back(SIMPL_NEW_STRING_FP("Axis Lengths", AxisLengthsArrayName, FilterParameter::CreatedArray, FindTriangleGeomShapes));
  parameters.push_back(SIMPL_NEW_STRING_FP("Axis Euler Angles", AxisEulerAnglesArrayName, FilterParameter::CreatedArray, FindTriangleGeomShapes));
  parameters.push_back(SIMPL_NEW_STRING_FP("Aspect Ratios", AspectRatiosArrayName, FilterParameter::CreatedArray, FindTriangleGeomShapes));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomShapes::initialize()
{
  m_ScaleFactor = 1.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomShapes::dataCheck()
{
  setErrorCondition(0);
  initialize();
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getFaceLabelsArrayPath().getDataContainerName());

  INIT_DataArray(m_FeatureMoments, double);
  INIT_DataArray(m_FeatureEigenVals, double);

  //REQUIRED FACE DATA
  QVector<size_t> cDims(1, 2);
  m_FaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFaceLabelsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (nullptr != m_FaceLabelsPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
	  m_FaceLabels = m_FaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  //REQUIRED FEATURE DATA
  cDims[0] = 3;
  m_CentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCentroidsArrayPath(),
	  cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (nullptr != m_CentroidsPtr.lock().get())                                                                 /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
	  m_Centroids = m_CentroidsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  m_VolumesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getVolumesArrayPath(),
	  cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (nullptr != m_VolumesPtr.lock().get())                                                                 /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
	  m_Volumes = m_VolumesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */


  //CREATED FEATURE DATA
  tempPath.update(getFeatureAttributeMatrixName().getDataContainerName(), getFeatureAttributeMatrixName().getAttributeMatrixName(), getOmega3sArrayName());
  m_Omega3sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0,
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_Omega3sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Omega3s = m_Omega3sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  tempPath.update(getFeatureAttributeMatrixName().getDataContainerName(), getFeatureAttributeMatrixName().getAttributeMatrixName(), getAxisLengthsArrayName());
  m_AxisLengthsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0,
                                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AxisLengthsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AxisLengths = m_AxisLengthsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getFeatureAttributeMatrixName().getDataContainerName(), getFeatureAttributeMatrixName().getAttributeMatrixName(), getAxisEulerAnglesArrayName());
  m_AxisEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims);                   /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AxisEulerAnglesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AxisEulerAngles = m_AxisEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 2;
  tempPath.update(getFeatureAttributeMatrixName().getDataContainerName(), getFeatureAttributeMatrixName().getAttributeMatrixName(), getAspectRatiosArrayName());
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
void FindTriangleGeomShapes::preflight()
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
void FindTriangleGeomShapes::findTetrahedronInfo(int64_t vertIds[3], float* vertPtr, float centroid[3], float tetInfo[32])
{
	double coords[30];
	coords[0] = vertPtr[3 * vertIds[0] + 0];
	coords[1] = vertPtr[3 * vertIds[0] + 1];
	coords[2] = vertPtr[3 * vertIds[0] + 2];
	coords[3] = vertPtr[3 * vertIds[1] + 0];
	coords[4] = vertPtr[3 * vertIds[1] + 1];
	coords[5] = vertPtr[3 * vertIds[1] + 2];
	coords[6] = vertPtr[3 * vertIds[2] + 0];
	coords[7] = vertPtr[3 * vertIds[2] + 1];
	coords[8] = vertPtr[3 * vertIds[2] + 2];
	coords[9] = centroid[0];
	coords[10] = centroid[1];
	coords[11] = centroid[2];
	coords[12] = 0.5 * (vertPtr[3 * vertIds[0] + 0] + centroid[0]);
	coords[13] = 0.5 * (vertPtr[3 * vertIds[0] + 1] + centroid[1]);
	coords[14] = 0.5 * (vertPtr[3 * vertIds[0] + 2] + centroid[2]);
	coords[15] = 0.5 * (vertPtr[3 * vertIds[1] + 0] + centroid[0]);
	coords[16] = 0.5 * (vertPtr[3 * vertIds[1] + 1] + centroid[1]);
	coords[17] = 0.5 * (vertPtr[3 * vertIds[1] + 2] + centroid[2]);
	coords[18] = 0.5 * (vertPtr[3 * vertIds[2] + 0] + centroid[0]);
	coords[19] = 0.5 * (vertPtr[3 * vertIds[2] + 1] + centroid[1]);
	coords[20] = 0.5 * (vertPtr[3 * vertIds[2] + 2] + centroid[2]);
	coords[21] = 0.5 * (vertPtr[3 * vertIds[0] + 0] + vertPtr[3 * vertIds[1] + 0]);
	coords[22] = 0.5 * (vertPtr[3 * vertIds[0] + 1] + vertPtr[3 * vertIds[1] + 1]);
	coords[23] = 0.5 * (vertPtr[3 * vertIds[0] + 2] + vertPtr[3 * vertIds[1] + 2]);
	coords[24] = 0.5 * (vertPtr[3 * vertIds[1] + 0] + vertPtr[3 * vertIds[2] + 0]);
	coords[25] = 0.5 * (vertPtr[3 * vertIds[1] + 1] + vertPtr[3 * vertIds[2] + 1]);
	coords[26] = 0.5 * (vertPtr[3 * vertIds[1] + 2] + vertPtr[3 * vertIds[2] + 2]);
	coords[27] = 0.5 * (vertPtr[3 * vertIds[2] + 0] + vertPtr[3 * vertIds[0] + 0]);
	coords[28] = 0.5 * (vertPtr[3 * vertIds[2] + 1] + vertPtr[3 * vertIds[0] + 1]);
	coords[29] = 0.5 * (vertPtr[3 * vertIds[2] + 2] + vertPtr[3 * vertIds[0] + 2]);

	int32_t tets[32];
	tets[0] = 4;
	tets[1] = 5;
	tets[2] = 6;
	tets[3] = 3;

	tets[4] = 0;
	tets[5] = 7;
	tets[6] = 9;
	tets[7] = 4;

	tets[8] = 1;
	tets[9] = 8;
	tets[10] = 7;
	tets[11] = 5;

	tets[12] = 2;
	tets[13] = 9;
	tets[14] = 8;
	tets[15] = 6;

	tets[16] = 7;
	tets[17] = 5;
	tets[18] = 6;
	tets[19] = 4;

	tets[20] = 6;
	tets[21] = 9;
	tets[22] = 7;
	tets[23] = 4;

	tets[24] = 6;
	tets[25] = 5;
	tets[26] = 7;
	tets[27] = 8;

	tets[28] = 7;
	tets[29] = 9;
	tets[30] = 6;
	tets[31] = 8;

	for (size_t iter = 0; iter < 8; iter++)
	{
		float ax = coords[3 * tets[4 * iter + 0] + 0];
		float ay = coords[3 * tets[4 * iter + 0] + 1];
		float az = coords[3 * tets[4 * iter + 0] + 2];
		float bx = coords[3 * tets[4 * iter + 1] + 0];
		float by = coords[3 * tets[4 * iter + 1] + 1];
		float bz = coords[3 * tets[4 * iter + 1] + 2];
		float cx = coords[3 * tets[4 * iter + 2] + 0];
		float cy = coords[3 * tets[4 * iter + 2] + 1];
		float cz = coords[3 * tets[4 * iter + 2] + 2];
		float dx = coords[3 * tets[4 * iter + 3] + 0];
		float dy = coords[3 * tets[4 * iter + 3] + 1];
		float dz = coords[3 * tets[4 * iter + 3] + 2];

		float vertMatrix[3][3] = { { bx - ax, cx - ax, dx - ax },
								   { by - ay, cy - ay, dy - ay },
								   { bz - az, cz - az, dz - az } };

		tetInfo[4 * iter + 0] = (MatrixMath::Determinant3x3(vertMatrix) / 6.0f);
		tetInfo[4 * iter + 1] = 0.25 * (ax + bx + cx + dx);
		tetInfo[4 * iter + 2] = 0.25 * (ay + by + cy + dy);
		tetInfo[4 * iter + 3] = 0.25 * (az + bz + cz + dz);
	}

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomShapes::find_moments()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FaceLabelsArrayPath.getDataContainerName());

  TriangleGeom::Pointer triangles = getDataContainerArray()->getDataContainer(m_FaceLabelsArrayPath.getDataContainerName())->getGeometryAs<TriangleGeom>();
  float* vertPtr = triangles->getVertexPointer(0);

  size_t numFaces = m_FaceLabelsPtr.lock()->getNumberOfTuples();

  float u200 = 0.0f;
  float u020 = 0.0f;
  float u002 = 0.0f;
  float u110 = 0.0f;
  float u011 = 0.0f;
  float u101 = 0.0f;
  float xx = 0.0f, yy = 0.0f, zz = 0.0f, xy = 0.0f, xz = 0.0f, yz = 0.0f;
  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();
  m_FeatureMoments->resize(numfeatures * 6);
  featuremoments = m_FeatureMoments->getPointer(0);

  for(size_t i = 0; i < numfeatures; i++)
  {
    featuremoments[6 * i + 0] = 0.0f;
    featuremoments[6 * i + 1] = 0.0f;
    featuremoments[6 * i + 2] = 0.0f;
    featuremoments[6 * i + 3] = 0.0f;
    featuremoments[6 * i + 4] = 0.0f;
    featuremoments[6 * i + 5] = 0.0f;
  }

  float centroid[3];
  float tetInfo[32];
  int64_t vertIds[3];
  double xdist = 0.0f;
  double ydist = 0.0f;
  double zdist = 0.0f;

  for (size_t i = 0; i < numFaces; i++)
  {
	  triangles->getVertsAtTri(i, vertIds);
	  for (size_t j = 0; j < 2; j++)
	  {
		  if (j == 1) { std::swap(vertIds[2], vertIds[1]); }
		  int32_t gnum = m_FaceLabels[2 * i + j];
		  if (gnum > 0)
		  {
			  centroid[0] = m_Centroids[3 * gnum + 0];
			  centroid[1] = m_Centroids[3 * gnum + 1];
			  centroid[2] = m_Centroids[3 * gnum + 2];
			  findTetrahedronInfo(vertIds, vertPtr, centroid, tetInfo);
			  for (size_t iter = 0; iter < 8; iter++)
			  {
				  xdist = (tetInfo[4 * iter + 1] - m_Centroids[gnum * 3 + 0]);
				  ydist = (tetInfo[4 * iter + 2] - m_Centroids[gnum * 3 + 1]);
				  zdist = (tetInfo[4 * iter + 3] - m_Centroids[gnum * 3 + 2]);

				  xx = ((ydist)* (ydist)) + ((zdist)* (zdist));
				  yy = ((xdist)* (xdist)) + ((zdist)* (zdist));
				  zz = ((xdist)* (xdist)) + ((ydist)* (ydist));
				  xy = ((xdist)* (ydist));
				  yz = ((ydist)* (zdist));
				  xz = ((xdist)* (zdist));

				  featuremoments[gnum * 6 + 0] = featuremoments[gnum * 6 + 0] + (xx * tetInfo[4 * iter + 0]);
				  featuremoments[gnum * 6 + 1] = featuremoments[gnum * 6 + 1] + (yy * tetInfo[4 * iter + 0]);
				  featuremoments[gnum * 6 + 2] = featuremoments[gnum * 6 + 2] + (zz * tetInfo[4 * iter + 0]);
				  featuremoments[gnum * 6 + 3] = featuremoments[gnum * 6 + 3] + (xy * tetInfo[4 * iter + 0]);
				  featuremoments[gnum * 6 + 4] = featuremoments[gnum * 6 + 4] + (yz * tetInfo[4 * iter + 0]);
				  featuremoments[gnum * 6 + 5] = featuremoments[gnum * 6 + 5] + (xz * tetInfo[4 * iter + 0]);
			  }
		  }
	  }
  }
  double sphere = (2000.0 * M_PI * M_PI) / 9.0;
  double o3 = 0.0, vol5 = 0.0, omega3 = 0.0;
  for(size_t i = 1; i < numfeatures; i++)
  {
	vol5 = pow(m_Volumes[i], 5.0);
	featuremoments[i * 6 + 3] = -featuremoments[i * 6 + 3];
	featuremoments[i * 6 + 4] = -featuremoments[i * 6 + 4];
	featuremoments[i * 6 + 5] = -featuremoments[i * 6 + 5];
	u200 = static_cast<float>((featuremoments[i * 6 + 1] + featuremoments[i * 6 + 2] - featuremoments[i * 6 + 0]) / 2.0f);
    u020 = static_cast<float>((featuremoments[i * 6 + 0] + featuremoments[i * 6 + 2] - featuremoments[i * 6 + 1]) / 2.0f);
    u002 = static_cast<float>((featuremoments[i * 6 + 0] + featuremoments[i * 6 + 1] - featuremoments[i * 6 + 2]) / 2.0f);
    u110 = static_cast<float>(-featuremoments[i * 6 + 3]);
    u011 = static_cast<float>(-featuremoments[i * 6 + 4]);
    u101 = static_cast<float>(-featuremoments[i * 6 + 5]);
    o3 = static_cast<double>((u200 * u020 * u002) + (2.0f * u110 * u101 * u011) - (u200 * u011 * u011) - (u020 * u101 * u101) - (u002 * u110 * u110));
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
void FindTriangleGeomShapes::find_axes()
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

  m_FeatureMoments->resize(numfeatures * 6);
  featuremoments = m_FeatureMoments->getPointer(0);

  m_FeatureEigenVals->resize(numfeatures * 3);
  featureeigenvals = m_FeatureEigenVals->getPointer(0);

  for(size_t i = 1; i < numfeatures; i++)
  {
    Ixx = featuremoments[i * 6 + 0];
    Iyy = featuremoments[i * 6 + 1];
    Izz = featuremoments[i * 6 + 2];

    Ixy = featuremoments[i * 6 + 3];
    Iyz = featuremoments[i * 6 + 4];
    Ixz = featuremoments[i * 6 + 5];

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
    featureeigenvals[3 * i] = r1;
    featureeigenvals[3 * i + 1] = r2;
    featureeigenvals[3 * i + 2] = r3;

    I1 = (15.0 * r1) / (4.0 * M_PI);
    I2 = (15.0 * r2) / (4.0 * M_PI);
    I3 = (15.0 * r3) / (4.0 * M_PI);
    A = (I1 + I2 - I3) / 2.0f;
    B = (I1 + I3 - I2) / 2.0f;
    C = (I2 + I3 - I1) / 2.0f;
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
    if(A == 0 || B == 0 || C == 0)
    {
      bovera = 0.0f, covera = 0.0f;
    }
    m_AspectRatios[2 * i] = bovera;
    m_AspectRatios[2 * i + 1] = covera;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomShapes::find_axiseulers()
{
  size_t numfeatures = m_CentroidsPtr.lock()->getNumberOfTuples();
  for(size_t i = 1; i < numfeatures; i++)
  {
    double Ixx = featuremoments[i * 6 + 0];
    double Iyy = featuremoments[i * 6 + 1];
    double Izz = featuremoments[i * 6 + 2];
    double Ixy = featuremoments[i * 6 + 3];
    double Iyz = featuremoments[i * 6 + 4];
    double Ixz = featuremoments[i * 6 + 5];
    double radius1 = featureeigenvals[3 * i];
    double radius2 = featureeigenvals[3 * i + 1];
    double radius3 = featureeigenvals[3 * i + 2];

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
void FindTriangleGeomShapes::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  find_moments();
  find_axes();
  find_axiseulers();

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindTriangleGeomShapes::newFilterInstance(bool copyFilterParameters)
{
  FindTriangleGeomShapes::Pointer filter = FindTriangleGeomShapes::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomShapes::getCompiledLibraryName()
{
	return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomShapes::getBrandingString()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomShapes::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomShapes::getGroupName()
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomShapes::getSubGroupName()
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomShapes::getHumanLabel()
{
  return "Find Feature Shapes from Triangle Geometry";
}
