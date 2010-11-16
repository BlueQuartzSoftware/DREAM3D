///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
/* Note that some of this implementation was inspired by the Qt source code,
 * in particular the QDir and QFileEngine source codes. The idea is to keep the
 * API the same between my implementation and the Qt Implementation so that
 * switching between then is easy.
 *
 */


#include "MXAFileSystemPath.h"
#include <MXA/Common/LogTime.h>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <ctype.h>
#include <string.h> // Needed for memset

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



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXAFileSystemPath::MXAFileSystemPath()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXAFileSystemPath::~MXAFileSystemPath()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXAFileSystemPath::getSeparator()
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
bool MXAFileSystemPath::isDirectory(const std::string &fsPath)
{
#if defined (WIN32)
  bool existed = false;
  return MXAFileSystemPath::isDirPath(fsPath, &existed);
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
bool MXAFileSystemPath::isFile(const std::string &fsPath)
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
uint64 MXAFileSystemPath::fileSize(const std::string &path)
{
  int error;
  MXA_STATBUF st;
  error = MXA_STAT(path.c_str(), &st);
  if (!error && (st.st_mode & S_IFMT) == S_IFREG)
  {
    return (uint64)(st.st_size);
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXAFileSystemPath::isRelativePath(const std::string &path)
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
bool MXAFileSystemPath::isAbsolutePath(const std::string &path)
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
std::string MXAFileSystemPath::currentPath()
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
      std::cout << logTime() << "Error: MXAFileSystemPath::currentPath result was NULL." << std::endl;
    }
    else
    {
      currentPath = std::string(currentName);
    }

  }
  else
  {
    std::cout << logTime() << "Error: MXAFileSystemPath::currentPath stat function failed." << std::endl;
  }
#if defined(WIN32)
  currentPath = MXAFileSystemPath::cleanPath(currentPath);
#endif

  return currentPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXAFileSystemPath::absolutePath(const std::string &path)
{
  std::string abspath;
  if ( true == MXAFileSystemPath::isAbsolutePath(path))
  { return path; }

  abspath = MXAFileSystemPath::currentPath();
  if(abspath[abspath.length()-1] != MXAFileSystemPath::Separator)
  {
    abspath = abspath + MXAFileSystemPath::Separator;
  }
  abspath.append(path);
  abspath = MXAFileSystemPath::cleanPath(abspath);
  return abspath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::string> MXAFileSystemPath::entryList(const std::string &path)
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
std::string MXAFileSystemPath::parentPath(const std::string &path)
{
  std::string curAbsPath = MXAFileSystemPath::absolutePath(path);
  curAbsPath = MXAFileSystemPath::fromNativeSeparators(curAbsPath);
  std::string::size_type nextToLastSlashPos = 0;
  std::string::size_type lastSlashPos = curAbsPath.find_last_of(MXAFileSystemPath::UnixSeparator);
// Remove trailing '/' if found
  if (lastSlashPos == curAbsPath.length() - 1)
  {
    curAbsPath = curAbsPath.substr(0, curAbsPath.length()-2);
    lastSlashPos = curAbsPath.find_last_of(MXAFileSystemPath::Separator);
  }

  if (lastSlashPos > 0)
  {
    nextToLastSlashPos = curAbsPath.find_last_of(MXAFileSystemPath::UnixSeparator, lastSlashPos - 1);
  }

  if (nextToLastSlashPos == std::string::npos) // Only 1 slash found, return the root directory
  {
#if defined (WIN32)
    curAbsPath = curAbsPath.substr(0, 3);
    return MXAFileSystemPath::toNativeSeparators(curAbsPath);
#else
    return curAbsPath.substr(0, 1);
#endif
  }

  curAbsPath = curAbsPath.substr(0, lastSlashPos);
#if defined (WIN32)
  curAbsPath = MXAFileSystemPath::toNativeSeparators(curAbsPath);
#endif
  return curAbsPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXAFileSystemPath::exists(const std::string &fsPath)
{
  int error;
  std::string dirName(fsPath);
  // Both windows and OS X both don't like trailing slashes so just get rid of them
  // for all Operating Systems.
  if (dirName[dirName.length() - 1] == MXAFileSystemPath::Separator) {
        dirName = dirName.substr(0, dirName.length() - 1);
  }
  MXA_STATBUF st;
  error = MXA_STAT(dirName.c_str(), &st);
  return (error == 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXAFileSystemPath::extension(const std::string &fsPath)
{
  std::string::size_type pos = fsPath.find_last_of('.');
  // No '.' characters appeared in the path at all
  if(std::string::npos == pos)
  {
    return std::string();
  }
  // Look for a "Hidden" file  .som ./.some ../.something.something
  if (pos == 0
      || fsPath[pos-1] == MXAFileSystemPath::Separator)
  {
    return std::string();  // Return empty string, there is no extension
  }

  std::string::size_type slashpos = fsPath.find_last_of(MXAFileSystemPath::Separator);
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
std::string MXAFileSystemPath::filename(const std::string &fsPath)
{

  std::string::size_type slashPos = fsPath.find_last_of(MXAFileSystemPath::Separator);
  if (slashPos == fsPath.size() - 1)
  {
    return MXAFileSystemPath::filename(fsPath.substr(0, fsPath.size() - 1) );
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
std::string MXAFileSystemPath::fileNameWithOutExtension(const std::string &fsPath)
{
  std::string fname = MXAFileSystemPath::filename(fsPath);
  std::string ext = MXAFileSystemPath::extension(fsPath);
  std::string::size_type pos = fname.find_last_of(ext);
  if (pos != std::string::npos)
  {
    fname = fname.substr(0, fname.size() - ext.size() - 1);
  }

//  std::string parentPath = MXAFileSystemPath::parentPath(fsPath);
//  if (parentPath.size() > 0)
//  {
//    return parentPath + MXAFileSystemPath::getSeparator() + fname;
//  }
//  else
//  {
    return fname;
//  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXAFileSystemPath::mkdir(const std::string &name, bool createParentDirectories)
{
#if defined (WIN32)
  std::string dirName = name;
  if (createParentDirectories) {
      dirName = MXAFileSystemPath::toNativeSeparators(MXAFileSystemPath::cleanPath(dirName));
      // We specifically search for / so \ would break it..
      std::string::size_type oldslash = std::string::npos;
      if (dirName[0] == '\\' && dirName[1] == '\\') 
      {
          // Don't try to create the root fsPath of a UNC fsPath;
          // CreateDirectory() will just return ERROR_INVALID_NAME.
          for (unsigned int i = 0; i < dirName.size(); ++i) {
              if (dirName[i] != MXAFileSystemPath::Separator) 
              {
                  oldslash = i;
                  break;
              }
          }
          if (oldslash != std::string::npos) {
              oldslash = dirName.find(MXAFileSystemPath::Separator, oldslash);
          }
      }
      for (std::string::size_type slash=0; slash != std::string::npos; oldslash = slash) {
          slash = dirName.find(MXAFileSystemPath::Separator, oldslash+1);
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
  if (createParentDirectories) {
      dirName = MXAFileSystemPath::cleanPath(dirName);
      std::string::size_type slash = 0;
      for(std::string::size_type oldslash = std::string::npos; slash != std::string::npos; oldslash = slash)
      {
          slash = dirName.find(MXAFileSystemPath::Separator, oldslash+1);
          if (slash == std::string::npos) {
              if (oldslash == dirName.length())
                  break;
              slash = dirName.length();
          }
          if (slash != std::string::npos) {
              std::string chunk = dirName.substr(0, slash);
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
bool MXAFileSystemPath::remove(const std::string &fsPath)
{
  return UNLINK(MXAFileSystemPath::toNativeSeparators(fsPath).c_str()) == 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXAFileSystemPath::rmdir(const std::string &name, bool recurseParentDirectories)
{
#if defined (WIN32)
  std::string dirName = name;
  if (recurseParentDirectories) 
  {
      dirName = MXAFileSystemPath::toNativeSeparators(MXAFileSystemPath::cleanPath(dirName));
      for (std::string::size_type oldslash = 0, slash=dirName.length(); slash > 0; oldslash = slash) 
      {
          std::string chunk = dirName.substr(0, slash);
          if (chunk.length() == 2 && isalpha(chunk[0]) && chunk[1] == ':')
              break;
          if (!isDirPath(chunk, 0))
              return false;
          if (!_rmdir(chunk.c_str()))
              return oldslash != 0;
          slash = dirName.find_last_of(MXAFileSystemPath::Separator, oldslash-1);
      }
      return true;
  }
  return (bool)(_rmdir(name.c_str()) == 0 );
#else
  std::string dirName = name;
    if (recurseParentDirectories) {
        dirName = MXAFileSystemPath::cleanPath(dirName);
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
            slash = dirName.find_last_of(MXAFileSystemPath::Separator, oldslash-1);
        }
        return true;
    }
    return ::rmdir(dirName.c_str()) == 0;
#endif
}

#if defined (WIN32)
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXAFileSystemPath::isDirPath(const std::string &folderPath, bool *existed)
{
    std::string fsPath = folderPath;
    if (fsPath.length() == 2 &&fsPath.at(1) == ':')
        fsPath += MXAFileSystemPath::Separator;

    DWORD fileAttrib = INVALID_FILE_ATTRIBUTES;
    fileAttrib = ::GetFileAttributesA(fsPath.c_str() );

    if (existed)
        *existed = fileAttrib != INVALID_FILE_ATTRIBUTES;

    if (fileAttrib == INVALID_FILE_ATTRIBUTES)
        return false;

    return (bool)(fileAttrib & FILE_ATTRIBUTE_DIRECTORY);
}
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXAFileSystemPath::fromNativeSeparators(const std::string  &fsPath)
{
  std::string path(fsPath);
#if defined (WIN32)
  for (int i=0; i<(int)path.length(); i++) {
      if (path[i] ==  MXAFileSystemPath::Separator )
        path[i] =  MXAFileSystemPath::UnixSeparator;
  }
#endif
  return path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXAFileSystemPath::toNativeSeparators(const std::string &fsPath)
{
    std::string path(fsPath);
#if defined (WIN32)
    for (int i=0; i<(int)path.length(); i++) {
        if (path[i] ==  MXAFileSystemPath::UnixSeparator )
          path[i] =  MXAFileSystemPath::Separator;
    }
#endif
    return path;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string MXAFileSystemPath::cleanPath(const std::string &fsPath)
{
    if (fsPath.length() == 0)
        return fsPath;
      std::string path(fsPath);
     char slash = '/';
     char dot = '.';
     if (MXAFileSystemPath::Separator != MXAFileSystemPath::UnixSeparator)
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
