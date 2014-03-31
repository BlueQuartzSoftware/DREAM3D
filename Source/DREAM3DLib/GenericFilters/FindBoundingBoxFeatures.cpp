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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundingBoxFeatures::FindBoundingBoxFeatures() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CentroidsArrayName(DREAM3D::FeatureData::Centroids),
  m_Centroids(NULL),
  m_SurfaceFeaturesArrayName(DREAM3D::FeatureData::SurfaceFeatures),
  m_SurfaceFeatures(NULL),
  m_BiasedFeaturesArrayName(DREAM3D::FeatureData::BiasedFeatures),
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
void FindBoundingBoxFeatures::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 3);
  m_CentroidsPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_CentroidsArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_SurfaceFeaturesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<bool>, AbstractFilter>(this, m_SurfaceFeaturesArrayName, -302, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_BiasedFeaturesPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<bool>, AbstractFilter, bool>(this, m_BiasedFeaturesArrayName, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_BiasedFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_BiasedFeatures = m_BiasedFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundingBoxFeatures::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundingBoxFeatures::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  if(m->getXPoints() > 1 && m->getYPoints() > 1 && m->getZPoints() > 1) { find_boundingboxfeatures(); }
  if(m->getXPoints() == 1 || m->getYPoints() == 1 || m->getZPoints() == 1) { find_boundingboxfeatures2D(); }
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundingBoxFeatures::find_boundingboxfeatures()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  size_t size = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
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

  size_t size = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindBoundingBoxFeatures::newFilterInstance(bool copyFilterParameters)
{
  /*
  */
  FindBoundingBoxFeatures::Pointer filter = FindBoundingBoxFeatures::New();
  if(true == copyFilterParameters)
  {
  }
  return filter;
}
