/* ============================================================================
 * Software developed by US federal government employees (including military personnel)
 * as part of their official duties is not subject to copyright protection and is
 * considered "public domain" (see 17 USC Section 105). Public domain software can be used
 * by anyone for any purpose, and cannot be released under a copyright license
 * (including typical open source software licenses).
 *
 * This source code file was originally written by United States DoD employees. The
 * original source code files are released into the Public Domain.
 *
 * Subsequent changes to the codes by others may elect to add a copyright and license
 * for those changes.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "LabelTriangleGeometry.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"

#include "SIMPLib/Geometry/TriangleGeom.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataArrayID30 = 30,
  DataArrayID31 = 31,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LabelTriangleGeometry::LabelTriangleGeometry() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LabelTriangleGeometry::~LabelTriangleGeometry() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LabelTriangleGeometry::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  DataContainerSelectionFilterParameter::RequirementType dcsReq;
  IGeometry::Types geomTypes = {IGeometry::Type::Triangle};
  dcsReq.dcGeometryTypes = geomTypes;
  parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("CAD Geometry", CADDataContainerPath, FilterParameter::Category::RequiredArray, LabelTriangleGeometry, dcsReq));

  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Region Ids", RegionIdArrayPath, FilterParameter::Category::CreatedArray, LabelTriangleGeometry, req));
  }

  {
    parameters.push_back(
        SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Feature Attribute Matrix", TriangleAttributeMatrixName, CADDataContainerPath, FilterParameter::Category::CreatedArray, LabelTriangleGeometry));
    parameters.push_back(
        SIMPL_NEW_DA_WITH_LINKED_AM_FP("NumTriangles", NumTrianglesArrayName, CADDataContainerPath, TriangleAttributeMatrixName, FilterParameter::Category::CreatedArray, LabelTriangleGeometry));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LabelTriangleGeometry::updateTriangleInstancePointers()
{
  clearErrorCode();
  clearWarningCode();

  if(nullptr != m_RegionIdPtr.lock())
  {
    m_RegionId = m_RegionIdPtr.lock()->getPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LabelTriangleGeometry::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  DataArrayPath tempPath;

  TriangleGeom::Pointer triangle = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom>(this, getCADDataContainerPath());
  std::vector<size_t> cDims(1, 1);
  std::vector<size_t> tDims(1, 0);
  auto m = getDataContainerArray()->getDataContainer(getCADDataContainerPath().getDataContainerName());
  if(nullptr != m)
  {
    m->createNonPrereqAttributeMatrix(this, getTriangleAttributeMatrixName(), tDims, AttributeMatrix::Type::FaceFeature);
  }

  if(getErrorCode() < 0)
  {
    return;
  }

  tempPath.update(getCADDataContainerPath().getDataContainerName(), getTriangleAttributeMatrixName(), getNumTrianglesArrayName());
  m_NumTrianglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint64_t>>(this, tempPath, 0, cDims, "", DataArrayID30);
  if(nullptr != m_NumTrianglesPtr.lock())
  {
    m_NumTriangles = m_NumTrianglesPtr.lock()->getPointer(0);
  }
  if(getErrorCode() < 0)
  {
    return;
  }

  m_RegionIdPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>>(this, getRegionIdArrayPath(), 0, cDims, "", DataArrayID31);
  if(nullptr != m_RegionIdPtr.lock())
  {
    m_RegionId = m_RegionIdPtr.lock()->getPointer(0);
  }
  if(getErrorCode() < 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LabelTriangleGeometry::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  TriangleGeom::Pointer triangle = getDataContainerArray()->getDataContainer(getCADDataContainerPath())->getGeometryAs<TriangleGeom>();

  size_t numTris = triangle->getNumberOfTris();

  DataContainer::Pointer dataContainerCAD = getDataContainerArray()->getDataContainer(getCADDataContainerPath());

  int check = triangle->findElementNeighbors();
  if(check < 0)
  {
    QString ss = "Error finding element neighbors";
    setErrorCondition(check, ss);
    return;
  }

  ElementDynamicList::Pointer m_TriangleNeighbors = triangle->getElementNeighbors();

  size_t chunkSize = 1000;
  std::vector<int32_t> triList(chunkSize, -1);
  std::vector<uint32_t> triangleCounts = {0, 0};
  // first identify connected triangle sets as features
  size_t size = 0;
  int32_t regionCount = 1;
  for(size_t i = 0; i < numTris; i++)
  {
    if(m_RegionId[i] == 0)
    {
      m_RegionId[i] = regionCount;
      triangleCounts[regionCount]++;

      size = 0;
      triList[size] = i;
      size++;
      while(size > 0)
      {
        MeshIndexType tri = triList[size - 1];
        size -= 1;
        uint16_t tCount = m_TriangleNeighbors->getNumberOfElements(tri);
        MeshIndexType* data = m_TriangleNeighbors->getElementListPointer(tri);
        for(int j = 0; j < tCount; j++)
        {
          MeshIndexType neighTri = data[j];
          if(m_RegionId[neighTri] == 0)
          {
            m_RegionId[neighTri] = regionCount;
            triangleCounts[regionCount]++;
            triList[size] = neighTri;
            size++;
            if(size >= triList.size())
            {
              size = triList.size();
              triList.resize(size + chunkSize);
              for(std::vector<int64_t>::size_type k = size; k < triList.size(); ++k)
              {
                triList[k] = -1;
              }
            }
          }
        }
      }
      regionCount++;
      triangleCounts.push_back(0);
    }
  }

  // Resize the Triangle Region AttributeMatrix
  std::vector<size_t> tDims(1, triangleCounts.size());
  dataContainerCAD->getAttributeMatrix(getTriangleAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateTriangleInstancePointers();

  // copy triangleCounts into the proper DataArray "NumTriangles" in the Feature Attribute Matrix
  auto& numTriangles = *m_NumTrianglesPtr.lock();
  for(size_t index = 0; index < m_NumTrianglesPtr.lock()->getSize(); index++)
  {
    numTriangles[index] = triangleCounts[index];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer LabelTriangleGeometry::newFilterInstance(bool copyFilterParameters) const
{
  LabelTriangleGeometry::Pointer filter = LabelTriangleGeometry::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString LabelTriangleGeometry::getCompiledLibraryName() const
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString LabelTriangleGeometry::getBrandingString() const
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString LabelTriangleGeometry::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString LabelTriangleGeometry::getGroupName() const
{
  return SIMPL::FilterGroups::SurfaceMeshingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString LabelTriangleGeometry::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MappingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString LabelTriangleGeometry::getHumanLabel() const
{
  return "Label CAD Geometry";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid LabelTriangleGeometry::getUuid() const
{
  return QUuid("{a250a228-3b6b-5b37-a6e4-8687484f04c4}");
}

// -----------------------------------------------------------------------------
LabelTriangleGeometry::Pointer LabelTriangleGeometry::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<LabelTriangleGeometry> LabelTriangleGeometry::New()
{
  struct make_shared_enabler : public LabelTriangleGeometry
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString LabelTriangleGeometry::getNameOfClass() const
{
  return QString("LabelTriangleGeometry");
}

// -----------------------------------------------------------------------------
QString LabelTriangleGeometry::ClassName()
{
  return QString("LabelTriangleGeometry");
}

// -----------------------------------------------------------------------------
void LabelTriangleGeometry::setCADDataContainerPath(const DataArrayPath& value)
{
  m_CADDataContainerPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath LabelTriangleGeometry::getCADDataContainerPath() const
{
  return m_CADDataContainerPath;
}

// -----------------------------------------------------------------------------
void LabelTriangleGeometry::setTriangleAttributeMatrixName(const QString& value)
{
  m_TriangleAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString LabelTriangleGeometry::getTriangleAttributeMatrixName() const
{
  return m_TriangleAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void LabelTriangleGeometry::setRegionIdArrayPath(const DataArrayPath& value)
{
  m_RegionIdArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath LabelTriangleGeometry::getRegionIdArrayPath() const
{
  return m_RegionIdArrayPath;
}

// -----------------------------------------------------------------------------
void LabelTriangleGeometry::setNumTrianglesArrayName(const QString& value)
{
  m_NumTrianglesArrayName = value;
}

// -----------------------------------------------------------------------------
QString LabelTriangleGeometry::getNumTrianglesArrayName() const
{
  return m_NumTrianglesArrayName;
}
