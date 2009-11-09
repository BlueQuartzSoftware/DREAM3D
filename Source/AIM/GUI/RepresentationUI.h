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
#include <AIM/SurfaceMesh/SurfaceMesh.h>
#include <AIM/VolumeMesh/VolumeMesh.h>

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

    /* Reconstruction Slots */
    void on_angDirBtn_clicked();
    void on_outputDirBtn_clicked();
    void on_alreadyFormed_stateChanged(int);
    void on_rec_GoBtn_clicked();

    /* Grain Generator Slots*/
    void on_gg_InputDirBtn_clicked();
    void on_gg_OutputDirBtn_clicked();
    void on_gg_GoBtn_clicked();

    /* Surface Meshing Slots */
    void on_sm_DxFileBtn_clicked();
    void on_sm_OutputDirBtn_clicked();
    void on_sm_GoBtn_clicked();

    /* Volume Meshing Slots */
    void on_vm_NodesFileBtn_clicked();
    void on_vm_TrianglesFileBtn_clicked();
    void on_vm_OutputDirBtn_clicked();
    void on_vm_GoBtn_clicked();



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

    /* Reconstruction Thread communicates throught these methods */
    void rec_ThreadFinished();
    void rec_ThreadProgressed(int value);
    /* Synthetic Builder Thread communicates throught these methods */
    void gg_ThreadFinished();
    void gg_ThreadProgressed(int value);
    /* Surface Mesh Thread communicates throught these methods */
    void sm_ThreadFinished();
    void sm_ThreadProgressed(int value);
    /* Volume Mesh Thread communicates throught these methods */
    void vm_ThreadFinished();
    void vm_ThreadProgressed(int value);

    // slots to catch signals emittd by the various QLineEdit widgets
    void on_angDir_textChanged(const QString & text);
    void on_rec_OutputDir_textChanged(const QString & text);

    void on_gg_InputDir_textChanged(const QString & text);
    void on_gg_OutputDir_textChanged(const QString & text);

    void on_sm_DxFile_textChanged(const QString & text);
    void on_sm_OutputDir_textChanged(const QString & text);

    void on_vm_NodesFile_textChanged(const QString & text);
    void on_vm_TrianglesFile_textChanged(const QString & text);
    void on_vm_OutputDir_textChanged(const QString & text);

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

    /*
     * These methods are the various GUI related setup methods that do some more setup operations on the widgets before
     * the initial window is displayed. Each of the methods is called from the 'setupGui' method.
     */
    void rec_SetupGui();
    void rec_CheckIOFiles();

    void gg_SetupGui();
    void gg_CheckIOFiles();

    void sm_SetupGui();
    void sm_CheckIOFiles();

    void vm_SetupGui();
    void vm_CheckIOFiles();

    /**
     * @brief Checks the currently open file for changes that need to be saved
     * @return
     */
    qint32 _checkDirtyDocument();

    /**
     * @brief Over ride the resize event
     * @param event The event to process
     */
    void resizeEvent ( QResizeEvent * event );

    /**
     * @brief Method to attempt the extraction of the .ang max slice value and prefix
     */
    void findAngMaxSliceAndPrefix();

  private:
    QString                     m_OpenDialogLastDirectory;
    QList<QWidget*>             m_WidgetList;

    /*
     * We keep a shared_pointer to the four types of processing that we do.
     */
    Reconstruction::Pointer     m_Reconstruction;
    GrainGenerator::Pointer     m_GrainGenerator;
    SurfaceMesh::Pointer        m_SurfaceMesh;
    VolumeMesh::Pointer         m_VolumeMesh;

    RepresentationUI(const RepresentationUI&);    // Copy Constructor Not Implemented
    void operator=(const RepresentationUI&);  // Operator '=' Not Implemented

};

#endif /* AIMREPRESENTATION_H_ */
