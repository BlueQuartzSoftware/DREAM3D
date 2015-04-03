/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "RotateSampleRefFrame.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range3d.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/Math/GeometryMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "OrientationLib/Math/OrientationMath.h"
#include "OrientationLib/OrientationOps/OrientationOps.h"


typedef struct
{
  size_t   xp;
  size_t   yp;
  size_t   zp;
  float   xRes;
  float   yRes;
  float   zRes;
  size_t   xpNew;
  size_t   ypNew;
  size_t   zpNew;
  float   xResNew;
  float   yResNew;
  float   zResNew;
  float   xMinNew;
  float   yMinNew;
  float   zMinNew;

} RotateSampleRefFrameImplArg_t;

/**
 * @brief The RotateSampleRefFrameImpl class does that actual computation of the rotation
 * applying the rotation to each euler angle
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

    void convert(size_t zStart, size_t zEnd, size_t yStart, size_t yEnd, size_t xStart, size_t xEnd) const
    {

      int64_t* newindicies = newIndicesPtr->getPointer(0);
      int64_t index = 0;
      int64_t ktot, jtot;
      //      float rotMatrixInv[3][3];
      float coords[3];
      float coordsNew[3];
      int32_t colOld, rowOld, planeOld;

      for (size_t k = zStart; k < zEnd; k++)
      {
        ktot = (m_params->xpNew * m_params->ypNew) * k;
        for (size_t j = yStart; j < yEnd; j++)
        {
          jtot = (m_params->xpNew) * j;
          for (size_t i = xStart; i < xEnd; i++)
          {
            index = ktot + jtot + i;
            newindicies[index] = -1;
            coords[2] = (float(k) * m_params->zResNew) + m_params->zMinNew;
            coords[1] = (float(j) * m_params->yResNew) + m_params->yMinNew;
            coords[0] = (float(i) * m_params->xResNew) + m_params->xMinNew;
            coordsNew[0] = rotMatrixInv[0][0] * coords[0] + rotMatrixInv[0][1] * coords[1] + rotMatrixInv[0][2] * coords[2];
            coordsNew[1] = rotMatrixInv[1][0] * coords[0] + rotMatrixInv[1][1] * coords[1] + rotMatrixInv[1][2] * coords[2];
            coordsNew[2] = rotMatrixInv[2][0] * coords[0] + rotMatrixInv[2][1] * coords[1] + rotMatrixInv[2][2] * coords[2];
            colOld = coordsNew[0] / m_params->xRes;
            rowOld = coordsNew[1] / m_params->yRes;
            planeOld = coordsNew[2] / m_params->zRes;
            if(m_SliceBySlice == true) { planeOld = k; }
            if(colOld >= 0 && colOld < m_params->xp && rowOld >= 0 && rowOld < m_params->yp && planeOld >= 0 && planeOld < m_params->zp)
            {
              newindicies[index] = (m_params->xp * m_params->yp * planeOld) + (m_params->xp * rowOld) + colOld;
            }
          }
        }
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range3d<size_t, size_t, size_t>& r) const
    {
      convert(r.pages().begin(), r.pages().end(), r.rows().begin(), r.rows().end(), r.cols().begin(), r.cols().end());
    }
#endif

  private:


};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateSampleRefFrame::RotateSampleRefFrame() :
  AbstractFilter(),
  m_CellAttributeMatrixPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, ""),
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
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellAttributeMatrixPath(), false));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Rotation Axis");
    parameter->setPropertyName("RotationAxis");
    parameter->setWidgetType(FilterParameterWidgetType::FloatVec3Widget);
//   //parameter->setValueType("FloatVec3_t");
    parameter->setUnits("ijk");
    parameters.push_back(parameter);
  }
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Rotation Angle");
    parameter->setPropertyName("RotationAngle");
    parameter->setWidgetType(FilterParameterWidgetType::DoubleWidget);
//   //parameter->setValueType("float");
//   parameter->setCastableValueType("double");
    parameter->setUnits("Degrees");
    parameters.push_back(parameter);
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
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(RotationAxis)
  DREAM3D_FILTER_WRITE_PARAMETER(RotationAngle)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
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

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName(), false);
  if (getErrorCondition() < 0) { setInPreflight(false); return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixPath().getAttributeMatrixName(), -301);
  if (getErrorCondition() < 0 || NULL == cellAttrMat.get()) { setInPreflight(false); return; }

  ImageGeom::Pointer image = m->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if (getErrorCondition() < 0 || NULL == image.get()) { return; }

  float rotAngle = m_RotationAngle * DREAM3D::Constants::k_Pi / 180.0;

  int32_t xp, yp, zp;
  float xRes, yRes, zRes;
  int32_t xpNew, ypNew, zpNew;
  float xResNew, yResNew, zResNew;
  RotateSampleRefFrameImplArg_t params;

  xp = static_cast<int32_t>(m->getGeometryAs<ImageGeom>()->getXPoints());
  xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  yp = static_cast<int32_t>(m->getGeometryAs<ImageGeom>()->getYPoints());
  yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  zp = static_cast<int32_t>(m->getGeometryAs<ImageGeom>()->getZPoints());
  zRes = m->getGeometryAs<ImageGeom>()->getZRes();

  params.xp = xp;
  params.xRes = xRes;
  params.yp = yp;
  params.yRes = yRes;
  params.zp = zp;
  params.zRes = zRes;

  size_t col, row, plane;
  float rotMat[3][3];
  float coords[3];
  float newcoords[3];
  float xMin = 100000000, xMax = 0, yMin = 100000000, yMax = 0, zMin = 100000000, zMax = 0;

  OrientationMath::AxisAngletoMat(rotAngle, m_RotationAxis.x, m_RotationAxis.y, m_RotationAxis.z, rotMat);
  for(int i = 0; i < 8; i++)
  {
    if(i == 0) { col = 0, row = 0, plane = 0; }
    if(i == 1) { col = xp - 1, row = 0, plane = 0; }
    if(i == 2) { col = 0, row = yp - 1, plane = 0; }
    if(i == 3) { col = xp - 1, row = yp - 1, plane = 0; }
    if(i == 4) { col = 0, row = 0, plane = zp - 1; }
    if(i == 5) { col = xp - 1, row = 0, plane = zp - 1; }
    if(i == 6) { col = 0, row = yp - 1, plane = zp - 1; }
    if(i == 7) { col = xp - 1, row = yp - 1, plane = zp - 1; }
    coords[0] = col * xRes;
    coords[1] = row * yRes;
    coords[2] = plane * zRes;
    MatrixMath::Multiply3x3with3x1(rotMat, coords, newcoords);
    if(newcoords[0] < xMin) { xMin = newcoords[0]; }
    if(newcoords[0] > xMax) { xMax = newcoords[0]; }
    if(newcoords[1] < yMin) { yMin = newcoords[1]; }
    if(newcoords[1] > yMax) { yMax = newcoords[1]; }
    if(newcoords[2] < zMin) { zMin = newcoords[2]; }
    if(newcoords[2] > zMax) { zMax = newcoords[2]; }
  }
  float xAxis[3] = {1, 0, 0};
  float yAxis[3] = {0, 1, 0};
  float zAxis[3] = {0, 0, 1};
  float xAxisNew[3];
  float yAxisNew[3];
  float zAxisNew[3];
  MatrixMath::Multiply3x3with3x1(rotMat, xAxis, xAxisNew);
  MatrixMath::Multiply3x3with3x1(rotMat, yAxis, yAxisNew);
  MatrixMath::Multiply3x3with3x1(rotMat, zAxis, zAxisNew);
  float closestAxis;
  xResNew = xRes;
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, xAxisNew));
  if(fabs(GeometryMath::CosThetaBetweenVectors(yAxis, xAxisNew)) > closestAxis) { xResNew = yRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, xAxisNew)); }
  if(fabs(GeometryMath::CosThetaBetweenVectors(zAxis, xAxisNew)) > closestAxis) { xResNew = zRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, xAxisNew)); }
  yResNew = yRes;
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, yAxisNew));
  if(fabs(GeometryMath::CosThetaBetweenVectors(xAxis, yAxisNew)) > closestAxis) { yResNew = xRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, yAxisNew)); }
  if(fabs(GeometryMath::CosThetaBetweenVectors(zAxis, yAxisNew)) > closestAxis) { yResNew = zRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, yAxisNew)); }
  zResNew = zRes;
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, zAxisNew));
  if(fabs(GeometryMath::CosThetaBetweenVectors(xAxis, zAxisNew)) > closestAxis) { zResNew = xRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, zAxisNew)); }
  if(fabs(GeometryMath::CosThetaBetweenVectors(yAxis, zAxisNew)) > closestAxis) { zResNew = yRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, zAxisNew)); }

  xpNew = ((xMax - xMin) / xResNew) + 1;
  ypNew = ((yMax - yMin) / yResNew) + 1;
  zpNew = ((zMax - zMin) / zResNew) + 1;

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

  float rotAngle = m_RotationAngle * DREAM3D::Constants::k_Pi / 180.0;

  int32_t xp, yp, zp;
  float xRes, yRes, zRes;
  int32_t xpNew, ypNew, zpNew;
  float xResNew, yResNew, zResNew;
  RotateSampleRefFrameImplArg_t params;

  xp = static_cast<int32_t>(m->getGeometryAs<ImageGeom>()->getXPoints());
  xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  yp = static_cast<int32_t>(m->getGeometryAs<ImageGeom>()->getYPoints());
  yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  zp = static_cast<int32_t>(m->getGeometryAs<ImageGeom>()->getZPoints());
  zRes = m->getGeometryAs<ImageGeom>()->getZRes();

  params.xp = xp;
  params.xRes = xRes;
  params.yp = yp;
  params.yRes = yRes;
  params.zp = zp;
  params.zRes = zRes;

  int32_t col = 0, row = 0, plane = 0;
  float rotMat[3][3];
  float coords[3];
  float newcoords[3];
  float xMin = 100000000, xMax = 0, yMin = 100000000, yMax = 0, zMin = 100000000, zMax = 0;

  OrientationMath::AxisAngletoMat(rotAngle, m_RotationAxis.x, m_RotationAxis.y, m_RotationAxis.z, rotMat);
  for(int i = 0; i < 8; i++)
  {
    if(i == 0) { col = 0, row = 0, plane = 0; }
    if(i == 1) { col = xp - 1, row = 0, plane = 0; }
    if(i == 2) { col = 0, row = yp - 1, plane = 0; }
    if(i == 3) { col = xp - 1, row = yp - 1, plane = 0; }
    if(i == 4) { col = 0, row = 0, plane = zp - 1; }
    if(i == 5) { col = xp - 1, row = 0, plane = zp - 1; }
    if(i == 6) { col = 0, row = yp - 1, plane = zp - 1; }
    if(i == 7) { col = xp - 1, row = yp - 1, plane = zp - 1; }
    if(col < 0) { col = 0; }
    if(row < 0) { row = 0; }
    if(plane < 0) { plane = 0; }
    coords[0] = col * xRes;
    coords[1] = row * yRes;
    coords[2] = plane * zRes;
    MatrixMath::Multiply3x3with3x1(rotMat, coords, newcoords);
    if(newcoords[0] < xMin) { xMin = newcoords[0]; }
    if(newcoords[0] > xMax) { xMax = newcoords[0]; }
    if(newcoords[1] < yMin) { yMin = newcoords[1]; }
    if(newcoords[1] > yMax) { yMax = newcoords[1]; }
    if(newcoords[2] < zMin) { zMin = newcoords[2]; }
    if(newcoords[2] > zMax) { zMax = newcoords[2]; }
  }
  float xAxis[3] = {1, 0, 0};
  float yAxis[3] = {0, 1, 0};
  float zAxis[3] = {0, 0, 1};
  float xAxisNew[3];
  float yAxisNew[3];
  float zAxisNew[3];
  MatrixMath::Multiply3x3with3x1(rotMat, xAxis, xAxisNew);
  MatrixMath::Multiply3x3with3x1(rotMat, yAxis, yAxisNew);
  MatrixMath::Multiply3x3with3x1(rotMat, zAxis, zAxisNew);
  float closestAxis;
  xResNew = xRes;
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, xAxisNew));
  if(fabs(GeometryMath::CosThetaBetweenVectors(yAxis, xAxisNew)) > closestAxis) { xResNew = yRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, xAxisNew)); }
  if(fabs(GeometryMath::CosThetaBetweenVectors(zAxis, xAxisNew)) > closestAxis) { xResNew = zRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, xAxisNew)); }
  yResNew = yRes;
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, yAxisNew));
  if(fabs(GeometryMath::CosThetaBetweenVectors(xAxis, yAxisNew)) > closestAxis) { yResNew = xRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, yAxisNew)); }
  if(fabs(GeometryMath::CosThetaBetweenVectors(zAxis, yAxisNew)) > closestAxis) { yResNew = zRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, yAxisNew)); }
  zResNew = zRes;
  closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(zAxis, zAxisNew));
  if(fabs(GeometryMath::CosThetaBetweenVectors(xAxis, zAxisNew)) > closestAxis) { zResNew = xRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(xAxis, zAxisNew)); }
  if(fabs(GeometryMath::CosThetaBetweenVectors(yAxis, zAxisNew)) > closestAxis) { zResNew = yRes, closestAxis = fabs(GeometryMath::CosThetaBetweenVectors(yAxis, zAxisNew)); }

  xpNew = ((xMax - xMin) / xResNew) + 1;
  ypNew = ((yMax - yMin) / yResNew) + 1;
  zpNew = ((zMax - zMin) / zResNew) + 1;

  params.xpNew = xpNew;
  params.xResNew = xResNew;
  params.xMinNew = xMin;
  params.ypNew = ypNew;
  params.yResNew = yResNew;
  params.yMinNew = yMin;
  params.zpNew = zpNew;
  params.zResNew = zResNew;
  params.zMinNew = zMin;

  size_t newNumCellTuples = params.xpNew * params.ypNew * params.zpNew;

  DataArray<int64_t>::Pointer newIndiciesPtr = DataArray<int64_t>::CreateArray(newNumCellTuples, "RotateSampleRef_NewIndicies");
  newIndiciesPtr->initializeWithValue(-1);
  int64_t* newindicies = newIndiciesPtr->getPointer(0);

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range3d<size_t, size_t, size_t>(0, params.zpNew, 0, params.ypNew, 0, params.xpNew),
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
    //QString name = *iter;
    IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(*iter);
    // Make a copy of the 'p' array that has the same name. When placed into
    // the data container this will over write the current array with
    // the same name.
    IDataArray::Pointer data = p->createNewArray(newNumCellTuples, p->getComponentDimensions(), p->getName());
    void* source = NULL;
    void* destination = NULL;
    int64_t newIndicies_I = 0;
    int nComp = data->getNumberOfComponents();
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
  /*
  * RotationAxis
  * RotationAngle
  * SliceBySlice
  */
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
{ return Sampling::SamplingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RotateSampleRefFrame::getHumanLabel()
{ return "Rotate Sample Reference Frame"; }

