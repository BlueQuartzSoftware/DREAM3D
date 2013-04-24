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

#include "FindRadialDist.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"
#include "DREAM3DLib/GenericFilters/FindSurfaceGrains.h"
#include "DREAM3DLib/GenericFilters/FindGrainCentroids.h"
#include "DREAM3DLib/StatisticsFilters/FindSizes.h"

const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindRadialDist::FindRadialDist() :
AbstractFilter(),
m_EquivalentDiametersArrayName(DREAM3D::FieldData::EquivalentDiameters),
m_CentroidsArrayName(DREAM3D::FieldData::Centroids),
m_VolumesArrayName(DREAM3D::FieldData::Volumes),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_SurfaceFieldsArrayName(DREAM3D::FieldData::SurfaceFields),
m_FieldPhases(NULL),
m_EquivalentDiameters(NULL),
m_Volumes(NULL),
m_Centroids(NULL),
m_SurfaceFields(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindRadialDist::~FindRadialDist()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRadialDist::setupFilterParameters()
{

}
// -----------------------------------------------------------------------------
void FindRadialDist::writeFilterParameters(AbstractFilterParametersWriter* writer)
{

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRadialDist::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -304, int32_t, Int32ArrayType, fields, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, SurfaceFields, ss, -302, bool, BoolArrayType, fields, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, ss, -305, float, FloatArrayType, fields, 3)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, Volumes, ss, -302, float, FloatArrayType, fields, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, -302, float, FloatArrayType, fields, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRadialDist::preflight()
{
  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRadialDist::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  find_radialdist();
 notifyStatusMessage("FindRadialDist Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRadialDist::find_radialdist()
{
  VoxelDataContainer* m = getVoxelDataContainer();

  float x, y, z;
  float xn, yn, zn;
  float dist;
  size_t numgrains = m->getNumFieldTuples();

  std::string m_OutputFile = "test.txt";
  std::ofstream outFile;
  outFile.open(m_OutputFile.c_str());

  float orig[3] =
  { 0, 0, 0 };
  m->getOrigin(orig);

  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]), };

  float sizex = dims[0] * m->getXRes();
  float sizey = dims[1] * m->getYRes();
  float sizez = dims[2] * m->getZRes();

  int number = 0;
  float totalvolume = 0;
  std::vector<float> distToSurface(numgrains,-1);
  for (size_t i = 1; i < numgrains; i++)
  {
  if(m_SurfaceFields[i] == false)
  {
    number++;
    totalvolume = totalvolume+m_Volumes[i];
    dist = (m_Centroids[3*i] - orig[0]);
    if(((orig[0]+sizex) - m_Centroids[3*i]) < dist) dist = ((orig[0]+sizex) - m_Centroids[3*i]);
    if((m_Centroids[3*i+1] - orig[1]) < dist) dist = (m_Centroids[3*i+1] - orig[1]);
    if(((orig[1]+sizey) - m_Centroids[3*i+1]) < dist) dist = ((orig[1]+sizey) - m_Centroids[3*i+1]);
    if((m_Centroids[3*i+2] - orig[2]) < dist) dist = (m_Centroids[3*i+2] - orig[2]);
    if(((orig[2]+sizez) - m_Centroids[3*i+2]) < dist) dist = ((orig[2]+sizez) - m_Centroids[3*i+2]);
    distToSurface[i] = dist;
  }
  }
  outFile << number << "	" << totalvolume << std::endl;
  for (size_t i = 1; i < numgrains; i++)
  {
  if(m_SurfaceFields[i] == false)
  {
    x = m_Centroids[3*i];
    y = m_Centroids[3*i+1];
    z = m_Centroids[3*i+2];
    if(m->getXPoints() == 1) x = 0;
    if(m->getYPoints() == 1) y = 0;
    if(m->getZPoints() == 1) z = 0;
    for (size_t j = 1; j < numgrains; j++)
    {
      if(m_SurfaceFields[j] == false && i != j)
      {
        xn = m_Centroids[3*j];
        yn = m_Centroids[3*j+1];
        zn = m_Centroids[3*j+2];
        if(m->getXPoints() == 1) xn = 0;
        if(m->getYPoints() == 1) yn = 0;
        if(m->getZPoints() == 1) zn = 0;
        dist = ((x - xn)*(x - xn))+((y - yn)*(y - yn))+((z - zn)*(z - zn));
        dist = sqrt(dist);
        if(dist < distToSurface[i])
        {
          outFile << (dist/m_EquivalentDiameters[i]) << std::endl;
        }
        if(dist < distToSurface[j])
        {
          outFile << (dist/m_EquivalentDiameters[j]) << std::endl;
        }
      }
    }
  }
  }
}
