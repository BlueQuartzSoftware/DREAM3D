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

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

#include <MXA/Common/LogTime.h>
#include <MXA/Utilities/MXALogger.h>
#include <MXA/Utilities/MXADir.h>

#include "DREAM3D/Common/Constants.h"
#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3D/Common/AIMArray.hpp"

#include "GrainGenerator/GrainGenerator.h"

/**
 * @brief Parses numeric values from a delimited string into a preallocated array storage.
 * The programmer MUST know in advance how many values there will be.
 * @param values The string to be parsed
 * @param format The stdio format specifier to use (%f for floats, %d for integers
 * @param output The output location to store the parsed values
 * @return Error condition
 */
template<typename T>
int parseValues(const std::string &values, const char* format, T* output)
{
  std::string::size_type pos = values.find(",", 0);
  size_t index = 0;
  int n = sscanf(values.substr(0, pos).c_str(), format, &(output[index]) );
  if (n != 1)
  {
    return -1;
  }

  ++index;
  while(pos != std::string::npos && pos != values.size() - 1)
  {
    n = sscanf(values.substr(pos+1).c_str(), format, &(output[index]) );
    pos = values.find(",", pos+1);
    ++index;
  }
  return 0;
}

/**
 * @brief Parses unknown number of numeric values from a delimited string and places
 * the values into the output variable.
 * @param values The string to be parsed
 * @param format The stdio format specifier to use (%f for floats, %d for integers
 * @param output The output location to store the parsed values
 * @return Error condition
 */
template<typename T>
int parseUnknownArray(const std::string &values, const char* format, std::vector<T> &output)
{
  std::string::size_type pos = values.find(",", 0);
  size_t index = 0;
  T t;
  int n = sscanf(values.substr(0, pos).c_str(), format, &t );
  if (n != 1)
  {
    return -1;
  }
  output.push_back(t);

  ++index;
  while(pos != std::string::npos && pos != values.size() - 1)
  {
    n = sscanf(values.substr(pos+1).c_str(), format, &(t) );
    output.push_back(t);
    pos = values.find(",", pos+1);
    ++index;
  }
  return 0;
}

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
    TCLAP::CmdLine cmd("DREAM.3D Grain Generator", ' ', DREAM3DLib::Version::Complete);

    TCLAP::ValueArg<std::string>   h5StatsFile( "i", "input", "HDF5 Stats File", true, "", "HDF5 Stats File");
    cmd.add(h5StatsFile);

    TCLAP::ValueArg<std::string>  voxels( "", "voxels", "Comma separated list of integer in microns (x,y,x)", true, "64,64,64", "Size of Volume in Voxels");
    cmd.add(voxels);


    TCLAP::ValueArg<std::string>  resolution( "", "resolution", "Comma separated list of floating point values in microns (x,y,x).", true, "0.25,0.25,0.25", "Micron Resolution");
    cmd.add(resolution);

    TCLAP::ValueArg<float>  m_SizeDistErrorWeight( "", "sizedisterror", "Size Distribution Error Weight", false, 1.0, "Size Distribution Error Weight");
    cmd.add(m_SizeDistErrorWeight);

    TCLAP::ValueArg<float>  m_NeighborhoodErrorWeight( "", "neighborhooderror", "Neighborhood Error Weight", false, 1.0, "Neighborhood Error Weight");
    cmd.add(m_NeighborhoodErrorWeight);

    TCLAP::ValueArg<float>  m_FillingErrorWeight( "", "spacefillerror", "Space Filling Error Weight", false, 1.0, "Space Filling Error Weight");
    cmd.add(m_FillingErrorWeight);

    TCLAP::SwitchArg   m_PeriodicBoundaryConditions( "", "periodic", "Enable Periodic Boundary Conditions", false);
    cmd.add(m_PeriodicBoundaryConditions);

    TCLAP::ValueArg<std::string>  shapeTypeStr( "", "shapetypes", "Comma separated list of shape type for each phase: (0)Ellipsoid (1)Superellipsoid (2)Cube-Octahedron (3)Cylinder:", false, "0", "Shape Type");
    cmd.add(shapeTypeStr);

    TCLAP::ValueArg<std::string > outputDir("", "outputdir", "Output Directory For Files", true, "", "Output Directory");
    cmd.add(outputDir);

    TCLAP::ValueArg<std::string > outputPrefix("", "outputprefix", "Prefix for generated files.", true, "", "Output File Prefix");
    cmd.add(outputPrefix);


    TCLAP::SwitchArg   m_VisualizationVizFile( "", "no-vtk", "Disable Writing of VTK File", true);
    cmd.add(m_VisualizationVizFile);
    TCLAP::SwitchArg   m_WriteSurfaceVoxelScalars( "", "no-surfacevoxels", "Disable Writing of Surface Voxel Scalars", true);
    cmd.add(m_WriteSurfaceVoxelScalars);
    TCLAP::SwitchArg   m_WritePhaseIdScalars( "", "no-phase", "Disable Writing of Phase ID Scalars", true);
    cmd.add(m_WritePhaseIdScalars);
    TCLAP::SwitchArg   m_WriteIPFColorScalars( "", "no-ipf", "Disable Writing of IPF Colors Scalars", true);
    cmd.add(m_WriteIPFColorScalars);
    TCLAP::SwitchArg   m_WriteBinaryVTKFile( "", "vtk_ascii", "Write ASCII Vtk Files instead of Binary Files", true);
    cmd.add(m_WriteBinaryVTKFile);

    TCLAP::SwitchArg   m_HDF5GrainFile( "", "hdf5-grain", "Write the HDF5 Grain file (.h5grain).", false);
    cmd.add(m_HDF5GrainFile);

    TCLAP::SwitchArg   m_PhFile( "", "no-phfile", "Disable Writing PH file", true);
    cmd.add(m_PhFile);

    TCLAP::SwitchArg   m_writegraindata( "", "no-graindata", "Disable the Writing of the Grain Data file", true);
    cmd.add(m_writegraindata);



    if (argc < 2)
    {
      std::cout << "DREAM.3D Grain Generator Command Line Version " << cmd.getVersion() << std::endl;
      std::vector<std::string> args;
      args.push_back(argv[0]);
      args.push_back("-h");
      cmd.parse(args);
      return -1;
    }

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
    m_GrainGenerator->setOutputFilePrefix(outputPrefix.getValue());

    int voxels_values[3];
    if (parseValues(voxels.getValue(), "%d", voxels_values) < 0)
    {
      std::cout << "Error Parsing the Voxel Dimensions. They should be entered as --voxels 64,128,256" << std::endl;
      return EXIT_FAILURE;
    }
    m_GrainGenerator->setXPoints(voxels_values[0]);
    m_GrainGenerator->setYPoints(voxels_values[1]);
    m_GrainGenerator->setZPoints(voxels_values[2]);

    //TODO: Get the Shape Class from the Command line options
  //  m_GrainGenerator->setShapeTypes(shapeClass.getValue());
    float resolution_values[3];
    if (parseValues(resolution.getValue(), "%f", resolution_values) < 0)
    {
      std::cout << "Error Parsing the Resolution. They should be entered as --resolution 0.25,0.25,0.25" << std::endl;
      return EXIT_FAILURE;
    }
    m_GrainGenerator->setXResolution(resolution_values[0]);
    m_GrainGenerator->setYResolution(resolution_values[1]);
    m_GrainGenerator->setZResolution(resolution_values[2]);

    m_GrainGenerator->setFillingErrorWeight(m_FillingErrorWeight.getValue());
    m_GrainGenerator->setNeighborhoodErrorWeight(m_NeighborhoodErrorWeight.getValue());
    m_GrainGenerator->setSizeDistErrorWeight(m_SizeDistErrorWeight.getValue());

    m_GrainGenerator->setPeriodicBoundary(m_PeriodicBoundaryConditions.getValue());

    std::vector<DREAM3D::SyntheticBuilder::ShapeType> shapeTypes(1, DREAM3D::SyntheticBuilder::EllipsoidShape);
    if ( parseUnknownArray(shapeTypeStr.getValue(), "%d", shapeTypes) < 0)
    {
      std::cout << "Error parsing the Shape Types. The value should be entered as --shapetypes 1,0,1 for 3 phases." << std::endl;
      return EXIT_FAILURE;
    }
    m_GrainGenerator->setShapeTypes(shapeTypes);


    m_GrainGenerator->setAlreadyFormed(false);

    m_GrainGenerator->setWriteVtkFile(m_VisualizationVizFile.getValue());
    m_GrainGenerator->setWriteSurfaceVoxel(m_WriteSurfaceVoxelScalars.getValue());
    m_GrainGenerator->setWritePhaseId(m_WritePhaseIdScalars.getValue());
    m_GrainGenerator->setWriteIPFColor(m_WriteIPFColorScalars.getValue());
    m_GrainGenerator->setWriteBinaryVTKFiles(m_WriteBinaryVTKFile.getValue());

    m_GrainGenerator->setWriteHDF5GrainFile(m_HDF5GrainFile.getValue());
    m_GrainGenerator->setWritePhFile(m_PhFile.getValue());

    m_GrainGenerator->setWriteGrainData(m_writegraindata.getValue());


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
