/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "PoleFigureUtilities.h"

#include <QtCore/QFile>
#include <QtCore/QByteArray>
#include <QtCore/QTextStream>

#include "SIMPLib/Utilities/ColorTable.h"


#include "OrientationLib/Utilities/ModifiedLambertProjection.h"
#include "OrientationLib/SpaceGroupOps/CubicOps.h"
#include "OrientationLib/SpaceGroupOps/HexagonalOps.h"
#include "OrientationLib/SpaceGroupOps/OrthoRhombicOps.h"



#define WRITE_XYZ_SPHERE_COORD_VTK 0
#define WRITE_LAMBERT_SQUARES 0

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PoleFigureUtilities::PoleFigureUtilities()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PoleFigureUtilities::~PoleFigureUtilities()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int writeVtkFile(FloatArrayType* xyz, const QString& filename)
{

  QFile in(filename);
  if (!in.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    return -100;
  }

  QTextStream ss(&in);
  ss << "# vtk DataFile Version 2.0\n";
  ss << "data set from DREAM3D\n";
  ss << "ASCII";
  ss << "\n";

  ss << "DATASET UNSTRUCTURED_GRID\nPOINTS " << xyz->getNumberOfTuples() << " float\n";
  size_t count = xyz->getNumberOfTuples();
  for(size_t i = 0; i < count; ++i)
  {
    ss << xyz->getComponent(i, 0) << " " << xyz->getComponent(i, 1) << " " << xyz->getComponent(i, 2) << "\n";
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UInt8ArrayType::Pointer PoleFigureUtilities::CreateColorImage(DoubleArrayType* data, int width, int height, int nColors, const QString& name, double min, double max)
{
  QVector<size_t> dims(1, 4);
  UInt8ArrayType::Pointer image = UInt8ArrayType::CreateArray(width * height, dims, name);
  PoleFigureConfiguration_t config;
  config.imageDim = width;
  config.numColors = nColors;
  config.eulers = NULL;
  config.minScale = min;
  config.maxScale = max;
  config.sphereRadius = 1.0;
  PoleFigureUtilities::CreateColorImage(data, config, image.get());
  return image;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PoleFigureUtilities::CreateColorImage(DoubleArrayType* data, PoleFigureConfiguration_t& config, UInt8ArrayType* image)
{
  int width = config.imageDim ;
  int height = config.imageDim ;

  int halfWidth = width / 2;
  int halfHeight = height / 2;

  float xres = 2.0 / (float)(width);
  float yres = 2.0 / (float)(height);
  float xtmp, ytmp;

  float max = static_cast<float>(config.maxScale);
  float min = static_cast<float>(config.minScale);

  // Initialize the image with all zeros
  image->initializeWithZeros();
  uint32_t* rgbaPtr = reinterpret_cast<uint32_t*>(image->getPointer(0));

  int numColors = config.numColors;
  QVector<float> colors(numColors * 3, 0.0);
  SIMPLColorTable::GetColorTable(config.numColors, colors);

  float r = 0.0, g = 0.0, b = 0.0;

  double* dataPtr = data->getPointer(0);
  size_t idx = 0;
  double value;
  int bin;
  for (int64_t y = 0; y < height; y++)
  {
    for (int64_t x = 0; x < width; x++)
    {
      xtmp = float(x - halfWidth) * xres + (xres * 0.5);
      ytmp = float(y - halfHeight) * yres + (yres * 0.5);
      idx = (width * y) + x;
      if( ( xtmp * xtmp + ytmp * ytmp) <= 1.0) // Inside the circle
      {
        value = dataPtr[y * width + x];
        value = (value - min) / (max - min);
        bin = int(value * numColors);
        if(bin > numColors - 1)
        {
          bin = numColors - 1;
        }
        if (bin < 0 || bin >= colors.size())
        {
          r = 0x00;
          b = 0x00;
          g = 0x00;
        }
        else
        {
          r = colors[3 * bin];
          g = colors[3 * bin + 1];
          b = colors[3 * bin + 2];
        }
        rgbaPtr[idx] = RgbColor::dRgb(r * 255, g * 255, b * 255, 255);
      }
      else // Outside the Circle - Set pixel to White
      {
        rgbaPtr[idx] = 0xFFFFFFFF; // White
      }
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PoleFigureUtilities::GenerateHexPoleFigures(FloatArrayType* eulers, int lambertDimension, int poleFigureDim,
                                                 DoubleArrayType::Pointer& intensity0001,
                                                 DoubleArrayType::Pointer& intensity1010,
                                                 DoubleArrayType::Pointer& intensity1120)
{
  int numOrientations = eulers->getNumberOfTuples();


  // Create an Array to hold the XYZ Coordinates which are the coords on the sphere.
  // this is size for HEX ONLY, <0001> Family
  QVector<size_t> dims(1, 3);
  FloatArrayType::Pointer xyz0001 = FloatArrayType::CreateArray(numOrientations * 2, dims, "TEMP_<0001>_xyzCoords");
  // this is size for HEX ONLY, <1010> Family
  FloatArrayType::Pointer xyz1010 = FloatArrayType::CreateArray(numOrientations * 6, dims, "TEMP_<1010>_xyzCoords");
  // this is size for HEX ONLY, <1120> Family
  FloatArrayType::Pointer xyz1120 = FloatArrayType::CreateArray(numOrientations * 6, dims, "TEMP_<1120>_xyzCoords");



  float sphereRadius = 1.0f;

  // Generate the coords on the sphere
  HexagonalOps ops;
  ops.generateSphereCoordsFromEulers(eulers, xyz0001.get(), xyz1010.get(), xyz1120.get());
#if WRITE_XYZ_SPHERE_COORD_VTK
  writeVtkFile(xyz0001.get(), "c:/Users/GroebeMA/Desktop/Sphere_XYZ_FROM_EULER_0001.vtk");
  writeVtkFile(xyz1010.get(), "c:/Users/GroebeMA/Desktop/Sphere_XYZ_FROM_EULER_1010.vtk");
  writeVtkFile(xyz1120.get(), "c:/Users/GroebeMA/Desktop/Sphere_XYZ_FROM_EULER_1120.vtk");
#endif

  // Generate the modified Lambert projection images (Squares, 2 of them, 1 for northern hemisphere, 1 for southern hemisphere
  ModifiedLambertProjection::Pointer lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz0001.get(), lambertDimension, sphereRadius);
  // Now create the intensity image that will become the actual Pole figure image
  DoubleArrayType::Pointer poleFigurePtr = lambert->createStereographicProjection(poleFigureDim);
  poleFigurePtr->setName("PoleFigure_<0001>");
  intensity0001.swap(poleFigurePtr);

#if WRITE_LAMBERT_SQUARES
  size_t dims[3] = {lambert->getDimension(), lambert->getDimension(), 1 };
  float res[3] = {lambert->getStepSize(), lambert->getStepSize(), lambert->getStepSize() };
  DoubleArrayType::Pointer north = lambert->getNorthSquare();
  DoubleArrayType::Pointer south = lambert->getSouthSquare();
  VtkRectilinearGridWriter::WriteDataArrayToFile("/tmp/ModifiedLambert_North.vtk", north.get(), dims, res, "double", true);
  VtkRectilinearGridWriter::WriteDataArrayToFile("/tmp/ModifiedLambert_South.vtk", south.get(), dims, res, "double", true);
#endif


// Generate the <011> pole figure which will generate a new set of Lambert Squares
  lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz1010.get(), lambertDimension, sphereRadius);
  poleFigurePtr = lambert->createStereographicProjection(poleFigureDim);
  poleFigurePtr->setName("PoleFigure_<1010>");
  intensity1010.swap(poleFigurePtr);

  // Generate the <111> pole figure which will generate a new set of Lambert Squares
  lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz1120.get(), lambertDimension, sphereRadius);
  poleFigurePtr = lambert->createStereographicProjection(poleFigureDim);
  poleFigurePtr->setName("PoleFigure_<1120>");
  intensity1120.swap(poleFigurePtr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PoleFigureUtilities::GenerateOrthoPoleFigures(FloatArrayType* eulers, int lambertDimension, int poleFigureDim,
                                                   DoubleArrayType::Pointer& intensity100,
                                                   DoubleArrayType::Pointer& intensity010,
                                                   DoubleArrayType::Pointer& intensity001)
{
  int numOrientations = eulers->getNumberOfTuples();


  // Create an Array to hold the XYZ Coordinates which are the coords on the sphere.
  // this is size for ORTHO ONLY, <100> Family
  QVector<size_t> dims(1, 3);
  FloatArrayType::Pointer xyz100 = FloatArrayType::CreateArray(numOrientations * 2, dims, "TEMP_<100>_xyzCoords");
  // this is size for ORTHO ONLY, <010> Family
  FloatArrayType::Pointer xyz010 = FloatArrayType::CreateArray(numOrientations * 2, dims, "TEMP_<010>_xyzCoords");
  // this is size for ORTHO ONLY, <001> Family
  FloatArrayType::Pointer xyz001 = FloatArrayType::CreateArray(numOrientations * 2, dims, "TEMP_<001>_xyzCoords");



  float sphereRadius = 1.0f;

  // Generate the coords on the sphere
  OrthoRhombicOps ops;
  ops.generateSphereCoordsFromEulers(eulers, xyz100.get(), xyz010.get(), xyz001.get());
#if WRITE_XYZ_SPHERE_COORD_VTK
  writeVtkFile(xyz100.get(), "c:/Users/GroebeMA/Desktop/Sphere_XYZ_FROM_EULER_100.vtk");
  writeVtkFile(xyz010.get(), "c:/Users/GroebeMA/Desktop/Sphere_XYZ_FROM_EULER_010.vtk");
  writeVtkFile(xyz001.get(), "c:/Users/GroebeMA/Desktop/Sphere_XYZ_FROM_EULER_001.vtk");
#endif

  // Generate the modified Lambert projection images (Squares, 2 of them, 1 for northern hemisphere, 1 for southern hemisphere
  ModifiedLambertProjection::Pointer lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz100.get(), lambertDimension, sphereRadius);
  // Now create the intensity image that will become the actual Pole figure image
  DoubleArrayType::Pointer poleFigurePtr = lambert->createStereographicProjection(poleFigureDim);
  poleFigurePtr->setName("PoleFigure_<100>");
  intensity100.swap(poleFigurePtr);

#if WRITE_LAMBERT_SQUARES
  size_t dims[3] = {lambert->getDimension(), lambert->getDimension(), 1 };
  float res[3] = {lambert->getStepSize(), lambert->getStepSize(), lambert->getStepSize() };
  DoubleArrayType::Pointer north = lambert->getNorthSquare();
  DoubleArrayType::Pointer south = lambert->getSouthSquare();
  VtkRectilinearGridWriter::WriteDataArrayToFile("/tmp/ModifiedLambert_North.vtk", north.get(), dims, res, "double", true);
  VtkRectilinearGridWriter::WriteDataArrayToFile("/tmp/ModifiedLambert_South.vtk", south.get(), dims, res, "double", true);
#endif


// Generate the <011> pole figure which will generate a new set of Lambert Squares
  lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz010.get(), lambertDimension, sphereRadius);
  poleFigurePtr = lambert->createStereographicProjection(poleFigureDim);
  poleFigurePtr->setName("PoleFigure_<010>");
  intensity010.swap(poleFigurePtr);

  // Generate the <111> pole figure which will generate a new set of Lambert Squares
  lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz001.get(), lambertDimension, sphereRadius);
  poleFigurePtr = lambert->createStereographicProjection(poleFigureDim);
  poleFigurePtr->setName("PoleFigure_<001>");
  intensity001.swap(poleFigurePtr);
}
