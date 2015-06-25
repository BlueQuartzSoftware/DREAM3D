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

#ifndef _DREAM3DMenu_H_
#define _DREAM3DMenu_H_

#include <QtCore/QObject>

#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>

class DREAM3DMenu : public QObject
{
  Q_OBJECT

public:
  DREAM3DMenu();
  virtual ~DREAM3DMenu();

  void initialize();

  QMenuBar* getMenuBar();

  QMenu* getFileMenu();
  QMenu* getViewMenu();
  QMenu* getBookmarksMenu();
  QMenu* getPipelineMenu();
  QMenu* getHelpMenu();

private:

  QMenuBar*                       m_GlobalMenu;

  // File Menu
  QMenu*                          m_MenuFile;
  QAction*                        m_ActionNew;
  QAction*                        m_ActionOpen;
  QAction*                        m_ActionSave;
  QAction*                        m_ActionSaveAs;
  QMenu*                          m_MenuRecentFiles;
  QAction*                        m_ActionClearRecentFiles;
  QAction*                        m_ActionExit;

  // View Menu
  QMenu*                          m_MenuView;
  QAction*                        m_ActionShowFilterLibrary;
  QAction*                        m_ActionShowFilterList;
  QAction*                        m_ActionShowPrebuiltPipelines;
  QAction*                        m_ActionShowFavorites;
  QAction*                        m_ActionShowIssues;

  // Bookmarks Menu
  QMenu*                          m_MenuBookmarks;
  QAction*                        m_ActionAddPipeline;
  QAction*                        m_ActionNewFolder;

  // Pipeline Menu
  QMenu*                          m_MenuPipeline;
  QAction*                        m_ActionClearPipeline;

  // Help Menu
  QMenu*                          m_MenuHelp;
  QAction*                        m_ActionShowDREAM3DHelp;
  QAction*                        m_ActionCheckForUpdates;
  QAction*                        m_ActionAboutDREAM3D;
  QAction*                        m_ActionPluginInformation;

  // Contextual Menus
  QAction*                        m_ActionRenamePipeline;
  QAction*                        m_ActionRemovePipeline;
  QAction*                        m_ActionLocateFile;
  QAction*                        m_ActionShowInFileSystem;

  DREAM3DMenu(const DREAM3DMenu&); // Copy Constructor Not Implemented
  void operator=(const DREAM3DMenu&); // Operator '=' Not Implemented
};

#endif /* DREAM3DMenu_H_ */
