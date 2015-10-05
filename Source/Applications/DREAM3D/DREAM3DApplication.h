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

#ifndef _DREAM3DApplication_H_
#define _DREAM3DApplication_H_

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QSet>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

#include "Applications/DREAM3D/DREAM3DMenu.h"

#include "SIMPLib/SIMPLib.h"
//#include "SIMPLib/Plugin/ISIMPLibPlugin.h"

#define dream3dApp (static_cast<DREAM3DApplication *>(qApp))

class DSplashScreen;
class DREAM3D_UI;
class QPluginLoader;
class ISIMPLibPlugin;

class DREAM3DApplication : public QApplication
{
    Q_OBJECT

  public:
    DREAM3DApplication(int& argc, char** argv);
    virtual ~DREAM3DApplication();

    bool initialize(int argc, char* argv[]);

    QMap<DREAM3D_UI*, QMenu*> getDREAM3DInstanceMap();

    void registerDREAM3DWindow(DREAM3D_UI* window, QMenu* viewMenu);
    void unregisterDREAM3DWindow(DREAM3D_UI* window);

    DREAM3D_UI* getNewDREAM3DInstance();

    bool isCurrentlyRunning(DREAM3D_UI* instance);

    /**
     * @brief event
     * @param event
     * @return
     */
    bool event(QEvent* event);

  public slots:

    void newInstanceFromFile(const QString& filePath, const bool& setOpenedFilePath, const bool& addToRecentFiles);


  protected:

    void toggleGlobalMenuItems(bool value);

  protected slots:

    /**
    * @brief Updates the QMenu 'Recent Files' with the latest list of files. This
    * should be connected to the Signal QRecentFileList->fileListChanged
    * @param file The newly added file.
    */
    void updateRecentFileList(const QString& file);

    /**
    * @brief activeWindowChanged
    */
    void activeWindowChanged(DREAM3D_UI* instance);

    // DREAM3D_UI slots
    void openRecentFile();

    void on_actionShowFilterList_triggered(bool visible);
    void on_actionShowFilterLibrary_triggered(bool visible);
    void on_actionShowBookmarks_triggered(bool visible);
    void on_actionShowPrebuiltPipelines_triggered(bool visible);
    void on_actionShowIssues_triggered(bool visible);

    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();

    void on_actionAddBookmark_triggered();
    void on_actionNewFolder_triggered();
    void on_actionRenamePipeline_triggered();
    void on_actionRemovePipeline_triggered();
    void on_actionShowBookmarkInFileSystem_triggered();
    void on_actionShowPrebuiltInFileSystem_triggered();
    void on_actionLocateFile_triggered();
    void on_actionClearPipeline_triggered();
    void on_actionClearCache_triggered();

    void on_pipelineViewContextMenuRequested(const QPoint&);
    void on_bookmarksDockContextMenuRequested(const QPoint&);
    void on_prebuiltsDockContextMenuRequested(const QPoint&);

    void on_actionClearRecentFiles_triggered();
    void on_actionCloseWindow_triggered();
    void on_actionExit_triggered();
    void on_actionShowDREAM3DHelp_triggered();
    void on_actionCheckForUpdates_triggered();
    void on_actionPluginInformation_triggered();
    void on_actionAboutDREAM3D_triggered();

    void toPipelineRunningState();

    void toPipelineIdleState();

  private:

    // This map stores each DREAM3D instance with its accompanying "View" menu
    QMap<DREAM3D_UI*, QMenu*>               m_DREAM3DInstanceMap;

    // This is the set of DREAM3D instances that are currently running a pipeline
    QSet<DREAM3D_UI*>                       m_CurrentlyRunningInstances;

    // The currently active DREAM3D instance
    DREAM3D_UI*                             m_ActiveWindow;

    // The global menu (used on Mac OS X only)
#if defined(Q_OS_MAC)
    DREAM3DMenu*                            m_GlobalMenu;
#endif

    /* Used on Mac OS X when there are no DREAM3D instances instantiated,
    * but the application is still running. */
    QMenu*                                  m_PlaceholderViewMenu;

    QString                                 m_OpenDialogLastDirectory;

    bool                                    show_splash;
    DSplashScreen*                          Splash;

    QVector<QPluginLoader*>                 m_PluginLoaders;

    QVector<ISIMPLibPlugin*> loadPlugins();

    QMenu* createPlaceholderViewMenu();

    DREAM3DApplication(const DREAM3DApplication&); // Copy Constructor Not Implemented
    void operator=(const DREAM3DApplication&); // Operator '=' Not Implemented
};

#endif /* _DREAM3DApplication_H */

