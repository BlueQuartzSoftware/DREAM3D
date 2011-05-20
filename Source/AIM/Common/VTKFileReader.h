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

#ifndef VTKFILEREADER_H_
#define VTKFILEREADER_H_

#include <fstream>


#include <MXA/Common/MXASetGetMacros.h>
#include "AIM/Common/AIMCommonConfiguration.h"

/**
 * @class VTKFileReader VTKFileReader.h PathToHeader/VTKFileReader.h
 * @brief This class holds some basic methods to read various items from a legacy
 * VTK file.
 *
 * <b>Legacy VTK File Header </b>
 * @code
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
 * @endcode
 *
 *
 *
 * @author Michael A. Jackson for BlueQuartz Software
 * @date May 19, 2011
 * @version 1.0
 */
class AIMCOMMON_EXPORT VTKFileReader
{
  public:
    MXA_SHARED_POINTERS(VTKFileReader)
    MXA_TYPE_MACRO(VTKFileReader)
    MXA_STATIC_NEW_MACRO(VTKFileReader)

    virtual ~VTKFileReader();

    MXA_INSTANCE_STRING_PROPERTY(InputFileName)
    MXA_INSTANCE_STRING_PROPERTY(Comment)
    MXA_INSTANCE_STRING_PROPERTY(DatasetType)
    MXA_INSTANCE_VEC3_PROPERTY(int, Dims)
    MXA_INSTANCE_VEC3_PROPERTY(float, Scaling)
    MXA_INSTANCE_VEC3_PROPERTY(float, Origin)
    MXA_INSTANCE_PROPERTY(bool, FileIsBinary)

    /**
     * @brief Reads the VTK header and sets the values that are described in the header
     * @return Error Condition. Negative is Error.
     */
    int readHeader();

    /**
     * @brief This function parses 3 floating point values from a comma delimited string
     * @param input
     * @param output
     * @param defaultValue The value to set if the parsing fails
     * @return Zero on Success, Negative on Error
     */
    int parseFloat3V(const char* input, float* output, float defaultValue);

    /**
     * @brief This function parses 3 integer values from a comma delimited string
     * @param input
     * @param output
     * @param defaultValue The value to set if the parsing fails
     * @return Zero on Success, Negative on Error
     */
    int parseInt3V(const char* input, int* output, int defaultValue);

    /**
     * @brief Reads a single line from a buffer
     * @param in The input stream
     * @param buf The buffer
     * @param bufSize The size of the buffer
     * @return
     */
    int readLine(std::istream &in, char* buf, int bufSize);

    /**
     * @brief Parses the byte size from a data set declaration line
     * @param text
     * @return
     */
    size_t parseByteSize(char text[256]);

    int skipVolume(std::ifstream &inStream, int byteSize, int xDim, int yDim, int zDim);

  protected:
    VTKFileReader();


  private:
    VTKFileReader(const VTKFileReader&); // Copy Constructor Not Implemented
    void operator=(const VTKFileReader&); // Operator '=' Not Implemented
};

#endif /* VTKFILEREADER_H_ */
