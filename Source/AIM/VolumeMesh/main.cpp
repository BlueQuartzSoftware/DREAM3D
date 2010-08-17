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


#include <MXA/Common/LogTime.h>
#include <MXA/Utilities/MXALogger.h>
#include <MXA/Utilities/MXAFileSystemPath.h>

#include <AIM/Common/Constants.h>
#include <AIM/Common/AIMVersion.h>
#include <AIM/Common/AIMArray.hpp>

#include <AIM/VolumeMesh/VolumeMesh.h>


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


int main(int argc, char **argv)
{

  std::cout << logTime() << "Starting Volume Meshing ... " << std::endl;
    MXALOGGER_METHOD_VARIABLE_INSTANCE

    std::string nodesfile;
    std::string trianglefile;
    std::string outputDir;


    double xDim;
    double yDim;
    double zDim;

    double xRes;
    double yRes;
    double zRes;

    int numgrains;

    std::string logFile;

    // Handle program options passed on command line.
    boost::program_options::options_description desc("Possible Parameters");
    desc.add_options()
    ("help", "Produce help message")
    ("nodesfile", boost::program_options::value<std::string>(&nodesfile), "REQUIRED: Input Nodes File")
    ("trianglefile", boost::program_options::value<std::string>(&trianglefile), "REQUIRED: Input Triangle File")
    ("outputDir", boost::program_options::value<std::string>(&outputDir), "REQUIRED: Output Directory")
    ("xDim,x", boost::program_options::value<double>(&xDim), "REQUIRED: X Dimension of your volume")
    ("yDim,y", boost::program_options::value<double>(&yDim), "REQUIRED: Y Dimension of your volume")
    ("zDim,z", boost::program_options::value<double>(&zDim), "REQUIRED: Z Dimension of your volume")
    ("xRes", boost::program_options::value<double>(&xRes), "REQUIRED: X Resolution of your volume")
    ("yRes", boost::program_options::value<double>(&yRes), "REQUIRED: Y Resolution of your volume")
    ("zRes", boost::program_options::value<double>(&zRes), "REQUIRED: Z Resolution of your volume")
    ("numgrains", boost::program_options::value<int>(&numgrains), "Total number of grains in the volume")
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
      mxa_log << logTime() << "Volume Meshing Version " << AIMRepresentation::Version::Complete << " Starting " << std::endl;

      mxa_log << "Parameters being used are: " << std::endl;

      CHECK_ARG( nodesfile, true);
      CHECK_ARG( trianglefile, true);
      CHECK_ARG( outputDir, true);
      CHECK_ARG( xDim, true);
      CHECK_ARG( yDim, true);
      CHECK_ARG( zDim, true);
      CHECK_ARG( xRes, true);
      CHECK_ARG( yRes, true);
      CHECK_ARG( zRes, true);
      CHECK_ARG( numgrains, true);

      VolumeMesh::Pointer volmesh = VolumeMesh::New();
      volmesh->setNodesFile(nodesfile );
      volmesh->setTrianglesFile(trianglefile );
      volmesh->setOutputDirectory(outputDir);
      volmesh->setXDim(xDim);
      volmesh->setYDim(yDim);
      volmesh->setZDim(zDim);
      volmesh->setXRes(xRes);
      volmesh->setYRes(yRes);
      volmesh->setZRes(zRes);
      volmesh->setNumGrains(numgrains);
      volmesh->compute();
      err = volmesh->getErrorCondition();
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
    std::cout << "++++++++++++ Volume Meshing Complete ++++++++++++" << std::endl;
    return err;
  }
