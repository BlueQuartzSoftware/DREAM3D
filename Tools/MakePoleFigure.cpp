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


#include <iostream>
#include <string>

#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <QtGui/QColor>
#include <QtGui/QApplication>


// TCLAP Includes
#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>


#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/HKL/CtfReader.h"

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/StatsGen.hpp"
#include "DREAM3DLib/Common/Texture.hpp"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Utilities/PoleFigureUtilities.h"
#include "DREAM3DLib/IOFilters/VtkRectilinearGridWriter.h"

#include "QtSupport/PoleFigureImageUtilities.h"



static const int EulerType = 0;
static const int QuaternionType = 1;
static const int RodriguesType = 2;
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int getLambertSize() { return 24; }
int getImageSize() { return 512; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString generateImagePath(QString outputPath, QString imagePrefix, QString label)
{
  QString path = outputPath + QDir::separator() + imagePrefix + label;
  path.append(".tif");

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
QString generateVtkPath(QString outputPath, QString imagePrefix, QString label)
{
  QString path = outputPath + QDir::separator() + imagePrefix + label;

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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeImage(QString outputPath, DoubleArrayType *intensity, int dimension, QString label, int numColors)
{
 // std::stringstream ss;


  QImage image = PoleFigureImageUtilities::CreateQImage(intensity, dimension, numColors, label, true);

  QString filename = generateImagePath(outputPath, "", label);

    qDebug() << "Writing Image: " << filename;
 // std::cout << ss.str() << std::endl;

  bool saved = image.save(filename);
  if(!saved)
  {
   // ss.str("");
    qDebug() << "The Pole Figure image file '" << filename << "' was not saved.";
 //   std::cout << ss.str() << std::endl;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeVtkFile(const std::string filename, DoubleArrayType *poleFigurePtr, int dimension)
{
  std::cout << "writeVtkFile: " << filename << std::endl;
  //notifyStatusMessage("Writing VTK File");

  size_t dims[3] = {dimension, dimension, 1};
  float res[3] = {  2.0/(float)(dimension),
                    2.0/(float)(dimension),
                    ( 2.0/(float)(dimension) + 2.0/(float)(dimension) )/2.0 };

  int err = VtkRectilinearGridWriter::WriteDataArrayToFile(filename, poleFigurePtr, dims, res, "double", true);
  if (err < 0)
  {
    std::cout << "Error writing Vtk file" << std::endl;
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void generateFromAngFile(QString inputFile, QString outputPath)
{
  std::cout << "generateFromAngFile....." << std::endl;
  int numOrientations = 0;
  FloatArrayType::Pointer eulers = FloatArrayType::NullPointer();
  std::vector<AngPhase::Pointer> phases;
  // Scope This section so we dump the reader data soon after we copy it.
  {
    std::cout << "Reading ANG file....." << std::endl;
    // Get the ANG reader object
    AngReader reader;
    reader.setFileName(inputFile.toStdString());
    int err = reader.readFile();
    if (err < 0)
    {
      qDebug() << "Could not Read Ang file. " << err;
      qDebug() << QString::fromStdString(reader.getErrorMessage());
      return;
    }
    numOrientations = reader.getNumRows() * reader.getNumEvenCols();

    // Create an Array to hold the eulers
    eulers = FloatArrayType::CreateArray(numOrientations, 3, "TEMP_Eulers");


    float* e1s = reader.getPhi1Pointer();
    float* e2s = reader.getPhiPointer();
    float* e3s = reader.getPhi2Pointer();


    std::cout << "Copying Eulers... " << std::endl;
    // Copy the Eulers from the reader's memory into our own memory location
    for(int i = 0; i < numOrientations; ++i)
    {
      eulers->SetComponent(i, 0, e1s[i]);
      eulers->SetComponent(i, 1, e2s[i]);
      eulers->SetComponent(i, 2, e3s[i]);
    }

    phases = reader.getPhaseVector();
  }


  unsigned int symmetry = phases[0]->determineCrystalStructure();

  if (symmetry == Ebsd::CrystalStructure::Cubic_High)
  {
    DoubleArrayType::Pointer poleFigure001 = DoubleArrayType::NullPointer();
    DoubleArrayType::Pointer poleFigure011 = DoubleArrayType::NullPointer();
    DoubleArrayType::Pointer poleFigure111 = DoubleArrayType::NullPointer();
    PoleFigureUtilities::GenerateCubicPoleFigures(eulers.get(), getLambertSize(), getImageSize(), poleFigure001, poleFigure011, poleFigure111);


    // Generate the <001> pole figure
    writeImage(outputPath, poleFigure001.get(), getImageSize(), "001", 32);
    QString path = generateVtkPath(outputPath, "", "001");
    writeVtkFile(path.toStdString(), poleFigure001.get(), getImageSize());
    // Generate the <011> pole figure image
    writeImage(outputPath, poleFigure011.get(), getImageSize(), "011", 32);
    path = generateVtkPath(outputPath, "", "011");
    writeVtkFile(path.toStdString(), poleFigure011.get(), getImageSize());
    // Generate the <111> pole figure image
    writeImage(outputPath, poleFigure111.get(), getImageSize(), "111", 32);
    path = generateVtkPath(outputPath, "", "111");
    writeVtkFile(path.toStdString(), poleFigure111.get(), getImageSize());
  }
  else
  {
    std::cout << "Only Cubic Symmetry is handled for Pole Figures." << std::endl;
    return;
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
void generateFromListFile(QString file, QString outputPath, int angleType)
{
  int numOrients = 0;
  FILE* f = fopen(file.toStdString().c_str(), "rb");

  fscanf(f, "%d\n", &numOrients);

  FloatArrayType::Pointer eulers = FloatArrayType::CreateArray(numOrients, 3, "TEMP_Eulers");

  float e1, e2, e3;
  float r1, r2, r3;

  QuaternionMathF::Quaternion quat;

  for(size_t i = 0; i < numOrients; i++)
  {
    if (angleType == EulerType)
    {
      fscanf(f, "(%f, %f, %f)\n", &e1, &e2, &e3);
    }
    else if (angleType == QuaternionType)
    {
      fscanf(f, "(%f, %f, %f, %f)\n", &quat.x, &quat.y, &quat.z, &quat.w);
      OrientationMath::QuattoEuler(quat, e1, e2, e3);
    }
    else if (angleType == RodriguesType)
    {
      fscanf(f, "(%f, %f, %f)\n", &r1, &r2, &r3);
      OrientationMath::RodtoEuler(r1, r2, r3, e1, e2, e3);
    }
    eulers->SetComponent(i, 0, e1);
    eulers->SetComponent(i, 1, e2);
    eulers->SetComponent(i, 2, e3);
    //   std::cout << "reading line: " << i << std::endl;
  }

  DoubleArrayType::Pointer poleFigure001 = DoubleArrayType::NullPointer();
  DoubleArrayType::Pointer poleFigure011 = DoubleArrayType::NullPointer();
  DoubleArrayType::Pointer poleFigure111 = DoubleArrayType::NullPointer();
  PoleFigureUtilities::GenerateCubicPoleFigures(eulers.get(), getLambertSize(), getImageSize(), poleFigure001, poleFigure011, poleFigure111);

  // Generate the <001> pole figure
  writeImage(outputPath, poleFigure001.get(), getImageSize(), "001", 32);
  QString path = generateVtkPath(outputPath, "", "001");
  writeVtkFile(path.toStdString(), poleFigure001.get(), getImageSize());
  // Generate the <011> pole figure image
  writeImage(outputPath, poleFigure011.get(), getImageSize(), "011", 32);
  path = generateVtkPath(outputPath, "", "011");
  writeVtkFile(path.toStdString(), poleFigure011.get(), getImageSize());
  // Generate the <111> pole figure image
  writeImage(outputPath, poleFigure111.get(), getImageSize(), "111", 32);
  path = generateVtkPath(outputPath, "", "111");
  writeVtkFile(path.toStdString(), poleFigure111.get(), getImageSize());


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{

  QApplication app(argc, argv);

  QString inputFile;
  QString outputPath;
  try
  {

    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("PoleFigureMaker", ' ', "0.1.0");

    TCLAP::ValueArg<std::string> inputFileArg( "", "file", "The input data file", true, "", "Input File");
    cmd.add(inputFileArg);

    TCLAP::ValueArg<std::string> outputPathArg( "", "outpath", "The output path", true, "", "Output Path");
    cmd.add(outputPathArg);

    TCLAP::ValueArg<int> angleType("", "angleType", "The type of Angles: Euler=0, Quaternion=1. Rodrigues=2", false, 0, "Angle Type");
    cmd.add(angleType);


    // Parse the argv array.
    cmd.parse(argc, argv);
    if (argc == 1)
    {
      std::cout << "PoleFigureMaker program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }

    inputFile = QString::fromStdString(inputFileArg.getValue());
    outputPath = QString::fromStdString(outputPathArg.getValue());

    QDir dir(outputPath);
    // make sure the directory is available
    dir.mkpath(outputPath);

    QFileInfo fi(inputFile);
    if (fi.suffix().compare("ang") == 0)
    {
      generateFromAngFile(inputFile, outputPath);
    }
    else if (fi.suffix().compare("txt") == 0)
    {
      generateFromListFile(inputFile, outputPath, angleType.getValue());
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
