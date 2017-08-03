/*
 * Your License or Copyright can go here
 */

#include "ComputeFeatureRect.h"

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "Reconstruction/ReconstructionConstants.h"
#include "Reconstruction/ReconstructionVersion.h"

// Include the MOC generated file for this class
#include "moc_ComputeFeatureRect.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComputeFeatureRect::ComputeFeatureRect()
  : AbstractFilter()
  , m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
  , m_FeatureRectArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, "FeatureRect")
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComputeFeatureRect::~ComputeFeatureRect()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeFeatureRect::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeFeatureRect::setupFilterParameters()
{
  FilterParameterVector parameters;

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    AttributeMatrix::Types amTypes = {AttributeMatrix::Type::Cell};
    req.amTypes = amTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, ComputeFeatureRect, req));
  }

  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));

  DataArrayCreationFilterParameter::RequirementType dacReq;
  dacReq.amTypes = {AttributeMatrix::Type::CellFeature};
  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Feature Rect", FeatureRectArrayPath, FilterParameter::CreatedArray, ComputeFeatureRect, dacReq));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeFeatureRect::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  IGeometry::Pointer igeom = getDataContainerArray()->getPrereqGeometryFromDataContainer<IGeometry, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims);
  if(nullptr != m_FeatureIdsPtr.lock().get())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }

  cDims[0] = 6;
  m_FeatureRectPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this, getFeatureRectArrayPath(), 0, cDims);
  if(nullptr != m_FeatureRectPtr.lock().get())
  {
    m_FeatureRect = m_FeatureRectPtr.lock()->getPointer(0);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeFeatureRect::preflight()
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
void ComputeFeatureRect::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }



  int featureId = 0;
  size_t numComps = 6;
  QVector<size_t> cDims(1, numComps);
  int err = 0;
  AttributeMatrix::Pointer featureAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, m_FeatureRectArrayPath, err);

  Int32ArrayType::Pointer cellFeatureIds = m_FeatureIdsPtr.lock();

  // Create corners array, which stores pixel coordinates for the top-left and bottom-right coordinates of each feature object
  UInt32ArrayType::Pointer corners = m_FeatureRectPtr.lock();
  for(size_t i = 0; i < corners->getNumberOfTuples(); i++)
  {
    corners->setComponent(i, 0, std::numeric_limits<uint32_t>::max());
    corners->setComponent(i, 1, std::numeric_limits<uint32_t>::max());
    corners->setComponent(i, 2, std::numeric_limits<uint32_t>::max());
    corners->setComponent(i, 3, std::numeric_limits<uint32_t>::min());
    corners->setComponent(i, 4, std::numeric_limits<uint32_t>::min());
    corners->setComponent(i, 5, std::numeric_limits<uint32_t>::min());
  }
  AttributeMatrix::Pointer featureIdsAM = getDataContainerArray()->getAttributeMatrix(m_FeatureIdsArrayPath);
  QVector<size_t> imageDims = featureIdsAM->getTupleDimensions();
  size_t xDim = imageDims[0], yDim = imageDims[1], zDim = imageDims[2];

  size_t index = 0;
  // Store the coordinates in the corners array
  for(uint32_t z = 0; z < zDim; z++)
  {
    if(getCancel() == true)
    {
      return;
    }

    for(uint32_t y = 0; y < yDim; y++)
    {
      for(uint32_t x = 0; x < xDim; x++)
      {
        index = sub2ind(imageDims, x, y, z); // Index into cellFeatureIds array

        featureId = m_FeatureIds[index];
        if(featureId == 0)
        {
          continue;
        }

        if(featureId >= corners->getNumberOfTuples())
        {
          setErrorCondition(-31000);
          QString ss = QObject::tr("The feature attribute matrix '%1' has a smaller tuple count than the maximum feature id in '%2'").arg(featureAM->getName()).arg(cellFeatureIds->getName());
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
          return;
        }

        uint32_t* featureCorner = corners->getPointer(featureId * numComps);

        uint32_t val = featureCorner[0];
        if(x < featureCorner[0])
        {
          featureCorner[0] = x;
        }
        val = featureCorner[1];
        if(y < featureCorner[1])
        {
          featureCorner[1] = y;
        }
        val = featureCorner[2];
        if(z < featureCorner[2])
        {
          featureCorner[2] = z;
        }

        val = featureCorner[3];
        if(x > featureCorner[3])
        {
          featureCorner[3] = x;
        }
        val = featureCorner[4];
        if(y > featureCorner[4])
        {
          featureCorner[4] = y;
        }
        val = featureCorner[5];
        if(z > featureCorner[5])
        {
          featureCorner[5] = z;
        }
      }
    }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
// Helper Method - Grabs Index From Matrix Coordinates
// -----------------------------------------------------------------------------
size_t ComputeFeatureRect::sub2ind(QVector<size_t> tDims, size_t x, size_t y, size_t z) const
{
  return (tDims[1] * tDims[0] * z) + (tDims[0] * y) + x;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ComputeFeatureRect::newFilterInstance(bool copyFilterParameters)
{
  ComputeFeatureRect::Pointer filter = ComputeFeatureRect::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeFeatureRect::getCompiledLibraryName()
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeFeatureRect::getBrandingString()
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeFeatureRect::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << Reconstruction::Version::Major() << "." << Reconstruction::Version::Minor() << "." << Reconstruction::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeFeatureRect::getGroupName()
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeFeatureRect::getSubGroupName()
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ComputeFeatureRect::getHumanLabel()
{
  return "Compute Feature Corners";
}
