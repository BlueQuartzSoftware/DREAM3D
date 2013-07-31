


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
#include "QtSupport/PoleFigureGeneration.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void generateFromAngFile(QString inputFile, QString outputFile)
{
  AngReader reader;
  reader.setFileName(inputFile.toStdString());
  int err = reader.readFile();
  if (err < 0)
  {
    qDebug() << "Could not Read Ang file";
    return;
  }
  int numEntries = reader.getNumRows() * reader.getNumEvenCols();


  float* e1s = reader.getPhi1Pointer();
  float* e2s = reader.getPhiPointer();
  float* e3s = reader.getPhi2Pointer();
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
