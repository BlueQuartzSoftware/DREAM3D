
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//


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
