

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

#include "GrainGenerator.h"

#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/MXAFileInfo.h"

#include "DREAM3D/Common/PhWriter.hpp"
#include "DREAM3D/Common/VTKUtils/VTKFileWriters.hpp"
#include "DREAM3D/StructureReaders/AbstractStructureReader.h"
#include "DREAM3D/StructureReaders/VTKStructureReader.h"
#include "DREAM3D/StructureReaders/DXStructureReader.h"
#include "DREAM3D/HDF5/H5GrainWriter.h"
#include "DREAM3D/HDF5/H5VoxelWriter.h"
#include "DREAM3D/HDF5/H5VoxelReader.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGenerator::GrainGenerator() :
m_H5StatsFile(""),
m_OutputDirectory("."),
m_OutputFilePrefix("GrainGenerator_"),
m_XPoints(0),
m_YPoints(0),
m_ZPoints(0),
m_XResolution(0.0),
m_YResolution(0.0),
m_ZResolution(0.0),
m_FillingErrorWeight(0.0),
m_NeighborhoodErrorWeight(0.0),
m_SizeDistErrorWeight(0.0),
m_PeriodicBoundary(true),
m_WriteGrainData(true),
m_AlreadyFormed(false),
m_Precipitates(0),
m_WriteBinaryVTKFiles(true),
m_WriteVtkFile(false),
m_WriteIPFColor(false),
m_WritePhFile(false),
m_WriteHDF5GrainFile(false)
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
  m = GrainGeneratorFunc::New();
  // Initialize some benchmark timers
  START_CLOCK()

  // Open the HDF5 Stats file
  H5ReconStatsReader::Pointer h5reader = H5ReconStatsReader::New(m_H5StatsFile);
  if (h5reader.get() == NULL)
  {
    CHECK_FOR_ERROR(GrainGeneratorFunc, "GrainGenerator Error Opening the HDF5 Input file", -1)
    return;
  }

  if (m_AlreadyFormed == false)
  {
    m->periodic_boundaries = m_PeriodicBoundary;
    m->xpoints = m_XPoints;
    m->ypoints = m_YPoints;
    m->zpoints = m_ZPoints;
    m->shapeTypes = m_ShapeTypes;
    m->resx = m_XResolution;
    m->resy = m_YResolution;
    m->resz = m_ZResolution;
    m->fillingerrorweight = m_FillingErrorWeight;
    m->neighborhooderrorweight = m_NeighborhoodErrorWeight;
    m->sizedisterrorweight = m_SizeDistErrorWeight;

    updateProgressAndMessage(("Loading Stats Data"), 5);
    err = m->readReconStatsData(h5reader);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "readReconStatsData Was canceled", readReconStatsData)

    updateProgressAndMessage(("Loading Axis Orientation Data"), 10);
    err = m->readAxisOrientationData(h5reader);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "readAxisOrientationData Was canceled", readAxisOrientationData);

    updateProgressAndMessage(("Packing Grains"), 25);
    m->pack_grains();
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", pack_grains)

    updateProgressAndMessage(("Assigning Voxels"), 30);
    m->assign_voxels();
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", assign_voxels)

    updateProgressAndMessage(("Filling Gaps"), 40);
    m->fill_gaps();
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", fill_gaps)

//    updateProgressAndMessage(("Adjusting Boundaries"), 42);
//    m->adjust_boundaries();
//    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", adjust_boundaries)
  }
  else if (m_AlreadyFormed == true)
  {
    updateProgressAndMessage(("Loading Stats Data"), 10);
    err = m->readReconStatsData(h5reader);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "readReconStatsData Was canceled", readReconStatsData)

    updateProgressAndMessage(("Loading Axis Orientation Data"), 25);
    err = m->readAxisOrientationData(h5reader);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "readAxisOrientationData Was canceled", readAxisOrientationData);

    updateProgressAndMessage(("Reading Structure"), 40);
    std::string ext = MXAFileInfo::extension(m_StructureFile);
    if (ext.compare("vtk") == 0)
    {
      VTKStructureReader::Pointer reader = VTKStructureReader::New();
      reader->setInputFileName(m_StructureFile);
      reader->setGrainIdScalarName(AIM::VTK::GrainIdScalarName);
      reader->setPhaseIdScalarName(AIM::VTK::PhaseIdScalarName);
      err = reader->readStructure(m.get());
      CHECK_FOR_ERROR(GrainGeneratorFunc, "GrainGenerator Error getting size and resolution from VTK Voxel File", err);
    }
    else if (ext.compare("h5") == 0)
    {
      // Load up the voxel data
      H5VoxelReader::Pointer h5Reader = H5VoxelReader::New();
      h5Reader->setFilename(m_StructureFile);
      int dims[3];
      float spacing[3];
      err = h5Reader->getSizeAndResolution(dims, spacing);

      CHECK_FOR_ERROR(GrainGeneratorFunc, "GrainGenerator Error getting size and resolution from HDF5 Voxel File", err);

      m->xpoints = dims[0];
      m->ypoints = dims[1];
      m->zpoints = dims[2];
      m->totalpoints = dims[0] * dims[1] * dims[2];
      m->resx = spacing[0];
      m->resy = spacing[1];
      m->resz = spacing[2];

      updateProgressAndMessage("Allocating Voxel Memory", 5);
      //Allocate all of our Voxel Objects
	  m->initializeAttributes();

      updateProgressAndMessage(("Reading the Voxel Data from the HDF5 File"), 10);
	  err = h5Reader->readVoxelData(m->grain_indicies, m->phases, m->euler1s, m->euler2s, m->euler3s, m->crystruct, m->totalpoints);
      CHECK_FOR_ERROR(GrainGeneratorFunc, "GrainGenerator Error reading voxel data from HDF5 Voxel File", err);
    }
    else
    {
      err = -1;
      CHECK_FOR_ERROR(GrainGeneratorFunc, "GrainGenerator Error No suitable Voxel Structure Reader found", err);
    }
  }

  updateProgressAndMessage(("Finding Neighbors"), 44);
  m->find_neighbors();
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", find_neighbors)


  if (m_AlreadyFormed == false)
  {
    updateProgressAndMessage(("Placing Precipitates"), 45);
    m->place_precipitates();
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", place_precipitates)

    updateProgressAndMessage(("Filling In Precipitates"), 47);
    m->fillin_precipitates();
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", fill_gaps)
  }

  updateProgressAndMessage(("Loading ODF Data"), 48);
  err = m->readODFData(h5reader);
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", readODFData)

  updateProgressAndMessage(("Loading Misorientation Data"), 50);
  err = m->readMisorientationData(h5reader);
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", readMisorientationData)

  updateProgressAndMessage(("Assigning Eulers"), 60);
  m->assign_eulers();
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", assign_eulers)

  updateProgressAndMessage(("Measuring Misorientations"), 65);
  m->measure_misorientations();
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", measure_misorientations)

  MAKE_OUTPUT_FILE_PATH ( eulerFile , AIM::SyntheticBuilder::GrainAnglesFile)


  updateProgressAndMessage(("Matching Crystallography"), 65);
  m->matchCrystallography();
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", matchCrystallography)

  updateProgressAndMessage(("Writing Euler Angles"), 81);

  m->write_eulerangles(eulerFile);
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", write_eulerangles)

  /** ********** This section writes the Voxel Data for the Stats Module ****/
  // Create a new HDF5 Volume file by overwriting any HDF5 file that may be in the way
  MAKE_OUTPUT_FILE_PATH ( h5VoxelFile, AIM::SyntheticBuilder::H5VoxelFile)
  H5VoxelWriter::Pointer h5VoxelWriter = H5VoxelWriter::New();
  if (h5VoxelWriter.get() == NULL)
  {
    updateProgressAndMessage("The HDF5 Voxel file could not be created. Does the path exist and do you have write access to the output directory.", 100);
    m = GrainGeneratorFunc::NullPointer();  // Clean up the memory
    return;
  }
  h5VoxelWriter->setFilename(h5VoxelFile);
  updateProgressAndMessage(("Writing HDF5 Voxel Data File"), 83);
  err = h5VoxelWriter->writeVoxelData<GrainGeneratorFunc>(m.get());
  CHECK_FOR_ERROR(GrainGeneratorFunc, "The HDF5 Voxel file could not be written to. Does the path exist and do you have write access to the output directory.", err);

  /************ This writes the grain data if desired*/
  if(m_WriteGrainData)
  {
    updateProgressAndMessage(("Writing Grain Data"), 88);
	MAKE_OUTPUT_FILE_PATH ( GrainDataFile, AIM::MicroStats::GrainDataFile);
    m->write_graindata(GrainDataFile);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", write_graindata)
  }

  /* ********** This section writes the VTK files for visualization *** */
  if (m_WriteVtkFile) {
    updateProgressAndMessage(("Writing VTK Visualization File"), 93);
    MAKE_OUTPUT_FILE_PATH ( vtkVizFile, AIM::Reconstruction::VisualizationVizFile);

    // Setup all the classes that will help us write the Scalars to the VTK File
    std::vector<VtkScalarWriter*> scalarsToWrite;
    {
      VtkScalarWriter* w0 =
          static_cast<VtkScalarWriter*>(new VoxelGrainIdScalarWriter<GrainGeneratorFunc>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if (m_WriteSurfaceVoxel == true) {
      VtkScalarWriter* w0 =
        static_cast<VtkScalarWriter*>(new VoxelSurfaceVoxelScalarWriter<GrainGeneratorFunc>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if (m_WritePhaseId == true){
      VtkScalarWriter* w0 =
        static_cast<VtkScalarWriter*>(new VoxelPhaseIdScalarWriter<GrainGeneratorFunc>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    if (m_WriteIPFColor == true) {
      VtkScalarWriter* w0 =
        static_cast<VtkScalarWriter*>(new VoxelIPFColorScalarWriter<GrainGeneratorFunc>(m.get()));
      w0->m_WriteBinaryFiles = m_WriteBinaryVTKFiles;
      scalarsToWrite.push_back(w0);
    }

    // Create our File Output Writer Object. This will handle all the File Output duties
    VTKRectilinearGridFileWriter vtkWriter;
    vtkWriter.setWriteBinaryFiles(m_WriteBinaryVTKFiles);
    err = vtkWriter.write<GrainGeneratorFunc>(vtkVizFile, m.get(), scalarsToWrite);

    // Now Delete all the Scalar Helpers that we just created and used.
    for (std::vector<VtkScalarWriter*>::iterator iter = scalarsToWrite.begin(); iter != scalarsToWrite.end(); ++iter )
    {
      delete (*iter);
    }


    CHECK_FOR_ERROR(GrainGeneratorFunc, "The Grain Generator threw an Error writing the VTK file format.", err);
  }
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", writeVisualizationFile)
  /* ******* End VTK Visualization File Writing Section ****** */

  /* **********   This CMU's ph format */
  updateProgressAndMessage(("Writing Ph Voxel File"), 95);
  if (m_WritePhFile) {
    MAKE_OUTPUT_FILE_PATH ( phFile, AIM::Reconstruction::PhFile);
    PhWriter phWriter;
    err = phWriter.writeGrainPhFile(phFile, m->grain_indicies, m->xpoints, m->ypoints, m->zpoints);
    CHECK_FOR_ERROR(GrainGeneratorFunc, "The Grain Generator threw an Error writing the Ph file format.", err);
  }


  /* ********** HDF5 Grains File  ********** */
  if (m_WriteHDF5GrainFile)
  {
    updateProgressAndMessage(("Writing Out HDF5 Grain File. This may take a few minutes to complete."), 96);
    MAKE_OUTPUT_FILE_PATH ( hdf5GrainFile, AIM::Reconstruction::HDF5GrainFile);
    H5GrainWriter::Pointer h5GrainWriter = H5GrainWriter::New();
    err = h5GrainWriter->writeHDF5GrainsFile(m.get(), hdf5GrainFile);
    CHECK_FOR_ERROR(GrainGeneratorFunc, "The Grain Generator threw an Error writing the HDF5 Grain file format.", err);
  }
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", writeHDF5GrainsFile)

  // Clean up all the memory
  updateProgressAndMessage(("Cleaning up Memory."), 98);

  m = GrainGeneratorFunc::NullPointer();

  updateProgressAndMessage(("Generation Completed"), 100);
}


