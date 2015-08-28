/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#include "FindBoundingBoxFeatures.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/StringFilterParameter.h"
#include "DREAM3DLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"

#include "Generic/GenericConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundingBoxFeatures::FindBoundingBoxFeatures() :
  AbstractFilter(),
  m_CalcByPhase(false),
  m_CentroidsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Centroids),
  m_PhasesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_SurfaceFeaturesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::SurfaceFeatures),
  m_BiasedFeaturesArrayName(DREAM3D::FeatureData::BiasedFeatures),
  m_Phases(NULL),
  m_Centroids(NULL),
  m_SurfaceFeatures(NULL),
  m_BiasedFeatures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundingBoxFeatures::~FindBoundingBoxFeatures()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundingBoxFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;
  QStringList linkedProps("PhasesArrayPath");
  parameters.push_back(LinkedBooleanFilterParameter::New("Apply Phase by Phase", "CalcByPhase", getCalcByPhase(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Float, 3, DREAM3D::AttributeMatrixType::CellFeature, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Centroids", "CentroidsArrayPath", getCentroidsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Bool, 1, DREAM3D::AttributeMatrixType::CellFeature, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Surface Features", "SurfaceFeaturesArrayPath", getSurfaceFeaturesArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::CellFeature, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "PhasesArrayPath", getPhasesArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Biased Features", "BiasedFeaturesArrayName", getBiasedFeaturesArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundingBoxFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCalcByPhase(reader->readValue("CalcByPhase", getCalcByPhase()));
  setPhasesArrayPath(reader->readDataArrayPath("PhasesArrayPath", getPhasesArrayPath()));
  setBiasedFeaturesArrayName(reader->readString("BiasedFeaturesArrayName", getBiasedFeaturesArrayName() ) );
  setSurfaceFeaturesArrayPath(reader->readDataArrayPath("SurfaceFeaturesArrayPath", getSurfaceFeaturesArrayPath() ) );
  setCentroidsArrayPath(reader->readDataArrayPath("CentroidsArrayPath", getCentroidsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindBoundingBoxFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(CalcByPhase)
  DREAM3D_FILTER_WRITE_PARAMETER(PhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(BiasedFeaturesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceFeaturesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CentroidsArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundingBoxFeatures::dataCheck()
{
  setErrorCondition(0);

  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getCentroidsArrayPath().getDataContainerName());

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 3);
  m_CentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCentroidsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getCentroidsArrayPath()); }

  cDims[0] = 1;
  m_SurfaceFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getSurfaceFeaturesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getSurfaceFeaturesArrayPath()); }

  tempPath.update(getCentroidsArrayPath().getDataContainerName(), getCentroidsArrayPath().getAttributeMatrixName(), getBiasedFeaturesArrayName() );
  m_BiasedFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_BiasedFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_BiasedFeatures = m_BiasedFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if (getCalcByPhase() == true)
  {
    m_PhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getPhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_PhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_Phases = m_PhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getPhasesArrayPath()); }
  }

  getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundingBoxFeatures::preflight()
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
void FindBoundingBoxFeatures::find_boundingboxfeatures()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCentroidsArrayPath().getDataContainerName());

  size_t size = m_CentroidsPtr.lock()->getNumberOfTuples();
  float boundbox[7] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, };
  float coords[7] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, };
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
  float dist[7] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, };
  float mindist = std::numeric_limits<float>::max();
  int32_t sidetomove = 0;
  int32_t move = 0;

  // loop first to determine number of phases if calcByPhase is being used
  int32_t numPhases = 1;
  if (m_CalcByPhase == true)
  {
    for (size_t i = 1; i < size; i++)
    {
      if (m_Phases[i] > numPhases) { numPhases = m_Phases[i]; }
    }
  }
  for (int32_t iter = 1; iter <= numPhases; iter++)
  {
    if (m_CalcByPhase == true)
    {
      QString ss = QObject::tr("Working on Phase %1 of %2").arg(iter).arg(numPhases);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    }
    // reset boundbox for each phase
    boundbox[1] = 0;
    boundbox[2] = m->getGeometryAs<ImageGeom>()->getXPoints() * m->getGeometryAs<ImageGeom>()->getXRes();
    boundbox[3] = 0;
    boundbox[4] = m->getGeometryAs<ImageGeom>()->getYPoints() * m->getGeometryAs<ImageGeom>()->getYRes();
    boundbox[5] = 0;
    boundbox[6] = m->getGeometryAs<ImageGeom>()->getZPoints() * m->getGeometryAs<ImageGeom>()->getZRes();
    for (size_t i = 1; i < size; i++)
    {
      if (m_SurfaceFeatures[i] == true && (m_CalcByPhase == false || m_Phases[i] == iter))
      {
        move = 1;
        mindist = std::numeric_limits<float>::max();
        x = m_Centroids[3 * i];
        y = m_Centroids[3 * i + 1];
        z = m_Centroids[3 * i + 2];
        coords[1] = x;
        coords[2] = x;
        coords[3] = y;
        coords[4] = y;
        coords[5] = z;
        coords[6] = z;
        for (int32_t j = 1; j < 7; j++)
        {
          dist[j] = std::numeric_limits<float>::max();
          if (j % 2 == 1)
          {
            if (coords[j] > boundbox[j]) { dist[j] = (coords[j] - boundbox[j]); }
            if (coords[j] <= boundbox[j]) { move = 0; }
          }
          if (j % 2 == 0)
          {
            if (coords[j] < boundbox[j]) { dist[j] = (boundbox[j] - coords[j]); }
            if (coords[j] >= boundbox[j]) { move = 0; }
          }
          if (dist[j] < mindist) { mindist = dist[j], sidetomove = j; }
        }
        if (move == 1) { boundbox[sidetomove] = coords[sidetomove]; }
      }
    }
    for (size_t j = 1; j < size; j++)
    {
      if (m_CalcByPhase == false || m_Phases[j] == iter)
      {
        if (m_Centroids[3 * j] <= boundbox[1])
        { m_BiasedFeatures[j] = true; }
        if (m_Centroids[3 * j] >= boundbox[2])
        { m_BiasedFeatures[j] = true; }
        if (m_Centroids[3 * j + 1] <= boundbox[3])
        { m_BiasedFeatures[j] = true; }
        if (m_Centroids[3 * j + 1] >= boundbox[4])
        { m_BiasedFeatures[j] = true; }
        if (m_Centroids[3 * j + 2] <= boundbox[5])
        { m_BiasedFeatures[j] = true; }
        if (m_Centroids[3 * j + 2] >= boundbox[6])
        { m_BiasedFeatures[j] = true; }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundingBoxFeatures::find_boundingboxfeatures2D()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCentroidsArrayPath().getDataContainerName());

  size_t size = m_CentroidsPtr.lock()->getNumberOfTuples();
  float boundbox[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
  float coords[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
  float x = 0.0f;
  float y = 0.0f;
  float dist[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
  float mindist = std::numeric_limits<float>::max();
  int32_t sidetomove = 0;
  int32_t move = 0;

  int32_t xPoints = 0, yPoints = 0;
  float xRes = 0.0f, yRes = 0.0f;

  if (m->getGeometryAs<ImageGeom>()->getXPoints() == 1)
  {
    xPoints = m->getGeometryAs<ImageGeom>()->getYPoints();
    xRes = m->getGeometryAs<ImageGeom>()->getYRes();
    yPoints = m->getGeometryAs<ImageGeom>()->getZPoints();
    yRes = m->getGeometryAs<ImageGeom>()->getZRes();
  }
  if (m->getGeometryAs<ImageGeom>()->getYPoints() == 1)
  {
    xPoints = m->getGeometryAs<ImageGeom>()->getXPoints();
    xRes = m->getGeometryAs<ImageGeom>()->getXRes();
    yPoints = m->getGeometryAs<ImageGeom>()->getZPoints();
    yRes = m->getGeometryAs<ImageGeom>()->getZRes();
  }
  if (m->getGeometryAs<ImageGeom>()->getZPoints() == 1)
  {
    xPoints = m->getGeometryAs<ImageGeom>()->getXPoints();
    xRes = m->getGeometryAs<ImageGeom>()->getXRes();
    yPoints = m->getGeometryAs<ImageGeom>()->getYPoints();
    yRes = m->getGeometryAs<ImageGeom>()->getYRes();
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
      mindist = std::numeric_limits<float>::max();
      x = m_Centroids[3 * i];
      y = m_Centroids[3 * i + 1];
      coords[1] = x;
      coords[2] = x;
      coords[3] = y;
      coords[4] = y;
      for(int32_t j = 1; j < 5; j++)
      {
        dist[j] = std::numeric_limits<float>::max();
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
void FindBoundingBoxFeatures::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCentroidsArrayPath().getDataContainerName());

  if(m->getGeometryAs<ImageGeom>()->getXPoints() > 1 && m->getGeometryAs<ImageGeom>()->getYPoints() > 1 && m->getGeometryAs<ImageGeom>()->getZPoints() > 1) { find_boundingboxfeatures(); }
  if(m->getGeometryAs<ImageGeom>()->getXPoints() == 1 || m->getGeometryAs<ImageGeom>()->getYPoints() == 1 || m->getGeometryAs<ImageGeom>()->getZPoints() == 1) { find_boundingboxfeatures2D(); }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindBoundingBoxFeatures::newFilterInstance(bool copyFilterParameters)
{
  FindBoundingBoxFeatures::Pointer filter = FindBoundingBoxFeatures::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundingBoxFeatures::getCompiledLibraryName()
{ return GenericConstants::GenericBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundingBoxFeatures::getGroupName()
{ return DREAM3D::FilterGroups::GenericFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundingBoxFeatures::getSubGroupName()
{ return DREAM3D::FilterSubGroups::SpatialFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundingBoxFeatures::getHumanLabel()
{ return "Find Biased Features (Bounding Box)"; }

