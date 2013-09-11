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



#include "AngDirectoryPatterns.h"

#include <sstream>
#include <iomanip>

#include "EbsdLib/Utilities/MXADir.h"
#include "EbsdLib/Utilities/StringUtils.h"

using namespace Ang;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngDirectoryPatterns::Pointer AngDirectoryPatterns::New(const QString &parentDirectory,
    const QString &fileprefix,
    int32_t width)
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
QString AngDirectoryPatterns::generateFullPathAngFileName(int slice)
{
  std::stringstream ss;
  ss.setf(std::ios::fixed);
  ss.fill('0');

  ss << m_ParentDirectory << MXADir::Separator << m_Prefix << QSetw(m_MaxSlice) << slice << m_Suffix << Ang::DirectoryPatterns::Dot << m_Extension;
  return ss.str();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AngDirectoryPatterns::generateAngFileName(int slice)
{
  std::stringstream ss;
  ss.setf(std::ios::fixed);
  ss.fill('0');

  ss << m_Prefix << QSetw(m_MaxSlice) << slice << m_Suffix << Ang::DirectoryPatterns::Dot << m_Extension;
  return ss.str();
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AngDirectoryPatterns::extractSlice(const QString &dirName)
{
  std::string::size_type pos = m_Prefix.length();
  const char* data = dirName.data();
  // Now walk past any slice number
  while (data[pos] >= '0' && data[pos] <= '9')
  {
    ++pos;
  }

  std::string::size_type sliceLen = pos - m_Prefix.length();
  QString sliceStr = dirName.substr(m_Prefix.length(),  sliceLen);
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
int AngDirectoryPatterns::parseDocumentName(const QString &prefix,
                                            const QString &sampleAngFile)
{
  //std::cout << "AngDirectoryPatterns::parseDocumentName: " << sampleAngFile << std::endl;
  m_Extension = EbsdLib::MXADir::extension(sampleAngFile);

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

