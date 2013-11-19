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

#ifndef FILEREADER_H_
#define FILEREADER_H_

#include <vector>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"


/*
 *
 */
class DREAM3DLib_EXPORT FileReader : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(FileReader)
    DREAM3D_STATIC_NEW_MACRO(FileReader)
    DREAM3D_TYPE_MACRO_SUPER(FileReader, AbstractFilter)

    virtual ~FileReader();

    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)

    /* This will internally call readHeader() and readFile() with appropriate
     * error checkes after each one. Subclasses should just implement the readHeader
     * and readFile methods unless special setups are needed.
     */
    virtual void execute();
#if 0
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
     * @brief This function parses 3  64 bit integer values from a comma delimited string
     * @param input
     * @param output
     * @param defaultValue The value to set if the parsing fails
     * @return Zero on Success, Negative on Error
     */
    int parseSizeT_3V(const char* input, size_t* output, size_t defaultValue);
    int parse64_3V(const char* input, int64_t* output, int64_t defaultValue);

    /**
     * @brief Reads a single line from a buffer
     * @param in The input stream
     * @param buf The buffer
     * @param bufSize The size of the buffer
     * @return
     */
    int readLine(std::istream& in, char* buf, int bufSize);



    /**
     * @brief
     * @param buf
     * @param bufSize
     * @return
     */
    size_t nonPrintables(char* buf, size_t bufSize);

    /**
     * @brief
     * @param str
     * @param tokens
     * @param delimiters
     */
    void tokenize(const QString& str, QVector<QString>& tokens, const QString& delimiters = " ");
#endif

  protected:
    FileReader();

    virtual int readHeader();

    virtual int readFile();

    virtual void dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles);

  private:
    FileReader(const FileReader&); // Copy Constructor Not Implemented
    void operator=(const FileReader&); // Operator '=' Not Implemented
};

#endif /* FILEREADER_H_ */
