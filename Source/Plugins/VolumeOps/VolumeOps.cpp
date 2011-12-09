/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "VolumeOps.h"

#include "MXA/MXA.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/HDF5/H5VoxelReader.h"
#include "DREAM3DLib/HDF5/H5VoxelWriter.h"

#include "DREAM3DLib/StatisticsFilters/LoadVolume.h"
#include "DREAM3DLib/ManipulationFilters/CropVolume.h"
#include "DREAM3DLib/ManipulationFilters/ChangeResolution.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeOps::VolumeOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeOps::~VolumeOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOps::execute()
{
  int err = 0;
  // Start the Benchmark Clock
  START_CLOCK()

  m = DataContainer::New();

  updateProgressAndMessage(("Reading the Voxel Data from the HDF5 File"), 10);
  LoadVolume::Pointer load_volume = LoadVolume::New();
  load_volume->setInputFile(m_H5InputFile);
  load_volume->addObserver(static_cast<Observer*>(this));
  load_volume->setDataContainer(m.get());
  load_volume->execute();
  err = load_volume->getErrorCondition();
  CHECK_FOR_ERROR(DataContainer, "Error Loading Volume", err);
  CHECK_FOR_CANCELED(DataContainer, "VolumeOps was canceled",  load_volume)

  float sizex = (m_XMax-m_XMin)*m->resx;
  float sizey = (m_YMax-m_YMin)*m->resy;
  float sizez = (m_ZMax-m_ZMin)*m->resz;
  int xp = int(sizex / m_XRes);
  int yp = int(sizey / m_YRes);
  int zp = int(sizez / m_ZRes);
  int totpoints = xp * yp * zp;
  float xstart = m_XMin*m->resx;
  float ystart = m_YMin*m->resy;
  float zstart = m_ZMin*m->resz;

  // updateProgressAndMessage(("Operating on Volume"), 50);
  if(m->xpoints != xp || m->ypoints != yp || m->zpoints != zp)
  {
	  updateProgressAndMessage(("Cropping Volume"), 50);
	  CropVolume::Pointer crop_volume = CropVolume::New();
	  crop_volume->setXStart(xstart);
	  crop_volume->setYStart(ystart);
	  crop_volume->setZStart(zstart);
	  crop_volume->setXP(xp);
	  crop_volume->setYP(yp);
	  crop_volume->setZP(zp);
	  crop_volume->addObserver(static_cast<Observer*>(this));
	  crop_volume->setDataContainer(m.get());
	  crop_volume->execute();
	  err = crop_volume->getErrorCondition();
	  CHECK_FOR_ERROR(DataContainer, "Error Croping Volume", err);
	  CHECK_FOR_CANCELED(DataContainer, "VolumeOps was canceled",  crop_volume)
  }

  if(m->resx != m_XRes || m->resy != m_YRes || m->resz != m_ZRes)
  {
	  updateProgressAndMessage(("Changing Resolution"), 50);
	  ChangeResolution::Pointer change_resolution = ChangeResolution::New();
	  change_resolution->setXRes(m_XRes);
	  change_resolution->setYRes(m_YRes);
	  change_resolution->setZRes(m_ZRes);
	  change_resolution->setXP(xp);
	  change_resolution->setYP(yp);
	  change_resolution->setZP(zp);
	  change_resolution->addObserver(static_cast<Observer*>(this));
	  change_resolution->setDataContainer(m.get());
	  change_resolution->execute();
	  err = change_resolution->getErrorCondition();
	  CHECK_FOR_ERROR(DataContainer, "Error Changing Resolution", err);
	  CHECK_FOR_CANCELED(DataContainer, "VolumeOps was canceled",  change_resolution)
  }

  /** ********** This section writes the Voxel Data for the VolumeOps Module *** */
  // Create a new HDF5 Volume file by overwriting any HDF5 file that may be in the way
  //MAKE_OUTPUT_FILE_PATH ( m_H5OutputFile, DREAM3D::Reconstruction::H5VoxelFile)
  H5VoxelWriter::Pointer h5VolWriter = H5VoxelWriter::New();
  if (h5VolWriter.get() == NULL)
  {
    CHECK_FOR_ERROR(DataContainer, "The HDF5 Voxel file could not be created. Does the path exist and do you have write access to the output directory.", -1)
  }
  h5VolWriter->setFileName(m_H5OutputFile);
  updateProgressAndMessage(("Writing HDF5 Voxel Data File"), 90);
  err = h5VolWriter->writeData<DataContainer>(m.get());
  CHECK_FOR_ERROR(DataContainer, "The HDF5 Voxel file could not be written to. Does the path exist and do you have write access to the output directory.", err);

  updateProgressAndMessage(("Generation Completed"), 100);
}
