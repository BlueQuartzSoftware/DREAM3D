/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef ABSTRACTARRAYWRITER_H_
#define ABSTRACTARRAYWRITER_H_

#include <hdf5.h>

#include <string>

#include "H5Support/H5Lite.h"
#include "DREAM3DLib/HDF5/VTKH5Constants.h"



template<typename T>
class H5DataArrayWriter
{
  public:
  virtual ~H5DataArrayWriter() {}

  static int writeArray(hid_t gid, const std::string name, size_t numTuples, int numComp, T* data)
  {
    int32_t rank = 0;
    if (numComp == 1) { rank = 1;}
    else { rank = 2;}
    hsize_t dims[2] = { numTuples, numComp};
    int err = 0;

    err = H5Lite::writePointerDataset(gid, name, rank, dims, data);
    if (err < 0)
    {
    //  std::cout << "Error writing array with name: " << name << std::endl;
    }
    err = H5Lite::writeScalarAttribute(gid, name, std::string(H5_NUMCOMPONENTS), numComp);
    if (err < 0)
    {
    //  std::cout << "Error writing dataset " << name << std::endl;
    }
    return err;
  }

  protected:
  H5DataArrayWriter(){}

  private:
  H5DataArrayWriter(const H5DataArrayWriter&); // Copy Constructor Not Implemented
    void operator=(const H5DataArrayWriter&); // Operator '=' Not Implemented
};

#endif /* ABSTRACTARRAYWRITER_H_ */
