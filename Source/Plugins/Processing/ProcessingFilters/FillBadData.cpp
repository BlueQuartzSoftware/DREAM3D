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

#include "FillBadData.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Processing/ProcessingConstants.h"
#include "Processing/ProcessingVersion.h"

// Include the MOC generated file for this class
#include "moc_FillBadData.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FillBadData::FillBadData()
: AbstractFilter()
, m_StoreAsNewPhase(false)
, m_MinAllowedDefectSize(1)
, m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_CellPhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases)
, m_AlreadyChecked(nullptr)
, m_Neighbors(nullptr)
, m_FeatureIds(nullptr)
, m_CellPhases(nullptr)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FillBadData::~FillBadData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FillBadData::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Minimum Allowed Defect Size", MinAllowedDefectSize, FilterParameter::Parameter, FillBadData));
  QStringList linkedProps;
  linkedProps << "CellPhasesArrayPath";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Store Defects as New Phase", StoreAsNewPhase, FilterParameter::Parameter, FillBadData, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, FillBadData, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, FillBadData, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FillBadData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  setMinAllowedDefectSize(reader->readValue("MinAllowedDefectSize", getMinAllowedDefectSize()));
  setStoreAsNewPhase(reader->readValue("StoreAsNewPhase", getStoreAsNewPhase()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FillBadData::initialize()
{
  m_AlreadyChecked = nullptr;
  m_Neighbors = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FillBadData::dataCheck()
{
  setErrorCondition(0);
  initialize();

  QVector<DataArrayPath> dataArrayPaths;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getFeatureIdsArrayPath());
  }

  if(m_StoreAsNewPhase == true)
  {
    m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(),
                                                                                                          cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_CellPhasesPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0)
    {
      dataArrayPaths.push_back(getCellPhasesArrayPath());
    }
  }

  getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FillBadData::preflight()
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
void FillBadData::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(totalPoints, "_INTERNAL_USE_ONLY_Neighbors");
  m_Neighbors = neighborsPtr->getPointer(0);
  neighborsPtr->initializeWithValue(-1);

  BoolArrayType::Pointer alreadCheckedPtr = BoolArrayType::CreateArray(totalPoints, "_INTERNAL_USE_ONLY_AlreadyChecked");
  m_AlreadyChecked = alreadCheckedPtr->getPointer(0);
  alreadCheckedPtr->initializeWithZeros();

  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  size_t count = 1;
  int32_t good = 1;
  int64_t neighbor;
  int64_t index = 0;
  float x = 0.0f, y = 0.0f, z = 0.0f;
  int64_t column = 0, row = 0, plane = 0;
  int64_t neighpoint = 0;
  int32_t featurename = 0, feature = 0;
  size_t numfeatures = 0;
  size_t maxPhase = 0;

  for(size_t i = 0; i < totalPoints; i++)
  {
    featurename = m_FeatureIds[i];
    if(featurename > numfeatures)
    {
      numfeatures = featurename;
    }
  }

  if(m_StoreAsNewPhase == true)
  {
    for(size_t i = 0; i < totalPoints; i++)
    {
      if(m_CellPhases[i] > maxPhase)
      {
        maxPhase = m_CellPhases[i];
      }
    }
  }

  int64_t neighpoints[6] = {0, 0, 0, 0, 0, 0};
  neighpoints[0] = -dims[0] * dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0] * dims[1];
  std::vector<int64_t> currentvlist;

  for(size_t iter = 0; iter < totalPoints; iter++)
  {
    m_AlreadyChecked[iter] = false;
    if(m_FeatureIds[iter] != 0)
    {
      m_AlreadyChecked[iter] = true;
    }
  }

  for(size_t i = 0; i < totalPoints; i++)
  {
    if(m_AlreadyChecked[i] == false && m_FeatureIds[i] == 0)
    {
      currentvlist.push_back(static_cast<int64_t>(i));
      count = 0;
      while(count < currentvlist.size())
      {
        index = currentvlist[count];
        column = index % dims[0];
        row = (index / dims[0]) % dims[1];
        plane = index / (dims[0] * dims[1]);
        for(int32_t j = 0; j < 6; j++)
        {
          good = 1;
          neighbor = index + neighpoints[j];
          if(j == 0 && plane == 0)
          {
            good = 0;
          }
          if(j == 5 && plane == (dims[2] - 1))
          {
            good = 0;
          }
          if(j == 1 && row == 0)
          {
            good = 0;
          }
          if(j == 4 && row == (dims[1] - 1))
          {
            good = 0;
          }
          if(j == 2 && column == 0)
          {
            good = 0;
          }
          if(j == 3 && column == (dims[0] - 1))
          {
            good = 0;
          }
          if(good == 1 && m_FeatureIds[neighbor] == 0 && m_AlreadyChecked[neighbor] == false)
          {
            currentvlist.push_back(neighbor);
            m_AlreadyChecked[neighbor] = true;
          }
        }
        count++;
      }
      if((int32_t)currentvlist.size() >= m_MinAllowedDefectSize)
      {
        for(size_t k = 0; k < currentvlist.size(); k++)
        {
          m_FeatureIds[currentvlist[k]] = 0;
          if(m_StoreAsNewPhase == true)
          {
            m_CellPhases[currentvlist[k]] = maxPhase + 1;
          }
        }
      }
      if((int32_t)currentvlist.size() < m_MinAllowedDefectSize)
      {
        for(std::vector<int64_t>::size_type k = 0; k < currentvlist.size(); k++)
        {
          m_FeatureIds[currentvlist[k]] = -1;
        }
      }
      currentvlist.clear();
    }
  }

  int32_t current = 0;
  int32_t most = 0;
  std::vector<int32_t> n(numfeatures + 1, 0);

  while(count != 0)
  {
    count = 0;
    for(size_t i = 0; i < totalPoints; i++)
    {
      featurename = m_FeatureIds[i];
      if(featurename < 0)
      {
        count++;
        current = 0;
        most = 0;
        x = static_cast<float>(i % dims[0]);
        y = static_cast<float>((i / dims[0]) % dims[1]);
        z = static_cast<float>(i / (dims[0] * dims[1]));
        for(int32_t j = 0; j < 6; j++)
        {
          good = 1;
          neighpoint = static_cast<int64_t>(i + neighpoints[j]);
          if(j == 0 && z == 0)
          {
            good = 0;
          }
          if(j == 5 && z == (dims[2] - 1))
          {
            good = 0;
          }
          if(j == 1 && y == 0)
          {
            good = 0;
          }
          if(j == 4 && y == (dims[1] - 1))
          {
            good = 0;
          }
          if(j == 2 && x == 0)
          {
            good = 0;
          }
          if(j == 3 && x == (dims[0] - 1))
          {
            good = 0;
          }
          if(good == 1)
          {
            feature = m_FeatureIds[neighpoint];
            if(feature > 0)
            {
              n[feature]++;
              current = n[feature];
              if(current > most)
              {
                most = current;
                m_Neighbors[i] = neighpoint;
              }
            }
          }
        }
        for(int32_t l = 0; l < 6; l++)
        {
          good = 1;
          neighpoint = i + neighpoints[l];
          if(l == 0 && z == 0)
          {
            good = 0;
          }
          if(l == 5 && z == (dims[2] - 1))
          {
            good = 0;
          }
          if(l == 1 && y == 0)
          {
            good = 0;
          }
          if(l == 4 && y == (dims[1] - 1))
          {
            good = 0;
          }
          if(l == 2 && x == 0)
          {
            good = 0;
          }
          if(l == 3 && x == (dims[0] - 1))
          {
            good = 0;
          }
          if(good == 1)
          {
            feature = m_FeatureIds[neighpoint];
            if(feature > 0)
            {
              n[feature] = 0;
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
      if(featurename < 0 && neighbor != -1 && m_FeatureIds[neighbor] > 0)
      {
        for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(*iter);
          p->copyTuple(neighbor, j);
        }
      }
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FillBadData::newFilterInstance(bool copyFilterParameters)
{
  FillBadData::Pointer filter = FillBadData::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FillBadData::getCompiledLibraryName()
{
  return ProcessingConstants::ProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FillBadData::getBrandingString()
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FillBadData::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << Processing::Version::Major() << "." << Processing::Version::Minor() << "." << Processing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FillBadData::getGroupName()
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FillBadData::getSubGroupName()
{
  return SIMPL::FilterSubGroups::CleanupFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FillBadData::getHumanLabel()
{
  return "Fill Bad Data";
}
