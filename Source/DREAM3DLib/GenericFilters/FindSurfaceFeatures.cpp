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

#include "FindSurfaceFeatures.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceFeatures::FindSurfaceFeatures() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
/*[]*/m_FeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_SurfaceFeaturesArrayName(DREAM3D::FeatureData::SurfaceFeatures),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_SurfaceFeatures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceFeatures::~FindSurfaceFeatures()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
/*[]*/parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
/*##*/parameters.push_back(FilterParameter::New("SurfaceFeatures", "SurfaceFeaturesArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindSurfaceFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
/*[]*/setSurfaceFeaturesArrayName(reader->readString("SurfaceFeaturesArrayName", getSurfaceFeaturesArrayName() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSurfaceFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
/*[]*/writer->writeValue("SurfaceFeaturesArrayName", getSurfaceFeaturesArrayName() );
  writer->writeValue("FeatureIdsArrayPath", getFeatureIdsArrayPath() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeatures::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -302);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Data
  m_SurfaceFeaturesPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<bool>, AbstractFilter, bool>(this, m_SurfaceFeaturesArrayName, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getSurfaceFeaturesArrayName() );
////==>MIKE_GROEBER_FIX m_SurfaceFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeatures::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeatures::execute()
{
  setErrorCondition(0);
  QString ss;

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  if(m->getXPoints() > 1 && m->getYPoints() > 1 && m->getZPoints() > 1) { find_surfacefeatures(); }
  if(m->getXPoints() == 1 || m->getYPoints() == 1 || m->getZPoints() == 1) { find_surfacefeatures2D(); }

  notifyStatusMessage(getHumanLabel(), "Complete");

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeatures::find_surfacefeatures()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  size_t xPoints = m->getXPoints();
  size_t yPoints = m->getYPoints();
  size_t zPoints = m->getZPoints();

  int zStride, yStride;
  for(size_t i = 0; i < zPoints; i++)
  {
    zStride = i * xPoints * yPoints;
    for (size_t j = 0; j < yPoints; j++)
    {
      yStride = j * xPoints;
      for(size_t k = 0; k < xPoints; k++)
      {
        int gnum = m_FeatureIds[zStride + yStride + k];
        if(m_SurfaceFeatures[gnum] == false)
        {
          if(k <= 0) { m_SurfaceFeatures[gnum] = true; }
          if(k >= xPoints - 1) { m_SurfaceFeatures[gnum] = true; }
          if(j <= 0) { m_SurfaceFeatures[gnum] = true; }
          if(j >= yPoints - 1) { m_SurfaceFeatures[gnum] = true; }
          if(i <= 0) { m_SurfaceFeatures[gnum] = true; }
          if(i >= zPoints - 1) { m_SurfaceFeatures[gnum] = true; }
          if(m_SurfaceFeatures[gnum] == false)
          {
            if(m_FeatureIds[zStride + yStride + k - 1] == 0) { m_SurfaceFeatures[gnum] = true; }
            if(m_FeatureIds[zStride + yStride + k + 1] == 0) { m_SurfaceFeatures[gnum] = true; }
            if(m_FeatureIds[zStride + yStride + k - xPoints] == 0) { m_SurfaceFeatures[gnum] = true; }
            if(m_FeatureIds[zStride + yStride + k + xPoints] == 0) { m_SurfaceFeatures[gnum] = true; }
            if(m_FeatureIds[zStride + yStride + k - (xPoints * yPoints)] == 0) { m_SurfaceFeatures[gnum] = true; }
            if(m_FeatureIds[zStride + yStride + k + (xPoints * yPoints)] == 0) { m_SurfaceFeatures[gnum] = true; }
          }
        }
      }
    }
  }
}
void FindSurfaceFeatures::find_surfacefeatures2D()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

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

  int yStride;
  for (int j = 0; j < yPoints; j++)
  {
    yStride = j * xPoints;
    for(int k = 0; k < xPoints; k++)
    {
      int gnum = m_FeatureIds[yStride + k];
      if(m_SurfaceFeatures[gnum] == false)
      {
        if(k <= 0) { m_SurfaceFeatures[gnum] = true; }
        if(k >= xPoints - 1) { m_SurfaceFeatures[gnum] = true; }
        if(j <= 0) { m_SurfaceFeatures[gnum] = true; }
        if(j >= yPoints - 1) { m_SurfaceFeatures[gnum] = true; }
        if(m_SurfaceFeatures[gnum] == false)
        {
          if(m_FeatureIds[yStride + k - 1] == 0) { m_SurfaceFeatures[gnum] = true; }
          if(m_FeatureIds[yStride + k + 1] == 0) { m_SurfaceFeatures[gnum] = true; }
          if(m_FeatureIds[yStride + k - m->getXPoints()] == 0) { m_SurfaceFeatures[gnum] = true; }
          if(m_FeatureIds[yStride + k + m->getXPoints()] == 0) { m_SurfaceFeatures[gnum] = true; }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindSurfaceFeatures::newFilterInstance(bool copyFilterParameters)
{
  FindSurfaceFeatures::Pointer filter = FindSurfaceFeatures::New();
  if(true == copyFilterParameters)
  {
    filter->setSurfaceFeaturesArrayName(getSurfaceFeaturesArrayName());
    filter->setFeatureIdsArrayPath(getFeatureIdsArrayPath());
  }
  return filter;
}
