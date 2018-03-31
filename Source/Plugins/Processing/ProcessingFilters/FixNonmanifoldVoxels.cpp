/*
 * Your License or Copyright can go here
 */

#include "FixNonmanifoldVoxels.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Processing/ProcessingConstants.h"
#include "Processing/ProcessingVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FixNonmanifoldVoxels::FixNonmanifoldVoxels()
: m_FeatureIdsArrayPath("", "", "")
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FixNonmanifoldVoxels::~FixNonmanifoldVoxels() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FixNonmanifoldVoxels::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FixNonmanifoldVoxels::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Element);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, FixNonmanifoldVoxels, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FixNonmanifoldVoxels::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  IGeometry::Pointer geom = getDataContainerArray()->getPrereqGeometryFromDataContainer<IGeometry, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());
  ImageGeom::Pointer imageGeom = std::dynamic_pointer_cast<ImageGeom>(geom);
  if(nullptr == imageGeom)
  {
    setErrorCondition(-12001);
    QString ss = QObject::tr("This filter only works on ImageGeometry.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims);
  if(nullptr != m_FeatureIdsPtr.lock())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FixNonmanifoldVoxels::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FixNonmanifoldVoxels::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FixNonmanifoldVoxels::newFilterInstance(bool copyFilterParameters) const
{
  FixNonmanifoldVoxels::Pointer filter = FixNonmanifoldVoxels::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FixNonmanifoldVoxels::getCompiledLibraryName() const
{
  return ProcessingConstants::ProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FixNonmanifoldVoxels::getBrandingString() const
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FixNonmanifoldVoxels::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Processing::Version::Major() << "." << Processing::Version::Minor() << "." << Processing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FixNonmanifoldVoxels::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FixNonmanifoldVoxels::getSubGroupName() const
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FixNonmanifoldVoxels::getHumanLabel() const
{
  return "FixNonmanifoldVoxels";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid FixNonmanifoldVoxels::getUuid()
{
  return QUuid("{1f7f2bec-50d9-5e24-a8d4-8fd291ffed9d}");
}
