///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////


#include <MXA/Common/LogTime.h>
#include <MXA/Utilities/MXALogger.h>
#include <MXA/Utilities/MXAFileSystemPath.h>

#include <AIM/Common/Constants.h>
#include <AIM/Common/AIMVersion.h>
#include <AIM/Common/AIMArray.hpp>

#include <AIM/GrainGenerator/GrainGenerator.h>


#include <string>
#include <iostream>

//-- Boost Program Options
#include <boost/program_options.hpp>

#include <iostream>


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

  std::string inputDir;
  std::string outputDir;

  int numGrains = 0;
  int shapeClass = 0;

  double xRes;
  double yRes;
  double zRes;

  double overlapAllowed = 0.0;
  int overlapAssignment = 0;

  int crystruct = AIM::Representation::UnknownCrystalStructure;

  std::string logFile;

  // Handle program options passed on command line.
  boost::program_options::options_description desc("Possible Parameters");
  desc.add_options()
  ("help", "Produce help message")
  ("inputDir", boost::program_options::value<std::string>(&inputDir), "REQUIRED: Input Directory")
  ("outputDir", boost::program_options::value<std::string>(&outputDir), "REQUIRED: Output Directory")
  ("numGrains", boost::program_options::value<int>(&numGrains), "REQUIRED: The total number of grains to be created")
  ("shapeClass", boost::program_options::value<int>(&shapeClass), "REQUIRED: The shape class you desire (1)Ellipsoid (2)Superellipsoid (3)Cube-Octahedron:")
  ("xRes,x", boost::program_options::value<double>(&xRes), "REQUIRED: X resolution of your volume")
  ("yRes,y", boost::program_options::value<double>(&yRes), "REQUIRED: Y resolution of your volume")
  ("zRes,z", boost::program_options::value<double>(&zRes), "REQUIRED: Z resolution of your volume")
  ("overlapAllowed", boost::program_options::value<double>(&overlapAllowed), "REQUIRED: The overlap between grains that is acceptable")
  ("overlapAssignment", boost::program_options::value<int>(&overlapAssignment), "REQUIRED: Enter how the overlap between grains is handled (1)Rigid (2)Progressive.")
  ("crystruct", boost::program_options::value<int>(&crystruct), "REQUIRED: Do you have a HCP (1) or FCC (2) material")
  ;

  int err = 0;
  try
  {

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    // Print help message if requested by user and return.
    if (vm.count("help") || argc < 2)
    {
      std::cout << desc << std::endl;
      return EXIT_SUCCESS;
    }
    if (vm.count("logfile") != 0)
    {
      logFile = MXAFileSystemPath::toNativeSeparators(logFile);
    }
    if (false == logFile.empty())
    {
      mxa_log.open(logFile);
    }
    mxa_log << logTime() << "Synthetic Builder Version " << AIMRepresentation::Version::Complete << " Starting " << std::endl;

    mxa_log << "Parameters being used are: " << std::endl;

    CHECK_ARG( inputDir, true);
    CHECK_ARG( outputDir, true);
    CHECK_ARG( numGrains, true);
    CHECK_ARG( shapeClass, true);
    CHECK_ARG( xRes, true);
    CHECK_ARG( yRes, true);
    CHECK_ARG( zRes, true);
    CHECK_ARG( overlapAllowed, true);
    CHECK_ARG( overlapAssignment, true);
    CHECK_ARG( crystruct, true);

    GrainGenerator::Pointer r = GrainGenerator::New();
    r->setInputDirectory(inputDir);
    r->setOutputDirectory(outputDir);
    r->setNumGrains(numGrains);
    r->setShapeClass(shapeClass);
    r->setXResolution(xRes);
    r->setYResolution(yRes);
    r->setZResolution(zRes);
    r->setOverlapAllowed(overlapAllowed);
    r->setOverlapAssignment(overlapAssignment);
    r->setCrystalStructure(crystruct);

    r->compute();
    err = r->getErrorCondition();
  } catch (...)
  {
    std::cout << "Error on Input: Displaying help listing instead. **" << std::endl;
    std::cout << desc << std::endl;
    for (int i = 0; i < argc; ++i)
    {
      std::cout << argv[i] << std::endl;
    }
    return EXIT_FAILURE;
  }
  std::cout << "++++++++++++ Synthetic Builder Complete ++++++++++++" << std::endl;
  return err;
}
