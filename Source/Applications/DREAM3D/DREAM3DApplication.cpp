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

#include <QtCore/QPluginLoader>
#include <QtCore/QProcess>

#include <QtWidgets/QFileDialog>

#include <QtGui/QDesktopServices>
#include <QtGui/QBitmap>

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"

#include "QtSupportLib/QRecentFileList.h"
#include "QtSupportLib/DREAM3DHelpUrlGenerator.h"
#include "QtSupportLib/ApplicationAboutBoxDialog.h"

#include "DREAM3DWidgetsLib/Widgets/DREAM3DUpdateCheckDialog.h"
#ifdef DREAM3D_USE_QtWebEngine
#include "Applications/Common/DREAM3DUserManualDialog.h"
#endif

#include "Applications/DREAM3D/DREAM3D_UI.h"
#include "Applications/DREAM3D/AboutDREAM3D.h"
#include "Applications/DREAM3D/AboutPlugins.h"
#include "Applications/DREAM3D/DREAM3DConstants.h"
#include "Applications/DREAM3D/DREAM3DToolbox.h"
#include "Applications/DREAM3D/DREAM3DMenuItems.h"

#include "DSplashScreen.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_DREAM3DApplication.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DApplication::DREAM3DApplication(int& argc, char** argv) :
  QApplication(argc, argv),
  m_ActiveWindow(NULL),
  m_OpenDialogLastDirectory(""),
  show_splash(true),
  Splash(NULL)
{
  // Create the toolbox
  m_Toolbox = DREAM3DToolbox::Instance();

  connect(m_Toolbox, SIGNAL(toolboxChangedState()), this, SLOT(toolboxWindowChanged()));

  // Connection to update the recent files list on all windows when it changes
  QRecentFileList* recentsList = QRecentFileList::instance();
  connect(recentsList, SIGNAL(fileListChanged(const QString&)),
          this, SLOT(updateRecentFileList(const QString&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DApplication::~DREAM3DApplication()
{
  delete this->Splash;
  this->Splash = NULL;

  DREAM3DToolbox* toolbox = DREAM3DToolbox::Instance();
  toolbox->writeSettings();

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

  // Create and show the splash screen as the main window is being created.
  QPixmap pixmap(QLatin1String(":/splash/branded_splash.png"));
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

  // Read the toolbox settings and update the filter list
  m_Toolbox->readSettings();
  m_Toolbox->getFilterListWidget()->updateFilterList(true);

  // Set the "Show Toolbox" action to the correct state
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();
  menuItems->getActionShowToolbox()->setChecked(m_Toolbox->isVisible());

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
  // This should never be executed
  return;
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
void DREAM3DApplication::on_actionClearRecentFiles_triggered()
{
  // This should never be executed
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::addFilter(const QString &text)
{
  if (NULL != m_ActiveWindow)
  {
    m_ActiveWindow->getPipelineViewWidget()->addFilter(text);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionCloseToolbox_triggered()
{
  DREAM3DToolbox* toolbox = DREAM3DToolbox::Instance();
  toolbox->close();
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
  if (filePath.isEmpty())
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
  DREAM3DToolbox* toolbox = DREAM3DToolbox::Instance();

  toolbox->setCurrentTab(DREAM3DToolbox::Bookmarks);

  BookmarksToolboxWidget* bookmarksToolboxWidget = toolbox->getBookmarksWidget();

  if (NULL != bookmarksToolboxWidget)
  {
    QString proposedDir = m_OpenDialogLastDirectory;
    QList<QString> newPrefPaths;

    newPrefPaths = QFileDialog::getOpenFileNames(toolbox, tr("Choose Pipeline File(s)"),
      proposedDir, tr("Json File (*.json);;DREAM.3D File (*.dream3d);;Text File (*.txt);;Ini File (*.ini);;All Files (*.*)"));
    if (true == newPrefPaths.isEmpty()) { return; }

    QModelIndex parent = m_Toolbox->getBookmarksWidget()->getBookmarksTreeView()->currentIndex();

    if (parent.isValid() == false)
    {
      parent = QModelIndex();
    }

    for (int i = 0; i < newPrefPaths.size(); i++)
    {
      QString newPrefPath = newPrefPaths[i];
      newPrefPath = QDir::toNativeSeparators(newPrefPath);
      bookmarksToolboxWidget->addBookmark(newPrefPath, parent);
    }

    if (newPrefPaths.size() > 0)
    {
      // Cache the directory from the last path added
      m_OpenDialogLastDirectory = newPrefPaths[newPrefPaths.size() - 1];
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionNewFolder_triggered()
{
  DREAM3DToolbox* toolbox = DREAM3DToolbox::Instance();

  toolbox->setCurrentTab(DREAM3DToolbox::Bookmarks);

  BookmarksModel* model = BookmarksModel::Instance();
  BookmarksToolboxWidget* bookmarksToolboxWidget = toolbox->getBookmarksWidget();

  QModelIndex parent = m_Toolbox->getBookmarksWidget()->getBookmarksTreeView()->currentIndex();

  if (parent.isValid() == false)
  {
    parent = QModelIndex();
  }

  QString name = "New Folder";

  bookmarksToolboxWidget->addTreeItem(parent, name, QIcon(":/folder_blue.png"), "", model->rowCount(parent), true, true, false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionShowToolbox_triggered(bool visible)
{
  DREAM3DToolbox* toolbox = DREAM3DToolbox::Instance();
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();
  QAction* actionShowToolbox = menuItems->getActionShowToolbox();

  actionShowToolbox->blockSignals(true);
  toolbox->blockSignals(true);

  actionShowToolbox->setChecked(visible);
  toolbox->setVisible(visible);

  actionShowToolbox->blockSignals(false);
  toolbox->blockSignals(false);
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
  if (false == didOpen)
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
      for (int i = 0; i < m_DREAM3DInstances.size(); i++)
      {
        DREAM3D_UI* dream3d = m_DREAM3DInstances[i];
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
void DREAM3DApplication::on_actionRenameBookmark_triggered()
{
    BookmarksTreeView* bookmarksTreeView = m_Toolbox->getBookmarksWidget()->getBookmarksTreeView();
    QModelIndex index = bookmarksTreeView->currentIndex();
    bookmarksTreeView->edit(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionRemoveBookmark_triggered()
{
  BookmarksToolboxWidget* bookmarksToolboxWidget = m_Toolbox->getBookmarksWidget();

  BookmarksModel* model = BookmarksModel::Instance();

  QModelIndexList indexList = m_Toolbox->getBookmarksWidget()->getBookmarksTreeView()->selectionModel()->selectedRows(BookmarksItem::Name);

  indexList = m_Toolbox->getBookmarksWidget()->getBookmarksTreeView()->filterOutDescendants(indexList);

  if (indexList.size() <= 0)
  {
    return;
  }

  QList<QPersistentModelIndex> persistentList;
  for (int i = 0; i < indexList.size(); i++)
  {
    persistentList.push_back(indexList[i]);
  }

  QModelIndex singleIndex = model->index(indexList[0].row(), BookmarksItem::Name, indexList[0].parent());

  QMessageBox msgBox;
  if (indexList.size() > 1)
  {
    msgBox.setWindowTitle("Remove Bookmark Items");
    msgBox.setText("Are you sure that you want to remove these bookmark items? The original bookmark files will not be removed.");
  }
  else if (model->flags(singleIndex).testFlag(Qt::ItemIsDropEnabled) == false)
  {
    msgBox.setWindowTitle("Remove Bookmark");
    msgBox.setText("Are you sure that you want to remove the bookmark \"" + singleIndex.data().toString() + "\"? The original file will not be removed.");
  }
  else
  {
    msgBox.setWindowTitle("Remove Folder");
    msgBox.setText("Are you sure that you want to remove the folder \"" + singleIndex.data().toString() + "\"? The folder's contents will also be removed.");
  }
  msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
  msgBox.setDefaultButton(QMessageBox::Yes);
  int ret = msgBox.exec();

  if (ret == QMessageBox::Yes)
  {
    for (int i = 0; i < persistentList.size(); i++)
    {
      QModelIndex nameIndex = model->index(persistentList[i].row(), BookmarksItem::Name, persistentList[i].parent());
      QString name = nameIndex.data().toString();

      //Remove bookmark from the DREAM3D interface
      model->removeRow(persistentList[i].row(), persistentList[i].parent());
    }

    // Write these changes out to the preferences file
    emit bookmarksToolboxWidget->fireWriteSettings();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionLocateFile_triggered()
{
  BookmarksModel* model = BookmarksModel::Instance();
  BookmarksToolboxWidget* bookmarksToolboxWidget = m_Toolbox->getBookmarksWidget();
  BookmarksTreeView* bookmarksTreeView = bookmarksToolboxWidget->getBookmarksTreeView();

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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionShowBookmarkInFileSystem_triggered()
{
  BookmarksModel* model = BookmarksModel::Instance();
  BookmarksToolboxWidget* bookmarksToolboxWidget = m_Toolbox->getBookmarksWidget();
  BookmarksTreeView* bookmarksTreeView = bookmarksToolboxWidget->getBookmarksTreeView();

  QModelIndex index = bookmarksTreeView->currentIndex();
  if (index.isValid())
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_pipelineViewContextMenuRequested(const QPoint& pos)
{
  if (NULL != m_ActiveWindow)
  {
    PipelineViewWidget* pipelineView = m_ActiveWindow->getPipelineViewWidget();
    DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();
    QMenu menu;

    menu.addAction(menuItems->getActionClearPipeline());
    menu.exec(pipelineView->mapToGlobal(pos));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_bookmarksDockContextMenuRequested(const QPoint& pos)
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();
  BookmarksTreeView* bookmarksTreeView = m_ActiveWindow->getBookmarksToolboxWidget()->getBookmarksTreeView();

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

  QAction* actionAddBookmark = menuItems->getActionAddBookmark();
  QAction* actionNewFolder = menuItems->getActionNewFolder();
  QAction* actionRenameBookmark = menuItems->getActionRenameBookmark();
  QAction* actionRemoveBookmark = menuItems->getActionRemoveBookmark();
  QAction* actionLocateFile = menuItems->getActionLocateFile();
  QAction* actionShowBookmarkInFileSystem = menuItems->getActionShowBookmarkInFileSystem();

  QModelIndexList indexList = m_Toolbox->getBookmarksWidget()->getBookmarksTreeView()->selectionModel()->selectedRows(BookmarksItem::Name);

  QMenu menu;
  if (index.isValid() == false)
  {
    menu.addAction(actionAddBookmark);
    {
      QAction* separator = new QAction(this);
      separator->setSeparator(true);
      menu.addAction(separator);
    }
    menu.addAction(actionNewFolder);
  }
  else
  {
    QModelIndex actualIndex = model->index(index.row(), BookmarksItem::Path, index.parent());
    QString path = actualIndex.data().toString();
    if (indexList.size() > 1)
    {
      actionRemoveBookmark->setText("Remove Items");
      menu.addAction(actionRemoveBookmark);
    }
    else if (path.isEmpty() == false)
    {
      bool itemHasErrors = model->data(actualIndex, Qt::UserRole).value<bool>();
      if (itemHasErrors == true)
      {
        menu.addAction(actionLocateFile);

        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }

        actionRemoveBookmark->setText("Remove Bookmark");
        menu.addAction(actionRemoveBookmark);
      }
      else
      {
        actionRenameBookmark->setText("Rename Bookmark");
        menu.addAction(actionRenameBookmark);

        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }

        actionRemoveBookmark->setText("Remove Bookmark");
        menu.addAction(actionRemoveBookmark);

        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }

        menu.addAction(actionShowBookmarkInFileSystem);
      }
    }
    else if (path.isEmpty())
    {
      menu.addAction(actionAddBookmark);

      actionRenameBookmark->setText("Rename Folder");
      menu.addAction(actionRenameBookmark);

      {
        QAction* separator = new QAction(this);
        separator->setSeparator(true);
        menu.addAction(separator);
      }

      actionRemoveBookmark->setText("Remove Folder");
      menu.addAction(actionRemoveBookmark);

      {
        QAction* separator = new QAction(this);
        separator->setSeparator(true);
        menu.addAction(separator);
      }

      menu.addAction(actionNewFolder);
    }
  }

  menu.exec(mapped);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::on_actionClearBookmarks_triggered()
{
  QMessageBox msgBox;
  msgBox.setWindowTitle("Clear DREAM3D Bookmarks");
  msgBox.setText("Are you sure that you want to clear all DREAM3D bookmarks?");
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::Yes);
  int response = msgBox.exec();

  if (response == QMessageBox::Yes)
  {
    BookmarksModel* model = BookmarksModel::Instance();
    if (model->isEmpty() == false)
    {
      model->removeRows(0, model->rowCount(QModelIndex()));
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
  msgBox.setText("Clearing the DREAM3D cache will clear the DREAM3D window settings, and will restore DREAM3D back to its default settings on the program's next run.");
  msgBox.setInformativeText("Clear the DREAM3D cache?");
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::Yes);
  int response = msgBox.exec();

  if (response == QMessageBox::Yes)
  {
    QSharedPointer<DREAM3DSettings> prefs = QSharedPointer<DREAM3DSettings>(new DREAM3DSettings());

    // Set a flag in the preferences file, so that we know that we are in "Clear Cache" mode
    prefs->setValue("Program Mode", QString("Clear Cache"));

    if (NULL != m_ActiveWindow)
    {
      m_ActiveWindow->getPipelineViewWidget()->getStatusBar()->showMessage("The cache has been cleared successfully.  Please restart DREAM3D.");
    }
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
  for (int i = 0; i<m_DREAM3DInstances.size(); i++)
  {
    DREAM3D_UI* dream3dWindow = m_DREAM3DInstances[i];
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
void DREAM3DApplication::bookmarkSelectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
  BookmarksModel* model = BookmarksModel::Instance();
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();
  QAction* actionAddBookmark = menuItems->getActionAddBookmark();
  QAction* actionNewFolder = menuItems->getActionNewFolder();

  if (model->index(current.row(), BookmarksItem::Path, current.parent()).data().toString().isEmpty() == true)
  {
    actionAddBookmark->setEnabled(true);
    actionNewFolder->setEnabled(true);
  }
  else
  {
    actionAddBookmark->setDisabled(true);
    actionNewFolder->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::dream3dWindowChanged(DREAM3D_UI *instance)
{
  // This should never be executed
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::toolboxWindowChanged()
{
  // This should never be executed
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<DREAM3D_UI*> DREAM3DApplication::getDREAM3DInstances()
{
  return m_DREAM3DInstances;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::registerDREAM3DWindow(DREAM3D_UI* window)
{
  m_DREAM3DInstances.push_back(window);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::unregisterDREAM3DWindow(DREAM3D_UI* window)
{
  // This should never be executed
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::toPipelineRunningState()
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();
  menuItems->getActionClearPipeline()->setDisabled(true);

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
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();
  menuItems->getActionClearPipeline()->setEnabled(true);

  DREAM3D_UI* runningInstance = qobject_cast<DREAM3D_UI*>(sender());
  if (NULL != runningInstance)
  {
    m_CurrentlyRunningInstances.remove(runningInstance);
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
  newInstance->setWindowTitle("[*]Untitled Pipeline - DREAM.3D");

  if (NULL != m_ActiveWindow)
  {
    newInstance->move(m_ActiveWindow->x() + 45, m_ActiveWindow->y() + 45);
  }

  m_ActiveWindow = newInstance;

  connect(newInstance, SIGNAL(dream3dWindowChangedState(DREAM3D_UI*)), this, SLOT(dream3dWindowChanged(DREAM3D_UI*)));

  // Check if this is the first run of DREAM3D
  newInstance->checkFirstRun();

  return newInstance;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::setActiveWindow(DREAM3D_UI* instance)
{
  m_ActiveWindow = instance;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DREAM3DApplication::isCurrentlyRunning(DREAM3D_UI* instance)
{
  return m_CurrentlyRunningInstances.contains(instance);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenuBar* DREAM3DApplication::getDREAM3DMenuBar()
{
  // This should never be executed
  return NULL;
}






