#include "FindFieldClustering.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/StatisticsFilters/FindSizes.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"
#include "DREAM3DLib/GenericFilters/FindGrainCentroids.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFieldClustering::FindFieldClustering() :
AbstractFilter(),
m_CentroidsArrayName(DREAM3D::FieldData::Centroids),
m_EquivalentDiametersArrayName(DREAM3D::FieldData::EquivalentDiameters),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_ClustersArrayName(DREAM3D::FieldData::Clusters),
m_ClusteringListArrayName(DREAM3D::FieldData::ClusteringList),
m_FieldPhases(NULL),
m_Centroids(NULL),
m_EquivalentDiameters(NULL),
m_Clusters(NULL),
m_ClusteringList(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFieldClustering::~FindFieldClustering()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFieldClustering::setupFilterParameters()
{
}
// -----------------------------------------------------------------------------
void FindFieldClustering::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int FindFieldClustering::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFieldClustering::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VolumeDataContainer* m = getVolumeDataContainer();

  // Field Data
  // Do this whole block FIRST otherwise the side effect is that a call to m->getNumFieldTuples will = 0
  // because we are just creating an empty NeighborList object.
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_ClusteringList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>* >
                                          (m->getFieldData(m_ClusteringListArrayName).get());
  if(m_ClusteringList == NULL)
  {
    NeighborList<float>::Pointer clusteringPtr = NeighborList<float>::New();
    clusteringPtr->SetName(m_ClusteringListArrayName);
    clusteringPtr->Resize(fields);
    clusteringPtr->setNumNeighborsArrayName(m_ClusteringListArrayName);
    m->addFieldData(m_ClusteringListArrayName, clusteringPtr);
    if (clusteringPtr.get() == NULL) {
      ss << "Clustering Array Not Initialized at Beginning of FindFieldClustering Filter" << std::endl;
      setErrorCondition(-308);
    }
    m_ClusteringList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>* >
                                          (m->getFieldData(m_ClusteringListArrayName).get());

    CreatedArrayHelpIndexEntry::Pointer e = CreatedArrayHelpIndexEntry::New();
    e->setFilterName(this->getNameOfClass());
    e->setFilterHumanLabel(this->getHumanLabel());
    e->setFilterGroup(this->getGroupName());
    e->setFilterSubGroup(this->getSubGroupName());
    e->setArrayDefaultName(m_ClusteringListArrayName);
    e->setArrayGroup("Field");
    e->setArrayNumComponents(0);
    e->setArrayType("NeighborList");
    addCreatedArrayHelpIndexEntry(e);
  }

  GET_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, -302, float, FloatArrayType, fields, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -304, int32_t, Int32ArrayType, fields, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, ss, -305, float, FloatArrayType, fields, 3)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Clusters, ss, int32_t, Int32ArrayType, 0, fields, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFieldClustering::preflight()
{
  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFieldClustering::execute()
{
  VolumeDataContainer* m = getVolumeDataContainer();
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

  find_clustering();
  notifyStatusMessage("FindFieldClustering Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFieldClustering::find_clustering()
{
  std::stringstream ss;
  VolumeDataContainer* m = getVolumeDataContainer();

  float x, y, z;
  float xn, yn, zn;
  float r;

  std::vector<std::vector<float> > clusteringlist;

  int totalFields = int(m->getNumFieldTuples());

  clusteringlist.resize(totalFields);

  for (size_t i = 1; i < totalFields; i++)
  {
    m_Clusters[i] = 0;
  }

  for (size_t i = 1; i < totalFields; i++)
  {
      x = m_Centroids[3*i];
      y = m_Centroids[3*i+1];
      z = m_Centroids[3*i+2];
  }
  for (size_t i = 1; i < totalFields; i++)
  {
    if (i%1000 == 0)
    {
      ss.str("");
      ss << "Working On Grain " << i << " of " << totalFields;
      notifyStatusMessage(ss.str());
    }
    x = m_Centroids[3*i];
    y = m_Centroids[3*i+1];
    z = m_Centroids[3*i+2];
    for (size_t j = i+1; j < totalFields; j++)
    {
      xn = m_Centroids[3*j];
      yn = m_Centroids[3*j+1];
      zn = m_Centroids[3*j+2];
      r = sqrtf((x - xn) * (x - xn) + (y - yn) * (y - yn) + (z - zn) * (z - zn));
      m_Clusters[i]++;
      clusteringlist[i].push_back(r);
      m_Clusters[j]++;
      clusteringlist[j].push_back(r);

    }
  }
  for (size_t i = 1; i < totalFields; i++)
  {
      // Set the vector for each list into the Clustering Object
      NeighborList<float>::SharedVectorType sharedClustLst(new std::vector<float>);
      sharedClustLst->assign(clusteringlist[i].begin(), clusteringlist[i].end());
      m_ClusteringList->setList(static_cast<int>(i), sharedClustLst);
  }
}

