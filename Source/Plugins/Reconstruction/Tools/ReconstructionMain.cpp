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
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
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
#include <vector>

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

#include <MXA/Common/LogTime.h>
#include <MXA/Utilities/MXALogger.h>
#include <MXA/Utilities/MXADir.h>

#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "Reconstruction/Reconstruction.h"


#define RECONSTRUCTION_MANUAL_DEBUG 0

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
  if (true)
  {
    std::cout << "Reconstruction Tool is NOT complete and is missing the ability to ingest filters. Please use the GUI version instead" << std::endl;
    return err;
  }

  try
  {
    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("DREAM.3D Reconstruction", ' ', DREAM3DLib::Version::Complete);

    TCLAP::ValueArg<std::string> h5InputFile( "i", "input", "HDF5 Input File", false, "", "HDF5 Input File");
    cmd.add(h5InputFile);

    TCLAP::ValueArg<int>  zStartIndex( "", "zStartIndex", "Starting Slice", false, 0, "Starting Slice");
    cmd.add(zStartIndex);
    TCLAP::ValueArg<int>  zEndIndex( "", "zEndIndex", "Ending Slice", false, 0, "Ending Slice");
    cmd.add(zEndIndex);

    TCLAP::ValueArg<int>  AlignMeth( "", "alignment", "Alignment Method [0] OuterBoundary [1] Misorientation [2] Mutual Information", false, 0, "Default=0");
    cmd.add(AlignMeth);

    TCLAP::ValueArg<int>  MinAllowedGrainSize( "", "minGrainSize", "What is the minimum allowed grain size", false, 50, "Default=50");
    cmd.add(MinAllowedGrainSize);
    TCLAP::ValueArg<double>  MisOrientationTolerance( "", "misorientationTolerance", "What is the misorientation tolerance (degrees)", false, 4.0, "Default=4.0");
    cmd.add(MisOrientationTolerance);

    TCLAP::SwitchArg mergeColonies("", "mergeColonies", "Do you want to merge colonies", false);
    cmd.add(mergeColonies);
    TCLAP::SwitchArg mergeTwins("", "mergeTwins", "Do you want to merge twins", false);
    cmd.add(mergeTwins);
    TCLAP::SwitchArg rectangularize("", "rectangularize", "Rectangularize Sample", false);
    cmd.add(rectangularize);

    TCLAP::ValueArg<int>  CrystalStructure( "", "crystruct", "Do you have a HCP (0) or FCC (1) material", false, 1, "Default=1");
    cmd.add(CrystalStructure);

    TCLAP::ValueArg<double>  DownSampleFactor( "", "downSampleRes", "Down Sampling Resolution", false, 1.0, "Default=1.0");
    cmd.add(DownSampleFactor);

    TCLAP::ValueArg<std::string> OutputDir("", "outputDir", "Where to write all the output files. If it does not exist it will be created.", true, ".", "/Path/To/Output");
    cmd.add(OutputDir);
    TCLAP::ValueArg<std::string> OutputFilePrefix("", "outputFilePrefix", "This is an optional file prefix for each of the output files", false, "Reconstruction_", "Default=Reconstruction_");
    cmd.add(OutputFilePrefix);

    TCLAP::ValueArg<std::string > logfile("l", "logfile", "Name of the Log file to store any output into", false, "", "/Path/To/LogFile.log");
    cmd.add(logfile);

    // Parse the argv array.
    cmd.parse(argc, argv);
#if (RECONSTRUCTION_MANUAL_DEBUG == 0)
    if (argc == 1)
    {
      std::cout << "DREAM.3D Reconstruction program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }
#endif
    Reconstruction::Pointer m_Reconstruction = Reconstruction::New();

    m_Reconstruction->setH5EbsdFile(h5InputFile.getValue());

    m_Reconstruction->setZStartIndex(zStartIndex.getValue());
    m_Reconstruction->setZEndIndex(zEndIndex.getValue() + 1);

    m_Reconstruction->setMergeColonies(mergeColonies.getValue() );
    m_Reconstruction->setMergeTwins(mergeTwins.getValue() );

#if 0
    unsigned int crystruct = static_cast<unsigned int>(CrystalStructure.getValue());
    std::vector<unsigned int> xtals(static_cast<unsigned int>(CrystalStructure.getValue()));
    m_Reconstruction->setCrystalStructure(crystruct);
#endif

    unsigned int alignmeth = static_cast<unsigned int>(AlignMeth.getValue() );

    m_Reconstruction->setAlignmentMethod(alignmeth);

    m_Reconstruction->setMinAllowedGrainSize(MinAllowedGrainSize.getValue());
    m_Reconstruction->setMisorientationTolerance(MisOrientationTolerance.getValue());

    m_Reconstruction->setOutputDirectory(OutputDir.getValue());
    m_Reconstruction->setOutputFilePrefix(OutputFilePrefix.getValue());
    m_Reconstruction->setWriteVtkFile(true);
    m_Reconstruction->setWritePhaseId(true);
    m_Reconstruction->setWriteIPFColor(true);
    m_Reconstruction->setWriteBinaryVTKFiles(true);

    m_Reconstruction->setWriteHDF5GrainFile(false);

#if RECONSTRUCTION_MANUAL_DEBUG
#if (_WIN32)
    m_Reconstruction->setH5AngFile("C:\\Data\\12_strain.h5ang");
    m_Reconstruction->setOutputDirectory("C:\\Data\\ReconstructionOutput");
    if (MXADir::exists("C:\\Data\\ReconstructionOutput") == false )
    {
      MXADir::mkdir("C:\\Data\\ReconstructionOutput", true);
    }
#else
    m_Reconstruction->setH5AngFile("/Users/Shared/Data/Ang_Data/2PercentStrain/2PercentStrain.h5ang");
    m_Reconstruction->setOutputDirectory("/tmp/2PercentStrain");
    if (MXADir::exists("/tmp/2PercentStrain") == false )
    {
      MXADir::mkdir("/tmp/2PercentStrain", true);
    }
#endif
    m_Reconstruction->setZStartIndex(10074);
    m_Reconstruction->setZEndIndex(10376);
    m_Reconstruction->setCrystalStructure(Ebsd::CrystalStructure::Cubic);
    m_Reconstruction->setAlignmentMethod(DREAM3D::Reconstruction::OuterBoundary);
    m_Reconstruction->setm_minallowedgrainsize(24);
    m_Reconstruction->setMisorientationTolerance(5.0);
    m_Reconstruction->setMinSeedImageQuality(600);
    m_Reconstruction->setMinSeedConfidence(0.1);
    m_Reconstruction->setOrientation(Ang::UpperRightOrigin);

    m_Reconstruction->printSettings(std::cout);
#endif
    m_Reconstruction->run();
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

