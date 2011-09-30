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

#ifndef VTKRECTILINEARGRIDREADER_H_
#define VTKRECTILINEARGRIDREADER_H_

#include <string.h> // needed for the ::memcpy function below
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AIMArray.hpp"
#include "DREAM3DLib/IO/DREAM3DFileReader.h"
#include "VTKFileReader.h"

class DREAM3DLib_EXPORT GrainIdVtkRectilinearGridReader: public VTKFileReader,
                                                         public DREAM3DFileReader
{
  public:
    DREAM3D_SHARED_POINTERS(GrainIdVtkRectilinearGridReader);
    DREAM3D_TYPE_MACRO(VTKRectilinearGridFileReader)
    DREAM3D_STATIC_NEW_MACRO(GrainIdVtkRectilinearGridReader);
    DREAM3D_STATIC_NEW_SUPERCLASS(DREAM3DFileReader, GrainIdVtkRectilinearGridReader);

    virtual ~GrainIdVtkRectilinearGridReader();

    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdScalarName)
    DREAM3D_INSTANCE_PROPERTY(AIMArray<int>::Pointer, GrainIds);

    int parseCoordinateLine(const char* input, int &value);

    /**
     * @brief Reads the VTK header and sets the values that are described in the header
     * @return Error Condition. Negative is Error.
     */
    int readHeader();

    int readFile();

    virtual int readFile(std::string FileName, std::vector<int> &data, int &nx, int &ny, int &nz)
    {
      setInputFileName(FileName);
      int err = readFile();
      if (err < 0)
      {
        return err;
      }
      getDims(nx, ny, nz);
      int totalpoints = nx * ny * nz;
      data.resize(totalpoints);
      // Copy the data from the AIMArray into the std::vector<int>
      ::memcpy( &(data.front()), getGrainIds()->GetPointer(0), totalpoints * sizeof(int));
      return err;
    }

  protected:
    GrainIdVtkRectilinearGridReader();

  private:

    GrainIdVtkRectilinearGridReader(const GrainIdVtkRectilinearGridReader&); // Copy Constructor Not Implemented
    void operator=(const GrainIdVtkRectilinearGridReader&); // Operator '=' Not Implemented
};

#endif /* VTKRECTILINEARGRIDREADER_H_ */
