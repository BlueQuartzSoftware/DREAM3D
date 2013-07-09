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

#include "FindTwinBoundaryInfo.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Common/MatrixMath.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindSurfaceGrains.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"

const static float m_pi = static_cast<float>(M_PI);


class CalculateTwinBoundaryImpl
{
    float m_AxisTol;
    float m_AngTol;
    int32_t* m_Labels;
    double* m_Normals;
    int32_t* m_Phases;
    float* m_Quats;
    bool* m_TwinBoundary;
    float* m_TwinBoundarySchmidFactors;
    unsigned int* m_CrystalStructures;
    float* m_LoadDir;
    std::vector<OrientationMath::Pointer> m_OrientationOps;

  public:
    CalculateTwinBoundaryImpl(float angtol, float axistol, float* LoadingDir, int32_t* Labels, double* Normals, float* Quats, int32_t* Phases, unsigned int* CrystalStructures, bool* TwinBoundary, float* TwinBoundarySchmidFactors) :
      m_AxisTol(axistol),
      m_AngTol(angtol),
      m_Labels(Labels),
      m_Normals(Normals),
      m_Phases(Phases),
      m_Quats(Quats),
      m_TwinBoundary(TwinBoundary),
      m_TwinBoundarySchmidFactors(TwinBoundarySchmidFactors),
      m_CrystalStructures(CrystalStructures),
      m_LoadDir(LoadingDir)
    {
    m_OrientationOps = OrientationMath::getOrientationOpsVector();
    }
    virtual ~CalculateTwinBoundaryImpl(){}

    void generate(size_t start, size_t end) const
    {
      int grain1, grain2;
      float normal[3];
      float g1[3][3];
      float w;
      float schmid1, schmid2, schmid3;
      int plane =0;
      unsigned int phase1, phase2;
      bool isTwin = false;
      float q1[5], q2[5];
      float axisdiff111, angdiff60;
      float n[3], b[3];
      float crystalLoading[3];
      float cosPhi, cosLambda;
      float n1 = 0.0f, n2 = 0.0f, n3= 0.0f;
      float misq[5], sym_q[5], s_misq[5];
      float xstl_norm[3];

      for (size_t i = start; i < end; i++)
      {
        grain1 = m_Labels[2*i];
        grain2 = m_Labels[2*i+1];
        normal[0] = m_Normals[3*i];
        normal[1] = m_Normals[3*i+1];
        normal[2] = m_Normals[3*i+2];
        if(m_Phases[grain1] == m_Phases[grain2])
        {
          w = 10000.0;
          isTwin = false;
          schmid1 = 0, schmid2 = 0, schmid3 = 0;
          plane = 0;
          for(int m=0;m<5;m++)
          {
            q1[m]=m_Quats[5*grain1+m];
            q2[m]=m_Quats[5*grain2+m];
          }

          phase1 = m_CrystalStructures[m_Phases[grain1]];
          phase2 = m_CrystalStructures[m_Phases[grain2]];
          if (phase1 == phase2)
          {
            OrientationMath::invertQuaternion(q2);
            OrientationMath::multiplyQuaternions(q2, q1, misq);
            int nsym = m_OrientationOps[phase1]->getNumSymOps();
            for (int k=0; k< nsym;k++)
            {
              //calculate crystal direction parallel to normal
              OrientationMath::QuattoMat(q1, g1);
              MatrixMath::multiply3x3with3x1(g1,normal,xstl_norm);
              //calculate the symmetric misorienation
              m_OrientationOps[phase1]->getQuatSymOp(k, sym_q);
              OrientationMath::invertQuaternion(sym_q);
              OrientationMath::multiplyQuaternions(misq, sym_q, s_misq);
              OrientationMath::QuattoAxisAngle(s_misq, w, n1, n2, n3);
              w = w*180.0/m_pi;
              axisdiff111 = acosf(fabs(n1)*0.57735f+fabs(n2)*0.57735f+fabs(n3)*0.57735f);
              angdiff60 = fabs(w-60.0f);
              if (axisdiff111 < m_AxisTol && angdiff60 < m_AngTol)
              {
                isTwin = true;
                n[0] = xstl_norm[0], n[1] = xstl_norm[1], n[2] = xstl_norm[2];
              }
            }
            MatrixMath::multiply3x3with3x1(g1, m_LoadDir, crystalLoading);
            if (isTwin == true)
            {
              m_TwinBoundary[i] = true;
              if(n[2] < 0) n[0] = -n[0], n[1] = -n[1], n[2] = -n[2];
              if(n[0] > 0 && n[1] > 0)
              {
                plane = 1;
                n[0] = 1, n[1] = 1, n[2] = 1;
                cosPhi = fabs(MatrixMath::dotProduct(crystalLoading, n));
                b[0] = 1, b[1] = -1, b[2] = 0;
                cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
                schmid1 = cosPhi*cosLambda;
                b[0] = -1, b[1] = 0, b[2] = 1;
                cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
                schmid2 = cosPhi*cosLambda;
                b[0] = 0, b[1] = -1, b[2] = 1;
                cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
                schmid3 = cosPhi*cosLambda;
                m_TwinBoundarySchmidFactors[3*i] = schmid1;
                m_TwinBoundarySchmidFactors[3*i+1] = schmid2;
                m_TwinBoundarySchmidFactors[3*i+2] = schmid3;
              }
              else if(n[0] > 0 && n[1] < 0)
              {
                plane = 2;
                n[0] = 1, n[1] = -1, n[2] = 1;
                cosPhi = fabs(MatrixMath::dotProduct(crystalLoading, n));
                b[0] = 1, b[1] = 1, b[2] = 0;
                cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
                schmid1 = cosPhi*cosLambda;
                b[0] = 0, b[1] = 1, b[2] = 1;
                cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
                schmid2 = cosPhi*cosLambda;
                b[0] = -1, b[1] = 0, b[2] = 1;
                cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
                schmid3 = cosPhi*cosLambda;
                m_TwinBoundarySchmidFactors[3*i] = schmid1;
                m_TwinBoundarySchmidFactors[3*i+1] = schmid2;
                m_TwinBoundarySchmidFactors[3*i+2] = schmid3;
              }
              else if(n[0] < 0 && n[1] > 0)
              {
                plane = 3;
                n[0] = -1, n[1] = 1, n[2] = 1;
                cosPhi = fabs(MatrixMath::dotProduct(crystalLoading, n));
                b[0] = 1, b[1] = 1, b[2] = 0;
                cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
                schmid1 = cosPhi*cosLambda;
                b[0] = 1, b[1] = 0, b[2] = 1;
                cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
                schmid2 = cosPhi*cosLambda;
                b[0] = 0, b[1] = -1, b[2] = 1;
                cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
                schmid3 = cosPhi*cosLambda;
                m_TwinBoundarySchmidFactors[3*i] = schmid1;
                m_TwinBoundarySchmidFactors[3*i+1] = schmid2;
                m_TwinBoundarySchmidFactors[3*i+2] = schmid3;
              }
              else if(n[0] < 0 && n[1] < 0)
              {
                plane = 4;
                n[0] = -1, n[1] = -1, n[2] = 1;
                cosPhi = fabs(MatrixMath::dotProduct(crystalLoading, n));
                b[0] = 1, b[1] = 0, b[2] = 1;
                cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
                schmid1 = cosPhi*cosLambda;
                b[0] = 0, b[1] = 1, b[2] = 1;
                cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
                schmid2 = cosPhi*cosLambda;
                b[0] = 1, b[1] = -1, b[2] = 0;
                cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
                schmid3 = cosPhi*cosLambda;
                m_TwinBoundarySchmidFactors[3*i] = schmid1;
                m_TwinBoundarySchmidFactors[3*i+1] = schmid2;
                m_TwinBoundarySchmidFactors[3*i+2] = schmid3;
              }
            }
          }
        }
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t> &r) const
    {
      generate(r.begin(), r.end());
    }
#endif
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTwinBoundaryInfo::FindTwinBoundaryInfo()  :
  AbstractFilter(),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceNormalsArrayName(DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshTwinBoundaryArrayName(DREAM3D::FaceData::SurfaceMeshTwinBoundary),
  m_SurfaceMeshTwinBoundarySchmidFactorsArrayName(DREAM3D::FaceData::SurfaceMeshTwinBoundarySchmidFactors),
  m_AvgQuats(NULL),
  m_FieldPhases(NULL),
  m_CrystalStructures(NULL),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFaceNormals(NULL),
  m_SurfaceMeshTwinBoundary(NULL),
  m_SurfaceMeshTwinBoundarySchmidFactors(NULL)
{
  m_LoadingDir.x = 1.0f;
  m_LoadingDir.y = 1.0f;
  m_LoadingDir.z = 1.0f;
  m_OrientationOps = OrientationMath::getOrientationOpsVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTwinBoundaryInfo::~FindTwinBoundaryInfo()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaryInfo::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Twin Boundary Info File");
    option->setPropertyName("TwinBoundaryInfoFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
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
  {
    FilterParameter::Pointer option = FilterParameter::New();

    option->setHumanLabel("Loading Direction");
    option->setPropertyName("LoadingDir");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindTwinBoundaryInfo::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaryInfo::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("TwinBoundaryInfoFile", getTwinBoundaryInfoFile() );
  writer->writeValue("AxisTolerance", getAxisTolerance() );
  writer->writeValue("AngleTolerance", getAngleTolerance() );
  writer->writeValue("LoadingDirection", getLoadingDir() );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaryInfo::dataCheckVoxel(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 5)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -303, int32_t, Int32ArrayType, fields, 1)

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -305, unsigned int, XTalStructArrayType, ensembles, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaryInfo::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();

  GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshFaceLabels, ss, -386, int32_t, Int32ArrayType, fields, 2)
  GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshFaceNormals, ss, -387, double, DoubleArrayType, fields, 3)
  CREATE_NON_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshTwinBoundary, ss, bool, BoolArrayType, false, fields, 1)
  CREATE_NON_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshTwinBoundarySchmidFactors, ss, float, FloatArrayType, 0, fields, 3)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaryInfo::preflight()
{
  dataCheckVoxel(true, 1, 1, 1);
  dataCheckSurfaceMesh(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaryInfo::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
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
  dataCheckVoxel(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  dataCheckSurfaceMesh(false, 0, numTriangles, 0);
  if (getErrorCondition() < 0)
  {
    return;
  }

  float angtol = m_AngleTolerance;
  float axistol = static_cast<float>( m_AxisTolerance*M_PI/180.0f );
  float LoadingDir[3];
  LoadingDir[0] = m_LoadingDir.x;
  LoadingDir[1] = m_LoadingDir.y;
  LoadingDir[2] = m_LoadingDir.z;

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, numTriangles),
        CalculateTwinBoundaryImpl(angtol, axistol, LoadingDir, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_AvgQuats, m_FieldPhases, m_CrystalStructures, m_SurfaceMeshTwinBoundary, m_SurfaceMeshTwinBoundarySchmidFactors), tbb::auto_partitioner());

    }
    else
#endif
    {
      CalculateTwinBoundaryImpl serial(angtol, axistol, LoadingDir, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_AvgQuats, m_FieldPhases, m_CrystalStructures, m_SurfaceMeshTwinBoundary, m_SurfaceMeshTwinBoundarySchmidFactors);
      serial.generate(0, numTriangles);
    }


  std::ofstream outFile;
  outFile.open(m_TwinBoundaryInfoFile.c_str(), std::ios_base::binary);

  outFile << "Grain1	Grain2	IsTwin	Plane	Schmid1	Schmid2	Schmid3" << std::endl;
  for (size_t i = 0; i < numTriangles; i++)
  {
    outFile << m_SurfaceMeshFaceLabels[2*i] << "  " << m_SurfaceMeshFaceLabels[2*i+1] << "  " << m_SurfaceMeshTwinBoundary[i] << "  " << m_SurfaceMeshTwinBoundarySchmidFactors[3*i] << "  " << m_SurfaceMeshTwinBoundarySchmidFactors[3*i+1] << "  " << m_SurfaceMeshTwinBoundarySchmidFactors[3*i+2] << std::endl;
  }
  outFile.close();

  notifyStatusMessage("FindTwinBoundaryInfo Completed");
}
