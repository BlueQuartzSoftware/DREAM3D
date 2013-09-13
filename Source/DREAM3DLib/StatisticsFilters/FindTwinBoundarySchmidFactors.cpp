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

#include "FindTwinBoundarySchmidFactors.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindSurfaceGrains.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"




class CalculateTwinBoundarySchmidFactorsImpl
{
    int32_t* m_Labels;
    double* m_Normals;
    float* m_Quats;
    bool* m_TwinBoundary;
    float* m_TwinBoundarySchmidFactors;
    float* m_LoadDir;
    QVector<OrientationOps::Pointer> m_OrientationOps;

  public:
    CalculateTwinBoundarySchmidFactorsImpl(float* LoadingDir, int32_t* Labels, double* Normals, float* Quats, bool* TwinBoundary, float* TwinBoundarySchmidFactors) :
      m_Labels(Labels),
      m_Normals(Normals),
      m_Quats(Quats),
      m_TwinBoundary(TwinBoundary),
      m_TwinBoundarySchmidFactors(TwinBoundarySchmidFactors),
      m_LoadDir(LoadingDir)
    {
    m_OrientationOps = OrientationOps::getOrientationOpsVector();
    }
    virtual ~CalculateTwinBoundarySchmidFactorsImpl(){}

    void generate(size_t start, size_t end) const
    {
      int grain1, grain2, grain;
      float normal[3];
      float g1[3][3];
      float schmid1, schmid2, schmid3;
 //     int plane =0;
 //     unsigned int phase1, phase2;
      QuatF q1;
   //   QuatF q2;
      QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

      float n[3], b[3];
      float crystalLoading[3];
      float cosPhi, cosLambda;
//      float misq[5], sym_q[5], s_misq[5];

      for (size_t i = start; i < end; i++)
      {
        if(m_TwinBoundary[i] == true)
        {
          grain1 = m_Labels[2*i];
          grain2 = m_Labels[2*i+1];
          normal[0] = m_Normals[3*i];
          normal[1] = m_Normals[3*i+1];
          normal[2] = m_Normals[3*i+2];
          schmid1 = 0, schmid2 = 0, schmid3 = 0;
      //    plane = 0;
          if(grain1 > grain2) grain = grain1;
          else grain = grain2;

          QuaternionMathF::Copy(quats[grain], q1);
//          for(int m=0;m<5;m++)
//          {
//            q1[m]=m_Quats[5*grain+m];
//          }
          //calculate crystal direction parallel to normal
          OrientationMath::QuattoMat(q1, g1);
          MatrixMath::Multiply3x3with3x1(g1,normal,n);
          //calculate crystal direction parallel to loading direction
          MatrixMath::Multiply3x3with3x1(g1, m_LoadDir, crystalLoading);

          if(n[2] < 0) n[0] = -n[0], n[1] = -n[1], n[2] = -n[2];
          if(n[0] > 0 && n[1] > 0)
          {
         //   plane = 1;
            n[0] = 1, n[1] = 1, n[2] = 1;
            cosPhi = fabs(MatrixMath::CosThetaBetweenVectors(crystalLoading, n));
            b[0] = 1, b[1] = -1, b[2] = 0;
            cosLambda = fabs(MatrixMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid1 = cosPhi*cosLambda;
            b[0] = -1, b[1] = 0, b[2] = 1;
            cosLambda = fabs(MatrixMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid2 = cosPhi*cosLambda;
            b[0] = 0, b[1] = -1, b[2] = 1;
            cosLambda = fabs(MatrixMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid3 = cosPhi*cosLambda;
            m_TwinBoundarySchmidFactors[3*i] = schmid1;
            m_TwinBoundarySchmidFactors[3*i+1] = schmid2;
            m_TwinBoundarySchmidFactors[3*i+2] = schmid3;
          }
          else if(n[0] > 0 && n[1] < 0)
          {
     //       plane = 2;
            n[0] = 1, n[1] = -1, n[2] = 1;
            cosPhi = fabs(MatrixMath::CosThetaBetweenVectors(crystalLoading, n));
            b[0] = 1, b[1] = 1, b[2] = 0;
            cosLambda = fabs(MatrixMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid1 = cosPhi*cosLambda;
            b[0] = 0, b[1] = 1, b[2] = 1;
            cosLambda = fabs(MatrixMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid2 = cosPhi*cosLambda;
            b[0] = -1, b[1] = 0, b[2] = 1;
            cosLambda = fabs(MatrixMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid3 = cosPhi*cosLambda;
            m_TwinBoundarySchmidFactors[3*i] = schmid1;
            m_TwinBoundarySchmidFactors[3*i+1] = schmid2;
            m_TwinBoundarySchmidFactors[3*i+2] = schmid3;
          }
          else if(n[0] < 0 && n[1] > 0)
          {
     //       plane = 3;
            n[0] = -1, n[1] = 1, n[2] = 1;
            cosPhi = fabs(MatrixMath::CosThetaBetweenVectors(crystalLoading, n));
            b[0] = 1, b[1] = 1, b[2] = 0;
            cosLambda = fabs(MatrixMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid1 = cosPhi*cosLambda;
            b[0] = 1, b[1] = 0, b[2] = 1;
            cosLambda = fabs(MatrixMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid2 = cosPhi*cosLambda;
            b[0] = 0, b[1] = -1, b[2] = 1;
            cosLambda = fabs(MatrixMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid3 = cosPhi*cosLambda;
            m_TwinBoundarySchmidFactors[3*i] = schmid1;
            m_TwinBoundarySchmidFactors[3*i+1] = schmid2;
            m_TwinBoundarySchmidFactors[3*i+2] = schmid3;
          }
          else if(n[0] < 0 && n[1] < 0)
          {
       //     plane = 4;
            n[0] = -1, n[1] = -1, n[2] = 1;
            cosPhi = fabs(MatrixMath::CosThetaBetweenVectors(crystalLoading, n));
            b[0] = 1, b[1] = 0, b[2] = 1;
            cosLambda = fabs(MatrixMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid1 = cosPhi*cosLambda;
            b[0] = 0, b[1] = 1, b[2] = 1;
            cosLambda = fabs(MatrixMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid2 = cosPhi*cosLambda;
            b[0] = 1, b[1] = -1, b[2] = 0;
            cosLambda = fabs(MatrixMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid3 = cosPhi*cosLambda;
            m_TwinBoundarySchmidFactors[3*i] = schmid1;
            m_TwinBoundarySchmidFactors[3*i+1] = schmid2;
            m_TwinBoundarySchmidFactors[3*i+2] = schmid3;
          }
        }
        else
        {
          m_TwinBoundarySchmidFactors[3*i] = 0;
          m_TwinBoundarySchmidFactors[3*i+1] = 0;
          m_TwinBoundarySchmidFactors[3*i+2] = 0;
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
FindTwinBoundarySchmidFactors::FindTwinBoundarySchmidFactors()  :
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
  m_OrientationOps = OrientationOps::getOrientationOpsVector();
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
  QVector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Twin Boundary Info File");
    option->setPropertyName("TwinBoundarySchmidFactorsFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
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
void FindTwinBoundarySchmidFactors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setLoadingDir( reader->readValue("LoadingDir", getLoadingDir()));
  setTwinBoundarySchmidFactorsFile( reader->readValue("TwinBoundarySchmidFactorsFile", getTwinBoundarySchmidFactorsFile()));
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindTwinBoundarySchmidFactors::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("TwinBoundarySchmidFactorsFile", getTwinBoundarySchmidFactorsFile() );
  writer->writeValue("LoadingDir", getLoadingDir() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundarySchmidFactors::dataCheckVoxel(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  
  VolumeDataContainer* m = getVolumeDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellFieldData, AvgQuats, -301, float, FloatArrayType, fields, 4)

  GET_PREREQ_DATA(m, DREAM3D, CellFieldData, FieldPhases, -303, int32_t, Int32ArrayType, fields, 1)

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, CellEnsembleData, CrystalStructures, -305, unsigned int, XTalStructArrayType, ensembles, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundarySchmidFactors::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  
  SurfaceDataContainer* sm = getSurfaceDataContainer();

  GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshFaceLabels, -386, int32_t, Int32ArrayType, fields, 2)
  GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshFaceNormals, -387, double, DoubleArrayType, fields, 3)
  GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshTwinBoundary, -388, bool, BoolArrayType, fields, 1)
  CREATE_NON_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshTwinBoundarySchmidFactors, float, FloatArrayType, 0, fields, 3)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundarySchmidFactors::preflight()
{
  dataCheckVoxel(true, 1, 1, 1);
  dataCheckSurfaceMesh(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundarySchmidFactors::execute()
{
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  SurfaceDataContainer* sm = getSurfaceDataContainer();
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
  dataCheckVoxel(false, m->getTotalPoints(), m->getNumCellFieldTuples(), m->getNumCellEnsembleTuples());
  dataCheckSurfaceMesh(false, 0, numTriangles, 0);
  if (getErrorCondition() < 0)
  {
    return;
  }

  float LoadingDir[3];
  LoadingDir[0] = m_LoadingDir.x;
  LoadingDir[1] = m_LoadingDir.y;
  LoadingDir[2] = m_LoadingDir.z;

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
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


  std::ofstream outFile;
  outFile.open(m_TwinBoundarySchmidFactorsFile.toLatin1().data(), std::ios_base::binary);

  outFile << "Grain1	Grain2	IsTwin	Plane	Schmid1	Schmid2	Schmid3" << "\n";
  for (size_t i = 0; i < numTriangles; i++)
  {
    outFile << m_SurfaceMeshFaceLabels[2*i] << "  " << m_SurfaceMeshFaceLabels[2*i+1] << "  " << m_SurfaceMeshTwinBoundary[i] << "  " << m_SurfaceMeshTwinBoundarySchmidFactors[3*i] << "  " << m_SurfaceMeshTwinBoundarySchmidFactors[3*i+1] << "  " << m_SurfaceMeshTwinBoundarySchmidFactors[3*i+2] << "\n";
  }
  outFile.close();

  notifyStatusMessage("FindTwinBoundarySchmidFactors Completed");
}
