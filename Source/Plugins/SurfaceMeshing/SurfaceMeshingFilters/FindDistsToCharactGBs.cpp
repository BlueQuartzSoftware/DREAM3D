/* ============================================================================
 * This filter has been created by Krzysztof Glowinski (kglowinski at ymail.com).
 * It computes 'approximate distances' to the nearest characteristic GBs.
 * Besides the calculation of the distances, many parts of the code come from
 * the sources of other filters, mainly "Find GBCD".
 * Therefore, the below copyright notice applies.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

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

#include "FindDistsToCharactGBs.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"
#include "OrientationLib/LaueOps/LaueOps.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

// Include the MOC generated file for this class
#include "moc_FindDistsToCharactGBs.cpp"

const double FindDistsToCharactGBs::INF_DIST = 999.0;

/**
 * @brief The TrisProcessor class implements a threader algorithm that computes the distances for
 * triangles from certain characteristic boundaries
 */
class TrisProcessor
{
  double* m_DistToTilt;
  double* m_DistToTwist;
  double* m_DistToSymmetric;
  double* m_DistTo180Tilt;
  uint32_t* m_CrystalStructures;
  float* m_Eulers;
  int32_t* m_Phases;
  int32_t* m_FaceLabels;
  double* m_FaceNormals;
  QVector<LaueOps::Pointer> m_OrientationOps;

public:
  TrisProcessor(double* __m_DistToTilt, double* __m_DistToTwist, double* __m_DistToSymmetric, double* __m_DistTo180Tilt, uint32_t* __m_CrystalStructures, float* __m_Eulers, int32_t* __m_Phases,
                int32_t* __m_FaceLabels, double* __m_FaceNormals)
  : m_DistToTilt(__m_DistToTilt)
  , m_DistToTwist(__m_DistToTwist)
  , m_DistToSymmetric(__m_DistToSymmetric)
  , m_DistTo180Tilt(__m_DistTo180Tilt)
  , m_CrystalStructures(__m_CrystalStructures)
  , m_Eulers(__m_Eulers)
  , m_Phases(__m_Phases)
  , m_FaceLabels(__m_FaceLabels)
  , m_FaceNormals(__m_FaceNormals)
  {
    m_OrientationOps = LaueOps::getOrientationOpsQVector();
  }

  virtual ~TrisProcessor()
  {
  }

  void process(size_t start, size_t end) const
  {
    float g1ea[3] = {0.0f, 0.0f, 0.0f};
    float g2ea[3] = {0.0f, 0.0f, 0.0f};
    float g1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float g2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float g1s[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float g2s[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float sym1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float sym2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float g2sT[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float dg[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float normal_lab[3] = {0.0f, 0.0f, 0.0f};
    float normal_grain1[3] = {0.0f, 0.0f, 0.0f};

    for(size_t triIdx = start; triIdx < end; triIdx++)
    {
      int32_t feature1 = m_FaceLabels[2 * triIdx];
      int32_t feature2 = m_FaceLabels[2 * triIdx + 1];

      // For interphase boundaries, tilt, twist, symmetric, etc. GBs are not defined
      // The default value of INF_DIST will be saved
      // Currently no idea how to do it better
      if(feature1 < 1 || feature2 < 1 || m_Phases[feature1] != m_Phases[feature2])
      {
        m_DistToTilt[triIdx] = FindDistsToCharactGBs::INF_DIST;
        m_DistToTwist[triIdx] = FindDistsToCharactGBs::INF_DIST;
        m_DistToSymmetric[triIdx] = FindDistsToCharactGBs::INF_DIST;
        m_DistTo180Tilt[triIdx] = FindDistsToCharactGBs::INF_DIST;
        continue;
      }

      double minDistToTilt = FindDistsToCharactGBs::INF_DIST;
      double minDistToTwist = FindDistsToCharactGBs::INF_DIST;
      double minDistToSymmetric = FindDistsToCharactGBs::INF_DIST;
      double minDistTo180Tilt = FindDistsToCharactGBs::INF_DIST;

      normal_lab[0] = static_cast<float>(m_FaceNormals[3 * triIdx]);
      normal_lab[1] = static_cast<float>(m_FaceNormals[3 * triIdx + 1]);
      normal_lab[2] = static_cast<float>(m_FaceNormals[3 * triIdx + 2]);

      for(int whichEa = 0; whichEa < 3; whichEa++)
      {
        g1ea[whichEa] = m_Eulers[3 * feature1 + whichEa];
        g2ea[whichEa] = m_Eulers[3 * feature2 + whichEa];
      }

      FOrientArrayType om(9, 0.0f);
      FOrientTransformsType::eu2om(FOrientArrayType(g1ea, 3), om);
      om.toGMatrix(g1);
      FOrientTransformsType::eu2om(FOrientArrayType(g2ea, 3), om);
      om.toGMatrix(g2);

      int32_t cryst = m_CrystalStructures[m_Phases[feature1]];
      int32_t nsym = m_OrientationOps[cryst]->getNumSymOps();

      for(int j = 0; j < nsym; j++)
      {
        // rotate g1 by symOp
        m_OrientationOps[cryst]->getMatSymOp(j, sym1);
        MatrixMath::Multiply3x3with3x3(sym1, g1, g1s);
        // get the crystal directions along the triangle normals
        MatrixMath::Multiply3x3with3x1(g1s, normal_lab, normal_grain1);

        for(int k = 0; k < nsym; k++)
        {
          // calculate the symmetric misorienation
          m_OrientationOps[cryst]->getMatSymOp(k, sym2);
          // rotate g2 by symOp
          MatrixMath::Multiply3x3with3x3(sym2, g2, g2s);
          // transpose rotated g2
          MatrixMath::Transpose3x3(g2s, g2sT);
          // calculate delta g
          MatrixMath::Multiply3x3with3x3(g1s, g2sT, dg); // dg -- the misorientation between adjacent grains

          FOrientArrayType omAxisAngle(4, 0.0f);
          FOrientTransformsType::om2ax(FOrientArrayType(dg), omAxisAngle);

          double misorAngle = omAxisAngle[3];
          double dotProd = fabs(omAxisAngle[0] * normal_grain1[0] + omAxisAngle[1] * normal_grain1[1] + omAxisAngle[2] * normal_grain1[2]);
          double alpha;
          if(dotProd < -1.0)
          {
            alpha = SIMPLib::Constants::k_Pi;
          }
          else if(dotProd > 1.0)
          {
            alpha = 0.0;
          }
          else
          {
            alpha = acos(dotProd);
          }

          double dist2tilt = SIMPLib::Constants::k_PiOver2 - alpha;
          double piMinusMisorAngleSq = SIMPLib::Constants::k_Pi - misorAngle;
          piMinusMisorAngleSq *= piMinusMisorAngleSq;

          double dist2tiltSq = dist2tilt * dist2tilt;
          double dist2symSq = alpha * alpha + piMinusMisorAngleSq;
          double dist2180tiltSq = dist2tiltSq + piMinusMisorAngleSq;

          if(dist2tilt < minDistToTilt)
          {
            minDistToTilt = dist2tilt;
          }
          if(alpha < minDistToTwist)
          {
            minDistToTwist = alpha;
          }
          if(dist2symSq < minDistToSymmetric)
          {
            minDistToSymmetric = dist2symSq;
          }
          if(dist2180tiltSq < minDistTo180Tilt)
          {
            minDistTo180Tilt = dist2180tiltSq;
          }
        }
      }

      m_DistToTilt[triIdx] = minDistToTilt * SIMPLib::Constants::k_180OverPi;
      m_DistToTwist[triIdx] = minDistToTwist * SIMPLib::Constants::k_180OverPi;
      m_DistToSymmetric[triIdx] = sqrt(minDistToSymmetric) * SIMPLib::Constants::k_180OverPi;
      m_DistTo180Tilt[triIdx] = sqrt(minDistTo180Tilt) * SIMPLib::Constants::k_180OverPi;
    }
  }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    process(r.begin(), r.end());
  }
#endif
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindDistsToCharactGBs::FindDistsToCharactGBs()
: SurfaceMeshFilter()
, m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures)
, m_FeatureEulerAnglesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::AvgEulerAngles)
, m_FeaturePhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases)
, m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels)
, m_SurfaceMeshFaceNormalsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceNormals)
, m_DistToTiltArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, "DistanceToTilt")
, m_DistToTwistArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, "DistanceToTwist")
, m_DistToSymmetricArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, "DistanceToSymmetric")
, m_DistTo180TiltArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, "DistanceTo180tilt")
, m_CrystalStructures(nullptr)
, m_FeatureEulerAngles(nullptr)
, m_FeaturePhases(nullptr)
, m_SurfaceMeshFaceLabels(nullptr)
, m_SurfaceMeshFaceNormals(nullptr)
, m_DistToTilt(nullptr)
, m_DistToTwist(nullptr)
, m_DistToSymmetric(nullptr)
, m_DistTo180Tilt(nullptr)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindDistsToCharactGBs::~FindDistsToCharactGBs()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDistsToCharactGBs::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", SurfaceMeshFaceLabelsArrayPath, FilterParameter::RequiredArray, FindDistsToCharactGBs, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Double, 3, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Normals", SurfaceMeshFaceNormalsArrayPath, FilterParameter::RequiredArray, FindDistsToCharactGBs, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Average Euler Angles", FeatureEulerAnglesArrayPath, FilterParameter::RequiredArray, FindDistsToCharactGBs, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, FindDistsToCharactGBs, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, FindDistsToCharactGBs, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::CreatedArray));
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Distance to Nearest Tilt Boundary", DistToTiltArrayPath, FilterParameter::CreatedArray, FindDistsToCharactGBs, req));
  }
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Distance to Nearest Twist Boundary", DistToTwistArrayPath, FilterParameter::CreatedArray, FindDistsToCharactGBs, req));
  }
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Distance to Nearest Symmetric Boundary", DistToSymmetricArrayPath, FilterParameter::CreatedArray, FindDistsToCharactGBs, req));
  }
  {
    QString degSymbol = QChar(0x00B0);
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Distance to Nearest 180" + degSymbol + "-tilt  Boundary", DistTo180TiltArrayPath, FilterParameter::CreatedArray, FindDistsToCharactGBs, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDistsToCharactGBs::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructures", getCrystalStructuresArrayPath()));
  setFeatureEulerAnglesArrayPath(reader->readDataArrayPath("FeatureEulerAngles", getFeatureEulerAnglesArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhases", getFeaturePhasesArrayPath()));
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabels", getSurfaceMeshFaceLabelsArrayPath()));
  setSurfaceMeshFaceNormalsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceNormals", getSurfaceMeshFaceNormalsArrayPath()));
  setDistToTiltArrayPath(reader->readDataArrayPath("DistToTiltArrayPath", getDistToTiltArrayPath()));
  setDistToTwistArrayPath(reader->readDataArrayPath("DistToTwistArrayPath", getDistToTwistArrayPath()));
  setDistToSymmetricArrayPath(reader->readDataArrayPath("DistToSymmetricArrayPath", getDistToSymmetricArrayPath()));
  setDistTo180TiltArrayPath(reader->readDataArrayPath("DistTo180TiltArrayPath", getDistTo180TiltArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDistsToCharactGBs::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDistsToCharactGBs::dataCheck()
{
  setErrorCondition(0);

  // Crystal Structures
  QVector<size_t> cDims(1, 1);
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Euler Angels
  cDims[0] = 3;
  m_FeatureEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getFeatureEulerAnglesArrayPath(),
                                                                                                              cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureEulerAnglesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Phases
  cDims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Face Labels
  cDims[0] = 2;
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(),
                                                                                                                   cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceLabelsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Face Normals
  cDims[0] = 3;
  m_SurfaceMeshFaceNormalsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceNormalsArrayPath(),
                                                                                                                   cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceNormalsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Distances
  QVector<IDataArray::Pointer> dataArrays;
  cDims[0] = 1;
  m_DistToTiltPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, getDistToTiltArrayPath(), 0,
                                                                                                                     cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_DistToTiltPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_DistToTilt = m_DistToTiltPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(m_DistToTiltPtr.lock());
  }

  cDims[0] = 1;
  m_DistToTwistPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, getDistToTwistArrayPath(), 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_DistToTwistPtr.lock().get())    /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_DistToTwist = m_DistToTwistPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(m_DistToTwistPtr.lock());
  }

  cDims[0] = 1;
  m_DistToSymmetricPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, getDistToSymmetricArrayPath(), 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_DistToSymmetricPtr.lock().get())    /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_DistToSymmetric = m_DistToSymmetricPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(m_DistToSymmetricPtr.lock());
  }

  cDims[0] = 1;
  m_DistTo180TiltPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, getDistTo180TiltArrayPath(), 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_DistTo180TiltPtr.lock().get())    /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_DistTo180Tilt = m_DistTo180TiltPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(m_DistTo180TiltPtr.lock());
  }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrays);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDistsToCharactGBs::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDistsToCharactGBs::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceNormalsArrayPath().getDataContainerName());
  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();
  int64_t numMeshTris = triangleGeom->getNumberOfTris();

  // We want to work with the raw pointers for speed so get those pointers.
  uint32_t* m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  float* m_Eulers = m_FeatureEulerAnglesPtr.lock()->getPointer(0);
  int32_t* m_Phases = m_FeaturePhasesPtr.lock()->getPointer(0);
  int32_t* m_FaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  double* m_FaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0);

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  int64_t trisChunkSize = 50000;
  if(numMeshTris < trisChunkSize)
  {
    trisChunkSize = numMeshTris;
  }

  for(int64_t i = 0; i < numMeshTris; i = i + trisChunkSize)
  {
    if(getCancel() == true)
    {
      return;
    }
    QString ss = QObject::tr("--> %1% completed").arg(int(100.0 * float(i) / float(numMeshTris)));
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    if(i + trisChunkSize >= numMeshTris)
    {
      trisChunkSize = numMeshTris - i;
    }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if(doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(i, i + trisChunkSize),
                        TrisProcessor(m_DistToTilt, m_DistToTwist, m_DistToSymmetric, m_DistTo180Tilt, m_CrystalStructures, m_Eulers, m_Phases, m_FaceLabels, m_FaceNormals), tbb::auto_partitioner());
    }
    else
#endif
    {
      TrisProcessor serial(m_DistToTilt, m_DistToTwist, m_DistToSymmetric, m_DistTo180Tilt, m_CrystalStructures, m_Eulers, m_Phases, m_FaceLabels, m_FaceNormals);
      serial.process(i, i + trisChunkSize);
    }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindDistsToCharactGBs::newFilterInstance(bool copyFilterParameters)
{
  FindDistsToCharactGBs::Pointer filter = FindDistsToCharactGBs::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDistsToCharactGBs::getCompiledLibraryName()
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDistsToCharactGBs::getBrandingString()
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDistsToCharactGBs::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDistsToCharactGBs::getGroupName()
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDistsToCharactGBs::getSubGroupName()
{
  return SIMPL::FilterSubGroups::CrystallographicFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDistsToCharactGBs::getHumanLabel()
{
  return "Find Distances to Characteristic Grain Boundaries";
}
