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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <memory>

#include "MinSize.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "Processing/ProcessingConstants.h"
#include "Processing/ProcessingVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MinSize::MinSize()
: m_MinAllowedFeatureSize(1)
, m_ApplyToSinglePhase(false)
, m_PhaseNumber(0)
, m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_FeaturePhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases)
, m_NumCellsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::NumCells)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MinSize::~MinSize() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinSize::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Minimum Allowed Feature Size", MinAllowedFeatureSize, FilterParameter::Parameter, MinSize));
  QStringList linkedProps;
  linkedProps << "PhaseNumber"
              << "FeaturePhasesArrayPath";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Apply to Single Phase Only", ApplyToSinglePhase, FilterParameter::Parameter, MinSize, linkedProps));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Phase Index", PhaseNumber, FilterParameter::Parameter, MinSize));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, MinSize, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, MinSize, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Number of Cells", NumCellsArrayPath, FilterParameter::RequiredArray, MinSize, req));
  }
  {
    MultiDataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Attribute Arrays to Ignore", IgnoredDataArrayPaths, FilterParameter::Parameter, MinSize, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinSize::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setNumCellsArrayPath(reader->readDataArrayPath("NumCellsArrayPath", getNumCellsArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setMinAllowedFeatureSize(reader->readValue("MinAllowedFeatureSize", getMinAllowedFeatureSize()));
  setApplyToSinglePhase(reader->readValue("ApplyToSinglePhase", getApplyToSinglePhase()));
  setPhaseNumber(reader->readValue("PhaseNumber", getPhaseNumber()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinSize::initialize()
{
  m_Neighbors = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinSize::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  initialize();

  QVector<DataArrayPath> dataArrayPaths;

  if(getMinAllowedFeatureSize() < 0)
  {
    QString ss = QObject::tr("The minimum Feature size (%1) must be 0 or positive").arg(getMinAllowedFeatureSize());
    setErrorCondition(-5555, ss);
  }

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  std::vector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims);
  if(nullptr != m_FeatureIdsPtr.lock())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_NumCellsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getNumCellsArrayPath(), cDims);
  if(nullptr != m_NumCellsPtr.lock())
  {
    m_NumCells = m_NumCellsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getNumCellsArrayPath());
  }

  if(m_ApplyToSinglePhase)
  {
    m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), cDims);
    if(nullptr != m_FeaturePhasesPtr.lock())
    {
      m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCode() >= 0)
    {
      dataArrayPaths.push_back(getFeaturePhasesArrayPath());
    }
  }

  getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);

  // Throw a warning to inform the user that the neighbor list arrays could be deleted by this filter
  QString featureIdsPath = getFeatureIdsArrayPath().getDataContainerName() + "/" + getFeatureIdsArrayPath().getAttributeMatrixName() + "/" + getFeatureIdsArrayPath().getDataArrayName();
  int err = -1;
  AttributeMatrix::Pointer featureAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getNumCellsArrayPath(), err);
  if(nullptr == featureAM.get())
  {
    return;
  }

  QString ss = QObject::tr("If this filter modifies the Cell Level Array '%1', all arrays of type NeighborList will be deleted.  These arrays are:\n").arg(featureIdsPath);
  QList<QString> featureArrayNames = featureAM->getAttributeArrayNames();
  for(const auto& featureArrayName : featureArrayNames)
  {
    IDataArray::Pointer arr = featureAM->getAttributeArray(featureArrayName);
    QString type = arr->getTypeAsString();
    if(type.compare("NeighborList<T>") == 0)
    {
      ss.append("\n" + getNumCellsArrayPath().getDataContainerName() + "/" + getNumCellsArrayPath().getAttributeMatrixName() + "/" + arr->getName());
    }
  }

  setWarningCondition(-5556, ss);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinSize::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinSize::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  // If running on a single phase, validate that the user has not entered a phase number
  // that is not in the system ; the filter would not crash otherwise, but the user should
  // be notified of unanticipated behavior ; this cannot be done in the dataCheck since
  // we don't have acces to the data yet
  if(m_ApplyToSinglePhase)
  {
    AttributeMatrix::Pointer featAttrMat =
        getDataContainerArray()->getDataContainer(getFeaturePhasesArrayPath().getDataContainerName())->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName());
    size_t numFeatures = featAttrMat->getNumberOfTuples();
    bool unavailablePhase = true;
    for(size_t i = 0; i < numFeatures; i++)
    {
      if(m_FeaturePhases[i] == m_PhaseNumber)
      {
        unavailablePhase = false;
        break;
      }
    }

    if(unavailablePhase)
    {
      QString ss = QObject::tr("The phase number (%1) is not available in the supplied Feature phases array with path (%2)").arg(m_PhaseNumber).arg(m_FeaturePhasesArrayPath.serialize());
      setErrorCondition(-5555, ss);
      return;
    }
  }

  QVector<bool> activeObjects = remove_smallfeatures();
  if(getErrorCode() < 0)
  {
    return;
  }
  assign_badpoints();

  AttributeMatrix::Pointer cellFeatureAttrMat = getDataContainerArray()->getAttributeMatrix(m_NumCellsArrayPath);
  cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock().get());


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinSize::assign_badpoints()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  SizeVec3Type udims = m->getGeometryAs<ImageGeom>()->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(totalPoints, "_INTERNAL_USE_ONLY_Neighbors", true);
  m_Neighbors = neighborsPtr->getPointer(0);
  neighborsPtr->initializeWithValue(-1);

  int32_t good = 1;
  int32_t current = 0;
  int32_t most = 0;
  int64_t neighpoint = 0;

  int64_t neighpoints[6] = {0, 0, 0, 0, 0, 0};
  neighpoints[0] = -dims[0] * dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0] * dims[1];

  size_t counter = 1;
  int64_t count = 0;
  int64_t kstride = 0, jstride = 0;
  int32_t featurename = 0, feature = 0;
  int32_t neighbor = 0;
  QVector<int32_t> n(m_NumCellsPtr.lock()->getNumberOfTuples(), 0);
  while(counter != 0)
  {
    counter = 0;
    for(int64_t k = 0; k < dims[2]; k++)
    {
      kstride = dims[0] * dims[1] * k;
      for(int64_t j = 0; j < dims[1]; j++)
      {
        jstride = dims[0] * j;
        for(int64_t i = 0; i < dims[0]; i++)
        {
          count = kstride + jstride + i;
          featurename = m_FeatureIds[count];
          if(featurename < 0)
          {
            counter++;
            most = 0;
            for(int32_t l = 0; l < 6; l++)
            {
              good = 1;
              neighpoint = count + neighpoints[l];
              if(l == 0 && k == 0)
              {
                good = 0;
              }
              if(l == 5 && k == (dims[2] - 1))
              {
                good = 0;
              }
              if(l == 1 && j == 0)
              {
                good = 0;
              }
              if(l == 4 && j == (dims[1] - 1))
              {
                good = 0;
              }
              if(l == 2 && i == 0)
              {
                good = 0;
              }
              if(l == 3 && i == (dims[0] - 1))
              {
                good = 0;
              }
              if(good == 1)
              {
                feature = m_FeatureIds[neighpoint];
                if(feature >= 0)
                {
                  n[feature]++;
                  current = n[feature];
                  if(current > most)
                  {
                    most = current;
                    m_Neighbors[count] = neighpoint;
                  }
                }
              }
            }
            for(int32_t l = 0; l < 6; l++)
            {
              good = 1;
              neighpoint = count + neighpoints[l];
              if(l == 0 && k == 0)
              {
                good = 0;
              }
              if(l == 5 && k == (dims[2] - 1))
              {
                good = 0;
              }
              if(l == 1 && j == 0)
              {
                good = 0;
              }
              if(l == 4 && j == (dims[1] - 1))
              {
                good = 0;
              }
              if(l == 2 && i == 0)
              {
                good = 0;
              }
              if(l == 3 && i == (dims[0] - 1))
              {
                good = 0;
              }
              if(good == 1)
              {
                feature = m_FeatureIds[neighpoint];
                if(feature >= 0)
                {
                  n[feature] = 0;
                }
              }
            }
          }
        }
      }
    }
    QString attrMatName = m_FeatureIdsArrayPath.getAttributeMatrixName();
    QList<QString> voxelArrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNames();
    for(const auto& dataArrayPath : m_IgnoredDataArrayPaths)
    {
      voxelArrayNames.removeAll(dataArrayPath.getDataArrayName());
    }
    // TODO: This loop could be parallelized. Look at NeighborOrientationCorrelation filter
    for(size_t j = 0; j < totalPoints; j++)
    {
      featurename = m_FeatureIds[j];
      neighbor = m_Neighbors[j];
      if(neighbor >= 0)
      {
        if(featurename < 0 && m_FeatureIds[neighbor] >= 0)
        {
          for(auto& voxelArrayName : voxelArrayNames)
          {
            IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(voxelArrayName);
            p->copyTuple(neighbor, j);
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<bool> MinSize::remove_smallfeatures()
{
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  bool good = false;
  int32_t gnum;

  size_t totalFeatures = m_NumCellsPtr.lock()->getNumberOfTuples();
  QVector<bool> activeObjects(totalFeatures, true);

  for(size_t i = 1; i < totalFeatures; i++)
  {
    if(!m_ApplyToSinglePhase)
    {
      if(m_NumCells[i] >= m_MinAllowedFeatureSize)
      {
        good = true;
      }
      else
      {
        activeObjects[i] = false;
      }
    }
    else
    {
      if(m_NumCells[i] >= getMinAllowedFeatureSize() || m_FeaturePhases[i] != m_PhaseNumber)
      {
        good = true;
      }
      else
      {
        activeObjects[i] = false;
      }
    }
  }
  if(!good)
  {
    setErrorCondition(-1, "The minimum size is larger than the largest Feature.  All Features would be removed");
    return activeObjects;
  }
  for(size_t i = 0; i < totalPoints; i++)
  {
    gnum = m_FeatureIds[i];
    if(!activeObjects[gnum])
    {
      m_FeatureIds[i] = -1;
    }
  }
  return activeObjects;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MinSize::newFilterInstance(bool copyFilterParameters) const
{
  MinSize::Pointer filter = MinSize::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MinSize::getCompiledLibraryName() const
{
  return ProcessingConstants::ProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MinSize::getBrandingString() const
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MinSize::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Processing::Version::Major() << "." << Processing::Version::Minor() << "." << Processing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MinSize::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid MinSize::getUuid() const
{
  return QUuid("{53ac1638-8934-57b8-b8e5-4b91cdda23ec}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MinSize::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CleanupFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MinSize::getHumanLabel() const
{
  return "Minimum Size";
}

// -----------------------------------------------------------------------------
MinSize::Pointer MinSize::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<MinSize> MinSize::New()
{
  struct make_shared_enabler : public MinSize
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString MinSize::getNameOfClass() const
{
  return QString("MinSize");
}

// -----------------------------------------------------------------------------
QString MinSize::ClassName()
{
  return QString("MinSize");
}

// -----------------------------------------------------------------------------
void MinSize::setMinAllowedFeatureSize(int value)
{
  m_MinAllowedFeatureSize = value;
}

// -----------------------------------------------------------------------------
int MinSize::getMinAllowedFeatureSize() const
{
  return m_MinAllowedFeatureSize;
}

// -----------------------------------------------------------------------------
void MinSize::setApplyToSinglePhase(bool value)
{
  m_ApplyToSinglePhase = value;
}

// -----------------------------------------------------------------------------
bool MinSize::getApplyToSinglePhase() const
{
  return m_ApplyToSinglePhase;
}

// -----------------------------------------------------------------------------
void MinSize::setPhaseNumber(int value)
{
  m_PhaseNumber = value;
}

// -----------------------------------------------------------------------------
int MinSize::getPhaseNumber() const
{
  return m_PhaseNumber;
}

// -----------------------------------------------------------------------------
void MinSize::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath MinSize::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void MinSize::setFeaturePhasesArrayPath(const DataArrayPath& value)
{
  m_FeaturePhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath MinSize::getFeaturePhasesArrayPath() const
{
  return m_FeaturePhasesArrayPath;
}

// -----------------------------------------------------------------------------
void MinSize::setNumCellsArrayPath(const DataArrayPath& value)
{
  m_NumCellsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath MinSize::getNumCellsArrayPath() const
{
  return m_NumCellsArrayPath;
}

// -----------------------------------------------------------------------------
void MinSize::setIgnoredDataArrayPaths(const QVector<DataArrayPath>& value)
{
  m_IgnoredDataArrayPaths = value;
}

// -----------------------------------------------------------------------------
QVector<DataArrayPath> MinSize::getIgnoredDataArrayPaths() const
{
  return m_IgnoredDataArrayPaths;
}
