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

#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"

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
void VisualizeGBCD::readFilterParameters(AbstractFilterParametersReader* reader)
{
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
		GET_PREREQ_DATA(sm, DREAM3D, EnsembleData, GBCD, ss, -388, float, FloatArrayType, ensembles, (60*29*60*60*29))
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

  float binsize = 6.0*m_pi/180.0;
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

  //determine phi, theta for each bin
  int count = 0;
  for(int j=0;j<m_GBCDsizes[4];j++)
  {
    for(int i=0;i<m_GBCDsizes[3];i++)
    {
      xyz_temp_ph[3*count+1] = i;
      xyz_temp_ph[3*count+2] = j;
      xyz_temp_ph[3*count+1] = xyz_temp_ph[3*count+1] * m_GBCDdeltas[3];
      xyz_temp_ph[3*count+1] = xyz_temp_ph[3*count+1] + m_GBCDlimits[3] + (m_GBCDdeltas[3]/2.0);
      xyz_temp_ph[3*count+2] = xyz_temp_ph[3*count+2] * m_GBCDdeltas[4];
      xyz_temp_ph[3*count+2] = xyz_temp_ph[3*count+2] + m_GBCDlimits[4] + (m_GBCDdeltas[4]/2.0);
      xyz_temp_ph[3*count+2] = acosf(xyz_temp_ph[3*count+2]);
      count++;
    }
  }

  //set up temp array for x,y,z,intensity for numPhiBins*numThetaBins times 1152 symmetric misorientations
  FloatArrayType::Pointer xyz_tempArray = FloatArrayType::NullPointer();
  xyz_tempArray = FloatArrayType::CreateArray((m_GBCDsizes[3]*m_GBCDsizes[4]), 4, "xyztemp");
  xyz_tempArray->initializeWithValues(0.0f);
  float* xyz_temp = xyz_tempArray->GetPointer(0);

  //determine cartesian coordinates for phi,theta values at each bin
  for(int i=0;i<(m_GBCDsizes[3]*m_GBCDsizes[4]);i++)
  {
    xyz_temp[4*i] = sinf(xyz_temp_ph[3*i+2])*cosf(xyz_temp_ph[3*i+1]);
    xyz_temp[4*i+1] = sinf(xyz_temp_ph[3*i+2])*sinf(xyz_temp_ph[3*i+1]);
    xyz_temp[4*i+2] = cosf(xyz_temp_ph[3*i+2]);
  }
  
  float vec[3];
  float rotNormal[3];
  float rotNormalsc[2];
  float dg[3][3];
  float dgOrig[3][3];
  float dgt[3][3];
  float dg1[3][3];
  float dg2[3][3];
  float sym1[3][3];
  float sym1t[3][3];
  float sym2[3][3];
  float sym2t[3][3];
  float mis_euler1[3];
  int theta, cosPhi;

  float mis_angle = 60;
  float misvect[3] = {1,1,1};

  mis_angle = mis_angle * m_pi/180.0f;

  //convert axis angle to matrix representation of misorientation
  OrientationMath::axisAngletoMat(mis_angle, misvect[0], misvect[1], misvect[2], dg);
    
  //get number of symmetry operators
  int n_sym = m_OrientationOps[1]->getNumSymOps();

  //set up array for x,y,z,intensity for numPhiBins*numThetaBins times 1152 symmetric misorientations
  int nchunk = m_GBCDsizes[3]*m_GBCDsizes[4];
  FloatArrayType::Pointer xyzArray = FloatArrayType::NullPointer();
  xyzArray = FloatArrayType::CreateArray((nchunk), "xyz");
  xyzArray->initializeWithValues(0.0f);
  float* xyz = xyzArray->GetPointer(0);
  int counter = 0;

  for(int q=0;q<2;q++)
  {
    if(q == 2)
    { 
      //take inverse of misorientation, but leave in misorientation variable to use for switching symmetry
      MatrixMath::transpose3x3(dg, dg);
      //take inverse again of misorientation and put in original
      MatrixMath::transpose3x3(dg, dgOrig);
    }
    for(int i=0; i<n_sym; i++)
    {
      //get symmetry operator1
      m_OrientationOps[1]->getMatSymOp(i, sym1);
      MatrixMath::multiply3x3with3x3(sym1,dg,dg1);
      //transpose sym1 for later use with normals
      MatrixMath::transpose3x3(sym1,sym1t);
      for(int j=0; j<n_sym; j++)
      {
        //get symmetry operator2
        m_OrientationOps[1]->getMatSymOp(j, sym2);
        MatrixMath::transpose3x3(sym2,sym2t);
        //calculate symmetric misorientation
        MatrixMath::multiply3x3with3x3(dg1,sym2t,dg2);
        //convert to euler angle
        OrientationMath::mattoEuler(dg2, mis_euler1[0], mis_euler1[1], mis_euler1[2]);

        mis_euler1[1] = cosf(mis_euler1[1]);

        //find bins in GBCD
        int location1 = int((mis_euler1[0]-m_GBCDlimits[0])/m_GBCDdeltas[0]);
        int location2 = int((mis_euler1[1]-m_GBCDlimits[1])/m_GBCDdeltas[1]);
        int location3 = int((mis_euler1[2]-m_GBCDlimits[2])/m_GBCDdeltas[2]);
        //make sure that euler angles are within the GBCD space
        if(location1 >= 0 && location2 >= 0 && location3 >= 0 && location1 < m_GBCDsizes[0] && location2 < m_GBCDsizes[1] && location3 < m_GBCDsizes[2])
        {
          //calculate slot in the flattened GBCD that corresponds to the misorientation and then the next m_GBCDsizes[3]*m_GBCDsizes[4] slots are the phi,theta bins
          int shift = (location1)+(location2*m_GBCDsizes[0])+(location3*m_GBCDsizes[0]*m_GBCDsizes[1]);

          //copy intensity for each phi,theta bin in 4th slot of temp cartesian x,y,z,value array
          for(int k=0;k<m_GBCDsizes[4];k++)
          {
            for(int l=0;l<m_GBCDsizes[3];l++)
            {
              xyz_temp[4*(l+(m_GBCDsizes[3]*k))+3] = m_GBCD[shift+(l*m_GBCDsizes[0]*m_GBCDsizes[1]*m_GBCDsizes[2])+(k*m_GBCDsizes[0]*m_GBCDsizes[1]*m_GBCDsizes[2]*m_GBCDsizes[3])];
            }
          }

          for(int k=0;k<(m_GBCDsizes[3]*m_GBCDsizes[4]);k++)
          {
            //create vector using x,y,z for each phi,theta bin
            vec[0] = xyz_temp[4*k];
            vec[1] = xyz_temp[4*k+1];
            vec[2] = xyz_temp[4*k+2];
            //find symmetric poles using inverted symmetry operator
            MatrixMath::multiply3x3with3x1(sym1t, vec, rotNormal);
            if(q == 2)
            {
              //rotate symmetric pole by original misorientation
              MatrixMath::multiply3x3with3x1(dgOrig, rotNormal, rotNormal);
              //take negative of vector
              rotNormal[0] = -rotNormal[0];
              rotNormal[1] = -rotNormal[1];
              rotNormal[2] = -rotNormal[2];
            }
            //calculate the crystal normals in aspherical coordinates ->[theta, cos(phi) ]
            rotNormalsc[0] = atan2f(rotNormal[1], rotNormal[0]);
            if (rotNormalsc[0] < 0) rotNormalsc[0] += m_pi2;
            rotNormalsc[1] = rotNormal[2];

            theta = (int) floorf((rotNormalsc[0] - m_GBCDlimits[3])/m_GBCDdeltas[3]);
            cosPhi = (int) floorf((rotNormalsc[1] - m_GBCDlimits[4])/m_GBCDdeltas[4]);

            //add symmetric vector intensity to final x,y,z,value array
            xyz[(theta+(m_GBCDsizes[3]*cosPhi))] = xyz[(theta+(m_GBCDsizes[3]*cosPhi))] + xyz_temp[4*k+3];
          }
          counter += 1;
        }
      }
    }
  }

  float x, y, z;

  std::string file = "testGBCDViz.vtk";
  FILE* f = NULL;
  f = fopen(file.c_str(), "wb");

  int xpoints = 500;
  int ypoints = 500;
  int zpoints = 1;
  int xpointshalf = xpoints/2;
  int ypointshalf = ypoints/2;
  float xres = 2.0/float(xpoints);
  float yres = 2.0/float(ypoints);
  float zres = (xres+yres)/2.0;

  // Write the correct header
  fprintf(f, "# vtk DataFile Version 2.0\n");
  fprintf(f, "data set from DREAM3D\n");
  fprintf(f, "BINARY"); fprintf(f, "\n");
  fprintf(f, "DATASET RECTILINEAR_GRID\n");
  fprintf(f, "DIMENSIONS %ld %ld %ld\n", xpoints+1, ypoints+1, zpoints+1);

  // Write the Coords
  writeCoords(f, "X_COORDINATES", "float", xpoints + 1, 0.0f - xres * 0.5f, (float)(xpoints + 1 * xres), xres);
  writeCoords(f, "Y_COORDINATES", "float", ypoints + 1, 0.0f - yres * 0.5f, (float)(ypoints + 1 * yres), yres);
  writeCoords(f, "Z_COORDINATES", "float", zpoints + 1, 0.0f - zres * 0.5f, (float)(zpoints + 1 * zres), zres);

  size_t total = xpoints * ypoints * zpoints;
  fprintf(f, "CELL_DATA %d\n", (int)total);

  fprintf(f, "SCALARS %s %s 1\n", "Intensity", "float");
  fprintf(f, "LOOKUP_TABLE default\n");
  { 
    float* gn = new float[total];
    float t;
    int count = 0;
    for (int64_t i = 0; i < (xpoints); i++) 
    {
      for (int64_t j = 0; j < (ypoints); j++) 
      {
        x = (i-xpointshalf)*(xres)+(xres/2.0);
        y = (j-ypointshalf)*(yres)+(yres/2.0);
        if((x*x+y*y) <= 1)
        {
          z = -((x*x+y*y)-1)/((x*x+y*y)+1);
          x = x*(1-z);
          y = y*(1-z);
          rotNormalsc[0] = atan2f(y, x);
          if (rotNormalsc[0] < 0) rotNormalsc[0] += m_pi2;
          rotNormalsc[1] = z;
          theta = (int) floorf((rotNormalsc[0] - m_GBCDlimits[3])/m_GBCDdeltas[3]);
          cosPhi = (int) floorf((rotNormalsc[1] - m_GBCDlimits[4])/m_GBCDdeltas[4]);
          t = xyz[(theta+(m_GBCDsizes[3]*cosPhi))];
        }
        else
        {
          t = 0;
        }
        MXA::Endian::FromSystemToBig::convert<float>(t); 
        gn[count] = t; 
        count++;
      }
    }
    int64_t totalWritten = fwrite(gn, sizeof(float), (total), f);
    delete[] gn;
    if (totalWritten != (total))  {
      std::cout << "Error Writing Binary VTK Data into file " << file << std::endl;
      fclose(f);
    }
  }

  fclose(f);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

