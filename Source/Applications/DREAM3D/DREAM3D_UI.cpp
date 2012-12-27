/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "DREAM3D_UI.h"

//-- Qt Includes
#include <QtCore/QPluginLoader>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QThread>
#include <QtCore/QFileInfoList>
#include <QtGui/QApplication>
#include <QtGui/QFileDialog>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QListWidget>


//-- DREAM3D Includes
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DREAM3DVersion.h"
#include "QtSupport/ApplicationAboutBoxDialog.h"
#include "QtSupport/QRecentFileList.h"
#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/DREAM3DQtMacros.h"
#include "QtSupport/DREAM3DPluginFrame.h"
#include "QtSupport/DREAM3DPluginInterface.h"
#include "QtSupport/HelpDialog.h"
#include "PipelineBuilder/PipelineBuilderWidget.h"
#include "FilterWidgets/FilterWidgetsLib.h"

#include "DREAM3D/License/DREAM3DLicenseFiles.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D_UI::DREAM3D_UI(QWidget *parent) :
  QMainWindow(parent),
  m_WorkerThread(NULL),
  m_PluginActionGroup(NULL),
  m_PluginPrefsActionGroup(NULL),
  m_ActivePlugin(NULL),
  m_PluginToolBar(NULL),
  m_HelpDialog(NULL),
  m_PipelineBuilderWidget(NULL)
{
  m_OpenDialogLastDirectory = QDir::homePath();
  // Calls the Parent Class to do all the Widget Initialization that were created
  // using the QDesigner program
  setupUi(this);

  // Do our own widget initializations
  setupGui();

  // Read the Preferences for each plugin and our own settings
  readSettings();

  // Get out initial Recent File List
  this->updateRecentFileList(QString::null);
  this->setAcceptDrops(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D_UI::~DREAM3D_UI()
{
  if (m_WorkerThread != NULL) {
    delete m_WorkerThread;
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::resizeEvent ( QResizeEvent * event )
{
 // std::cout << "DREAM3D_UI::resizeEvent" << std::endl;
 // std::cout << "   oldSize: " << event->oldSize().width() << " x " << event->oldSize().height() << std::endl;
 // std::cout << "   newSize: " << event->size().width() << " x " << event->size().height() << std::endl;
  emit parentResized();
 // std::cout << "DREAM3D_UI::resizeEvent --- Done" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionExit_triggered()
{
  this->close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionOpen_Pipeline_2_triggered() {
  QString file = QFileDialog::getOpenFileName(m_PipelineBuilderWidget, tr("Select Settings File"),
    m_PipelineBuilderWidget->getLastDirectory(),
    tr("Settings File (*.txt)") );
  if ( true == file.isEmpty() ) { return; }
  QSettings prefs(file, QSettings::IniFormat, this);
  m_PipelineBuilderWidget->readSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionSave_Pipeline_2_triggered() {
  QString proposedFile = m_PipelineBuilderWidget->getLastDirectory() + QDir::separator() + "PipelineBuilderSettings.txt";
  QString filePath = QFileDialog::getSaveFileName(this, tr("Save PipelineBuilder Settings"),
    proposedFile,
    tr("*.txt") );
  if ( true == filePath.isEmpty() ) { return; }

  //If the filePath already exists - delete it so that we get a clean write to the file
  QFileInfo fi(filePath);
  if (fi.exists() == true)
  {
    QFile f(filePath);
    f.remove();
  }
  QSettings prefs(filePath, QSettings::IniFormat, this);
  m_PipelineBuilderWidget->savePipeline(prefs);
}

// -----------------------------------------------------------------------------
//  Called when the main window is closed.
// -----------------------------------------------------------------------------
void DREAM3D_UI::closeEvent(QCloseEvent *event)
{
  qint32 err = checkDirtyDocument();
  if (err < 0)
  {
    event->ignore();
  }
  else
  {
    writeSettings();
    event->accept();
  }
}



// -----------------------------------------------------------------------------
//  Read the prefs from the local storage file
// -----------------------------------------------------------------------------
void DREAM3D_UI::readSettings()
{
  // std::cout << "Read Settings" << std::endl;
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif
//  bool ok = false;

//  Qt::ToolButtonStyle tbstyle = static_cast<Qt::ToolButtonStyle>(prefs.value("PluginDisplay").toUInt(&ok));
//  m_PluginToolBar->setToolButtonStyle(tbstyle);
//
//  // Read the preference for each plugin
//  foreach (DREAM3DPluginInterface* plugin, m_LoadedPlugins) {
//    plugin->readSettings(prefs);
//  }

  // Have the PipelineBuilder Widget read its settings
  m_PipelineBuilderWidget->readSettings(prefs);
  readWindowSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::readWindowSettings(QSettings &prefs)
{
  bool ok = false;
  prefs.beginGroup("WindowSettings");
  if (prefs.contains(QString("Geometry")) )
  {
    QByteArray geo_data = prefs.value(QString("Geometry")).toByteArray();
    ok = restoreGeometry(geo_data);
    if (!ok)
    {
      std::cout << "Error Restoring the Window Geometry" << std::endl;
    }
  }

  if (prefs.contains(QString("Layout")))
  {
    QByteArray layout_data = prefs.value(QString("Layout")).toByteArray();
    restoreState(layout_data);
  }
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//  Write our Prefs to file
// -----------------------------------------------------------------------------
void DREAM3D_UI::writeSettings()
{
  // std::cout << "writeSettings" << std::endl;
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif

//  if (m_ActivePlugin != NULL) {
//    prefs.setValue("ActivePlugin" , this->m_ActivePlugin->getPluginName());
//  }
//
//
//  foreach (DREAM3DPluginInterface* plugin, m_LoadedPlugins) {
//    plugin->writeSettings(prefs);
//  }

  // Have the pipeline builder write its settings to the prefs file
  m_PipelineBuilderWidget->writeSettings(prefs);

  writeWindowSettings(prefs);
//  prefs.setValue("PluginDisplay", this->m_PluginToolBar->toolButtonStyle());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::savePipeline(QSettings &prefs) {

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::writeWindowSettings(QSettings &prefs)
{
  prefs.beginGroup("WindowSettings");
  QByteArray geo_data = saveGeometry();
  QByteArray layout_data = saveState();
  prefs.setValue(QString("Geometry"), geo_data);
  prefs.setValue(QString("Layout"), layout_data);
  prefs.endGroup();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setupGui()
{
//  std::cout << "DREAM3D_UI::setupGui" << std::endl;
#if 0
  m_PluginActionGroup = new QActionGroup(this);
  m_PluginToolBar = new QToolBar(this);
  m_PluginToolBar->setObjectName(QString("PluginToolbar"));
  m_PluginToolBar->setVisible(false);

  // This should be a preference setting somewhere.
  m_PluginToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  m_PluginToolBar->setWindowTitle(tr("Show Plugin Toolbar"));
  addToolBar(m_PluginToolBar);

  // Get the Action to Display/Hide the
  QAction* showToolbarAction = m_PluginToolBar->toggleViewAction();
  menuPlugins->addAction(showToolbarAction);

  // Make these Menus Mutually Exclusive
  m_PluginPrefsActionGroup = new QActionGroup(this);
  m_PluginPrefsActionGroup->addAction(action_IconText);
  m_PluginPrefsActionGroup->addAction(action_IconOnly);
  m_PluginPrefsActionGroup->addAction(action_TextOnly);

  action_ShowPluginToolbar->setChecked(m_PluginToolBar->isVisible());
#endif

 // menubar->removeAction(menuPlugins->menuAction());

  m_HelpDialog = new HelpDialog(this);
  m_HelpDialog->setWindowModality(Qt::NonModal);

  // Look for plugins
  loadPlugins();

  // Register all of the Filters we know about - the rest will be loaded through plugins
  //  which all should have been loaded by now.
  FilterWidgetsLib::RegisterKnownQFilterWidgets();
  Q_INIT_RESOURCE(Generated_FilterDocs);

  // Now create our central widget
  m_PipelineBuilderWidget = new PipelineBuilderWidget(this);
  m_PipelineBuilderWidget->setStatusBar(this->statusBar());
  m_PipelineBuilderWidget->setPipelineMenu(this->menuPipeline);
  centerWidget->layout()->addWidget(m_PipelineBuilderWidget);

  connect(m_PipelineBuilderWidget, SIGNAL(fireWriteSettings()),
          this, SLOT(writeSettings()) );
  connect(m_PipelineBuilderWidget, SIGNAL(fireReadSettings()),
          this, SLOT(readSettings() ) );


  QKeySequence actionOpenKeySeq(Qt::CTRL + Qt::Key_O);
  actionOpen_Pipeline_2->setShortcut(actionOpenKeySeq);

  QKeySequence actionSaveKeySeq(Qt::CTRL + Qt::Key_S);
  actionSave_Pipeline_2->setShortcut(actionSaveKeySeq);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_action_ShowPluginToolbar_toggled(bool state )
{
  m_PluginToolBar->setVisible(state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_action_IconText_toggled(bool state)
{
  m_PluginToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_action_IconOnly_toggled(bool state)
{
  m_PluginToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_action_TextOnly_toggled(bool state)
{
  m_PluginToolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::dragEnterEvent(QDragEnterEvent* e)
{
  const QMimeData* dat = e->mimeData();
  QList<QUrl> urls = dat->urls();
  QString file = urls.count() ? urls[0].toLocalFile() : QString();
  QDir parent(file);
  this->m_OpenDialogLastDirectory = parent.dirName();
  QFileInfo fi(file );
  QString ext = fi.suffix();
  if (fi.exists() && fi.isFile() && ( ext.compare("mxa") || ext.compare("h5") || ext.compare("hdf5") ) )
  {
    e->accept();
  }
  else
  {
    e->ignore();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::dropEvent(QDropEvent* e)
{
  const QMimeData* dat = e->mimeData();
  QList<QUrl> urls = dat->urls();
  QString file = urls.count() ? urls[0].toLocalFile() : QString();
  QDir parent(file);
  this->m_OpenDialogLastDirectory = parent.dirName();
  QFileInfo fi(file );
  QString ext = fi.suffix();
  file = QDir::toNativeSeparators(file);
  if (fi.exists() && fi.isFile() )
  {
    //TODO: INSERT Drop Event CODE HERE
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint32 DREAM3D_UI::checkDirtyDocument()
{
  qint32 err = -1;

  if (this->isWindowModified() == true)
  {
    int r = QMessageBox::warning(this, tr("DREAM.3D"),
                            tr("The Data has been modified.\nDo you want to save your changes?"),
                            QMessageBox::Save | QMessageBox::Default,
                            QMessageBox::Discard,
                            QMessageBox::Cancel | QMessageBox::Escape);
    if (r == QMessageBox::Save)
    {
      //TODO: Save the current document or otherwise save the state.
    }
    else if (r == QMessageBox::Discard)
    {
      err = 1;
    }
    else if (r == QMessageBox::Cancel)
    {
      err = -1;
    }
  }
  else
  {
    err = 1;
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::updateRecentFileList(const QString &file)
{
 // std::cout << "DREAM3D_UI::updateRecentFileList" << std::endl;

  // Clear the Recent Items Menu
  this->menu_RecentFiles->clear();

  // Get the list from the static object
  QStringList files = QRecentFileList::instance()->fileList();
  foreach (QString file, files)
    {
      QAction* action = new QAction(this->menu_RecentFiles);
      action->setText(QRecentFileList::instance()->parentAndFileName(file));
      action->setData(file);
      action->setVisible(true);
      this->menu_RecentFiles->addAction(action);
      connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::openRecentFile()
{
  //std::cout << "QRecentFileList::openRecentFile()" << std::endl;

  QAction *action = qobject_cast<QAction *>(sender());
  if (action)
  {
    //std::cout << "Opening Recent file: " << action->data().toString().toStdString() << std::endl;
    QString file = action->data().toString();
    //TODO: use the 'file' object to figure out what to open
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::threadHasMessage(QString message)
{
 // std::cout << "DREAM3D_UI::threadHasMessage()" << message.toStdString() << std::endl;
  this->statusBar()->showMessage(message);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::loadPlugins()
{
//  std::cout << "DREAM3D_UI::loadPlugins" << std::endl;

  foreach (QObject *plugin, QPluginLoader::staticInstances())
    populateMenus(plugin);

  m_PluginDirs.clear();
  m_PluginDirs << qApp->applicationDirPath();

  QDir aPluginDir = QDir(qApp->applicationDirPath());
 // std::cout << "aPluginDir: " << aPluginDir.absolutePath().toStdString() << std::endl;
  QString thePath;

#if defined(Q_OS_WIN)
  if (aPluginDir.cd("plugins") )
  {
    thePath = aPluginDir.absolutePath();
    m_PluginDirs << thePath;
  }
#elif defined(Q_OS_MAC)
  if (aPluginDir.dirName() == "MacOS")
  {
    aPluginDir.cdUp();
    thePath = aPluginDir.absolutePath() + "/Plugins";
    m_PluginDirs << thePath;
    aPluginDir.cdUp();
    aPluginDir.cdUp();
  }
  // aPluginDir.cd("Plugins");
  thePath = aPluginDir.absolutePath() + "/Plugins";
  m_PluginDirs << thePath;

   // This is here for Xcode compatibility
#ifdef CMAKE_INTDIR
   aPluginDir.cdUp();
   thePath = aPluginDir.absolutePath() + "/Plugins/" + CMAKE_INTDIR;
   m_PluginDirs << thePath;
#endif
#else
  // We are on Linux - I think
  aPluginDir.cdUp();
  if (aPluginDir.cd("plugins"))
  {
    thePath = aPluginDir.absolutePath();
    m_PluginDirs << thePath;
  }
#endif

  this->setWindowTitle("DREAM3D");

  QStringList pluginFilePaths;

  foreach (QString pluginDirString, m_PluginDirs)
  {
    //std::cout << "Plugin Directory being Searched: " << pluginDirString.toStdString() << std::endl;
    aPluginDir = QDir(pluginDirString);
    foreach (QString fileName, aPluginDir.entryList(QDir::Files))
    {
      //   std::cout << "File: " << fileName.toStdString() << std::endl;
#ifdef QT_DEBUG
      if (fileName.endsWith("_debug.plugin", Qt::CaseSensitive))
#else
      if (fileName.endsWith( ".plugin", Qt::CaseSensitive) )
#endif
      {
        pluginFilePaths << aPluginDir.absoluteFilePath(fileName);
        //qWarning(aPluginDir.absoluteFilePath(fileName).toAscii(), "%s");
        //std::cout << "Adding " << aPluginDir.absoluteFilePath(fileName).toStdString() << std::endl;
      }
    }
  }


  // Now that we have a sorted list of plugins, go ahead and load them all from the
  // file system and add each to the toolbar and menu
  foreach(QString path, pluginFilePaths)
  {
    std::cout << "File Extension matches.." << std::endl;
    QPluginLoader loader(path);
    QFileInfo fi(path);
    QString fileName = fi.fileName();
    QObject *plugin = loader.instance();
    std::cout << "plugin Pointer: " << plugin << std::endl;
    if (plugin && m_PluginFileNames.contains(fileName, Qt::CaseSensitive) == false)
    {
      //populateMenus(plugin);
      DREAM3DPluginInterface* ipPlugin = qobject_cast<DREAM3DPluginInterface * > (plugin);
      if (ipPlugin)
      {
        m_LoadedPlugins.push_back(ipPlugin);
        ipPlugin->registerFilterWidgets();
      }

      m_PluginFileNames += fileName;
    }
    else
    {
      QString message("The plugin did not load with the following error\n");
      message.append(loader.errorString());
      QMessageBox::critical(this, "DREAM.3D Plugin Load Error",
                                message,
                                QMessageBox::Ok | QMessageBox::Default);
      //std::cout << "The plugin did not load with the following error\n   " << loader.errorString().toStdString() << std::endl;
    }
  }
#if 0
  if (NULL != m_PluginActionGroup)
  {
    QList<QAction*> actions = m_PluginActionGroup->actions();
    if (actions.isEmpty() == false)
    {
      menuPlugins->setEnabled(true);
    }
  }
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

 void DREAM3D_UI::populateMenus(QObject *plugin)
{
#if 0
#ifdef QT_DEBUG
  std::cout << "Found Plugin..." << std::endl;
#endif
  DREAM3DPluginInterface* ipPlugin = qobject_cast<DREAM3DPluginInterface * > (plugin);
  if (ipPlugin)
  {
    m_LoadedPlugins.push_back(ipPlugin);
#ifdef QT_DEBUG
    qWarning(ipPlugin->getPluginName().toAscii(), "%s");
#endif
    QIcon newIcon = ipPlugin->icon();

    addToPluginMenu(plugin, ipPlugin->getPluginName(),
                    menuPlugins, SLOT(setInputUI()), m_PluginActionGroup, newIcon);

    QAction* action = new QAction(newIcon, ipPlugin->getPluginName(), this);
    connect(action, SIGNAL(triggered()),
            plugin, SLOT(displayHelp()));
    connect(plugin, SIGNAL(showHelp(QUrl)),
            m_HelpDialog, SLOT(setContentFile(QUrl)));
    menuHelp->addAction(action);

  }
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::displayHelp(QString file)
{
  m_HelpDialog->setContentFile(file);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::addToPluginMenu(QObject *plugin, const QString &text,
                                     QMenu *menu, const char *member,
                                     QActionGroup *actionGroup, QIcon icon)
{
  QAction *action = new QAction(icon, text, plugin);
  connect(action, SIGNAL(triggered()), this, member);
  menu->addAction(action);
  m_PluginToolBar->addAction(action);

  if (actionGroup)
  {
    action->setCheckable(true);
    actionGroup->addAction(action);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setInputUI()
{
#if 0
  // Get the current QWidget
  if (NULL != m_ActivePlugin)
  {
    QWidget* activeInputWidget = m_ActivePlugin->getInputWidget(this);
    centerWidget->layout()->removeWidget(activeInputWidget);
  }
  // Get the action Associated with the Plugin that was just activated
  QAction *action = qobject_cast<QAction*>(sender());
  // Get a pointer to the new active plugin instance
  m_ActivePlugin = qobject_cast<DREAM3DPluginInterface*>(action->parent());
  this->setWindowTitle(m_ActivePlugin->getPluginName() + " is now Active");

  // Get a pointer to the plugins Input Widget
  QWidget* inputWidget = m_ActivePlugin->getInputWidget(this);
  centerWidget->layout()->addWidget(inputWidget);

  DREAM3DPluginFrame* frame = m_ActivePlugin->getPluginFrame(NULL);
  if (frame)
  {
    frame->setStatusBar(this->statusBar());
  }
#endif
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionAbout_triggered()
{
  QString msg ("DREAM3D Version ");
  msg.append(DREAM3DLib::Version::Complete().c_str());
  msg.append("\n\nThe Primary Developers are:\n");
  msg.append("Dr. Michael Groeber\n  US Air Force Research Laboratory\n  michael.groeber@wpafb.af.mil\n");
  msg.append("Mr. Michael Jackson\n  BlueQuartz Software\n  mike.jackson@bluequartz.net\n\n");
  msg.append("Please send any help, bug or feature requests dream3d@bluequartz.net\n\n");
  msg.append("The latest version can always be downloaded from http://dream3d.bluequartz.net\n");
  QMessageBox::information(this, QString("About DREAM.3D"), msg, QMessageBox::Ok | QMessageBox::Default);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionLicense_Information_triggered()
{
  ApplicationAboutBoxDialog about(DREAM3D::LicenseList, this);
  QString an = QCoreApplication::applicationName();
  QString version("");
  version.append(DREAM3DLib::Version::PackageComplete().c_str());
  about.setApplicationInfo(an, version);
  about.exec();
}
