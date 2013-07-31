


#include <iostream>
#include <string>

#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QFileInfo>

// TCLAP Includes
#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>


#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/HKL/CtfReader.h"


#include "DREAM3DLib/Common/StatsGen.hpp"
#include "DREAM3DLib/Common/Texture.hpp"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"

#include "QtSupport/PoleFigureGeneration.h"

#define SET_DIRECTION(i, j, k)\
  direction[0] = i; direction[1] = j; direction[2] = k;



void generateCubicSphereCoordsFromEulers(FloatArrayType::Pointer eulers, FloatArrayType::Pointer xyz001, FloatArrayType::Pointer xyz011, FloatArrayType::Pointer xyz111)
{
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
    MatrixMath::Multiply3x3with3x1(g, direction, xyz111->GetPointer(i*12 + 0));
    direction[0] = DREAM3D::Constants::k_1OverRoot3; direction[1] = -DREAM3D::Constants::k_1OverRoot3; direction[2] = DREAM3D::Constants::k_1OverRoot3;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz111->GetPointer(i*12 + 0));
    direction[0] = DREAM3D::Constants::k_1OverRoot3; direction[1] = DREAM3D::Constants::k_1OverRoot3; direction[2] = -DREAM3D::Constants::k_1OverRoot3;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz111->GetPointer(i*12 + 0));
  }

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void generateFromAngFile(QString inputFile, QString outputFile)
{

  int numOrientations = 0;
  FloatArrayType::Pointer eulers = FloatArrayType::NullPointer();
  {
    // Get the ANG reader object
    AngReader reader;
    reader.setFileName(inputFile.toStdString());
    int err = reader.readFile();
    if (err < 0)
    {
      qDebug() << "Could not Read Ang file";
      return;
    }
    numOrientations = reader.getNumRows() * reader.getNumEvenCols();

    // Create an Array to hold the eulers
    eulers = FloatArrayType::CreateArray(numOrientations, 3, "TEMP_Eulers");


    float* e1s = reader.getPhi1Pointer();
    float* e2s = reader.getPhiPointer();
    float* e3s = reader.getPhi2Pointer();

    // Copy the Eulers from the reader's memory into our own memory location
    for(int i = 0; i < numOrientations; ++i)
    {
      eulers->SetComponent(i, 0, e1s[i]);
      eulers->SetComponent(i, 1, e2s[i]);
      eulers->SetComponent(i, 2, e3s[i]);
    }
  }
  // Create an Array to hold the XYZ Coordinates which are the coords on the sphere.
  // this is size for CUBIC ONLY, <001> Family
  FloatArrayType::Pointer xyz001 = FloatArrayType::CreateArray(numOrientations * 3, 3, "TEMP_<001>_xyzCoords");
  // this is size for CUBIC ONLY, <011> Family
  FloatArrayType::Pointer xyz011 = FloatArrayType::CreateArray(numOrientations * 6, 3, "TEMP_<011>_xyzCoords");
  // this is size for CUBIC ONLY, <111> Family
  FloatArrayType::Pointer xyz111 = FloatArrayType::CreateArray(numOrientations * 4, 3, "TEMP_<111>_xyzCoords");

  // Generate the coords on the sphere
  generateCubicSphereCoordsFromEulers(eulers, xyz001, xyz011, xyz111);






#if 0
  QVector<float> weights(numEntries, 1.0);
  QVector<float> sigmas(numEntries, 1.0);
  QVector<float> odf(CubicOps::k_OdfSize);

  // Lets assume Cubic for a second
  Texture::CalculateCubicODFData(e1s, e2s, e3s,
                                 weights.data(), sigmas.data(), true,
                                 odf.data(), numEntries);

  size_t npoints = 5000;
  QVector<float > x001(npoints * 3);
  QVector<float > y001(npoints * 3);
  QVector<float > x011(npoints * 6);
  QVector<float > y011(npoints * 6);
  QVector<float > x111(npoints * 4);
  QVector<float > y111(npoints * 4);

  qint32 kRad[2] = {4, 4};
  qint32 pfSize[2] = {1024, 1024};
  PoleFigureGeneration colorPoleFigure;




  err = StatsGen::GenCubicODFPlotData(odf.data(), x001.data(), y001.data(),
                                      x011.data(), y011.data(), x111.data(), y111.data(), npoints);

  {
    QImage image = colorPoleFigure.generateColorPoleFigureImage(PoleFigureData(x001, y001, QString("<001>"), kRad, pfSize));

    bool saved = image.save(outputFile);
    if (!saved)
    {
      std::cout << "Image Not Saved: " << outputFile.toStdString() << std::endl;
    }
    else
    {
      std::cout << "Image Saved: " << outputFile.toStdString() << std::endl;
    }
  }

  #endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void generateFromCtfFile(QString inputFile)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void generateFromListFile(QString inputFile)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{

  QString inputFile;
  QString outputFile;
  try
  {

    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("PoleFigureMaker", ' ', "0.1.0");

    TCLAP::ValueArg<std::string> inputFileArg( "", "file", "The input data file", true, "", "Input File");
    cmd.add(inputFileArg);

    TCLAP::ValueArg<std::string> outputFileArg( "", "out", "The output image file", true, "", "Output File");
    cmd.add(outputFileArg);

    // Parse the argv array.
    cmd.parse(argc, argv);
    if (argc == 1)
    {
      std::cout << "PoleFigureMaker program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }

    inputFile = QString::fromStdString(inputFileArg.getValue());
    outputFile = QString::fromStdString(outputFileArg.getValue());

    QFileInfo fi(inputFile);
    if (fi.suffix().compare("ang") == 0)
    {
      generateFromAngFile(inputFile, outputFile);
    }
    else if (fi.suffix().compare("txt") == 0)
    {
      generateFromListFile(inputFile);
    }
    else if (fi.suffix().compare("ctf") == 0)
    {
      generateFromCtfFile(inputFile);
    }


  }
  catch (TCLAP::ArgException &e) // catch any exceptions
  {
    std::cerr << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }




  return 0;
}
