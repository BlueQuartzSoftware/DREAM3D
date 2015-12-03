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
#include "MacDREAM3DApplication.h"

#if ! defined(_MSC_VER)
#include <unistd.h>
#endif

#include <iostream>

#include <QtCore/QTime>
#include <QtCore/QPluginLoader>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <QtGui/QDesktopServices>
#include <QtGui/QBitmap>
#include <QtGui/QFileOpenEvent>

#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/PluginProxy.h"

#include "QtSupportLib/QRecentFileList.h"
#include "QtSupportLib/DREAM3DHelpUrlGenerator.h"
#include "QtSupportLib/ApplicationAboutBoxDialog.h"

#include "DREAM3DWidgetsLib/FilterWidgetManager.h"
#include "DREAM3DWidgetsLib/Widgets/DREAM3DUpdateCheckDialog.h"
#ifdef DREAM3D_USE_QtWebEngine
#include "Applications/Common/DREAM3DUserManualDialog.h"
#endif

#include "Applications/DREAM3D/DREAM3D_UI.h"
#include "Applications/DREAM3D/AboutDREAM3D.h"
#include "Applications/DREAM3D/AboutPlugins.h"
#include "Applications/DREAM3D/DREAM3DConstants.h"
#include "Applications/DREAM3D/DREAM3DToolbox.h"
#include "Applications/DREAM3D/DREAM3DToolboxMenu.h"
#include "Applications/DREAM3D/DREAM3DMenuItems.h"

#include "DSplashScreen.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_MacDREAM3DApplication.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MacDREAM3DApplication::MacDREAM3DApplication(int& argc, char** argv) :
DREAM3DApplication(argc, argv),
m_GlobalMenu(NULL)
{
  // Connection to update the recent files list on all windows when it changes
  QRecentFileList* recentsList = QRecentFileList::instance();
  connect(recentsList, SIGNAL(fileListChanged(const QString&)),
    this, SLOT(updateRecentFileList(const QString&)));

  // Initialize the global menu
  createGlobalMenu();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MacDREAM3DApplication::~MacDREAM3DApplication()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MacDREAM3DApplication::updateRecentFileList(const QString& file)
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();
  QMenu* recentFilesMenu = menuItems->getMenuRecentFiles();
  QAction* clearRecentFilesAction = menuItems->getActionClearRecentFiles();

  // Clear the Recent Items Menu
  recentFilesMenu->clear();

  // Get the list from the static object
  QStringList files = QRecentFileList::instance()->fileList();
  foreach(QString file, files)
  {
    QAction* action = new QAction(recentFilesMenu);
    action->setText(QRecentFileList::instance()->parentAndFileName(file));
    action->setData(file);
    action->setVisible(true);
    recentFilesMenu->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
  }

  recentFilesMenu->addSeparator();
  recentFilesMenu->addAction(clearRecentFilesAction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MacDREAM3DApplication::on_actionClearRecentFiles_triggered()
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();

  QMenu* recentFilesMenu = menuItems->getMenuRecentFiles();
  QAction* clearRecentFilesAction = menuItems->getActionClearRecentFiles();

  // Clear the Recent Items Menu
  recentFilesMenu->clear();
  recentFilesMenu->addSeparator();
  recentFilesMenu->addAction(clearRecentFilesAction);

  // Clear the actual list
  QRecentFileList* recents = QRecentFileList::instance();
  recents->clear();

  // Write out the empty list
  DREAM3DSettings prefs;
  recents->writeList(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MacDREAM3DApplication::unregisterDREAM3DWindow(DREAM3D_UI* window)
{
  m_DREAM3DInstances.removeAll(window);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MacDREAM3DApplication::on_pipelineViewContextMenuRequested(const QPoint& pos)
{
  PipelineViewWidget* pipelineView = m_ActiveWindow->getPipelineViewWidget();
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();
  QMenu menu;

  menu.addAction(menuItems->getActionClearPipeline());

  if (NULL != m_ActiveWindow)
  {
    menu.exec(pipelineView->mapToGlobal(pos));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MacDREAM3DApplication::on_bookmarksDockContextMenuRequested(const QPoint& pos)
{
  BookmarksTreeView* bookmarksTreeView = m_ActiveWindow->getBookmarksToolboxWidget()->getBookmarksTreeView();

  QModelIndex index = bookmarksTreeView->indexAt(pos);

  QPoint mapped;
  if (index.isValid())
  {
    // Note: We must map the point to global from the viewport to
    // account for the header.
    mapped = bookmarksTreeView->viewport()->mapToGlobal(pos);
  }
  else
  {
    index = QModelIndex();
    mapped = bookmarksTreeView->mapToGlobal(pos);
  }

  BookmarksModel* model = BookmarksModel::Instance();
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();

  QAction* m_ActionAddPipeline;
  QAction* m_ActionNewFolder;
  QAction* m_ActionRenamePipeline;
  QAction* m_ActionRemovePipeline;
  QAction* m_ActionLocateFile;
  QAction* m_ActionShowBookmarkInFileSystem;

  m_ActionAddPipeline = menuItems->getActionAddBookmark();
  m_ActionNewFolder = menuItems->getActionNewFolder();
  m_ActionRenamePipeline = menuItems->getActionRenamePipeline();
  m_ActionRemovePipeline = menuItems->getActionRemovePipeline();
  m_ActionLocateFile = menuItems->getActionLocateFile();
  m_ActionShowBookmarkInFileSystem = menuItems->getActionShowBookmarkInFileSystem();

  QMenu menu;
  if (index.isValid() == false)
  {
    menu.addAction(m_ActionAddPipeline);
    {
      QAction* separator = new QAction(this);
      separator->setSeparator(true);
      menu.addAction(separator);
    }
    menu.addAction(m_ActionNewFolder);
  }
  else
  {
    QModelIndex actualIndex = model->index(index.row(), BookmarksItem::Path, index.parent());
    QString path = actualIndex.data().toString();
    if (path.isEmpty() == false)
    {
      bool itemHasErrors = model->data(actualIndex, Qt::UserRole).value<bool>();
      if (itemHasErrors == true)
      {
        menu.addAction(m_ActionLocateFile);

        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }

        m_ActionRemovePipeline->setText("Remove Bookmark");
        menu.addAction(m_ActionRemovePipeline);
      }
      else
      {
        menu.addAction(m_ActionAddPipeline);

        m_ActionRenamePipeline->setText("Rename Bookmark");
        menu.addAction(m_ActionRenamePipeline);

        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }

        m_ActionRemovePipeline->setText("Remove Bookmark");
        menu.addAction(m_ActionRemovePipeline);

        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }

        menu.addAction(m_ActionShowBookmarkInFileSystem);
      }
    }
    else if (path.isEmpty())
    {
      menu.addAction(m_ActionAddPipeline);

      m_ActionRenamePipeline->setText("Rename Folder");
      menu.addAction(m_ActionRenamePipeline);

      {
        QAction* separator = new QAction(this);
        separator->setSeparator(true);
        menu.addAction(separator);
      }

      m_ActionRemovePipeline->setText("Remove Folder");
      menu.addAction(m_ActionRemovePipeline);

      {
        QAction* separator = new QAction(this);
        separator->setSeparator(true);
        menu.addAction(separator);
      }

      menu.addAction(m_ActionNewFolder);
    }
  }

  menu.exec(mapped);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MacDREAM3DApplication::activeWindowChanged(DREAM3D_UI* instance)
{
  if (instance->isActiveWindow())
  {
    m_ActiveWindow = instance;

    /* If the active signal got fired and there is now only one window,
     * this means that the first window has been opened.
     * Enable menu items. */
    if (m_DREAM3DInstances.size() == 1)
    {
      toggleGlobalMenuItems(true);
    }

    // Set the active window's menu state
    if (isCurrentlyRunning(m_ActiveWindow) == true)
    {
      toPipelineRunningState();
    }
    else
    {
      toPipelineIdleState();
    }
  }
  else if (m_DREAM3DInstances.size() <= 0)
  {
    /* If the inactive signal got fired and there are no more windows,
     * this means that the last window has been closed.
     * Disable menu items. */
    m_ActiveWindow = NULL;
    toggleGlobalMenuItems(false);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MacDREAM3DApplication::toggleGlobalMenuItems(bool value)
{
//  m_GlobalMenu->toggleMenuChildren(m_GlobalMenu->getViewMenu(), value);
//  m_GlobalMenu->toggleMenuChildren(m_GlobalMenu->getBookmarksMenu(), value);
//  m_GlobalMenu->toggleMenuChildren(m_GlobalMenu->getPipelineMenu(), value);

//  m_GlobalMenu->getSave()->setEnabled(value);
//  m_GlobalMenu->getSaveAs()->setEnabled(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MacDREAM3DApplication::toPipelineRunningState()
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();
  menuItems->getActionClearPipeline()->setDisabled(true);

  DREAM3D_UI* runningInstance = qobject_cast<DREAM3D_UI*>(sender());
  if (NULL != runningInstance)
  {
    m_CurrentlyRunningInstances.insert(runningInstance);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MacDREAM3DApplication::toPipelineIdleState()
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();
  menuItems->getActionClearPipeline()->setEnabled(true);

  DREAM3D_UI* runningInstance = qobject_cast<DREAM3D_UI*>(sender());
  if (NULL != runningInstance)
  {
    m_CurrentlyRunningInstances.remove(runningInstance);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MacDREAM3DApplication::createGlobalMenu()
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();

  QMenu* menuFile = menuItems->getMenuFile();
  QMenu* menuView = menuItems->getMenuView();
  QMenu* menuToolbox = menuItems->getMenuToolbox();
  QMenu* menuBookmarks = menuItems->getMenuBookmarks();
  QMenu* menuPipeline = menuItems->getMenuPipeline();
  QMenu* menuHelp = menuItems->getMenuHelp();
  QMenu* menuAdvanced = menuItems->getMenuAdvanced();
  QAction* actionNew = menuItems->getActionNew();
  QAction* actionOpen = menuItems->getActionOpen();
  QAction* actionSave = menuItems->getActionSave();
  QAction* actionSaveAs = menuItems->getActionSaveAs();
  QMenu* menuRecentFiles = menuItems->getMenuRecentFiles();
  QAction* actionClearRecentFiles = menuItems->getActionClearRecentFiles();
  QAction* actionExit = menuItems->getActionExit();
  QAction* actionClearPipeline = menuItems->getActionClearPipeline();
  QAction* actionShowDREAM3DHelp = menuItems->getActionShowDREAM3DHelp();
  QAction* actionCheckForUpdates = menuItems->getActionCheckForUpdates();
  QAction* actionClearCache = menuItems->getActionClearCache();
  QAction* actionAboutDREAM3D = menuItems->getActionAboutDREAM3D();
  QAction* actionPluginInformation = menuItems->getActionPluginInformation();
  QAction* actionShowIssues = menuItems->getActionShowIssues();
  QAction* actionShowToolbox = menuItems->getActionShowToolbox();
  QAction* actionShowFilterLibrary = menuItems->getActionShowFilterLibrary();
  QAction* actionShowFilterList = menuItems->getActionShowFilterList();
  QAction* actionShowBookmarks = menuItems->getActionShowBookmarks();
  QAction* actionAddBookmark = menuItems->getActionAddBookmark();
  QAction* actionNewFolder = menuItems->getActionNewFolder();

  m_GlobalMenu = new QMenuBar(NULL);

  // Create File Menu
  m_GlobalMenu->addMenu(menuFile);
  menuFile->addAction(actionNew);
  menuFile->addAction(actionOpen);
  menuFile->addSeparator();
  menuFile->addAction(actionSave);
  menuFile->addAction(actionSaveAs);
  menuFile->addSeparator();
  menuFile->addAction(menuRecentFiles->menuAction());
  menuRecentFiles->addSeparator();
  menuRecentFiles->addAction(actionClearRecentFiles);
  menuFile->addSeparator();
  menuFile->addAction(actionExit);

  // Create View Menu
  m_GlobalMenu->addMenu(menuView);
  menuView->addAction(actionShowToolbox);
  menuView->addMenu(menuToolbox);
  menuToolbox->addAction(actionShowFilterList);
  menuToolbox->addAction(actionShowFilterLibrary);
  menuToolbox->addAction(actionShowBookmarks);
  menuView->addSeparator();
  menuView->addAction(actionShowIssues);

  // Create Bookmarks Menu
  m_GlobalMenu->addMenu(menuBookmarks);
  menuBookmarks->addAction(actionAddBookmark);
  menuBookmarks->addSeparator();
  menuBookmarks->addAction(actionNewFolder);

  // Create Pipeline Menu
  m_GlobalMenu->addMenu(menuPipeline);
  menuPipeline->addAction(actionClearPipeline);

  // Create Help Menu
  m_GlobalMenu->addMenu(menuHelp);
  menuHelp->addAction(actionShowDREAM3DHelp);
  menuHelp->addSeparator();
  menuHelp->addAction(actionCheckForUpdates);
  menuHelp->addSeparator();
  menuHelp->addMenu(menuAdvanced);
  menuAdvanced->addAction(actionClearCache);
  menuHelp->addSeparator();
  menuHelp->addAction(actionAboutDREAM3D);
  menuHelp->addAction(actionPluginInformation);
}
