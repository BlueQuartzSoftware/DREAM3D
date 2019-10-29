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

#include "FindTwinBoundaries.h"

#include <array>
#include <cmath>


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
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "OrientationLib/Core/Orientation.hpp"
#include "OrientationLib/Core/OrientationTransformation.hpp"
#include "OrientationLib/Core/Quaternion.hpp"
#include "OrientationLib/LaueOps/LaueOps.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

/**
 * @brief The CalculateTwinBoundaryImpl class implements a threaded algorithm that determines whether a boundary is twin related and calculates
 * the reseptive incoherence.  The calculations are performed on a surface mesh.
 */
class CalculateTwinBoundaryImpl
{
  float m_AxisTol;
  float m_AngTol;
  int32_t* m_Labels = nullptr;
  double* m_Normals = nullptr;
  int32_t* m_Phases = nullptr;
  float* m_Quats = nullptr;
  bool* m_TwinBoundary = nullptr;
  float* m_TwinBoundaryIncoherence = nullptr;
  uint32_t* m_CrystalStructures = nullptr;
  bool m_FindCoherence;
  QVector<LaueOps::Pointer> m_OrientationOps;

public:
  CalculateTwinBoundaryImpl(float angtol, float axistol, int32_t* Labels, double* Normals, float* Quats, int32_t* Phases, unsigned int* CrystalStructures, bool* TwinBoundary,
                            float* TwinBoundaryIncoherence, bool FindCoherence)
  : m_AxisTol(axistol)
  , m_AngTol(angtol)
  , m_Labels(Labels)
  , m_Normals(Normals)
  , m_Phases(Phases)
  , m_Quats(Quats)
  , m_TwinBoundary(TwinBoundary)
  , m_TwinBoundaryIncoherence(TwinBoundaryIncoherence)
  , m_CrystalStructures(CrystalStructures)
  , m_FindCoherence(FindCoherence)
  {
    m_OrientationOps = LaueOps::getOrientationOpsQVector();
  }

  virtual ~CalculateTwinBoundaryImpl() = default;

  void generate(size_t start, size_t end) const
  {
    int32_t feature1 = 0, feature2 = 0;
    double normal[3] = {0.0, 0.0, 0.0};
    double g1[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    double w = 0.0;
    uint32_t phase1 = 0, phase2 = 0;

    double axisdiff111 = 0.0, angdiff60 = 0.0;
    double n[3] = {0.0, 0.0, 0.0};
    double incoherence = 0.0;
    double n1 = 0.0, n2 = 0.0, n3 = 0.0;

    QuatType misq;
    QuatType sym_q;
    QuatType s1_misq;
    QuatType s2_misq;

    // QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

    std::array<double, 3> xstl_norm = {0.0, 0.0, 0.0};
    std::array<double, 3> s_xstl_norm = {0.0, 0.0, 0.0};

    for(size_t i = start; i < end; i++)
    {
      feature1 = m_Labels[2 * i];
      feature2 = m_Labels[2 * i + 1];
      if(m_FindCoherence)
      {
        normal[0] = m_Normals[3 * i];
        normal[1] = m_Normals[3 * i + 1];
        normal[2] = m_Normals[3 * i + 2];
      }
      if(feature1 > 0 && feature2 > 0 && m_Phases[feature1] == m_Phases[feature2])
      {
        w = std::numeric_limits<float>::max();
        float* quatPtr = m_Quats + feature1 * 4;
        QuatType q1(quatPtr[0], quatPtr[1], quatPtr[2], quatPtr[3]);

        quatPtr = m_Quats + feature2 * 4;
        QuatType q2(quatPtr[0], quatPtr[1], quatPtr[2], quatPtr[3]);

        phase1 = m_CrystalStructures[m_Phases[feature1]];
        phase2 = m_CrystalStructures[m_Phases[feature2]];
        if(phase1 == phase2)
        {
          int32_t nsym = m_OrientationOps[phase1]->getNumSymOps();
          q2 = q2.conjugate();
          misq = q1 * q2;
          OrientationTransformation::qu2om<QuatType, OrientationD>(q1).toGMatrix(g1);

          if(m_FindCoherence)
          {
            MatrixMath::Multiply3x3with3x1(g1, normal, xstl_norm.data());
          }

          for(int32_t j = 0; j < nsym; j++)
          {
            sym_q = m_OrientationOps[phase1]->getQuatSymOp(j);
            // calculate crystal direction parallel to normal
            s1_misq = misq * sym_q;

            if(m_FindCoherence)
            {
              s_xstl_norm = sym_q.multiplyByVector(xstl_norm.data());
            }

            for(int32_t k = 0; k < nsym; k++)
            {
              // calculate the symmetric misorienation
              sym_q = m_OrientationOps[phase1]->getQuatSymOp(k);
              sym_q = sym_q.conjugate();
              s2_misq = sym_q * s1_misq;

              OrientationTransformation::qu2ax<QuatType, OrientationD>(s2_misq).toAxisAngle(n1, n2, n3, w);

              w = w * 180.0f / SIMPLib::Constants::k_Pi;
              axisdiff111 = acos(std::fabs(n1) * 0.57735f + std::fabs(n2) * 0.57735f + std::fabs(n3) * 0.57735f);
              angdiff60 = std::fabs(w - 60.0f);
              if(axisdiff111 < m_AxisTol && angdiff60 < m_AngTol)
              {
                n[0] = n1;
                n[1] = n2;
                n[2] = n3;
                m_TwinBoundary[i] = true;
                if(m_FindCoherence)
                {
                  incoherence = 180.0 * std::acos(GeometryMath::CosThetaBetweenVectors(n, s_xstl_norm.data())) / SIMPLib::Constants::k_Pi;
                  if(incoherence > 90.0)
                  {
                    incoherence = 180.0 - incoherence;
                  }
                  if(incoherence < m_TwinBoundaryIncoherence[i])
                  {
                    m_TwinBoundaryIncoherence[i] = incoherence;
                  }
                }
              }
            }
          }
        }
      }
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    generate(r.begin(), r.end());
  }
#endif
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTwinBoundaries::FindTwinBoundaries()
: m_AxisTolerance(0.0f)
, m_AngleTolerance(0.0f)
, m_FindCoherence(true)
, m_AvgQuatsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::AvgQuats)
, m_FeaturePhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases)
, m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures)
, m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels)
, m_SurfaceMeshFaceNormalsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceNormals)
, m_SurfaceMeshTwinBoundaryArrayName(SIMPL::FaceData::SurfaceMeshTwinBoundary)
, m_SurfaceMeshTwinBoundaryIncoherenceArrayName(SIMPL::FaceData::SurfaceMeshTwinBoundaryIncoherence)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTwinBoundaries::~FindTwinBoundaries() = default;
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaries::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Axis Tolerance (Degrees)", AxisTolerance, FilterParameter::Parameter, FindTwinBoundaries));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Angle Tolerance (Degrees)", AngleTolerance, FilterParameter::Parameter, FindTwinBoundaries));
  QStringList linkedProps;
  linkedProps << "SurfaceMeshFaceNormalsArrayPath"
              << "SurfaceMeshTwinBoundaryIncoherenceArrayName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Compute Coherence", FindCoherence, FilterParameter::Parameter, FindTwinBoundaries, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Average Quaternions", AvgQuatsArrayPath, FilterParameter::RequiredArray, FindTwinBoundaries, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, FindTwinBoundaries, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, FindTwinBoundaries, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", SurfaceMeshFaceLabelsArrayPath, FilterParameter::RequiredArray, FindTwinBoundaries, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Double, 3, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Normals", SurfaceMeshFaceNormalsArrayPath, FilterParameter::RequiredArray, FindTwinBoundaries, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Twin Boundary", SurfaceMeshTwinBoundaryArrayName, SurfaceMeshFaceLabelsArrayPath, SurfaceMeshFaceLabelsArrayPath, FilterParameter::CreatedArray, FindTwinBoundaries));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Twin Boundary Incoherence", SurfaceMeshTwinBoundaryIncoherenceArrayName, SurfaceMeshFaceLabelsArrayPath, SurfaceMeshFaceLabelsArrayPath, FilterParameter::CreatedArray, FindTwinBoundaries));
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindTwinBoundaries::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshTwinBoundaryIncoherenceArrayName(reader->readString("SurfaceMeshTwinBoundaryIncoherenceArrayName", getSurfaceMeshTwinBoundaryIncoherenceArrayName()));
  setSurfaceMeshTwinBoundaryArrayName(reader->readString("SurfaceMeshTwinBoundaryArrayName", getSurfaceMeshTwinBoundaryArrayName()));
  setSurfaceMeshFaceNormalsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceNormalsArrayPath", getSurfaceMeshFaceNormalsArrayPath()));
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath()));
  setAxisTolerance(reader->readValue("AxisTolerance", getAxisTolerance()));
  setAngleTolerance(reader->readValue("AngleTolerance", getAngleTolerance()));
  setFindCoherence(reader->readValue("FindCoherence", getFindCoherence()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaries::dataCheckVoxel()
{
  clearErrorCode();
  clearWarningCode();

  QVector<DataArrayPath> dataArrayPaths;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getAvgQuatsArrayPath().getDataContainerName());

  std::vector<size_t> cDims(1, 4);
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(),
                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AvgQuatsPtr.lock())                                                                       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getAvgQuatsArrayPath());
  }

  cDims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getFeaturePhasesArrayPath());
  }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaries::dataCheckSurfaceMesh()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  QVector<DataArrayPath> dataArrayPaths;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  std::vector<size_t> cDims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(),
                                                                                                                   cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceLabelsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getSurfaceMeshFaceLabelsArrayPath());
  }

  if(getFindCoherence())
  {
    cDims[0] = 3;
    m_SurfaceMeshFaceNormalsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceNormalsArrayPath(),
                                                                                                                     cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_SurfaceMeshFaceNormalsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCode() >= 0)
    {
      dataArrayPaths.push_back(getSurfaceMeshFaceNormalsArrayPath());
    }
  }

  cDims[0] = 1;
  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), getSurfaceMeshFaceLabelsArrayPath().getAttributeMatrixName(), getSurfaceMeshTwinBoundaryArrayName());
  m_SurfaceMeshTwinBoundaryPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(
      this, tempPath, false, cDims);                       /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshTwinBoundaryPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshTwinBoundary = m_SurfaceMeshTwinBoundaryPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(getFindCoherence())
  {
    tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), getSurfaceMeshFaceLabelsArrayPath().getAttributeMatrixName(), getSurfaceMeshTwinBoundaryIncoherenceArrayName());
    m_SurfaceMeshTwinBoundaryIncoherencePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
        this, tempPath, 180.0, cDims);                                  /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_SurfaceMeshTwinBoundaryIncoherencePtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_SurfaceMeshTwinBoundaryIncoherence = m_SurfaceMeshTwinBoundaryIncoherencePtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaries::preflight()
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
void FindTwinBoundaries::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheckVoxel();
  if(getErrorCode() < 0)
  {
    return;
  }
  dataCheckSurfaceMesh();
  if(getErrorCode() < 0)
  {
    return;
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  size_t numTriangles = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();

  float angtol = m_AngleTolerance;
  float axistol = static_cast<float>(m_AxisTolerance * M_PI / 180.0f);

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numTriangles), CalculateTwinBoundaryImpl(angtol, axistol, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_AvgQuats, m_FeaturePhases,
                                                                                             m_CrystalStructures, m_SurfaceMeshTwinBoundary, m_SurfaceMeshTwinBoundaryIncoherence, m_FindCoherence),
                      tbb::auto_partitioner());
  }
  else
#endif
  {
    CalculateTwinBoundaryImpl serial(angtol, axistol, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_AvgQuats, m_FeaturePhases, m_CrystalStructures, m_SurfaceMeshTwinBoundary,
                                     m_SurfaceMeshTwinBoundaryIncoherence, m_FindCoherence);
    serial.generate(0, numTriangles);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindTwinBoundaries::newFilterInstance(bool copyFilterParameters) const
{
  FindTwinBoundaries::Pointer filter = FindTwinBoundaries::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTwinBoundaries::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTwinBoundaries::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTwinBoundaries::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTwinBoundaries::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindTwinBoundaries::getUuid() const
{
  return QUuid("{a10124f3-05d0-5f49-93a0-e93926f5b48b}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTwinBoundaries::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTwinBoundaries::getHumanLabel() const
{
  return "Find Twin Boundaries";
}

// -----------------------------------------------------------------------------
FindTwinBoundaries::Pointer FindTwinBoundaries::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindTwinBoundaries> FindTwinBoundaries::New()
{
  struct make_shared_enabler : public FindTwinBoundaries
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindTwinBoundaries::getNameOfClass() const
{
  return QString("FindTwinBoundaries");
}

// -----------------------------------------------------------------------------
QString FindTwinBoundaries::ClassName()
{
  return QString("FindTwinBoundaries");
}

// -----------------------------------------------------------------------------
void FindTwinBoundaries::setAxisTolerance(float value)
{
  m_AxisTolerance = value;
}

// -----------------------------------------------------------------------------
float FindTwinBoundaries::getAxisTolerance() const
{
  return m_AxisTolerance;
}

// -----------------------------------------------------------------------------
void FindTwinBoundaries::setAngleTolerance(float value)
{
  m_AngleTolerance = value;
}

// -----------------------------------------------------------------------------
float FindTwinBoundaries::getAngleTolerance() const
{
  return m_AngleTolerance;
}

// -----------------------------------------------------------------------------
void FindTwinBoundaries::setFindCoherence(bool value)
{
  m_FindCoherence = value;
}

// -----------------------------------------------------------------------------
bool FindTwinBoundaries::getFindCoherence() const
{
  return m_FindCoherence;
}

// -----------------------------------------------------------------------------
void FindTwinBoundaries::setAvgQuatsArrayPath(const DataArrayPath& value)
{
  m_AvgQuatsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindTwinBoundaries::getAvgQuatsArrayPath() const
{
  return m_AvgQuatsArrayPath;
}

// -----------------------------------------------------------------------------
void FindTwinBoundaries::setFeaturePhasesArrayPath(const DataArrayPath& value)
{
  m_FeaturePhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindTwinBoundaries::getFeaturePhasesArrayPath() const
{
  return m_FeaturePhasesArrayPath;
}

// -----------------------------------------------------------------------------
void FindTwinBoundaries::setCrystalStructuresArrayPath(const DataArrayPath& value)
{
  m_CrystalStructuresArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindTwinBoundaries::getCrystalStructuresArrayPath() const
{
  return m_CrystalStructuresArrayPath;
}

// -----------------------------------------------------------------------------
void FindTwinBoundaries::setSurfaceMeshFaceLabelsArrayPath(const DataArrayPath& value)
{
  m_SurfaceMeshFaceLabelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindTwinBoundaries::getSurfaceMeshFaceLabelsArrayPath() const
{
  return m_SurfaceMeshFaceLabelsArrayPath;
}

// -----------------------------------------------------------------------------
void FindTwinBoundaries::setSurfaceMeshFaceNormalsArrayPath(const DataArrayPath& value)
{
  m_SurfaceMeshFaceNormalsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindTwinBoundaries::getSurfaceMeshFaceNormalsArrayPath() const
{
  return m_SurfaceMeshFaceNormalsArrayPath;
}

// -----------------------------------------------------------------------------
void FindTwinBoundaries::setSurfaceMeshTwinBoundaryArrayName(const QString& value)
{
  m_SurfaceMeshTwinBoundaryArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindTwinBoundaries::getSurfaceMeshTwinBoundaryArrayName() const
{
  return m_SurfaceMeshTwinBoundaryArrayName;
}

// -----------------------------------------------------------------------------
void FindTwinBoundaries::setSurfaceMeshTwinBoundaryIncoherenceArrayName(const QString& value)
{
  m_SurfaceMeshTwinBoundaryIncoherenceArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindTwinBoundaries::getSurfaceMeshTwinBoundaryIncoherenceArrayName() const
{
  return m_SurfaceMeshTwinBoundaryIncoherenceArrayName;
}
