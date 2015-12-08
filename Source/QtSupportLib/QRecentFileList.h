/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef _QRECENTFILELIST_H_
#define _QRECENTFILELIST_H_

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QtCore/QFileSystemWatcher>
// #include <QtCore/QList>
// #include <QtWidgets/QAction>

#include "QtSupportLib/QtSupportLib.h"
#include "QtSupportLib/DREAM3DSettings.h"

enum AddType
{
  PREPEND,
  APPEND
};

class QMenu;

/**
 * @class QRecentFileList QRecentFileList.h Shared/QRecentFileList.h
 * @brief Holds a list of recently used files. Uses the Singleton design pattern
 * to ensure there is only one list per application instance.
 *
 * @date Oct 2008
 * @version $Revision: 1.2 $
 */
class QtSupportLib_EXPORT QRecentFileList: public QObject
{

    Q_OBJECT

  public:
    QRecentFileList(QObject* parent = 0);

    /**
     * @brief Returns the singleton instance of this class.
     * @return Instance of this class
     */
    static QRecentFileList* instance();

    /**
     * @brief Stripps the path from a file argument given as a full path.
     * @param fullfilepath The filename to extract
     * @return File name
     */
    static QString strippedName(const QString& fullfilepath);

    /**
     * @brief Returns the Filename plus it's parent folder name
     * @param file The path to the file including its filename
     * @return
     */
    static QString parentAndFileName(const QString& file);

    /**
     * @brief Returns the current list of 'Recent Files' for this application
     * @return
     */
    QStringList fileList();

    /**
     * @brief Adds a file to the recent files
     * @param file The file to add to the list
     */
    void addFile(const QString& file, AddType type = PREPEND);

    /**
     * @brief Writes the list of files to a preference file.
     * @param prefs The QSettings object to use.
     */
    void writeList(DREAM3DSettings* prefs);

    /**
     * @brief Reads the list of Recent Files from the preferences file
     * @param prefs The QSettins object to use
     */
    void readList(DREAM3DSettings* prefs);

    /**
    * @brief Clears the list of recent files
    */
    void clear();

  public slots:

    /**
    * @brief Removes a file from the list of recent files.
    * @param file The file to remove
    */
    void removeFile(const QString& file);

    /* ******************* Begin Qt Signals ********************************* */
  signals:

    /**
     * @brief Qt Signal emitted when the list of files has changed.
     * @param file The newly added file
     */
    void fileListChanged(const QString& file);

  protected:

    /**
     * @brief Returns true if the list already contains the file.
     * @param file The file to check.
     */
    bool contains(const QString& file);

  private:
    ~QRecentFileList();

    QStringList recentFiles;

    QFileSystemWatcher* m_Watcher;
    // QList<QAction*> actions;
    // QMenu* recentFileMenu;


    QRecentFileList(const QRecentFileList&); //Copy Constructor Not Implemented
    void operator=(const QRecentFileList&); //Copy Assignment Not Implemented

};

#endif /* QRECENTFILELIST_H_ */

