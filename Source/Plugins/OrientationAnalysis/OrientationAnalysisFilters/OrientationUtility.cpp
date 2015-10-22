/*
 * Your License or Copyright can go here
 */

#include "OrientationUtility.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "OrientationAnalysis/FilterParameters/OrientationUtilityFilterParameter.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"

// Include the MOC generated file for this class
#include "moc_OrientationUtility.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationUtility::OrientationUtility() :
  AbstractFilter()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationUtility::~OrientationUtility()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationUtility::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(OrientationUtilityFilterParameter::New("", "", "", FilterParameter::Parameter));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationUtility::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int OrientationUtility::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);

  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationUtility::dataCheck()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationUtility::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationUtility::execute()
{
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer OrientationUtility::newFilterInstance(bool copyFilterParameters)
{
  OrientationUtility::Pointer filter = OrientationUtility::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString OrientationUtility::getCompiledLibraryName()
{ return OrientationAnalysisConstants::OrientationAnalysisBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString OrientationUtility::getGroupName()
{ return DREAM3D::FilterGroups::Utilities; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString OrientationUtility::getSubGroupName()
{ return "OrientationAnalysis"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString OrientationUtility::getHumanLabel()
{ return "Orientation Utility"; }

