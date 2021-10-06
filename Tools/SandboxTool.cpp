

#include <cstdint>
#include <iostream>
#include <list>

#include <QtCore/QUuid>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>

#include "DREAM3DToolsConfiguration.h"
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
    for(QFileInfo fi : dirList)
    {
      RecursiveFileSearch<T>(QDir(fi.absoluteFilePath()), filters); // Recursive call
    }
  }

  QFileInfoList itemList = currentDir.entryInfoList(filters);
  for(QFileInfo itemInfo : itemList)
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
    // Comment this to run this program
    //    qDebug() << "Exited SandboxTool early.  Comment logic statement at the top of main to allow the program to run.";
    //    return 0;
  }

  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("SandboxTool");

  std::cout << "SandboxTool Starting.\nVersion "
            << "1.0.0" << std::endl;

  std::list<QDir> dirs;
#if 1
  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir()));
  // dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins"));
  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../SIMPL/Source/SIMPLib/CoreFilters"));
  // dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../SIMPLView/Source"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../SIMPLVtkLib"));

  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/Source"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/Test"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/Tools"));

  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../EbsdLib"));
  //  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../H5Support"));

  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/AFRLDistributionC/AFRLDistributionCFilters"));
  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/CRADA3DSystems/CRADA3DSystemsFilters"));
  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/BrukerIntegration/BrukerIntegrationFilters"));
  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/CAxisByPolarizedLight/CAxisByPolarizedLightFilters"));
  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/NETLIntegration/NETLIntegrationFilters"));
  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/ProgWorkshop/ProgWorkshopFilters"));

  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/VolumeMeshing/VolumeMeshingFilters"));
  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/DREAM3DReview/DREAM3DReviewFilters"));
  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/ImageProcessing/ImageProcessingFilters"));
  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/ITKImageProcessing/ITKImageProcessingFilters"));
  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/SimulationIO/SimulationIOFilters"));
  dirs.emplace_back(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins/UCSBUtilities/UCSBUtilitiesFilters"));
#endif

  QStringList filters;
  filters.append("*.cpp");

  for(auto const& dir : dirs)
  {
    RecursiveFileSearch<UpdateFilterHeaders>(dir, filters);
  }

  return 0;
}
