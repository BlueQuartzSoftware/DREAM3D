/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include <string>
#include <iostream>

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

#include <MXA/Common/LogTime.h>
#include <MXA/Utilities/MXALogger.h>
#include <MXA/Utilities/MXADir.h>

#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/DREAM3DVersion.h"
#include "DREAM3D/Common/AIMArray.hpp"

#include "GrainGenerator/GrainGenerator.h"





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
    TCLAP::CmdLine cmd("DREAM.3D Grain Generator", ' ', DREAM3D::Version::Complete);

    TCLAP::ValueArg<std::string>   h5StatsFile( "i", "input", "HDF5 Stats File", true, "", "HDF5 Stats File");
    cmd.add(h5StatsFile);

    TCLAP::ValueArg<std::string > outputDir("", "outputDir", "Output Directory", true, "", "Output Directory");
    cmd.add(outputDir);

    TCLAP::ValueArg<int>  shapeClass( "", "shapeClass", "The shape class you desire (1)Ellipsoid (2)Superellipsoid (3)Cube-Octahedron:", false, 1, "The shape class you desire (1)Ellipsoid (2)Superellipsoid (3)Cube-Octahedron:");
    cmd.add(shapeClass);

    TCLAP::ValueArg<float>  xRes( "", "xres", "X resolution of your volume", true, 0.0, "X resolution of your volume");
    cmd.add(xRes);
    TCLAP::ValueArg<float>  yRes( "", "yres", "Y resolution of your volume", true, 0.0, "Y resolution of your volume");
    cmd.add(yRes);
    TCLAP::ValueArg<float>  zRes( "", "zres", "Z resolution of your volume", true, 0.0, "Z resolution of your volume");
    cmd.add(zRes);

    TCLAP::ValueArg<int>  m_XPoints( "x", "xdim", "X Voxels of your volume", true, 0.0, "X Voxels of your volume");
    cmd.add(m_XPoints);
    TCLAP::ValueArg<int>  m_YPoints( "y", "ydim", "Y Voxels of your volume", true, 0.0, "Y Voxels of your volume");
    cmd.add(m_YPoints);
    TCLAP::ValueArg<int>  m_ZPoints( "z", "zdim", "Z Voxels of your volume", true, 0.0, "Z Voxels of your volume");
    cmd.add(m_ZPoints);

//    TCLAP::ValueArg<double>  overlapAllowed( "", "overlapAllowed", "The overlap between grains that is acceptable", true, 0.0, "The overlap between grains that is acceptable");
//    cmd.add(overlapAllowed);
//    TCLAP::ValueArg<int>  overlapAssignment( "", "overlapAssignment", "Enter how the overlap between grains is handled (1)Rigid (2)Progressive.", true, 1, "Enter how the overlap between grains is handled (1)Rigid (2)Progressive.");
//    cmd.add(overlapAssignment);
//    TCLAP::ValueArg<int>  crystruct( "", "crystruct", "Do you have a HCP (1) or FCC (2) material", true, 2, "Do you have a HCP (1) or FCC (2) material");
//    cmd.add(crystruct);


    // Parse the argv array.
    cmd.parse(argc, argv);
    if (argc == 1)
    {
      std::cout << "Grain Generator program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }

    MXADir::mkdir(outputDir.getValue(), true);

    GrainGenerator::Pointer m_GrainGenerator = GrainGenerator::New();
    m_GrainGenerator->setH5StatsFile(h5StatsFile.getValue());
    m_GrainGenerator->setOutputDirectory(outputDir.getValue());

    m_GrainGenerator->setXPoints(m_XPoints.getValue());
    m_GrainGenerator->setYPoints(m_YPoints.getValue());
    m_GrainGenerator->setZPoints(m_ZPoints.getValue());

    //TODO: Get the Shape Class from the Command line options
  //  m_GrainGenerator->setShapeClass(shapeClass.getValue());

    m_GrainGenerator->setXResolution(xRes.getValue());
    m_GrainGenerator->setYResolution(yRes.getValue());
    m_GrainGenerator->setZResolution(zRes.getValue());
    m_GrainGenerator->setFillingErrorWeight(1.0);
    m_GrainGenerator->setNeighborhoodErrorWeight(1.0);
    m_GrainGenerator->setSizeDistErrorWeight(1.0);

    m_GrainGenerator->setPeriodicBoundary(false);
    m_GrainGenerator->setAlreadyFormed(false);

    m_GrainGenerator->setWriteVtkFile(false);
    m_GrainGenerator->setWriteSurfaceVoxel(false);
    m_GrainGenerator->setWritePhaseId(false);
    m_GrainGenerator->setWriteIPFColor(false);
    m_GrainGenerator->setWriteBinaryVTKFiles(false);

    m_GrainGenerator->setWriteHDF5GrainFile(false);
    m_GrainGenerator->setWritePhFile(false);


    //r->setCrystalStructure( static_cast<AIM::Reconstruction::CrystalStructure>(crystruct.getValue()) );
    m_GrainGenerator->run();
    err = m_GrainGenerator->getErrorCondition();
  }

  catch (TCLAP::ArgException &e) // catch any exceptions
  {
    std::cerr << logTime() << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "++++++++++++ Synthetic Builder Complete ++++++++++++" << std::endl;
  return err;
}
