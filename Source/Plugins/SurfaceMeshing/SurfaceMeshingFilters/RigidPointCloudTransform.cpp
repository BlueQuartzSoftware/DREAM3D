/*
 * Your License or Copyright can go here
 */

#include "RigidPointCloudTransform.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RigidPointCloudTransform::RigidPointCloudTransform() 
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RigidPointCloudTransform::~RigidPointCloudTransform() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RigidPointCloudTransform::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RigidPointCloudTransform::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  DataContainerSelectionFilterParameter::RequirementType dcsReq;
  parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Geometry To Align", MovingGeometry, FilterParameter::Category::Parameter, RigidPointCloudTransform, dcsReq));
  // DynamicTableWidget - Use optional parameters to enable dynamic rows or dynamic columns
  parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Key Points from Moving", MovingKeyPoints, FilterParameter::Category::Parameter, RigidPointCloudTransform));
  // DynamicTableWidget - Use optional parameters to enable dynamic rows or dynamic columns
  parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Key Points from Fixed", FixedKeyPoints, FilterParameter::Category::Parameter, RigidPointCloudTransform));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RigidPointCloudTransform::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RigidPointCloudTransform::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0) { return; }

  if (getCancel()) { return; }

  if (getWarningCode() < 0)
  {
    QString ss = QObject::tr("Some warning message");
    setWarningCondition(-88888888, ss);
  }

  if (getErrorCode() < 0)
  {
    QString ss = QObject::tr("Some error message");
    setErrorCondition(-99999999, ss);
    return;
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RigidPointCloudTransform::newFilterInstance(bool copyFilterParameters) const
{
  RigidPointCloudTransform::Pointer filter = RigidPointCloudTransform::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RigidPointCloudTransform::getCompiledLibraryName() const
{ 
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RigidPointCloudTransform::getBrandingString() const
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RigidPointCloudTransform::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RigidPointCloudTransform::getGroupName() const
{ 
  return SIMPL::FilterGroups::Unsupported; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RigidPointCloudTransform::getSubGroupName() const
{ 
  return "SurfaceMeshing"; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RigidPointCloudTransform::getHumanLabel() const
{ 
  return "RigidPointCloudTransform"; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid RigidPointCloudTransform::getUuid() const
{
  return QUuid("{b254dd43-6234-5c44-8231-4545cf3845bc}");
}

// -----------------------------------------------------------------------------
RigidPointCloudTransform::Pointer RigidPointCloudTransform::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<RigidPointCloudTransform> RigidPointCloudTransform::New()
{
  struct make_shared_enabler : public RigidPointCloudTransform
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString RigidPointCloudTransform::getNameOfClass() const
{
  return QString("RigidPointCloudTransform");
}

// -----------------------------------------------------------------------------
QString RigidPointCloudTransform::ClassName()
{
  return QString("RigidPointCloudTransform");
}


// -----------------------------------------------------------------------------
void RigidPointCloudTransform::setMovingGeometry(const DataArrayPath& value)
{
    m_MovingGeometry = value;
}
// -----------------------------------------------------------------------------
DataArrayPath RigidPointCloudTransform::getMovingGeometry() const
{
  return m_MovingGeometry;
}
// -----------------------------------------------------------------------------
void RigidPointCloudTransform::setMovingKeyPoints(const DynamicTableData& value)
{
    m_MovingKeyPoints = value;
}
// -----------------------------------------------------------------------------
DynamicTableData RigidPointCloudTransform::getMovingKeyPoints() const
{
  return m_MovingKeyPoints;
}
// -----------------------------------------------------------------------------
void RigidPointCloudTransform::setFixedKeyPoints(const DynamicTableData& value)
{
    m_FixedKeyPoints = value;
}
// -----------------------------------------------------------------------------
DynamicTableData RigidPointCloudTransform::getFixedKeyPoints() const
{
  return m_FixedKeyPoints;
}


