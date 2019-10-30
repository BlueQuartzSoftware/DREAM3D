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

#include "FindLocalAverageCAxisMisalignments.h"

#include "OrientationLib/LaueOps/LaueOps.h"
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Math/SIMPLibRandom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLocalAverageCAxisMisalignments::FindLocalAverageCAxisMisalignments()
: m_NewCellFeatureAttributeMatrixName("", "", "")
, m_CalcUnbiasedAvg(false)
, m_CalcBiasedAvg(false)
, m_NeighborListArrayPath("", "", "")
, m_CAxisMisalignmentListArrayPath("", "", "")
, m_FeatureIdsArrayPath("", "", "")
, m_CellParentIdsArrayPath("", "", "")
, m_AvgCAxisMisalignmentsArrayPath("", "", "")
, m_FeatureParentIdsArrayPath("", "", "")
, m_CrystalStructuresArrayPath("", "", "")
, m_UnbiasedLocalCAxisMisalignmentsArrayName(SIMPL::FeatureData::UnbiasedLocalCAxisMisalignments)
, m_NumFeaturesPerParentArrayName(SIMPL::FeatureData::NumFeaturesPerParent)
{
  m_OrientationOps = LaueOps::getOrientationOpsQVector();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLocalAverageCAxisMisalignments::~FindLocalAverageCAxisMisalignments() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  QStringList linkedProps("LocalCAxisMisalignmentsArrayName");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Calculate Local C-Axis Misalignments", CalcBiasedAvg, FilterParameter::Parameter, FindLocalAverageCAxisMisalignments, linkedProps));
  linkedProps.clear();
  linkedProps << "UnbiasedLocalCAxisMisalignmentsArrayName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Calculate Unbiased Local C-Axis Misalignments", CalcUnbiasedAvg, FilterParameter::Parameter, FindLocalAverageCAxisMisalignments, linkedProps));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Neighbor List Array Name", NeighborListArrayPath, FilterParameter::RequiredArray, FindLocalAverageCAxisMisalignments, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("CAxis Misalignment List Array Name", CAxisMisalignmentListArrayPath, FilterParameter::RequiredArray, FindLocalAverageCAxisMisalignments, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("FeatureIds", FeatureIdsArrayPath, FilterParameter::RequiredArray, FindLocalAverageCAxisMisalignments, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("CellParentIds", CellParentIdsArrayPath, FilterParameter::RequiredArray, FindLocalAverageCAxisMisalignments, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("AvgCAxisMisalignments", AvgCAxisMisalignmentsArrayPath, FilterParameter::RequiredArray, FindLocalAverageCAxisMisalignments, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("FeatureParentIds", FeatureParentIdsArrayPath, FilterParameter::RequiredArray, FindLocalAverageCAxisMisalignments, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, FindLocalAverageCAxisMisalignments, req));
  }
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::CellFeature, IGeometry::Type::Unknown);
    parameters.push_back(
        SIMPL_NEW_AM_SELECTION_FP("New Cell Feature Attribute Matrix Name", NewCellFeatureAttributeMatrixName, FilterParameter::RequiredArray, FindLocalAverageCAxisMisalignments, req));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("NumFeaturesPerParent", NumFeaturesPerParentArrayName, FilterParameter::CreatedArray, FindLocalAverageCAxisMisalignments));
  parameters.push_back(SIMPL_NEW_STRING_FP("LocalCAxisMisalignments", LocalCAxisMisalignmentsArrayName, FilterParameter::CreatedArray, FindLocalAverageCAxisMisalignments));
  parameters.push_back(SIMPL_NEW_STRING_FP("UnbiasedLocalCAxisMisalignments", UnbiasedLocalCAxisMisalignmentsArrayName, FilterParameter::CreatedArray, FindLocalAverageCAxisMisalignments));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewCellFeatureAttributeMatrixName(reader->readDataArrayPath("NewCellFeatureAttributeMatrixName", getNewCellFeatureAttributeMatrixName()));
  setNeighborListArrayPath(reader->readDataArrayPath("NeighborListArrayPath", getNeighborListArrayPath()));
  setCAxisMisalignmentListArrayPath(reader->readDataArrayPath("CAxisMisalignmentListArrayPath", getCAxisMisalignmentListArrayPath()));
  setNumFeaturesPerParentArrayName(reader->readString("NumFeaturesPerParentArrayName", getNumFeaturesPerParentArrayName()));
  setLocalCAxisMisalignmentsArrayName(reader->readString("LocalCAxisMisalignmentsArrayName", getLocalCAxisMisalignmentsArrayName()));
  setUnbiasedLocalCAxisMisalignmentsArrayName(reader->readString("UnbiasedLocalCAxisMisalignmentsArrayName", getUnbiasedLocalCAxisMisalignmentsArrayName()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setFeatureParentIdsArrayPath(reader->readDataArrayPath("FeatureParentIdsArrayPath", getFeatureParentIdsArrayPath()));
  setAvgCAxisMisalignmentsArrayPath(reader->readDataArrayPath("AvgCAxisMisalignmentsArrayPath", getAvgCAxisMisalignmentsArrayPath()));
  setCellParentIdsArrayPath(reader->readDataArrayPath("CellParentIdsArrayPath", getCellParentIdsArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setCalcUnbiasedAvg(reader->readValue("CalcUnbiasedAvg", getCalcUnbiasedAvg()));
  setCalcBiasedAvg(reader->readValue("CalcBiasedAvg", getCalcBiasedAvg()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::initialize()
{
  m_NeighborList = NeighborList<int32_t>::NullPointer();
  m_CAxisMisalignmentList = NeighborList<float>::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::dataCheck()
{
  DataArrayPath tempPath;
  clearErrorCode();
  clearWarningCode();

  // Cell Data
  std::vector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())                                                                        /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellParentIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellParentIdsArrayPath(),
                                                                                                           dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellParentIdsPtr.lock())                                                                        /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellParentIds = m_CellParentIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Data
  if(m_CalcUnbiasedAvg)
  {
    tempPath.update(getNewCellFeatureAttributeMatrixName().getDataContainerName(), getNewCellFeatureAttributeMatrixName().getAttributeMatrixName(), getUnbiasedLocalCAxisMisalignmentsArrayName());
    m_UnbiasedLocalCAxisMisalignmentsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
        this, tempPath, 0.0, dims);                                  /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_UnbiasedLocalCAxisMisalignmentsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_UnbiasedLocalCAxisMisalignments = m_UnbiasedLocalCAxisMisalignmentsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
    m_NeighborList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int>, AbstractFilter>(this, getNeighborListArrayPath(), dims);
    m_CAxisMisalignmentList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<float>, AbstractFilter>(this, getCAxisMisalignmentListArrayPath(), dims);
  }

  if(m_CalcBiasedAvg)
  {
    m_AvgCAxisMisalignmentsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgCAxisMisalignmentsArrayPath(),
                                                                                                                   dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_AvgCAxisMisalignmentsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_AvgCAxisMisalignments = m_AvgCAxisMisalignmentsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    tempPath.update(getNewCellFeatureAttributeMatrixName().getDataContainerName(), getNewCellFeatureAttributeMatrixName().getAttributeMatrixName(), getLocalCAxisMisalignmentsArrayName());
    m_LocalCAxisMisalignmentsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
        this, tempPath, 0.0, dims);                          /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_LocalCAxisMisalignmentsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_LocalCAxisMisalignments = m_LocalCAxisMisalignmentsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  m_FeatureParentIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureParentIdsArrayPath(),
                                                                                                              dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureParentIdsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureParentIds = m_FeatureParentIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // New Feature Data
  tempPath.update(getNewCellFeatureAttributeMatrixName().getDataContainerName(), getNewCellFeatureAttributeMatrixName().getAttributeMatrixName(), getNumFeaturesPerParentArrayName());
  m_NumFeaturesPerParentPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, float>(
      this, tempPath, 0, dims);                         /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_NumFeaturesPerParentPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_NumFeaturesPerParent = m_NumFeaturesPerParentPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Ensemble Data
  // typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                    dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::preflight()
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
void FindLocalAverageCAxisMisalignments::execute()
{
  clearErrorCode();
  clearWarningCode();

  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  size_t numFeatures = m_FeatureParentIdsPtr.lock()->getNumberOfTuples();
  size_t newNumFeatures = m_NumFeaturesPerParentPtr.lock()->getNumberOfTuples();

  std::vector<int32_t> NumUnbiasedFeaturesPerParent(numFeatures, 0);

  if(m_CalcUnbiasedAvg)
  {
    NeighborList<int>& neighborlist = *(m_NeighborList.lock());
    NeighborList<float>& caxismisalignmentList = *(m_CAxisMisalignmentList.lock());
    for(size_t i = 1; i < numFeatures; i++)
    {
      int parentid = m_FeatureParentIds[i];
      for(size_t j = 0; j < neighborlist[i].size(); j++)
      {
        if(m_FeatureParentIds[neighborlist[i][j]] == m_FeatureParentIds[i])
        {
          m_UnbiasedLocalCAxisMisalignments[parentid] += caxismisalignmentList[i][j];
          NumUnbiasedFeaturesPerParent[parentid]++;
        }
      }
    }
  }

  if(m_CalcBiasedAvg)
  {
    for(size_t i = 1; i < numFeatures; i++)
    {
      int parentid = m_FeatureParentIds[i];
      m_NumFeaturesPerParent[parentid]++;
      m_LocalCAxisMisalignments[parentid] += m_AvgCAxisMisalignments[i];
    }
  }

  for(size_t i = 1; i < newNumFeatures; i++)
  {
    if(m_CalcBiasedAvg)
    {
      m_LocalCAxisMisalignments[i] /= m_NumFeaturesPerParent[i];
    }
    if(m_CalcUnbiasedAvg)
    {
      if(NumUnbiasedFeaturesPerParent[i] > 0)
      {
        m_UnbiasedLocalCAxisMisalignments[i] /= NumUnbiasedFeaturesPerParent[i];
      }
      else
      {
        m_UnbiasedLocalCAxisMisalignments[i] = 0.0f;
      }
    }
  }


}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindLocalAverageCAxisMisalignments::newFilterInstance(bool copyFilterParameters) const
{
  FindLocalAverageCAxisMisalignments::Pointer filter = FindLocalAverageCAxisMisalignments::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindLocalAverageCAxisMisalignments::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindLocalAverageCAxisMisalignments::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindLocalAverageCAxisMisalignments::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindLocalAverageCAxisMisalignments::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindLocalAverageCAxisMisalignments::getUuid() const
{
  return QUuid("{49b2dd47-bb29-50d4-a051-5bad9b6b9f80}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindLocalAverageCAxisMisalignments::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindLocalAverageCAxisMisalignments::getHumanLabel() const
{
  return "Find Local Average C-Axis Misalignments";
}

// -----------------------------------------------------------------------------
FindLocalAverageCAxisMisalignments::Pointer FindLocalAverageCAxisMisalignments::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindLocalAverageCAxisMisalignments> FindLocalAverageCAxisMisalignments::New()
{
  struct make_shared_enabler : public FindLocalAverageCAxisMisalignments
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindLocalAverageCAxisMisalignments::getNameOfClass() const
{
  return QString("FindLocalAverageCAxisMisalignments");
}

// -----------------------------------------------------------------------------
QString FindLocalAverageCAxisMisalignments::ClassName()
{
  return QString("FindLocalAverageCAxisMisalignments");
}

// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::setNewCellFeatureAttributeMatrixName(const DataArrayPath& value)
{
  m_NewCellFeatureAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindLocalAverageCAxisMisalignments::getNewCellFeatureAttributeMatrixName() const
{
  return m_NewCellFeatureAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::setCalcUnbiasedAvg(bool value)
{
  m_CalcUnbiasedAvg = value;
}

// -----------------------------------------------------------------------------
bool FindLocalAverageCAxisMisalignments::getCalcUnbiasedAvg() const
{
  return m_CalcUnbiasedAvg;
}

// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::setCalcBiasedAvg(bool value)
{
  m_CalcBiasedAvg = value;
}

// -----------------------------------------------------------------------------
bool FindLocalAverageCAxisMisalignments::getCalcBiasedAvg() const
{
  return m_CalcBiasedAvg;
}

// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::setNeighborListArrayPath(const DataArrayPath& value)
{
  m_NeighborListArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindLocalAverageCAxisMisalignments::getNeighborListArrayPath() const
{
  return m_NeighborListArrayPath;
}

// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::setCAxisMisalignmentListArrayPath(const DataArrayPath& value)
{
  m_CAxisMisalignmentListArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindLocalAverageCAxisMisalignments::getCAxisMisalignmentListArrayPath() const
{
  return m_CAxisMisalignmentListArrayPath;
}

// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindLocalAverageCAxisMisalignments::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::setCellParentIdsArrayPath(const DataArrayPath& value)
{
  m_CellParentIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindLocalAverageCAxisMisalignments::getCellParentIdsArrayPath() const
{
  return m_CellParentIdsArrayPath;
}

// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::setAvgCAxisMisalignmentsArrayPath(const DataArrayPath& value)
{
  m_AvgCAxisMisalignmentsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindLocalAverageCAxisMisalignments::getAvgCAxisMisalignmentsArrayPath() const
{
  return m_AvgCAxisMisalignmentsArrayPath;
}

// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::setFeatureParentIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureParentIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindLocalAverageCAxisMisalignments::getFeatureParentIdsArrayPath() const
{
  return m_FeatureParentIdsArrayPath;
}

// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::setCrystalStructuresArrayPath(const DataArrayPath& value)
{
  m_CrystalStructuresArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindLocalAverageCAxisMisalignments::getCrystalStructuresArrayPath() const
{
  return m_CrystalStructuresArrayPath;
}

// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::setUnbiasedLocalCAxisMisalignmentsArrayName(const QString& value)
{
  m_UnbiasedLocalCAxisMisalignmentsArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindLocalAverageCAxisMisalignments::getUnbiasedLocalCAxisMisalignmentsArrayName() const
{
  return m_UnbiasedLocalCAxisMisalignmentsArrayName;
}

// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::setLocalCAxisMisalignmentsArrayName(const QString& value)
{
  m_LocalCAxisMisalignmentsArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindLocalAverageCAxisMisalignments::getLocalCAxisMisalignmentsArrayName() const
{
  return m_LocalCAxisMisalignmentsArrayName;
}

// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::setNumFeaturesPerParentArrayName(const QString& value)
{
  m_NumFeaturesPerParentArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindLocalAverageCAxisMisalignments::getNumFeaturesPerParentArrayName() const
{
  return m_NumFeaturesPerParentArrayName;
}
