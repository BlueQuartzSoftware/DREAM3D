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

#include "MinNeighbors.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Processing/ProcessingConstants.h"
#include "Processing/ProcessingVersion.h"

// Include the MOC generated file for this class
#include "moc_MinNeighbors.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MinNeighbors::MinNeighbors()
: AbstractFilter()
, m_MinNumNeighbors(1)
, m_ApplyToSinglePhase(false)
, m_PhaseNumber(0)
, m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_FeaturePhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases)
, m_NumNeighborsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::NumNeighbors)
, m_Neighbors(nullptr)
, m_FeatureIds(nullptr)
, m_FeaturePhases(nullptr)
, m_NumNeighbors(nullptr)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MinNeighbors::~MinNeighbors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Minimum Number Neighbors", MinNumNeighbors, FilterParameter::Parameter, MinNeighbors));
  QStringList linkedProps;
  linkedProps << "PhaseNumber"
              << "FeaturePhasesArrayPath";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Apply to Single Phase Only", ApplyToSinglePhase, FilterParameter::Parameter, MinNeighbors, linkedProps));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Phase Index", PhaseNumber, FilterParameter::Parameter, MinNeighbors));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, MinNeighbors, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, MinNeighbors, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Number of Neighbors", NumNeighborsArrayPath, FilterParameter::RequiredArray, MinNeighbors, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setNumNeighborsArrayPath(reader->readDataArrayPath("NumNeighborsArrayPath", getNumNeighborsArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setApplyToSinglePhase(reader->readValue("ApplyToSinglePhase", getApplyToSinglePhase()));
  setPhaseNumber(reader->readValue("PhaseNumber", getPhaseNumber()));
  setMinNumNeighbors(reader->readValue("MinNumNeighbors", getMinNumNeighbors()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::initialize()
{
  m_Neighbors = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  initialize();

  QVector<DataArrayPath> dataArrayPaths;

  if(getMinNumNeighbors() < 0)
  {
    QString ss = QObject::tr("The minimum number of neighbors (%1) must be 0 or positive").arg(getMinNumNeighbors());
    setErrorCondition(-5555);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_NumNeighborsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getNumNeighborsArrayPath(),
                                                                                                          cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_NumNeighborsPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_NumNeighbors = m_NumNeighborsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getNumNeighborsArrayPath());
  }

  if(getApplyToSinglePhase() == true)
  {
    m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(),
                                                                                                             cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_FeaturePhasesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0)
    {
      dataArrayPaths.push_back(getFeaturePhasesArrayPath());
    }
  }

  getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);

  // Throw a warning to inform the user that the neighbor list arrays could be deleted by this filter
  QString featureIdsPath = getFeatureIdsArrayPath().getDataContainerName() + "/" + getFeatureIdsArrayPath().getAttributeMatrixName() + "/" + getFeatureIdsArrayPath().getDataArrayName();
  int err = 0;
  AttributeMatrix::Pointer featureAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getNumNeighborsArrayPath(), err);
  if(nullptr == featureAM.get())
  {
    return;
  }

  QString ss = QObject::tr("If this filter changes the selected feature ids array located at '%1', all arrays of type NeighborList will be deleted.  These arrays are:\n").arg(featureIdsPath);
  QList<QString> featureArrayNames = featureAM->getAttributeArrayNames();
  for(int i = 0; i < featureArrayNames.size(); i++)
  {
    IDataArray::Pointer arr = featureAM->getAttributeArray(featureArrayNames[i]);
    QString type = arr->getTypeAsString();
    if(type.compare("NeighborList<T>") == 0)
    {
      ss.append("\n" + getNumNeighborsArrayPath().getDataContainerName() + "/" + getNumNeighborsArrayPath().getAttributeMatrixName() + "/" + arr->getName());
    }
  }

  setWarningCondition(-5556);
  notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::preflight()
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
void MinNeighbors::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  // If running on a single phase, validate that the user has not entered a phase number
  // that is not in the system ; the filter would not crash otherwise, but the user should
  // be notified of unanticipated behavior ; this cannot be done in the dataCheck since
  // we don't have acces to the data yet
  if(m_ApplyToSinglePhase == true)
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

    if(unavailablePhase == true)
    {
      QString ss = QObject::tr("The phase number (%1) is not available in the supplied Feature phases array with path (%2)").arg(m_PhaseNumber).arg(m_FeaturePhasesArrayPath.serialize());
      setErrorCondition(-5555);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }

  QVector<bool> activeObjects = merge_containedfeatures();
  if(getErrorCondition() < 0)
  {
    return;
  }
  assign_badpoints();

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_NumNeighborsArrayPath.getDataContainerName());
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(m_NumNeighborsArrayPath.getAttributeMatrixName());
  cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MinNeighbors::assign_badpoints()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_NumNeighborsArrayPath.getDataContainerName());

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(totalPoints, "_INTERNAL_USE_ONLY_Neighbors");
  m_Neighbors = neighborsPtr->getPointer(0);
  neighborsPtr->initializeWithValue(-1);

  int32_t good = 1;
  int32_t current = 0;
  int32_t most = 0;
  int64_t neighpoint = 0;
  size_t numfeatures = m_NumNeighborsPtr.lock()->getNumberOfTuples();

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
  QVector<int32_t> n(numfeatures + 1, 0);
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
            current = 0;
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
    for(size_t j = 0; j < totalPoints; j++)
    {
      featurename = m_FeatureIds[j];
      neighbor = m_Neighbors[j];
      if(featurename < 0 && neighbor >= 0 && m_FeatureIds[neighbor] >= 0)
      {
        for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(*iter);
          p->copyTuple(neighbor, j);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<bool> MinNeighbors::merge_containedfeatures()
{
  // Since this method is called from the 'execute' and the DataContainer validity
  // was checked there we are just going to get the Shared Pointer to the DataContainer
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_NumNeighborsArrayPath.getDataContainerName());

  bool good = false;

  size_t totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();
  size_t totalFeatures = m_NumNeighborsPtr.lock()->getNumberOfTuples();

  QVector<bool> activeObjects(totalFeatures, true);

  for(size_t i = 1; i < totalFeatures; i++)
  {
    if(m_ApplyToSinglePhase == false)
    {
      if(m_NumNeighbors[i] >= m_MinNumNeighbors)
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
      if(m_NumNeighbors[i] >= m_MinNumNeighbors || m_FeaturePhases[i] != m_PhaseNumber)
      {
        good = true;
      }
      else
      {
        activeObjects[i] = false;
      }
    }
  }
  if(good == false)
  {
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), "The minimum number of neighbors is larger than the Feature with the most neighbors.  All Features would be removed", getErrorCondition());
    return activeObjects;
  }
  for(size_t i = 0; i < totalPoints; i++)
  {
    int32_t featurename = m_FeatureIds[i];
    if(activeObjects[featurename] == false)
    {
      m_FeatureIds[i] = -1;
    }
  }
  return activeObjects;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MinNeighbors::newFilterInstance(bool copyFilterParameters)
{
  MinNeighbors::Pointer filter = MinNeighbors::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MinNeighbors::getCompiledLibraryName()
{
  return ProcessingConstants::ProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MinNeighbors::getBrandingString()
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MinNeighbors::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << Processing::Version::Major() << "." << Processing::Version::Minor() << "." << Processing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MinNeighbors::getGroupName()
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MinNeighbors::getSubGroupName()
{
  return SIMPL::FilterSubGroups::CleanupFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MinNeighbors::getHumanLabel()
{
  return "Minimum Number of Neighbors";
}
