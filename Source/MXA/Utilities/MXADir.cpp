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
QVector<QString> MXA_FILESYSTEM_BASE_CLASS::entryList(const QString &path)
{
  DIR* dir = NULL;
  struct dirent* de = NULL;
  QVector<QString> list;
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
      QString::size_type oldslash = QString::npos;
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
          if (oldslash != QString::npos) {
              oldslash = dirName.find(MXA_FILESYSTEM_BASE_CLASS::Separator, oldslash);
          }
      }
      for (QString::size_type slash=0; slash != QString::npos; oldslash = slash) {
          slash = dirName.find(MXA_FILESYSTEM_BASE_CLASS::Separator, oldslash+1);
          if (slash == QString::npos) {
              if(oldslash == dirName.length())
                  break;
              slash = dirName.length();
          }
          if (slash != QString::npos) {
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
    QString::size_type slash = 0;
    for (QString::size_type oldslash = 1; slash != QString::npos; oldslash = slash)
    {
      slash = dirName.find(MXA_FILESYSTEM_BASE_CLASS::Separator, oldslash + 1);
      if (slash == QString::npos)
      {
        if (oldslash == dirName.length()) break;
        slash = dirName.length();
      }
      if (slash != QString::npos)
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
      for (QString::size_type oldslash = 0, slash=dirName.length(); slash > 0; oldslash = slash)
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MXA_FILESYSTEM_BASE_CLASS::tempPath()
{
  QString ret;
#if (WIN32)
  TCHAR path[MAX_PATH];

  DWORD retLength = GetTempPath(MAX_PATH, path);

  if (retLength) 
  {
    ret = QString(path);
  }

  if (!ret.empty()) 
  {
    ret = MXADir::fromNativeSeparators(ret);
  }
  else
  {
    ret = QString("/Temp");
  }
#else

    char* pPath;
     pPath = getenv ("TMPDIR");
     if (pPath!=NULL) {
       ret = QString(pPath);
     }
     else
     {
       ret = "/tmp/";
     }
#endif
    return ret;
}
