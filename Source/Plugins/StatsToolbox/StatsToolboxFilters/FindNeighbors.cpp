/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "FindNeighbors.h"

#include <algorithm>

#include <QtCore/QDateTime>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "StatsToolbox/StatsToolboxConstants.h"
#include "StatsToolbox/StatsToolboxVersion.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID30 = 30,
  DataArrayID31 = 31,
  DataArrayID32 = 32,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighbors::FindNeighbors()
{
  m_NeighborList = NeighborList<int32_t>::NullPointer();
  m_SharedSurfaceAreaList = NeighborList<float>::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighbors::~FindNeighbors() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  std::vector<QString> linkedProps = {"BoundaryCellsArrayName"};
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Store Boundary Cells Array", StoreBoundaryCells, FilterParameter::Category::Parameter, FindNeighbors, linkedProps));
  linkedProps.clear();
  linkedProps.push_back("SurfaceFeaturesArrayName");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Store Surface Features Array", StoreSurfaceFeatures, FilterParameter::Category::Parameter, FindNeighbors, linkedProps));
  parameters.push_back(SeparatorFilterParameter::Create("Cell Data", FilterParameter::Category::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::Category::RequiredArray, FindNeighbors, req));
  }
  parameters.push_back(SeparatorFilterParameter::Create("Cell Feature Data", FilterParameter::Category::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Feature Attribute Matrix", CellFeatureAttributeMatrixPath, FilterParameter::Category::RequiredArray, FindNeighbors, req));
  }
  parameters.push_back(SeparatorFilterParameter::Create("Cell Data", FilterParameter::Category::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Boundary Cells", BoundaryCellsArrayName, FeatureIdsArrayPath, FeatureIdsArrayPath, FilterParameter::Category::CreatedArray, FindNeighbors));
  parameters.push_back(SeparatorFilterParameter::Create("Cell Feature Data", FilterParameter::Category::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Number of Neighbors", NumNeighborsArrayName, CellFeatureAttributeMatrixPath, CellFeatureAttributeMatrixPath,
                                                      FilterParameter::Category::CreatedArray, FindNeighbors));
  parameters.push_back(
      SIMPL_NEW_DA_WITH_LINKED_AM_FP("Neighbor List", NeighborListArrayName, CellFeatureAttributeMatrixPath, CellFeatureAttributeMatrixPath, FilterParameter::Category::CreatedArray, FindNeighbors));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Shared Surface Area List", SharedSurfaceAreaListArrayName, CellFeatureAttributeMatrixPath, CellFeatureAttributeMatrixPath,
                                                      FilterParameter::Category::CreatedArray, FindNeighbors));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Surface Features", SurfaceFeaturesArrayName, CellFeatureAttributeMatrixPath, CellFeatureAttributeMatrixPath,
                                                      FilterParameter::Category::CreatedArray, FindNeighbors));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixPath(reader->readDataArrayPath("CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setBoundaryCellsArrayName(reader->readString("BoundaryCellsArrayName", getBoundaryCellsArrayName()));
  setSurfaceFeaturesArrayName(reader->readString("SurfaceFeaturesArrayName", getSurfaceFeaturesArrayName()));
  setStoreBoundaryCells(reader->readValue("StoreBoundaryCells", getStoreBoundaryCells()));
  setStoreSurfaceFeatures(reader->readValue("StoreSurfaceFeatures", getStoreSurfaceFeatures()));
  setNumNeighborsArrayName(reader->readString("NumNeighborsArrayName", getNumNeighborsArrayName()));
  setNeighborListArrayName(reader->readString("NeighborListArrayName", getNeighborListArrayName()));
  setSharedSurfaceAreaListArrayName(reader->readString("SharedSurfaceAreaListArrayName", getSharedSurfaceAreaListArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::initialize()
{
  m_NeighborList = NeighborList<int32_t>::NullPointer();
  m_SharedSurfaceAreaList = NeighborList<float>::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  initialize();
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom>(this, getFeatureIdsArrayPath().getDataContainerName());

  std::vector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<Int32ArrayType>(this, getFeatureIdsArrayPath(), cDims);
  if(nullptr != m_FeatureIdsPtr.lock())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }

  tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), m_FeatureIdsArrayPath.getAttributeMatrixName(), getBoundaryCellsArrayName());
  if(m_StoreBoundaryCells)
  {
    m_BoundaryCellsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<Int8ArrayType>(this, tempPath, 0, cDims);
    if(nullptr != m_BoundaryCellsPtr.lock())
    {
      m_BoundaryCells = m_BoundaryCellsPtr.lock()->getPointer(0);
    }
  }

  getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, getCellFeatureAttributeMatrixPath(), -301);

  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getNumNeighborsArrayName());

  m_NumNeighborsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<Int32ArrayType>(this, tempPath, 0, cDims);
  if(nullptr != m_NumNeighborsPtr.lock())
  {
    m_NumNeighbors = m_NumNeighborsPtr.lock()->getPointer(0);
  }

  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getSurfaceFeaturesArrayName());
  if(m_StoreSurfaceFeatures)
  {
    m_SurfaceFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<BoolArrayType>(this, tempPath, false, cDims);
    if(nullptr != m_SurfaceFeaturesPtr.lock())
    {
      m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0);
    }
  }

  // Feature Data
  // Do this whole block FIRST otherwise the side effect is that a call to m->getNumCellFeatureTuples will = 0
  // because we are just creating an empty NeighborList object.
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getNeighborListArrayName());
  m_NeighborList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<int32_t>>(this, tempPath, 0, cDims, "", DataArrayID31);
  if(getErrorCode() < 0)
  {
    return;
  }
  m_NeighborList.lock()->setNumNeighborsArrayName(getNumNeighborsArrayName());

  // And we do the same for the SharedSurfaceArea list
  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getSharedSurfaceAreaListArrayName());
  m_SharedSurfaceAreaList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<float>>(this, tempPath, 0, cDims, "", DataArrayID32);
  m_SharedSurfaceAreaList.lock()->setNumNeighborsArrayName(getNumNeighborsArrayName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  AttributeMatrix::Pointer featureAM = m->getAttributeMatrix(getCellFeatureAttributeMatrixPath());
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t totalFeatures = m_NumNeighborsPtr.lock()->getNumberOfTuples();

  /* Ensure that we will be able to work with the user selected feature Id Array */
  Int32ArrayType& featureIdsPtr = *(m_FeatureIdsPtr.lock().get());
  const auto [minFeatureId, maxFeatureId] = std::minmax_element(m_FeatureIds, m_FeatureIds + totalPoints);
  if(static_cast<size_t>(*maxFeatureId) >= totalFeatures)
  {
    QString msg;
    QTextStream out(&msg);
    out << "Data Array " << featureIdsPtr.getName() << " has a maximum value of " << *maxFeatureId << " which is greater than the "
        << " number of features from array " << m_NumNeighborsPtr.lock()->getName() << " which has " << totalFeatures << ". Did you select the "
        << " incorrect array for the 'FeatureIds' array?";
    setErrorCondition(-24500, msg);
    return;
  }

  SizeVec3Type udims = m->getGeometryAs<ImageGeom>()->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]),
      static_cast<int64_t>(udims[1]),
      static_cast<int64_t>(udims[2]),
  };

  int64_t neighpoints[6] = {0, 0, 0, 0, 0, 0};
  neighpoints[0] = -dims[0] * dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0] * dims[1];

  int64_t column = 0, row = 0, plane = 0;
  int32_t feature = 0;
  int32_t nnum = 0;
  int8_t onsurf = 0;
  bool good = false;
  int64_t neighbor = 0;

  std::vector<std::vector<int32_t>> neighborlist;
  std::vector<std::vector<float>> neighborsurfacearealist;

  int32_t nListSize = 100;
  neighborlist.resize(totalFeatures);
  neighborsurfacearealist.resize(totalFeatures);

  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;

  for(size_t i = 1; i < totalFeatures; i++)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if(currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Finding Neighbors || Initializing Neighbor Lists || %1% Complete").arg((static_cast<float>(i) / totalFeatures) * 100);
      notifyStatusMessage(ss);
      millis = QDateTime::currentMSecsSinceEpoch();
    }

    if(getCancel())
    {
      return;
    }

    m_NumNeighbors[i] = 0;
    neighborlist[i].resize(nListSize);
    neighborsurfacearealist[i].assign(nListSize, -1.0f);
    if(m_StoreSurfaceFeatures)
    {
      m_SurfaceFeatures[i] = false;
    }
  }

  for(size_t j = 0; j < totalPoints; j++)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if(currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Finding Neighbors || Determining Neighbor Lists || %1% Complete").arg((static_cast<float>(j) / totalPoints) * 100);
      notifyStatusMessage(ss);
      millis = QDateTime::currentMSecsSinceEpoch();
    }

    if(getCancel())
    {
      return;
    }

    onsurf = 0;
    feature = m_FeatureIds[j];
    if(feature > 0 && feature < neighborlist.size())
    {
      column = static_cast<int64_t>(j % m->getGeometryAs<ImageGeom>()->getXPoints());
      row = static_cast<int64_t>((j / m->getGeometryAs<ImageGeom>()->getXPoints()) % m->getGeometryAs<ImageGeom>()->getYPoints());
      plane = static_cast<int64_t>(j / (m->getGeometryAs<ImageGeom>()->getXPoints() * m->getGeometryAs<ImageGeom>()->getYPoints()));
      if(m_StoreSurfaceFeatures)
      {
        if((column == 0 || column == static_cast<int64_t>((m->getGeometryAs<ImageGeom>()->getXPoints() - 1)) || row == 0 ||
            row == static_cast<int64_t>((m->getGeometryAs<ImageGeom>()->getYPoints()) - 1) || plane == 0 || plane == static_cast<int64_t>((m->getGeometryAs<ImageGeom>()->getZPoints() - 1))) &&
           m->getGeometryAs<ImageGeom>()->getZPoints() != 1)
        {
          m_SurfaceFeatures[feature] = true;
        }
        if((column == 0 || column == static_cast<int64_t>((m->getGeometryAs<ImageGeom>()->getXPoints() - 1)) || row == 0 ||
            row == static_cast<int64_t>((m->getGeometryAs<ImageGeom>()->getYPoints() - 1))) &&
           m->getGeometryAs<ImageGeom>()->getZPoints() == 1)
        {
          m_SurfaceFeatures[feature] = true;
        }
      }
      for(int32_t k = 0; k < 6; k++)
      {
        good = true;
        neighbor = static_cast<int64_t>(j + neighpoints[k]);
        if(k == 0 && plane == 0)
        {
          good = false;
        }
        if(k == 5 && plane == (m->getGeometryAs<ImageGeom>()->getZPoints() - 1))
        {
          good = false;
        }
        if(k == 1 && row == 0)
        {
          good = false;
        }
        if(k == 4 && row == (m->getGeometryAs<ImageGeom>()->getYPoints() - 1))
        {
          good = false;
        }
        if(k == 2 && column == 0)
        {
          good = false;
        }
        if(k == 3 && column == (m->getGeometryAs<ImageGeom>()->getXPoints() - 1))
        {
          good = false;
        }
        if(good && m_FeatureIds[neighbor] != feature && m_FeatureIds[neighbor] > 0)
        {
          onsurf++;
          nnum = m_NumNeighbors[feature];
          neighborlist[feature].push_back(m_FeatureIds[neighbor]);
          nnum++;
          m_NumNeighbors[feature] = nnum;
        }
      }
    }
    if(m_StoreBoundaryCells)
    {
      m_BoundaryCells[j] = onsurf;
    }
  }

  FloatVec3Type spacing = m->getGeometryAs<ImageGeom>()->getSpacing();

  // We do this to create new set of NeighborList objects
  for(size_t i = 1; i < totalFeatures; i++)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if(currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Finding Neighbors || Calculating Surface Areas || %1% Complete").arg(((float)i / totalFeatures) * 100);
      notifyStatusMessage(ss);
      millis = QDateTime::currentMSecsSinceEpoch();
    }

    if(getCancel())
    {
      return;
    }

    QMap<int32_t, int32_t> neighToCount;
    int32_t numneighs = static_cast<int32_t>(neighborlist[i].size());

    // this increments the voxel counts for each feature
    for(int32_t j = 0; j < numneighs; j++)
    {
      neighToCount[neighborlist[i][j]]++;
    }

    QMap<int32_t, int32_t>::Iterator neighiter = neighToCount.find(0);
    neighToCount.erase(neighiter);
    neighiter = neighToCount.find(-1);
    neighToCount.erase(neighiter);
    // Resize the features neighbor list to zero
    neighborlist[i].resize(0);
    neighborsurfacearealist[i].resize(0);

    for(QMap<int32_t, int32_t>::iterator iter = neighToCount.begin(); iter != neighToCount.end(); ++iter)
    {
      int32_t neigh = iter.key();    // get the neighbor feature
      int32_t number = iter.value(); // get the number of voxels
      float area = float(number) * spacing[0] * spacing[1];

      // Push the neighbor feature id back onto the list so we stay synced up
      neighborlist[i].push_back(neigh);
      neighborsurfacearealist[i].push_back(area);
    }
    m_NumNeighbors[i] = int32_t(neighborlist[i].size());

    // Set the vector for each list into the NeighborList Object
    NeighborList<int32_t>::SharedVectorType sharedNeiLst(new std::vector<int32_t>);
    sharedNeiLst->assign(neighborlist[i].begin(), neighborlist[i].end());
    m_NeighborList.lock()->setList(static_cast<int32_t>(i), sharedNeiLst);

    NeighborList<float>::SharedVectorType sharedSAL(new std::vector<float>);
    sharedSAL->assign(neighborsurfacearealist[i].begin(), neighborsurfacearealist[i].end());
    m_SharedSurfaceAreaList.lock()->setList(static_cast<int32_t>(i), sharedSAL);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindNeighbors::newFilterInstance(bool copyFilterParameters) const
{
  FindNeighbors::Pointer filter = FindNeighbors::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindNeighbors::getCompiledLibraryName() const
{
  return StatsToolboxConstants::StatsToolboxBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindNeighbors::getBrandingString() const
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindNeighbors::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << StatsToolbox::Version::Major() << "." << StatsToolbox::Version::Minor() << "." << StatsToolbox::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindNeighbors::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindNeighbors::getUuid() const
{
  return QUuid("{97cf66f8-7a9b-5ec2-83eb-f8c4c8a17bac}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindNeighbors::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindNeighbors::getHumanLabel() const
{
  return "Find Feature Neighbors";
}

// -----------------------------------------------------------------------------
FindNeighbors::Pointer FindNeighbors::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindNeighbors> FindNeighbors::New()
{
  struct make_shared_enabler : public FindNeighbors
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindNeighbors::getNameOfClass() const
{
  return QString("FindNeighbors");
}

// -----------------------------------------------------------------------------
QString FindNeighbors::ClassName()
{
  return QString("FindNeighbors");
}

// -----------------------------------------------------------------------------
void FindNeighbors::setCellFeatureAttributeMatrixPath(const DataArrayPath& value)
{
  m_CellFeatureAttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindNeighbors::getCellFeatureAttributeMatrixPath() const
{
  return m_CellFeatureAttributeMatrixPath;
}

// -----------------------------------------------------------------------------
void FindNeighbors::setSharedSurfaceAreaListArrayName(const QString& value)
{
  m_SharedSurfaceAreaListArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindNeighbors::getSharedSurfaceAreaListArrayName() const
{
  return m_SharedSurfaceAreaListArrayName;
}

// -----------------------------------------------------------------------------
void FindNeighbors::setNeighborListArrayName(const QString& value)
{
  m_NeighborListArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindNeighbors::getNeighborListArrayName() const
{
  return m_NeighborListArrayName;
}

// -----------------------------------------------------------------------------
void FindNeighbors::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindNeighbors::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void FindNeighbors::setBoundaryCellsArrayName(const QString& value)
{
  m_BoundaryCellsArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindNeighbors::getBoundaryCellsArrayName() const
{
  return m_BoundaryCellsArrayName;
}

// -----------------------------------------------------------------------------
void FindNeighbors::setNumNeighborsArrayName(const QString& value)
{
  m_NumNeighborsArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindNeighbors::getNumNeighborsArrayName() const
{
  return m_NumNeighborsArrayName;
}

// -----------------------------------------------------------------------------
void FindNeighbors::setSurfaceFeaturesArrayName(const QString& value)
{
  m_SurfaceFeaturesArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindNeighbors::getSurfaceFeaturesArrayName() const
{
  return m_SurfaceFeaturesArrayName;
}

// -----------------------------------------------------------------------------
void FindNeighbors::setStoreBoundaryCells(bool value)
{
  m_StoreBoundaryCells = value;
}

// -----------------------------------------------------------------------------
bool FindNeighbors::getStoreBoundaryCells() const
{
  return m_StoreBoundaryCells;
}

// -----------------------------------------------------------------------------
void FindNeighbors::setStoreSurfaceFeatures(bool value)
{
  m_StoreSurfaceFeatures = value;
}

// -----------------------------------------------------------------------------
bool FindNeighbors::getStoreSurfaceFeatures() const
{
  return m_StoreSurfaceFeatures;
}
