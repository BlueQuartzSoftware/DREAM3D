/* ============================================================================
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "UpdateGrainIDs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UpdateGrainIDs::UpdateGrainIDs() :
  AbstractFilter(),
  m_GoodVoxelsArrayName(""),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_GrainIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UpdateGrainIDs::~UpdateGrainIDs()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateGrainIDs::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Good Voxels Array");
    parameter->setPropertyName("GoodVoxelsArrayName");
    parameter->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    parameter->setValueType("string");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateGrainIDs::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setGoodVoxelsArrayName( reader->readValue( "GoodVoxelsArrayName", getGoodVoxelsArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int UpdateGrainIDs::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("GoodVoxelsArrayName", getGoodVoxelsArrayName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateGrainIDs::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if(m_GoodVoxelsArrayName.empty() == true)
  {
    ss.str("");
    ss << "The GoodVoxels Array Name is blank (empty).";
    setErrorCondition(-397);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, ss, -300, bool, BoolArrayType, voxels, 1)
  }

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateGrainIDs::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateGrainIDs::execute()
{
  int32_t err =0;
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  int64_t totalPoints = m->getTotalPoints();
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, numgrains, numensembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  IDataArray::Pointer pGoodArray = m->getCellData(m_GoodVoxelsArrayName);
  for(int i=0; i<totalPoints; i++)
  {
    if(0==m_GoodVoxels[i])
    {
      m_GrainIds[i]=0;
    }
  }
  notifyStatusMessage("Complete");
}
