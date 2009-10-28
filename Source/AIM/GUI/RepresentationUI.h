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
#include <AIM/Reconstruction/Reconstruction.h>
#include <AIM/GrainGenerator/GrainGenerator.h>


//-- Qt Includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtGui/QWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QResizeEvent>

//-- UIC generated Header
#include <ui_RepresentationUI.h>

/**
* @class RepresentationUI RepresentationUI AIM/RepresentationUI/GUI/RepresentationUI.h
* @brief
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

    void on_actionClose_triggered();
    void on_actionExit_triggered();
    void on_angDirBtn_clicked();
    void on_outputDirBtn_clicked();
    void on_alreadyFormed_stateChanged(int);
    void on_reconstructBtn_clicked();

    /* Grain Generator Slots*/
    void on_gg_InputDirBtn_clicked();
    void on_gg_OutputDirBtn_clicked();
    void on_gg_GoBtn_clicked();


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

    void reconstruction_ThreadFinished();
    void reconstruction_ThreadProgressed(int value);

    void gg_ThreadFinished();
    void gg_ThreadProgressed(int value);

    void surfaceMesh_ThreadFinished();
    void surfaceMesh_ThreadProgressed(int value);

    void volumeMesh_ThreadFinished();
    void volumeMesh_ThreadProgressed(int value);

  signals:
    void parentResized();
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
    bool _verifyPathExists(QString outFilePath, QLineEdit* lineEdit);

    /**
     * @brief Verifies that a parent path exists on the file system.
     * @param outFilePath The parent file path to check
     * @param lineEdit The QLineEdit object to modify visuals of (Usually by placing a red line around the QLineEdit widget)
     */
    bool _verifyOutputPathParentExists(QString outFilePath, QLineEdit* lineEdit);

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
    void setupGui_Reconstruction();
    void setupGui_GrainGenerator();

    /**
     * @brief Checks the currently open file for changes that need to be saved
     * @return
     */
    qint32 _checkDirtyDocument();


    void resizeEvent ( QResizeEvent * event );

    void findAngMaxSliceAndPrefix();
    void findReconstructionOutputFiles();

  private:
    QString                     m_OpenDialogLastDirectory;
    QList<QWidget*>             m_WidgetList;

    Reconstruction::Pointer     m_Reconstruction;
    GrainGenerator::Pointer     m_GrainGenerator;

    RepresentationUI(const RepresentationUI&);    // Copy Constructor Not Implemented
    void operator=(const RepresentationUI&);  // Operator '=' Not Implemented

};

#endif /* AIMREPRESENTATION_H_ */
