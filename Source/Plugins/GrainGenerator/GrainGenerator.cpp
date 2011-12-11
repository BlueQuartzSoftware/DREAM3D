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

#include "GrainGenerator.h"

#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/MXAFileInfo.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/VTKUtils/VTKFileWriters.hpp"
#include "DREAM3DLib/HDF5/H5GrainWriter.hpp"
#include "DREAM3DLib/HDF5/H5VoxelWriter.h"
#include "DREAM3DLib/HDF5/H5VoxelReader.h"

#include "GrainGenerator/StructureReaders/AbstractStructureReader.h"
#include "GrainGenerator/StructureReaders/VTKStructureReader.h"
#include "DREAM3DLib/GenericFilters/FindNeighbors.h"
#include "DREAM3DLib/SyntheticBuilderFilters/MatchCrystallography.h"
#include "DREAM3DLib/SyntheticBuilderFilters/PlacePrecipitates.h"
#include "DREAM3DLib/SyntheticBuilderFilters/PackGrainsGen2.h"
#include "DREAM3DLib/SyntheticBuilderFilters/AdjustVolume.h"
#include "DREAM3DLib/GenericFilters/WriteFieldData.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGenerator::GrainGenerator() :
    m_H5StatsFile(""), m_OutputDirectory("."), m_OutputFilePrefix("GrainGenerator_"), m_XPoints(0), m_YPoints(0), m_ZPoints(0), m_XResolution(0.0), m_YResolution(0.0), m_ZResolution(0.0), m_NeighborhoodErrorWeight(0.0), m_PeriodicBoundary(true), m_AlreadyFormed(false), m_Precipitates(0), m_WriteBinaryVTKFiles(true), m_WriteVtkFile(false), m_WriteIPFColor(false), m_WriteHDF5GrainFile(false)
{
//  std::cout << "GrainGenerator Constructor" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGenerator::~GrainGenerator()
{
//  std::cout << "GrainGenerator::~GrainGenerator()" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGenerator::execute()
{
  int err = 0;
  // Instantiate our GrainGeneratorFunc object
  m = DataContainer::New();
  // Initialize some benchmark timers
  START_CLOCK()

  if(m_AlreadyFormed == false)
  {
    m->xpoints = m_XPoints;
    m->ypoints = m_YPoints;
    m->zpoints = m_ZPoints;
    m->shapeTypes = m_ShapeTypes;
    m->resx = m_XResolution;
    m->resy = m_YResolution;
    m->resz = m_ZResolution;

    updateProgressAndMessage(("Packing Grains"), 25);
    PackGrainsGen2::Pointer pack_grains = PackGrainsGen2::New();
    pack_grains->setH5StatsFile(getH5StatsFile());
    pack_grains->setperiodic_boundaries(m_PeriodicBoundary);
    pack_grains->setneighborhooderrorweight(m_NeighborhoodErrorWeight);
    pack_grains->addObserver(static_cast<Observer*>(this));
    pack_grains->setDataContainer(m.get());
    setCurrentFilter(pack_grains);
    pack_grains->execute();
    err = pack_grains->getErrorCondition();
    CHECK_FOR_ERROR(DataContainer, "Error Packing Grains", err)CHECK_FOR_CANCELED(DataContainer, "GrainGenerator Was canceled", pack_grains)

    updateProgressAndMessage(("Adjusting Grains"), 25);
    AdjustVolume::Pointer adjust_grains = AdjustVolume::New();
    adjust_grains->addObserver(static_cast<Observer*>(this));
    adjust_grains->setDataContainer(m.get());
    setCurrentFilter(adjust_grains);
    adjust_grains->execute();
    err = adjust_grains->getErrorCondition();
    CHECK_FOR_ERROR(DataContainer, "Error Adjusting Grains", err)CHECK_FOR_CANCELED(DataContainer, "GrainGenerator Was canceled", adjust_grains)
  }
  else if(m_AlreadyFormed == true)
  {
    updateProgressAndMessage(("Reading Structure"), 40);
    std::string ext = MXAFileInfo::extension(m_StructureFile);
    if(ext.compare("vtk") == 0)
    {
      VTKStructureReader::Pointer reader = VTKStructureReader::New();
      reader->setInputFileName(m_StructureFile);
      reader->setGrainIdScalarName(DREAM3D::VTK::GrainIdScalarName);
      reader->setPhaseIdScalarName(DREAM3D::VTK::PhaseIdScalarName);
      err = reader->readStructure(m.get());
      CHECK_FOR_ERROR(DataContainer, "GrainGenerator Error getting size and resolution from VTK Voxel File", err);
    }
    else if(ext.compare("h5") == 0 || ext.compare("h5voxel") == 0)
    {
      // Load up the voxel data
      H5VoxelReader::Pointer h5Reader = H5VoxelReader::New();
      h5Reader->setFileName(m_StructureFile);
      int dims[3];
      float spacing[3];
      float origin[3];
      err = h5Reader->getSizeResolutionOrigin(dims, spacing, origin);

      CHECK_FOR_ERROR(DataContainer, "GrainGenerator Error getting size and resolution from HDF5 Voxel File", err);

      m->xpoints = dims[0];
      m->ypoints = dims[1];
      m->zpoints = dims[2];
      m->totalpoints = dims[0] * dims[1] * dims[2];
      m->resx = spacing[0];
      m->resy = spacing[1];
      m->resz = spacing[2];

      updateProgressAndMessage(("Reading the Voxel Data from the HDF5 File"), 10);
      err = h5Reader->readVoxelData(m->m_GrainIndicies, m->m_Phases, m->m_Euler1s, m->m_Euler2s, m->m_Euler3s, m->crystruct, m->phaseType, m->totalpoints);
      CHECK_FOR_ERROR(DataContainer, "GrainGenerator Error reading voxel data from HDF5 Voxel File", err);
    }
    else
    {
      err = -1;
      CHECK_FOR_ERROR(DataContainer, "GrainGenerator Error No suitable Voxel Structure Reader found", err);
    }
  }

  if(m_AlreadyFormed == false)
  {
    updateProgressAndMessage(("Placing Precipitates"), 50);
    PlacePrecipitates::Pointer place_precipitates = PlacePrecipitates::New();
    place_precipitates->setH5StatsFile(getH5StatsFile());
    place_precipitates->setperiodic_boundaries(m_PeriodicBoundary);
    place_precipitates->addObserver(static_cast<Observer*>(this));
    place_precipitates->setDataContainer(m.get());
    setCurrentFilter(place_precipitates);
    place_precipitates->execute();
    err = place_precipitates->getErrorCondition();
    CHECK_FOR_ERROR(DataContainer, "Error Placing Preciptates", err)CHECK_FOR_CANCELED(DataContainer, "GrainGenerator Was canceled", place_precipitates)
  }

  updateProgressAndMessage(("Matching Crystallography"), 65);
  MatchCrystallography::Pointer match_crystallography = MatchCrystallography::New();
  match_crystallography->setH5StatsFile(getH5StatsFile());
  match_crystallography->addObserver(static_cast<Observer*>(this));
  match_crystallography->setDataContainer(m.get());
  setCurrentFilter(match_crystallography);
  match_crystallography->execute();
  err = match_crystallography->getErrorCondition();
  CHECK_FOR_ERROR(DataContainer, "Error Matching Crystallography", err)CHECK_FOR_CANCELED(DataContainer, "GrainGenerator Was canceled", match_crystallography)

  MAKE_OUTPUT_FILE_PATH( FieldDataFile, DREAM3D::SyntheticBuilder::GrainDataFile)

  updateProgressAndMessage(("Writing Field Data"), 90);
  WriteFieldData::Pointer write_fielddata = WriteFieldData::New();
  write_fielddata->setFieldDataFile(FieldDataFile);
  write_fielddata->addObserver(static_cast<Observer*>(this));
  write_fielddata->setDataContainer(m.get());
  setCurrentFilter(write_fielddata);
  write_fielddata->execute();
  err = write_fielddata->getErrorCondition();
  CHECK_FOR_ERROR(DataContainer, "Error Writing Field Data", err)CHECK_FOR_CANCELED(DataContainer, "GrainGenerator Was canceled", write_fielddata)

  /** ********** This section writes the Voxel Data for the Stats Module ****/
  // Create a new HDF5 Volume file by overwriting any HDF5 file that may be in the way
  MAKE_OUTPUT_FILE_PATH( h5VoxelFile, DREAM3D::SyntheticBuilder::H5VoxelFile)
  H5VoxelWriter::Pointer h5VoxelWriter = H5VoxelWriter::New();
  if(h5VoxelWriter.get() == NULL)
  {
    CHECK_FOR_ERROR(DataContainer, "The HDF5 Voxel file could not be created. Does the path exist and do you have write access to the output directory.", -1);
    m = DataContainer::NullPointer(); // Clean up the memory
    return;
  }
  h5VoxelWriter->setFileName(h5VoxelFile);
  updateProgressAndMessage(("Writing HDF5 Voxel Data File"), 83);
  err = h5VoxelWriter->writeData<DataContainer>(m.get());
  CHECK_FOR_ERROR(DataContainer, "The HDF5 Voxel file could not be written to. Does the path exist and do you have write access to the output directory.", err);

  /* ********** This section writes the VTK files for visualization *** */
  if(m_WriteVtkFile)
  {
    updateProgressAndMessage(("Writing VTK Visualization File"), 93);
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
    }CHECK_FOR_ERROR(DataContainer, "The Grain Generator threw an Error writing the VTK file format.", err);

  }

  CHECK_FOR_CANCELED(DataContainer, "GrainGenerator Was canceled", writeVisualizationFile)
  /* ******* End VTK Visualization File Writing Section ****** */

  /* ********** HDF5 Grains File  ********** */
  if(m_WriteHDF5GrainFile)
  {
    updateProgressAndMessage(("Writing Out HDF5 Grain File. This may take a few minutes to complete."), 96);
    MAKE_OUTPUT_FILE_PATH( hdf5GrainFile, DREAM3D::Reconstruction::HDF5GrainFile);
    H5GrainWriter::Pointer h5GrainWriter = H5GrainWriter::New();
    err = h5GrainWriter->writeHDF5GrainsFile<DataContainer>(m.get(), hdf5GrainFile);
    CHECK_FOR_ERROR(DataContainer, "The Grain Generator threw an Error writing the HDF5 Grain file format.", err);
  }

  CHECK_FOR_CANCELED(DataContainer, "GrainGenerator Was canceled", writeHDF5GrainsFile)

  // Clean up all the memory
  updateProgressAndMessage(("Cleaning up Memory."), 98);
  m = DataContainer::NullPointer();

  updateProgressAndMessage(("Generation Completed"), 100);
}

