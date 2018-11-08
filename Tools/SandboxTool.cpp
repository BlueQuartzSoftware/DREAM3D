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
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QMetaProperty>
#include <QtCore/QSet>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QUuid>

// SIMPLib includes
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"

#include "DREAM3DToolsConfiguration.h"

#include "SandboxTool/AddPybindMacros.hpp"
#include "SandboxTool/CheckClassForSuperClass.hpp"
#include "SandboxTool/RemoveSetupFilterParameters.hpp"
#include "SandboxTool/UpdateFilterHeaders.hpp"

/**
 * @brief findPath
 * @param groupName
 * @param filtName
 * @param ext
 * @return
 */
QString findPath(const QString& groupName, const QString& filtName, const QString& ext)
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

  PluginManager* pm = PluginManager::Instance();
  QStringList libs = pm->getPluginNames();

  prefix = D3DTools::GetSIMPLibPluginDir();

  for(int i = 0; i < libs.size(); ++i)
  {
    QString path = prefix + "/" + libs.at(i) + "/" + libs.at(i) + "Filters/" + filtName + ext;
    // std::cout << "    ****" << path.toStdString() << std::endl;

    QFileInfo fi(path);
    if(fi.exists())
    {
      return path;
    }
  }

  prefix = D3DTools::GetDREAM3DProjParentDir() + "/DREAM3D_Plugins";
  for(int i = 0; i < libs.size(); ++i)
  {
    QString path = prefix + "/" + libs.at(i) + "/" + libs.at(i) + "Filters/" + filtName + ext;
    //  std::cout << "    ****" << path.toStdString() << std::endl;

    QFileInfo fi(path);
    if(fi.exists())
    {
      return path;
    }
  }

  qDebug() << "Error Finding File for " << groupName << "/" << filtName << "/" << ext;
  return "NOT FOUND";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> void RecursiveFileSearch(const QDir& currentDir, const QStringList& filters)
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

  Q_ASSERT(true); // We don't want anyone to run this program.

  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("SandboxTool");

  std::cout << "SandboxTool Starting.\nVersion " << SIMPLib::Version::PackageComplete().toStdString() << std::endl;

#if 0
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);
#endif
  // Send progress messages from PipelineBuilder to this object for display
  qRegisterMetaType<PipelineMessage>();

  std::list<QDir> dirs;
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/SIMPL/Source"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/SIMPLView/Source"));

  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/Source/Plugins"));
  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/Source/EbsdLib"));
  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/Source/OrientationLib"));

  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/Plugins/Anisotropy/AnisotropyFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/Plugins/CellularAutomata/CellularAutomataFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/Plugins/DDDAnalysisToolbox/DDDAnalysisToolboxFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/Plugins/DREAM3DReview/DREAM3DReviewFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/Plugins/FiberToolbox/FiberToolboxFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/Plugins/HEDMAnalysis/HEDMAnalysisFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/Plugins/ITKImageProcessing/ITKImageProcessingFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/Plugins/ImageProcessing/ImageProcessingFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/Plugins/MASSIFUtilities/MASSIFUtilitiesFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/Plugins/ProgWorkshop/ProgWorkshopFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/Plugins/TransformationPhase/TransformationPhaseFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/Plugins/UCSBUtilities/UCSBUtilitiesFilters"));

  //#if 1
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/AMProcessMonitoring/AMProcessMonitoringFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/DictionaryIndexing/DictionaryIndexingFilters"));
  //#endif
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/NETLIntegration/NETLIntegrationFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/AskNDEToolbox/AskNDEToolboxFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/EMsoftToolbox/EMsoftToolboxFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/ProcessModeling/ProcessModelingFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/BrukerIntegration/BrukerIntegrationFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/Fusion/FusionFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/SMTKPlugin/SMTKPluginFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/CAxisByPolarizedLight/CAxisByPolarizedLightFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/Leroy2/Leroy2Filters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/TomvizToolbox/TomvizToolboxFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/DataFusion/DataFusionFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/MDIToolbox/MDIToolboxFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/VolumeMeshing/VolumeMeshingFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/DatasetMerging/DatasetMergingFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/MultiscaleFusion/MultiscaleFusionFilters"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/ZeissImport/ZeissImportFilters"));

  QStringList filters;
  // filters.append("*.cpp");
  filters.append("*.h");


  for(auto const& dir : dirs)
  {
    RecursiveFileSearch<UpdateFilterHeaders>(dir, filters);
  }


  return 0;
}
