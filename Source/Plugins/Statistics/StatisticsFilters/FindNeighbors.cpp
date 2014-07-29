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

#include "FindNeighbors.h"

#include <sstream>

#include <QtCore/QDateTime>

#include "DREAM3DLib/Common/Constants.h"

#include "StatisticsConstants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighbors::FindNeighbors() :
  AbstractFilter(),
  m_CellFeatureAttributeMatrixPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_SharedSurfaceAreaListArrayName(DREAM3D::FeatureData::SharedSurfaceAreaList),
  m_NeighborListArrayName(DREAM3D::FeatureData::NeighborList),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_BoundaryCellsArrayName(DREAM3D::CellData::BoundaryCells),
  m_NumNeighborsArrayName(DREAM3D::FeatureData::NumNeighbors),
  m_SurfaceFeaturesArrayName(DREAM3D::FeatureData::SurfaceFeatures),
  m_StoreBoundaryCells(false),
  m_StoreSurfaceFeatures(false),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_BoundaryCells(NULL),
  m_SurfaceFeatures(NULL),
  m_NumNeighbors(NULL)
{
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
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Feature Ids Array Path", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix", "CellFeatureAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellFeatureAttributeMatrixPath(), true));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Number Of Neighbors Array Name", "NumNeighborsArrayName", FilterParameterWidgetType::StringWidget, getNumNeighborsArrayName(), true));
  parameters.push_back(FilterParameter::New("Neighbor List Array Name", "NeighborListArrayName", FilterParameterWidgetType::StringWidget, getNeighborListArrayName(), true));
  parameters.push_back(FilterParameter::New("Neighbor Surface Area List Array Name", "SharedSurfaceAreaListArrayName", FilterParameterWidgetType::StringWidget, getSharedSurfaceAreaListArrayName(), true));
  QStringList linkedProps("BoundaryCellsArrayName");
  parameters.push_back(FilterParameter::NewConditional("Store Surface Cells Array", "StoreBoundaryCells", FilterParameterWidgetType::LinkedBooleanWidget, getStoreBoundaryCells(), true, linkedProps));
  parameters.push_back(FilterParameter::New("Boundary Cells Array Name", "BoundaryCellsArrayName", FilterParameterWidgetType::StringWidget, getBoundaryCellsArrayName(), true));
  linkedProps.clear();
  linkedProps << "SurfaceFeaturesArrayName";
  parameters.push_back(FilterParameter::NewConditional("Store Surface Features Array", "StoreSurfaceFeatures", FilterParameterWidgetType::LinkedBooleanWidget, getStoreSurfaceFeatures(), true, linkedProps));
  parameters.push_back(FilterParameter::New("Surface Features Array Name", "SurfaceFeaturesArrayName", FilterParameterWidgetType::StringWidget, getSurfaceFeaturesArrayName(), true));

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
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixPath)
      DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
      DREAM3D_FILTER_WRITE_PARAMETER(BoundaryCellsArrayName)
      DREAM3D_FILTER_WRITE_PARAMETER(SurfaceFeaturesArrayName)
      DREAM3D_FILTER_WRITE_PARAMETER(StoreBoundaryCells)
      DREAM3D_FILTER_WRITE_PARAMETER(StoreSurfaceFeatures)
      DREAM3D_FILTER_WRITE_PARAMETER(NumNeighborsArrayName)
      DREAM3D_FILTER_WRITE_PARAMETER(NeighborListArrayName)
      DREAM3D_FILTER_WRITE_PARAMETER(SharedSurfaceAreaListArrayName)
      writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  // This is for convenience
  DataContainerArray::Pointer dca = getDataContainerArray();

  QVector<size_t> dims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = dca->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, m_FeatureIdsArrayPath, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), m_FeatureIdsArrayPath.getAttributeMatrixName(), getBoundaryCellsArrayName() );
  if(m_StoreBoundaryCells == true)
  {
    m_BoundaryCellsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter, int8_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_BoundaryCellsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_BoundaryCells = m_BoundaryCellsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  AttributeMatrix::Pointer cellFeatureAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<VolumeDataContainer, AbstractFilter>(this, getCellFeatureAttributeMatrixPath(), -304);
  if(getErrorCondition() < 0) { return; }

  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getNumNeighborsArrayName() );
  m_NumNeighborsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumNeighborsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumNeighbors = m_NumNeighborsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getSurfaceFeaturesArrayName() );
  if(m_StoreSurfaceFeatures == true)
  {
    m_SurfaceFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  // Feature Data
  // Do this whole block FIRST otherwise the side effect is that a call to m->getNumCellFeatureTuples will = 0
  // because we are just creating an empty NeighborList object.
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  typedef NeighborList<int> IntNeighborList_t;
  typedef NeighborList<float> FloatNeighborList_t;

  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getNeighborListArrayName() );
  m_NeighborList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */

  // And we do the same for the SharedSurfaceArea list
  //m_SharedSurfaceAreaList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>*>(cellFeatureAttrMat->getAttributeArray(m_SharedSurfaceAreaListArrayName.getDataArrayName()).get());
  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getSharedSurfaceAreaListArrayName() );
  m_SharedSurfaceAreaList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
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

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());
  int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t totalFeatures = m_NumNeighborsPtr.lock()->getNumberOfTuples();

  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  DimType neighpoints[6];
  neighpoints[0] = -dims[0] * dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0] * dims[1];

  float column, row, plane;
  int feature;
  size_t nnum;
  int onsurf = 0;
  int good = 0;
  int neighbor = 0;

  QVector<QVector<int> > neighborlist;
  QVector<QVector<float> > neighborsurfacearealist;

  int nListSize = 100;
  neighborlist.resize(totalFeatures);
  neighborsurfacearealist.resize(totalFeatures);

  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;
  //  uint64_t startMillis = millis;
  //  uint64_t estimatedTime = 0;
  //  float timeDiff = 0.0f;

  for (int i = 1; i < totalFeatures; i++)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if (currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Finding Neighbors - Initializing Neighbor Lists - %1 Percent Complete").arg((static_cast<float>(i) / totalFeatures) * 100);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      millis = QDateTime::currentMSecsSinceEpoch();
    }

    if(getCancel() == true) { break; }

    m_NumNeighbors[i] = 0;
    neighborlist[i].resize(nListSize);
    neighborsurfacearealist[i].fill(-1.0, nListSize);
    if(m_StoreSurfaceFeatures == true) { m_SurfaceFeatures[i] = false; }

  }

  for (int64_t j = 0; j < totalPoints; j++)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if (currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Finding Neighbors - Determining Neighbor Lists - %1 Percent Complete").arg((static_cast<float>(j) / totalPoints) * 100);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      millis = QDateTime::currentMSecsSinceEpoch();
    }
    if(getCancel() == true) { break; }

    onsurf = 0;
    feature = m_FeatureIds[j];
    if(feature > 0)
    {
      column = static_cast<float>( j % m->getXPoints() );
      row = static_cast<float>( (j / m->getXPoints()) % m->getYPoints() );
      plane = static_cast<float>( j / (m->getXPoints() * m->getYPoints()) );
      if(m_StoreSurfaceFeatures == true)
      {
        if((column == 0 || column == (m->getXPoints() - 1) || row == 0 || row == (m->getYPoints() - 1) || plane == 0 || plane == (m->getZPoints() - 1)) && m->getZPoints() != 1)
        {
          m_SurfaceFeatures[feature] = true;
        }
        if((column == 0 || column == (m->getXPoints() - 1) || row == 0 || row == (m->getYPoints() - 1)) && m->getZPoints() == 1)
        {
          m_SurfaceFeatures[feature] = true;
        }
      }
      for (int k = 0; k < 6; k++)
      {
        good = 1;
        neighbor = static_cast<int>( j + neighpoints[k] );
        if(k == 0 && plane == 0) { good = 0; }
        if(k == 5 && plane == (m->getZPoints() - 1)) { good = 0; }
        if(k == 1 && row == 0) { good = 0; }
        if(k == 4 && row == (m->getYPoints() - 1)) { good = 0; }
        if(k == 2 && column == 0) { good = 0; }
        if(k == 3 && column == (m->getXPoints() - 1)) { good = 0; }
        if(good == 1 && m_FeatureIds[neighbor] != feature && m_FeatureIds[neighbor] > 0)
        {
          onsurf++;
          nnum = m_NumNeighbors[feature];
          neighborlist[feature].push_back(m_FeatureIds[neighbor]);
          nnum++;
          m_NumNeighbors[feature] = static_cast<int32_t>(nnum);
        }
      }
    }
    if(m_StoreBoundaryCells == true) { m_BoundaryCells[j] = onsurf; }
  }

  // We do this to create new set of NeighborList objects
  for (size_t i = 1; i < totalFeatures; i++)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if (currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Finding Neighbors - Calculating Surface Areas - %1 Percent Complete").arg(((float)i / totalFeatures) * 100);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      millis = QDateTime::currentMSecsSinceEpoch();
    }
    if(getCancel() == true) { break; }


    QMap<int, int> neighToCount;
    int numneighs = static_cast<int>( neighborlist[i].size() );

    // this increments the voxel counts for each feature
    for (int j = 0; j < numneighs; j++)
    {
      neighToCount[neighborlist[i][j]]++;
    }

    QMap<int, int>::Iterator neighiter = neighToCount.find(0);
    neighToCount.erase(neighiter);
    neighiter = neighToCount.find(-1);
    neighToCount.erase(neighiter);
    //Resize the features neighbor list to zero
    neighborlist[i].resize(0);
    neighborsurfacearealist[i].resize(0);

    for (QMap<int, int>::iterator iter = neighToCount.begin(); iter != neighToCount.end(); ++iter)
    {
      int neigh = iter.key(); // get the neighbor feature
      int number = iter.value(); // get the number of voxels
      float area = number * m->getXRes() * m->getYRes();

      // Push the neighbor feature id back onto the list so we stay synced up
      neighborlist[i].push_back(neigh);
      neighborsurfacearealist[i].push_back(area);
    }
    m_NumNeighbors[i] = int32_t( neighborlist[i].size() );

    // Set the vector for each list into the NeighborList Object
    NeighborList<int>::SharedVectorType sharedNeiLst(new std::vector<int>);
    sharedNeiLst->assign(neighborlist[i].begin(), neighborlist[i].end());
    m_NeighborList.lock()->setList(static_cast<int>(i), sharedNeiLst);

    NeighborList<float>::SharedVectorType sharedSAL(new std::vector<float>);
    sharedSAL->assign(neighborsurfacearealist[i].begin(), neighborsurfacearealist[i].end());
    m_SharedSurfaceAreaList.lock()->setList(static_cast<int>(i), sharedSAL);
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
{ return Statistics::StatisticsBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighbors::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighbors::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MorphologicalFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindNeighbors::getHumanLabel()
{ return "Find Feature Neighbors"; }

