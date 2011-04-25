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
///////////////////////////////////////////////////////////////////////////////
// This code was partly written under US Air Force Contract FA8650-07-D-5800
///////////////////////////////////////////////////////////////////////////////

#ifndef _ANG_FILE_DIRECTORY_PATTERN_H_
#define _ANG_FILE_DIRECTORY_PATTERN_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <string>


#include "ANG/AngTypes.h"
#include "AngSetGetMacros.h"

namespace Ang {
  namespace DirectoryPatterns {
    const std::string Dot (".");
  }
}


/**
 *
 */
class TSLANG_EXPORT AngDirectoryPatterns
{
  public:
    ANG_SHARED_POINTERS(AngDirectoryPatterns)
    ANG_TYPE_MACRO(AngDirectoryPatterns)
    static Pointer New(const std::string &parentDirectory,
                       const std::string &fileprefix,
                       int32_t width);

    virtual ~AngDirectoryPatterns();

    ANG_INSTANCE_STRING_PROPERTY(ParentDirectory)
    ANG_INSTANCE_STRING_PROPERTY(Prefix)
    ANG_INSTANCE_STRING_PROPERTY(Suffix)
    ANG_INSTANCE_STRING_PROPERTY(Extension)
    ANG_INSTANCE_PROPERTY(int32_t, MaxSlice);

    std::string generateFullPathAngFileName(int slice);

    std::string generateAngFileName(int slice);

    void print(std::ostream &ostream);

protected:
    AngDirectoryPatterns();

  private:


    AngDirectoryPatterns(const AngDirectoryPatterns&);    // Copy Constructor Not Implemented
    void operator=(const AngDirectoryPatterns&);  // Operator '=' Not Implemented
};

#endif /* _ANG_FILE_DIRECTORY_PATTERN_H_ */
