/* ============================================================================
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ChangeGrainIds.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChangeGrainIds::ChangeGrainIds() :
  AbstractFilter(),
  m_GoodVoxelsArrayName(""),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_GrainIds(NULL),
  m_NewID(0)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChangeGrainIds::~ChangeGrainIds()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeGrainIds::setupFilterParameters()
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
void ChangeGrainIds::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setGoodVoxelsArrayName( reader->readValue( "GoodVoxelsArrayName", getGoodVoxelsArrayName() ) );
  setNewID( reader->readValue( "NewID", getNewID() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ChangeGrainIds::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("GoodVoxelsArrayName", getGoodVoxelsArrayName() );
  writer->writeValue("NewID", getNewID() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeGrainIds::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
void ChangeGrainIds::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeGrainIds::execute()
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
    if(0==m_GoodVoxels[i])
    {
      m_GrainIds[i]=m_NewID;
    }
  }
  notifyStatusMessage("Complete");
}
