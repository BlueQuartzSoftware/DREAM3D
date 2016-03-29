/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#ifndef _statsgeneratorui_h_
#define _statsgeneratorui_h_


#include <QtCore/QObject>

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>

#include "SGWidget.h"
#include "PrimaryPhaseWidget.h"
#include "PrecipitatePhaseWidget.h"
#include "MatrixPhaseWidget.h"
#include "BoundaryPhaseWidget.h"
#include "TransformationPhaseWidget.h"

#include <ui_StatsGeneratorUI.h>

class HelpDialog;

/**
 * @class StatsGeneratorUI StatsGeneratorUI.h StatsGenerator/StatsGeneratorUI.h
 * @brief
 *
 * @date Jan 4, 2011
 * @version 1.0
 */
class StatsGeneratorUI : public QMainWindow, private Ui::StatsGeneratorUI
{
    Q_OBJECT

  public:
    StatsGeneratorUI(QWidget* parent = 0);
    virtual ~StatsGeneratorUI();

    void adjustWindowTitle();

    void setFilePath(QString filePath);
    QString getFilePath();

    /**
     * @brief openFile
     * @param h5file
     * @return
     */
    int openFile(QString h5file);

    SGWidget* createNewSGWidget();

    QUrl htmlHelpIndexFile();

    void displayDialogBox(QString title, QString text, QMessageBox::Icon icon);

  signals:
    void windowIsClosing(StatsGeneratorUI*);

  protected slots:

    /* Menu Slots */
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionClose_triggered();
    void on_actionExit_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    // Tools Menu
    void on_action_OpenDREAM3D_triggered();

    // HELP Menu
    void on_actionAbout_triggered();
    void on_actionLicense_Information_triggered();
    void on_actionStatsGenerator_Help_triggered();

    void on_addPhase_clicked();
    void on_deletePhase_clicked();
    void on_editPhase_clicked();

    void on_phaseTabs_tabCloseRequested ( int index );

  protected:

    /**
     * @brief Implements the CloseEvent to Quit the application and write settings
     * to the preference file
     */
    void closeEvent(QCloseEvent* event);

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


  private slots:

    /**
     * @brief Updates the QMenu 'Recent Files' with the latest list of files. This
     * should be connected to the Signal QRecentFileList->fileListChanged
     * @param file The newly added file.
     */
    void updateRecentFileList(const QString& file);

    /**
     * @brief Qt Slot that fires in response to a click on a "Recent File' Menu entry.
     */
    void openRecentFile();

  private:
    QString              m_FilePath;
    bool                 m_FileSelected;
    //  QVector<SGWidget*>   m_SGWidgets;
    HelpDialog*          m_HelpDialog;

    QString m_OpenDialogLastDirectory; // Must be last in the list
    StatsGeneratorUI(const StatsGeneratorUI&); // Copy Constructor Not Implemented
    void operator=(const StatsGeneratorUI&); // Operator '=' Not Implemented
};

#endif /* _STATSGENERATOR_H_ */

