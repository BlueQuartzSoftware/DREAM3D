/*
 * Your License or Copyright can go here
 */

#include "OrientationUtility.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"

#include "OrientationAnalysis/FilterParameters/OrientationUtilityFilterParameter.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

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
void OrientationUtility::initialize()
{

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
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString OrientationUtility::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString OrientationUtility::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString OrientationUtility::getGroupName()
{ return SIMPL::FilterGroups::Utilities; }

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

