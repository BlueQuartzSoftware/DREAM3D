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
AbstractFilter(),
m_GrainIds(NULL),
m_NumCells(NULL),
m_Centroids(NULL),
m_Volumes(NULL),
m_EquivalentDiameters(NULL),
m_Neighborhoods(NULL),
m_Active(NULL)
{

  setupFilterOptions();
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
void FindNeighborhoods::setupFilterOptions()
{
#if 0
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Output Statistics File");
    option->setPropertyName("H5StatsFile");
    option->setWidgetType(FilterOption::OutputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  setFilterOptions(options);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, VoxelData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1);


  GET_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, -304, bool, BoolArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, ss, float, FloatArrayType, fields, 3);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Volumes, ss, float, FloatArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, float,FloatArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, NumCells, ss, int32_t, Int32ArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Neighborhoods, ss, int32_t, Int32ArrayType, fields, 3);


  setErrorMessage(ss.str());
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::preflight()
{
  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::execute()
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

  dataCheck(false, m->totalPoints(), m->getTotalFields(), m->crystruct.size());


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

  int64_t totalPoints = m->totalPoints();
  DECLARE_WRAPPED_ARRAY(graincenters, m_GrainCenters, float); // N x 5 Array
  graincenters = NULL;
  INIT_DataArray(m_GrainCenters,float);

  float x, y, z;
  int col, row, plane;
  float radcubed;
  float diameter;
//  float allvol = 0.0;
  size_t numgrains = m->getTotalFields();
  graincenters = m_GrainCenters->WritePointer(0, numgrains * 5);
  m_GrainCenters->SetNumberOfComponents(5);

  // Initialize every element to 0.0
  for (size_t i = 0; i < numgrains * 5; i++)
  {
    graincenters[i] = 0.0f;
  }
  for (int j = 0; j < totalPoints; j++)
  {
    int gnum = m_GrainIds[j];
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
    m_Centroids[3*i] = graincenters[i*5 + 1];
    m_Centroids[3*i+1] = graincenters[i*5 + 2];
    m_Centroids[3*i+2] = graincenters[i*5 + 3];
    m_NumCells[i] = graincenters[i*5 + 0];
    m_Volumes[i] = (graincenters[i*5 + 0] * res_scalar);
    radcubed = m_Volumes[i]/vol_term;
    diameter = 2.0f*powf(radcubed, 0.3333333333f);
    m_EquivalentDiameters[i] = diameter;
  }
}
void FindNeighborhoods::find_centroids2D()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();
  DECLARE_WRAPPED_ARRAY(graincenters, m_GrainCenters, float); // N x 5 Array
  graincenters = NULL;
  INIT_DataArray(m_GrainCenters,float);

  float x, y;
  int col, row;
  float radsquared;
  float diameter;
  size_t numgrains = m->getTotalFields();
  graincenters = m_GrainCenters->WritePointer(0, numgrains * 5);
  m_GrainCenters->SetNumberOfComponents(5);

  for (size_t i = 0; i < numgrains*5; i++)
  {
      graincenters[i] = 0.0f;
  }
  for (int j = 0; j < totalPoints; j++)
  {
    int gnum = m_GrainIds[j];
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
    m_Centroids[3*i] = graincenters[i*5 + 1];
    m_Centroids[3*i+1] = graincenters[i*5 + 2];
    m_NumCells[i] = graincenters[i*5 + 0];
    m_Volumes[i] = (graincenters[i*5 + 0] * m->getXRes() * m->getYRes());
    radsquared = m_Volumes[i] / m_pi;
    diameter = (2 * sqrt(radsquared));
    m_EquivalentDiameters[i] = diameter;
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
  size_t numgrains = m->getTotalFields();

  for (size_t i = 1; i < numgrains; i++)
  {
    if (m_Active[i] == true)
    {
      x = m_Centroids[3*i];
      y = m_Centroids[3*i+1];
      z = m_Centroids[3*i+2];
      diam = m_EquivalentDiameters[i];
      for (size_t j = i; j < numgrains; j++)
      {
        if (m_Active[j] == true)
        {
		  xn = m_Centroids[3*j];
		  yn = m_Centroids[3*j+1];
		  zn = m_Centroids[3*j+2];
		  diam2 = m_EquivalentDiameters[j];
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
              m_Neighborhoods[3*i+dist_int]++;
            }
          }
          if (dist2_int < 3)
          {
            for (int iter = dist2_int; iter < 3; iter++)
            {
              m_Neighborhoods[3*j+dist2_int]++;
            }
          }
        }
      }
    }
  }
}

