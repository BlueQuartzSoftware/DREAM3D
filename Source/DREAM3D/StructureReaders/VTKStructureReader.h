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

#ifndef GRAINGENERATORVTKREADER_H_
#define GRAINGENERATORVTKREADER_H_



#include <MXA/Common/MXASetGetMacros.h>
#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/VTKUtils/VTKFileReader.h"
#include "DREAM3D/StructureReaders/AbstractStructureReader.h"

class GrainGeneratorFunc;

/**
 * @class VTKStructureReader VTKStructureReader.h AIM/Common/StructureReaders/VTKStructureReader.h
 * @brief This class inherits both from VTKFileReader and AbstractStructureReader
 * and provides the necessary methods to read an existing grain structure from a
 * file on disk. The layout of the VTK legacy file is very strict and must match
 * one of the following forms:
 *
 @code
   1: # vtk DataFile Version 2.0
   2: NRL TiBeta Stack
   3: BINARY
   4: DATASET STRUCTURED_POINTS
   5: DIMENSIONS 1670 770 201
   6: ORIGIN 0.00 0.00 0.00
   7: SPACING 0.665 0.665 1.48
   8: POINT_DATA 258465900
   9:
   10: SCALARS GrainID int 1
   11: LOOKUP_TABLE default

   OR

   1: # vtk DataFile Version 2.0
   2: data set from FFT2dx_GB
   3: ASCII
   4: DATASET STRUCTURED_POINTS
   5: DIMENSIONS 189 201 100
   6: ORIGIN 0.0 0.0 0.0
   7: SPACING 0.25 0.25 0.25
   8: POINT_DATA 3798900
   9:
   10: SCALARS GrainID int 1
   11: LOOKUP_TABLE default
 @endcode
  In addition it must also have 2 scalar arrays: <b>GrainID</b and <b>PhaseId</b>
  which BOTH must be of the <b>int</b> type.
 *
 * @author Michael A. Jackson for BlueQuartz Software
 * @date May 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT VTKStructureReader : public VTKFileReader, public AbstractStructureReader
{
  public:
    MXA_SHARED_POINTERS(VTKStructureReader)
    MXA_STATIC_NEW_MACRO(VTKStructureReader)
    MXA_TYPE_MACRO(VTKStructureReader)

    virtual ~VTKStructureReader();

    /* These can be used to tune the reader to read the proper Scalars */
    MXA_INSTANCE_STRING_PROPERTY(GrainIdScalarName)
    MXA_INSTANCE_STRING_PROPERTY(PhaseIdScalarName)

    int readStructure(GrainGeneratorFunc* m);

  protected:
    VTKStructureReader();


  private:

    VTKStructureReader(const VTKStructureReader&); // Copy Constructor Not Implemented
    void operator=(const VTKStructureReader&); // Operator '=' Not Implemented
};

#endif /* GRAINGENERATORVTKREADER_H_ */
