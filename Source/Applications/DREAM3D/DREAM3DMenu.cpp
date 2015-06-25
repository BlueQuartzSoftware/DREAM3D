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

#include "DREAM3DMenu.h"

#include "Applications/DREAM3D/DREAM3DApplication.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DMenu::DREAM3DMenu()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DMenu::~DREAM3DMenu()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DMenu::initialize()
{
  m_GlobalMenu = new QMenuBar(NULL);
  m_GlobalMenu->setObjectName(QStringLiteral("m_GlobalMenu"));
  m_GlobalMenu->setGeometry(QRect(0, 0, 1104, 21));

  m_MenuFile = new QMenu(m_GlobalMenu);
  m_MenuFile->setObjectName(QStringLiteral("menuFile"));
  m_MenuRecentFiles = new QMenu(m_MenuFile);
  m_MenuRecentFiles->setObjectName(QStringLiteral("menu_RecentFiles"));
  m_MenuView = new QMenu(m_GlobalMenu);
  m_MenuView->setObjectName(QStringLiteral("menuView"));
  m_MenuBookmarks = new QMenu(m_GlobalMenu);
  m_MenuBookmarks->setObjectName(QStringLiteral("menuBookmarks"));
  m_MenuHelp = new QMenu(m_GlobalMenu);
  m_MenuHelp->setObjectName(QStringLiteral("menuHelp"));
  m_MenuPipeline = new QMenu(m_GlobalMenu);
  m_MenuPipeline->setObjectName(QStringLiteral("menuPipeline"));

#if 0
  //  m_FilterListBtn = new QToolButton(this);
  //  makeStatusBarButton("Filters", filterListDockWidget, m_FilterListBtn, 0);
  menuView->removeAction(actionShow_Filter_List);
  delete actionShow_Filter_List;
  actionShow_Filter_List = filterListDockWidget->toggleViewAction();
  actionShow_Filter_List->setText("Filter List");

  menuView->addAction(actionShow_Filter_List);
  connect(actionShow_Filter_List, SIGNAL(triggered(bool)), this, SLOT(on_actionShow_Filter_List_triggered(bool)) );
  //  connect(m_FilterListBtn, SIGNAL(toggled(bool)),
  //          this, SLOT(on_actionShow_Filter_List_triggered(bool)) );


  //  m_FilterLibraryBtn = new QToolButton(this);
  //  makeStatusBarButton("Filter Library", filterLibraryDockWidget, m_FilterLibraryBtn, 1);
  menuView->removeAction(actionShow_Filter_Library);
  delete actionShow_Filter_Library;
  actionShow_Filter_Library = filterLibraryDockWidget->toggleViewAction();
  actionShow_Filter_Library->setText("Filter Library");
  menuView->addAction(actionShow_Filter_Library);
  connect(actionShow_Filter_Library, SIGNAL(triggered(bool)), this, SLOT(on_actionShow_Filter_Library_triggered(bool)) );
  //  connect(m_FilterLibraryBtn, SIGNAL(toggled(bool)),
  //          this, SLOT(on_actionShow_Filter_Library_triggered(bool)) );

  //  m_FavoritesBtn = new QToolButton(this);
  //  makeStatusBarButton("Favorites", BookmarksDockWidget, m_FavoritesBtn, 2);
  menuView->removeAction(actionShow_Favorites);
  delete actionShow_Favorites;
  actionShow_Favorites = bookmarksDockWidget->toggleViewAction();
  actionShow_Favorites->setText("Favorite Pipelines");
  menuView->addAction(actionShow_Favorites);
  connect(actionShow_Favorites, SIGNAL(triggered(bool)), this, SLOT(on_actionShow_Favorites_triggered(bool)) );
  //  connect(m_FavoritesBtn, SIGNAL(toggled(bool)),
  //          this, SLOT(on_actionShow_Favorites_triggered(bool)) );

  //  m_PrebuiltBtn = new QToolButton(this);
  //  makeStatusBarButton("Prebuilt", prebuiltPipelinesDockWidget, m_PrebuiltBtn, 3);
  menuView->removeAction(actionShow_Prebuilt_Pipelines);
  delete actionShow_Prebuilt_Pipelines;
  actionShow_Prebuilt_Pipelines = prebuiltPipelinesDockWidget->toggleViewAction();
  actionShow_Prebuilt_Pipelines->setText("Prebuilt Pipelines");
  menuView->addAction(actionShow_Prebuilt_Pipelines);
  connect(actionShow_Prebuilt_Pipelines, SIGNAL(triggered(bool)), this, SLOT(on_actionShow_Prebuilt_Pipelines_triggered(bool)) );
  //  connect(m_PrebuiltBtn, SIGNAL(toggled(bool)),
  //          this, SLOT(on_actionShow_Prebuilt_Pipelines_triggered(bool)) );


  //  m_IssuesBtn = new QToolButton(this);
  //  makeStatusBarButton("Issues", issuesDockWidget, m_IssuesBtn, 4);
  menuView->removeAction(actionShow_Issues);
  delete actionShow_Issues;
  actionShow_Issues = issuesDockWidget->toggleViewAction();
  actionShow_Issues->setText("Show Warnings/Errors");
  menuView->addAction(actionShow_Issues);
  connect(actionShow_Issues, SIGNAL(triggered(bool)), this, SLOT(on_actionShow_Issues_triggered(bool)) );
  //  connect(m_IssuesBtn, SIGNAL(toggled(bool)),
  //          this, SLOT(on_actionShow_Issues_triggered(bool)) );
#endif

  /* m_ActionRenamePipeline */
  //bookmarksDockWidget->setRenameAction(m_ActionRenamePipeline);

  /* m_ActionRemovePipeline */
  //bookmarksDockWidget->setDeleteAction(m_ActionRemovePipeline);

  m_ActionShowFilterLibrary = new QAction(this);
  m_ActionShowFilterLibrary->setObjectName(QStringLiteral("actionShow_Filter_Library"));
  m_ActionShowFilterLibrary->setIconVisibleInMenu(false);
  m_ActionShowPrebuiltPipelines = new QAction(this);
  m_ActionShowPrebuiltPipelines->setObjectName(QStringLiteral("actionShow_Prebuilt_Pipelines"));
  m_ActionShowPrebuiltPipelines->setIconVisibleInMenu(false);
  m_ActionAddPipeline = new QAction(m_MenuBookmarks);
  m_ActionAddPipeline->setObjectName(QString::fromUtf8("m_ActionAddPipeline"));
  m_ActionRenamePipeline = new QAction(this);
  m_ActionRenamePipeline->setObjectName(QString::fromUtf8("m_ActionRenamePipeline"));
  m_ActionRemovePipeline = new QAction(this);
  m_ActionRemovePipeline->setObjectName(QString::fromUtf8("m_ActionRemovePipeline"));
  m_ActionClearPipeline = new QAction(m_MenuPipeline);
  m_ActionClearPipeline->setObjectName(QString::fromUtf8("m_ActionClearPipeline"));
  m_ActionLocateFile = new QAction(this);
  m_ActionLocateFile->setObjectName(QString::fromUtf8("m_ActionLocateFile"));
  m_ActionShowInFileSystem = new QAction(this);
  m_ActionShowInFileSystem->setObjectName(QString::fromUtf8("m_ActionShowInFileSystem"));
  m_ActionExit = new QAction(this);
  m_ActionExit->setObjectName(QString::fromUtf8("m_ActionExit"));
  m_ActionNewFolder = new QAction(m_MenuBookmarks);
  m_ActionNewFolder->setObjectName(QString::fromUtf8("m_ActionNewFolder"));
  m_ActionOpen = new QAction(this);
  m_ActionOpen->setObjectName(QStringLiteral("actionOpen"));
  m_ActionNew = new QAction(this);
  m_ActionNew->setObjectName(QStringLiteral("actionNew"));
  m_ActionShowFavorites = new QAction(this);
  m_ActionShowFavorites->setObjectName(QStringLiteral("actionShow_Favorites"));
  m_ActionShowFavorites->setIconVisibleInMenu(false);
  m_ActionClearRecentFiles = new QAction(this);
  m_ActionClearRecentFiles->setObjectName(QStringLiteral("actionClearRecentFiles"));
  m_ActionShowIssues = new QAction(this);
  m_ActionShowIssues->setObjectName(QStringLiteral("actionShow_Issues"));
  m_ActionShowIssues->setIconVisibleInMenu(false);
  m_ActionShowFilterList = new QAction(this);
  m_ActionShowFilterList->setObjectName(QStringLiteral("actionShow_Filter_List"));
  m_ActionShowFilterList->setIconVisibleInMenu(false);
  m_ActionShowDREAM3DHelp = new QAction(this);
  m_ActionShowDREAM3DHelp->setObjectName(QStringLiteral("actionShowIndex"));
  m_ActionAboutDREAM3D = new QAction(this);
  m_ActionAboutDREAM3D->setObjectName(QStringLiteral("actionAbout_DREAM3D"));
  m_ActionCheckForUpdates = new QAction(this);
  m_ActionCheckForUpdates->setObjectName(QStringLiteral("actionCheck_For_Updates"));
  m_ActionPluginInformation = new QAction(this);
  m_ActionPluginInformation->setObjectName(QStringLiteral("actionPlugin_Information"));
  m_ActionSave = new QAction(this);
  m_ActionSave->setObjectName(QStringLiteral("actionSave"));
  m_ActionSaveAs = new QAction(this);
  m_ActionSaveAs->setObjectName(QStringLiteral("actionSaveAs"));

  m_ActionShowFilterLibrary->setText(QApplication::translate("DREAM3D_UI", "Filter Library", 0));
  m_ActionShowPrebuiltPipelines->setText(QApplication::translate("DREAM3D_UI", "Prebuilt Pipelines", 0));
  m_ActionLocateFile->setText(QApplication::translate("DREAM3D_UI", "Locate Bookmark...", 0));
  m_ActionAddPipeline->setText(QApplication::translate("DREAM3D_UI", "Add Bookmark", 0));
  m_ActionAddPipeline->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+B", 0));
  m_ActionRenamePipeline->setText(QApplication::translate("DREAM3D_UI", "Rename Pipeline", 0));
  m_ActionRemovePipeline->setText(QApplication::translate("DREAM3D_UI", "Remove Bookmark", 0));
  m_ActionNewFolder->setText(QApplication::translate("DREAM3D_UI", "New Folder", 0));
  m_ActionNewFolder->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+F", 0));
  m_ActionClearPipeline->setText(QApplication::translate("DREAM3D_UI", "Clear Pipeline", 0));
  m_ActionClearPipeline->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Backspace));
  m_ActionOpen->setText(QApplication::translate("DREAM3D_UI", "Open...", 0));
  m_ActionOpen->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+O", 0));
  m_ActionNew->setText(QApplication::translate("DREAM3D_UI", "New...", 0));
  m_ActionNew->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+N", 0));
  m_ActionShowFavorites->setText(QApplication::translate("DREAM3D_UI", "Favorites", 0));
  m_ActionClearRecentFiles->setText(QApplication::translate("DREAM3D_UI", "Clear Recent Files", 0));
  m_ActionExit->setText(QApplication::translate("DREAM3D_UI", "Exit DREAM3D", 0));
#if defined(Q_OS_WIN)
  m_ActionExit->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F4));
#else
  m_ActionExit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
#endif
#if defined(Q_OS_WIN)
  m_ActionShowInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in Windows Explorer", 0));
#elif defined(Q_OS_MAC)
  m_ActionShowInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in Finder", 0));
#else
  m_ActionShowInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in File System", 0));
#endif
  m_ActionShowIssues->setText(QApplication::translate("DREAM3D_UI", "Issues", 0));
  m_ActionShowFilterList->setText(QApplication::translate("DREAM3D_UI", "Filter List", 0));
  m_ActionShowDREAM3DHelp->setText(QApplication::translate("DREAM3D_UI", "DREAM3D Help", 0));
  m_ActionShowDREAM3DHelp->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+H", 0));
  m_ActionAboutDREAM3D->setText(QApplication::translate("DREAM3D_UI", "About DREAM3D", 0));
  m_ActionCheckForUpdates->setText(QApplication::translate("DREAM3D_UI", "Check For Updates", 0));
  m_ActionCheckForUpdates->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+U", 0));
  m_ActionPluginInformation->setText(QApplication::translate("DREAM3D_UI", "Plugin Information", 0));
  m_ActionPluginInformation->setIconText(QApplication::translate("DREAM3D_UI", "Plugin Information", 0));
#ifndef QT_NO_TOOLTIP
  m_ActionPluginInformation->setToolTip(QApplication::translate("DREAM3D_UI", "Plugin Information", 0));
#endif // QT_NO_TOOLTIP
  m_ActionSave->setText(QApplication::translate("DREAM3D_UI", "Save", 0));
  m_ActionSave->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+S", 0));
  m_ActionSaveAs->setText(QApplication::translate("DREAM3D_UI", "Save As...", 0));
  m_ActionSaveAs->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+Shift+S", 0));
  m_MenuFile->setTitle(QApplication::translate("DREAM3D_UI", "File", 0));
  m_MenuRecentFiles->setTitle(QApplication::translate("DREAM3D_UI", "Recent Files", 0));
  m_MenuView->setTitle(QApplication::translate("DREAM3D_UI", "View", 0));
  m_MenuBookmarks->setTitle(QApplication::translate("DREAM3D_UI", "Bookmarks", 0));
  m_MenuHelp->setTitle(QApplication::translate("DREAM3D_UI", "Help", 0));
  m_MenuPipeline->setTitle(QApplication::translate("DREAM3D_UI", "Pipeline", 0));

  // Connections
  connect(m_ActionNew, SIGNAL(triggered()), dream3dApp, SLOT(on_actionNew_triggered()));
  connect(m_ActionOpen, SIGNAL(triggered()), dream3dApp, SLOT(on_actionOpen_triggered()));
  connect(m_ActionSave, SIGNAL(triggered()), dream3dApp, SLOT(on_actionSave_triggered()));
  connect(m_ActionSaveAs, SIGNAL(triggered()), dream3dApp, SLOT(on_actionSaveAs_triggered()));
  connect(m_ActionExit, SIGNAL(triggered()), dream3dApp, SLOT(on_actionExit_triggered()));
  connect(m_ActionClearRecentFiles, SIGNAL(triggered()), dream3dApp, SLOT(on_actionClearRecentFiles_triggered()));
  connect(m_ActionAboutDREAM3D, SIGNAL(triggered()), dream3dApp, SLOT(on_actionAbout_DREAM3D_triggered()));
  connect(m_ActionCheckForUpdates, SIGNAL(triggered()), dream3dApp, SLOT(on_actionCheck_For_Updates_triggered()));

  //connect(m_ActionAddPipeline, SIGNAL(triggered()), bookmarksDockWidget, SLOT(m_ActionAddPipeline_triggered()));
  //connect(m_ActionRenamePipeline, SIGNAL(triggered()), bookmarksDockWidget, SLOT(m_ActionRenamePipeline_triggered()));
  //connect(m_ActionRemovePipeline, SIGNAL(triggered()), bookmarksDockWidget, SLOT(m_ActionRemovePipeline_triggered()));
  //connect(m_ActionNewFolder, SIGNAL(triggered()), bookmarksDockWidget, SLOT(m_ActionNewFolder_triggered()));
  //connect(m_ActionLocateFile, SIGNAL(triggered()), bookmarksDockWidget, SLOT(m_ActionLocateFile_triggered()));
  //connect(m_ActionShowInFileSystem, SIGNAL(triggered()), bookmarksDockWidget, SLOT(m_ActionShowInFileSystem_triggered()));
  #if defined(Q_OS_WIN)
  //connect(m_ActionExit, SIGNAL(triggered()), this, SLOT(closeWindow()));
  #else
  //connect(m_ActionExit, SIGNAL(triggered()), dream3dApp, SLOT(exitTriggered()));
  //connect(m_ActionCloseWindow, SIGNAL(triggered()), this, SLOT(closeWindow()));
  #endif
  //connect(m_ActionClearPipeline, SIGNAL(triggered()), this, SLOT(clearPipeline()));

  // Add the actions to their respective menus
  m_GlobalMenu->addAction(m_MenuFile->menuAction());
  m_GlobalMenu->addAction(m_MenuView->menuAction());
  m_GlobalMenu->addAction(m_MenuBookmarks->menuAction());
  m_GlobalMenu->addAction(m_MenuPipeline->menuAction());
  m_GlobalMenu->addAction(m_MenuHelp->menuAction());
  m_MenuFile->addAction(m_ActionNew);
  m_MenuFile->addAction(m_ActionOpen);
  m_MenuFile->addSeparator();
  m_MenuFile->addAction(m_ActionSave);
  m_MenuFile->addAction(m_ActionSaveAs);
  m_MenuFile->addSeparator();
  m_MenuFile->addAction(m_MenuRecentFiles->menuAction());
  m_MenuFile->addSeparator();
  m_MenuFile->addAction(m_ActionExit);
  m_MenuRecentFiles->addSeparator();
  m_MenuRecentFiles->addAction(m_ActionClearRecentFiles);
  m_MenuView->addAction(m_ActionShowFilterLibrary);
  m_MenuView->addAction(m_ActionShowFilterList);
  m_MenuView->addAction(m_ActionShowPrebuiltPipelines);
  m_MenuView->addAction(m_ActionShowFavorites);
  m_MenuView->addAction(m_ActionShowIssues);
  m_MenuBookmarks->addAction(m_ActionAddPipeline);
  {
    m_MenuBookmarks->addSeparator();
  }
  m_MenuBookmarks->addAction(m_ActionNewFolder);
  m_MenuPipeline->addAction(m_ActionClearPipeline);
  m_MenuHelp->addAction(m_ActionShowDREAM3DHelp);
  m_MenuHelp->addSeparator();
  m_MenuHelp->addAction(m_ActionCheckForUpdates);
  m_MenuHelp->addSeparator();
  m_MenuHelp->addAction(m_ActionAboutDREAM3D);
  m_MenuHelp->addAction(m_ActionPluginInformation);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenuBar* DREAM3DMenu::getMenuBar()
{
  return m_GlobalMenu;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* DREAM3DMenu::getFileMenu()
{
  return m_MenuFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* DREAM3DMenu::getViewMenu()
{
  return m_MenuView;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* DREAM3DMenu::getBookmarksMenu()
{
  return m_MenuBookmarks;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* DREAM3DMenu::getPipelineMenu()
{
  return m_MenuPipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* DREAM3DMenu::getHelpMenu()
{
  return m_MenuHelp;
}



