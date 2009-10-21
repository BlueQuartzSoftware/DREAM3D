/*
 * AngDirectoryPatterns.cpp
 *
 *  Created on: Oct 16, 2009
 *      Author: mjackson
 */

#include "AngDirectoryPatterns.h"
#include <sstream>
#include <iomanip>

#include <MXA/Utilities/MXAFileSystemPath.h>
#include <MXA/Utilities/StringUtils.h>

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

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string AngDirectoryPatterns::generateAngDirectory(int slice)
{
  std::stringstream ss;
  ss << m_Prefix << slice << m_Suffix << Ang::DirectoryPatterns::Dot << m_Extension << Ang::DirectoryPatterns::_Files;
  return ss.str();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string AngDirectoryPatterns::generateAngDirectory(const std::string &slice)
{
  std::stringstream ss;
  ss << m_Prefix << slice << m_Suffix << Ang::DirectoryPatterns::Dot << m_Extension << Ang::DirectoryPatterns::_Files;
  return ss.str();
}
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string AngDirectoryPatterns::generateFullPathAngFileName(int slice, int numberWidth)
{
  std::stringstream ss;
  ss.setf(std::ios::fixed);
  ss.fill('0');
  int32 width = numberWidth;
  ss << m_ParentDirectory << MXAFileSystemPath::Separator << m_Prefix << std::setw(width) << slice << m_Suffix << Ang::DirectoryPatterns::Dot << m_Extension;
  return ss.str();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string AngDirectoryPatterns::generateAngFileName(int slice, int numberWidth)
{
  std::stringstream ss;
  ss.setf(std::ios::fixed);
  ss.fill('0');
  int32 width = numberWidth;
  ss << m_Prefix << std::setw(width) << slice << m_Suffix << Ang::DirectoryPatterns::Dot << m_Extension;
  return ss.str();
}

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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngDirectoryPatterns::print(std::ostream &ostream)
{
  ostream << "Prefix: " << getPrefix() << std::endl;
  ostream << "Suffix: " << getSuffix() << std::endl;
  ostream << "Extension: " << getExtension() << std::endl;
}

