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
#include <MXA/Utilities/MXAFileInfo.h>

#include <AIM/Common/Constants.h>
#include <AIM/Common/AIMVersion.h>
#include <AIM/Common/AIMArray.hpp>

#include <AIM/Reconstruction/Reconstruction.h>





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

  std::cout << logTime() << "Starting Reconstruction ... " << std::endl;
  MXALOGGER_METHOD_VARIABLE_INSTANCE
  int err = EXIT_FAILURE;

  try
  {

    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("AIMRepresentation Reconstruction", ' ', AIMRepresentation::Version::Complete);

    TCLAP::ValueArg<std::string>   inputDir( "i", "inputDir", "Input Directory", false, "", "Input Directory");
    cmd.add(inputDir);

    TCLAP::ValueArg<std::string > outputDir("", "outputDir", "Output Directory", false, "", "Output Directory");
    cmd.add(outputDir);

    TCLAP::ValueArg<std::string>  angFilePrefix( "f", "angFilePrefix", "Ang File Prefix", false, "", "Ang File Prefix");
    cmd.add(angFilePrefix);

    TCLAP::ValueArg<int>  angMaxSlice( "", "angMaxSlice", "Ang Max Slice Number", false, 0, "Ang Max Slice Number");
    cmd.add(angMaxSlice);

    TCLAP::ValueArg<int>  zStartIndex( "s", "zStartIndex", "Starting Slice", false, 0, "Starting Slice");
    cmd.add(zStartIndex);
    TCLAP::ValueArg<int>  zEndIndex( "e", "zEndIndex", "Ending Slice", false, 0, "Ending Slice");
    cmd.add(zEndIndex);
    TCLAP::ValueArg<double>  resz( "z", "resz", "z resolution of your volume", false, 0.0, "z resolution of your volume");
    cmd.add(resz);

    TCLAP::SwitchArg mergetwinsoption("t", "merge-twins", "Do you want to merge twins", false);
    cmd.add(mergetwinsoption);

    TCLAP::ValueArg<int>  minallowedgrainsize( "g", "minallowedgrainsize", "What is the minimum allowed grain size", false, 0, "What is the minimum allowed grain size");
    cmd.add(minallowedgrainsize);


    TCLAP::ValueArg<double>  minseedconfidence( "c", "minseedconfidence", "AWhat is the minimum allowed confidence", false, 0.0, "What is the minimum allowed confidence");
    cmd.add(minseedconfidence);

    TCLAP::ValueArg<double>  misorientationtolerance( "o", "misorientationtolerance", "What is the misorientation tolerance (degrees)", false, 0.0, "What is the misorientation tolerance (degrees)");
    cmd.add(misorientationtolerance);
    TCLAP::ValueArg<int>  crystruct( "x", "crystruct", "Do you have a HCP (1) or FCC (2) material", false, 0, "Do you have a HCP (1) or FCC (2) material");
    cmd.add(crystruct);

    TCLAP::SwitchArg alreadyformed("a", "alreadyformed", "Have you already formed grains", false);
    cmd.add(alreadyformed);

    TCLAP::ValueArg<std::string > logfile("l", "logfile", "Name of the Log file to store any output into", false, "", "Name of the Log file to store any output into");
    cmd.add(logfile);

    // Parse the argv array.
    cmd.parse(argc, argv);
    if (argc == 1)
    {
      std::cout << "AIM Reconstruction program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }


    Reconstruction::Pointer r = Reconstruction::New();
    r->setInputDirectory(inputDir.getValue());
    r->setOutputDirectory(outputDir.getValue());
    r->setAngFilePrefix(angFilePrefix.getValue());
    r->setAngSeriesMaxSlice(angMaxSlice.getValue());
    r->setZStartIndex(zStartIndex.getValue());
    r->setZEndIndex(zEndIndex.getValue());
    r->setZResolution(resz.getValue());
    r->setMergeTwins(mergetwinsoption.getValue());
    r->setMinAllowedGrainSize(minallowedgrainsize.getValue());
    r->setMinSeedConfidence(minseedconfidence.getValue());
    r->setMisorientationTolerance(misorientationtolerance.getValue());
    r->setCrystalStructure(static_cast<AIM::Representation::CrystalStructure> (crystruct.getValue()));
    r->setAlreadyFormed(alreadyformed.getValue());

    r->compute();
    err = r->getErrorCondition();
  }
  catch (TCLAP::ArgException &e) // catch any exceptions
  {
    std::cerr << logTime() << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "++++++++++++ Reconstruction Complete ++++++++++++" << std::endl;
  return err;
}

