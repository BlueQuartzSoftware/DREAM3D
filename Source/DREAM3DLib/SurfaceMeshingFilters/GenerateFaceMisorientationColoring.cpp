/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "GenerateFaceMisorientationColoring.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/SurfaceMeshingFilters/util/Vector3.h"
#include "DREAM3DLib/SurfaceMeshingFilters/util/TriangleOps.h"

#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/CubicLowOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/TrigonalOps.h"
#include "DREAM3DLib/OrientationOps/TetragonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/OrientationOps/MonoclinicOps.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#endif



/**
 * @brief The CalculateNormalsImpl class is the actual code that does the computation and can be called either
 * from serial code or from Parallelized code (using TBB).
 */
class CalculateFaceMisorientationColorsImpl
{
    int32_t* m_Labels;
    int32_t* m_Phases;
    float* m_Quats;
    float* m_Colors;
    unsigned int* m_CrystalStructures;
    QVector<OrientationOps::Pointer> m_OrientationOps;

  public:
    CalculateFaceMisorientationColorsImpl(int32_t* labels, int32_t* phases, float* quats, float* colors, unsigned int* crystalStructures) :
      m_Labels(labels),
      m_Phases(phases),
      m_Quats(quats),
      m_Colors(colors),
      m_CrystalStructures(crystalStructures)
    {
      m_OrientationOps = OrientationOps::getOrientationOpsVector();
    }
    virtual ~CalculateFaceMisorientationColorsImpl() {}

    /**
     * @brief generate Generates the Normals for the triangles
     * @param start The starting FaceArray::Face_t Index
     * @param end The ending FaceArray::Face_t Index
     */
    void generate(size_t start, size_t end) const
    {
      int feature1, feature2, phase1, phase2;
      QuatF q1;
      QuatF q2;
      QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

      float w, n1, n2 , n3;
      float radToDeg = 180.0 / DREAM3D::Constants::k_Pi;
      for (size_t i = start; i < end; i++)
      {
        feature1 = m_Labels[2 * i];
        feature2 = m_Labels[2 * i + 1];
        if(feature1 > 0) { phase1 = m_Phases[feature1]; }
        else { phase1 = 0; }
        if(feature2 > 0) { phase2 = m_Phases[feature2]; }
        else { phase2 = 0; }
        if(phase1 > 0)
        {
          if(phase1 == phase2 && m_CrystalStructures[phase1] == Ebsd::CrystalStructure::Cubic_High)
          {
            QuaternionMathF::Copy(quats[feature1], q1);

            QuaternionMathF::Copy(quats[feature2], q2);
            w = m_OrientationOps[m_CrystalStructures[phase1]]->getMisoQuat(q1, q2, n1, n2, n3);
            w = w * radToDeg;
            m_Colors[3 * i + 0] = w * n1;
            m_Colors[3 * i + 1] = w * n2;
            m_Colors[3 * i + 2] = w * n3;
          }
          else if(phase1 == phase2 && m_CrystalStructures[phase1] == Ebsd::CrystalStructure::Hexagonal_High)
          {
            QuaternionMathF::Copy(quats[feature1], q1);
            QuaternionMathF::Copy(quats[feature2], q2);
            w = m_OrientationOps[m_CrystalStructures[phase1]]->getMisoQuat(q1, q2, n1, n2, n3);
            w = w * radToDeg;
            m_Colors[3 * i + 0] = w * n1;
            m_Colors[3 * i + 1] = w * n2;
            m_Colors[3 * i + 2] = w * n3;
          }
        }
        else
        {
          m_Colors[3 * i + 0] = 0;
          m_Colors[3 * i + 1] = 0;
          m_Colors[3 * i + 2] = 0;
        }
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    /**
     * @brief operator () This is called from the TBB stye of code
     * @param r The range to compute the values
     */
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      generate(r.begin(), r.end());
    }
#endif


};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateFaceMisorientationColoring::GenerateFaceMisorientationColoring() :
  SurfaceMeshFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_SurfaceDataContainerName(DREAM3D::HDF5::SurfaceDataContainerName),
  m_FaceAttributeMatrixName(DREAM3D::HDF5::FaceAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::HDF5::CellFeatureAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::HDF5::CellFeatureAttributeMatrixName),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceMisorientationColorsArrayName(DREAM3D::FaceData::SurfaceMeshFaceMisorientationColors),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFaceMisorientationColors(NULL),
  m_AvgQuats(NULL),
  m_FeaturePhases(NULL),
  m_CrystalStructures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateFaceMisorientationColoring::~GenerateFaceMisorientationColoring()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceMisorientationColoring::setupFilterParameters()
{
  FilterParameterVector parameters;
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceMisorientationColoring::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenerateFaceMisorientationColoring::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceMisorientationColoring::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());

  // We MUST have Nodes
  if(sm->getVertices().get() == NULL)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
    setErrorCondition(-384);
  }

  // We MUST have Triangles defined also.
  if(sm->getFaces().get() == NULL)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -385);
    setErrorCondition(-385);
  }
  else
  {
    QVector<int> dims(1, 2);
    m_SurfaceMeshFaceLabelsPtr = sm->getPrereqArray<int32_t, AbstractFilter>(this, m_FaceAttributeMatrixName,  m_SurfaceMeshFaceLabelsArrayName, -386, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dims[0] = 3;
    m_SurfaceMeshFaceMisorientationColorsPtr = sm->createNonPrereqArray<float, AbstractFilter>(this, m_FaceAttributeMatrixName,  m_SurfaceMeshFaceMisorientationColorsArrayName, 0, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceMisorientationColorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceMisorientationColors = m_SurfaceMeshFaceMisorientationColorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceMisorientationColoring::dataCheckVoxel(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 4);
  m_AvgQuatsPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_AvgQuatsArrayName, -301, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_FeaturePhasesPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_FeaturePhasesArrayName, -302,  features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = m->getPrereqArray<unsigned int, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_CrystalStructuresArrayName, -304, ensembles, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceMisorientationColoring::preflight()
{
  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  if(NULL == sm)
  {
    QString ss = QObject::tr("The Surface Data Container with name '%1'' was not found in the Data Container Array.").arg(getSurfaceDataContainerName());
    setErrorCondition(-10000);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }


  dataCheckSurfaceMesh(true, 1, 1, 1);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    QString ss = QObject::tr("The Volume Data Container with name '%1'' was not found in the Data Container Array.").arg(getDataContainerName());
    setErrorCondition(-10000);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  dataCheckVoxel(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceMisorientationColoring::execute()
{
  int err = 0;

  setErrorCondition(err);
  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMesh DataContainer Object was NULL", -999);
    return;
  }
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Voxel DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  notifyStatusMessage("Starting");

  // Run the data check to allocate the memory for the centroid array
  int64_t numTriangles = sm->getAttributeMatrix(getFaceAttributeMatrixName())->getNumTuples();
  dataCheckSurfaceMesh(false, numTriangles, 0, 0);
  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  size_t totalEnsembles = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->getNumTuples();
  dataCheckVoxel(false, 0, totalFeatures, totalEnsembles);

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  bool doParallel = true;
#endif


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numTriangles),
                      CalculateFaceMisorientationColorsImpl(m_SurfaceMeshFaceLabels, m_FeaturePhases, m_AvgQuats, m_SurfaceMeshFaceMisorientationColors, m_CrystalStructures), tbb::auto_partitioner());
  }
  else
#endif
  {
    CalculateFaceMisorientationColorsImpl serial(m_SurfaceMeshFaceLabels, m_FeaturePhases, m_AvgQuats, m_SurfaceMeshFaceMisorientationColors, m_CrystalStructures);
    serial.generate(0, numTriangles);
  }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
