/*
 * Your License or Copyright can go here
 */

#include "FindDifferenceMap.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/TemplateHelpers.hpp"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "Statistics/StatisticsConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindDifferenceMap::FindDifferenceMap() :
  AbstractFilter(),
  m_FirstInputArrayPath("", "", ""),
  m_SecondInputArrayPath("", "", ""),
  m_DifferenceMapArrayPath("", "", ""),
  m_FirstInputArray(NULL),
  m_SecondInputArray(NULL),
  m_DifferenceMap(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindDifferenceMap::~FindDifferenceMap()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDifferenceMap::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(DataArraySelectionFilterParameter::New("First Attribute Array", "FirstInputArrayPath", getFirstInputArrayPath(), FilterParameter::RequiredArray));
  parameters.push_back(DataArraySelectionFilterParameter::New("Second Attribute Array", "SecondInputArrayPath", getSecondInputArrayPath(), FilterParameter::RequiredArray));
  parameters.push_back(DataArrayCreationFilterParameter::New("Difference Map", "DifferenceMapArrayPath", getDifferenceMapArrayPath(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDifferenceMap::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFirstInputArrayPath(reader->readDataArrayPath("FirstInputArrayPath", getFirstInputArrayPath()));
  setSecondInputArrayPath(reader->readDataArrayPath("SecondInputArrayPath", getSecondInputArrayPath()));
  setDifferenceMapArrayPath(reader->readDataArrayPath("DifferenceMapArrayPath", getDifferenceMapArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindDifferenceMap::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FirstInputArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SecondInputArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(DifferenceMapArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename DataType>
void validateArrayTypes(AbstractFilter* filter, QVector<IDataArray::Pointer> ptrArray)
{
  for (QVector<IDataArray::Pointer>::iterator it = ptrArray.begin(); it != ptrArray.end(); ++it)
  {
    if (!TemplateHelpers::CanDynamicCast<DataArray<DataType> >()((*it)))
    {
      filter->setErrorCondition(-90000);
      QString ss = QObject::tr("Selected Attribute Arrays must all be of the same type");
      filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDifferenceMap::dataCheck()
{
  setErrorCondition(0);

  QVector<IDataArray::Pointer> dataArrays;

  m_FirstInputArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getFirstInputArrayPath());
  if (getErrorCondition() >= 0) { dataArrays.push_back(m_FirstInputArrayPtr.lock()); }

  m_SecondInputArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSecondInputArrayPath());
  if (getErrorCondition() >= 0) { dataArrays.push_back(m_SecondInputArrayPtr.lock()); }

  EXECUTE_FUNCTION_TEMPLATE(this, validateArrayTypes, dataArrays[0], this, dataArrays)

  //if (NULL != m_FirstInputArrayPtr.lock())

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDifferenceMap::preflight()
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
void FindDifferenceMap::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (getCancel() == true) { return; }

  if (getErrorCondition() < 0)
  {
    QString ss = QObject::tr("Some error message");
    setErrorCondition(-99999999);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindDifferenceMap::newFilterInstance(bool copyFilterParameters)
{
  FindDifferenceMap::Pointer filter = FindDifferenceMap::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDifferenceMap::getCompiledLibraryName()
{ return StatisticsConstants::StatisticsBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDifferenceMap::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDifferenceMap::getHumanLabel()
{ return "Find Difference Map"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDifferenceMap::getSubGroupName()
{ return "Statistics"; }

