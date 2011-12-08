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

#include "MXA/MXA.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"

#include "EbsdLib/TSL/AngDirectoryPatterns.h"
#include "EbsdLib/TSL/AngReader.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/EbsdColoring.hpp"
#include "DREAM3DLib/VTKUtils/VTKFileWriters.hpp"
#include "DREAM3DLib/StatisticsFilters/FindSurfaceGrains.h"
#include "DREAM3DLib/StatisticsFilters/LoadVolume.h"
#include "DREAM3DLib/StatisticsFilters/FindSizes.h"
#include "DREAM3DLib/StatisticsFilters/FindShapes.h"
#include "DREAM3DLib/StatisticsFilters/FindSchmids.h"
#include "DREAM3DLib/StatisticsFilters/FindBoundingBoxGrains.h"
#include "DREAM3DLib/GenericFilters/WriteFieldData.h"
#include "DREAM3DLib/GenericFilters/FindNeighbors.h"
#include "DREAM3DLib/StatisticsFilters/FindNeighborhoods.h"
#include "DREAM3DLib/StatisticsFilters/FindDeformationStatistics.h"
#include "DREAM3DLib/StatisticsFilters/FindLocalMisorientationGradients.h"
#include "DREAM3DLib/StatisticsFilters/FindAvgOrientations.h"
#include "DREAM3DLib/StatisticsFilters/FindAxisODF.h"
#include "DREAM3DLib/StatisticsFilters/FindODF.h"
#include "DREAM3DLib/StatisticsFilters/FindMDF.h"
#include "DREAM3DLib/StatisticsFilters/FindEuclideanDistMap.h"
#include "DREAM3DLib/StatisticsFilters/WriteH5StatsFile.h"

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

  m = DataContainer::New();

  MAKE_OUTPUT_FILE_PATH ( reconDeformStatsFile, DREAM3D::MicroStats::DeformationStatsFile);
  MAKE_OUTPUT_FILE_PATH ( reconDeformIPFFile, DREAM3D::MicroStats::IPFDeformVTKFile);
  MAKE_OUTPUT_FILE_PATH ( reconVisFile, DREAM3D::Reconstruction::VisualizationVizFile);
  MAKE_OUTPUT_FILE_PATH ( hdf5ResultsFile, DREAM3D::MicroStats::H5StatisticsFile)

    updateProgressAndMessage(("Reading the Voxel Data from the HDF5 File"), 10);
    LoadVolume::Pointer load_volume = LoadVolume::New();
	load_volume->setInputFile(m_InputFile);
    load_volume->addObserver(static_cast<Observer*>(this));
    load_volume->setDataContainer(m.get());
    load_volume->execute();
    err = load_volume->getErrorCondition();
    CHECK_FOR_ERROR(DataContainer, "Error Loading Volume", err);
    CHECK_FOR_CANCELED(DataContainer, "MicrostructureStatistics was canceled",  load_volume)

    updateProgressAndMessage(("Finding Surface Grains"), 20);
    FindSurfaceGrains::Pointer find_surfacegrains = FindSurfaceGrains::New();
    find_surfacegrains->addObserver(static_cast<Observer*>(this));
    find_surfacegrains->setDataContainer(m.get());
    find_surfacegrains->execute();
    err = find_surfacegrains->getErrorCondition();
    CHECK_FOR_ERROR(DataContainer, "Error Finding Surface Grains", err)
    CHECK_FOR_CANCELED(DataContainer, "GrainGenerator Was canceled", find_surfacegrains)
	
  // Start Computing the statistics
  if(m_ComputeGrainSize == true)
  {
	  updateProgressAndMessage(("Determining Grain Sizes"), 25);
	  FindSizes::Pointer find_sizes = FindSizes::New();
	  find_sizes->addObserver(static_cast<Observer*>(this));
	  find_sizes->setDataContainer(m.get());
	  find_sizes->execute();
	  err = find_sizes->getErrorCondition();
	  CHECK_FOR_ERROR(DataContainer, "Error Determining Grain Sizes", err);
	  CHECK_FOR_CANCELED(DataContainer, "MicrostructureStatistics was canceled",  find_sizes)
  }

  if(m_ComputeGrainShapes == true)
  {
	  updateProgressAndMessage(("Finding Grain Shapes"), 30);
	  FindShapes::Pointer find_shapes = FindShapes::New();
	  find_shapes->addObserver(static_cast<Observer*>(this));
	  find_shapes->setDataContainer(m.get());
	  find_shapes->execute();
	  err = find_shapes->getErrorCondition();
	  CHECK_FOR_ERROR(DataContainer, "Error Finding Grain Shapes", err);
	  CHECK_FOR_CANCELED(DataContainer, "MicrostructureStatistics was canceled",  find_shapes)

	  updateProgressAndMessage(("Determining Bounding Box"), 28);
	  FindBoundingBoxGrains::Pointer find_boundingboxgrains = FindBoundingBoxGrains::New();
	  find_boundingboxgrains->addObserver(static_cast<Observer*>(this));
	  find_boundingboxgrains->setDataContainer(m.get());
	  find_boundingboxgrains->execute();
	  err = find_boundingboxgrains->getErrorCondition();
	  CHECK_FOR_ERROR(DataContainer, "Error Determining the Bounding Box", err);
	  CHECK_FOR_CANCELED(DataContainer, "MicrostructureStatistics was canceled",  find_boundingboxgrains)
  }

  if(m_ComputeNumNeighbors == true)
  {
	  updateProgressAndMessage(("Finding Neighbors"), 45);
	  FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
	  find_neighbors->addObserver(static_cast<Observer*>(this));
	  find_neighbors->setDataContainer(m.get());
	  find_neighbors->execute();
	  setErrorCondition(find_neighbors->getErrorCondition());
	  CHECK_FOR_ERROR(DataContainer, "Error finding neighbors", err);
	  CHECK_FOR_CANCELED(DataContainer, "MicrostructureStatistics was canceled",  find_neighbors)

	  updateProgressAndMessage(("Finding Neighborhoods"), 55);
	  FindNeighborhoods::Pointer find_neighborhoods = FindNeighborhoods::New();
	  find_neighborhoods->addObserver(static_cast<Observer*>(this));
	  find_neighborhoods->setDataContainer(m.get());
	  find_neighborhoods->execute();
	  setErrorCondition(find_neighborhoods->getErrorCondition());
	  CHECK_FOR_ERROR(DataContainer, "Error Finding Neighborhoods", err);
	  CHECK_FOR_CANCELED(DataContainer, "MicrostructureStatistics was canceled",  find_neighborhoods)
  }

  if(m_WriteAverageOrientations == true || m_WriteH5StatsFile == true || m_WriteKernelMisorientations == true)
  {
	  updateProgressAndMessage(("Finding Average Orientations For Grains"), 60);
	  FindAvgOrientations::Pointer find_avgorientations = FindAvgOrientations::New();
	  find_avgorientations->addObserver(static_cast<Observer*>(this));
	  find_avgorientations->setDataContainer(m.get());
	  find_avgorientations->execute();
	  setErrorCondition(find_avgorientations->getErrorCondition());
	  CHECK_FOR_ERROR(DataContainer, "Error Finding Average Orientations For Grains", err);
	  CHECK_FOR_CANCELED(DataContainer, "MicrostructureStatistics was canceled",  find_avgorientations)
  }

  if(m_WriteH5StatsFile == true)
  {
	  // Create a new Writer for the Stats Data.

	  updateProgressAndMessage(("Finding Grain Axis ODF"), 60);
	  FindAxisODF::Pointer find_axisodf = FindAxisODF::New();
	  find_axisodf->setH5StatsFile(hdf5ResultsFile);
	  find_axisodf->addObserver(static_cast<Observer*>(this));
	  find_axisodf->setDataContainer(m.get());
	  find_axisodf->execute();
	  err = find_axisodf->getErrorCondition();
	  CHECK_FOR_ERROR(DataContainer, "Error Finding Grain Axis ODF", err);
	  CHECK_FOR_CANCELED(DataContainer, "MicrostructureStatistics was canceled",  find_axisodf)

	  updateProgressAndMessage(("Finding Euler ODF"), 70);
	  FindODF::Pointer find_odf = FindODF::New();
	  find_odf->setH5StatsFile(hdf5ResultsFile);
	  find_odf->addObserver(static_cast<Observer*>(this));
	  find_odf->setDataContainer(m.get());
	  find_odf->execute();
	  err = find_odf->getErrorCondition();
	  CHECK_FOR_ERROR(DataContainer, "Error Finding Euler ODF", err);
	  CHECK_FOR_CANCELED(DataContainer, "MicrostructureStatistics was canceled",  find_odf)

	  updateProgressAndMessage(("Finding MDF"), 75);
	  FindMDF::Pointer find_mdf = FindMDF::New();
	  find_mdf->setH5StatsFile(hdf5ResultsFile);
	  find_mdf->addObserver(static_cast<Observer*>(this));
	  find_mdf->setDataContainer(m.get());
	  find_mdf->execute();
	  err = find_mdf->getErrorCondition();
	  CHECK_FOR_ERROR(DataContainer, "Error Finding MDF", err);
	  CHECK_FOR_CANCELED(DataContainer, "MicrostructureStatistics was canceled",  find_mdf)

	  updateProgressAndMessage(("Writing Statistics"), 85);
	  WriteH5StatsFile::Pointer write_h5statsfile = WriteH5StatsFile::New();
	  write_h5statsfile->setBinStepSize(m_BinStepSize);
	  write_h5statsfile->setH5StatsFile(hdf5ResultsFile);
	  write_h5statsfile->addObserver(static_cast<Observer*>(this));
	  write_h5statsfile->setDataContainer(m.get());
	  write_h5statsfile->execute();
	  err = write_h5statsfile->getErrorCondition();
	  CHECK_FOR_ERROR(DataContainer, "Error Writing Statistics", err);
	  CHECK_FOR_CANCELED(DataContainer, "MicrostructureStatistics was canceled",  write_h5statsfile)
  }

  if(m_WriteKernelMisorientations == true)
  {
	  updateProgressAndMessage(("Finding Local Misorientation Gradients"), 90);
	  FindLocalMisorientationGradients::Pointer find_localmisorientationgradients = FindLocalMisorientationGradients::New();
	  find_localmisorientationgradients->addObserver(static_cast<Observer*>(this));
	  find_localmisorientationgradients->setDataContainer(m.get());
	  find_localmisorientationgradients->execute();
	  err = find_localmisorientationgradients->getErrorCondition();
	  CHECK_FOR_ERROR(DataContainer, "Error Finding Local Misorientation Gradients", err);
	  CHECK_FOR_CANCELED(DataContainer, "MicrostructureStatistics was canceled",  find_localmisorientationgradients)
  }

  updateProgressAndMessage(("Finding Grain Schmid Factors"), 80);
  FindSchmids::Pointer find_schmids = FindSchmids::New();
  find_schmids->addObserver(static_cast<Observer*>(this));
  find_schmids->setDataContainer(m.get());
  find_schmids->execute();
  err = find_schmids->getErrorCondition();
  CHECK_FOR_ERROR(DataContainer, "Error Finding Grain Schmid Factors", err);
  CHECK_FOR_CANCELED(DataContainer, "MicrostructureStatistics was canceled",  find_schmids)

  updateProgressAndMessage(("Finding Euclidean Distance Maps"), 90);
  FindEuclideanDistMap::Pointer find_euclideandistmap = FindEuclideanDistMap::New();
  find_euclideandistmap->addObserver(static_cast<Observer*>(this));
  find_euclideandistmap->setDataContainer(m.get());
  find_euclideandistmap->execute();
  err = find_euclideandistmap->getErrorCondition();
  CHECK_FOR_ERROR(DataContainer, "Error Finding Euclidean Distance Map", err);
  CHECK_FOR_CANCELED(DataContainer, "MicrostructureStatistics was canceled",  find_euclideanmap)

  updateProgressAndMessage(("Finding Deformation Statistics"), 80);
  FindDeformationStatistics::Pointer find_deformationstatistics = FindDeformationStatistics::New();
  find_deformationstatistics->setOutputFile1(reconDeformStatsFile);
  find_deformationstatistics->setOutputFile2(reconDeformIPFFile);
  find_deformationstatistics->addObserver(static_cast<Observer*>(this));
  find_deformationstatistics->setDataContainer(m.get());
  find_deformationstatistics->execute();
  err = find_deformationstatistics->getErrorCondition();
  CHECK_FOR_ERROR(DataContainer, "Error Finding Deformation Statistics", err);
  CHECK_FOR_CANCELED(DataContainer, "MicrostructureStatistics was canceled",  find_deformationstatistics)

  MAKE_OUTPUT_FILE_PATH ( FieldDataFile, DREAM3D::MicroStats::GrainDataFile);

  if(m_WriteGrainFile == true)
  {
    updateProgressAndMessage(("Writing Field Data"), 90);
	WriteFieldData::Pointer write_fielddata = WriteFieldData::New();
	write_fielddata->setFieldDataFile(FieldDataFile);
    write_fielddata->addObserver(static_cast<Observer*>(this));
    write_fielddata->setDataContainer(m.get());
    write_fielddata->execute();
    err = write_fielddata->getErrorCondition();
    CHECK_FOR_ERROR(DataContainer, "Error Writing Field Data", err)
    CHECK_FOR_CANCELED(DataContainer, "GrainGenerator Was canceled", write_fielddata)
  }

  /* ********** This section writes the VTK files for visualization *** */
  if (m_WriteVtkFile) {
    updateProgressAndMessage(("Writing VTK Visualization File"), 98);
    MAKE_OUTPUT_FILE_PATH ( vtkVizFile, DREAM3D::Reconstruction::VisualizationVizFile);

    // Setup all the classes that will help us write the Scalars to the VTK File
    std::vector<VtkScalarWriter*> scalarsToWrite;
    {
      VtkScalarWriter* w0 =
      static_cast<VtkScalarWriter*>(new VoxelGrainIdScalarWriter<DataContainer>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if (m_WriteSurfaceVoxel == true) {
      VtkScalarWriter* w0 =
        static_cast<VtkScalarWriter*>(new VoxelSurfaceVoxelScalarWriter<DataContainer>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if (m_WritePhaseId == true){
      VtkScalarWriter* w0 =
        static_cast<VtkScalarWriter*>(new VoxelPhaseIdScalarWriter<DataContainer>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if (m_WriteKernelMisorientations == true){
      VtkScalarWriter* w0 =
        static_cast<VtkScalarWriter*>(new VoxelKAMScalarWriter<DataContainer>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if (m_WriteIPFColor == true) {
      VtkScalarWriter* w0 =
        static_cast<VtkScalarWriter*>(new VoxelIPFColorScalarWriter<DataContainer>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    // Create our File Output Writer Object. This will handle all the File Output duties
    VTKRectilinearGridFileWriter vtkWriter;
    vtkWriter.setWriteBinaryFiles(m_WriteBinaryVTKFiles);
    err = vtkWriter.write<DataContainer>(vtkVizFile, m.get(), scalarsToWrite);
    // Now Delete all the Scalar Helpers that we just created and used.
    for (std::vector<VtkScalarWriter*>::iterator iter = scalarsToWrite.begin(); iter != scalarsToWrite.end(); ++iter )
    {
      delete (*iter);
    }

    CHECK_FOR_ERROR(DataContainer, "The MicrostructureStatisticsFunc threw an Error writing the VTK file format.", err);
  }

  // Clean up all the memory by forcibly setting a NULL pointer to the Shared
  // pointer object.
  m = DataContainer::NullPointer();  // Clean up the memory
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
