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
#include "DREAM3DApplication.h"

#if ! defined(_MSC_VER)
#include <unistd.h>
#endif

#include <iostream>

#include <QtCore/QTime>
#include <QtCore/QPluginLoader>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <QtGui/QDesktopServices>
#include <QtGui/QBitmap>
#include <QtGui/QFileOpenEvent>

#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/PluginProxy.h"

#include "QtSupportLib/QRecentFileList.h"
#include "QtSupportLib/DREAM3DHelpUrlGenerator.h"
#include "QtSupportLib/ApplicationAboutBoxDialog.h"

#include "DREAM3DWidgetsLib/FilterWidgetManager.h"
#include "DREAM3DWidgetsLib/Widgets/DREAM3DUpdateCheckDialog.h"
#ifdef DREAM3D_USE_QtWebEngine
#include "Applications/Common/DREAM3DUserManualDialog.h"
#endif

#include "Applications/DREAM3D/DREAM3D_UI.h"
#include "Applications/DREAM3D/AboutDREAM3D.h"
#include "Applications/DREAM3D/AboutPlugins.h"
#include "Applications/DREAM3D/DREAM3DConstants.h"

#include "DSplashScreen.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_DREAM3DApplication.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DApplication::DREAM3DApplication(int& argc, char** argv) :
  QApplication(argc, argv),
  m_ActiveWindow(NULL),
#if defined(Q_OS_MAC)
  m_GlobalMenu(NULL),
#endif
  m_OpenDialogLastDirectory(""),
  show_splash(true),
  Splash(NULL)
{
  // Connection to update the recent files list on all windows when it changes
  QRecentFileList* recentsList = QRecentFileList::instance();
  connect(recentsList, SIGNAL(fileListChanged(const QString&)),
          this, SLOT(updateRecentFileList(const QString&)));

  // Create the placeholder View Menu
  m_PlaceholderViewMenu = createPlaceholderViewMenu();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DApplication::~DREAM3DApplication()
{
  delete this->Splash;
  this->Splash = NULL;

  DREAM3DSettings prefs;
  if (prefs.value("Program Mode", QString("")) == "Clear Cache")
  {
    prefs.clear();
    prefs.setValue("First Run", QVariant(false));

    prefs.setValue("Program Mode", QString("Standard"));
  }
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
  QApplication::setApplicationVersion(SIMPLib::Version::Complete());

  // If Mac, initialize global menu
#if defined (Q_OS_MAC)
  m_GlobalMenu = new DREAM3DMenu();
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
  QVector<ISIMPLibPlugin*> plugins = loadPlugins();

  // give GUI components time to update before the mainwindow is shown
  QApplication::instance()->processEvents();
  if (show_splash)
  {
//   delay(1);
    this->Splash->finish(NULL);
  }
  QApplication::instance()->processEvents();

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<ISIMPLibPlugin*> DREAM3DApplication::loadPlugins()
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
      if( no_error < 0)
      {
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
    if (plugin)
    {
      ISIMPLibPlugin* ipPlugin = qobject_cast<ISIMPLibPlugin*>(plugin);
      if (ipPlugin)
      {
        QString pluginName = ipPlugin->getPluginName();
        if (loadingMap.value(pluginName, true) == true)
        {
          QString msg = QObject::tr("Loading Plugin %1").arg(fileName);
          this->Splash->showMessage(msg);
          //ISIMPLibPlugin::Pointer ipPluginPtr(ipPlugin);
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
bool DREAM3DApplication::event(QEvent* event)
{
  if (event->type() == QEvent::FileOpen)
  {
    QFileOpenEvent* openEvent = static_cast<QFileOpenEvent*>(event);
    QString filePath = openEvent->file();

    newInstanceFromFile(filePath, true, true);

    return true;
  }

  return QApplication::event(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::updateRecentFileList(const QString& file)
{
#if defined (Q_OS_MAC)
  QMenu* recentFilesMenu = m_GlobalMenu->getRecentFilesMenu();
  QAction* clearRecentFilesAction = m_GlobalMenu->getClearRecentFiles();

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
#else
  QMap<DREAM3D_UI*, QMenu*> windows = dream3dApp->getDREAM3DInstanceMap();

  QMapIterator<DREAM3D_UI*, QMenu*> iter(windows);
  while (iter.hasNext())
  {
    iter.next();

    DREAM3D_UI* window = iter.key();

    if (NULL != window)
    {
      QMenu* recentFilesMenu = window->getDREAM3DMenu()->getRecentFilesMenu();
      QAction* clearRecentFilesAction = window->getDREAM3DMenu()->getClearRecentFiles();

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
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionClearRecentFiles_triggered()
{
#if defined (Q_OS_MAC)
  QMenu* recentFilesMenu = m_GlobalMenu->getRecentFilesMenu();
  QAction* clearRecentFilesAction = m_GlobalMenu->getClearRecentFiles();

  // Clear the Recent Items Menu
  recentFilesMenu->clear();
  recentFilesMenu->addSeparator();
  recentFilesMenu->addAction(clearRecentFilesAction);

  // Clear the actual list
  QRecentFileList* recents = QRecentFileList::instance();
  recents->clear();

  // Write out the empty list
  DREAM3DSettings prefs;
  recents->writeList(prefs);
#else
  QMap<DREAM3D_UI*, QMenu*> windows = dream3dApp->getDREAM3DInstanceMap();

  QMapIterator<DREAM3D_UI*, QMenu*> iter(windows);
  while (iter.hasNext())
  {
    iter.next();

    DREAM3D_UI* window = iter.key();

    if (NULL != window)
    {
      QMenu* recentFilesMenu = window->getDREAM3DMenu()->getRecentFilesMenu();
      QAction* clearRecentFilesAction = window->getDREAM3DMenu()->getClearRecentFiles();

      // Clear the Recent Items Menu
      recentFilesMenu->clear();
      recentFilesMenu->addSeparator();
      recentFilesMenu->addAction(clearRecentFilesAction);

      // Clear the actual list
      QRecentFileList* recents = QRecentFileList::instance();
      recents->clear();

      // Write out the empty list
      DREAM3DSettings prefs;
      recents->writeList(prefs);
    }
  }
#endif
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

    newInstanceFromFile(filePath, true, true);

    // Add file path to the recent files list for both instances
    QRecentFileList* list = QRecentFileList::instance();
    list->addFile(filePath);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<DREAM3D_UI*, QMenu*> DREAM3DApplication::getDREAM3DInstanceMap()
{
  return m_DREAM3DInstanceMap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::registerDREAM3DWindow(DREAM3D_UI* window, QMenu* viewMenu)
{
  m_DREAM3DInstanceMap.insert(window, viewMenu);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::unregisterDREAM3DWindow(DREAM3D_UI* window)
{
  m_DREAM3DInstanceMap.remove(window);

//#if defined (Q_OS_MAC)
//    m_GlobalMenu->setViewMenu(NULL);
//#endif

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
  QString proposedDir = m_OpenDialogLastDirectory;
  QString filePath = QFileDialog::getOpenFileName(NULL, tr("Open Pipeline"),
                                                  proposedDir, tr("Json File (*.json);;DREAM.3D File (*.dream3d);;Text File (*.txt);;Ini File (*.ini);;All Files (*.*)"));
  if(filePath.isEmpty())
  {
    return;
  }

  newInstanceFromFile(filePath, true, true);

  // Cache the last directory on old instance
  m_OpenDialogLastDirectory = filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionSave_triggered()
{
  if (NULL != m_ActiveWindow)
  {
    m_ActiveWindow->savePipeline();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionSaveAs_triggered()
{
  if (NULL != m_ActiveWindow)
  {
    m_ActiveWindow->savePipelineAs();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionAddBookmark_triggered()
{
  if (NULL != m_ActiveWindow)
  {
    BookmarksDockWidget* bookmarksDockWidget = m_ActiveWindow->getBookmarksDockWidget();

    if (NULL != bookmarksDockWidget)
    {
      QString proposedDir = m_OpenDialogLastDirectory;
      QList<QString> newPrefPaths;

      newPrefPaths = QFileDialog::getOpenFileNames(m_ActiveWindow, tr("Choose Pipeline File(s)"),
                                                   proposedDir, tr("Json File (*.json);;DREAM.3D File (*.dream3d);;Text File (*.txt);;Ini File (*.ini);;All Files (*.*)"));
      if (true == newPrefPaths.isEmpty()) { return; }

      QModelIndex parent = bookmarksDockWidget->getSelectedParentTreeItem();

      for (int i = 0; i < newPrefPaths.size(); i++)
      {
        QString newPrefPath = newPrefPaths[i];
        newPrefPath = QDir::toNativeSeparators(newPrefPath);
        bookmarksDockWidget->addBookmark(newPrefPath, parent);
      }

      if (newPrefPaths.size() > 0)
      {
        // Cache the directory from the last path added
        m_OpenDialogLastDirectory = newPrefPaths[newPrefPaths.size() - 1];
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionNewFolder_triggered()
{
  if (NULL != m_ActiveWindow)
  {
    BookmarksModel* model = BookmarksModel::Instance();
    BookmarksDockWidget* bookmarksDockWidget = m_ActiveWindow->getBookmarksDockWidget();

    QModelIndex parent = bookmarksDockWidget->getSelectedParentTreeItem();
    QString parentName = model->index(parent.row(), BookmarksItem::Name, parent.parent()).data().toString();

    QString name = "New Folder";

    bookmarksDockWidget->addTreeItem(parent, name, QIcon(":/folder_blue.png"), "", true, true, false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionClearPipeline_triggered()
{
  if (NULL != m_ActiveWindow)
  {
    m_ActiveWindow->clearPipeline();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionShowDREAM3DHelp_triggered()
{
  // Generate help page
  QUrl helpURL = DREAM3DHelpUrlGenerator::generateHTMLUrl("index");
  #ifdef DREAM3D_USE_QtWebEngine
    DREAM3DUserManualDialog::LaunchHelpDialog(helpURL);
  #else
    bool didOpen = QDesktopServices::openUrl(helpURL);
    if(false == didOpen)
    {
      QMessageBox msgBox;
      msgBox.setText(QString("Error Opening Help File"));
      msgBox.setInformativeText(QString::fromLatin1("DREAM3D could not open the help file path ") + helpURL.path());
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setDefaultButton(QMessageBox::Ok);
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
    }
  #endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionAboutDREAM3D_triggered()
{
  AboutDREAM3D d(NULL);
  d.exec();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionCheckForUpdates_triggered()
{
  DREAM3DUpdateCheckDialog* d = new DREAM3DUpdateCheckDialog(NULL);

  d->setCurrentVersion((SIMPLib::Version::Complete()));
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
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionPluginInformation_triggered()
{
  AboutPlugins dialog(NULL);
  dialog.exec();

  // Write cache on exit
  dialog.writePluginCache();

  /* If any of the load checkboxes were changed, display a dialog warning
   * the user that they must restart DREAM3D to see the changes.
   */
  if (dialog.getLoadPreferencesDidChange() == true)
  {
    QMessageBox msgBox;
    msgBox.setText("DREAM3D must be restarted to allow these changes to take effect.");
    msgBox.setInformativeText("Restart?");
    msgBox.setWindowTitle("Restart Needed");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int choice = msgBox.exec();

    if (choice == QMessageBox::Yes)
    {
      QMutableMapIterator<DREAM3D_UI*, QMenu*> iter(m_DREAM3DInstanceMap);
      while (iter.hasNext())
      {
        iter.next();

        DREAM3D_UI* dream3d = iter.key();
        dream3d->close();
      }

      // Restart DREAM3D
      QProcess::startDetached(QApplication::applicationFilePath());
      dream3dApp->quit();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionRenamePipeline_triggered()
{
  if (NULL != m_ActiveWindow)
  {
    BookmarksTreeView* bookmarksTreeView = m_ActiveWindow->getBookmarksDockWidget()->getBookmarksTreeView();
    QModelIndex index = bookmarksTreeView->currentIndex();
    bookmarksTreeView->edit(index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionRemovePipeline_triggered()
{
  if (NULL != m_ActiveWindow)
  {
    BookmarksDockWidget* bookmarksDockWidget = m_ActiveWindow->getBookmarksDockWidget();
    BookmarksTreeView* bookmarksTreeView = bookmarksDockWidget->getBookmarksTreeView();

    BookmarksModel* model = BookmarksModel::Instance();

    QModelIndex index = bookmarksTreeView->currentIndex();
    QModelIndex nameIndex = model->index(index.row(), BookmarksItem::Name, index.parent());
    QString name = nameIndex.data().toString();

    QMessageBox msgBox;
    if (model->flags(nameIndex).testFlag(Qt::ItemIsDropEnabled) == false)
    {
      msgBox.setWindowTitle("Remove Bookmark");
      msgBox.setText("Are you sure that you want to remove the bookmark \"" + name + "\"? The original file will not be removed.");
    }
    else
    {
      msgBox.setWindowTitle("Remove Folder");
      msgBox.setText("Are you sure that you want to remove the folder \"" + name + "\"? The folder's contents will also be removed.");
    }
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes)
    {
      //Remove favorite, graphically, from the DREAM3D interface
      model->removeRow(index.row(), index.parent());

      // Write these changes out to the preferences file
      emit bookmarksDockWidget->fireWriteSettings();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionLocateFile_triggered()
{
  if (NULL != m_ActiveWindow)
  {
    BookmarksModel* model = BookmarksModel::Instance();
    BookmarksDockWidget* bookmarksDockWidget = m_ActiveWindow->getBookmarksDockWidget();
    BookmarksTreeView* bookmarksTreeView = bookmarksDockWidget->getBookmarksTreeView();

    QModelIndex current = bookmarksTreeView->currentIndex();

    QModelIndex nameIndex = model->index(current.row(), BookmarksItem::Name, current.parent());
    QModelIndex pathIndex = model->index(current.row(), BookmarksItem::Path, current.parent());

    QFileInfo fi(pathIndex.data().toString());
    QString restrictions;
    if (fi.completeSuffix() == "json")
    {
      restrictions = "Json File (*.json)";
    }
    else if (fi.completeSuffix() == "dream3d")
    {
      restrictions = "Dream3d File(*.dream3d)";
    }
    else if (fi.completeSuffix() == "txt")
    {
      restrictions = "Text File (*.txt)";
    }
    else
    {
      restrictions = "Ini File (*.ini)";
    }

    QString filePath = QFileDialog::getOpenFileName(bookmarksTreeView, tr("Locate Pipeline File"),
                                                    pathIndex.data().toString(), tr(restrictions.toStdString().c_str()));
    if (true == filePath.isEmpty()) { return; }

    filePath = QDir::toNativeSeparators(filePath);

    // Set the new path into the item
    model->setData(pathIndex, filePath, Qt::DisplayRole);

    // Change item back to default look and functionality
    model->setData(nameIndex, false, Qt::UserRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionShowBookmarkInFileSystem_triggered()
{
  if (NULL != m_ActiveWindow)
  {
    BookmarksModel* model = BookmarksModel::Instance();
    BookmarksDockWidget* bookmarksDockWidget = m_ActiveWindow->getBookmarksDockWidget();
    BookmarksTreeView* bookmarksTreeView = bookmarksDockWidget->getBookmarksTreeView();

    QModelIndex index = bookmarksTreeView->currentIndex();
    if(index.isValid())
    {
      QString pipelinePath = model->index(index.row(), BookmarksItem::Path, index.parent()).data().toString();

      QFileInfo pipelinePathInfo(pipelinePath);
      QString pipelinePathDir = pipelinePathInfo.path();

      QString s("file://");
#if defined(Q_OS_WIN)
      s = s + "/"; // Need the third slash on windows because file paths start with a drive letter
#elif defined(Q_OS_MAC)

#else
      // We are on Linux - I think

#endif
      s = s + pipelinePathDir;
      QDesktopServices::openUrl(s);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionShowPrebuiltInFileSystem_triggered()
{
  if (NULL != m_ActiveWindow)
  {
    PrebuiltPipelinesDockWidget* prebuiltsDockWidget = m_ActiveWindow->getPrebuiltsDockWidget();
    FilterLibraryTreeWidget* prebuiltsLibraryTree = prebuiltsDockWidget->getFilterLibraryTreeWidget();

    QTreeWidgetItem* item = prebuiltsLibraryTree->currentItem();
    QString pipelinePath = item->data(1, Qt::UserRole).toString();

    QFileInfo pipelinePathInfo(pipelinePath);
    QString pipelinePathDir = pipelinePathInfo.path();

    QString s("file://");
#if defined(Q_OS_WIN)
    s = s + "/"; // Need the third slash on windows because file paths start with a drive letter
#elif defined(Q_OS_MAC)

#else
    // We are on Linux - I think

#endif
    s = s + pipelinePathDir;
    QDesktopServices::openUrl(s);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionCloseWindow_triggered()
{
  m_ActiveWindow->close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionExit_triggered()
{
  bool shouldReallyClose = true;
  QMutableMapIterator<DREAM3D_UI*, QMenu*> iter(m_DREAM3DInstanceMap);
  while (iter.hasNext())
  {
    iter.next();

    DREAM3D_UI* dream3dWindow = iter.key();
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
void DREAM3DApplication::on_pipelineViewContextMenuRequested(const QPoint& pos)
{
  PipelineViewWidget* pipelineView = m_ActiveWindow->getPipelineViewWidget();
  QMenu menu;

#if defined(Q_OS_MAC)
  menu.addAction(m_GlobalMenu->getClearPipeline());
#else
  if (NULL != m_ActiveWindow)
  {
    menu.addAction(m_ActiveWindow->getDREAM3DMenu()->getClearPipeline());
  }
#endif

  if (NULL != m_ActiveWindow)
  {
    menu.exec(pipelineView->mapToGlobal(pos));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_bookmarksDockContextMenuRequested(const QPoint& pos)
{
  BookmarksTreeView* bookmarksTreeView = m_ActiveWindow->getBookmarksDockWidget()->getBookmarksTreeView();

  QModelIndex index = bookmarksTreeView->indexAt(pos);

  QPoint mapped;
  if (index.isValid())
  {
    // Note: We must map the point to global from the viewport to
    // account for the header.
    mapped = bookmarksTreeView->viewport()->mapToGlobal(pos);
  }
  else
  {
    index = QModelIndex();
    mapped = bookmarksTreeView->mapToGlobal(pos);
  }

  BookmarksModel* model = BookmarksModel::Instance();

  QAction* m_ActionAddPipeline;
  QAction* m_ActionNewFolder;
  QAction* m_ActionRenamePipeline;
  QAction* m_ActionRemovePipeline;
  QAction* m_ActionLocateFile;
  QAction* m_ActionShowBookmarkInFileSystem;
#if defined(Q_OS_MAC)
  m_ActionAddPipeline = m_GlobalMenu->getAddBookmark();
  m_ActionNewFolder = m_GlobalMenu->getNewFolder();
  m_ActionRenamePipeline = m_GlobalMenu->getRenamePipeline();
  m_ActionRemovePipeline = m_GlobalMenu->getRemovePipeline();
  m_ActionLocateFile = m_GlobalMenu->getLocateFile();
  m_ActionShowBookmarkInFileSystem = m_GlobalMenu->getShowBookmarkInFileSystem();
#else
  m_ActionAddPipeline = m_ActiveWindow->getDREAM3DMenu()->getAddBookmark();
  m_ActionNewFolder = m_ActiveWindow->getDREAM3DMenu()->getNewFolder();
  m_ActionRenamePipeline = m_ActiveWindow->getDREAM3DMenu()->getRenamePipeline();
  m_ActionRemovePipeline = m_ActiveWindow->getDREAM3DMenu()->getRemovePipeline();
  m_ActionLocateFile = m_ActiveWindow->getDREAM3DMenu()->getLocateFile();
  m_ActionShowBookmarkInFileSystem = m_ActiveWindow->getDREAM3DMenu()->getShowBookmarkInFileSystem();
#endif

  QMenu menu;
  if (index.isValid() == false)
  {
    menu.addAction(m_ActionAddPipeline);
    {
      QAction* separator = new QAction(this);
      separator->setSeparator(true);
      menu.addAction(separator);
    }
    menu.addAction(m_ActionNewFolder);
  }
  else
  {
    QModelIndex actualIndex = model->index(index.row(), BookmarksItem::Path, index.parent());
    QString path = actualIndex.data().toString();
    if (path.isEmpty() == false)
    {
      bool itemHasErrors = model->data(actualIndex, Qt::UserRole).value<bool>();
      if (itemHasErrors == true)
      {
        menu.addAction(m_ActionLocateFile);

        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }

        m_ActionRemovePipeline->setText("Remove Bookmark");
        menu.addAction(m_ActionRemovePipeline);
      }
      else
      {
        menu.addAction(m_ActionAddPipeline);

        m_ActionRenamePipeline->setText("Rename Bookmark");
        menu.addAction(m_ActionRenamePipeline);

        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }

        m_ActionRemovePipeline->setText("Remove Bookmark");
        menu.addAction(m_ActionRemovePipeline);

        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }

        menu.addAction(m_ActionShowBookmarkInFileSystem);
      }
    }
    else if (path.isEmpty())
    {
      menu.addAction(m_ActionAddPipeline);

      m_ActionRenamePipeline->setText("Rename Folder");
      menu.addAction(m_ActionRenamePipeline);

      {
        QAction* separator = new QAction(this);
        separator->setSeparator(true);
        menu.addAction(separator);
      }

      m_ActionRemovePipeline->setText("Remove Folder");
      menu.addAction(m_ActionRemovePipeline);

      {
        QAction* separator = new QAction(this);
        separator->setSeparator(true);
        menu.addAction(separator);
      }

      menu.addAction(m_ActionNewFolder);
    }
  }

  menu.exec(mapped);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_prebuiltsDockContextMenuRequested(const QPoint& pos)
{
  PrebuiltPipelinesDockWidget* prebuiltsDockWidget = m_ActiveWindow->getPrebuiltsDockWidget();
  QMenu menu;

#if defined(Q_OS_MAC)
  menu.addAction(m_GlobalMenu->getShowPrebuiltInFileSystem());
#else
  if (NULL != m_ActiveWindow)
  {
    menu.addAction(m_ActiveWindow->getDREAM3DMenu()->getShowPrebuiltInFileSystem());
  }
#endif

  if (NULL != m_ActiveWindow)
  {
    menu.exec(prebuiltsDockWidget->mapToGlobal(pos));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionShowFilterList_triggered(bool visible)
{
  if (NULL != m_ActiveWindow)
  {
    QAction* actionShowFilterList = qobject_cast<QAction*>(sender());
    FilterListDockWidget* filterListDockWidget = m_ActiveWindow->getFilterListDockWidget();

    if (NULL != actionShowFilterList && NULL != filterListDockWidget)
    {
      m_ActiveWindow->updateAndSyncDockWidget(actionShowFilterList, filterListDockWidget, visible);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionShowFilterLibrary_triggered(bool visible)
{
  if (NULL != m_ActiveWindow)
  {
    QAction* actionShowFilterLibrary = qobject_cast<QAction*>(sender());
    FilterLibraryDockWidget* filterLibraryDockWidget = m_ActiveWindow->getFilterLibraryDockWidget();

    if (NULL != actionShowFilterLibrary && NULL != filterLibraryDockWidget)
    {
      m_ActiveWindow->updateAndSyncDockWidget(actionShowFilterLibrary, filterLibraryDockWidget, visible);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionShowBookmarks_triggered(bool visible)
{
  if (NULL != m_ActiveWindow)
  {
    QAction* actionShowBookmarks = qobject_cast<QAction*>(sender());
    BookmarksDockWidget* bookmarksDockWidget = m_ActiveWindow->getBookmarksDockWidget();

    if (NULL != actionShowBookmarks && NULL != bookmarksDockWidget)
    {
      m_ActiveWindow->updateAndSyncDockWidget(actionShowBookmarks, bookmarksDockWidget, visible);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionShowPrebuiltPipelines_triggered(bool visible)
{
  if (NULL != m_ActiveWindow)
  {
    QAction* actionShowPrebuilts = qobject_cast<QAction*>(sender());
    PrebuiltPipelinesDockWidget* prebuiltsDockWidget = m_ActiveWindow->getPrebuiltsDockWidget();

    if (NULL != actionShowPrebuilts && NULL != prebuiltsDockWidget)
    {
      m_ActiveWindow->updateAndSyncDockWidget(actionShowPrebuilts, prebuiltsDockWidget, visible);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionShowIssues_triggered(bool visible)
{
  if (NULL != m_ActiveWindow)
  {
    QAction* actionShowIssues = qobject_cast<QAction*>(sender());
    IssuesDockWidget* issuesDockWidget = m_ActiveWindow->getIssuesDockWidget();

    if (NULL != actionShowIssues && NULL != issuesDockWidget)
    {
      m_ActiveWindow->updateAndSyncDockWidget(actionShowIssues, issuesDockWidget, visible);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionClearCache_triggered()
{
  QMessageBox msgBox;
  msgBox.setWindowTitle("Clear DREAM3D Cache");
  msgBox.setText("Clearing the DREAM3D cache will clear the bookmarks dock and DREAM3D window settings, and will restore DREAM3D back to its default settings on the program's next run.");
  msgBox.setInformativeText("Would you like to clear the DREAM3D cache?");
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::Yes);
  int response = msgBox.exec();

  if (response == QMessageBox::Yes)
  {
    DREAM3DSettings prefs;

    // Set a flag in the preferences file, so that we know that we are in "Clear Cache" mode
    prefs.setValue("Program Mode", QString("Clear Cache"));

    m_ActiveWindow->getPipelineViewWidget()->getStatusBar()->showMessage("The cache has been cleared successfully.  Please restart DREAM3D.");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::newInstanceFromFile(const QString& filePath, const bool& setOpenedFilePath, const bool& addToRecentFiles)
{
  DREAM3D_UI* ui = getNewDREAM3DInstance();
  QString nativeFilePath = QDir::toNativeSeparators(filePath);

  ui->getPipelineViewWidget()->openPipeline(nativeFilePath, 0, setOpenedFilePath, true);

  QRecentFileList* list = QRecentFileList::instance();
  if (addToRecentFiles == true)
  {
    // Add file to the recent files list
    list->addFile(filePath);
  }
  ui->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D_UI* DREAM3DApplication::getNewDREAM3DInstance()
{
  PluginManager* pluginManager = PluginManager::Instance();
  QVector<ISIMPLibPlugin*> plugins = pluginManager->getPluginsVector();

  // Create new DREAM3D instance
  DREAM3D_UI* newInstance = new DREAM3D_UI(NULL);
  newInstance->setLoadedPlugins(plugins);
  newInstance->setAttribute(Qt::WA_DeleteOnClose);
  newInstance->setWindowTitle("[*]Untitled Pipeline - DREAM3D");

  if (NULL != m_ActiveWindow)
  {
    newInstance->move(m_ActiveWindow->x() + 45, m_ActiveWindow->y() + 45);
    m_ActiveWindow->connectSignalsSlots(newInstance);
  }

  m_ActiveWindow = newInstance;

  connect(newInstance, SIGNAL(dream3dWindowChangedState(DREAM3D_UI*)), this, SLOT(activeWindowChanged(DREAM3D_UI*)));

  // Check if this is the first run of DREAM3D
  newInstance->checkFirstRun();

  return newInstance;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::activeWindowChanged(DREAM3D_UI* instance)
{
  if (instance->isActiveWindow())
  {
    m_ActiveWindow = instance;

#if defined(Q_OS_MAC)
    // Set this instance's view menu to the global menu
    QMenuBar* menuBar = m_GlobalMenu->getMenuBar();
    QMenu* viewMenuToAdd = m_DREAM3DInstanceMap.value(instance, NULL);
    if (NULL != viewMenuToAdd)
    {
      m_GlobalMenu->setViewMenu(viewMenuToAdd);
    }

    /* If the active signal got fired and there is now only one window,
     * this means that the first window has been opened.
     * Enable menu items. */
    if (m_DREAM3DInstanceMap.size() == 1)
    {
      // We are launching the first window, so remove the placeholder view menu
      if (NULL != m_PlaceholderViewMenu)
      {
        menuBar->removeAction(m_PlaceholderViewMenu->menuAction());
      }
      toggleGlobalMenuItems(true);
    }

    // Set the active window's menu state
    if (isCurrentlyRunning(m_ActiveWindow) == true)
    {
      toPipelineRunningState();
    }
    else
    {
      toPipelineIdleState();
    }
#endif
  }
  else
  {
#if defined(Q_OS_MAC)
    // Remove the inactive window's view menu from the menu bar
    QMenuBar* menuBar = m_GlobalMenu->getMenuBar();
    QMenu* viewMenuToRemove = m_DREAM3DInstanceMap.value(instance, NULL);
    if(NULL != viewMenuToRemove)
    {
      menuBar->removeAction(viewMenuToRemove->menuAction());
    }
#endif

    /* If the inactive signal got fired and there are no more windows,
     * this means that the last window has been closed.
     * Disable menu items. */
    if (m_DREAM3DInstanceMap.size() <= 0)
    {
      m_ActiveWindow = NULL;

#if defined(Q_OS_MAC)
      m_GlobalMenu->setViewMenu(m_PlaceholderViewMenu);
      toggleGlobalMenuItems(false);
#endif
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::toggleGlobalMenuItems(bool value)
{
#if defined(Q_OS_MAC)
  m_GlobalMenu->toggleMenuChildren(m_GlobalMenu->getViewMenu(), value);
  m_GlobalMenu->toggleMenuChildren(m_GlobalMenu->getBookmarksMenu(), value);
  m_GlobalMenu->toggleMenuChildren(m_GlobalMenu->getPipelineMenu(), value);

  m_GlobalMenu->getSave()->setEnabled(value);
  m_GlobalMenu->getSaveAs()->setEnabled(value);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::toPipelineRunningState()
{
#if defined (Q_OS_MAC)
  m_GlobalMenu->getClearPipeline()->setDisabled(true);
#else
  m_ActiveWindow->getDREAM3DMenu()->getClearPipeline()->setDisabled(true);
#endif

  DREAM3D_UI* runningInstance = qobject_cast<DREAM3D_UI*>(sender());
  if (NULL != runningInstance)
  {
    m_CurrentlyRunningInstances.insert(runningInstance);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::toPipelineIdleState()
{
#if defined (Q_OS_MAC)
  m_GlobalMenu->getClearPipeline()->setEnabled(true);
#else
  m_ActiveWindow->getDREAM3DMenu()->getClearPipeline()->setEnabled(true);
#endif

  DREAM3D_UI* runningInstance = qobject_cast<DREAM3D_UI*>(sender());
  if (NULL != runningInstance)
  {
    m_CurrentlyRunningInstances.remove(runningInstance);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DREAM3DApplication::isCurrentlyRunning(DREAM3D_UI* instance)
{
  return m_CurrentlyRunningInstances.contains(instance);
}

// -----------------------------------------------------------------------------
// This function is used if the last DREAM3D window gets closed.  Since the View
// menu relies on a DREAM3D instance, we usually create each instance's View menu
// inside of the DREAM3D_UI constructor.  However in this case, since there is no
// DREAM3D instance to use, we have to create a placeholder View menu here to use.
// -----------------------------------------------------------------------------
QMenu* DREAM3DApplication::createPlaceholderViewMenu()
{
  QMenu* menuView = new QMenu(NULL);
  menuView->setTitle(QApplication::translate("DREAM3D_UI", "View", 0));
  menuView->setObjectName(QStringLiteral("menuView"));

  QAction* actionShowFilterList = new QAction(menuView);
  actionShowFilterList->setText("Filter List");
  menuView->addAction(actionShowFilterList);


  QAction* actionShowFilterLibrary = new QAction(menuView);
  actionShowFilterLibrary->setText("Filter Library");
  menuView->addAction(actionShowFilterLibrary);


  QAction* actionShowBookmarks = new QAction(menuView);
  actionShowBookmarks->setText("Bookmarks");
  menuView->addAction(actionShowBookmarks);


  QAction* actionShowPrebuiltPipelines = new QAction(menuView);
  actionShowPrebuiltPipelines->setText("Prebuilt Pipelines");
  menuView->addAction(actionShowPrebuiltPipelines);

  QAction* actionShowIssues = new QAction(menuView);
  actionShowIssues->setText("Show Warnings/Errors");
  menuView->addAction(actionShowIssues);

  return menuView;
}






