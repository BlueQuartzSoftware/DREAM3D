

#include "VolumeOps.h"

#include "MXA/MXA.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/HDF5/H5VoxelReader.h"
#include "DREAM3DLib/HDF5/H5VoxelWriter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeOps::VolumeOps()
{
std::cout << "VolumeOps Constructor" << std::endl;
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

  m = VolumeOpsFunc::New();


  updateProgressAndMessage("Reading the Voxel Dimensions and Resolution", 2);
  // Load up the voxel data
  H5VoxelReader::Pointer h5Reader = H5VoxelReader::New();
  h5Reader->setFileName(m_H5InputFile);
  int dims[3];
  float spacing[3];
  float origin[3];
  err = h5Reader->getSizeResolutionOrigin(dims, spacing, origin);
  if (err < 0)
  {
    CHECK_FOR_ERROR(VolumeOpsFunc, "Error Reading the Dimensions and Resolution from the input File.", err);
    setErrorCondition(err);
    return;
  }

  m->xpoints_old = dims[0];
  m->ypoints_old = dims[1];
  m->zpoints_old = dims[2];
  m->totalpoints_old = dims[0] * dims[1] * dims[2];
  m->resx_old = spacing[0];
  m->resy_old = spacing[1];
  m->resz_old = spacing[2];
  m->sizex_old = dims[0]*spacing[0];
  m->sizey_old = dims[1]*spacing[1];
  m->sizez_old = dims[2]*spacing[2];
  m->resx = m_XRes_New;
  m->resy = m_YRes_New;
  m->resz = m_ZRes_New;
  m->sizex = (m_xMax-m_xMin)*m->resx_old;
  m->sizey = (m_yMax-m_yMin)*m->resy_old;
  m->sizez = (m_zMax-m_zMin)*m->resz_old;
  m->xpoints = int(m->sizex / m->resx);
  m->ypoints = int(m->sizey / m->resy);
  m->zpoints = int(m->sizez / m->resz);
  m->totalpoints = m->xpoints * m->ypoints * m->zpoints;
  m->xstart = m_xMin*spacing[0];
  m->ystart = m_yMin*spacing[1];
  m->zstart = m_zMin*spacing[2];

  m->initializeAttributes();
  
  updateProgressAndMessage(("Reading the Voxel Data from the HDF5 File"), 10);
  err = h5Reader->readVoxelData(m->m_GrainIndicies_old, m->m_Phases_old, m->m_Euler1s_old, m->m_Euler2s_old, m->m_Euler3s_old, m->crystruct, m->phaseType, m->totalpoints_old);
  CHECK_FOR_ERROR(VolumeOpsFunc, (h5Reader->getErrorMessage()), err);
  CHECK_FOR_CANCELED(VolumeOpsFunc, "VolumeOps was canceled",  readVoxelData)

  updateProgressAndMessage(("Operating on Volume"), 50);
  m->manipulate_volume();
  CHECK_FOR_ERROR(VolumeOpsFunc, "Error operating on volume", err);
  CHECK_FOR_CANCELED(VolumeOpsFunc, "VolumeOps was canceled",  manipulate_volume)
  
  /** ********** This section writes the Voxel Data for the Stats Module *** */
  // Create a new HDF5 Volume file by overwriting any HDF5 file that may be in the way
  MAKE_OUTPUT_FILE_PATH ( hdf5VolumeFile, DREAM3D::Reconstruction::H5VoxelFile)
  H5VoxelWriter::Pointer h5VolWriter = H5VoxelWriter::New();
  if (h5VolWriter.get() == NULL)
  {
    CHECK_FOR_ERROR(VolumeOpsFunc, "The HDF5 Voxel file could not be created. Does the path exist and do you have write access to the output directory.", -1)
  }
  h5VolWriter->setFileName(hdf5VolumeFile);
  updateProgressAndMessage(("Writing HDF5 Voxel Data File"), 90);
  err = h5VolWriter->writeData<VolumeOpsFunc>(m.get());
  CHECK_FOR_ERROR(VolumeOpsFunc, "The HDF5 Voxel file could not be written to. Does the path exist and do you have write access to the output directory.", err);

  updateProgressAndMessage(("Generation Completed"), 100);
}
