/*
 * Your License or Copyright can go here
 */

#include "ComputeMomentInvariants2D.h"

#include <Eigen/Dense>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsFilters/util/MomentInvariants2D.h"
#include "Statistics/StatisticsVersion.h"

// Include the MOC generated file for this class
#include "moc_ComputeMomentInvariants2D.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComputeMomentInvariants2D::ComputeMomentInvariants2D()
  : AbstractFilter()
  , m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
  , m_FeatureRectArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, "FeatureRect")
  , m_NormalizeMomentInvariants(true)
  , m_Omega1ArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, "Omega1")
  , m_Omega2ArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, "Omega2")
  , m_SaveCentralMoments(false)
  , m_CentralMomentsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, "CentralMoments")
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComputeMomentInvariants2D::~ComputeMomentInvariants2D()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeMomentInvariants2D::initialize()
{
  setErrorCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeMomentInvariants2D::setupFilterParameters()
{
  FilterParameterVector parameters;

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    AttributeMatrix::Types amTypes = {AttributeMatrix::Type::Cell};
    req.amTypes = amTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, ComputeMomentInvariants2D, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 6, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    AttributeMatrix::Types amTypes = {AttributeMatrix::Type::CellFeature};
    req.amTypes = amTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Rect", FeatureRectArrayPath, FilterParameter::RequiredArray, ComputeMomentInvariants2D, req));
  }

  parameters.push_back(SIMPL_NEW_BOOL_FP("Normalize Moment Invariants", NormalizeMomentInvariants, FilterParameter::Parameter, ComputeMomentInvariants2D));


  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  DataArrayCreationFilterParameter::RequirementType dacReq;
  dacReq.amTypes = {AttributeMatrix::Type::CellFeature};
  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Omega 1", Omega1ArrayPath, FilterParameter::CreatedArray, ComputeMomentInvariants2D, dacReq));
  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Omega 2", Omega2ArrayPath, FilterParameter::CreatedArray, ComputeMomentInvariants2D, dacReq));

  QStringList linkedProps;
  linkedProps << "CentralMomentsArrayPath";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Save Central Moments", SaveCentralMoments, FilterParameter::Parameter, ComputeMomentInvariants2D, linkedProps));
  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Central Moments", CentralMomentsArrayPath, FilterParameter::CreatedArray, ComputeMomentInvariants2D, dacReq));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeMomentInvariants2D::dataCheck()
{
  setErrorCondition(0);

  IGeometry::Pointer igeom = getDataContainerArray()->getPrereqGeometryFromDataContainer<IGeometry, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims);
  if(nullptr != m_FeatureIdsPtr.lock().get())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }

  cDims[0] = 6;
  m_FeatureRectPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getFeatureRectArrayPath(), cDims);
  if(nullptr != m_FeatureRectPtr.lock().get())
  {
    m_FeatureRect = m_FeatureRectPtr.lock()->getPointer(0);
  }

  cDims[0] = 1;
  m_Omega1Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getOmega1ArrayPath(), 0.0f, cDims);
  if(nullptr != m_Omega1Ptr.lock().get())
  {
    m_Omega1 = m_Omega1Ptr.lock()->getPointer(0);
  }
  m_Omega2Ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getOmega2ArrayPath(), 0.0f, cDims);
  if(nullptr != m_Omega2Ptr.lock().get())
  {
    m_Omega2 = m_Omega2Ptr.lock()->getPointer(0);
  }

  cDims.resize(2);
  cDims[0] = 3;
  cDims[1] = 3;
  if(getSaveCentralMoments())
  {
    m_CentralMomentsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getCentralMomentsArrayPath(), 0.0f, cDims);
  }
  if(nullptr != m_CentralMomentsPtr.lock().get())
  {
    m_CentralMoments = m_CentralMomentsPtr.lock()->getPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeMomentInvariants2D::preflight()
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
void ComputeMomentInvariants2D::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(getCancel() == true)
  {
    return;
  }

  IGeometry::Pointer igeom = getDataContainerArray()->getPrereqGeometryFromDataContainer<IGeometry, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  ImageGeom::Pointer imageGeom = std::dynamic_pointer_cast<ImageGeom>(igeom);
  size_t volDims[3] = { 0, 0, 0 };
  imageGeom->getDimensions(volDims);

  Int32ArrayType::Pointer featureIdArray = m_FeatureIdsPtr.lock();
  int32_t numFeatures = static_cast<int32_t>(m_FeatureRectPtr.lock()->getNumberOfTuples());

  static const double k_Pi14 = std::pow(SIMPLib::Constants::k_Pi, 0.25);
  static const double k_Root2 = std::sqrt(2.0);

  // This loop SHOULD be parallelized

  for(int32_t featureId = 1; featureId < numFeatures; featureId++)
  {
    uint32_t* corner = m_FeatureRectPtr.lock()->getTuplePointer(featureId);
    MomentInvariants2D moments;
    size_t max_order = 2;
    //size_t mDim = max_order + 1;

    // Figure the largest X || Y dimension so we can create a square matrix
    uint32_t xDim = corner[3] - corner[0] + 1;
    uint32_t yDim = corner[4] - corner[1] + 1;
    uint32_t zDim = corner[5] - corner[2] + 1;

    if(zDim != 1)
    {
      m_Omega1[featureId] = 0.0f;
      m_Omega2[featureId] = 0.0f;
      QString ss = QObject::tr("Feature %1 is NOT strictly 2D in the XY plane. Skipping this feature.").arg(featureId);
      setErrorCondition(0);
      notifyWarningMessage(getHumanLabel(), ss, getErrorCondition());
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
        size_t index =  (volDims[1] * volDims[0] * height) + (volDims[0] * y) + x;
        if( *(m_FeatureIds + index) == featureId)
        {
          input2D(y - corner[1], x - corner[0]) = 1;
        }
        else
        {
          input2D(y - corner[1], x - corner[0]) = 0;
        }
      }
    }

    size_t inputDims[2] = { dim, dim };
    MomentInvariants2D::DoubleMatrixType m2D = moments.computeMomentInvariants(input2D, inputDims, max_order);
    //std::cout << "Central Moments=\n" << m2D << std::endl;
    // compute the second order moment invariants
    double omega1 = 2.0 * (m2D(0,0)* m2D(0,0)) / (m2D(0,2) + m2D(2,0));
    double omega2 = std::pow(m2D(0,0), 4) / ( m2D(2,0)*m2D(0,2) - std::pow(m2D(1,1), 2) );
    //std::cout << ",'2D moment invariants : " << omega1 << "\t" << omega2 << " (should be 12 and 144)" << std::endl;

    if(getNormalizeMomentInvariants())
    {
      // normalize the invariants by those of the circle
      double circle_omega[2] = { 4.0*M_PI, 16.0 * M_PI*M_PI };
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
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ComputeMomentInvariants2D::newFilterInstance(bool copyFilterParameters)
{
  ComputeMomentInvariants2D::Pointer filter = ComputeMomentInvariants2D::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeMomentInvariants2D::getCompiledLibraryName()
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeMomentInvariants2D::getBrandingString()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeMomentInvariants2D::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeMomentInvariants2D::getGroupName()
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeMomentInvariants2D::getSubGroupName()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeMomentInvariants2D::getHumanLabel()
{
  return "Compute MomentInvariants (2D)";
}
