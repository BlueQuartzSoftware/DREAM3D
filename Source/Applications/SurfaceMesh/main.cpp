/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <string>
#include <iostream>

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

#include <MXA/Common/LogTime.h>
#include <MXA/Utilities/MXALogger.h>

#include "DREAM3D/Common/Constants.h"
#include <DREAM3D/DREAM3DVersion.h>
#include <DREAM3D/Common/AIMArray.hpp>

#include <DREAM3D/SurfaceMesh/SurfaceMesh.h>

#define CHECK_ARG(var, mandatory)\
    if (vm.count(#var) > 1) { mxa_log << logTime() << "Multiple Occurances for Parameter " << #var << std::endl; }\
    if (vm.count(#var) == 0 && mandatory == true) { \
    mxa_log << "Parameter --" << #var << " ==> Required. Program will Terminate." << std::endl; }\
    if (vm.count(#var) == 0 && mandatory == false) { \
    mxa_log << "--" << #var << " Using Default: '"  << var << "'" << std::endl; }\
    if (vm.count(#var) == 1 && mandatory == true) { \
    mxa_log << "--" << #var << " Value: '"  << var << "'" << std::endl; }

#define CHECK_BOOL_ARG(var)\
  mxa_log << "--" << #var << " is ";\
  if (var == true) { mxa_log << "TRUE"; } else { mxa_log << "FALSE"; }\
  mxa_log << "" << std::endl;

int main(int argc, char **argv)
{
  MXALOGGER_METHOD_VARIABLE_INSTANCE
  int err = EXIT_FAILURE;

  std::string logFile;
  try
  {

    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("DREAM.3D Surface Meshing", ' ', DREAM3D::Version::Complete);

    TCLAP::ValueArg<std::string> vtkFile("v", "vtkfile", "VTK Structured Points File to be used as input.", false, "", "VTK Structured Points File");
    cmd.add(vtkFile);


    TCLAP::ValueArg<std::string > outputDir("o", "outputDir", "Output Directory", false, "", "Output Directory");
    cmd.add(outputDir);


    TCLAP::ValueArg<std::string > dxFile("", "dxfile", "Input DX File", false, "", "Input DX File");
 //   cmd.add(dxFile);

    TCLAP::ValueArg<int > xDim("x", "xDim", "xDim", false, 0, "X Dimension");
  //  cmd.add(xDim);

    TCLAP::ValueArg<int > yDim("y", "yDim", "yDim", false, 0, "Y Dimension");
//    cmd.add(yDim);

    TCLAP::ValueArg<int >  zDim("z", "zDim", "zDim", false, 0, "Z Dimension");
//    cmd.add(zDim);

    TCLAP::SwitchArg smoothMesh("", "smoothMesh", "Smooth the mesh after initial mesh generation [Default=FALSE]", false);
 //   cmd.add(smoothMesh);

    TCLAP::ValueArg<int > smoothIterations("", "smoothiterations", "Smooth Iterations. Anything greater than 0 will trigger a Laplacian smoothing of the mesh. 6 is a good place to start.", false, 0, "Smooth Iterations");
//    cmd.add(smoothIterations);

    TCLAP::SwitchArg lockQuadPoints("q", "lockquad", "Lock Quad (or More) Juntions Points from Smoothing & decimation", false);
 //   cmd.add(lockQuadPoints);

    TCLAP::ValueArg<int > writeOutputFileIncrement("", "writeOutputFileIncrement", "The inrement in iterations to write an output file", false, 0, "The inrement in iterations to write an output file");
//    cmd.add(writeOutputFileIncrement);


    int err = 0;

    // Parse the argv array.
    cmd.parse(argc, argv);
    if (argc == 1)
    {
      std::cout << "AIMRepresentation Surface Mesh program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }

    std::cout << logTime() << "Starting Surface Meshing ... " << std::endl;
    SurfaceMesh::Pointer surfaceMesh = SurfaceMesh::New();

#if 0
    surfaceMesh->setXDim(xDim.getValue());
    surfaceMesh->setYDim(yDim.getValue());
    surfaceMesh->setZDim(zDim.getValue());
#endif

    surfaceMesh->setInputFile(vtkFile.getValue());
    surfaceMesh->setOutputDirectory(outputDir.getValue());
    surfaceMesh->setSmoothMesh(smoothMesh.getValue());
    surfaceMesh->setSmoothIterations(smoothIterations.getValue());
    surfaceMesh->setSmoothFileOutputIncrement(writeOutputFileIncrement.getValue());
    surfaceMesh->setSmoothLockQuadPoints(lockQuadPoints.getValue());

    surfaceMesh->compute();
    err = surfaceMesh->getErrorCondition();
  }
  catch (TCLAP::ArgException &e) // catch any exceptions
  {
    std::cerr << logTime() << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "++++++++++++ Surface Meshing Complete ++++++++++++" << std::endl;
  return err;
}
