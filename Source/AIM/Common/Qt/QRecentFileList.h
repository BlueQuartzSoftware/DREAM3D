///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
////////////////////////////////////////////////////////////////////////////////

#ifndef QRECENTFILELIST_H_
#define QRECENTFILELIST_H_

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QtCore/QList>
#include <QtGui/QAction>

class QMenu;

/**
 * @class QRecentFileList QRecentFileList.h Shared/QRecentFileList.h
 * @brief Holds a list of recently used files. Uses the Singleton design pattern
 * to ensure there is only one list per application instance.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Oct 2008
 * @version $Revision: 1.2 $
 */
class QRecentFileList: public QObject
{

  Q_OBJECT
    ;

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
    static QString strippedName(const QString &fullfilepath);

    /**
     * @brief Returns the Filename plus it's parent folder name
     * @param file The path to the file including its filename
     * @return
     */
    static QString parentAndFileName(const QString &file);

    /**
     * @brief Returns the current list of 'Recent Files' for this application
     * @return
     */
    QStringList fileList();

    /**
     * @brief Adds a file to the recent files
     * @param file The file to add to the list
     */
    void addFile(const QString &file);

    /**
     * @brief Writes the list of files to a preference file.
     * @param prefs The QSettings object to use.
     */
    void writeList(QSettings &prefs);

    /**
     * @brief Reads the list of Recent Files from the preferences file
     * @param prefs The QSettins object to use
     */
    void readList(QSettings &prefs);

    /* ******************* Begin Qt Signals ********************************* */
  signals:

    /**
     * @brief Qt Signal emitted when the list of files has changed.
     * @param file The newly added file
     */
    void fileListChanged(const QString &file);

  protected:

    /**
     * @brief Removes a file from the list of recent files.
     * @param file The file to remove
     */
    void removeFile(const QString &file);

    /**
     * @brief Returns true if the list already contains the file.
     * @param file The file to check.
     */
    bool contains(const QString &file);

  private:
    ~QRecentFileList();

    QStringList recentFiles;
    // QList<QAction*> actions;
    // QMenu* recentFileMenu;


    QRecentFileList(const QRecentFileList&); //Copy Constructor Not Implemented
    void operator=(const QRecentFileList&); //Copy Assignment Not Implemented

};

#endif /* QRECENTFILELIST_H_ */
