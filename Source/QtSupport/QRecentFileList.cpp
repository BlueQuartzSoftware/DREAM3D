/* ============================================================================
 * Copyright (c) 2008, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
  //qDebug() << "QRecentFileList()" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QRecentFileList::~QRecentFileList()
{
// qDebug() << "~QRecentFileList()" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QRecentFileList* QRecentFileList::instance()
{
// qDebug() << "QRecentFileList::instance()" << "\n";
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
bool QRecentFileList::contains(const QString& file)
{
  return this->recentFiles.contains(file);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QRecentFileList::addFile(const QString& file)
{
  //qDebug() << "QRecentFileList::addFile()" << "\n";
  //qDebug() << "recentFiles.count: " << recentFiles.count() << "\n";
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
void QRecentFileList::removeFile(const QString& file)
{
  //qDebug() << "QRecentFileList::removeFile()" << "\n";
  this->recentFiles.removeAll(file);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QRecentFileList::writeList(QSettings& prefs)
{
  //qDebug() << "QRecentFileList::writeList()" << "\n";
  prefs.beginGroup("RecentFiles");
  prefs.setValue("recentFileList", this->recentFiles );
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QRecentFileList::readList(QSettings& prefs)
{
  //qDebug() << "QRecentFileList::readList()" << "\n";
  prefs.beginGroup("RecentFiles");
  this->recentFiles = prefs.value("recentFileList").toStringList();
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QRecentFileList::strippedName(const QString& fullFileName)
{
  return QFileInfo(fullFileName).fileName();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QRecentFileList::parentAndFileName(const QString& file)
{
  QFileInfo fileinfo(file);

  QDir parent = fileinfo.dir();
  return parent.dirName() + QDir::separator() + fileinfo.fileName();
}

