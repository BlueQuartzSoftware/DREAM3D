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

#include "MicrostructureStatistics.h"

#include "MXA/MXATypes.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"

#include "ANG/AngDirectoryPatterns.h"
#include "ANG/AngReader.h"

#include "DREAM3D/ANGSupport/AbstractAngDataLoader.h"
#include "DREAM3D/ANGSupport/AngDataLoader.h"
#include "DREAM3D/ANGSupport/H5AngDataLoader.h"

#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/OIMColoring.hpp"
#include "DREAM3D/Common/VTKUtils/VTKFileWriters.hpp"
#include "DREAM3D/HDF5/H5ReconStatsWriter.h"
#include "DREAM3D/HDF5/H5GrainWriter.h"
#include "DREAM3D/HDF5/H5VoxelReader.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicrostructureStatistics::MicrostructureStatistics() :
m_OutputDirectory("."),
m_OutputFilePrefix("MicrostructureStatistics_"),
m_ComputeGrainSize(false),
m_ComputeGrainShapes(false),
m_ComputeNumNeighbors(false),
m_ComputeODF(false),
m_ComputeMDF(false)
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


  MAKE_OUTPUT_FILE_PATH ( graindataFile, AIM::MicroStats::GrainDataFile);
  MAKE_OUTPUT_FILE_PATH ( reconDeformStatsFile, AIM::MicroStats::DeformationStatsFile);
  MAKE_OUTPUT_FILE_PATH ( reconDeformIPFFile, AIM::MicroStats::IPFDeformVTKFile);
  MAKE_OUTPUT_FILE_PATH ( reconVisFile, AIM::Reconstruction::VisualizationVizFile);
  MAKE_OUTPUT_FILE_PATH ( hdf5ResultsFile, AIM::MicroStats::H5StatisticsFile)


  updateProgressAndMessage("Reading the Voxel Dimensions and Resolution", 2);
  // Load up the voxel data
  H5VoxelReader::Pointer h5Reader = H5VoxelReader::New();
  h5Reader->setFilename(m_InputFile);
  int dims[3];
  float spacing[3];
  err = h5Reader->getSizeAndResolution(dims, spacing);
  if (err < 0)
  {
    updateProgressAndMessage("Error Reading the Dimensions and Resolution from the File.", 100);
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
  m->computeodf = m_ComputeODF;
  m->computemdf = m_ComputeMDF;

  updateProgressAndMessage("Allocating Voxel Memory", 5);
  //Allocate all of our Voxel Objects
  m->voxels.reset(new MicrostructureStatisticsVoxel[m->totalpoints]);

  updateProgressAndMessage(("Reading the Voxel Data from the HDF5 File"), 10);
  err = h5Reader->readVoxelData(m->voxels, m->crystruct, m->totalpoints);
  if (err < 0)
  {
    updateProgressAndMessage("Error Reading the Voxel Data from the File.", 100);
    setErrorCondition(err);
    return;
  }
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  readVoxelData)

// We need to allocate the proper number of grains, assign the voxel list to them
  m->initializeGrains();
  m->initializeArrays();


  // Create a new Writer for the Stats Data.
  H5ReconStatsWriter::Pointer h5io = H5ReconStatsWriter::New(hdf5ResultsFile);

  updateProgressAndMessage(("Finding Surface Grains"), 20);
  if(m->zpoints > 1) m->find_surfacegrains();
  if(m->zpoints == 1) m->find_surfacegrains2D();
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_surfacegrains2D)

  // Start Computing the statistics
  if(m_ComputeGrainSize == true || m_ComputeGrainShapes == true || m_ComputeODF == true || m_ComputeNumNeighbors == true || m_ComputeMDF == true)
  {
	  updateProgressAndMessage(("Determining Grain Sizes"), 25);
	  if(m->zpoints > 1) m->find_centroids();
	  if(m->zpoints == 1)m->find_centroids2D();
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_centroids2D)
  }

  if(m_ComputeGrainShapes == true)
  {
	  updateProgressAndMessage(("Finding Grain Moments"), 30);
	  if(m->zpoints > 1) m->find_moments();
	  if(m->zpoints == 1) m->find_moments2D();
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_moments2D)

	  updateProgressAndMessage(("Finding Grain Principal Axes Lengths"), 35);
	  if(m->zpoints > 1) m->find_axes();
	  if(m->zpoints == 1) m->find_axes2D();
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_axes2D)

	  updateProgressAndMessage(("Finding Grain Pricipal Axes Vectors"), 40);
	  if(m->zpoints > 1) m->find_vectors(h5io);
	  if(m->zpoints == 1) m->find_vectors2D(h5io);
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_vectors2D)
  }

  if(m_ComputeNumNeighbors == true || m_ComputeMDF == true)
  {
	  updateProgressAndMessage(("Finding Neighbors"), 45);
	  m->find_neighbors();
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_neighbors)

	  updateProgressAndMessage(("Defining Neighborhoods"), 55);
	  m->define_neighborhood();
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  define_neighborhood)
  }

  if(m_ComputeODF == true || m_ComputeMDF == true)
  {
	  updateProgressAndMessage(("Finding Average Orientations For Grains"), 60);
	  m->find_grainorientations();
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_grain_and_kernel_misorientations)
  }

  if(m_ComputeODF == true)
  {
	  updateProgressAndMessage(("Finding Euler ODF"), 65);
	  m->find_eulerodf(h5io);
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_eulerodf)
  }

  if(m_ComputeMDF == true)
  {
	  updateProgressAndMessage(("Measuring Misorientations"), 75);
	  m->measure_misorientations(h5io);
	  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  measure_misorientations)
  }

  updateProgressAndMessage(("Writing Statistics"), 85);
  if(m->zpoints > 1) { m->volume_stats(h5io, m_BinStepSize); }
  if(m->zpoints == 1) { m->volume_stats2D(h5io, m_BinStepSize); }


/*  updateProgressAndMessage(("Finding Grain Schmid Factors"), 20);
  m->find_schmids();
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_schmids)

  updateProgressAndMessage(("Finding Reference Orientations For Grains"), 70);
  m->find_grain_and_kernel_misorientations();
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_grain_and_kernel_misorientations)

  updateProgressAndMessage(("Finding Euclidean Distance Maps"), 50);
  m->find_euclidean_map();
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  find_euclidean_map)

  updateProgressAndMessage(("Finding Grain IPF Colors"), 75);
  m->find_colors();
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled", find_colors)

  updateProgressAndMessage(("Writing Deformation Statistics"), 80);
  m->deformation_stats(reconDeformStatsFile, reconDeformIPFFile);
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics was canceled",  volume_stats)
*/

  updateProgressAndMessage(("Writing Grain Data"), 95);
  m->write_graindata(graindataFile, m_ComputeGrainSize, m_ComputeGrainShapes, m_ComputeNumNeighbors);

/*  if (m_WriteVtkFile) {
    VTKFileWriters::Pointer vtkWriter = VTKFileWriters::New();
    vtkWriter->setWriteBinaryFiles(m_WriteBinaryVTKFiles);
    updateProgressAndMessage(("Writing VTK Visualization File"), 93);
    err = vtkWriter->writeGrainVisualizationFile(m.get(), reconVisFile, m_WriteSurfaceVoxel, m_WritePhaseId, m_WriteIPFColor);
    CHECK_FOR_ERROR(MicrostructureStatisticsFunc, "The Microstructure Statistics threw an Error writing the VTK file format.", err);
  }
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, "MicrostructureStatistics Was canceled", writeVisualizationFile)
*/
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
  ostream << "MicrostructureStatistics Settings Being Used" << std::endl;

}
