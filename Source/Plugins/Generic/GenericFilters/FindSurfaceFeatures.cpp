/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "FindSurfaceFeatures.h"

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Generic/GenericConstants.h"
#include "Generic/GenericVersion.h"

// Include the MOC generated file for this class
#include "moc_FindSurfaceFeatures.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceFeatures::FindSurfaceFeatures()
: AbstractFilter()
, m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_SurfaceFeaturesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::SurfaceFeatures)
, m_FeatureIds(nullptr)
, m_SurfaceFeatures(nullptr)
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
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, FindSurfaceFeatures, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Category::Feature);
    req.dcGeometryTypes = IGeometry::Types(1, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Surface Features", SurfaceFeaturesArrayPath, FilterParameter::CreatedArray, FindSurfaceFeatures, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceFeaturesArrayPath(reader->readDataArrayPath("SurfaceFeaturesArrayPath", getSurfaceFeaturesArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeatures::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeatures::dataCheck()
{
  setErrorCondition(0);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_SurfaceFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(
      this, getSurfaceFeaturesArrayPath(), false, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceFeaturesPtr.lock().get())        /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeatures::preflight()
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
void FindSurfaceFeatures::find_surfacefeatures()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());

  int64_t xPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints());
  int64_t yPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getYPoints());
  int64_t zPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getZPoints());

  int64_t zStride = 0, yStride = 0;
  for(int64_t i = 0; i < zPoints; i++)
  {
    zStride = i * xPoints * yPoints;
    for(int64_t j = 0; j < yPoints; j++)
    {
      yStride = j * xPoints;
      for(int64_t k = 0; k < xPoints; k++)
      {
        int32_t gnum = m_FeatureIds[zStride + yStride + k];
        if(m_SurfaceFeatures[gnum] == false)
        {
          if(k <= 0)
          {
            m_SurfaceFeatures[gnum] = true;
          }
          if(k >= xPoints - 1)
          {
            m_SurfaceFeatures[gnum] = true;
          }
          if(j <= 0)
          {
            m_SurfaceFeatures[gnum] = true;
          }
          if(j >= yPoints - 1)
          {
            m_SurfaceFeatures[gnum] = true;
          }
          if(i <= 0)
          {
            m_SurfaceFeatures[gnum] = true;
          }
          if(i >= zPoints - 1)
          {
            m_SurfaceFeatures[gnum] = true;
          }
          if(m_SurfaceFeatures[gnum] == false)
          {
            if(m_FeatureIds[zStride + yStride + k - 1] == 0)
            {
              m_SurfaceFeatures[gnum] = true;
            }
            if(m_FeatureIds[zStride + yStride + k + 1] == 0)
            {
              m_SurfaceFeatures[gnum] = true;
            }
            if(m_FeatureIds[zStride + yStride + k - xPoints] == 0)
            {
              m_SurfaceFeatures[gnum] = true;
            }
            if(m_FeatureIds[zStride + yStride + k + xPoints] == 0)
            {
              m_SurfaceFeatures[gnum] = true;
            }
            if(m_FeatureIds[zStride + yStride + k - (xPoints * yPoints)] == 0)
            {
              m_SurfaceFeatures[gnum] = true;
            }
            if(m_FeatureIds[zStride + yStride + k + (xPoints * yPoints)] == 0)
            {
              m_SurfaceFeatures[gnum] = true;
            }
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeatures::find_surfacefeatures2D()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());

  int64_t xPoints = 0, yPoints = 0;

  if(m->getGeometryAs<ImageGeom>()->getXPoints() == 1)
  {
    xPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getYPoints());
    yPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getZPoints());
  }
  if(m->getGeometryAs<ImageGeom>()->getYPoints() == 1)
  {
    xPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints());
    yPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getZPoints());
  }
  if(m->getGeometryAs<ImageGeom>()->getZPoints() == 1)
  {
    xPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints());
    yPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getYPoints());
  }

  int64_t yStride;
  for(int64_t j = 0; j < yPoints; j++)
  {
    yStride = j * xPoints;
    for(int64_t k = 0; k < xPoints; k++)
    {
      int32_t gnum = m_FeatureIds[yStride + k];
      if(m_SurfaceFeatures[gnum] == false)
      {
        if(k <= 0)
        {
          m_SurfaceFeatures[gnum] = true;
        }
        if(k >= xPoints - 1)
        {
          m_SurfaceFeatures[gnum] = true;
        }
        if(j <= 0)
        {
          m_SurfaceFeatures[gnum] = true;
        }
        if(j >= yPoints - 1)
        {
          m_SurfaceFeatures[gnum] = true;
        }
        if(m_SurfaceFeatures[gnum] == false)
        {
          if(m_FeatureIds[yStride + k - 1] == 0)
          {
            m_SurfaceFeatures[gnum] = true;
          }
          if(m_FeatureIds[yStride + k + 1] == 0)
          {
            m_SurfaceFeatures[gnum] = true;
          }
          if(m_FeatureIds[yStride + k - static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints())] == 0)
          {
            m_SurfaceFeatures[gnum] = true;
          }
          if(m_FeatureIds[yStride + k + static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints())] == 0)
          {
            m_SurfaceFeatures[gnum] = true;
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceFeatures::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());

  if(m->getGeometryAs<ImageGeom>()->getXPoints() > 1 && m->getGeometryAs<ImageGeom>()->getYPoints() > 1 && m->getGeometryAs<ImageGeom>()->getZPoints() > 1)
  {
    find_surfacefeatures();
  }
  if(m->getGeometryAs<ImageGeom>()->getXPoints() == 1 || m->getGeometryAs<ImageGeom>()->getYPoints() == 1 || m->getGeometryAs<ImageGeom>()->getZPoints() == 1)
  {
    find_surfacefeatures2D();
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindSurfaceFeatures::newFilterInstance(bool copyFilterParameters)
{
  FindSurfaceFeatures::Pointer filter = FindSurfaceFeatures::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSurfaceFeatures::getCompiledLibraryName()
{
  return GenericConstants::GenericBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSurfaceFeatures::getBrandingString()
{
  return "Generic";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSurfaceFeatures::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << Generic::Version::Major() << "." << Generic::Version::Minor() << "." << Generic::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSurfaceFeatures::getGroupName()
{
  return SIMPL::FilterGroups::GenericFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSurfaceFeatures::getSubGroupName()
{
  return SIMPL::FilterSubGroups::SpatialFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSurfaceFeatures::getHumanLabel()
{
  return "Find Surface Features";
}
