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

#include "FindNeighborhoods.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

const static float m_pi = M_PI;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighborhoods::FindNeighborhoods() :
            AbstractFilter()
{
  graincenters = NULL;
  INIT_DataArray(m_GrainCenters,float);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighborhoods::~FindNeighborhoods()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::execute()
{
  DataContainer* m = getDataContainer();
  setErrorCondition(0);

  if(m->getZPoints() > 1) find_centroids();
  if(m->getZPoints() == 1) find_centroids2D();
  find_neighborhoods();
  notify("FindNeighborhoods Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::find_centroids()
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
    int32_t* grain_indicies = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
  if (NULL == grain_indicies) { return; }

  float x, y, z;
  int col, row, plane;
  float radcubed;
  float diameter;
//  float allvol = 0.0;
  size_t numgrains = m->m_Grains.size();
  graincenters = m_GrainCenters->WritePointer(0, numgrains * 5);
  m_GrainCenters->SetNumberOfComponents(5);

  // Initialize every element to 0.0
  for (size_t i = 0; i < numgrains * 5; i++)
  {
    graincenters[i] = 0.0f;
  }
  for (int j = 0; j < totalPoints; j++)
  {
    int gnum = grain_indicies[j];
    graincenters[gnum*5 + 0]++;
    col = j % m->getXPoints();
    row = (j / m->getXPoints()) % m->getYPoints();
    plane = j / (m->getXPoints() * m->getYPoints());
	x = float(col)*m->getXRes();
	y = float(row)*m->getYRes();
	z = float(plane)*m->getZRes();
    graincenters[gnum*5 + 1] = graincenters[gnum*5 + 1] + x;
    graincenters[gnum*5 + 2] = graincenters[gnum*5 + 2] + y;
    graincenters[gnum*5 + 3] = graincenters[gnum*5 + 3] + z;
  }
  float res_scalar = m->getXRes() * m->getYRes() * m->getZRes();
  float vol_term = (4.0/3.0)*m_pi;
  for (size_t i = 1; i < numgrains; i++)
  {
    graincenters[i*5 + 1] = graincenters[i*5 + 1] / graincenters[i*5 + 0];
    graincenters[i*5 + 2] = graincenters[i*5 + 2] / graincenters[i*5 + 0];
    graincenters[i*5 + 3] = graincenters[i*5 + 3] / graincenters[i*5 + 0];
    m->m_Grains[i]->centroidx = graincenters[i*5 + 1];
    m->m_Grains[i]->centroidy = graincenters[i*5 + 2];
    m->m_Grains[i]->centroidz = graincenters[i*5 + 3];
    m->m_Grains[i]->numvoxels = graincenters[i*5 + 0];
    m->m_Grains[i]->volume = (graincenters[i*5 + 0] * res_scalar);
    radcubed = m->m_Grains[i]->volume/vol_term;
    diameter = 2.0*powf(radcubed, 0.3333333333);
    m->m_Grains[i]->equivdiameter = diameter;
  }
}
void FindNeighborhoods::find_centroids2D()
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
    int32_t* grain_indicies = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
  if (NULL == grain_indicies) { return; }

  float x, y;
  int col, row;
  float radsquared;
  float diameter;
  size_t numgrains = m->m_Grains.size();
  graincenters = m_GrainCenters->WritePointer(0, numgrains * 5);
  m_GrainCenters->SetNumberOfComponents(5);

  for (size_t i = 0; i < numgrains*5; i++)
  {
      graincenters[i] = 0.0f;
  }
  for (int j = 0; j < totalPoints; j++)
  {
    int gnum = grain_indicies[j];
    graincenters[gnum*5 + 0]++;
    col = j % m->getXPoints();
    row = (j / m->getXPoints()) % m->getYPoints();
	x = float(col)*m->getXRes();
	y = float(row)*m->getYRes();
    graincenters[gnum*5 + 1] = graincenters[gnum*5 + 1] + x;
    graincenters[gnum*5 + 2] = graincenters[gnum*5 + 2] + y;
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    graincenters[i*5 + 1] = graincenters[i*5 + 1] / graincenters[i*5 + 0];
    graincenters[i*5 + 2] = graincenters[i*5 + 2] / graincenters[i*5 + 0];
    m->m_Grains[i]->centroidx = graincenters[i*5 + 1];
    m->m_Grains[i]->centroidy = graincenters[i*5 + 2];
    m->m_Grains[i]->numvoxels = graincenters[i*5 + 0];
    m->m_Grains[i]->volume = (graincenters[i*5 + 0] * m->getXRes() * m->getYRes());
    radsquared = m->m_Grains[i]->volume / m_pi;
    diameter = (2 * sqrt(radsquared));
    m->m_Grains[i]->equivdiameter = diameter;
  }
}

void FindNeighborhoods::find_neighborhoods()
{
  DataContainer* m = getDataContainer();
  float x, y, z;
  float xn, yn, zn;
  float xdist, ydist, zdist;
  float dist, dist2, diam, diam2;
  unsigned int dist_int, dist2_int;
  size_t numgrains = m->m_Grains.size();

  for (size_t i = 1; i < numgrains; i++)
  {
    Field& grain = *(m->m_Grains[i].get());
    if (grain.active == true)
    {
      x = grain.centroidx;
      y = grain.centroidy;
      z = grain.centroidz;
      diam = grain.equivdiameter;
      for (size_t j = i; j < numgrains; j++)
      {
        Field& grain_j = *(m->m_Grains[j].get());
        if (grain_j.active == true)
        {
          xn = grain_j.centroidx;
          yn = grain_j.centroidy;
          zn = grain_j.centroidz;
          diam2 = grain_j.equivdiameter;
          xdist = fabs(x - xn);
          ydist = fabs(y - yn);
          zdist = fabs(z - zn);
          dist = (xdist * xdist) + (ydist * ydist) + (zdist * zdist);
          dist = sqrt(dist);
          dist2 = dist;
          dist_int = int(dist / (diam / 2.0));
          dist2_int = int(dist2 / (diam2 / 2.0));
          if (dist_int < 3)
          {
            for (int iter = dist_int; iter < 3; iter++)
            {
              grain.neighbordistfunc[dist_int]++;
            }
          }
          if (dist2_int < 3)
          {
            for (int iter = dist2_int; iter < 3; iter++)
            {
              grain_j.neighbordistfunc[dist2_int]++;
            }
          }
        }
      }
    }
  }
}

