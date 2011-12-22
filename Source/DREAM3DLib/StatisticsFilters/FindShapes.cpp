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

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

const static float m_pi = M_PI;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindShapes::FindShapes() :
grain_indicies(NULL)
{
  graincenters = NULL;
  grainmoments = NULL;

  INIT_DataArray(m_GrainCenters,float);
  INIT_DataArray(m_GrainMoments,float);
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
void FindShapes::execute()
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
  GET_NAMED_ARRAY_SIZE_CHK(m, Voxel, DREAM3D::VoxelData::GrainIds, Int32ArrayType, int32_t, (totalPoints), gi);
  grain_indicies = gi;
  setErrorCondition(0);

  if(m->zpoints > 1) find_centroids();
  if(m->zpoints == 1) find_centroids2D();

  if(m->zpoints > 1) find_moments();
  if(m->zpoints == 1) find_moments2D();

  if(m->zpoints > 1) find_axes();
  if(m->zpoints == 1) find_axes2D();

  notify("FindShapes Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_centroids()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();
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
    col = j % m->xpoints;
    row = (j / m->xpoints) % m->ypoints;
    plane = j / (m->xpoints * m->ypoints);
	x = float(col)*m->resx;
	y = float(row)*m->resy;
	z = float(plane)*m->resz;
    graincenters[gnum*5 + 1] = graincenters[gnum*5 + 1] + x;
    graincenters[gnum*5 + 2] = graincenters[gnum*5 + 2] + y;
    graincenters[gnum*5 + 3] = graincenters[gnum*5 + 3] + z;
  }
  float res_scalar = m->resx * m->resy * m->resz;
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
void FindShapes::find_centroids2D()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();

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
    col = j % m->xpoints;
    row = (j / m->xpoints) % m->ypoints;
	x = float(col)*m->resx;
	y = float(row)*m->resy;
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
    m->m_Grains[i]->volume = (graincenters[i*5 + 0] * m->resx * m->resy);
    radsquared = m->m_Grains[i]->volume / m_pi;
    diameter = (2 * sqrt(radsquared));
    m->m_Grains[i]->equivdiameter = diameter;
  }
}

void FindShapes::find_moments()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();

  float u200 = 0;
  float u020 = 0;
  float u002 = 0;
  float u110 = 0;
  float u011 = 0;
  float u101 = 0;
  size_t numgrains = m->m_Grains.size();
  grainmoments = m_GrainMoments->WritePointer(0, numgrains * 6);

  for (size_t i = 0; i < numgrains*6; i++)
  {
      grainmoments[i] = 0.0f;
  }
  for (size_t j = 0; j < totalPoints; j++)
  {
    u200 = 0;
    u020 = 0;
    u002 = 0;
    u110 = 0;
    u011 = 0;
    u101 = 0;
    int gnum = grain_indicies[j];
    float x = find_xcoord(j);
    float y = find_ycoord(j);
    float z = find_zcoord(j);
    float x1 = x + (m->resx / 4);
    float x2 = x - (m->resx / 4);
    float y1 = y + (m->resy / 4);
    float y2 = y - (m->resy / 4);
    float z1 = z + (m->resz / 4);
    float z2 = z - (m->resz / 4);
    float xdist1 = (x1 - graincenters[gnum*5 + 1]);
    float ydist1 = (y1 - graincenters[gnum*5 + 2]);
    float zdist1 = (z1 - graincenters[gnum*5 + 3]);
    float xdist2 = (x1 - graincenters[gnum*5 + 1]);
    float ydist2 = (y1 - graincenters[gnum*5 + 2]);
    float zdist2 = (z2 - graincenters[gnum*5 + 3]);
    float xdist3 = (x1 - graincenters[gnum*5 + 1]);
    float ydist3 = (y2 - graincenters[gnum*5 + 2]);
    float zdist3 = (z1 - graincenters[gnum*5 + 3]);
    float xdist4 = (x1 - graincenters[gnum*5 + 1]);
    float ydist4 = (y2 - graincenters[gnum*5 + 2]);
    float zdist4 = (z2 - graincenters[gnum*5 + 3]);
    float xdist5 = (x2 - graincenters[gnum*5 + 1]);
    float ydist5 = (y1 - graincenters[gnum*5 + 2]);
    float zdist5 = (z1 - graincenters[gnum*5 + 3]);
    float xdist6 = (x2 - graincenters[gnum*5 + 1]);
    float ydist6 = (y1 - graincenters[gnum*5 + 2]);
    float zdist6 = (z2 - graincenters[gnum*5 + 3]);
    float xdist7 = (x2 - graincenters[gnum*5 + 1]);
    float ydist7 = (y2 - graincenters[gnum*5 + 2]);
    float zdist7 = (z1 - graincenters[gnum*5 + 3]);
    float xdist8 = (x2 - graincenters[gnum*5 + 1]);
    float ydist8 = (y2 - graincenters[gnum*5 + 2]);
    float zdist8 = (z2 - graincenters[gnum*5 + 3]);
    u200 = u200 + ((ydist1) * (ydist1)) + ((zdist1) * (zdist1)) + ((ydist2) * (ydist2)) + ((zdist2) * (zdist2)) + ((ydist3) * (ydist3)) + ((zdist3) * (zdist3))
        + ((ydist4) * (ydist4)) + ((zdist4) * (zdist4)) + ((ydist5) * (ydist5)) + ((zdist5) * (zdist5)) + ((ydist6) * (ydist6)) + ((zdist6) * (zdist6))
        + ((ydist7) * (ydist7)) + ((zdist7) * (zdist7)) + ((ydist8) * (ydist8)) + ((zdist8) * (zdist8));
    u020 = u020 + ((xdist1) * (xdist1)) + ((zdist1) * (zdist1)) + ((xdist2) * (xdist2)) + ((zdist2) * (zdist2)) + ((xdist3) * (xdist3)) + ((zdist3) * (zdist3))
        + ((xdist4) * (xdist4)) + ((zdist4) * (zdist4)) + ((xdist5) * (xdist5)) + ((zdist5) * (zdist5)) + ((xdist6) * (xdist6)) + ((zdist6) * (zdist6))
        + ((xdist7) * (xdist7)) + ((zdist7) * (zdist7)) + ((xdist8) * (xdist8)) + ((zdist8) * (zdist8));
    u002 = u002 + ((xdist1) * (xdist1)) + ((ydist1) * (ydist1)) + ((xdist2) * (xdist2)) + ((ydist2) * (ydist2)) + ((xdist3) * (xdist3)) + ((ydist3) * (ydist3))
        + ((xdist4) * (xdist4)) + ((ydist4) * (ydist4)) + ((xdist5) * (xdist5)) + ((ydist5) * (ydist5)) + ((xdist6) * (xdist6)) + ((ydist6) * (ydist6))
        + ((xdist7) * (xdist7)) + ((ydist7) * (ydist7)) + ((xdist8) * (xdist8)) + ((ydist8) * (ydist8));
    u110 = u110 + ((xdist1) * (ydist1)) + ((xdist2) * (ydist2)) + ((xdist3) * (ydist3)) + ((xdist4) * (ydist4)) + ((xdist5) * (ydist5)) + ((xdist6) * (ydist6))
        + ((xdist7) * (ydist7)) + ((xdist8) * (ydist8));
    u011 = u011 + ((ydist1) * (zdist1)) + ((ydist2) * (zdist2)) + ((ydist3) * (zdist3)) + ((ydist4) * (zdist4)) + ((ydist5) * (zdist5)) + ((ydist6) * (zdist6))
        + ((ydist7) * (zdist7)) + ((ydist8) * (zdist8));
    u101 = u101 + ((xdist1) * (zdist1)) + ((xdist2) * (zdist2)) + ((xdist3) * (zdist3)) + ((xdist4) * (zdist4)) + ((xdist5) * (zdist5)) + ((xdist6) * (zdist6))
        + ((xdist7) * (zdist7)) + ((xdist8) * (zdist8));
    grainmoments[gnum*6 + 0] = grainmoments[gnum*6 + 0] + u200;
    grainmoments[gnum*6 + 1] = grainmoments[gnum*6 + 1] + u020;
    grainmoments[gnum*6 + 2] = grainmoments[gnum*6 + 2] + u002;
    grainmoments[gnum*6 + 3] = grainmoments[gnum*6 + 3] + u110;
    grainmoments[gnum*6 + 4] = grainmoments[gnum*6 + 4] + u011;
    grainmoments[gnum*6 + 5] = grainmoments[gnum*6 + 5] + u101;
  }
  float sphere = (2000.0*m_pi*m_pi)/9.0;
  for (size_t i = 1; i < numgrains; i++)
  {
    grainmoments[i*6 + 0] = grainmoments[i*6 + 0] * (m->resx / 2.0) * (m->resy / 2.0) * (m->resz / 2.0);
    grainmoments[i*6 + 1] = grainmoments[i*6 + 1] * (m->resx / 2.0) * (m->resy / 2.0) * (m->resz / 2.0);
    grainmoments[i*6 + 2] = grainmoments[i*6 + 2] * (m->resx / 2.0) * (m->resy / 2.0) * (m->resz / 2.0);
    grainmoments[i*6 + 3] = grainmoments[i*6 + 3] * (m->resx / 2.0) * (m->resy / 2.0) * (m->resz / 2.0);
    grainmoments[i*6 + 4] = grainmoments[i*6 + 4] * (m->resx / 2.0) * (m->resy / 2.0) * (m->resz / 2.0);
    grainmoments[i*6 + 5] = grainmoments[i*6 + 5] * (m->resx / 2.0) * (m->resy / 2.0) * (m->resz / 2.0);
    u200 = (grainmoments[i*6 + 1] + grainmoments[i*6 + 2] - grainmoments[i*6 + 0]) / 2;
    u020 = (grainmoments[i*6 + 0] + grainmoments[i*6 + 2] - grainmoments[i*6 + 1]) / 2;
    u002 = (grainmoments[i*6 + 0] + grainmoments[i*6 + 1] - grainmoments[i*6 + 2]) / 2;
    u110 = -grainmoments[i*6 + 3];
    u011 = -grainmoments[i*6 + 4];
    u101 = -grainmoments[i*6 + 5];
    float o3 = (u200 * u020 * u002) + (2.0 * u110 * u101 * u011) - (u200 * u011 * u011) - (u020 * u101 * u101) - (u002 * u110 * u110);
    float vol5 = m->m_Grains[i]->volume;
    vol5 = powf(vol5, 5);
    float omega3 = vol5 / o3;
    omega3 = omega3 / sphere;
    if (omega3 > 1) omega3 = 1;
	if(vol5 == 0) omega3 = 0;
    m->m_Grains[i]->omega3 = omega3;
  }
}
void FindShapes::find_moments2D()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();
  float u200 = 0;
  float u020 = 0;
  float u110 = 0;
  size_t numgrains = m->m_Grains.size();
  grainmoments = m_GrainMoments->WritePointer(0, numgrains*6);
  m_GrainMoments->SetNumberOfComponents(6);
  for (size_t i = 0; i < numgrains; i++)
  {
      grainmoments[i] = 0.0f;
  }
  for (int j = 0; j < totalPoints; j++)
  {
    u200 = 0;
    u020 = 0;
    u110 = 0;
    int gnum = grain_indicies[j];
    float x = find_xcoord(j);
    float y = find_ycoord(j);
    float x1 = x + (m->resx / 2);
    float x2 = x - (m->resx / 2);
    float y1 = y + (m->resy / 2);
    float y2 = y - (m->resy / 2);
    float xdist1 = (x1 - graincenters[gnum*5 + 1]);
    float ydist1 = (y1 - graincenters[gnum*5 + 2]);
    float xdist2 = (x1 - graincenters[gnum*5 + 1]);
    float ydist2 = (y2 - graincenters[gnum*5 + 2]);
    float xdist3 = (x2 - graincenters[gnum*5 + 1]);
    float ydist3 = (y1 - graincenters[gnum*5 + 2]);
    float xdist4 = (x2 - graincenters[gnum*5 + 1]);
    float ydist4 = (y2 - graincenters[gnum*5 + 2]);
    u200 = u200 + ((ydist1) * (ydist1)) + ((ydist2) * (ydist2)) + ((ydist3) * (ydist3)) + ((ydist4) * (ydist4));
    u020 = u020 + ((xdist1) * (xdist1)) + ((xdist2) * (xdist2)) + ((xdist3) * (xdist3)) + ((xdist4) * (xdist4));
    u110 = u110 + ((xdist1) * (ydist1)) + ((xdist2) * (ydist2)) + ((xdist3) * (ydist3)) + ((xdist4) * (ydist4));
    grainmoments[gnum*6 + 0] = grainmoments[gnum*6 + 0] + u200;
    grainmoments[gnum*6 + 1] = grainmoments[gnum*6 + 1] + u020;
    grainmoments[gnum*6 + 2] = grainmoments[gnum*6 + 2] + u110;
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    grainmoments[i*6 + 0] = grainmoments[i*6 + 0] * (m->resx / 2.0) * (m->resy / 2.0);
    grainmoments[i*6 + 1] = grainmoments[i*6 + 1] * (m->resx / 2.0) * (m->resy / 2.0);
    grainmoments[i*6 + 2] = -grainmoments[i*6 + 2] * (m->resx / 2.0) * (m->resy / 2.0);
  }
}
void FindShapes::find_axes()
{
  DataContainer* m = getDataContainer();
  float I1, I2, I3;
  float Ixx, Iyy, Izz, Ixy, Ixz, Iyz;
  float a, b, c, d, f, g, h;
  float rsquare, r, theta;
  float A, B, C;
  float r1, r2, r3;
  float bovera, covera;
  float value;
  size_t numgrains = m->m_Grains.size();
  for (size_t i = 1; i < numgrains; i++)
  {
    Ixx = grainmoments[i*6+0];
    Iyy = grainmoments[i*6+1];
    Izz = grainmoments[i*6+2];
    Ixy = grainmoments[i*6+3];
    Iyz = grainmoments[i*6+4];
    Ixz = grainmoments[i*6+5];
    a = 1;
    b = -Ixx - Iyy - Izz;
    c = ((Ixx * Izz) + (Ixx * Iyy) + (Iyy * Izz) - (Ixz * Ixz) - (Ixy * Ixy) - (Iyz * Iyz));
    d = ((Ixz * Iyy * Ixz) + (Ixy * Izz * Ixy) + (Iyz * Ixx * Iyz) - (Ixx * Iyy * Izz) - (Ixy * Iyz * Ixz) - (Ixy * Iyz * Ixz));
    f = ((3 * c / a) - ((b / a) * (b / a))) / 3;
    g = ((2 * (b / a) * (b / a) * (b / a)) - (9 * b * c / (a * a)) + (27 * (d / a))) / 27;
    h = (g * g / 4) + (f * f * f / 27);
    rsquare = (g * g / 4) - h;
    r = sqrt(rsquare);
	if(rsquare < 0) r = 0;
    theta = 0;
    if (r == 0)
    {
      theta = 0;
    }
    if (r != 0)
    {
	  value = -g / (2 * r);
	  if(value > 1) value = 1;
	  if(value < -1) value = -1;
      theta = acos(value);
    }
    r1 = 2 * powf(r, 0.33333333333) * cosf(theta / 3) - (b / (3 * a));
    r2 = -powf(r, 0.33333333333) * (cosf(theta / 3) - (1.7320508 * sinf(theta / 3))) - (b / (3 * a));
    r3 = -powf(r, 0.33333333333) * (cosf(theta / 3) + (1.7320508 * sinf(theta / 3))) - (b / (3 * a));
    m->m_Grains[i]->radius1 = r1;
    m->m_Grains[i]->radius2 = r2;
    m->m_Grains[i]->radius3 = r3;
    I1 = (15 * r1) / (4 * m_pi);
    I2 = (15 * r2) / (4 * m_pi);
    I3 = (15 * r3) / (4 * m_pi);
    A = (I1 + I2 - I3) / 2;
    B = (I1 + I3 - I2) / 2;
    C = (I2 + I3 - I1) / 2;
    a = (A * A * A * A) / (B * C);
    a = powf(a, 0.1);
    b = B / A;
    b = sqrt(b) * a;
    c = A / (a * a * a * b);
    bovera = b / a;
    covera = c / a;
	if(A == 0 || B == 0 || C == 0) bovera = 0, covera = 0;
    m->m_Grains[i]->aspectratio1 = bovera;
    m->m_Grains[i]->aspectratio2 = covera;
  }
}
void FindShapes::find_axes2D()
{
  DataContainer* m = getDataContainer();
  float Ixx, Iyy, Ixy;
  size_t numgrains = m->m_Grains.size();
  for (size_t i = 1; i < numgrains; i++)
  {
    Ixx = grainmoments[i*6+0];
    Iyy = grainmoments[i*6+1];
    Ixy = grainmoments[i*6+2];
    float r1 = (Ixx + Iyy) / 2.0 + sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 + (Ixy * Ixy - Ixx * Iyy));
    float r2 = (Ixx + Iyy) / 2.0 - sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 + (Ixy * Ixy - Ixx * Iyy));
    float preterm = 4 / 3.1415926535897;
    preterm = powf(preterm, 0.25);
    float postterm1 = r1 * r1 * r1 / r2;
    float postterm2 = r2 * r2 * r2 / r1;
    postterm1 = powf(postterm1, 0.125);
    postterm2 = powf(postterm2, 0.125);
    r1 = preterm * postterm1;
    r2 = preterm * postterm2;
    m->m_Grains[i]->radius1 = r1;
    m->m_Grains[i]->radius2 = r2;
  }
}
float FindShapes::find_xcoord(size_t index)
{
  DataContainer* m = getDataContainer();
  float x = m->resx * float(index % m->xpoints);
  return x;
}
float FindShapes::find_ycoord(size_t index)
{
  DataContainer* m = getDataContainer();
  float y = m->resy * float((index / m->xpoints) % m->ypoints);
  return y;
}
float FindShapes::find_zcoord(size_t index)
{
  DataContainer* m = getDataContainer();
  float z = m->resz * float(index / (m->xpoints * m->ypoints));
  return z;
}
