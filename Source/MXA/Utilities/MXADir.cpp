///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "MXADir.h"

#define MXA_FILESYSTEM_BASE_CLASS MXADir

#include "MXAFileSystemPath.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::string> MXA_FILESYSTEM_BASE_CLASS::entryList(const std::string &path)
{
  DIR* dir = NULL;
  struct dirent* de = NULL;
  std::vector<std::string> list;
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
bool MXA_FILESYSTEM_BASE_CLASS::mkdir(const std::string &name, bool createParentDirectories)
{
#if defined (WIN32)
  std::string dirName = name;
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
            std::string chunk = dirName.substr(0, slash);
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

  std::string dirName = name;
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
        std::string chunk = dirName.substr(0, slash);
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
bool MXA_FILESYSTEM_BASE_CLASS::rmdir(const std::string &name, bool recurseParentDirectories)
{
#if defined (WIN32)
  std::string dirName = name;
  if (recurseParentDirectories)
  {
      dirName = MXA_FILESYSTEM_BASE_CLASS::toNativeSeparators(MXA_FILESYSTEM_BASE_CLASS::cleanPath(dirName));
      for (std::string::size_type oldslash = 0, slash=dirName.length(); slash > 0; oldslash = slash)
      {
          std::string chunk = dirName.substr(0, slash);
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
  std::string dirName = name;
    if (recurseParentDirectories) {
        dirName = MXA_FILESYSTEM_BASE_CLASS::cleanPath(dirName);
        for(int oldslash = 0, slash=dirName.length(); slash > 0; oldslash = slash) {
            std::string chunk = dirName.substr(0, slash);
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
bool MXA_FILESYSTEM_BASE_CLASS::remove(const std::string &fsPath)
{
  return UNLINK(MXA_FILESYSTEM_BASE_CLASS::toNativeSeparators(fsPath).c_str()) == 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXA_FILESYSTEM_BASE_CLASS::tempPath()
{
  std::string ret;
#if (WIN32)
  TCHAR path[MAX_PATH];

  DWORD retLength = GetTempPath(MAX_PATH, path);

  if (retLength) 
  {
    ret = std::string(path);
  }

  if (!ret.empty()) 
  {
    ret = MXADir::fromNativeSeparators(ret);
  }
  else
  {
    ret = std::string("/Temp");
  }
#else

    char* pPath;
     pPath = getenv ("TMPDIR");
     if (pPath!=NULL) {
       ret = std::string(pPath);
     }
     else
     {
       ret = "/tmp/";
     }
#endif
    return ret;
}
