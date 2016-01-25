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

#include "DREAM3DMenuItems.h"

#include <QtWidgets/QApplication>

#include "Applications/DREAM3D/DREAM3DApplication.h"
#include "Applications/DREAM3D/DREAM3DToolbox.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_DREAM3DMenuItems.cpp"

DREAM3DMenuItems* DREAM3DMenuItems::self = NULL;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DMenuItems::DREAM3DMenuItems(QObject* parent) :
QObject(parent)
{
  createMenus();
  createActions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DMenuItems::~DREAM3DMenuItems()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DMenuItems* DREAM3DMenuItems::Instance()
{
  if (NULL == self)
  {
    self = new DREAM3DMenuItems();
  }

  return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DMenuItems::createMenus()
{
  m_MenuRecentFiles = new QMenu("Recent Files");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DMenuItems::createActions()
{
  m_ActionRenameBookmark = new QAction("Rename Pipeline", this);
  m_ActionRemoveBookmark = new QAction("Remove Bookmark", this);
  m_ActionClearPipeline = new QAction("Clear Pipeline", this);
  m_ActionLocateFile = new QAction("Locate Bookmark...", this);
  m_ActionShowBookmarkInFileSystem = new QAction(this);
  m_ActionExit = new QAction("Exit DREAM.3D", this);
  m_ActionOpen = new QAction("Open...", this);
  m_ActionNew = new QAction("New...", this);
  m_ActionClearRecentFiles = new QAction("Clear Recent Files", this);
  m_ActionShowDREAM3DHelp = new QAction("DREAM.3D Help", this);
  m_ActionAboutDREAM3D = new QAction("About DREAM.3D", this);
  m_ActionCheckForUpdates = new QAction("Check For Updates", this);
  m_ActionPluginInformation = new QAction("Plugin Information", this);
  m_ActionSave = new QAction("Save", this);
  m_ActionSaveAs = new QAction("Save As...", this);
  m_ActionClearCache = new QAction("Clear Cache", this);
  m_ActionClearBookmarks = new QAction("Clear Bookmarks", this);
  m_ActionShowFilterLibrary = new QAction("Show Filter Library", this);
  m_ActionShowFilterLibrary->setCheckable(true);
  m_ActionShowFilterList = new QAction("Show Filter List", this);
  m_ActionShowFilterList->setCheckable(true);
  m_ActionShowBookmarks = new QAction("Show Bookmarks", this);
  m_ActionShowBookmarks->setCheckable(true);
  m_ActionShowIssues = new QAction("Show Warnings/Errors", this);
  m_ActionShowIssues->setCheckable(true);
  m_ActionShowToolbox = new QAction("Show Toolbox", this);
  m_ActionShowToolbox->setCheckable(true);
  m_ActionAddBookmark = new QAction("Add Bookmark", this);
  m_ActionNewFolder = new QAction("New Folder", this);


#if defined(Q_OS_WIN)
  m_ActionShowBookmarkInFileSystem->setText("Show in Windows Explorer");
#elif defined(Q_OS_MAC)
  m_ActionShowBookmarkInFileSystem->setText("Show in Finder");
#else
  m_ActionShowBookmarkInFileSystem->setText("Show in File System");
#endif


  m_ActionClearPipeline->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Backspace));
  m_ActionOpen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
  m_ActionNew->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
#if defined(Q_OS_WIN)
  m_ActionExit->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F4));
#else
  m_ActionExit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
#endif
  m_ActionShowDREAM3DHelp->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
  m_ActionCheckForUpdates->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
  m_ActionPluginInformation->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
  m_ActionSave->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
  m_ActionSaveAs->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
  m_ActionAddBookmark->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
  m_ActionNewFolder->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));

  DREAM3DToolbox* toolbox = DREAM3DToolbox::Instance();

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
  connect(m_ActionClearPipeline, SIGNAL(triggered()), dream3dApp, SLOT(on_actionClearPipeline_triggered()));
  connect(m_ActionShowBookmarkInFileSystem, SIGNAL(triggered()), dream3dApp, SLOT(on_actionShowBookmarkInFileSystem_triggered()));
  connect(m_ActionRenameBookmark, SIGNAL(triggered()), dream3dApp, SLOT(on_actionRenameBookmark_triggered()));
  connect(m_ActionRemoveBookmark, SIGNAL(triggered()), dream3dApp, SLOT(on_actionRemoveBookmark_triggered()));
  connect(m_ActionLocateFile, SIGNAL(triggered()), dream3dApp, SLOT(on_actionLocateFile_triggered()));
  connect(m_ActionClearCache, SIGNAL(triggered()), dream3dApp, SLOT(on_actionClearCache_triggered()));
  connect(m_ActionClearBookmarks, SIGNAL(triggered()), dream3dApp, SLOT(on_actionClearBookmarks_triggered()));
  connect(m_ActionShowFilterLibrary, SIGNAL(triggered(bool)), toolbox, SLOT(actionShowFilterLibrary_triggered(bool)));
  connect(m_ActionShowFilterList, SIGNAL(triggered(bool)), toolbox, SLOT(actionShowFilterList_triggered(bool)));
  connect(m_ActionShowBookmarks, SIGNAL(triggered(bool)), toolbox, SLOT(actionShowBookmarks_triggered(bool)));
  connect(m_ActionShowToolbox, SIGNAL(triggered(bool)), dream3dApp, SLOT(on_actionShowToolbox_triggered(bool)));
  connect(m_ActionShowIssues, SIGNAL(triggered(bool)), dream3dApp, SLOT(on_actionShowIssues_triggered(bool)));
  connect(m_ActionAddBookmark, SIGNAL(triggered()), dream3dApp, SLOT(on_actionAddBookmark_triggered()));
  connect(m_ActionNewFolder, SIGNAL(triggered()), dream3dApp, SLOT(on_actionNewFolder_triggered()));
}



