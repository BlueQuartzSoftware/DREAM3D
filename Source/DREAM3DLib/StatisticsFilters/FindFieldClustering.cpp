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
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_CentroidsArrayName(DREAM3D::FieldData::Centroids),
  m_EquivalentDiametersArrayName(DREAM3D::FieldData::EquivalentDiameters),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_ClusteringListArrayName(DREAM3D::FieldData::ClusteringList),
  m_ErrorOutputFile(),
  m_FieldPhases(NULL),
  m_Centroids(NULL),
  m_EquivalentDiameters(NULL),
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
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  // Field Data
  // Do this whole block FIRST otherwise the side effect is that a call to m->getNumFieldTuples will = 0
  // because we are just creating an empty NeighborList object.
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_ClusteringList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>* >
                     (m->getCellFieldData(m_ClusteringListArrayName).get());
  if(m_ClusteringList == NULL)
  {
    NeighborList<float>::Pointer clusteringPtr = NeighborList<float>::New();
    clusteringPtr->SetName(m_ClusteringListArrayName);
    clusteringPtr->Resize(fields);
    clusteringPtr->setNumNeighborsArrayName(m_ClusteringListArrayName);
    m->addCellFieldData(m_ClusteringListArrayName, clusteringPtr);
    if (clusteringPtr.get() == NULL)
    {
      QString ss = QObject::tr("Clustering Array Not Initialized at Beginning of FindFieldClustering Filter");
      setErrorCondition(-308);
      addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    m_ClusteringList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>* >
                       (m->getCellFieldData(m_ClusteringListArrayName).get());

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

  QVector<int> dims(1,1);
  GET_PREREQ_DATA(m, DREAM3D, CellFieldData, EquivalentDiameters, -302, float, FloatArrayType, fields, dims)

  GET_PREREQ_DATA(m, DREAM3D, CellFieldData, FieldPhases, -304, int32_t, Int32ArrayType, fields, dims)
  dims[0] = 3;
  GET_PREREQ_DATA(m, DREAM3D, CellFieldData, Centroids, -305, float, FloatArrayType, fields, dims)
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
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  dataCheck(false, m->getTotalPoints(), m->getNumCellFieldTuples(), m->getNumCellEnsembleTuples());
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
  bool writeErrorFile = true;
  std::ofstream outFile;
  if(m_ErrorOutputFile.isEmpty() == false)
  {
    outFile.open(m_ErrorOutputFile.toLatin1().data(), std::ios_base::binary);
    writeErrorFile = true;
  }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  float x, y, z;
  float xn, yn, zn;
  float r;

  std::vector<std::vector<float> > clusteringlist;

  int totalFields = int(m->getNumCellFieldTuples());

  clusteringlist.resize(totalFields);

  for (size_t i = 1; i < totalFields; i++)
  {
    x = m_Centroids[3 * i];
    y = m_Centroids[3 * i + 1];
    z = m_Centroids[3 * i + 2];
  }
  for (size_t i = 1; i < totalFields; i++)
  {
    if (i % 1000 == 0)
    {

      QString ss = QObject::tr("Working On Grain %1 of %2").arg(i).arg(totalFields);
      notifyStatusMessage(ss);
    }
    x = m_Centroids[3 * i];
    y = m_Centroids[3 * i + 1];
    z = m_Centroids[3 * i + 2];
    for (size_t j = i + 1; j < totalFields; j++)
    {
      if (m_FieldPhases[i] == m_FieldPhases[j])
      {
        xn = m_Centroids[3 * j];
        yn = m_Centroids[3 * j + 1];
        zn = m_Centroids[3 * j + 2];
        r = sqrtf((x - xn) * (x - xn) + (y - yn) * (y - yn) + (z - zn) * (z - zn));
        clusteringlist[i].push_back(r);
        clusteringlist[j].push_back(r);

        if(writeErrorFile == true && m_FieldPhases[j] == 2)
        {
          outFile << r << "\n" << r << "\n";
        }
      }
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

