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

/**
 * @file H5VoxelToVtk.cpp
 * @brief This program is mainly a debugging program to ensure the .h5voxel file
 * has the correct data since at the current time there is no way to visualize
 * the .h5voxel in ParaView or anything else. This will convert the .h5voxel to a
 * VTK structured points data file with Grain IDs and IPS Colors.
 */

#include <iostream>
#include <string>
#include <vector>

#include "MXA/MXA.h"
#include "MXA/Common/MXASetGetMacros.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AIMArray.hpp"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/VTKUtils/VTKWriterMacros.h"
#include "DREAM3DLib/VTKUtils/VTKFileWriters.hpp"

#include "DREAM3DLib/HDF5/H5VoxelReader.h"


typedef struct {
    int xpoints;
    int ypoints;
    int zpoints;
    float resx; float resy; float resz;
    int* grain_indicies;
    int* phases;
    float* euler1s;
    float* euler2s;
    float* euler3s;
    float* quats;
    Ebsd::CrystalStructure* crystruct;
} Test;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::cout << "Starting Conversion of H5Voxel to VTK with Grain ID and IPF Colors" << std::endl;
  if (argc < 3)
  {
    std::cout << "This program takes 2 arguments: Input .h5voxel file and output vtk file." << std::endl;
    return EXIT_FAILURE;
  }


  std::string iFile = argv[1];

  int err = 0;

  H5VoxelReader::Pointer h5Reader = H5VoxelReader::New();
  h5Reader->setFileName(iFile);

  int dims[3];
  float spacing[3];
  std::cout << "Getting Size and Resolution" << std::endl;
  err = h5Reader->getSizeAndResolution(dims, spacing);
  if (err < 0)
  {
    std::cout << "Error Reading the Dimensions and Resolution from the File." << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<Ebsd::CrystalStructure> crystruct;
  std::vector<DREAM3D::Reconstruction::PhaseType> phaseType;

  DECLARE_WRAPPED_ARRAY(grain_indicies, m_GrainIndicies, int)
  DECLARE_WRAPPED_ARRAY(phases, m_Phases, int);
  DECLARE_WRAPPED_ARRAY(euler1s, m_Euler1s, float);
  DECLARE_WRAPPED_ARRAY(euler2s, m_Euler2s, float);
  DECLARE_WRAPPED_ARRAY(euler3s, m_Euler3s, float);

  m_GrainIndicies = AIMArray<int>::CreateArray(0);
  m_Phases = AIMArray<int>::CreateArray(0);
  m_Euler1s = AIMArray<float>::CreateArray(0);
  m_Euler2s = AIMArray<float>::CreateArray(0);
  m_Euler3s = AIMArray<float>::CreateArray(0);


  int totalpoints = dims[0] * dims[1] * dims[2];
  grain_indicies = m_GrainIndicies->WritePointer(0, totalpoints);
  phases = m_Phases->WritePointer(0, totalpoints);
  euler1s = m_Euler1s->WritePointer(0, totalpoints);
  euler2s = m_Euler2s->WritePointer(0, totalpoints);
  euler3s = m_Euler3s->WritePointer(0, totalpoints);

  std::cout << "Reading Voxel Data" << std::endl;
  err = h5Reader->readVoxelData(m_GrainIndicies, m_Phases, m_Euler1s, m_Euler2s, m_Euler3s, crystruct, phaseType, totalpoints);
  if (err < 0)
  {
    std::cout << "Error reading h5voxel file." << std::endl;
    return EXIT_FAILURE;
  }

  Test* ptr = (Test*)(malloc(sizeof(Test)));
  ptr->xpoints = dims[0];
  ptr->ypoints = dims[1];
  ptr->zpoints = dims[2];
  ptr->resx = spacing[0];
  ptr->resy = spacing[1];
  ptr->resz = spacing[2];
  ptr->grain_indicies = grain_indicies;
  ptr->phases = phases;
  ptr->euler1s = euler1s;
  ptr->euler2s = euler2s;
  ptr->euler3s = euler3s;
  ptr->crystruct = &(crystruct.front());

  size_t total = totalpoints;

  std::cout << "Writing VTK file" << std::endl;

  FILE* f = fopen(argv[2], "wb");

  WRITE_STRUCTURED_POINTS_HEADER("ASCII", ptr)

  VoxelIPFColorScalarWriter<Test> ipfWriter(ptr);
  ipfWriter.m_WriteBinaryFiles = false;
  ipfWriter.writeScalars(f);

  WRITE_VTK_GRAIN_IDS_ASCII(ptr, DREAM3D::VTK::GrainIdScalarName)

  fclose(f);
  free(ptr);

  std::cout << "Done Converting" << std::endl;
  return EXIT_SUCCESS;
}
