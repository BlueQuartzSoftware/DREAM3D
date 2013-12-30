/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef _DREAM3D_UI_H_
#define _DREAM3D_UI_H_


//-- Qt Includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QVector>
#include <QtCore/QSettings>
#include <QtGui/QWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QResizeEvent>
#include <QtGui/QToolBar>


//-- UIC generated Header
#include <ui_DREAM3D_UI.h>

#include <DREAM3DLib/Common/FilterManager.h>


class DREAM3DPluginInterface;

class QFilterLibraryDockWidget;
class QDream3DDocumentsDockWidget;
class QPrebuiltPipelinesDockWidget;
class FilterListWidget;
class DREAM3DUpdateCheckDialog;
class UpdateCheckData;
class UpdateCheck;
class HelpDialog;

/**
* @class DREAM3D_UI DREAM3D_UI Applications/DREAM3D/DREAM3D_UI.h
* @brief The class is the actual implementation for the GUI window that appears on screen
* when the application is launched. It contains all the subwidgets that the user
* can utilize to perform the various actions of the DREAM.3D Software Suite.
* @author Michael A. Jackson for BlueQuartz Software
* @date Oct 19, 2009
* @version 1.0
*/
class DREAM3D_UI : public QMainWindow, private Ui::DREAM3D_UI
{
    Q_OBJECT

  public:
    DREAM3D_UI(QWidget *parent = 0);
    virtual ~DREAM3D_UI();

  protected slots:

    /* Menu Slots */
    // File MENU
    void on_actionExit_triggered();
    void on_actionOpenPipeline_triggered();
    void on_actionSavePipeline_triggered();
    void on_actionCheck_For_Updates_triggered();

    //Tools Menu
    //void on_action_OpenStatsGenerator_triggered();

    // Help Menu
    void on_actionLicense_Information_triggered();
    void on_actionAbout_triggered();
    void on_actionShow_User_Manual_triggered();

    /**
     * @brief Reads the Preferences from the users pref file
     */
    void readSettings();

    /**
     * @brief Writes the preferences to the users pref file
     */
    void writeSettings();

    /**
     * @brief Writes the pipeline order to a text file
     */
    void savePipeline(QSettings &prefs);

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

    void setInputUI();

    void displayHelp(QString helpFile);

    void versionCheckReply(UpdateCheckData*);

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

    void loadPlugins(FilterManager* fm);
    void populateMenus(QObject *plugin);
    void addToPluginMenu(QObject *plugin, const QString &text,
                         QMenu *menu, const char *member,
                         QActionGroup *actionGroup, QIcon icon);


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
     *
     * @param prefs
     */
    void writeWindowSettings(QSettings &prefs);

    void readWindowSettings(QSettings &prefs);

    void readVersionCheckSettings(QSettings &prefs);
    void writeVersionCheckSettings(QSettings &prefs);

    void checkForUpdatesAtStartup();

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    void setupGui();

    void makeStatusBarButton(QString text, QDockWidget *dockWidget, int index);


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
    QStringList                 m_PluginDirs;
    QStringList                 m_PluginFileNames;
    QActionGroup*               m_PluginActionGroup;
    QActionGroup*               m_PluginPrefsActionGroup;
    DREAM3DPluginInterface*     m_ActivePlugin;
    QVector<DREAM3DPluginInterface*> m_LoadedPlugins;
    QToolBar*                   m_PluginToolBar;
    HelpDialog*                 m_HelpDialog;

    UpdateCheck*                m_UpdateCheck;

    QString                     m_OpenDialogLastDirectory;

    QThread*                    m_UpdateCheckThread;
    FilterManager::Pointer      m_FilterManager;


    QFilterLibraryDockWidget*     m_FilterLibrary;
    QDream3DDocumentsDockWidget*  m_FavoritePipelines;
    QPrebuiltPipelinesDockWidget* m_PrebuiltPipelines;
    FilterListWidget*             m_FilterListWidget;

    DREAM3D_UI(const DREAM3D_UI&);    // Copy Constructor Not Implemented
    void operator=(const DREAM3D_UI&);  // Operator '=' Not Implemented

};

#endif /* _DREAM3D_UI_H_ */
