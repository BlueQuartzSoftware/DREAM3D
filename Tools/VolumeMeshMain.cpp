/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AREw
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <string>
#include <iostream>

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

#include <MXA/Common/LogTime.h>
#include <MXA/Utilities/MXALogger.h>
#include <MXA/Utilities/MXADir.h>

#include "DREAM3DLib/Common/Constants.h"
#include <DREAM3D/Common/DREAM3DVersion.h>
#include "DREAM3DLib/DataArrays/DataArray.hpp"

#include <AIM/VolumeMesh/VolumeMesh.h>

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

  std::cout << logTime() << "Starting Volume Meshing ... " << std::endl;
  MXALOGGER_METHOD_VARIABLE_INSTANCE

  int err = 0;
  try
  {
    TCLAP::CmdLine cmd("DREAM.3D Volume Meshing", ' ', DREAM3DLib::Version::Complete);

    TCLAP::ValueArg<std::string > nodesfile("", "nodesfile", "Input Nodes File", false, "", "Input Nodes File");
    cmd.add(nodesfile);

    TCLAP::ValueArg<std::string > trianglefile("", "trianglefile", "Input Triangle File", false, "", "Input Triangle File");
    cmd.add(trianglefile);

    TCLAP::ValueArg<std::string > outputDir("", "outputDir", "Output Directory", false, "", "Output Directory");
    cmd.add(outputDir);

    TCLAP::ValueArg<int > xDim("x", "xDim", "xDim", false, 0, "X Dimension");
    cmd.add(xDim);

    TCLAP::ValueArg<int > yDim("y", "yDim", "yDim", false, 0, "Y Dimension");
    cmd.add(yDim);

    TCLAP::ValueArg<int > zDim("z", "zDim", "zDim", false, 0, "Z Dimension");
    cmd.add(zDim);

    TCLAP::ValueArg<double > xRes("", "xRes", "X resolution of your volume", true, 0.0, "X resolution of your volume");
    cmd.add(xRes);
    TCLAP::ValueArg<double > yRes("", "yRes", "Y resolution of your volume", true, 0.0, "Y resolution of your volume");
    cmd.add(yRes);
    TCLAP::ValueArg<double > zRes("", "zRes", "Z resolution of your volume", true, 0.0, "Z resolution of your volume");
    cmd.add(zRes);

    TCLAP::ValueArg<int > numGrains("", "numGrains", "The total number of grains to be created", true, 0, "The total number of grains to be created");
    cmd.add(numGrains);

    // Parse the argv array.
    cmd.parse(argc, argv);
    if (argc == 1)
    {
      std::cout << "Volume Meshing program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }

    VolumeMesh::Pointer volmesh = VolumeMesh::New();
    volmesh->setNodesFile(nodesfile.getValue());
    volmesh->setTrianglesFile(trianglefile.getValue());
    volmesh->setOutputDirectory(outputDir.getValue());
    volmesh->setXDim(xDim.getValue());
    volmesh->setYDim(yDim.getValue());
    volmesh->setZDim(zDim.getValue());
    volmesh->setXRes(xRes.getValue());
    volmesh->setYRes(yRes.getValue());
    volmesh->setZRes(zRes.getValue());
    volmesh->setNumGrains(numGrains.getValue());
    volmesh->compute();
    err = volmesh->getErrorCondition();
  }
  catch (TCLAP::ArgException &e) // catch any exceptions
  {
    std::cerr << logTime() << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "++++++++++++ Volume Meshing Complete ++++++++++++" << std::endl;
  return err;
}
