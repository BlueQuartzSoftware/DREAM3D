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
#include "GenerateTriangleIPFColoring.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/MatrixMath.h"
#include "DREAM3DLib/SurfaceMeshingFilters/util/Vector3.h"
#include "DREAM3DLib/SurfaceMeshingFilters/util/TriangleOps.h"

#include "DREAM3DLib/Common/EbsdColoring.hpp"
#include "DREAM3DLib/Common/MatrixMath.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#endif


/**
 * @brief The CalculateNormalsImpl class is the actual code that does the computation and can be called either
 * from serial code or from Parallelized code (using TBB).
 */
class CalculateTriangleIPFColorsImpl
{
    int32_t* m_Labels;
    int32_t* m_Phases;
    double* m_Normals;
    float* m_Eulers;
    uint8_t* m_Colors;
    uint8_t* m_ColorsG1;
    uint8_t* m_ColorsG2;
    unsigned int* m_CrystalStructures;

  public:
    CalculateTriangleIPFColorsImpl(int32_t* labels, int32_t* phases, double* normals, float* eulers, uint8_t* colors, uint8_t* colorsG1, uint8_t* colorsG2, unsigned int* crystalStructures) :
      m_Labels(labels),
      m_Phases(phases),
      m_Normals(normals),
      m_Eulers(eulers),
      m_Colors(colors),
      m_ColorsG1(colorsG1),
      m_ColorsG2(colorsG2),
      m_CrystalStructures(crystalStructures)
    {}
    virtual ~CalculateTriangleIPFColorsImpl(){}

    /**
     * @brief generate Generates the Normals for the triangles
     * @param start The starting SurfaceMesh::DataStructures::Face_t Index
     * @param end The ending SurfaceMesh::DataStructures::Face_t Index
     */
    void generate(size_t start, size_t end) const
    {
    int grain1, grain2, phase1, phase2;
    uint8_t hkl[3] = { 0, 0, 0 };
      for (size_t i = start; i < end; i++)
      {
        grain1 = m_Labels[2*i];
        grain2 = m_Labels[2*i+1];
      if(grain1 > 0) phase1 = m_Phases[grain1];
      else phase1 = 0;
      if(grain2 > 0) phase2 = m_Phases[grain2];
      else phase2 = 0;
      if(phase1 > 0)
      {
        if(m_CrystalStructures[phase1] == Ebsd::CrystalStructure::Cubic)
        {
        EbsdColoring::GenerateCubicIPFColor(m_Eulers[3*grain1+0], m_Eulers[3*grain1+1], m_Eulers[3*grain1+2], m_Normals[3*i+0], m_Normals[3*i+1], m_Normals[3*i+2], m_Colors + (6*i), hkl);
        m_ColorsG1[3*i+0] = m_Colors[6*i+0];
        m_ColorsG1[3*i+1] = m_Colors[6*i+1];
        m_ColorsG1[3*i+2] = m_Colors[6*i+2];
        }
        else if(m_CrystalStructures[phase1] == Ebsd::CrystalStructure::Hexagonal)
        {
        EbsdColoring::GenerateHexIPFColor(m_Eulers[3*grain1+0], m_Eulers[3*grain1+1], m_Eulers[3*grain1+2], m_Normals[3*i+0], m_Normals[3*i+1], m_Normals[3*i+2], m_Colors + (6*i));
        m_ColorsG1[3*i+0] = m_Colors[6*i+0];
        m_ColorsG1[3*i+1] = m_Colors[6*i+1];
        m_ColorsG1[3*i+2] = m_Colors[6*i+2];
        }
      }
      else
      {
        m_Colors[6*i+0] = 0;
        m_Colors[6*i+1] = 0;
        m_Colors[6*i+2] = 0;
        m_ColorsG1[3*i+0] = 0;
        m_ColorsG1[3*i+1] = 0;
        m_ColorsG1[3*i+2] = 0;
      }
      if(phase2 > 0)
      {
        if(m_CrystalStructures[phase2] == Ebsd::CrystalStructure::Cubic)
        {
        EbsdColoring::GenerateCubicIPFColor(m_Eulers[3*grain2+0], m_Eulers[3*grain2+1], m_Eulers[3*grain2+2], -m_Normals[3*i+0], -m_Normals[3*i+1], -m_Normals[3*i+2], m_Colors + (6*i+3), hkl);
        m_ColorsG2[3*i+0] = m_Colors[6*i+3];
        m_ColorsG2[3*i+1] = m_Colors[6*i+4];
        m_ColorsG2[3*i+2] = m_Colors[6*i+5];
        }
        else if(m_CrystalStructures[phase2] == Ebsd::CrystalStructure::Hexagonal)
        {
        EbsdColoring::GenerateHexIPFColor(m_Eulers[3*grain2+0], m_Eulers[3*grain2+1], m_Eulers[3*grain2+2], -m_Normals[3*i+0], -m_Normals[3*i+1], -m_Normals[3*i+2], m_Colors + (6*i+3));
        m_ColorsG2[3*i+0] = m_Colors[6*i+3];
        m_ColorsG2[3*i+1] = m_Colors[6*i+4];
        m_ColorsG2[3*i+2] = m_Colors[6*i+5];
        }
      }
      else
      {
        m_Colors[6*i+3] = 0;
        m_Colors[6*i+4] = 0;
        m_Colors[6*i+5] = 0;
        m_ColorsG2[3*i+0] = 0;
        m_ColorsG2[3*i+1] = 0;
        m_ColorsG2[3*i+2] = 0;
      }
    }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    /**
     * @brief operator () This is called from the TBB stye of code
     * @param r The range to compute the values
     */
    void operator()(const tbb::blocked_range<size_t> &r) const
    {
      generate(r.begin(), r.end());
    }
#endif


};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateTriangleIPFColoring::GenerateTriangleIPFColoring() :
SurfaceMeshFilter(),
m_FieldEulerAnglesArrayName(DREAM3D::FieldData::EulerAngles),
m_SurfaceMeshTriangleNormalsArrayName(DREAM3D::FaceData::SurfaceMeshTriangleNormals),
m_SurfaceMeshTriangleLabelsArrayName(DREAM3D::FaceData::SurfaceMeshTriangleLabels),
m_SurfaceMeshTriangleIPFColorsArrayName(DREAM3D::FaceData::SurfaceMeshTriangleIPFColors),
m_SurfaceMeshTriangleIPFColorsGrain1ArrayName(DREAM3D::FaceData::SurfaceMeshTriangleIPFColorsGrain1),
m_SurfaceMeshTriangleIPFColorsGrain2ArrayName(DREAM3D::FaceData::SurfaceMeshTriangleIPFColorsGrain2),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
m_SurfaceMeshTriangleLabels(NULL),
m_SurfaceMeshTriangleNormals(NULL),
m_FieldEulerAngles(NULL),
m_SurfaceMeshTriangleIPFColors(NULL),
m_SurfaceMeshTriangleIPFColorsGrain1(NULL),
m_SurfaceMeshTriangleIPFColorsGrain2(NULL),
m_FieldPhases(NULL),
m_CrystalStructures(NULL)

{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateTriangleIPFColoring::~GenerateTriangleIPFColoring()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateTriangleIPFColoring::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateTriangleIPFColoring::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateTriangleIPFColoring::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-383);
  }
  else
  {
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
      GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshTriangleLabels, ss, -386, int32_t, Int32ArrayType, fields, 2)
      GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshTriangleNormals, ss, -387, double, DoubleArrayType, fields, 3)
      CREATE_NON_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshTriangleIPFColors, ss, uint8_t, UInt8ArrayType, 0, fields, 6)
      CREATE_NON_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshTriangleIPFColorsGrain1, ss, uint8_t, UInt8ArrayType, 0, fields, 3)
      CREATE_NON_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshTriangleIPFColorsGrain2, ss, uint8_t, UInt8ArrayType, 0, fields, 3)
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateTriangleIPFColoring::dataCheckVoxel(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    addErrorMessage(getHumanLabel(), "VoxelDataContainer is missing", -383);
    setErrorCondition(-383);
  }
  else
  {
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldEulerAngles, ss, -301, float, FloatArrayType, fields, 3)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -302, int32_t, Int32ArrayType,  fields, 1)
  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1)
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateTriangleIPFColoring::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheckSurfaceMesh(true, 1, 1, 1);
  dataCheckVoxel(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateTriangleIPFColoring::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  VoxelDataContainer* m = getVoxelDataContainer();
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
  dataCheckSurfaceMesh(false, 0, sm->getNumFaceTuples(), 0);
  dataCheckVoxel(false, m->getNumCellTuples(), m->getNumFieldTuples(), m->getNumEnsembleTuples());

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::parallel_for(tbb::blocked_range<size_t>(0, sm->getNumFaceTuples()),
                    CalculateTriangleIPFColorsImpl(m_SurfaceMeshTriangleLabels, m_FieldPhases, m_SurfaceMeshTriangleNormals, m_FieldEulerAngles, m_SurfaceMeshTriangleIPFColors, m_SurfaceMeshTriangleIPFColorsGrain1, m_SurfaceMeshTriangleIPFColorsGrain2, m_CrystalStructures), tbb::auto_partitioner());

#else
  CalculateTriangleIPFColorsImpl serial(m_SurfaceMeshTriangleLabels, m_FieldPhases, m_SurfaceMeshTriangleNormals, m_FieldEulerAngles, m_SurfaceMeshTriangleIPFColors, m_SurfaceMeshTriangleIPFColorsGrain1, m_SurfaceMeshTriangleIPFColorsGrain2, m_CrystalStructures);
  serial.generate(0, sm->getNumFaceTuples());
#endif

  DataArray<int32_t>::Pointer faceLabel1Ptr = DataArray<int32_t>::CreateArray(sm->getNumFaceTuples(), 1, "SurfaceMeshTriangleLabelsG1");
  int32_t* faceLabels1 = faceLabel1Ptr->GetPointer(0);
  DataArray<int32_t>::Pointer faceLabel2Ptr = DataArray<int32_t>::CreateArray(sm->getNumFaceTuples(), 1, "SurfaceMeshTriangleLabelsG2");
  int32_t* faceLabels2 = faceLabel2Ptr->GetPointer(0);

  for(size_t i=0;i<sm->getNumFaceTuples();i++)
  {
  faceLabels1[i] = m_SurfaceMeshTriangleLabels[2*i];
  faceLabels2[i] = m_SurfaceMeshTriangleLabels[2*i+1];
  }

  sm->addFaceData(faceLabel1Ptr->GetName(), faceLabel1Ptr);
  sm->addFaceData(faceLabel2Ptr->GetName(), faceLabel2Ptr);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
