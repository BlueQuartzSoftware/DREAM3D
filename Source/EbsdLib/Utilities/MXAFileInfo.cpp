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

#include "MXAFileInfo.h"

#define MXA_FILESYSTEM_BASE_CLASS MXAFileInfo

#include "MXAFileSystemPath.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint64_t MXA_FILESYSTEM_BASE_CLASS::fileSize(const std::string &path)
{
  int error;
  MXA_STATBUF st;
  error = MXA_STAT(path.c_str(), &st);
  if (!error && (st.st_mode & S_IFMT) == S_IFREG)
  {
    return (uint64_t)(st.st_size);
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXA_FILESYSTEM_BASE_CLASS::extension(const std::string &fsPath)
{
  std::string::size_type pos = fsPath.find_last_of('.');
  // No '.' characters appeared in the path at all
  if(std::string::npos == pos)
  {
    return std::string();
  }
  // Look for a "Hidden" file  .som ./.some ../.something.something
  if (pos == 0
      || fsPath[pos-1] == MXA_FILESYSTEM_BASE_CLASS::Separator)
  {
    return std::string();  // Return empty string, there is no extension
  }

  std::string::size_type slashpos = fsPath.find_last_of(MXA_FILESYSTEM_BASE_CLASS::Separator);
  // Check for just a plain filename, ie a path with NO directory delimiters in the string
  if (std::string::npos == slashpos && std::string::npos != pos)
  {
    return fsPath.substr(pos + 1);
  }

  if (pos > slashpos)
  {
    return fsPath.substr(pos + 1);
  }

  return std::string();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXA_FILESYSTEM_BASE_CLASS::filename(const std::string &fsPath)
{

  std::string::size_type slashPos = fsPath.find_last_of(MXA_FILESYSTEM_BASE_CLASS::Separator);
  if (slashPos == fsPath.size() - 1)
  {
    return MXA_FILESYSTEM_BASE_CLASS::filename(fsPath.substr(0, fsPath.size() - 1) );
  }

  std::string fn = fsPath.substr(slashPos + 1, fsPath.size() - slashPos);
  if (fn.at(fn.size()-1) == '.')
  {
    return fn.substr(0, fn.size() - 1);
  }
  return fn;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXA_FILESYSTEM_BASE_CLASS::fileNameWithOutExtension(const std::string &fsPath)
{
  std::string fname = MXA_FILESYSTEM_BASE_CLASS::filename(fsPath);
  std::string ext = MXA_FILESYSTEM_BASE_CLASS::extension(fsPath);
  std::string::size_type pos = fname.find_last_of(ext);
  if (pos != std::string::npos)
  {
    fname = fname.substr(0, fname.size() - ext.size() - 1);
  }

//  std::string parentPath = MXA_FILESYSTEM_BASE_CLASS::parentPath(fsPath);
//  if (parentPath.size() > 0)
//  {
//    return parentPath + MXA_FILESYSTEM_BASE_CLASS::getSeparator() + fname;
//  }
//  else
//  {
    return fname;
//  }
}
