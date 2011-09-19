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

#include "MicrostructureStatistics.h"

#include "MXA/MXATypes.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"

#include "EbsdLib/TSL/AngDirectoryPatterns.h"
#include "EbsdLib/TSL/AngReader.h"

#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/OIMColoring.hpp"
#include "DREAM3D/Common/VTKUtils/VTKFileWriters.hpp"
#include "DREAM3D/HDF5/H5ReconStatsWriter.h"
#include "DREAM3D/HDF5/H5VoxelReader.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicrostructureStatistics::MicrostructureStatistics() :
m_OutputDirectory("."),
m_OutputFilePrefix("MicrostructureStatistics_"),
m_ComputeGrainSize(false),
m_ComputeGrainShapes(false),
m_ComputeNumNeighbors(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicrostructureStatistics::~MicrostructureStatistics()
{
 // std::cout << "~MicrostructureStatistics()" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatistics::execute()
{
  int err = -1;
  //std::cout << "MicrostructureStatistics::compute Start" << std::endl;
  // Start the Benchmark Clock
  START_CLOCK()

  m = MicrostructureStatisticsFunc::New();


  MAKE_OUTPUT_FILE_PATH ( graindataFile, DREAM3D::MicroStats::GrainDataFile);
  MAKE_OUTPUT_FILE_PATH ( reconDeformStatsFile, DREAM3D::MicroStats::DeformationStatsFile);
  MAKE_OUTPUT_FILE_PATH ( reconDeformIPFFile, DREAM3D::MicroStats::IPFDeformVTKFile);
  MAKE_OUTPUT_FILE_PATH ( reconVisFile, DREAM3D::Reconstruction::VisualizationVizFile);
  MAKE_OUTPUT_FILE_PATH ( hdf5ResultsFile, DREAM3D::MicroStats::H5StatisticsFile)


  updateProgressAndMessage("Reading the Voxel Dimensions and Resolution", 2);
  // Load up the voxel data
  H5VoxelReader::Pointer h5Reader = H5VoxelReader::New();
  h5Reader->setFilename(m_InputFile);
  int dims[3];
  float spacing[3];
  err = h5Reader->getSizeAndResolution(dims, spacing);
  if (err < 0)
  {
    CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "Error Reading the Dimensions and Resolution from the input File.", err);
    setErrorCondition(err);
    return;
  }

  m->xpoints = dims[0];
  m->ypoints = dims[1];
  m->zpoints = dims[2];
  m->totalpoints = dims[0] * dims[1] * dims[2];
  m->resx = spacing[0];
  m->resy = spacing[1];
  m->resz = spacing[2];
  m->sizex = dims[0]*spacing[0];
  m->sizey = dims[1]*spacing[1];
  m->sizez = dims[2]*spacing[2];

  m->computesizes = m_ComputeGrainSize;
  m->computeshapes = m_ComputeGrainShapes;
  m->computeneighbors = m_ComputeNumNeighbors;

  updateProgressAndMessage("Allocating Memory", 5);
  m->initializeAttributes();

  updateProgressAndMessage(("Reading the Voxel Data from the HDF5 File"), 10);
  err = h5Reader->readVoxelData(m->m_GrainIndicies, m->m_Phases, m->m_Euler1s, m->m_Euler2s, m->m_Euler3s, m->crystruct, m->totalpoints);
  CHECK_FOR_ERROR(MicrostructureStatisticsFunc, (h5Reader->getErrorMessage()), err);
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  readVoxelData)

// We need to allocate the proper number of grains, assign the voxel list to them
  m->initializeGrains();
  m->initializeArrays();

  updateProgressAndMessage(("Finding Surface Grains"), 20);
  if(m->zpoints > 1) m->find_surfacegrains();
  if(m->zpoints == 1) m->find_surfacegrains2D();
  CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "Error Finding Surface Grains", err);
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_surfacegrains2D)

  // Start Computing the statistics
  if(m_ComputeGrainSize == true || m_ComputeGrainShapes == true || m_ComputeNumNeighbors == true)
  {
	  updateProgressAndMessage(("Determining Grain Sizes"), 25);
	  if(m->zpoints > 1) m->find_centroids();
	  if(m->zpoints == 1)m->find_centroids2D();
	  CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "Error determining the grain size", err);
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_centroids2D)

	  updateProgressAndMessage(("Determining Bounding Box"), 28);
	  if(m->zpoints > 1) m->find_boundingboxgrains();
	  if(m->zpoints == 1)m->find_boundingboxgrains2D();
	  CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "Error determining the bounding box", err);
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_centroids2D)
  }

  if(m_ComputeGrainShapes == true)
  {
	  updateProgressAndMessage(("Finding Grain Moments"), 30);
	  if(m->zpoints > 1) m->find_moments();
	  if(m->zpoints == 1) m->find_moments2D();
	  CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "Error Finding the Grain Moments", err);
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_moments2D)

	  updateProgressAndMessage(("Finding Grain Principal Axes Lengths"), 35);
	  if(m->zpoints > 1) m->find_axes();
	  if(m->zpoints == 1) m->find_axes2D();
	  CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "Error Finding the Grain Principal Axes Length", err);
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_axes2D)
  }

  if(m_ComputeNumNeighbors == true)
  {
	  updateProgressAndMessage(("Finding Neighbors"), 45);
	  m->find_neighbors();
	  CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "Error finding neighbors", err);
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_neighbors)

	  updateProgressAndMessage(("Defining Neighborhoods"), 55);
	  m->define_neighborhood();
	  CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "Error defining neighborhoods", err);
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  define_neighborhood)
  }

  if(m_WriteAverageOrientations == true || m_WriteH5StatsFile == true || m_WriteKernelMisorientations == true)
  {
	  updateProgressAndMessage(("Finding Average Orientations For Grains"), 60);
	  m->find_grainorientations();
	  CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "Error finding average orientations for Grains", err);
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_grain_and_kernel_misorientations)
  }

  if(m_WriteH5StatsFile == true)
  {
	  // Create a new Writer for the Stats Data.
	  H5ReconStatsWriter::Pointer h5io = H5ReconStatsWriter::New(hdf5ResultsFile);

	  updateProgressAndMessage(("Finding Grain Axes ODF"), 65);
	  if(m->zpoints > 1) m->find_vectors(h5io);
	  if(m->zpoints == 1) m->find_vectors2D(h5io);
	  CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "Error Finding Grain Axes ODF.", err);
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_vectors2D)

	  updateProgressAndMessage(("Finding Euler ODF"), 70);
	  m->find_eulerodf(h5io);
	  CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "Error Finding Euler ODF", err);
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_eulerodf)

	  updateProgressAndMessage(("Measuring Misorientations"), 75);
	  m->measure_misorientations(h5io);
	  CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "Error measuring misorientations", err);
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  measure_misorientations)

	  updateProgressAndMessage(("Writing Statistics"), 85);
	  if(m->zpoints > 1) { m->volume_stats(h5io, m_BinStepSize); }
	  if(m->zpoints == 1) { m->volume_stats2D(h5io, m_BinStepSize); }
	  CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "Error writing statistics file.", err);
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  volume_stats)
  }

  if(m_WriteKernelMisorientations == true)
  {
	  updateProgressAndMessage(("Finding Reference Orientations For Grains"), 90);
	  m->find_grain_and_kernel_misorientations();
	  CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "Error finding reference orientations for Grains", err);
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_grain_and_kernel_misorientations)
  }

/*  updateProgressAndMessage(("Finding Grain Schmid Factors"), 20);
  m->find_schmids();
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_schmids)

  updateProgressAndMessage(("Finding Euclidean Distance Maps"), 50);
  m->find_euclidean_map();
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_euclidean_map)

  updateProgressAndMessage(("Writing Deformation Statistics"), 80);
  m->deformation_stats(reconDeformStatsFile, reconDeformIPFFile);
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  volume_stats)
*/

  if(m_WriteGrainFile == true)
  {
	  updateProgressAndMessage(("Writing Grain Data"), 95);
	  m->write_graindata(graindataFile, m_WriteGrainSize, m_WriteGrainShapes, m_WriteNumNeighbors, m_WriteAverageOrientations);
	  CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "Error writing grain data file.", err);
  }

  /* ********** This section writes the VTK files for visualization *** */
  if (m_WriteVtkFile) {
    updateProgressAndMessage(("Writing VTK Visualization File"), 98);
    MAKE_OUTPUT_FILE_PATH ( vtkVizFile, DREAM3D::Reconstruction::VisualizationVizFile);

    // Setup all the classes that will help us write the Scalars to the VTK File
    std::vector<VtkScalarWriter*> scalarsToWrite;
    {
      VtkScalarWriter* w0 =
      static_cast<VtkScalarWriter*>(new VoxelGrainIdScalarWriter<MicrostructureStatisticsFunc>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if (m_WriteSurfaceVoxel == true) {
      VtkScalarWriter* w0 =
        static_cast<VtkScalarWriter*>(new VoxelSurfaceVoxelScalarWriter<MicrostructureStatisticsFunc>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if (m_WritePhaseId == true){
      VtkScalarWriter* w0 =
        static_cast<VtkScalarWriter*>(new VoxelPhaseIdScalarWriter<MicrostructureStatisticsFunc>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if (m_WriteKernelMisorientations == true){
      VtkScalarWriter* w0 =
        static_cast<VtkScalarWriter*>(new VoxelKAMScalarWriter<MicrostructureStatisticsFunc>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if (m_WriteIPFColor == true) {
      VtkScalarWriter* w0 =
        static_cast<VtkScalarWriter*>(new VoxelIPFColorScalarWriter<MicrostructureStatisticsFunc>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    // Create our File Output Writer Object. This will handle all the File Output duties
    VTKRectilinearGridFileWriter vtkWriter;
    vtkWriter.setWriteBinaryFiles(m_WriteBinaryVTKFiles);
    err = vtkWriter.write<MicrostructureStatisticsFunc>(vtkVizFile, m.get(), scalarsToWrite);
    // Now Delete all the Scalar Helpers that we just created and used.
    for (std::vector<VtkScalarWriter*>::iterator iter = scalarsToWrite.begin(); iter != scalarsToWrite.end(); ++iter )
    {
      delete (*iter);
    }

    CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "The MicrostructureStatisticsFunc threw an Error writing the VTK file format.", err);
  }

  // Clean up all the memory by forcibly setting a NULL pointer to the Shared
  // pointer object.
  m = MicrostructureStatisticsFunc::NullPointer();  // Clean up the memory
  updateProgressAndMessage("MicrostructureStatistics Complete", 100);

}


#define PRINT_PROPERTY( out, var)\
  out << #var << ": " << m_##var << std::endl;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatistics::printSettings(std::ostream &ostream)
{
  ostream << " ###################################### " << std::endl;
  ostream << "MicrostructureStatistics Settings Being Used" << std::endl;
    PRINT_PROPERTY(ostream, InputFile)
    PRINT_PROPERTY(ostream, OutputDirectory)
    PRINT_PROPERTY(ostream, OutputFilePrefix)
    PRINT_PROPERTY(ostream, BinStepSize)
    PRINT_PROPERTY(ostream, WriteH5StatsFile)
    PRINT_PROPERTY(ostream, ComputeGrainSize)
    PRINT_PROPERTY(ostream, ComputeGrainShapes)
    PRINT_PROPERTY(ostream, ComputeNumNeighbors)
    PRINT_PROPERTY(ostream, ComputeMDF)
    PRINT_PROPERTY(ostream, WriteGrainFile)
    PRINT_PROPERTY(ostream, WriteGrainSize)
    PRINT_PROPERTY(ostream, WriteGrainShapes)
    PRINT_PROPERTY(ostream, WriteNumNeighbors)
    PRINT_PROPERTY(ostream, WriteAverageOrientations)


    PRINT_PROPERTY(ostream, WriteBinaryVTKFiles)
    PRINT_PROPERTY(ostream, WriteVtkFile)
    PRINT_PROPERTY(ostream, WriteSurfaceVoxel)
    PRINT_PROPERTY(ostream, WritePhaseId)
    PRINT_PROPERTY(ostream, WriteIPFColor)

    ostream << " ###################################### " << std::endl;

}
