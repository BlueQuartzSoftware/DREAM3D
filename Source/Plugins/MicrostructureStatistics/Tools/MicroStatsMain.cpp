/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXALogger.h"
#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/EbsdColoring.hpp"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/VTKUtils/VTKFileWriters.hpp"
#include "DREAM3DLib/IOFilters/FieldDataCSVWriter.h"
#include "DREAM3DLib/IOFilters/DataContainerReader.h"
#include "DREAM3DLib/PrivateFilters/FindNeighbors.h"
#include "DREAM3DLib/PrivateFilters/FindBoundingBoxGrains.h"
#include "DREAM3DLib/PrivateFilters/FindSurfaceGrains.h"
#include "DREAM3DLib/StatisticsFilters/LoadVolume.h"
#include "DREAM3DLib/StatisticsFilters/FindSizes.h"
#include "DREAM3DLib/StatisticsFilters/FindShapes.h"
#include "DREAM3DLib/StatisticsFilters/FindSchmids.h"
#include "DREAM3DLib/StatisticsFilters/FindNeighborhoods.h"
#include "DREAM3DLib/StatisticsFilters/FindDeformationStatistics.h"
#include "DREAM3DLib/StatisticsFilters/FindLocalMisorientationGradients.h"
#include "DREAM3DLib/StatisticsFilters/FindAvgOrientations.h"
#include "DREAM3DLib/StatisticsFilters/FindAxisODF.h"
#include "DREAM3DLib/StatisticsFilters/FindODF.h"
#include "DREAM3DLib/StatisticsFilters/FindMDF.h"
#include "DREAM3DLib/StatisticsFilters/FindEuclideanDistMap.h"
#include "DREAM3DLib/StatisticsFilters/WriteH5StatsFile.h"

#define MAKE_OUTPUT_FILE_PATH(outpath, filename, outdir, prefix)\
    std::string outpath = outdir + MXADir::Separator + prefix + filename;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
int parseValues(const std::string &values, const char* format, T* output)
{
  std::string::size_type pos = values.find(",", 0);
  size_t index = 0;
  int n = sscanf(values.substr(0, pos).c_str(), format, &(output[index]));
  if(n != 1)
  {
    return -1;
  }

  ++index;
  while (pos != std::string::npos && pos != values.size() - 1)
  {
    n = sscanf(values.substr(pos + 1).c_str(), format, &(output[index]));
    pos = values.find(",", pos + 1);
    ++index;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
int parseUnknownArray(const std::string &values, const char* format, std::vector<T> &output)
{
  std::string::size_type pos = values.find(",", 0);
  size_t index = 0;
  T t;
  int n = sscanf(values.substr(0, pos).c_str(), format, &t);
  if(n != 1)
  {
    return -1;
  }
  output.push_back(t);

  ++index;
  while (pos != std::string::npos && pos != values.size() - 1)
  {
    n = sscanf(values.substr(pos + 1).c_str(), format, &(t));
    output.push_back(t);
    pos = values.find(",", pos + 1);
    ++index;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::cout << logTime() << "Starting Microstructure Statistics... " << std::endl;
  MXALOGGER_METHOD_VARIABLE_INSTANCE

  int err = 0;
  try
  {

    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("DREAM.3D Microstructure Statistics", ' ', DREAM3DLib::Version::Complete);

    TCLAP::ValueArg<std::string> m_InputFile("i", "input", "HDF5 Voxel File", true, "", "HDF5 Voxel File");
    cmd.add(m_InputFile);
    TCLAP::ValueArg<float> m_BinStepSize("", "binstepsize", "Size of each Bin in the Histogram", false, 1.0, "Size of each Bin in the Histogram");
    cmd.add(m_BinStepSize);
    TCLAP::SwitchArg m_H5StatisticsFile("", "no-h5stats", "Disable Writing of the H5 Statistics file", true);
    cmd.add(m_H5StatisticsFile);

    /* The switch arguments at first seem to be backward but they are not. The _absence_
     * of the argument on the command line will set the variable to its default value.
     * For example, the "no-vtk" switch has a default value of "true" which means
     * that if the user does NOT use the "--no-vtk" the m_VisualizationVizFile variable
     * will have a "TRUE" value and the vtk file WILL be written. If the user adds
     * the "--no-vtk" to the command line then the "m_VisualizationVizFile" variable
     * will be "FALSE" and the vtk file will NOT be written
     */
    TCLAP::SwitchArg m_VisualizationVizFile("", "no-vtk", "Disable Writing of VTK File", true);
    cmd.add(m_VisualizationVizFile);
    TCLAP::SwitchArg m_WriteSurfaceVoxelScalars_arg("", "no-surfacevoxels", "Disable Writing of Surface Voxel Scalars", true);
    cmd.add(m_WriteSurfaceVoxelScalars_arg);
    TCLAP::SwitchArg m_WritePhaseIdScalars_arg("", "no-phase", "Disable Writing of Phase ID Scalars", true);
    cmd.add(m_WritePhaseIdScalars_arg);
    TCLAP::SwitchArg m_WriteIPFColorScalars_arg("", "no-ipf", "Disable Writing of IPF Colors Scalars", true);
    cmd.add(m_WriteIPFColorScalars_arg);
    TCLAP::SwitchArg m_WriteKernelMisorientationsScalars_arg("", "no-kernel-miso", "Enable Periodic Boundary Conditions", true);
    cmd.add(m_WriteKernelMisorientationsScalars_arg);
    TCLAP::SwitchArg m_WriteBinaryVTKFile_arg("", "vtk_ascii", "Write ASCII Vtk Files instead of Binary Files", true);
    cmd.add(m_WriteBinaryVTKFile_arg);


    TCLAP::SwitchArg m_GrainDataFile("", "no-grain-file", "Disable Writing of the Grain Data file", true);
    cmd.add(m_GrainDataFile);
    TCLAP::SwitchArg m_WriteGrainSize_arg("", "no-GrainSize", "Write the Grain Sizes", true);
    cmd.add(m_WriteGrainSize_arg);
    TCLAP::SwitchArg m_WriteGrainShapes_arg("", "no-GrainShapes", "Write the Grain Sizes", true);
    cmd.add(m_WriteGrainShapes_arg);
    TCLAP::SwitchArg m_WriteNumNeighbors_arg("", "no-NumNeighbors", "Write the Grain Sizes", true);
    cmd.add(m_WriteNumNeighbors_arg);
    TCLAP::SwitchArg m_WriteAverageOrientations_arg("", "no-AverageOrientations", "Write the Grain Sizes", true);
    cmd.add(m_WriteAverageOrientations_arg);


    TCLAP::ValueArg<std::string> outputDir("", "outputdir", "Output Directory For Files", true, "", "Output Directory");
    cmd.add(outputDir);

    TCLAP::ValueArg<std::string> outputPrefix("", "outputprefix", "Prefix for generated files.", true, "", "Output File Prefix");
    cmd.add(outputPrefix);

    if(argc < 2)
    {
      std::cout << "DREAM.3D Microstructure Statistics Command Line Version " << cmd.getVersion() << std::endl;
      std::vector<std::string> args;
      args.push_back(argv[0]);
      args.push_back("-h");
      cmd.parse(args);
      return -1;
    }

    // Parse the argv array.
    cmd.parse(argc, argv);
    if(argc == 1)
    {
      std::cout << "Microstructure Statistics program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }

    MXADir::mkdir(outputDir.getValue(), true);


    bool m_WriteGrainSize = m_WriteGrainSize_arg.getValue();
    bool m_WriteGrainShapes = m_WriteGrainShapes_arg.getValue();
    bool m_WriteNumNeighbors = m_WriteNumNeighbors_arg.getValue();
    bool m_WriteAverageOrientations = m_WriteAverageOrientations_arg.getValue();
    bool m_WriteSurfaceVoxelScalars = m_WriteSurfaceVoxelScalars_arg.getValue();
    bool m_WritePhaseIdScalars = m_WritePhaseIdScalars_arg.getValue();
    bool m_WriteKernelMisorientationsScalars = m_WriteKernelMisorientationsScalars_arg.getValue();
    bool m_WriteIPFColorScalars = m_WriteIPFColorScalars_arg.getValue();
    bool m_WriteBinaryVTKFile = m_WriteBinaryVTKFile_arg.getValue();

    bool m_ComputeGrainSize = false;
    bool m_ComputeGrainShapes = false;
    bool m_ComputeNumNeighbors = false;
    bool m_ComputeAverageOrientations = false;
    if(m_H5StatisticsFile.getValue() == true || m_WriteGrainSize == true) m_ComputeGrainSize = true;
    if(m_H5StatisticsFile.getValue() == true || m_WriteGrainShapes == true) m_ComputeGrainShapes = true;
    if(m_H5StatisticsFile.getValue() == true || m_WriteNumNeighbors == true) m_ComputeNumNeighbors = true;
    if(m_WriteAverageOrientations == true) m_ComputeAverageOrientations = true;
    if(m_GrainDataFile.getValue() == false)
    {
      m_WriteGrainSize = false;
      m_WriteGrainShapes = false;
      m_WriteNumNeighbors = false;
      m_WriteAverageOrientations = false;
    }
    if(m_VisualizationVizFile.getValue() == false)
    {
      m_WriteSurfaceVoxelScalars = false;
      m_WritePhaseIdScalars = false;
      m_WriteKernelMisorientationsScalars = false;
      m_WriteIPFColorScalars = false;
      m_WriteBinaryVTKFile = false;
    }
#if 0
    // Move the MicrostructureStatistics object into the thread that we just created.
    m_MicrostructureStatistics->setOutputDirectory(MXADir::toNativeSeparators(m_OutputDir.getValue()));
    m_MicrostructureStatistics->setOutputFilePrefix(m_OutputFilePrefix.getValue());
    m_MicrostructureStatistics->setInputFile(MXADir::toNativeSeparators(m_InputFile.getValue()));

    /********************************
     * Gather any other values from the User Interface and send those to the lower level code
     */
    m_MicrostructureStatistics->setComputeGrainSize(m_ComputeGrainSize);
    m_MicrostructureStatistics->setComputeGrainShapes(m_ComputeGrainShapes);
    m_MicrostructureStatistics->setComputeNumNeighbors(m_ComputeNumNeighbors);

    m_MicrostructureStatistics->setBinStepSize(m_BinStepSize.getValue());
    m_MicrostructureStatistics->setWriteH5StatsFile(m_H5StatisticsFile.getValue());

    m_MicrostructureStatistics->setWriteGrainFile(m_GrainDataFile.getValue());
    m_MicrostructureStatistics->setWriteGrainSize(m_WriteGrainSize);
    m_MicrostructureStatistics->setWriteGrainShapes(m_WriteGrainShapes);
    m_MicrostructureStatistics->setWriteNumNeighbors(m_WriteNumNeighbors);
    m_MicrostructureStatistics->setWriteAverageOrientations(m_WriteAverageOrientations);

    m_MicrostructureStatistics->setWriteVtkFile(m_VisualizationVizFile.getValue());
    m_MicrostructureStatistics->setWriteSurfaceVoxel(m_WriteSurfaceVoxelScalars);
    m_MicrostructureStatistics->setWritePhaseId(m_WritePhaseIdScalars);
    m_MicrostructureStatistics->setWriteKernelMisorientations(m_WriteKernelMisorientationsScalars);
    m_MicrostructureStatistics->setWriteIPFColor(m_WriteIPFColorScalars);
    m_MicrostructureStatistics->setWriteBinaryVTKFiles(m_WriteBinaryVTKFile);

    m_MicrostructureStatistics->printSettings(std::cout);

    m_MicrostructureStatistics->run();
    err = m_MicrostructureStatistics->getErrorCondition();

#endif


    // Create a FilterPipeline Object to hold all the filters. Later on we will execute all the filters
    FilterPipeline::Pointer m_FilterPipeline = FilterPipeline::New();

    MAKE_OUTPUT_FILE_PATH( reconDeformStatsFile, DREAM3D::MicroStats::DeformationStatsFile, outputDir.getValue(), outputPrefix.getValue());
    MAKE_OUTPUT_FILE_PATH( reconDeformIPFFile, DREAM3D::MicroStats::IPFDeformVTKFile, outputDir.getValue(), outputPrefix.getValue());
    MAKE_OUTPUT_FILE_PATH( reconVisFile, DREAM3D::Reconstruction::VisualizationVizFile, outputDir.getValue(), outputPrefix.getValue());
    MAKE_OUTPUT_FILE_PATH( hdf5ResultsFile, DREAM3D::MicroStats::H5StatisticsFile, outputDir.getValue(), outputPrefix.getValue())


    LoadVolume::Pointer load_volume = LoadVolume::New();
    load_volume->setInputFile(m_InputFile.getValue());
    m_FilterPipeline->pushBack(load_volume);

    FindSurfaceGrains::Pointer find_surfacegrains = FindSurfaceGrains::New();
    m_FilterPipeline->pushBack(find_surfacegrains);

    // Start Computing the statistics
    if(m_ComputeGrainSize == true)
    {
      FindSizes::Pointer find_sizes = FindSizes::New();
      m_FilterPipeline->pushBack(find_sizes);
    }

    if(m_ComputeGrainShapes == true)
    {
      FindShapes::Pointer find_shapes = FindShapes::New();
      m_FilterPipeline->pushBack(find_shapes);

      FindBoundingBoxGrains::Pointer find_boundingboxgrains = FindBoundingBoxGrains::New();
      m_FilterPipeline->pushBack(find_boundingboxgrains);
    }

    if(m_ComputeNumNeighbors == true)
    {
      FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
      m_FilterPipeline->pushBack(find_neighbors);

      FindNeighborhoods::Pointer find_neighborhoods = FindNeighborhoods::New();
      m_FilterPipeline->pushBack(find_neighborhoods);
    }

    if(m_WriteAverageOrientations == true
        || m_H5StatisticsFile.getValue() == true
        || m_WriteKernelMisorientationsScalars == true)
    {
      FindAvgOrientations::Pointer find_avgorientations = FindAvgOrientations::New();
      m_FilterPipeline->pushBack(find_avgorientations);
    }

    if(m_H5StatisticsFile.getValue() == true)
    {
      // Create a new Writer for the Stats Data.
      FindAxisODF::Pointer find_axisodf = FindAxisODF::New();
      find_axisodf->setH5StatsFile(hdf5ResultsFile);
      find_axisodf->setCreateNewStatsFile(true);
      m_FilterPipeline->pushBack(find_axisodf);

      FindODF::Pointer find_odf = FindODF::New();
      find_odf->setH5StatsFile(hdf5ResultsFile);
      find_odf->setCreateNewStatsFile(false);
      m_FilterPipeline->pushBack(find_odf);

      FindMDF::Pointer find_mdf = FindMDF::New();
      find_mdf->setH5StatsFile(hdf5ResultsFile);
      find_mdf->setCreateNewStatsFile(false);
      m_FilterPipeline->pushBack(find_mdf);

      WriteH5StatsFile::Pointer write_h5statsfile = WriteH5StatsFile::New();
      write_h5statsfile->setBinStepSize(m_BinStepSize.getValue());
      write_h5statsfile->setH5StatsFile(hdf5ResultsFile);
      write_h5statsfile->setCreateNewStatsFile(false);
      m_FilterPipeline->pushBack(write_h5statsfile);
    }

    if(m_WriteKernelMisorientationsScalars == true)
    {
      FindLocalMisorientationGradients::Pointer find_localmisorientationgradients = FindLocalMisorientationGradients::New();
      m_FilterPipeline->pushBack(find_localmisorientationgradients);
    }


    FindSchmids::Pointer find_schmids = FindSchmids::New();
    m_FilterPipeline->pushBack(find_schmids);

    FindEuclideanDistMap::Pointer find_euclideandistmap = FindEuclideanDistMap::New();
    m_FilterPipeline->pushBack(find_euclideandistmap);

    FindDeformationStatistics::Pointer find_deformationstatistics = FindDeformationStatistics::New();
    find_deformationstatistics->setDeformationStatisticsFile(reconDeformStatsFile);
    find_deformationstatistics->setVtkOutputFile(reconDeformIPFFile);
    m_FilterPipeline->pushBack(find_deformationstatistics);



    if(m_GrainDataFile.getValue() == true)
    {
      MAKE_OUTPUT_FILE_PATH( FieldDataFile, DREAM3D::MicroStats::GrainDataFile, outputDir.getValue(), outputPrefix.getValue());
      FieldDataCSVWriter::Pointer write_fielddata = FieldDataCSVWriter::New();
      write_fielddata->setFieldDataFile(FieldDataFile);
      m_FilterPipeline->pushBack(write_fielddata);
    }

    // Now preflight this pipeline to make sure we can actually run it
    int err = m_FilterPipeline->preflightPipeline();
    // If any error occured during the preflight exit now
    if(err < 0)
    {

      // Show a Dialog with the error from the Preflight
      return EXIT_FAILURE;
    }
    DataContainer::Pointer m = DataContainer::New();
    m_FilterPipeline->setDataContainer(m);

    m_FilterPipeline->run();
    err = m_FilterPipeline->getErrorCondition();
    if (err < 0)
    {
      std::cout << "Microstructure Statistics threw an error during execution." << std::endl;
      return EXIT_FAILURE;
    }

  }

  catch (TCLAP::ArgException &e) // catch any exceptions
  {
    std::cerr << logTime() << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "++++++++++++ Synthetic Builder Complete ++++++++++++" << std::endl;
  return err;
}
