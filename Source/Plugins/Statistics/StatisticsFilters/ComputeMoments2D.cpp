/*
 * Your License or Copyright can go here
 */

#include "ComputeMoments2D.h"

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixCreationFilterParameter.h"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"

// Include the MOC generated file for this class
#include "moc_ComputeMoments2D.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComputeMoments2D::ComputeMoments2D() :
  AbstractFilter(),
  m_FeatureIdsArrayPath(DataArrayPath()),
  m_FeatureAttributeMatrix(DataArrayPath()),
  m_MomentsArrayName(QString("Moments2D"))
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComputeMoments2D::~ComputeMoments2D()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeMoments2D::initialize()
{
  setErrorCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeMoments2D::setupFilterParameters()
{
  FilterParameterVector parameters;
  DataArraySelectionFilterParameter::RequirementType dasReq;
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("FeatureIdsArrayPath", FeatureIdsArrayPath, FilterParameter::Parameter, ComputeMoments2D, dasReq));
  AttributeMatrixCreationFilterParameter::RequirementType amReq;
  parameters.push_back(SIMPL_NEW_AM_CREATION_FP("Feature AttributeMatrix", FeatureAttributeMatrix, FilterParameter::Parameter, ComputeMoments2D, amReq));

  parameters.push_back(SIMPL_NEW_AM_CREATION_FP("Moments 2D ArrayName", MomentsArrayName, FilterParameter::Parameter, ComputeMoments2D, amReq));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeMoments2D::dataCheck()
{
  setErrorCondition(0);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeMoments2D::preflight()
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
void ComputeMoments2D::execute()
{
  initialize();
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
AbstractFilter::Pointer ComputeMoments2D::newFilterInstance(bool copyFilterParameters)
{
  ComputeMoments2D::Pointer filter = ComputeMoments2D::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeMoments2D::getCompiledLibraryName()
{ return StatisticsConstants::StatisticsBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeMoments2D::getBrandingString()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeMoments2D::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeMoments2D::getGroupName()
{ return SIMPL::FilterGroups::Unsupported; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeMoments2D::getSubGroupName()
{ return "Statistics"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeMoments2D::getHumanLabel()
{ return "ComputeMoments2D"; }

