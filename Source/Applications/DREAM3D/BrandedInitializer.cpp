/* ============================================================================
 * Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2014 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "BrandedInitializer.h"

#include <QtCore/QTime>
#include <QtCore/QPluginLoader>
#include <QtCore/QCoreApplication>
#include <QtGui/QSplashScreen>
#include <QtGui/QBitmap>
#include <QtGui/QMessageBox>




#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"

#include "QtSupport/QRecentFileList.h"

#include "DREAM3D_UI.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BrandedInitializer::BrandedInitializer() :
  show_splash(true),
  Splash(NULL),
  MainWindow(NULL)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BrandedInitializer::~BrandedInitializer()
{
  delete this->Splash;
  this->Splash = NULL;

  delete this->MainWindow;
  this->MainWindow = NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void delay(int seconds)
{
  QTime dieTime= QTime::currentTime().addSecs(seconds);
  while( QTime::currentTime() < dieTime ) {
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BrandedInitializer::initialize(int argc, char* argv[])
{

  QApplication::setApplicationVersion(DREAM3DLib::Version::Complete());

  // Create and show the splash screen as the main window is being created.
  QPixmap pixmap(":/branded_splash.png");
  this->Splash = new QSplashScreen(pixmap);
  this->Splash->setMask(pixmap.createMaskFromColor(QColor(Qt::transparent)));
  this->Splash->show();

  // Not sure why this is needed. Andy added this ages ago with comment saying
  // needed for Mac apps. Need to check that it's indeed still required.
  QDir dir(QApplication::applicationDirPath());
#if defined (Q_OS_MAC)
  dir.cdUp();
  dir.cd("Plugins");

#elif defined (Q_OS_LINUX)

#elif defined (Q_OS_WIN)
  dir.cdUp();
  dir.cd("Plugins");
#endif
  QApplication::addLibraryPath(dir.absolutePath());

  QMetaObjectUtilities::RegisterMetaTypes();

  // Load application plugins.
  loadPlugins();

  // Create main window.
  this->MainWindow = new DREAM3D_UI();
  this->MainWindow->setWindowTitle("[*] DREAM.3D Version " + DREAM3DLib::Version::Package());
  this->MainWindow->setLoadedPlugins(m_LoadedPlugins);

  // give GUI components time to update before the mainwindow is shown
  QApplication::instance()->processEvents();
  this->MainWindow->show();

  QApplication::instance()->processEvents();
  if (show_splash)
  {
    this->Splash->finish(this->MainWindow);
  }
  return true;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BrandedInitializer::loadPlugins()
{
  FilterManager::Pointer fm = FilterManager::Instance();

  qDebug() << "DREAM3D_UI::loadPlugins" << "\n";

  //  foreach (QObject *plugin, QPluginLoader::staticInstances())
  //    populateMenus(plugin);
  QStringList m_PluginDirs;

  m_PluginDirs << qApp->applicationDirPath();

  QDir aPluginDir = QDir(qApp->applicationDirPath());
  qDebug() << "aPluginDir: " << aPluginDir.absolutePath();
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
  // Try the current location of where the application was launched from
  if (aPluginDir.cd("Plugins"))
  {
    thePath = aPluginDir.absolutePath();
    m_PluginDirs << thePath;
  }
  // Now try moving up a directory which is what should happen when running from a
  // proper distribution of DREAM3D
  aPluginDir.cdUp();
  if (aPluginDir.cd("plugins"))
  {
    thePath = aPluginDir.absolutePath();
    m_PluginDirs << thePath;
  }
#endif

  QStringList pluginFilePaths;

  foreach (QString pluginDirString, m_PluginDirs)
  {
    qDebug() << "Plugin Directory being Searched: " << pluginDirString << "\n";
    aPluginDir = QDir(pluginDirString);
    foreach (QString fileName, aPluginDir.entryList(QDir::Files))
    {
      //   qDebug() << "File: " << fileName() << "\n";
#ifdef QT_DEBUG
      if (fileName.endsWith("_debug.plugin", Qt::CaseSensitive))
#else
      if (fileName.endsWith( ".plugin", Qt::CaseSensitive) )
#endif
      {
        pluginFilePaths << aPluginDir.absoluteFilePath(fileName);
        //qWarning(aPluginDir.absoluteFilePath(fileName).toAscii(), "%s");
        //qDebug() << "Adding " << aPluginDir.absoluteFilePath(fileName)() << "\n";
      }
    }
  }

  // Now that we have a sorted list of plugins, go ahead and load them all from the
  // file system and add each to the toolbar and menu
  foreach(QString path, pluginFilePaths)
  {
    qDebug() << "Plugin Being Loaded:";
    qDebug() << "    File Extension: .plugin";
    qDebug() << "    Path: " << path;
    QPluginLoader loader(path);
    QFileInfo fi(path);
    QString fileName = fi.fileName();
    QObject *plugin = loader.instance();
    qDebug() << "    Pointer: " << plugin << "\n";
    if (plugin )
    {
      DREAM3DPluginInterface* ipPlugin = qobject_cast<DREAM3DPluginInterface*>(plugin);
      if (ipPlugin)
      {
        QString msg = QObject::tr("Loading Plugin %1").arg(fileName);
        this->Splash->showMessage(msg);
        DREAM3DPluginInterface::Pointer ipPluginPtr(ipPlugin);
        m_LoadedPlugins.push_back(ipPluginPtr);
        ipPlugin->registerFilterWidgets();
        ipPlugin->registerFilters(fm.get());
      }
    }
    else
    {
      QString message("The plugin did not load with the following error\n");
      message.append(loader.errorString());
      QMessageBox::critical(MainWindow, "DREAM.3D Plugin Load Error",
                            message,
                            QMessageBox::Ok | QMessageBox::Default);
    }
  }

}
