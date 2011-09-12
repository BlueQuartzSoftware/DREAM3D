/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include <DREAM3D/DREAM3DVersion.h>

#include <MXA/Common/LogTime.h>
#include <MXA/Utilities/MXALogger.h>

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

int main(int argc, char* argv[])
{
  std::cout << "Starting Mesh Smoothing ... " << std::endl;
  MXALOGGER_METHOD_VARIABLE_INSTANCE

  std::string nodesFile;
  std::string trianglesFile;
  std::string outputDir;
  int iterations = 0;
  int writeinterval = 0;
  bool lockQuadPoints = 0;
  std::string logFile;

  // Handle program options passed on command line.
  boost::program_options::options_description desc("Possible Parameters");
  desc.add_options()("help", "Produce help message")("nodesFile", boost::program_options::value < std::string > (&nodesFile), "REQUIRED: Input Nodes File")("trianglesFile", boost::program_options::value
      < std::string > (&trianglesFile), "REQUIRED: Input Triangles File")("outputDir", boost::program_options::value < std::string > (&outputDir), "REQUIRED: Output Directory")("iterations", boost::program_options::value<
                                                                                                                                                                                     int>(&iterations), "How many iterations to use to smooth the mesh")("writeinterval", boost::program_options::value<
                                                                                                                                                                                                                                                             int>(&writeinterval), "The inrement in iterations to write an output file")("lockQuadPoints", boost::program_options::bool_switch(&lockQuadPoints), "Lock The Quad Points during smoothing [Default=FALSE");

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
    mxa_log << logTime() << "Surface Mesh Smoothing " << DREAM3DLib::Version::Complete << " Starting " << std::endl;

    mxa_log << "Parameters being used are: " << std::endl;

    CHECK_ARG( nodesFile, true);
    CHECK_ARG( trianglesFile, true);
    CHECK_ARG( outputDir, true);
    CHECK_ARG( iterations, true);
    CHECK_ARG( writeinterval, true);
    CHECK_ARG( lockQuadPoints, true);

    int m_lockquads = 0;
    if (lockQuadPoints)
    {
      m_lockquads = 1;
    }

    err = SmoothGrain3D(nodesFile, trianglesFile, outputDir, iterations, writeinterval, m_lockquads);

  }
  catch (...)
  {
    std::cout << "Error on Input: Displaying help listing instead. **" << std::endl;
    std::cout << desc << std::endl;
    for (int i = 0; i < argc; ++i)
    {
      std::cout << argv[i] << std::endl;
    }
    return EXIT_FAILURE;
  }
  std::cout << "++++++++++++ Surface Mesh Smoothing Complete ++++++++++++" << std::endl;
  return err;
}
