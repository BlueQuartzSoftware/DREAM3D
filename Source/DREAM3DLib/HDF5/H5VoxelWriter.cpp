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
#include "H5VoxelWriter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5VoxelWriter::H5VoxelWriter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5VoxelWriter::~H5VoxelWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5VoxelWriter::writeEulerData(float* ea,
                                  int64_t totalPoints,
                                  bool appendFile)
{
  int err = -1;
  H5DataWriter::Pointer h5writer = H5DataWriter::New();
  h5writer->setFileName(m_FileName);
  err = h5writer->openFile(appendFile);
  if (err < 0)
  {
    return -1;
  }
  int numComp = 3;
  int32_t rank = 2;
  hsize_t dims[2] = { totalPoints, numComp };
  err = h5writer->createVtkObjectGroup(DREAM3D::HDF5::DataContainerName, H5_VTK_STRUCTURED_POINTS);
  if (err < 0)
  {
    std::cout << "Error creating HDF Group " << DREAM3D::HDF5::DataContainerName << std::endl;
    return err;
  }
  err = h5writer->writeScalarData(DREAM3D::HDF5::DataContainerName, ea, DREAM3D::CellData::EulerAngles.c_str(), numComp, rank, dims);
  if (err < 0)
  {
    std::cout << "Error Writing Scalars '" << DREAM3D::CellData::EulerAngles << "' to " << DREAM3D::HDF5::DataContainerName << std::endl;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5VoxelWriter::writeGrainIds(int* grain_indicies, int64_t totalPoints, bool appendFile)
{
  int err = -1;
  H5DataWriter::Pointer h5writer = H5DataWriter::New();
  h5writer->setFileName(m_FileName);
  err = h5writer->openFile(appendFile);
  if (err < 0)
  {
    return -1;
  }
  int numComp = 1;
  int32_t rank = 1;
  hsize_t dims[2] = { totalPoints, numComp };
  err = h5writer->createVtkObjectGroup(DREAM3D::HDF5::DataContainerName, H5_VTK_STRUCTURED_POINTS);
  if (err < 0)
  {
    std::cout << "Error creating HDF Group " << DREAM3D::HDF5::DataContainerName << std::endl;
    return err;
  }
  err = h5writer->writeScalarData(DREAM3D::HDF5::DataContainerName, grain_indicies, DREAM3D::CellData::GrainIds.c_str(), numComp, rank, dims);
  if (err < 0)
  {
    std::cout << "Error Writing Scalars '" << DREAM3D::CellData::GrainIds << "' to " << DREAM3D::HDF5::DataContainerName << std::endl;
    return -1;
  }
  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5VoxelWriter::writePhaseIds(int* phases, int64_t totalPoints, bool appendFile)
{
  int err = -1;
  H5DataWriter::Pointer h5writer = H5DataWriter::New();
  h5writer->setFileName(m_FileName);
  err = h5writer->openFile(appendFile);
  if (err < 0)
  {
    return -1;
  }
  int numComp = 1;
  int32_t rank = 1;
  hsize_t dims[2] = { totalPoints, numComp };
  err = h5writer->createVtkObjectGroup(DREAM3D::HDF5::DataContainerName, H5_VTK_STRUCTURED_POINTS);
  if (err < 0)
  {
    std::cout << "Error creating HDF Group " << DREAM3D::HDF5::DataContainerName << std::endl;
    return err;
  }
  err = h5writer->writeScalarData(DREAM3D::HDF5::DataContainerName, phases, DREAM3D::CellData::Phases.c_str(), numComp, rank, dims);
  if (err < 0)
    {
      std::cout << "Error Writing Scalars '" << DREAM3D::CellData::Phases << "' to " << DREAM3D::HDF5::DataContainerName << std::endl;
      return -1;
    }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5VoxelWriter::writeStructuredPoints(int64_t volDims[3], float spacing[3], float origin[3], bool appendFile)
{
  int err = -1;
  H5DataWriter::Pointer h5writer = H5DataWriter::New();
  h5writer->setFileName(m_FileName);
  err = h5writer->openFile(appendFile);
  if (err < 0)
  {
    return -1;
  }
  err = h5writer->createVtkObjectGroup(DREAM3D::HDF5::DataContainerName, H5_VTK_STRUCTURED_POINTS);
  if (err < 0)
  {
    std::cout << "Error creating HDF Group " << DREAM3D::HDF5::DataContainerName << std::endl;
    return err;
  }
  // This just creates the group and writes the header information
  err = h5writer->writeStructuredPoints(DREAM3D::HDF5::DataContainerName, volDims, spacing, origin);
  if (err < 0)
  {
    std::cout << "Error Writing Structured Points to .h5voxel file" << std::endl;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5VoxelWriter::writeCrystalStructures(const std::vector<Ebsd::CrystalStructure>  &crystruct, bool appendFile)
 {
   H5DataWriter::Pointer h5writer = H5DataWriter::New();
   h5writer->setFileName(m_FileName);
   int err = h5writer->openFile(appendFile);
   if (err < 0)
   {
     return -1;
   }

   err = h5writer->createVtkObjectGroup(DREAM3D::HDF5::DataContainerName, H5_VTK_STRUCTURED_POINTS);
   if (err < 0)
   {
     std::cout << "Error creating HDF Group " << DREAM3D::HDF5::DataContainerName << std::endl;
     return err;
   }
   std::vector<int> fieldData(crystruct.size());
   for (size_t i = 0; i < crystruct.size(); ++i)
   {
     fieldData[i] = crystruct[i];
   }
   err = h5writer->writeFieldData<int>(DREAM3D::HDF5::DataContainerName, fieldData,
                                       DREAM3D::EnsembleData::CrystalStructure.c_str(), 1);
   if (err < 0)
   {
     std::cout << "Error Writing Ensemble Data '" << DREAM3D::EnsembleData::CrystalStructure << "' to " << DREAM3D::HDF5::DataContainerName << std::endl;
   }
   return err;
 }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5VoxelWriter::writePhaseTypes(const std::vector<DREAM3D::Reconstruction::PhaseType> &phaseType, bool appendFile)
{
  H5DataWriter::Pointer h5writer = H5DataWriter::New();
  h5writer->setFileName(m_FileName);
  int err = h5writer->openFile(appendFile);
  if (err < 0)
  {
    return -1;
  }

  err = h5writer->createVtkObjectGroup(DREAM3D::HDF5::DataContainerName, H5_VTK_STRUCTURED_POINTS);
  if (err < 0)
  {
    std::cout << "Error creating HDF Group " << DREAM3D::HDF5::DataContainerName << std::endl;
    return err;
  }

  std::vector<int> fieldData(phaseType.size());
  for (size_t i = 0; i < phaseType.size(); ++i)
  {
    fieldData[i] = phaseType[i];
  }
  err = h5writer->writeFieldData<int>(DREAM3D::HDF5::DataContainerName, fieldData, DREAM3D::FieldData::Phases.c_str(), 1);
  if (err < 0)
  {
    std::cout << "Error Writing Ensemble Data '" << DREAM3D::FieldData::Phases << "' to " << DREAM3D::HDF5::DataContainerName << std::endl;
  }
  return err;
}

