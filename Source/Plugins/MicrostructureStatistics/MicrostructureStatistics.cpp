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

#include <vector>

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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatistics::execute()
{
  int err = -1;

  m = DataContainer::New();

  MAKE_OUTPUT_FILE_PATH( reconDeformStatsFile, DREAM3D::MicroStats::DeformationStatsFile);
  MAKE_OUTPUT_FILE_PATH( reconDeformIPFFile, DREAM3D::MicroStats::IPFDeformVTKFile);
  MAKE_OUTPUT_FILE_PATH( reconVisFile, DREAM3D::Reconstruction::VisualizationVizFile);
  MAKE_OUTPUT_FILE_PATH( hdf5ResultsFile, DREAM3D::MicroStats::H5StatisticsFile)

  // Create a Vector to hold all the filters. Later on we will execute all the filters
  std::vector<AbstractFilter::Pointer> pipeline;

  LoadVolume::Pointer load_volume = LoadVolume::New();
  load_volume->setInputFile(m_InputFile);
  pipeline.push_back(load_volume);

  FindSurfaceGrains::Pointer find_surfacegrains = FindSurfaceGrains::New();
  pipeline.push_back(find_surfacegrains);

  // Start Computing the statistics
  if(m_ComputeGrainSize == true)
  {
    FindSizes::Pointer find_sizes = FindSizes::New();
    pipeline.push_back(find_sizes);
  }

  if(m_ComputeGrainShapes == true)
  {
    FindShapes::Pointer find_shapes = FindShapes::New();
    pipeline.push_back(find_shapes);

    updateProgressAndMessage(("Determining Bounding Box"), 28);
    FindBoundingBoxGrains::Pointer find_boundingboxgrains = FindBoundingBoxGrains::New();
    pipeline.push_back(find_boundingboxgrains);
  }

  if(m_ComputeNumNeighbors == true)
  {
    FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
    pipeline.push_back(find_neighbors);

    FindNeighborhoods::Pointer find_neighborhoods = FindNeighborhoods::New();
    pipeline.push_back(find_neighborhoods);
  }

  if(m_WriteAverageOrientations == true || m_WriteH5StatsFile == true || m_WriteKernelMisorientations == true)
  {
    FindAvgOrientations::Pointer find_avgorientations = FindAvgOrientations::New();
    pipeline.push_back(find_avgorientations);
  }

  if(m_WriteH5StatsFile == true)
  {
    // Create a new Writer for the Stats Data.
    FindAxisODF::Pointer find_axisodf = FindAxisODF::New();
    find_axisodf->setH5StatsFile(hdf5ResultsFile);
    pipeline.push_back(find_axisodf);

    FindODF::Pointer find_odf = FindODF::New();
    find_odf->setH5StatsFile(hdf5ResultsFile);
    find_odf->addObserver(static_cast<Observer*>(this));
    find_odf->setDataContainer(m.get());
    pipeline.push_back(find_odf);

    FindMDF::Pointer find_mdf = FindMDF::New();
    find_mdf->setH5StatsFile(hdf5ResultsFile);
    pipeline.push_back(find_mdf);

    WriteH5StatsFile::Pointer write_h5statsfile = WriteH5StatsFile::New();
    write_h5statsfile->setBinStepSize(m_BinStepSize);
    write_h5statsfile->setH5StatsFile(hdf5ResultsFile);
    pipeline.push_back(write_h5statsfile);
  }

  if(m_WriteKernelMisorientations == true)
  {
    FindLocalMisorientationGradients::Pointer find_localmisorientationgradients = FindLocalMisorientationGradients::New();
    pipeline.push_back(find_localmisorientationgradients);
  }


  FindSchmids::Pointer find_schmids = FindSchmids::New();
  pipeline.push_back(find_schmids);

  FindEuclideanDistMap::Pointer find_euclideandistmap = FindEuclideanDistMap::New();
  pipeline.push_back(find_euclideandistmap);

  FindDeformationStatistics::Pointer find_deformationstatistics = FindDeformationStatistics::New();
  find_deformationstatistics->setOutputFile1(reconDeformStatsFile);
  find_deformationstatistics->setOutputFile2(reconDeformIPFFile);
  pipeline.push_back(find_deformationstatistics);



  if(m_WriteGrainFile == true)
  {
    MAKE_OUTPUT_FILE_PATH( FieldDataFile, DREAM3D::MicroStats::GrainDataFile);
    WriteFieldData::Pointer write_fielddata = WriteFieldData::New();
    write_fielddata->setFieldDataFile(FieldDataFile);
    pipeline.push_back(find_deformationstatistics);
  }


  // Start a Benchmark Clock so we can keep track of each filter's execution time
  START_CLOCK()
  // Start looping through the Pipeline
  float progress = 0.0f;
  std::stringstream ss;
  for (std::vector<AbstractFilter::Pointer>::iterator filter = pipeline.begin(); filter != pipeline.end(); ++filter )
  {
    progress = progress + 1.0f;
    pipelineProgress( progress/(pipeline.size() + 1) * 100.0f);
    ss.str("");
    ss << "Executing Filter [" << progress << "/" << pipeline.size() << "] - " << (*filter)->getNameOfClass() ;
    pipelineProgressMessage(ss.str());
    (*filter)->addObserver(static_cast<Observer*>(this));
    (*filter)->setDataContainer(m.get());
    setCurrentFilter(*filter);
    (*filter)->execute();
    (*filter)->removeObserver(static_cast<Observer*>(this));
    err = (*filter)->getErrorCondition();
    if(err < 0)
    {
      setErrorCondition(err);
      pipelineErrorMessage((*filter)->getErrorMessage().c_str());
      pipelineProgress(100);
      pipelineFinished();
      return;
    }
    CHECK_FOR_CANCELED(DataContainer, "MicrostructureStatistics was canceled", write_fielddata)

    if(AIM_RECONSTRUCTION_BENCHMARKS)
    {
      std::cout << (*filter)->getNameOfClass() << " Finish Time(ms): " << (MXA::getMilliSeconds() - millis) << std::endl;
      millis = MXA::getMilliSeconds();
    }
  }





  /* ********** This section writes the VTK files for visualization *** */
  if(m_WriteVtkFile)
  {
    updateProgressAndMessage(("Writing VTK Visualization File"), 98);
    MAKE_OUTPUT_FILE_PATH( vtkVizFile, DREAM3D::Reconstruction::VisualizationVizFile);

    // Setup all the classes that will help us write the Scalars to the VTK File
    std::vector<VtkScalarWriter*> scalarsToWrite;
    {
      VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelGrainIdScalarWriter<DataContainer>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if(m_WriteSurfaceVoxel == true)
    {
      VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelSurfaceVoxelScalarWriter<DataContainer>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if(m_WritePhaseId == true)
    {
      VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelPhaseIdScalarWriter<DataContainer>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if(m_WriteKernelMisorientations == true)
    {
      VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelKAMScalarWriter<DataContainer>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if(m_WriteIPFColor == true)
    {
      VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelIPFColorScalarWriter<DataContainer>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    // Create our File Output Writer Object. This will handle all the File Output duties
    VTKRectilinearGridFileWriter vtkWriter;
    vtkWriter.setWriteBinaryFiles(m_WriteBinaryVTKFiles);
    err = vtkWriter.write<DataContainer>(vtkVizFile, m.get(), scalarsToWrite);
    // Now Delete all the Scalar Helpers that we just created and used.
    for (std::vector<VtkScalarWriter*>::iterator iter = scalarsToWrite.begin(); iter != scalarsToWrite.end(); ++iter)
    {
      delete (*iter);
    }

    CHECK_FOR_ERROR(DataContainer, "The MicrostructureStatisticsFunc threw an Error writing the VTK file format.", err);
  }

  // Clean up all the memory by forcibly setting a NULL pointer to the Shared
  // pointer object.
  m = DataContainer::NullPointer(); // Clean up the memory
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
  PRINT_PROPERTY(ostream, InputFile)PRINT_PROPERTY(ostream, OutputDirectory)PRINT_PROPERTY(ostream, OutputFilePrefix)PRINT_PROPERTY(ostream, BinStepSize)PRINT_PROPERTY(ostream, WriteH5StatsFile)PRINT_PROPERTY(ostream, ComputeGrainSize)PRINT_PROPERTY(ostream, ComputeGrainShapes)PRINT_PROPERTY(ostream, ComputeNumNeighbors)PRINT_PROPERTY(ostream, ComputeMDF)PRINT_PROPERTY(ostream, WriteGrainFile)PRINT_PROPERTY(ostream, WriteGrainSize)PRINT_PROPERTY(ostream, WriteGrainShapes)PRINT_PROPERTY(ostream, WriteNumNeighbors)PRINT_PROPERTY(ostream, WriteAverageOrientations)

  PRINT_PROPERTY(ostream, WriteBinaryVTKFiles)PRINT_PROPERTY(ostream, WriteVtkFile)PRINT_PROPERTY(ostream, WriteSurfaceVoxel)PRINT_PROPERTY(ostream, WritePhaseId)PRINT_PROPERTY(ostream, WriteIPFColor)

  ostream << " ###################################### " << std::endl;

}
