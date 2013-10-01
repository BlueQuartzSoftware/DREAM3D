/* ============================================================================
 * Copyright (c) 2009, Michael A. Jackson (BlueQuartz Software)
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* Note that some of this implementation was inspired by the Qt source code,
 * in particular the QDir and QFileEngine source codes. The idea is to keep the
 * API the same between my implementation and the Qt Implementation so that
 * switching between them is easy.
 *
 */


#include <iostream>
#include <vector>

#include <stdlib.h>
#include <ctype.h>
#include <string.h> // Needed for memset

#if defined (CMP_HAVE_UNISTD_H)
#include <unistd.h>
#endif

#if defined (_MSC_VER)
#include <direct.h>
#include "MXADirent.h"
#define UNLINK _unlink
#define MXA_PATH_MAX MAX_PATH
#define MXA_GET_CWD _getcwd
#else
#define UNLINK ::unlink
#include <dirent.h>
#define MXA_PATH_MAX PATH_MAX
#define MXA_GET_CWD ::getcwd
#endif

#if defined (CMP_HAVE_SYS_STAT_H)
#include <sys/stat.h>

#if defined (_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#define MXA_STATBUF    struct _stati64   // non-ANSI defs
#define MXA_STATBUF4TSTAT  struct _stati64   // non-ANSI defs
#define MXA_STAT     _stati64
#define MXA_FSTAT    _fstati64

#define MXA_STAT_REG   _S_IFREG
#define MXA_STAT_DIR   _S_IFDIR
#define MXA_STAT_MASK    _S_IFMT
#if defined(_S_IFLNK)
#  define MXA_STAT_LNK   _S_IFLNK
#endif

#elif defined (__APPLE__)

#define MXA_STATBUF    struct stat
#define MXA_STATBUF4TSTAT  struct stat
#define MXA_STAT     stat
#define MXA_FSTAT    fstat

#define MXA_STAT_REG   S_IFREG
#define MXA_STAT_DIR   S_IFDIR
#define MXA_STAT_MASK    S_IFMT
#define MXA_STAT_LNK   S_IFLNK

#else
#define MXA_STATBUF    struct stat
#define MXA_STATBUF4TSTAT  struct stat
#define MXA_STAT     stat
#define MXA_FSTAT    fstat

#define MXA_STAT_REG   S_IFREG
#define MXA_STAT_DIR   S_IFDIR
#define MXA_STAT_MASK    S_IFMT
#define MXA_STAT_LNK   S_IFLNK
#endif

#endif /* defined (CMP_HAVE_SYS_STATS_H) */



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXA_FILESYSTEM_BASE_CLASS::MXA_FILESYSTEM_BASE_CLASS()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXA_FILESYSTEM_BASE_CLASS::~MXA_FILESYSTEM_BASE_CLASS()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXA_FILESYSTEM_BASE_CLASS::getSeparator()
{
#if defined (WIN32)
      return "\\";
#else
      return "/";
#endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXA_FILESYSTEM_BASE_CLASS::isDirectory(const std::string &fsPath)
{
#if defined (WIN32)
  bool existed = false;
  return MXA_FILESYSTEM_BASE_CLASS::isDirPath(fsPath, &existed);
#else
  int error;
  MXA_STATBUF st;
  error = MXA_STAT(fsPath.c_str(), &st);
  if (!error && (st.st_mode & S_IFMT) == S_IFDIR)
  {
    return true;
  }
  return false;
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXA_FILESYSTEM_BASE_CLASS::isFile(const std::string &fsPath)
{
  int error;
  MXA_STATBUF st;
  error = MXA_STAT(fsPath.c_str(), &st);
  if (!error && (st.st_mode & S_IFMT) == S_IFREG)
  {
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXA_FILESYSTEM_BASE_CLASS::isRelativePath(const std::string &path)
{
#if defined (WIN32)
  if (path.length() > 2 && isalpha(path[0]) == false && path[1] != ':' && path[2] != '\\') {return true;}
#else
  if (path.length() > 0 && path[0] != '/') { return true; }
#endif
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXA_FILESYSTEM_BASE_CLASS::isAbsolutePath(const std::string &path)
{
#if defined (WIN32)
  if (path.length() > 2 && isalpha(path[0]) != 0 && path[1] == ':' && path[2] == '\\') {return true;}
  if (path.length() > 1 && path[0] == '\\' && path[1] == '\\') { return true;}
#else
  if (path.length() > 0 && path[0] == '/') { return true; }
#endif
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXA_FILESYSTEM_BASE_CLASS::currentPath()
{
  std::string currentPath;
  MXA_STATBUF st;
  if (0 == MXA_STAT(".", &st) )
  {
    char currentName[MXA_PATH_MAX+1];
    char* result = NULL;
    ::memset(&currentName[0], 0, MXA_PATH_MAX + 1); // Clear everything to zeros.
    result = MXA_GET_CWD(currentName, MXA_PATH_MAX);
    if (NULL == result)
    {
      std::cout << "Error: MXA_FILESYSTEM_BASE_CLASS::currentPath result was NULL." << std::endl;
    }
    else
    {
      currentPath = std::string(currentName);
    }

  }
  else
  {
    std::cout << "Error: MXA_FILESYSTEM_BASE_CLASS::currentPath stat function failed." << std::endl;
  }
#if defined(WIN32)
  currentPath = MXA_FILESYSTEM_BASE_CLASS::cleanPath(currentPath);
#endif

  return currentPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXA_FILESYSTEM_BASE_CLASS::absolutePath(const std::string &path)
{
  std::string abspath;
  if ( true == MXA_FILESYSTEM_BASE_CLASS::isAbsolutePath(path))
  { return path; }

  abspath = MXA_FILESYSTEM_BASE_CLASS::currentPath();
  if(abspath[abspath.length()-1] != MXA_FILESYSTEM_BASE_CLASS::Separator)
  {
    abspath = abspath + MXA_FILESYSTEM_BASE_CLASS::Separator;
  }
  abspath.append(path);
  abspath = MXA_FILESYSTEM_BASE_CLASS::cleanPath(abspath);
  return abspath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXA_FILESYSTEM_BASE_CLASS::parentPath(const std::string &path)
{
  std::string curAbsPath = MXA_FILESYSTEM_BASE_CLASS::absolutePath(path);
  curAbsPath = MXA_FILESYSTEM_BASE_CLASS::fromNativeSeparators(curAbsPath);
  std::string::size_type nextToLastSlashPos = 0;
  std::string::size_type lastSlashPos = curAbsPath.find_last_of(MXA_FILESYSTEM_BASE_CLASS::UnixSeparator);
// Remove trailing '/' if found
  if (lastSlashPos == curAbsPath.length() - 1)
  {
    curAbsPath = curAbsPath.substr(0, curAbsPath.length()-2);
    lastSlashPos = curAbsPath.find_last_of(MXA_FILESYSTEM_BASE_CLASS::Separator);
  }

  if (lastSlashPos > 0)
  {
    nextToLastSlashPos = curAbsPath.find_last_of(MXA_FILESYSTEM_BASE_CLASS::UnixSeparator, lastSlashPos - 1);
  }

  if (nextToLastSlashPos == std::string::npos) // Only 1 slash found, return the root directory
  {
#if defined (WIN32)
    curAbsPath = curAbsPath.substr(0, 3);
    return MXA_FILESYSTEM_BASE_CLASS::toNativeSeparators(curAbsPath);
#else
    return curAbsPath.substr(0, 1);
#endif
  }

  curAbsPath = curAbsPath.substr(0, lastSlashPos);
#if defined (WIN32)
  curAbsPath = MXA_FILESYSTEM_BASE_CLASS::toNativeSeparators(curAbsPath);
#endif
  return curAbsPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXA_FILESYSTEM_BASE_CLASS::exists(const std::string &fsPath)
{
  int error;
  std::string dirName(fsPath);
  // Both windows and OS X both don't like trailing slashes so just get rid of them
  // for all Operating Systems.
  if (dirName[dirName.length() - 1] == MXA_FILESYSTEM_BASE_CLASS::Separator) {
        dirName = dirName.substr(0, dirName.length() - 1);
  }
  MXA_STATBUF st;
  error = MXA_STAT(dirName.c_str(), &st);
  return (error == 0);
}




#if defined (WIN32)
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXA_FILESYSTEM_BASE_CLASS::isDirPath(const std::string &folderPath, bool *existed)
{
    std::string fsPath = folderPath;
    if (fsPath.length() == 2 &&fsPath.at(1) == ':')
        fsPath += MXA_FILESYSTEM_BASE_CLASS::Separator;

    DWORD fileAttrib = INVALID_FILE_ATTRIBUTES;
    fileAttrib = ::GetFileAttributesA(fsPath.c_str() );

    if (existed)
        *existed = fileAttrib != INVALID_FILE_ATTRIBUTES;

    if (fileAttrib == INVALID_FILE_ATTRIBUTES)
        return false;

    return (fileAttrib & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
}
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXA_FILESYSTEM_BASE_CLASS::fromNativeSeparators(const std::string  &fsPath)
{
  std::string path(fsPath);
#if defined (WIN32)
  for (int i=0; i<(int)path.length(); i++) {
      if (path[i] ==  MXA_FILESYSTEM_BASE_CLASS::Separator )
        path[i] =  MXA_FILESYSTEM_BASE_CLASS::UnixSeparator;
  }
#endif
  return path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXA_FILESYSTEM_BASE_CLASS::toNativeSeparators(const std::string &fsPath)
{
    std::string path(fsPath);
#if defined (WIN32)
    for (int i=0; i<(int)path.length(); i++) {
        if (path[i] ==  MXA_FILESYSTEM_BASE_CLASS::UnixSeparator )
          path[i] =  MXA_FILESYSTEM_BASE_CLASS::Separator;
    }
#endif
    return path;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXA_FILESYSTEM_BASE_CLASS::cleanPath(const std::string &fsPath)
{
    if (fsPath.length() == 0)
        return fsPath;
      std::string path(fsPath);
     char slash = '/';
     char dot = '.';
     if (MXA_FILESYSTEM_BASE_CLASS::Separator != MXA_FILESYSTEM_BASE_CLASS::UnixSeparator)
     {
       path = fromNativeSeparators(path);
     }

     // Peel off any trailing slash
     if (path[path.length() -1 ] == slash)
     {
       path = path.substr(0, path.length() -1);
     }

     std::vector<std::string> stk;
     std::string::size_type pos = 0;
     std::string::size_type pos1 = 0;

     // Check for UNC style paths first

     pos = path.find_first_of(slash, pos);
     pos1 = path.find_first_of(slash, pos + 1);
   #if defined (WIN32)
     if (pos == 0 && pos1 == 1)
     {
       pos1 = path.find_first_of(slash, pos1 + 1);
     } else
   #endif
     if (pos != 0)
     {
       stk.push_back(path.substr(0, pos));
     }


     while (pos1 != std::string::npos)
     {
       if (pos1 - pos == 3 && path[pos+1] == dot && path[pos+2] == dot)
       {
       //  std::cout << "Popping back element" << std::endl;
         if (stk.size() > 0) {
           stk.pop_back();
         }
       }
       else if (pos1 - pos == 2 && path[pos+1] == dot )
       {

       }
       else if (pos + 1 == pos1) {

       }
       else {
         stk.push_back(path.substr(pos, pos1-pos));
       }
       pos = pos1;
       pos1 = path.find_first_of(slash, pos + 1);
       if (pos1 == std::string::npos)
       {
         stk.push_back(path.substr(pos, path.length() - pos));
       }
     }
     std::string ret;
     for (std::vector<std::string>::iterator iter = stk.begin(); iter != stk.end(); ++iter ) {
       ret.append(*iter);
     }
     ret = toNativeSeparators(ret);
     #if defined (WIN32)
     if (ret.length() > 2
       && isalpha(ret[0]) != 0
       && islower(ret[0]) != 0
        && ret[1] == ':' && ret[2] == '\\')
      {
        //we have a lower case drive letter which needs to be changed to upper case.
        ret[0] = toupper(ret[0]);
      }
#endif
     return ret;
}
