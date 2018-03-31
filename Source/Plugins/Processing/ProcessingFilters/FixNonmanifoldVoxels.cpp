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

  DataArrayPath featurePath = getFeatureIdsArrayPath();
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(featurePath.getDataContainerName());

  size_t udims[3] = {0, 0, 0};
  std::tie(udims[0], udims[1], udims[2]) = m->getGeometryAs<ImageGeom>()->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]),
      static_cast<int64_t>(udims[1]),
      static_cast<int64_t>(udims[2]),
  };

  int64_t maxId = dims[0] * dims[1] * dims[2];
  int64_t plane = (dims[0] * dims[1]);
  int64_t row = (dims[0]);

  int64_t neighpointsIndices[20] = {
      // Top Plane of indices
      -plane - row - 1,
      -plane - row,
      -plane - row + 1,
      -plane - 1,
      -plane + 1,
      -plane + row - 1,
      -plane + row,
      -plane + row + 1,
      // Middle plane of indices
      -row - 1,
      -row + 1,

      row - 1,
      row + 1,
      // Bottom Plane of indices
      +plane - row - 1,
      +plane - row,
      +plane - row + 1,
      +plane - 1,
      +plane + 1,
      +plane + row - 1,
      +plane + row,
      +plane + row + 1,
  };
  std::cout << "Dims: " << dims[0] << ", " << dims[1] << ", " << dims[2] << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << neighpointsIndices[0] << " " << neighpointsIndices[1] << " " << neighpointsIndices[2] << std::endl;
  std::cout << neighpointsIndices[3] << " "
            << " XX " << neighpointsIndices[4] << std::endl;
  std::cout << neighpointsIndices[5] << " " << neighpointsIndices[6] << " " << neighpointsIndices[7] << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << neighpointsIndices[8] << " "
            << " XX "
            << " " << neighpointsIndices[9] << std::endl;
  std::cout << " XX "
            << " "
            << " XX "
            << " "
            << " XX " << std::endl;
  std::cout << neighpointsIndices[10] << " "
            << " XX "
            << " " << neighpointsIndices[11] << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << neighpointsIndices[12] << " " << neighpointsIndices[13] << " " << neighpointsIndices[14] << std::endl;
  std::cout << neighpointsIndices[15] << " "
            << " XX "
            << " " << neighpointsIndices[16] << std::endl;
  std::cout << neighpointsIndices[17] << " " << neighpointsIndices[18] << " " << neighpointsIndices[19] << std::endl;

  for(int64_t z = 0; z < dims[2]; z++)
  {

    for(int64_t y = 0; y < dims[1]; y++)
    {
      if(getCancel())
      {
        break;
      }
      for(int64_t x = 0; x < dims[0]; x++)
      {
        int64_t curIdx = (z * plane) + (y * row) + x;
        int32_t featureId = m_FeatureIds[static_cast<size_t>(curIdx)];

        for(size_t i = 0; i < 20; i++)
        {
          int64_t neighIdx = curIdx + neighpointsIndices[i];
          // Validate we are within the array bounds
          if(neighIdx >= 0 && neighIdx < maxId)
          {
            int32_t neighFeatId = m_FeatureIds[static_cast<size_t>(neighIdx)];
            if(neighFeatId == featureId)
            {
              // std::cout << "Nonmanifold Voxel at " << x << ", " << y << ", " << z << " Feature Id: " << featureId << std::endl;
            }
          }
        }
      }
    }
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
