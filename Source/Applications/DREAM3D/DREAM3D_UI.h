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
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QToolBar>


#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Plugin/IDREAM3DPlugin.h"
#include "DREAM3DWidgetsLib/FilterWidgetManager.h"


//-- UIC generated Header
#include "ui_DREAM3D_UI.h"


class IDREAM3DPlugin;
class FilterLibraryDockWidget;
class FavoritesDockWidget;
class PrebuiltPipelinesDockWidget;
class FilterListWidget;
class DREAM3DUpdateCheckDialog;
class UpdateCheckData;
class UpdateCheck;
class HelpDialog;
class QToolButton;
class AboutDREAM3D;


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
    DREAM3D_UI(QWidget* parent = 0);
    virtual ~DREAM3D_UI();


    /**
     * @brief setLoadedPlugins This will set the plugins that have already been loaded by another mechanism. The plugins are NOT
     * deleted by this class and the unloading and clean up of the plugin pointers is the responsibility of the caller.
     * @param plugins The plugins that adhere to the IDREAM3DPlugin
     */
    void setLoadedPlugins(QVector<IDREAM3DPlugin*> plugins);

    /**
     * @brief displayHelp
     * @param helpFile
     */
    void displayHelp(QString helpFile);

    /**
     * @brief versionCheckReply
     */
    void versionCheckReply(UpdateCheckData*);

    /**
     * @brief Writes the preferences to the users pref file
     */
    void writeSettings();


  protected slots:

    /* Menu Slots */

    // File Menu
    void on_actionImportPipeline_triggered();
    void on_actionExportPipeline_triggered();
    void on_actionExit_triggered();

    //Pipeline Menu
//    void on_actionUpdatePipeline_triggered();
//    void on_actionSaveAsNewPipeline_triggered();
//    void on_actionAppendToExistingPipeline_triggered();
    void on_actionClearPipeline_triggered();

    // Filter Menu
//    void on_actionCopyCurrentFilter_triggered();
//    void on_actionPasteCopiedFilter_triggered();
//    void on_actionRemoveCurrentFilter_triggered();

    //View Menu
    // These all need to take a bool b  argument
    void on_actionShow_Filter_Library_triggered(bool b);
    void on_actionShow_Filter_List_triggered(bool b);
    void on_actionShow_Prebuilt_Pipelines_triggered(bool b);
    void on_actionShow_Favorites_triggered(bool b);
    void on_actionShow_Issues_triggered(bool b);

    // Help Menu
    void on_actionCheck_For_Updates_triggered();
    void on_actionLicense_Information_triggered();
    void on_actionAbout_DREAM3D_triggered();
    void on_actionPlugin_Information_triggered();
    void on_actionShow_User_Manual_triggered();

    // Buttons and other widgets that send signals that we want to catch
    void on_startPipelineBtn_clicked();

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

    /**
     * @brief pipelineFileLoaded
     * @param file
     * @param format
     * @param append
     */
    void pipelineFileLoaded(QString file, QSettings::Format format, bool append);


    void pipelineDidFinish();

    void processPipelineMessage(const PipelineMessage& msg);

    /**
     * @brief on_pipelineViewWidget_pipelineChanged AUTO HOOKUP by name
     */
    void on_pipelineViewWidget_pipelineChanged();
    void on_pipelineViewWidget_pipelineTitleUpdated(QString title);
    void on_pipelineViewWidget_pipelineIssuesCleared();
    void on_pipelineViewWidget_pipelineHasNoErrors();
    void on_pipelineViewWidget_pipelineFileDropped(QString& file);


    // Our Signals that we can emit custom for this class
  signals:
    void parentResized();

    /**
     * @brief A signal that is emitted when we want to cancel a process
     */
    void pipelineCanceled();

    /**
     * @brief pipelineStarted
     */
    void pipelineStarted();

  protected:

    /**
     * @brief populateMenus This is a planned API that plugins would use to add Menus to the main application
     * @param plugin
     */
    void populateMenus(QObject* plugin);

    /**
     * @brief setupPipelineMenu This will collect specific menu items and allow them to be used from a contextual menu
     * assigned to the various Pipeline/Filter/Favorites type items
     */
    void setupPipelineContextMenu();

    void setupViewMenu();

    /**
     * @brief Implements the CloseEvent to Quit the application and write settings
     * to the preference file
     */
    void closeEvent(QCloseEvent* event);

    /**
     * @brief Drag and drop implementation
     */
    void dragEnterEvent(QDragEnterEvent*);

    /**
     * @brief Drag and drop implementation
     */
    void dropEvent(QDropEvent*);

    /**
     *
     * @param prefs
     */
    void writeWindowSettings(QSettings& prefs);
    void writeVersionCheckSettings(QSettings& prefs);
    void writeSearchListSettings(QSettings& prefs, FilterListDockWidget* dw);


    void readWindowSettings();
    void readVersionSettings();
    void readLastPipeline();
    void readSearchListSettings(QSettings& prefs, FilterListDockWidget* dw);

    void checkForUpdatesAtStartup();

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    void setupGui();

    /**
     * @brief DREAM3D_UI::setupDockWidget
     * @param prefs
     * @param dw
     */
    void readDockWidgetSettings(QSettings& prefs, QDockWidget* dw);

    /**
     * @brief writeDockWidgetSettings
     * @param prefs
     * @param dw
     */
    void writeDockWidgetSettings(QSettings& prefs, QDockWidget* dw);


    void makeStatusBarButton(QString text, QDockWidget* dockWidget, QToolButton* btn, int index);

    void updateAndSyncDockWidget(QAction* action, QDockWidget* dock, QToolButton* btn, bool b);

    /**
     * @brief Checks the currently open file for changes that need to be saved
     * @return
     */
    qint32 checkDirtyDocument();

    /**
     * @brief Over ride the resize event
     * @param event The event to process
     */
    void resizeEvent ( QResizeEvent* event );

  private:
    QThread*                    m_WorkerThread;
    IDREAM3DPlugin*     m_ActivePlugin;
    QVector<IDREAM3DPlugin*> m_LoadedPlugins;

    HelpDialog*                 m_HelpDialog;

    QSharedPointer<UpdateCheck>   m_UpdateCheck;

    QThread*                    m_UpdateCheckThread;
    FilterManager*      m_FilterManager;
    FilterWidgetManager*  m_FilterWidgetManager;

    FilterPipeline::Pointer      m_PipelineInFlight;

    QToolButton*         m_FilterListBtn;
    QToolButton*        m_FilterLibraryBtn;
    QToolButton*        m_FavoritesBtn;
    QToolButton*        m_PrebuiltBtn;
    QToolButton*        m_IssuesBtn;
    bool                m_ShouldRestart;


    static QString    m_OpenDialogLastDirectory;

    DREAM3D_UI(const DREAM3D_UI&);    // Copy Constructor Not Implemented
    void operator=(const DREAM3D_UI&);  // Operator '=' Not Implemented

};

#endif /* _DREAM3D_UI_H_ */

