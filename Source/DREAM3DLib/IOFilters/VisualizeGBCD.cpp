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
#include "VisualizeGBCD.h"


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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VisualizeGBCD::VisualizeGBCD() :
  SurfaceMeshFilter(),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_GBCDArrayName(DREAM3D::EnsembleData::GBCD),
  m_CrystalStructures(NULL),
  m_GBCD(NULL)
{
    m_OrientationOps = OrientationMath::getOrientationOpsVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VisualizeGBCD::~VisualizeGBCD()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCD::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCD::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCD::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
		GET_PREREQ_DATA(sm, DREAM3D, EnsembleData, GBCD, ss, -388, float, FloatArrayType, ensembles, (36*18*36*36*18))
	}

  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCD::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheckSurfaceMesh(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCD::execute()
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
  dataCheckSurfaceMesh(false, 0, totalFaces, sm->getNumEnsembleTuples());

  FloatArrayType::Pointer gbcdDeltasArray = FloatArrayType::NullPointer();
  FloatArrayType::Pointer gbcdLimitsArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer gbcdSizesArray = Int32ArrayType::NullPointer();
  gbcdDeltasArray = FloatArrayType::CreateArray(5, "GBCDDeltas");
  gbcdDeltasArray->SetNumberOfComponents(1);
  gbcdDeltasArray->initializeWithZeros();
  gbcdLimitsArray = FloatArrayType::CreateArray(10, "GBCDLimits");
  gbcdLimitsArray->SetNumberOfComponents(1);
  gbcdLimitsArray->initializeWithZeros();
  gbcdSizesArray = Int32ArrayType::CreateArray(5, "GBCDSizes");
  gbcdSizesArray->SetNumberOfComponents(1);
  gbcdSizesArray->initializeWithZeros();
  float* m_GBCDdeltas = gbcdDeltasArray->GetPointer(0);
  int* m_GBCDsizes = gbcdSizesArray->GetPointer(0);
  float* m_GBCDlimits = gbcdLimitsArray->GetPointer(0);

  m_GBCDlimits[0] = 0.0;
  m_GBCDlimits[1] = cosf(1.0*m_pi);
  m_GBCDlimits[2] = 0.0;
  m_GBCDlimits[3] = 0.0;
  m_GBCDlimits[4] = cosf(1.0*m_pi);
  m_GBCDlimits[5] = 2.0*m_pi;
  m_GBCDlimits[6] = cosf(0.0);
  m_GBCDlimits[7] = 2.0*m_pi;
  m_GBCDlimits[8] = 2.0*m_pi;
  m_GBCDlimits[9] = cosf(0.0);

  float binsize = 10.0*m_pi/180.0;
  float binsize2 = binsize*(2.0/m_pi);
  m_GBCDdeltas[0] = binsize;
  m_GBCDdeltas[1] = binsize2;
  m_GBCDdeltas[2] = binsize;
  m_GBCDdeltas[3] = binsize;
  m_GBCDdeltas[4] = binsize2;

  m_GBCDsizes[0] = int((m_GBCDlimits[5]-m_GBCDlimits[0])/m_GBCDdeltas[0]);
  m_GBCDsizes[1] = int((m_GBCDlimits[6]-m_GBCDlimits[1])/m_GBCDdeltas[1]);
  m_GBCDsizes[2] = int((m_GBCDlimits[7]-m_GBCDlimits[2])/m_GBCDdeltas[2]);
  m_GBCDsizes[3] = int((m_GBCDlimits[8]-m_GBCDlimits[3])/m_GBCDdeltas[3]);
  m_GBCDsizes[4] = int((m_GBCDlimits[9]-m_GBCDlimits[4])/m_GBCDdeltas[4]);
    
  int inversion = 1;
  float tol = 5.0;
      
  FloatArrayType::Pointer xyz_temp_phArray = FloatArrayType::NullPointer();
  xyz_temp_phArray = FloatArrayType::CreateArray((m_GBCDsizes[3]*m_GBCDsizes[4]), 3, "xyz");
  xyz_temp_phArray->initializeWithValues(1.0f);
  float* xyz_temp_ph = xyz_temp_phArray->GetPointer(0);

  int count = 0;
  for(int j=0;j<m_GBCDsizes[4];j++)
  {
    for(int i=0;i<m_GBCDsizes[3];i++)
    {
      xyz_temp_ph[3*count+1] = i;
      xyz_temp_ph[3*count+2] = j;
      xyz_temp_ph[3*count+1] = xyz_temp_ph[3*count+1] * m_GBCDdeltas[3];
      xyz_temp_ph[3*count+1] = xyz_temp_ph[3*count+1] + m_GBCDlimits[3];
      xyz_temp_ph[3*count+2] = xyz_temp_ph[3*count+2] * m_GBCDdeltas[4];
      xyz_temp_ph[3*count+2] = xyz_temp_ph[3*count+2] + m_GBCDlimits[4];
      xyz_temp_ph[3*count+2] = acosf(xyz_temp_ph[3*count+2]);
      count++;
    }
  }

  FloatArrayType::Pointer xyz_tempArray = FloatArrayType::NullPointer();
  xyz_tempArray = FloatArrayType::CreateArray((m_GBCDsizes[3]*m_GBCDsizes[4]), 4, "xyz");
  xyz_tempArray->initializeWithValues(0.0f);
  float* xyz_temp = xyz_tempArray->GetPointer(0);

  for(int i=0;i<(m_GBCDsizes[3]*m_GBCDsizes[4]);i++)
  {
    xyz_temp[4*i] = sinf(xyz_temp_ph[3*i+2])*cosf(xyz_temp_ph[3*i+1]);
    xyz_temp[4*i+1] = sinf(xyz_temp_ph[3*i+2])*sinf(xyz_temp_ph[3*i+1]);
    xyz_temp[4*i+2] = cosf(xyz_temp_ph[3*i+2]);
  }
  
  float mis_quat[5];
  float mis_quat1[5];
  float mis_quat2[5];
  float mis_euler1[3];
  float sym_q[5];

  float mis_angle = 60;
  float misvect[3] = {1,1,1};

  mis_angle = mis_angle * m_pi/180.0f;
  
  OrientationMath::axisAngletoQuat(mis_angle, misvect[0], misvect[1], misvect[2], mis_quat);
    
  int n_sym = m_OrientationOps[1]->getNumSymOps();

  int nchunk = m_GBCDsizes[3]*m_GBCDsizes[4];
  FloatArrayType::Pointer xyzArray = FloatArrayType::NullPointer();
  xyzArray = FloatArrayType::CreateArray((nchunk*2*n_sym*n_sym), 4, "xyz");
  xyzArray->initializeWithValues(0.0f);
  float* xyz = xyzArray->GetPointer(0);
  int counter = 0;
  for(int q=0;q<2;q++)
  {
    if(q == 1)
	  { 
		  OrientationMath::invertQuaternion(mis_quat);
	  }
    for(int i=0; i<n_sym; i++)
	  {
      m_OrientationOps[1]->getQuatSymOp(i, sym_q);
      OrientationMath::multiplyQuaternions(sym_q, mis_quat, mis_quat1);
      for(int j=0; j<n_sym; j++)
	    {
        m_OrientationOps[1]->getQuatSymOp(j, sym_q);
        OrientationMath::invertQuaternion(sym_q);
        OrientationMath::multiplyQuaternions(mis_quat1, sym_q, mis_quat2);
        OrientationMath::QuattoEuler(mis_quat2, mis_euler1[0], mis_euler1[1], mis_euler1[2]);
        
        mis_euler1[1] = cosf(mis_euler1[1]);
        
        int location1 = int((mis_euler1[0]-m_GBCDlimits[0])/m_GBCDdeltas[0]);
        int location2 = int((mis_euler1[1]-m_GBCDlimits[1])/m_GBCDdeltas[1]);
        int location3 = int((mis_euler1[2]-m_GBCDlimits[2])/m_GBCDdeltas[2]);
        //make sure that euler angles are within the GBCD space
        if(Min(location) >= 0 && Min(location < szGBCD[1:3]) > 0)
		    {
          interface_D = gbcd[location[0], location[1], location[2], *,*]
          
          xyz_temp1[3,*] =  Reform(interface_D, szgbcd[4]*szgbcd[5])
          
          trash = Quat_vector(Quat_inverse(symmetry[*,i]),xyz_temp )
          if(q == 1)
		      {
			      trash = -1.*Quat_vector(Quat_inverse(mis_quat), trash)
		      }
          xyz_temp1[0:2,*] = trash
          xyz[*,counter*nchunk:(counter+1)*nchunk -1] = xyz_temp1
          counter += 1l
          
		    }
	    }
	}
  
  xyz = xyz[*,0:nchunk*counter-1]
  temp = xyz
  poles_raw = xyz
  intensity = xyz[3,*]
  xyz = xyz[0:2,*]

  [xyz, Transpose(intensity)]

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

