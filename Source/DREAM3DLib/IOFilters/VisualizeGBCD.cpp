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


#include <cmath>
#include <algorithm>
#include <limits>

#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VisualizeGBCD::VisualizeGBCD() :
  SurfaceMeshFilter(),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_GBCDArrayName(DREAM3D::EnsembleData::GBCD),
  m_GBCDdimensionsArrayName(DREAM3D::EnsembleData::GBCDdimensions),
  m_MisAngle(60.0f),
  m_OutputFile(""),
  m_CrystalStructures(NULL),
  m_GBCD(NULL),
  m_GBCDdimensions(NULL)
{
  m_MisAxis.x = 1;
  m_MisAxis.y = 1;
  m_MisAxis.z = 1;

  m_OrientationOps = OrientationOps::getOrientationOpsVector();
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
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setPropertyName("MisAngle");
    option->setHumanLabel("Misorientation Angle");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("Degrees");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();

    option->setHumanLabel("Misorientation Axis");
    option->setPropertyName("MisAxis");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Regular Grid Pole Figure");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.vtk");
    option->setFileType("VTK File");
    option->setValueType("string");
    parameters.push_back(option);
  }
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
  writer->writeValue("MisorientationAngle", getMisAngle() );
  writer->writeValue("MisorientationAxis", getMisAxis() );
  writer->writeValue("OutputFile", getOutputFile() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCD::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();

  if(getOutputFile().empty() == true)
  {
    ss.str("");
    ss << ClassName() << " needs the Output File Set and it was not.";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    setErrorCondition(-387);
  }

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
      setErrorCondition(-384);
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());

    }

    // We MUST have Triangles defined also.
    if(sm->getFaces().get() == NULL)
    {
      setErrorCondition(-385);
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());

    }
    else
    {
      IDataArray::Pointer iDataArray = sm->getEnsembleData(DREAM3D::EnsembleData::GBCD);
      if (NULL == iDataArray.get())
      {
        setErrorCondition(-387);
        addErrorMessage(getHumanLabel(), "The GBCD Array was not found in the Surface Mesh Ensemble Data. ", getErrorCondition());
      }
      else
      {
        GET_PREREQ_DATA(sm, DREAM3D, EnsembleData, GBCDdimensions, ss, -301, int32_t, Int32ArrayType, ensembles, 5)
        int numComp = iDataArray->GetNumberOfComponents();
        GET_PREREQ_DATA(sm, DREAM3D, EnsembleData, GBCD, ss, -301, double, DoubleArrayType, ensembles, numComp)
      }
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

  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = sm->getVertices();

  DREAM3D::SurfaceMesh::FaceListPointer_t trianglesPtr = sm->getFaces();
  size_t totalFaces = trianglesPtr->GetNumberOfTuples();

  // Run the data check to allocate the memory for the centroid array
  dataCheckSurfaceMesh(false, 0, totalFaces, sm->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  FloatArrayType::Pointer gbcdDeltasArray = FloatArrayType::CreateArray(5, "GBCDDeltas");
  gbcdDeltasArray->initializeWithZeros();

  FloatArrayType::Pointer gbcdLimitsArray = FloatArrayType::CreateArray(10, "GBCDLimits");
  gbcdLimitsArray->initializeWithZeros();

  Int32ArrayType::Pointer gbcdSizesArray = Int32ArrayType::CreateArray(5, "GBCDSizes");
  gbcdSizesArray->initializeWithZeros();

  float* gbcdDeltas = gbcdDeltasArray->GetPointer(0);
  int* gbcdSizes = gbcdSizesArray->GetPointer(0);
  float* gbcdLimits = gbcdLimitsArray->GetPointer(0);

  gbcdLimits[0] = 0.0;
  gbcdLimits[1] = cosf(1.0*DREAM3D::Constants::k_Pi);
  gbcdLimits[2] = 0.0;
  gbcdLimits[3] = -sqrt(DREAM3D::Constants::k_Pi/2.0);
  gbcdLimits[4] = -sqrt(DREAM3D::Constants::k_Pi/2.0);
  gbcdLimits[5] = 2.0*DREAM3D::Constants::k_Pi;
  gbcdLimits[6] = cosf(0.0);
  gbcdLimits[7] = 2.0*DREAM3D::Constants::k_Pi;
  gbcdLimits[8] = sqrt(DREAM3D::Constants::k_Pi/2.0);
  gbcdLimits[9] = sqrt(DREAM3D::Constants::k_Pi/2.0);

  gbcdSizes[0] = m_GBCDdimensions[5*1+0];
  gbcdSizes[1] = m_GBCDdimensions[5*1+1];
  gbcdSizes[2] = m_GBCDdimensions[5*1+2];
  gbcdSizes[3] = m_GBCDdimensions[5*1+3];
  gbcdSizes[4] = m_GBCDdimensions[5*1+4];

  gbcdDeltas[0] = gbcdLimits[5]/float(gbcdSizes[0]);
  gbcdDeltas[1] = (gbcdLimits[6]-gbcdLimits[1])/float(gbcdSizes[1]);
  gbcdDeltas[2] = gbcdLimits[7]/float(gbcdSizes[2]);
  gbcdDeltas[3] = (gbcdLimits[8]-gbcdLimits[3])/float(gbcdSizes[3]);
  gbcdDeltas[4] = (gbcdLimits[9]-gbcdLimits[4])/float(gbcdSizes[4]);

  float vec[3];
  float rotNormal[3];
  float rotNormal2[3];
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

  m_MisAngle = m_MisAngle * DREAM3D::Constants::k_Pi/180.0f;
  //convert axis angle to matrix representation of misorientation
  OrientationMath::AxisAngletoMat(m_MisAngle, m_MisAxis.x, m_MisAxis.y, m_MisAxis.z, dg);

  //  int inversion = 1;
  //get number of symmetry operators
  int n_sym = m_OrientationOps[1]->getNumSymOps();

  int xpoints = 200;
  int ypoints = 200;
  int zpoints = 1;
  int xpointshalf = xpoints/2;
  int ypointshalf = ypoints/2;
  float xres = 2.0/float(xpoints);
  float yres = 2.0/float(ypoints);
  float zres = (xres+yres)/2.0;
  float x, y;
  float a, b;
//  int xbin, ybin;
  int abin, bbin;
  int abinMod, bbinMod;
  float modX, modY;
  float intensity1, intensity2, intensity3, intensity4, interpolatedIntensity;
  int hemisphere = 0;
  float adjust = 1.0;

  DoubleArrayType::Pointer poleFigureArray = DoubleArrayType::NullPointer();
  poleFigureArray = DoubleArrayType::CreateArray(xpoints*ypoints, 1, "PoleFigure");
  poleFigureArray->initializeWithValues(0);
  double* poleFigure = poleFigureArray->GetPointer(0);
  FloatArrayType::Pointer poleFigureCountsArray = FloatArrayType::NullPointer();
  poleFigureCountsArray = FloatArrayType::CreateArray(xpoints*ypoints, 1, "PoleFigureCounts");
  poleFigureCountsArray->initializeWithValues(0);
  float* poleFigureCounts = poleFigureCountsArray->GetPointer(0);
  FloatArrayType::Pointer vecsArray = FloatArrayType::NullPointer();
  vecsArray = FloatArrayType::CreateArray(xpoints*ypoints, 3, "Vecs");
  vecsArray->initializeWithValues(-1000);
  float* vecs = vecsArray->GetPointer(0);

  int count = 0;
  for (int64_t k = 0; k < (xpoints); k++)
  {
    for (int64_t l = 0; l < (ypoints); l++)
    {
      //get (x,y) for stereographic projection pixel
      x = float(k-xpointshalf)*xres+(xres/2.0);
      y = float(l-ypointshalf)*yres+(yres/2.0);
      if((x*x+y*y) <= 1.0)
      {
        vecs[3*count+2] = -((x*x+y*y)-1)/((x*x+y*y)+1);
        vecs[3*count+0] = x*(1+vecs[3*count+2]);
        vecs[3*count+1] = y*(1+vecs[3*count+2]);
      }
      count++;
    }
  }

  for(int q=0;q<2;q++)
  {
    if(q == 1)
    {
      //copy original misorientation for use later
      MatrixMath::Copy3x3(dg, dgOrig);
      //take inverse of misorientation, then copy into misorientation variable to use for switching symmetry
      MatrixMath::Transpose3x3(dg, dgt);
      MatrixMath::Copy3x3(dgt, dg);
    }
    for(int i=0; i<n_sym; i++)
    {
      //get symmetry operator1
      m_OrientationOps[1]->getMatSymOp(i, sym1);
      MatrixMath::Multiply3x3with3x3(sym1,dg,dg1);
      //get transpose for rotation of directions
      MatrixMath::Transpose3x3(sym1, sym1t);
      for(int j=0; j<n_sym; j++)
      {
        //get symmetry operator2
        m_OrientationOps[1]->getMatSymOp(j, sym2);
        MatrixMath::Transpose3x3(sym2,sym2t);
        //calculate symmetric misorientation
        MatrixMath::Multiply3x3with3x3(dg1,sym2t,dg2);
        //convert to euler angle
        OrientationMath::MattoEuler(dg2, mis_euler1[0], mis_euler1[1], mis_euler1[2]);

        mis_euler1[1] = cosf(mis_euler1[1]);

        //find bins in GBCD
        int location1 = (int) floorf((mis_euler1[0]-gbcdLimits[0])/gbcdDeltas[0]);
        int location2 = (int) floorf((mis_euler1[1]-gbcdLimits[1])/gbcdDeltas[1]);
        int location3 = (int) floorf((mis_euler1[2]-gbcdLimits[2])/gbcdDeltas[2]);
        //make sure that euler angles are within the GBCD space
        if(location1 >= 0 && location2 >= 0 && location3 >= 0 && location1 < gbcdSizes[0] && location2 < gbcdSizes[1] && location3 < gbcdSizes[2])
        {
          //calculate slot in the flattened GBCD that corresponds to the misorientation and then the next m_GBCDsizes[3]*m_GBCDsizes[4] slots are the phi,theta bins
          int shift = (location1)+(location2*gbcdSizes[0])+(location3*gbcdSizes[0]*gbcdSizes[1]);

          count = 0;
          for (int64_t k = 0; k < (xpoints); k++)
          {
            for (int64_t l = 0; l < (ypoints); l++)
            {
              if(vecs[3*count+0] != -1000)
              {
                vec[0] = vecs[3*count+0];
                vec[1] = vecs[3*count+1];
                vec[2] = vecs[3*count+2];
                for (int64_t m = 0; m < 2; m++)
                {
                  //look at s.h. points on second trip around
                  if(m == 1)
                  {
                    MatrixMath::Multiply3x1withConstant(vec, -1);
                  }
                  //find symmetric poles using the first symmetry operator
                  MatrixMath::Multiply3x3with3x1(sym1, vec, rotNormal);
                  if(q == 1)
                  {
                    //rotate symmetric pole by original misorientation
                    MatrixMath::Multiply3x3with3x1(dgOrig, rotNormal, rotNormal2);
                    //take negative of vector
                    rotNormal[0] = -rotNormal2[0];
                    rotNormal[1] = -rotNormal2[1];
                    rotNormal[2] = -rotNormal2[2];
                  }
                  adjust = 1.0;
                  hemisphere = 1;
                  if(rotNormal[2] >= 0.0)
                  {
                    adjust = -1.0;
                    hemisphere = 0;
                  }
                  if(fabs(rotNormal[0]) >= fabs(rotNormal[1]))
                  {
                    a = (rotNormal[0]/fabs(rotNormal[0]))*sqrt(2.0*1.0*(1.0+(rotNormal[2]*adjust)))*(sqrt(DREAM3D::Constants::k_Pi)/2.0);
                    b = (rotNormal[0]/fabs(rotNormal[0]))*sqrt(2.0*1.0*(1.0+(rotNormal[2]*adjust)))*((2.0/sqrt(DREAM3D::Constants::k_Pi))*atan(rotNormal[1]/rotNormal[0]));
                  }
                  else
                  {
                    a = (rotNormal[1]/fabs(rotNormal[1]))*sqrt(2.0*1.0*(1.0+(rotNormal[2]*adjust)))*((2.0/sqrt(DREAM3D::Constants::k_Pi))*atan(rotNormal[0]/rotNormal[1]));
                    b = (rotNormal[1]/fabs(rotNormal[1]))*sqrt(2.0*1.0*(1.0+(rotNormal[2]*adjust)))*(sqrt(DREAM3D::Constants::k_Pi)/2.0);
                  }
                  modX = ((a-gbcdLimits[3])/gbcdDeltas[3]);
                  modY = ((b-gbcdLimits[4])/gbcdDeltas[4]);
                  abin = (int) modX;
                  bbin = (int) modY;
                  modX -= abin;
                  modY -= bbin;
                  if(abin < gbcdSizes[3]-1) abinMod = abin+1;
                  else abinMod = abin+1-gbcdSizes[3];
                  if(bbin < gbcdSizes[4]-1) bbinMod = bbin+1;
                  else bbinMod = bbin+1-gbcdSizes[4];
                  intensity1 = m_GBCD[2*(shift+(abin*gbcdSizes[0]*gbcdSizes[1]*gbcdSizes[2])+(bbin*gbcdSizes[0]*gbcdSizes[1]*gbcdSizes[2]*gbcdSizes[3]))+hemisphere];
                  intensity2 = m_GBCD[2*(shift+(abinMod*gbcdSizes[0]*gbcdSizes[1]*gbcdSizes[2])+(bbin*gbcdSizes[0]*gbcdSizes[1]*gbcdSizes[2]*gbcdSizes[3]))+hemisphere];
                  intensity3 = m_GBCD[2*(shift+(abin*gbcdSizes[0]*gbcdSizes[1]*gbcdSizes[2])+(bbinMod*gbcdSizes[0]*gbcdSizes[1]*gbcdSizes[2]*gbcdSizes[3]))+hemisphere];
                  intensity4 = m_GBCD[2*(shift+(abinMod*gbcdSizes[0]*gbcdSizes[1]*gbcdSizes[2])+(bbinMod*gbcdSizes[0]*gbcdSizes[1]*gbcdSizes[2]*gbcdSizes[3]))+hemisphere];
                  interpolatedIntensity = ((intensity1*(1-modX)*(1-modY))+(intensity2*(modX)*(1-modY))+(intensity3*(1-modX)*(modY))+(intensity4*(modX)*(modY)));
                  poleFigure[(l*xpoints)+k] += interpolatedIntensity;
                  poleFigureCounts[(l*xpoints)+k] += 1.0;
                }
              }
              count++;
            }
          }
        }
      }
    }
  }

  for (int64_t k = 0; k < (xpoints); k++)
  {
    for (int64_t l = 0; l < (ypoints); l++)
    {
      //get (x,y) for stereographic projection pixel
      x = float(k-xpointshalf)*xres+(xres/2.0);
      y = float(l-ypointshalf)*yres+(yres/2.0);
      if((x*x+y*y) <= 1.0)
      {
        poleFigure[(l*xpoints)+k] /= poleFigureCounts[(l*xpoints)+k];
      }
    }
  }

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  std::string parentPath = MXAFileInfo::parentPath(getOutputFile());
  if(!MXADir::mkdir(parentPath, true))
  {
    ss.str("");
    ss << "Error creating parent path '" << parentPath << "'";
    setErrorCondition(-998);
    notifyErrorMessage(ss.str(), getErrorCondition());
    return;
  }

  {
    FILE* f = NULL;
    f = fopen(m_OutputFile.c_str(), "wb");
    if(NULL == f)
    {

      ss.str("");
      ss << "Could not open GBCD viz file " << m_OutputFile << " for writing. Please check access permissions and the path to the output location exists";
      notifyErrorMessage(ss.str(), getErrorCondition());
      return;
    }



    // Write the correct header
    fprintf(f, "# vtk DataFile Version 2.0\n");
    fprintf(f, "data set from DREAM3D\n");
    fprintf(f, "BINARY"); fprintf(f, "\n");
    fprintf(f, "DATASET RECTILINEAR_GRID\n");
    fprintf(f, "DIMENSIONS %d %d %d\n", xpoints+1, ypoints+1, zpoints+1);

    // Write the Coords
    writeCoords(f, "X_COORDINATES", "float", xpoints + 1, (-float(xpoints)*xres/2.0), xres);
    writeCoords(f, "Y_COORDINATES", "float", ypoints + 1, (-float(ypoints)*yres/2.0), yres);
    writeCoords(f, "Z_COORDINATES", "float", zpoints + 1, (-float(zpoints)*zres/2.0), zres);

    size_t total = xpoints * ypoints * zpoints;
    fprintf(f, "CELL_DATA %d\n", (int)total);

    fprintf(f, "SCALARS %s %s 1\n", "Intensity", "float");
    fprintf(f, "LOOKUP_TABLE default\n");
    {
      float* gn = new float[total];
      float t;
      int count = 0;
      for (int64_t j = 0; j < (ypoints); j++)
      {
        for (int64_t i = 0; i < (xpoints); i++)
        {
          t = float(poleFigure[(j*xpoints)+i]);
          MXA::Endian::FromSystemToBig::convert<float>(t);
          gn[count] = t;
          count++;
        }
      }
      int64_t totalWritten = fwrite(gn, sizeof(float), (total), f);
      delete[] gn;
      if (totalWritten != (total))  {
        std::cout << "Error Writing Binary VTK Data into file " << m_OutputFile << std::endl;
        fclose(f);
      }
    }

    fclose(f);
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
