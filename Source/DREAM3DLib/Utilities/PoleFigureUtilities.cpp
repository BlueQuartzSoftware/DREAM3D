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


  float resolution = sqrt(M_PI*0.5) * 2.0 / (float)(lambertDimension);
  float sphereRadius = 1.0f;

  // Generate the coords on the sphere
  CubicOps ops;
  ops.generateSphereCoordsFromEulers(eulers, xyz001.get(), xyz011.get(), xyz111.get());
  // Generate the modified Lambert projection images (Squares, 2 of them, 1 for northern hemisphere, 1 for southern hemisphere
  ModifiedLambertProjection::Pointer lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz001.get(), lambertDimension, resolution, sphereRadius);
  // Now create the intensity image that will become the actual Pole figure image
  DoubleArrayType::Pointer poleFigurePtr = lambert->createStereographicProjection(poleFigureDim);
  poleFigurePtr->SetName("PoleFigure_<001>");
  intensity001.swap(poleFigurePtr);


// Generate the <011> pole figure
  lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz011.get(), lambertDimension, resolution, sphereRadius);
  poleFigurePtr = lambert->createStereographicProjection(poleFigureDim);
  poleFigurePtr->SetName("PoleFigure_<011>");
  intensity011.swap(poleFigurePtr);

    // Generate the <111> pole figure
  lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz111.get(), lambertDimension, resolution, sphereRadius);
  poleFigurePtr = lambert->createStereographicProjection(poleFigureDim);
  poleFigurePtr->SetName("PoleFigure_<111>");
  intensity111.swap(poleFigurePtr);
}
