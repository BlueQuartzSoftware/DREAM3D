/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include <QtCore/QCoreApplication>
#include <QtCore/QPluginLoader>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>



#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Observer.h"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginInterface.h"
#include "DREAM3DLib/TestFilters/GenericExample.h"
#include "DREAM3DLib/TestFilters/ArraySelectionExample.h"
#include "DREAM3DLib/TestFilters/MakeVolumeDataContainer.h"
#include "DREAM3DLib/TestFilters/ThresholdExample.h"
#include "DREAM3DLib/TestFilters/TestFilters.h"

#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"

// -----------------------------------------------------------------------------
// These are input files
// -----------------------------------------------------------------------------
QString testFile1()
{
  return UnitTest::DataDir + QString("/SmallIN100/Slice_1.ang");
}
QString testFile2()
{
  return UnitTest::DataDir + QString("/SmallIN100/Slice_1.ang");
}
QString testFile3()
{
  return UnitTest::DataDir + QString("/SmallIN100/Slice_1.ang");
}

// -----------------------------------------------------------------------------
// These are the output files
// -----------------------------------------------------------------------------
QString outputDREAM3DFile()
{
  return UnitTest::TestTempDir + QString("/FilterPipelineTest.dream3d");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(outputDREAM3DFile());
#endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestPipelinePushPop()
{

  // Create our Pipeline object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  MakeVolumeDataContainer::Pointer read_h5ebsd = MakeVolumeDataContainer::New();
  //  pipeline->pushBack(read_h5ebsd);

  GenericExample::Pointer align_sections = GenericExample::New();
  pipeline->pushBack(align_sections);

  ArraySelectionExample::Pointer segment_features = ArraySelectionExample::New();
  pipeline->pushBack(segment_features);

  ThresholdExample::Pointer min_size = ThresholdExample::New();
  pipeline->pushBack(min_size);

  Filt0::Pointer writer = Filt0::New();
  pipeline->pushBack(writer);

  Filt1::Pointer vtkWriter = Filt1::New();
  pipeline->pushBack(vtkWriter);

  pipeline->popFront();
  //  pipeline->printFilterNames(std::cout);
  DREAM3D_REQUIRE_EQUAL(4, pipeline->size());

  pipeline->pushFront(align_sections);
  //  pipeline->printFilterNames(std::cout);
  DREAM3D_REQUIRE_EQUAL(5, pipeline->size());

  pipeline->pushFront(read_h5ebsd);
  //  pipeline->printFilterNames(std::cout);
  DREAM3D_REQUIRE_EQUAL(6, pipeline->size());

  pipeline->popBack(); // Pop off the vtkwriter
  //  pipeline->printFilterNames(std::cout);
  DREAM3D_REQUIRE_EQUAL(5, pipeline->size());

  pipeline->erase(pipeline->size() - 1); // Erase the data container writer
  //  pipeline->printFilterNames(std::cout);
  DREAM3D_REQUIRE_EQUAL(4, pipeline->size());

  pipeline->insert(pipeline->size(), writer);
  pipeline->insert(pipeline->size(), vtkWriter);
  //  pipeline->printFilterNames(std::cout);
  DREAM3D_REQUIRE_EQUAL(6, pipeline->size());

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loadPlugins(FilterManager *fm)
{
  //  qDebug() << "DREAM3D_UI::loadPlugins" << "\n";


  QStringList m_PluginDirs;
  m_PluginDirs << qApp->applicationDirPath();

  QDir aPluginDir = QDir(qApp->applicationDirPath());
  // qDebug() << "aPluginDir: " << aPluginDir.absolutePath() << "\n";
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


  QStringList pluginFilePaths;

  foreach (QString pluginDirString, m_PluginDirs)
  {
    //qDebug() << "Plugin Directory being Searched: " << pluginDirString() << "\n";
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

  QStringList m_PluginFileNames;
  QVector<DREAM3DPluginInterface*> m_LoadedPlugins;

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
    if (plugin && m_PluginFileNames.contains(fileName, Qt::CaseSensitive) == false)
    {
      //populateMenus(plugin);
      DREAM3DPluginInterface* ipPlugin = qobject_cast<DREAM3DPluginInterface * > (plugin);
      if (ipPlugin)
      {
        m_LoadedPlugins.push_back(ipPlugin);
        ipPlugin->registerFilters(fm);
      }

      m_PluginFileNames += fileName;
    }
    else
    {
      QString message("The plugin did not load with the following error\n");
      message.append(loader.errorString());
      qDebug() << "The plugin did not load with the following error\n   " << loader.errorString() << "\n";
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loadFilterPlugins()
{
  // Register all of the Filters we know about - the rest will be loaded through plugins
  //  which all should have been loaded by now.
  FilterManager::Pointer m_FilterManager = FilterManager::Instance();
  m_FilterManager->RegisterKnownFilters(m_FilterManager.get());
  // Look for plugins
  loadPlugins(m_FilterManager.get());
}


// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;

#if !REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() );
#endif

  DREAM3D_REGISTER_TEST( loadFilterPlugins() );

  DREAM3D_REGISTER_TEST( TestPipelinePushPop() );

#if REMOVE_TEST_FILES
//  DREAM3D_REGISTER_TEST( RemoveTestFiles() );
#endif
  PRINT_TEST_SUMMARY();
  return err;
}
