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
void DREAM3DApplication::initializeApplication()
{
  #if defined (Q_OS_WIN)
  #else
    initializeGlobalMenu();
  #endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::initializeGlobalMenu()
{
  m_GlobalMenu = new QMenuBar(NULL);
  m_GlobalMenu->setObjectName(QStringLiteral("m_GlobalMenu"));

  m_MenuFile = new QMenu("File", m_GlobalMenu);
  m_MenuFile->setObjectName(QStringLiteral("m_MenuFile"));
  m_GlobalMenu->addMenu(m_MenuFile);
  m_Menu_RecentFiles = new QMenu("Recent Files", m_GlobalMenu);
  m_Menu_RecentFiles->setObjectName(QStringLiteral("m_Menu_RecentFiles"));
  m_MenuFile->addMenu(m_Menu_RecentFiles);
  m_MenuHelp = new QMenu("Help", m_GlobalMenu);
  m_MenuHelp->setObjectName(QStringLiteral("m_MenuHelp"));
  m_GlobalMenu->addMenu(m_MenuHelp);

  m_ActionNew = new QAction("New...", this);
  m_ActionNew->setObjectName(QStringLiteral("m_ActionNew"));
  m_ActionNew->setShortcut((QKeySequence(Qt::CTRL + Qt::Key_N)));
  m_ActionOpen = new QAction("Open...", this);
  m_ActionOpen->setObjectName(QStringLiteral("m_ActionOpen"));
  m_ActionOpen->setShortcut((QKeySequence(Qt::CTRL + Qt::Key_O)));
  m_ActionClearRecentFiles = new QAction("Clear Recent Files", this);
  m_ActionClearRecentFiles->setObjectName(QStringLiteral("m_ActionClearRecentFiles"));
  m_ActionExit = new QAction("Exit DREAM3D", this);
  m_ActionExit->setObjectName(QStringLiteral("m_ActionExit"));

  m_ActionShowIndex = new QAction("DREAM3D Help", this);
  m_ActionShowIndex->setObjectName(QStringLiteral("m_ActionShowIndex"));
  m_ActionAbout_DREAM3D = new QAction("About DREAM3D", this);
  m_ActionAbout_DREAM3D->setObjectName(QStringLiteral("m_ActionAbout_DREAM3D"));
  m_ActionCheck_For_Updates = new QAction("Check For Updates", this);
  m_ActionCheck_For_Updates->setObjectName(QStringLiteral("m_ActionCheck_For_Updates"));

  m_MenuFile->addAction(m_ActionNew);
  m_MenuFile->addAction(m_ActionOpen);
  m_MenuFile->addSeparator();
  m_MenuFile->addAction(m_Menu_RecentFiles->menuAction());
  m_Menu_RecentFiles->addSeparator();
  m_Menu_RecentFiles->addAction(m_ActionClearRecentFiles);

  m_MenuHelp->addAction(m_ActionShowIndex);
  m_MenuHelp->addSeparator();
  m_MenuHelp->addAction(m_ActionCheck_For_Updates);
  m_MenuHelp->addAction(m_ActionAbout_DREAM3D);
  m_MenuHelp->addAction(m_ActionExit);

  m_GlobalMenu->show();

  QRecentFileList* recentsList = QRecentFileList::instance();

  DREAM3DSettings prefs;
  recentsList->readList(prefs);

  connect(recentsList, SIGNAL(fileListChanged(const QString &)), this, SLOT(updateRecentFileList(const QString &)));

  connect(m_ActionNew, SIGNAL(triggered()), this, SLOT(on_m_ActionNew_triggered()));
  connect(m_ActionOpen, SIGNAL(triggered()), this, SLOT(on_m_ActionOpen_triggered()));
  connect(m_ActionClearRecentFiles, SIGNAL(triggered()), this, SLOT(on_m_ActionClearRecentFiles_triggered()));
  connect(m_ActionShowIndex, SIGNAL(triggered()), this, SLOT(on_m_ActionShowIndex_triggered()));
  connect(m_ActionAbout_DREAM3D, SIGNAL(triggered()), this, SLOT(on_m_ActionAbout_DREAM3D_triggered()));
  connect(m_ActionCheck_For_Updates, SIGNAL(triggered()), this, SLOT(on_m_ActionCheck_For_Updates_triggered()));
  connect(m_ActionExit, SIGNAL(triggered()), this, SLOT(on_m_ActionExit_triggered()));
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
void DREAM3DApplication::on_m_ActionNew_triggered()
{
  DREAM3D_UI* newInstance = getNewDREAM3DInstance();
  newInstance->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_m_ActionOpen_triggered()
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
void DREAM3DApplication::on_m_ActionClearRecentFiles_triggered()
{
  // Clear the Recent Items Menu
  this->m_Menu_RecentFiles->clear();

  // Clear the actual list
  QRecentFileList* recents = QRecentFileList::instance();
  recents->clear();

  // Write out the empty list
  DREAM3DSettings prefs;
  recents->writeList(prefs);

  this->m_Menu_RecentFiles->addSeparator();
  this->m_Menu_RecentFiles->addAction(m_ActionClearRecentFiles);
}

// -----------------------------------------------------------------------------
// This is copied from the DREAM3D_UI class.
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_m_ActionShowIndex_triggered()
{
  // Generate help page
  QUrl helpURL = DREAM3DHelpUrlGenerator::generateHTMLUrl("index");
  DREAM3DUserManualDialog::LaunchHelpDialog(helpURL);
}

// -----------------------------------------------------------------------------
// This is copied from the DREAM3D_UI class.
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_m_ActionAbout_DREAM3D_triggered()
{
  AboutDREAM3D d(NULL);
  d.exec();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_m_ActionLicense_Information_triggered()
{
//  ApplicationAboutBoxDialog about(DREAM3D::LicenseList, NULL);
//  QString an = QCoreApplication::applicationName();
//  QString version("");
//  version.append(DREAM3DLib::Version::PackageComplete().toLatin1().data());
//  about.setApplicationInfo(an, version);
//  about.exec();
}

// -----------------------------------------------------------------------------
//  This is copied from the DREAM3D_UI class.
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_m_ActionCheck_For_Updates_triggered()
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
void DREAM3DApplication::on_m_ActionExit_triggered()
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






