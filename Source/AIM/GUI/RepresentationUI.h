///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////

#ifndef AIMREPRESENTATION_H_
#define AIMREPRESENTATION_H_

#include <MXA/Common/MXASetGetMacros.h>


//-- Qt Includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QSettings>
#include <QtGui/QWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QResizeEvent>


//-- UIC generated Header
#include <ui_RepresentationUI.h>

/**
* @class RepresentationUI RepresentationUI AIM/RepresentationUI/GUI/RepresentationUI.h
* @brief The class is the actual implementation for the GUI window that appears on screen
* when the application is launched.
* @author Michael A. Jackson for BlueQuartz Software
* @date Oct 19, 2009
* @version 1.0
*/
class RepresentationUI : public QMainWindow, private Ui::RepresentationUI
{
  Q_OBJECT;

  public:
    RepresentationUI(QWidget *parent = 0);
    virtual ~RepresentationUI();

  protected slots:

  /* Menu Slots */
    void on_actionClose_triggered();
    void on_actionExit_triggered();

  /**
   * @brief Updates the QMenu 'Recent Files' with the latest list of files. This
   * should be connected to the Signal QRecentFileList->fileListChanged
   * @param file The newly added file.
   */
  void updateRecentFileList(const QString &file);

  /**
   * @brief Qt Slot that fires in response to a click on a "Recent File' Menu entry.
   */
  void openRecentFile();


  private slots:
    // slots for our worker thread to communicate
    void threadHasMessage(QString message);



    // Our Signals that we can emit custom for this class
  signals:
    void parentResized();

    /**
     * @brief A signal that is emitted when we want to cancel a process
     */
    void sig_CancelWorker();

  protected:

    /**
     * @brief Implements the CloseEvent to Quit the application and write settings
     * to the preference file
     */
    void closeEvent(QCloseEvent *event);

    /**
     * @brief Drag and drop implementation
     */
    void dragEnterEvent(QDragEnterEvent*);

    /**
     * @brief Drag and drop implementation
     */
    void dropEvent(QDropEvent*);

    /**
     * @brief Enables or Disables all the widgets in a list
     * @param b
     */
    void setWidgetListEnabled(bool b);

    /**
     * @brief Verifies that a path exists on the file system.
     * @param outFilePath The file path to check
     * @param lineEdit The QLineEdit object to modify visuals of (Usually by placing a red line around the QLineEdit widget)
     */
    bool verifyPathExists(QString outFilePath, QLineEdit* lineEdit);

    /**
     * @brief Verifies that a parent path exists on the file system.
     * @param outFilePath The parent file path to check
     * @param lineEdit The QLineEdit object to modify visuals of (Usually by placing a red line around the QLineEdit widget)
     */
    bool verifyOutputPathParentExists(QString outFilePath, QLineEdit* lineEdit);

    /**
     * @brief Reads the Preferences from the users pref file
     */
    void readSettings();

    /**
     * @brief Writes the preferences to the users pref file
     */
    void writeSettings();

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    void setupGui();


    /**
     * @brief Checks the currently open file for changes that need to be saved
     * @return
     */
    qint32 checkDirtyDocument();

    /**
     * @brief Over ride the resize event
     * @param event The event to process
     */
    void resizeEvent ( QResizeEvent * event );

  private:
    QList<QWidget*>             m_WidgetList;
    QThread*                    m_WorkerThread;
    QString                     m_OpenDialogLastDirectory;

    RepresentationUI(const RepresentationUI&);    // Copy Constructor Not Implemented
    void operator=(const RepresentationUI&);  // Operator '=' Not Implemented

};

#endif /* AIMREPRESENTATION_H_ */
