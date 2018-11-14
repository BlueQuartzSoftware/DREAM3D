/*
 * Your License or Copyright can go here
 */

#include "OrientationUtility.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"

#include "OrientationAnalysis/FilterParameters/OrientationUtilityFilterParameter.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationUtility::OrientationUtility() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationUtility::~OrientationUtility() = default;

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
AbstractFilter::Pointer OrientationUtility::newFilterInstance(bool copyFilterParameters) const
{
  OrientationUtility::Pointer filter = OrientationUtility::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString OrientationUtility::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString OrientationUtility::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString OrientationUtility::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString OrientationUtility::getGroupName() const
{
  return SIMPL::FilterGroups::Utilities;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid OrientationUtility::getUuid()
{
  return QUuid("{5af9c1e6-ed6f-5672-9ae0-2b931344d729}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString OrientationUtility::getSubGroupName() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString OrientationUtility::getHumanLabel() const
{
  return "Orientation Utility";
}
