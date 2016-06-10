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

#include "RotateSampleRefFrame.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range3d.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "Sampling/SamplingConstants.h"

typedef struct
{
  int64_t   xp;
  int64_t   yp;
  int64_t   zp;
  float   xRes;
  float   yRes;
  float   zRes;
  int64_t   xpNew;
  int64_t   ypNew;
  int64_t   zpNew;
  float   xResNew;
  float   yResNew;
  float   zResNew;
  float   xMinNew;
  float   yMinNew;
  float   zMinNew;

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
    RotateSampleRefFrameImplArg_t*  m_params;

  public:
    RotateSampleRefFrameImpl(DataArray<int64_t>::Pointer newindices, RotateSampleRefFrameImplArg_t*  args, float rotMat[3][3], bool sliceBySlice) :
      newIndicesPtr(newindices),
      m_SliceBySlice(sliceBySlice),
      m_params(args)
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
    virtual ~RotateSampleRefFrameImpl() {}

    void convert(int64_t zStart, int64_t zEnd, int64_t yStart, int64_t yEnd, int64_t xStart, int64_t xEnd) const
    {

      int64_t* newindicies = newIndicesPtr->getPointer(0);
      int64_t index = 0;
      int64_t ktot = 0, jtot = 0;
      //      float rotMatrixInv[3][3];
      float coords[3] = { 0.0f, 0.0f, 0.0f };
      float coordsNew[3] = { 0.0f, 0.0f, 0.0f };
      int64_t colOld = 0, rowOld = 0, planeOld = 0;

      for (int64_t k = zStart; k < zEnd; k++)
      {
        ktot = (m_params->xpNew * m_params->ypNew) * k;
        for (int64_t j = yStart; j < yEnd; j++)
        {
          jtot = (m_params->xpNew) * j;
          for (int64_t i = xStart; i < xEnd; i++)
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
            if(m_SliceBySlice == true) { planeOld = k; }
            if(colOld >= 0 && colOld < m_params->xp && rowOld >= 0 && rowOld < m_params->yp && planeOld >= 0 && planeOld < m_params->zp)
            {
              newindicies[index] = (m_params->xp * m_params->yp * planeOld) + (m_params->xp * rowOld) + colOld;
            }
          }
        }
      }
    }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range3d<int64_t, int64_t, int64_t>& r) const
    {
      convert(r.pages().begin(), r.pages().end(), r.rows().begin(), r.rows().end(), r.cols().begin(), r.cols().end());
    }
#endif

  private:


};

// Include the MOC generated file for this class
#include "moc_RotateSampleRefFrame.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateSampleRefFrame::RotateSampleRefFrame() :
  AbstractFilter(),
  m_CellAttributeMatrixPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, ""),
  m_RotationAngle(0.0),
  m_SliceBySlice(false)
{
  m_RotationAxis.x = 0.0;
  m_RotationAxis.y = 0.0;
  m_RotationAxis.z = 1.0;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateSampleRefFrame::~RotateSampleRefFrame()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(FloatVec3FilterParameter::New("Rotation Axis (ijk)", "RotationAxis", getRotationAxis(), FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Rotation Angle (Degrees)", "RotationAngle", getRotationAngle(), FilterParameter::Parameter));

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixPath", getCellAttributeMatrixPath(), FilterParameter::RequiredArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellAttributeMatrixPath( reader->readDataArrayPath("CellAttributeMatrixPath", getCellAttributeMatrixPath() ) );
  setRotationAxis( reader->readFloatVec3("RotationAxis", getRotationAxis() ) );
  setRotationAngle( reader->readValue("RotationAngle", getRotationAngle()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RotateSampleRefFrame::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(CellAttributeMatrixPath)
  SIMPL_FILTER_WRITE_PARAMETER(RotationAxis)
  SIMPL_FILTER_WRITE_PARAMETER(RotationAngle)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
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
  setErrorCondition(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::preflight()
{
  setInPreflight(true);
  setErrorCondition(0);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();

  if(getErrorCondition() < 0) { setInPreflight(false); return; }

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());
  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getCellAttributeMatrixPath(), -301);
  if(getErrorCondition() < 0) { setInPreflight(false); return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());

  float rotAngle = m_RotationAngle * SIMPLib::Constants::k_Pi / 180.0;

  int64_t xp = 0, yp = 0, zp = 0;
  float xRes = 0.0f, yRes = 0.0f, zRes = 0.0f;
  int64_t xpNew = 0, ypNew = 0, zpNew = 0;
  float xResNew = 0.0f, yResNew = 0.0f, zResNew = 0.0f;
  RotateSampleRefFrameImplArg_t params;

  xp = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints());
  xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  yp = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getYPoints());
  yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  zp = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getZPoints());
  zRes = m->getGeometryAs<ImageGeom>()->getZRes();

  params.xp = xp;
  params.xRes = xRes;
  params.yp = yp;
  params.yRes = yRes;
  params.zp = zp;
  params.zRes = zRes;

  size_t col = 0, row = 0, plane = 0;
  float rotMat[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float coords[3] = { 0.0f, 0.0f, 0.0f };
  float newcoords[3] = { 0.0f, 0.0f, 0.0f };
  float xMin = std::numeric_limits<float>::max();
  float xMax = -xMin;
  float yMin = std::numeric_limits<float>::max();
  float yMax = -yMin;
  float zMin = std::numeric_limits<float>::max();
  float zMax = -zMin;

  FOrientArrayType om(9);
  FOrientTransformsType::ax2om(FOrientArrayType(m_RotationAxis.x, m_RotationAxis.y, m_RotationAxis.z, rotAngle), om);
  om.toGMatrix(rotMat);
  for (int32_t i = 0; i < 8; i++)
  {
    if (i == 0) { col = 0, row = 0, plane = 0; }
    if (i == 1) { col = xp - 1, row = 0, plane = 0; }
    if (i == 2) { col = 0, row = yp - 1, plane = 0; }
    if (i == 3) { col = xp - 1, row = yp - 1, plane = 0; }
    if (i == 4) { col = 0, row = 0, plane = zp - 1; }
    if (i == 5) { col = xp - 1, row = 0, plane = zp - 1; }
    if (i == 6) { col = 0, row = yp - 1, plane = zp - 1; }
    if (i == 7) { col = xp - 1, row = yp - 1, plane = zp - 1; }
    coords[0] = static_cast<float>(col * xRes);
    coords[1] = static_cast<float>(row * yRes);
    coords[2] = static_cast<float>(plane * zRes);
    MatrixMath::Multiply3x3with3x1(rotMat, coords, newcoords);
    if (newcoords[0] < xMin) { xMin = newcoords[0]; }
    if (newcoords[0] > xMax) { xMax = newcoords[0]; }
    if (newcoords[1] < yMin) { yMin = newcoords[1]; }
    if (newcoords[1] > yMax) { yMax = newcoords[1]; }
    if (newcoords[2] < zMin) { zMin = newcoords[2]; }
    if (newcoords[2] > zMax) { zMax = newcoords[2]; }
  }
  float xAxis[3] = {1, 0, 0};
  float yAxis[3] = {0, 1, 0};
  float zAxis[3] = {0, 0, 1};
  float xAxisNew[3] = { 0.0f, 0.0f, 0.0f };
  float yAxisNew[3] = { 0.0f, 0.0f, 0.0f };
  float zAxisNew[3] = { 0.0f, 0.0f, 0.0f };
  MatrixMath::Multiply3x3with3x1(rotMat, xAxis, xAxisNew);
  MatrixMath::Multiply3x3with3x1(rotMat, yAxis, yAxisNew);
  MatrixMath::Multiply3x3with3x1(rotMat, zAxis, zAxisNew);
  float closestAxis = 0.0f;
  xResNew = xRes;
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, xAxisNew));
  if (fabs(GeometryMath::CosThetaBetweenVectors(yAxis, xAxisNew)) > closestAxis) { xResNew = yRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, xAxisNew)); }
  if (fabs(GeometryMath::CosThetaBetweenVectors(zAxis, xAxisNew)) > closestAxis) { xResNew = zRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, xAxisNew)); }
  yResNew = yRes;
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, yAxisNew));
  if (fabs(GeometryMath::CosThetaBetweenVectors(xAxis, yAxisNew)) > closestAxis) { yResNew = xRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, yAxisNew)); }
  if (fabs(GeometryMath::CosThetaBetweenVectors(zAxis, yAxisNew)) > closestAxis) { yResNew = zRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, yAxisNew)); }
  zResNew = zRes;
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, zAxisNew));
  if (fabs(GeometryMath::CosThetaBetweenVectors(xAxis, zAxisNew)) > closestAxis) { zResNew = xRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, zAxisNew)); }
  if (fabs(GeometryMath::CosThetaBetweenVectors(yAxis, zAxisNew)) > closestAxis) { zResNew = yRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, zAxisNew)); }

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

  m->getGeometryAs<ImageGeom>()->setResolution(params.xResNew, params.yResNew, params.zResNew);
  m->getGeometryAs<ImageGeom>()->setDimensions(params.xpNew, params.ypNew, params.zpNew);
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());

  float rotAngle = m_RotationAngle * SIMPLib::Constants::k_Pi / 180.0;

  int64_t xp = 0, yp = 0, zp = 0;
  float xRes = 0.0f, yRes = 0.0f, zRes = 0.0f;
  int64_t xpNew = 0, ypNew = 0, zpNew = 0;
  float xResNew = 0.0f, yResNew = 0.0f, zResNew = 0.0f;
  RotateSampleRefFrameImplArg_t params;

  xp = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints());
  xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  yp = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getYPoints());
  yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  zp = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getZPoints());
  zRes = m->getGeometryAs<ImageGeom>()->getZRes();

  params.xp = xp;
  params.xRes = xRes;
  params.yp = yp;
  params.yRes = yRes;
  params.zp = zp;
  params.zRes = zRes;

  size_t col = 0, row = 0, plane = 0;
  float rotMat[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float coords[3] = { 0.0f, 0.0f, 0.0f };
  float newcoords[3] = { 0.0f, 0.0f, 0.0f };
  float xMin = std::numeric_limits<float>::max();
  float xMax = std::numeric_limits<float>::min();
  float yMin = std::numeric_limits<float>::max();
  float yMax = std::numeric_limits<float>::min();
  float zMin = std::numeric_limits<float>::max();
  float zMax = std::numeric_limits<float>::min();

  FOrientArrayType om(9);
  FOrientTransformsType::ax2om(FOrientArrayType(m_RotationAxis.x, m_RotationAxis.y, m_RotationAxis.z, rotAngle), om);
  om.toGMatrix(rotMat);
  for (int32_t i = 0; i < 8; i++)
  {
    if (i == 0) { col = 0, row = 0, plane = 0; }
    if (i == 1) { col = xp - 1, row = 0, plane = 0; }
    if (i == 2) { col = 0, row = yp - 1, plane = 0; }
    if (i == 3) { col = xp - 1, row = yp - 1, plane = 0; }
    if (i == 4) { col = 0, row = 0, plane = zp - 1; }
    if (i == 5) { col = xp - 1, row = 0, plane = zp - 1; }
    if (i == 6) { col = 0, row = yp - 1, plane = zp - 1; }
    if (i == 7) { col = xp - 1, row = yp - 1, plane = zp - 1; }
    coords[0] = static_cast<float>(col * xRes);
    coords[1] = static_cast<float>(row * yRes);
    coords[2] = static_cast<float>(plane * zRes);
    MatrixMath::Multiply3x3with3x1(rotMat, coords, newcoords);
    if (newcoords[0] < xMin) { xMin = newcoords[0]; }
    if (newcoords[0] > xMax) { xMax = newcoords[0]; }
    if (newcoords[1] < yMin) { yMin = newcoords[1]; }
    if (newcoords[1] > yMax) { yMax = newcoords[1]; }
    if (newcoords[2] < zMin) { zMin = newcoords[2]; }
    if (newcoords[2] > zMax) { zMax = newcoords[2]; }
  }
  float xAxis[3] = {1, 0, 0};
  float yAxis[3] = {0, 1, 0};
  float zAxis[3] = {0, 0, 1};
  float xAxisNew[3] = { 0.0f, 0.0f, 0.0f };
  float yAxisNew[3] = { 0.0f, 0.0f, 0.0f };
  float zAxisNew[3] = { 0.0f, 0.0f, 0.0f };
  MatrixMath::Multiply3x3with3x1(rotMat, xAxis, xAxisNew);
  MatrixMath::Multiply3x3with3x1(rotMat, yAxis, yAxisNew);
  MatrixMath::Multiply3x3with3x1(rotMat, zAxis, zAxisNew);
  float closestAxis = 0.0f;
  xResNew = xRes;
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, xAxisNew));
  if (fabs(GeometryMath::CosThetaBetweenVectors(yAxis, xAxisNew)) > closestAxis) { xResNew = yRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, xAxisNew)); }
  if (fabs(GeometryMath::CosThetaBetweenVectors(zAxis, xAxisNew)) > closestAxis) { xResNew = zRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, xAxisNew)); }
  yResNew = yRes;
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, yAxisNew));
  if (fabs(GeometryMath::CosThetaBetweenVectors(xAxis, yAxisNew)) > closestAxis) { yResNew = xRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, yAxisNew)); }
  if (fabs(GeometryMath::CosThetaBetweenVectors(zAxis, yAxisNew)) > closestAxis) { yResNew = zRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, yAxisNew)); }
  zResNew = zRes;
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, zAxisNew));
  if (fabs(GeometryMath::CosThetaBetweenVectors(xAxis, zAxisNew)) > closestAxis) { zResNew = xRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, zAxisNew)); }
  if (fabs(GeometryMath::CosThetaBetweenVectors(yAxis, zAxisNew)) > closestAxis) { zResNew = yRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, zAxisNew)); }

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

  DataArray<int64_t>::Pointer newIndiciesPtr = DataArray<int64_t>::CreateArray(newNumCellTuples, "_INTERNAL_USE_ONLY_RotateSampleRef_NewIndicies");
  newIndiciesPtr->initializeWithValue(-1);
  int64_t* newindicies = newIndiciesPtr->getPointer(0);

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range3d<int64_t, int64_t, int64_t>(0, params.zpNew, 0, params.ypNew, 0, params.xpNew),
                      RotateSampleRefFrameImpl(newIndiciesPtr, &params, rotMat, m_SliceBySlice), tbb::auto_partitioner());
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
  QVector<size_t> tDims(3);
  tDims[0] = params.xpNew;
  tDims[1] = params.ypNew;
  tDims[2] = params.zpNew;
  m->getAttributeMatrix(attrMatName)->resizeAttributeArrays(tDims);

  for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(*iter);
    // Make a copy of the 'p' array that has the same name. When placed into
    // the data container this will over write the current array with
    // the same name.
    IDataArray::Pointer data = p->createNewArray(newNumCellTuples, p->getComponentDimensions(), p->getName());
    void* source = NULL;
    void* destination = NULL;
    int64_t newIndicies_I = 0;
    int32_t nComp = data->getNumberOfComponents();
    for (size_t i = 0; i < static_cast<size_t>(newNumCellTuples); i++)
    {
      newIndicies_I = newindicies[i];
      if(newIndicies_I >= 0)
      {
        source = p->getVoidPointer((nComp * newIndicies_I));
        if (NULL == source)
        {
          QString ss = QObject::tr("The index is outside the bounds of the source array");
          setErrorCondition(-11004);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
          return;
        }
        destination = data->getVoidPointer((data->getNumberOfComponents() * i));
        ::memcpy(destination, source, p->getTypeSize() * data->getNumberOfComponents());
      }
      else
      {
        data->initializeTuple(i, 0);
      }
    }
    m->getAttributeMatrix(attrMatName)->addAttributeArray(*iter, data);
  }
  m->getGeometryAs<ImageGeom>()->setResolution(params.xResNew, params.yResNew, params.zResNew);
  m->getGeometryAs<ImageGeom>()->setDimensions(params.xpNew, params.ypNew, params.zpNew);
  m->getGeometryAs<ImageGeom>()->setOrigin(xMin, yMin, zMin);

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RotateSampleRefFrame::newFilterInstance(bool copyFilterParameters)
{
  RotateSampleRefFrame::Pointer filter = RotateSampleRefFrame::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RotateSampleRefFrame::getCompiledLibraryName()
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RotateSampleRefFrame::getBrandingString()
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RotateSampleRefFrame::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RotateSampleRefFrame::getGroupName()
{ return SIMPL::FilterGroups::SamplingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RotateSampleRefFrame::getSubGroupName()
{ return SIMPL::FilterSubGroups::RotationTransformationFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RotateSampleRefFrame::getHumanLabel()
{ return "Rotate Sample Reference Frame"; }

