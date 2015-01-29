/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
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
 *
 *  This code was written under United States Air Force Contract number
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
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
#include "DREAM3DLib/Math/GeometryMath.h"
#include "OrientationLib/Math/OrientationMath.h"


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
    bool m_FindCoherence;
    QVector<OrientationOps::Pointer> m_OrientationOps;

  public:
    CalculateTwinBoundaryImpl(float angtol, float axistol, int32_t* Labels, double* Normals, float* Quats, int32_t* Phases, unsigned int* CrystalStructures, bool* TwinBoundary, float* TwinBoundaryIncoherence, bool FindCoherence) :
      m_AxisTol(axistol),
      m_AngTol(angtol),
      m_Labels(Labels),
      m_Normals(Normals),
      m_Phases(Phases),
      m_Quats(Quats),
      m_TwinBoundary(TwinBoundary),
      m_TwinBoundaryIncoherence(TwinBoundaryIncoherence),
      m_CrystalStructures(CrystalStructures),
      m_FindCoherence(FindCoherence)
    {
      m_OrientationOps = OrientationOps::getOrientationOpsQVector();
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
        if(m_FindCoherence)
        {
          normal[0] = m_Normals[3 * i];
          normal[1] = m_Normals[3 * i + 1];
          normal[2] = m_Normals[3 * i + 2];
        }
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
          
            if(m_FindCoherence)
              MatrixMath::Multiply3x3with3x1(g1, normal, xstl_norm);

            for (int j = 0; j < nsym; j++)
            {
              m_OrientationOps[phase1]->getQuatSymOp(j, sym_q);
              //calculate crystal direction parallel to normal
              QuaternionMathF::Multiply(sym_q, misq, s1_misq);

              if(m_FindCoherence)
                OrientationMath::MultiplyQuaternionVector(sym_q, xstl_norm, s_xstl_norm);
              //QuaternionMathF::MultiplyQuatVec(sym_q, xstl_norm, s_xstl_norm);
              
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
                  if(m_FindCoherence)
                  {
                    incoherence = 180.0 * acos(GeometryMath::CosThetaBetweenVectors(n, s_xstl_norm)) / DREAM3D::Constants::k_Pi;
                    if(incoherence > 90.0) { incoherence = 180.0 - incoherence; }
                    if(incoherence < m_TwinBoundaryIncoherence[i]) { m_TwinBoundaryIncoherence[i] = incoherence; }
                  }
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
  m_AxisTolerance(0.0),
  m_AngleTolerance(0.0),
  m_FindCoherence(true),
  m_AvgQuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::SurfaceDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceNormalsArrayPath(DREAM3D::Defaults::SurfaceDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshTwinBoundaryArrayName(DREAM3D::FaceData::SurfaceMeshTwinBoundary),
  m_SurfaceMeshTwinBoundaryIncoherenceArrayName(DREAM3D::FaceData::SurfaceMeshTwinBoundaryIncoherence),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_AvgQuats(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFaceNormalsArrayName(DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshFaceNormals(NULL),
  m_SurfaceMeshTwinBoundary(NULL),
  m_SurfaceMeshTwinBoundaryIncoherence(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsQVector();
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
  parameters.push_back(FilterParameter::New("Axis Tolerance", "AxisTolerance", FilterParameterWidgetType::DoubleWidget, getAxisTolerance(), false, "Degrees"));
  parameters.push_back(FilterParameter::New("Angle Tolerance", "AngleTolerance", FilterParameterWidgetType::DoubleWidget, getAngleTolerance(), false, "Degrees"));

  QStringList linkedProps;
  linkedProps << "SurfaceMeshFaceNormalsArrayPath" << "SurfaceMeshTwinBoundaryIncoherenceArrayName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Compute Coherence", "FindCoherence", getFindCoherence(), linkedProps, false));

  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("AvgQuats", "AvgQuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getAvgQuatsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("FeaturePhases", "FeaturePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeaturePhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("SurfaceMeshFaceLabels", "SurfaceMeshFaceLabelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFaceLabelsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("SurfaceMeshFaceNormals", "SurfaceMeshFaceNormalsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFaceNormalsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("SurfaceMeshTwinBoundary", "SurfaceMeshTwinBoundaryArrayName", FilterParameterWidgetType::StringWidget, getSurfaceMeshTwinBoundaryArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("SurfaceMeshTwinBoundaryIncoherence", "SurfaceMeshTwinBoundaryIncoherenceArrayName", FilterParameterWidgetType::StringWidget, getSurfaceMeshTwinBoundaryIncoherenceArrayName(), true, ""));
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindTwinBoundaries::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshTwinBoundaryIncoherenceArrayName(reader->readString("SurfaceMeshTwinBoundaryIncoherenceArrayName", getSurfaceMeshTwinBoundaryIncoherenceArrayName() ) );
  setSurfaceMeshTwinBoundaryArrayName(reader->readString("SurfaceMeshTwinBoundaryArrayName", getSurfaceMeshTwinBoundaryArrayName() ) );
  setSurfaceMeshFaceNormalsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceNormalsArrayPath", getSurfaceMeshFaceNormalsArrayPath() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath() ) );
  setAxisTolerance( reader->readValue("AxisTolerance", getAxisTolerance() ) );
  setAngleTolerance( reader->readValue("AngleTolerance", getAngleTolerance()) );
  setFindCoherence( reader->readValue("FindCoherence", getFindCoherence()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindTwinBoundaries::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshTwinBoundaryIncoherenceArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshTwinBoundaryArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceNormalsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(AvgQuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(AxisTolerance)
  DREAM3D_FILTER_WRITE_PARAMETER(AngleTolerance)
  DREAM3D_FILTER_WRITE_PARAMETER(FindCoherence)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaries::dataCheckVoxel()
{
  setErrorCondition(0);

  QVector<size_t> dims(1, 4);
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
//typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaries::dataCheckSurfaceMesh()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  
  if(getFindCoherence())
  {
    dims[0] = 3;
    m_SurfaceMeshFaceNormalsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceNormalsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceNormalsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  dims[0] = 1;
  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), m_SurfaceMeshFaceLabelsArrayPath.getAttributeMatrixName(), getSurfaceMeshTwinBoundaryArrayName() );
  m_SurfaceMeshTwinBoundaryPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshTwinBoundaryPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshTwinBoundary = m_SurfaceMeshTwinBoundaryPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(getFindCoherence())
  {
    tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), m_SurfaceMeshFaceLabelsArrayPath.getAttributeMatrixName(), getSurfaceMeshTwinBoundaryIncoherenceArrayName() );
    m_SurfaceMeshTwinBoundaryIncoherencePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 180.0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshTwinBoundaryIncoherencePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshTwinBoundaryIncoherence = m_SurfaceMeshTwinBoundaryIncoherencePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
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

  float angtol = m_AngleTolerance;
  float axistol = static_cast<float>( m_AxisTolerance * M_PI / 180.0f );

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numTriangles),
                      CalculateTwinBoundaryImpl(angtol, axistol, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_AvgQuats, m_FeaturePhases, m_CrystalStructures, m_SurfaceMeshTwinBoundary, m_SurfaceMeshTwinBoundaryIncoherence, m_FindCoherence), tbb::auto_partitioner());

  }
  else
#endif
  {
    CalculateTwinBoundaryImpl serial(angtol, axistol, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_AvgQuats, m_FeaturePhases, m_CrystalStructures, m_SurfaceMeshTwinBoundary, m_SurfaceMeshTwinBoundaryIncoherence, m_FindCoherence);
    serial.generate(0, numTriangles);
  }

  notifyStatusMessage(getHumanLabel(), "FindTwinBoundaries Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindTwinBoundaries::newFilterInstance(bool copyFilterParameters)
{
  FindTwinBoundaries::Pointer filter = FindTwinBoundaries::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTwinBoundaries::getCompiledLibraryName()
{ return Statistics::StatisticsBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTwinBoundaries::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTwinBoundaries::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CrystallographicFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTwinBoundaries::getHumanLabel()
{ return "Find Twin Boundaries"; }

