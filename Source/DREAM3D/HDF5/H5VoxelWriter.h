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

#ifndef _H5RECONVOLUMEWRITER_H_
#define _H5RECONVOLUMEWRITER_H_

#include "hdf5.h"

#include <vector>
#include <string>

#include <boost/shared_array.hpp>

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/HDF5/AIM_H5VtkDataWriter.h"

#define H5_WRITE_SCALAR(type, group, scalar_name, voxel_attr)\
{\
std::vector<type> data(totalPoints);\
int32_t rank = 1;\
hsize_t dims[2] = {totalPoints};\
 for (int i = 0; i < totalPoints; ++i)\
 {\
   data[i] = voxels[i].voxel_attr;\
 }\
 err = h5writer->writeScalarData(group, data, scalar_name, numComp, rank, dims);\
 if (err < 0)\
 {\
   std::cout << "Error Writing Scalars '" << scalar_name << "' to " << group << std::endl;\
   return err;\
 }\
 }


/**
 * @class H5ReconVolumeWriter H5ReconVolumeWriter.h DREAM3D/Reconstruction/H5ReconVolumeWriter.h
 * @brief This class will write Voxel data to an HDF5 file for the Reconstruction
 * module.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jun 2, 2011
 * @version 1.0
 */

class DREAM3DLib_EXPORT H5VoxelWriter
{
  public:
    MXA_SHARED_POINTERS(H5VoxelWriter)
    MXA_TYPE_MACRO(H5VoxelWriter)
    MXA_STATIC_NEW_MACRO(H5VoxelWriter)

    virtual ~H5VoxelWriter();

    MXA_INSTANCE_STRING_PROPERTY(Filename);

    /**
     *
     */
    int writeEulerData(float* e1,
                       float* e2,
                       float* e3,
                       size_t totalPoints,
                       bool appendFile = true);

    /**
     *
     */
    int writeGrainIds(int* grain_indicies, size_t totalPoints, bool appendFile = true);

    /**
     *
     */
    int writePhaseIds(int* phases, size_t totalPoints, bool appendFile = true);

    /**
     *
     */
    int writeStructuredPoints(int volDims[3], float spacing[3], float origin[3], bool appendFile = true);

    /**
     *
     */
    int writeCrystalStructures(const std::vector<Ebsd::CrystalStructure> &crystruct, bool appendFile = true);

    /**
     *
     */
    int writePhaseTypes(const std::vector<DREAM3D::Reconstruction::PhaseType> &phaseType, bool appendFile = true);

    /**
     * @brief Writes a Complete .h5voxel file with all scalar and field data
     * @param A class implementing the "*Func" interface
     * @return Negative Value on Error
     */
    template<typename T>
    int writeData(T* m)
    {
      int err = -1;

      err = writeCrystalStructures(m->crystruct, false);
      if (err < 0) { return err; }

      err = writePhaseTypes(m->phaseType, true);
      if (err < 0) { return err; }


      err = writeEulerData(m->euler1s, m->euler2s, m->euler3s, m->totalpoints, true);
      if (err < 0) { return err; }

      err = writeGrainIds(m->grain_indicies, m->totalpoints, true);
      if (err < 0) { return err; }

      err = writePhaseIds(m->phases, m->totalpoints, true);
      if (err < 0) { return err; }

      int volDims[3] =
      { m->xpoints, m->ypoints, m->zpoints };
      float spacing[3] =
      { m->resx, m->resy, m->resz };
      float origin[3] =
      { 0.0f, 0.0f, 0.0f };
      err = writeStructuredPoints(volDims, spacing, origin, true);
      if (err < 0) { return err; }

      return err;
    }



  protected:
    H5VoxelWriter();

  private:
    H5VoxelWriter(const H5VoxelWriter&); // Copy Constructor Not Implemented
    void operator=(const H5VoxelWriter&); // Operator '=' Not Implemented
};

#endif /* _H5RECONVOLUMEWRITER_H_ */
