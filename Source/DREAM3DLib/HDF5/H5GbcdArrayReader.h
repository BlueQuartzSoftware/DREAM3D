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

#ifndef H5GbcdArrayReader_HPP_
#define H5GbcdArrayReader_HPP_

#include <hdf5.h>

#include <string>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/IDataArray.h"

/**
 * @class H5GbcdArrayReader H5GbcdArrayReader.h DREAM3DLib/HDF5/H5GbcdArrayReader.h
 * @brief This class handles reading DataArray<T> objects from an HDF5 file
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Feb 22, 2012
 * @version 1.0
 */
class DREAM3DLib_EXPORT H5GbcdArrayReader
{
  public:
    virtual ~H5GbcdArrayReader();


    /**
     *
     * @param gid
     * @param name
     * @return
     */
    static IDataArray::Pointer readIDataArray(hid_t gid, const QString &name, bool preflightOnly = false);

  protected:
    H5GbcdArrayReader();

  private:
    H5GbcdArrayReader(const H5GbcdArrayReader&); // Copy Constructor Not Implemented
    void operator=(const H5GbcdArrayReader&); // Operator '=' Not Implemented

};

#endif /* H5GbcdArrayReader_HPP_ */
