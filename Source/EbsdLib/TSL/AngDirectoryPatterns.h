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




#ifndef _EBSD_FILE_DIRECTORY_PATTERN_H_
#define _EBSD_FILE_DIRECTORY_PATTERN_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <string>


#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdSetGetMacros.h"

namespace Ang {
  namespace DirectoryPatterns {
    const std::string Dot (".");
  }
}


/**
 * @class AngDirectoryPatterns AngDirectoryPatterns.h EbsdLib/TSL/AngDirectoryPatterns.h
 * @brief  This class serves as a way to parse out the list of ebsd data files from
 * a directory.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Aug 8, 2011
 * @version 1.0
 */
class EbsdLib_EXPORT AngDirectoryPatterns
{
  public:
    EBSD_SHARED_POINTERS(AngDirectoryPatterns)
    EBSD_TYPE_MACRO(AngDirectoryPatterns)

    /**
     * @brief Creates a new instance returning a boost::Shared_Ptr<> wrapped object
     * @param  parentDirectory The parent directory path
     * @param fileprefix The prefix common to all the ebsd files
     * @param width The maximum number of digits that make up the index value on each
     * of the data files.
     */
    static Pointer New(const std::string &parentDirectory,
                       const std::string &fileprefix,
                       int32_t width);

    virtual ~AngDirectoryPatterns();

    /**
     * @brief Sets/Gets the ParentDirectory instance variable.
     */
    EBSD_INSTANCE_STRING_PROPERTY(ParentDirectory)

    /**
     * @brief Sets/Gets the Prefix instance variable.
     */
    EBSD_INSTANCE_STRING_PROPERTY(Prefix)

    /**
     * @brief Sets/Gets the Suffix instance variable.
     */
    EBSD_INSTANCE_STRING_PROPERTY(Suffix)

    /**
     * @brief Sets/Gets the Extension instance variable.
     */
    EBSD_INSTANCE_STRING_PROPERTY(Extension)

    /**
     * @brief Sets/Gets the Maximum slice value instance variable.
     */
    EBSD_INSTANCE_PROPERTY(int32_t, MaxSlice)

    /**
     * @brief Creates an absolution path to a specific data file indicated by its index
     * @param slice The index to return a path for
     */
    std::string generateFullPathAngFileName(int slice);

    /**
     * @brief Creates a file name based on a slice index
     * @param slice The slice to generate a file name for.
     */
    std::string generateAngFileName(int slice);

    /**
     * @brief Prints out debugging info about this class
     * @param ostream The std::stream to print to
     */
    void print(std::ostream &ostream);

  protected:
    AngDirectoryPatterns();

  private:
    AngDirectoryPatterns(const AngDirectoryPatterns&);    // Copy Constructor Not Implemented
    void operator=(const AngDirectoryPatterns&);  // Operator '=' Not Implemented
};

#endif /* _EBSD_FILE_DIRECTORY_PATTERN_H_ */
