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

  find_boundingbox();
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


  int numbins = 40;
  int number = 0;
  float totalvolume = 0;
  float largestESD = 0;
  float largestDistToSurface = 0;
  std::vector<float> distToSurface(numgrains,-1);
  for (size_t i = 1; i < numgrains; i++)
  {

 if(m_SurfaceFields[i] == false)
 {
  number++;
  totalvolume = totalvolume+m_Volumes[i];
  if(m_EquivalentDiameters[i] > largestESD) largestESD = m_EquivalentDiameters[i];
  dist = (m_Centroids[3*i] - boundbox[1]);
  if((boundbox[2] - m_Centroids[3*i]) < dist) dist = (boundbox[2] - m_Centroids[3*i]);
  if((m_Centroids[3*i+1] - boundbox[3]) < dist) dist = (m_Centroids[3*i+1] - boundbox[3]);
  if((boundbox[4] - m_Centroids[3*i+1]) < dist) dist = (boundbox[4] - m_Centroids[3*i+1]);
  if((m_Centroids[3*i+2] - boundbox[5]) < dist) dist = (m_Centroids[3*i+2] - boundbox[5]);
  if((boundbox[6] - m_Centroids[3*i+2]) < dist) dist = (boundbox[6] - m_Centroids[3*i+2]);
  distToSurface[i] = dist;
  if(dist > largestDistToSurface) largestDistToSurface = dist;
 }

  }
  float binSize = largestDistToSurface/float(numbins);
  float avgDensity = float(number)/totalvolume;
  std::vector<std::vector<float> > count(int(largestESD)+1);
  std::vector<std::vector<float> > volume(int(largestESD)+1);
  for (size_t i = 0; i < count.size(); i++)
  {
   count[i].resize(numbins,0);
   volume[i].resize(numbins,0);
  }
  for (size_t i = 1; i < numgrains; i++)
  {

 if(m_SurfaceFields[i] == false)
 {
  for(int j = 0; j < numbins; j++)
  {
   if(j < int(distToSurface[i]/binSize))
   {
    volume[int(m_EquivalentDiameters[i])][j] = volume[int(m_EquivalentDiameters[i])][j] + ((4.0/3.0)*m_pi*float((j+1)*binSize)*float((j+1)*binSize)*float((j+1)*binSize)) - ((4.0/3.0)*m_pi*float(j*binSize)*float(j*binSize)*float(j*binSize));
   }
  }
  x = m_Centroids[3*i];
  y = m_Centroids[3*i+1];
  z = m_Centroids[3*i+2];
  for (size_t j = 1; j < numgrains; j++)
  {
   if(m_SurfaceFields[j] == false && i != j)
   {
     xn = m_Centroids[3*j];
     yn = m_Centroids[3*j+1];
     zn = m_Centroids[3*j+2];
     dist = ((x - xn)*(x - xn))+((y - yn)*(y - yn))+((z - zn)*(z - zn));
     dist = sqrt(dist);
     if(int(dist/binSize) < int(distToSurface[i]/binSize))
     {
      count[int(m_EquivalentDiameters[i])][int(dist/binSize)]++;
     }
     //if(dist < distToSurface[j])
     //{
     // count[int(m_EquivalentDiameters[j])][int(dist/m_EquivalentDiameters[j])]++;
     //}
   }
  }
 }

  }
  for (size_t i = 0; i < numbins; i++)
  {
   outFile << float(i)*binSize << " ";
  }
  outFile << std::endl;
  for (size_t i = 0; i < count.size(); i++)
  {
   for (size_t j = 0; j < numbins; j++)
   {
    float value = (count[i][j]/volume[i][j])/avgDensity;
    if(count[i][j] == 0) value = 0;
    outFile << value << " "; 
   }
   outFile << std::endl;
  }
}
void FindRadialDist::find_boundingbox()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  size_t size = m->getNumFieldTuples();

  float coords[7];
  float x, y, z;
  float dist[7];
  float mindist;
  int sidetomove, move;
  boundbox[1] = 0;
  boundbox[2] = m->getXPoints()*m->getXRes();
  boundbox[3] = 0;
  boundbox[4] = m->getYPoints()*m->getYRes();
  boundbox[5] = 0;
  boundbox[6] = m->getZPoints()*m->getZRes();
  for (size_t i = 1; i < size; i++)
  {
   if(m_SurfaceFields[i] == true)
   {
    move = 1;
    mindist = 10000000000.0;
    x = m_Centroids[3*i];
    y = m_Centroids[3*i+1];
    z = m_Centroids[3*i+2];
    coords[1] = x;
    coords[2] = x;
    coords[3] = y;
    coords[4] = y;
    coords[5] = z;
    coords[6] = z;
    for(int j=1;j<7;j++)
    {
      dist[j] = 10000000000.0;
   if(j%2 == 1)
   {
    if(coords[j] > boundbox[j]) dist[j] = (coords[j]-boundbox[j]);
    if(coords[j] <= boundbox[j]) move = 0;
   }
   if(j%2 == 0)
   {
    if(coords[j] < boundbox[j]) dist[j] = (boundbox[j]-coords[j]);
    if(coords[j] >= boundbox[j]) move = 0;
   }
   if(dist[j] < mindist) mindist = dist[j], sidetomove = j;
    }
    if(move == 1)
    {
      boundbox[sidetomove] = coords[sidetomove];
    }
   }
  }

}
