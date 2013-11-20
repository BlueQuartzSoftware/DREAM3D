#include "BridgeParentIdsStatisticsToFeatureIds.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"

#include "DREAM3DLib/GenericFilters/RenameCellArray.h"
#include "DREAM3DLib/GenericFilters/LinkFeatureMapToCellArray.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1




#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BridgeParentIdsStatisticsToFeatureIds::BridgeParentIdsStatisticsToFeatureIds() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellParentIdsArrayName(DREAM3D::CellData::ParentIds),
  m_FeatureParentIdsArrayName(DREAM3D::FeatureData::ParentIds),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_FeatureIds(NULL),
  m_CellParentIds(NULL),
  m_FeatureParentIds(NULL),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BridgeParentIdsStatisticsToFeatureIds::~BridgeParentIdsStatisticsToFeatureIds()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BridgeParentIdsStatisticsToFeatureIds::setupFilterParameters()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BridgeParentIdsStatisticsToFeatureIds::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int BridgeParentIdsStatisticsToFeatureIds::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BridgeParentIdsStatisticsToFeatureIds::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles, bool afterLink)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  // Cell Data

  QVector<int> dims(1 ,1);
  m_FeatureIdsPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -301, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_FeatureIds */
  m_CellParentIdsPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellParentIdsArrayName, -304, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_CellParentIds = m_CellParentIdsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_CellParentIds */

  // Feature Data
  m_FeatureParentIdsPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_FeatureParentIdsArrayName, -302, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_FeatureParentIds = m_FeatureParentIdsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_FeatureParentIds */

  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = m->getPrereqArray<unsigned int, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_CrystalStructuresArrayName, -305, ensembles, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_CrystalStructures */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BridgeParentIdsStatisticsToFeatureIds::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    addErrorMessage(getHumanLabel(), "The VolumeDataContainer Object with the specific name " + getDataContainerName() + " was not available.", getErrorCondition());
    return;
  }

  dataCheck(true, 1, 1, 1, false);

  RenameCellArray::Pointer rename_cell_array = RenameCellArray::New();
  rename_cell_array->setObservers(this->getObservers());
  rename_cell_array->setDataContainerArray(getDataContainerArray());
  rename_cell_array->setMessagePrefix(getMessagePrefix());
  rename_cell_array->setSelectedCellArrayName(m_CellParentIdsArrayName);
  rename_cell_array->setNewCellArrayName(m_FeatureIdsArrayName);
  rename_cell_array->preflight();
  int err1 = rename_cell_array->getErrorCondition();
  if (err1 < 0)
  {
    setErrorCondition(rename_cell_array->getErrorCondition());
    addErrorMessages(rename_cell_array->getPipelineMessages());
    return;
  }

  LinkFeatureMapToCellArray::Pointer link_feature_map_to_cell_array = LinkFeatureMapToCellArray::New();
  link_feature_map_to_cell_array->setObservers(this->getObservers());
  link_feature_map_to_cell_array->setDataContainerArray(getDataContainerArray());
  link_feature_map_to_cell_array->setMessagePrefix(getMessagePrefix());
  link_feature_map_to_cell_array->setSelectedCellDataArrayName(m_FeatureIdsArrayName);
  link_feature_map_to_cell_array->preflight();
  int err2 = link_feature_map_to_cell_array->getErrorCondition();
  if (err2 < 0)
  {
    setErrorCondition(link_feature_map_to_cell_array->getErrorCondition());
    addErrorMessages(link_feature_map_to_cell_array->getPipelineMessages());
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BridgeParentIdsStatisticsToFeatureIds::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  setErrorCondition(0);
  dataCheck(false, m->getTotalPoints(), m->getNumCellFeatureTuples(), m->getNumCellEnsembleTuples(), true);
  if (getErrorCondition() < 0)
  {
    return;
  }

  RenameCellArray::Pointer rename_cell_array = RenameCellArray::New();
  rename_cell_array->setObservers(this->getObservers());
  rename_cell_array->setDataContainerArray(getDataContainerArray());
  rename_cell_array->setMessagePrefix(getMessagePrefix());
  rename_cell_array->setSelectedCellArrayName(m_CellParentIdsArrayName);
  rename_cell_array->setNewCellArrayName(m_FeatureIdsArrayName);
  rename_cell_array->preflight();
  int err1 = rename_cell_array->getErrorCondition();
  if (err1 < 0)
  {
    setErrorCondition(rename_cell_array->getErrorCondition());
    addErrorMessages(rename_cell_array->getPipelineMessages());
    return;
  }

  LinkFeatureMapToCellArray::Pointer link_feature_map_to_cell_array = LinkFeatureMapToCellArray::New();
  link_feature_map_to_cell_array->setObservers(this->getObservers());
  link_feature_map_to_cell_array->setDataContainerArray(getDataContainerArray());
  link_feature_map_to_cell_array->setMessagePrefix(getMessagePrefix());
  link_feature_map_to_cell_array->setSelectedCellDataArrayName(m_FeatureIdsArrayName);
  link_feature_map_to_cell_array->preflight();
  int err2 = link_feature_map_to_cell_array->getErrorCondition();
  if (err2 < 0)
  {
    setErrorCondition(link_feature_map_to_cell_array->getErrorCondition());
    addErrorMessages(link_feature_map_to_cell_array->getPipelineMessages());
    return;
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("BridgeParentIdsStatisticsToFeatureIds Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

