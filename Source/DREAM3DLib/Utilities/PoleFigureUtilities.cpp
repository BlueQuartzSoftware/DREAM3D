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
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/IOFilters/VtkRectilinearGridWriter.h"


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
void writeVtkFile(FloatArrayType* xyz, const std::string &filename)
{
  std::stringstream ss;
  FILE* f = NULL;
  f = fopen(filename.c_str(), "wb");
  if(NULL == f)
  {

    ss.str("");
    ss << "Could not open vtk viz file " << filename << " for writing. Please check access permissions and the path to the output location exists";
    return;
  }

  // Write the correct header
  fprintf(f, "# vtk DataFile Version 2.0\n");
  fprintf(f, "data set from DREAM3D\n");
  fprintf(f, "ASCII");
  fprintf(f, "\n");

  fprintf(f, "DATASET UNSTRUCTURED_GRID\nPOINTS %d float\n", xyz->GetNumberOfTuples() );
  size_t count = xyz->GetNumberOfTuples();
  for(int i = 0; i < count; ++i)
  {
    fprintf(f, "%f %f %f\n", xyz->GetComponent(i, 0), xyz->GetComponent(i, 1), xyz->GetComponent(i, 2));
  }
  fclose(f);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PoleFigureUtilities::GenerateCubicPoleFigures(FloatArrayType* eulers, int lambertDimension, int poleFigureDim,
                                              DoubleArrayType::Pointer &intensity001,
                                              DoubleArrayType::Pointer &intensity011,
                                              DoubleArrayType::Pointer &intensity111)
{
  int numOrientations = eulers->GetNumberOfTuples();


  // Create an Array to hold the XYZ Coordinates which are the coords on the sphere.
  // this is size for CUBIC ONLY, <001> Family
  FloatArrayType::Pointer xyz001 = FloatArrayType::CreateArray(numOrientations * 3, 3, "TEMP_<001>_xyzCoords");
  // this is size for CUBIC ONLY, <011> Family
  FloatArrayType::Pointer xyz011 = FloatArrayType::CreateArray(numOrientations * 6, 3, "TEMP_<011>_xyzCoords");
  // this is size for CUBIC ONLY, <111> Family
  FloatArrayType::Pointer xyz111 = FloatArrayType::CreateArray(numOrientations * 4, 3, "TEMP_<111>_xyzCoords");



  float sphereRadius = 1.0f;

  // Generate the coords on the sphere
  CubicOps ops;
  ops.generateSphereCoordsFromEulers(eulers, xyz001.get(), xyz011.get(), xyz111.get());
#if WRITE_XYZ_SPHERE_COORD_VTK
  writeVtkFile(xyz001.get(), "/tmp/Sphere_XYZ_FROM_EULER_001.vtk");
  writeVtkFile(xyz001.get(), "/tmp/Sphere_XYZ_FROM_EULER_011.vtk");
  writeVtkFile(xyz001.get(), "/tmp/Sphere_XYZ_FROM_EULER_111.vtk");
#endif

  // Generate the modified Lambert projection images (Squares, 2 of them, 1 for northern hemisphere, 1 for southern hemisphere
  ModifiedLambertProjection::Pointer lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz001.get(), lambertDimension, sphereRadius);
  // Now create the intensity image that will become the actual Pole figure image
  DoubleArrayType::Pointer poleFigurePtr = lambert->createStereographicProjection(poleFigureDim);
  poleFigurePtr->SetName("PoleFigure_<001>");
  intensity001.swap(poleFigurePtr);

#if WRITE_LAMBERT_SQUARES
  size_t dims[3] = {lambert->getDimension(), lambert->getDimension(), 1 };
  float res[3] = {lambert->getStepSize(), lambert->getStepSize(), lambert->getStepSize() };
  DoubleArrayType::Pointer north = lambert->getNorthSquare();
  DoubleArrayType::Pointer south = lambert->getSouthSquare();
  VtkRectilinearGridWriter::WriteDataArrayToFile("/tmp/ModifiedLambert_North.vtk", north.get(), dims, res, "double", true);
  VtkRectilinearGridWriter::WriteDataArrayToFile("/tmp/ModifiedLambert_South.vtk", south.get(), dims, res, "double", true);
#endif


// Generate the <011> pole figure which will generate a new set of Lambert Squares
  lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz011.get(), lambertDimension, sphereRadius);
  poleFigurePtr = lambert->createStereographicProjection(poleFigureDim);
  poleFigurePtr->SetName("PoleFigure_<011>");
  intensity011.swap(poleFigurePtr);

    // Generate the <111> pole figure which will generate a new set of Lambert Squares
  lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz111.get(), lambertDimension, sphereRadius);
  poleFigurePtr = lambert->createStereographicProjection(poleFigureDim);
  poleFigurePtr->SetName("PoleFigure_<111>");
  intensity111.swap(poleFigurePtr);
}
