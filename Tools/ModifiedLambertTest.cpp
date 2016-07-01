/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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



#include <QtGui/QImageReader>

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/ModifiedLambertProjection.h"
#include "SIMPLib/Math/MatrixMath.h"

#include "IOFilters/VtkRectilinearGridWriter.h"

ModifiedLambertProjection::Pointer forwardProjectGreyScaleImage(QImage image)
{
  float xpoints = image.size().width();
  float ypoints = image.size().height();

  float xpointshalf = xpoints / 2.0;
  float ypointshalf = ypoints / 2.0;

  float xres = 2.0 / (float)(xpoints);
  float yres = 2.0 / (float)(ypoints);
  float xtmp, ytmp;
  float sqCoord[2];
  float xyz[3];
  bool nhCheck = false;

  ModifiedLambertProjection::Pointer lambert = ModifiedLambertProjection::New();
  lambert->initializeSquares(1001, 1.0);

  int sqIndex = 0;

  for (int64_t y = 0; y < ypoints; y++)
  {
    for (int64_t x = 0; x < xpoints; x++)
    {
      //get (x,y) for stereographic projection pixel
      xtmp = float(x - xpointshalf) * xres + (xres * 0.5);
      ytmp = float(y - ypointshalf) * yres + (yres * 0.5);
      // int index = y * xpoints + x;
      if((xtmp * xtmp + ytmp * ytmp) <= 1.0)
      {
        //project xy from stereo projection to the unit spehere
        xyz[2] = -((xtmp * xtmp + ytmp * ytmp) - 1) / ((xtmp * xtmp + ytmp * ytmp) + 1);
        xyz[0] = xtmp * (1 + xyz[2]);
        xyz[1] = ytmp * (1 + xyz[2]);

        QRgb rgb = image.pixel(x, y);
        int gray = qGray(rgb);

        for( int64_t m = 0; m < 2; m++)
        {
          if(m == 1) { MatrixMath::Multiply3x1withConstant(xyz, -1.0); }
          nhCheck = lambert->getSquareCoord(xyz, sqCoord);
          sqIndex = lambert->getSquareIndex(sqCoord);


          if (nhCheck == true)
          {
            lambert->setValue(ModifiedLambertProjection::NorthSquare, sqIndex,  static_cast<double>(gray));
          }
          else
          {
            lambert->setValue(ModifiedLambertProjection::SouthSquare, sqIndex,  static_cast<double>(gray));
          }
        }
      }
    }
  }

  return lambert;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void forwardAndReverse()
{

  QImageReader origStereoReader("/Users/mjackson/Desktop/MikeJackson/Ni-circle.jpeg");
  QImage origStereoImage = origStereoReader.read();
  ModifiedLambertProjection::Pointer lambert = forwardProjectGreyScaleImage(origStereoImage);


// QSize size = origStereoImage.size();
  size_t dims[3] = {1001, 1001, 1};
  float res[3] = { 1.0, 1.0, 1.0};
  DoubleArrayType::Pointer lambertSquare = lambert->getNorthSquare();
  lambertSquare->setName(QString("North_") + QString("lambert_from_orig_stereo"));
  VtkRectilinearGridWriter::WriteDataArrayToFile("/tmp/North_lambert_from_orig_stereo.vtk", lambertSquare.get(), dims, res, "double", true);

  lambertSquare = lambert->getSouthSquare();
  lambertSquare->setName(QString("South_") + QString("lambert_from_orig_stereo"));
  VtkRectilinearGridWriter::WriteDataArrayToFile("/tmp/South_lambert_from_orig_stereo.vtk", lambertSquare.get(), dims, res, "double", true);



}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  forwardAndReverse();
  if(true)
  { return EXIT_SUCCESS; }


  QImageReader reader("/Users/mjackson/Desktop/MikeJackson/Ni-Lambert.jpeg");

  QImage image = reader.read();

  QSize size = image.size();

  ModifiedLambertProjection::Pointer lamproj = ModifiedLambertProjection::New();
  lamproj->initializeSquares(size.height(), 1.0f);


  size_t idx = 0;
  for(int y = 0; y < size.height(); ++y)
  {
    for(int x = 0; x < size.width(); ++x)
    {
      idx = (y * size.width()) + x;
      QRgb rgb = image.pixel(x, y);
      int gray = qGray(rgb);
      lamproj->setValue(ModifiedLambertProjection::NorthSquare, idx, static_cast<double>(gray));
    }
  }


  size_t dims[3] = {size.width(), size.height(), 1};
  float res[3] = { 1.0, 1.0, 1.0};
  DoubleArrayType::Pointer north = lamproj->getNorthSquare();
  VtkRectilinearGridWriter::WriteDataArrayToFile("/tmp/MarcLambert_North.vtk", north.get(), dims, res, "double", true);


  DoubleArrayType::Pointer stereoProj = lamproj->createStereographicProjection(1501);


  VtkRectilinearGridWriter::WriteDataArrayToFile("/tmp/MarcStereoGraphicProjection.vtk", stereoProj.get(), dims, res, "double", true);

  return 0;
}
