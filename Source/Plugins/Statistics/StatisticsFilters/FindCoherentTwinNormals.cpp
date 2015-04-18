/* ============================================================================
 * Copyright (c) 2015 William C Lenthe (UCSB)
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
 * Neither the name of William C. Lenthe, UCSB, nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FindCoherentTwinNormals.h"

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
#include "DREAM3DLib/Math/GeometryMath.h"
#include "OrientationLib/Math/OrientationMath.h"


class FindCoherentTwinNormalsImpl
{
    int32_t* m_Labels;
    int32_t* m_Phases;
    float* m_Quats;
    bool* m_TwinBoundary;
    double* m_CoherentNormals;
    unsigned int* m_CrystalStructures;
    QVector<OrientationOps::Pointer> m_OrientationOps;

  public:
    FindCoherentTwinNormalsImpl(int32_t* Labels, float* Quats, int32_t* Phases, unsigned int* CrystalStructures, bool* TwinBoundary, double* CoherentFaceNormals) :
      m_Labels(Labels),
      m_Quats(Quats),
      m_Phases(Phases),
      m_CrystalStructures(CrystalStructures),
      m_TwinBoundary(TwinBoundary),
      m_CoherentNormals(CoherentFaceNormals)
    {
      m_OrientationOps = OrientationOps::getOrientationOpsQVector();
    }

    virtual ~FindCoherentTwinNormalsImpl() {}

    void generate(size_t start, size_t end) const
    {
      //convert to quats
      QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

      //create vectors for {111} normals (I'll use all positive Z)
      float n[4][3] = {{ DREAM3D::Constants::k_1OverRoot3,  DREAM3D::Constants::k_1OverRoot3, DREAM3D::Constants::k_1OverRoot3},
                       {-DREAM3D::Constants::k_1OverRoot3,  DREAM3D::Constants::k_1OverRoot3, DREAM3D::Constants::k_1OverRoot3},
                       { DREAM3D::Constants::k_1OverRoot3, -DREAM3D::Constants::k_1OverRoot3, DREAM3D::Constants::k_1OverRoot3},
                       {-DREAM3D::Constants::k_1OverRoot3, -DREAM3D::Constants::k_1OverRoot3, DREAM3D::Constants::k_1OverRoot3}};

      for(size_t i = start; i < end; i++)
      {
        //set normal to 0 by default
        m_CoherentNormals[3 * i + 0] = 0;
        m_CoherentNormals[3 * i + 1] = 0;
        m_CoherentNormals[3 * i + 2] = 0;

        //check if this is a twin boundary
        if(m_TwinBoundary[i])
        {
          //get bounding grains
          int32_t grain1 = m_Labels[2 * i + 0];
          int32_t grain2 = m_Labels[2 * i + 1];

          //make sure than this is a cubic twin boundary (both phases should be the same if already flagged as a twin)
          if(Ebsd::CrystalStructure::Cubic_High == m_CrystalStructures[m_Phases[grain1]])
          {
            //get orientations of bounding grains
            QuatF q1, q2, deltaQ;
            QuaternionMathF::Copy(quats[grain1], q1);
            QuaternionMathF::Copy(quats[grain2], q2);

            //get rotation from grain1 to grain2
            QuaternionMathF::Conjugate(q1);
            QuaternionMathF::Multiply(q1, q2, deltaQ);

            //rotate vectors from crystal frame of grain2 to crystal frame of grain1
            float np[4][3];
            QuaternionMathF::MultiplyQuatVec(deltaQ, n[0], np[0]);
            QuaternionMathF::MultiplyQuatVec(deltaQ, n[1], np[1]);
            QuaternionMathF::MultiplyQuatVec(deltaQ, n[2], np[2]);
            QuaternionMathF::MultiplyQuatVec(deltaQ, n[3], np[3]);

            //find rotated vector that is still 111 direction (others will be permutation of 115 -> dotproduct of 1 or 0.777)
            size_t bestJ;
            float maxCos = 0;
            for(size_t j = 0; j < 4; j++)
            {
              float cos111 = fabs(np[j][0]) * DREAM3D::Constants::k_1OverRoot3 + fabs(np[j][1]) * DREAM3D::Constants::k_1OverRoot3 + fabs(np[j][2]) * DREAM3D::Constants::k_1OverRoot3;
              if(cos111 > maxCos)
              {
                maxCos = cos111;
                bestJ = j;
              }
            }

            //check for antiparallel case
            if(np[bestJ][2] < 0.0f)
            {
              np[bestJ][0] = - np[bestJ][0];
              np[bestJ][1] = - np[bestJ][1];
              np[bestJ][2] = - np[bestJ][2];
            }

            //get matching direction in this grain
            size_t type = 0;
            if(np[bestJ][0] > 0 && np[bestJ][1] > 0)
              type = 0;
            else if(np[bestJ][0] < 0 && np[bestJ][1] > 0)
              type = 1;
            else if(np[bestJ][0] > 0 && np[bestJ][1] < 0)
              type = 2;
            else// if(np[bestJ][0] < 0 && np[bestJ][1] < 0)
              type = 3;

            //if the twin isn't perfect the normals may not be exactly aligned, compute average
            float c[3];
            MatrixMath::Add3x1s(n[type], np[bestJ], c);
            MatrixMath::Normalize3x1(c);

            //rotate from grain2 crystal frame to orientation reference frame
            float norm[3];
            QuaternionMathF::Conjugate(q2);
            QuaternionMathF::MultiplyQuatVec(q2, c, norm);

            //copy
            m_CoherentNormals[3 * i + 0] = norm[0];
            m_CoherentNormals[3 * i + 1] = norm[1];
            m_CoherentNormals[3 * i + 2] = norm[2];
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
FindCoherentTwinNormals::FindCoherentTwinNormals()  :
  AbstractFilter(),
  m_AvgQuatsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshTwinBoundaryArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshTwinBoundary),
  m_SurfaceMeshCoherentFaceNormalsArrayName(DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_AvgQuats(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshTwinBoundaryArrayName(DREAM3D::FaceData::SurfaceMeshTwinBoundary),
  m_SurfaceMeshTwinBoundary(NULL),
  m_SurfaceMeshCoherentFaceNormals(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsQVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindCoherentTwinNormals::~FindCoherentTwinNormals()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCoherentTwinNormals::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("AvgQuats", "AvgQuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getAvgQuatsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("FeaturePhases", "FeaturePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeaturePhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("SurfaceMeshFaceLabels", "SurfaceMeshFaceLabelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFaceLabelsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("SurfaceMeshTwinBoundary", "SurfaceMeshTwinBoundaryArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshTwinBoundaryArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("SurfaceMeshCoherentFaceNormals", "SurfaceMeshCoherentFaceNormalsArrayName", FilterParameterWidgetType::StringWidget, getSurfaceMeshCoherentFaceNormalsArrayName(), true, ""));
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindCoherentTwinNormals::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshCoherentFaceNormalsArrayName(reader->readString("SurfaceMeshCoherentFaceNormalsArrayName", getSurfaceMeshCoherentFaceNormalsArrayName() ) );
  setSurfaceMeshTwinBoundaryArrayPath(reader->readDataArrayPath("SurfaceMeshTwinBoundaryArrayPath", getSurfaceMeshTwinBoundaryArrayPath() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindCoherentTwinNormals::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshCoherentFaceNormalsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshTwinBoundaryArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(AvgQuatsArrayPath)
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCoherentTwinNormals::dataCheckVoxel()
{
  setErrorCondition(0);

  QVector<size_t> dims(1, 4);
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), dims);
  if( NULL != m_AvgQuatsPtr.lock().get() )
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);

  dims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), dims);
  if( NULL != m_FeaturePhasesPtr.lock().get() )
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims);
  if( NULL != m_CrystalStructuresPtr.lock().get() )
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCoherentTwinNormals::dataCheckSurfaceMesh()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), dims);
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() )
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);

  dims[0] = 1;
  m_SurfaceMeshTwinBoundaryPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getSurfaceMeshTwinBoundaryArrayPath(), dims);
  if( NULL != m_SurfaceMeshTwinBoundaryPtr.lock().get() )
    m_SurfaceMeshTwinBoundary = m_SurfaceMeshTwinBoundaryPtr.lock()->getPointer(0);

  dims[0] = 3;
  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), m_SurfaceMeshFaceLabelsArrayPath.getAttributeMatrixName(), getSurfaceMeshCoherentFaceNormalsArrayName() );
  m_SurfaceMeshCoherentFaceNormalsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this,  tempPath, 180.0, dims);
  if( NULL != m_SurfaceMeshCoherentFaceNormalsPtr.lock().get() )
    m_SurfaceMeshCoherentFaceNormals = m_SurfaceMeshCoherentFaceNormalsPtr.lock()->getPointer(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCoherentTwinNormals::preflight()
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
void FindCoherentTwinNormals::execute()
{
  setErrorCondition(0);
  dataCheckVoxel();
  if(getErrorCondition() < 0) { return; }
  dataCheckSurfaceMesh();
  if(getErrorCondition() < 0) { return; }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  int64_t numTriangles = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numTriangles), FindCoherentTwinNormalsImpl(m_SurfaceMeshFaceLabels, m_AvgQuats, m_FeaturePhases, m_CrystalStructures, m_SurfaceMeshTwinBoundary, m_SurfaceMeshCoherentFaceNormals), tbb::auto_partitioner());

  }
  else
#endif
  {
    FindCoherentTwinNormalsImpl serial(m_SurfaceMeshFaceLabels, m_AvgQuats, m_FeaturePhases, m_CrystalStructures, m_SurfaceMeshTwinBoundary, m_SurfaceMeshCoherentFaceNormals);
    serial.generate(0, numTriangles);
  }

  notifyStatusMessage(getHumanLabel(), "Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindCoherentTwinNormals::newFilterInstance(bool copyFilterParameters)
{
  FindCoherentTwinNormals::Pointer filter = FindCoherentTwinNormals::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindCoherentTwinNormals::getCompiledLibraryName()
{ return Statistics::StatisticsBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindCoherentTwinNormals::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindCoherentTwinNormals::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CrystallographicFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindCoherentTwinNormals::getHumanLabel()
{ return "Find Coherent Twin Normals"; }

