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

#include <AIM/Reconstruction/Reconstruction.h>

#if 0
./Bin/Reconstruction_debug -i /Users/Shared/Data/12_strain/12_strain.h5ang --zStartIndex 10074 --zEndIndex 10099 --crystruct 1 --alignment 0 --minGrainSize 8 --misorientationTolerance 5.0 --minImageQuality 50 --minConfidenceIndex 0.1 --outputDir /tmp/12Strain
#endif



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

    TCLAP::ValueArg<std::string> h5InputFile( "i", "input", "HDF5 Input File", false, "", "HDF5 Input File");
    cmd.add(h5InputFile);

    TCLAP::ValueArg<int>  zStartIndex( "", "zStartIndex", "Starting Slice", false, 0, "Starting Slice");
    cmd.add(zStartIndex);
    TCLAP::ValueArg<int>  zEndIndex( "", "zEndIndex", "Ending Slice", false, 0, "Ending Slice");
    cmd.add(zEndIndex);


    TCLAP::SwitchArg mergeColonies("", "mergeColonies", "Do you want to merge colonies", false);
    cmd.add(mergeColonies);
    TCLAP::SwitchArg alreadyFormed("", "alreadyFormed", "Have you already formed grains", false);
    cmd.add(alreadyFormed);
    TCLAP::SwitchArg mergeTwins("", "mergeTwins", "Do you want to merge twins", false);
    cmd.add(mergeTwins);
    TCLAP::SwitchArg fillinSample("", "rectangularize", "Rectangularize Sample", false);
    cmd.add(fillinSample);

    TCLAP::ValueArg<int>  CrystalStructure( "", "crystruct", "Do you have a HCP (0) or FCC (1) material", false, 1, "Default=1");
    cmd.add(CrystalStructure);
    TCLAP::ValueArg<int>  AlignMeth( "", "alignment", "Alignment Method [0] OuterBoundary [1] Misorientation [2] Mutual Information", false, 0, "Default=0");
    cmd.add(AlignMeth);

    TCLAP::ValueArg<int>  MinAllowedGrainSize( "", "minGrainSize", "What is the minimum allowed grain size", false, 50, "Default=50");
    cmd.add(MinAllowedGrainSize);
    TCLAP::ValueArg<double>  MisOrientationTolerance( "", "misorientationTolerance", "What is the misorientation tolerance (degrees)", false, 4.0, "Default=4.0");
    cmd.add(MisOrientationTolerance);
    TCLAP::ValueArg<double>  MinImageQuality( "", "minImageQuality", "What is the minimum Image Quality Value", false, 50, "Default=50");
    cmd.add(MinImageQuality);
    TCLAP::ValueArg<double>  MinConfidence( "", "minConfidenceIndex", "What is the minimum allowed confidence index", false, 0.1, "Default=0.1");
    cmd.add(MinConfidence);
    TCLAP::ValueArg<double>  DownSampleFactor( "", "downSampleRes", "Down Sampling Resolution", false, 1.0, "Default=1.0");
    cmd.add(DownSampleFactor);
    TCLAP::ValueArg<double>  BinStepSize( "", "binStepSize", "Width of each Bin in the Reconstruction Stats", false, 1.0, "Default=1.0");
    cmd.add(BinStepSize);

    TCLAP::ValueArg<std::string> OutputDir("", "outputDir", "Where to write all the output files. If it does not exist it will be created.", false, "", "/Path/To/Output");
    cmd.add(OutputDir);


    TCLAP::ValueArg<std::string > logfile("l", "logfile", "Name of the Log file to store any output into", false, "", "/Path/To/LogFile.log");
    cmd.add(logfile);

    // Parse the argv array.
    cmd.parse(argc, argv);
    if (argc == 1)
    {
      std::cout << "AIM Reconstruction program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }


    Reconstruction::Pointer m_Reconstruction = Reconstruction::New();

    m_Reconstruction->setH5AngFile(h5InputFile.getValue());

    m_Reconstruction->setZStartIndex(zStartIndex.getValue());
    m_Reconstruction->setZEndIndex(zEndIndex.getValue() + 1);

    m_Reconstruction->setMergeColonies(mergeColonies.getValue() );
    m_Reconstruction->setAlreadyFormed(alreadyFormed.getValue());
    m_Reconstruction->setMergeTwins(mergeTwins.getValue() );
    m_Reconstruction->setFillinSample(fillinSample.getValue() );

    AIM::Reconstruction::CrystalStructure crystruct = static_cast<AIM::Reconstruction::CrystalStructure>(CrystalStructure.getValue());
    AIM::Reconstruction::AlignmentMethod alignmeth = static_cast<AIM::Reconstruction::AlignmentMethod>(AlignMeth.getValue() );
    m_Reconstruction->setCrystalStructure(crystruct);
    m_Reconstruction->setAlignmentMethod(alignmeth);

    m_Reconstruction->setMinAllowedGrainSize(MinAllowedGrainSize.getValue());
    m_Reconstruction->setMisorientationTolerance(MisOrientationTolerance.getValue());
    m_Reconstruction->setMinSeedImageQuality(MinImageQuality.getValue());
    m_Reconstruction->setMinSeedConfidence(MinConfidence.getValue());
    m_Reconstruction->setDownSampleFactor(DownSampleFactor.getValue());
    m_Reconstruction->setSizeBinStepSize(BinStepSize.getValue());

    m_Reconstruction->setOutputDirectory(OutputDir.getValue());
    m_Reconstruction->setWriteVisualizationFile(true);
    m_Reconstruction->setWriteIPFFile(true);
    m_Reconstruction->setWriteDisorientationFile(true);
    m_Reconstruction->setWriteImageQualityFile(true);
    m_Reconstruction->setWriteSchmidFactorFile(true);
    m_Reconstruction->setWriteDownSampledFile(true);
    m_Reconstruction->setWriteHDF5GrainFile(true);

    m_Reconstruction->compute();
    err = m_Reconstruction->getErrorCondition();
  }
  catch (TCLAP::ArgException &e) // catch any exceptions
  {
    std::cerr << logTime() << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Reconstruction Complete" << std::endl;
  return err;
}

