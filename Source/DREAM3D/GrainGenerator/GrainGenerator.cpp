

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

#include "DREAM3D/GrainGenerator/GrainGeneratorVoxelWriter.h"
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
m_NumGrains(0),
m_ShapeClass(0),
m_XResolution(0.0),
m_YResolution(0.0),
m_ZResolution(0.0),
m_FillingErrorWeight(0.0),
m_NeighborhoodErrorWeight(0.0),
m_SizeDistErrorWeight(0.0),
m_PeriodicBoundary(true),
m_AlreadyFormed(false),
m_Precipitates(0),
m_WriteBinaryFiles(true),
m_WriteVisualizationFile(false),
m_WriteIPFFile(false),
m_WriteHDF5GrainFile(false),
m_WritePhFile(false)

{
std::cout << "GrainGenerator Constructor" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGenerator::~GrainGenerator()
{
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
    updateProgressAndMessage(("Error Opening HDF5 Stats File. Nothing generated"), 100);
    setCancel(true);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", readHDF5StatsFile)
    return;
  }

  if (m_AlreadyFormed == false)
  {
    m->periodic_boundaries = m_PeriodicBoundary;
    m->numgrains = m_NumGrains;
    m->shapeclass = m_ShapeClass;
    m->resx = m_XResolution;
    m->resy = m_YResolution;
    m->resz = m_ZResolution;
    m->fillingerrorweight = m_FillingErrorWeight;
    m->neighborhooderrorweight = m_NeighborhoodErrorWeight;
    m->sizedisterrorweight = m_SizeDistErrorWeight;

    updateProgressAndMessage(("Loading Stats Data"), 5);
    err = m->readReconStatsData(h5reader);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", readReconStatsData)

    updateProgressAndMessage(("Loading Axis Orientation Data"), 10);
    err = m->readAxisOrientationData(h5reader);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", readAxisOrientationData);

    updateProgressAndMessage(("Packing Grains"), 25);
    m->numgrains = m->pack_grains(m->numgrains);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", pack_grains)

    updateProgressAndMessage(("Assigning Voxels"), 30);
    m->numgrains = m->assign_voxels(m->numgrains);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", assign_voxels)

    updateProgressAndMessage(("Filling Gaps"), 40);
    m->fill_gaps(m->numgrains);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", fill_gaps)

    updateProgressAndMessage(("Adjusting Boundaries"), 42);
//    m->numgrains = m->adjust_boundaries(m->numgrains);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", adjust_boundaries)
  }
  else if (m_AlreadyFormed == true)
  {
    updateProgressAndMessage(("Loading Stats Data"), 10);
    err = m->readReconStatsData(h5reader);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", readReconStatsData)

    updateProgressAndMessage(("Loading Axis Orientation Data"), 25);
    err = m->readAxisOrientationData(h5reader);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", readAxisOrientationData);

    updateProgressAndMessage(("Reading Structure"), 40);
    std::string ext = MXAFileInfo::extension(m_StructureFile);
    if (ext.compare("vtk") == 0) 
	{
      VTKStructureReader::Pointer reader = VTKStructureReader::New();
      reader->setInputFileName(m_StructureFile);
      reader->setGrainIdScalarName(AIM::Reconstruction::GrainIdScalarName);
      reader->setPhaseIdScalarName(AIM::Reconstruction::PhaseIdScalarName);
      err = reader->readStructure(m.get());
      if (err < 0) { setCancel(true); }
      CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", reading_structure)
    }
    else if (ext.compare("h5") == 0)
	{
	  // Load up the voxel data
	  H5VoxelReader::Pointer h5Reader = H5VoxelReader::New();
	  h5Reader->setFilename(m_StructureFile);
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

	  updateProgressAndMessage("Allocating Voxel Memory", 5);
	  //Allocate all of our Voxel Objects
	  m->voxels.reset(new GrainGeneratorVoxel[m->totalpoints]);

	  updateProgressAndMessage(("Reading the Voxel Data from the HDF5 File"), 10);
	  err = h5Reader->readVoxelData(m->voxels, m->crystruct, m->totalpoints);
	  if (err < 0)
	  {
		updateProgressAndMessage("Error Reading the Voxel Data from the File.", 100);
		setErrorCondition(err);
		return;
      }
	}
	else
    {
      setCancel(true);
      CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", reading_structure);
    }
  }

  updateProgressAndMessage(("Finding Neighbors"), 44);
  m->find_neighbors();
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", find_neighbors)

  if (m_AlreadyFormed == false)
  {
    updateProgressAndMessage(("Placing Precipitates"), 45);
    m->numgrains = m->place_precipitates(m->numgrains);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", place_precipitates)

    updateProgressAndMessage(("Filling In Precipitates"), 47);
    m->fillin_precipitates(m->numgrains);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", fill_gaps)
  }

  updateProgressAndMessage(("Loading ODF Data"), 48);
  err = m->readODFData(h5reader);
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", readODFData)

  updateProgressAndMessage(("Loading Misorientation Data"), 50);
  err = m->readMisorientationData(h5reader);
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", readMisorientationData)

  updateProgressAndMessage(("Assigning Eulers"), 60);
  m->assign_eulers(m->numgrains);
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", assign_eulers)

  updateProgressAndMessage(("Measuring Misorientations"), 65);
  m->measure_misorientations();
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", measure_misorientations)

  MAKE_OUTPUT_FILE_PATH ( eulerFile , AIM::SyntheticBuilder::GrainAnglesFile)
  MAKE_OUTPUT_FILE_PATH ( reconVisFile, AIM::Reconstruction::VisualizationVizFile);
  MAKE_OUTPUT_FILE_PATH ( reconIPFVisFile, AIM::Reconstruction::IPFVizFile);
  MAKE_OUTPUT_FILE_PATH ( hdf5GrainFile, AIM::Reconstruction::HDF5GrainFile);
  MAKE_OUTPUT_FILE_PATH ( phFile, AIM::Reconstruction::PhFile);

  updateProgressAndMessage(("Matching Crystallography"), 65);
  m->matchCrystallography();
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", matchCrystallography)

  updateProgressAndMessage(("Writing Euler Angles"), 81);

  m->write_eulerangles(eulerFile);
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", write_eulerangles)

  /** ********** This section writes the Voxel Data for the Stats Module *** */
  // Create a new HDF5 Volume file by overwriting any HDF5 file that may be in the way
  MAKE_OUTPUT_FILE_PATH ( hdf5VolumeFile, AIM::SyntheticBuilder::H5VolumeFile)
  H5VoxelWriter::Pointer h5VolWriter = H5VoxelWriter::New();
  if (h5VolWriter.get() == NULL)
  {
    updateProgressAndMessage("The HDF5 Voxel file could not be created. Does the path exist and do you have write access to the output directory.", 100);
    m = GrainGeneratorFunc::NullPointer();  // Clean up the memory
    return;
  }
  h5VolWriter->setFilename(hdf5VolumeFile);
  updateProgressAndMessage(("Writing HDF5 Voxel Data File"), 83);
  err = h5VolWriter->writeVoxelData<GrainGeneratorFunc, GrainGeneratorVoxel>(m.get());
  if (err < 0)
  {
    updateProgressAndMessage("The HDF5 Voxel file could not be written to. Does the path exist and do you have write access to the output directory.", 100);
    m = GrainGeneratorFunc::NullPointer();  // Clean up the memory
    return;
  }

  /** ********** This section writes the VTK files for visualization *** */
  GrainGeneratorVoxelWriter::Pointer vtkWriter = GrainGeneratorVoxelWriter::New();
  vtkWriter->setWriteBinaryFiles(m_WriteBinaryFiles);

  updateProgressAndMessage(("Writing VTK Visualization File"), 93);
  if (m_WriteVisualizationFile) {vtkWriter->writeVisualizationFile(m.get(), reconVisFile);}
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", writeVisualizationFile)

  updateProgressAndMessage(("Writing VTK Inverse Pole Figure File"), 94);
  if (m_WriteIPFFile) {vtkWriter->writeIPFVizFile(m.get(), reconIPFVisFile);}
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", writeIPFVizFile)

  /** ******* End VTK Visualization File Writing Section ****** */

  /*  This CMU's ph format */
  updateProgressAndMessage(("Writing Ph Voxel File"), 95);
  if (m_WritePhFile) {vtkWriter->writePhFile(m.get(), phFile);}
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", writePhFile)

  updateProgressAndMessage(("Writing Out HDF5 Grain File. This may take a few minutes to complete."), 96);
  H5GrainWriter::Pointer h5GrainWriter = H5GrainWriter::New();
  if (m_WriteHDF5GrainFile) { h5GrainWriter->writeHDF5GrainsFile(m.get(), hdf5GrainFile); }
  CHECK_FOR_CANCELED(GrainGeneratorFunc, "GrainGenerator Was canceled", writeHDF5GrainsFile)

  // Clean up all the memory
  updateProgressAndMessage(("Cleaning up Memory."), 98);
  m = GrainGeneratorFunc::NullPointer();

  updateProgressAndMessage(("Generation Completed"), 100);
}
