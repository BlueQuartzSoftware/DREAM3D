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
FindSurfaceGrains::FindSurfaceGrains()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceGrains::~FindSurfaceGrains()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceGrains::execute()
{
  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }
  setErrorCondition(0);

  if(m->getZPoints() > 1) find_surfacegrains();

  if (getErrorCondition() < 0)
  {return;}

  if(m->getZPoints() == 1) find_surfacegrains2D();

  if (getErrorCondition() < 0)
  {return;}

  notify("FindSurfaceGrains Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceGrains::find_surfacegrains()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();

    int32_t* grain_indicies = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
  if (NULL == grain_indicies) { return; }
  size_t xPoints = m->getXPoints();
  size_t yPoints = m->getYPoints();
  size_t zPoints = m->getZPoints();
  size_t col, row, plane;
  for (int64_t j = 0; j < totalPoints; j++)
  {
    int gnum = grain_indicies[j];
    if(m->m_Grains[gnum]->surfacefield == false)
    {
      col = j % xPoints;
      row = (j / xPoints) % yPoints;
      plane = j / (xPoints * yPoints);
      if(col <= 0) m->m_Grains[gnum]->surfacefield = true;
      if(col >= xPoints - 1) m->m_Grains[gnum]->surfacefield = true;
      if(row <= 0) m->m_Grains[gnum]->surfacefield = true;
      if(row >= yPoints - 1) m->m_Grains[gnum]->surfacefield = true;
      if(plane <= 0) m->m_Grains[gnum]->surfacefield = true;
      if(plane >= zPoints - 1) m->m_Grains[gnum]->surfacefield = true;
      if(m->m_Grains[gnum]->surfacefield == false)
      {
        if(grain_indicies[j - 1] == 0) m->m_Grains[gnum]->surfacefield = true;
        if(grain_indicies[j + 1] == 0) m->m_Grains[gnum]->surfacefield = true;
        if(grain_indicies[j - xPoints] == 0) m->m_Grains[gnum]->surfacefield = true;
        if(grain_indicies[j + xPoints] == 0) m->m_Grains[gnum]->surfacefield = true;
        if(grain_indicies[j - (xPoints * yPoints)] == 0) m->m_Grains[gnum]->surfacefield = true;
        if(grain_indicies[j + (xPoints * yPoints)] == 0) m->m_Grains[gnum]->surfacefield = true;
      }
    }
  }
}
void FindSurfaceGrains::find_surfacegrains2D()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();

  int32_t* grain_indicies = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
  if (NULL == grain_indicies) { return; }

  size_t dims[3] = {0,0,0};

  m->getDimensions(dims);
  size_t planePoints = dims[0] * dims[1];

  size_t col, row;
  for (size_t j = 0; j < planePoints; j++)
  {
    int gnum = grain_indicies[j];
    if(m->m_Grains[gnum]->surfacefield == false)
    {
      col = j % m->getXPoints();
      row = (j / m->getXPoints()) % m->getYPoints();
      if(col <= 0) m->m_Grains[gnum]->surfacefield = true;
      if(col >= m->getXPoints() - 1) m->m_Grains[gnum]->surfacefield = true;
      if(row <= 0) m->m_Grains[gnum]->surfacefield = true;
      if(row >= m->getYPoints() - 1) m->m_Grains[gnum]->surfacefield = true;
      if(m->m_Grains[gnum]->surfacefield == false)
      {
        if(grain_indicies[j - 1] == 0) m->m_Grains[gnum]->surfacefield = true;
        if(grain_indicies[j + 1] == 0) m->m_Grains[gnum]->surfacefield = true;
        if(grain_indicies[j - m->getXPoints()] == 0) m->m_Grains[gnum]->surfacefield = true;
        if(grain_indicies[j + m->getXPoints()] == 0) m->m_Grains[gnum]->surfacefield = true;
      }
    }
  }
}
