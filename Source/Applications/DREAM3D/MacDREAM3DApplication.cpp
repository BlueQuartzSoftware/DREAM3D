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

#include "QtSupportLib/QRecentFileList.h"

#include "Applications/DREAM3D/DREAM3D_UI.h"
#include "Applications/DREAM3D/DREAM3DToolbox.h"
#include "Applications/DREAM3D/DREAM3DMenuItems.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_MacDREAM3DApplication.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MacDREAM3DApplication::MacDREAM3DApplication(int& argc, char** argv) :
DREAM3DApplication(argc, argv),
m_GlobalMenu(NULL)
{
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
  QSharedPointer<DREAM3DSettings> prefs = QSharedPointer<DREAM3DSettings>(new DREAM3DSettings());
  recents->writeList(prefs.data());
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
void MacDREAM3DApplication::dream3dWindowChanged(DREAM3D_UI* instance)
{
  if (instance->isActiveWindow())
  {
    DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();

    m_ActiveWindow = instance;
    toDREAM3DMenuState(instance);

    // Update the issues menu item with the correct value
    QAction* issuesToggle = m_ActiveWindow->getIssuesDockWidget()->toggleViewAction();
    menuItems->getActionShowIssues()->setChecked(issuesToggle->isChecked());
  }
  else if (m_DREAM3DInstances.size() <= 0)
  {
    /* If the inactive signal got fired and there are no more windows,
     * this means that the last window has been closed. */
    m_ActiveWindow = NULL;
    toEmptyMenuState();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MacDREAM3DApplication::toolboxWindowChanged()
{
  if (m_Toolbox->isActiveWindow())
  {
    toToolboxMenuState();
  }
  else if (m_DREAM3DInstances.size() <= 0)
  {
    toEmptyMenuState();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MacDREAM3DApplication::toToolboxMenuState()
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();
  BookmarksModel* model = BookmarksModel::Instance();

  QModelIndex currentBookmark = m_Toolbox->getBookmarksWidget()->getBookmarksTreeView()->currentIndex();

  menuItems->getActionSave()->setDisabled(true);
  menuItems->getActionSaveAs()->setDisabled(true);
  menuItems->getActionShowIssues()->setDisabled(true);
  menuItems->getActionClearPipeline()->setDisabled(true);

  menuItems->getActionShowFilterList()->setEnabled(true);
  menuItems->getActionShowFilterLibrary()->setEnabled(true);
  menuItems->getActionShowBookmarks()->setEnabled(true);

  if (currentBookmark.isValid() == false || model->index(currentBookmark.row(), BookmarksItem::Path, currentBookmark.parent()).data().toString().isEmpty() == true)
  {
    menuItems->getActionAddBookmark()->setEnabled(true);
    menuItems->getActionNewFolder()->setEnabled(true);
  }
  else
  {
    menuItems->getActionAddBookmark()->setDisabled(true);
    menuItems->getActionNewFolder()->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MacDREAM3DApplication::toDREAM3DMenuState(DREAM3D_UI* instance)
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();

  if (isCurrentlyRunning(instance) == true)
  {
    menuItems->getActionClearPipeline()->setDisabled(true);
  }
  else
  {
    menuItems->getActionClearPipeline()->setEnabled(true);
  }

  menuItems->getActionShowFilterList()->setDisabled(true);
  menuItems->getActionShowFilterLibrary()->setDisabled(true);
  menuItems->getActionShowBookmarks()->setDisabled(true);
  menuItems->getActionAddBookmark()->setDisabled(true);
  menuItems->getActionNewFolder()->setDisabled(true);

  menuItems->getActionSave()->setEnabled(true);
  menuItems->getActionSaveAs()->setEnabled(true);
  menuItems->getActionShowIssues()->setEnabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MacDREAM3DApplication::toEmptyMenuState()
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();

  menuItems->getActionShowFilterList()->setDisabled(true);
  menuItems->getActionShowFilterLibrary()->setDisabled(true);
  menuItems->getActionShowBookmarks()->setDisabled(true);
  menuItems->getActionAddBookmark()->setDisabled(true);
  menuItems->getActionNewFolder()->setDisabled(true);
  menuItems->getActionSave()->setDisabled(true);
  menuItems->getActionSaveAs()->setDisabled(true);
  menuItems->getActionShowIssues()->setDisabled(true);
  menuItems->getActionClearPipeline()->setDisabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MacDREAM3DApplication::createGlobalMenu()
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();

  QMenu* menuFile = new QMenu("File", m_GlobalMenu);
  QMenu* menuView = new QMenu("View", m_GlobalMenu);
  QMenu* menuToolbox = new QMenu("Toolbox", m_GlobalMenu);
  QMenu* menuBookmarks = new QMenu("Bookmarks", m_GlobalMenu);
  QMenu* menuPipeline = new QMenu("Pipeline", m_GlobalMenu);
  QMenu* menuHelp = new QMenu("Help", m_GlobalMenu);
  QMenu* menuAdvanced = new QMenu("Advanced", m_GlobalMenu);
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
  QAction* actionClearBookmarks = menuItems->getActionClearBookmarks();
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
  menuAdvanced->addSeparator();
  menuAdvanced->addAction(actionClearBookmarks);
  menuHelp->addSeparator();
  menuHelp->addAction(actionAboutDREAM3D);
  menuHelp->addAction(actionPluginInformation);
}
