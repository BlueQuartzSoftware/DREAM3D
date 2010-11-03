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
#include <MXA/Utilities/MXAFileSystemPath.h>

#include <AIM/Common/Constants.h>
#include <AIM/Common/AIMVersion.h>
#include <AIM/Common/AIMArray.hpp>

#include <AIM/GrainGenerator/GrainGenerator.h>





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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::cout << logTime() << "Starting Synthetic Builder... " << std::endl;
  MXALOGGER_METHOD_VARIABLE_INSTANCE



  int err = 0;
  try
  {

    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("AIMRepresentation Grain Generator", ' ', AIMRepresentation::Version::Complete);

    TCLAP::ValueArg<std::string>   inputDir( "i", "inputDir", "Input Directory", true, "", "Input Directory");
    cmd.add(inputDir);

    TCLAP::ValueArg<std::string > outputDir("", "outputDir", "Output Directory", true, "", "Output Directory");
    cmd.add(outputDir);

    TCLAP::ValueArg<int>  numGrains( "", "numGrains", "The total number of grains to be created", true, 0, "The total number of grains to be created");
    cmd.add(numGrains);


    TCLAP::ValueArg<int>  shapeClass( "", "shapeClass", "The shape class you desire (1)Ellipsoid (2)Superellipsoid (3)Cube-Octahedron:", false, 1, "The shape class you desire (1)Ellipsoid (2)Superellipsoid (3)Cube-Octahedron:");
    cmd.add(shapeClass);

    TCLAP::ValueArg<double>  xRes( "x", "xRes", "X resolution of your volume", true, 0.0, "X resolution of your volume");
    cmd.add(xRes);
    TCLAP::ValueArg<double>  yRes( "y", "yRes", "Y resolution of your volume", true, 0.0, "Y resolution of your volume");
    cmd.add(yRes);
    TCLAP::ValueArg<double>  zRes( "z", "zRes", "Z resolution of your volume", true, 0.0, "Z resolution of your volume");
    cmd.add(zRes);
    TCLAP::ValueArg<double>  overlapAllowed( "", "overlapAllowed", "The overlap between grains that is acceptable", true, 0.0, "The overlap between grains that is acceptable");
    cmd.add(overlapAllowed);
    TCLAP::ValueArg<int>  overlapAssignment( "", "overlapAssignment", "Enter how the overlap between grains is handled (1)Rigid (2)Progressive.", true, 1, "Enter how the overlap between grains is handled (1)Rigid (2)Progressive.");
    cmd.add(overlapAssignment);
    TCLAP::ValueArg<int>  crystruct( "", "crystruct", "Do you have a HCP (1) or FCC (2) material", true, 2, "Do you have a HCP (1) or FCC (2) material");
    cmd.add(crystruct);


    // Parse the argv array.
    cmd.parse(argc, argv);
    if (argc == 1)
    {
      std::cout << "Grain Generator program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }


    GrainGenerator::Pointer r = GrainGenerator::New();
    r->setInputDirectory(inputDir.getValue());
    r->setOutputDirectory(outputDir.getValue());
    r->setNumGrains(numGrains.getValue());
    r->setShapeClass(shapeClass.getValue());
    r->setXResolution(xRes.getValue());
    r->setYResolution(yRes.getValue());
    r->setZResolution(zRes.getValue());
    r->setOverlapAllowed(overlapAllowed.getValue());
    r->setOverlapAssignment(overlapAssignment.getValue());
    r->setCrystalStructure( static_cast<AIM::Representation::CrystalStructure>(crystruct.getValue()) );

    r->compute();
    err = r->getErrorCondition();
  }

  catch (TCLAP::ArgException &e) // catch any exceptions
  {
    std::cerr << logTime() << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "++++++++++++ Synthetic Builder Complete ++++++++++++" << std::endl;
  return err;
}
