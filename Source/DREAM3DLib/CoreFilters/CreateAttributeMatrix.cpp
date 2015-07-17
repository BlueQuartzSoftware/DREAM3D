/*
 * Your License or Copyright can go here
 */

#include "CreateAttributeMatrix.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/AttributeMatrixCreationFilterParameter.h"
#include "DREAM3DLib/FilterParameters/ChoiceFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DynamicTableFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateAttributeMatrix::CreateAttributeMatrix() :
  AbstractFilter(),
  m_CreatedAttributeMatrix("", "AttributeMatrix", ""),
  m_AttributeMatrixType(DREAM3D::AttributeMatrixType::Generic),
  m_TupleDimensions()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateAttributeMatrix::~CreateAttributeMatrix()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAttributeMatrix::setupFilterParameters()
{
  FilterParameterVector parameters;

  {
    QVector<QString> choices;
    choices.push_back("Vertex");
    choices.push_back("Edge");
    choices.push_back("Face");
    choices.push_back("Cell");
    choices.push_back("VertexFeature");
    choices.push_back("EdgeFeature");
    choices.push_back("FaceFeature");
    choices.push_back("CellFeature");
    choices.push_back("VertexEnsemble");
    choices.push_back("EdgeEnsemble");
    choices.push_back("FaceEnsemble");
    choices.push_back("CellEnsemble");
    choices.push_back("MetaData");
    choices.push_back("Generic");
    parameters.push_back(ChoiceFilterParameter::New("Attribute Matrix Type", "AttributeMatrixType", getAttributeMatrixType(), choices, false, FilterParameter::Parameter));
  }

  QStringList rHeaders, cHeaders;
  rHeaders << "Tuple Dimensions";

  std::vector<std::vector<double> > defaultTable;
  parameters.push_back(DynamicTableFilterParameter::New("Tuple Dimensions", "TupleDimensions", rHeaders, cHeaders, defaultTable, FilterParameter::Parameter, false, true, 0));
  {
    AttributeMatrixCreationFilterParameter::DataStructureRequirements req;
    parameters.push_back(AttributeMatrixCreationFilterParameter::New("Created Attribute Matrix", "CreatedAttributeMatrix", getCreatedAttributeMatrix(), FilterParameter::CreatedArray, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAttributeMatrix::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCreatedAttributeMatrix(reader->readDataArrayPath("CreatedAttributeMatrix", getCreatedAttributeMatrix()));
  setAttributeMatrixType(reader->readValue("AttributeMatrixType", getAttributeMatrixType()));
  setTupleDimensions(reader->readDynamicTableData("TupleDimensions", getTupleDimensions()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CreateAttributeMatrix::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(CreatedAttributeMatrix)
  DREAM3D_FILTER_WRITE_PARAMETER(AttributeMatrixType)
  DREAM3D_FILTER_WRITE_PARAMETER(TupleDimensions)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAttributeMatrix::dataCheck()
{
  setErrorCondition(0);

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getCreatedAttributeMatrix().getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  DynamicTableData dtd = getTupleDimensions();
  std::vector<std::vector<double> > rows = dtd.getTableData();

  if(rows.size() != 1)
  {
    QString ss = QObject::tr("The number of rows of data must be 1. The data currently has %1").arg(rows.size());
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  std::vector<double> cols = rows[0]; // Get the first (and only) row of data
  QVector<size_t> tDims(cols.size(), 0);
  for(int i = 0; i < cols.size(); i++)
  {
    tDims[i] = static_cast<size_t>(cols[i]);
  }

  m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCreatedAttributeMatrix().getAttributeMatrixName(), tDims, getAttributeMatrixType());
  if(getErrorCondition() < 0) { return; }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAttributeMatrix::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAttributeMatrix::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (getCancel() == true) { return; }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateAttributeMatrix::newFilterInstance(bool copyFilterParameters)
{
  CreateAttributeMatrix::Pointer filter = CreateAttributeMatrix::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateAttributeMatrix::getCompiledLibraryName()
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateAttributeMatrix::getGroupName()
{ return DREAM3D::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateAttributeMatrix::getHumanLabel()
{ return "Create Attribute Matrix"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateAttributeMatrix::getSubGroupName()
{ return DREAM3D::FilterSubGroups::GenerationFilters; }

