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
#include "GenerateFaceIPFColoring.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/SurfaceMeshingFilters/util/Vector3.h"
#include "DREAM3DLib/SurfaceMeshingFilters/util/TriangleOps.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/CubicLowOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalLowOps.h"
#include "DREAM3DLib/OrientationOps/TrigonalOps.h"
#include "DREAM3DLib/OrientationOps/TrigonalLowOps.h"
#include "DREAM3DLib/OrientationOps/TetragonalOps.h"
#include "DREAM3DLib/OrientationOps/TetragonalLowOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/OrientationOps/MonoclinicOps.h"
#include "DREAM3DLib/OrientationOps/TriclinicOps.h"
#include "DREAM3DLib/Utilities/ColorTable.h"


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#endif


/**
 * @brief The CalculateNormalsImpl class is the actual code that does the computation and can be called either
 * from serial code or from Parallelized code (using TBB).
 */
class CalculateFaceIPFColorsImpl
{
    int32_t* m_Labels;
    int32_t* m_Phases;
    double* m_Normals;
    float* m_Eulers;
    uint8_t* m_Colors;
    unsigned int* m_CrystalStructures;

  public:
    CalculateFaceIPFColorsImpl(int32_t* labels, int32_t* phases, double* normals, float* eulers, uint8_t* colors, unsigned int* crystalStructures) :
      m_Labels(labels),
      m_Phases(phases),
      m_Normals(normals),
      m_Eulers(eulers),
      m_Colors(colors),
      m_CrystalStructures(crystalStructures)
    {}
    virtual ~CalculateFaceIPFColorsImpl() {}

    /**
     * @brief generate Generates the Normals for the triangles
     * @param start The starting FaceArray::Face_t Index
     * @param end The ending FaceArray::Face_t Index
     */
    void generate(size_t start, size_t end) const
    {

      // Create 1 of every type of Ops class. This condenses the code below
      QVector<OrientationOps::Pointer> ops;
      ops.push_back(HexagonalOps::New());
      ops.push_back(CubicOps::New());
      ops.push_back(HexagonalLowOps::New());
      ops.push_back(CubicLowOps::New());
      ops.push_back(TriclinicOps::New());
      ops.push_back(MonoclinicOps::New());
      ops.push_back(OrthoRhombicOps::New());
      ops.push_back(TetragonalLowOps::New());
      ops.push_back(TetragonalOps::New());
      ops.push_back(TrigonalLowOps::New());
      ops.push_back(TrigonalOps::New());

      double refDir[3] = {0.0, 0.0, 0.0};
      double dEuler[3] = {0.0, 0.0, 0.0};
      DREAM3D::Rgb argb = 0x00000000;

      int feature1, feature2, phase1, phase2;
      for (size_t i = start; i < end; i++)
      {
        feature1 = m_Labels[2 * i];
        feature2 = m_Labels[2 * i + 1];
        if(feature1 > 0) { phase1 = m_Phases[feature1]; }
        else { phase1 = 0; }

        if(feature2 > 0) { phase2 = m_Phases[feature2]; }
        else { phase2 = 0; }

        if(phase1 > 0 )
        {
          // Make sure we are using a valid Euler Angles with valid crystal symmetry
          if( m_CrystalStructures[phase1] < Ebsd::CrystalStructure::LaueGroupEnd )
          {
            dEuler[0] = m_Eulers[3 * feature1 + 0];
            dEuler[1] = m_Eulers[3 * feature1 + 1];
            dEuler[2] = m_Eulers[3 * feature1 + 2];
            refDir[0] = m_Normals[3 * i + 0];
            refDir[1] = m_Normals[3 * i + 1];
            refDir[2] = m_Normals[3 * i + 2];

            argb = ops[m_CrystalStructures[phase1]]->generateIPFColor(dEuler, refDir, false);
            m_Colors[6 * i] = RgbColor::dRed(argb);
            m_Colors[6 * i + 1] = RgbColor::dGreen(argb);
            m_Colors[6 * i + 2] = RgbColor::dBlue(argb);
          }
        }
        else // Phase 1 was Zero so assign a black color
        {
          m_Colors[6 * i + 0] = 0;
          m_Colors[6 * i + 1] = 0;
          m_Colors[6 * i + 2] = 0;
        }


        // Now compute for Phase 2
        if(phase2 > 0)
        {
          // Make sure we are using a valid Euler Angles with valid crystal symmetry
          if( m_CrystalStructures[phase1] < Ebsd::CrystalStructure::LaueGroupEnd )
          {
            dEuler[0] = m_Eulers[3 * feature2 + 0];
            dEuler[1] = m_Eulers[3 * feature2 + 1];
            dEuler[2] = m_Eulers[3 * feature2 + 2];
            refDir[0] = -m_Normals[3 * i + 0];
            refDir[1] = -m_Normals[3 * i + 1];
            refDir[2] = -m_Normals[3 * i + 2];

            argb = ops[m_CrystalStructures[phase1]]->generateIPFColor(dEuler, refDir, false);
            m_Colors[6 * i + 3] = RgbColor::dRed(argb);
            m_Colors[6 * i + 4] = RgbColor::dGreen(argb);
            m_Colors[6 * i + 5] = RgbColor::dBlue(argb);
          }
        }
        else
        {
          m_Colors[6 * i + 3] = 0;
          m_Colors[6 * i + 4] = 0;
          m_Colors[6 * i + 5] = 0;
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
GenerateFaceIPFColoring::GenerateFaceIPFColoring() :
  SurfaceMeshFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_FaceAttributeMatrixName(DREAM3D::Defaults::FaceAttributeMatrixName),
  m_SurfaceDataContainerName(DREAM3D::Defaults::SurfaceDataContainerName),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFaceNormalsArrayName(DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshFaceNormals(NULL),
  m_SurfaceMeshFaceIPFColorsArrayName(DREAM3D::FaceData::SurfaceMeshFaceIPFColors),
  m_SurfaceMeshFaceIPFColors(NULL),
  m_FeatureEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_FeatureEulerAngles(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateFaceIPFColoring::~GenerateFaceIPFColoring()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceIPFColoring::setupFilterParameters()
{
  FilterParameterVector parameters;
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceIPFColoring::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int GenerateFaceIPFColoring::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceIPFColoring::dataCheckSurfaceMesh()
{
  setErrorCondition(0);

  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getSurfaceDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* faceAttrMat = sm->getPrereqAttributeMatrix<AbstractFilter>(this, getFaceAttributeMatrixName(), -302);
  if(getErrorCondition() < 0) { return; }

  // We MUST have Nodes
  if(sm->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    PipelineMessage em (getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
  }

  // We MUST have Triangles defined also.
  if(sm->getFaces().get() == NULL)
  {
    setErrorCondition(-385);
    PipelineMessage em (getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
  }
  else
  {
    QVector<int> dims(1, 2);
    m_SurfaceMeshFaceLabelsPtr = faceAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayName, -386, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dims[0] = 3;
    m_SurfaceMeshFaceNormalsPtr = faceAttrMat->getPrereqArray<DataArray<double>, AbstractFilter>(this, m_SurfaceMeshFaceNormalsArrayName, -387, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceNormalsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dims[0] = 6;
    m_SurfaceMeshFaceIPFColorsPtr = faceAttrMat->createNonPrereqArray<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, m_SurfaceMeshFaceIPFColorsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceIPFColorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceIPFColors = m_SurfaceMeshFaceIPFColorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceIPFColoring::dataCheckVoxel()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -302);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellEnsembleAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), -303);
  if(getErrorCondition() < 0) { return; }

  QVector<int> dims(1, 3);
  m_FeatureEulerAnglesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_FeatureEulerAnglesArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_FeaturePhasesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeaturePhasesArrayName, -302, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = cellEnsembleAttrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(this, m_CrystalStructuresArrayName, -304, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceIPFColoring::preflight()
{
  dataCheckSurfaceMesh();
  dataCheckVoxel();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceIPFColoring::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheckSurfaceMesh();
  if(getErrorCondition() < 0) { return; }

  dataCheckVoxel();
  if(getErrorCondition() < 0) { return; }


  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  emit filterGeneratedMessage(PipelineMessage::CreateStatusMessage(getHumanLabel(), "Starting") );

  // Run the data check to allocate the memory for the centroid array
  int64_t numTriangles = sm->getAttributeMatrix(getFaceAttributeMatrixName())->getNumTuples();

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  bool doParallel = true;
#endif

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numTriangles),
                      CalculateFaceIPFColorsImpl(m_SurfaceMeshFaceLabels, m_FeaturePhases, m_SurfaceMeshFaceNormals, m_FeatureEulerAngles, m_SurfaceMeshFaceIPFColors, m_CrystalStructures), tbb::auto_partitioner());
  }
  else
#endif
  {
    CalculateFaceIPFColorsImpl serial(m_SurfaceMeshFaceLabels, m_FeaturePhases, m_SurfaceMeshFaceNormals, m_FeatureEulerAngles, m_SurfaceMeshFaceIPFColors, m_CrystalStructures);
    serial.generate(0, numTriangles);
  }

  /* Let the GUI know we are done with this filter */
  emit filterGeneratedMessage(PipelineMessage::CreateStatusMessage(getHumanLabel(), "Complete") );
}

