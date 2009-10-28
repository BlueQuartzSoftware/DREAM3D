///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ANG_FILE_DIRECTORY_PATTERN_H_
#define _ANG_FILE_DIRECTORY_PATTERN_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/Common/MXATypes.h>
#include <string>

namespace Ang {
  namespace DirectoryPatterns {
    const std::string Dot (".");
  }
}


/**
 *
 */
class AngDirectoryPatterns
{
  public:
    MXA_SHARED_POINTERS(AngDirectoryPatterns)
    MXA_TYPE_MACRO(AngDirectoryPatterns)
    static Pointer New(const std::string &parentDirectory,
                       const std::string &fileprefix,
                       int32 width);

    virtual ~AngDirectoryPatterns();

    MXA_INSTANCE_STRING_PROPERTY(ParentDirectory, m_ParentDirectory)
    MXA_INSTANCE_STRING_PROPERTY(Prefix, m_Prefix)
    MXA_INSTANCE_STRING_PROPERTY(Suffix, m_Suffix)
    MXA_INSTANCE_STRING_PROPERTY(Extension, m_Extension)
    MXA_INSTANCE_PROPERTY_m(int , MaxSlice);

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
