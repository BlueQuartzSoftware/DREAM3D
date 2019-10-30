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

#include "CAxisSegmentFeatures.h"

#include <chrono>
#include <random>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Math/SIMPLibRandom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "OrientationLib/Core/OrientationTransformation.hpp"
#include "OrientationLib/LaueOps/LaueOps.h"

#include "Reconstruction/ReconstructionConstants.h"
#include "Reconstruction/ReconstructionVersion.h"

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
CAxisSegmentFeatures::CAxisSegmentFeatures()
: m_CellFeatureAttributeMatrixName(SIMPL::Defaults::CellFeatureAttributeMatrixName)
, m_MisorientationTolerance(5.0f)
, m_RandomizeFeatureIds(true)
, m_UseGoodVoxels(true)
, m_CellPhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases)
, m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures)
, m_QuatsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Quats)
, m_GoodVoxelsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Mask)
, m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds)
, m_ActiveArrayName(SIMPL::FeatureData::Active)
{
  m_OrientationOps = LaueOps::getOrientationOpsQVector();

  m_MisoTolerance = 0.0f;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CAxisSegmentFeatures::~CAxisSegmentFeatures() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void CAxisSegmentFeatures::setupFilterParameters()
{
  SegmentFeatures::setupFilterParameters();
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_FLOAT_FP("C-Axis Misorientation Tolerance (Degrees)", MisorientationTolerance, FilterParameter::Parameter, CAxisSegmentFeatures));
  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Mask Array", UseGoodVoxels, FilterParameter::Parameter, CAxisSegmentFeatures, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Quaternions", QuatsArrayPath, FilterParameter::RequiredArray, CAxisSegmentFeatures, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, CAxisSegmentFeatures, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", GoodVoxelsArrayPath, FilterParameter::RequiredArray, CAxisSegmentFeatures, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Image);

    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, CAxisSegmentFeatures, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Feature Ids", FeatureIdsArrayName, QuatsArrayPath, QuatsArrayPath, FilterParameter::CreatedArray, CAxisSegmentFeatures));
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Feature Attribute Matrix", CellFeatureAttributeMatrixName, QuatsArrayPath, FilterParameter::CreatedArray, CAxisSegmentFeatures));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Active", ActiveArrayName, QuatsArrayPath, CellFeatureAttributeMatrixName, FilterParameter::CreatedArray, CAxisSegmentFeatures));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setActiveArrayName(reader->readString("ActiveArrayName", getActiveArrayName()));
  setCellFeatureAttributeMatrixName(reader->readString("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName()));
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath()));
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels()));
  setMisorientationTolerance(reader->readValue("MisorientationTolerance", getMisorientationTolerance()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::updateFeatureInstancePointers()
{
  clearErrorCode();
  clearWarningCode();

  if(nullptr != m_ActivePtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Active = m_ActivePtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::initialize()
{
  m_MisoTolerance = 0.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::dataCheck()
{
  DataArrayPath tempPath;
  clearErrorCode();
  clearWarningCode();

  // Set the DataContainerName for the Parent Class (SegmentFeatures) to Use
  setDataContainerName(m_QuatsArrayPath.getDataContainerName());

  SegmentFeatures::dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerName(), false);
  if(getErrorCode() < 0 || nullptr == m)
  {
    return;
  }

  std::vector<size_t> tDims(1, 0);
  m->createNonPrereqAttributeMatrix(this, getCellFeatureAttributeMatrixName(), tDims, AttributeMatrix::Type::CellFeature, AttributeMatrixID21);

  QVector<DataArrayPath> dataArrayPaths;

  std::vector<size_t> cDims(1, 1);
  if(m_UseGoodVoxels)
  {
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

  tempPath.update(getDataContainerName(), m_QuatsArrayPath.getAttributeMatrixName(), getFeatureIdsArrayName());
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims);              /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getDataContainerName(), getCellFeatureAttributeMatrixName(), getActiveArrayName());
  m_ActivePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, true, cDims, "", DataArrayID31);
  if(nullptr != m_ActivePtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Active = m_ActivePtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

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
    dataArrayPaths.push_back(getQuatsArrayPath());
  }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::preflight()
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
void CAxisSegmentFeatures::randomizeFeatureIds(int64_t totalPoints, int64_t totalFeatures)
{
  notifyStatusMessage("Randomizing Feature Ids");
  // Generate an even distribution of numbers between the min and max range
  const int64_t rangeMin = 1;
  const int64_t rangeMax = totalFeatures - 1;
  initializeVoxelSeedGenerator(rangeMin, rangeMax);

  DataArray<int64_t>::Pointer rndNumbers = DataArray<int64_t>::CreateArray(totalFeatures, "_INTERNAL_USE_ONLY_NewFeatureIds", true);

  int64_t* gid = rndNumbers->getPointer(0);
  gid[0] = 0;
  for(int64_t i = 1; i < totalFeatures; ++i)
  {
    gid[i] = i;
  }

  int64_t r = 0;
  int64_t temp = 0;

  //--- Shuffle elements by randomly exchanging each with one other.
  for(int64_t i = 1; i < totalFeatures; i++)
  {
    r = m_Distribution(m_Generator); // Random remaining position.
    if(r >= totalFeatures)
    {
      continue;
    }
    temp = gid[i];
    gid[i] = gid[r];
    gid[r] = temp;
  }

  // Now adjust all the Grain Id values for each Voxel
  for(int64_t i = 0; i < totalPoints; ++i)
  {
    m_FeatureIds[i] = gid[m_FeatureIds[i]];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t CAxisSegmentFeatures::getSeed(int32_t gnum, int64_t nextSeed)
{
  clearErrorCode();
  clearWarningCode();
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  int64_t seed = -1;
  // start with the next voxel after the last seed
  size_t randpoint = static_cast<size_t>(nextSeed);
  while(seed == -1 && randpoint < totalPoints)
  {
    if(m_FeatureIds[randpoint] == 0) // If the GrainId of the voxel is ZERO then we can use this as a seed point
    {
      if((!m_UseGoodVoxels || m_GoodVoxels[randpoint]) && m_CellPhases[randpoint] > 0)
      {
        seed = randpoint;
      }
      else
      {
        randpoint += 1;
      }
    }
    else
    {
      randpoint += 1;
    }
  }
  if(seed >= 0)
  {
    m_FeatureIds[seed] = gnum;
    std::vector<size_t> tDims(1, gnum + 1);
    m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
    updateFeatureInstancePointers();
  }
  return seed;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool CAxisSegmentFeatures::determineGrouping(int64_t referencepoint, int64_t neighborpoint, int32_t gnum)
{
  bool group = false;
  float w = std::numeric_limits<float>::max();
  //  QuatF q1 = QuaternionMathF::New();
  //  QuatF q2 = QuaternionMathF::New();
  //  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

  float g1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float g2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float g1t[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float g2t[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float caxis[3] = {0.0f, 0.0f, 1.0f};
  float c1[3] = {0.0f, 0.0f, 0.0f};
  float c2[3] = {0.0f, 0.0f, 0.0f};

  if(m_FeatureIds[neighborpoint] == 0 && (!m_UseGoodVoxels || m_GoodVoxels[neighborpoint]))
  {
    QuatF q1(m_Quats + referencepoint * 4);
    QuatF q2(m_Quats + neighborpoint * 4);

    if(m_CellPhases[referencepoint] == m_CellPhases[neighborpoint])
    {
      OrientationTransformation::qu2om<QuatF, Orientation<float>>(q1).toGMatrix(g1);
      OrientationTransformation::qu2om<QuatF, Orientation<float>>(q2).toGMatrix(g2);

      // transpose the g matricies so when caxis is multiplied by it
      // it will give the sample direction that the caxis is along
      MatrixMath::Transpose3x3(g1, g1t);
      MatrixMath::Transpose3x3(g2, g2t);
      MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
      MatrixMath::Multiply3x3with3x1(g2t, caxis, c2);

      // normalize so that the dot product can be taken below without
      // dividing by the magnitudes (they would be 1)
      MatrixMath::Normalize3x1(c1);
      MatrixMath::Normalize3x1(c2);

      w = ((c1[0] * c2[0]) + (c1[1] * c2[1]) + (c1[2] * c2[2]));
      w = acosf(w);
      if(w <= m_MisoTolerance || (SIMPLib::Constants::k_Pi - w) <= m_MisoTolerance)
      {
        group = true;
        m_FeatureIds[neighborpoint] = gnum;
      }
    }
  }
  return group;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::initializeVoxelSeedGenerator(const int64_t rangeMin, const int64_t rangeMax)
{

  std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
  m_Generator.seed(seed);
  m_Distribution = std::uniform_int_distribution<int64_t>(rangeMin, rangeMax);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  int64_t totalPoints = static_cast<int64_t>(m_FeatureIdsPtr.lock()->getNumberOfTuples());

  std::vector<size_t> tDims(1, 1);
  m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateFeatureInstancePointers();

  // Convert user defined tolerance to radians.
  m_MisoTolerance = m_MisorientationTolerance * SIMPLib::Constants::k_Pi / 180.0f;

  // Generate the random voxel indices that will be used for the seed points to start a new grain growth/agglomeration
  const int64_t rangeMin = 0;
  const int64_t rangeMax = totalPoints - 1;
  initializeVoxelSeedGenerator(rangeMin, rangeMax);

  SegmentFeatures::execute();

  int64_t totalFeatures = static_cast<int64_t>(m_ActivePtr.lock()->getNumberOfTuples());
  if(totalFeatures < 2)
  {
    setErrorCondition(-87000, "The number of Features was 0 or 1 which means no Features were detected. A threshold value may be set too high");
    return;
  }

  // By default we randomize grains
  if(m_RandomizeFeatureIds)
  {
    totalPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getNumberOfElements());
    randomizeFeatureIds(totalPoints, totalFeatures);
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CAxisSegmentFeatures::newFilterInstance(bool copyFilterParameters) const
{
  CAxisSegmentFeatures::Pointer filter = CAxisSegmentFeatures::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CAxisSegmentFeatures::getCompiledLibraryName() const
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CAxisSegmentFeatures::getBrandingString() const
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CAxisSegmentFeatures::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Reconstruction::Version::Major() << "." << Reconstruction::Version::Minor() << "." << Reconstruction::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CAxisSegmentFeatures::getGroupName() const
{
  return SIMPL::FilterGroups::ReconstructionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid CAxisSegmentFeatures::getUuid() const
{
  return QUuid("{bff6be19-1219-5876-8838-1574ad29d965}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CAxisSegmentFeatures::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::SegmentationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CAxisSegmentFeatures::getHumanLabel() const
{
  return "Segment Features (C-Axis Misalignment)";
}

// -----------------------------------------------------------------------------
CAxisSegmentFeatures::Pointer CAxisSegmentFeatures::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<CAxisSegmentFeatures> CAxisSegmentFeatures::New()
{
  struct make_shared_enabler : public CAxisSegmentFeatures
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString CAxisSegmentFeatures::getNameOfClass() const
{
  return QString("CAxisSegmentFeatures");
}

// -----------------------------------------------------------------------------
QString CAxisSegmentFeatures::ClassName()
{
  return QString("CAxisSegmentFeatures");
}

// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::setCellFeatureAttributeMatrixName(const QString& value)
{
  m_CellFeatureAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString CAxisSegmentFeatures::getCellFeatureAttributeMatrixName() const
{
  return m_CellFeatureAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::setMisorientationTolerance(float value)
{
  m_MisorientationTolerance = value;
}

// -----------------------------------------------------------------------------
float CAxisSegmentFeatures::getMisorientationTolerance() const
{
  return m_MisorientationTolerance;
}

// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::setRandomizeFeatureIds(bool value)
{
  m_RandomizeFeatureIds = value;
}

// -----------------------------------------------------------------------------
bool CAxisSegmentFeatures::getRandomizeFeatureIds() const
{
  return m_RandomizeFeatureIds;
}

// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::setUseGoodVoxels(bool value)
{
  m_UseGoodVoxels = value;
}

// -----------------------------------------------------------------------------
bool CAxisSegmentFeatures::getUseGoodVoxels() const
{
  return m_UseGoodVoxels;
}

// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::setCellPhasesArrayPath(const DataArrayPath& value)
{
  m_CellPhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CAxisSegmentFeatures::getCellPhasesArrayPath() const
{
  return m_CellPhasesArrayPath;
}

// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::setCrystalStructuresArrayPath(const DataArrayPath& value)
{
  m_CrystalStructuresArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CAxisSegmentFeatures::getCrystalStructuresArrayPath() const
{
  return m_CrystalStructuresArrayPath;
}

// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::setQuatsArrayPath(const DataArrayPath& value)
{
  m_QuatsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CAxisSegmentFeatures::getQuatsArrayPath() const
{
  return m_QuatsArrayPath;
}

// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::setGoodVoxelsArrayPath(const DataArrayPath& value)
{
  m_GoodVoxelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CAxisSegmentFeatures::getGoodVoxelsArrayPath() const
{
  return m_GoodVoxelsArrayPath;
}

// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::setFeatureIdsArrayName(const QString& value)
{
  m_FeatureIdsArrayName = value;
}

// -----------------------------------------------------------------------------
QString CAxisSegmentFeatures::getFeatureIdsArrayName() const
{
  return m_FeatureIdsArrayName;
}

// -----------------------------------------------------------------------------
void CAxisSegmentFeatures::setActiveArrayName(const QString& value)
{
  m_ActiveArrayName = value;
}

// -----------------------------------------------------------------------------
QString CAxisSegmentFeatures::getActiveArrayName() const
{
  return m_ActiveArrayName;
}
