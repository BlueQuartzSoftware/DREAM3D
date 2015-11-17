/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
* Copyright (c) 2015 William Lenthe
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "FindSurfaceFeaturesMask.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "Generic/GenericConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceFeaturesMask::FindSurfaceFeaturesMask() :
  AbstractFilter(),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_MaskArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Mask),
  m_SurfaceFeaturesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::SurfaceFeatures),
  m_FeatureIds(NULL),
  m_SurfaceFeatures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceFeaturesMask::~FindSurfaceFeaturesMask()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeaturesMask::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Ids", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));

    req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Bool, 1, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Sample Mask", "MaskArrayPath", getMaskArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(DREAM3D::AttributeMatrixType::CellFeature);
    req.dcGeometryTypes = QVector<unsigned int>(1, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArrayCreationFilterParameter::New("Surface Features", "SurfaceFeaturesArrayPath", getSurfaceFeaturesArrayPath(), FilterParameter::CreatedArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeaturesMask::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceFeaturesArrayPath(reader->readDataArrayPath("SurfaceFeaturesArrayPath", getSurfaceFeaturesArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSurfaceFeaturesMask::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceFeaturesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(MaskArrayPath)
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeaturesMask::dataCheck()
{
  setErrorCondition(0);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1);
  QVector<DataArrayPath> dataArrayPaths;
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims);
  if( NULL != m_FeatureIdsPtr.lock().get() )
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); }
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getFeatureIdsArrayPath()); }

  m_MaskPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getMaskArrayPath(), cDims);
  if( NULL != m_MaskPtr.lock().get() )
  { m_Mask = m_MaskPtr.lock()->getPointer(0); }
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getMaskArrayPath()); }
  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);

  m_SurfaceFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, getSurfaceFeaturesArrayPath(), false, cDims);
  if( NULL != m_SurfaceFeaturesPtr.lock().get() )
  { m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeaturesMask::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeaturesMask::find_surfacefeatures()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());

  int64_t xPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints());
  int64_t yPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getYPoints());
  int64_t zPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getZPoints());

  int64_t zStride = 0, yStride = 0;
  for (int64_t i = 0; i < zPoints; i++)
  {
    zStride = i * xPoints * yPoints;
    for (int64_t j = 0; j < yPoints; j++)
    {
      yStride = j * xPoints;
      for (int64_t k = 0; k < xPoints; k++)
      {
        int32_t gnum = m_FeatureIds[zStride + yStride + k];
        if( m_SurfaceFeatures[gnum] == false)
        {
          //check if touching box edge
          if(k <= 0) { m_SurfaceFeatures[gnum] = true; }
          if(k >= xPoints - 1) { m_SurfaceFeatures[gnum] = true; }
          if(j <= 0) { m_SurfaceFeatures[gnum] = true; }
          if(j >= yPoints - 1) { m_SurfaceFeatures[gnum] = true; }
          if(i <= 0) { m_SurfaceFeatures[gnum] = true; }
          if(i >= zPoints - 1) { m_SurfaceFeatures[gnum] = true; }
          if(m_SurfaceFeatures[gnum] == false)
          {
            //check if touching false mask
            if(!m_Mask[zStride + yStride + k - 1]) { m_SurfaceFeatures[gnum] = true; }
            else if(!m_Mask[zStride + yStride + k + 1]) { m_SurfaceFeatures[gnum] = true; }
            else if(!m_Mask[zStride + yStride + k - xPoints]) { m_SurfaceFeatures[gnum] = true; }
            else if(!m_Mask[zStride + yStride + k + xPoints]) { m_SurfaceFeatures[gnum] = true; }
            else if(!m_Mask[zStride + yStride + k - (xPoints * yPoints)]) { m_SurfaceFeatures[gnum] = true; }
            else if(!m_Mask[zStride + yStride + k + (xPoints * yPoints)]) { m_SurfaceFeatures[gnum] = true; }
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeaturesMask::find_surfacefeatures2D()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());

  int64_t xPoints = 0, yPoints = 0;

  if (m->getGeometryAs<ImageGeom>()->getXPoints() == 1)
  {
    xPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getYPoints());
    yPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getZPoints());
  }
  if (m->getGeometryAs<ImageGeom>()->getYPoints() == 1)
  {
    xPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints());
    yPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getZPoints());
  }
  if (m->getGeometryAs<ImageGeom>()->getZPoints() == 1)
  {
    xPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints());
    yPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getYPoints());
  }

  int64_t yStride;
  for (int64_t j = 0; j < yPoints; j++)
  {
    yStride = j * xPoints;
    for (int64_t k = 0; k < xPoints; k++)
    {
      int32_t gnum = m_FeatureIds[yStride + k];
      if (m_SurfaceFeatures[gnum] == false)
      {
        //check if touching box edge
        if(k <= 0) { m_SurfaceFeatures[gnum] = true; }
        if(k >= xPoints - 1) { m_SurfaceFeatures[gnum] = true; }
        if(j <= 0) { m_SurfaceFeatures[gnum] = true; }
        if(j >= yPoints - 1) { m_SurfaceFeatures[gnum] = true; }
        if(m_SurfaceFeatures[gnum] == false)
        {
          //check if touching false mask
          if(!m_Mask[yStride + k - 1] == 0) { m_SurfaceFeatures[gnum] = true; }
          else if(!m_Mask[yStride + k + 1]) { m_SurfaceFeatures[gnum] = true; }
          else if(!m_Mask[yStride + k - static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints())]) { m_SurfaceFeatures[gnum] = true; }
          else if(!m_Mask[yStride + k + static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints())]) { m_SurfaceFeatures[gnum] = true; }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeaturesMask::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());

  if(m->getGeometryAs<ImageGeom>()->getXPoints() > 1 && m->getGeometryAs<ImageGeom>()->getYPoints() > 1 && m->getGeometryAs<ImageGeom>()->getZPoints() > 1) { find_surfacefeatures(); }
  if(m->getGeometryAs<ImageGeom>()->getXPoints() == 1 || m->getGeometryAs<ImageGeom>()->getYPoints() == 1 || m->getGeometryAs<ImageGeom>()->getZPoints() == 1) { find_surfacefeatures2D(); }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindSurfaceFeaturesMask::newFilterInstance(bool copyFilterParameters)
{
  FindSurfaceFeaturesMask::Pointer filter = FindSurfaceFeaturesMask::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSurfaceFeaturesMask::getCompiledLibraryName()
{ return GenericConstants::GenericBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSurfaceFeaturesMask::getGroupName()
{ return DREAM3D::FilterGroups::GenericFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSurfaceFeaturesMask::getSubGroupName()
{ return DREAM3D::FilterSubGroups::SpatialFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSurfaceFeaturesMask::getHumanLabel()
{ return "Find Surface Features (Mask)"; }

