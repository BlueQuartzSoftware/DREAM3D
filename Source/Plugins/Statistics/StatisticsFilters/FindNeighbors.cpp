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

#include "FindNeighbors.h"

#include <QtCore/QDateTime>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"

// Include the MOC generated file for this class
#include "moc_FindNeighbors.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighbors::FindNeighbors() :
  AbstractFilter(),
  m_CellFeatureAttributeMatrixPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, ""),
  m_SharedSurfaceAreaListArrayName(SIMPL::FeatureData::SharedSurfaceAreaList),
  m_NeighborListArrayName(SIMPL::FeatureData::NeighborList),
  m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds),
  m_BoundaryCellsArrayName(SIMPL::CellData::BoundaryCells),
  m_NumNeighborsArrayName(SIMPL::FeatureData::NumNeighbors),
  m_SurfaceFeaturesArrayName(SIMPL::FeatureData::SurfaceFeatures),
  m_StoreBoundaryCells(false),
  m_StoreSurfaceFeatures(false),
  m_FeatureIds(NULL),
  m_BoundaryCells(NULL),
  m_SurfaceFeatures(NULL),
  m_NumNeighbors(NULL)
{
  m_NeighborList = NeighborList<int32_t>::NullPointer();
  m_SharedSurfaceAreaList = NeighborList<float>::NullPointer();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighbors::~FindNeighbors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::setupFilterParameters()
{
  FilterParameterVector parameters;
  QStringList linkedProps("BoundaryCellsArrayName");
  parameters.push_back(LinkedBooleanFilterParameter::New("Store Boundary Cells Array", "StoreBoundaryCells", getStoreBoundaryCells(), linkedProps, FilterParameter::Parameter));
  linkedProps.clear();
  linkedProps << "SurfaceFeaturesArrayName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Store Surface Features Array", "StoreSurfaceFeatures", getStoreSurfaceFeatures(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Ids", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Cell Feature Attribute Matrix", "CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Boundary Cells", "BoundaryCellsArrayName", getBoundaryCellsArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Number of Neighbors", "NumNeighborsArrayName", getNumNeighborsArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Neighbor List", "NeighborListArrayName", getNeighborListArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Shared Surface Area List", "SharedSurfaceAreaListArrayName", getSharedSurfaceAreaListArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Surface Features", "SurfaceFeaturesArrayName", getSurfaceFeaturesArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixPath(reader->readDataArrayPath("CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setBoundaryCellsArrayName(reader->readString("BoundaryCellsArrayName", getBoundaryCellsArrayName() ) );
  setSurfaceFeaturesArrayName(reader->readString("SurfaceFeaturesArrayName", getSurfaceFeaturesArrayName() ) );
  setStoreBoundaryCells(reader->readValue("StoreBoundaryCells", getStoreBoundaryCells() ) );
  setStoreSurfaceFeatures(reader->readValue("StoreSurfaceFeatures", getStoreSurfaceFeatures() ) );
  setNumNeighborsArrayName(reader->readString("NumNeighborsArrayName", getNumNeighborsArrayName() ) );
  setNeighborListArrayName(reader->readString("NeighborListArrayName", getNeighborListArrayName() ) );
  setSharedSurfaceAreaListArrayName(reader->readString("SharedSurfaceAreaListArrayName", getSharedSurfaceAreaListArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindNeighbors::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixPath)
  SIMPL_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(BoundaryCellsArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceFeaturesArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(StoreBoundaryCells)
  SIMPL_FILTER_WRITE_PARAMETER(StoreSurfaceFeatures)
  SIMPL_FILTER_WRITE_PARAMETER(NumNeighborsArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(NeighborListArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(SharedSurfaceAreaListArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::initialize()
{
  m_NeighborList = NeighborList<int32_t>::NullPointer();
  m_SharedSurfaceAreaList = NeighborList<float>::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::dataCheck()
{
  setErrorCondition(0);
  initialize();
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), m_FeatureIdsArrayPath.getAttributeMatrixName(), getBoundaryCellsArrayName() );
  if (m_StoreBoundaryCells == true)
  {
    m_BoundaryCellsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter, int8_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_BoundaryCellsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_BoundaryCells = m_BoundaryCellsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getCellFeatureAttributeMatrixPath(), -301);

  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getNumNeighborsArrayName() );
  m_NumNeighborsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumNeighborsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumNeighbors = m_NumNeighborsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getSurfaceFeaturesArrayName() );
  if (m_StoreSurfaceFeatures == true)
  {
    m_SurfaceFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  // Feature Data
  // Do this whole block FIRST otherwise the side effect is that a call to m->getNumCellFeatureTuples will = 0
  // because we are just creating an empty NeighborList object.
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getNeighborListArrayName() );
  m_NeighborList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */

  // And we do the same for the SharedSurfaceArea list
  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getSharedSurfaceAreaListArrayName() );
  m_SharedSurfaceAreaList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::preflight()
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
void FindNeighbors::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t totalFeatures = m_NumNeighborsPtr.lock()->getNumberOfTuples();

  size_t udims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  int64_t dims[3] =
  {
    static_cast<int64_t>(udims[0]),
    static_cast<int64_t>(udims[1]),
    static_cast<int64_t>(udims[2]),
  };

  int64_t neighpoints[6] = { 0, 0, 0, 0, 0, 0 };
  neighpoints[0] = -dims[0] * dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0] * dims[1];

  int64_t column = 0, row = 0, plane = 0;
  int32_t feature = 0;
  int32_t nnum = 0;
  int8_t onsurf = 0;
  bool good = false;
  int64_t neighbor = 0;

  std::vector<std::vector<int32_t> > neighborlist;
  std::vector<std::vector<float> > neighborsurfacearealist;

  int32_t nListSize = 100;
  neighborlist.resize(totalFeatures);
  neighborsurfacearealist.resize(totalFeatures);

  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;

  for (size_t i = 1; i < totalFeatures; i++)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if (currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Finding Neighbors || Initializing Neighbor Lists || %1% Complete").arg((static_cast<float>(i) / totalFeatures) * 100);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      millis = QDateTime::currentMSecsSinceEpoch();
    }

    if (getCancel() == true) { return; }

    m_NumNeighbors[i] = 0;
    neighborlist[i].resize(nListSize);
    neighborsurfacearealist[i].assign(nListSize, -1.0f);
    if (m_StoreSurfaceFeatures == true) { m_SurfaceFeatures[i] = false; }

  }

  for (size_t j = 0; j < totalPoints; j++)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if (currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Finding Neighbors || Determining Neighbor Lists || %1% Complete").arg((static_cast<float>(j) / totalPoints) * 100);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      millis = QDateTime::currentMSecsSinceEpoch();
    }

    if (getCancel() == true) { return; }

    onsurf = 0;
    feature = m_FeatureIds[j];
    if (feature > 0)
    {
      column = static_cast<int64_t>( j % m->getGeometryAs<ImageGeom>()->getXPoints() );
      row = static_cast<int64_t>( (j / m->getGeometryAs<ImageGeom>()->getXPoints()) % m->getGeometryAs<ImageGeom>()->getYPoints() );
      plane = static_cast<int64_t>( j / (m->getGeometryAs<ImageGeom>()->getXPoints() * m->getGeometryAs<ImageGeom>()->getYPoints()) );
      if (m_StoreSurfaceFeatures == true)
      {
        if ((column == 0 || column == static_cast<int64_t>((m->getGeometryAs<ImageGeom>()->getXPoints() - 1))
             || row == 0 || row == static_cast<int64_t>((m->getGeometryAs<ImageGeom>()->getYPoints()) - 1)
             || plane == 0 || plane == static_cast<int64_t>((m->getGeometryAs<ImageGeom>()->getZPoints() - 1)))
            && m->getGeometryAs<ImageGeom>()->getZPoints() != 1)
        {
          m_SurfaceFeatures[feature] = true;
        }
        if ((column == 0 || column == static_cast<int64_t>((m->getGeometryAs<ImageGeom>()->getXPoints() - 1)) || row == 0 || row == static_cast<int64_t>((m->getGeometryAs<ImageGeom>()->getYPoints() - 1))) && m->getGeometryAs<ImageGeom>()->getZPoints() == 1)
        {
          m_SurfaceFeatures[feature] = true;
        }
      }
      for (int32_t k = 0; k < 6; k++)
      {
        good = true;
        neighbor = static_cast<int64_t>( j + neighpoints[k] );
        if (k == 0 && plane == 0) { good = false; }
        if (k == 5 && plane == (m->getGeometryAs<ImageGeom>()->getZPoints() - 1)) { good = false; }
        if (k == 1 && row == 0) { good = false; }
        if (k == 4 && row == (m->getGeometryAs<ImageGeom>()->getYPoints() - 1)) { good = false; }
        if (k == 2 && column == 0) { good = false; }
        if (k == 3 && column == (m->getGeometryAs<ImageGeom>()->getXPoints() - 1)) { good = false; }
        if (good == true && m_FeatureIds[neighbor] != feature && m_FeatureIds[neighbor] > 0)
        {
          onsurf++;
          nnum = m_NumNeighbors[feature];
          neighborlist[feature].push_back(m_FeatureIds[neighbor]);
          nnum++;
          m_NumNeighbors[feature] = nnum;
        }
      }
    }
    if (m_StoreBoundaryCells == true) { m_BoundaryCells[j] = onsurf; }
  }

  // We do this to create new set of NeighborList objects
  for (size_t i = 1; i < totalFeatures; i++)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if (currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Finding Neighbors || Calculating Surface Areas || %1% Complete").arg(((float)i / totalFeatures) * 100);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      millis = QDateTime::currentMSecsSinceEpoch();
    }

    if(getCancel() == true) { return; }


    QMap<int32_t, int32_t> neighToCount;
    int32_t numneighs = static_cast<int32_t>( neighborlist[i].size() );

    // this increments the voxel counts for each feature
    for (int32_t j = 0; j < numneighs; j++)
    {
      neighToCount[neighborlist[i][j]]++;
    }

    QMap<int32_t, int32_t>::Iterator neighiter = neighToCount.find(0);
    neighToCount.erase(neighiter);
    neighiter = neighToCount.find(-1);
    neighToCount.erase(neighiter);
    // Resize the features neighbor list to zero
    neighborlist[i].resize(0);
    neighborsurfacearealist[i].resize(0);

    for (QMap<int32_t, int32_t>::iterator iter = neighToCount.begin(); iter != neighToCount.end(); ++iter)
    {
      int32_t neigh = iter.key(); // get the neighbor feature
      int32_t number = iter.value(); // get the number of voxels
      float area = float(number) * m->getGeometryAs<ImageGeom>()->getXRes() * m->getGeometryAs<ImageGeom>()->getYRes();

      // Push the neighbor feature id back onto the list so we stay synced up
      neighborlist[i].push_back(neigh);
      neighborsurfacearealist[i].push_back(area);
    }
    m_NumNeighbors[i] = int32_t(neighborlist[i].size());

    // Set the vector for each list into the NeighborList Object
    NeighborList<int32_t>::SharedVectorType sharedNeiLst(new std::vector<int32_t>);
    sharedNeiLst->assign(neighborlist[i].begin(), neighborlist[i].end());
    m_NeighborList.lock()->setList(static_cast<int32_t>(i), sharedNeiLst);

    NeighborList<float>::SharedVectorType sharedSAL(new std::vector<float>);
    sharedSAL->assign(neighborsurfacearealist[i].begin(), neighborsurfacearealist[i].end());
    m_SharedSurfaceAreaList.lock()->setList(static_cast<int32_t>(i), sharedSAL);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindNeighbors::newFilterInstance(bool copyFilterParameters)
{
  FindNeighbors::Pointer filter = FindNeighbors::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighbors::getCompiledLibraryName()
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighbors::getBrandingString()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighbors::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighbors::getGroupName()
{ return SIMPL::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighbors::getSubGroupName()
{ return SIMPL::FilterSubGroups::MorphologicalFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighbors::getHumanLabel()
{ return "Find Feature Neighbors"; }
