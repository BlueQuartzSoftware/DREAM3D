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
#include "FindGBCD.h"


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Common/MatrixMath.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"

const static float m_pi = static_cast<float>(M_PI);
const static float m_pi2 = static_cast<float>(2*M_PI);


/**
 * @brief The CalculateAreasImpl class
 */
class CalculateGBCDImpl
{
    DREAM3D::SurfaceMesh::VertListPointer_t m_Nodes;
    DREAM3D::SurfaceMesh::FaceListPointer_t m_Triangles;
    int32_t* m_Labels;
    double* m_Normals;
    double* m_Areas;
    int32_t* m_Phases;
    float* m_Quats;
    float* m_GBCDarea;
    float* m_GBCDcount;
    float* m_GBCDdeltas;
    int* m_GBCDsizes;
    float* m_GBCDlimits;
    unsigned int* m_CrystalStructures;
    std::vector<OrientationMath*> m_OrientationOps;
    CubicOps::Pointer m_CubicOps;
    HexagonalOps::Pointer m_HexOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

  public:
    CalculateGBCDImpl(DREAM3D::SurfaceMesh::VertListPointer_t nodes, DREAM3D::SurfaceMesh::FaceListPointer_t triangles,
                      int32_t* Labels, double* Normals, double* Areas, float* Quats,
                      int32_t* Phases, unsigned int* CrystalStructures, float* GBCDarea,
                      float* GBCDcount, float* GBCDdeltas, int* GBCDsizes, float* GBCDlimits) :
      m_Nodes(nodes),
      m_Triangles(triangles),
      m_Labels(Labels),
      m_Normals(Normals),
      m_Areas(Areas),
      m_Phases(Phases),
      m_Quats(Quats),
      m_GBCDarea(GBCDarea),
      m_GBCDcount(GBCDcount),
      m_GBCDdeltas(GBCDdeltas),
      m_GBCDsizes(GBCDsizes),
      m_GBCDlimits(GBCDlimits),
      m_CrystalStructures(CrystalStructures)
    {
      m_HexOps = HexagonalOps::New();
      m_OrientationOps.push_back(dynamic_cast<OrientationMath*>(m_HexOps.get()));

      m_CubicOps = CubicOps::New();
      m_OrientationOps.push_back(dynamic_cast<OrientationMath*>(m_CubicOps.get()));

      m_OrthoOps = OrthoRhombicOps::New();
      m_OrientationOps.push_back(dynamic_cast<OrientationMath*>(m_OrthoOps.get()));
    }
    virtual ~CalculateGBCDImpl(){}

    void generate(size_t start, size_t end) const
    {

    //  DREAM3D::SurfaceMesh::Vert_t* nodes = m_Nodes->GetPointer(0);
    //  DREAM3D::SurfaceMesh::Face_t* triangles = m_Triangles->GetPointer(0);

      int j;//, j4;
      int k;//, k4;
      int m;
      int gbcd_index;
      int inversion = 1;
      int grain1, grain2;
      float q1[5], q2[5], misq[5], sym_q[5], s1misq[5], s2misq[5], euler_mis[3];
      float /*sym1[5],*/ sym2[5];
      float normal[3], xstl_norm0[3], xstl_norm1[3], xstl_norm_sc[2], xstl_norm_sc_inv[2];

      for (size_t i = start; i < end; i++)
      {
        grain1 = m_Labels[2*i];
        grain2 = m_Labels[2*i+1];
        for(m=1; m < 4; m++)
        {
          q1[m] = m_Quats[5*grain1+m];
          q2[m] = m_Quats[5*grain2+m];
          q2[m] *= -1.0; //inverse q2
        }
        //get the misorientation between grain1 and grain2
        OrientationMath::multiplyQuaternions(q1, q2, misq);
        OrientationMath::multiplyQuaternionVector(q1, normal, xstl_norm0);

        int nsym = m_OrientationOps[m_CrystalStructures[m_Phases[grain1]]]->getNumSymOps();
        for (j=0; j< nsym;j++)
        {
          //calculate the symmetric misorienation
          m_OrientationOps[m_CrystalStructures[m_Phases[grain1]]]->getQuatSymOp(j, sym_q);
          OrientationMath::multiplyQuaternions(sym_q, misq, s1misq);

          //calculate the crystal normal and put it into normalized coordinates ->[theta, cos(phi) ]
          OrientationMath::multiplyQuaternionVector(sym_q, xstl_norm0, xstl_norm1);
          xstl_norm_sc[0] = atan2f(xstl_norm1[1], xstl_norm1[0]);
          if (xstl_norm_sc[0] < 0) xstl_norm_sc[0] += m_pi2;
          xstl_norm_sc[1] = xstl_norm1[2];

          if (inversion == 1){
            xstl_norm_sc_inv[0] = xstl_norm_sc[0] + m_pi;
            if (xstl_norm_sc_inv[0] > m_pi2) xstl_norm_sc[0] -= m_pi2;
            xstl_norm_sc_inv[1] = -1.0*xstl_norm_sc[1];
          }

          for (k=0; k < nsym; k++)
          {
            for(m=1; m < 4; m++)
            {
              sym2[m] = -1.*sym_q[m]; //invert the symmetry operator
            }
            OrientationMath::multiplyQuaternions(s1misq, sym2, s2misq);
            OrientationMath::QuattoEuler(s2misq, euler_mis[0], euler_mis[1], euler_mis[2]);
            euler_mis[1] = cosf(euler_mis[1]);

            //get the index that this point would be in the GBCD histogram
            gbcd_index = GBCDIndex (m_GBCDdeltas, m_GBCDsizes, m_GBCDlimits, euler_mis, xstl_norm_sc);

            if (gbcd_index != -1)
            {
              // Add the points and up the count on the gbcd histograms.  Broke this out
              // so that it could be protected in an openMP thread
              {
                m_GBCDcount[gbcd_index] += 1;
                m_GBCDarea[gbcd_index] += m_Areas[i];
              }
            }
            //if inversion is on, do the same for that
            if (inversion == 1)
            {
              gbcd_index = GBCDIndex (m_GBCDdeltas, m_GBCDsizes, m_GBCDlimits, euler_mis, xstl_norm_sc_inv);
              if (gbcd_index != -1 )
              {
                {
                  m_GBCDcount[gbcd_index] += 1;
                  m_GBCDarea[gbcd_index] += m_Areas[i];
                }
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

    int GBCDIndex (float* gbcddelta, int* gbcdsz, float* gbcdlimits, float* eulerN, float* xstl_norm_sc) const
    {
      int gbcd_index;
      int i, index[5];
      int n1, n1n2, n1n2n3, n1n2n3n4;
      int flag_good = 1;
      float mis_eulerNorm[5];

      //concatonate the normalized euler angles and normalized spherical corrdinate normal
      for (i=0; i<3; i++) mis_eulerNorm[i] = eulerN[i];
      for (i=0; i<2; i++) mis_eulerNorm[i+3] = xstl_norm_sc[i];
      //Check for a valid point in the GBCD space
      for (i=0; i<5; i++){
        if (mis_eulerNorm[i] < gbcdlimits[i]) flag_good = 0;
        if (mis_eulerNorm[i] > gbcdlimits[i+5]) flag_good = 0;
      }

      if (flag_good == 0) return -1; //does not fit in the gbcd space

      n1 =  gbcdsz[0];
      n1n2 = n1*( gbcdsz[1]);
      n1n2n3 = n1n2*( gbcdsz[2]);
      n1n2n3n4 = n1n2n3*( gbcdsz[3]);

      //determine the bin that the point should go into.
      for (i=0;i < 5; i++){
        index[i] = (int) floorf((mis_eulerNorm[i] - gbcdlimits[i])/gbcddelta[i]);
        //printf("%1i ",index[j]);
        if (index[i] >  (gbcdsz[i]-1)) {
          index[i] =  (gbcdsz[i]-1);
        }
        if (index[i] <  0) {
          index[i] =  0;
        }
      }
      gbcd_index = index[0]+n1*index[1]+
          n1n2*index[2]+n1n2n3*index[3] +
          n1n2n3n4*index[4];

      return gbcd_index;
    }
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindGBCD::FindGBCD() :
  SurfaceMeshFilter(),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceAreasArrayName(DREAM3D::FaceData::SurfaceMeshFaceAreas),
  m_SurfaceMeshFaceNormalsArrayName(DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_SurfaceMeshFaceAreas(NULL),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFaceNormals(NULL),
  m_AvgQuats(NULL),
  m_FieldPhases(NULL),
  m_CrystalStructures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindGBCD::~FindGBCD()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -383);
      setErrorCondition(-384);
    }
    else
    {
      GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshFaceLabels, ss, -386, int32_t, Int32ArrayType, fields, 2)
          GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshFaceNormals, ss, -387, double, DoubleArrayType, fields, 3)
          GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshFaceAreas, ss, -388, double, DoubleArrayType, fields, 1)
    }

  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::dataCheckVoxel(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
    GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 5)
        GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -302, int32_t, Int32ArrayType,  fields, 1)
        typedef DataArray<unsigned int> XTalStructArrayType;
    GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1)
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheckSurfaceMesh(true, 1, 1, 1);
  dataCheckVoxel(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMeshing DataContainer Object was NULL", -999);
    return;
  }
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMeshing DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  notifyStatusMessage("Starting");

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = sm->getVertices();

  DREAM3D::SurfaceMesh::FaceListPointer_t trianglesPtr = sm->getFaces();
  size_t totalFaces = trianglesPtr->GetNumberOfTuples();

  // Run the data check to allocate the memory for the centroid array
  dataCheckSurfaceMesh(false, 0, totalFaces, 0);

  size_t totalFields = m->getNumFieldTuples();
  size_t totalEnsembles = m->getNumEnsembleTuples();

  dataCheckVoxel(false, 0, totalFields, totalEnsembles);

<<<<<<< HEAD
  FloatArrayType::Pointer gbcdArray = FloatArrayType::NullPointer();
  FloatArrayType::Pointer gbcdCountArray = FloatArrayType::NullPointer();
  FloatArrayType::Pointer gbcdDeltasArray = FloatArrayType::NullPointer();
  FloatArrayType::Pointer gbcdLimitsArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer gbcdSizesArray = Int32ArrayType::NullPointer();
  gbcdArray = FloatArrayType::CreateArray(36*18*36*36*18*36, "GBCD");
  gbcdArray->SetNumberOfComponents(1);
  gbcdArray->initializeWithZeros();
  gbcdCountArray = FloatArrayType::CreateArray(36*18*36*36*18*36, "GBCDCount");
  gbcdCountArray->SetNumberOfComponents(1);
  gbcdCountArray->initializeWithZeros();
  gbcdDeltasArray = FloatArrayType::CreateArray(5, "GBCDDeltas");
  gbcdDeltasArray->SetNumberOfComponents(1);
  gbcdDeltasArray->initializeWithZeros();
  gbcdLimitsArray = FloatArrayType::CreateArray(10, "GBCDLimits");
  gbcdLimitsArray->SetNumberOfComponents(1);
  gbcdLimitsArray->initializeWithZeros();
  gbcdSizesArray = Int32ArrayType::CreateArray(5, "GBCDSizes");
  gbcdSizesArray->SetNumberOfComponents(1);
  gbcdSizesArray->initializeWithZeros();
  float* m_GBCD = gbcdArray->GetPointer(0);
  float* m_GBCDcount = gbcdCountArray->GetPointer(0);
  float* m_GBCDdeltas = gbcdDeltasArray->GetPointer(0);
  int* m_GBCDsizes = gbcdSizesArray->GetPointer(0);
  float* m_GBCDlimits = gbcdLimitsArray->GetPointer(0);
=======
  float* m_GBCD = NULL;
  float* m_GBCDcount = NULL;
  float* m_GBCDdeltas = NULL;
  int* m_GBCDsizes = NULL;
  float* m_GBCDlimits = NULL;
>>>>>>> c89d7fc69eb7a3413fc19dd13ac5cc3648c5e9d4

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalFaces),
                      CalculateGBCDImpl(nodesPtr, trianglesPtr, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_SurfaceMeshFaceAreas, m_AvgQuats, m_FieldPhases, m_CrystalStructures, m_GBCD, m_GBCDcount, m_GBCDdeltas, m_GBCDsizes, m_GBCDlimits), tbb::auto_partitioner());

  }
  else
#endif
  {
    CalculateGBCDImpl serial(nodesPtr, trianglesPtr, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_SurfaceMeshFaceAreas, m_AvgQuats, m_FieldPhases, m_CrystalStructures, m_GBCD, m_GBCDcount, m_GBCDdeltas, m_GBCDsizes, m_GBCDlimits);
    serial.generate(0, totalFaces);
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

