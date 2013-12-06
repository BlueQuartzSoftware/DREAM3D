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
#include "DREAM3DLib/GenericFilters/FindSurfaceFeatures.h"
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"




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
    virtual ~CalculateTwinBoundarySchmidFactorsImpl() {}

    void generate(size_t start, size_t end) const
    {
      int feature1, feature2, feature;
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
          feature1 = m_Labels[2 * i];
          feature2 = m_Labels[2 * i + 1];
          normal[0] = m_Normals[3 * i];
          normal[1] = m_Normals[3 * i + 1];
          normal[2] = m_Normals[3 * i + 2];
          schmid1 = 0, schmid2 = 0, schmid3 = 0;
          //    plane = 0;
          if(feature1 > feature2) { feature = feature1; }
          else { feature = feature2; }

          QuaternionMathF::Copy(quats[feature], q1);
//          for(int m=0;m<5;m++)
//          {
//            q1[m]=m_Quats[5*feature+m];
//          }
          //calculate crystal direction parallel to normal
          OrientationMath::QuattoMat(q1, g1);
          MatrixMath::Multiply3x3with3x1(g1, normal, n);
          //calculate crystal direction parallel to loading direction
          MatrixMath::Multiply3x3with3x1(g1, m_LoadDir, crystalLoading);

          if(n[2] < 0) { n[0] = -n[0], n[1] = -n[1], n[2] = -n[2]; }
          if(n[0] > 0 && n[1] > 0)
          {
            //   plane = 1;
            n[0] = 1, n[1] = 1, n[2] = 1;
            cosPhi = fabs(GeometryMath::CosThetaBetweenVectors(crystalLoading, n));
            b[0] = 1, b[1] = -1, b[2] = 0;
            cosLambda = fabs(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid1 = cosPhi * cosLambda;
            b[0] = -1, b[1] = 0, b[2] = 1;
            cosLambda = fabs(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid2 = cosPhi * cosLambda;
            b[0] = 0, b[1] = -1, b[2] = 1;
            cosLambda = fabs(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid3 = cosPhi * cosLambda;
            m_TwinBoundarySchmidFactors[3 * i] = schmid1;
            m_TwinBoundarySchmidFactors[3 * i + 1] = schmid2;
            m_TwinBoundarySchmidFactors[3 * i + 2] = schmid3;
          }
          else if(n[0] > 0 && n[1] < 0)
          {
            //       plane = 2;
            n[0] = 1, n[1] = -1, n[2] = 1;
            cosPhi = fabs(GeometryMath::CosThetaBetweenVectors(crystalLoading, n));
            b[0] = 1, b[1] = 1, b[2] = 0;
            cosLambda = fabs(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid1 = cosPhi * cosLambda;
            b[0] = 0, b[1] = 1, b[2] = 1;
            cosLambda = fabs(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid2 = cosPhi * cosLambda;
            b[0] = -1, b[1] = 0, b[2] = 1;
            cosLambda = fabs(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid3 = cosPhi * cosLambda;
            m_TwinBoundarySchmidFactors[3 * i] = schmid1;
            m_TwinBoundarySchmidFactors[3 * i + 1] = schmid2;
            m_TwinBoundarySchmidFactors[3 * i + 2] = schmid3;
          }
          else if(n[0] < 0 && n[1] > 0)
          {
            //       plane = 3;
            n[0] = -1, n[1] = 1, n[2] = 1;
            cosPhi = fabs(GeometryMath::CosThetaBetweenVectors(crystalLoading, n));
            b[0] = 1, b[1] = 1, b[2] = 0;
            cosLambda = fabs(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid1 = cosPhi * cosLambda;
            b[0] = 1, b[1] = 0, b[2] = 1;
            cosLambda = fabs(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid2 = cosPhi * cosLambda;
            b[0] = 0, b[1] = -1, b[2] = 1;
            cosLambda = fabs(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid3 = cosPhi * cosLambda;
            m_TwinBoundarySchmidFactors[3 * i] = schmid1;
            m_TwinBoundarySchmidFactors[3 * i + 1] = schmid2;
            m_TwinBoundarySchmidFactors[3 * i + 2] = schmid3;
          }
          else if(n[0] < 0 && n[1] < 0)
          {
            //     plane = 4;
            n[0] = -1, n[1] = -1, n[2] = 1;
            cosPhi = fabs(GeometryMath::CosThetaBetweenVectors(crystalLoading, n));
            b[0] = 1, b[1] = 0, b[2] = 1;
            cosLambda = fabs(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid1 = cosPhi * cosLambda;
            b[0] = 0, b[1] = 1, b[2] = 1;
            cosLambda = fabs(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid2 = cosPhi * cosLambda;
            b[0] = 1, b[1] = -1, b[2] = 0;
            cosLambda = fabs(GeometryMath::CosThetaBetweenVectors(crystalLoading, b));
            schmid3 = cosPhi * cosLambda;
            m_TwinBoundarySchmidFactors[3 * i] = schmid1;
            m_TwinBoundarySchmidFactors[3 * i + 1] = schmid2;
            m_TwinBoundarySchmidFactors[3 * i + 2] = schmid3;
          }
        }
        else
        {
          m_TwinBoundarySchmidFactors[3 * i] = 0;
          m_TwinBoundarySchmidFactors[3 * i + 1] = 0;
          m_TwinBoundarySchmidFactors[3 * i + 2] = 0;
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
FindTwinBoundarySchmidFactors::FindTwinBoundarySchmidFactors()  :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_SurfaceDataContainerName(DREAM3D::Defaults::SurfaceDataContainerName),
  m_FaceAttributeMatrixName(DREAM3D::Defaults::FaceAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_TwinBoundarySchmidFactorsFile(""),
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
  m_SurfaceMeshTwinBoundaryArrayName(DREAM3D::FaceData::SurfaceMeshTwinBoundary),
  m_SurfaceMeshTwinBoundary(NULL),
  m_SurfaceMeshTwinBoundarySchmidFactorsArrayName(DREAM3D::FaceData::SurfaceMeshTwinBoundarySchmidFactors),
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
  FilterParameterVector parameters;
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
  setLoadingDir( reader->readFloatVec3("LoadingDir", getLoadingDir()));
  setTwinBoundarySchmidFactorsFile( reader->readString("TwinBoundarySchmidFactorsFile", getTwinBoundarySchmidFactorsFile()));
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
void FindTwinBoundarySchmidFactors::dataCheckVoxel()
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -302);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellEnsembleAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), -303);
  if(getErrorCondition() < 0) { return; }

  QVector<int> dims(1, 4);
  m_AvgQuatsPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_AvgQuatsArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_FeaturePhasesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeaturePhasesArrayName, -303, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = cellEnsembleAttrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(this, m_CrystalStructuresArrayName, -305, dims)
; /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundarySchmidFactors::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getSurfaceDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* faceAttrMat = sm->getPrereqAttributeMatrix<AbstractFilter>(this, getFaceAttributeMatrixName(), -302);
  if(getErrorCondition() < 0) { return; }

  QVector<int> dims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = faceAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayName, -386, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  m_SurfaceMeshFaceNormalsPtr = faceAttrMat->getPrereqArray<DataArray<double>, AbstractFilter>(this, m_SurfaceMeshFaceNormalsArrayName, -387, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceNormalsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_SurfaceMeshTwinBoundarySchmidFactorsPtr = faceAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_SurfaceMeshTwinBoundarySchmidFactorsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshTwinBoundarySchmidFactorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshTwinBoundarySchmidFactors = m_SurfaceMeshTwinBoundarySchmidFactorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_SurfaceMeshTwinBoundaryPtr = faceAttrMat->getPrereqArray<DataArray<bool>, AbstractFilter>(this, m_SurfaceMeshTwinBoundaryArrayName, -388, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshTwinBoundaryPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshTwinBoundary = m_SurfaceMeshTwinBoundaryPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
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
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
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

  int64_t numTriangles = sm->getAttributeMatrix(getFaceAttributeMatrixName())->getNumTuples();
  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  size_t totalEnsembles = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->getNumTuples();
  dataCheckVoxel(false, 0, totalFeatures, totalEnsembles);
  dataCheckSurfaceMesh(false, numTriangles, 0, 0);
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

  outFile << "Feature1	Feature2	IsTwin	Plane	Schmid1	Schmid2	Schmid3" << "\n";
  for (size_t i = 0; i < numTriangles; i++)
  {
    outFile << m_SurfaceMeshFaceLabels[2 * i] << "  " << m_SurfaceMeshFaceLabels[2 * i + 1] << "  " << m_SurfaceMeshTwinBoundary[i] << "  " << m_SurfaceMeshTwinBoundarySchmidFactors[3 * i] << "  " << m_SurfaceMeshTwinBoundarySchmidFactors[3 * i + 1] << "  " << m_SurfaceMeshTwinBoundarySchmidFactors[3 * i + 2] << "\n";
  }
  outFile.close();

  notifyStatusMessage("FindTwinBoundarySchmidFactors Completed");
}

