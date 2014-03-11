/* ============================================================================
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ChangeGrainIdsField.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChangeGrainIdsField::ChangeGrainIdsField() :
  AbstractFilter(),
  m_GoodFieldsArrayName(""),
  m_GrainIdsArrayName(DREAM3D::FieldData::GoodFields),
  m_GrainIds(NULL),
  m_NewID(0)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChangeGrainIdsField::~ChangeGrainIdsField()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeGrainIdsField::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Good Fields Array");
    parameter->setPropertyName("GoodFieldsArrayName");
    parameter->setWidgetType(FilterParameter::VoxelFieldArrayNameSelectionWidget);
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
void ChangeGrainIdsField::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setGoodFieldsArrayName( reader->readValue( "GoodFieldsArrayName", getGoodFieldsArrayName() ) );
  setNewID( reader->readValue( "NewID", getNewID() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ChangeGrainIdsField::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("GoodFieldsArrayName", getGoodFieldsArrayName() );
  writer->writeValue("NewID", getNewID() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeGrainIdsField::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if(m_GoodFieldsArrayName.empty() == true)
  {
    ss.str("");
    ss << "The GoodFields Array Name is blank (empty).";
    setErrorCondition(-397);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else
  {
    //GET_PREREQ_DATA(m, DREAM3D, FieldData, GoodFields, ss, -300, bool, BoolArrayType, fields, 1)
  }

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeGrainIdsField::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeGrainIdsField::execute()
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
    if(0==m_GoodFields[m_GrainIds[i]])
    {
      m_GrainIds[i]=m_NewID;
    }
  }
  notifyStatusMessage("Complete");
}
