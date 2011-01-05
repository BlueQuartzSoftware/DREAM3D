///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

#include <MXA/Common/LogTime.h>
#include <MXA/Utilities/MXALogger.h>
#include <MXA/Utilities/MXADir.h>

#include <AIM/Common/Constants.h>
#include <AIM/Common/AIMRepresentationVersion.h>
#include <AIM/Common/AIMArray.hpp>

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
    TCLAP::CmdLine cmd("AIMRepresentation Volume Meshing", ' ', AIMRepresentation::Version::Complete);

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
