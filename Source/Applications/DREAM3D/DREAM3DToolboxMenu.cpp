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

#include "DREAM3DToolboxMenu.h"

#include <iostream>

#include <QtCore/QDebug>

#include "Applications/DREAM3D/DREAM3DApplication.h"
#include "Applications/DREAM3D/DREAM3DToolbox.h"

 // Include the MOC generated CPP file which has all the QMetaObject methods/data
 #include "moc_DREAM3DToolboxMenu.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DToolboxMenu::DREAM3DToolboxMenu() :
  m_MenuBar(NULL),

  // File Menu
  m_MenuFile(NULL),
  m_ActionCloseToolbox(NULL),

  // View Menu
  m_MenuView(NULL),
  m_ActionShowFilterLibrary(NULL),
  m_ActionShowFilterList(NULL),
  m_ActionShowBookmarks(NULL),

  // Bookmarks Menu
  m_MenuBookmarks(NULL),
  m_ActionAddBookmark(NULL),
  m_ActionNewFolder(NULL)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DToolboxMenu::~DREAM3DToolboxMenu()
{
  qDebug() << "~DREAM3DToolboxMenu";
  if (m_MenuBar) { delete m_MenuBar; }
  if (m_MenuFile) { delete m_MenuFile; }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolboxMenu::initialize()
{
  m_MenuBar = new QMenuBar(NULL);
  m_MenuBar->setObjectName(QStringLiteral("m_GlobalMenu"));
  m_MenuBar->setGeometry(QRect(0, 0, 1104, 21));

  m_MenuFile = new QMenu(m_MenuBar);
  m_MenuFile->setObjectName(QStringLiteral("m_MenuFile"));
  m_MenuView = new QMenu(m_MenuBar);
  m_MenuView->setObjectName(QStringLiteral("m_MenuView"));
  m_MenuBookmarks = new QMenu(m_MenuBar);
  m_MenuBookmarks->setObjectName(QStringLiteral("m_MenuBookmarks"));

  m_ActionCloseToolbox = new QAction(m_MenuFile);
  m_ActionCloseToolbox->setObjectName(QString::fromUtf8("m_ActionCloseToolbox"));
  m_ActionShowFilterLibrary = new QAction(m_MenuView);
  m_ActionShowFilterLibrary->setCheckable(true);
  m_ActionShowFilterLibrary->setObjectName(QString::fromUtf8("m_ActionShowFilterLibrary"));
  m_ActionShowFilterList = new QAction(m_MenuView);
  m_ActionShowFilterList->setCheckable(true);
  m_ActionShowFilterList->setObjectName(QString::fromUtf8("m_ActionShowFilterList"));
  m_ActionShowBookmarks = new QAction(m_MenuView);
  m_ActionShowBookmarks->setCheckable(true);
  m_ActionShowBookmarks->setObjectName(QString::fromUtf8("m_ActionShowBookmarks"));
  m_ActionAddBookmark = new QAction(m_MenuBookmarks);
  m_ActionAddBookmark->setObjectName(QString::fromUtf8("m_ActionAddPipeline"));
  m_ActionNewFolder = new QAction(m_MenuBookmarks);
  m_ActionNewFolder->setObjectName(QString::fromUtf8("m_ActionNewFolder"));


  m_ActionCloseToolbox->setText(QApplication::translate("DREAM3D_UI", "Close Toolbox", 0));
  m_ActionShowFilterLibrary->setText(QApplication::translate("DREAM3D_UI", "Show Filter Library", 0));
  m_ActionShowFilterList->setText(QApplication::translate("DREAM3D_UI", "Show Filter List", 0));
  m_ActionShowBookmarks->setText(QApplication::translate("DREAM3D_UI", "Show Bookmarks", 0));
  m_ActionAddBookmark->setText(QApplication::translate("DREAM3D_UI", "Add Bookmark", 0));
  m_ActionAddBookmark->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+B", 0));
  m_ActionNewFolder->setText(QApplication::translate("DREAM3D_UI", "New Folder", 0));
  m_ActionNewFolder->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+F", 0));

  m_MenuFile->setTitle(QApplication::translate("DREAM3D_UI", "File", 0));
  m_MenuView->setTitle(QApplication::translate("DREAM3D_UI", "View", 0));
  m_MenuBookmarks->setTitle(QApplication::translate("DREAM3D_UI", "Bookmarks", 0));

  DREAM3DToolbox* toolbox = DREAM3DToolbox::Instance();

  // Connections
  connect(m_ActionCloseToolbox, SIGNAL(triggered()), dream3dApp, SLOT(on_actionCloseToolbox_triggered()));
  connect(m_ActionShowFilterLibrary, SIGNAL(triggered(bool)), toolbox, SLOT(actionShowFilterLibrary_triggered(bool)));
  connect(m_ActionShowFilterList, SIGNAL(triggered(bool)), toolbox, SLOT(actionShowFilterList_triggered(bool)));
  connect(m_ActionShowBookmarks, SIGNAL(triggered(bool)), toolbox, SLOT(actionShowBookmarks_triggered(bool)));
  connect(m_ActionAddBookmark, SIGNAL(triggered()), dream3dApp, SLOT(on_actionAddBookmark_triggered()));
  connect(m_ActionNewFolder, SIGNAL(triggered()), dream3dApp, SLOT(on_actionNewFolder_triggered()));

  // Add the actions to their respective menus
  m_MenuBar->addAction(m_MenuFile->menuAction());
  m_MenuBar->addAction(m_MenuView->menuAction());
  m_MenuBar->addAction(m_MenuBookmarks->menuAction());

  m_MenuFile->addAction(m_ActionCloseToolbox);

  m_MenuView->addAction(m_ActionShowFilterList);
  m_MenuView->addAction(m_ActionShowFilterLibrary);
  m_MenuView->addAction(m_ActionShowBookmarks);

  m_MenuBookmarks->addAction(m_ActionAddBookmark);
  {
    m_MenuBookmarks->addSeparator();
  }
  m_MenuBookmarks->addAction(m_ActionNewFolder);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolboxMenu::toggleMenuChildren(QMenu* menu, bool value)
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
QMenuBar* DREAM3DToolboxMenu::getMenuBar()
{
  return m_MenuBar;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* DREAM3DToolboxMenu::getFileMenu()
{
  return m_MenuFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* DREAM3DToolboxMenu::getViewMenu()
{
  return m_MenuView;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* DREAM3DToolboxMenu::getBookmarksMenu()
{
  return m_MenuBookmarks;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DToolboxMenu::getShowFilterLibrary()
{
  return m_ActionShowFilterLibrary;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DToolboxMenu::getShowFilterList()
{
  return m_ActionShowFilterList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DToolboxMenu::getShowBookmarks()
{
  return m_ActionShowBookmarks;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DToolboxMenu::getCloseToolbox()
{
  return m_ActionCloseToolbox;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DToolboxMenu::getAddBookmark()
{
  return m_ActionAddBookmark;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* DREAM3DToolboxMenu::getNewFolder()
{
  return m_ActionNewFolder;
}



