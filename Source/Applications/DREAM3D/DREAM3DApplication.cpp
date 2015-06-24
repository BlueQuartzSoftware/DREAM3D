/* ============================================================================
* Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
* Copyright (c) 2012 Joseph B. Kleingers (Student Research Assistant)
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
* Neither the name of Michael A. Groeber, Michael A. Jackson, Joseph B. Kleingers,
* the US Air Force, BlueQuartz Software nor the names of its contributors may be
* used to endorse or promote products derived from this software without specific
* prior written permission.
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

#include "DREAM3DApplication.h"

#include <QtCore/QTime>
#include <QtCore/QPluginLoader>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <QtGui/QBitmap>
#include <QtGui/QFileOpenEvent>
#include <iostream>

#include "Applications/DREAM3D/DREAM3D_UI.h"
#include "Applications/DREAM3D/AboutDREAM3D.h"
#include "Applications/DREAM3D/AboutPlugins.h"
#include "Applications/DREAM3D/DREAM3DConstants.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/DREAM3DLibVersion.h"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"
#include "DREAM3DLib/Plugin/PluginManager.h"
#include "DREAM3DLib/Plugin/PluginProxy.h"

#include "DREAM3DWidgetsLib/Widgets/DREAM3DUserManualDialog.h"
#include "DREAM3DWidgetsLib/Widgets/DREAM3DUpdateCheckDialog.h"
#include "DREAM3DWidgetsLib/FilterWidgetManager.h"

#include "QtSupportLib/QRecentFileList.h"
#include "QtSupportLib/DREAM3DHelpUrlGenerator.h"
#include "QtSupportLib/ApplicationAboutBoxDialog.h"

#include "DSplashScreen.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DApplication::DREAM3DApplication(int & argc, char ** argv) :
  QApplication(argc, argv),
  m_GlobalMenu(NULL),
  m_OpenDialogLastDirectory(""),
  show_splash(true),
  Splash(NULL),
  MainWindow(NULL),
  m_ActiveWindow(NULL)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DApplication::~DREAM3DApplication()
{
  delete this->Splash;
  this->Splash = NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void delay(int seconds)
{
  QTime dieTime = QTime::currentTime().addSecs(seconds);
  while( QTime::currentTime() < dieTime )
  {
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DREAM3DApplication::initialize(int argc, char* argv[])
{
  QApplication::setApplicationVersion(DREAM3DLib::Version::Complete());

  // If Mac or Linux, initialize global menu
#if defined (Q_OS_WIN)
#else
  m_GlobalMenu = createMenu();
#endif

  // Create and show the splash screen as the main window is being created.
  QPixmap pixmap(QLatin1String(":/branded_splash.png"));
  this->Splash = new DSplashScreen(pixmap);
  this->Splash->setMask(pixmap.createMaskFromColor(QColor(Qt::transparent)));
  this->Splash->show();

  QDir dir(QApplication::applicationDirPath());

#if defined (Q_OS_MAC)
  dir.cdUp();
  dir.cd("Plugins");

#elif defined (Q_OS_LINUX)
  if (! dir.cd("Plugins"))
  {
    dir.cdUp();
    dir.cd("Plugins");
  }
#elif defined (Q_OS_WIN)
  dir.cdUp();
  dir.cd("Plugins");
#endif
  QApplication::addLibraryPath(dir.absolutePath());

  QMetaObjectUtilities::RegisterMetaTypes();

  // Load application plugins.
  QVector<IDREAM3DPlugin*> plugins = loadPlugins();

  // Create main window.
  this->MainWindow = new DREAM3D_UI(NULL);
  this->MainWindow->setWindowTitle("[*]Untitled Pipeline - DREAM3D");
  this->MainWindow->setLoadedPlugins(plugins);
  this->MainWindow->setAttribute(Qt::WA_DeleteOnClose);
  connect(this->MainWindow, SIGNAL(dream3dWindowChangedState(DREAM3D_UI*)), this, SLOT(activeWindowChanged(DREAM3D_UI*)));

  // Open pipeline if DREAM3D was opened from a compatible file
  if (argc == 2)
  {
    char* two = argv[1];
    QString filePath = QString::fromLatin1(two);
    if (!filePath.isEmpty())
    {
      QFileInfo fi(filePath);
      this->MainWindow->openNewPipeline(filePath, 0, true, true);
    }
  }

  // give GUI components time to update before the mainwindow is shown
  QApplication::instance()->processEvents();
  this->MainWindow->show();
  if (show_splash)
  {
 //   delay(1);
    this->Splash->finish(this->MainWindow);
  }
  QApplication::instance()->processEvents();

  // Check if this is the first run of DREAM3D
  this->MainWindow->checkFirstRun();

  // Register the DREAM3D window with the application
  registerDREAM3DWindow(this->MainWindow);

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<IDREAM3DPlugin*> DREAM3DApplication::loadPlugins()
{
  QStringList pluginDirs;
  pluginDirs << applicationDirPath();

  QDir aPluginDir = QDir(applicationDirPath());
  qDebug() << "Loading DREAM3D Plugins....";
  QString thePath;

#if defined(Q_OS_WIN)
  if (aPluginDir.cd("Plugins") )
  {
    thePath = aPluginDir.absolutePath();
    pluginDirs << thePath;
  }
#elif defined(Q_OS_MAC)
  // Look to see if we are inside an .app package or inside the 'tools' directory
  if (aPluginDir.dirName() == "MacOS")
  {
    aPluginDir.cdUp();
    thePath = aPluginDir.absolutePath() + "/Plugins";
    qDebug() << "  Adding Path " << thePath;
    pluginDirs << thePath;
    aPluginDir.cdUp();
    aPluginDir.cdUp();
    // We need this because Apple (in their infinite wisdom) changed how the current working directory is set in OS X 10.9 and above. Thanks Apple.
    chdir(aPluginDir.absolutePath().toLatin1().constData());
  }
  if (aPluginDir.dirName() == "tools")
  {
    aPluginDir.cdUp();
    // thePath = aPluginDir.absolutePath() + "/Plugins";
    // qDebug() << "  Adding Path " << thePath;
    // m_PluginDirs << thePath;
    // We need this because Apple (in their infinite wisdom) changed how the current working directory is set in OS X 10.9 and above. Thanks Apple.
    chdir(aPluginDir.absolutePath().toLatin1().constData());
  }
  // aPluginDir.cd("Plugins");
  thePath = aPluginDir.absolutePath() + "/Plugins";
  qDebug() << "  Adding Path " << thePath;
  pluginDirs << thePath;

  // This is here for Xcode compatibility
#ifdef CMAKE_INTDIR
  aPluginDir.cdUp();
  thePath = aPluginDir.absolutePath() + "/Plugins/" + CMAKE_INTDIR;
  pluginDirs << thePath;
#endif
#else
  // We are on Linux - I think
  // Try the current location of where the application was launched from which is
  // typically the case when debugging from a build tree
  if (aPluginDir.cd("Plugins"))
  {
    thePath = aPluginDir.absolutePath();
    pluginDirs << thePath;
    aPluginDir.cdUp(); // Move back up a directory level
  }

  if(thePath.isEmpty())
  {
    // Now try moving up a directory which is what should happen when running from a
    // proper distribution of DREAM3D
    aPluginDir.cdUp();
    if (aPluginDir.cd("Plugins"))
    {
      thePath = aPluginDir.absolutePath();
      pluginDirs << thePath;
      aPluginDir.cdUp(); // Move back up a directory level
      int no_error = chdir(aPluginDir.absolutePath().toLatin1().constData());
      if( no_error < 0) {
        qDebug() << "Could not set the working directory.";
      }
    }
  }
#endif

  int dupes = pluginDirs.removeDuplicates();
  qDebug() << "Removed " << dupes << " duplicate Plugin Paths";
  QStringList pluginFilePaths;

  foreach (QString pluginDirString, pluginDirs)
  {
    qDebug() << "Plugin Directory being Searched: " << pluginDirString;
    aPluginDir = QDir(pluginDirString);
    foreach (QString fileName, aPluginDir.entryList(QDir::Files))
    {
      //   qDebug() << "File: " << fileName() << "\n";
#ifdef QT_DEBUG
      if (fileName.endsWith("_debug.plugin", Qt::CaseSensitive))
#else
      if (fileName.endsWith( ".plugin", Qt::CaseSensitive) // We want ONLY Release plugins
      && ! fileName.endsWith("_debug.plugin", Qt::CaseSensitive)) // so ignore these plugins
#endif
      {
        pluginFilePaths << aPluginDir.absoluteFilePath(fileName);
        //qWarning(aPluginDir.absoluteFilePath(fileName).toLatin1(), "%s");
        //qDebug() << "Adding " << aPluginDir.absoluteFilePath(fileName)() << "\n";
      }
    }
  }

  FilterManager* fm = FilterManager::Instance();
  FilterWidgetManager*  fwm = FilterWidgetManager::Instance();

  // THIS IS A VERY IMPORTANT LINE: It will register all the known filters in the dream3d library. This
  // will NOT however get filters from plugins. We are going to have to figure out how to compile filters
  // into their own plugin and load the plugins from a command line.
  fm->RegisterKnownFilters(fm);

  PluginManager* pluginManager = PluginManager::Instance();
  QList<PluginProxy::Pointer> proxies = AboutPlugins::readPluginCache();
  QMap<QString, bool> loadingMap;
  for (QList<PluginProxy::Pointer>::iterator nameIter = proxies.begin(); nameIter != proxies.end(); nameIter++)
  {
    PluginProxy::Pointer proxy = *nameIter;
    loadingMap.insert(proxy->getPluginName(), proxy->getEnabled());
  }

  // Now that we have a sorted list of plugins, go ahead and load them all from the
  // file system and add each to the toolbar and menu
  foreach(QString path, pluginFilePaths)
  {
    qDebug() << "Plugin Being Loaded:" << path;
    QApplication::instance()->processEvents();
    QPluginLoader* loader = new QPluginLoader(path);
    QFileInfo fi(path);
    QString fileName = fi.fileName();
    QObject* plugin = loader->instance();
    qDebug() << "    Pointer: " << plugin << "\n";
    if (plugin )
    {
      IDREAM3DPlugin* ipPlugin = qobject_cast<IDREAM3DPlugin*>(plugin);
      if (ipPlugin)
      {
        QString pluginName = ipPlugin->getPluginName();
        if (loadingMap.value(pluginName, true) == true)
        {
          QString msg = QObject::tr("Loading Plugin %1").arg(fileName);
          this->Splash->showMessage(msg);
          //IDREAM3DPlugin::Pointer ipPluginPtr(ipPlugin);
          ipPlugin->registerFilterWidgets(fwm);
          ipPlugin->registerFilters(fm);
          ipPlugin->setDidLoad(true);
        }
        else
        {
          ipPlugin->setDidLoad(false);
        }

        ipPlugin->setLocation(path);
        pluginManager->addPlugin(ipPlugin);
      }
      m_PluginLoaders.push_back(loader);
    }
    else
    {
      Splash->hide();
      QString message("The plugin did not load with the following error\n");
      message.append(loader->errorString());
      QMessageBox box(QMessageBox::Critical, tr("DREAM3D Plugin Load Error"), tr(message.toStdString().c_str()));
      box.setStandardButtons(QMessageBox::Ok | QMessageBox::Default);
      box.setDefaultButton(QMessageBox::Ok);
      box.setWindowFlags(box.windowFlags() | Qt::WindowStaysOnTopHint);
      box.exec();
      Splash->show();
      delete loader;
    }
  }

  return pluginManager->getPluginsVector();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenuBar* DREAM3DApplication::createMenu()
{
  QMenuBar* menubar = new QMenuBar(NULL);
  menubar->setObjectName(QStringLiteral("m_GlobalMenu"));
  menubar->setGeometry(QRect(0, 0, 1104, 21));

  menuFile = new QMenu(menubar);
  menuFile->setObjectName(QStringLiteral("menuFile"));
  menu_RecentFiles = new QMenu(menuFile);
  menu_RecentFiles->setObjectName(QStringLiteral("menu_RecentFiles"));
  menuView = new QMenu(menubar);
  menuView->setObjectName(QStringLiteral("menuView"));
  menuBookmarks = new QMenu(menubar);
  menuBookmarks->setObjectName(QStringLiteral("menuBookmarks"));
  menuHelp = new QMenu(menubar);
  menuHelp->setObjectName(QStringLiteral("menuHelp"));
  menuPipeline = new QMenu(menubar);
  menuPipeline->setObjectName(QStringLiteral("menuPipeline"));

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

  actionShow_Filter_Library = new QAction(this);
  actionShow_Filter_Library->setObjectName(QStringLiteral("actionShow_Filter_Library"));
  actionShow_Filter_Library->setIconVisibleInMenu(false);
  actionShow_Prebuilt_Pipelines = new QAction(this);
  actionShow_Prebuilt_Pipelines->setObjectName(QStringLiteral("actionShow_Prebuilt_Pipelines"));
  actionShow_Prebuilt_Pipelines->setIconVisibleInMenu(false);
#if defined(Q_OS_WIN)
#else
  actionCloseWindow = new QAction(this);
  actionCloseWindow->setObjectName(QString::fromUtf8("m_ActionCloseWindow"));
#endif
  actionAddPipeline = new QAction(menuBookmarks);
  actionAddPipeline->setObjectName(QString::fromUtf8("m_ActionAddPipeline"));
  actionRenamePipeline = new QAction(this);
  actionRenamePipeline->setObjectName(QString::fromUtf8("m_ActionRenamePipeline"));
  actionRemovePipeline = new QAction(this);
  actionRemovePipeline->setObjectName(QString::fromUtf8("m_ActionRemovePipeline"));
  actionClearPipeline = new QAction(menuPipeline);
  actionClearPipeline->setObjectName(QString::fromUtf8("m_ActionClearPipeline"));
  actionLocateFile = new QAction(this);
  actionLocateFile->setObjectName(QString::fromUtf8("m_ActionLocateFile"));
  actionShowInFileSystem = new QAction(this);
  actionShowInFileSystem->setObjectName(QString::fromUtf8("m_ActionShowInFileSystem"));
  actionExit = new QAction(this);
  actionExit->setObjectName(QString::fromUtf8("m_ActionExit"));
  actionNewFolder = new QAction(menuBookmarks);
  actionNewFolder->setObjectName(QString::fromUtf8("m_ActionNewFolder"));
  actionOpen = new QAction(this);
  actionOpen->setObjectName(QStringLiteral("actionOpen"));
  actionNew = new QAction(this);
  actionNew->setObjectName(QStringLiteral("actionNew"));
  actionShow_Favorites = new QAction(this);
  actionShow_Favorites->setObjectName(QStringLiteral("actionShow_Favorites"));
  actionShow_Favorites->setIconVisibleInMenu(false);
  actionClearRecentFiles = new QAction(this);
  actionClearRecentFiles->setObjectName(QStringLiteral("actionClearRecentFiles"));
  actionShow_Issues = new QAction(this);
  actionShow_Issues->setObjectName(QStringLiteral("actionShow_Issues"));
  actionShow_Issues->setIconVisibleInMenu(false);
  actionShow_Filter_List = new QAction(this);
  actionShow_Filter_List->setObjectName(QStringLiteral("actionShow_Filter_List"));
  actionShow_Filter_List->setIconVisibleInMenu(false);
  actionShowIndex = new QAction(this);
  actionShowIndex->setObjectName(QStringLiteral("actionShowIndex"));
  actionAbout_DREAM3D = new QAction(this);
  actionAbout_DREAM3D->setObjectName(QStringLiteral("actionAbout_DREAM3D"));
  actionCheck_For_Updates = new QAction(this);
  actionCheck_For_Updates->setObjectName(QStringLiteral("actionCheck_For_Updates"));
  actionPlugin_Information = new QAction(this);
  actionPlugin_Information->setObjectName(QStringLiteral("actionPlugin_Information"));
  actionSave = new QAction(this);
  actionSave->setObjectName(QStringLiteral("actionSave"));
  actionSaveAs = new QAction(this);
  actionSaveAs->setObjectName(QStringLiteral("actionSaveAs"));

  actionShow_Filter_Library->setText(QApplication::translate("DREAM3D_UI", "Filter Library", 0));
  actionShow_Prebuilt_Pipelines->setText(QApplication::translate("DREAM3D_UI", "Prebuilt Pipelines", 0));
  actionLocateFile->setText(QApplication::translate("DREAM3D_UI", "Locate Bookmark...", 0));
  actionAddPipeline->setText(QApplication::translate("DREAM3D_UI", "Add Bookmark", 0));
  actionAddPipeline->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+B", 0));
  actionRenamePipeline->setText(QApplication::translate("DREAM3D_UI", "Rename Pipeline", 0));
  actionRemovePipeline->setText(QApplication::translate("DREAM3D_UI", "Remove Bookmark", 0));
  actionNewFolder->setText(QApplication::translate("DREAM3D_UI", "New Folder", 0));
  actionAddPipeline->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+F", 0));
  actionClearPipeline->setText(QApplication::translate("DREAM3D_UI", "Clear Pipeline", 0));
  actionClearPipeline->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Backspace));
#if defined(Q_OS_WIN)
#else
  actionCloseWindow->setText(QApplication::translate("DREAM3D_UI", "Close Window", 0));
  actionCloseWindow->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
#endif
  actionOpen->setText(QApplication::translate("DREAM3D_UI", "Open...", 0));
  actionOpen->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+O", 0));
  actionNew->setText(QApplication::translate("DREAM3D_UI", "New...", 0));
  actionNew->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+N", 0));
  actionShow_Favorites->setText(QApplication::translate("DREAM3D_UI", "Favorites", 0));
  actionClearRecentFiles->setText(QApplication::translate("DREAM3D_UI", "Clear Recent Files", 0));
  actionExit->setText(QApplication::translate("DREAM3D_UI", "Exit DREAM3D", 0));
#if defined(Q_OS_WIN)
  m_ActionExit->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F4));
#else
  actionExit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
#endif
#if defined(Q_OS_WIN)
  m_ActionShowInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in Windows Explorer", 0));
#elif defined(Q_OS_MAC)
  actionShowInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in Finder", 0));
#else
  m_ActionShowInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in File System", 0));
#endif
  actionShow_Issues->setText(QApplication::translate("DREAM3D_UI", "Issues", 0));
  actionShow_Filter_List->setText(QApplication::translate("DREAM3D_UI", "Filter List", 0));
  actionShowIndex->setText(QApplication::translate("DREAM3D_UI", "DREAM3D Help", 0));
  actionShowIndex->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+H", 0));
  actionAbout_DREAM3D->setText(QApplication::translate("DREAM3D_UI", "About DREAM3D", 0));
  actionCheck_For_Updates->setText(QApplication::translate("DREAM3D_UI", "Check For Updates", 0));
  actionCheck_For_Updates->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+U", 0));
  actionPlugin_Information->setText(QApplication::translate("DREAM3D_UI", "Plugin Information", 0));
  actionPlugin_Information->setIconText(QApplication::translate("DREAM3D_UI", "Plugin Information", 0));
#ifndef QT_NO_TOOLTIP
  actionPlugin_Information->setToolTip(QApplication::translate("DREAM3D_UI", "Plugin Information", 0));
#endif // QT_NO_TOOLTIP
  actionSave->setText(QApplication::translate("DREAM3D_UI", "Save", 0));
  actionSave->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+S", 0));
  actionSaveAs->setText(QApplication::translate("DREAM3D_UI", "Save As...", 0));
  actionSaveAs->setShortcut(QApplication::translate("DREAM3D_UI", "Ctrl+Shift+S", 0));
  menuFile->setTitle(QApplication::translate("DREAM3D_UI", "File", 0));
  menu_RecentFiles->setTitle(QApplication::translate("DREAM3D_UI", "Recent Files", 0));
  menuView->setTitle(QApplication::translate("DREAM3D_UI", "View", 0));
  menuBookmarks->setTitle(QApplication::translate("DREAM3D_UI", "Bookmarks", 0));
  menuHelp->setTitle(QApplication::translate("DREAM3D_UI", "Help", 0));
  menuPipeline->setTitle(QApplication::translate("DREAM3D_UI", "Pipeline", 0));

  // Connections
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
  menubar->addAction(menuFile->menuAction());
  menubar->addAction(menuView->menuAction());
  menubar->addAction(menuBookmarks->menuAction());
  menubar->addAction(menuPipeline->menuAction());
  menubar->addAction(menuHelp->menuAction());
  menuFile->addAction(actionNew);
  menuFile->addAction(actionOpen);
  menuFile->addSeparator();
  menuFile->addAction(actionSave);
  menuFile->addAction(actionSaveAs);
  menuFile->addSeparator();
  menuFile->addAction(menu_RecentFiles->menuAction());
  menuFile->addSeparator();
  menuFile->addAction(actionExit);
#if defined(Q_OS_WIN)
#else
  menuFile->addAction(actionCloseWindow);
#endif
  menu_RecentFiles->addSeparator();
  menu_RecentFiles->addAction(actionClearRecentFiles);
  menuView->addAction(actionShow_Filter_Library);
  menuView->addAction(actionShow_Filter_List);
  menuView->addAction(actionShow_Prebuilt_Pipelines);
  menuView->addAction(actionShow_Favorites);
  menuView->addAction(actionShow_Issues);
  menuBookmarks->addAction(actionAddPipeline);
  {
    menuBookmarks->addSeparator();
  }
  menuBookmarks->addAction(actionNewFolder);
  menuPipeline->addAction(actionClearPipeline);
  menuHelp->addAction(actionShowIndex);
  menuHelp->addSeparator();
  menuHelp->addAction(actionCheck_For_Updates);
  menuHelp->addSeparator();
  menuHelp->addAction(actionAbout_DREAM3D);
  menuHelp->addAction(actionPlugin_Information);

  return menubar;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DREAM3DApplication::event(QEvent* event)
{
  if (event->type() == QEvent::FileOpen)
  {
    QFileOpenEvent* openEvent = static_cast<QFileOpenEvent*>(event);
    QString filePath = openEvent->file();

    // Create new DREAM3D_UI instance, and register it
    DREAM3D_UI* newInstance = new DREAM3D_UI(NULL);
    registerDREAM3DWindow(newInstance);

    // Open the pipeline in a new window
    newInstance->openNewPipeline(filePath, true, true, true);

    return true;
  }

  return QApplication::event(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::updateRecentFileList(const QString &file)
{
#if 0
  // Clear the Recent Items Menu
  this->m_Menu_RecentFiles->clear();

  // Get the list from the static object
  QStringList files = QRecentFileList::instance()->fileList();
  foreach (QString file, files)
  {
    QAction* action = new QAction(this->m_Menu_RecentFiles);
    action->setText(QRecentFileList::instance()->parentAndFileName(file));
    action->setData(file);
    action->setVisible(true);
    this->m_Menu_RecentFiles->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
  }

  this->m_Menu_RecentFiles->addSeparator();
  this->m_Menu_RecentFiles->addAction(m_ActionClearRecentFiles);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionClearRecentFiles_triggered()
{
  // Clear the Recent Items Menu
  this->menu_RecentFiles->clear();

  // Clear the actual list
  QRecentFileList* recents = QRecentFileList::instance();
  recents->clear();

  // Write out the empty list
  DREAM3DSettings prefs;
  recents->writeList(prefs);

  this->menu_RecentFiles->addSeparator();
  this->menu_RecentFiles->addAction(actionClearRecentFiles);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::openRecentFile()
{
  QAction* action = qobject_cast<QAction*>(sender());

  if (action)
  {
    QString filePath = action->data().toString();

    DREAM3D_UI* newInstance = getNewDREAM3DInstance();
    newInstance->openNewPipeline(filePath, true, true, false);

    // Add file path to the recent files list for both instances
    QRecentFileList* list = QRecentFileList::instance();
    list->addFile(filePath);

    newInstance->show();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QWidget*> DREAM3DApplication::getDREAM3DWindowList()
{
  return m_DREAM3DWidgetList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::registerDREAM3DWindow(QWidget* widget)
{
  m_DREAM3DWidgetList.push_back(widget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::unregisterDREAM3DWindow(QWidget* widget)
{
  m_DREAM3DWidgetList.removeAll(widget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::exitTriggered()
{
  bool shouldReallyClose = true;
  for (int i = 0; i < m_DREAM3DWidgetList.size(); i++)
  {
    QWidget* dream3dWindow = m_DREAM3DWidgetList.at(i);
    if (NULL != dream3dWindow)
    {
      if (dream3dWindow->close() == false)
      {
        shouldReallyClose = false;
      }
    }
  }

  if (shouldReallyClose == true)
  {
    quit();
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionNew_triggered()
{
  DREAM3D_UI* newInstance = getNewDREAM3DInstance();
  newInstance->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionOpen_triggered()
{
  DREAM3D_UI* newInstance = getNewDREAM3DInstance();

  QString proposedDir = m_OpenDialogLastDirectory;
  QString filePath = QFileDialog::getOpenFileName(NULL, tr("Open Pipeline"),
    proposedDir, tr("Json File (*.json);;Dream3d File (*.dream3d);;Text File (*.txt);;Ini File (*.ini);;All Files (*.*)"));
  if (true == filePath.isEmpty())
  {
    delete newInstance;
    return;
  }

  filePath = QDir::toNativeSeparators(filePath);
  QFileInfo fi(filePath);

  newInstance->openNewPipeline(filePath, true, true, true);

  newInstance->show();

  // Cache the last directory on old instance
  m_OpenDialogLastDirectory = fi.path();
}

// -----------------------------------------------------------------------------
// This is copied from the DREAM3D_UI class.
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionShowIndex_triggered()
{
  // Generate help page
  QUrl helpURL = DREAM3DHelpUrlGenerator::generateHTMLUrl("index");
  DREAM3DUserManualDialog::LaunchHelpDialog(helpURL);
}

// -----------------------------------------------------------------------------
// This is copied from the DREAM3D_UI class.
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionAbout_DREAM3D_triggered()
{
  AboutDREAM3D d(NULL);
  d.exec();
}

// -----------------------------------------------------------------------------
//  This is copied from the DREAM3D_UI class.
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionCheck_For_Updates_triggered()
{
  DREAM3DUpdateCheckDialog* d = new DREAM3DUpdateCheckDialog(NULL);

  d->setCurrentVersion((DREAM3DLib::Version::Complete()));
  d->setUpdateWebSite(DREAM3D::UpdateWebsite::UpdateWebSite);
  d->setApplicationName("DREAM3D");

  // Read from the DREAM3DSettings Pref file the information that we need
  DREAM3DSettings prefs;
  prefs.beginGroup(DREAM3D::UpdateWebsite::VersionCheckGroupName);
  QDateTime dateTime = prefs.value(DREAM3D::UpdateWebsite::LastVersionCheck, QDateTime::currentDateTime()).toDateTime();
  d->setLastCheckDateTime(dateTime);
  prefs.endGroup();

  // Now display the dialog box
  d->exec();
}

// -----------------------------------------------------------------------------
// This is copied from the DREAM3D_UI class.
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionExit_triggered()
{
#if defined (Q_OS_MAC)
  closeAllWindows();
#endif
  quit();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D_UI* DREAM3DApplication::getNewDREAM3DInstance()
{
  PluginManager* pluginManager = PluginManager::Instance();
  QVector<IDREAM3DPlugin*> plugins = pluginManager->getPluginsVector();

  // Create new DREAM3D instance
  DREAM3D_UI* newInstance = new DREAM3D_UI(NULL);
  newInstance->setLoadedPlugins(plugins);
  registerDREAM3DWindow(newInstance);

  // Show the new instance
  newInstance->setAttribute(Qt::WA_DeleteOnClose);
  newInstance->setWindowTitle("[*]Untitled Pipeline - DREAM3D");
  return newInstance;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::activeWindowChanged(DREAM3D_UI* instance)
{
  m_ActiveWindow = instance;
}






