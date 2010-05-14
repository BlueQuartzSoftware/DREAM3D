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

#include <AIM/SurfaceMesh/SurfaceMesh.h>


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

  std::cout << logTime() << "Starting Surface Meshing ... " << std::endl;
    MXALOGGER_METHOD_VARIABLE_INSTANCE

    std::string dxFile;
    std::string outputDir;

    int xDim;
    int yDim;
    int zDim;

    bool smoothMesh;
    int smoothIterations;
    int writeOutputFileIncrement;
    bool lockQuadPoints;

    std::string logFile;

    // Handle program options passed on command line.
    boost::program_options::options_description desc("Possible Parameters");
    desc.add_options()
    ("help", "Produce help message")
    ("dxFile", boost::program_options::value<std::string>(&dxFile), "REQUIRED: Input Dx File")
//    ("edgeTableFile", boost::program_options::value<std::string>(&edgeTableFile), "REQUIRED: Input edgeTableFile File")
//    ("neighSpinTableFile", boost::program_options::value<std::string>(&neighSpinTableFile), "REQUIRED: Input neighSpinTableFile File")
    ("outputDir", boost::program_options::value<std::string>(&outputDir), "REQUIRED: Output Directory")
    ("xDim,x", boost::program_options::value<int>(&xDim), "REQUIRED: X Dimension of your volume")
    ("yDim,y", boost::program_options::value<int>(&yDim), "REQUIRED: Y Dimension of your volume")
    ("zDim,z", boost::program_options::value<int>(&zDim), "REQUIRED: Z Dimension of your volume")
    ("smoothMesh", boost::program_options::bool_switch(&smoothMesh), "Smooth the mesh after initial mesh generation [Default=FALSE]")
    ("smoothIterations", boost::program_options::value<int>(&smoothIterations), "How many iterations to use to smooth the mesh")
    ("writeOutputFileIncrement", boost::program_options::value<int>(&writeOutputFileIncrement), "The inrement in iterations to write an output file")
    ("lockQuadPoints", boost::program_options::bool_switch(&lockQuadPoints), "Lock The Quad Points during smoothing [Default=FALSE")
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
      mxa_log << logTime() << "Surface Mesh Version " << AIMRepresentation::Version::Complete << " Starting " << std::endl;

      mxa_log << "Parameters being used are: " << std::endl;

      CHECK_ARG( dxFile, true);
//      CHECK_ARG( edgeTableFile, true);
//      CHECK_ARG( neighSpinTableFile, true);
      CHECK_ARG( outputDir, true);
      CHECK_ARG( xDim, true);
      CHECK_ARG( yDim, true);
      CHECK_ARG( zDim, true);
      CHECK_ARG( smoothMesh, true);
      CHECK_ARG( smoothIterations, true);
      CHECK_ARG( writeOutputFileIncrement, true);
      CHECK_ARG( lockQuadPoints, true);

      SurfaceMesh::Pointer surfaceMesh = SurfaceMesh::New();
//     surfaceMesh->setDXFile(dxFile );
//      surfaceMesh->setEdgeTableFile(edgeTableFile );
//      surfaceMesh->setNeighSpinTableFile(neighSpinTableFile );
      surfaceMesh->setOutputDirectory(outputDir);
      surfaceMesh->setXDim(xDim);
      surfaceMesh->setYDim(yDim);
      surfaceMesh->setZDim(zDim);
      surfaceMesh->setSmoothMesh(smoothMesh);
      surfaceMesh->setSmoothIterations(smoothIterations);
      surfaceMesh->setSmoothFileOutputIncrement(writeOutputFileIncrement);
      surfaceMesh->setSmoothLockQuadPoints(lockQuadPoints);

      surfaceMesh->compute();
      err = surfaceMesh->getErrorCondition();
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
    std::cout << "++++++++++++ Surface Meshing Complete ++++++++++++" << std::endl;
    return err;
  }
