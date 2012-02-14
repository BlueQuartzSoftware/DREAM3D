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
#include "DREAM3DLib/StatisticsFilters/FindSizes.h"


const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

FindShapes::FindShapes()  :
AbstractFilter(),
m_GrainIds(NULL),
m_BiasedFields(NULL),
m_Phases(NULL),
m_AxisEulerAngles(NULL),
m_Centroids(NULL),
m_AxisLengths(NULL),
m_Omega3s(NULL),
m_EquivalentDiameters(NULL),
m_AspectRatios(NULL)
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
void FindShapes::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1);

  bool exists = doesPipelineContainFilterBeforeThis(FindSizes::ClassName());
  if (false == exists)
  {
	FindSizes::Pointer find_sizes = FindSizes::New();
	find_sizes->setObservers(this->getObservers());
	find_sizes->setDataContainer(getDataContainer());
	if(preflight == true) find_sizes->preflight();
	if(preflight == false) find_sizes->execute();
  }
  GET_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, -300, float, FloatArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, BiasedFields, ss, -300, bool, BoolArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Phases, ss, -300, int32_t, Int32ArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, ss, float, FloatArrayType, fields, 3);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AxisLengths, ss, float, FloatArrayType, fields, 3);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AxisEulerAngles, ss, float, FloatArrayType, fields, 3);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Omega3s, ss, float, FloatArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AspectRatios, ss, float, FloatArrayType, fields, 2);

  m_StatsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getEnsembleData(DREAM3D::EnsembleData::Statistics).get());
  if(m_StatsDataArray == NULL)
  {
	m_StatsDataArray->fillArrayWithNewStatsData(ensembles);
  }


  setErrorMessage(ss.str());
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::preflight()
{
  dataCheck(true, 1, 1, 1);
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
  setErrorCondition(0);

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  if(m->getZPoints() > 1) find_centroids();
  if(m->getZPoints() == 1) find_centroids2D();

  if(m->getZPoints() > 1) find_moments();
  if(m->getZPoints() == 1) find_moments2D();

  if(m->getZPoints() > 1) find_axes();
  if(m->getZPoints() == 1) find_axes2D();

  notify("FindShapes Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindShapes::find_centroids()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->getTotalPoints();
  float x, y, z;
  int col, row, plane;
//  float allvol = 0.0;
  size_t numgrains = m->getNumFieldTuples();
  graincenters = m_GrainCenters->WritePointer(0, numgrains * 5);
  m_GrainCenters->SetNumberOfComponents(5);

  int xPoints = static_cast<int>(m->getXPoints());
  int yPoints = static_cast<int>(m->getYPoints());
  int zPoints = static_cast<int>(m->getZPoints());

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

  // Initialize every element to 0.0
  for (size_t i = 0; i < numgrains * 5; i++)
  {
    graincenters[i] = 0.0f;
  }
  for (int j = 0; j < totalPoints; j++)
  {
    int gnum = m_GrainIds[j];
    graincenters[gnum * 5 + 0]++;
    col = j % xPoints;
    row = (j / xPoints) % yPoints;
    plane = j / (xPoints * yPoints);
    x = float(col) * xRes;
    y = float(row) * yRes;
    z = float(plane) *zRes;
    graincenters[gnum * 5 + 1] = graincenters[gnum * 5 + 1] + x;
    graincenters[gnum * 5 + 2] = graincenters[gnum * 5 + 2] + y;
    graincenters[gnum * 5 + 3] = graincenters[gnum * 5 + 3] + z;
  }
  float res_scalar = xRes * yRes * zRes;
  float vol_term = (4.0f / 3.0f) * m_pi;
  for (size_t i = 1; i < numgrains; i++)
  {
    graincenters[i * 5 + 1] = graincenters[i * 5 + 1] / graincenters[i * 5 + 0];
    graincenters[i * 5 + 2] = graincenters[i * 5 + 2] / graincenters[i * 5 + 0];
    graincenters[i * 5 + 3] = graincenters[i * 5 + 3] / graincenters[i * 5 + 0];
    m_Centroids[3 * i] = graincenters[i * 5 + 1];
    m_Centroids[3 * i + 1] = graincenters[i * 5 + 2];
    m_Centroids[3 * i + 2] = graincenters[i * 5 + 3];
  }
}
void FindShapes::find_centroids2D()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->getTotalPoints();

  float x, y;
  int col, row;
  size_t numgrains = m->getNumFieldTuples();
  graincenters = m_GrainCenters->WritePointer(0, numgrains * 5);
  m_GrainCenters->SetNumberOfComponents(5);

  int xPoints = static_cast<int>(m->getXPoints());
  int yPoints = static_cast<int>(m->getYPoints());
  int zPoints = static_cast<int>(m->getZPoints());

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

  for (size_t i = 0; i < numgrains*5; i++)
  {
      graincenters[i] = 0.0f;
  }
  for (int j = 0; j < totalPoints; j++)
  {
    int gnum = m_GrainIds[j];
    graincenters[gnum*5 + 0]++;
    col = j % xPoints;
    row = (j / xPoints) % yPoints;
	  x = float(col)*xRes;
	  y = float(row)*yRes;
    graincenters[gnum*5 + 1] = graincenters[gnum*5 + 1] + x;
    graincenters[gnum*5 + 2] = graincenters[gnum*5 + 2] + y;
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    graincenters[i*5 + 1] = graincenters[i*5 + 1] / graincenters[i*5 + 0];
    graincenters[i*5 + 2] = graincenters[i*5 + 2] / graincenters[i*5 + 0];
    m_Centroids[3*i] = graincenters[i*5 + 1];
    m_Centroids[3*i+1] = graincenters[i*5 + 2];

  }
}

void FindShapes::find_moments()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->getTotalPoints();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float u200 = 0;
  float u020 = 0;
  float u002 = 0;
  float u110 = 0;
  float u011 = 0;
  float u101 = 0;
  std::vector<float> avgomega3;
  std::vector<float> sdomega3;
  std::vector<size_t> unbiasedcount;
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();

  grainmoments = m_GrainMoments->WritePointer(0, numgrains * 6);
  avgomega3.resize(numensembles,0);
  sdomega3.resize(numensembles,0);
  unbiasedcount.resize(numensembles,0);

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

  for (size_t i = 0; i < numgrains*6; i++)
  {
      grainmoments[i] = 0.0f;
  }
  for (int64_t j = 0; j < totalPoints; j++)
  {
    u200 = 0;
    u020 = 0;
    u002 = 0;
    u110 = 0;
    u011 = 0;
    u101 = 0;
    int gnum = m_GrainIds[j];
    float x = find_xcoord(j);
    float y = find_ycoord(j);
    float z = find_zcoord(j);
    float x1 = x + (xRes / 4);
    float x2 = x - (xRes / 4);
    float y1 = y + (yRes / 4);
    float y2 = y - (yRes / 4);
    float z1 = z + (zRes / 4);
    float z2 = z - (zRes / 4);
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
  float sphere = (2000.0f*m_pi*m_pi)/9.0f;
  float konst1 =  (xRes / 2.0f) * (yRes / 2.0f) * (zRes / 2.0f);
  float konst2 =  (xRes) * (yRes) * (zRes);
  for (size_t i = 1; i < numgrains; i++)
  {
    grainmoments[i*6 + 0] = grainmoments[i*6 + 0] * konst1;
    grainmoments[i*6 + 1] = grainmoments[i*6 + 1] * konst1;
    grainmoments[i*6 + 2] = grainmoments[i*6 + 2] * konst1;
    grainmoments[i*6 + 3] = grainmoments[i*6 + 3] * konst1;
    grainmoments[i*6 + 4] = grainmoments[i*6 + 4] * konst1;
    grainmoments[i*6 + 5] = grainmoments[i*6 + 5] * konst1;
    u200 = (grainmoments[i*6 + 1] + grainmoments[i*6 + 2] - grainmoments[i*6 + 0]) / 2.0f;
    u020 = (grainmoments[i*6 + 0] + grainmoments[i*6 + 2] - grainmoments[i*6 + 1]) / 2.0f;
    u002 = (grainmoments[i*6 + 0] + grainmoments[i*6 + 1] - grainmoments[i*6 + 2]) / 2.0f;
    u110 = -grainmoments[i*6 + 3];
    u011 = -grainmoments[i*6 + 4];
    u101 = -grainmoments[i*6 + 5];
    float o3 = (u200 * u020 * u002) + (2.0f * u110 * u101 * u011) - (u200 * u011 * u011) - (u020 * u101 * u101) - (u002 * u110 * u110);
    float vol5 = graincenters[i*5 + 0]*konst2;
    vol5 = powf(vol5, 5);
    float omega3 = vol5 / o3;
    omega3 = omega3 / sphere;
    if (omega3 > 1) omega3 = 1;
	if(vol5 == 0) omega3 = 0;
    m_Omega3s[i] = omega3;
	if(m_BiasedFields[i] == false)
	{
		unbiasedcount[m_Phases[i]]++;
		avgomega3[m_Phases[i]] = avgomega3[m_Phases[i]] + m_Omega3s[i];
	}
  }
  for (size_t i = 1; i < numensembles; i++)
  {
	  avgomega3[i] = avgomega3[i]/float(unbiasedcount[i]);
//	  statsDataArray[i]->setGrainSizeAverage(avgomega3[i]);
  }
  for (size_t i = 1; i < numgrains; i++)
  {
	if(m_BiasedFields[i] == false)
	{
		sdomega3[m_Phases[i]] = sdomega3[m_Phases[i]] + ((m_Omega3s[i]-avgomega3[m_Phases[i]])*(m_Omega3s[i]-avgomega3[m_Phases[i]]));
	}
  }
  for (size_t i = 1; i < numensembles; i++)
  {
	  sdomega3[i] = sdomega3[i]/float(unbiasedcount[i]);
	  sdomega3[i] = sqrt(sdomega3[i]);
//	  statsDataArray[i]->setGrainSizeAverage(avgomega3[i]);
  }
}
void FindShapes::find_moments2D()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->getTotalPoints();
  float u200 = 0;
  float u020 = 0;
  float u110 = 0;
  size_t numgrains = m->getNumFieldTuples();
  grainmoments = m_GrainMoments->WritePointer(0, numgrains*6);
  m_GrainMoments->SetNumberOfComponents(6);

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

  for (size_t i = 0; i < numgrains; i++)
  {
      grainmoments[i] = 0.0f;
  }
  for (int j = 0; j < totalPoints; j++)
  {
    u200 = 0;
    u020 = 0;
    u110 = 0;
    int gnum = m_GrainIds[j];
    float x = find_xcoord(j);
    float y = find_ycoord(j);
    float x1 = x + (xRes / 2);
    float x2 = x - (xRes / 2);
    float y1 = y + (yRes / 2);
    float y2 = y - (yRes / 2);
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
  float konst1 = (xRes / 2.0f) * (yRes / 2.0f);
  for (size_t i = 1; i < numgrains; i++)
  {
    grainmoments[i*6 + 0] = grainmoments[i*6 + 0] * konst1;
    grainmoments[i*6 + 1] = grainmoments[i*6 + 1] * konst1;
    grainmoments[i*6 + 2] = -grainmoments[i*6 + 2] * konst1;
  }
}
void FindShapes::find_axes()
{
  DataContainer* m = getDataContainer();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float I1, I2, I3;
  float Ixx, Iyy, Izz, Ixy, Ixz, Iyz;
  float a, b, c, d, f, g, h;
  float rsquare, r, theta;
  float A, B, C;
  float r1, r2, r3;
  float bovera, covera;
  float value;
  size_t numgrains = m->getNumFieldTuples();
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
    f = ((3 * c / a) - ((b / a) * (b / a))) / 3.0f;
    g = ((2 * (b / a) * (b / a) * (b / a)) - (9.0f * b * c / (a * a)) + (27.0f * (d / a))) / 27.0f;
    h = (g * g / 4.0f) + (f * f * f / 27.0f);
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
	  value = -g / (2.0f * r);
	  if(value > 1) value = 1.0f;
	  if(value < -1) value = -1.0f;
      theta = acos(value);
    }
    float const1 = powf(r, 0.33333333333f);
    float const2 = cosf(theta / 3.0f);
    float const3 = b / (3.0f * a);
    float const4 = 1.7320508f * sinf(theta / 3.0f);

    r1 = 2 * const1 * const2 - (const3);
    r2 = -const1 * (const2 - (const4)) - const3;
    r3 = -const1 * (const2 + (const4)) - const3;
    m_AxisLengths[3*i] = r1;
    m_AxisLengths[3*i+1] = r2;
    m_AxisLengths[3*i+2] = r3;
    I1 = (15 * r1) / (4 * m_pi);
    I2 = (15 * r2) / (4 * m_pi);
    I3 = (15 * r3) / (4 * m_pi);
    A = (I1 + I2 - I3) / 2;
    B = (I1 + I3 - I2) / 2;
    C = (I2 + I3 - I1) / 2;
    a = (A * A * A * A) / (B * C);
    a = powf(a, 0.1f);
    b = B / A;
    b = sqrt(b) * a;
    c = A / (a * a * a * b);
    bovera = b / a;
    covera = c / a;
	if(A == 0 || B == 0 || C == 0) bovera = 0, covera = 0;
    m_AspectRatios[2*i] = bovera;
    m_AspectRatios[2*i+1] = covera;
  }
}
void FindShapes::find_axes2D()
{
  DataContainer* m = getDataContainer();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float Ixx, Iyy, Ixy;
  size_t numgrains = m->getNumFieldTuples();
  for (size_t i = 1; i < numgrains; i++)
  {
    Ixx = grainmoments[i*6+0];
    Iyy = grainmoments[i*6+1];
    Ixy = grainmoments[i*6+2];
    float r1 = (Ixx + Iyy) / 2.0f + sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0f + (Ixy * Ixy - Ixx * Iyy));
    float r2 = (Ixx + Iyy) / 2.0f - sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0f + (Ixy * Ixy - Ixx * Iyy));
    float preterm = 4.0f / m_pi;
    preterm = powf(preterm, 0.25f);
    float postterm1 = r1 * r1 * r1 / r2;
    float postterm2 = r2 * r2 * r2 / r1;
    postterm1 = powf(postterm1, 0.125f);
    postterm2 = powf(postterm2, 0.125f);
    r1 = preterm * postterm1;
    r2 = preterm * postterm2;
    m_AxisLengths[3*i] = r1;
    m_AxisLengths[3*i+1] = r2;
	m_AspectRatios[2*i] = r2/r1;
  }
}
float FindShapes::find_xcoord(size_t index)
{
  DataContainer* m = getDataContainer();
  float x = m->getXRes() * float(index % m->getXPoints());
  return x;
}
float FindShapes::find_ycoord(size_t index)
{
  DataContainer* m = getDataContainer();
  float y = m->getYRes() * float((index / m->getXPoints()) % m->getYPoints());
  return y;
}
float FindShapes::find_zcoord(size_t index)
{
  DataContainer* m = getDataContainer();
  float z = m->getZRes() * float(index / (m->getXPoints() * m->getYPoints()));
  return z;
}

void FindShapes::find_axiseulers()
{
  DataContainer* m = getDataContainer();
  size_t numgrains = m->getNumFieldTuples();
  for (size_t i = 1; i < numgrains; i++)
  {
    float Ixx = grainmoments[i*6+0];
    float Iyy = grainmoments[i*6+1];
    float Izz = grainmoments[i*6+2];
    float Ixy = grainmoments[i*6+3];
    float Iyz = grainmoments[i*6+4];
    float Ixz = grainmoments[i*6+5];
    float radius1 = m_AxisLengths[3*i];
    float radius2 = m_AxisLengths[3*i+1];
    float radius3 = m_AxisLengths[3*i+2];
    float e[3][1];
    float uber[3][3];
    float bmat[3][1];
    float vect[3][3];
    e[0][0] = radius1;
    e[1][0] = radius2;
    e[2][0] = radius3;
    bmat[0][0] = 0.0000001f;
    bmat[1][0] = 0.0000001f;
    bmat[2][0] = 0.0000001f;
    for (int j = 0; j < 3; j++)
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
      float **uberelim;
      float **uberbelim;
      uberelim = new float *[3];
      uberbelim = new float *[3];
      for (int d = 0; d < 3; d++)
      {
        uberelim[d] = new float[3];
        uberbelim[d] = new float[1];
      }
      int elimcount = 0;
      int elimcount1 = 0;
      float q = 0;
      float sum = 0;
      float c = 0;
      for (int a = 0; a < 3; a++)
      {
        elimcount1 = 0;
        for (int b = 0; b < 3; b++)
        {
          uberelim[elimcount][elimcount1] = uber[a][b];
          elimcount1++;
        }
        uberbelim[elimcount][0] = bmat[a][0];
        elimcount++;
      }
      for (int k = 0; k < elimcount - 1; k++)
      {
        for (int l = k + 1; l < elimcount; l++)
        {
          c = uberelim[l][k] / uberelim[k][k];
          for (int r = k + 1; r < elimcount; r++)
          {
            uberelim[l][r] = uberelim[l][r] - c * uberelim[k][r];
          }
          uberbelim[l][0] = uberbelim[l][0] - c * uberbelim[k][0];
        }
      }
      uberbelim[elimcount - 1][0] = uberbelim[elimcount - 1][0] / uberelim[elimcount - 1][elimcount - 1];
      for (int l = 1; l < elimcount; l++)
      {
        int r = (elimcount - 1) - l;
        sum = 0;
        for (int n = r + 1; n < elimcount; n++)
        {
          sum = sum + (uberelim[r][n] * uberbelim[n][0]);
        }
        uberbelim[r][0] = (uberbelim[r][0] - sum) / uberelim[r][r];
      }
      for (int p = 0; p < elimcount; p++)
      {
        q = uberbelim[p][0];
        vect[j][p] = q;
      }
      for (int d = 0; d < 3; d++)
      {
        delete uberelim[d];
        delete uberbelim[d];
      }
      delete uberelim;
      delete uberbelim;
    }
    float n1x = vect[0][0];
    float n1y = vect[0][1];
    float n1z = vect[0][2];
    float n2x = vect[1][0];
    float n2y = vect[1][1];
    float n2z = vect[1][2];
    float n3x = vect[2][0];
    float n3y = vect[2][1];
    float n3z = vect[2][2];
    float norm1 = sqrt(((n1x * n1x) + (n1y * n1y) + (n1z * n1z)));
    float norm2 = sqrt(((n2x * n2x) + (n2y * n2y) + (n2z * n2z)));
    float norm3 = sqrt(((n3x * n3x) + (n3y * n3y) + (n3z * n3z)));
    n1x = n1x / norm1;
    n1y = n1y / norm1;
    n1z = n1z / norm1;
    n2x = n2x / norm2;
    n2y = n2y / norm2;
    n2z = n2z / norm2;
    n3x = n3x / norm3;
    n3y = n3y / norm3;
    n3z = n3z / norm3;
    float ea2 = acos(n3z);
    float cosine3 = (n3y / sinf(ea2));
    float sine3 = (n3x / sinf(ea2));
    float cosine1 = (-n2z / sinf(ea2));
    float sine1 = (n1z / sinf(ea2));
    float ea3 = acos(cosine3);
    float ea1 = acos(cosine1);
    if (sine3 < 0) ea3 = (2 * m_pi) - ea3;
    if (sine1 < 0) ea1 = (2 * m_pi) - ea1;
    m_AxisEulerAngles[3*i] = ea1;
    m_AxisEulerAngles[3*i+1] = ea2;
    m_AxisEulerAngles[3*i+2] = ea3;
  }
}

void FindShapes::find_axiseulers2D()
{
  DataContainer* m = getDataContainer();
  size_t numgrains = m->getNumFieldTuples();

  for (size_t i = 1; i < numgrains; i++)
  {
    float Ixx = grainmoments[i*6+0];
    float Iyy = grainmoments[i*6+1];
    float Ixy = grainmoments[i*6+2];
    float I1 = (Ixx + Iyy) / 2.0f + sqrtf(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0f + (Ixy * Ixy - Ixx * Iyy));
    float I2 = (Ixx + Iyy) / 2.0f - sqrtf(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0f + (Ixy * Ixy - Ixx * Iyy));
    float n1x = (Ixx - I1) / Ixy;
    float n1y = 1;
    float n2x = (Ixx - I2) / Ixy;
    float n2y = 1;
    float norm1 = sqrtf((n1x * n1x + n1y * n1y));
    float norm2 = sqrtf((n2x * n2x + n2y * n2y));
    n1x = n1x / norm1;
    n1y = n1y / norm1;
    n2x = n2x / norm2;
    n2y = n2y / norm2;
    float cosine1 = n1x;
    float ea1 = acosf(cosine1);
    if (ea1 > m_pi) ea1 = ea1 - m_pi;
    m_AxisEulerAngles[3*i] = ea1;
    m_AxisEulerAngles[3*i+1] = 0.0f;
    m_AxisEulerAngles[3*i+2] = 0.0f;
  }
}

