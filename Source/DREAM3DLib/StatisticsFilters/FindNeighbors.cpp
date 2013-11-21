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


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"
#include "DREAM3DLib/DataArrays/IDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighbors::FindNeighbors() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_SurfaceVoxelsArrayName(DREAM3D::CellData::SurfaceVoxels),
  m_NumNeighborsArrayName(DREAM3D::FeatureData::NumNeighbors),
  m_SurfaceFeaturesArrayName(DREAM3D::FeatureData::SurfaceFeatures),
  m_SharedSurfaceAreaListArrayName(DREAM3D::FeatureData::SharedSurfaceAreaList),
  m_NeighborListArrayName(DREAM3D::FeatureData::NeighborList),
  m_FeatureIds(NULL),
  m_SurfaceVoxels(NULL),
  m_SurfaceFeatures(NULL),
  m_NumNeighbors(NULL),
  m_NeighborList(NULL),
  m_SharedSurfaceAreaList(NULL)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighbors::~FindNeighbors()
{
}
// -----------------------------------------------------------------------------
void FindNeighbors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int FindNeighbors::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -300, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_SurfaceVoxelsPtr = m->createNonPrereqArray<int8_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_SurfaceVoxelsArrayName, 0, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_SurfaceVoxels = m_SurfaceVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_NumNeighborsPtr = m->createNonPrereqArray<int32_t, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_NumNeighborsArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumNeighborsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_NumNeighbors = m_NumNeighborsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_SurfaceFeaturesPtr = m->createNonPrereqArray<bool, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_SurfaceFeaturesArrayName, false, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Data
  // Do this whole block FIRST otherwise the side effect is that a call to m->getNumCellFeatureTuples will = 0
  // because we are just creating an empty NeighborList object.
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >
                   (m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(m_NeighborListArrayName).get());
  if(m_NeighborList == NULL)
  {
    NeighborList<int>::Pointer neighborlistPtr = NeighborList<int>::New();
    neighborlistPtr->SetName(m_NeighborListArrayName);
    neighborlistPtr->Resize(features);
    neighborlistPtr->setNumNeighborsArrayName(m_NumNeighborsArrayName);
    m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(m_NeighborListArrayName, neighborlistPtr);
    if (neighborlistPtr.get() == NULL)
    {
      QString ss = QObject::tr("NeighborLists Array Not Initialized at Beginning of FindNeighbors Filter");
      setErrorCondition(-308);
    }
    m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >
                     (m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(m_NeighborListArrayName).get());

    CreatedArrayHelpIndexEntry::Pointer e = CreatedArrayHelpIndexEntry::New();
    e->setFilterName(this->getNameOfClass());
    e->setFilterHumanLabel(this->getHumanLabel());
    e->setFilterGroup(this->getGroupName());
    e->setFilterSubGroup(this->getSubGroupName());
    e->setArrayDefaultName(m_NeighborListArrayName);
    e->setArrayGroup("Feature");
    e->setArrayNumComponents(0);
    e->setArrayType("NeighborList");
    addCreatedArrayHelpIndexEntry(e);
  }

  // And we do the same for the SharedSurfaceArea list
  m_SharedSurfaceAreaList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>*>
                            (m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(m_SharedSurfaceAreaListArrayName).get());
  if(m_SharedSurfaceAreaList == NULL)
  {
    NeighborList<float>::Pointer sharedSurfaceAreaListPtr = NeighborList<float>::New();
    sharedSurfaceAreaListPtr->SetName(m_SharedSurfaceAreaListArrayName);
    sharedSurfaceAreaListPtr->Resize(features);
    sharedSurfaceAreaListPtr->setNumNeighborsArrayName(m_NumNeighborsArrayName);
    m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(m_SharedSurfaceAreaListArrayName, sharedSurfaceAreaListPtr);
    if (sharedSurfaceAreaListPtr.get() == NULL)
    {

      QString ss = QObject::tr("SurfaceAreaLists Array Not Initialized correctly at Beginning of FindNeighbors Filter");
      setErrorCondition(-308);
      addErrorMessage(getHumanLabel(), ss, -308);
    }
    m_SharedSurfaceAreaList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>*>
                              (m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(m_SharedSurfaceAreaListArrayName).get());
    CreatedArrayHelpIndexEntry::Pointer e = CreatedArrayHelpIndexEntry::New();
    e->setFilterName(this->getNameOfClass());
    e->setFilterHumanLabel(this->getHumanLabel());
    e->setFilterGroup(this->getGroupName());
    e->setFilterSubGroup(this->getSubGroupName());
    e->setArrayDefaultName(m_SharedSurfaceAreaListArrayName);
    e->setArrayGroup("Feature");
    e->setArrayNumComponents(0);
    e->setArrayType("SurfaceAreaLists");
    addCreatedArrayHelpIndexEntry(e);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    addErrorMessage(getHumanLabel(), "The VolumeDataContainer Object with the specific name " + getDataContainerName() + " was not available.", getErrorCondition());
    return;
  }

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::execute()
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  int totalFeatures = int(m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  dataCheck(false, totalPoints, totalFeatures, m->getNumCellEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

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

  //size_t xtalCount = m->getCellEnsembleData(DREAM3D::EnsembleData::CrystalStructures)->getNumberOfTuples();

  QVector<QVector<int> > neighborlist;
  QVector<QVector<float> > neighborsurfacearealist;

  int nListSize = 100;
  neighborlist.resize(totalFeatures);
  neighborsurfacearealist.resize(totalFeatures);
  for (int i = 1; i < totalFeatures; i++)
  {
    QString ss = QObject::tr("Finding Neighbors - Initializing Neighbor Lists - %1 Percent Complete").arg((static_cast<float>(i) / totalFeatures) * 100);
    //   notifyStatusMessage(ss);
    m_NumNeighbors[i] = 0;
    neighborlist[i].resize(nListSize);
    neighborsurfacearealist[i].fill(-1.0, nListSize);
    m_SurfaceFeatures[i] = false;
  }

  totalPoints = m->getTotalPoints();

  for (int64_t j = 0; j < totalPoints; j++)
  {
    QString ss = QObject::tr("Finding Neighbors - Determining Neighbor Lists - %1 Percent Complete").arg((static_cast<float>(j) / totalPoints) * 100);
    //   notifyStatusMessage(ss);
    onsurf = 0;
    feature = m_FeatureIds[j];
    if(feature > 0)
    {
      column = static_cast<float>( j % m->getXPoints() );
      row = static_cast<float>( (j / m->getXPoints()) % m->getYPoints() );
      plane = static_cast<float>( j / (m->getXPoints() * m->getYPoints()) );
      if((column == 0 || column == (m->getXPoints() - 1) || row == 0 || row == (m->getYPoints() - 1) || plane == 0 || plane == (m->getZPoints() - 1)) && m->getZPoints() != 1)
      {
        m_SurfaceFeatures[feature] = true;
      }
      if((column == 0 || column == (m->getXPoints() - 1) || row == 0 || row == (m->getYPoints() - 1)) && m->getZPoints() == 1)
      {
        m_SurfaceFeatures[feature] = true;
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
    m_SurfaceVoxels[j] = onsurf;
  }

  // We do this to create new set of NeighborList objects
  dataCheck(false, totalPoints, totalFeatures, m->getNumCellEnsembleTuples());

  for (size_t i = 1; i < m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples(); i++)
  {

    QString ss = QObject::tr("Finding Neighbors - Calculating Surface Areas - %1 Percent Complete").arg(((float)i / totalFeatures) * 100);
    //  notifyStatusMessage(ss);

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
    m_NeighborList->setList(static_cast<int>(i), sharedNeiLst);

    NeighborList<float>::SharedVectorType sharedSAL(new std::vector<float>);
    sharedSAL->assign(neighborsurfacearealist[i].begin(), neighborsurfacearealist[i].end());
    m_SharedSurfaceAreaList->setList(static_cast<int>(i), sharedSAL);
  }

  notifyStatusMessage("Finding Neighbors Complete");
}

