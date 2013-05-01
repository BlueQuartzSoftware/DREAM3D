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


#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/MatrixMath.h"

const static float m_pi = static_cast<float>(M_PI);

typedef struct {
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
    RotateSampleRefFrameImplArg_t*  m_params;
    float rotMatrixInv[3][3];

  public:
    RotateSampleRefFrameImpl(DataArray<int64_t>::Pointer newindices, RotateSampleRefFrameImplArg_t*  args, float rotMat[3][3]) :
      newIndicesPtr(newindices),
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
    virtual ~RotateSampleRefFrameImpl(){}

    void convert(size_t zStart, size_t zEnd, size_t yStart, size_t yEnd, size_t xStart, size_t xEnd) const
    {

      int64_t* newindicies = newIndicesPtr->GetPointer(0);
      int64_t index = 0;
      int64_t ktot, jtot;
      //      float rotMatrixInv[3][3];
      float coords[3];
      float coordsNew[3];
      size_t colOld, rowOld, planeOld;

      //MatrixMath::transpose3x3(rotMatrix, rotMatrixInv);

      for (size_t k = zStart; k < zEnd; k++)
      {
        ktot = (m_params->xpNew*m_params->ypNew)*k;
        for (size_t j = yStart; j < yEnd; j++)
        {
          jtot = (m_params->xpNew)*j;
          for (size_t i = xStart; i < xEnd; i++)
          {
            index = ktot + jtot + i;
            newindicies[index] = -1;
            coords[2] = (float(k)*m_params->zResNew)+m_params->zMinNew;
            coords[1] = (float(j)*m_params->yResNew)+m_params->yMinNew;
            coords[0] = (float(i)*m_params->xResNew)+m_params->xMinNew;
            coordsNew[0] = rotMatrixInv[0][0]*coords[0]+rotMatrixInv[0][1]*coords[1]+rotMatrixInv[0][2]*coords[2];
            coordsNew[1] = rotMatrixInv[1][0]*coords[0]+rotMatrixInv[1][1]*coords[1]+rotMatrixInv[1][2]*coords[2];
            coordsNew[2] = rotMatrixInv[2][0]*coords[0]+rotMatrixInv[2][1]*coords[1]+rotMatrixInv[2][2]*coords[2];
            colOld = coordsNew[0]/m_params->xRes;
            rowOld = coordsNew[1]/m_params->yRes;
            planeOld = coordsNew[2]/m_params->zRes;
            if(colOld >= 0 && colOld < m_params->xp && rowOld >= 0 && rowOld < m_params->yp && planeOld >= 0 && planeOld < m_params->zp)
            {
              newindicies[index] = (m_params->xp*m_params->yp*planeOld)+(m_params->xp*rowOld)+colOld;
            }
          }
        }
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range3d<size_t, size_t, size_t> &r) const
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
  m_RotationAngle(0.0)
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
  std::vector<FilterParameter::Pointer> parameters;
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Rotation Axis");
    option->setPropertyName("RotationAxis");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("ijk");
    parameters.push_back(option);
  }
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Rotation Angle");
    option->setPropertyName("RotationAngle");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("Degrees");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("RotationAxis", getRotationAxis() );
  writer->writeValue("RotationAngle", getRotationAngle() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::preflight()
{
  dataCheck(true, 1, 1, 1);

  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  m_RotationAngle = m_RotationAngle*m_pi/180.0;

  int32_t xp, yp, zp;
  float xRes, yRes, zRes;
  int32_t xpNew, ypNew, zpNew;
  float xResNew, yResNew, zResNew;
  RotateSampleRefFrameImplArg_t params;


  xp = static_cast<int32_t>(m->getXPoints());
  xRes = m->getXRes();
  yp = static_cast<int32_t>(m->getYPoints());
  yRes = m->getYRes();
  zp = static_cast<int32_t>(m->getZPoints());
  zRes = m->getZRes();

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

  OrientationMath::axisAngletoMat(m_RotationAngle, m_RotationAxis.x, m_RotationAxis.y, m_RotationAxis.z, rotMat);
  for(int i=0;i<8;i++)
  {
    if(i == 0) col = 0, row = 0, plane = 0;
    if(i == 1) col = xp-1, row = 0, plane = 0;
    if(i == 2) col = 0, row = yp-1, plane = 0;
    if(i == 3) col = xp-1, row = yp-1, plane = 0;
    if(i == 4) col = 0, row = 0, plane = zp-1;
    if(i == 5) col = xp-1, row = 0, plane = zp-1;
    if(i == 6) col = 0, row = yp-1, plane = zp-1;
    if(i == 7) col = xp-1, row = yp-1, plane = zp-1;
    coords[0] = col*xRes;
    coords[1] = row*yRes;
    coords[2] = plane*zRes;
    MatrixMath::multiply3x3with3x1(rotMat,coords,newcoords);
    if(newcoords[0] < xMin) xMin = newcoords[0];
    if(newcoords[0] > xMax) xMax = newcoords[0];
    if(newcoords[1] < yMin) yMin = newcoords[1];
    if(newcoords[1] > yMax) yMax = newcoords[1];
    if(newcoords[2] < zMin) zMin = newcoords[2];
    if(newcoords[2] > zMax) zMax = newcoords[2];
  }
  float xAxis[3] = {1,0,0};
  float yAxis[3] = {0,1,0};
  float zAxis[3] = {0,0,1};
  float xAxisNew[3];
  float yAxisNew[3];
  float zAxisNew[3];
  MatrixMath::multiply3x3with3x1(rotMat,xAxis,xAxisNew);
  MatrixMath::multiply3x3with3x1(rotMat,yAxis,yAxisNew);
  MatrixMath::multiply3x3with3x1(rotMat,zAxis,zAxisNew);
  float closestAxis;
  xResNew = xRes;
  closestAxis = fabs(MatrixMath::dotProduct(xAxis,xAxisNew));
  if(fabs(MatrixMath::dotProduct(yAxis,xAxisNew)) > closestAxis) xResNew = yRes, closestAxis = fabs(MatrixMath::dotProduct(yAxis,xAxisNew));
  if(fabs(MatrixMath::dotProduct(zAxis,xAxisNew)) > closestAxis) xResNew = zRes, closestAxis = fabs(MatrixMath::dotProduct(zAxis,xAxisNew));
  yResNew = yRes;
  closestAxis = fabs(MatrixMath::dotProduct(yAxis,yAxisNew));
  if(fabs(MatrixMath::dotProduct(xAxis,yAxisNew)) > closestAxis) yResNew = xRes, closestAxis = fabs(MatrixMath::dotProduct(xAxis,yAxisNew));
  if(fabs(MatrixMath::dotProduct(zAxis,yAxisNew)) > closestAxis) yResNew = zRes, closestAxis = fabs(MatrixMath::dotProduct(zAxis,yAxisNew));
  zResNew = zRes;
  closestAxis = fabs(MatrixMath::dotProduct(zAxis,zAxisNew));
  if(fabs(MatrixMath::dotProduct(xAxis,zAxisNew)) > closestAxis) zResNew = xRes, closestAxis = fabs(MatrixMath::dotProduct(xAxis,zAxisNew));
  if(fabs(MatrixMath::dotProduct(yAxis,zAxisNew)) > closestAxis) zResNew = yRes, closestAxis = fabs(MatrixMath::dotProduct(yAxis,zAxisNew));

  xpNew = ((xMax-xMin)/xResNew)+1;
  ypNew = ((yMax-yMin)/yResNew)+1;
  zpNew = ((zMax-zMin)/zResNew)+1;

  params.xpNew = xpNew;
  params.xResNew = xResNew;
  params.xMinNew = xMin;
  params.ypNew = ypNew;
  params.yResNew = yResNew;
  params.yMinNew = yMin;
  params.zpNew = zpNew;
  params.zResNew = zResNew;
  params.zMinNew = zMin;

  m->setResolution(params.xResNew, params.yResNew, params.zResNew);
  m->setDimensions(params.xpNew, params.ypNew, params.zpNew);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  int64_t totalPoints = m->getTotalPoints();
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, numgrains, numensembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  m_RotationAngle = m_RotationAngle*m_pi/180.0;

  int32_t xp, yp, zp;
  float xRes, yRes, zRes;
  int32_t xpNew, ypNew, zpNew;
  float xResNew, yResNew, zResNew;
  RotateSampleRefFrameImplArg_t params;


  xp = static_cast<int32_t>(m->getXPoints());
  xRes = m->getXRes();
  yp = static_cast<int32_t>(m->getYPoints());
  yRes = m->getYRes();
  zp = static_cast<int32_t>(m->getZPoints());
  zRes = m->getZRes();

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

  OrientationMath::axisAngletoMat(m_RotationAngle, m_RotationAxis.x, m_RotationAxis.y, m_RotationAxis.z, rotMat);
  for(int i=0;i<8;i++)
  {
    if(i == 0) col = 0, row = 0, plane = 0;
    if(i == 1) col = xp-1, row = 0, plane = 0;
    if(i == 2) col = 0, row = yp-1, plane = 0;
    if(i == 3) col = xp-1, row = yp-1, plane = 0;
    if(i == 4) col = 0, row = 0, plane = zp-1;
    if(i == 5) col = xp-1, row = 0, plane = zp-1;
    if(i == 6) col = 0, row = yp-1, plane = zp-1;
    if(i == 7) col = xp-1, row = yp-1, plane = zp-1;
    coords[0] = col*xRes;
    coords[1] = row*yRes;
    coords[2] = plane*zRes;
    MatrixMath::multiply3x3with3x1(rotMat,coords,newcoords);
    if(newcoords[0] < xMin) xMin = newcoords[0];
    if(newcoords[0] > xMax) xMax = newcoords[0];
    if(newcoords[1] < yMin) yMin = newcoords[1];
    if(newcoords[1] > yMax) yMax = newcoords[1];
    if(newcoords[2] < zMin) zMin = newcoords[2];
    if(newcoords[2] > zMax) zMax = newcoords[2];
  }
  float xAxis[3] = {1,0,0};
  float yAxis[3] = {0,1,0};
  float zAxis[3] = {0,0,1};
  float xAxisNew[3];
  float yAxisNew[3];
  float zAxisNew[3];
  MatrixMath::multiply3x3with3x1(rotMat,xAxis,xAxisNew);
  MatrixMath::multiply3x3with3x1(rotMat,yAxis,yAxisNew);
  MatrixMath::multiply3x3with3x1(rotMat,zAxis,zAxisNew);
  float closestAxis;
  xResNew = xRes;
  closestAxis = fabs(MatrixMath::dotProduct(xAxis,xAxisNew));
  if(fabs(MatrixMath::dotProduct(yAxis,xAxisNew)) > closestAxis) xResNew = yRes, closestAxis = fabs(MatrixMath::dotProduct(yAxis,xAxisNew));
  if(fabs(MatrixMath::dotProduct(zAxis,xAxisNew)) > closestAxis) xResNew = zRes, closestAxis = fabs(MatrixMath::dotProduct(zAxis,xAxisNew));
  yResNew = yRes;
  closestAxis = fabs(MatrixMath::dotProduct(yAxis,yAxisNew));
  if(fabs(MatrixMath::dotProduct(xAxis,yAxisNew)) > closestAxis) yResNew = xRes, closestAxis = fabs(MatrixMath::dotProduct(xAxis,yAxisNew));
  if(fabs(MatrixMath::dotProduct(zAxis,yAxisNew)) > closestAxis) yResNew = zRes, closestAxis = fabs(MatrixMath::dotProduct(zAxis,yAxisNew));
  zResNew = zRes;
  closestAxis = fabs(MatrixMath::dotProduct(zAxis,zAxisNew));
  if(fabs(MatrixMath::dotProduct(xAxis,zAxisNew)) > closestAxis) zResNew = xRes, closestAxis = fabs(MatrixMath::dotProduct(xAxis,zAxisNew));
  if(fabs(MatrixMath::dotProduct(yAxis,zAxisNew)) > closestAxis) zResNew = yRes, closestAxis = fabs(MatrixMath::dotProduct(yAxis,zAxisNew));

  xpNew = ((xMax-xMin)/xResNew)+1;
  ypNew = ((yMax-yMin)/yResNew)+1;
  zpNew = ((zMax-zMin)/zResNew)+1;

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


  DataArray<int64_t>::Pointer newIndiciesPtr = DataArray<int64_t>::CreateArray(newNumCellTuples, 1, "RotateSampleRef_NewIndicies");
  newIndiciesPtr->initializeWithValues(-1);
  int64_t* newindicies = newIndiciesPtr->GetPointer(0);

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range3d<size_t, size_t, size_t>(0, params.zpNew, 0, params.ypNew, 0, params.xpNew),
                      RotateSampleRefFrameImpl(newIndiciesPtr, &params, rotMat), tbb::auto_partitioner());
  }
  else
#endif
  {
    RotateSampleRefFrameImpl serial(newIndiciesPtr, &params, rotMat);
    serial.convert(0, params.zpNew, 0, params.ypNew, 0, params.xpNew);
  }


  // This could technically be parallelized also where each thred takes an array to adjust. Except
  // that the DataContainer is NOT thread safe or re-entrant so that would actually be a BAD idea.
  std::list<std::string> voxelArrayNames = m->getCellArrayNameList();
  for (std::list<std::string>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    //std::string name = *iter;
    IDataArray::Pointer p = m->getCellData(*iter);
    // Make a copy of the 'p' array that has the same name. When placed into
    // the data container this will over write the current array with
    // the same name.
    IDataArray::Pointer data = p->createNewArray(newNumCellTuples, p->GetNumberOfComponents(), p->GetName());
    void* source = NULL;
    void* destination = NULL;
    int64_t newIndicies_I = 0;
    int nComp = data->GetNumberOfComponents();
    for (size_t i = 0; i < static_cast<size_t>(newNumCellTuples); i++)
    {
      newIndicies_I = newindicies[i];
      if(newIndicies_I >= 0)
      {
        source = p->GetVoidPointer((nComp * newIndicies_I));
        destination = data->GetVoidPointer((data->GetNumberOfComponents() * i));
        ::memcpy(destination, source, p->GetTypeSize() * data->GetNumberOfComponents());
      }
      else
      {
        data->InitializeTuple(i,0);
      }
    }
    m->addCellData(*iter, data);
  }
  m->setResolution(params.xResNew, params.yResNew, params.zResNew);
  m->setDimensions(params.xpNew, params.ypNew, params.zpNew);

  notifyStatusMessage("Complete");
}
