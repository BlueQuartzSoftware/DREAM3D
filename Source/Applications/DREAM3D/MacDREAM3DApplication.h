#if 0
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

#ifndef _MacDREAM3DApplication_H_
#define _MacDREAM3DApplication_H_

#include "Applications/DREAM3D/DREAM3DApplication.h"

class MacDREAM3DApplication : public DREAM3DApplication
{
  Q_OBJECT

public:
  MacDREAM3DApplication(int& argc, char** argv);
  virtual ~MacDREAM3DApplication();

  void unregisterDREAM3DWindow(DREAM3D_UI* window);

  QAction* getShowToolboxAction();

  QMenu* getDREAM3DMenu();

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

  void addFilter(const QString &text);

  void on_actionShowToolbox_triggered(bool visible);
  void on_actionShowIssues_triggered(bool visible);

  void on_actionCloseToolbox_triggered();
  void on_actionNew_triggered();
  void on_actionOpen_triggered();
  void on_actionSave_triggered();
  void on_actionSaveAs_triggered();

  void on_actionAddBookmark_triggered();
  void on_actionNewFolder_triggered();
  void on_actionRenamePipeline_triggered();
  void on_actionRemovePipeline_triggered();
  void on_actionShowBookmarkInFileSystem_triggered();
  void on_actionLocateFile_triggered();
  void on_actionClearPipeline_triggered();
  void on_actionClearCache_triggered();

  void on_pipelineViewContextMenuRequested(const QPoint&);
  void on_bookmarksDockContextMenuRequested(const QPoint&);

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
  // The global menu
  DREAM3DMenu*                            m_GlobalMenu;

  QAction*                                m_ActionShowToolbox;

  /* Used on Mac OS X when there are no DREAM3D instances instantiated,
  * but the application is still running. */
  QMenu*                                  m_PlaceholderViewMenu;

  QMenu* createPlaceholderViewMenu();

  MacDREAM3DApplication(const MacDREAM3DApplication&); // Copy Constructor Not Implemented
  void operator=(const MacDREAM3DApplication&); // Operator '=' Not Implemented
};

#endif /* _MacDREAM3DApplication_H */


#endif // 0
