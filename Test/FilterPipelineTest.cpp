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


#include <QtCore/QPluginLoader>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>

#include "Applications/DREAM3D/DREAM3DApplication.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Observer.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#ifdef DREAM3D_BUILD_TEST_FILTERS
#include "SIMPLib/TestFilters/GenericExample.h"
#include "SIMPLib/TestFilters/ArraySelectionExample.h"
#include "SIMPLib/TestFilters/MakeDataContainer.h"
#include "SIMPLib/TestFilters/ThresholdExample.h"
#include "SIMPLib/TestFilters/TestFilters.h"
#endif
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "DREAM3DTestFileLocations.h"

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

#ifdef DREAM3D_BUILD_TEST_FILTERS

  // Create our Pipeline object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  MakeDataContainer::Pointer read_h5ebsd = MakeDataContainer::New();
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
#endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loadPlugins(FilterManager* fm)
{
  //  qDebug() << "DREAM3D_UI::loadPlugins" << "\n";


  QStringList m_PluginDirs;
  m_PluginDirs << dream3dApp->applicationDirPath();

  QDir aPluginDir = QDir(dream3dApp->applicationDirPath());
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
        //qWarning(aPluginDir.absoluteFilePath(fileName).toLatin1(), "%s");
        //qDebug() << "Adding " << aPluginDir.absoluteFilePath(fileName)() << "\n";
      }
    }
  }

  QStringList m_PluginFileNames;
  QVector<ISIMPLibPlugin*> m_LoadedPlugins;

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
    QObject* plugin = loader.instance();
    qDebug() << "    Pointer: " << plugin << "\n";
    if (plugin && m_PluginFileNames.contains(fileName, Qt::CaseSensitive) == false)
    {
      //populateMenus(plugin);
      ISIMPLibPlugin* ipPlugin = qobject_cast<ISIMPLibPlugin* > (plugin);
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
  FilterManager* m_FilterManager = FilterManager::Instance();
  m_FilterManager->RegisterKnownFilters(m_FilterManager);
  // Look for plugins
  loadPlugins(m_FilterManager);
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
