


#include <QtGui/QImageReader>

#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Common/ModifiedLambertProjection.h"
#include "DREAM3DLib/IOFilters/VtkRectilinearGridWriter.h"
#include "DREAM3DLib/Math/MatrixMath.h"


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
      xtmp = float(x-xpointshalf)*xres+(xres * 0.5);
      ytmp = float(y-ypointshalf)*yres+(yres * 0.5);
     // int index = y * xpoints + x;
      if((xtmp*xtmp+ytmp*ytmp) <= 1.0)
      {
        //project xy from stereo projection to the unit spehere
        xyz[2] = -((xtmp*xtmp+ytmp*ytmp)-1)/((xtmp*xtmp+ytmp*ytmp)+1);
        xyz[0] = xtmp*(1+xyz[2]);
        xyz[1] = ytmp*(1+xyz[2]);

        QRgb rgb = image.pixel(x, y);
        int gray = qGray(rgb);

        for( int64_t m = 0; m < 2; m++)
        {
          if(m == 1) MatrixMath::Multiply3x1withConstant(xyz, -1.0);
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
int main(int argc, char *argv[])
{
  forwardAndReverse();
  if(true)
    return EXIT_SUCCESS;


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
