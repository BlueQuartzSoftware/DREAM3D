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

#include "MXA/MXA.h"
#include "MXA/Common/MXASetGetMacros.h"

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DataContainer.h"
#include "DREAM3DLib/HDF5/H5DataWriter.h"

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

    MXA_INSTANCE_STRING_PROPERTY(FileName);

    /**
     *
     */
    int writeEulerData(float* ea,
                       int64_t totalPoints,
                       bool appendFile = true);

    /**
     *
     */
    int writeGrainIds(int* grain_indicies, int64_t totalPoints, bool appendFile = true);

    /**
     *
     */
    int writePhaseIds(int* phases, int64_t totalPoints, bool appendFile);

    /**
     *
     */
    int writeStructuredPoints(int64_t volDims[3], float spacing[3], float origin[3], bool appendFile = true);

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
      int64_t totalPoints = m->totalPoints();
      int err = -1;
      GET_NAMED_ARRAY_SIZE_CHK_NOMSG_RET(m, Cell, DREAM3D::CellData::GrainIds, Int32ArrayType, int32_t, totalPoints, grain_indicies);
      GET_NAMED_ARRAY_SIZE_CHK_NOMSG_RET(m, Cell, DREAM3D::CellData::Phases, Int32ArrayType, int32_t, totalPoints, phases);
      GET_NAMED_ARRAY_SIZE_CHK_NOMSG_RET(m, Cell, DREAM3D::CellData::EulerAngles, FloatArrayType, float, 3*totalPoints, eulerangles);


      err = writeCrystalStructures(m->crystruct, false);
      if (err < 0) { return err; }

      err = writePhaseTypes(m->phaseType, true);
      if (err < 0) { return err; }


      err = writeEulerData(eulerangles, totalPoints, true);
      if (err < 0) { return err; }

      err = writeGrainIds(grain_indicies, totalPoints, true);
      if (err < 0) { return err; }

      err = writePhaseIds(phases, totalPoints, true);
      if (err < 0) { return err; }

      int64_t volDims[3] =
      { m->getXPoints(), m->getYPoints(), m->getZPoints() };
      float spacing[3] =
      { m->getXRes(), m->getYRes(), m->getZRes() };
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
