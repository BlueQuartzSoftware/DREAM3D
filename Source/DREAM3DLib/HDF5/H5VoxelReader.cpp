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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5VoxelReader::H5VoxelReader() :
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
    H5Utilities::closeFile(m_FileId);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5VoxelReader::getSizeResolutionOrigin(int volDims[3], float spacing[3], float origin[3])
{
  int err = 0;

  if(m_FileName.empty() == true)
  {
    setErrorMessage("H5ReconVolumeReader Error; Filename was empty");
    return -1;
  }

  OPEN_HDF5_FILE(fileId, m_FileName);
  OPEN_RECONSTRUCTION_GROUP(reconGid, DREAM3D::HDF5::VoxelDataName.c_str(), fileId);

  err = H5Lite::readPointerDataset(reconGid, H5_DIMENSIONS, volDims);
  if(err < 0)
  {
    setErrorMessage("H5ReconVolumeReader Error Reading the Dimensions");
    err = H5Gclose(reconGid);
    err = H5Fclose(fileId);
    return err;
  }
  err = H5Lite::readPointerDataset(reconGid, H5_SPACING, spacing);
  if(err < 0)
  {
    setErrorMessage("H5ReconVolumeReader Error Reading the Spacing (Resolution)");
    err = H5Gclose(reconGid);
    err = H5Fclose(fileId);
    return err;
  }

  err = H5Lite::readPointerDataset(reconGid, H5_ORIGIN, origin);
  if(err < 0)
  {
    setErrorMessage("H5ReconVolumeReader Error Reading the Origin");
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
int H5VoxelReader::readHyperSlab(int xdim, int ydim, int zIndex, int* fileVoxelLayer)
{
  int err = 0;

  if(m_FileName.empty() == true)
  {
    std::cout << "H5ReconVolumeReader Error; Filename was empty" << std::endl;
    return -1;
  }
  OPEN_HDF5_FILE(fileId, m_FileName);
  OPEN_RECONSTRUCTION_GROUP(reconGid, DREAM3D::HDF5::VoxelDataName.c_str(), fileId);
  OPEN_RECONSTRUCTION_GROUP(scalarGid, H5_SCALAR_DATA_GROUP_NAME, reconGid);

  hid_t dataset;
  hid_t filespace;
  hid_t memspace;

  hsize_t col_dims[1];
  hsize_t count[2];
  hsize_t offset[2];
  herr_t status;
  hsize_t rankc = 1;

  dataset = H5Dopen(scalarGid, DREAM3D::VTK::GrainIdScalarName.c_str(), H5P_DEFAULT);
  filespace = H5Dget_space(dataset); /* Get filespace handle first. */
  col_dims[0] = xdim * ydim;
  memspace = H5Screate_simple(rankc, col_dims, NULL);

  offset[0] = zIndex * xdim * ydim;
  count[0] = xdim * ydim;

  status = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, count, NULL);
  status = H5Dread(dataset, H5T_NATIVE_INT, memspace, filespace, H5P_DEFAULT, fileVoxelLayer);

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



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5VoxelReader::readVoxelData(AIMArray<int>::Pointer grain_indicies,
                  AIMArray<int>::Pointer phases,
                  AIMArray<float>::Pointer euler1s,
                  AIMArray<float>::Pointer euler2s,
                  AIMArray<float>::Pointer euler3s,
                  std::vector<Ebsd::CrystalStructure> &crystruct,
                  std::vector<DREAM3D::Reconstruction::PhaseType> &phaseType,
                  int totalpoints)
{
  int err = 0;
  if(m_FileName.empty() == true)
  {
    m_ErrorMessage = "H5ReconVolumeReader Error; Filename was empty";
    return -1;
  }


  err = readScalarData(DREAM3D::VTK::GrainIdScalarName, grain_indicies->GetPointer(0));
  if (err < 0) { return err; }

  err = readScalarData(DREAM3D::VTK::PhaseIdScalarName, phases->GetPointer(0));
  if (err < 0) { return err; }


  OPEN_RECONSTRUCTION_GROUP(reconGid, DREAM3D::HDF5::VoxelDataName.c_str(), m_FileId);
  OPEN_RECONSTRUCTION_GROUP(scalarGid, H5_SCALAR_DATA_GROUP_NAME, reconGid);

  // Allocate an Array for the Euler Data with is nRows X 3 Columns
  AIMArray<float>::Pointer fData = AIMArray<float>::CreateArray(0);
  fData->WritePointer(0, totalpoints*3);
  fData->SetNumberOfComponents(3);
  err = readScalarData(DREAM3D::VTK::EulerAnglesName, fData->GetPointer(0));
  if(err < 0)
  {
    m_ErrorMessage = "H5VoxelReader Error Reading the Euler Angles";
    err = H5Gclose(scalarGid);
    err = H5Gclose(reconGid);
    return err;
  }
  // Now copy the data into our 3 separate Euler Arrays
  float* e = fData->GetPointer(0);
  for (int i = 0; i < totalpoints; ++i)
  {
    euler1s->SetValue(i, e[i * 3]);
    euler2s->SetValue(i, e[i * 3 + 1]);
    euler3s->SetValue(i, e[i * 3 + 2]);
  }

// Close the group as we are done with it.
  err |= H5Gclose(scalarGid);


  err = readFieldDataWithCast<Ebsd::CrystalStructure, uint32_t>(DREAM3D::VTK::CrystalStructureName, crystruct);
  if(err < 0)
    {
      m_ErrorMessage = "H5VoxelReader Error Reading the Crystal Structure Field Data";
      err = H5Gclose(reconGid);
      return err;
    }

  err = readFieldDataWithCast<DREAM3D::Reconstruction::PhaseType, uint32_t>(DREAM3D::VTK::PhaseTypeName, phaseType);
  if(err < 0)
    {
      m_ErrorMessage = "H5VoxelReader Error Reading the Phase Type Data";
      err = H5Gclose(reconGid);
      return err;
    }


  err = H5Gclose(reconGid);
  err = H5Fclose(m_FileId);
  m_FileId = 0;
  return err;
}

