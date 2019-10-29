/*
 * Your License or Copyright can go here
 */

#include <memory>

#include "OrientationUtility.h"

#include <QtCore/QTextStream>

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
  FilterParameterVectorType parameters;

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
QString OrientationUtility::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString OrientationUtility::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString OrientationUtility::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString OrientationUtility::getGroupName() const
{
  return SIMPL::FilterGroups::Utilities;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid OrientationUtility::getUuid() const
{
  return QUuid("{5af9c1e6-ed6f-5672-9ae0-2b931344d729}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString OrientationUtility::getSubGroupName() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString OrientationUtility::getHumanLabel() const
{
  return "Orientation Utility";
}

// -----------------------------------------------------------------------------
OrientationUtility::Pointer OrientationUtility::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<OrientationUtility> OrientationUtility::New()
{
  struct make_shared_enabler : public OrientationUtility
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString OrientationUtility::getNameOfClass() const
{
  return QString("OrientationUtility");
}

// -----------------------------------------------------------------------------
QString OrientationUtility::ClassName()
{
  return QString("OrientationUtility");
}
