

#include <iostream>
#include <string>
#include <vector>

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"

#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/AIMArray.hpp"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/VTKUtils/VTKWriterMacros.h"
#include "DREAM3D/Common/VTKUtils/VTKFileWriters.hpp"

#include "DREAM3D/HDF5/H5VoxelReader.h"


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
    AIM::Reconstruction::CrystalStructure* crystruct;
} Test;


int main(int argc, char **argv)
{
  std::cout << "Starting Conversion of H5Voxel to VTK with Grain ID and IPF Colors" << std::endl;
  std::string iFile = "/private/tmp/BadRecon_UpperRight_v2/BadReconUpperRightv2_VoxelData.h5voxel";

  int err = 0;

  H5VoxelReader::Pointer h5Reader = H5VoxelReader::New();
  h5Reader->setFilename(iFile);

  int dims[3];
  float spacing[3];
  std::cout << "Getting Size and Resolution" << std::endl;
  err = h5Reader->getSizeAndResolution(dims, spacing);
  if (err < 0)
  {
    std::cout << "Error Reading the Dimensions and Resolution from the File." << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<AIM::Reconstruction::CrystalStructure> crystruct;

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
  err = h5Reader->readVoxelData(m_GrainIndicies, m_Phases, m_Euler1s, m_Euler2s, m_Euler3s, crystruct, totalpoints);
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

  FILE* f = fopen("/tmp/out.vtk", "wb");

  WRITE_STRUCTURED_POINTS_HEADER("ASCII", ptr)



  VoxelIPFColorScalarWriter<Test> ipfWriter(ptr);
  ipfWriter.m_WriteBinaryFiles = false;
  ipfWriter.writeScalars(f);

  WRITE_VTK_GRAIN_IDS_ASCII(ptr, AIM::VTK::GrainIdScalarName)

  fclose(f);

  std::cout << "Done Converting" << std::endl;
  return EXIT_SUCCESS;
}
