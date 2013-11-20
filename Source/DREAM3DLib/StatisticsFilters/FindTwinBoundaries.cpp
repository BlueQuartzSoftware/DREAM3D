/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FindTwinBoundaries.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/QuaternionMath.hpp"
#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindSurfaceFeatures.h"
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"




class CalculateTwinBoundaryImpl
{
    float m_AxisTol;
    float m_AngTol;
    int32_t* m_Labels;
    double* m_Normals;
    int32_t* m_Phases;
    float* m_Quats;
    bool* m_TwinBoundary;
    float* m_TwinBoundaryIncoherence;
    unsigned int* m_CrystalStructures;
    QVector<OrientationOps::Pointer> m_OrientationOps;

  public:
    CalculateTwinBoundaryImpl(float angtol, float axistol, int32_t* Labels, double* Normals, float* Quats, int32_t* Phases, unsigned int* CrystalStructures, bool* TwinBoundary, float* TwinBoundaryIncoherence) :
      m_AxisTol(axistol),
      m_AngTol(angtol),
      m_Labels(Labels),
      m_Normals(Normals),
      m_Phases(Phases),
      m_Quats(Quats),
      m_TwinBoundary(TwinBoundary),
      m_TwinBoundaryIncoherence(TwinBoundaryIncoherence),
      m_CrystalStructures(CrystalStructures)
    {
      m_OrientationOps = OrientationOps::getOrientationOpsVector();
    }

    virtual ~CalculateTwinBoundaryImpl() {}

    void generate(size_t start, size_t end) const
    {
      int feature1, feature2;
      float normal[3];
      float g1[3][3];
      float w;
      unsigned int phase1, phase2;
      QuatF q1;
      QuatF q2;
      float axisdiff111, angdiff60;
      float n[3];
      float incoherence;
      float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;

      QuatF misq;
      QuatF sym_q;
      QuatF s1_misq;
      QuatF s2_misq;
      QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

      float xstl_norm[3], s_xstl_norm[3];

      for (size_t i = start; i < end; i++)
      {
        feature1 = m_Labels[2 * i];
        feature2 = m_Labels[2 * i + 1];
        normal[0] = m_Normals[3 * i];
        normal[1] = m_Normals[3 * i + 1];
        normal[2] = m_Normals[3 * i + 2];
        if(feature1 > 0 && feature2 > 0 && m_Phases[feature1] == m_Phases[feature2])
        {
          w = 10000.0;

          QuaternionMathF::Copy(quats[feature1], q1);
          QuaternionMathF::Copy(quats[feature2], q2);

          phase1 = m_CrystalStructures[m_Phases[feature1]];
          phase2 = m_CrystalStructures[m_Phases[feature2]];
          if (phase1 == phase2)
          {
            int nsym = m_OrientationOps[phase1]->getNumSymOps();
            QuaternionMathF::Conjugate(q2);
            QuaternionMathF::Multiply(q2, q1, misq);
            OrientationMath::QuattoMat(q1, g1);
            MatrixMath::Multiply3x3with3x1(g1, normal, xstl_norm);
            for (int j = 0; j < nsym; j++)
            {
              m_OrientationOps[phase1]->getQuatSymOp(j, sym_q);
              //calculate crystal direction parallel to normal
              QuaternionMathF::Multiply(sym_q, misq, s1_misq);
              OrientationMath::MultiplyQuaternionVector(sym_q, xstl_norm, s_xstl_norm);
              for (int k = 0; k < nsym; k++)
              {
                //calculate the symmetric misorienation
                m_OrientationOps[phase1]->getQuatSymOp(k, sym_q);
                QuaternionMathF::Conjugate(sym_q);
                QuaternionMathF::Multiply(s1_misq, sym_q, s2_misq);
                OrientationMath::QuattoAxisAngle(s2_misq, w, n1, n2, n3);
                w = w * 180.0 / DREAM3D::Constants::k_Pi;
                axisdiff111 = acosf(fabs(n1) * 0.57735f + fabs(n2) * 0.57735f + fabs(n3) * 0.57735f);
                angdiff60 = fabs(w - 60.0f);
                if (axisdiff111 < m_AxisTol && angdiff60 < m_AngTol)
                {
                  n[0] = n1;
                  n[1] = n2;
                  n[2] = n3;
                  m_TwinBoundary[i] = true;
                  incoherence = 180.0 * acos(GeometryMath::CosThetaBetweenVectors(n, s_xstl_norm)) / DREAM3D::Constants::k_Pi;
                  if(incoherence < m_TwinBoundaryIncoherence[i]) { m_TwinBoundaryIncoherence[i] = incoherence; }
                }
              }
            }
          }
        }
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      generate(r.begin(), r.end());
    }
#endif
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTwinBoundaries::FindTwinBoundaries()  :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_SurfaceDataContainerName(DREAM3D::HDF5::SurfaceDataContainerName),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceNormalsArrayName(DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshTwinBoundaryArrayName(DREAM3D::FaceData::SurfaceMeshTwinBoundary),
  m_SurfaceMeshTwinBoundaryIncoherenceArrayName(DREAM3D::FaceData::SurfaceMeshTwinBoundaryIncoherence),
  m_AvgQuats(NULL),
  m_FeaturePhases(NULL),
  m_CrystalStructures(NULL),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFaceNormals(NULL),
  m_SurfaceMeshTwinBoundary(NULL),
  m_SurfaceMeshTwinBoundaryIncoherence(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTwinBoundaries::~FindTwinBoundaries()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaries::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Axis Tolerance");
    option->setPropertyName("AxisTolerance");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("Degrees");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Angle Tolerance");
    option->setPropertyName("AngleTolerance");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("Degrees");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindTwinBoundaries::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setAxisTolerance( reader->readValue("AxisTolerance", getAxisTolerance() ) );
  setAngleTolerance( reader->readValue("AngleTolerance", getAngleTolerance()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindTwinBoundaries::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("AxisTolerance", getAxisTolerance() );
  writer->writeValue("AngleTolerance", getAngleTolerance() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaries::dataCheckVoxel(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 4);
  m_AvgQuats = m->getPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_AvgQuatsArrayName, -301, features, dims);
  dims[0] = 1;
  m_FeaturePhases = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_FeaturePhasesArrayName, -303, features, dims);
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructures = m->getPrereqArray<unsigned int, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_CrystalStructuresArrayName, -305, ensembles, dims);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaries::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());

  QVector<int> dims(1, 2);
  m_SurfaceMeshFaceLabels = sm->getPrereqArray<int32_t, AbstractFilter>(this, m_FaceAttributeMatrixName,  m_SurfaceMeshFaceLabelsArrayName, -386, features, dims);
  dims[0] = 3;
  m_SurfaceMeshFaceNormals = sm->getPrereqArray<double, AbstractFilter>(this, m_FaceAttributeMatrixName,  m_SurfaceMeshFaceNormalsArrayName, -387, features, dims);
  dims[0] = 1;
  m_SurfaceMeshTwinBoundary = sm->createNonPrereqArray<bool, AbstractFilter>(this, m_FaceAttributeMatrixName,  m_SurfaceMeshTwinBoundaryArrayName, false, features, dims);
  m_SurfaceMeshTwinBoundaryIncoherence = sm->createNonPrereqArray<float, AbstractFilter>(this, m_FaceAttributeMatrixName,  m_SurfaceMeshTwinBoundaryIncoherenceArrayName, 180.0, features, dims);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaries::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    addErrorMessage(getHumanLabel(), "The VolumeDataContainer Object with the specific name " + getDataContainerName() + " was not available.", getErrorCondition());
    return;
  }

  dataCheckVoxel(true, 1, 1, 1);

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  dataCheckSurfaceMesh(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaries::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMesh DataContainer Object was NULL", -999);
    return;
  }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  int numTriangles = sm->getNumFaceTuples();
  dataCheckVoxel(false, m->getTotalPoints(), m->getNumCellFeatureTuples(), m->getNumCellEnsembleTuples());
  dataCheckSurfaceMesh(false, 0, numTriangles, 0);
  if (getErrorCondition() < 0)
  {
    return;
  }

  float angtol = m_AngleTolerance;
  float axistol = static_cast<float>( m_AxisTolerance * M_PI / 180.0f );


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numTriangles),
                      CalculateTwinBoundaryImpl(angtol, axistol, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_AvgQuats, m_FeaturePhases, m_CrystalStructures, m_SurfaceMeshTwinBoundary, m_SurfaceMeshTwinBoundaryIncoherence), tbb::auto_partitioner());

  }
  else
#endif
  {
    CalculateTwinBoundaryImpl serial(angtol, axistol, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_AvgQuats, m_FeaturePhases, m_CrystalStructures, m_SurfaceMeshTwinBoundary, m_SurfaceMeshTwinBoundaryIncoherence);
    serial.generate(0, numTriangles);
  }

  notifyStatusMessage("FindTwinBoundaries Completed");
}
