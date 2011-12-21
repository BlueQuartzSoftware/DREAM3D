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

#include "Reconstruction.h"

#include "MXA/MXA.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/EbsdColoring.hpp"
#include "DREAM3DLib/VTKUtils/VTKFileWriters.hpp"
#include "DREAM3DLib/HDF5/H5VoxelWriter.h"
#include "DREAM3DLib/HDF5/H5GrainWriter.hpp"

#include "DREAM3DLib/ReconstructionFilters/LoadSlices.h"
#include "DREAM3DLib/ReconstructionFilters/AlignSections.h"
#include "DREAM3DLib/ReconstructionFilters/SegmentGrains.h"
#include "DREAM3DLib/ReconstructionFilters/CleanupGrains.h"
#include "DREAM3DLib/ReconstructionFilters/MergeTwins.h"
#include "DREAM3DLib/ReconstructionFilters/MergeColonies.h"

#define MIKE_G_DEBUG 0

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Reconstruction::Reconstruction() :
    m_H5AngFile(""), m_OutputDirectory("."), m_OutputFilePrefix("Reconstruction_"), m_MergeTwins(false), m_MergeColonies(false), m_MinAllowedGrainSize(0), m_MisorientationTolerance(0.0), m_WriteBinaryVTKFiles(true), m_WriteVtkFile(true), m_WritePhaseId(true),
//m_WriteImageQuality(true),
    m_WriteIPFColor(true), m_WriteHDF5GrainFile(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Reconstruction::~Reconstruction()
{
}

#define MAKE_OUTPUT_FILE_PATH(outpath, filename)\
    std::string outpath = m_OutputDirectory + MXADir::Separator + m_OutputFilePrefix + filename;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Reconstruction::execute()
{
  int err = -1;

  // Create the DataContainer object
  m = DataContainer::New();
  m->addObserver(static_cast<Observer*>(this));

  m_OutputDirectory = MXADir::toNativeSeparators(m_OutputDirectory);

  // Create a Vector to hold all the filters. Later on we will execute all the filters
  std::vector<AbstractFilter::Pointer> pipeline;

  updateProgressAndMessage(("Loading Slices"), 10);
  LoadSlices::Pointer load_slices = LoadSlices::New();
  load_slices->setH5AngFile(getH5AngFile());
  load_slices->setZStartIndex(getZStartIndex());
  load_slices->setZEndIndex(getZEndIndex());
  load_slices->setPhaseTypes(getPhaseTypes());
  load_slices->setQualityMetricFilters(getQualityMetricFilters());
  load_slices->setRefFrameZDir(getRefFrameZDir());
  load_slices->setMisoTolerance(m_MisorientationTolerance);
  pipeline.push_back(load_slices);

  AlignSections::Pointer align_sections = AlignSections::New();
  align_sections->setmisorientationtolerance(m_MisorientationTolerance);
  align_sections->setalignmeth(m_AlignmentMethod);
  pipeline.push_back(align_sections);

  SegmentGrains::Pointer segment_grains = SegmentGrains::New();
  segment_grains->setMisoTolerance(m_MisorientationTolerance);
  pipeline.push_back(segment_grains);

  CleanupGrains::Pointer cleanup_grains = CleanupGrains::New();
  cleanup_grains->setminallowedgrainsize(m_MinAllowedGrainSize);
  cleanup_grains->setmisorientationtolerance(m_MisorientationTolerance);
  pipeline.push_back(cleanup_grains);


  if(m_MergeTwins == true)
  {
    MergeTwins::Pointer merge_twins = MergeTwins::New();
    pipeline.push_back(merge_twins);
  }

  if(m_MergeColonies == true)
  {
    MergeColonies::Pointer merge_colonies = MergeColonies::New();
    pipeline.push_back(merge_colonies);
  }

  // Start a Benchmark Clock so we can keep track of each filter's execution time
  START_CLOCK()
  // Start looping through the Pipeline
  float progress = 0.0f;
  std::stringstream ss;
  for (std::vector<AbstractFilter::Pointer>::iterator filter = pipeline.begin(); filter != pipeline.end(); ++filter)
  {
    progress = progress + 1.0f;
    pipelineProgress(progress / (pipeline.size() + 1) * 100.0f);
    ss.str("");
    ss << "Executing Filter [" << progress << "/" << pipeline.size() << "] - " << (*filter)->getNameOfClass();
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
    CHECK_FOR_CANCELED(DataContainer, "Reconstruction was canceled", write_fielddata)

    if(DREAM3D_BENCHMARKS)
    {
      std::cout << (*filter)->getNameOfClass() << " Finish Time(ms): " << (MXA::getMilliSeconds() - millis) << std::endl;
      millis = MXA::getMilliSeconds();
    }
  }

  /** ********** This section writes the Voxel Data for the Stats Module *** */
  // Create a new HDF5 Volume file by overwriting any HDF5 file that may be in the way
  MAKE_OUTPUT_FILE_PATH( hdf5VolumeFile, DREAM3D::Reconstruction::H5VoxelFile)
  H5VoxelWriter::Pointer h5VolWriter = H5VoxelWriter::New();
  if(h5VolWriter.get() == NULL)
  {
    CHECK_FOR_ERROR(DataContainer, "The HDF5 Voxel file could not be created. Does the path exist and do you have write access to the output directory.", -1)
  }
  h5VolWriter->setFileName(hdf5VolumeFile);
  updateProgressAndMessage(("Writing HDF5 Voxel Data File"), 83);
  err = h5VolWriter->writeData<DataContainer>(m.get());
  CHECK_FOR_ERROR(DataContainer, "The HDF5 Voxel file could not be written to. Does the path exist and do you have write access to the output directory.", err);

  /** ********** This section writes the VTK files for visualization *** */
  updateProgressAndMessage(("Writing VTK Visualization File"), 93);
  if(m_WriteVtkFile)
  {
    MAKE_OUTPUT_FILE_PATH( reconVisFile, DREAM3D::Reconstruction::VisualizationVizFile);

    // Setup all the classes that will help us write the Scalars to the VTK File
    std::vector<VtkScalarWriter*> scalarsToWrite;
    {
      VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelGrainIdScalarWriter<DataContainer>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if(m_WritePhaseId == true)
    {
      VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelPhaseIdScalarWriter<DataContainer>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if (m_WriteGoodVoxels == true)
    {
      VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelGoodVoxelScalarWriter<DataContainer>(m.get()));
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
    err = vtkWriter.write<DataContainer>(reconVisFile, m.get(), scalarsToWrite);
    // Now Delete all the Scalar Helpers that we just created and used.
    for (std::vector<VtkScalarWriter*>::iterator iter = scalarsToWrite.begin(); iter != scalarsToWrite.end(); ++iter)
    {
      delete (*iter);
    }
    CHECK_FOR_ERROR(DataContainer, vtkWriter.getErrorMessage(), err);
  }
  /* ******* End VTK Visualization File Writing Section ****** */

  CHECK_FOR_CANCELED(DataContainer, "Reconstruction was canceled", vtk_viz_files)

  /* ********** Optionally write the .h5grain file */
  if(m_WriteHDF5GrainFile)
  {
    updateProgressAndMessage(("Writing Out HDF5 Grain File. This may take a few minutes to complete."), 99);
    MAKE_OUTPUT_FILE_PATH( hdf5GrainFile, DREAM3D::Reconstruction::HDF5GrainFile);
    H5GrainWriter::Pointer h5GrainWriter = H5GrainWriter::New();
    err = h5GrainWriter->writeHDF5GrainsFile<DataContainer>(m.get(), hdf5GrainFile);
    CHECK_FOR_ERROR(DataContainer, "The HDF5 Grain file could not be written to. Does the path exist and do you have write access to the output directory.", err);
  }

  updateProgressAndMessage(("Reconstruction Complete"), 100);

  // Clean up all the memory by forcibly setting a NULL pointer to the Shared
  // pointer object.
  m = DataContainer::NullPointer(); // Clean up the memory
}

#define PRINT_PROPERTY( out, var)\
  out << #var << ": " << m_##var << std::endl;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Reconstruction::printSettings(std::ostream &ostream)
{
  ostream << "Reconstruction Settings Being Used" << std::endl;
  PRINT_PROPERTY(ostream, H5AngFile)PRINT_PROPERTY(ostream, ZStartIndex)PRINT_PROPERTY(ostream, ZEndIndex)PRINT_PROPERTY(ostream, OutputDirectory)PRINT_PROPERTY(ostream, MergeTwins)PRINT_PROPERTY(ostream, MergeColonies)PRINT_PROPERTY(ostream, MinAllowedGrainSize)PRINT_PROPERTY(ostream, MisorientationTolerance)PRINT_PROPERTY(ostream, AlignmentMethod)

  PRINT_PROPERTY(ostream, WriteVtkFile)PRINT_PROPERTY(ostream, WritePhaseId)PRINT_PROPERTY(ostream, WriteIPFColor)
  PRINT_PROPERTY(ostream, WriteHDF5GrainFile)
}
