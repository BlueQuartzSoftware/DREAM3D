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

#include "FindSizes.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

const static float m_pi = M_PI;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSizes::FindSizes()
{
  graincounts = NULL;
  INIT_DataArray(m_GrainCounts,float);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSizes::~FindSizes()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::execute()
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

  int64_t totalPoints = m->totalPoints();
    int32_t* gi = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
  if (NULL == gi) { return; }
  grain_indicies = gi;

  setErrorCondition(0);

  if(m->getZPoints() > 1) find_sizes();
  if(m->getZPoints() == 1) find_sizes2D();
  notify("FindSizes Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::find_sizes()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();

  float radcubed;
  float diameter;
  size_t numgrains = m->m_Grains.size();
  graincounts = m_GrainCounts->WritePointer(0, numgrains);

  // Initialize every element to 0.0
  for (size_t i = 0; i < numgrains * 1; i++)
  {
    graincounts[i] = 0.0f;
  }
  for (int j = 0; j < totalPoints; j++)
  {
    int gnum = grain_indicies[j];
    graincounts[gnum]++;
  }
  float res_scalar = m->getXRes() * m->getYRes() * m->getZRes();
  float vol_term = (4.0/3.0)*m_pi;
  for (size_t i = 1; i < numgrains; i++)
  {
    m->m_Grains[i]->numvoxels = graincounts[i];
    m->m_Grains[i]->volume = (graincounts[i] * res_scalar);
    radcubed = m->m_Grains[i]->volume/vol_term;
    diameter = 2.0*powf(radcubed, 0.3333333333);
    m->m_Grains[i]->equivdiameter = diameter;
  }
}
void FindSizes::find_sizes2D()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();

  float radsquared;
  float diameter;
  size_t numgrains = m->m_Grains.size();
  graincounts = m_GrainCounts->WritePointer(0, numgrains);

  for (size_t i = 0; i < numgrains; i++)
  {
      graincounts[i] = 0.0f;
  }
  for (int j = 0; j < totalPoints; j++)
  {
    int gnum = grain_indicies[j];
    graincounts[gnum]++;
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    m->m_Grains[i]->numvoxels = graincounts[i];
    m->m_Grains[i]->volume = (graincounts[i] * m->getXRes() * m->getYRes());
    radsquared = m->m_Grains[i]->volume / m_pi;
    diameter = (2 * sqrt(radsquared));
    m->m_Grains[i]->equivdiameter = diameter;
  }
}

