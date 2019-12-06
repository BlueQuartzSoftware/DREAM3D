/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include <cstdint>
#include <iostream>
#include <list>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>

// SIMPLib includes
#if 0
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"
#endif

#include "DREAM3DToolsConfiguration.h"

#include "SandboxTool/RemoveSIMPLMacros.h"
#include "SandboxTool/UpdateFilterHeaders.hpp"

    /**
     * @brief findPath
     * @param groupName
     * @param filtName
     * @param ext
     * @return
     */
    QString
    findPath(const QString& groupName, const QString& filtName, const QString& ext)
{
  //  std::cout << groupName.toStdString() << "::" << filtName.toStdString() << std::endl;
  QString prefix = D3DTools::GetDREAM3DSourceDir() + "/";
  {
    QString path = D3DTools::GetSIMPLibDir() + "/" + groupName + "Filters/" + filtName + ext;
    QFileInfo fi(path);
    if(fi.exists())
    {
      return path;
    }
  }

  //  PluginManager* pm = PluginManager::Instance();
  //  QStringList libs = pm->getPluginNames();

  //  prefix = D3DTools::GetSIMPLibPluginDir();

  //  for(int i = 0; i < libs.size(); ++i)
  //  {
  //    QString path = prefix + "/" + libs.at(i) + "/" + libs.at(i) + "Filters/" + filtName + ext;
  //    // std::cout << "    ****" << path.toStdString() << std::endl;

  //    QFileInfo fi(path);
  //    if(fi.exists())
  //    {
  //      return path;
  //    }
  //  }

  //  prefix = D3DTools::GetDREAM3DProjParentDir() + "/DREAM3D_Plugins";
  //  for(int i = 0; i < libs.size(); ++i)
  //  {
  //    QString path = prefix + "/" + libs.at(i) + "/" + libs.at(i) + "Filters/" + filtName + ext;
  //    //  std::cout << "    ****" << path.toStdString() << std::endl;

  //    QFileInfo fi(path);
  //    if(fi.exists())
  //    {
  //      return path;
  //    }
  //  }

  qDebug() << "Error Finding File for " << groupName << "/" << filtName << "/" << ext;
  return "NOT FOUND";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void RecursiveFileSearch(const QDir& currentDir, const QStringList& filters)
{

  if(currentDir.dirName().compare("zRel") == 0 || currentDir.dirName().compare("Build") == 0)
  {
    return;
  }
  // Get a list of all the directories
  QFileInfoList dirList = currentDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
  if(!dirList.isEmpty())
  {
    foreach(QFileInfo fi, dirList)
    {
      RecursiveFileSearch<T>(QDir(fi.absoluteFilePath()), filters); // Recursive call
    }
  }

  QFileInfoList itemList = currentDir.entryInfoList(filters);
  foreach(QFileInfo itemInfo, itemList)
  {
    QString headerFilePath = itemInfo.absoluteFilePath();
    T()(headerFilePath);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  {
    // Uncomment this to run this program
    //    qDebug() << "Exited SandboxTool early.  Uncomment logic statement at the top of main to allow the program to run.";
    //    return 0;
  }

  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("SandboxTool");

  //  std::cout << "SandboxTool Starting.\nVersion " << SIMPLib::Version::PackageComplete().toStdString() << std::endl;

#if 0
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);
#endif

  std::list<QDir> dirs;
#if 1
  // dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir()));
  // dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins"));
  // dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/SIMPL/Source/SIMPLib/CoreFilters"));
  // dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../SIMPLView/Source"));

  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/SIMPLView/Source"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/SIMPLVtkLib"));
  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/Plugins/SimulationIO"));
  // dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/Plugins/ITKImageProcessing"));
  // dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/Source"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/Test"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/Tools"));

  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/Source/EbsdLib"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/Source/OrientationLib"));

//  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/AskNDEToolbox/"));
//  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/BrukerIntegration/"));
//  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/CAxisByPolarizedLight/"));
//  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/DictionaryIndexing/"));
//  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/MDIToolbox/"));
//  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/AskNDEToolbox/"));
//  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/Leroy2/"));
//  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/NETLIntegration/"));
//  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/ProgWorkshop/"));
//  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/SMTKPlugin/"));
//  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/TomvizToolbox/"));
//  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/VolumeMeshing/"));
#endif

  QStringList filters;
  filters.append("*.cpp");
  //  filters.append("*.hpp");
  filters.append("*.h");
  // filters.append("*.md");

  for(auto const& dir : dirs)
  {
    RecursiveFileSearch<RemoveSIMPLMacros>(dir, filters);
  }

  return 0;
}
