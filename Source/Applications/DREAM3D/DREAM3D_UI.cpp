/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/DREAM3DVersion.h"
#include "QtSupport/ApplicationAboutBoxDialog.h"
#include "QtSupport/QRecentFileList.h"
#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/AIM_QtMacros.h"
#include "QtSupport/AIMPluginFrame.h"
#include "QtSupport/DREAM3DPluginInterface.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D_UI::DREAM3D_UI(QWidget *parent) :
  QMainWindow(parent),
  m_WorkerThread(NULL),
  m_ActivePlugin(NULL),
  m_PluginToolBar(NULL),
#if defined(Q_WS_WIN)
m_OpenDialogLastDirectory("C:\\")
#else
m_OpenDialogLastDirectory("~/")
#endif
{
  // Calls the Parent Class to do all the Widget Initialization that were created
  // using the QDesigner program
  setupUi(this);

  // Do our own widget initializations
  setupGui();
  // Look for plugins
  loadPlugins();

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
  bool ok = false;

  Qt::ToolButtonStyle tbstyle = static_cast<Qt::ToolButtonStyle>(prefs.value("PluginDisplay").toUInt(&ok));
  m_PluginToolBar->setToolButtonStyle(tbstyle);

  // Read the preference for each plugin
  foreach (DREAM3DPluginInterface* plugin, m_LoadedPlugins) {
    plugin->readSettings(prefs);
  }

  bool loaded = false;
  QString pluginName = prefs.value("ActivePlugin").toString();
  for (int i = 0; i < m_LoadedPlugins.size(); ++i)
  {
    if (m_LoadedPlugins[i]->getPluginName().compare(pluginName) == 0)
    {
      if (m_PluginActionGroup->actions().size() > 0)
      {
        m_PluginActionGroup->actions().at(i)->activate(QAction::Trigger);
        loaded = true;
      }
      break;
    }
  }

  // If the proper plugin was not found, Load the first plugin found
  if (loaded == false)
  {
    if (m_PluginActionGroup->actions().size() > 0)
    {
      m_PluginActionGroup->actions().at(0)->activate(QAction::Trigger);
    }
  }

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

  prefs.setValue("ActivePlugin" , this->m_ActivePlugin->getPluginName());

  foreach (DREAM3DPluginInterface* plugin, m_LoadedPlugins) {
    plugin->writeSettings(prefs);
  }

  writeWindowSettings(prefs);
  prefs.setValue("PluginDisplay", this->m_PluginToolBar->toolButtonStyle());
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
  m_PluginActionGroup = new QActionGroup(this);
  m_PluginToolBar = new QToolBar(this);
  m_PluginToolBar->setObjectName(QString("PluginToolbar"));

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
    int r = QMessageBox::warning(this, tr("AIMRepresentation"),
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
void DREAM3D_UI::on_actionClose_triggered() {
 // std::cout << "DREAM3D_UI::on_actionClose_triggered" << std::endl;
  qint32 err = -1;
  err = checkDirtyDocument();
  if (err >= 0)
  {
    // Close the window. Files have been saved if needed
    if (QApplication::activeWindow() == this)
    {
      this->close();
    }
    else
    {
      QApplication::activeWindow()->close();
    }
  }
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
  foreach (QObject *plugin, QPluginLoader::staticInstances())
    populateMenus(plugin);

  m_PluginDirs.clear();
  m_PluginDirs << qApp->applicationDirPath();

  QDir aPluginDir = QDir(qApp->applicationDirPath());
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
    thePath = aPluginDir.absolutePath() + "/Plugins";
    // m_PluginDirs << thePath;
    aPluginDir.cdUp();
  }
  // aPluginDir.cd("Plugins");
  thePath = aPluginDir.absolutePath() + "/Plugins";
  m_PluginDirs << thePath;
#else
  if (aPluginDir.cd("plugins"))
  {
    thePath = aPluginDir.absolutePath();
    m_PluginDirs << thePath;
  }
#endif

  this->setWindowTitle("DREAM3D - No Plugins Loaded");

  foreach (QString pluginDirString, m_PluginDirs)
  {
    // std::cout << "Plugin Directory being Searched: " << pluginDirString.toStdString() << std::endl;
    aPluginDir = QDir(pluginDirString);
    foreach (QString fileName, aPluginDir.entryList(QDir::Files))
    {
      //   std::cout << "File: " << fileName.toStdString() << std::endl;
#ifdef QT_DEBUG
      if (fileName.endsWith("_debug.plugin", Qt::CaseSensitive))
#endif

#if defined (QT_NO_DEBUG)
      if (fileName.endsWith( ".plugin", Qt::CaseSensitive) )
#endif
      {
        //     std::cout << "File Extension matches.." << std::endl;
        QPluginLoader loader(aPluginDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        //  std::cout << "plugin Pointer: " << plugin << std::endl;
        if (plugin && m_PluginFileNames.contains(fileName, Qt::CaseSensitive) == false)
        {
          populateMenus(plugin);
          m_PluginFileNames += fileName;
        }
        else
        {
          std::cout << "The plugin did not load with the following error\n   " << loader.errorString().toStdString() << std::endl;
        }
      }
    }
    menuPlugins->setEnabled(!m_PluginActionGroup->actions().isEmpty());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
 void DREAM3D_UI::populateMenus(QObject *plugin)
{
#ifdef QT_DEBUG
  std::cout << "Found Plugin..." << std::endl;
#endif
  DREAM3DPluginInterface* ipPlugin = qobject_cast<DREAM3DPluginInterface * > (plugin);
  if (ipPlugin)
  {
    quint32 order = ipPlugin->pluginOrder();
    if (order >= m_LoadedPlugins.size() && order < DREAM3D::UserDefinedPluginOrder)
    {
      m_LoadedPlugins.resize(order + 1);
      m_LoadedPlugins[order] = ipPlugin;
    }
    m_LoadedPlugins.push_back(ipPlugin);
#ifdef QT_DEBUG
    qWarning(ipPlugin->getPluginName().toAscii(), "%s");
#endif
    QIcon newIcon = ipPlugin->icon();

    addToPluginMenu(plugin, ipPlugin->getPluginName(),
                    menuPlugins, SLOT(setInputUI()), m_PluginActionGroup, newIcon);
  }
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

  AIMPluginFrame* frame = m_ActivePlugin->getPluginFrame(NULL);
  if (frame)
  {
    frame->setStatusBar(this->statusBar());
  }

}

