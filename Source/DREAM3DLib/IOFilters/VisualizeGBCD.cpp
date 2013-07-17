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

const static float m_pi = static_cast<float>(M_PI);
const static float m_pi2 = static_cast<float>(2*M_PI);

#define WRITE_XYZ_POINTS 1



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VisualizeGBCD::VisualizeGBCD() :
  SurfaceMeshFilter(),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_GBCDArrayName(DREAM3D::EnsembleData::GBCD),
  m_MisAngle(60.0f),
  m_OutputFile(""),
  m_StereoOutputFile(""),
  m_SphericalOutputFile(""),
  m_GMTOutputFile(""),
  m_CrystalStructures(NULL),
  m_GBCD(NULL)
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
#if WRITE_XYZ_POINTS
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Stereographic Projection");
    option->setPropertyName("StereoOutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.vtk");
    option->setFileType("VTK File");
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Spherical Data");
    option->setPropertyName("SphericalOutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.vtk");
    option->setFileType("VTK File");
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("GMT Output File");
    option->setPropertyName("GMTOutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.dat");
    option->setFileType("GMT File");
    option->setValueType("string");
    parameters.push_back(option);
  }
#endif
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
  writer->writeValue("StereoOutputFile", getStereoOutputFile() );
  writer->writeValue("SphericalOutputFile", getSphericalOutputFile() );
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
#if WRITE_XYZ_POINTS
  if(getStereoOutputFile().empty() == true)
  {
    ss.str("");
    ss << ClassName() << " needs the Stereographic Projection Output File Set and it was not.";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    setErrorCondition(-387);
  }
  if(getSphericalOutputFile().empty() == true)
  {
    ss.str("");
    ss << ClassName() << " needs the Spherical Data Output File Set and it was not.";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    setErrorCondition(-387);
  }
#endif

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
  gbcdLimits[1] = cosf(1.0*m_pi);
  gbcdLimits[2] = 0.0;
  gbcdLimits[3] = 0.0;
  gbcdLimits[4] = cosf(1.0*m_pi);
  gbcdLimits[5] = 2.0*m_pi;
  gbcdLimits[6] = cosf(0.0);
  gbcdLimits[7] = 2.0*m_pi;
  gbcdLimits[8] = 2.0*m_pi;
  gbcdLimits[9] = cosf(0.0);

  //get num components of GBCD
  int numComp = sm->getEnsembleData(DREAM3D::EnsembleData::GBCD)->GetNumberOfComponents();
  //determine the size of the 1,3,4 dimensions of the GBCD - 2,4 are dim/1.5
  int dim = int(pow((numComp/(2.0*2.0*2.0)),(1.0/5.0))+0.5);

  gbcdSizes[0] = dim*2;
  gbcdSizes[1] = dim;
  gbcdSizes[2] = dim*2;
  gbcdSizes[3] = dim*2;
  gbcdSizes[4] = dim;

  float binsize = gbcdLimits[5]/float(gbcdSizes[0]);
  float binsize2 = binsize*(2.0/m_pi);
  gbcdDeltas[0] = binsize;
  gbcdDeltas[1] = binsize2;
  gbcdDeltas[2] = binsize;
  gbcdDeltas[3] = binsize;
  gbcdDeltas[4] = binsize2;

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
  QuaternionMathF::Quat_t qtest;
  float w, n1, n2, n3;
  //  float dist;
  float theta, cosPhi, phi;

  m_MisAngle = m_MisAngle * m_pi/180.0f;
  //convert axis angle to matrix representation of misorientation
  OrientationMath::AxisAngletoMat(m_MisAngle, m_MisAxis.x, m_MisAxis.y, m_MisAxis.z, dg);

  //  int inversion = 1;
  //get number of symmetry operators
  int n_sym = m_OrientationOps[1]->getNumSymOps();

  int xpoints = 500;
  int ypoints = 500;
  int zpoints = 1;
  int xpointshalf = xpoints/2;
  int ypointshalf = ypoints/2;
  float xres = 2.0/float(xpoints);
  float yres = 2.0/float(ypoints);
  float zres = (xres+yres)/2.0;
  float x, y, z;
  int xbin, ybin;

  DoubleArrayType::Pointer poleFigureArray = DoubleArrayType::NullPointer();
  poleFigureArray = DoubleArrayType::CreateArray(xpoints*ypoints, 1, "PoleFigure");
  poleFigureArray->initializeWithValues(-1);
  double* poleFigure = poleFigureArray->GetPointer(0);

  FloatArrayType::Pointer xyzArray = FloatArrayType::NullPointer();
  xyzArray = FloatArrayType::CreateArray(gbcdSizes[3]*gbcdSizes[4], 3, "xyz");
  xyzArray->initializeWithValues(0.0);
  float* xyz = xyzArray->GetPointer(0);

#if WRITE_XYZ_POINTS
  size_t nPoints = 0;
#endif
  for (int64_t k = 0; k < (gbcdSizes[3]); k++)
  {
    for (int64_t l = 0; l < (gbcdSizes[4]); l++)
    {
      theta = float(k)*gbcdDeltas[3]+gbcdLimits[3]+(gbcdDeltas[3]/2.0);
      cosPhi = float(l)*gbcdDeltas[4]+gbcdLimits[4]+(gbcdDeltas[4]/2.0);
      phi = acosf(cosPhi);
      xyz[3*(l*gbcdSizes[3]+k)+0] = cosf(theta)*sinf(phi);
      xyz[3*(l*gbcdSizes[3]+k)+1] = sinf(theta)*sinf(phi);
      xyz[3*(l*gbcdSizes[3]+k)+2] = cosPhi;
    }
  }

  std::stringstream positions;
  std::stringstream scalars;
  std::stringstream sphericalPositions;

  std::vector<float> gmtValues;

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
        OrientationMath::EulertoQuat(qtest, mis_euler1[0], mis_euler1[1], mis_euler1[2]);
        OrientationMath::QuattoAxisAngle(qtest, w, n1, n2, n3);

        mis_euler1[1] = cosf(mis_euler1[1]);

        //find bins in GBCD
        int location1 = int((mis_euler1[0]-gbcdLimits[0])/gbcdDeltas[0]);
        int location2 = int((mis_euler1[1]-gbcdLimits[1])/gbcdDeltas[1]);
        int location3 = int((mis_euler1[2]-gbcdLimits[2])/gbcdDeltas[2]);
        //make sure that euler angles are within the GBCD space
        if(location1 >= 0 && location2 >= 0 && location3 >= 0 && location1 < gbcdSizes[0] && location2 < gbcdSizes[1] && location3 < gbcdSizes[2])
        {
          //calculate slot in the flattened GBCD that corresponds to the misorientation and then the next m_GBCDsizes[3]*m_GBCDsizes[4] slots are the phi,theta bins
          int shift = (location1)+(location2*gbcdSizes[0])+(location3*gbcdSizes[0]*gbcdSizes[1]);

          for (int64_t k = 0; k < (gbcdSizes[3]); k++)
          {
            for (int64_t l = 0; l < (gbcdSizes[4]); l++)
            {
              vec[0] = xyz[3*(l*gbcdSizes[3]+k)+0];
              vec[1] = xyz[3*(l*gbcdSizes[3]+k)+1];
              vec[2] = xyz[3*(l*gbcdSizes[3]+k)+2];
              //find symmetric poles using the first symmetry operator
              MatrixMath::Multiply3x3with3x1(sym1t, vec, rotNormal);
              if(q == 1)
              {
                //rotate symmetric pole by original misorientation
                MatrixMath::Multiply3x3with3x1(dgOrig, rotNormal, rotNormal2);
                //take negative of vector
                rotNormal[0] = -rotNormal2[0];
                rotNormal[1] = -rotNormal2[1];
                rotNormal[2] = -rotNormal2[2];
              }
#if WRITE_XYZ_POINTS
              sphericalPositions  << rotNormal[0] << " " << rotNormal[1] << " " << rotNormal[2] << "\n";
              if(rotNormal[2] < 0) {
                x = -rotNormal[0];
                y = -rotNormal[1];
                z = -rotNormal[2];
              }
              float lon = atan2(y, x) * 180.0/M_PI;
              if (lon < 0.0)
              {
                lon =lon + 360.0;
              }
              float lat = asin(z) * 180/M_PI;
              // IntReverse(rotNormal[0], rotNormal[1], rotNormal[2], lat, lon, h, NULL);
              gmtValues.push_back(lon);
              gmtValues.push_back(lat);
#endif
              if(rotNormal[2] < 0) {
                rotNormal[0] = -rotNormal[0];
                rotNormal[1] = -rotNormal[1];
                rotNormal[2] = -rotNormal[2];
              }
              //find x,y point in stereographic projection
              x = rotNormal[0]/(rotNormal[2]+1);
              y = rotNormal[1]/(rotNormal[2]+1);

#if WRITE_XYZ_POINTS
              positions << x << " " << y << " 0.0\n";
              nPoints++;
#endif
              //shift projection from -1 to 1 to 0 to 2
              x = x+1.0;
              y = y+1.0;
              //find bin in poleFigure array
              xbin = int(x/xres);
              ybin = int(y/yres);
              if(xbin < 0) xbin = 0;
              if(ybin < 0) ybin = 0;
              if(xbin >= xpoints) xbin = xpoints-1;
              if(ybin >= ypoints) ybin = ypoints-1;
              poleFigure[(ybin*xpoints)+xbin] = m_GBCD[shift+(k*gbcdSizes[0]*gbcdSizes[1]*gbcdSizes[2])+(l*gbcdSizes[0]*gbcdSizes[1]*gbcdSizes[2]*gbcdSizes[3])];
#if WRITE_XYZ_POINTS
              scalars << poleFigure[(ybin*xpoints)+xbin] << "\n";
              gmtValues.push_back(poleFigure[(ybin*xpoints)+xbin]);
#endif
            }
          }
        }
      }
    }
  }

#if WRITE_XYZ_POINTS
  {
    FILE* f = fopen(m_StereoOutputFile.c_str(), "wb");
    // Write the correct header
    fprintf(f, "# vtk DataFile Version 2.0\n");
    fprintf(f, "GBCD Stereograhic Projection irregular grid data\n");
    fprintf(f, "ASCII"); fprintf(f, "\n");
    fprintf(f, "DATASET POLYDATA\n");
    fprintf(f, "POINTS %lu float\n", nPoints);
    fprintf(f, "%s\n", positions.str().c_str());

    fprintf(f, "\n");
    fprintf(f, "POINT_DATA %lu\n", nPoints);
    fprintf(f, "SCALARS GBCD float 1\n");
    fprintf(f, "LOOKUP_TABLE default\n");
    fprintf(f, "%s\n", scalars.str().c_str());
    fclose(f);
  }

  {
    FILE* f = fopen(m_SphericalOutputFile.c_str(), "wb");
    // Write the correct header
    fprintf(f, "# vtk DataFile Version 2.0\n");
    fprintf(f, "GBCD Spherical Data irregular grid\n");
    fprintf(f, "ASCII"); fprintf(f, "\n");
    fprintf(f, "DATASET POLYDATA\n");
    fprintf(f, "POINTS %lu float\n", nPoints);
    fprintf(f, "%s\n", sphericalPositions.str().c_str());

    fprintf(f, "\n");
    fprintf(f, "POINT_DATA %lu\n", nPoints);
    fprintf(f, "SCALARS GBCD float 1\n");
    fprintf(f, "LOOKUP_TABLE default\n");
    fprintf(f, "%s\n", scalars.str().c_str());
    fclose(f);
  }

// Write the GMT file
  {
    std::string parentPath = MXAFileInfo::parentPath(m_GMTOutputFile);
    std::string basename = MXAFileInfo::fileNameWithOutExtension(m_GMTOutputFile);
    std::string extension = MXAFileInfo::extension(m_GMTOutputFile);
    std::string path = parentPath + MXAFileInfo::Separator + basename + std::string("_gmt_1.") + extension;

    FILE* f = fopen(path.c_str(), "wb");
    fprintf(f, "%.1f %.1f %.1f %.1f\n", m_MisAxis.x, m_MisAxis.y, m_MisAxis.z, m_MisAngle * 180/M_PI);
    size_t size = gmtValues.size()/3;

    for(size_t i = 0; i < size; i=i+3)
    {
      fprintf(f, "%f %f %f\n", gmtValues[i], gmtValues[i+1], gmtValues[i+2]);
    }
    fclose(f);
  }



#endif

  int neighbors[4];
  neighbors[0] = -1;
  neighbors[1] = 1;
  neighbors[2] = -xpoints;
  neighbors[3] = xpoints;
  int counter = 1;
  int point, neighbor;
  int good = 1;
  std::vector<int> neighs(xpoints*ypoints,-1);
  while(counter != 0)
  {
    counter = 0;
    for (int64_t k = 0; k < (xpoints); k++)
    {
      for (int64_t l = 0; l < (ypoints); l++)
      {
        x = (k-xpointshalf)*(xres)+(xres/2.0);
        y = (l-ypointshalf)*(yres)+(yres/2.0);
        if((x*x+y*y) <= 1)
        {
          point = l*xpoints+k;
          if(poleFigure[point] == -1)
          {
            counter++;
            for(int m = 0; m< 4; m++)
            {
              good = 1;
              if(k == 0 && m == 0) good = 0;
              if(k == xpoints-1 && m == 1) good = 0;
              if(l == 0 && m == 2) good = 0;
              if(l == ypoints-1 && m == 3) good = 0;
              if(good == 1)
              {
                neighbor = point + neighbors[m];
                if(poleFigure[neighbor] != -1) neighs[point] = neighbor;
              }
            }
          }
        }
      }
    }
    for(int n = 0; n < (neighs.size()); n++)
    {
      if(neighs[n] != -1 && poleFigure[n] == -1)
      {
        poleFigure[n] = poleFigure[neighs[n]];
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
      for (int64_t i = 0; i < (xpoints); i++)
      {
        for (int64_t j = 0; j < (ypoints); j++)
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
