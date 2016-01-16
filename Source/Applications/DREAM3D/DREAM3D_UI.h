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



#ifndef _DREAM3D_UI_H_
#define _DREAM3D_UI_H_


//-- Qt Includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QVector>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QToolBar>

#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "DREAM3DWidgetsLib/FilterWidgetManager.h"

#include "QtSupportLib/DREAM3DSettings.h"


//-- UIC generated Header
#include "ui_DREAM3D_UI.h"


class ISIMPLibPlugin;
class FilterLibraryToolboxWidget;
class BookmarksToolboxWidget;
class PrebuiltsToolboxWidget;
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
     * @param plugins The plugins that adhere to the ISIMPLibPlugin
     */
    void setLoadedPlugins(QVector<ISIMPLibPlugin*> plugins);

    /**
    * @brief getPipelineViewWidget
    * @param
    */
    PipelineViewWidget* getPipelineViewWidget();

    /**
    * @brief getBookmarksToolboxWidget
    * @param
    */
    BookmarksToolboxWidget* getBookmarksToolboxWidget();

    /**
    * @brief getFilterListToolboxWidget
    * @param
    */
    FilterListToolboxWidget* getFilterListToolboxWidget();

    /**
    * @brief getFilterLibraryToolboxWidget
    * @param
    */
    FilterLibraryToolboxWidget* getFilterLibraryToolboxWidget();

    /**
    * @brief getFilterListToolboxWidget
    * @param
    */
    IssuesDockWidget* getIssuesDockWidget();

    /**
    * @brief setOpenedFilePath
    * @param path
    */
    void setOpenDialogLastDirectory(const QString& path);

    /**
     * @brief updateAndSyncDockWidget
     * @param action
     * @param dock
     * @param b
     */
    void updateAndSyncDockWidget(QAction* action, QDockWidget* dock, bool b);

    /**
    * @brief Reads the preferences from the users pref file
    */
    void readSettings();

    /**
     * @brief Writes the preferences to the users pref file
     */
    void writeSettings();

    /**
    * @brief Checks if this the first run of DREAM3D
    * and if so, displays an informative dialog box.
    */
    void checkFirstRun();

    /**
     * @brief savePipeline Helper function that saves the pipeline
     */
    bool savePipeline();

    /**
     * @brief savePipelineAs Helper function that saves the pipeline
     */
    bool savePipelineAs();

  public slots:

    void clearPipeline();

    /**
    * @brief setOpenedFilePath
    * @param filePath
    */
    void setOpenedFilePath(const QString& filePath);

    /**
    * @brief setFilterBeingDragged
    * @param w
    */
    void setStatusBarMessage(const QString& msg);

    /**
    * @brief versionCheckReply
    */
    void versionCheckReply(UpdateCheckData*);

    /**
     * @brief showFilterHelp
     * @param className
     */
    void showFilterHelp(const QString& className);

    /**
     * @brief showFilterHelp
     * @param url
     */
    void showFilterHelpUrl(const QUrl& url);

  protected slots:

    // Buttons and other widgets that send signals that we want to catch
    void on_startPipelineBtn_clicked();

    /**
     * @brief pipelineDidFinish
     */
    void pipelineDidFinish();

    /**
     * @brief processPipelineMessage
     * @param msg
     */
    void processPipelineMessage(const PipelineMessage& msg);

    /**
     * @brief on_pipelineViewWidget_pipelineChanged AUTO HOOKUP by name
     */
    void on_pipelineViewWidget_pipelineChanged();
    void on_pipelineViewWidget_pipelineIssuesCleared();
    void on_pipelineViewWidget_pipelineHasNoErrors();
    void on_pipelineViewWidget_pipelineOpened(QString& file, const bool& setOpenedFilePath, const bool& changeTitle);

    /**
    * @brief setFilterInputWidget
    * @param widget
    */
    void setFilterInputWidget(FilterInputWidget* widget);

    /**
    * @brief clearFilterInputWidget
    */
    void clearFilterInputWidget();

    /**
    * @brief markDocumentAsDirty
    */
    void markDocumentAsDirty();

    // Our Signals that we can emit custom for this class
  signals:

    /**
    * @brief bookmarkNeedsToBeAdded
    */
    void bookmarkNeedsToBeAdded(const QString& filePath, const QModelIndex& parent);

    void parentResized();

    /**
     * @brief A signal that is emitted when we want to cancel a process
     */
    void pipelineCanceled();

    /**
     * @brief pipelineStarted
     */
    void pipelineStarted();

    /**
    * @brief pipelineFinished
    */
    void pipelineFinished();

    /**
    * @brief dream3dWindowChangedState
    */
    void dream3dWindowChangedState(DREAM3D_UI* self);

  protected:

    /**
     * @brief populateMenus This is a planned API that plugins would use to add Menus to the main application
     * @param plugin
     */
    void populateMenus(QObject* plugin);

    /**
    * @brief
    */
    void connectSignalsSlots();

    /**
    * @brief
    */
    void disconnectSignalsSlots();

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

    void changeEvent(QEvent* event);

    /**
     *
     * @param prefs
     */
    void writeWindowSettings(DREAM3DSettings* prefs);
    void writeVersionCheckSettings(DREAM3DSettings* prefs);

    void readWindowSettings(DREAM3DSettings* prefs);
    void readVersionSettings(DREAM3DSettings* prefs);

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
    void readDockWidgetSettings(DREAM3DSettings* prefs, QDockWidget* dw);

    /**
     * @brief writeDockWidgetSettings
     * @param prefs
     * @param dw
     */
    void writeDockWidgetSettings(DREAM3DSettings* prefs, QDockWidget* dw);

    /**
     * @brief Checks the currently open file for changes that need to be saved
     * @return QMessageBox::StandardButton
     */
    QMessageBox::StandardButton checkDirtyDocument();

    /**
     * @brief Over ride the resize event
     * @param event The event to process
     */
    void resizeEvent ( QResizeEvent* event );

  private:
    QThread*                              m_WorkerThread;
    ISIMPLibPlugin*                       m_ActivePlugin;
    QVector<ISIMPLibPlugin*>              m_LoadedPlugins;

    QSharedPointer<UpdateCheck>           m_UpdateCheck;

    QThread*                              m_UpdateCheckThread;
    FilterManager*                        m_FilterManager;
    FilterWidgetManager*                  m_FilterWidgetManager;

    FilterPipeline::Pointer               m_PipelineInFlight;
#if !defined(Q_OS_MAC)
    QMenuBar*                             m_InstanceMenuBar;
#endif
    bool                                  m_ShouldRestart;

    QString                               m_OpenedFilePath;
    static QString                        m_OpenDialogLastDirectory;

    /**
    * @brief Updates the "first run" variable in the preferences file
    */
    void updateFirstRun();

    DREAM3D_UI(const DREAM3D_UI&);    // Copy Constructor Not Implemented
    void operator=(const DREAM3D_UI&);  // Operator '=' Not Implemented

};

#endif /* _DREAM3D_UI_H_ */

