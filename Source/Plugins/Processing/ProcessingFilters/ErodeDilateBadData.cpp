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

#include "ErodeDilateBadData.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
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
ErodeDilateBadData::ErodeDilateBadData()
: m_Direction(0)
, m_NumIterations(1)
, m_XDirOn(true)
, m_YDirOn(true)
, m_ZDirOn(true)
, m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_Neighbors(nullptr)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ErodeDilateBadData::~ErodeDilateBadData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateBadData::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Operation");
    parameter->setPropertyName("Direction");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(ErodeDilateBadData, this, Direction));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(ErodeDilateBadData, this, Direction));

    QVector<QString> choices;
    choices.push_back("Dilate");
    choices.push_back("Erode");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Iterations", NumIterations, FilterParameter::Parameter, ErodeDilateBadData));
  parameters.push_back(SIMPL_NEW_BOOL_FP("X Direction", XDirOn, FilterParameter::Parameter, ErodeDilateBadData));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Y Direction", YDirOn, FilterParameter::Parameter, ErodeDilateBadData));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Z Direction", ZDirOn, FilterParameter::Parameter, ErodeDilateBadData));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, ErodeDilateBadData, req));
  }
  {
    MultiDataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Attribute Arrays to Ignore", IgnoredDataArrayPaths, FilterParameter::Parameter, ErodeDilateBadData, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateBadData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setDirection(reader->readValue("Direction", getDirection()));
  setNumIterations(reader->readValue("NumIterations", getNumIterations()));
  setXDirOn(reader->readValue("XDirOn", getXDirOn()));
  setYDirOn(reader->readValue("YDirOn", getYDirOn()));
  setZDirOn(reader->readValue("ZDirOn", getZDirOn()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateBadData::initialize()
{
  m_Neighbors = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateBadData::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  initialize();
  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  if(getNumIterations() <= 0)
  {
    QString ss = QObject::tr("The number of iterations (%1) must be positive").arg(getNumIterations());
    setErrorCondition(-5555, ss);
  }

  std::vector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateBadData::preflight()
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
void ErodeDilateBadData::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(totalPoints, "_INTERNAL_USE_ONLY_Neighbors", true);
  m_Neighbors = neighborsPtr->getPointer(0);
  neighborsPtr->initializeWithValue(-1);

  SizeVec3Type udims = m->getGeometryAs<ImageGeom>()->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  int32_t good = 1;
  int64_t count = 0;
  int64_t kstride = 0, jstride = 0;
  int32_t featurename = 0, feature = 0;
  int32_t current = 0;
  int32_t most = 0;
  int64_t neighpoint = 0;
  size_t numfeatures = 0;

  for(size_t i = 0; i < totalPoints; i++)
  {
    featurename = m_FeatureIds[i];
    if(featurename > numfeatures)
    {
      numfeatures = featurename;
    }
  }

  int64_t neighpoints[6] = {0, 0, 0, 0, 0, 0};
  neighpoints[0] = -dims[0] * dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0] * dims[1];

  QVector<int32_t> n(numfeatures + 1, 0);

  for(int32_t iteration = 0; iteration < m_NumIterations; iteration++)
  {
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
          if(featurename == 0)
          {
            current = 0;
            most = 0;
            for(int32_t l = 0; l < 6; l++)
            {
              good = 1;
              neighpoint = count + neighpoints[l];
              if(l == 0 && (k == 0 || !m_ZDirOn))
              {
                good = 0;
              }
              else if(l == 5 && (k == (dims[2] - 1) || !m_ZDirOn))
              {
                good = 0;
              }
              else if(l == 1 && (j == 0 || !m_YDirOn))
              {
                good = 0;
              }
              else if(l == 4 && (j == (dims[1] - 1) || !m_YDirOn))
              {
                good = 0;
              }
              else if(l == 2 && (i == 0 || !m_XDirOn))
              {
                good = 0;
              }
              else if(l == 3 && (i == (dims[0] - 1) || !m_XDirOn))
              {
                good = 0;
              }
              if(good == 1)
              {
                feature = m_FeatureIds[neighpoint];
                if(m_Direction == 0 && feature > 0)
                {
                  m_Neighbors[neighpoint] = count;
                }
                if(feature > 0 && m_Direction == 1)
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
            if(m_Direction == 1)
            {
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

    for(size_t j = 0; j < totalPoints; j++)
    {
      featurename = m_FeatureIds[j];
      int32_t neighbor = m_Neighbors[j];
      if(neighbor >= 0)
      {
        if((featurename == 0 && m_FeatureIds[neighbor] > 0 && m_Direction == 1) || (featurename > 0 && m_FeatureIds[neighbor] == 0 && m_Direction == 0))
        {
          for(const auto& arrayName : voxelArrayNames)
          {
            IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(arrayName);
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
AbstractFilter::Pointer ErodeDilateBadData::newFilterInstance(bool copyFilterParameters) const
{
  ErodeDilateBadData::Pointer filter = ErodeDilateBadData::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ErodeDilateBadData::getCompiledLibraryName() const
{
  return ProcessingConstants::ProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ErodeDilateBadData::getBrandingString() const
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ErodeDilateBadData::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Processing::Version::Major() << "." << Processing::Version::Minor() << "." << Processing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ErodeDilateBadData::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ErodeDilateBadData::getUuid() const
{
  return QUuid("{3adfe077-c3c9-5cd0-ad74-cf5f8ff3d254}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ErodeDilateBadData::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CleanupFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ErodeDilateBadData::getHumanLabel() const
{
  return "Erode/Dilate Bad Data";
}

// -----------------------------------------------------------------------------
ErodeDilateBadData::Pointer ErodeDilateBadData::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ErodeDilateBadData> ErodeDilateBadData::New()
{
  struct make_shared_enabler : public ErodeDilateBadData
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ErodeDilateBadData::getNameOfClass() const
{
  return QString("ErodeDilateBadData");
}

// -----------------------------------------------------------------------------
QString ErodeDilateBadData::ClassName()
{
  return QString("ErodeDilateBadData");
}

// -----------------------------------------------------------------------------
void ErodeDilateBadData::setDirection(unsigned int value)
{
  m_Direction = value;
}

// -----------------------------------------------------------------------------
unsigned int ErodeDilateBadData::getDirection() const
{
  return m_Direction;
}

// -----------------------------------------------------------------------------
void ErodeDilateBadData::setNumIterations(int value)
{
  m_NumIterations = value;
}

// -----------------------------------------------------------------------------
int ErodeDilateBadData::getNumIterations() const
{
  return m_NumIterations;
}

// -----------------------------------------------------------------------------
void ErodeDilateBadData::setXDirOn(bool value)
{
  m_XDirOn = value;
}

// -----------------------------------------------------------------------------
bool ErodeDilateBadData::getXDirOn() const
{
  return m_XDirOn;
}

// -----------------------------------------------------------------------------
void ErodeDilateBadData::setYDirOn(bool value)
{
  m_YDirOn = value;
}

// -----------------------------------------------------------------------------
bool ErodeDilateBadData::getYDirOn() const
{
  return m_YDirOn;
}

// -----------------------------------------------------------------------------
void ErodeDilateBadData::setZDirOn(bool value)
{
  m_ZDirOn = value;
}

// -----------------------------------------------------------------------------
bool ErodeDilateBadData::getZDirOn() const
{
  return m_ZDirOn;
}

// -----------------------------------------------------------------------------
void ErodeDilateBadData::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ErodeDilateBadData::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void ErodeDilateBadData::setIgnoredDataArrayPaths(const QVector<DataArrayPath>& value)
{
  m_IgnoredDataArrayPaths = value;
}

// -----------------------------------------------------------------------------
QVector<DataArrayPath> ErodeDilateBadData::getIgnoredDataArrayPaths() const
{
  return m_IgnoredDataArrayPaths;
}
