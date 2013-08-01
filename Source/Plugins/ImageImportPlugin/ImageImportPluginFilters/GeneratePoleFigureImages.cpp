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
#include "GeneratePoleFigureImages.h"

#include <iostream>
#include <string>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QDebug>

#include <QtGui/QImage>
#include <QtGui/QColor>

#include "QtSupport/PoleFigureGeneration.h"

#include "MXA/MXA.h"
#include "MXA/Common/MXAEndian.h"
#include "MXA/Utilities/MXADir.h"

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/HKL/CtfReader.h"

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/StatsGen.hpp"
#include "DREAM3DLib/Common/Texture.hpp"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/Common/Texture.hpp"
#include "DREAM3DLib/Common/StatsGen.hpp"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GeneratePoleFigureImages::GeneratePoleFigureImages() :
  AbstractFilter(),
  m_ImagePrefix(""),
  m_OutputPath(""),
  m_ImageFormat(0),
  m_ImageSize(512),
  m_LambertSize(75),
  m_NumColors(32),
  m_EulersArrayName(DREAM3D::CellData::EulerAngles),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CellEulerAngles(NULL),
  m_CrystalStructures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GeneratePoleFigureImages::~GeneratePoleFigureImages()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  /* Place all your option initialization code here */
  /* To Display a Combobox with a list of current Voxel Cell Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Eulers Array");
    option->setPropertyName("EulersArrayName");
    option->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    parameters.push_back(option);
  }
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Image Format");
    parameter->setPropertyName("ImageFormat");
    parameter->setWidgetType(FilterParameter::ChoiceWidget);
    parameter->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("tif");
    choices.push_back("bmp");
    choices.push_back("png");
    //   choices.push_back("jpg");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  /* For String input use this code */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel(" Image Prefix");
    parameter->setPropertyName("ImagePrefix");
    parameter->setWidgetType(FilterParameter::StringWidget);
    parameter->setValueType("string");
    parameters.push_back(parameter);
  }
  /*   For an output path use this code*/
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output Path");
    option->setPropertyName("OutputPath");
    option->setWidgetType(FilterParameter::OutputPathWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setPropertyName("ImageSize");
    option->setHumanLabel("Image Size (Square)");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Pixels");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setPropertyName("LambertSize");
    option->setHumanLabel("Lambert Image Size");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setPropertyName("NumColors");
    option->setHumanLabel("Number of Colors");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }

    setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::writeFilterParameters(AbstractFilterParametersWriter* writer)

{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  writer->writeValue("ImagePrefix", getImagePrefix() );
  writer->writeValue("OutputPath", getOutputPath() );
  writer->writeValue("EulersArrayName", getEulersArrayName() );
  writer->writeValue("ImageFormat", getImageFormat() );
  writer->writeValue("ImageSize", getImageSize() );
  writer->writeValue("LabertSize", getLambertSize() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  /* Example code for preflighting looking for a valid string for the output file
   * but not necessarily the fact that the file exists: Example code to make sure
   * we have something in a string before proceeding.*/

  if (m_OutputPath.empty() == true)
  {
    setErrorCondition(-1003);
    addErrorMessage(getHumanLabel(), "Output Directory is Not set correctly", getErrorCondition());
  }

  else if (MXADir::exists(m_OutputPath) == false)
  {
    ss.str("");
    ss <<  "The directory path for the output file does not exist. DREAM3D will attempt to create this path during execution of the filter.";
    addWarningMessage(getHumanLabel(), ss.str(), -1);
  }

  if(m_EulersArrayName.empty() == true)
  {
    setErrorCondition(-1004);
    addErrorMessage(getHumanLabel(), "Input Euler Array name is empty", getErrorCondition());
  }
  else
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, -300, float, FloatArrayType, voxels, 3)
        typedef DataArray<unsigned int> XTalStructArrayType;
    GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1)
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::execute()
{
  int err = 0;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Voxel DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  /* Place all your code to execute your filter here. */
  size_t dims[3];
  m->getDimensions(dims);

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());

  FloatArrayType* eulers = FloatArrayType::SafePointerDownCast(m->getCellData(getCellEulerAnglesArrayName()).get());


  if ( Ebsd::CrystalStructure::Check::IsCubic(m_CrystalStructures[1]))
  {
    generateCubicPoleFigures(eulers);
  }
  else if (Ebsd::CrystalStructure::Check::IsHexagonal(m_CrystalStructures[1]))
  {

  }



  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ModifiedLambertProjection::Pointer GeneratePoleFigureImages::createModifiedLambertProjection(FloatArrayType *coords, int dimension, float resolution)
{
  notifyStatusMessage("Creating Modified Lambert Projections");
  size_t npoints = coords->GetNumberOfTuples();
  bool nhCheck = false;
  float sqCoord[2];

  ModifiedLambertProjection::Pointer squareProj = ModifiedLambertProjection::New();



  squareProj->initializeSquares(dimension, resolution, 1.0f);
  int sqIndex = 0;
  for(int i = 0; i < npoints; ++i)
  {
    sqCoord[0] = 0.0; sqCoord[1] = 0.0;
    //get coordinates in square projection of crystal normal parallel to boundary normal
    nhCheck = squareProj->getSquareCoord(coords->GetPointer(i * 3), sqCoord);
    sqIndex = squareProj->getSquareIndex(sqCoord);
    if (nhCheck == true)
    {
      //north increment by 1
      squareProj->addValue(ModifiedLambertProjection::NorthSquare, sqIndex, 1.0);
    }
    else
    {
      // south increment by 1
      squareProj->addValue(ModifiedLambertProjection::SouthSquare, sqIndex, 1.0);
    }
  }

  // squareProj->normalizeSquares();

  return squareProj;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DoubleArrayType::Pointer GeneratePoleFigureImages::createPoleFigure(ModifiedLambertProjection::Pointer proj, int poleFigureDim)
{
  notifyStatusMessage("Creating Pole Figures from Modified Lambert Projections");
  int xpoints = poleFigureDim;
  int ypoints = poleFigureDim;

  int xpointshalf = xpoints / 2;
  int ypointshalf = ypoints / 2;

  float xres = 2.0 / (float)(xpoints);
  float yres = 2.0 / (float)(ypoints);
  float xtmp, ytmp;
  float sqCoord[2];
  float xyz[3];
  bool nhCheck = false;

  DoubleArrayType::Pointer intensityPtr = DoubleArrayType::CreateArray(xpoints * ypoints, 1, "Intensity_Image");
  intensityPtr->initializeWithZeros();
  double* intensity = intensityPtr->GetPointer(0);

  for (int64_t k = 0; k < (xpoints); k++)
  {
    for (int64_t l = 0; l < (ypoints); l++)
    {
      //get (x,y) for stereographic projection pixel
      xtmp = float(k-xpointshalf)*xres+(xres/2.0);
      ytmp = float(l-ypointshalf)*yres+(yres/2.0);
      if((xtmp*xtmp+ytmp*ytmp) <= 1.0)
      {
        xyz[2] = -((xtmp*xtmp+ytmp*ytmp)-1)/((xtmp*xtmp+ytmp*ytmp)+1);
        xyz[0] = xtmp*(1+xyz[2]);
        xyz[1] = ytmp*(1+xyz[2]);

        int index = l * xpoints + k;
        for( int64_t m = 0; m < 2; m++)
        {
          if(m == 1) MatrixMath::Multiply3x1withConstant(xyz, -1.0);
          nhCheck = proj->getSquareCoord(xyz, sqCoord);
          //   int sqIndex = proj->getSquareIndex(sqCoord);

          if (nhCheck == true)
          {
            //get Value from North square
            //          intensity[index] = proj->getValue(ModifiedLambertProjection::NorthSquare, sqIndex);
            intensity[index] += proj->getInterpolatedValue(ModifiedLambertProjection::NorthSquare, sqCoord);
          }
          else
          {
            //get Value from South square
            //          intensity[index] = proj->getValue(ModifiedLambertProjection::SouthSquare, sqIndex);
            intensity[index] += proj->getInterpolatedValue(ModifiedLambertProjection::SouthSquare, sqCoord);
          }
        }
        intensity[index] /= 2.0;

      }

    }
  }
  return intensityPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::generateCubicSphereCoordsFromEulers(FloatArrayType *eulers, FloatArrayType *xyz001, FloatArrayType *xyz011, FloatArrayType *xyz111)
{
  notifyStatusMessage("Calculating Euler Angle Projection to Unit Sphere");
  size_t nOrientations = eulers->GetNumberOfTuples();
  QuaternionMath<float>::Quaternion q1;
  CubicOps ops;
  float g[3][3];
  float* currentEuler = NULL;
  float direction[3] = {0.0, 0.0, 0.0};

  for(size_t i = 0; i < nOrientations; ++i)
  {

    currentEuler = eulers->GetPointer(i * 3);

    OrientationMath::EulertoQuat(q1, currentEuler);
    ops.getFZQuat(q1);
    OrientationMath::QuattoMat(q1, g);

    // -----------------------------------------------------------------------------
    // 001 Family

    direction[0] = 1.0; direction[1] = 0.0; direction[2] = 0.0;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz001->GetPointer(i*9));
    direction[0] = 0.0; direction[1] = 1.0; direction[2] = 0.0;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz001->GetPointer(i*9 + 3));
    direction[0] = 0.0; direction[1] = 0.0; direction[2] = 1.0;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz001->GetPointer(i*9 + 6));

    // -----------------------------------------------------------------------------
    // 011 Family

    direction[0] = DREAM3D::Constants::k_1OverRoot2; direction[1] = DREAM3D::Constants::k_1OverRoot2; direction[2] = 0.0;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz011->GetPointer(i*18 + 0));
    direction[0] = DREAM3D::Constants::k_1OverRoot2; direction[1] = 0.0; direction[2] = DREAM3D::Constants::k_1OverRoot2;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz011->GetPointer(i*18 + 3));
    direction[0] = 0.0; direction[1] = DREAM3D::Constants::k_1OverRoot2; direction[2] = DREAM3D::Constants::k_1OverRoot2;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz011->GetPointer(i*18 + 6));
    direction[0] = -DREAM3D::Constants::k_1OverRoot2; direction[1] = -DREAM3D::Constants::k_1OverRoot2; direction[2] = 0.0;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz011->GetPointer(i*18 + 9));
    direction[0] = -DREAM3D::Constants::k_1OverRoot2; direction[1] = 0.0; direction[2] = DREAM3D::Constants::k_1OverRoot2;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz011->GetPointer(i*18 + 12));
    direction[0] = 0.0; direction[1] = -DREAM3D::Constants::k_1OverRoot2; direction[2] = DREAM3D::Constants::k_1OverRoot2;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz011->GetPointer(i*18 + 15));

    // -----------------------------------------------------------------------------
    // 111 Family

    direction[0] = DREAM3D::Constants::k_1OverRoot3; direction[1] = DREAM3D::Constants::k_1OverRoot3; direction[2] = DREAM3D::Constants::k_1OverRoot3;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz111->GetPointer(i*12 + 0));
    direction[0] = -DREAM3D::Constants::k_1OverRoot3; direction[1] = DREAM3D::Constants::k_1OverRoot3; direction[2] = DREAM3D::Constants::k_1OverRoot3;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz111->GetPointer(i*12 + 3));
    direction[0] = DREAM3D::Constants::k_1OverRoot3; direction[1] = -DREAM3D::Constants::k_1OverRoot3; direction[2] = DREAM3D::Constants::k_1OverRoot3;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz111->GetPointer(i*12 + 6));
    direction[0] = DREAM3D::Constants::k_1OverRoot3; direction[1] = DREAM3D::Constants::k_1OverRoot3; direction[2] = -DREAM3D::Constants::k_1OverRoot3;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz111->GetPointer(i*12 + 9));
  }

}



/**
 * @brief This function writes a set of Axis coordinates to that are needed
 * for a Rectilinear Grid based data set.
 * @param f The "C" FILE* pointer to the file being written to.
 * @param axis The name of the Axis that is being written
 * @param type The type of primitive being written (float, int, ...)
 * @param npoints The total number of points in the array
 * @param min The minimum value of the axis
 * @param max The maximum value of the axis
 * @param step The step value between each point on the axis.
 */

int GeneratePoleFigureImages::writeCoords(FILE* f, const char* axis, const char* type, int64_t npoints, float min, float step)
{
  int err = 0;
  fprintf(f, "%s %lld %s\n", axis, npoints, type);
  float* data = new float[npoints];
  float d;
  for (int idx = 0; idx < npoints; ++idx)
  {
    d = idx * step + min;
    MXA::Endian::FromSystemToBig::convert<float>(d);
    data[idx] = d;
  }
  size_t totalWritten = fwrite(static_cast<void*>(data), sizeof(float), static_cast<size_t>(npoints), f);
  delete[] data;
  if (totalWritten != static_cast<size_t>(npoints) )
  {
    std::cout << "Error Writing Binary VTK Data into file " << std::endl;
    fclose(f);
    return -1;
  }
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::writeVtkFile(const std::string filename,  DoubleArrayType *poleFigurePtr, int dimension)
{

  notifyStatusMessage("Writing VTK File");
  double* poleFigure = poleFigurePtr->GetPointer(0);
  int xpoints = dimension;
  int ypoints = dimension;
  int zpoints = 1;
  float xres = 2.0 / (float)(xpoints);
  float yres = 2.0 / (float)(ypoints);
  float zres = (xres+yres)/2.0;

  std::stringstream ss;
  FILE* f = NULL;
  f = fopen(filename.c_str(), "wb");
  if(NULL == f)
  {

    ss.str("");
    ss << "Could not open GBCD viz file " << filename << " for writing. Please check access permissions and the path to the output location exists";
    //  notifyErrorMessage(ss.str(), getErrorCondition());
    std::cout << ss.str() << std::endl;
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
      std::cout << "Error Writing Binary VTK Data into file " << filename << std::endl;
      fclose(f);
    }
  }

  fclose(f);
}


///getColorCorrespondingToValue ////////////////////////////////////////////////
//
// Assumes you've already generated min and max -- the extrema for the data
// to which you're applying the color map. Then define the number of colorNodes
// and make sure there's a row of three float values (representing r, g, and b
// in a 0.0-1.0 range) for each node. Then call this method for with parameter
// val some float value between min and max inclusive. The corresponding rgb
// values will be returned in the reference-to-float parameters r, g, and b.
//
////////////////////////////////////////////////////////////////////////////////
void GeneratePoleFigureImages::getColorCorrespondingTovalue(float val,
                                                            float &r, float &g, float &b,
                                                            float max, float min)
{
  static const int numColorNodes = 8;
  float color[numColorNodes][3] =
  {
    {0.0f, 1.0f/255.0f, 253.0f/255.0f},    // blue
    {105.0f/255.0f, 145.0f/255.0f, 2.0f/255.0f},    // yellow
    {1.0f/255.0f, 255.0f/255.0f, 29.0f/255.0f},    // Green
    {180.0f/255.0f, 255.0f/255.0f, 0.0f/255.0f},
    {255.0f/255.0f, 215.0f/255.0f, 6.0f/255.0f},
    {255.0f/255.0f, 143.0f/255.0f, 1.0f/255.0f},
    {255.0f/255.0f, 69.0f/255.0f, 0.0f/255.0f},
    {253.0f/255.0f, 1.0f/255.0f, 0.0f/255.0f}     // red
  };
  float range = max - min;
  for (int i = 0; i < (numColorNodes - 1); i++)
  {
    float currFloor = min + ((float)i / (numColorNodes - 1)) * range;
    float currCeil = min + ((float)(i + 1) / (numColorNodes - 1)) * range;

    if((val >= currFloor) && (val <= currCeil))
    {
      float currFraction = (val - currFloor) / (currCeil - currFloor);
      r = color[i][0] * (1.0 - currFraction) + color[i + 1][0] * currFraction;
      g = color[i][1] * (1.0 - currFraction) + color[i + 1][1] * currFraction;
      b = color[i][2] * (1.0 - currFraction) + color[i + 1][2] * currFraction;
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::writeImage(const std::string filename, DoubleArrayType *poleFigurePtr, int dimension)
{
  notifyStatusMessage("Writing Image");

  size_t npoints = poleFigurePtr->GetNumberOfTuples();
  double max = std::numeric_limits<double>::min();
  double min = std::numeric_limits<double>::max();
  double value = 0.0;
  for(size_t i = 0; i < npoints; ++i)
  {
    value = poleFigurePtr->GetValue(i);
    if (value > max) { max = value;}
    if (value < min) { min = value;}
  }


  int imageWidth = dimension;
  int imageHeight = dimension;
  // This figures out if we are "inside" the circle or not
  float radSqrd = (float)(imageWidth/2.0f) * (imageWidth/2.0f);
  float cX = imageWidth/2.0f;  // Center x
  float cY = imageHeight/2.0f; // Center y
  float delta = 0.0;

  //if (max < 14) { max = 14; }
  QImage image (imageWidth, imageHeight, QImage::Format_ARGB32_Premultiplied);
  image.fill(0);
 // qint32 numColors = max + 1;
  qint32 numColors = getNumColors() + 1;
  qint32 colorRange = getNumColors();

  QVector<QColor> colorTable(numColors);
  qint32 range = max - min;

  float r, g, b;
  for (int i = 0; i < getNumColors(); i++)
  {
    //int val = min + ((float)i / numColors) * range;
    int val = ((float)i / (float)numColors) * colorRange;
    getColorCorrespondingTovalue(i, r, g, b, getNumColors(), 0);
    colorTable[i] = QColor(r*255, g*255, b*255, 255);
  }
  // Index 0 is all white which is every pixel outside of the Pole Figure circle
  colorTable[getNumColors()] = QColor(255, 255, 255, 255);

  for (int yCoord = 0; yCoord < imageHeight; ++yCoord)
  {
    for (int xCoord = 0; xCoord < imageWidth; ++xCoord)
    {
      delta = (xCoord-cX)*(xCoord-cX) + (yCoord-cY)*(yCoord-cY);
      if (delta > radSqrd)
      {
        image.setPixel(xCoord, yCoord, colorTable[getNumColors()].rgba());
      }
      else
      {
        quint32 colorIndex = quint32(poleFigurePtr->GetValue(yCoord * imageWidth + xCoord));
        colorIndex = (colorIndex-min) / (max - min) * getNumColors();

        image.setPixel(xCoord, yCoord, colorTable[colorIndex].rgba());
      }
    }
  }

  // Flip the image so the (-1, -1) is in the lower left
  image = image.mirrored(true, false);
  bool saved = image.save(QString::fromStdString(filename));
  if(!saved)
  {
    std::cout << "did NOT save QImage at path: " << filename << std::endl;
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::generateCubicPoleFigures(FloatArrayType* eulers)
{
  notifyStatusMessage("Generating Cubic Based Pole Figures for <001>, <011> & <111>");
  int numOrientations = eulers->GetNumberOfTuples();


  // Create an Array to hold the XYZ Coordinates which are the coords on the sphere.
  // this is size for CUBIC ONLY, <001> Family
  FloatArrayType::Pointer xyz001 = FloatArrayType::CreateArray(numOrientations * 3, 3, "TEMP_<001>_xyzCoords");
  // this is size for CUBIC ONLY, <011> Family
  FloatArrayType::Pointer xyz011 = FloatArrayType::CreateArray(numOrientations * 6, 3, "TEMP_<011>_xyzCoords");
  // this is size for CUBIC ONLY, <111> Family
  FloatArrayType::Pointer xyz111 = FloatArrayType::CreateArray(numOrientations * 4, 3, "TEMP_<111>_xyzCoords");

  int dimension = getLambertSize();
  float resolution = sqrt(M_PI*0.5) * 2.0 / (float)(dimension);
  int poleFigureDim = getImageSize();

  // Generate the coords on the sphere
  generateCubicSphereCoordsFromEulers(eulers, xyz001.get(), xyz011.get(), xyz111.get());



  // Generate the modified Lambert projection images (Squares, 2 of them, 1 for northern hemisphere, 1 for southern hemisphere
  ModifiedLambertProjection::Pointer lambert = createModifiedLambertProjection(xyz001.get(), dimension, resolution);
  // Now create the intensity image that will become the actual Pole figure image
  DoubleArrayType::Pointer poleFigurePtr = createPoleFigure(lambert, poleFigureDim);
  QString path = generateVtkPath("001");
  writeVtkFile(path.toStdString(), poleFigurePtr.get(), poleFigureDim);
  path = generateImagePath("001");
  writeImage(path.toStdString(), poleFigurePtr.get(), poleFigureDim);

  // Generate the <011> pole figure
  lambert = createModifiedLambertProjection(xyz011.get(), dimension, resolution);
  poleFigurePtr = createPoleFigure(lambert, poleFigureDim);
  path = generateVtkPath("011");
  writeVtkFile(path.toStdString(), poleFigurePtr.get(), poleFigureDim);
  path = generateImagePath("011");
  writeImage(path.toStdString(), poleFigurePtr.get(), poleFigureDim);

  // Generate the <111> pole figure
  lambert = createModifiedLambertProjection(xyz111.get(), dimension, resolution);
  poleFigurePtr = createPoleFigure(lambert, poleFigureDim);
  path = generateVtkPath("111");
  writeVtkFile(path.toStdString(), poleFigurePtr.get(), poleFigureDim);
  path = generateImagePath("111");
  writeImage(path.toStdString(), poleFigurePtr.get(), poleFigureDim);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GeneratePoleFigureImages::generateImagePath( QString label)
{
  QString path = QString::fromStdString(m_OutputPath) + QDir::separator() + QString::fromStdString(m_ImagePrefix) + label;
  if(m_ImageFormat == TifImageType)
  {
    path.append(".tif");
  }
  else if (m_ImageFormat == BmpImageType)
  {
    path.append(".bmp");
  }
  else if (m_ImageFormat == PngImageType)
  {
    path.append(".png");
  }
  else if (m_ImageFormat == JpgImageType)
  {
    path.append(".jpg");
  }
  path = QDir::toNativeSeparators(path);
  QFileInfo fi(path);
  QDir parent(fi.absolutePath());
  if (parent.exists() == false)
  {
    parent.mkpath(fi.absolutePath());
  }

  return path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GeneratePoleFigureImages::generateVtkPath( QString label)
{
  QString path = QString::fromStdString(m_OutputPath) + QDir::separator() + QString::fromStdString(m_ImagePrefix) + label;

  path.append(".vtk");

  path = QDir::toNativeSeparators(path);
  QFileInfo fi(path);
  QDir parent(fi.absolutePath());
  if (parent.exists() == false)
  {
    parent.mkpath(fi.absolutePath());
  }

  return path;
}
