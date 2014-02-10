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
#include "DREAM3DLib/Math/DREAM3DMath.h"

const static float m_piOver2 = static_cast<float>(M_PI/2.0);
const static float m_pi = static_cast<float>(M_PI);
const static float m_pi2 = static_cast<float>(2.0*M_PI);
const static float m_180Overpi = static_cast<float>(180.0/M_PI);

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
  m_GMTOutputFile(""),
  m_CrystalStructure(1),
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
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Crystal Structure");
    option->setPropertyName("CrystalStructure");
    option->setWidgetType(FilterParameter::ChoiceWidget);
    option->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("Hexagonal-High 6/mmm");
    choices.push_back("Cubic-High m-3m");
    choices.push_back("Hexagonal-Low 6/m");
    choices.push_back("Cubic-Low m-3 (Tetrahedral)");
    choices.push_back("TriClinic -1");
    choices.push_back("Monoclinic 2/m");
    choices.push_back("OrthoRhombic mmm");
    choices.push_back("Tetragonal-Low 4/m");
    choices.push_back("Tetragonal-High 4/mmm");
    choices.push_back("Trigonal-Low -3");
    choices.push_back("Trigonal-High -3m");
    option->setChoices(choices);
    parameters.push_back(option);
  }
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
void VisualizeGBCD::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int VisualizeGBCD::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("MisorientationAngle", getMisAngle() );
  writer->writeValue("MisorientationAxis", getMisAxis() );
  writer->writeValue("OutputFile", getOutputFile() );
  writer->writeValue("GMTOutputFile", getGMTOutputFile() );
  writer->writeValue("CrystalStructure", getCrystalStructure() );
      writer->closeFilterGroup();
    return ++index; // we want to return the next index that was just written to
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
  if(getGMTOutputFile().empty() == true)
  {
    ss.str("");
    ss << ClassName() << " needs the GMT Data Output File Set and it was not.";
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
    notifyErrorMessage("The SurfaceMeshing DataContainer Object was NULL", -998);
    return;
  }
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The VoxelDataContainer DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  notifyStatusMessage("Initializing GBCD Visualization Data");

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

  //Original Ranges from Dave R.
  //m_GBCDlimits[0] = 0.0;
  //m_GBCDlimits[1] = cosf(1.0*m_pi);
  //m_GBCDlimits[2] = 0.0;
  //m_GBCDlimits[3] = 0.0;
  //m_GBCDlimits[4] = cosf(1.0*m_pi);
  //m_GBCDlimits[5] = 2.0*m_pi;
  //m_GBCDlimits[6] = cosf(0.0);
  //m_GBCDlimits[7] = 2.0*m_pi;
  //m_GBCDlimits[8] = 2.0*m_pi;
  //m_GBCDlimits[9] = cosf(0.0);

  //Greg's Ranges
  gbcdLimits[0] = 0.0;
  gbcdLimits[1] = 0.0;
  gbcdLimits[2] = 0.0;
  gbcdLimits[3] = 0.0;
  gbcdLimits[4] = 0.0;
  gbcdLimits[5] = m_pi/2.0;
  gbcdLimits[6] = 1.0;
  gbcdLimits[7] = m_pi/2.0;
  gbcdLimits[8] = 1.0;
  gbcdLimits[9] = 2.0*m_pi;

  //get num components of GBCD
  int numComp = sm->getEnsembleData(DREAM3D::EnsembleData::GBCD)->GetNumberOfComponents();
  //determine the size of the 1,3,4 dimensions of the GBCD - 2,4 are dim/1.5
  int dim = int(pow((numComp/(4.0)),(1.0/5.0))+0.5);

  gbcdSizes[0] = dim;
  gbcdSizes[1] = dim;
  gbcdSizes[2] = dim;
  gbcdSizes[3] = dim;
  gbcdSizes[4] = dim*4;

  float binsize = gbcdLimits[5]/float(gbcdSizes[0]);
  float binsize2 = binsize*(2.0/m_pi);
  gbcdDeltas[0] = binsize;
  gbcdDeltas[1] = binsize2;
  gbcdDeltas[2] = binsize;
  gbcdDeltas[3] = binsize;
  gbcdDeltas[4] = binsize2;

  float vec[3];
  float vec2[3];
  float rotNormal[3];
  float rotNormal2[3];
  float rotNormal_sc[2];
  float rotNormal2_sc[2];
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
  QuatF qtest;
  float w, n1, n2, n3;
  //  float dist;
  float theta, cosPhi, phi;

  m_MisAngle = m_MisAngle * m_pi/180.0f;
  //convert axis angle to matrix representation of misorientation
  OrientationMath::AxisAngletoMat(m_MisAngle, m_MisAxis.x, m_MisAxis.y, m_MisAxis.z, dg);
  //take inverse of misorientation variable to use for switching symmetry
  MatrixMath::Transpose3x3(dg, dgt);

  // Get our OrientationOps pointer for the selected crystal structure
  OrientationOps::Pointer orientOps = m_OrientationOps[m_CrystalStructure];

  //get number of symmetry operators
  int n_sym = orientOps->getNumSymOps();

  int xpoints = 100;
  int ypoints = 100;
  int zpoints = 1;
  int xpointshalf = xpoints/2;
  int ypointshalf = ypoints/2;
  float xres = 2.0/float(xpoints);
  float yres = 2.0/float(ypoints);
  float zres = (xres+yres)/2.0;
  float x, y, z;
  int xbin, ybin;
  float sum = 0;
  int count = 0;

  int shift1 = gbcdSizes[0];
  int shift2 = gbcdSizes[0]*gbcdSizes[1];
  int shift3 = gbcdSizes[0]*gbcdSizes[1]*gbcdSizes[2];
  int shift4 = gbcdSizes[0]*gbcdSizes[1]*gbcdSizes[2]*gbcdSizes[3];

  DoubleArrayType::Pointer poleFigureArray = DoubleArrayType::NullPointer();
  poleFigureArray = DoubleArrayType::CreateArray(xpoints*ypoints, 1, "PoleFigure");
  poleFigureArray->initializeWithValues(0);
  double* poleFigure = poleFigureArray->GetPointer(0);

#if WRITE_XYZ_POINTS
  std::vector<float> gmtValues;
#endif

  for (int64_t k = 0; k < ypoints; k++)
  {
    for (int64_t l = 0; l < xpoints; l++)
    {
      //get (x,y) for stereographic projection pixel
      x = float(k - xpointshalf) * xres + (xres / 2.0);
      y = float(l - ypointshalf) * yres + (yres / 2.0);
      if((x * x + y * y) <= 1.0)
      {
        sum = 0;
        count = 0;
        vec[2] = -((x * x + y * y) - 1) / ((x * x + y * y) + 1);
        vec[0] = x * (1 + vec[2]);
        vec[1] = y * (1 + vec[2]);
        MatrixMath::Multiply3x3with3x1(dgt, vec, vec2);

#if WRITE_XYZ_POINTS
        float lon = atan2(vec[1], vec[0]) * m_180Overpi;
        if (lon < 0.0)
        {
          lon =lon + 360.0;
        }
        float lat = asin(vec[2]) * m_180Overpi;
        gmtValues.push_back(lon);
        gmtValues.push_back(lat);
#endif

        // Loop over all the symetry operators in the given cystal symmetry
        for(int i=0; i<n_sym; i++)
        {
          //get symmetry operator1
          orientOps->getMatSymOp(i, sym1);
          for(int j=0; j<n_sym; j++)
          {
            //get symmetry operator2
            orientOps->getMatSymOp(j, sym2);
            MatrixMath::Transpose3x3(sym2,sym2t);
            //calculate symmetric misorientation
            MatrixMath::Multiply3x3with3x3(dg,sym2t,dg1);
            MatrixMath::Multiply3x3with3x3(sym1,dg1,dg2);
            //convert to euler angle
            OrientationMath::MatToEuler(dg2, mis_euler1[0], mis_euler1[1], mis_euler1[2]);
            if(mis_euler1[0] < m_piOver2 && mis_euler1[1] < m_piOver2 && mis_euler1[2] < m_piOver2)
            {
              mis_euler1[1] = cosf(mis_euler1[1]);
              //find bins in GBCD
              int location1 = int((mis_euler1[0]-gbcdLimits[0])/gbcdDeltas[0]);
              int location2 = int((mis_euler1[1]-gbcdLimits[1])/gbcdDeltas[1]);
              int location3 = int((mis_euler1[2]-gbcdLimits[2])/gbcdDeltas[2]);
              //find symmetric poles using the first symmetry operator
              MatrixMath::Multiply3x3with3x1(sym1, vec, rotNormal);
              //calculate the crystal normals in aspherical coordinates ->[theta, cos(phi) ]
              rotNormal_sc[0] = atan2f(rotNormal[1], rotNormal[0]);
              if (rotNormal_sc[0] < 0) rotNormal_sc[0] += m_pi2;
              rotNormal_sc[1] = rotNormal[2];
              //Note the switch to have theta in the 4 slot and cos(Phi) int he 3 slot
              int location4 = int((rotNormal_sc[1]-gbcdLimits[3])/gbcdDeltas[3]);
              int location5 = int((rotNormal_sc[0]-gbcdLimits[4])/gbcdDeltas[4]);
              sum += m_GBCD[(location5*shift4)+(location4*shift3)+(location3*shift2)+(location2*shift1)+location1];
              count++;
            }

            //again in second crystal reference frame
            //calculate symmetric misorientation
            MatrixMath::Multiply3x3with3x3(dgt,sym2,dg1);
            MatrixMath::Multiply3x3with3x3(sym1,dg1,dg2);
            //convert to euler angle
            OrientationMath::MatToEuler(dg2, mis_euler1[0], mis_euler1[1], mis_euler1[2]);
            if(mis_euler1[0] < m_piOver2 && mis_euler1[1] < m_piOver2 && mis_euler1[2] < m_piOver2)
            {
              mis_euler1[1] = cosf(mis_euler1[1]);
              //find bins in GBCD
              int location1 = int((mis_euler1[0]-gbcdLimits[0])/gbcdDeltas[0]);
              int location2 = int((mis_euler1[1]-gbcdLimits[1])/gbcdDeltas[1]);
              int location3 = int((mis_euler1[2]-gbcdLimits[2])/gbcdDeltas[2]);
              //find symmetric poles using the first symmetry operator
              MatrixMath::Multiply3x3with3x1(sym1, vec2, rotNormal2);
              //calculate the crystal normals in aspherical coordinates ->[theta, cos(phi) ]
              rotNormal2_sc[0] = atan2f(rotNormal2[1], rotNormal2[0]);
              if (rotNormal2_sc[0] < 0) rotNormal2_sc[0] += m_pi2;
              rotNormal2_sc[1] = rotNormal2[2];
              //Note the switch to have theta in the 4 slot and cos(Phi) int he 3 slot
              int location4 = int((rotNormal2_sc[1]-gbcdLimits[3])/gbcdDeltas[3]);
              int location5 = int((rotNormal2_sc[0]-gbcdLimits[4])/gbcdDeltas[4]);
              sum += m_GBCD[(location5*shift4)+(location4*shift3)+(location3*shift2)+(location2*shift1)+location1];
              count++;
            }
          }
        }
        poleFigure[(k*xpoints)+l] = sum/float(count);;
#if WRITE_XYZ_POINTS
        gmtValues.push_back(poleFigure[(k*xpoints)+l]);
#endif
      }
    }
  }

  notifyStatusMessage(ss.str());
#if WRITE_XYZ_POINTS

  // Write the GMT file
  if (verifyPathExists(getGMTOutputFile()) == true)
  {
    std::string parentPath = MXAFileInfo::parentPath(getGMTOutputFile());
    std::string basename = MXAFileInfo::fileNameWithOutExtension(getGMTOutputFile());
    std::string extension = MXAFileInfo::extension(getGMTOutputFile());
    std::string path = parentPath + MXAFileInfo::Separator + basename + std::string("_gmt_1.") + extension;

    ss.str("");
    ss << "Writing GMT Output File '" << path << "'";
    notifyStatusMessage(ss.str());

    FILE* f = fopen(path.c_str(), "wb");
    fprintf(f, "%.1f %.1f %.1f %.1f\n", m_MisAxis.x, m_MisAxis.y, m_MisAxis.z, m_MisAngle * m_180Overpi);
    size_t size = gmtValues.size()/3;

    for(size_t i = 0; i < size; i=i+3)
    {
      fprintf(f, "%f %f %f\n", gmtValues[i], gmtValues[i+1], gmtValues[i+2]);
    }
    fclose(f);
  }
#endif


  ss.str("");
  ss << "Generating data for VTK Polefigure File '" << getOutputFile() << "'";
  notifyStatusMessage(ss.str());

  if (verifyPathExists(getOutputFile()) == true)
  {
    ss.str("");
    ss << "Writing VTK Polefigure File '" << getOutputFile() << "'";
    notifyStatusMessage(ss.str());
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VisualizeGBCD::verifyPathExists(const std::string &file)
{
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  std::string parentPath = MXAFileInfo::parentPath(file);
  if(!MXADir::mkdir(parentPath, true))
  {
    std::stringstream ss;
    ss << "Error creating parent path '" << parentPath << "'";
    setErrorCondition(-998);
    notifyErrorMessage(ss.str(), getErrorCondition());
    return false;
  }
  return true;
}
