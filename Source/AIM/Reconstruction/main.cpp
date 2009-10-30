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

#include <AIM/Reconstruction/Reconstruction.h>


#include <string>
#include <iostream>

//-- Boost Program Options
#include <boost/program_options.hpp>



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

  std::string inputDir;
  std::string outputDir;
  std::string angFilePrefix;
  int angMaxSlice = 300;

  int zStartIndex = 0;
  int zEndIndex = 0;

  double resz = 0.0;

  bool mergetwinsoption = false;
  int minallowedgrainsize = 0.0;
  double minseedconfidence = 0.0;
  double misorientationtolerance = 0.0;

  int crystruct = AIM::Representation::UnknownCrystalStructure;
  bool alreadyformed = false;

  std::string logFile;

  // Handle program options passed on command line.
  boost::program_options::options_description desc("Possible Parameters");
  desc.add_options()
  ("help", "Produce help message")
  ("inputDir,i", boost::program_options::value<std::string>(&inputDir), "REQUIRED: Input Directory")
  ("outputDir", boost::program_options::value<std::string>(&outputDir), "REQUIRED: Output Directory")
  ("angFilePrefix,f", boost::program_options::value<std::string>(&angFilePrefix), "REQUIRED: The prefix that is common to every ANG file")
  ("angMaxSlice", boost::program_options::value<int>(&angMaxSlice), "The max slice value of the series. Needed to be able to generate ang file names")
  ("zStartIndex,s", boost::program_options::value<int>(&zStartIndex), "Starting Slice")
  ("zEndIndex,e", boost::program_options::value<int>(&zEndIndex), "Ending Slice")
  ("resz,z", boost::program_options::value<double>(&resz), "z resolution of your volume")
  ("merge-twins,t", boost::program_options::bool_switch(&mergetwinsoption), "Do you want to merge twins")
  ("minallowedgrainsize,g", boost::program_options::value<int>(&minallowedgrainsize), "What is the minimum allowed grain size")
  ("minseedconfidence,c", boost::program_options::value<double>(&minseedconfidence), "What is the minimum allowed confidence")
  ("misorientationtolerance,o", boost::program_options::value<double>(&misorientationtolerance), "What is the misorientation tolerance (degrees)")
  ("crystruct,x", boost::program_options::value<int>(&crystruct), "Do you have a HCP (1) or FCC (2) material")
  ("alreadyformed,a", boost::program_options::bool_switch(&alreadyformed), "Have you already formed grains (1) Yes (0) No")
  ("logfile,l", boost::program_options::value<std::string>(&logFile), "Name of the Log file to store any output into")
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
    mxa_log << logTime() << "Reconstruction Version " << AIMRepresentation::Version::Complete << " Starting " << std::endl;

    mxa_log << "Parameters being used are: " << std::endl;

    CHECK_ARG( inputDir, true);
    CHECK_ARG( outputDir, true);
    CHECK_ARG( angFilePrefix, true);
    CHECK_ARG( angMaxSlice, true);
    CHECK_ARG( zStartIndex, true);
    CHECK_ARG( zEndIndex, true);
    CHECK_ARG( resz, true);
    CHECK_BOOL_ARG( mergetwinsoption);
    CHECK_ARG( minallowedgrainsize, true);
    CHECK_ARG( minseedconfidence, true);
    CHECK_ARG( misorientationtolerance, true);
    CHECK_ARG( crystruct, true);
    CHECK_BOOL_ARG( alreadyformed);

    Reconstruction::Pointer r = Reconstruction::New();
    r->setInputDirectory(inputDir);
    r->setOutputDirectory(outputDir);
    r->setAngFilePrefix(angFilePrefix);
    r->setAngSeriesMaxSlice(angMaxSlice);
    r->setZStartIndex(zStartIndex);
    r->setZEndIndex(zEndIndex);
    r->setZResolution(resz);
    r->setMergeTwins(mergetwinsoption);
    r->setMinAllowedGrainSize(minallowedgrainsize);
    r->setMinSeedConfidence(minseedconfidence);
    r->setMisorientationTolerance(misorientationtolerance);
    r->setCrystalStructure(static_cast<AIM::Representation::CrystalStructure> (crystruct));
    r->setAlreadyFormed(alreadyformed);

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
  std::cout << "++++++++++++ Reconstruction Complete ++++++++++++" << std::endl;
  return err;
}

