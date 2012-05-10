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
#include "DREAM3DLib/IOFilters/DataContainerReader.h"
#include "DREAM3DLib/IOFilters/DataContainerWriter.h"
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
int VolumeOps::preflightPipeline(VolumeOps::FilterContainerType &pipeline)
{
  // Create the DataContainer object
  DataContainer::Pointer m = DataContainer::New();
  m->addObserver(static_cast<Observer*>(this));
  setErrorCondition(0);
  int preflightError = 0;
  std::stringstream ss;


  // Start looping through the Pipeline and preflight everything
  for (FilterContainerType::iterator filter = pipeline.begin(); filter != pipeline.end(); ++filter)
  {
    (*filter)->setDataContainer(m.get());
    setCurrentFilter(*filter);
    (*filter)->preflight();
    int err = (*filter)->getErrorCondition();
    if(err < 0)
    {
      preflightError |= err;
      setErrorCondition(preflightError);
      setErrorCondition(err);
      ss << (*filter)->getErrorMessage();
    }
  }
  if (preflightError < 0)
  {
    pipelineErrorMessage(ss.str().c_str());
  }
  return preflightError;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOps::execute()
{
  int err = 0;

  m = DataContainer::New();
  m->addObserver(static_cast<Observer*>(this));

  // Create a Vector to hold all the filters. Later on we will execute all the filters
  FilterContainerType pipeline;

  DataContainerReader::Pointer h5Reader = DataContainerReader::New();
  h5Reader->setInputFile(m_H5InputFile);
  pipeline.push_back(h5Reader);


  CropVolume::Pointer crop_volume = CropVolume::New();
  crop_volume->setXMin(m_XMin);
  crop_volume->setYMin(m_YMin);
  crop_volume->setZMin(m_ZMin);
  crop_volume->setXMax(m_XMax);
  crop_volume->setYMax(m_YMax);
  crop_volume->setZMax(m_ZMax);
//  crop_volume->setXRes(m_XRes);
//  crop_volume->setYRes(m_YRes);
//  crop_volume->setZRes(m_ZRes);
  pipeline.push_back(crop_volume);

  ChangeResolution::Pointer change_resolution = ChangeResolution::New();
//  change_resolution->setXMin(m_XMin);
//  change_resolution->setYMin(m_YMin);
//  change_resolution->setZMin(m_ZMin);
//  change_resolution->setXMax(m_XMax);
//  change_resolution->setYMax(m_YMax);
//  change_resolution->setZMax(m_ZMax);
  change_resolution->setXRes(m_XRes);
  change_resolution->setYRes(m_YRes);
  change_resolution->setZRes(m_ZRes);
  pipeline.push_back(change_resolution);


  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile(m_H5OutputFile);
  pipeline.push_back(writer);


  err = preflightPipeline(pipeline);
  if (err < 0)
  {
    return;
  }
  m = DataContainer::New();

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

    if(DREAM3D_BENCHMARKS)
    {
      std::cout << (*filter)->getNameOfClass() << " Finish Time(ms): " << (MXA::getMilliSeconds() - millis) << std::endl;
      millis = MXA::getMilliSeconds();
    }
  }

#if 0
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
#endif

  updateProgressAndMessage(("Generation Completed"), 100);
}
