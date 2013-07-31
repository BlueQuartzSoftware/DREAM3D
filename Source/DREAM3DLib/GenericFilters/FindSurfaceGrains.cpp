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

#include "FindSurfaceGrains.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceGrains::FindSurfaceGrains() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_SurfaceFieldsArrayName(DREAM3D::FieldData::SurfaceFields),
m_GrainIds(NULL),
m_SurfaceFields(NULL)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceGrains::~FindSurfaceGrains()
{
}
// -----------------------------------------------------------------------------
void FindSurfaceGrains::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSurfaceGrains::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceGrains::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  // Cell Data
  GET_PREREQ_DATA( m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1)

  // Field Data
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, SurfaceFields, ss, bool, BoolArrayType, false, fields, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceGrains::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceGrains::execute()
{
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << " DataContainer was NULL";
    notifyErrorMessage(ss.str(), -1);
    return;
  }
  setErrorCondition(0);

  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumFieldTuples();
  ss << "FSG Points - " << totalPoints << ", Fields - " << totalFields;
  notifyStatusMessage(ss.str());
  dataCheck(false, totalPoints, totalFields, 1);
  if (getErrorCondition() < 0)
  {
    return;
  }

  if(m->getXPoints() > 1 && m->getYPoints() > 1 && m->getZPoints() > 1) find_surfacegrains();
  if(m->getXPoints() == 1 || m->getYPoints() == 1 || m->getZPoints() == 1) find_surfacegrains2D();


 notifyStatusMessage("FindSurfaceGrains Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceGrains::find_surfacegrains()
{
  VoxelDataContainer* m = getVoxelDataContainer();
//  int64_t totalPoints = m->getTotalPoints();

  size_t xPoints = m->getXPoints();
  size_t yPoints = m->getYPoints();
  size_t zPoints = m->getZPoints();

  int zStride, yStride;
  for(size_t i=0;i<zPoints;i++)
  {
  zStride = i*xPoints*yPoints;
  for (size_t j=0;j<yPoints;j++)
  {
    yStride = j*xPoints;
    for(size_t k=0;k<xPoints;k++)
    {
      int gnum = m_GrainIds[zStride+yStride+k];
      if(m_SurfaceFields[gnum] == false)
      {
        if(k <= 0) m_SurfaceFields[gnum] = true;
        if(k >= xPoints - 1) m_SurfaceFields[gnum] = true;
        if(j <= 0) m_SurfaceFields[gnum] = true;
        if(j >= yPoints - 1) m_SurfaceFields[gnum] = true;
        if(i <= 0) m_SurfaceFields[gnum] = true;
        if(i >= zPoints - 1) m_SurfaceFields[gnum] = true;
        if(m_SurfaceFields[gnum] == false)
        {
        if(m_GrainIds[zStride+yStride+k - 1] == 0) m_SurfaceFields[gnum] = true;
        if(m_GrainIds[zStride+yStride+k + 1] == 0) m_SurfaceFields[gnum] = true;
        if(m_GrainIds[zStride+yStride+k - xPoints] == 0) m_SurfaceFields[gnum] = true;
        if(m_GrainIds[zStride+yStride+k + xPoints] == 0) m_SurfaceFields[gnum] = true;
        if(m_GrainIds[zStride+yStride+k - (xPoints * yPoints)] == 0) m_SurfaceFields[gnum] = true;
        if(m_GrainIds[zStride+yStride+k + (xPoints * yPoints)] == 0) m_SurfaceFields[gnum] = true;
        }
      }
    }
  }
  }
}
void FindSurfaceGrains::find_surfacegrains2D()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  //int64_t totalPoints = m->getTotalPoints();

  //size_t dims[3] = {0,0,0};

  int xPoints = 0, yPoints = 0;
  float xRes= 0.0f, yRes = 0.0f;

  if(m->getXPoints() == 1)
  {
    xPoints = m->getYPoints();
    xRes = m->getYRes();
    yPoints = m->getZPoints();
    yRes = m->getZRes();
  }
  if(m->getYPoints() == 1)
  {
    xPoints = m->getXPoints();
    xRes = m->getXRes();
    yPoints = m->getZPoints();
    yRes = m->getZRes();
  }
  if(m->getZPoints() == 1)
  {
    xPoints = m->getXPoints();
    xRes = m->getXRes();
    yPoints = m->getYPoints();
    yRes = m->getYRes();
  }

  int yStride;
  for (int j=0;j<yPoints;j++)
  {
  yStride = j*xPoints;
  for(int k=0;k<xPoints;k++)
  {
    int gnum = m_GrainIds[yStride+k];
    if(m_SurfaceFields[gnum] == false)
    {
      if(k <= 0) m_SurfaceFields[gnum] = true;
      if(k >= xPoints - 1) m_SurfaceFields[gnum] = true;
      if(j <= 0) m_SurfaceFields[gnum] = true;
      if(j >= yPoints - 1) m_SurfaceFields[gnum] = true;
      if(m_SurfaceFields[gnum] == false)
      {
      if(m_GrainIds[yStride+k - 1] == 0) m_SurfaceFields[gnum] = true;
      if(m_GrainIds[yStride+k + 1] == 0) m_SurfaceFields[gnum] = true;
      if(m_GrainIds[yStride+k - m->getXPoints()] == 0) m_SurfaceFields[gnum] = true;
      if(m_GrainIds[yStride+k + m->getXPoints()] == 0) m_SurfaceFields[gnum] = true;
      }
    }
  }
  }
}
