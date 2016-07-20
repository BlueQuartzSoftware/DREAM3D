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

#include "FindTwinBoundarySchmidFactors.h"

#include <fstream>

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

/**
 * @brief The CalculateTwinBoundarySchmidFactorsImpl class implements a threaded algorithm that computes the
 * Schmid factors across twin boundaries.
 */
class CalculateTwinBoundarySchmidFactorsImpl
{
    int32_t* m_Labels;
    double* m_Normals;
    float* m_Quats;
    bool* m_TwinBoundary;
    float* m_TwinBoundarySchmidFactors;
    float* m_LoadDir;
    QVector<SpaceGroupOps::Pointer> m_OrientationOps;

  public:
    CalculateTwinBoundarySchmidFactorsImpl(float* LoadingDir, int32_t* Labels, double* Normals, float* Quats, bool* TwinBoundary, float* TwinBoundarySchmidFactors) :
      m_Labels(Labels),
      m_Normals(Normals),
      m_Quats(Quats),
      m_TwinBoundary(TwinBoundary),
      m_TwinBoundarySchmidFactors(TwinBoundarySchmidFactors),
      m_LoadDir(LoadingDir)
    {
      m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();
    }
    virtual ~CalculateTwinBoundarySchmidFactorsImpl() {}

    void generate(size_t start, size_t end) const
    {
      int32_t feature1 = 0, feature2 = 0, feature = 0;
      float normal[3] = { 0.0f, 0.0f, 0.0f };
      float g1[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
      float schmid1 = 0.0f, schmid2 = 0.0f, schmid3 = 0.0f;
      QuatF q1 = QuaternionMathF::New();
      QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

      float n[3] = { 0.0f, 0.0f, 0.0f };
      float b[3] = { 0.0f, 0.0f, 0.0f };
      float crystalLoading[3] = { 0.0f, 0.0f, 0.0f };
      float cosPhi = 0.0f, cosLambda = 0.0f;

      for (size_t i = start; i < end; i++)
      {
        if (m_TwinBoundary[i] == true)
        {
          feature1 = m_Labels[2 * i];
          feature2 = m_Labels[2 * i + 1];
          normal[0] = m_Normals[3 * i];
          normal[1] = m_Normals[3 * i + 1];
          normal[2] = m_Normals[3 * i + 2];
          schmid1 = 0.0f, schmid2 = 0.0f, schmid3 = 0.0f;
          if (feature1 > feature2) { feature = feature1; }
          else { feature = feature2; }

          QuaternionMathF::Copy(quats[feature], q1);
          // calculate crystal direction parallel to normal
          FOrientArrayType om(9);
          FOrientTransformsType::qu2om(FOrientArrayType(q1), om);
          om.toGMatrix(g1);
          MatrixMath::Multiply3x3with3x1(g1, normal, n);
          // calculate crystal direction parallel to loading direction
          MatrixMath::Multiply3x3with3x1(g1, m_LoadDir, crystalLoading);

          if (n[2] < 0.0f) { n[0] = -n[0], n[1] = -n[1], n[2] = -n[2]; }
          if (n[0] > 0.0f && n[1] > 0.0f)
          {
            n[0] = 1.0f, n[1] = 1.0f, n[2] = 1.0f;
            cosPhi = fabsf(GeometryMath::CosThetaBetweenVectors(crystalLoading, n));
            b[0] = 1.0f, b[1] = -1.0f, b[2] = 0.0f;
            cosLambda = fabsf(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid1 = cosPhi * cosLambda;
            b[0] = -1.0f, b[1] = 0.0f, b[2] = 1.0f;
            cosLambda = fabsf(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid2 = cosPhi * cosLambda;
            b[0] = 0.0f, b[1] = -1.0f, b[2] = 1.0f;
            cosLambda = fabsf(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid3 = cosPhi * cosLambda;
            m_TwinBoundarySchmidFactors[3 * i] = schmid1;
            m_TwinBoundarySchmidFactors[3 * i + 1] = schmid2;
            m_TwinBoundarySchmidFactors[3 * i + 2] = schmid3;
          }
          else if (n[0] > 0.0f && n[1] < 0.0f)
          {
            n[0] = 1.0f, n[1] = -1.0f, n[2] = 1.0f;
            cosPhi = fabsf(GeometryMath::CosThetaBetweenVectors(crystalLoading, n));
            b[0] = 1.0f, b[1] = 1.0f, b[2] = 0.0f;
            cosLambda = fabsf(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid1 = cosPhi * cosLambda;
            b[0] = 0.0f, b[1] = 1.0f, b[2] = 1.0f;
            cosLambda = fabsf(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid2 = cosPhi * cosLambda;
            b[0] = -1.0f, b[1] = 0.0f, b[2] = 1.0f;
            cosLambda = fabsf(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid3 = cosPhi * cosLambda;
            m_TwinBoundarySchmidFactors[3 * i] = schmid1;
            m_TwinBoundarySchmidFactors[3 * i + 1] = schmid2;
            m_TwinBoundarySchmidFactors[3 * i + 2] = schmid3;
          }
          else if (n[0] < 0.0f && n[1] > 0.0f)
          {
            n[0] = -1.0f, n[1] = 1.0f, n[2] = 1.0f;
            cosPhi = fabsf(GeometryMath::CosThetaBetweenVectors(crystalLoading, n));
            b[0] = 1.0f, b[1] = 1.0f, b[2] = 0.0f;
            cosLambda = fabsf(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid1 = cosPhi * cosLambda;
            b[0] = 1.0f, b[1] = 0.0f, b[2] = 1.0f;
            cosLambda = fabsf(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid2 = cosPhi * cosLambda;
            b[0] = 0.0f, b[1] = -1.0f, b[2] = 1.0f;
            cosLambda = fabsf(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid3 = cosPhi * cosLambda;
            m_TwinBoundarySchmidFactors[3 * i] = schmid1;
            m_TwinBoundarySchmidFactors[3 * i + 1] = schmid2;
            m_TwinBoundarySchmidFactors[3 * i + 2] = schmid3;
          }
          else if (n[0] < 0.0f && n[1] < 0.0f)
          {
            n[0] = -1.0f, n[1] = -1.0f, n[2] = 1.0f;
            cosPhi = fabsf(GeometryMath::CosThetaBetweenVectors(crystalLoading, n));
            b[0] = 1.0f, b[1] = 0.0f, b[2] = 1.0f;
            cosLambda = fabsf(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid1 = cosPhi * cosLambda;
            b[0] = 0.0f, b[1] = 1.0f, b[2] = 1.0f;
            cosLambda = fabsf(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid2 = cosPhi * cosLambda;
            b[0] = 1.0f, b[1] = -1.0f, b[2] = 0.0f;
            cosLambda = fabsf(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid3 = cosPhi * cosLambda;
            m_TwinBoundarySchmidFactors[3 * i] = schmid1;
            m_TwinBoundarySchmidFactors[3 * i + 1] = schmid2;
            m_TwinBoundarySchmidFactors[3 * i + 2] = schmid3;
          }
        }
        else
        {
          m_TwinBoundarySchmidFactors[3 * i] = 0.0f;
          m_TwinBoundarySchmidFactors[3 * i + 1] = 0.0f;
          m_TwinBoundarySchmidFactors[3 * i + 2] = 0.0f;
        }
      }
    }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      generate(r.begin(), r.end());
    }
#endif
};

// Include the MOC generated file for this class
#include "moc_FindTwinBoundarySchmidFactors.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTwinBoundarySchmidFactors::FindTwinBoundarySchmidFactors() :
  AbstractFilter(),
  m_WriteFile(false),
  m_TwinBoundarySchmidFactorsFile(""),
  m_AvgQuatsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::AvgQuats),
  m_FeaturePhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases),
  m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures),
  m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceNormalsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshTwinBoundaryArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshTwinBoundary),
  m_SurfaceMeshTwinBoundarySchmidFactorsArrayName(SIMPL::FaceData::SurfaceMeshTwinBoundarySchmidFactors),
  m_AvgQuats(NULL),
  m_FeaturePhases(NULL),
  m_CrystalStructures(NULL),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFaceNormals(NULL),
  m_SurfaceMeshTwinBoundary(NULL),
  m_SurfaceMeshTwinBoundarySchmidFactors(NULL)
{
  m_LoadingDir.x = 1.0f;
  m_LoadingDir.y = 1.0f;
  m_LoadingDir.z = 1.0f;

  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTwinBoundarySchmidFactors::~FindTwinBoundarySchmidFactors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundarySchmidFactors::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FloatVec3FilterParameter::New("Loading Direction", "LoadingDir", getLoadingDir(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FindTwinBoundarySchmidFactors, this, LoadingDir), SIMPL_BIND_GETTER(FindTwinBoundarySchmidFactors, this, LoadingDir)));

  QStringList linkedProps("TwinBoundarySchmidFactorsFile");
  parameters.push_back(LinkedBooleanFilterParameter::New("Write Twin Boundary Info File", "WriteFile", getWriteFile(), linkedProps, FilterParameter::Parameter, SIMPL_BIND_SETTER(FindTwinBoundarySchmidFactors, this, WriteFile), SIMPL_BIND_GETTER(FindTwinBoundarySchmidFactors, this, WriteFile)));
  parameters.push_back(OutputFileFilterParameter::New("Twin Boundary Info File", "TwinBoundarySchmidFactorsFile", getTwinBoundarySchmidFactorsFile(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FindTwinBoundarySchmidFactors, this, TwinBoundarySchmidFactorsFile), SIMPL_BIND_GETTER(FindTwinBoundarySchmidFactors, this, TwinBoundarySchmidFactorsFile)));
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 4, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Average Quaternions", "AvgQuatsArrayPath", getAvgQuatsArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(FindTwinBoundarySchmidFactors, this, AvgQuatsArrayPath), SIMPL_BIND_GETTER(FindTwinBoundarySchmidFactors, this, AvgQuatsArrayPath)));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "FeaturePhasesArrayPath", getFeaturePhasesArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(FindTwinBoundarySchmidFactors, this, FeaturePhasesArrayPath), SIMPL_BIND_GETTER(FindTwinBoundarySchmidFactors, this, FeaturePhasesArrayPath)));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, SIMPL::AttributeMatrixType::CellEnsemble, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(FindTwinBoundarySchmidFactors, this, CrystalStructuresArrayPath), SIMPL_BIND_GETTER(FindTwinBoundarySchmidFactors, this, CrystalStructuresArrayPath)));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, SIMPL::AttributeMatrixType::Face, SIMPL::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Face Labels", "SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(FindTwinBoundarySchmidFactors, this, SurfaceMeshFaceLabelsArrayPath), SIMPL_BIND_GETTER(FindTwinBoundarySchmidFactors, this, SurfaceMeshFaceLabelsArrayPath)));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Double, 3, SIMPL::AttributeMatrixType::Face, SIMPL::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Face Normals", "SurfaceMeshFaceNormalsArrayPath", getSurfaceMeshFaceNormalsArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(FindTwinBoundarySchmidFactors, this, SurfaceMeshFaceNormalsArrayPath), SIMPL_BIND_GETTER(FindTwinBoundarySchmidFactors, this, SurfaceMeshFaceNormalsArrayPath)));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Bool, 1, SIMPL::AttributeMatrixType::Face, SIMPL::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Twin Boundary", "SurfaceMeshTwinBoundaryArrayPath", getSurfaceMeshTwinBoundaryArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(FindTwinBoundarySchmidFactors, this, SurfaceMeshTwinBoundaryArrayPath), SIMPL_BIND_GETTER(FindTwinBoundarySchmidFactors, this, SurfaceMeshTwinBoundaryArrayPath)));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Twin Boundary Schmid Factors", "SurfaceMeshTwinBoundarySchmidFactorsArrayName", getSurfaceMeshTwinBoundarySchmidFactorsArrayName(), FilterParameter::CreatedArray, SIMPL_BIND_SETTER(FindTwinBoundarySchmidFactors, this, SurfaceMeshTwinBoundarySchmidFactorsArrayName), SIMPL_BIND_GETTER(FindTwinBoundarySchmidFactors, this, SurfaceMeshTwinBoundarySchmidFactorsArrayName)));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundarySchmidFactors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setWriteFile(reader->readValue("WriteFile", getWriteFile()));
  setSurfaceMeshTwinBoundarySchmidFactorsArrayName(reader->readString("SurfaceMeshTwinBoundarySchmidFactorsArrayName", getSurfaceMeshTwinBoundarySchmidFactorsArrayName() ) );
  setSurfaceMeshTwinBoundaryArrayPath(reader->readDataArrayPath("SurfaceMeshTwinBoundaryArrayPath", getSurfaceMeshTwinBoundaryArrayPath() ) );
  setSurfaceMeshFaceNormalsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceNormalsArrayPath", getSurfaceMeshFaceNormalsArrayPath() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath() ) );
  setLoadingDir( reader->readFloatVec3("LoadingDir", getLoadingDir()));
  setTwinBoundarySchmidFactorsFile( reader->readString("TwinBoundarySchmidFactorsFile", getTwinBoundarySchmidFactorsFile()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundarySchmidFactors::dataCheckVoxel()
{
  setErrorCondition(0);

  QVector<DataArrayPath> dataArrayPaths;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getAvgQuatsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 4);
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getAvgQuatsArrayPath()); }

  cDims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getFeaturePhasesArrayPath()); }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundarySchmidFactors::dataCheckSurfaceMesh()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  QVector<DataArrayPath> dataArrayPaths;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getSurfaceMeshFaceLabelsArrayPath()); }

  cDims[0] = 3;
  m_SurfaceMeshFaceNormalsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceNormalsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceNormalsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getSurfaceMeshFaceNormalsArrayPath()); }

  tempPath.update(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName(), getSurfaceMeshFaceLabelsArrayPath().getAttributeMatrixName(), getSurfaceMeshTwinBoundarySchmidFactorsArrayName() );
  m_SurfaceMeshTwinBoundarySchmidFactorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshTwinBoundarySchmidFactorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshTwinBoundarySchmidFactors = m_SurfaceMeshTwinBoundarySchmidFactorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  m_SurfaceMeshTwinBoundaryPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getSurfaceMeshTwinBoundaryArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshTwinBoundaryPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshTwinBoundary = m_SurfaceMeshTwinBoundaryPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getSurfaceMeshTwinBoundaryArrayPath()); }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundarySchmidFactors::preflight()
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
void FindTwinBoundarySchmidFactors::execute()
{
  setErrorCondition(0);
  dataCheckVoxel();
  if(getErrorCondition() < 0) { return; }
  dataCheckSurfaceMesh();
  if(getErrorCondition() < 0) { return; }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  size_t numTriangles = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();

  float LoadingDir[3] = { 0.0f, 0.0f, 0.0f };
  LoadingDir[0] = m_LoadingDir.x;
  LoadingDir[1] = m_LoadingDir.y;
  LoadingDir[2] = m_LoadingDir.z;

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numTriangles),
                      CalculateTwinBoundarySchmidFactorsImpl(LoadingDir, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_AvgQuats, m_SurfaceMeshTwinBoundary, m_SurfaceMeshTwinBoundarySchmidFactors), tbb::auto_partitioner());

  }
  else
#endif
  {
    CalculateTwinBoundarySchmidFactorsImpl serial(LoadingDir, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_AvgQuats, m_SurfaceMeshTwinBoundary, m_SurfaceMeshTwinBoundarySchmidFactors);
    serial.generate(0, numTriangles);
  }


  if (m_WriteFile == true)
  {
    std::ofstream outFile;
    outFile.open(m_TwinBoundarySchmidFactorsFile.toLatin1().data(), std::ios_base::binary);

    outFile << "Feature1	Feature2	IsTwin	Plane	Schmid1	Schmid2	Schmid3" << "\n";
    for (size_t i = 0; i < numTriangles; i++)
    {
      outFile << m_SurfaceMeshFaceLabels[2 * i] << "  " << m_SurfaceMeshFaceLabels[2 * i + 1] << "  " << m_SurfaceMeshTwinBoundary[i] << "  " << m_SurfaceMeshTwinBoundarySchmidFactors[3 * i] << "  " << m_SurfaceMeshTwinBoundarySchmidFactors[3 * i + 1] << "  " << m_SurfaceMeshTwinBoundarySchmidFactors[3 * i + 2] << "\n";
    }
    outFile.close();
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindTwinBoundarySchmidFactors::newFilterInstance(bool copyFilterParameters)
{
  FindTwinBoundarySchmidFactors::Pointer filter = FindTwinBoundarySchmidFactors::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTwinBoundarySchmidFactors::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTwinBoundarySchmidFactors::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTwinBoundarySchmidFactors::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTwinBoundarySchmidFactors::getGroupName()
{ return SIMPL::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTwinBoundarySchmidFactors::getSubGroupName()
{ return SIMPL::FilterSubGroups::CrystallographicFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTwinBoundarySchmidFactors::getHumanLabel()
{ return "Find Twin Boundary Schmid Factors"; }
