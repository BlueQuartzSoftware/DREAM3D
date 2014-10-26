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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "PoleFigureUtilities.h"

#include <QtCore/QFile>
#include <QtCore/QByteArray>
#include <QtCore/QTextStream>


#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/Common/ModifiedLambertProjection.h"


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
