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

#include <iostream>

#include <QtCore/QDebug>

#include "Applications/DREAM3D/DREAM3DApplication.h"

 // Include the MOC generated CPP file which has all the QMetaObject methods/data
 #include "moc_DREAM3DMenu.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DMenu::DREAM3DMenu() :
  m_MenuBar(NULL),

  // File Menu
  m_MenuFile(NULL),
  m_ActionNew(NULL),
  m_ActionOpen(NULL),
  m_ActionSave(NULL),
  m_ActionSaveAs(NULL),
  m_MenuRecentFiles(NULL),
  m_ActionClearRecentFiles(NULL),
  m_ActionExit(NULL),

  // View Menu
  m_MenuView(NULL),
  m_ActionShowFilterLibrary(NULL),
  m_ActionShowFilterList(NULL),
  m_ActionShowPrebuiltPipelines(NULL),
  m_ActionShowBookmarks(NULL),
  m_ActionShowIssues(NULL),

  // Bookmarks Menu
  m_MenuBookmarks(NULL),
  m_ActionAddBookmark(NULL),
  m_ActionNewFolder(NULL),

  // Pipeline Menu
  m_MenuPipeline(NULL),
  m_ActionClearPipeline(NULL),

  // Help Menu
  m_MenuHelp(NULL),
  m_ActionShowDREAM3DHelp(NULL),
  m_ActionCheckForUpdates(NULL),
  m_ActionAboutDREAM3D(NULL),
  m_ActionPluginInformation(NULL),

  // Contextual Menus
  m_ActionRenamePipeline(NULL),
  m_ActionRemovePipeline(NULL),
  m_ActionLocateFile(NULL),
  m_ActionShowBookmarkInFileSystem(NULL),
  m_ActionShowPrebuiltInFileSystem(NULL)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DMenu::~DREAM3DMenu()
{
  qDebug() << "~DREAM3DMenu";
  if (m_MenuBar) { delete m_MenuBar; }
  if (m_MenuFile) { delete m_MenuFile; }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DMenu::initialize()
{
  m_MenuBar = new QMenuBar(NULL);
  m_MenuBar->setObjectName(QStringLiteral("m_GlobalMenu"));
  m_MenuBar->setGeometry(QRect(0, 0, 1104, 21));

  m_MenuFile = new QMenu(m_MenuBar);
  m_MenuFile->setObjectName(QStringLiteral("menuFile"));
  m_MenuRecentFiles = new QMenu(m_MenuFile);
  m_MenuRecentFiles->setObjectName(QStringLiteral("menu_RecentFiles"));
  m_MenuBookmarks = new QMenu(m_MenuBar);
  m_MenuBookmarks->setObjectName(QStringLiteral("menuBookmarks"));
  m_MenuHelp = new QMenu(m_MenuBar);
  m_MenuHelp->setObjectName(QStringLiteral("menuHelp"));
  m_MenuAdvanced = new QMenu(m_MenuHelp);
  m_MenuAdvanced->setObjectName(QStringLiteral("m_MenuAdvanced"));
  m_MenuPipeline = new QMenu(m_MenuBar);
  m_MenuPipeline->setObjectName(QStringLiteral("menuPipeline"));



  m_ActionAddBookmark = new QAction(m_MenuBookmarks);
  m_ActionAddBookmark->setObjectName(QString::fromUtf8("m_ActionAddPipeline"));
  m_ActionRenamePipeline = new QAction(this);
  m_ActionRenamePipeline->setObjectName(QString::fromUtf8("m_ActionRenamePipeline"));
  m_ActionRemovePipeline = new QAction(this);
  m_ActionRemovePipeline->setObjectName(QString::fromUtf8("m_ActionRemovePipeline"));
  m_ActionClearPipeline = new QAction(m_MenuPipeline);
  m_ActionClearPipeline->setObjectName(QString::fromUtf8("m_ActionClearPipeline"));
  m_ActionLocateFile = new QAction(this);
  m_ActionLocateFile->setObjectName(QString::fromUtf8("m_ActionLocateFile"));
  m_ActionShowBookmarkInFileSystem = new QAction(this);
  m_ActionShowBookmarkInFileSystem->setObjectName(QString::fromUtf8("m_ActionShowBookmarkInFileSystem"));
  m_ActionShowPrebuiltInFileSystem = new QAction(this);
  m_ActionShowPrebuiltInFileSystem->setObjectName(QString::fromUtf8("m_ActionShowPrebuiltInFileSystem"));
  m_ActionExit = new QAction(this);
  m_ActionExit->setObjectName(QString::fromUtf8("m_ActionExit"));
  m_ActionNewFolder = new QAction(m_MenuBookmarks);
  m_ActionNewFolder->setObjectName(QString::fromUtf8("m_ActionNewFolder"));
  m_ActionOpen = new QAction(this);
  m_ActionOpen->setObjectName(QStringLiteral("actionOpen"));
  m_ActionNew = new QAction(this);
  m_ActionNew->setObjectName(QStringLiteral("actionNew"));
  m_ActionClearRecentFiles = new QAction(this);
  m_ActionClearRecentFiles->setObjectName(QStringLiteral("actionClearRecentFiles"));
  m_ActionShowDREAM3DHelp = new QAction(this);
  m_ActionShowDREAM3DHelp->setObjectName(QStringLiteral("actionShowDREAM3DHelp"));
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
  m_ActionClearCache = new QAction(this);
  m_ActionClearCache->setObjectName(QStringLiteral("actionClearCache"));




  m_ActionLocateFile->setText(QApplication::translate("DREAM3D_UI", "Locate Bookmark...", 0));
  m_ActionAddBookmark->setText(QApplication::translate("DREAM3D_UI", "Add Bookmark", 0));
  m_ActionAddBookmark->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+B", 0));
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
  m_ActionClearRecentFiles->setText(QApplication::translate("DREAM3D_UI", "Clear Recent Files", 0));
  m_ActionExit->setText(QApplication::translate("DREAM3D_UI", "Exit DREAM.3D", 0));
#if defined(Q_OS_WIN)
  m_ActionExit->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F4));
#else
  m_ActionExit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
#endif
#if defined(Q_OS_WIN)
  m_ActionShowBookmarkInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in Windows Explorer", 0));
  m_ActionShowPrebuiltInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in Windows Explorer", 0));
#elif defined(Q_OS_MAC)
  m_ActionShowBookmarkInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in Finder", 0));
  m_ActionShowPrebuiltInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in Finder", 0));
#else
  m_ActionShowBookmarkInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in File System", 0));
  m_ActionShowPrebuiltInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in File System", 0));
#endif
  m_ActionShowDREAM3DHelp->setText(QApplication::translate("DREAM3D_UI", "DREAM.3D Help", 0));
  m_ActionShowDREAM3DHelp->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+H", 0));
  m_ActionAboutDREAM3D->setText(QApplication::translate("DREAM3D_UI", "About DREAM.3D", 0));
  m_ActionCheckForUpdates->setText(QApplication::translate("DREAM3D_UI", "Check For Updates", 0));
  m_ActionCheckForUpdates->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+U", 0));
  m_ActionPluginInformation->setText(QApplication::translate("DREAM3D_UI", "Plugin Information", 0));
  m_ActionPluginInformation->setIconText(QApplication::translate("DREAM3D_UI", "Plugin Information", 0));
#ifndef QT_NO_TOOLTIP
  m_ActionPluginInformation->setToolTip(QApplication::translate("DREAM3D_UI", "Plugin Information", 0));
#endif // QT_NO_TOOLTIP
  m_ActionPluginInformation->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+I", 0));
  m_ActionSave->setText(QApplication::translate("DREAM3D_UI", "Save", 0));
  m_ActionSave->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+S", 0));
  m_ActionSaveAs->setText(QApplication::translate("DREAM3D_UI", "Save As...", 0));
  m_ActionSaveAs->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+Shift+S", 0));
  m_ActionClearCache->setText(QApplication::translate("DREAM3D_UI", "Clear Cache", 0));
  m_MenuFile->setTitle(QApplication::translate("DREAM3D_UI", "File", 0));
  m_MenuRecentFiles->setTitle(QApplication::translate("DREAM3D_UI", "Recent Files", 0));
  m_MenuBookmarks->setTitle(QApplication::translate("DREAM3D_UI", "Bookmarks", 0));
  m_MenuHelp->setTitle(QApplication::translate("DREAM3D_UI", "Help", 0));
  m_MenuAdvanced->setTitle(QApplication::translate("DREAM3D_UI", "Advanced", 0));
  m_MenuPipeline->setTitle(QApplication::translate("DREAM3D_UI", "Pipeline", 0));

  // Connections
  connect(m_ActionNew, SIGNAL(triggered()), dream3dApp, SLOT(on_actionNew_triggered()));
  connect(m_ActionOpen, SIGNAL(triggered()), dream3dApp, SLOT(on_actionOpen_triggered()));
  connect(m_ActionSave, SIGNAL(triggered()), dream3dApp, SLOT(on_actionSave_triggered()));
  connect(m_ActionSaveAs, SIGNAL(triggered()), dream3dApp, SLOT(on_actionSaveAs_triggered()));
  connect(m_ActionExit, SIGNAL(triggered()), dream3dApp, SLOT(on_actionExit_triggered()));
  connect(m_ActionClearRecentFiles, SIGNAL(triggered()), dream3dApp, SLOT(on_actionClearRecentFiles_triggered()));
  connect(m_ActionAboutDREAM3D, SIGNAL(triggered()), dream3dApp, SLOT(on_actionAboutDREAM3D_triggered()));
  connect(m_ActionCheckForUpdates, SIGNAL(triggered()), dream3dApp, SLOT(on_actionCheckForUpdates_triggered()));
  connect(m_ActionShowDREAM3DHelp, SIGNAL(triggered()), dream3dApp, SLOT(on_actionShowDREAM3DHelp_triggered()));
  connect(m_ActionPluginInformation, SIGNAL(triggered()), dream3dApp, SLOT(on_actionPluginInformation_triggered()));
  connect(m_ActionAddBookmark, SIGNAL(triggered()), dream3dApp, SLOT(on_actionAddBookmark_triggered()));
  connect(m_ActionNewFolder, SIGNAL(triggered()), dream3dApp, SLOT(on_actionNewFolder_triggered()));
  connect(m_ActionClearPipeline, SIGNAL(triggered()), dream3dApp, SLOT(on_actionClearPipeline_triggered()));
  connect(m_ActionShowBookmarkInFileSystem, SIGNAL(triggered()), dream3dApp, SLOT(on_actionShowBookmarkInFileSystem_triggered()));
  connect(m_ActionShowPrebuiltInFileSystem, SIGNAL(triggered()), dream3dApp, SLOT(on_actionShowPrebuiltInFileSystem_triggered()));
  connect(m_ActionRenamePipeline, SIGNAL(triggered()), dream3dApp, SLOT(on_actionRenamePipeline_triggered()));
  connect(m_ActionRemovePipeline, SIGNAL(triggered()), dream3dApp, SLOT(on_actionRemovePipeline_triggered()));
  connect(m_ActionLocateFile, SIGNAL(triggered()), dream3dApp, SLOT(on_actionLocateFile_triggered()));
  connect(m_ActionClearCache, SIGNAL(triggered()), dream3dApp, SLOT(on_actionClearCache_triggered()));


  // Add the actions to their respective menus
  m_MenuBar->addAction(m_MenuFile->menuAction());
  m_MenuBar->addAction(m_MenuBookmarks->menuAction());
  m_MenuBar->addAction(m_MenuPipeline->menuAction());
  m_MenuBar->addAction(m_MenuHelp->menuAction());
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
  m_MenuBookmarks->addAction(m_ActionAddBookmark);
  {
    m_MenuBookmarks->addSeparator();
  }
  m_MenuBookmarks->addAction(m_ActionNewFolder);
  m_MenuPipeline->addAction(m_ActionClearPipeline);
  m_MenuHelp->addAction(m_ActionShowDREAM3DHelp);
  m_MenuHelp->addSeparator();
  m_MenuHelp->addAction(m_ActionCheckForUpdates);
  m_MenuHelp->addSeparator();
  m_MenuHelp->addAction(m_MenuAdvanced->menuAction());
  m_MenuAdvanced->addAction(m_ActionClearCache);
  m_MenuHelp->addSeparator();
  m_MenuHelp->addAction(m_ActionAboutDREAM3D);
  m_MenuHelp->addAction(m_ActionPluginInformation);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DMenu::toggleMenuChildren(QMenu* menu, bool value)
{
  if(menu)
  {
    QObjectList menus = menu->children();
    foreach(QObject* obj, menus)
    {
      QMenu* menu = qobject_cast<QMenu*>(obj);
      if(menu)
      {
        menu->setEnabled(value);
      }
      QAction* action = qobject_cast<QAction*>(obj);
      if(action)
      {
        action->setEnabled(value);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenuBar* DREAM3DMenu::getMenuBar()
{
  return m_MenuBar;
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
void DREAM3DMenu::setViewMenu(QMenu* viewMenu)
{
  if (m_MenuBar != NULL)
  {
    m_MenuView = viewMenu;

    QAction* bookmarksMenuAction = m_MenuBookmarks->menuAction();
    m_MenuBar->insertMenu(bookmarksMenuAction, viewMenu);
  }
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* DREAM3DMenu::getRecentFilesMenu()
{
  return m_MenuRecentFiles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getClearRecentFiles()
{
  return m_ActionClearRecentFiles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getShowFilterLibrary()
{
  return m_ActionShowFilterLibrary;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getShowFilterList()
{
  return m_ActionShowFilterList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getShowPrebuiltPipelines()
{
  return m_ActionShowPrebuiltPipelines;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getShowBookmarks()
{
  return m_ActionShowBookmarks;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getShowIssues()
{
  return m_ActionShowIssues;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getNew()
{
  return m_ActionNew;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getOpen()
{
  return m_ActionOpen;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getSave()
{
  return m_ActionSave;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getSaveAs()
{
  return m_ActionSaveAs;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getClearPipeline()
{
  return m_ActionClearPipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getShowPrebuiltInFileSystem()
{
  return m_ActionShowPrebuiltInFileSystem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getShowBookmarkInFileSystem()
{
  return m_ActionShowBookmarkInFileSystem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getAddBookmark()
{
  return m_ActionAddBookmark;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getNewFolder()
{
  return m_ActionNewFolder;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getRenamePipeline()
{
  return m_ActionRenamePipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getRemovePipeline()
{
  return m_ActionRemovePipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DMenu::getLocateFile()
{
  return m_ActionLocateFile;
}



