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

#include "FindBoundingBoxFeatures.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/FindSurfaceFeatures.h"
#include "DREAM3DLib/GenericFilters/FindFeatureCentroids.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundingBoxFeatures::FindBoundingBoxFeatures() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_CentroidsArrayName(DREAM3D::FeatureData::Centroids),
  m_SurfaceFeaturesArrayName(DREAM3D::FeatureData::SurfaceFeatures),
  m_BiasedFeaturesArrayName(DREAM3D::FeatureData::BiasedFeatures),
  m_Centroids(NULL),
  m_SurfaceFeatures(NULL),
  m_BiasedFeatures(NULL)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundingBoxFeatures::~FindBoundingBoxFeatures()
{
}
// -----------------------------------------------------------------------------
void FindBoundingBoxFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int FindBoundingBoxFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundingBoxFeatures::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 3);
  GET_PREREQ_DATA(m, DREAM3D, CellFeatureData, Centroids, -301, float, FloatArrayType, features, dims)
  if(getErrorCondition() == -301)
  {
    setErrorCondition(0);
    FindFeatureCentroids::Pointer find_featurecentroids = FindFeatureCentroids::New();
    find_featurecentroids->setObservers(this->getObservers());
    find_featurecentroids->setDataContainerArray(getDataContainerArray());
    if(preflight == true) { find_featurecentroids->preflight(); }
    if(preflight == false) { find_featurecentroids->execute(); }
    GET_PREREQ_DATA(m, DREAM3D, CellFeatureData, Centroids, -301, float, FloatArrayType, features, dims)
  }
  dims[0] = 1;
  GET_PREREQ_DATA(m, DREAM3D, CellFeatureData, SurfaceFeatures, -302, bool, BoolArrayType, features, dims)
  if(getErrorCondition() == -302)
  {
    setErrorCondition(0);
    FindSurfaceFeatures::Pointer find_surfacefeatures = FindSurfaceFeatures::New();
    find_surfacefeatures->setObservers(this->getObservers());
    find_surfacefeatures->setDataContainerArray(getDataContainerArray());
    if(preflight == true) { find_surfacefeatures->preflight(); }
    if(preflight == false) { find_surfacefeatures->execute(); }
    GET_PREREQ_DATA(m, DREAM3D, CellFeatureData, SurfaceFeatures, -302, bool, BoolArrayType, features, dims)
  }

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFeatureData, BiasedFeatures, bool, BoolArrayType, false, features, dims)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundingBoxFeatures::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundingBoxFeatures::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if (NULL == m)
  {
    setErrorCondition(-1);
    notifyErrorMessage(QObject::tr("VolumeDataContainer was NULL. Returning from Execute Method for filter %1").arg(getHumanLabel()), getErrorCondition());
    return;
  }
  setErrorCondition(0);

  dataCheck(false, m->getTotalPoints(), m->getNumCellFeatureTuples(),  m->getNumCellEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  if(m->getXPoints() > 1 && m->getYPoints() > 1 && m->getZPoints() > 1) { find_boundingboxfeatures(); }
  if(m->getXPoints() == 1 || m->getYPoints() == 1 || m->getZPoints() == 1) { find_boundingboxfeatures2D(); }

  notifyStatusMessage("FindBoundingBoxFeatures Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundingBoxFeatures::find_boundingboxfeatures()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  size_t size = m->getNumCellFeatureTuples();
  float boundbox[7];
  float coords[7];
  float x, y, z;
  float dist[7];
  float mindist;
  int sidetomove, move;
  boundbox[1] = 0;
  boundbox[2] = m->getXPoints() * m->getXRes();
  boundbox[3] = 0;
  boundbox[4] = m->getYPoints() * m->getYRes();
  boundbox[5] = 0;
  boundbox[6] = m->getZPoints() * m->getZRes();
  for (size_t i = 1; i < size; i++)
  {
    if(m_SurfaceFeatures[i] == true)
    {
      move = 1;
      mindist = 10000000000.0;
      x = m_Centroids[3 * i];
      y = m_Centroids[3 * i + 1];
      z = m_Centroids[3 * i + 2];
      coords[1] = x;
      coords[2] = x;
      coords[3] = y;
      coords[4] = y;
      coords[5] = z;
      coords[6] = z;
      for(int j = 1; j < 7; j++)
      {
        dist[j] = 10000000000.0;
        if(j % 2 == 1)
        {
          if(coords[j] > boundbox[j]) { dist[j] = (coords[j] - boundbox[j]); }
          if(coords[j] <= boundbox[j]) { move = 0; }
        }
        if(j % 2 == 0)
        {
          if(coords[j] < boundbox[j]) { dist[j] = (boundbox[j] - coords[j]); }
          if(coords[j] >= boundbox[j]) { move = 0; }
        }
        if(dist[j] < mindist) { mindist = dist[j], sidetomove = j; }
      }
      if(move == 1) { boundbox[sidetomove] = coords[sidetomove]; }
    }
  }
  for (size_t j = 1; j < size; j++)
  {
    if(m_Centroids[3 * j] <= boundbox[1]) { m_BiasedFeatures[j] = true; }
    if(m_Centroids[3 * j] >= boundbox[2]) { m_BiasedFeatures[j] = true; }
    if(m_Centroids[3 * j + 1] <= boundbox[3]) { m_BiasedFeatures[j] = true; }
    if(m_Centroids[3 * j + 1] >= boundbox[4]) { m_BiasedFeatures[j] = true; }
    if(m_Centroids[3 * j + 2] <= boundbox[5]) { m_BiasedFeatures[j] = true; }
    if(m_Centroids[3 * j + 2] >= boundbox[6]) { m_BiasedFeatures[j] = true; }
  }
}
void FindBoundingBoxFeatures::find_boundingboxfeatures2D()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  size_t size = m->getNumCellFeatureTuples();
  float boundbox[5];
  float coords[5];
  float x, y;
  float dist[5];
  float mindist;
  int sidetomove, move;

  int xPoints = 0, yPoints = 0;
  float xRes = 0.0f, yRes = 0.0f;

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

  boundbox[1] = 0;
  boundbox[2] = xPoints * xRes;
  boundbox[3] = 0;
  boundbox[4] = yPoints * yRes;
  for (size_t i = 1; i < size; i++)
  {
    if(m_SurfaceFeatures[i] == true)
    {
      move = 1;
      mindist = 10000000000.0;
      x = m_Centroids[3 * i];
      y = m_Centroids[3 * i + 1];
      coords[1] = x;
      coords[2] = x;
      coords[3] = y;
      coords[4] = y;
      for(int j = 1; j < 5; j++)
      {
        dist[j] = 10000000000.0;
        if(j % 2 == 1)
        {
          if(coords[j] > boundbox[j]) { dist[j] = (coords[j] - boundbox[j]); }
          if(coords[j] <= boundbox[j]) { move = 0; }
        }
        if(j % 2 == 0)
        {
          if(coords[j] < boundbox[j]) { dist[j] = (boundbox[j] - coords[j]); }
          if(coords[j] >= boundbox[j]) { move = 0; }
        }
        if(dist[j] < mindist) { mindist = dist[j], sidetomove = j; }
      }
      if(move == 1) { boundbox[sidetomove] = coords[sidetomove]; }
    }
  }
  for (size_t j = 1; j < size; j++)
  {
    if(m_Centroids[3 * j] <= boundbox[1]) { m_BiasedFeatures[j] = true; }
    if(m_Centroids[3 * j] >= boundbox[2]) { m_BiasedFeatures[j] = true; }
    if(m_Centroids[3 * j + 1] <= boundbox[3]) { m_BiasedFeatures[j] = true; }
    if(m_Centroids[3 * j + 1] >= boundbox[4]) { m_BiasedFeatures[j] = true; }
  }
}
