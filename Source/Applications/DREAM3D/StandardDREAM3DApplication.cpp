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
#include "StandardDREAM3DApplication.h"

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
#include "moc_StandardDREAM3DApplication.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StandardDREAM3DApplication::StandardDREAM3DApplication(int& argc, char** argv) :
  DREAM3DApplication(argc, argv)
{
  // Create the menu
  DREAM3DToolboxMenu* toolboxMenu = new DREAM3DToolboxMenu();
  m_Toolbox->setToolboxMenu(toolboxMenu);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StandardDREAM3DApplication::~StandardDREAM3DApplication()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardDREAM3DApplication::updateRecentFileList(const QString& file)
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();

  for (int i = 0; i < m_DREAM3DInstances.size(); i++)
  {
    DREAM3D_UI* window = m_DREAM3DInstances[i];

    if (NULL != window)
    {
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
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardDREAM3DApplication::on_actionClearRecentFiles_triggered()
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();

  for (int i = 0; i < m_DREAM3DInstances.size(); i++)
  {
    DREAM3D_UI* window = m_DREAM3DInstances[i];

    if (NULL != window)
    {
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
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardDREAM3DApplication::on_pipelineViewContextMenuRequested(const QPoint& pos)
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();
  PipelineViewWidget* pipelineView = m_ActiveWindow->getPipelineViewWidget();
  QMenu menu;

  if (NULL != m_ActiveWindow)
  {
    menu.addAction(menuItems->getActionClearPipeline());
    menu.exec(pipelineView->mapToGlobal(pos));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardDREAM3DApplication::on_bookmarksDockContextMenuRequested(const QPoint& pos)
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();
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

  QAction* actionAddBookmark = menuItems->getActionAddBookmark();
  QAction* actionNewFolder = menuItems->getActionNewFolder();
  QAction* actionRenameBookmark = menuItems->getActionRenamePipeline();
  QAction* actionRemoveBookmark = menuItems->getActionRemovePipeline();
  QAction* actionLocateFile = menuItems->getActionLocateFile();
  QAction* actionShowBookmarkInFileSystem = menuItems->getActionShowBookmarkInFileSystem();

  QMenu menu;
  if (index.isValid() == false)
  {
    menu.addAction(actionAddBookmark);
    {
      QAction* separator = new QAction(this);
      separator->setSeparator(true);
      menu.addAction(separator);
    }
    menu.addAction(actionNewFolder);
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
        menu.addAction(actionLocateFile);

        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }

        actionRemoveBookmark->setText("Remove Bookmark");
        menu.addAction(actionRemoveBookmark);
      }
      else
      {
        menu.addAction(actionAddBookmark);

        actionRenameBookmark->setText("Rename Bookmark");
        menu.addAction(actionRenameBookmark);

        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }

        actionRemoveBookmark->setText("Remove Bookmark");
        menu.addAction(actionRemoveBookmark);

        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }

        menu.addAction(actionShowBookmarkInFileSystem);
      }
    }
    else if (path.isEmpty())
    {
      menu.addAction(actionAddBookmark);

      actionRenameBookmark->setText("Rename Folder");
      menu.addAction(actionRenameBookmark);

      {
        QAction* separator = new QAction(this);
        separator->setSeparator(true);
        menu.addAction(separator);
      }

      actionRemoveBookmark->setText("Remove Folder");
      menu.addAction(actionRemoveBookmark);

      {
        QAction* separator = new QAction(this);
        separator->setSeparator(true);
        menu.addAction(separator);
      }

      menu.addAction(actionNewFolder);
    }
  }

  menu.exec(mapped);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardDREAM3DApplication::dream3dWindowChanged(DREAM3D_UI* instance)
{
  if (instance->isActiveWindow())
  {
    DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();

    m_ActiveWindow = instance;

    // Update the issues menu item with the correct value
    QAction* issuesToggle = m_ActiveWindow->getIssuesDockWidget()->toggleViewAction();
    menuItems->getActionShowIssues()->setChecked(issuesToggle->isChecked());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardDREAM3DApplication::unregisterDREAM3DWindow(DREAM3D_UI* window)
{
  m_DREAM3DInstances.removeAll(window);
  if (m_DREAM3DInstances.size() <= 0)
  {
    quit();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenuBar* StandardDREAM3DApplication::getDREAM3DMenuBar()
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();

  QMenuBar* menuBar = new QMenuBar();
  QMenu* menuFile = menuItems->getMenuFile();
  QMenu* menuView = menuItems->getMenuView();
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


  // Create File Menu
  menuBar->addMenu(menuFile);
  menuFile->addAction(actionNew);
  menuFile->addAction(actionOpen);
  menuFile->addSeparator();
  menuFile->addAction(actionSave);
  menuFile->addAction(actionSaveAs);
  menuFile->addSeparator();
  menuFile->addAction(menuRecentFiles->menuAction());
  menuFile->addSeparator();
  menuFile->addAction(actionExit);
  menuRecentFiles->addSeparator();
  menuRecentFiles->addAction(actionClearRecentFiles);

  // Create View Menu
  menuBar->addMenu(menuView);
  menuView->addAction(actionShowIssues);
  menuView->addAction(actionShowToolbox);

  // Create Pipeline Menu
  menuBar->addMenu(menuPipeline);
  menuPipeline->addAction(actionClearPipeline);

  // Create Help Menu
  menuBar->addMenu(menuHelp);
  menuHelp->addAction(actionShowDREAM3DHelp);
  menuHelp->addSeparator();
  menuHelp->addAction(actionCheckForUpdates);
  menuHelp->addSeparator();
  menuHelp->addMenu(menuAdvanced);
  menuAdvanced->addAction(actionClearCache);
  menuHelp->addSeparator();
  menuHelp->addAction(actionAboutDREAM3D);
  menuHelp->addAction(actionPluginInformation);

  return menuBar;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenuBar* StandardDREAM3DApplication::getToolboxMenuBar()
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();

  QMenuBar* menuBar = new QMenuBar();
  QMenu* menuView = new QMenu("View");
  QMenu* menuBookmarks = menuItems->getMenuBookmarks();
  QAction* actionShowFilterLibrary = menuItems->getActionShowFilterLibrary();
  QAction* actionShowFilterList = menuItems->getActionShowFilterList();
  QAction* actionShowBookmarks = menuItems->getActionShowBookmarks();
  QAction* actionAddBookmark = menuItems->getActionAddBookmark();
  QAction* actionNewFolder = menuItems->getActionNewFolder();

  // Add the actions to their respective menus
  menuBar->addAction(menuView->menuAction());
  menuBar->addAction(menuBookmarks->menuAction());

  menuView->addAction(actionShowFilterList);
  menuView->addAction(actionShowFilterLibrary);
  menuView->addAction(actionShowBookmarks);

  menuBookmarks->addAction(actionAddBookmark);
  menuBookmarks->addSeparator();
  menuBookmarks->addAction(actionNewFolder);

  return menuBar;
}

