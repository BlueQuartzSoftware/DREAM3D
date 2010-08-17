
#include "QRecentFileList.h"

#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QMenu>

#include <iostream>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QRecentFileList::QRecentFileList(QObject* parent) :
QObject(parent)
{
  // TODO Auto-generated constructor stub
  //std::cout << "QRecentFileList()" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QRecentFileList::~QRecentFileList()
{
  // TODO Auto-generated destructor stub
 // std::cout << "~QRecentFileList()" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QRecentFileList* QRecentFileList::instance()
{
 // std::cout << "QRecentFileList::instance()" << std::endl;
  static QRecentFileList* singleton;

  if (singleton == NULL)
  {
    singleton = new QRecentFileList();
  }
  return singleton;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QRecentFileList::contains(const QString &file)
{
  return this->recentFiles.contains(file);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QRecentFileList::addFile(const QString &file)
{
  //std::cout << "QRecentFileList::addFile()" << std::endl;
  //std::cout << "recentFiles.count: " << recentFiles.count() << std::endl;
  if (QFile::exists(file) == true)
  {
    if (this->recentFiles.contains(file) == false)
    {
      this->recentFiles.append(file);
      emit fileListChanged(file); // Emit the signal so all the menus can update thier contents
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList QRecentFileList::fileList()
{
  return this->recentFiles;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QRecentFileList::removeFile(const QString &file)
{
  //std::cout << "QRecentFileList::removeFile()" << std::endl;
  this->recentFiles.removeAll(file);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QRecentFileList::writeList(QSettings &prefs)
{
  //std::cout << "QRecentFileList::writeList()" << std::endl;
  prefs.beginGroup("RecentFiles");
  prefs.setValue("recentFileList", this->recentFiles );
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QRecentFileList::readList(QSettings &prefs)
{
  //std::cout << "QRecentFileList::readList()" << std::endl;
  prefs.beginGroup("RecentFiles");
  this->recentFiles = prefs.value("recentFileList").toStringList();
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QRecentFileList::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QRecentFileList::parentAndFileName(const QString &file)
{
  QFileInfo fileinfo(file);

  QDir parent = fileinfo.dir();
  return parent.dirName() + QDir::separator() +fileinfo.fileName();
}

