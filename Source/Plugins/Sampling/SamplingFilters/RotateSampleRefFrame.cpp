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

#include <memory>

#include "RotateSampleRefFrame.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range3d.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "OrientationLib/Core/Orientation.hpp"
#include "OrientationLib/Core/OrientationTransformation.hpp"
#include "OrientationLib/Core/Quaternion.hpp"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

typedef struct
{
  int64_t xp;
  int64_t yp;
  int64_t zp;
  float xRes;
  float yRes;
  float zRes;
  int64_t xpNew;
  int64_t ypNew;
  int64_t zpNew;
  float xResNew;
  float yResNew;
  float zResNew;
  float xMinNew;
  float yMinNew;
  float zMinNew;

} RotateSampleRefFrameImplArg_t;

/**
 * @brief The RotateSampleRefFrameImpl class implements a threaded algorithm to do the
 * actual computation of the rotation by applying the rotation to each Euler angle
 */
class RotateSampleRefFrameImpl
{

  DataArray<int64_t>::Pointer newIndicesPtr;
  float rotMatrixInv[3][3];
  bool m_SliceBySlice;
  RotateSampleRefFrameImplArg_t* m_params;

public:
  RotateSampleRefFrameImpl(DataArray<int64_t>::Pointer newindices, RotateSampleRefFrameImplArg_t* args, float rotMat[3][3], bool sliceBySlice)
  : newIndicesPtr(newindices)
  , m_SliceBySlice(sliceBySlice)
  , m_params(args)
  {
    // We have to inline the 3x3 Maxtrix transpose here because of the "const" nature of the 'convert' function
    rotMatrixInv[0][0] = rotMat[0][0];
    rotMatrixInv[0][1] = rotMat[1][0];
    rotMatrixInv[0][2] = rotMat[2][0];
    rotMatrixInv[1][0] = rotMat[0][1];
    rotMatrixInv[1][1] = rotMat[1][1];
    rotMatrixInv[1][2] = rotMat[2][1];
    rotMatrixInv[2][0] = rotMat[0][2];
    rotMatrixInv[2][1] = rotMat[1][2];
    rotMatrixInv[2][2] = rotMat[2][2];
  }
  virtual ~RotateSampleRefFrameImpl() = default;

  void convert(int64_t zStart, int64_t zEnd, int64_t yStart, int64_t yEnd, int64_t xStart, int64_t xEnd) const
  {

    int64_t* newindicies = newIndicesPtr->getPointer(0);
    int64_t index = 0;
    int64_t ktot = 0, jtot = 0;
    //      float rotMatrixInv[3][3];
    float coords[3] = {0.0f, 0.0f, 0.0f};
    float coordsNew[3] = {0.0f, 0.0f, 0.0f};
    int64_t colOld = 0, rowOld = 0, planeOld = 0;

    for(int64_t k = zStart; k < zEnd; k++)
    {
      ktot = (m_params->xpNew * m_params->ypNew) * k;
      for(int64_t j = yStart; j < yEnd; j++)
      {
        jtot = (m_params->xpNew) * j;
        for(int64_t i = xStart; i < xEnd; i++)
        {
          index = ktot + jtot + i;
          newindicies[index] = -1;
          coords[2] = (float(k) * m_params->zResNew) + m_params->zMinNew;
          coords[1] = (float(j) * m_params->yResNew) + m_params->yMinNew;
          coords[0] = (float(i) * m_params->xResNew) + m_params->xMinNew;
          coordsNew[0] = rotMatrixInv[0][0] * coords[0] + rotMatrixInv[0][1] * coords[1] + rotMatrixInv[0][2] * coords[2];
          coordsNew[1] = rotMatrixInv[1][0] * coords[0] + rotMatrixInv[1][1] * coords[1] + rotMatrixInv[1][2] * coords[2];
          coordsNew[2] = rotMatrixInv[2][0] * coords[0] + rotMatrixInv[2][1] * coords[1] + rotMatrixInv[2][2] * coords[2];
          colOld = static_cast<int64_t>(nearbyint(coordsNew[0] / m_params->xRes));
          rowOld = static_cast<int64_t>(nearbyint(coordsNew[1] / m_params->yRes));
          planeOld = static_cast<int64_t>(nearbyint(coordsNew[2] / m_params->zRes));
          if(m_SliceBySlice)
          {
            planeOld = k;
          }
          if(colOld >= 0 && colOld < m_params->xp && rowOld >= 0 && rowOld < m_params->yp && planeOld >= 0 && planeOld < m_params->zp)
          {
            newindicies[index] = (m_params->xp * m_params->yp * planeOld) + (m_params->xp * rowOld) + colOld;
          }
        }
      }
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range3d<int64_t, int64_t, int64_t>& r) const
  {
    convert(r.pages().begin(), r.pages().end(), r.rows().begin(), r.rows().end(), r.cols().begin(), r.cols().end());
  }
#endif

private:
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateSampleRefFrame::RotateSampleRefFrame()
: m_CellAttributeMatrixPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, "")
, m_RotationAngle(0.0)
, m_SliceBySlice(false)
{
  m_RotationAxis[0] = 0.0;
  m_RotationAxis[1] = 0.0;
  m_RotationAxis[2] = 1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateSampleRefFrame::~RotateSampleRefFrame() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_FLOAT_FP("Rotation Angle (Degrees)", RotationAngle, FilterParameter::Parameter, RotateSampleRefFrame));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Rotation Axis (ijk)", RotationAxis, FilterParameter::Parameter, RotateSampleRefFrame));

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Attribute Matrix", CellAttributeMatrixPath, FilterParameter::RequiredArray, RotateSampleRefFrame, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellAttributeMatrixPath(reader->readDataArrayPath("CellAttributeMatrixPath", getCellAttributeMatrixPath()));
  setRotationAxis(reader->readFloatVec3("RotationAxis", getRotationAxis()));
  setRotationAngle(reader->readValue("RotationAngle", getRotationAngle()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::preflight()
{
  setInPreflight(true);
  clearErrorCode();
  clearWarningCode();
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();

  if(getErrorCode() < 0)
  {
    setInPreflight(false);
    return;
  }

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());
  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getCellAttributeMatrixPath(), -301);
  if(getErrorCode() < 0)
  {
    setInPreflight(false);
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());

  float rotAngle = m_RotationAngle * SIMPLib::Constants::k_Pi / 180.0;

  MeshIndexType xpNew = 0, ypNew = 0, zpNew = 0;
  float xResNew = 0.0f, yResNew = 0.0f, zResNew = 0.0f;
  RotateSampleRefFrameImplArg_t params;
  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();
  SizeVec3Type origDims = imageGeom->getDimensions();
  FloatVec3Type spacing = imageGeom->getSpacing();
  FloatVec3Type origin = imageGeom->getOrigin();

  params.xp = origDims[0];
  params.xRes = spacing[0];
  params.yp = origDims[1];
  params.yRes = spacing[1];
  params.zp = origDims[2];
  params.zRes = spacing[2];

  size_t col = 0, row = 0, plane = 0;
  float rotMat[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float coords[3] = {0.0f, 0.0f, 0.0f};
  float newcoords[3] = {0.0f, 0.0f, 0.0f};
  float xMin = std::numeric_limits<float>::max();
  float xMax = std::numeric_limits<float>::min();
  float yMin = std::numeric_limits<float>::max();
  float yMax = std::numeric_limits<float>::min();
  float zMin = std::numeric_limits<float>::max();
  float zMax = std::numeric_limits<float>::min();

  OrientationTransformation::ax2om<OrientationF, OrientationF>(OrientationF(m_RotationAxis[0], m_RotationAxis[1], m_RotationAxis[2], rotAngle)).toGMatrix(rotMat);
  for(int32_t i = 0; i < 8; i++)
  {
    if(i == 0)
    {
      col = 0, row = 0, plane = 0;
    }
    if(i == 1)
    {
      col = origDims[0] - 1, row = 0, plane = 0;
    }
    if(i == 2)
    {
      col = 0, row = origDims[1] - 1, plane = 0;
    }
    if(i == 3)
    {
      col = origDims[0] - 1, row = origDims[1] - 1, plane = 0;
    }
    if(i == 4)
    {
      col = 0, row = 0, plane = origDims[2] - 1;
    }
    if(i == 5)
    {
      col = origDims[0] - 1, row = 0, plane = origDims[2] - 1;
    }
    if(i == 6)
    {
      col = 0, row = origDims[1] - 1, plane = origDims[2] - 1;
    }
    if(i == 7)
    {
      col = origDims[0] - 1, row = origDims[1] - 1, plane = origDims[2] - 1;
    }
    coords[0] = static_cast<float>(col * spacing[0]);
    coords[1] = static_cast<float>(row * spacing[1]);
    coords[2] = static_cast<float>(plane * spacing[2]);
    MatrixMath::Multiply3x3with3x1(rotMat, coords, newcoords);
    if(newcoords[0] < xMin)
    {
      xMin = newcoords[0];
    }
    if(newcoords[0] > xMax)
    {
      xMax = newcoords[0];
    }
    if(newcoords[1] < yMin)
    {
      yMin = newcoords[1];
    }
    if(newcoords[1] > yMax)
    {
      yMax = newcoords[1];
    }
    if(newcoords[2] < zMin)
    {
      zMin = newcoords[2];
    }
    if(newcoords[2] > zMax)
    {
      zMax = newcoords[2];
    }
  }
  float xAxis[3] = {1, 0, 0};
  float yAxis[3] = {0, 1, 0};
  float zAxis[3] = {0, 0, 1};
  float xAxisNew[3] = {0.0f, 0.0f, 0.0f};
  float yAxisNew[3] = {0.0f, 0.0f, 0.0f};
  float zAxisNew[3] = {0.0f, 0.0f, 0.0f};
  MatrixMath::Multiply3x3with3x1(rotMat, xAxis, xAxisNew);
  MatrixMath::Multiply3x3with3x1(rotMat, yAxis, yAxisNew);
  MatrixMath::Multiply3x3with3x1(rotMat, zAxis, zAxisNew);
  float closestAxis = 0.0f;
  xResNew = spacing[0];
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, xAxisNew));
  if(fabs(GeometryMath::CosThetaBetweenVectors(yAxis, xAxisNew)) > closestAxis)
  {
    xResNew = spacing[1], closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, xAxisNew));
  }
  if(fabs(GeometryMath::CosThetaBetweenVectors(zAxis, xAxisNew)) > closestAxis)
  {
    xResNew = spacing[2], closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, xAxisNew));
  }
  yResNew = spacing[1];
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, yAxisNew));
  if(fabs(GeometryMath::CosThetaBetweenVectors(xAxis, yAxisNew)) > closestAxis)
  {
    yResNew = spacing[0], closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, yAxisNew));
  }
  if(fabs(GeometryMath::CosThetaBetweenVectors(zAxis, yAxisNew)) > closestAxis)
  {
    yResNew = spacing[2], closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, yAxisNew));
  }
  zResNew = spacing[2];
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, zAxisNew));
  if(fabs(GeometryMath::CosThetaBetweenVectors(xAxis, zAxisNew)) > closestAxis)
  {
    zResNew = spacing[0], closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, zAxisNew));
  }
  if(fabs(GeometryMath::CosThetaBetweenVectors(yAxis, zAxisNew)) > closestAxis)
  {
    zResNew = spacing[1], closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, zAxisNew));
  }

  xpNew = static_cast<int64_t>(nearbyint((xMax - xMin) / xResNew) + 1);
  ypNew = static_cast<int64_t>(nearbyint((yMax - yMin) / yResNew) + 1);
  zpNew = static_cast<int64_t>(nearbyint((zMax - zMin) / zResNew) + 1);

  params.xpNew = xpNew;
  params.xResNew = xResNew;
  params.xMinNew = xMin;
  params.ypNew = ypNew;
  params.yResNew = yResNew;
  params.yMinNew = yMin;
  params.zpNew = zpNew;
  params.zResNew = zResNew;
  params.zMinNew = zMin;

  // int64_t newNumCellTuples = params.xpNew * params.ypNew * params.zpNew;

  m->getGeometryAs<ImageGeom>()->setSpacing(FloatVec3Type(params.xResNew, params.yResNew, params.zResNew));
  m->getGeometryAs<ImageGeom>()->setDimensions(params.xpNew, params.ypNew, params.zpNew);
  origin[0] += xMin;
  origin[1] += yMin;
  origin[2] += zMin;
  m->getGeometryAs<ImageGeom>()->setOrigin(origin);

  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());

  float rotAngle = m_RotationAngle * SIMPLib::Constants::k_Pi / 180.0;

  MeshIndexType xpNew = 0, ypNew = 0, zpNew = 0;
  float xResNew = 0.0f, yResNew = 0.0f, zResNew = 0.0f;
  RotateSampleRefFrameImplArg_t params;
  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();
  SizeVec3Type origDims = imageGeom->getDimensions();
  FloatVec3Type spacing = imageGeom->getSpacing();
  FloatVec3Type origin = imageGeom->getOrigin();

  params.xp = origDims[0];
  params.xRes = spacing[0];
  params.yp = origDims[1];
  params.yRes = spacing[1];
  params.zp = origDims[2];
  params.zRes = spacing[2];

  size_t col = 0, row = 0, plane = 0;
  float rotMat[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float coords[3] = {0.0f, 0.0f, 0.0f};
  float newcoords[3] = {0.0f, 0.0f, 0.0f};
  float xMin = std::numeric_limits<float>::max();
  float xMax = std::numeric_limits<float>::min();
  float yMin = std::numeric_limits<float>::max();
  float yMax = std::numeric_limits<float>::min();
  float zMin = std::numeric_limits<float>::max();
  float zMax = std::numeric_limits<float>::min();

  OrientationTransformation::ax2om<OrientationF, OrientationF>(OrientationF(m_RotationAxis[0], m_RotationAxis[1], m_RotationAxis[2], rotAngle)).toGMatrix(rotMat);
  for(int32_t i = 0; i < 8; i++)
  {
    if(i == 0)
    {
      col = 0, row = 0, plane = 0;
    }
    if(i == 1)
    {
      col = origDims[0] - 1, row = 0, plane = 0;
    }
    if(i == 2)
    {
      col = 0, row = origDims[1] - 1, plane = 0;
    }
    if(i == 3)
    {
      col = origDims[0] - 1, row = origDims[1] - 1, plane = 0;
    }
    if(i == 4)
    {
      col = 0, row = 0, plane = origDims[2] - 1;
    }
    if(i == 5)
    {
      col = origDims[0] - 1, row = 0, plane = origDims[2] - 1;
    }
    if(i == 6)
    {
      col = 0, row = origDims[1] - 1, plane = origDims[2] - 1;
    }
    if(i == 7)
    {
      col = origDims[0] - 1, row = origDims[1] - 1, plane = origDims[2] - 1;
    }
    coords[0] = static_cast<float>(col * spacing[0]);
    coords[1] = static_cast<float>(row * spacing[1]);
    coords[2] = static_cast<float>(plane * spacing[2]);
    MatrixMath::Multiply3x3with3x1(rotMat, coords, newcoords);
    if(newcoords[0] < xMin)
    {
      xMin = newcoords[0];
    }
    if(newcoords[0] > xMax)
    {
      xMax = newcoords[0];
    }
    if(newcoords[1] < yMin)
    {
      yMin = newcoords[1];
    }
    if(newcoords[1] > yMax)
    {
      yMax = newcoords[1];
    }
    if(newcoords[2] < zMin)
    {
      zMin = newcoords[2];
    }
    if(newcoords[2] > zMax)
    {
      zMax = newcoords[2];
    }
  }
  float xAxis[3] = {1, 0, 0};
  float yAxis[3] = {0, 1, 0};
  float zAxis[3] = {0, 0, 1};
  float xAxisNew[3] = {0.0f, 0.0f, 0.0f};
  float yAxisNew[3] = {0.0f, 0.0f, 0.0f};
  float zAxisNew[3] = {0.0f, 0.0f, 0.0f};
  MatrixMath::Multiply3x3with3x1(rotMat, xAxis, xAxisNew);
  MatrixMath::Multiply3x3with3x1(rotMat, yAxis, yAxisNew);
  MatrixMath::Multiply3x3with3x1(rotMat, zAxis, zAxisNew);
  float closestAxis = 0.0f;
  xResNew = spacing[0];
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, xAxisNew));
  if(fabs(GeometryMath::CosThetaBetweenVectors(yAxis, xAxisNew)) > closestAxis)
  {
    xResNew = spacing[1], closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, xAxisNew));
  }
  if(fabs(GeometryMath::CosThetaBetweenVectors(zAxis, xAxisNew)) > closestAxis)
  {
    xResNew = spacing[2], closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, xAxisNew));
  }
  yResNew = spacing[1];
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, yAxisNew));
  if(fabs(GeometryMath::CosThetaBetweenVectors(xAxis, yAxisNew)) > closestAxis)
  {
    yResNew = spacing[0], closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, yAxisNew));
  }
  if(fabs(GeometryMath::CosThetaBetweenVectors(zAxis, yAxisNew)) > closestAxis)
  {
    yResNew = spacing[2], closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, yAxisNew));
  }
  zResNew = spacing[2];
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, zAxisNew));
  if(fabs(GeometryMath::CosThetaBetweenVectors(xAxis, zAxisNew)) > closestAxis)
  {
    zResNew = spacing[0], closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, zAxisNew));
  }
  if(fabs(GeometryMath::CosThetaBetweenVectors(yAxis, zAxisNew)) > closestAxis)
  {
    zResNew = spacing[1], closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, zAxisNew));
  }

  xpNew = static_cast<int64_t>(nearbyint((xMax - xMin) / xResNew) + 1);
  ypNew = static_cast<int64_t>(nearbyint((yMax - yMin) / yResNew) + 1);
  zpNew = static_cast<int64_t>(nearbyint((zMax - zMin) / zResNew) + 1);

  params.xpNew = xpNew;
  params.xResNew = xResNew;
  params.xMinNew = xMin;
  params.ypNew = ypNew;
  params.yResNew = yResNew;
  params.yMinNew = yMin;
  params.zpNew = zpNew;
  params.zResNew = zResNew;
  params.zMinNew = zMin;

  int64_t newNumCellTuples = params.xpNew * params.ypNew * params.zpNew;

  DataArray<int64_t>::Pointer newIndiciesPtr = DataArray<int64_t>::CreateArray(newNumCellTuples, "_INTERNAL_USE_ONLY_RotateSampleRef_NewIndicies", true);
  newIndiciesPtr->initializeWithValue(-1);
  int64_t* newindicies = newIndiciesPtr->getPointer(0);

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = false;
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel)
  {
    tbb::parallel_for(tbb::blocked_range3d<int64_t, int64_t, int64_t>(0, params.zpNew, 0, params.ypNew, 0, params.xpNew), RotateSampleRefFrameImpl(newIndiciesPtr, &params, rotMat, m_SliceBySlice),
                      tbb::auto_partitioner());
  }
  else
#endif
  {
    RotateSampleRefFrameImpl serial(newIndiciesPtr, &params, rotMat, m_SliceBySlice);
    serial.convert(0, params.zpNew, 0, params.ypNew, 0, params.xpNew);
  }

  // This could technically be parallelized also where each thread takes an array to adjust. Except
  // that the DataContainer is NOT thread safe or re-entrant so that would actually be a BAD idea.
  QString attrMatName = getCellAttributeMatrixPath().getAttributeMatrixName();
  QList<QString> voxelArrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNames();

  // resize attribute matrix
  std::vector<size_t> tDims(3);
  tDims[0] = params.xpNew;
  tDims[1] = params.ypNew;
  tDims[2] = params.zpNew;
  m->getAttributeMatrix(attrMatName)->resizeAttributeArrays(tDims);

  for(const auto& attrArrayName : voxelArrayNames)
  {
    IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(attrArrayName);
    // Make a copy of the 'p' array that has the same name. When placed into
    // the data container this will over write the current array with
    // the same name.
    IDataArray::Pointer data = p->createNewArray(newNumCellTuples, p->getComponentDimensions(), p->getName());
    //    void* source = nullptr;
    //    void* destination = nullptr;
    int64_t newIndicies_I = 0;
    //   int32_t nComp = data->getNumberOfComponents();
    for(size_t i = 0; i < static_cast<size_t>(newNumCellTuples); i++)
    {
      newIndicies_I = newindicies[i];
      if(newIndicies_I >= 0)
      {
        //        source = p->getVoidPointer((nComp * newIndicies_I));
        //        if(nullptr == source)
        //        {
        //          QString ss = QObject::tr("The index is outside the bounds of the source array");
        //          setErrorCondition(-11004);
        //          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        //          return;
        //        }
        //        destination = data->getVoidPointer((data->getNumberOfComponents() * i));
        //        ::memcpy(destination, source, p->getTypeSize() * data->getNumberOfComponents());

        if(!data->copyFromArray(i, p, newIndicies_I, 1))
        {
          QString ss = QObject::tr("copyFromArray Failed: ");
          QTextStream out(&ss);
          out << "Source Array Name: " << p->getName() << " Source Tuple Index: " << newIndicies_I << "\n";
          out << "Dest Array Name: " << data->getName() << "  Dest. Tuple Index: " << i << "\n";
          setErrorCondition(-11004, ss);
          return;
        }
      }
      else
      {
        int var = 0;
        data->initializeTuple(i, &var);
      }
    }
    m->getAttributeMatrix(attrMatName)->insertOrAssign(data);
  }
  m->getGeometryAs<ImageGeom>()->setSpacing(FloatVec3Type(params.xResNew, params.yResNew, params.zResNew));
  m->getGeometryAs<ImageGeom>()->setDimensions(params.xpNew, params.ypNew, params.zpNew);
  origin[0] += xMin;
  origin[1] += yMin;
  origin[2] += zMin;
  m->getGeometryAs<ImageGeom>()->setOrigin(origin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RotateSampleRefFrame::newFilterInstance(bool copyFilterParameters) const
{
  RotateSampleRefFrame::Pointer filter = RotateSampleRefFrame::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getCompiledLibraryName() const
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getBrandingString() const
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getGroupName() const
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid RotateSampleRefFrame::getUuid() const
{
  return QUuid("{e25d9b4c-2b37-578c-b1de-cf7032b5ef19}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::RotationTransformationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getHumanLabel() const
{
  return "Rotate Sample Reference Frame";
}

// -----------------------------------------------------------------------------
RotateSampleRefFrame::Pointer RotateSampleRefFrame::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<RotateSampleRefFrame> RotateSampleRefFrame::New()
{
  struct make_shared_enabler : public RotateSampleRefFrame
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getNameOfClass() const
{
  return QString("RotateSampleRefFrame");
}

// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::ClassName()
{
  return QString("RotateSampleRefFrame");
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setCellAttributeMatrixPath(const DataArrayPath& value)
{
  m_CellAttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath RotateSampleRefFrame::getCellAttributeMatrixPath() const
{
  return m_CellAttributeMatrixPath;
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setRotationAxis(const FloatVec3Type& value)
{
  m_RotationAxis = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type RotateSampleRefFrame::getRotationAxis() const
{
  return m_RotationAxis;
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setRotationAngle(float value)
{
  m_RotationAngle = value;
}

// -----------------------------------------------------------------------------
float RotateSampleRefFrame::getRotationAngle() const
{
  return m_RotationAngle;
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setSliceBySlice(bool value)
{
  m_SliceBySlice = value;
}

// -----------------------------------------------------------------------------
bool RotateSampleRefFrame::getSliceBySlice() const
{
  return m_SliceBySlice;
}
