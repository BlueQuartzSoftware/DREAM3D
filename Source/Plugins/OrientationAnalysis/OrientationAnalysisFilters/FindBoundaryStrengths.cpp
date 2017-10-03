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

#include "FindBoundaryStrengths.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

// Include the MOC generated file for this class
#include "moc_FindBoundaryStrengths.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundaryStrengths::FindBoundaryStrengths()
: AbstractFilter()
, m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels)
, m_AvgQuatsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::AvgQuats)
, m_FeaturePhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases)
, m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures)
, m_SurfaceMeshF1sArrayName(SIMPL::FaceData::SurfaceMeshF1s)
, m_SurfaceMeshF1sptsArrayName(SIMPL::FaceData::SurfaceMeshF1spts)
, m_SurfaceMeshF7sArrayName(SIMPL::FaceData::SurfaceMeshF7s)
, m_SurfaceMeshmPrimesArrayName(SIMPL::FaceData::SurfaceMeshmPrimes)
, m_FeaturePhases(nullptr)
, m_AvgQuats(nullptr)
, m_CrystalStructures(nullptr)
, m_SurfaceMeshFaceLabels(nullptr)
, m_SurfaceMeshF1s(nullptr)
, m_SurfaceMeshF1spts(nullptr)
, m_SurfaceMeshF7s(nullptr)
, m_SurfaceMeshmPrimes(nullptr)
{
  m_OrientationOps = LaueOps::getOrientationOpsQVector();

  m_Loading.x = 1.0f;
  m_Loading.y = 1.0f;
  m_Loading.z = 1.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundaryStrengths::~FindBoundaryStrengths()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryStrengths::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Loading Direction (XYZ)", Loading, FilterParameter::Parameter, FindBoundaryStrengths));
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));

  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", SurfaceMeshFaceLabelsArrayPath, FilterParameter::RequiredArray, FindBoundaryStrengths, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Average Quaternions", AvgQuatsArrayPath, FilterParameter::RequiredArray, FindBoundaryStrengths, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, FindBoundaryStrengths, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, FindBoundaryStrengths, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("F1s", SurfaceMeshF1sArrayName, FilterParameter::CreatedArray, FindBoundaryStrengths));
  parameters.push_back(SIMPL_NEW_STRING_FP("F1spts", SurfaceMeshF1sptsArrayName, FilterParameter::CreatedArray, FindBoundaryStrengths));
  parameters.push_back(SIMPL_NEW_STRING_FP("F7s", SurfaceMeshF7sArrayName, FilterParameter::CreatedArray, FindBoundaryStrengths));
  parameters.push_back(SIMPL_NEW_STRING_FP("mPrimes", SurfaceMeshmPrimesArrayName, FilterParameter::CreatedArray, FindBoundaryStrengths));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryStrengths::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshmPrimesArrayName(reader->readString("SurfaceMeshmPrimesArrayName", getSurfaceMeshmPrimesArrayName()));
  setSurfaceMeshF7sArrayName(reader->readString("SurfaceMeshF7sArrayName", getSurfaceMeshF7sArrayName()));
  setSurfaceMeshF1sptsArrayName(reader->readString("SurfaceMeshF1sptsArrayName", getSurfaceMeshF1sptsArrayName()));
  setSurfaceMeshF1sArrayName(reader->readString("SurfaceMeshF1sArrayName", getSurfaceMeshF1sArrayName()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath()));
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath()));
  setLoading(reader->readFloatVec3("Loading", getLoading()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryStrengths::dataCheckSurfaceMesh()
{
  setErrorCondition(0);
  setWarningCondition(0);
  DataArrayPath tempPath;

  TriangleGeom::Pointer triangles = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  QVector<IDataArray::Pointer> dataArrays;

  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(triangles->getTriangles());
  }

  QVector<size_t> cDims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(),
                                                                                                                   cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceLabelsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(m_SurfaceMeshFaceLabelsPtr.lock());
  }

  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), getSurfaceMeshFaceLabelsArrayPath().getAttributeMatrixName(), getSurfaceMeshF1sArrayName());
  m_SurfaceMeshF1sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims);                  /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshF1sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshF1s = m_SurfaceMeshF1sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), getSurfaceMeshFaceLabelsArrayPath().getAttributeMatrixName(), getSurfaceMeshF1sptsArrayName());
  m_SurfaceMeshF1sptsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims);                     /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshF1sptsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshF1spts = m_SurfaceMeshF1sptsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), getSurfaceMeshFaceLabelsArrayPath().getAttributeMatrixName(), getSurfaceMeshF7sArrayName());
  m_SurfaceMeshF7sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims);                  /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshF7sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshF7s = m_SurfaceMeshF7sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), getSurfaceMeshFaceLabelsArrayPath().getAttributeMatrixName(), getSurfaceMeshmPrimesArrayName());
  m_SurfaceMeshmPrimesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims);                      /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshmPrimesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshmPrimes = m_SurfaceMeshmPrimesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrays);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryStrengths::dataCheckVoxel()
{
  setErrorCondition(0);
  setWarningCondition(0);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getAvgQuatsArrayPath().getDataContainerName());

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 4);
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(),
                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AvgQuatsPtr.lock().get())                                                                 /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getAvgQuatsArrayPath());
  }

  cDims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getFeaturePhasesArrayPath());
  }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryStrengths::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheckVoxel();
  dataCheckSurfaceMesh();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryStrengths::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheckVoxel();
  if(getErrorCondition() < 0)
  {
    return;
  }
  dataCheckSurfaceMesh();
  if(getErrorCondition() < 0)
  {
    return;
  }

  size_t numTriangles = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();

  float mPrime_1 = 0.0f, mPrime_2 = 0.0f, F1_1 = 0.0f, F1_2 = 0.0f, F1spt_1 = 0.0f, F1spt_2 = 0.0f, F7_1 = 0.0f, F7_2 = 0.0f;
  int32_t gname1 = 0, gname2 = 0;
  QuatF q1 = QuaternionMathF::New();
  QuatF q2 = QuaternionMathF::New();
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  float LD[3] = {0.0f, 0.0f, 0.0f};

  LD[0] = m_Loading.x;
  LD[1] = m_Loading.y;
  LD[2] = m_Loading.z;
  MatrixMath::Normalize3x1(LD);

  for(size_t i = 0; i < numTriangles; i++)
  {
    gname1 = m_SurfaceMeshFaceLabels[i * 2];
    gname2 = m_SurfaceMeshFaceLabels[i * 2 + 1];
    if(gname1 > 0 && gname2 > 0)
    {
      QuaternionMathF::Copy(avgQuats[gname1], q1);
      QuaternionMathF::Copy(avgQuats[gname2], q2);
      if(m_CrystalStructures[m_FeaturePhases[gname1]] == m_CrystalStructures[m_FeaturePhases[gname2]] && m_FeaturePhases[gname1] > 0)
      {
        m_OrientationOps[m_CrystalStructures[m_FeaturePhases[gname1]]]->getmPrime(q1, q2, LD, mPrime_1);
        m_OrientationOps[m_CrystalStructures[m_FeaturePhases[gname1]]]->getmPrime(q2, q1, LD, mPrime_2);
        m_OrientationOps[m_CrystalStructures[m_FeaturePhases[gname1]]]->getF1(q1, q2, LD, true, F1_1);
        m_OrientationOps[m_CrystalStructures[m_FeaturePhases[gname1]]]->getF1(q2, q1, LD, true, F1_2);
        m_OrientationOps[m_CrystalStructures[m_FeaturePhases[gname1]]]->getF1spt(q1, q2, LD, true, F1spt_1);
        m_OrientationOps[m_CrystalStructures[m_FeaturePhases[gname1]]]->getF1spt(q2, q1, LD, true, F1spt_2);
        m_OrientationOps[m_CrystalStructures[m_FeaturePhases[gname1]]]->getF7(q1, q2, LD, true, F7_1);
        m_OrientationOps[m_CrystalStructures[m_FeaturePhases[gname1]]]->getF7(q2, q1, LD, true, F7_2);
      }
      else
      {
        mPrime_1 = 0.0f;
        F1_1 = 0.0f;
        F1spt_1 = 0.0f;
        F7_1 = 0.0f;
        mPrime_2 = 0.0f;
        F1_2 = 0.0f;
        F1spt_2 = 0.0f;
        F7_2 = 0.0f;
      }
    }
    else
    {
      mPrime_1 = 0.0f;
      F1_1 = 0.0f;
      F1spt_1 = 0.0f;
      F7_1 = 0.0f;
      mPrime_2 = 0.0f;
      F1_2 = 0.0f;
      F1spt_2 = 0.0f;
      F7_2 = 0.0f;
    }
    m_SurfaceMeshmPrimes[2 * i] = mPrime_1;
    m_SurfaceMeshmPrimes[2 * i + 1] = mPrime_2;
    m_SurfaceMeshF1s[2 * i] = F1_1;
    m_SurfaceMeshF1s[2 * i + 1] = F1_2;
    m_SurfaceMeshF1spts[2 * i] = F1spt_1;
    m_SurfaceMeshF1spts[2 * i + 1] = F1spt_2;
    m_SurfaceMeshF7s[2 * i] = F7_1;
    m_SurfaceMeshF7s[2 * i + 1] = F7_2;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindBoundaryStrengths::newFilterInstance(bool copyFilterParameters)
{
  FindBoundaryStrengths::Pointer filter = FindBoundaryStrengths::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundaryStrengths::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundaryStrengths::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundaryStrengths::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundaryStrengths::getGroupName()
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundaryStrengths::getSubGroupName()
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundaryStrengths::getHumanLabel()
{
  return "Find Feature Boundary Strength Metrics";
}
