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

#include "BadDataNeighborOrientationCheck.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BadDataNeighborOrientationCheck::BadDataNeighborOrientationCheck()
: m_MisorientationTolerance(5.0f)
, m_NumberOfNeighbors(6)
, m_GoodVoxelsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Mask)
, m_CellPhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases)
, m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures)
, m_QuatsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Quats)
{
  m_OrientationOps = LaueOps::getOrientationOpsQVector();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BadDataNeighborOrientationCheck::~BadDataNeighborOrientationCheck() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Misorientation Tolerance (Degrees)", MisorientationTolerance, FilterParameter::Parameter, BadDataNeighborOrientationCheck));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Required Number of Neighbors", NumberOfNeighbors, FilterParameter::Parameter, BadDataNeighborOrientationCheck));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Quaternions", QuatsArrayPath, FilterParameter::RequiredArray, BadDataNeighborOrientationCheck, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", GoodVoxelsArrayPath, FilterParameter::RequiredArray, BadDataNeighborOrientationCheck, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, BadDataNeighborOrientationCheck, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, BadDataNeighborOrientationCheck, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath()));
  setMisorientationTolerance(reader->readValue("MisorientationTolerance", getMisorientationTolerance()));
  setNumberOfNeighbors(reader->readValue("NumberOfNeighbors", getNumberOfNeighbors()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  QVector<DataArrayPath> dataArrayPaths;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getGoodVoxelsArrayPath().getDataContainerName());

  std::vector<size_t> cDims(1, 1);
  m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(),
                                                                                                     cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_GoodVoxelsPtr.lock())                                                                      /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getGoodVoxelsArrayPath());
  }

  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellPhasesPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getCellPhasesArrayPath());
  }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 4;
  m_QuatsPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_QuatsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Quats = m_QuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getCellPhasesArrayPath());
  }

  getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::preflight()
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
void BadDataNeighborOrientationCheck::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  float misorientationTolerance = m_MisorientationTolerance * SIMPLib::Constants::k_Pif / 180.0f;


  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_GoodVoxelsArrayPath.getDataContainerName());
  size_t totalPoints = m_GoodVoxelsPtr.lock()->getNumberOfTuples();

  SizeVec3Type udims = m->getGeometryAs<ImageGeom>()->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  int32_t good = 1;
  int64_t neighbor = 0;
  int64_t column = 0, row = 0, plane = 0;

  int64_t neighpoints[6] = {0, 0, 0, 0, 0, 0};
  neighpoints[0] = static_cast<int64_t>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int64_t>(-dims[0]);
  neighpoints[2] = static_cast<int64_t>(-1);
  neighpoints[3] = static_cast<int64_t>(1);
  neighpoints[4] = static_cast<int64_t>(dims[0]);
  neighpoints[5] = static_cast<int64_t>(dims[0] * dims[1]);

  float w = 10000.0f;

  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  uint32_t phase1 = 0, phase2 = 0;

  QVector<int32_t> neighborCount(totalPoints, 0);

  for(size_t i = 0; i < totalPoints; i++)
  {
    if(!m_GoodVoxels[i])
    {
      column = i % dims[0];
      row = (i / dims[0]) % dims[1];
      plane = i / (dims[0] * dims[1]);
      for(int32_t j = 0; j < 6; j++)
      {
        good = 1;
        neighbor = i + neighpoints[j];
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
        if(good == 1 && m_GoodVoxels[neighbor])
        {
          phase1 = m_CrystalStructures[m_CellPhases[i]];
          QuatF q1(m_Quats + i * 4); // BEWARE POINTER MATH!!

          phase2 = m_CrystalStructures[m_CellPhases[neighbor]];
          QuatF q2(m_Quats + neighbor * 4); // BEWARE POINTER MATH!!

          if(m_CellPhases[i] == m_CellPhases[neighbor] && m_CellPhases[i] > 0)
          {
            w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);
          }
          if(w < misorientationTolerance)
          {
            neighborCount[i]++;
          }
        }
      }
    }
  }

  int32_t currentLevel = 6;
  int32_t counter = 0;

  while(currentLevel > m_NumberOfNeighbors)
  {
    counter = 1;
    while(counter > 0)
    {
      counter = 0;
      for(size_t i = 0; i < totalPoints; i++)
      {
        if(neighborCount[i] >= currentLevel && !m_GoodVoxels[i])
        {
          m_GoodVoxels[i] = true;
          counter++;
          column = i % dims[0];
          row = (i / dims[0]) % dims[1];
          plane = i / (dims[0] * dims[1]);
          for(int64_t j = 0; j < 6; j++)
          {
            good = 1;
            neighbor = i + neighpoints[j];
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
            if(good == 1 && !m_GoodVoxels[neighbor])
            {
              phase1 = m_CrystalStructures[m_CellPhases[i]];
              QuatF q1(m_Quats + i * 4); // BEWARE POINTER MATH!!

              phase2 = m_CrystalStructures[m_CellPhases[neighbor]];
              QuatF q2(m_Quats + neighbor * 4); // BEWARE POINTER MATH!!

              if(m_CellPhases[i] == m_CellPhases[neighbor] && m_CellPhases[i] > 0)
              {
                w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);
              }
              if(w < misorientationTolerance)
              {
                neighborCount[neighbor]++;
              }
            }
          }
        }
      }
    }
    currentLevel = currentLevel - 1;
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer BadDataNeighborOrientationCheck::newFilterInstance(bool copyFilterParameters) const
{
  BadDataNeighborOrientationCheck::Pointer filter = BadDataNeighborOrientationCheck::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString BadDataNeighborOrientationCheck::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString BadDataNeighborOrientationCheck::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString BadDataNeighborOrientationCheck::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString BadDataNeighborOrientationCheck::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid BadDataNeighborOrientationCheck::getUuid() const
{
  return QUuid("{f4a7c2df-e9b0-5da9-b745-a862666d6c99}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString BadDataNeighborOrientationCheck::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CleanupFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString BadDataNeighborOrientationCheck::getHumanLabel() const
{
  return "Neighbor Orientation Comparison (Bad Data)";
}

// -----------------------------------------------------------------------------
BadDataNeighborOrientationCheck::Pointer BadDataNeighborOrientationCheck::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<BadDataNeighborOrientationCheck> BadDataNeighborOrientationCheck::New()
{
  struct make_shared_enabler : public BadDataNeighborOrientationCheck
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString BadDataNeighborOrientationCheck::getNameOfClass() const
{
  return QString("BadDataNeighborOrientationCheck");
}

// -----------------------------------------------------------------------------
QString BadDataNeighborOrientationCheck::ClassName()
{
  return QString("BadDataNeighborOrientationCheck");
}

// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::setMisorientationTolerance(float value)
{
  m_MisorientationTolerance = value;
}

// -----------------------------------------------------------------------------
float BadDataNeighborOrientationCheck::getMisorientationTolerance() const
{
  return m_MisorientationTolerance;
}

// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::setNumberOfNeighbors(int value)
{
  m_NumberOfNeighbors = value;
}

// -----------------------------------------------------------------------------
int BadDataNeighborOrientationCheck::getNumberOfNeighbors() const
{
  return m_NumberOfNeighbors;
}

// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::setGoodVoxelsArrayPath(const DataArrayPath& value)
{
  m_GoodVoxelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath BadDataNeighborOrientationCheck::getGoodVoxelsArrayPath() const
{
  return m_GoodVoxelsArrayPath;
}

// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::setCellPhasesArrayPath(const DataArrayPath& value)
{
  m_CellPhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath BadDataNeighborOrientationCheck::getCellPhasesArrayPath() const
{
  return m_CellPhasesArrayPath;
}

// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::setCrystalStructuresArrayPath(const DataArrayPath& value)
{
  m_CrystalStructuresArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath BadDataNeighborOrientationCheck::getCrystalStructuresArrayPath() const
{
  return m_CrystalStructuresArrayPath;
}

// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::setQuatsArrayPath(const DataArrayPath& value)
{
  m_QuatsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath BadDataNeighborOrientationCheck::getQuatsArrayPath() const
{
  return m_QuatsArrayPath;
}
