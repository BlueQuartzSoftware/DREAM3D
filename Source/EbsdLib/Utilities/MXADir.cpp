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

#include "EbsdLib/Utilities/MXADir.h"

#define MXA_FILESYSTEM_BASE_CLASS EbsdDir

#include "EbsdLib/Utilities/MXAFileSystemPath.cpp"

#ifdef _WIN32
#include "EbsdLib/Utilities/MXADirent.h"
#else
#define AngDIR DIR
#define ang_dirent dirent
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<std::string> MXA_FILESYSTEM_BASE_CLASS::entryList(const QString &path)
{
  AngDIR* dir = NULL;
  struct ang_dirent* de = NULL;
  QVector<std::string> list;
  dir = opendir( path.c_str() );

  if (NULL != dir)
  {

    while ( NULL != (de = readdir(dir) ) )
    {
      if (de->d_name[0] != '.' && de->d_name[1] != '.')
      list.push_back(de->d_name);
    }
    closedir( dir );
  }
  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXA_FILESYSTEM_BASE_CLASS::mkdir(const QString &name, bool createParentDirectories)
{
#if defined (WIN32)
  QString dirName = name;
  if (createParentDirectories) {
      dirName = MXA_FILESYSTEM_BASE_CLASS::toNativeSeparators(MXA_FILESYSTEM_BASE_CLASS::cleanPath(dirName));
      // We specifically search for / so \ would break it..
      std::string::size_type oldslash = std::string::npos;
      if (dirName[0] == '\\' && dirName[1] == '\\')
      {
          // Don't try to create the root fsPath of a UNC fsPath;
          // CreateDirectory() will just return ERROR_INVALID_NAME.
          for (unsigned int i = 0; i < dirName.size(); ++i) {
              if (dirName[i] != MXA_FILESYSTEM_BASE_CLASS::Separator)
              {
                  oldslash = i;
                  break;
              }
          }
          if (oldslash != std::string::npos) {
              oldslash = dirName.find(MXA_FILESYSTEM_BASE_CLASS::Separator, oldslash);
          }
      }
      for (std::string::size_type slash=0; slash != std::string::npos; oldslash = slash) {
          slash = dirName.find(MXA_FILESYSTEM_BASE_CLASS::Separator, oldslash+1);
          if (slash == std::string::npos) {
              if(oldslash == dirName.length())
                  break;
              slash = dirName.length();
          }
          if (slash != std::string::npos) {
            QString chunk = dirName.substr(0, slash);
            bool existed = false;
            if (!isDirPath(chunk, &existed) && !existed)
            {
              if (!::CreateDirectoryA(chunk.c_str(), 0) ) { return false; }
            }
          }
      }
      return true;
  }
  return (!::CreateDirectoryA(dirName.c_str(), 0)  == 0);
#else

  QString dirName = name;
  if (createParentDirectories)
  {
    dirName = MXA_FILESYSTEM_BASE_CLASS::cleanPath(dirName);
    std::string::size_type slash = 0;
    for (std::string::size_type oldslash = 1; slash != std::string::npos; oldslash = slash)
    {
      slash = dirName.find(MXA_FILESYSTEM_BASE_CLASS::Separator, oldslash + 1);
      if (slash == std::string::npos)
      {
        if (oldslash == dirName.length()) break;
        slash = dirName.length();
      }
      if (slash != std::string::npos)
      {
        QString chunk = dirName.substr(0, slash);
        if (exists(chunk) == false)
        {
          MXA_STATBUF st;
          if (MXA_STAT(chunk.c_str(), &st) != -1)
          {
            if ((st.st_mode & S_IFMT) != S_IFDIR)
            {
              return false;
            }
          }
          else if (::mkdir(chunk.c_str(), 0777) != 0)
          {
            return false;
          }
        }
      }
    }
    return true;
  }
#if defined(__APPLE__)  // Mac X doesn't support trailing /'s
    if (dirName[dirName.length() - 1] == '/')
        dirName = dirName.substr(0, dirName.length() - 1);
#endif
    return (::mkdir(dirName.c_str(), 0777) == 0);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXA_FILESYSTEM_BASE_CLASS::rmdir(const QString &name, bool recurseParentDirectories)
{
#if defined (WIN32)
  QString dirName = name;
  if (recurseParentDirectories)
  {
      dirName = MXA_FILESYSTEM_BASE_CLASS::toNativeSeparators(MXA_FILESYSTEM_BASE_CLASS::cleanPath(dirName));
      for (std::string::size_type oldslash = 0, slash=dirName.length(); slash > 0; oldslash = slash)
      {
          QString chunk = dirName.substr(0, slash);
          if (chunk.length() == 2 && isalpha(chunk[0]) && chunk[1] == ':')
              break;
          if (!isDirPath(chunk, 0))
              return false;
          if (!_rmdir(chunk.c_str()))
              return oldslash != 0;
          slash = dirName.find_last_of(MXA_FILESYSTEM_BASE_CLASS::Separator, oldslash-1);
      }
      return true;
  }
  return (bool)(_rmdir(name.c_str()) == 0 );
#else
  QString dirName = name;
    if (recurseParentDirectories) {
        dirName = MXA_FILESYSTEM_BASE_CLASS::cleanPath(dirName);
        for(int oldslash = 0, slash=dirName.length(); slash > 0; oldslash = slash) {
            QString chunk = dirName.substr(0, slash);
            MXA_STATBUF st;
            if (MXA_STAT(chunk.c_str(), &st) != -1) {
                if ((st.st_mode & S_IFMT) != S_IFDIR)
                    return false;
                if (::rmdir(chunk.c_str()) != 0)
                    return oldslash != 0;
            } else {
                return false;
            }
            slash = dirName.find_last_of(MXA_FILESYSTEM_BASE_CLASS::Separator, oldslash-1);
        }
        return true;
    }
    return ::rmdir(dirName.c_str()) == 0;
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXA_FILESYSTEM_BASE_CLASS::remove(const QString &fsPath)
{
  return UNLINK(MXA_FILESYSTEM_BASE_CLASS::toNativeSeparators(fsPath).c_str()) == 0;
}
