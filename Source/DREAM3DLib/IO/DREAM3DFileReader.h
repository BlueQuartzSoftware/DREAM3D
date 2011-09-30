/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef DREAM3DFILEREADER_H_
#define DREAM3DFILEREADER_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

class DREAM3DLib_EXPORT DREAM3DFileReader
{
  public:

    DREAM3D_SHARED_POINTERS(DREAM3DFileReader)
    DREAM3D_STATIC_NEW_MACRO(DREAM3DFileReader);
    DREAM3D_TYPE_MACRO(DREAM3DFileReader);

    virtual ~DREAM3DFileReader() {};

    virtual int readFile(std::string FileName, std::vector<int> &data, int &nx, int &ny, int &nz)
    {
      return -1;
    }

    /**
     * @brief
     * @param str
     * @param tokens
     * @param delimiters
     */
    void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ")
    {
      // Skip delimiters at beginning.
      std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);

      // Find first "non-delimiter".
      std::string::size_type pos = str.find_first_of(delimiters, lastPos);

      while (std::string::npos != pos || std::string::npos != lastPos)
      {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);

        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
      }
    }

  protected:
    DREAM3DFileReader(){};

  private:
    DREAM3DFileReader(const DREAM3DFileReader&); // Copy Constructor Not Implemented
    void operator=(const DREAM3DFileReader&); // Operator '=' Not Implemented
};


#endif /* DREAM3DFILEREADER_H_ */
