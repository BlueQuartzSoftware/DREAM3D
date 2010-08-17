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
#include "AngDirectoryPatterns.h"

#include <MXA/Utilities/MXAFileSystemPath.h>
#include <MXA/Utilities/StringUtils.h>

#include <sstream>
#include <iomanip>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngDirectoryPatterns::Pointer AngDirectoryPatterns::New(const std::string &parentDirectory,
    const std::string &fileprefix,
    int32 width)
{
  Pointer sharedPtr (new AngDirectoryPatterns);
  sharedPtr->setParentDirectory(parentDirectory);
  sharedPtr->setPrefix(fileprefix);
  sharedPtr->setExtension("ang");
  sharedPtr->setMaxSlice(width);
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngDirectoryPatterns::AngDirectoryPatterns()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngDirectoryPatterns::~AngDirectoryPatterns()
{
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string AngDirectoryPatterns::generateFullPathAngFileName(int slice)
{
  std::stringstream ss;
  ss.setf(std::ios::fixed);
  ss.fill('0');

  ss << m_ParentDirectory << MXAFileSystemPath::Separator << m_Prefix << std::setw(m_MaxSlice) << slice << m_Suffix << Ang::DirectoryPatterns::Dot << m_Extension;
  return ss.str();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string AngDirectoryPatterns::generateAngFileName(int slice)
{
  std::stringstream ss;
  ss.setf(std::ios::fixed);
  ss.fill('0');

  ss << m_Prefix << std::setw(m_MaxSlice) << slice << m_Suffix << Ang::DirectoryPatterns::Dot << m_Extension;
  return ss.str();
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AngDirectoryPatterns::extractSlice(const std::string &dirName)
{
  std::string::size_type pos = m_Prefix.length();
  const char* data = dirName.data();
  // Now walk past any slice number
  while (data[pos] >= '0' && data[pos] <= '9')
  {
    ++pos;
  }

  std::string::size_type sliceLen = pos - m_Prefix.length();
  std::string sliceStr = dirName.substr(m_Prefix.length(),  sliceLen);
  int sliceVal = -1;
  bool ok = StringUtils::stringToNum<int>(sliceVal, sliceStr);
  if (!ok) {
    sliceVal = -1;
  }

 return sliceVal;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AngDirectoryPatterns::parseDocumentName(const std::string &prefix,
                                            const std::string &sampleAngFile)
{
  //std::cout << "AngDirectoryPatterns::parseDocumentName: " << sampleAngFile << std::endl;
  m_Extension = MXAFileSystemPath::extension(sampleAngFile);

  std::string::size_type m_PrefixPos = sampleAngFile.find(prefix);
  if (m_PrefixPos != std::string::npos  && m_PrefixPos == 0)
  {
    std::string::size_type pos = prefix.size() - m_PrefixPos;
    // Start looking for numeric characters in case the guessed prefix is not correct
    const char* data = sampleAngFile.data();
    while (data[pos] < '0' || data[pos] > '9' )
    {
      ++pos;
    }
    m_Prefix = sampleAngFile.substr(m_PrefixPos, pos);

    // Now walk past any slice number
    while (data[pos] >= '0' && data[pos] <= '9')
    {
      ++pos;
    }

    // Now find the '.' character
    std::string::size_type dotPos = sampleAngFile.find_first_of('.', pos);
    m_Suffix = sampleAngFile.substr(pos, dotPos - pos);
  }
  else
  {
    return -1;
  }
  return 1;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngDirectoryPatterns::print(std::ostream &ostream)
{
  ostream << "Prefix: " << getPrefix() << std::endl;
  ostream << "Suffix: " << getSuffix() << std::endl;
  ostream << "Extension: " << getExtension() << std::endl;
  ostream << "Width: " << getMaxSlice() << std::endl;
}

