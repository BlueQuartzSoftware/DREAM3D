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

#include "QtSupportLib/QRecentFileList.h"

#include "Applications/DREAM3D/DREAM3D_UI.h"
#include "Applications/DREAM3D/DREAM3DToolbox.h"
#include "Applications/DREAM3D/DREAM3DMenuItems.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_StandardDREAM3DApplication.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StandardDREAM3DApplication::StandardDREAM3DApplication(int& argc, char** argv) :
  DREAM3DApplication(argc, argv)
{
  m_Toolbox->setMenuBar(getToolboxMenuBar());
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
      QSharedPointer<DREAM3DSettings> prefs = QSharedPointer<DREAM3DSettings>(new DREAM3DSettings());
      recents->writeList(prefs.data());
    }
  }
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
  QMenu* menuFile = new QMenu("File", menuBar);
  QMenu* menuView = new QMenu("View", menuBar);
  QMenu* menuPipeline = new QMenu("Pipeline", menuBar);
  QMenu* menuHelp = new QMenu("Help", menuBar);
  QMenu* menuAdvanced = new QMenu("Advanced", menuBar);
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
  menuAdvanced->addSeparator();
  menuAdvanced->addAction(actionClearBookmarks);
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
  QMenu* menuView = new QMenu("View", menuBar);
  QMenu* menuBookmarks = new QMenu("Bookmarks", menuBar);
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

