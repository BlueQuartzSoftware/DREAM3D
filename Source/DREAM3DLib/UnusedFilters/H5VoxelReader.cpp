/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#include "H5VoxelReader.h"

#include <limits>
#include "DREAM3DLib/Common/PipelineMessage.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5VoxelReader::H5VoxelReader() :
  AbstractFilter(),
  m_FileId(-1)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5VoxelReader::~H5VoxelReader()
{
  if (m_FileId > 0)
  {
    QH5Utilities::closeFile(m_FileId);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5VoxelReader::getSizeResolutionOrigin(int64_t volDims[3], float spacing[3], float origin[3])
{
  int err = 0;

  if(m_FileName.isEmpty() == true)
  {
    addErrorMessage(getHumanLabel(), "H5ReconVolumeReader Error; Filename was empty", -1);
    return -1;
  }

  OPEN_HDF5_FILE(fileId, m_FileName);
  OPEN_RECONSTRUCTION_GROUP(reconGid, DREAM3D::StringConstants::VoxelDataName.toLatin1().data(), fileId);

  err = QH5Lite::readPointerDataset(reconGid, H5_DIMENSIONS, volDims);
  if(err < 0)
  {
    PipelineMessage em (getHumanLabel(), "H5ReconVolumeReader Error Reading the Dimensions", err);
    addErrorMessage(em);
    err = H5Gclose(reconGid);
    err = H5Fclose(fileId);
    return err;
  }
  err = QH5Lite::readPointerDataset(reconGid, H5_SPACING, spacing);
  if(err < 0)
  {
    PipelineMessage em (getHumanLabel(), "H5ReconVolumeReader Error Reading the Spacing (Resolution)", err);
    err = H5Gclose(reconGid);
    addErrorMessage(em);
    err = H5Fclose(fileId);
    return err;
  }

  err = QH5Lite::readPointerDataset(reconGid, H5_ORIGIN, origin);
  if(err < 0)
  {
    PipelineMessage em (getHumanLabel(), "H5ReconVolumeReader Error Reading the Origin", err);
    addErrorMessage(em);
    err = H5Gclose(reconGid);
    err = H5Fclose(fileId);
    return err;
  }

  err = H5Gclose(reconGid);
  err = H5Fclose(fileId);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5VoxelReader::readHyperSlab(int64_t xdim, int64_t ydim, int64_t zIndex, int* fileVoxelLayer)
{
  int err = 0;

  if(m_FileName.isEmpty() == true)
  {
    qDebug() << "H5ReconVolumeReader Error; Filename was empty" ;
    return -1;
  }
  OPEN_HDF5_FILE(fileId, m_FileName);
  OPEN_RECONSTRUCTION_GROUP(reconGid, DREAM3D::Defaults::VolumeDataContainerName.toLatin1().data(), fileId);
  OPEN_RECONSTRUCTION_GROUP(scalarGid, H5_CELL_DATA_GROUP_NAME, reconGid);

  hid_t dataset;
  hid_t filespace;
  hid_t memspace;

  hsize_t col_dims[1];
  hsize_t count[2];
  hsize_t offset[2];
  herr_t status;
  int rankc = 1;

  dataset = H5Dopen(scalarGid, DREAM3D::CellData::FeatureIds.toLatin1().data(), H5P_DEFAULT);
  filespace = H5Dget_space(dataset); /* Get filespace handle first. */
  col_dims[0] = xdim * ydim;
  memspace = H5Screate_simple(rankc, col_dims, NULL);

  offset[0] = zIndex * xdim * ydim;
  count[0] = xdim * ydim;

  status = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, count, NULL);
  if (status < 0)
  {
    //FIXME: Implement the error trapping
  }
  status = H5Dread(dataset, H5T_NATIVE_INT, memspace, filespace, H5P_DEFAULT, fileVoxelLayer);
  if (status < 0)
  {
    //FIXME: Implement the error trapping
  }

  H5Sclose(memspace);
  H5Dclose(dataset);
  H5Sclose(filespace);

  err = H5Gclose(scalarGid);
  err = H5Gclose(reconGid);
  err = H5Fclose(fileId);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5VoxelReader::readVoxelData(int* feature_indicies,
                                 int* phases,
                                 float* eulerangles,
                                 std::vector<unsigned int>& crystruct,
                                 std::vector<unsigned int>& phaseType,
                                 int64_t totalpoints)
{
  int err = 0;
  if(m_FileName.isEmpty() == true)
  {
    addErrorMessage(getHumanLabel(), "H5ReconVolumeReader Error; Filename was empty", -1);
    return -1;
  }


  err = readScalarData(DREAM3D::CellData::FeatureIds, feature_indicies);
  if (err < 0) { return err; }

  err = readScalarData(DREAM3D::CellData::Phases, phases);
  if (err < 0) { return err; }


  OPEN_RECONSTRUCTION_GROUP(reconGid, DREAM3D::Defaults::VolumeDataContainerName.toLatin1().data(), m_FileId);
  OPEN_RECONSTRUCTION_GROUP(scalarGid, H5_CELL_DATA_GROUP_NAME, reconGid);

  // Check to make sure we can read the amount of data requested. On a 32 bit
  // system size_t is a 32 bit unsigned value. On a 64 bit system it will be
  // a 64 bit unsigend number
#if   (CMP_SIZEOF_SSIZE_T==4)
  if (totalpoints * 3 > static_cast<int64_t>(std::numeric_limits<size_t>::max()) )
  {
    PipelineMessage em (getHumanLabel(), "Trying to read more data than this architecture can handle.", -1);
    addErrorMessage(em);
    return -400;
  }
#endif

  // Allocate an Array for the Euler Data with is nRows X 3 Columns
  QVector<int> dims(1, 3);
  DataArray<float>::Pointer fData = DataArray<float>::CreateArray(totalpoints, dims, DREAM3D::CellData::EulerAngles);
  err = readScalarData(DREAM3D::CellData::EulerAngles, eulerangles);
  if(err < 0)
  {
    addErrorMessage(getHumanLabel(), "H5VoxelReader Error Reading the Euler Angles", err);
    err |= H5Gclose(scalarGid);
    err |= H5Gclose(reconGid);
    return err;
  }

// Close the group as we are done with it.
  err |= H5Gclose(scalarGid);


  err = readFeatureData<unsigned int, uint32_t>(DREAM3D::EnsembleData::CrystalStructures, crystruct);
  if(err < 0)
  {
    addErrorMessage(getHumanLabel(), "H5VoxelReader Error Reading the Crystal Structure Feature Data", err);
    err |= H5Gclose(reconGid);
    return err;
  }

  err = readFeatureData<unsigned int, uint32_t>(DREAM3D::EnsembleData::PhaseTypes, phaseType);
  if(err < 0)
  {
    addErrorMessage(getHumanLabel(), "H5VoxelReader Error Reading the Phase Type Data", err);
    err |= H5Gclose(reconGid);
    return err;
  }


  err |= H5Gclose(reconGid);
  err |= H5Fclose(m_FileId);
  m_FileId = 0;
  return err;
}

