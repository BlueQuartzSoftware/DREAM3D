/*
 * Your License or Copyright can go here
 */

#include "ComputeFeatureRect.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "Reconstruction/ReconstructionConstants.h"
#include "Reconstruction/ReconstructionVersion.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID30 = 30,
  DataArrayID31 = 31,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComputeFeatureRect::ComputeFeatureRect()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComputeFeatureRect::~ComputeFeatureRect() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeFeatureRect::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeFeatureRect::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    AttributeMatrix::Types amTypes = {AttributeMatrix::Type::Cell};
    req.amTypes = amTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::Category::RequiredArray, ComputeFeatureRect, req));
  }

  parameters.push_back(SeparatorFilterParameter::Create("Cell Feature Data", FilterParameter::Category::CreatedArray));

  DataArrayCreationFilterParameter::RequirementType dacReq;
  dacReq.amTypes = {AttributeMatrix::Type::CellFeature};
  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Feature Rect", FeatureRectArrayPath, FilterParameter::Category::CreatedArray, ComputeFeatureRect, dacReq));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeFeatureRect::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  IGeometry::Pointer igeom = getDataContainerArray()->getPrereqGeometryFromDataContainer<IGeometry>(this, getFeatureIdsArrayPath().getDataContainerName());

  std::vector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getFeatureIdsArrayPath(), cDims);
  if(nullptr != m_FeatureIdsPtr.lock())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }

  cDims[0] = 6;
  m_FeatureRectPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>>(this, getFeatureRectArrayPath(), 0, cDims, "", DataArrayID31);
  if(nullptr != m_FeatureRectPtr.lock())
  {
    m_FeatureRect = m_FeatureRectPtr.lock()->getPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeFeatureRect::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  int featureId = 0;
  size_t numComps = 6;
  std::vector<size_t> cDims(1, numComps);
  int err = 0;
  AttributeMatrix::Pointer featureAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, m_FeatureRectArrayPath, err);

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
  std::vector<size_t> imageDims = featureIdsAM->getTupleDimensions();
  size_t xDim = imageDims[0], yDim = imageDims[1], zDim = imageDims[2];

  size_t index = 0;
  // Store the coordinates in the corners array
  for(uint32_t z = 0; z < zDim; z++)
  {
    if(getCancel())
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
          QString ss = QObject::tr("The feature attribute matrix '%1' has a smaller tuple count than the maximum feature id in '%2'").arg(featureAM->getName()).arg(cellFeatureIds->getName());
          setErrorCondition(-31000, ss);
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
}

// -----------------------------------------------------------------------------
// Helper Method - Grabs Index From Matrix Coordinates
// -----------------------------------------------------------------------------
size_t ComputeFeatureRect::sub2ind(std::vector<size_t> tDims, size_t x, size_t y, size_t z) const
{
  return (tDims[1] * tDims[0] * z) + (tDims[0] * y) + x;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ComputeFeatureRect::newFilterInstance(bool copyFilterParameters) const
{
  ComputeFeatureRect::Pointer filter = ComputeFeatureRect::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComputeFeatureRect::getCompiledLibraryName() const
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComputeFeatureRect::getBrandingString() const
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComputeFeatureRect::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Reconstruction::Version::Major() << "." << Reconstruction::Version::Minor() << "." << Reconstruction::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComputeFeatureRect::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ComputeFeatureRect::getUuid() const
{
  return QUuid("{6eda8dbf-dbd8-562a-ae1a-f2904157c189}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComputeFeatureRect::getSubGroupName() const
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComputeFeatureRect::getHumanLabel() const
{
  return "Compute Feature Corners";
}

// -----------------------------------------------------------------------------
ComputeFeatureRect::Pointer ComputeFeatureRect::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ComputeFeatureRect> ComputeFeatureRect::New()
{
  struct make_shared_enabler : public ComputeFeatureRect
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ComputeFeatureRect::getNameOfClass() const
{
  return QString("ComputeFeatureRect");
}

// -----------------------------------------------------------------------------
QString ComputeFeatureRect::ClassName()
{
  return QString("ComputeFeatureRect");
}

// -----------------------------------------------------------------------------
void ComputeFeatureRect::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ComputeFeatureRect::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void ComputeFeatureRect::setFeatureRectArrayPath(const DataArrayPath& value)
{
  m_FeatureRectArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ComputeFeatureRect::getFeatureRectArrayPath() const
{
  return m_FeatureRectArrayPath;
}
