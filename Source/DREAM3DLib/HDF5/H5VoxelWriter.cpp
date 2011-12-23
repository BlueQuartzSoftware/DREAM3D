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
int H5VoxelWriter::writeEulerData(float* e1,
                                  float* e2,
                                  float* e3,
                                  int64_t totalPoints,
                                  bool appendFile)
{
  int err = -1;
  AIM_H5VtkDataWriter::Pointer h5writer = AIM_H5VtkDataWriter::New();
  h5writer->setFileName(m_FileName);
  err = h5writer->openFile(appendFile);
  if (err < 0)
  {
    return -1;
  }
  int numComp = 3;
  int32_t rank = 2;
  hsize_t dims[2] = { totalPoints, numComp };
  std::vector<float> dataf( totalPoints * 3);
  for (int64_t i = 0; i <  totalPoints; ++i)
  {
    dataf[i * 3] = e1[i];
    dataf[i * 3 + 1] = e2[i];
    dataf[i * 3 + 2] = e3[i];
  }
  err = h5writer->createVtkObjectGroup(DREAM3D::HDF5::VoxelDataName, H5_VTK_STRUCTURED_POINTS);
  if (err < 0)
  {
    std::cout << "Error creating HDF Group " << DREAM3D::HDF5::VoxelDataName << std::endl;
    return err;
  }
  err = h5writer->writeScalarData(DREAM3D::HDF5::VoxelDataName, &(dataf.front()), DREAM3D::VTK::EulerAnglesName.c_str(), numComp, rank, dims);
  if (err < 0)
  {
    std::cout << "Error Writing Scalars '" << DREAM3D::VTK::EulerAnglesName << "' to " << DREAM3D::HDF5::VoxelDataName << std::endl;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5VoxelWriter::writeGrainIds(int* grain_indicies, int64_t totalPoints, bool appendFile)
{
  int err = -1;
  AIM_H5VtkDataWriter::Pointer h5writer = AIM_H5VtkDataWriter::New();
  h5writer->setFileName(m_FileName);
  err = h5writer->openFile(appendFile);
  if (err < 0)
  {
    return -1;
  }
  int numComp = 1;
  int32_t rank = 1;
  hsize_t dims[2] = { totalPoints, numComp };
  err = h5writer->createVtkObjectGroup(DREAM3D::HDF5::VoxelDataName, H5_VTK_STRUCTURED_POINTS);
  if (err < 0)
  {
    std::cout << "Error creating HDF Group " << DREAM3D::HDF5::VoxelDataName << std::endl;
    return err;
  }
  err = h5writer->writeScalarData(DREAM3D::HDF5::VoxelDataName, grain_indicies, DREAM3D::VTK::GrainIdScalarName.c_str(), numComp, rank, dims);
  if (err < 0)
  {
    std::cout << "Error Writing Scalars '" << DREAM3D::VTK::GrainIdScalarName << "' to " << DREAM3D::HDF5::VoxelDataName << std::endl;
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
  AIM_H5VtkDataWriter::Pointer h5writer = AIM_H5VtkDataWriter::New();
  h5writer->setFileName(m_FileName);
  err = h5writer->openFile(appendFile);
  if (err < 0)
  {
    return -1;
  }
  int numComp = 1;
  int32_t rank = 1;
  hsize_t dims[2] = { totalPoints, numComp };
  err = h5writer->createVtkObjectGroup(DREAM3D::HDF5::VoxelDataName, H5_VTK_STRUCTURED_POINTS);
  if (err < 0)
  {
    std::cout << "Error creating HDF Group " << DREAM3D::HDF5::VoxelDataName << std::endl;
    return err;
  }
  err = h5writer->writeScalarData(DREAM3D::HDF5::VoxelDataName, phases, DREAM3D::VTK::PhaseIdScalarName.c_str(), numComp, rank, dims);
  if (err < 0)
    {
      std::cout << "Error Writing Scalars '" << DREAM3D::VTK::PhaseIdScalarName << "' to " << DREAM3D::HDF5::VoxelDataName << std::endl;
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
  AIM_H5VtkDataWriter::Pointer h5writer = AIM_H5VtkDataWriter::New();
  h5writer->setFileName(m_FileName);
  err = h5writer->openFile(appendFile);
  if (err < 0)
  {
    return -1;
  }
  err = h5writer->createVtkObjectGroup(DREAM3D::HDF5::VoxelDataName, H5_VTK_STRUCTURED_POINTS);
  if (err < 0)
  {
    std::cout << "Error creating HDF Group " << DREAM3D::HDF5::VoxelDataName << std::endl;
    return err;
  }
  // This just creates the group and writes the header information
  err = h5writer->writeStructuredPoints(DREAM3D::HDF5::VoxelDataName, volDims, spacing, origin);
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
   AIM_H5VtkDataWriter::Pointer h5writer = AIM_H5VtkDataWriter::New();
   h5writer->setFileName(m_FileName);
   int err = h5writer->openFile(appendFile);
   if (err < 0)
   {
     return -1;
   }

   err = h5writer->createVtkObjectGroup(DREAM3D::HDF5::VoxelDataName, H5_VTK_STRUCTURED_POINTS);
   if (err < 0)
   {
     std::cout << "Error creating HDF Group " << DREAM3D::HDF5::VoxelDataName << std::endl;
     return err;
   }
   std::vector<int> fieldData(crystruct.size());
   for (size_t i = 0; i < crystruct.size(); ++i)
   {
     fieldData[i] = crystruct[i];
   }
   err = h5writer->writeFieldData<int>(DREAM3D::HDF5::VoxelDataName, fieldData,
                                       DREAM3D::VTK::CrystalStructureName.c_str(), 1);
   if (err < 0)
   {
     std::cout << "Error Writing Field Data '" << DREAM3D::VTK::CrystalStructureName << "' to " << DREAM3D::HDF5::VoxelDataName << std::endl;
   }
   return err;
 }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5VoxelWriter::writePhaseTypes(const std::vector<DREAM3D::Reconstruction::PhaseType> &phaseType, bool appendFile)
{
  AIM_H5VtkDataWriter::Pointer h5writer = AIM_H5VtkDataWriter::New();
  h5writer->setFileName(m_FileName);
  int err = h5writer->openFile(appendFile);
  if (err < 0)
  {
    return -1;
  }

  err = h5writer->createVtkObjectGroup(DREAM3D::HDF5::VoxelDataName, H5_VTK_STRUCTURED_POINTS);
  if (err < 0)
  {
    std::cout << "Error creating HDF Group " << DREAM3D::HDF5::VoxelDataName << std::endl;
    return err;
  }

  std::vector<int> fieldData(phaseType.size());
  for (size_t i = 0; i < phaseType.size(); ++i)
  {
    fieldData[i] = phaseType[i];
  }
  err = h5writer->writeFieldData<int>(DREAM3D::HDF5::VoxelDataName, fieldData, DREAM3D::VTK::PhaseTypeName.c_str(), 1);
  if (err < 0)
  {
    std::cout << "Error Writing Field Data '" << DREAM3D::VTK::PhaseTypeName << "' to " << DREAM3D::HDF5::VoxelDataName << std::endl;
  }
  return err;
}

