/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
#ifndef H5RECONVOLUMEREADER_H_
#define H5RECONVOLUMEREADER_H_

#include "hdf5.h"

#include <vector>
#include <string>

#include <boost/shared_array.hpp>

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"
#include "MXA/HDF5/H5Lite.h"
#include "MXA/HDF5/H5Utilities.h"

#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/HDF5/VTKH5Constants.h"
#include "DREAM3D/HDF5/H5Macros.h"


/**
 * @class H5VoxelReader H5VoxelReader.h DREAM3D/HDF5/H5VoxelReader.h
 * @brief Reads a Voxel volume from an HDF5 file
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jun 30, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT H5VoxelReader
{
  public:
    MXA_SHARED_POINTERS(H5VoxelReader)
    MXA_TYPE_MACRO(H5VoxelReader)
    MXA_STATIC_NEW_MACRO(H5VoxelReader)

    virtual ~H5VoxelReader();

    MXA_INSTANCE_STRING_PROPERTY(Filename);

    int getSizeAndResolution(int volDims[3], float spacing[3]);

    int readHyperSlab(int xdim, int ydim, int zIndex, int* fileVoxelLayer);




	template<typename T>
	int readVoxelData(boost::shared_array<T> grain_indicies, boost::shared_array<T> phases,
							boost::shared_array<T> euler1s, boost::shared_array<T> euler2s,
							boost::shared_array<T> euler3s, std::vector<AIM::Reconstruction::CrystalStructure> &crystruct,
                            int totalpoints)
{
  int err = 0;
  if (m_Filename.empty() == true)
  {
    std::cout << "H5ReconVolumeReader Error; Filename was empty" << std::endl;
    return -1;
  }

  OPEN_HDF5_FILE(fileId, m_Filename)
  OPEN_RECONSTRUCTION_GROUP(reconGid, AIM::HDF5::VoxelDataName.c_str(), fileId)
  OPEN_RECONSTRUCTION_GROUP(scalarGid, H5_SCALAR_DATA_GROUP_NAME, reconGid)

  int* iData = (int*)(malloc(totalpoints * sizeof(int)));

  // Read in the Grain ID data
  err = H5Lite::readPointerDataset(scalarGid, AIM::VTK::GrainIdScalarName, iData);
  if (err < 0)
  {
    std::cout << "H5ReconVolumeReader Error Reading the Grain IDs" << std::endl;
    free(iData);
    err = H5Gclose(scalarGid);
    err = H5Gclose(reconGid);
    err = H5Fclose(fileId);
    return err;
  }
  for(int i = 0; i < totalpoints; ++i)
  {
    grain_indicies[i] = iData[i];
  }

  // Read the Phase ID data
  err = H5Lite::readPointerDataset(scalarGid, AIM::VTK::PhaseIdScalarName, iData);
  if (err < 0)
  {
    std::cout << "H5ReconVolumeReader Error Reading the Phase IDs" << std::endl;
    free(iData);
    err = H5Gclose(scalarGid);
    err = H5Gclose(reconGid);
    err = H5Fclose(fileId);
    return err;
  }
  for(int i = 0; i < totalpoints; ++i)
  {
    phases[i] = iData[i];
  }
  free(iData);


  // Read in the Euler Angles Data
  float* fData = (float*)(malloc(totalpoints * 3 * sizeof(float)));
  err = H5Lite::readPointerDataset(scalarGid, AIM::VTK::EulerAnglesName, fData);
  if (err < 0)
  {
    std::cout << "H5ReconVolumeReader Error Reading the Euler Angles" << std::endl;
    free(fData);
    err = H5Gclose(scalarGid);
    err = H5Gclose(reconGid);
    err = H5Fclose(fileId);
    return err;
  }
  for(int i = 0; i < totalpoints; ++i)
  {
    euler1s[i] = fData[i*3];
    euler2s[i] = fData[i*3+1];
    euler3s[i] = fData[i*3+2];
  }
  free(fData);
  // Close the group as we are done with it.
  err = H5Gclose(scalarGid);

  // Open the Field Data Group
  OPEN_RECONSTRUCTION_GROUP(fieldGid, H5_FIELD_DATA_GROUP_NAME, reconGid)

  // Read the CrystalStructure Field Data
  std::vector<unsigned int> xtals;
  err = H5Lite::readVectorDataset(fieldGid, AIM::VTK::CrystalStructureName, xtals);
  if (err < 0)
  {
    std::cout << "H5ReconVolumeReader Error Reading the Crystal Structure Field Data" << std::endl;
    err = H5Gclose(fieldGid);
    err = H5Gclose(reconGid);
    err = H5Fclose(fileId);
    return err;
  }
  crystruct.resize(xtals.size());
  for (size_t i =0; i < xtals.size(); ++i)
  {
    crystruct[i] = static_cast<AIM::Reconstruction::CrystalStructure>(xtals[i]);
  }


  // Close all the HDF5 Groups and close the file
  err = H5Gclose(fieldGid);
  err = H5Gclose(reconGid);
  err = H5Fclose(fileId);

  return err;
}



  protected:
    H5VoxelReader();

  private:
    H5VoxelReader(const H5VoxelReader&); // Copy Constructor Not Implemented
    void operator=(const H5VoxelReader&); // Operator '=' Not Implemented
};

#endif /* H5RECONVOLUMEREADER_H_ */
