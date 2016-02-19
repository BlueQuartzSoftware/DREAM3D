/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _emmpminputparser_h_
#define _emmpminputparser_h_

#error

#include <string>

#include "EMMPMLib/EMMPMLib.h"
#include "EMMPMLib/Core/EMMPM_Data.h"

/**
 * @class EMMPMInputParser EMMPMInputParser.h emmpm/public/EMMPMInputParser.h
 * @brief This class uses the TCLAP project to parse the command line arguments
 * when using the command line version of the emmpm executable.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Mar 31, 2011
 * @version 1.0
 */
class EMMPMLib_EXPORT EMMPMInputParser
{
  public:
    EMMPMInputParser();
    virtual ~EMMPMInputParser();

    /**
     * @brief
     * @param argc
     * @param argv
     * @param inputs
     * @return
     */
    int parseCLIArguments(int argc, char* argv[], EMMPM_Data* inputs);

    /**
     * @brief This will parse the pixel coordinates for each class that is to be initialized
     * @param coords The raw string the user entered on the command line
     * @param inputs The EMMPM_Data structure to store the results
     * @return
     */
    int parseInitCoords(const std::string& coords, EMMPM_Data* inputs);

    /**
     * @brief Parse the Gray Scale Mapping Table as input by the user
     * @param values The raw string the user entered on the command line
     * @param inputs The EMMPM_Data structure to store the results
     * @return
     */
    int parseGrayTable(const std::string& values, EMMPM_Data* inputs);

    /**
     * @brief Parse the Mean and Variance values from the command line
     * @param values The raw string the user entered on the command line
     * @param inputs The EMMPM_Data structure to store the results
     * @return
     */
    int parseMuSigmaValues(const std::string& values, EMMPM_Data* inputs);

    /**
     * @brief Copys the std::string contents into a newly malloc'ed char array which
     * the programmer will need to free when they are finished with it.
     * @param fname The filename to copy
     */
    char* copyFilenameToNewCharBuffer( const std::string& fname);

  private:
    EMMPMInputParser(const EMMPMInputParser&); // Copy Constructor Not Implemented
    void operator=(const EMMPMInputParser&); // Operator '=' Not Implemented
};

#endif /* _EMMPMINPUTPARSER_H_ */
