/* ============================================================================
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "MoveGrain.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MoveGrain::MoveGrain() :
  AbstractFilter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_GrainIds(NULL),
  m_OldID(0),
  m_NewID(0)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MoveGrain::~MoveGrain()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveGrain::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Old Grain ID");
    option->setPropertyName("OldID");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("New Grain ID");
    option->setPropertyName("NewID");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveGrain::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOldID( reader->readValue( "OldID", getOldID() ) );
  setNewID( reader->readValue( "NewID", getNewID() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MoveGrain::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OldID", getOldID() );
  writer->writeValue("NewID", getNewID() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveGrain::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveGrain::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveGrain::execute()
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

  for(int i=0; i<totalPoints; i++)
  {
    if(m_OldID==m_GrainIds[i])
    {
      m_GrainIds[i]=m_NewID;
    }
  }
  notifyStatusMessage("Complete");
}
