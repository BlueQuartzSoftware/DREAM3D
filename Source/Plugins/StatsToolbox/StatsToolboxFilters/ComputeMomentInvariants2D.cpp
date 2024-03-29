/*
 * Your License or Copyright can go here
 */

#include "ComputeMomentInvariants2D.h"

#include <Eigen/Dense>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "StatsToolbox/StatsToolboxConstants.h"
#include "StatsToolbox/StatsToolboxFilters/util/MomentInvariants2D.h"
#include "StatsToolbox/StatsToolboxVersion.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID30 = 30,
  DataArrayID31 = 31,
  DataArrayID32 = 32,
  DataArrayID33 = 33,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComputeMomentInvariants2D::ComputeMomentInvariants2D()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComputeMomentInvariants2D::~ComputeMomentInvariants2D() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeMomentInvariants2D::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeMomentInvariants2D::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    AttributeMatrix::Types amTypes = {AttributeMatrix::Type::Cell};
    req.amTypes = amTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::Category::RequiredArray, ComputeMomentInvariants2D, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 6, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    AttributeMatrix::Types amTypes = {AttributeMatrix::Type::CellFeature};
    req.amTypes = amTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Rect", FeatureRectArrayPath, FilterParameter::Category::RequiredArray, ComputeMomentInvariants2D, req));
  }

  parameters.push_back(SIMPL_NEW_BOOL_FP("Normalize Moment Invariants", NormalizeMomentInvariants, FilterParameter::Category::Parameter, ComputeMomentInvariants2D));

  parameters.push_back(SeparatorFilterParameter::Create("Cell Feature Data", FilterParameter::Category::CreatedArray));
  DataArrayCreationFilterParameter::RequirementType dacReq;
  dacReq.amTypes = {AttributeMatrix::Type::CellFeature};
  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Omega 1", Omega1ArrayPath, FilterParameter::Category::CreatedArray, ComputeMomentInvariants2D, dacReq));
  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Omega 2", Omega2ArrayPath, FilterParameter::Category::CreatedArray, ComputeMomentInvariants2D, dacReq));

  std::vector<QString> linkedProps;
  linkedProps.push_back("CentralMomentsArrayPath");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Save Central Moments", SaveCentralMoments, FilterParameter::Category::Parameter, ComputeMomentInvariants2D, linkedProps));
  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Central Moments", CentralMomentsArrayPath, FilterParameter::Category::CreatedArray, ComputeMomentInvariants2D, dacReq));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeMomentInvariants2D::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  IGeometry::Pointer igeom = getDataContainerArray()->getPrereqGeometryFromDataContainer<IGeometry>(this, getFeatureIdsArrayPath().getDataContainerName());
  if(nullptr == igeom.get())
  {
    QString ss = QObject::tr("The ImageGeometry or DataContainer for %1 does not exist or is invalid.").arg(getFeatureIdsArrayPath().getDataContainerName());
    setErrorCondition(-73001, ss);
    return;
  }
  ImageGeom::Pointer imageGeom = std::dynamic_pointer_cast<ImageGeom>(igeom);
  SizeVec3Type imageDims = imageGeom->getDimensions();
  if(imageDims[2] != 1)
  {
    QString ss = QObject::tr("This filter currently only works on XY Planes in 2D data. Either crop the 3D data down to 2D in the Z Direction or use other data.");
    setErrorCondition(-73000, ss);
  }

  std::vector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getFeatureIdsArrayPath(), cDims);
  if(nullptr != m_FeatureIdsPtr.lock())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }

  cDims[0] = 6;
  m_FeatureRectPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>>(this, getFeatureRectArrayPath(), cDims);
  if(nullptr != m_FeatureRectPtr.lock())
  {
    m_FeatureRect = m_FeatureRectPtr.lock()->getPointer(0);
  }

  cDims[0] = 1;
  m_Omega1Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(this, getOmega1ArrayPath(), 0.0f, cDims, "", DataArrayID31);
  if(nullptr != m_Omega1Ptr.lock())
  {
    m_Omega1 = m_Omega1Ptr.lock()->getPointer(0);
  }
  m_Omega2Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(this, getOmega2ArrayPath(), 0.0f, cDims, "", DataArrayID32);
  if(nullptr != m_Omega2Ptr.lock())
  {
    m_Omega2 = m_Omega2Ptr.lock()->getPointer(0);
  }

  cDims.resize(2);
  cDims[0] = 3;
  cDims[1] = 3;
  if(getSaveCentralMoments())
  {
    m_CentralMomentsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(this, getCentralMomentsArrayPath(), 0.0f, cDims, "", DataArrayID33);
  }
  if(nullptr != m_CentralMomentsPtr.lock())
  {
    m_CentralMoments = m_CentralMomentsPtr.lock()->getPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeMomentInvariants2D::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getCancel())
  {
    return;
  }

  IGeometry::Pointer igeom = getDataContainerArray()->getPrereqGeometryFromDataContainer<IGeometry>(this, getFeatureIdsArrayPath().getDataContainerName());

  ImageGeom::Pointer imageGeom = std::dynamic_pointer_cast<ImageGeom>(igeom);
  SizeVec3Type volDims = imageGeom->getDimensions();

  Int32ArrayType::Pointer featureIdArray = m_FeatureIdsPtr.lock();
  int32_t numFeatures = static_cast<int32_t>(m_FeatureRectPtr.lock()->getNumberOfTuples());

  //  static const double k_Pi14 = std::pow(SIMPLib::Constants::k_Pi, 0.25);
  //  static const double k_Root2 = std::sqrt(2.0);

  // This loop SHOULD be parallelized

  for(int32_t featureId = 1; featureId < numFeatures; featureId++)
  {
    uint32_t* corner = m_FeatureRectPtr.lock()->getTuplePointer(featureId);
    MomentInvariants2D moments;
    size_t max_order = 2;
    // size_t mDim = max_order + 1;

    // Figure the largest X || Y dimension so we can create a square matrix
    uint32_t xDim = corner[3] - corner[0] + 1;
    uint32_t yDim = corner[4] - corner[1] + 1;
    uint32_t zDim = corner[5] - corner[2] + 1;

    if(zDim != 1)
    {
      m_Omega1[featureId] = 0.0f;
      m_Omega2[featureId] = 0.0f;
      QString ss = QObject::tr("Feature %1 is NOT strictly 2D in the XY plane. Skipping this feature.").arg(featureId);
      setWarningCondition(-3000, ss);
      continue;
    }

    size_t dim = xDim; // Assume XDim is the largest value
    if(yDim > xDim)
    {
      dim = yDim; // Nope, YDim is largest
    }

    MomentInvariants2D::DoubleMatrixType input2D(dim, dim);
    input2D.setZero();

    uint32_t height = 0;

    for(uint32_t y = corner[1]; y <= corner[4]; y++)
    {
      for(uint32_t x = corner[0]; x <= corner[3]; x++)
      {
        size_t index = (volDims[1] * volDims[0] * height) + (volDims[0] * y) + x;
        if(*(m_FeatureIds + index) == featureId)
        {
          input2D(y - corner[1], x - corner[0]) = 1;
        }
        else
        {
          input2D(y - corner[1], x - corner[0]) = 0;
        }
      }
    }

    size_t inputDims[2] = {dim, dim};
    MomentInvariants2D::DoubleMatrixType m2D = moments.computeMomentInvariants(input2D, inputDims, max_order);
    // std::cout << "Central Moments=\n" << m2D << std::endl;
    // compute the second order moment invariants
    double omega1 = 2.0 * (m2D(0, 0) * m2D(0, 0)) / (m2D(0, 2) + m2D(2, 0));
    double omega2 = std::pow(m2D(0, 0), 4) / (m2D(2, 0) * m2D(0, 2) - std::pow(m2D(1, 1), 2));
    // std::cout << ",'2D moment invariants : " << omega1 << "\t" << omega2 << " (should be 12 and 144)" << std::endl;

    if(getNormalizeMomentInvariants())
    {
      // normalize the invariants by those of the circle
      double circle_omega[2] = {4.0 * M_PI, 16.0 * M_PI * M_PI};
      omega1 /= circle_omega[0];
      omega2 /= circle_omega[1];
      //    std::cout << "normalized moment invariants: " << omega1 << "\t" << omega2 << std::endl;
    }
    m_Omega1[featureId] = static_cast<float>(omega1);
    m_Omega2[featureId] = static_cast<float>(omega2);

    if(getSaveCentralMoments())
    {
      double* m2DInternal = m2D.array().data();
      for(size_t comp = 0; comp < 9; comp++)
      {
        m_CentralMoments[static_cast<size_t>(featureId) * 9UL + comp] = static_cast<float>(m2DInternal[comp]);
      }
    }

#if 0
/** This section computes the Ellipse Major and Minor Axis lengths. As a double check if the
 * aspect ratio is > 1.0 then the shape is NOT an ellipse.
 */
    double ellipseAspctRatio = std::sqrt((m2D(0,2)/m2D(2,0)));
    double ellipseMajorAxis = (k_Root2 * std::pow(m2D(2,0), 3.0/8.0) ) / (std::pow(m2D(0,2), (1.0/8.0)) * k_Pi14);
    double ellipseMinorAxis = ellipseMajorAxis / ellipseAspctRatio;
    if(ellipseMinorAxis > ellipseMajorAxis)
    {
        std::cout << featureId << " " << omega1 << ", " << omega2 << "    " << ellipseMajorAxis << "," << ellipseMinorAxis << std::endl;
    }
#endif

    QString ss = QObject::tr("[%1/%2] Completed:").arg(featureId).arg(numFeatures);
    notifyStatusMessage(ss);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ComputeMomentInvariants2D::newFilterInstance(bool copyFilterParameters) const
{
  ComputeMomentInvariants2D::Pointer filter = ComputeMomentInvariants2D::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComputeMomentInvariants2D::getCompiledLibraryName() const
{
  return StatsToolboxConstants::StatsToolboxBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComputeMomentInvariants2D::getBrandingString() const
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComputeMomentInvariants2D::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << StatsToolbox::Version::Major() << "." << StatsToolbox::Version::Minor() << "." << StatsToolbox::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComputeMomentInvariants2D::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ComputeMomentInvariants2D::getUuid() const
{
  return QUuid("{27a132b2-a592-519a-8cb7-38599a7f28ec}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComputeMomentInvariants2D::getSubGroupName() const
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComputeMomentInvariants2D::getHumanLabel() const
{
  return "Compute MomentInvariants (2D)";
}

// -----------------------------------------------------------------------------
ComputeMomentInvariants2D::Pointer ComputeMomentInvariants2D::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ComputeMomentInvariants2D> ComputeMomentInvariants2D::New()
{
  struct make_shared_enabler : public ComputeMomentInvariants2D
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ComputeMomentInvariants2D::getNameOfClass() const
{
  return QString("ComputeMomentInvariants2D");
}

// -----------------------------------------------------------------------------
QString ComputeMomentInvariants2D::ClassName()
{
  return QString("ComputeMomentInvariants2D");
}

// -----------------------------------------------------------------------------
void ComputeMomentInvariants2D::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ComputeMomentInvariants2D::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void ComputeMomentInvariants2D::setFeatureRectArrayPath(const DataArrayPath& value)
{
  m_FeatureRectArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ComputeMomentInvariants2D::getFeatureRectArrayPath() const
{
  return m_FeatureRectArrayPath;
}

// -----------------------------------------------------------------------------
void ComputeMomentInvariants2D::setNormalizeMomentInvariants(bool value)
{
  m_NormalizeMomentInvariants = value;
}

// -----------------------------------------------------------------------------
bool ComputeMomentInvariants2D::getNormalizeMomentInvariants() const
{
  return m_NormalizeMomentInvariants;
}

// -----------------------------------------------------------------------------
void ComputeMomentInvariants2D::setOmega1ArrayPath(const DataArrayPath& value)
{
  m_Omega1ArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ComputeMomentInvariants2D::getOmega1ArrayPath() const
{
  return m_Omega1ArrayPath;
}

// -----------------------------------------------------------------------------
void ComputeMomentInvariants2D::setOmega2ArrayPath(const DataArrayPath& value)
{
  m_Omega2ArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ComputeMomentInvariants2D::getOmega2ArrayPath() const
{
  return m_Omega2ArrayPath;
}

// -----------------------------------------------------------------------------
void ComputeMomentInvariants2D::setSaveCentralMoments(bool value)
{
  m_SaveCentralMoments = value;
}

// -----------------------------------------------------------------------------
bool ComputeMomentInvariants2D::getSaveCentralMoments() const
{
  return m_SaveCentralMoments;
}

// -----------------------------------------------------------------------------
void ComputeMomentInvariants2D::setCentralMomentsArrayPath(const DataArrayPath& value)
{
  m_CentralMomentsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ComputeMomentInvariants2D::getCentralMomentsArrayPath() const
{
  return m_CentralMomentsArrayPath;
}
