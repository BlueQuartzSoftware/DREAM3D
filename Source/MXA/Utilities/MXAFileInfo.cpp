
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
uint64_t MXA_FILESYSTEM_BASE_CLASS::fileSize(const QString &path)
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
QString MXA_FILESYSTEM_BASE_CLASS::extension(const QString &fsPath)
{
  QString::size_type pos = fsPath.find_last_of('.');
  // No '.' characters appeared in the path at all
  if(QString::npos == pos)
  {
    return QString();
  }
  // Look for a "Hidden" file  .som ./.some ../.something.something
  if (pos == 0
      || fsPath[pos-1] == MXA_FILESYSTEM_BASE_CLASS::Separator)
  {
    return QString();  // Return empty string, there is no extension
  }

  QString::size_type slashpos = fsPath.find_last_of(MXA_FILESYSTEM_BASE_CLASS::Separator);
  // Check for just a plain filename, ie a path with NO directory delimiters in the string
  if (QString::npos == slashpos && QString::npos != pos)
  {
    return fsPath.substr(pos + 1);
  }

  if (pos > slashpos)
  {
    return fsPath.substr(pos + 1);
  }

  return QString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MXA_FILESYSTEM_BASE_CLASS::filename(const QString &fsPath)
{
  if (fsPath.size() == 0) { return QString(""); }
  QString::size_type slashPos = fsPath.find_last_of(MXA_FILESYSTEM_BASE_CLASS::Separator);
  if (slashPos == fsPath.size() - 1)
  {
    return MXA_FILESYSTEM_BASE_CLASS::filename(fsPath.substr(0, fsPath.size() - 1) );
  }

  QString fn = fsPath.substr(slashPos + 1, fsPath.size() - slashPos);
  if (fn.at(fn.size()-1) == '.')
  {
    return fn.substr(0, fn.size() - 1);
  }
  return fn;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MXA_FILESYSTEM_BASE_CLASS::fileNameWithOutExtension(const QString &fsPath)
{
  QString fname = MXA_FILESYSTEM_BASE_CLASS::filename(fsPath);
  QString ext = MXA_FILESYSTEM_BASE_CLASS::extension(fsPath);
  QString::size_type pos = fname.find_last_of(ext);
  if (pos != QString::npos)
  {
    fname = fname.substr(0, fname.size() - ext.size() - 1);
  }

//  QString parentPath = MXA_FILESYSTEM_BASE_CLASS::parentPath(fsPath);
//  if (parentPath.size() > 0)
//  {
//    return parentPath + MXA_FILESYSTEM_BASE_CLASS::getSeparator() + fname;
//  }
//  else
//  {
    return fname;
//  }
}
